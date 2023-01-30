#include "table.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <stddef.h>


table_entry_t* get_table_entry(table_entries_t *table_entries, uint32_t table_id, int64_t i)
{
	if (table_entries->table_entry[i]==NULL){
		return NULL;
	}
	if (table_entries->table_entry[i]->table_id == table_id){
		return table_entries->table_entry[i];
	}else{
		return NULL;
	}
			
}



table_entry_t* add_table_entry(table_entries_t *table_entries, uint32_t table_id)
{
	int index = table_entries->count;
    if (index<MAX_TABLE_ENTRY_SIZE){
    	if (table_entries->table_entry[index]==NULL) {
			table_entries->table_entry[index] = (table_entry_t*)malloc(sizeof(table_entry_t));
            table_entries->table_entry[index]->table_id = table_id;
            (table_entries->count)++;
			return table_entries->table_entry[index];
		}
    }	
	return NULL;

}


void init_table_entries(table_entries_t *table_entries) {
        int i;
        for (i=0;i<MAX_TABLE_ENTRY_SIZE;++i) {
		table_entries->table_entry[i]=NULL;
        }
}

void destroy_table_entries(table_entries_t *table_entries) {
	int i;
	for (i=0;i<MAX_TABLE_ENTRY_SIZE;++i) {
		if (table_entries->table_entry[i]!=NULL) {
			free(table_entries->table_entry[i]);
                        table_entries->table_entry[i] = NULL;
                        (table_entries->count)--;
		}
	}
}


