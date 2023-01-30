#include "device_mgr.h"
#include "google/rpc/code.pb.h"
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <arpa/inet.h>
#include <string>

extern struct p4_ctrl_msg* switch_m;
using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerWriter;
using grpc::ServerReaderWriter;
using grpc::Status;
using grpc::StatusCode;

namespace p4v1 = ::p4::v1;
extern std::unique_ptr<ServerCompletionQueue> cq_;
struct ConfigFile {
 public:
  ConfigFile() { }

  ~ConfigFile() {
    if (fp != nullptr) std::fclose(fp);
  }

  grpc::Status change_config(const ::p4::v1::ForwardingPipelineConfig &config_proto) {
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

  grpc::Status read_config(::p4::v1::ForwardingPipelineConfig *config_proto) {
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
    auto *device_config = config_proto->mutable_p4_device_config();
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
  std::FILE *fp{nullptr};
  size_t size{0};
};

ConfigFile saved_device_config;
bool has_config_cookie{false};
::p4::v1::ForwardingPipelineConfig::Cookie config_cookie;

struct p4_field_match_header* _gen_match_rule_exact(argument_t *arg, const ::p4::v1::FieldMatch::Exact &exact) {
	struct p4_field_match_exact *result = malloc(sizeof(struct p4_field_match_exact));

	strcpy(result->header.name, arg->name);
	result->header.type = P4_FMT_EXACT;
	result->length = arg->bitwidth;
	memcpy(result->bitmap, exact.value().c_str(), exact.value().size());

	return (struct p4_field_match_header*)result; /* TODO: NTOH !!! */
}

struct p4_field_match_header* _gen_match_rule_lpm(argument_t *arg, const ::p4::v1::FieldMatch::LPM &lpm) {
        struct p4_field_match_lpm *result = malloc(sizeof(struct p4_field_match_lpm));

        strcpy(result->header.name, arg->name);
        result->header.type = P4_FMT_LPM;
        result->prefix_length = lpm.prefix_len();
        memcpy(result->bitmap, lpm.value().c_str(), lpm.value().size());

        return (struct p4_field_match_header*)result; /* TODO: NTOH !!! */
}

struct p4_field_match_header* _gen_match_rule_ternary(argument_t *arg, const ::p4::v1::FieldMatch::Ternary &ternary) {
        struct p4_field_match_ternary *result = malloc(sizeof(struct p4_field_match_ternary));

        strcpy(result->header.name, arg->name);
        result->header.type = P4_FMT_TERNARY;
        result->length = arg->bitwidth;
        memcpy(result->bitmap, ternary.value().c_str(), ternary.value().size());
	memcpy(result->mask, ternary.mask().c_str(), ternary.mask().size());

        return (struct p4_field_match_header*)result; /* TODO: NTOH !!! */
}

struct p4_action_parameter* _gen_action_param(argument_t *arg, const ::p4::v1::Action::Param &param) {
	struct p4_action_parameter *result = malloc(sizeof(struct p4_action_parameter));
	uint16_t *tmp16;
	uint32_t *tmp32;

	strcpy(result->name, arg->name);
	result->length = arg->bitwidth;
	memcpy(result->bitmap, param.value().c_str(), param.value().size());
	if (param.value().size()==2) {
		tmp16 = (uint16_t*)result->bitmap;
		*tmp16 = htons(*tmp16);
	} else if (param.value().size()==4) {
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
	std::cout<<"table_id----------------:"<<table_id<<std::endl;

	element_t *elem = get_element(&(dm->id_map), table_id);
	argument_t *arg = NULL;

	struct p4_ctrl_msg ctrl_m;
	ctrl_m.num_field_matches = 0;
	ctrl_m.num_action_params = 0;
	ctrl_m.type = P4T_ADD_TABLE_ENTRY;
	ctrl_m.table_name = strdup(elem->value);

	for (const auto &match: table_entry.match()) {
		field_id = match.field_id();
		arg = get_argument(elem, field_id);
		if (arg==NULL) {
			printf("NULL ARGUMENT for FIELD_ID=%d\n", field_id);
		}
		switch(match.field_match_type_case()) {
			case ::p4::v1::FieldMatch::FieldMatchTypeCase::kExact:
				const auto exact = match.exact();
				ctrl_m.field_matches[ctrl_m.num_field_matches] = _gen_match_rule_exact(arg, exact);
				ctrl_m.num_field_matches++;	
				//status.set_code(Code::OK);
				break;
			case ::p4::v1::FieldMatch::FieldMatchTypeCase::kLpm:
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
				break;
			case ::p4::v1::FieldMatch::FieldMatchTypeCase::kTernary:
				const auto ternary = match.ternary();
				printf("TERNARY MATCH TableID:%d (%s) FieldID:%d (%s) KEY_LENGTH:%d VALUE16: %d M_LEN:%d MASK:%d  --\n", table_id, elem->value, field_id, arg->name, ternary.value().size(), ternary.value().c_str()[0], ternary.mask().size(), ternary.mask().c_str()[0]); /* len - length , data - uint8_t* */
				ctrl_m.field_matches[ctrl_m.num_field_matches] = _gen_match_rule_ternary(arg, ternary);
				ctrl_m.num_field_matches++;
                                //status.gcs_code = GOOGLE__RPC__CODE__OK;
                                break;

			case ::p4::v1::FieldMatch::FieldMatchTypeCase::kRange:	
			default:
				status = grpc::Status( grpc::StatusCode::UNIMPLEMENTED, "MatchType is not implemented" );
				break;
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
	size_t i;
	int j;
	int32_t prefix_len = 0;
	
	grpc::Status status = grpc::Status::OK;
	//status = grpc::Status( grpc::StatusCode::UNIMPLEMENTED, "table_delete is not implemented" );

	table_id = table_entry.table_id();

	element_t *elem = get_element(&(dm->id_map), table_id);
	argument_t *arg = NULL;

	struct p4_ctrl_msg ctrl_m;
	ctrl_m.num_field_matches = 0;
	ctrl_m.num_action_params = 0;
	ctrl_m.type = P4T_REMOVE_TABLE_ENTRY;
	ctrl_m.table_name = strdup(elem->value);

	for(const auto &match: table_entry.match()) {
		field_id = match.field_id();
		arg = get_argument(elem,field_id);
		if(arg==NULL) {
			printf("NULL ARGUMENT for FIELD_ID=%d\n",field_id);
		}
		switch (match.field_match_type_case())
		{
		case ::p4::v1::FieldMatch::FieldMatchTypeCase::kExact:
			const auto exact = match.exact();
			for(j = 0;j < sizeof(ctrl_m.field_matches)/sizeof(ctrl_m.field_matches[0]);j++){
				if(ctrl_m.field_matches[j] = _gen_match_rule_exact(arg, exact)){
					break;
				}
			}
			for(int k = j;k < sizeof(ctrl_m.field_matches)/sizeof(ctrl_m.field_matches[0]);k++){
				ctrl_m.field_matches[k] = ctrl_m.field_matches[k+1];
			}
			break;
		case ::p4::v1::FieldMatch::FieldMatchTypeCase::kLpm:
			const auto lpm = match.lpm();
			for(j = 0; j < sizeof(ctrl_m.field_matches)/sizeof(ctrl_m.field_matches[0]);j++){
				if(ctrl_m.field_matches[j] = _gen_match_rule_lpm(arg, lpm)){
					break;
				}
			}
			for(int k = j;k < sizeof(ctrl_m.field_matches)/sizeof(ctrl_m.field_matches[0]);k++){
				ctrl_m.field_matches[k] = ctrl_m.field_matches[k+1];
			}
			break;
		
		default:
			break;
		}
	}

	/*const auto action = table_entry.action();

	switch(action.type_case()) {
		case ::p4::v1::TableAction::TypeCase::kAction:
			ctrl_m.action_type = P4_AT_ACTION; //ACTION PROFILE IS NOT SUPPORTED 
			const auto tmp_act = action.action();
			action_id = tmp_act.action_id();
			elem = get_element(&(dm->id_map), action_id);
			ctrl_m.action_name = strdup(elem->value);
            const auto &param: tmp_act.params();

			for(j = 0;j < sizeof(ctrl_m.action_params)/sizeof(ctrl_m.action_params[0]);j++){
				if(ctrl_m.action_params[j] = _gen_match_rule_exact(arg, param)){
					break;
				}
			}
			for(int k = j;k < sizeof(ctrl_m.action_params)/sizeof(ctrl_m.action_params[0]);k++){
				ctrl_m.action_params[k] = ctrl_m.action_params[k+1];
			}
			break;
			//status.gcs_code = GOOGLE__RPC__CODE__OK;
			break;
		default:
			status = grpc::Status( grpc::StatusCode::UNIMPLEMENTED, "ActionType is not implemented" );
			break;
	}*/
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
		dm->cb(&ctrl_m);
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
			default:
				status = grpc::Status(grpc::StatusCode::UNKNOWN, "Entity case is unknown");
				break;
		}
		/* TODO:collect multiple status messages - now we assume a simple update */
		error_reporter.push_back(status);
	}
	
	return error_reporter.get_status();
}

grpc::Status dev_mgr_read(device_mgr_t *dm, const ::p4::v1::ReadRequest &request, ::p4::v1::ReadResponse *response) {
	grpc::Status status = grpc::Status::OK;
	//size_t i;
	//if (request.atomicity() != ::p4::v1::ReadRequest::CONTINUE_ON_ERROR) {
	//	status = grpc::Status(grpc::StatusCode::UNIMPLEMENTED, "Not implemented");
	//	return status;
	//}
	//P4ErrorReporter error_reporter;
	return grpc::Status::OK;
}

grpc::Status dev_mgr_set_pipeline_config(device_mgr_t *dm, ::p4::v1::SetForwardingPipelineConfigRequest_Action action, const ::p4::v1::ForwardingPipelineConfig config) {
	using SetConfigRequest = ::p4::v1::SetForwardingPipelineConfigRequest;
	grpc::Status status(grpc::StatusCode::OK, "ok");

	if (action == SetConfigRequest::VERIFY ||
        action == SetConfigRequest::VERIFY_AND_SAVE ||
        action == SetConfigRequest::VERIFY_AND_COMMIT ||
        action == SetConfigRequest::RECONCILE_AND_COMMIT) {
		printf("P4Info configuration received...\n");
		dm->p4info.CopyFrom( config.p4info() );
		dm->has_p4info = 1;
		config_cookie.CopyFrom(config.cookie());
		saved_device_config.change_config(config); // TODO: return value
		has_config_cookie = true;
		element_t *elem;
		size_t i,j;
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

// grpc::Status controller_packet_out(device_mgr_t *dm, const ::p4::v1::PacketMetadata &packetmetadata, const char* payload) {
// 	grpc::Status status = grpc::Status::OK;
// 	struct p4_ctrl_msg ctrl_m;
// 	ctrl_m.type = P4T_PACKET_OUT;
// 	ctrl_m.portid = packetmetadata.value();
// 	ctrl_m.packet = payload;
// 	if (status.ok()) {
// 		dm->cb(&ctrl_m);
// 	}
// 	return status;
// }

grpc::Status dev_mgr_packet(device_mgr_t *dm, const ::p4::v1::StreamMessageRequest &request, grpc::ServerReaderWriter<::p4::v1::StreamMessageResponse, ::p4::v1::StreamMessageRequest> *stream) {
	grpc::Status status = grpc::Status::OK;
	::p4::v1::StreamMessageResponse response;
	// p4v1::PacketOut* packetout;
	std::string payload = request.packet().payload();
	struct p4_ctrl_msg ctrl_m;
	ctrl_m.type = P4T_PACKET_OUT;
	ctrl_m.packet = const_cast<char*>(payload.data());
	// packetout = request.mutable_packet();
	// std::cout << packetout->payload() << std::endl;
	// std::cout << packetout->metadata_size() << std::endl;
	int size = request.packet().metadata_size();
	for(int i = 0; i < size; i++) {
		const auto &data = request.packet().metadata(i);
		// p4v1::PacketMetadata* packetmedadata;
		// packetmedadata = packetout->mutable_metadata(1);
		// std::cout << packetmedadata->value() << std::endl;
		// std::cout << packetmedadata->metadata_id() << std::endl;
		switch (data.metadata_id())
		{
		case 1:{
			const auto value1 = data.value();
			std::cout << "src mac: " << value1 << std::endl;
			ctrl_m.metadata[1] = (uint8_t*)const_cast<char*>(value1.data());
		}			
			break;
		case 2:{
			const auto value2 = data.value();
			std::cout << "dst mac: " << value2 << std::endl;
			ctrl_m.metadata[2] = (uint8_t*)const_cast<char*>(value2.data());
		}			
			break;
		case 3:{
			const auto value3 = data.value();
			std::cout << "type: " << value3 << std::endl;
			ctrl_m.metadata[3] = (uint8_t*)const_cast<char*>(value3.data());
		}			
			break;
		case 4:{
			const auto value4 = data.value();
			std::cout << "src ip: " << value4 << std::endl;
			ctrl_m.metadata[4] = (uint8_t*)const_cast<char*>(value4.data());
		}			
			break;
		case 5:{
			const auto value5 = data.value();
			std::cout << "dst ip: " << value5 << std::endl;
			ctrl_m.metadata[5] = (uint8_t*)const_cast<char*>(value5.data());
		}			
			break;
		case 6:{
			const auto value6 = data.value();
			std::cout << "fwd port: " << value6 << std::endl;
			ctrl_m.metadata[6] = (uint8_t*)const_cast<char*>(value6.data());
		}			
			break;
		default:
			break;
		} 
	}
	if (status.ok()) {
 		dm->cb(&ctrl_m);
 	}
	auto packet = response.mutable_packet();
	packet->set_payload(switch_m->packet);
	::p4::v1::PacketMetadata* metadata = packet->add_metadata();
	metadata->set_metadata_id(1);
	metadata->set_value((char*)switch_m->metadata[1]);
    ::p4::v1::PacketMetadata* metadata1 = packet->add_metadata();
	metadata1->set_metadata_id(2);
	metadata1->set_value((char*)switch_m->metadata[2]);
	stream->Write(response);
	return status;
}

class StreamChannelImpl {
{
	public:
        StreamChannelImpl(P4Runtime::AsyncService* service, ServerCompletionQueue* cq, uint8_t* data, CallStatus status, device_mgr_t *dm)
                : service_(service)
                , cq_(cq)
                , stream(&ctx_)
                , status_(status)
                , times_(0)
				, data_(data)
				, dm_(dm)
        {
            Proceed();
        }

		void Proceed()
        {
            if (status_ == CREATE)
            {
                status_ = PROCESS;
                service_->RequestStreamChannel(&ctx_, &stream, cq_, cq_, this);
			}
            else if(status_ == PACKETIN)
			{
				status_ = FINISH;
				reply_.packet().set_payload(data_);
				stream.Write(reply_, this);
				stream.Finish(Status::OK, this);
			}
			else if (status_ == PROCESS)
            {
                // Now that we go through this stage multiple times,
                // we don't want to create a new instance every time.
                // Refer to gRPC's original example if you don't understand
                // why we create a new instance of CallData here.
                // if (times_ == 0)
                // {
                //     new CallData(service_, cq_);
                // }
                status_ = FINISH;
                // std::cout<< request_.name() <<"  "<< request_.num_greetings() <<" write finished!!" <<std::endl;
                // std::string prefix("Hello ");
                //reply_.set_message(prefix + request_.name() + ", no " + std::to_string(times_) );
                stream.Read(&stream, this);
				string payload[] = request_.packet().payload();
				struct p4_ctrl_msg ctrl_m;
				ctrl_m.type = P4T_PACKET_OUT;
				ctrl_m.packet = payload;
				dm->cb(&ctrl_m);
            }
            else
            {
                GPR_ASSERT(status_ == FINISH);
                delete this;
            }
        }

	private:
		char* data_;
		P4Runtime::AsyncService* service_;
        ServerCompletionQueue* cq_;
        ServerContext ctx_;

        p4v1 ::StreamMessageRequest request_;
        p4v1 ::StreamMessageResponse reply_;

        ServerAsyncReaderWriter<reply_, request_> stream;

        int times_;

        CallStatus status_; // The current serving state.	

		device_mgr_t *dm_;		

}
void async_packetin_data(uint8_t* data) {
  	// std::unique_ptr<ServerCompletionQueue> cq_;
  	P4Runtime::AsyncService service_;
  	//std::unique_ptr<Server> server_;
	CallStatus status = PACKETIN;
	new StreamChannelImpl(&service_, cq_.get(), data, status);
}

void dev_mgr_init(device_mgr_t *dm) {
	init_map(&(dm->id_map));
	dm->has_p4info = 0;
}

void dev_mgr_init_with_t4p4s(device_mgr_t *dm, p4_msg_callback cb, uint64_t device_id) {
	dev_mgr_init(dm);
	dm->cb = cb;
	dm->device_id = device_id;
}
