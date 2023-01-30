# Copyright 2016 Eotvos Lorand University, Budapest, Hungary
# 
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
# 
#     http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIO-NS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

def match_type_order(t):
    match_types = {
        "exact":    0,
        "lpm":      1,
        "ternary":  2,
    }
    return match_types[t]

#[ #include "dpdk_lib.h"
#[ #include "actions.h"
#[ #include "tables.h"

#[ #include "PI/proto/pi_server.h"
#[ #include "p4rt/device_mgr.h"

#[ #define member_size(type, member) sizeof(((type *)0)->member)

#[ extern void table_setdefault_promote  (int tableid, uint8_t* value);
#[ extern void exact_add_promote  (int tableid, uint8_t* key, uint8_t* value, bool should_print);
#[ extern void lpm_add_promote    (int tableid, uint8_t* key, uint8_t depth, uint8_t* value, bool should_print);
#[ extern void ternary_add_promote(int tableid, uint8_t* key, uint8_t* mask, uint16_t priority, uint8_t* value, bool should_print);

#[ extern void exact_delete_promote(int tableid, uint8_t* key, bool should_print);
#[ extern void lpm_delete_promote(int tableid, uint8_t* key, uint8_t depth, bool should_print);
#[ extern void ternary_delete_promote(int tableid, uint8_t* key, bool should_print);

#[ extern void exact_lookup_promote(int tableid, uint8_t* key, bool should_print);
#[ extern void lpm_lookup_promote(int tableid, uint8_t* key, bool should_print);


#[ extern device_mgr_t *dev_mgr_ptr;

#[ extern all_metadatas_t all_metadatas; //l3要用
#[ FILE *fp3;

# TODO by Ian 
#[ extern void send_burst_from_controller(struct p4_ctrl_msg* ctrl_m);
#[ extern void send_packetout_from_controller(struct p4_ctrl_msg* ctrl_m);
#[ extern void async_packetin_data(uint8_t* data, int len);

def unique_stable(items):
    """Returns only the first occurrence of the items in a list. Equivalent to unique_everseen from Python3."""
    from collections import OrderedDict
    return list(OrderedDict.fromkeys(items))

# TODO is there a more appropriate source for this than the annotation?
def get_action_name_str(action):
    name_parts = action.action_object.annotations.annotations.get('name').expr[0].value
    if name_parts.rsplit(".")[0] == '':
        return name_parts.rsplit(".")[-1]
    else:
        return name_parts

def get_table_name_str(table):
    for action in table.actions:
        name_parts = action.action_object.annotations.annotations.get('name').expr[0].value
        if name_parts.rsplit(".")[0] == '':
            continue
        else:
            return name_parts.rsplit(".")[0] + '.' + table.name


#{ void ctrl_get_counter_entries(char *counter_name,uint32_t index, uint32_t *packets, uint32_t *bytes) {
#[ extern void extern_counter_read(counter_t*, int, uint32_t*);
for smem in unique_stable([smem for table, smem in hlir16.all_meters + hlir16.all_counters if smem.smem_type not in ["direct_counter", "direct_meter"]]):
    table = None
    #[ if(strstr("c_ingress.${smem.name}", counter_name)) {
    for c in smem.components:
        cname = c['name']
        ctype = c['type']
        #[ //printf("${smem.type._baseType.path.name}, cname: ${cname}, smem_type: ${smem.smem_type}\n");
        if (smem.smem_type == "register") or table != None:
            continue
        else:
            if "packets" in cname:
                #[ extern_counter_read(&(global_smem.$cname[index - 1]), index, packets);
            else:
                #[ extern_counter_read(&(global_smem.$cname[index - 1]), index, bytes);
    #[ }


#}}

#{ void ctrl_modify_counter_entries(char *counter_name, uint64_t index, uint32_t packets, uint32_t bytes) {
#[ extern void extern_counter_set(counter_t*, int, uint32_t);
for smem in unique_stable([smem for table, smem in hlir16.all_meters + hlir16.all_counters if smem.smem_type not in ["direct_counter", "direct_meter"]]):
    table = None
    #[ if(strstr("c_ingress.${smem.name}", counter_name)) {
    for c in smem.components:
        cname = c['name']
        ctype = c['type']
        if (smem.smem_type == "register") or table != None:
            continue
        else:
            if "packets" in cname:
                #[ extern_counter_set(&(global_smem.$cname[index - 1]), index, packets);

            else:
                #[ extern_counter_set(&(global_smem.$cname[index - 1]), index, bytes);
    #[ }

#}}

#{ void ctrl_get_direct_counter_entries(char *table_name, uint32_t *packets, uint32_t *bytes) {
for table in hlir16.tables:
    if hasattr(table, 'direct_counters'):
        table_name_str = get_table_name_str(table)
        #[ if(strcmp("${table_name_str}", table_name) == 0){
        #[ //printf("%s:%d\n", __FUNCTION__, __LINE__);
        for smem in table.direct_meters + table.direct_counters:
            for comp in smem.components:
                value = "pd->parsed_length" if comp['for'] == 'bytes' else "1"
                type = comp['type']
                name = comp['name']
                #[ extern void read_${smem.smem_type}(${smem.smem_type}_t*, const char*, const char*, const char*, int*);
                if 'packets' in name:
                    #[ read_${smem.smem_type}(&(global_smem.${name}_${table.name}), "${table.name}", "${smem.smem_type}", "$name", packets);

                else:
                    #[ read_${smem.smem_type}(&(global_smem.${name}_${table.name}), "${table.name}", "${smem.smem_type}", "$name", bytes);
        #[ }
#}}

#{ void ctrl_modify_direct_counter_entries(char *table_name, uint64_t packets, uint64_t bytes) {
for table in hlir16.tables:
    if hasattr(table, 'direct_counters'):
        table_name_str = get_table_name_str(table)
        #[ if(strcmp("${table_name_str}", table_name) == 0) {
        #[ //printf("%s:%d\n", __FUNCTION__, __LINE__);
        for smem in table.direct_meters + table.direct_counters:
            for comp in smem.components:
                type = comp['type']
                name = comp['name']
                #[ extern void set_${smem.smem_type}(${smem.smem_type}_t*, int, const char*, const char*, const char*);
                if 'packets' in name:
                    #[ set_${smem.smem_type}(&(global_smem.${name}_${table.name}), packets, "${table.name}", "${smem.smem_type}", "$name");
                else:
                    #[ set_${smem.smem_type}(&(global_smem.${name}_${table.name}), bytes, "${table.name}", "${smem.smem_type}", "$name");
        #[ }
#}}

#{ void ctrl_direct_counter_handle(struct p4_ctrl_counter* ctrl_c) {
#[ if (ctrl_c->type == P4T_MODIFY_COUNTER_ENTRY) {
#[ ctrl_modify_direct_counter_entries(ctrl_c->table_name, ctrl_c->packets, ctrl_c->bytes);
#[ } else if(ctrl_c->type == P4T_GET_COUNTER_ENTRIES){
#[ ctrl_get_direct_counter_entries(ctrl_c->table_name, &(ctrl_c->packets), &(ctrl_c->bytes));
#[ }
#}}

#{ void ctrl_counter_handle(struct p4_ctrl_counter* ctrl_c) {
#[ if (ctrl_c->type == P4T_MODIFY_COUNTER_ENTRY) {
#[ ctrl_modify_counter_entries(ctrl_c->counter_name, ctrl_c->index, ctrl_c->packets, ctrl_c->bytes);
#[ }else if(ctrl_c->type == P4T_GET_COUNTER_ENTRIES){
#[ //printf("ctrl_counter_handle\n");
#[ ctrl_get_counter_entries(ctrl_c->counter_name, ctrl_c->index, &(ctrl_c->packets), &(ctrl_c->bytes));
#[ }
#}}


#{ void ctrl_all_counter_handle(struct p4_ctrl_counter* ctrl_c) {
#[ if (ctrl_c->is_direct){
#[   //printf("ctrl_all_counter_handle1\n");
#[   ctrl_direct_counter_handle(ctrl_c);
#[ }else{
#[   //printf("ctrl_all_counter_handle2\n");
#[   ctrl_counter_handle(ctrl_c);
#[}
#}}

for table in hlir16.tables:
    #[ extern void table_${table.name}_key(packet_descriptor_t* pd, uint8_t* key); // defined in dataplane.c


if len(hlir16.tables)>0:
    max_bytes = max([t.key_length_bytes for t in hlir16.tables if hasattr(t, 'key')])
    #[ uint8_t reverse_buffer[$max_bytes];

# Variable width fields are not supported
def get_key_byte_width(k):
    # for special functions like isValid
    if k.get_attr('header') is None:
        return 0
        
    return (k.width+7)/8 if not k.header.type.type_ref.is_vw else 0


hlir16_tables_with_keys = [t for t in hlir16.tables if hasattr(t, 'key')]
keyed_table_names = ", ".join(["\"T4LIT(" + table.name + ",table)\"" for table in hlir16_tables_with_keys])


for table in hlir16_tables_with_keys:
    #[ // note: ${table.name}, ${table.match_type}, ${table.key_length_bytes}
    #{ void ${table.name}_add(
    for k in table.key.keyElements:
        if not hasattr(k,"header_name"):
            continue
        # TODO should properly handle specials (isValid etc.)
        if k.get_attr('header') is None:
            if k.header_name!='meta':
                continue

        if k.header_name=='meta':
            byte_width = 'member_size(all_metadatas_t, field_metadata_%s)' % k.field_name
            #[ uint8_t field_instance_${k.header_name}_${k.field_name}[$byte_width], 
        else:
            byte_width = get_key_byte_width(k)
            #[ uint8_t field_instance_${k.header.name}_${k.field_name}[$byte_width],
        
        # TODO have keys' and tables' match_type the same case (currently: LPM vs lpm)
        if k.match_type == "ternary":
            #[ uint8_t field_instance_${k.header.name}_${k.field_name}_mask[$byte_width],
        if k.match_type == "lpm":
            #[ uint8_t field_instance_${k.header.name}_${k.field_name}_prefix_length,
    if table.match_type == "TERNARY":
        #[ uint16_t priority,
    #}     struct ${table.name}_action action)
    #{ {
    if table.key_length_bytes==0:  # TODO: Check this part!!!
        if len(table.key.keyElements) == 0:
            #[     uint8_t key[0];
        elif table.key.keyElements[0].header_name=='meta':
            #[     uint8_t key[member_size(all_metadatas_t, field_metadata_${table.key.keyElements[0].field_name})];
    else:
         #[     uint8_t key[${table.key_length_bytes}];

    byte_idx = 0
    for k in sorted((k for k in table.key.keyElements if k.get_attr('match_type') is not None), key = lambda k: match_type_order(k.match_type)):
        # TODO should properly handle specials (isValid etc.)
        if not hasattr(k,"header_name"):
            continue
        if k.get_attr('header') is None:
            if k.header_name!='meta':
                continue
        if k.header_name=='meta':
            byte_width = 'member_size(all_metadatas_t, field_metadata_%s)' % k.field_name
            #[ memcpy(key+$byte_idx, field_instance_${k.header_name}_${k.field_name}, $byte_width);
            byte_idx += get_key_byte_width(k) # Metafield size???
        else:
            byte_width = get_key_byte_width(k)
            #[ memcpy(key+$byte_idx, field_instance_${k.header.name}_${k.field_name}, $byte_width);
	    #{     /*fp3 = fopen("/home/zhaoxing/log/l2_src.txt","a");
            #[       if(fp3){
	    #[          fprintf(fp3,"key_${k.header.name}:{%d}{%d}{%d}{%d}{%d}{%d}{%d}{%d}{%d}{%d}",key[0],key[1],key[2],key[3],key[4],key[5],key[6],key[7],key[8],key[9]);
            #[          fprintf(fp3,"\n");
            #[          fclose(fp3);
            #}     }*/
            byte_idx += byte_width 

    if table.match_type == "LPM":
        #[ uint8_t prefix_length = 0;
        for k in table.key.keyElements:
            if not hasattr(k,"header_name"):
                continue
            # TODO should properly handle specials (isValid etc.)
            if k.get_attr('header') is None:
                continue

            if k.match_type == "exact":
                #[ prefix_length += ${get_key_byte_width(k)};
            if k.match_type == "lpm":
                #[ prefix_length += field_instance_${k.header.name}_${k.field_name}_prefix_length;
        #[ int c, d;
        #[ for(c = ${byte_idx-1}, d = 0; c >= 0; c--, d++) *(reverse_buffer+d) = *(key+c);
        #[ for(c = 0; c < ${byte_idx}; c++) *(key+c) = *(reverse_buffer+c);
        #[ lpm_add_promote(TABLE_${table.name}, (uint8_t*)key, prefix_length, (uint8_t*)&action,true);

    if table.match_type == "EXACT":
        #[ exact_add_promote(TABLE_${table.name}, (uint8_t*)key, (uint8_t*)&action,true);
    
    table_byte_idex = 0
    if table.match_type == "TERNARY":
	#[    uint8_t mask[$byte_idx];
        for k in table.key.keyElements:
            # TODO should properly handle specials (isValid etc.)
            if k.get_attr('header') is None:
                continue

            if k.match_type == "exact":
                #[ for(int i=0;i<${get_key_byte_width(k)};i++) mask[($table_byte_idex+i)] = 255;
                table_byte_idex += get_key_byte_width(k)
            if k.match_type == "lpm":
                #[ for(int i=0;i<${get_key_byte_width(k)};i++){
                #[      mask[$table_byte_idex+i] = 255;
                #[      if(field_instance_${k.header.name}_${k.field_name}_prefix_length/8 < (i+1)){
                #[          int num = field_instance_${k.header.name}_${k.field_name}_prefix_length%8;
                #[          mask[$table_byte_idex+i] = mask[$table_byte_idex+i] << (8-num);
                #[ } 
                #[ } 
                table_byte_idex += get_key_byte_width(k)
            if k.match_type == "ternary":
                #[    memcpy(mask+$table_byte_idex,field_instance_${k.header.name}_${k.field_name}_mask,${get_key_byte_width(k)});
                table_byte_idex += get_key_byte_width(k)
	#[    ternary_add_promote(TABLE_${table.name},(uint8_t*)key,(uint8_t*)mask,priority,(uint8_t*)&action,true);

    #} }
#[  void multicast_group_table_add( 
#[  uint8_t field_instance_multicast_group_id[2], 
#[      struct multicast_group_table_action action)
#[  { 
#[      uint8_t key[2]; 
#[  memcpy(key+0, field_instance_multicast_group_id, 2); 
#[      /*fp3 = fopen("/home/zhaoxing/log/l2_src.txt","a"); 
#[        if(fp3){ 
#[           fprintf(fp3,"key_standard_metadata:{%d}{%d}{%d}{%d}{%d}{%d}{%d}{%d}{%d}{%d}",key[0],key[1],key[2],key[3],key[4],key[5],key[6],key[7],key[8],key[9]); 
#[           fprintf(fp3,"\n"); 
#[           fclose(fp3); 
#[      }*/ 
#[     exact_add_promote(TABLE_multicast_group_table,(uint8_t*)key,(uint8_t*)&action,true);
#[  } 
#[  void clone_session_table_add( 
#[  uint8_t field_instance_clone_session_id[4], 
#[      struct clone_session_table_action action)
#[  { 
#[      uint8_t key[4]; 
#[  memcpy(key+0, field_instance_clone_session_id, 4); 
#[      /*fp3 = fopen("/home/zhaoxing/log/l2_src.txt","a"); 
#[        if(fp3){ 
#[           fprintf(fp3,"key_standard_metadata:{%d}{%d}{%d}{%d}{%d}{%d}{%d}{%d}{%d}{%d}",key[0],key[1],key[2],key[3],key[4],key[5],key[6],key[7],key[8],key[9]); 
#[           fprintf(fp3,"\n"); 
#[           fclose(fp3); 
#[      }*/ 
#[     exact_add_promote(TABLE_clone_session_table,(uint8_t*)key,(uint8_t*)&action,true);
#[  } 

for table in hlir16_tables_with_keys:
    #[ // note: ${table.name}, ${table.match_type}, ${table.key_length_bytes}
    #{ void ${table.name}_remove(
    i = 0
    for k in table.key.keyElements:
        i = i + 1
        if not hasattr(k,"header_name"):
            continue
        # TODO should properly handle specials (isValid etc.)
        if k.get_attr('header') is None:
            if k.header_name!='meta':
                continue
        if i < len(table.key.keyElements):
            if k.header_name=='meta':
                byte_width = 'member_size(all_metadatas_t, field_metadata_%s)' % k.field_name
                #[ uint8_t field_instance_${k.header_name}_${k.field_name}[$byte_width],
            else:
                byte_width = get_key_byte_width(k)
                #[ uint8_t field_instance_${k.header.name}_${k.field_name}[$byte_width],
            
            # TODO have keys' and tables' match_type the same case (currently: LPM vs lpm)
            if k.match_type == "ternary":
                #[ uint8_t field_instance_${k.header.name}_${k.field_name}_mask[$byte_width],
            if k.match_type == "lpm":
                #[ uint8_t field_instance_${k.header.name}_${k.field_name}_prefix_length,
        else:
            if k.header_name=='meta':
                byte_width = 'member_size(all_metadatas_t, field_metadata_%s)' % k.field_name
                #[ uint8_t field_instance_${k.header_name}_${k.field_name}[$byte_width]
            else:
                byte_width = get_key_byte_width(k)
                #[ uint8_t field_instance_${k.header.name}_${k.field_name}[$byte_width]
            
            # TODO have keys' and tables' match_type the same case (currently: LPM vs lpm)
            if k.match_type == "ternary":
                #[ ,uint8_t field_instance_${k.header.name}_${k.field_name}_mask[$byte_width]
            if k.match_type == "lpm":
                #[ ,uint8_t field_instance_${k.header.name}_${k.field_name}_prefix_length          

    #})
    #{ {
    
    if table.key_length_bytes==0:  # TODO: Check this part!!!
        if len(table.key.keyElements) == 0:
            #[     uint8_t key[0];
        elif table.key.keyElements[0].header_name=='meta':
            #[     uint8_t key[member_size(all_metadatas_t, field_metadata_${table.key.keyElements[0].field_name})];
    else:
         #[     uint8_t key[${table.key_length_bytes}];

    byte_idx = 0
    for k in sorted((k for k in table.key.keyElements if k.get_attr('match_type') is not None), key = lambda k: match_type_order(k.match_type)):
        if not hasattr(k,"header_name"):
            continue
        # TODO should properly handle specials (isValid etc.)
        if k.get_attr('header') is None:
            if k.header_name!='meta':
                continue
        if k.header_name=='meta':
            byte_width = 'member_size(all_metadatas_t, field_metadata_%s)' % k.field_name
            #[ memcpy(key+$byte_idx, field_instance_${k.header_name}_${k.field_name}, $byte_width);
            byte_idx += get_key_byte_width(k) # Metafield size???
        else:
            byte_width = get_key_byte_width(k)
            #[ memcpy(key+$byte_idx, field_instance_${k.header.name}_${k.field_name}, $byte_width);
            byte_idx += byte_width 

    if table.match_type == "LPM":
        #[ uint8_t prefix_length = 0;
        for k in table.key.keyElements:
            if not hasattr(k,"header_name"):
                continue
            # TODO should properly handle specials (isValid etc.)
            if k.get_attr('header') is None:
                continue

            if k.match_type == "exact":
                #[ prefix_length += ${get_key_byte_width(k)};
            if k.match_type == "lpm":
                #[ prefix_length += field_instance_${k.header.name}_${k.field_name}_prefix_length;
        #[ int c, d;
        #[ for(c = ${byte_idx-1}, d = 0; c >= 0; c--, d++) *(reverse_buffer+d) = *(key+c);
        #[ for(c = 0; c < ${byte_idx}; c++) *(key+c) = *(reverse_buffer+c);
        #[ lpm_delete_promote(TABLE_${table.name}, (uint8_t*)key, prefix_length, true);

    if table.match_type == "EXACT":
        #[ exact_delete_promote(TABLE_${table.name}, (uint8_t*)key,true);
    table_byte_idex = 0
    if table.match_type == "TERNARY":
        #[ ternary_delete_promote(TABLE_${table.name}, (uint8_t*)key, true);

    #} }



for table in hlir16_tables_with_keys:
    #[ // note: ${table.name}, ${table.match_type}, ${table.key_length_bytes}
    #{ void ${table.name}_get(
    for k in table.key.keyElements:
        if not hasattr(k,"header_name"):
            continue
        # TODO should properly handle specials (isValid etc.)
        if k.get_attr('header') is None:
            if k.header_name!='meta':
                continue

        if k.header_name=='meta':
            byte_width = 'member_size(all_metadatas_t, field_metadata_%s)' % k.field_name
            #[ uint8_t field_instance_${k.header_name}_${k.field_name}[$byte_width], 
        else:
            byte_width = get_key_byte_width(k)
            #[ uint8_t field_instance_${k.header.name}_${k.field_name}[$byte_width],
        
        # TODO have keys' and tables' match_type the same case (currently: LPM vs lpm)
        if k.match_type == "ternary":
            #[ uint8_t field_instance_${k.header.name}_${k.field_name}_mask[$byte_width],
        if k.match_type == "lpm":
            #[ uint8_t field_instance_${k.header.name}_${k.field_name}_prefix_length,

    #}     struct ${table.name}_action action)
    #{ {
    
    if table.key_length_bytes==0:  # TODO: Check this part!!!
        if len(table.key.keyElements) == 0:
            #[     uint8_t key[0];
        elif table.key.keyElements[0].header_name=='meta':
            #[     uint8_t key[member_size(all_metadatas_t, field_metadata_${table.key.keyElements[0].field_name})];
    else:
         #[     uint8_t key[${table.key_length_bytes}];

    byte_idx = 0
    for k in sorted((k for k in table.key.keyElements if k.get_attr('match_type') is not None), key = lambda k: match_type_order(k.match_type)):
        if not hasattr(k,"header_name"):
            continue
        # TODO should properly handle specials (isValid etc.)
        if k.get_attr('header') is None:
            if k.header_name!='meta':
                continue
        if k.header_name=='meta':
            byte_width = 'member_size(all_metadatas_t, field_metadata_%s)' % k.field_name
            #[ memcpy(key+$byte_idx, field_instance_${k.header_name}_${k.field_name}, $byte_width);
            byte_idx += get_key_byte_width(k) # Metafield size???
        else:
            byte_width = get_key_byte_width(k)
            #[ memcpy(key+$byte_idx, field_instance_${k.header.name}_${k.field_name}, $byte_width);
            byte_idx += byte_width 

    if table.match_type == "LPM":
        #[ uint8_t prefix_length = 0;
        for k in table.key.keyElements:
            # TODO should properly handle specials (isValid etc.)
            if k.get_attr('header') is None:
                continue

            if k.match_type == "exact":
                #[ prefix_length += ${get_key_byte_width(k)};
            if k.match_type == "lpm":
                #[ prefix_length += field_instance_${k.header.name}_${k.field_name}_prefix_length;
        #[ int c, d;
        #[ for(c = ${byte_idx-1}, d = 0; c >= 0; c--, d++) *(reverse_buffer+d) = *(key+c);
        #[ for(c = 0; c < ${byte_idx}; c++) *(key+c) = *(reverse_buffer+c);
        #[ lpm_lookup_promote(TABLE_${table.name}, (uint8_t*)key, true);

    if table.match_type == "EXACT":
        #[ exact_lookup_promote(TABLE_${table.name}, (uint8_t*)key,true);

    #} }



for table in hlir16.tables:
    #[ void ${table.name}_setdefault(struct ${table.name}_action action)
    #[ {
    #[     debug("${table.name}_setdefault\n");
    #[     table_setdefault_promote(TABLE_${table.name}, (uint8_t*)&action);
    #[ }

for table in hlir16_tables_with_keys:
    #{ void ${table.name}_add_table_entry(struct p4_ctrl_msg* ctrl_m) {
    for i, k in enumerate(table.key.keyElements):
        # TODO should properly handle specials (isValid etc.)
        if not hasattr(k,"header_name"):
            continue
        if k.get_attr('header') is None:
            if k.header_name!='meta':
                continue

        if k.match_type == "exact":
            if k.header_name=='meta':
                #[ uint8_t* field_instance_${k.header_name}_${k.field_name} = (uint8_t*)(((struct p4_field_match_exact*)ctrl_m->field_matches[${i}])->bitmap);
            else:
                #[ uint8_t* field_instance_${k.header.name}_${k.field_name} = (uint8_t*)(((struct p4_field_match_exact*)ctrl_m->field_matches[${i}])->bitmap);
        if k.match_type == "lpm":
            #[ uint8_t* field_instance_${k.header.name}_${k.field_name} = (uint8_t*)(((struct p4_field_match_lpm*)ctrl_m->field_matches[${i}])->bitmap);
            #[ uint16_t field_instance_${k.header.name}_${k.field_name}_prefix_length = ((struct p4_field_match_lpm*)ctrl_m->field_matches[${i}])->prefix_length;
        if k.match_type == "ternary":
            # TODO are these right?
            #[ uint8_t* field_instance_${k.header.name}_${k.field_name} = (uint8_t*)(((struct p4_field_match_ternary*)ctrl_m->field_matches[${i}])->bitmap);
            #[ uint8_t* field_instance_${k.header.name}_${k.field_name}_mask = ((struct p4_field_match_ternary*)ctrl_m->field_matches[${i}])->mask;
    if table.match_type == "TERNARY":
        #[ uint16_t priority = ((struct p4_field_match_ternary*)ctrl_m->field_matches[0])->priority;
    for action in table.actions:
        # TODO is there a more appropriate source for this than the annotation?
        action_name_str = get_action_name_str(action)
        #{ if(strcmp("$action_name_str", ctrl_m->action_name)==0) {
        #[     struct ${table.name}_action action;
        #[     action.action_id = action_${action.action_object.name};
        #[     //debug("From controller: add new entry to $$[table]{table.name} with action $$[action]{action.action_object.name}\n");
        for j, p in enumerate(action.action_object.parameters.parameters):
            #[ uint8_t* ${p.name} = (uint8_t*)((struct p4_action_parameter*)ctrl_m->action_params[$j])->bitmap;

            if p.type('type_ref').size <= 32:
                size = 8 if p.type('type_ref').size <= 8 else 16 if p.type('type_ref').size <= 16 else 32
                #[ //debug("   :: $$[field]{p.name} ($${}{%d} bits): $$[bytes]{}{%d}\n", ${p.type('type_ref').size}, *(uint${size}_t*)${p.name});
            else:
                #[ //dbg_bytes(${p.name}, (${p.type('type_ref').size}+7)/8, "   :: $$[field]{p.name} ($${}{%d} bits): ", ${p.type('type_ref').size});

            #[ memcpy(action.${action.action_object.name}_params.${p.name}, ${p.name}, ${(p.type._type_ref.size+7)/8});
        #{     ${table.name}_add(
        for i, k in enumerate(table.key.keyElements):
            # TODO handle specials properly (isValid etc.)
            if not hasattr(k,"header_name"):
                continue
            if k.get_attr('header') is None:
                if k.header_name!='meta':
                    continue
            if k.header_name == 'meta':
                #[ field_instance_${k.header_name}_${k.field_name},
            else:
                #[ field_instance_${k.header.name}_${k.field_name},
            if k.match_type == "lpm":
                #[ field_instance_${k.header.name}_${k.field_name}_prefix_length,
            if k.match_type == "ternary":
                #[ field_instance_${k.header.name}_${k.field_name}_mask /* TODO dstPort_mask */,
        if table.match_type == "TERNARY":
            #[ priority,
        #[     action);
        #}
        #} } //else

    valid_actions = ", ".join(["\" T4LIT(" + get_action_name_str(a) + ",action) \"" for a in table.actions])
    #[ //debug(" $$[warning]{}{!!!! Table add entry} on table $$[table]{table.name}: action name $$[warning]{}{mismatch}: $$[action]{}{%s}, expected one of ($valid_actions).\n", ctrl_m->action_name);
    #} }
#[ void multicast_group_table_add_table_entry(struct p4_ctrl_msg* ctrl_m) { 
#[  uint8_t* field_instance_multicast_group_id = (uint8_t*)(((struct p4_field_match_exact*)ctrl_m->field_matches[0])->bitmap); 
#[  //debug("---------multicast_group_id:%d,%d,%d,%d,%d,%d\n",*field_instance_multicast_group_id,*(field_instance_multicast_group_id+1),*(field_instance_multicast_group_id+2),*(field_instance_multicast_group_id+3),*(field_instance_multicast_group_id+4),*(field_instance_multicast_group_id+5));
#[  if(strcmp("set_bitmcast", ctrl_m->action_name)==0) { 
#[      struct multicast_group_table_action action; 
#[      action.action_id = action_set_bitmcast; 
#[      uint8_t* bit_mcast = (uint8_t*)((struct p4_action_parameter*)ctrl_m->action_params[0])->bitmap;
#[       //debug("   :: " T4LIT(port_num_4,field) " (" T4LIT(%d) " bits): " T4LIT(%d,bytes) "\n", 9, *(uint16_t*)port_num_4);
#[       memcpy(action.set_bitmcast_params.bit_mcast, bit_mcast, 4);
#[      //debug("--------------------1111\n");
#[      //debug("From controller: add new entry to " T4LIT(acl_table_0,table) " with action " T4LIT(send_to_cpu,action) "\n");
#[      multicast_group_table_add( 
#[  field_instance_multicast_group_id,
#[      action); 
#[  } //else 
#[  //debug(" " T4LIT(!!!! Table add entry,warning) " on table " T4LIT(acl_table_0,table) ": action name " T4LIT(mismatch,warning) ": " T4LIT(%s,action) ", expected one of (" T4LIT(IngressPipeImpl.send_to_cpu,action) ", " T4LIT(IngressPipeImpl.drop,action) ", " T4LIT(NoAction,action) ").\n", ctrl_m->action_name);
#[  } 
#[ void clone_session_table_add_table_entry(struct p4_ctrl_msg* ctrl_m) { 
#[  uint8_t* field_instance_clone_session_id = (uint8_t*)(((struct p4_field_match_exact*)ctrl_m->field_matches[0])->bitmap); 
#[  //debug("---------clone_session_id:%d,%d,%d,%d,%d,%d\n",*field_instance_clone_session_id,*(field_instance_clone_session_id+1),*(field_instance_clone_session_id+2),*(field_instance_clone_session_id+3),*(field_instance_clone_session_id+4),*(field_instance_clone_session_id+5));
#[  if(strcmp("set_clone_egress_port", ctrl_m->action_name)==0) { 
#[      struct clone_session_table_action action; 
#[      action.action_id = action_set_clone_egress_port; 
#[      uint8_t* egress_port = (uint8_t*)((struct p4_action_parameter*)ctrl_m->action_params[0])->bitmap;
#[       //debug("   :: " T4LIT(port_num_4,field) " (" T4LIT(%d) " bits): " T4LIT(%d,bytes) "\n", 9, *(uint16_t*)port_num_4);
#[       memcpy(action.set_clone_egress_port_params.egress_port, egress_port, 2);
#[      //debug("--------------------1111\n");
#[      //debug("From controller: add new entry to " T4LIT(acl_table_0,table) " with action " T4LIT(send_to_cpu,action) "\n");
#[      clone_session_table_add( 
#[  field_instance_clone_session_id,
#[      action); 
#[  } //else 
#[  if(strcmp("set_clone_multicast_group_id", ctrl_m->action_name)==0) { 
#[      struct clone_session_table_action action; 
#[      action.action_id = action_set_clone_multicast_group_id; 
#[      uint8_t* multicast_group_id = (uint8_t*)((struct p4_action_parameter*)ctrl_m->action_params[0])->bitmap;
#[       //debug("   :: " T4LIT(port_num_4,field) " (" T4LIT(%d) " bits): " T4LIT(%d,bytes) "\n", 9, *(uint16_t*)port_num_4);
#[       memcpy(action.set_clone_multicast_group_id_params.multicast_group_id, multicast_group_id, 2);
#[      debug("--------------------1111\n");
#[      //debug("From controller: add new entry to " T4LIT(acl_table_0,table) " with action " T4LIT(send_to_cpu,action) "\n");
#[      clone_session_table_add( 
#[  field_instance_clone_session_id,
#[      action); 
#[  } //else 
#[  //debug(" " T4LIT(!!!! Table add entry,warning) " on table " T4LIT(acl_table_0,table) ": action name " T4LIT(mismatch,warning) ": " T4LIT(%s,action) ", expected one of (" T4LIT(IngressPipeImpl.send_to_cpu,action) ", " T4LIT(IngressPipeImpl.drop,action) ", " T4LIT(NoAction,action) ").\n", ctrl_m->action_name);
#[  } 

for table in hlir16_tables_with_keys:
    #{ void ${table.name}_remove_table_entry(struct p4_ctrl_msg* ctrl_m) {
    for i, k in enumerate(table.key.keyElements):
        # TODO should properly handle specials (isValid etc.)
        if not hasattr(k,"header_name"):
            continue
        if k.get_attr('header') is None:
            if k.header_name!='meta':
                continue

        if k.match_type == "exact":
            if k.header_name=='meta':
                #[ uint8_t* field_instance_${k.header_name}_${k.field_name} = (uint8_t*)(((struct p4_field_match_exact*)ctrl_m->field_matches[${i}])->bitmap);
            else:
                #[ uint8_t* field_instance_${k.header.name}_${k.field_name} = (uint8_t*)(((struct p4_field_match_exact*)ctrl_m->field_matches[${i}])->bitmap);
        if k.match_type == "lpm":
            #[ uint8_t* field_instance_${k.header.name}_${k.field_name} = (uint8_t*)(((struct p4_field_match_lpm*)ctrl_m->field_matches[${i}])->bitmap);
            #[ uint16_t field_instance_${k.header.name}_${k.field_name}_prefix_length = ((struct p4_field_match_lpm*)ctrl_m->field_matches[${i}])->prefix_length;
        if k.match_type == "ternary":
            # TODO are these right?
            #[ uint8_t* field_instance_${k.header.name}_${k.field_name} = (uint8_t*)(((struct p4_field_match_ternary*)ctrl_m->field_matches[${i}])->bitmap);
            #[ uint16_t field_instance_${k.header.name}_${k.field_name}_mask = ((struct p4_field_match_ternary*)ctrl_m->field_matches[${i}])->mask;

    j = 0
    #{     ${table.name}_remove(
    for i, k in enumerate(table.key.keyElements):
        j = j + 1
        # TODO handle specials properly (isValid etc.)
        if not hasattr(k,"header_name"):
            continue
        if k.get_attr('header') is None:
            if k.header_name!='meta':
                continue
        if j < len(table.key.keyElements):
            if k.header_name == 'meta':
                #[ field_instance_${k.header_name}_${k.field_name},
            else:
                #[ field_instance_${k.header.name}_${k.field_name},
            if k.match_type == "lpm":
                #[ field_instance_${k.header.name}_${k.field_name}_prefix_length,
            if k.match_type == "ternary":
                #[ field_instance_${k.header.name}_${k.field_name}_mask /* TODO dstPort_mask */,
        else:
            if k.header_name == 'meta':
                #[ field_instance_${k.header_name}_${k.field_name}
            else:
                #[ field_instance_${k.header.name}_${k.field_name}
            if k.match_type == "lpm":
                #[ ,field_instance_${k.header.name}_${k.field_name}_prefix_length
            if k.match_type == "ternary":
                #[ ,field_instance_${k.header.name}_${k.field_name}_mask /* TODO dstPort_mask */             
    #[     );
    #}


    valid_actions = ", ".join(["\" T4LIT(" + get_action_name_str(a) + ",action) \"" for a in table.actions])
    #[ //debug(" $$[warning]{}{!!!! Table add entry} on table $$[table]{table.name}: action name $$[warning]{}{mismatch}: $$[action]{}{%s}, expected one of ($valid_actions).\n", ctrl_m->action_name);
    #} }



for table in hlir16_tables_with_keys:
    #{ void ${table.name}_get_table_entries(struct p4_ctrl_msg* ctrl_m) {
    for i, k in enumerate(table.key.keyElements):
        # TODO should properly handle specials (isValid etc.)
        if not hasattr(k,"header_name"):
            continue
        if k.get_attr('header') is None:
            if k.header_name!='meta':
                continue

        if k.match_type == "exact":
            if k.header_name=='meta':
                #[ uint8_t* field_instance_${k.header_name}_${k.field_name} = (uint8_t*)(((struct p4_field_match_exact*)ctrl_m->field_matches[${i}])->bitmap);
            else:
                #[ uint8_t* field_instance_${k.header.name}_${k.field_name} = (uint8_t*)(((struct p4_field_match_exact*)ctrl_m->field_matches[${i}])->bitmap);
        if k.match_type == "lpm":
            #[ uint8_t* field_instance_${k.header.name}_${k.field_name} = (uint8_t*)(((struct p4_field_match_lpm*)ctrl_m->field_matches[${i}])->bitmap);
            #[ uint16_t field_instance_${k.header.name}_${k.field_name}_prefix_length = ((struct p4_field_match_lpm*)ctrl_m->field_matches[${i}])->prefix_length;
        if k.match_type == "ternary":
            # TODO are these right?
            #[ uint8_t* field_instance_${k.header.name}_${k.field_name} = (uint8_t*)(((struct p4_field_match_ternary*)ctrl_m->field_matches[${i}])->bitmap);
            #[ uint16_t field_instance_${k.header.name}_${k.field_name}_mask = ((struct p4_field_match_ternary*)ctrl_m->field_matches[${i}])->mask;

    for action in table.actions:
        # TODO is there a more appropriate source for this than the annotation?
        action_name_str = get_action_name_str(action)
        #{ if(strcmp("$action_name_str", ctrl_m->action_name)==0) {
        #[     struct ${table.name}_action action;
        #[     action.action_id = action_${action.action_object.name};
        for j, p in enumerate(action.action_object.parameters.parameters):
            #[ uint8_t* ${p.name} = (uint8_t*)((struct p4_action_parameter*)ctrl_m->action_params[$j])->bitmap;

            if p.type('type_ref').size <= 32:
                size = 8 if p.type('type_ref').size <= 8 else 16 if p.type('type_ref').size <= 16 else 32
                #[ //debug("   :: $$[field]{p.name} ($${}{%d} bits): $$[bytes]{}{%d}\n", ${p.type('type_ref').size}, *(uint${size}_t*)${p.name});
            else:
                #[ //dbg_bytes(${p.name}, (${p.type('type_ref').size}+7)/8, "   :: $$[field]{p.name} ($${}{%d} bits): ", ${p.type('type_ref').size});

            #[ memcpy(action.${action.action_object.name}_params.${p.name}, ${p.name}, ${(p.type._type_ref.size+7)/8});

        #{     ${table.name}_get(
        for i, k in enumerate(table.key.keyElements):
            # TODO handle specials properly (isValid etc.)
            if not hasattr(k,"header_name"):
                continue
            if k.get_attr('header') is None:
                if k.header_name!='meta':
                    continue
            if k.header_name == 'meta':
                #[ field_instance_${k.header_name}_${k.field_name},
            else:
                #[ field_instance_${k.header.name}_${k.field_name},
            if k.match_type == "lpm":
                #[ field_instance_${k.header.name}_${k.field_name}_prefix_length,
            if k.match_type == "ternary":
                #[ field_instance_${k.header.name}_${k.field_name}_mask /* TODO dstPort_mask */,
        #[     action);
        #}
        #} } //else

    valid_actions = ", ".join(["\" T4LIT(" + get_action_name_str(a) + ",action) \"" for a in table.actions])
    #[ //debug(" $$[warning]{}{!!!! Table add entry} on table $$[table]{table.name}: action name $$[warning]{}{mismatch}: $$[action]{}{%s}, expected one of ($valid_actions).\n", ctrl_m->action_name);
    #} }



for table in hlir16_tables_with_keys:
    #{ void ${table.name}_set_default_table_action(struct p4_ctrl_msg* ctrl_m) {
    for action in table.actions:
        action_name_str = get_action_name_str(action)
        #{ if(strcmp("$action_name_str", ctrl_m->action_name)==0) {
        #[     debug("${table.name}_set_default_table_action, $action_name_str\n");
        #[     struct ${table.name}_action action;
        #[     action.action_id = action_${action.action_object.name};
        for j, p in enumerate(action.action_object.parameters.parameters):
            #[ uint8_t* ${p.name} = (uint8_t*)((struct p4_action_parameter*)ctrl_m->action_params[$j])->bitmap;
            #[ memcpy(action.${action.action_object.name}_params.${p.name}, ${p.name}, ${(p.type._type_ref.size+7)/8});
        #[     //debug("From controller: set default action for $$[table]{table.name} with action $$[action]{action_name_str}\n");
        #[     ${table.name}_setdefault( action );
        #} } //else

    valid_actions = ", ".join(["\" T4LIT(" + get_action_name_str(a) + ",action) \"" for a in table.actions])
    #[ //debug(" $$[warning]{}{!!!! Table setdefault} on table $$[table]{table.name}: action name $$[warning]{}{mismatch} ($$[action]{}{%s}), expected one of ($valid_actions).\n", ctrl_m->action_name);
    #} }


#{ void ctrl_add_table_entry(struct p4_ctrl_msg* ctrl_m) {
for table in hlir16_tables_with_keys:
    table_name_str = get_table_name_str(table)
    #{ if (strcmp("${table_name_str}", ctrl_m->table_name) == 0) {
    #[     ${table.name}_add_table_entry(ctrl_m);
    #[     return;
    #} }
#[ if (strcmp("multicast_group_table", ctrl_m->table_name) == 0) { 
#[     multicast_group_table_add_table_entry(ctrl_m); 
#[      return; 
#[  }
#[ if (strcmp("clone_session_table", ctrl_m->table_name) == 0) { 
#[     clone_session_table_add_table_entry(ctrl_m); 
#[      return; 
#[  }
#[     //debug(" $$[warning]{}{!!!! Table add entry}: table name $$[warning]{}{mismatch} ($$[table]{}{%s}), expected one of ($keyed_table_names).\n", ctrl_m->table_name);
#} }


#########################################################  REMOVE  #####################################################################
#{ void ctrl_remove_table_entry(struct p4_ctrl_msg* ctrl_m) {
for table in hlir16_tables_with_keys:
    table_name_str = get_table_name_str(table)
    #{ if (strcmp("${table_name_str}", ctrl_m->table_name) == 0) {
    #[     ${table.name}_remove_table_entry(ctrl_m);
    #[     return;
    #} }
#} }


#{ void ctrl_get_table_entries(struct p4_ctrl_msg* ctrl_m) {
for table in hlir16_tables_with_keys:
    table_name_str = get_table_name_str(table)
    #{ if (strcmp("${table_name_str}", ctrl_m->table_name) == 0) {
    #[     ${table.name}_get_table_entries(ctrl_m);
    #[     return;
    #} }
#} }


#[ extern char* action_names[];

#{ void ctrl_setdefault(struct p4_ctrl_msg* ctrl_m) {
#[ //debug("Set default message from control plane for table $$[table]{}{%s}: $$[action]{}{%s}\n", ctrl_m->table_name, ctrl_m->action_name);
for table in hlir16_tables_with_keys:
    table_name_str = get_table_name_str(table)
    #{ if (strcmp("${table_name_str}", ctrl_m->table_name) == 0) {
    #[     ${table.name}_set_default_table_action(ctrl_m);
    #[     return;
    #} }

#[     //debug(" $$[warning]{}{!!!! Table setdefault}: table name $$[warning]{}{mismatch} ($$[table]{}{%s}), expected one of ($keyed_table_names).\n", ctrl_m->table_name);
#} }

hack_i={}
for smem in hlir16.registers:
        for c in smem.components:
            cname = c['name']
            if cname in hack_i:
		continue
            hack_i[cname] = 1
            if smem.smem_type not in ["direct_counter", "direct_meter"]:
                    #[uint${smem.bit_width}_t ctrl_${cname}[${smem.bit_width}];

#{ uint32_t* read_register_by_name(char* register_name, int* size){
#[   int i;
hack_i = {}
for smem in hlir16.registers:
        for c in smem.components:
            cname = c['name']
            if cname in hack_i:
                continue
            hack_i[cname] = 1
            if smem.smem_type not in ["direct_counter", "direct_meter"]:
                #{ if(strcmp("${cname}", register_name) == 0){
                #[    *size = ${smem.amount};
                #[    for (i=0; i<${smem.amount}; i++) extern_register_read_uint${smem.bit_width}_t(global_smem.${cname}, &ctrl_${cname}[i], i);
                #[    return  ctrl_${cname};
                #} }
#[   *size = -1;
#[   return size;
#} }

#{ uint32_t* write_register_by_name(char* register_name, int* size, int index, int data){
#[   int i;
#[   //printf("++++++++++++++++++++++++index = %d++++++++++++++++++++++++++\n", index);
#[   //printf("++++++++++++++++++++++++data = %d++++++++++++++++++++++++++\n", data);
hack_i = {}
for smem in hlir16.registers:
        for c in smem.components:
            cname = c['name']
            if cname in hack_i:
                continue
            hack_i[cname] = 1
            if smem.smem_type not in ["direct_counter", "direct_meter"]:
                #{ if(strcmp("${cname}", register_name) == 0){
                #[    *size = ${smem.amount};
                #[    extern_register_write_uint${smem.bit_width}_t(global_smem.${cname}, index, data);
                #[    return size;
                #} }
#[   *size = -1;
#[   return size;
#} }

#[ extern volatile int ctrl_is_initialized;
#{ void ctrl_initialized() {
#[     //debug("Control plane fully initialized.\n");
#[     ctrl_is_initialized = 1;
#} }


#{ void ctrl_packet_out(struct p4_ctrl_msg* ctrl_m) {
#[     //debug("Control plane send packetout\n");
#[     send_packetout_from_controller(ctrl_m);
#} }

#{ void recv_from_controller(struct p4_ctrl_msg* ctrl_m) {
#{     if (ctrl_m->type == P4T_ADD_TABLE_ENTRY) {
#[          ctrl_add_table_entry(ctrl_m);
#[     } else if (ctrl_m->type == P4T_SET_DEFAULT_ACTION) {
#[         ctrl_setdefault(ctrl_m);
#[     } else if (ctrl_m->type == P4T_CTRL_INITIALIZED) {
#[         ctrl_initialized();
#[     } else if (ctrl_m->type == P4T_REMOVE_TABLE_ENTRY) {
#[          ctrl_remove_table_entry(ctrl_m);
#[     } else if (ctrl_m->type == P4T_GET_TABLE_ENTRIES) {
#[          ctrl_get_table_entries(ctrl_m);
#[     } else if (ctrl_m->type == P4T_PACKET_OUT) {
#[         ctrl_packet_out(ctrl_m);
#}     }
#} }

#[ ctrl_plane_backend bg;
#[ void init_control_plane()
#[ {
#[ #ifndef T4P4S_NO_CONTROL_PLANE
#[     bg = create_backend(3, 1000, "localhost", 11111, recv_from_controller);
#[     launch_backend(bg);
#[     dev_mgr_init_with_t4p4s(dev_mgr_ptr, recv_from_controller, read_register_by_name, write_register_by_name, ctrl_all_counter_handle, 1);
#[     PIGrpcServerRunAddrGnmi("0.0.0.0:50051", 0);
#[     //PIGrpcServerRunAddrin("192.168.133.189:50052");
#[     //PIGrpcServerRunAddrout("192.168.133.189:50053");
#[     //bg = create_backend(3, 1000, "localhost", 11111, recv_from_controller);
#[     //launch_backend(bg);
#[     //dev_mgr_init_with_t4p4s(dev_mgr_ptr, recv_from_controller, 1);


#[ #endif
#[ }

#[ void async_packetin(uint8_t* data, int len)
#[ {   
#[    //for (int i = 0; i < len; i++)
#[    //{
#[        //printf("%02x",*data);
#[       // data++;
#[    //}
#[     //debug("async_packetin_data---------%02x",*data);
#[     async_packetin_data(data,len);
#[ }
