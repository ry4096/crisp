#ifndef HASH_MAP_PUSH_H_
#define HASH_MAP_PUSH_H_


#include "hash_table.h"
#include "list.h"

#define HASH_MAP_SYM_TOP_VALUE_MACRO

#ifdef HASH_MAP_SYM_TOP_VALUE_MACRO

#define hash_map_sym_top_value(self) \
   (self->value ? ((struct list*)self->value)->value : NULL)

#else

void* hash_map_sym_top_value(struct key_value* self);

#endif


void hash_map_sym_push_value(struct key_value* self, void* value);
void* hash_map_sym_pop_value(struct key_value* self);
void hash_map_push_value(struct hash_table* root, char* key, void* value, hash_obj query);
void* hash_map_pop_value(struct hash_table* map, char* key, hash_obj query);

// Does not free the key_value
void* hash_map_pop_value_nf(struct hash_table* map, char* key, hash_obj query);
void* hash_map_find_top_value(struct hash_table* map, char* key, hash_obj query);
void* hash_map_find_nth_value(struct hash_table* map, char* key, int index, hash_obj query);

#endif
