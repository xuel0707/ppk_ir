#include "device_mgr.h"
#include "cJSON.h"
#include "google/rpc/code.pb.h"
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string>
#include <unordered_map>
#define SIMPLELOG std::cout

extern switch_info_t switch_info;
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
                std::cout << "The config size is " << size << std::endl;
		return grpc::Status::OK;
	}

	grpc::Status read_config(::p4::v1::ForwardingPipelineConfig* config_proto) {
		if (!fp || size == 0) return grpc::Status::OK;  // no config was saved
		if (std::fseek(fp, 0, SEEK_SET) != 0) {  // seek to start
			return grpc::Status::OK; // TODO
		}
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

ports_info_t port_array_info;

void ports_info_get() {
    int port_nums = 5;
    char *tmp;
    int i = 0;
    for (i = 0; i < port_nums; ++i) {
        port_array_info.port_info[i].port_ = i;
        port_array_info.port_info[i].iface_name = "eth" + std::to_string(i);
        port_array_info.port_info[i].status = 1;
        port_array_info.port_info[i].extra_info = "The port is UP";
        port_array_info.port_num++;
    }
}

void append_device_and_port_info(::p4::v1::ForwardingPipelineConfig* config_proto, switch_info_t *info, ports_info_t *p_info) {
    
    if (info == NULL) {
        return;
    }
   
    auto device_config = config_proto->p4_device_config();
    size_t i = 0;
    ports_info_get();
    if (device_config.size() > 0) {
        cJSON *root = cJSON_CreateObject();
        cJSON *switch_info = cJSON_CreateObject();
        cJSON *ports_info = cJSON_CreateArray();

        cJSON_AddNumberToObject(switch_info, "device_id", info->device_id);
        cJSON_AddStringToObject(switch_info, "grpc-server", info->grpc_server.c_str());
        cJSON_AddNumberToObject(switch_info, "grpc-port", info->grpc_port);

        cJSON_AddStringToObject(root, "p4_device_config", device_config.data());
        cJSON_AddItemToObject(root, "switch_info", switch_info);

        for (i = 0; i < p_info->port_num; ++i) {
            cJSON *port = cJSON_CreateObject();
            cJSON_AddNumberToObject(port, "port_num", p_info->port_info[i].port_);
            cJSON_AddStringToObject(port, "iface_name", p_info->port_info[i].iface_name.c_str());
            cJSON_AddNumberToObject(port, "status", p_info->port_info[i].status);
            cJSON_AddStringToObject(port, "extra_info", p_info->port_info[i].extra_info.c_str());

            cJSON_AddItemToArray(ports_info, port);
        }
        cJSON_AddItemToObject(root, "ports_info", ports_info);

        if (root != NULL){
            char *strJson = cJSON_Print(root);
            config_proto->set_p4_device_config(strJson, strlen(strJson));
        }
    }
}

struct p4_field_match_header* _gen_match_rule_exact(argument_t* arg, const ::p4::v1::FieldMatch::Exact& exact) {
	struct p4_field_match_exact* result = (struct p4_field_match_exact*)malloc(sizeof(struct p4_field_match_exact));

	strcpy(result->header.name, arg->name);
	result->header.type = P4_FMT_EXACT;
	result->length = arg->bitwidth;
	//printf("value_exact:: %x,%x,%x,%x\n",*(exact.value().c_str()),*(exact.value().c_str()+1),*(exact.value().c_str()+2),*(exact.value().c_str()+3));
    memcpy(result->bitmap, exact.value().c_str(), exact.value().size());
	//printf("value:: %x,%x,%x,%x\n",*(result->bitmap),*(result->bitmap+1),*(result->bitmap+2),*(result->bitmap+3));

	return (struct p4_field_match_header*)result; /* TODO: NTOH !!! */
}

struct p4_field_match_header* _gen_match_rule_lpm(argument_t* arg, const ::p4::v1::FieldMatch::LPM& lpm) {
	struct p4_field_match_lpm* result = (struct p4_field_match_lpm*)malloc(sizeof(struct p4_field_match_lpm));

	strcpy(result->header.name, arg->name);
	result->header.type = P4_FMT_LPM;
    result->prefix_length = lpm.prefix_len();
    memcpy(result->bitmap, lpm.value().c_str(), lpm.value().size());
	
	return (struct p4_field_match_header*)result; /* TODO: NTOH !!! */
}

struct p4_field_match_header* _gen_match_rule_ternary(argument_t* arg, const ::p4::v1::FieldMatch::Ternary& ternary,uint16_t priority) {
	struct p4_field_match_ternary* result = (struct p4_field_match_ternary*)malloc(sizeof(struct p4_field_match_ternary));

	strcpy(result->header.name, arg->name);
	result->header.type = P4_FMT_TERNARY;
	result->length = arg->bitwidth;
    result->priority = priority;
    memcpy(result->bitmap, ternary.value().c_str(), ternary.value().size());
	memcpy(result->mask, ternary.mask().c_str(), ternary.mask().size());
    // printf("value:: %x,%x,%x,%x\n",*(result->bitmap),*(result->bitmap+1),*(result->bitmap+2),*(result->bitmap+3));
	

	return (struct p4_field_match_header*)result; /* TODO: NTOH !!! */
}

struct p4_action_parameter* _gen_action_param(argument_t* arg, const ::p4::v1::Action::Param& param) {
	struct p4_action_parameter* result = (struct p4_action_parameter*)malloc(sizeof(struct p4_action_parameter));
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
int get_table_entry_by_p4_entry(device_mgr_t *dm, const ::p4::v1::TableEntry *table_entry);
table_entry_t *update_table_entry_field(device_mgr_t *dm, int index);
grpc::Status table_insert(device_mgr_t *dm, const ::p4::v1::TableEntry &table_entry) {
	uint32_t table_id;
	uint32_t field_id;
	uint32_t action_id;
    uint32_t param_id;
	uint16_t value16; /* TODO: remove after testing */
    uint8_t ip[4]; /* TODO: remove after testing */
	size_t i;
    int index;
    char value[255];
	uint8_t match_type;
	int32_t prefix_len = 0; /* in bits */
    uint16_t priority;
	grpc::Status status;
	
    table_entry_t *table_entry_new;
	table_id = table_entry.table_id();
    // std::cout << "Start get_table_entry_by_p4_entry      " << std::endl;
    index = get_table_entry_by_p4_entry(dm, &table_entry);
    // std::cout << "The get_table_entry_by_p4_entry index is " << index << std::endl;
    if (index == -1) {
        table_entry_new = add_table_entry(&(dm->id_table_entries),table_id);
    }else{
        table_entry_new = update_table_entry_field(dm, index);
    }
	element_t *elem = get_element(&(dm->id_map), table_id);
	if(!strstr(elem->value, "_0")){
		elem->value = strcat(elem->value,"_0");
	}
	argument_t *arg = NULL;

	struct p4_ctrl_msg ctrl_m;
    int fields_match = elem->n_args;
	ctrl_m.num_field_matches = fields_match;
	ctrl_m.num_action_params = 0;
	ctrl_m.type = P4T_ADD_TABLE_ENTRY;
	ctrl_m.table_name = strdup(elem->value);
    match_t* match_new = NULL;
    table_entry_new->count = 0;
    for (i=0 ;i<fields_match ;i++){
        ctrl_m.field_matches[i] = NULL;
    }
    priority = table_entry.priority();
	for (const auto &match: table_entry.match()) {
        match_new = (match_t*)malloc(sizeof(match_t));
        // match_new = new match_t;
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
			match_new->prefix_len = lpm.prefix_len();
			char* new_value;
			new_value = (char*)malloc(lpm.value().size());
			memcpy(new_value, lpm.value().c_str(), lpm.value().size());
			match_new->value = new_value;
            prefix_len = lpm.prefix_len();
            if (lpm.value().size()>=4) {
                ip[0] = lpm.value().c_str()[0];
                ip[1] = lpm.value().c_str()[1];
                ip[2] = lpm.value().c_str()[2];
                ip[3] = lpm.value().c_str()[3];
            }
            ctrl_m.field_matches[field_id-1] = _gen_match_rule_lpm(arg, lpm);
            table_entry_new->matches[table_entry_new->count++] = match_new;
            status = grpc::Status::OK;
		}
		else if(match_type == 2){
			
            const auto exact = match.exact();
			char* new_value;
			new_value = (char*)malloc(exact.value().size());
			memcpy(new_value, exact.value().c_str(), exact.value().size());
            match_new->value = new_value;
            match_new->value_size = exact.value().size();
                        
            ctrl_m.field_matches[field_id-1] = _gen_match_rule_exact(arg, exact);
            table_entry_new->matches[table_entry_new->count++] = match_new;
            match_new = NULL;
            status = grpc::Status::OK;
		}
		else if(match_type == 3){
			const auto ternary = match.ternary();
            char* new_mask;
            new_mask = (char*)malloc(ternary.mask().size());
            memcpy(new_mask, ternary.mask().c_str(), ternary.mask().size());
			match_new->mask = new_mask;
            match_new->mask_size = ternary.mask().size();
			char* new_value;
			new_value = (char*)malloc(ternary.value().size());
			memcpy(new_value, ternary.value().c_str(), ternary.value().size());
			match_new->value = new_value;
            match_new->value_size = ternary.value().size();
            ctrl_m.field_matches[field_id-1] = _gen_match_rule_ternary(arg, ternary,priority);
            table_entry_new->matches[table_entry_new->count++] = match_new;
            status = grpc::Status::OK;
		}
	}
    for(i=0 ;i<fields_match ;i++){
        if(ctrl_m.field_matches[i] == NULL){
            arg = get_argument(elem, i+1);
            match_type =arg->matchtype;
            if (match_type == 3){
                ::p4::v1::FieldMatch::LPM lpm;
                lpm.set_prefix_len(0);
                for(i=0; i<arg->bitwidth; i++){
                    value[i] = 0;
                }
                lpm.set_value(value, arg->bitwidth);
                ctrl_m.field_matches[i] = _gen_match_rule_lpm(arg, lpm);
                status = grpc::Status::OK;
		    }
            else if(match_type == 2){
                ::p4::v1::FieldMatch::Exact exact;
                for(i=0; i<arg->bitwidth; i++){
                    value[i] = 0;
                }
                exact.set_value(value, arg->bitwidth);
                ctrl_m.field_matches[i] = _gen_match_rule_exact(arg, exact);
                status = grpc::Status::OK;
            }
            else if(match_type == 4){
                ::p4::v1::FieldMatch::Ternary ternary;
                uint64_t value =0;
                ternary.set_mask(&value, arg->bitwidth);
                ternary.set_value(&value, arg->bitwidth);
                ctrl_m.field_matches[i] = _gen_match_rule_ternary(arg, ternary,priority);
                status = grpc::Status::OK;
            }
        }
    }

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
                param_id = param.param_id();
				arg = get_argument(elem, param.param_id());
                param_t* param_new = (param_t*)malloc(sizeof(param_t));
                param_new->param_id = param.param_id();
				char* new_value;
				new_value = (char*)malloc(param.value().size());
				memcpy(new_value, param.value().c_str(), param.value().size());
                param_new->value = new_value;
                param_new->value_size = param.value().size();
                table_entry_new->action.param[table_entry_new->action.count++] = param_new;
                                
				ctrl_m.action_params[param_id-1] = _gen_action_param(arg, param);
				ctrl_m.num_action_params++;
			}
			break;
		default:
			status = grpc::Status( grpc::StatusCode::UNIMPLEMENTED, "ActionType is not implemented" );
			break;
	}
    table_entry_new->priority = priority;
	if (status.ok()) {
		dm->cb(&ctrl_m);

	}

        return status;
}

match_t* get_match_from_entry_by_field_id(table_entry_t* table_entry_new, uint32_t field_id) {
    if (table_entry_new == NULL || field_id < 0) {
        std::cerr << "Input param is invalid!!!!!!!!!!!!!!" << std::endl;
        return NULL;
    }
    for (int i = 0; i < table_entry_new->count; ++i) {
        match_t* match_new = table_entry_new->matches[i];
        if (match_new != NULL && match_new->field_id == field_id) {
            return match_new;
        }
    }
    return NULL;
}

bool match_cmp(const ::p4::v1::FieldMatch* match, const match_t* match_new) {
    bool state = false;
    if (match == NULL || match_new == NULL) {
        std::cerr << "[match_cmp] Input param is NULL!!!" << std::endl;
    }
    if (match->field_match_type_case() != match_new->match_type) return false;
    
    if(match->field_match_type_case() == 2){
        const auto exact = match->exact();
        state = (memcmp(exact.value().c_str(), match_new->value, match_new->value_size) == 0);
    }
    else if (match->field_match_type_case() == 3){
        const auto ternary = match->ternary();
        state = (memcmp(ternary.mask().c_str(), match_new->mask, match_new->mask_size) == 0)&& (memcmp(ternary.value().c_str(), match_new->value, match_new->value_size) == 0);
    }
    else if (match->field_match_type_case() == 4){
        const auto lpm = match->lpm();
        state = (lpm.prefix_len() == (match_new->prefix_len)) && (memcmp(lpm.value().c_str(), match_new->value, match_new->value_size) == 0);
        std::cout << "lpm.prefix_len=" << lpm.prefix_len() << " match_new->prefix_len: " << match_new->prefix_len << " lpm.value().c_str() = " << lpm.value().c_str() << " match_new->value=" << match_new->value << std::endl;
    }
    return state;
}   

grpc::Status table_delete_entry(device_mgr_t *dm, const ::p4::v1::TableEntry &table_entry);
grpc::Status table_modify_entry(device_mgr_t *dm, const ::p4::v1::TableEntry &table_entry) {
    grpc::Status status;
    uint32_t field_id;
    uint32_t table_id;
    uint64_t entry_count;
    bool cmp_rte;
    match_t *match_new = NULL;
    table_id = table_entry.table_id();
    entry_count = dm->id_table_entries.count;
    if (entry_count <= 0) {
        status = grpc::Status::OK;
        return status;
    }
    for (int i = 0; i < entry_count; ++i) {
        cmp_rte = false;
        table_entry_t *table_entry_new = get_table_entry(&(dm->id_table_entries), table_id, i);
        if (table_entry_new != NULL) {
            for (const auto &match: table_entry.match()) {
                field_id = match.field_id();
                match_new = get_match_from_entry_by_field_id(table_entry_new, field_id);
                cmp_rte = match_cmp(&match, match_new);
                if (!cmp_rte) break;
            }
            if (cmp_rte) {
                status = table_delete_entry(dm, table_entry);
                status = table_insert(dm, table_entry);
                break;
            }
        }
   }
   return status;
}

grpc::Status table_set_default(device_mgr_t *dm, const ::p4::v1::TableEntry &table_entry) {
	grpc::Status status;
        uint32_t table_id;
        uint32_t action_id;
        argument_t* arg = NULL;
        struct p4_ctrl_msg ctrl_m;
        
	table_id = table_entry.table_id();
        element_t* elem = get_element(&(dm->id_map), table_id);
        if(!strstr(elem->value, "_0")){
            elem->value = strcat(elem->value, "_0");
         }
        ctrl_m.table_name = strdup(elem->value);
        ctrl_m.num_field_matches = 0;
        ctrl_m.num_action_params = 0;
         if (table_entry.is_default_action())
         {
             ctrl_m.type = P4T_SET_DEFAULT_ACTION;
         }

        const auto action = table_entry.action();
        switch(action.type_case())
        {
           case ::p4::v1::TableAction::TypeCase::kAction:
                ctrl_m.action_type = P4_AT_ACTION;
                const auto tmp_act = action.action();
               action_id = tmp_act.action_id();
               elem = get_element(&(dm->id_map), action_id);
               ctrl_m.action_name = strdup(elem->value);
               for (const auto &param: tmp_act.params())
               {
                  arg = get_argument(elem, param.param_id());
                  ctrl_m.action_params[ctrl_m.num_action_params] = _gen_action_param(arg, param);
                  ctrl_m.num_action_params++;
              }
              status = grpc::Status::OK;
              break;
          default:
                  status = grpc::Status( grpc::StatusCode::UNIMPLEMENTED, "ActionType is not implemented" );
	          break;
          }
          if (status.ok())
          {
              std::cout << ctrl_m.table_name << " " << ctrl_m.action_name << std::endl;
              dm->cb(&ctrl_m);
          }

        return status;
}

grpc::Status table_modify(device_mgr_t *dm, const ::p4::v1::TableEntry &table_entry) {
    grpc::Status status;
    if (table_entry.is_default_action()) {
        status = table_set_default(dm, table_entry);
    }
    else{
        status = table_modify_entry(dm, table_entry);
    }
    return status;
}

void free_match_t(device_mgr_t *dm, uint32_t table_id, uint32_t field_id)
{
    uint64_t table_values_bytes = 0;
	if (dm->id_table_entries.count > 0) {
		table_values_bytes =  dm->id_table_entries.count;
	}
	for(int index = 0; index < table_values_bytes ; index++){
		table_entry_t *table_entry_new = get_table_entry(&(dm->id_table_entries),table_id,index);
		if (table_entry_new != NULL)
		{
		    uint64_t match_count = table_entry_new->count;
		    for(int i = 0; i < match_count; ++i)
		    {
			    if ((table_entry_new->matches[i]->value != NULL) && (table_entry_new->matches[i]->field_id == field_id))
			    {
				    free(table_entry_new->matches[i]->value);
                    table_entry_new->matches[i]->value = NULL;
				    table_entry_new->matches[i]->value_size = 0;
				    delete table_entry_new->matches[i];
                    table_entry_new->matches[i] = NULL;
				    table_entry_new->count--;
			    }
		    }
		    if (table_entry_new->count == 0)
		    {
		        uint64_t param_count = table_entry_new->action.count;
		        for (int i = 0; i < param_count; ++i)
			    {
                    free(table_entry_new->action.param[i]->value);
                    table_entry_new->action.param[i]->value = NULL;
                    table_entry_new->action.param[i]->value_size = 0;
                    delete table_entry_new->action.param[i];
                    table_entry_new->action.param[i] = NULL;
                    table_entry_new->action.count--;
		        }
                        
            free(table_entry_new);
            table_entry_new = NULL;
            dm->id_table_entries.count--;
            }
		}
	}
}
void free_match_(match_t* match) {
    if (match == NULL) {
        std::cerr << "[free_match_]Input param is NULL" << std::endl;
    }
    free(match->value);
    free(match);
    match->value = NULL;
    match = NULL;
}


void free_action_(action_t* action)
{
    if (action == NULL) {
        std::cerr << "[free_action_]Input param is NULL" << std::endl;
    }
    uint64_t param_count = action->count;
    for (int i = 0; i < param_count; ++i) {
        param_t* param_new = action->param[i];
        free(param_new->value);
        param_new->value_size = 0;
        action->count--;
    }
    
}

void free_table_entry_(device_mgr_t *dm, int index) {
    if (dm == NULL || index < 0) {
        std::cerr << "[free_table_entry_]Input param is NULL" << std::endl;
        return;
    }
    table_entry_t* table_entry = dm->id_table_entries.table_entry[index];
    uint64_t match_count = table_entry->count;
    for (int i = 0; i < match_count; ++i) {
        match_t* match_new = table_entry->matches[i];
        free_match_(match_new);
        table_entry->count--;
    }
    action_t action_new = table_entry->action;
    free_action_(&action_new);
    free(table_entry);
    uint64_t table_entry_count = dm->id_table_entries.count;
    
    for (int i = index; i < table_entry_count; ++i) {
        dm->id_table_entries.table_entry[i] = dm->id_table_entries.table_entry[i + 1];
    }
    dm->id_table_entries.table_entry[dm->id_table_entries.count - 1] = NULL;
    (dm->id_table_entries.count)--;
    table_entry = NULL;
}

table_entry_t *update_table_entry_field(device_mgr_t *dm, int index) {
    if (dm == NULL || index < 0) {
        std::cerr << "[free_table_entry_]Input param is NULL" << std::endl;
        return;
    }
    table_entry_t* table_entry = dm->id_table_entries.table_entry[index];
    uint64_t match_count = table_entry->count;
    for (int i = 0; i < match_count; ++i) {
        match_t* match_new = table_entry->matches[i];
        free_match_(match_new);
        table_entry->count--;
    }
    action_t action_new = table_entry->action;
    free_action_(&action_new);
    return table_entry;
}

int get_table_entry_by_p4_entry(device_mgr_t *dm, const ::p4::v1::TableEntry *table_entry) {
    uint32_t table_entry_count = 0;
    uint32_t table_id;
    uint32_t field_id;
    uint8_t match_type;
    bool match_cmp_rt;
    if (table_entry == NULL) {
        std::cerr << "[get_table_entry_by_p4_entry] Input param is NULL" << std::endl;
        return -1;
    }
    table_id = table_entry->table_id();
    if (dm->id_table_entries.count > 0) table_entry_count = dm->id_table_entries.count;
    std::cout << "dm->id_table_entries.count = " << dm->id_table_entries.count << std::endl;
    for (int i = 0; i < table_entry_count; ++i) {
        table_entry_t* table_entry_new = get_table_entry(&(dm->id_table_entries), table_id, i);
        if (table_entry_new != NULL) {

            if (table_entry_new->count != table_entry->match_size()) continue;
            for (const auto& match: table_entry->match()) {
                field_id = match.field_id();
                match_t* match_new = get_match_from_entry_by_field_id(table_entry_new, field_id);
                if (match_new == NULL) {
                    std::cerr << "match new is null!!!!" << std::endl;
                }   
                match_cmp_rt = match_cmp(&match, match_new);
                if (!match_cmp_rt) {
                    std::cerr << "match_cmp_rt is false!!!" << std::endl;
                    break;
                }
            }
            if (match_cmp_rt) {

                return i;
            }
       }

    }
    return -1;
}


grpc::Status table_clear(device_mgr_t *dm, const ::p4::v1::TableEntry &table_entry) {
    uint32_t table_id;
    uint32_t field_id;
    uint8_t match_type;
    uint32_t table_entry_count = 0;
    struct p4_ctrl_msg ctrl_m;
    uint16_t priority;
    grpc::Status status = grpc::Status::OK;
    argument_t* arg = NULL;
    element_t* elem = NULL;
    table_id = table_entry.table_id();
    elem = get_element(&(dm->id_map), table_id);
    if(!strstr(elem->value, "_0")){
        elem->value = strcat(elem->value,"_0");
    }

    ctrl_m.table_name = strdup(elem->value);
    priority = table_entry.priority();
    if (dm->id_table_entries.count > 0) {
        table_entry_count = dm->id_table_entries.count;
    }
    for (int index = 0; index < table_entry_count; ++index) {
        int fields_match = elem->n_args;
        ctrl_m.num_field_matches = fields_match;
        ctrl_m.num_action_params = 0;
        ctrl_m.type = P4T_REMOVE_TABLE_ENTRY;
        
        table_entry_t* table_entry_new = get_table_entry(&(dm->id_table_entries), table_id, index);
        if (table_entry_new != NULL){
            for(int i = 0; i < table_entry_new->count; ++i)
            {
                field_id = table_entry_new->matches[i]->field_id;
                match_type = table_entry_new->matches[i]->match_type;
                auto match = table_entry.add_match();
                match->set_field_id(field_id);
                arg = get_argument(elem, field_id);
                if (arg==NULL) {
                    std::cout << "NULL ARGUMENT for FIELD_ID= " << field_id << std::endl;
                }
                if (match_type == 2)
                {
                    auto exact = match->mutable_exact();
                    exact->set_value(table_entry_new->matches[i]->value, table_entry_new->matches[i]->value_size);
                    ctrl_m.field_matches[field_id - 1] = _gen_match_rule_exact(arg, *exact);
                }
                else if(match_type == 3)
                {
                    auto ternary = match->mutable_ternary();
                    ternary->set_mask(table_entry_new->matches[i]->mask, table_entry_new->matches[i]->mask_size);
                    ternary->set_value(table_entry_new->matches[i]->value, table_entry_new->matches[i]->value_size);
                    ctrl_m.field_matches[field_id - 1] = _gen_match_rule_ternary(arg, *ternary,priority);
                }
                else if(match_type == 4)
                {
                    auto lpm = match->mutable_lpm();
                    lpm->set_value(table_entry_new->matches[i]->value, table_entry_new->matches[i]->value_size);
                    lpm->set_prefix_len(table_entry_new->matches[i]->prefix_len);
                    ctrl_m.field_matches[field_id - 1] = _gen_match_rule_lpm(arg, *lpm);
                }
                free_match_(table_entry_new->matches[i]);
                table_entry.clear_match();
                status = grpc::Status::OK;
            }
            action_t action_new = table_entry_new->action;
            auto action = table_entry.mutable_action()->mutable_action();
            action->set_action_id(action_new.action_id);
            ctrl_m.action_type = P4_AT_ACTION;
            elem = get_element(&(dm->id_map), action_new.action_id);
            ctrl_m.action_name = strdup(elem->value);
            for (int i = 0; i < action_new.count; i++)
            {
                auto param = action->add_params();
                arg = get_argument(elem, action_new.param[i]->param_id);
                param->set_param_id(action_new.param[i]->param_id);
                param->set_value(action_new.param[i]->value, action_new.param[i]->value_size);
                ctrl_m.action_params[ctrl_m.num_action_params] = _gen_action_param(arg, *param);
                ctrl_m.num_action_params++;
                status = grpc::Status::OK;
            }
            free_action_(&action_new);
            table_entry.clear_action();
            if (status.ok())
            {
                dm->cb(&ctrl_m);
            }
        }
    }
    destroy_table_entries(&(dm->id_table_entries));
    return status;
}
grpc::Status table_delete_entry(device_mgr_t *dm, const ::p4::v1::TableEntry &table_entry) {
	uint32_t table_id;
	uint32_t field_id;
	uint32_t action_id;
    uint16_t priority;
	uint16_t value16; /* TODO: remove after testing */
	uint8_t ip[4]; /* TODO: remove after testing */
	size_t i;
	uint8_t match_type;
	int32_t prefix_len = 0; /* in bits */

	grpc::Status status = grpc::Status::OK;

	
	table_id = table_entry.table_id();

	argument_t *arg = NULL;
	element_t *elem = get_element(&(dm->id_map), table_id);
	if(!strstr(elem->value, "_0")){
		elem->value = strcat(elem->value,"_0");
	}

	struct p4_ctrl_msg ctrl_m;
    int fields_match = elem->n_args;
    ctrl_m.num_field_matches = fields_match;
	ctrl_m.num_action_params = 0;
	ctrl_m.type = P4T_REMOVE_TABLE_ENTRY;
	ctrl_m.table_name = strdup(elem->value);
    priority = table_entry.priority();
	for (const auto &match: table_entry.match()) {
		field_id = match.field_id();
		arg = get_argument(elem, field_id);
		if (arg==NULL) {
			printf("NULL ARGUMENT for FIELD_ID=%d\n", field_id);
		}
		match_type = match.field_match_type_case();
		if(match_type == 2){
			const auto exact = match.exact();
            ctrl_m.field_matches[field_id - 1] = _gen_match_rule_exact(arg, exact);
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
                         ctrl_m.field_matches[field_id - 1] = _gen_match_rule_lpm(arg, lpm);
                         //ctrl_m.num_field_matches++;
                         status = grpc::Status::OK;
		}
		else if(match_type == 3){
			const auto ternary = match.ternary();
            ctrl_m.field_matches[field_id - 1] = _gen_match_rule_ternary(arg, ternary,priority);
		}
	}
	if (table_entry.is_default_action()) { /* n_match is 0 in this case */
		ctrl_m.type = P4T_SET_DEFAULT_ACTION;
	}

	if (status.ok()) {	
		dm->cb(&ctrl_m);
        int index = 0;
        index = get_table_entry_by_p4_entry(dm, &table_entry);
        free_table_entry_(dm, index);
        std::cout << "After Table Entry count = " << dm->id_table_entries.count << std::endl;
//	    }
	}

        return status;
}

grpc::Status table_delete(device_mgr_t *dm, const ::p4::v1::TableEntry &table_entry) {
    grpc::Status status = grpc::Status::OK;
    if(table_entry.match_size() > 0){
         status = table_delete_entry(dm, table_entry);
    }
    else if(table_entry.match_size() == 0)
    {
        status = table_clear(dm, table_entry);
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

class P4ErrorReporter {
 public:
  void push_back(const ::p4::v1::Error &error) {
  }
  void push_back(const grpc::Status &status) {
  }

  grpc::Status get_status() const {
    grpc::Status status;
    if (errors.empty()) {
      status = grpc::Status::OK;
    } else {
      status = grpc::Status(grpc::StatusCode::UNKNOWN, "Unknown");
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
	
	switch(update) {
		case ::p4::v1::Update::MODIFY:
		case ::p4::v1::Update::DELETE:
		case ::p4::v1::Update::INSERT:
			grpc::Status status;
	
			int size_bytes   = -1;
			std::cout << elem->value <<std::endl;
			uint32_t* register_values_bytes = dm->write_register_by_name(elem->value, &size_bytes, index, value);
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

grpc::Status counter_modify_one(device_mgr_t *dm, char *counter_name, int64_t index, uint32_t packets, uint32_t bytes) {
    grpc::Status status;
    struct p4_ctrl_counter ctrl_c;
    std::cout << "counter_modify_one" << std::endl;
    ctrl_c.counter_name = counter_name;
    ctrl_c.index = index;
    ctrl_c.type = P4T_MODIFY_COUNTER_ENTRY;
    ctrl_c.packets = packets;
    ctrl_c.bytes = bytes;
    ctrl_c.is_direct = 0;
    dm->counter_cb(&ctrl_c);
    status = grpc::Status(grpc::StatusCode::OK, "OK");
    return status;
}

grpc::Status counter_modify(device_mgr_t *dm, const ::p4::v1::CounterEntry &counter_entry){

    grpc::Status status;
    uint32_t counter_id = 0;
    int64_t index = 0;
    uint64_t array_size = 0;
    uint32_t packets = 0;
    uint32_t bytes = 0;

    struct p4_ctrl_counter ctrl_c;
    if (!check_p4_id(counter_entry.counter_id(), P4IDS_REGISTER) && counter_entry.counter_id()!=0) {
                status = grpc::Status( grpc::StatusCode::UNIMPLEMENTED, "P4ID is not unknown" );
                return status; /*TODO: more informative error msg is needed!!!*/
    }
    counter_id = counter_entry.counter_id();
    if (counter_entry.has_data()) {
        packets = counter_entry.data().packet_count();
        bytes = counter_entry.data().byte_count();
    } else {
        packets = 0;
        bytes = 0;
    }
    
    
    element_t *elem = get_element(&(dm->id_map), counter_id);
    if (!strstr(elem->value, "_0")){
        elem->value = strcat(elem->value, "_0");
    }

 
    if (counter_entry.index().index() < 0){
        status = grpc::Status(grpc::StatusCode::UNIMPLEMENTED, "A negative number is not a valid index value.!!!");
        return status;
    }

    if (counter_entry.has_index() && counter_entry.index().index() != 0) {
        index = counter_entry.index().index();
        if (index < 0){
            status = grpc::Status(grpc::StatusCode::UNIMPLEMENTED, "A negative number is not a valid index value.");
            return status;
        }
        status = counter_modify_one(dm, elem->value, index, packets, bytes);
    } else {
        array_size = elem->n_args;
        for (index = 1; index <= array_size; index++) {
            status = counter_modify_one(dm, elem->value, index, packets, bytes);
        }
    }
    return status;

}

grpc::Status counter_write(device_mgr_t *dm, ::p4::v1::Update::Type update, const ::p4::v1::CounterEntry &counter_entry){
    grpc::Status status;
    if (!check_p4_id(counter_entry.counter_id(), P4IDS_TABLE)) {
                status = grpc::Status( grpc::StatusCode::UNKNOWN, "P4ID is not unknown" );
                return status; /*TODO: more informative error msg is needed!!!*/
        }

    if (counter_entry.index().index() < 0) {
        status = grpc::Status( grpc::StatusCode::UNKNOWN, "counter index negative" );
        return status;
    }

    switch(update) {
        case ::p4::v1::Update::INSERT || ::p4::v1::Update::DELETE || ::p4::v1::Update::UNSPECIFIED:
            status = grpc::Status( grpc::StatusCode::INVALID_ARGUMENT, "Invalid argument" );
            break;
        case ::p4::v1::Update::MODIFY:
            status = counter_modify(dm, counter_entry);
            break;
    }
    return status;
}

grpc::Status direct_counter_modify(device_mgr_t *dm, const ::p4::v1::DirectCounterEntry &direct_counter_entry) {
    grpc::Status status;
    uint32_t table_id = 0;
    struct p4_ctrl_counter ctrl_c;

    auto table_entry = direct_counter_entry.table_entry();
    if (!check_p4_id(table_entry.table_id(), P4IDS_TABLE)) {
		status = grpc::Status( grpc::StatusCode::UNKNOWN, "P4ID is not unknown" );
		return status; /*TODO: more informative error msg is needed!!!*/
	}
    auto data = direct_counter_entry.data();
    table_id = table_entry.table_id();
    element_t *elem = get_element(&(dm->id_map), table_id);
    if (!strstr(elem->value, "_0")) {
        elem->value = strcat(elem->value, "_0");
    }
    ctrl_c.type = P4T_MODIFY_COUNTER_ENTRY;
    ctrl_c.is_direct = 1;
    ctrl_c.bytes = data.byte_count();
    ctrl_c.packets = data.packet_count();
    ctrl_c.table_name = elem->value;
    dm->counter_cb(&ctrl_c);
    status = grpc::Status( grpc::StatusCode::OK, "ok" );
    return status;
}
grpc::Status direct_counter_write(device_mgr_t *dm, ::p4::v1::Update::Type update, const ::p4::v1::DirectCounterEntry &direct_counter_entry){
    grpc::Status status;
    uint32_t table_id = 0;
    table_id = direct_counter_entry.table_entry().table_id();
    if (!check_p4_id(table_id, P4IDS_TABLE)) {
		status = grpc::Status( grpc::StatusCode::UNKNOWN, "P4ID is not unknown" );
		return status; /*TODO: more informative error msg is needed!!!*/
	}
    switch(update) {
        case ::p4::v1::Update::INSERT || ::p4::v1::Update::DELETE || ::p4::v1::Update::UNSPECIFIED:
            status = grpc::Status( grpc::StatusCode::INVALID_ARGUMENT, "Invalid argument" );
            break;
        case ::p4::v1::Update::MODIFY:
            status = direct_counter_modify(dm, direct_counter_entry);
            break;
    }
    return status;
}

grpc::Status multicast_group_entry_insert(device_mgr_t *dm, const ::p4::v1::MulticastGroupEntry &multicastGroupEntry){
    uint32_t value32 = 0;
	grpc::Status status;
    
	argument_t arg;

	struct p4_ctrl_msg ctrl_m;
    int fields_match = 1;
	ctrl_m.num_field_matches = fields_match;
	ctrl_m.num_action_params = 0;
	ctrl_m.type = P4T_ADD_TABLE_ENTRY;
    std::string table_name = "multicast_group_table";
    ctrl_m.table_name=strdup(table_name.c_str());
    ctrl_m.field_matches[0] = NULL;
    arg.id = 19970701;
    std::string name = "multicast_group_id";
    strcpy(arg.name, name.c_str());
    arg.bitwidth = 16;
    struct p4_field_match_exact* field_match_result = (struct p4_field_match_exact*)malloc(sizeof(struct p4_field_match_exact));
	strcpy(field_match_result->header.name, arg.name);
	field_match_result->header.type = P4_FMT_EXACT;
	field_match_result->length = arg.bitwidth;
    uint16_t value16 = (uint16_t)multicastGroupEntry.multicast_group_id();
    char* multicast_group_id = (char*)&value16;
    memcpy(field_match_result->bitmap, multicast_group_id, 2);

    ctrl_m.field_matches[0] = (struct p4_field_match_header*)field_match_result;
	ctrl_m.action_type = P4_AT_ACTION; /* ACTION PROFILE IS NOT SUPPORTED */
    std::string action_name = "set_bitmcast";
    ctrl_m.action_name=strdup(action_name.c_str());
	for (const auto replicas: multicastGroupEntry.replicas()) {
        value32 = value32 | 1 << replicas.egress_port();
	}
    char* value = (char*)&value32;
    struct p4_action_parameter* result = (struct p4_action_parameter*)malloc(sizeof(struct p4_action_parameter));
	uint32_t* tmp32;
    std::string param_name = "bit_mcast";
	strcpy(result->name, param_name.c_str());
	result->length = 32;
	memcpy(result->bitmap, value, 4);
	tmp32 = (uint32_t*)result->bitmap;
	*tmp32 = htonl(*tmp32);
    ctrl_m.action_params[0] = result;
    ctrl_m.num_action_params++;
    status = grpc::Status::OK;
	if (status.ok()) {
		dm->cb(&ctrl_m);

	}

    return status;
}

grpc::Status clone_session_entry_insert(device_mgr_t *dm, const ::p4::v1::CloneSessionEntry &clone_session_entry){
    uint16_t value16 = 0;
    char value;
	grpc::Status status;
    
	argument_t arg;

	struct p4_ctrl_msg ctrl_m;
    int fields_match = 1;
	ctrl_m.num_field_matches = fields_match;
	ctrl_m.num_action_params = 0;
	ctrl_m.type = P4T_ADD_TABLE_ENTRY;
    std::string table_name = "clone_session_table";
    ctrl_m.table_name=strdup(table_name.c_str());
    ctrl_m.field_matches[0] = NULL;
    arg.id = 19970701;
    std::string name = "clone_session_id";
    strcpy(arg.name, name.c_str());
    arg.bitwidth = 32;
    struct p4_field_match_exact* field_match_result = (struct p4_field_match_exact*)malloc(sizeof(struct p4_field_match_exact));
	strcpy(field_match_result->header.name, arg.name);
	field_match_result->header.type = P4_FMT_EXACT;
	field_match_result->length = arg.bitwidth;
    uint32_t value32 = clone_session_entry.session_id();
    char* session_id = (char*)&value32;
    memcpy(field_match_result->bitmap, session_id, 4);

    ctrl_m.field_matches[0] = (struct p4_field_match_header*)field_match_result;
	ctrl_m.action_type = P4_AT_ACTION; /* ACTION PROFILE IS NOT SUPPORTED */

	for (const auto replicas: clone_session_entry.replicas()) {
        if (replicas.egress_port() != NULL){
            std::string action_name = "set_clone_egress_port";
            ctrl_m.action_name=strdup(action_name.c_str());
            // value = (char)value32;
            value16 = replicas.egress_port();
            // std::cout << "egress_port:" << value16<< std::endl;
            char* value = (char*)&value16;
            struct p4_action_parameter* result = (struct p4_action_parameter*)malloc(sizeof(struct p4_action_parameter));
            uint16_t* tmp16;
            uint32_t* tmp32;
            std::string param_name = "egress_port";
            strcpy(result->name, param_name.c_str());
            result->length = 9;
            memcpy(result->bitmap, value, 2);
            // printf("value:: %x,%x,%x,%x\n",*(result->bitmap),*(result->bitmap+1),*(result->bitmap+2),*(result->bitmap+3));
            // tmp16 = (uint16_t*)result->bitmap;
		    // *tmp16 = htons(*tmp16);
            // printf("value:: %x,%x,%x,%x\n",*(result->bitmap),*(result->bitmap+1),*(result->bitmap+2),*(result->bitmap+3));
            ctrl_m.action_params[0] = result;
            ctrl_m.num_action_params++;
        // }else{
        //     std::string action_name = "set_clone_multicast_group_id";
        //     ctrl_m.action_name=strdup(action_name.c_str());
        //     // value = (char)value32;
        //     value16 = replicas.multicast_group_id();
        //     char* value = (char*)&value16;
        //     struct p4_action_parameter* result = (struct p4_action_parameter*)malloc(sizeof(struct p4_action_parameter));
        //     uint16_t* tmp16;
        //     uint32_t* tmp32;
        //     std::string param_name = "multicast_group_id";
        //     strcpy(result->name, param_name.c_str());
        //     result->length = 16;
        //     memcpy(result->bitmap, &value, 2);
        //     tmp16 = (uint16_t*)result->bitmap;  
        //     *tmp16 = htonl(*tmp16);
        //     ctrl_m.action_params[0] = result;
        //     ctrl_m.num_action_params++;            
        }
	}
    status = grpc::Status::OK;
	if (status.ok()) {
		dm->cb(&ctrl_m);

	}

    return status;
}

grpc::Status engine_entry_insert(device_mgr_t *dm, const ::p4::v1::PacketReplicationEngineEntry &packet_replication_engine_entry){
    grpc::Status status;
    switch(packet_replication_engine_entry.type_case()) {
        case ::p4::v1::PacketReplicationEngineEntry::kMulticastGroupEntry:
            status = multicast_group_entry_insert(dm,packet_replication_engine_entry.multicast_group_entry());
            break;
        case ::p4::v1::PacketReplicationEngineEntry::kCloneSessionEntry:
            status = clone_session_entry_insert(dm,packet_replication_engine_entry.clone_session_entry());
            break;
        
    }
    return status;
}

grpc::Status engine_entry_write(device_mgr_t *dm, ::p4::v1::Update::Type update, const ::p4::v1::PacketReplicationEngineEntry &packet_replication_engine_entry){
    grpc::Status status;
    switch(update) {
        case ::p4::v1::Update::INSERT:
            status = engine_entry_insert(dm,packet_replication_engine_entry);
            break;
        case ::p4::v1::Update::MODIFY:
            status = grpc::Status( grpc::StatusCode::INVALID_ARGUMENT, "Invalid argument" );
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
            case ::p4::v1::Entity::kCounterEntry:
                std::cout << "dev_mgr_write" << std::endl;
                status = counter_write(dm, update.type(), entity.counter_entry());
                break;
            case ::p4::v1::Entity::kDirectCounterEntry:
                status = direct_counter_write(dm, update.type(), entity.direct_counter_entry());
                break;
            case ::p4::v1::Entity::kPacketReplicationEngineEntry:
                status = engine_entry_write(dm, update.type(), entity.packet_replication_engine_entry());
                break;
            default:
				status = grpc::Status(grpc::StatusCode::UNKNOWN, "Entity case is unknown");
				break;

		}
		error_reporter.push_back(status);
	}
	
	return error_reporter.get_status();
}

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
    table_id = table_entry.table_id();
    element_t *elem = get_element(&(dm->id_map), table_id);
    if(!strstr(elem->value, "_0")){
        elem->value = strcat(elem->value,"_0");
    }

        
	uint64_t table_values_bytes = 0;
	if (dm->id_table_entries.count > 0) {
		table_values_bytes =  dm->id_table_entries.count;
	}
    if (!table_entry.is_default_action()){
        for(int index = 0; index < table_values_bytes ; index++){
            table_entry_t *table_entry_new = get_table_entry(&(dm->id_table_entries),table_id,index);
            if (table_entry_new != NULL){
                auto entry = response->add_entities()->mutable_table_entry();
                entry->set_table_id(table_id);
                
                for(int i = 0; i < table_entry_new->count; i++)
                {   
                    if(table_entry_new->matches[i] == NULL){
                        continue;
                    }
                    auto match = entry->add_match();
                    match->set_field_id(table_entry_new->matches[i]->field_id);
                    if(table_entry_new->matches[i]->match_type == 4)
                    {
                        auto lpm = match->mutable_lpm();
                        lpm->set_prefix_len(table_entry_new->matches[i]->prefix_len);
                        lpm->set_value(table_entry_new->matches[i]->value, table_entry_new->matches[i]->value_size);
                    }
                    else if(table_entry_new->matches[i]->match_type == 2)
                    {
                        auto exact = match->mutable_exact();
                        exact->set_value(table_entry_new->matches[i]->value, table_entry_new->matches[i]->value_size);

                    }
                    else if(table_entry_new->matches[i]->match_type == 3)
                    {
                        auto ternary = match->mutable_ternary();
                        ternary->set_mask(table_entry_new->matches[i]->mask, table_entry_new->matches[i]->mask_size);
                        ternary->set_value(table_entry_new->matches[i]->value, table_entry_new->matches[i]->value_size);
                    }
                }
                action_t action_new = table_entry_new->action;
                auto action = entry->mutable_action()->mutable_action();
                action->set_action_id(action_new.action_id);
                elem = get_element(&(dm->id_map), action_new.action_id);
                for (int i = 0; i < action_new.count; i++)
                {
                    auto param = action->add_params();
                    param->set_param_id(action_new.param[i]->param_id);
                    param->set_value(action_new.param[i]->value, action_new.param[i]->value_size);
                                
                }
                if (table_entry_new -> priority != 0){
                    entry->set_priority(table_entry_new -> priority);
                }
            }
                            
        }
    }
    status = grpc::Status::OK;
	return status;
}

void register_read_one(uint32_t* register_values_bytes, uint32_t size_bytes, int index, ::p4::v1::RegisterEntry *entry) {
	if (register_values_bytes != 0x0 && size_bytes > 0){
		entry->mutable_data()->set_bitstring(register_values_bytes[index]);
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
		
		if (register_entry.index().index() < 0) {
			status = grpc::Status( grpc::StatusCode::UNKNOWN, "Register index negative" );
                	return status;
		}
		auto index = static_cast<size_t>(register_entry.index().index());
		auto entry = response->add_entities()->mutable_register_entry();
		entry->CopyFrom(register_entry);
		if(register_values_bytes != 0x0 && size_bytes > 0){
			uint8_t bytes_string[32];
			snprintf(bytes_string, sizeof(bytes_string), "%d", register_values_bytes[index]);
			entry->mutable_data()->set_bitstring(bytes_string);
			std::cout << bytes_string <<std::endl;
			
		}
		status = grpc::Status::OK;
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

grpc::Status counter_read_one(device_mgr_t *dm, uint32_t counter_id, uint64_t index, ::p4::v1::CounterEntry *counter_entry) {
    grpc::Status status(grpc::StatusCode::OK, "ok");
    struct p4_ctrl_counter ctrl_c;
    element_t *elem = get_element(&(dm->id_map), counter_id);
    if (elem == NULL) {
        std::cout << "counter_read_one: elem is NULL" << std::endl;
    }
    if(!strstr(elem->value, "_0")){
        elem->value = strcat(elem->value,"_0");
    }
    ctrl_c.counter_name = elem->value;
    ctrl_c.index = index;
    ctrl_c.packets = 0;
    ctrl_c.bytes = 0;
    ctrl_c.is_direct = 0;
    ctrl_c.type = P4T_GET_COUNTER_ENTRIES;
    dm->counter_cb(&ctrl_c);
    counter_entry->set_counter_id(counter_id);
    counter_entry->mutable_index()->set_index(index);
    auto entry_data = counter_entry->mutable_data();
    entry_data->set_byte_count(ctrl_c.bytes);
    entry_data->set_packet_count(ctrl_c.packets);
    return status;
}

grpc::Status counter_read_id_unset(device_mgr_t *dm, ::p4::v1::ReadResponse *response) {
    grpc::Status status(grpc::StatusCode::OK, "ok");
    uint64_t counter_id = 0;
    uint64_t size = 0;
    uint64_t index = 0;
    for (const auto &counter: dm->p4info.counters()) {
        counter_id = counter.preamble().id();
        size = counter.size();
        for (index = 1; index <= size; ++index) {
            auto entry = response->add_entities()->mutable_counter_entry();
            status = counter_read_one(dm, counter_id, index, entry);
        }
    }
    return status;
}

grpc::Status counter_read_index_unset(device_mgr_t *dm, uint32_t counter_id, ::p4::v1::ReadResponse *response) {
    grpc::Status status(grpc::StatusCode::OK, "ok");
    uint64_t size = 0;
    uint64_t index = 0;
    element_t *elem = get_element(&(dm->id_map), counter_id);
    size = elem->n_args;
    for (index = 1; index <= size; ++index) {
        auto entry = response->add_entities()->mutable_counter_entry();
        status = counter_read_one(dm, counter_id, index, entry);
    }
    return status;
}

grpc::Status counter_read(device_mgr_t *dm, const ::p4::v1::CounterEntry &counter_entry, ::p4::v1::ReadResponse *response) {
    grpc::Status status(grpc::StatusCode::OK, "ok");
    uint32_t counter_id = 0;
    counter_id = counter_entry.counter_id();
    if (counter_id == 0) {
        status = counter_read_id_unset(dm, response);
    }else {
        if (counter_entry.has_index() && counter_entry.index().index() > 0){
            auto index = static_cast<size_t>(counter_entry.index().index());
            auto entry = response->add_entities()->mutable_counter_entry();
            status = counter_read_one(dm, counter_id, index, entry);
        }else {
            status = counter_read_index_unset(dm, counter_id, response);
        }
    }
    return status;
}

grpc::Status direct_counter_read(device_mgr_t *dm, const ::p4::v1::DirectCounterEntry &direct_counter_entry, ::p4::v1::ReadResponse *response) {
    grpc::Status status(grpc::StatusCode::OK, "ok");
    uint32_t table_id = 0;
    struct p4_ctrl_counter ctrl_c;
    const auto table_entry = direct_counter_entry.table_entry();
    table_id = table_entry.table_id();
    element_t *elem = get_element(&(dm->id_map), table_id);
    if(!strstr(elem->value, "_0")){
        elem->value = strcat(elem->value,"_0");
    }
    ctrl_c.table_name = elem->value;
    ctrl_c.type = P4T_GET_COUNTER_ENTRIES;
    ctrl_c.is_direct = 1;
    ctrl_c.packets = 0;
    ctrl_c.bytes = 0;
    dm->counter_cb(&ctrl_c);
    std::cout << "table_name: " << elem->value << " packets: " << ctrl_c.packets << " bytes: " << ctrl_c.bytes << std::endl;

    auto entry = response->add_entities()->mutable_direct_counter_entry();
    auto table_entry_new = entry->mutable_table_entry();
    table_entry_new->CopyFrom(direct_counter_entry.table_entry());
    auto entry_data = entry->mutable_data();
    entry_data->set_byte_count(ctrl_c.bytes);
    entry_data->set_packet_count(ctrl_c.packets);
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
                case ::p4::v1::Entity::kCounterEntry: {
                        status = counter_read(dm, entity.counter_entry(), response);
                        break;
                }
                case ::p4::v1::Entity::kDirectCounterEntry: {
                        status = direct_counter_read(dm, entity.direct_counter_entry(), response);
                        break;
                }
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
				printf("   +-----> MATCH FIELD; name: %s; id: %d; bitwidth: %d; match_type: %d\n", mf.name().c_str(), mf.id(), mf.bitwidth(), mf.match_type());
				strcpy(elem->args[elem->n_args].name, mf.name().c_str());
	            elem->args[elem->n_args].id = mf.id();
        	    elem->args[elem->n_args].bitwidth = mf.bitwidth();
                elem->args[elem->n_args].matchtype = mf.match_type();
                elem->n_args++;
			}

                }
                
                for (const auto &counter: dm->p4info.counters()) {
                    const auto &pre = counter.preamble();
                    const auto &spec = counter.spec();
                    printf(" [+] COUNTER id: %d; name: %s\n", pre.id(), pre.name().c_str());
                    elem = add_element(&(dm->id_map), pre.id(), pre.name().c_str());

                    if (elem == NULL) {
                         printf("   +-----> ERROR\n");
                                break;

                    }
                    elem->args[0].bitwidth = spec.unit();
                    elem->n_args = counter.size();
               }

               for(const auto &direct_counter: dm->p4info.direct_counters()) {
                   const auto &pre = direct_counter.preamble();
                   const auto &spec = direct_counter.spec();
                   printf(" [+] COUNTER id: %d; name: %s\n", pre.id(), pre.name().c_str());
                   elem = add_element(&(dm->id_map), pre.id(), pre.name().c_str());

                   if(elem == NULL) {
                       printf("   +-----> ERROR\n");
                       break;
                   }
                   elem->args[0].bitwidth = spec.unit();
                   elem->args[0].id = direct_counter.direct_table_id();
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
                        append_device_and_port_info(config, &switch_info, &port_array_info);
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

bool pi_packetin_receive(const char *pkt,
                                size_t size) {
	if (packet_cb_mgr_ptr->default_cb.cb == NULL)
	{
		return false;
	}else {
  		const cb_data_t cb_data = packet_cb_mgr_ptr->default_cb;
  		((PIPacketInCb)(cb_data.cb))(pkt, size, cb_data.cookie);//(PIPacketInCb)作用是强制类型转换
		return true;
	}
}

void async_packetin_data(uint8_t* data, int len) {
	auto status = pi_packetin_receive(data, static_cast<size_t>(len));

}



void dev_mgr_init(device_mgr_t *dev_mgr_ptr) {
	init_map(&(dev_mgr_ptr->id_map));
	init_table_entries(&(dev_mgr_ptr->id_table_entries));
	dev_mgr_ptr->has_p4info = 0;
}

void cb_mgr_init(cb_mgr_t *cb_mgr) {
  cb_mgr->device_cbs._map = NULL;
  cb_mgr->default_cb.cb = NULL;
  cb_mgr->default_cb.cookie = NULL;
}

void dev_mgr_init_with_t4p4s(device_mgr_t *dev_mgr_ptr, p4_msg_callback cb, p4_reg_read read_register_by_name, p4_reg_write write_register_by_name, p4_counter_callback ctrl_counter_handle, uint64_t device_id) {
	dev_mgr_init(dev_mgr_ptr);
	cb_mgr_init(packet_cb_mgr_ptr);
	dev_mgr_ptr->cb = cb;
	dev_mgr_ptr->read_register_by_name = read_register_by_name;
	dev_mgr_ptr->write_register_by_name = write_register_by_name;
	dev_mgr_ptr->device_id = device_id;
    dev_mgr_ptr->counter_cb = ctrl_counter_handle;
}
