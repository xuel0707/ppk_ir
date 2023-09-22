// Autogenerated file (from compiler/backend/tables.h.py via ../compiler/backend/tables.h.py), do not modify directly.
// Generator: PPK

 #ifndef __TABLES_H__ // compiler/backend/tables.h.py:15
 #define __TABLES_H__ // compiler/backend/tables.h.py:16

 #include "stateful_memory.h" // compiler/backend/tables.h.py:18
 #include "actions.h" // compiler/backend/tables.h.py:19

 typedef bool entry_validity_t; // compiler/backend/tables.h.py:21

 typedef struct table_entry_mf_exact_s { // compiler/backend/tables.h.py:24
     struct mf_exact_action  action; // compiler/backend/tables.h.py:25
     entry_validity_t         is_entry_valid; // compiler/backend/tables.h.py:26
 } table_entry_mf_exact_t; // compiler/backend/tables.h.py:27
 typedef struct table_entry_geo_exact_s { // compiler/backend/tables.h.py:24
     struct geo_exact_action  action; // compiler/backend/tables.h.py:25
     entry_validity_t         is_entry_valid; // compiler/backend/tables.h.py:26
 } table_entry_geo_exact_t; // compiler/backend/tables.h.py:27
  typedef struct table_entry_multicast_group_table_s {  // compiler/backend/tables.h.py:28
      struct multicast_group_table_action  action;  // compiler/backend/tables.h.py:29
     entry_validity_t         is_entry_valid;  // compiler/backend/tables.h.py:30
 } table_entry_multicast_group_table_t;  // compiler/backend/tables.h.py:31
 typedef struct table_entry_clone_session_table_s {  // compiler/backend/tables.h.py:32
     struct clone_session_table_action  action;  // compiler/backend/tables.h.py:33
     entry_validity_t         is_entry_valid;  // compiler/backend/tables.h.py:34
 } table_entry_clone_session_table_t;  // compiler/backend/tables.h.py:35

 #define NB_TABLES 4 // compiler/backend/tables.h.py:38

 enum table_names { // compiler/backend/tables.h.py:40
 TABLE_mf_exact, // compiler/backend/tables.h.py:42
 TABLE_geo_exact, // compiler/backend/tables.h.py:42
 TABLE_multicast_group_table, // compiler/backend/tables.h.py:43
 TABLE_clone_session_table, // compiler/backend/tables.h.py:44
 TABLE_, // compiler/backend/tables.h.py:45
 }; // compiler/backend/tables.h.py:46

 #endif // compiler/backend/tables.h.py:48
