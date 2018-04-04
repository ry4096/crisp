#include <assert.h>
#include <malloc.h>

#include "hash_map_push.h"
#include "hash_table.h"
#include "list.h"

#define LOGGING_OFF
#include "log.h"


#ifndef HASH_MAP_SYM_TOP_VALUE_MACRO

void* hash_map_sym_top_value(struct key_value* self) {
   void* ret;
   struct list* top_node = (struct list*)self->value;
   if (!top_node)
      ret = NULL;
   else 
      ret = top_node->value;
   LOG("hash_map_sym_top_value: %s, %p\n", (char*)self->key, ret)
   return ret;
}

#endif

void hash_map_sym_push_value(struct key_value* self, void* value) {
   LOG("hash_map_sym_push_value: %s, %p\n", (char*)self->key, value)
   struct list** handle = (struct list**)&self->value;
   list_push(handle, value);
}

void* hash_map_sym_pop_value(struct key_value* self) {
   LOG("hash_map_sym_pop_value: %s\n", (char*)self->key)
   struct list** handle = (struct list**)&self->value;
   return list_pop(handle);
}


void hash_map_push_value(struct hash_table* root, char* key, void* value, hash_obj query) {
   struct key_value* key_val = hash_map_intern(root, key, query);
   struct list** handle = (struct list**)&key_val->value;
   list_push(handle, value);
}

void* hash_map_pop_value(struct hash_table* map, char* key, hash_obj query) {
   struct key_value* key_val = hash_map_find(map, key, query);

   if (!key_val)
      return NULL;

   struct list** handle = (struct list**)&key_val->value;
   if (*handle) {
      void* value = list_pop(handle);
      if (!*handle) {
         hash_map_remove(map, key, NULL);
         free(key_val);
      }
      return value;
   }
   else {
      return NULL;
   }
}

// Does not free the radix tree nodes underneath.
void* hash_map_pop_value_nf(struct hash_table* map, char* key, hash_obj query) {
   struct key_value* key_val = hash_map_find(map, key, query);

   if (!key_val)
      return NULL;

   struct list** handle = (struct list**)&key_val->value;
   if (*handle) {
      void* value = list_pop(handle);
      return value;
   }
   else {
      return NULL;
   }
}

void* hash_map_find_top_value(struct hash_table* map, char* key, hash_obj query) {
   struct list* head = (struct list*)hash_map_find_value(map, key, query);
   if (!head) {
      return NULL;
   }
   else {
      return head->value;
   }
}

void* hash_map_find_nth_value(struct hash_table* map, char* key, int index, hash_obj query) {
   struct list* head = (struct list*)hash_map_find_value(map, key, query);
   if (!head) {
      return NULL;
   }
   else {
      return list_nth(head, index);
   }
}

