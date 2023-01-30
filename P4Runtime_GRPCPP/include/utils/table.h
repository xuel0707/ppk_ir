
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <vector>

#define MAX_TABLE_ENTRY_SIZE 10000
#define MAX_MATCH_PARAM_SIZE 10

struct _param_t {
	uint64_t param_id;
	char* value;
    size_t value_size;
};

typedef struct _param_t param_t;

struct _action_t {
	uint64_t action_id;
        //std::vector<param_t> param;
	param_t* param[MAX_MATCH_PARAM_SIZE];
        uint64_t count;
};

typedef struct _action_t action_t;

struct _match_t {
    uint64_t field_id;
    uint32_t match_type;
    char* mask;
    size_t mask_size;
    int32_t prefix_len;
    char* value;
    size_t value_size;
};

typedef struct _match_t match_t;

struct _table_entry_t {
    uint32_t table_id;
    //std::vector<match_t> matches;
    match_t* matches[MAX_MATCH_PARAM_SIZE];
    uint64_t count;
    action_t action;
    uint16_t priority; 
};

typedef struct _table_entry_t table_entry_t;

struct _table_entries_t {
	table_entry_t* table_entry[MAX_TABLE_ENTRY_SIZE];
    uint64_t count;
};

typedef struct _table_entries_t table_entries_t;

table_entry_t* get_table_entry(table_entries_t *table_entries, uint32_t table_id, int64_t i);
table_entry_t* add_table_entry(table_entries_t *table_entries, uint32_t table_id);
void init_table_entries(table_entries_t *table_entries);
void destroy_table_entries(table_entries_t *table_entries);
// argument_t* get_argument( element_t *elem, uint64_t id);



