// Autogenerated file (from compiler/backend/tables.c.py via ../compiler/backend/tables.c.py), do not modify directly.
// Generator: PPK

 #include "dataplane.h" // compiler/backend/tables.c.py:16
 #include "actions.h" // compiler/backend/tables.c.py:17
 #include "tables.h" // compiler/backend/tables.c.py:18
 #include "stateful_memory.h" // compiler/backend/tables.c.py:19

 lookup_table_t table_config[NB_TABLES] = { // compiler/backend/tables.c.py:22
 { // compiler/backend/tables.c.py:26
  .name= "mf_exact", // compiler/backend/tables.c.py:27
  .id = TABLE_mf_exact, // compiler/backend/tables.c.py:28
  .type = LOOKUP_EXACT, // compiler/backend/tables.c.py:29
  .entry = { // compiler/backend/tables.c.py:31
      .entry_count = 0, // compiler/backend/tables.c.py:32
      .key_size = 4, // compiler/backend/tables.c.py:34
      .entry_size = sizeof(struct mf_exact_action) + sizeof(entry_validity_t), // compiler/backend/tables.c.py:36
      .action_size   = sizeof(struct mf_exact_action), // compiler/backend/tables.c.py:37
      .validity_size = sizeof(entry_validity_t), // compiler/backend/tables.c.py:38
  }, // compiler/backend/tables.c.py:39
  .min_size = 0, // compiler/backend/tables.c.py:41
  .max_size = 250000, // compiler/backend/tables.c.py:42
 }, // compiler/backend/tables.c.py:43
 { // compiler/backend/tables.c.py:26
  .name= "geo_exact", // compiler/backend/tables.c.py:27
  .id = TABLE_geo_exact, // compiler/backend/tables.c.py:28
  .type = LOOKUP_EXACT, // compiler/backend/tables.c.py:29
  .entry = { // compiler/backend/tables.c.py:31
      .entry_count = 0, // compiler/backend/tables.c.py:32
      .key_size = 16, // compiler/backend/tables.c.py:34
      .entry_size = sizeof(struct geo_exact_action) + sizeof(entry_validity_t), // compiler/backend/tables.c.py:36
      .action_size   = sizeof(struct geo_exact_action), // compiler/backend/tables.c.py:37
      .validity_size = sizeof(entry_validity_t), // compiler/backend/tables.c.py:38
  }, // compiler/backend/tables.c.py:39
  .min_size = 0, // compiler/backend/tables.c.py:41
  .max_size = 250000, // compiler/backend/tables.c.py:42
 }, // compiler/backend/tables.c.py:43
  {  // compiler/backend/tables.c.py:44
    .name= "multicast_group_table",  // compiler/backend/tables.c.py:45
    .id = TABLE_multicast_group_table,  // compiler/backend/tables.c.py:46
    .type = LOOKUP_EXACT,  // compiler/backend/tables.c.py:47
    .entry = {  // compiler/backend/tables.c.py:48
        .entry_count = 0,  // compiler/backend/tables.c.py:49
        .key_size = 2,  // compiler/backend/tables.c.py:50
        .entry_size = sizeof(struct multicast_group_table_action) + sizeof(entry_validity_t),  // compiler/backend/tables.c.py:51
        .action_size   = sizeof(struct multicast_group_table_action),  // compiler/backend/tables.c.py:52
        .validity_size = sizeof(entry_validity_t),  // compiler/backend/tables.c.py:53
    },  // compiler/backend/tables.c.py:54
    .min_size = 0,  // compiler/backend/tables.c.py:55
    .max_size = 250000,  // compiler/backend/tables.c.py:56
   },  // compiler/backend/tables.c.py:57
  {  // compiler/backend/tables.c.py:58
    .name= "clone_session_table",  // compiler/backend/tables.c.py:59
    .id = TABLE_clone_session_table,  // compiler/backend/tables.c.py:60
    .type = LOOKUP_EXACT,  // compiler/backend/tables.c.py:61
    .entry = {  // compiler/backend/tables.c.py:62
        .entry_count = 0,  // compiler/backend/tables.c.py:63
        .key_size = 4,  // compiler/backend/tables.c.py:64
        .entry_size = sizeof(struct clone_session_table_action) + sizeof(entry_validity_t),  // compiler/backend/tables.c.py:65
        .action_size   = sizeof(struct clone_session_table_action),  // compiler/backend/tables.c.py:66
        .validity_size = sizeof(entry_validity_t),  // compiler/backend/tables.c.py:67
    },  // compiler/backend/tables.c.py:68
    .min_size = 0,  // compiler/backend/tables.c.py:69
    .max_size = 250000,  // compiler/backend/tables.c.py:70
   },  // compiler/backend/tables.c.py:71
 }; // compiler/backend/tables.c.py:72

