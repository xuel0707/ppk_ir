#include "device_mgr.h"
#include "google/rpc/code.pb.h"
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string>
#include <unordered_map>
#define SIMPLELOG std::cout

// typedef void *(stream_message_response_cb)(DeviceMgr::device_id_t device_id,
//                                 p4v1::StreamMessageResponse *msg,
//                                 void *cookie);
// extern static cb_mgr_t packet_cb_mgr_t;

//----------------------------------------------------------recover----
// p4::server::v1::Config DeviceMgrImp::default_server_config;

// class DeviceMgrImp {
//  public:
//   explicit DeviceMgrImp(device_id_t device_id)
//       : device_id(device_id),
// 	    server_config(default_server_config),
//         packet_io(device_id, &server_config){ }

//   ~DeviceMgrImp() {
//     pi_remove_device(device_id);
//   }

//   DeviceMgrImp(const DeviceMgrImp &) = delete;
//   DeviceMgrImp &operator=(const DeviceMgrImp &) = delete;
//   DeviceMgrImp(DeviceMgrImp &&) = delete;
//   DeviceMgrImp &operator=(DeviceMgrImp &&) = delete;

//   static Status init() {
//     auto pi_status = pi_init(defaultMaxDevices, NULL);
//     if (pi_status != PI_STATUS_SUCCESS)
//       std::cout << "Error when initializing PI library";
// 	// return grpc::Status(grpc::StatusCode::OK)
//   }

//   void stream_message_response_register_cb(StreamMessageResponseCb cb,
//                                            void *cookie) {
//     // idle_timeout_register_cb(cb, cookie);
//     packet_io.packet_in_register_cb(cb, cookie);
//     // digest_mgr.stream_message_response_register_cb(cb, cookie);
//     cb_ = cb;
//     cookie_ = cookie;
//   }

//  private:
//   static constexpr size_t defaultMaxDevices = 256;
//   device_id_t device_id;
//   StreamMessageResponseCb cb_{};
//   void *cookie_{nullptr};
//   pi::fe::proto::PacketIOMgr packet_io;
//   static p4::server::v1::Config default_server_config;
//   pi::fe::proto::ServerConfigAccessor server_config;
// }

// DeviceMgr::DeviceMgr(device_id_t device_id) {
//   pimp = std::unique_ptr<DeviceMgrImp>(new DeviceMgrImp(device_id));
// }

// Status
// DeviceMgr::init() {
//   return DeviceMgrImp::init();
// }

// void
// DeviceMgr::stream_message_response_register_cb(StreamMessageResponseCb cb,
//                                                void *cookie) {
//   return pimp->stream_message_response_register_cb(std::move(cb), cookie);
// }


struct ConfigFile {
public:
	ConfigFile() { }

	~ConfigFile() {
		if (fp != nullptr) std::fclose(fp);
	}

	grpc::Status change_config(const ::p4::v1::ForwardingPipelineConfig& config_proto) {
		if (fp != nullptr) std::fclose(fp);  // delete old file
		fp = std::tmpfile();  // new temporary file
		if (!fp) {
			return grpc::Status::OK; //TODO
		}
		if (config_proto.p4_device_config().size() > 0) {
			auto nb_written = std::fwrite(config_proto.p4_device_config().data(),
				config_proto.p4_device_config().size(),
				1,
				fp);
			/*if (nb_written != 1) {
			  RETURN_ERROR_STATUS(
				  Code::INTERNAL, "Error when saving config to temporary file");
			}*/
		}
		size = config_proto.p4_device_config().size();
		return grpc::Status::OK;
	}

	grpc::Status read_config(::p4::v1::ForwardingPipelineConfig* config_proto) {
		if (!fp || size == 0) return grpc::Status::OK;  // no config was saved
		if (std::fseek(fp, 0, SEEK_SET) != 0) {  // seek to start
			return grpc::Status::OK; // TODO
		  /*RETURN_ERROR_STATUS(
			  Code::INTERNAL,
			  "Error when reading saved config from temporary file");*/
		}
		// Unfortunately, in C++11, one cannot write directly to the std::string
		// storage (unlike in C++17), so we need an extra copy. To avoid having 2
		// copies of the config simultaneously in memory, we read the file by chunks
		// of 512 bytes.
		char buffer[512];
		auto* device_config = config_proto->mutable_p4_device_config();
		device_config->reserve(size);
		size_t iters = size / sizeof(buffer);
		size_t remainder = size - iters * sizeof(buffer);
		size_t i;
		for (i = 0; i < iters && std::fread(buffer, sizeof(buffer), 1, fp); i++) {
			device_config->append(buffer, sizeof(buffer));
		}
		if (i != iters ||
			(remainder != 0 && !std::fread(buffer, remainder, 1, fp))) {
			return grpc::Status::OK;
			/*RETURN_ERROR_STATUS(
				Code::INTERNAL,
				"Error when reading saved config from temporary file");*/
		}
		device_config->append(buffer, remainder);
		return grpc::Status::OK;
	}

private:
	std::FILE* fp{ nullptr };
	size_t size{ 0 };
};

ConfigFile saved_device_config;
bool has_config_cookie{ false };
::p4::v1::ForwardingPipelineConfig::Cookie config_cookie;

struct p4_field_match_header* _gen_match_rule_exact(argument_t* arg, const ::p4::v1::FieldMatch::Exact& exact) {
	struct p4_field_match_exact* result = malloc(sizeof(struct p4_field_match_exact));

	strcpy(result->header.name, arg->name);
	result->header.type = P4_FMT_EXACT;
	result->length = arg->bitwidth;
	//printf("value_exact:: %x,%x,%x,%x\n",*(exact.value().c_str()),*(exact.value().c_str()+1),*(exact.value().c_str()+2),*(exact.value().c_str()+3));
	memcpy(result->bitmap, exact.value().c_str(), exact.value().size());
	//printf("value:: %x,%x,%x,%x\n",*(result->bitmap),*(result->bitmap+1),*(result->bitmap+2),*(result->bitmap+3));

	return (struct p4_field_match_header*)result; /* TODO: NTOH !!! */
}

struct p4_field_match_header* _gen_match_rule_lpm(argument_t* arg, const ::p4::v1::FieldMatch::LPM& lpm) {
	struct p4_field_match_lpm* result = malloc(sizeof(struct p4_field_match_lpm));

	strcpy(result->header.name, arg->name);
	result->header.type = P4_FMT_LPM;
	result->prefix_length = lpm.prefix_len();
	memcpy(result->bitmap, lpm.value().c_str(), lpm.value().size());

	return (struct p4_field_match_header*)result; /* TODO: NTOH !!! */
}

struct p4_field_match_header* _gen_match_rule_ternary(argument_t* arg, const ::p4::v1::FieldMatch::Ternary& ternary) {
	struct p4_field_match_ternary* result = malloc(sizeof(struct p4_field_match_ternary));

	strcpy(result->header.name, arg->name);
	result->header.type = P4_FMT_TERNARY;
	result->length = arg->bitwidth;
	memcpy(result->bitmap, ternary.value().c_str(), ternary.value().size());
	memcpy(result->mask, ternary.mask().c_str(), ternary.mask().size());

	return (struct p4_field_match_header*)result; /* TODO: NTOH !!! */
}

struct p4_action_parameter* _gen_action_param(argument_t* arg, const ::p4::v1::Action::Param& param) {
	struct p4_action_parameter* result = malloc(sizeof(struct p4_action_parameter));
	uint16_t* tmp16;
	uint32_t* tmp32;

	strcpy(result->name, arg->name);
	result->length = arg->bitwidth;
	memcpy(result->bitmap, param.value().c_str(), param.value().size());
	if (param.value().size() == 2) {
		tmp16 = (uint16_t*)result->bitmap;
		*tmp16 = htons(*tmp16);
	}
	else if (param.value().size() == 4) {
		tmp32 = (uint32_t*)result->bitmap;
		*tmp32 = htonl(*tmp32);
	}

	return result; /* TODO: NTOH  */
}

grpc::Status table_insert(device_mgr_t *dm, const ::p4::v1::TableEntry &table_entry) {
	uint32_t table_id;
	uint32_t field_id;
	uint32_t action_id;
	uint16_t value16; /* TODO: remove after testing */
	uint8_t ip[4]; /* TODO: remove after testing */
	size_t i;
	uint8_t match_type;
	int32_t prefix_len = 0; /* in bits */

//	::p4::v1::FieldMatch match;
//	::p4::v1::TableAction action;
//	::p4::v1::FieldMatch::Exact exact;
//	::p4::v1::FieldMatch::LPM lpm;
//	::p4::v1::FieldMatch::Ternary ternary;

	grpc::Status status;
	
//	::p4::v1::Action tmp_act;
//	::p4::v1::Action::Param param;
	
	table_id = table_entry.table_id();
        //printf("%s:%d\n", __FUNCTION__, __LINE__);
	table_entry_t *table_entry_new = add_table_entry(&(dm->id_table_entries),table_id);
        //printf("%s:%d\n", __FUNCTION__, __LINE__);
	element_t *elem = get_element(&(dm->id_map), table_id);
	if(!strstr(elem->value, "_0")){
		elem->value = strcat(elem->value,"_0");
	}
	argument_t *arg = NULL;

	struct p4_ctrl_msg ctrl_m;
	ctrl_m.num_field_matches = 0;
	ctrl_m.num_action_params = 0;
	ctrl_m.type = P4T_ADD_TABLE_ENTRY;
	ctrl_m.table_name = strdup(elem->value);
	
        match_t* match_new = NULL;
        table_entry_new->count = 0;
		int match = table_entry.match_size();
		std::cout << match <<std::endl;
	for (const auto &match: table_entry.match()) {
               // match_new = (match_t*)malloc(sizeof(match_t));
                match_new = new match_t;
		field_id = match.field_id();
		arg = get_argument(elem, field_id);
		if (arg==NULL) {
			printf("NULL ARGUMENT for FIELD_ID=%d\n", field_id);
		}
		match_type = match.field_match_type_case();
		match_new->field_id = field_id;
		match_new->match_type = match_type;
		if (match_type == 4){
                        const auto lpm = match.lpm();
		//	match_new.prefix_len = lpm.prefix_len();
		//	char* new_value;
		//	new_value = malloc(lpm.value().size());
		//	memcpy(new_value, lpm.value().c_str(), lpm.value().size());
		//	match_new.value = new_value;
                        prefix_len = lpm.prefix_len();
                        if (lpm.value().size()>=4) {
                              ip[0] = lpm.value().c_str()[0];
                              ip[1] = lpm.value().c_str()[1];
                              ip[2] = lpm.value().c_str()[2];
                              ip[3] = lpm.value().c_str()[3];
                        }
                        ctrl_m.field_matches[field_id-1] = _gen_match_rule_lpm(arg, lpm);
                        ctrl_m.num_field_matches++;
                  //      table_entry_new->matches.push_back(match_new);
                        status = grpc::Status::OK;
		}
		else if(match_type == 2){
			
                        const auto exact = match.exact();
			char* new_value;
			new_value = malloc(exact.value().size());
			memcpy(new_value, exact.value().c_str(), exact.value().size());
                        match_new->value = new_value;
                        match_new->value_size = exact.value().size();
                        
                        ctrl_m.field_matches[field_id-1] = _gen_match_rule_exact(arg, exact);
                        ctrl_m.num_field_matches++;
                        table_entry_new->matches[table_entry_new->count] = match_new;
                        table_entry_new->count++;
                        match_new = NULL;
                        status = grpc::Status::OK;
		}
		else if(match_type == 3){
			const auto ternary = match.ternary();
		//	match_new.mask = ternary.mask().c_str();
		//	char* new_value;
		//	new_value = malloc(ternary.value().size());
		//	memcpy(new_value, ternary.value().c_str(), ternary.value().size());
		//	match_new.value = new_value;
                        ctrl_m.field_matches[field_id-1] = _gen_match_rule_ternary(arg, ternary);
                        ctrl_m.num_field_matches++;
                  //      table_entry_new->matches.push_back(match_new);
                        status = grpc::Status::OK;
		}
	}
	if(ctrl_m.num_field_matches != match){
		status = grpc::Status( grpc::StatusCode::UNIMPLEMENTED, "Match_type is not given completely" );
	}
//return status; } //
	if (table_entry.is_default_action()) { /* n_match is 0 in this case */
		ctrl_m.type = P4T_SET_DEFAULT_ACTION;
	}

	const auto action = table_entry.action();

	switch(action.type_case()) {
		case ::p4::v1::TableAction::TypeCase::kAction:
			ctrl_m.action_type = P4_AT_ACTION; /* ACTION PROFILE IS NOT SUPPORTED */
			const auto tmp_act = action.action();
			action_id = tmp_act.action_id();
			table_entry_new->action.action_id = action_id;
			elem = get_element(&(dm->id_map), action_id);
			ctrl_m.action_name = strdup(elem->value);
			table_entry_new->action.count = 0;
			for (const auto &param: tmp_act.params()) {
				//param = tmp_act->params[i];
				arg = get_argument(elem, param.param_id());
                                param_t* param_new = (param_t*)malloc(sizeof(param_t));
                                param_new->param_id = param.param_id();
				
				char* new_value;
				new_value = malloc(param.value().size());
				memcpy(new_value, param.value().c_str(), param.value().size());
                                param_new->value = new_value;
                                param_new->value_size = param.value().size();
                                table_entry_new->action.param[table_entry_new->action.count++] = param_new;
                                
				ctrl_m.action_params[ctrl_m.num_action_params] = _gen_action_param(arg, param);
				ctrl_m.num_action_params++;
			}
			//status.gcs_code = GOOGLE__RPC__CODE__OK;
			break;
		default:
			status = grpc::Status( grpc::StatusCode::UNIMPLEMENTED, "ActionType is not implemented" );
			break;
	}

	if (status.ok()) {
		// =======文件写入=======
/*
		std::string data;

	
		std::ofstream outfile("/home/ndsc/t4p4s/tableentry.txt",std::ios::app | std::ios::out);
		if(!outfile.is_open()){
			std::cerr<<"Failed to open file,error code : "<<errno<<std::endl; 
		}else{
		std::cout << "Table entry Writing to the file" <<std::endl;
		outfile << ctrl_m.table_name <<" ";
		std::cout << ctrl_m.table_name <<" ";
		outfile << ctrl_m.type <<" ";
		std::cout << ctrl_m.type <<" ";
		outfile << ctrl_m.num_field_matches<<" ";
		std::cout << ctrl_m.num_field_matches<<" ";
		for(int a = 0; a < ctrl_m.num_field_matches; a++){
			outfile << ctrl_m.field_matches[a] <<" ";			
		}
		for(int a = 0; a < ctrl_m.num_field_matches; a++){
			std::cout << ctrl_m.field_matches[a] <<" ";			
		}
		outfile << ctrl_m.num_action_params <<" ";
		std::cout << ctrl_m.num_action_params <<" ";
		for(int a = 0; a < ctrl_m.num_action_params; a++){
			outfile << ctrl_m.action_params[a]<<" ";
		}
		for(int a = 0; a < ctrl_m.num_action_params; a++){
			std::cout << ctrl_m.action_params[a]<<" ";
		}
		outfile << std::endl;
		std::cout << std::endl; 
		std::cout << "File Writted" <<std::endl;
		outfile.close();
		std::cout << "File closed" <<std::endl;
		}
*/
		dm->cb(&ctrl_m);

	}

        return status;
}

grpc::Status table_modify(device_mgr_t *dm, const ::p4::v1::TableEntry &table_entry) {
	grpc::Status status;
	status = grpc::Status( grpc::StatusCode::UNIMPLEMENTED, "table_modify is not implemented" );
	return status;
}

grpc::Status table_delete(device_mgr_t *dm, const ::p4::v1::TableEntry &table_entry) {
	uint32_t table_id;
	uint32_t field_id;
	uint32_t action_id;
	uint16_t value16; /* TODO: remove after testing */
	uint8_t ip[4]; /* TODO: remove after testing */
	size_t i;
	uint8_t match_type;
	int32_t prefix_len = 0; /* in bits */

	grpc::Status status = grpc::Status::OK;
//	::p4::v1::FieldMatch match;
//	::p4::v1::TableAction action;
//	::p4::v1::FieldMatch::Exact exact;
//	::p4::v1::FieldMatch::LPM lpm;
//	::p4::v1::FieldMatch::Ternary ternary;
	
//	::p4::v1::Action tmp_act;
//	::p4::v1::Action::Param param;
	
	table_id = table_entry.table_id();

	argument_t *arg = NULL;
	element_t *elem = get_element(&(dm->id_map), table_id);
	if(!strstr(elem->value, "_0")){
		elem->value = strcat(elem->value,"_0");
	}

	struct p4_ctrl_msg ctrl_m;
	ctrl_m.num_field_matches = 0;
	ctrl_m.num_action_params = 0;
	ctrl_m.type = P4T_REMOVE_TABLE_ENTRY;
	ctrl_m.table_name = strdup(elem->value);

	for (const auto &match: table_entry.match()) {
		field_id = match.field_id();
		arg = get_argument(elem, field_id);
		if (arg==NULL) {
			printf("NULL ARGUMENT for FIELD_ID=%d\n", field_id);
		}
		match_type = match.field_match_type_case();
		if(match_type == 2){
			const auto exact = match.exact();
                        ctrl_m.field_matches[ctrl_m.num_field_matches] = _gen_match_rule_exact(arg, exact);
                        ctrl_m.num_field_matches++;
                        //status.set_code(Code::OK);
		}
		else if(match_type == 4){
			 const auto lpm = match.lpm();
                         prefix_len = lpm.prefix_len();
                         if (lpm.value().size()>=4) {
                                ip[0] = lpm.value().c_str()[0];
                                ip[1] = lpm.value().c_str()[1];
                                ip[2] = lpm.value().c_str()[2];
                                ip[3] = lpm.value().c_str()[3];
                         }
                         printf("LPM MATCH TableID:%:%d (%s) FieldID:%d (%s) KEY_LENGTH:%d VALUE_IP: %d.%d.%d.%d PREFIX_LEN: %d  -- \n", table_id, elem->value, field_id, arg->name, lpm.value().size(), (int)ip[0], (int)ip[1], (int)ip[2], (int)ip[3],  prefix_len);
                         ctrl_m.field_matches[ctrl_m.num_field_matches] = _gen_match_rule_lpm(arg, lpm);
                         ctrl_m.num_field_matches++;
                         status = grpc::Status::OK;
		}
		else if(match_type == 3){
			const auto ternary = match.ternary();
                        printf("TERNARY MATCH TableID:%d (%s) FieldID:%d (%s) KEY_LENGTH:%d VALUE16: %d M_LEN:%d MASK:%d  --\n", table_id, elem->value, field_id, arg->name, ternary.value().size(), ternary.value().c_str()[0], ternary.mask().size(), ternary.mask().c_str()[0]); /* len - length , data - uint8_t* */
                        ctrl_m.field_matches[ctrl_m.num_field_matches] = _gen_match_rule_ternary(arg, ternary);
                        ctrl_m.num_field_matches++;
                                //status.gcs_code = GOOGLE__RPC__CODE__OK;
		}
	}
//return status; } //
	if (table_entry.is_default_action()) { /* n_match is 0 in this case */
		ctrl_m.type = P4T_SET_DEFAULT_ACTION;
	}

	const auto action = table_entry.action();

	switch(action.type_case()) {
		case ::p4::v1::TableAction::TypeCase::kAction:
			ctrl_m.action_type = P4_AT_ACTION; /* ACTION PROFILE IS NOT SUPPORTED */
			const auto tmp_act = action.action();
			action_id = tmp_act.action_id();
			elem = get_element(&(dm->id_map), action_id);
			ctrl_m.action_name = strdup(elem->value);
			for (const auto &param: tmp_act.params()) {
				//param = tmp_act->params[i];
				arg = get_argument(elem, param.param_id());
				ctrl_m.action_params[ctrl_m.num_action_params] = _gen_action_param(arg, param);
				ctrl_m.num_action_params++;
			}
			//status.gcs_code = GOOGLE__RPC__CODE__OK;
			break;
		default:
			status = grpc::Status( grpc::StatusCode::UNIMPLEMENTED, "ActionType is not implemented" );
			break;
	}

	if (status.ok()) {
/*		std::string data;
		std::string line;
		std::string line1;

		std::ifstream infile;
		std::ofstream outfile;
		infile.open("/home/ndsc/t4p4s/tableentry.txt");
		outfile.open("/home/ndsc/t4p4s/tableentry-trans.txt",std::ios::trunc);
		if(!infile.is_open()&&!outfile.is_open()){
			std::cerr<<"Failed to open file,error code : "<<errno<<std::endl; 
		}else{
			while(!infile.eof()){
				std::getline(infile,line);
				if (strcmp(line[0],ctrl_m.table_name) != 0){
					outfile<<line<<std::endl;
				}
			}
		}
		infile.close();
		outfile.close();

		std::ifstream newinfile;
		std::ofstream newoutfile;
		newinfile.open("/home/ndsc/t4p4s/tableentry-trans.txt");
		newoutfile.open("/home/ndsc/t4p4s/tableentry.txt",std::ios::trunc);
		if(!newinfile.is_open()&&!newoutfile.is_open()){
			std::cerr<<"Failed to open file,error code : "<<errno<<std::endl; 
		}else{
			while(!newinfile.eof()){
				std::getline(newinfile,line1);
				newoutfile<<line1<<std::endl;
			}

            newinfile.close();
		    newoutfile.close();
			std::cout << "File Table Deleted" <<std::endl;
*/			
		    dm->cb(&ctrl_m);
//	    }
	}

        return status;
}

bool check_p4_id(uint32_t id, int type) {
	return true;
}

grpc::Status table_write(device_mgr_t *dm, ::p4::v1::Update::Type update, const ::p4::v1::TableEntry &table_entry) {
	grpc::Status status;
	if (!check_p4_id(table_entry.table_id(), P4IDS_TABLE)) {
		status = grpc::Status( grpc::StatusCode::UNKNOWN, "P4ID is not unknown" );
		return status; /*TODO: more informative error msg is needed!!!*/
	}

	switch (update) {
		case ::p4::v1::Update::UNSPECIFIED:
			status = grpc::Status( grpc::StatusCode::INVALID_ARGUMENT, "Invalid argument" );
			/*TODO: more informative error msg is needed!!!*/
	        	break;
		case ::p4::v1::Update::INSERT:
			return table_insert(dm, table_entry);
		case ::p4::v1::Update::MODIFY:
			return table_modify(dm, table_entry);
		case ::p4::v1::Update::DELETE:
			return table_delete(dm, table_entry);
		default:
			status = grpc::Status( grpc::StatusCode::UNKNOWN, "Unknown update message" );
			/*TODO: more informative error msg is needed!!!*/
			break;
	}
	return status;
}

// Copied from p4lang/PI
class P4ErrorReporter {
 public:
  void push_back(const ::p4::v1::Error &error) {
    //if (!error.canonical_code().ok())
    //  errors.emplace_back(index, error);
    //index++;
  }

  // TODO(antonin): remove this overload when we generalize the use of
  // p4v1::Error in the code?
  void push_back(const grpc::Status &status) {
    //if (!status.ok()) {
    //  ::p4::v1::Error error;
      //error.set_canonical_code(status.error_code());
      //error.set_message(status.message());
      //error.set_space("ALL-sswitch-p4org");
      //errors.emplace_back(index, error);
    //}
    //index++;
  }

  grpc::Status get_status() const {
    grpc::Status status;
    if (errors.empty()) {
      status = grpc::Status::OK;
    } else {
      status = grpc::Status(grpc::StatusCode::UNKNOWN, "Unknown");
/*      ::p4::v1::Error success;
      success.set_code(Code::OK);
      status.set_code(Code::UNKNOWN);
      size_t i = 0;
      for (const auto &p : errors) {
        for (; i++ < p.first;) {
          auto success_any = status.add_details();
          success_any->PackFrom(success);
        }
        auto error_any = status.add_details();
        error_any->PackFrom(p.second);
      }
      // add trailing OKs
      for (; i++ < index;) {
        auto success_any = status.add_details();
        success_any->PackFrom(success);
      }*/
    }
    return status;
  }

 private:
  std::vector<std::pair<size_t, ::p4::v1::Error> > errors{};
  size_t index{0};
};

grpc::Status register_write(device_mgr_t *dm, ::p4::v1::Update::Type update, const ::p4::v1::RegisterEntry &register_entry) {
        grpc::Status status;
        if (!check_p4_id(register_entry.register_id(), P4IDS_REGISTER)) {
                status = grpc::Status( grpc::StatusCode::UNKNOWN, "P4ID is not unknown" );
                return status; /*TODO: more informative error msg is needed!!!*/
        }
	if (!register_entry.has_index()) {
		status = grpc::Status( grpc::StatusCode::UNIMPLEMENTED, "Wildcard write is not supported" );
		return status;
	}
	if (register_entry.index().index() < 0) {
		status = grpc::Status( grpc::StatusCode::UNIMPLEMENTED, "A negative number is not a valid index value." );
		return status;
	}
	int index = register_entry.index().index();
	auto data = register_entry.data().bitstring();
	std::cout << data <<std::endl;
	int value = std::stoi(data);
	std::cout << value <<std::endl;
	auto register_id = register_entry.register_id();
	std::cout << register_id <<std::endl;
	element_t *elem = get_element(&(dm->id_map), register_id);
	if(!strstr(elem->value, "_0")){
		elem->value = strcat(elem->value,"_0");
	}
	
	std::cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++" <<std::endl;

	switch(update) {
		case ::p4::v1::Update::MODIFY:
		case ::p4::v1::Update::DELETE:
		case ::p4::v1::Update::INSERT:
			grpc::Status status;
	
			int size_bytes   = -1;
			std::cout << elem->value <<std::endl;
			uint32_t* register_values_bytes = dm->write_register_by_name(elem->value, &size_bytes, index, value);
			std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++" <<std::endl;
			if ((register_values_bytes == 0x0 || size_bytes == -1) ){
				return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT, "Register cannot be found %s",elem->value );
			}
			status = grpc::Status::OK;		
		default:
			status = grpc::Status( grpc::StatusCode::UNKNOWN, "Unknown update message" );
                        /*TODO: more informative error msg is needed!!!*/
                        break;

	}

	return status; 
}

grpc::Status dev_mgr_write(device_mgr_t *dm, const ::p4::v1::WriteRequest &request, ::p4::v1::WriteResponse *response) {
	grpc::Status status = grpc::Status::OK;
	size_t i;

	if (request.atomicity() != ::p4::v1::WriteRequest::CONTINUE_ON_ERROR) {
		status = grpc::Status(grpc::StatusCode::UNIMPLEMENTED, "Not implemented");
		return status;
	}

	P4ErrorReporter error_reporter;

	for (const auto &update : request.updates()) {
		const auto entity = update.entity();
		switch(entity.entity_case()) {
			case ::p4::v1::Entity::kTableEntry:
				status = table_write(dm, update.type(), entity.table_entry());
				break;
			case ::p4::v1::Entity::kRegisterEntry:
				status = register_write(dm, update.type(), entity.register_entry());
				break;
			default:
				status = grpc::Status(grpc::StatusCode::UNKNOWN, "Entity case is unknown");
				break;
		}
		/* TODO:collect multiple status messages - now we assume a simple update */
		error_reporter.push_back(status);
	}
	
	return error_reporter.get_status();
}
// void table_read_one(uint32_t* table_values_bytes, uint32_t size_bytes, int index, ::p4::v1::TableEntry *entry) {
// 	if (table_values_bytes != 0x0 && size_bytes > 0){
// 		entry->mutable_data()->set_bitstring(table_values_bytes[index]);
// 		//printf("COUNTER VALUE BYTES: %d\n", counter_values_bytes[index]);
// 	}
// }


grpc::Status table_read(device_mgr_t *dm,const ::p4::v1::TableEntry &table_entry,::p4::v1::ReadResponse *response) {
	grpc::Status status;
        uint32_t table_id;
	uint32_t field_id;
	uint32_t action_id;
	uint16_t value16; /* TODO: remove after testing */
	uint8_t ip[4]; /* TODO: remove after testing */
	uint8_t match_type;
	size_t i;
	int32_t prefix_len = 0; /* in bits */
	// if (!check_p4_id(table_entry.table_id(), P4IDS_TABLE)) {
	// 	status = grpc::Status( grpc::StatusCode::UNKNOWN, "P4ID is not unknown" );
	// 	return status; /*TODO: more informative error msg is needed!!!*/
	// }
        table_id = table_entry.table_id();
	uint64_t table_values_bytes = 0;
	if (dm->id_table_entries.count > 0) {
		table_values_bytes =  dm->id_table_entries.count;
	}
	for(int index = 0; index < table_values_bytes ; index++){
		table_entry_t *table_entry_new = get_table_entry(&(dm->id_table_entries),table_id,index);
		if (table_entry_new != NULL){
                        auto entry = response->add_entities()->mutable_table_entry();
                        entry->set_table_id(table_id);
                        
                        for(int i = 0; i < table_entry_new->count; i++)
                        {
                             auto match = entry->add_match();
                             match->set_field_id(table_entry_new->matches[i]->field_id);
                             if(table_entry_new->matches[i]->match_type == 4)
                              {
                                  auto lpm = match->mutable_lpm();
                                  lpm->set_prefix_len(table_entry_new->matches[i]->prefix_len);
                                  lpm->set_value(table_entry_new->matches[i]->value);
                              }
                              else if(table_entry_new->matches[i]->match_type == 2)
                              {
                                  auto exact = match->mutable_exact();
                                  exact->set_value(table_entry_new->matches[i]->value, table_entry_new->matches[i]->value_size);
                              }
                        }
                        action_t action_new = table_entry_new->action;
                        auto action = entry->mutable_action()->mutable_action();
                        action->set_action_id(action_new.action_id);
                        for (int i = 0; i < action_new.count; i++)
                        {
                             auto param = action->add_params();
                             param->set_param_id(action_new.param[i]->param_id);
                             param->set_value(action_new.param[i]->value, action_new.param[i]->value_size);
                        }
         }
                        
	}
	

    
    
    

    // if(table_values_bytes == 0X00){
    //     return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT, "Table cannot be found %s",elem->value );
    // }else if(table_values_bytes != 0X00){
    //     char bytes_string[32];
    //     snprintf(bytes_string, sizeof(bytes_string), "%d", table_values_bytes);
    //     entry->set_table_id(table_id);
    //     // entry->mutable_data()->set_bitstring(bytes_string);
    //     std::cout << bytes_string <<std::endl;
    // }

    
	// auto index_msg = entry->mutable_index();
	// index_msg->set_index(index);
	// table_read_one(table_values_bytes, size_bytes, index, entry);

    status = grpc::Status::OK;
	// std::cout << "--------------------------------------------------------" <<std::endl;
	    // return status;
    // }else{
    //     status = grpc::Status( grpc::StatusCode::UNKNOWN, "table id negative" );
    //             	return status;
    // }

	return status;
}

void register_read_one(uint32_t* register_values_bytes, uint32_t size_bytes, int index, ::p4::v1::RegisterEntry *entry) {
	if (register_values_bytes != 0x0 && size_bytes > 0){
		entry->mutable_data()->set_bitstring(register_values_bytes[index]);
		//printf("COUNTER VALUE BYTES: %d\n", counter_values_bytes[index]);
	}
}

grpc::Status register_read(device_mgr_t *dm, const ::p4::v1::RegisterEntry &register_entry, ::p4::v1::ReadResponse *response) {
        grpc::Status status;
        if (!check_p4_id(register_entry.register_id(), P4IDS_REGISTER) && register_entry.register_id()!=0) {
                status = grpc::Status( grpc::StatusCode::UNKNOWN, "P4ID is not unknown" );
                return status; /*TODO: more informative error msg is needed!!!*/
        }
	
	auto register_id = register_entry.register_id();
	element_t *elem = get_element(&(dm->id_map), register_id);
	if(!strstr(elem->value, "_0")){
		elem->value = strcat(elem->value,"_0");
	}
	std::cout << elem->value <<std::endl;
	int size_bytes   = -1;
	uint32_t* register_values_bytes = dm->read_register_by_name(elem->value, &size_bytes);
	if ((register_values_bytes == 0x0 || size_bytes == -1) ){
		return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT, "Register cannot be found %s",elem->value );
	}
#if 1
	if (register_entry.has_index()) {
		
		/*TODO: read one element as in https://github.com/p4lang/PI/blob/master/proto/frontend/src/device_mgr.cpp*/
		if (register_entry.index().index() < 0) {
			status = grpc::Status( grpc::StatusCode::UNKNOWN, "Register index negative" );
                	return status;
		}
		auto index = static_cast<size_t>(register_entry.index().index());
		auto entry = response->add_entities()->mutable_register_entry();
		std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++" <<std::endl;
		entry->CopyFrom(register_entry);
		//register_read_one(register_values_bytes, size_bytes, index, entry);
		if(register_values_bytes != 0x0 && size_bytes > 0){
			uint8_t bytes_string[32];
			snprintf(bytes_string, sizeof(bytes_string), "%d", register_values_bytes[index]);
			//std::string bytes_string = std::to_string(register_values_bytes[index]);
			std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++" <<std::endl;
			entry->mutable_data()->set_bitstring(bytes_string);
			std::cout << bytes_string <<std::endl;
			
		}
		status = grpc::Status::OK;
		std::cout << "--------------------------------------------------------" <<std::endl;
	        return status;
	} 

	/* TODO: read all elements as in https://github.com/p4lang/PI/blob/master/proto/frontend/src/device_mgr.cpp */

	for (int index = 0; index < size_bytes; index++){
		auto entry = response->add_entities()->mutable_register_entry();
		entry->set_register_id(register_id);
		auto index_msg = entry->mutable_index();
		index_msg->set_index(index);
		register_read_one(register_values_bytes, size_bytes, index, entry);
	}
#endif	
	status = grpc::Status::OK;
        return status;
}

grpc::Status dev_mgr_read_one(device_mgr_t *dm, const ::p4::v1::Entity &entity, ::p4::v1::ReadResponse *response){
	grpc::Status status(grpc::StatusCode::OK, "ok");
	switch(entity.entity_case()){
		case ::p4::v1::Entity::kTableEntry:{
			status = table_read(dm, entity.table_entry(), response);
			break;}
		case ::p4::v1::Entity::kRegisterEntry:{
			status = register_read(dm, entity.register_entry(), response);
			break;}
		default:{
			status = grpc::Status(grpc::StatusCode::UNKNOWN, "Entity case is unknown");}
		
	}
	return status;
}

grpc::Status dev_mgr_read(device_mgr_t *dm, const ::p4::v1::ReadRequest &request, ::p4::v1::ReadResponse *response){
	grpc::Status status(grpc::StatusCode::OK, "ok");
	for(const auto &entity : request.entities()){
		status = dev_mgr_read_one(dm, entity, response);
		if(!status.ok()) break;
	}
	return grpc::Status::OK;
}

grpc::Status dev_mgr_set_pipeline_config(device_mgr_t *dm, ::p4::v1::SetForwardingPipelineConfigRequest_Action action, const ::p4::v1::ForwardingPipelineConfig config) {
	using SetConfigRequest = ::p4::v1::SetForwardingPipelineConfigRequest;
	grpc::Status status(grpc::StatusCode::OK, "ok");

	if (action == SetConfigRequest::VERIFY ||
        action == SetConfigRequest::VERIFY_AND_SAVE ||
        action == SetConfigRequest::VERIFY_AND_COMMIT ||
        action == SetConfigRequest::RECONCILE_AND_COMMIT) {
		printf("P4Info configuration received...0000\n");
		dm->p4info.CopyFrom( config.p4info() );
		dm->has_p4info = 1;
		config_cookie.CopyFrom(config.cookie());
		saved_device_config.change_config(config); // TODO: return value
		has_config_cookie = true;
		element_t *elem;
		size_t i,j;
		for (const auto &registers : dm->p4info.registers()) {
                        const auto &pre = registers.preamble();
                        printf("  [#] REGISTER id: %d; name: %s\n", pre.id(), pre.name().c_str());
                        elem = add_element(&(dm->id_map), pre.id(), pre.name().c_str());
                }
		for (const auto &table : dm->p4info.tables()) {
			const auto &pre = table.preamble();
			printf("  [+] TABLE id: %d; name: %s\n", pre.id(), pre.name().c_str());
			elem = add_element(&(dm->id_map), pre.id(), pre.name().c_str());
	                if (elem == NULL) {
        	                printf("   +-----> ERROR\n");
                	        break;
                	}
			for (const auto &mf : table.match_fields()) {
				printf("   +-----> MATCH FIELD; name: %s; id: %d; bitwidth: %d\n", mf.name().c_str(), mf.id(), mf.bitwidth());
				strcpy(elem->args[elem->n_args].name, mf.name().c_str());
	                        elem->args[elem->n_args].id = mf.id();
        	                elem->args[elem->n_args].bitwidth = mf.bitwidth();
                	        elem->n_args++;
			}

                }
		for (const auto &taction : dm->p4info.actions()) {
			const auto &pre = taction.preamble();
			printf("  [#] ACTION id: %d; name: %s\n", pre.id(), pre.name().c_str());
                	elem = add_element(&(dm->id_map), pre.id(), pre.name().c_str());
                	for (const auto &param : taction.params()) {
                        	printf("   #-----> ACTION PARAM; name: %s; id: %d; bitwidth: %d\n", param.name().c_str(), param.id(), param.bitwidth());
                        	strcpy(elem->args[elem->n_args].name, param.name().c_str());
                        	elem->args[elem->n_args].id = param.id();
	                        elem->args[elem->n_args].bitwidth = param.bitwidth();
        	                elem->n_args++;
                	}
		}

		return status;
	}

	return status; // TODO
}

grpc::Status dev_mgr_get_pipeline_config(device_mgr_t *dm, ::p4::v1::GetForwardingPipelineConfigRequest::ResponseType response_type, ::p4::v1::ForwardingPipelineConfig *config) {
	using GetConfigRequest = ::p4::v1::GetForwardingPipelineConfigRequest;
	switch (response_type) {
      		case GetConfigRequest::ALL:
        		config->mutable_p4info()->CopyFrom(dm->p4info);
        		saved_device_config.read_config(config);
        		break;
      		case GetConfigRequest::COOKIE_ONLY:
        		break;
      		case GetConfigRequest::P4INFO_AND_COOKIE:
        		config->mutable_p4info()->CopyFrom(dm->p4info);
        		break;
      		case GetConfigRequest::DEVICE_CONFIG_AND_COOKIE:
        		saved_device_config.read_config(config);
        		break;
      		default:
			return grpc::Status::OK;
    	}
	if (has_config_cookie)
      		config->mutable_cookie()->CopyFrom(config_cookie);
	return grpc::Status::OK;
}

/*
TODO by Ian 
packet-in功能调用
*/

// class DeviceState {
//  public:
//   static constexpr size_t max_connections = 16;

//   explicit DeviceState(DeviceMgr::device_id_t device_id)
//       : device_id(device_id){ }

// //   DeviceMgr *get_p4_mgr() {
// //     return device_mgr.get();
// //   }

//   DeviceMgr *get_or_add_p4_mgr() {
//     if (device_mgr == nullptr) {
//       device_mgr.reset(new DeviceMgr(device_id));
//     //   auto status = device_mgr->server_config_set(
//     //       server_config.get_config());
//     //   assert(status.code() == ::google::rpc::Code::OK);
//     }
//     return device_mgr.get();
//   }

//   void send_stream_message(p4v1::StreamMessageResponse *msg) {
//     // auto master = get_master();
//     // if (master == nullptr) return;
//     // auto stream = master->stream();
// 	// StreamChannelReaderWriter *stream = New StreamChannelReaderWriter;
//     auto success = stream_store->Write(*msg);
//     if (msg->has_packet() && success) {
//       std::cout << "PACKET IN\n";
//     //   pkt_in_count++;
//     }
//   }

// //   void process_stream_message_request(
// //       Connection *connection, const p4v1::StreamMessageRequest &request) {
// //     // these are handled directly by StreamChannel
// //     assert(request.update_case() != p4v1::StreamMessageRequest::kArbitration);
// //     std::lock_guard<std::mutex> lock(m);
// //     if (!is_master(connection)) return;
// //     if (device_mgr == nullptr) return;
// //     device_mgr->stream_message_request_handle(request);
// //     if (request.update_case() == p4v1::StreamMessageRequest::kPacket) {
// //       SIMPLELOG << "PACKET OUT\n";
// //       pkt_out_count++;
// //     }
// //   }

// //   bool is_master(const Uint128 &election_id) const {
// //     std::lock_guard<std::mutex> lock(m);
// //     auto master = get_master();
// //     return (master == nullptr) ? false : (master->election_id() == election_id);
// //   }

//  private:

//   uint64_t pkt_in_count{0};
//   uint64_t pkt_out_count{0};
//   std::unique_ptr<DeviceMgr> device_mgr{nullptr};
//   DeviceMgr::device_id_t device_id;
// };

// class Devices {
//  public:
//   static DeviceState *get(DeviceMgr::device_id_t device_id) {
//     auto &instance = get_instance();
//     auto &map = instance.device_map;
//     auto it = map.find(device_id);
//     if (it != map.end()) return it->second.get();
//     auto device = new DeviceState(device_id);
//     map.emplace(device_id, std::unique_ptr<DeviceState>(device));
//     return device;
//   }

//  private:
//   static Devices &get_instance() {
//     static Devices devices;
//     return devices;
//   }

//   std::unordered_map<DeviceMgr::device_id_t,
//                      std::unique_ptr<DeviceState> > device_map{};
// };

bool pi_packetin_receive(const char *pkt,
                                size_t size) {
	if (packet_cb_mgr_ptr->default_cb.cb == NULL)
	{
		return false;
	}else {
  		const cb_data_t cb_data = packet_cb_mgr_ptr->default_cb;
  		((PIPacketInCb)(cb_data.cb))(pkt, size, cb_data.cookie);
		return true;
	}
}

void async_packetin_data(uint8_t* data, int len) {
	auto status = pi_packetin_receive(data, static_cast<size_t>(len));
	// const cb_data_t *cb_data = cb_mgr_get_or_default(&packet_cb_mgr_t, device_id);
	// if(cb_data){
	// 	((PIPacketIncb)(cb_data->cb))(data, len, cb_data->cookie);
	// 	printf("PIPacketIncb callback success");
	// }
	// new StreamChannelImpl(&(asyn_server_data_in->service), asyn_server_data_in->cq.get(), data, true, true, len);
}



void dev_mgr_init(device_mgr_t *dev_mgr_ptr) {
	// auto status = DeviceMgr::init();
	init_map(&(dev_mgr_ptr->id_map));
	init_table_entries(&(dev_mgr_ptr->id_table_entries));
	dev_mgr_ptr->has_p4info = 0;
}

void cb_mgr_init(cb_mgr_t *cb_mgr) {
  cb_mgr->device_cbs._map = NULL;
  cb_mgr->default_cb.cb = NULL;
  cb_mgr->default_cb.cookie = NULL;
}

void dev_mgr_init_with_t4p4s(device_mgr_t *dev_mgr_ptr, p4_msg_callback cb, p4_reg_read read_register_by_name, p4_reg_write write_register_by_name, uint64_t device_id) {
	dev_mgr_init(dev_mgr_ptr);
	cb_mgr_init(packet_cb_mgr_ptr);
	dev_mgr_ptr->cb = cb;
	dev_mgr_ptr->read_register_by_name = read_register_by_name;
	dev_mgr_ptr->write_register_by_name = write_register_by_name;
	dev_mgr_ptr->device_id = device_id;
	// auto device = Devices::get(device_id);
	// auto device_mgr = device->get_or_add_p4_mgr();
	// device_mgr->stream_message_response_register_cb(stream_message_response_cb, NULL);
}

// void stream_message_response_cb(DeviceMgr::device_id_t device_id,
//                                 p4v1::StreamMessageResponse *msg,
//                                 void *cookie) {
//   (void) cookie;
//   Devices::get(device_id)->send_stream_message(msg);
// }id;
//}
