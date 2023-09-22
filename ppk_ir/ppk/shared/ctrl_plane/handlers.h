// Copyright 2016 Eotvos Lorand University, Budapest, Hungary
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
//     http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#ifndef __HANDLERS_H__
#define __HANDLERS_H__

#include "messages.h"

#define P4_MAX_NUMBER_OF_ACTION_PARAMETERS 10
#define P4_MAX_NUMBER_OF_FIELD_MATCHES 10

struct packetout {
  void *payload;
  int egressport;
  int len;
};
struct p4_ctrl_msg {
	uint8_t type;
	uint32_t xid;
	char* table_name;
	uint8_t action_type;
	char* action_name;
	int num_action_params;
	struct p4_action_parameter* action_params[P4_MAX_NUMBER_OF_ACTION_PARAMETERS];
	int num_field_matches;
	struct p4_field_match_header* field_matches[P4_MAX_NUMBER_OF_FIELD_MATCHES];
  
  struct packetout pd;      
  char* metadata[10];
	char* packet;
	int len;
};

struct p4_ctrl_counter {
    uint8_t type;
    int is_direct;
    char* counter_name;
    char* table_name;
    uint64_t index;
    uint64_t packets;
    uint64_t bytes;
};

typedef void (*p4_msg_callback)(struct p4_ctrl_msg*);
typedef uint32_t* (*p4_reg_read)(char*, int*);
typedef uint32_t* (*p4_reg_write)(char*, int*, int, int*);
typedef void (*p4_counter_callback)(struct p4_ctrl_counter *);

int handle_p4_msg(char* buffer, int length, p4_msg_callback cb);
int handle_p4_ctrl_initialized(struct p4_header* header, struct p4_ctrl_msg* ctrl_m);
int handle_p4_set_default_action(struct p4_set_default_action* m, struct p4_ctrl_msg* ctrl_m);
int handle_p4_add_table_entry(struct p4_add_table_entry* m, struct p4_ctrl_msg* ctrl_m);


#endif
