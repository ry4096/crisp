#include <stdlib.h>
#include <assert.h>
#include <malloc.h>

#include "hash_table.h"

//#define LOGGING_OFF
#include "log.h"

#define LOG_RESIZE_FAULT

#define MAX_LOAD_RATIO .7
#define MIN_LOAD_RATIO .2
#define MIN_ARRAY_LENGTH 7


#define STRING_HASH_CONSTANT 31
// Make this bigger maybe?
#define INT_HASH_CONSTANT 31


// Asserts that keys that are now found
// really are not in the table
// by performing a linear search.
//#define ASSERT_UNIQUE


int string_hash(void* self, int seed, char* obj) {
   int hash = seed;
   while (*obj) {
      hash *= STRING_HASH_CONSTANT;
      hash += *obj;
      obj++;
   }
   return hash;
}

int string_eq(void* self, char* obj1, char* obj2) {
   return !strcmp(obj1, obj2);
}

struct hash_vtable hash_vtable_string = {
   .hash = (hash_fn)string_hash,
   .eq = (hash_eq_fn)string_eq
};
struct hash_vtable* hash_string_obj = &hash_vtable_string;


int pointer_hash(void* self, int seed, void* obj) {
   int hash = seed;
   hash *= INT_HASH_CONSTANT;
   hash += (int)obj;
   return hash;
}

int pointer_eq(void* self, void* obj1, void* obj2) {
   return obj1 == obj2;
}

struct hash_vtable hash_vtable_pointer = {
   .hash = (hash_fn)pointer_hash,
   .eq = (hash_eq_fn)pointer_eq
};
struct hash_vtable* hash_pointer_obj = &hash_vtable_pointer;


int key_value_left_hash(struct hash_key_value_obj* self, int seed, void* obj) {
   return (*self->hash_child)->hash(self->hash_child, seed, obj);
}

int key_value_left_eq(struct hash_key_value_obj* self, struct key_value* left, void* right) {
   return (*self->hash_child)->eq(self->hash_child, left->key, right);
}

struct hash_vtable hash_vtable_key_value_left = {
   .hash = (hash_fn)key_value_left_hash,
   .eq = (hash_eq_fn)key_value_left_eq
};

int key_value_hash(struct hash_key_value_obj* self, int seed, struct key_value* obj) {
   return (*self->hash_child)->hash(self->hash_child, seed, obj->key);
}

int key_value_eq(struct hash_key_value_obj* self, struct key_value* left, struct key_value* right) {
   return (*self->hash_child)->eq(self->hash_child, left->key, right->key);
}

struct hash_vtable hash_vtable_key_value = {
   .hash = (hash_fn)key_value_hash,
   .eq = (hash_eq_fn)key_value_eq
};


struct key_value* key_value(void* key, void* value) {
   struct key_value* alloc = (struct key_value*)malloc(sizeof(struct key_value));
   assert(alloc);
   alloc->key = key;
   alloc->value = value;
   return alloc;
}


// hasher is used internally to hash and compare keys within the same table.
void hash_table_init(struct hash_table* self, hash_obj hasher, int array_length) {
   if (array_length < MIN_ARRAY_LENGTH) {
      array_length = MIN_ARRAY_LENGTH;
   }

   self->array = (void**)calloc(array_length, sizeof(void*));
   assert(self->array);
   self->array_length = array_length;
   self->num_elts = 0;
   self->hasher = hasher;
}

void hash_table_destruct(struct hash_table* self) {
   free(self->array);
}

int hash_table_find_index(struct hash_table* self, void* key, hash_obj query) {
   int hash = (*query)->hash(query, 0, key);
   int i = hash % self->array_length;
   if (i < 0) i += self->array_length;

   for (;;) {
      void* curr = self->array[i];

      if (!curr) {
#ifdef ASSERT_UNIQUE
         int j;
         for (j = 0; j < self->array_length; j++) {
            if (self->array[j]) {
               assert(!(*query)->eq(query, self->array[j], key));
            }
         }
#endif
         return i;
      }

      if ((*query)->eq(query, curr, key))
         return i;

      i = (i + 1) % self->array_length;
   }
}

// It's assumed the slot will be assigned to something if null.
void** hash_table_slot_(struct hash_table* self, void* key, hash_obj query) {
   int i = hash_table_find_index(self, key, query);
   void** slot = self->array + i;
   if (!*slot) {
      self->num_elts++;
   }

   return slot;
}

void* hash_table_insert_(struct hash_table* self, void* key, hash_obj query) {
   void** slot = hash_table_slot_(self, key, query);
   void* old = *slot;
   *slot = key;
   return old;
}

void* hash_map_set_(struct hash_table* self, void* key, void* value, hash_obj query) {
   void** slot = hash_table_slot_(self, key, query);
   struct key_value* binding = (struct key_value*)(*slot);

   if (binding) {
      void* old = binding->value;
      binding->value = value;
      return old;
   }

   *slot = key_value(key, value);
   return NULL;
}

void* hash_set_find(struct hash_table* self, void* key, hash_obj query) {
   if (query == NULL)
      query = self->hasher;
   int i = hash_table_find_index(self, key, query);
   return self->array[i];
}

struct key_value* hash_map_find(struct hash_table* self, void* key, hash_obj query) {
   if (query == NULL)
      query = self->hasher;

   struct hash_key_value_obj key_val_hash;
   key_val_hash.vtable = &hash_vtable_key_value_left;
   key_val_hash.hash_child = query;

   return (struct key_value*)hash_set_find(self, key, (hash_obj)&key_val_hash);
}

void* hash_map_find_value(struct hash_table* self, void* key, hash_obj query) {
   struct key_value* key_val = hash_map_find(self, key, query);
   if (key_val)
      return key_val->value;

   return NULL;
}

void hash_table_resize(struct hash_table* self, int new_size, hash_obj query) {
   void** old_array = self->array;
   int old_length = self->array_length;

   self->array = (void**)calloc(new_size, sizeof(void*));
   assert(self->array);
   self->array_length = new_size;
   self->num_elts = 0;

   int i;
   for (i = 0; i < old_length; i++) {
      void* curr = old_array[i];
      
      if (curr) {
#ifndef LOG_RESIZE_FAULT
         void* ret = hash_table_insert_(self, curr, query);
         assert(ret == NULL);
#else
         void** slot = hash_table_slot_(self, curr, query);
         if (*slot) {
            LOG("hash_table_resize failure: %s\n", curr)
            assert(0);
         }
         *slot = curr;
#endif
      }
   }

   free(old_array);
}

int hash_table_check_up_size(struct hash_table* self, hash_obj query) {
   double ratio = (double)(self->num_elts + 1)/(double)self->array_length;
   if (ratio > MAX_LOAD_RATIO) {
      int new_size = (self->array_length << 1) + 1;
      hash_table_resize(self, new_size, query);
      return 1;
   }
   return 0;
}

int hash_table_check_down_size(struct hash_table* self, hash_obj query) {
   double ratio = (double)self->num_elts/(double)self->array_length;
   if (self->array_length > MIN_ARRAY_LENGTH && ratio < MIN_LOAD_RATIO) {
      int new_size = self->array_length >> 1;
      hash_table_resize(self, new_size, query);
      return 1;
   }
   return 0;
}

void** hash_table_slot(struct hash_table* self, void* key, hash_obj query, hash_obj internal) {
   hash_table_check_up_size(self, internal);
   return hash_table_slot_(self, key, query);
}

void** hash_set_slot(struct hash_table* self, void* key, hash_obj query) {
   if (query == NULL)
      query = self->hasher;

   void** ret = hash_table_slot(self, key, query, self->hasher);

   //LOG("hash_set_slot: %s -> %p => %p\n", key, ret, *ret)

   return ret;
}

struct key_value** hash_map_slot(struct hash_table* self, void* key, hash_obj query) {
   if (query == NULL)
      query = self->hasher;

   struct hash_key_value_obj key_query;
   key_query.vtable = &hash_vtable_key_value_left;
   key_query.hash_child = query;

   struct hash_key_value_obj internal;
   internal.vtable = &hash_vtable_key_value;
   internal.hash_child = self->hasher;

   return (struct key_value**)hash_table_slot(self, key, (hash_obj)&key_query, (hash_obj)&internal);
}

struct key_value* hash_map_intern(struct hash_table* self, void* key, hash_obj query) {
   struct key_value** slot = hash_map_slot(self, key, query);
   if (!*slot) {
      *slot = key_value(key, NULL);
   }

   return *slot;
}

void* hash_table_insert(struct hash_table* self, void* key, hash_obj internal) {
   hash_table_check_up_size(self, internal);
   return hash_table_insert_(self, key, internal);
}

void* hash_set_insert(struct hash_table* self, void* key) {
   return hash_table_insert(self, key, self->hasher);
}

struct key_value* hash_map_insert(struct hash_table* self, struct key_value* key) {
   struct hash_key_value_obj hasher;
   hasher.vtable = &hash_vtable_key_value;
   hasher.hash_child = self->hasher;

   return (struct key_value*)hash_table_insert(self, key, (hash_obj)&hasher);
}

void* hash_map_set(struct hash_table* self, void* key, void* value) {
   struct hash_key_value_obj key_query;
   key_query.vtable = &hash_vtable_key_value_left;
   key_query.hash_child = self->hasher;

   struct hash_key_value_obj internal;
   internal.vtable = &hash_vtable_key_value;
   internal.hash_child = self->hasher;

   hash_table_check_up_size(self, (hash_obj)&internal);
   return hash_map_set_(self, key, value, (hash_obj)&key_query);
}

void hash_table_rehash_index(struct hash_table* self, int i, hash_obj hasher) {
   void* key = self->array[i];
   self->array[i] = NULL;
   self->num_elts--;

   void* ret = hash_table_insert_(self, key, hasher);
   assert(!ret);
}

void hash_table_rehash_block(struct hash_table* self, int i, hash_obj hasher) {
   for (;;) {
      void* curr = self->array[i];

      if (!curr)
         break;

      hash_table_rehash_index(self, i, hasher);
      
      i = (i + 1) % self->array_length;
   }
}

void* hash_table_remove(struct hash_table* self, void* key, hash_obj query, hash_obj internal) {
   int i = hash_table_find_index(self, key, query);
   void* old = self->array[i];

   if (!old) {
      return NULL;
   }

   self->num_elts--;
   self->array[i] = NULL;

   int changed = hash_table_check_down_size(self, internal);
   
   if (!changed) {
      i = (i + 1) % self->array_length;
      hash_table_rehash_block(self, i, internal);
   }

   return old;
}

void* hash_set_remove(struct hash_table* self, void* key, hash_obj query) {
   if (query == NULL)
      query = self->hasher;

   return hash_table_remove(self, key, query, self->hasher);
}

void* hash_map_remove(struct hash_table* self, void* key, hash_obj query) {
   if (query == NULL)
      query = self->hasher;

   struct hash_key_value_obj key_query;
   key_query.vtable = &hash_vtable_key_value_left;
   key_query.hash_child = query;

   struct hash_key_value_obj internal;
   internal.vtable = &hash_vtable_key_value;
   internal.hash_child = self->hasher;

   return hash_table_remove(self, key, (hash_obj)&key_query, (hash_obj)&internal);
}

void hash_set_for_each(struct hash_table* self, hash_value_fn callback) {
   int i;
   for (i = 0; i < self->array_length; i++) {
      void* curr = self->array[i];

      if (curr) {
         callback(curr);
      }
   }
}

void hash_set_for_each2(struct hash_table* self, hash_value2_fn* callback) {
   int i;
   for (i = 0; i < self->array_length; i++) {
      void* curr = self->array[i];

      if (curr) {
         (*callback)(callback, curr);
      }
   }
}

void hash_set_for_each3(struct hash_table* self, hash_value2_fn callback, void* callback_data) {
   int i;
   for (i = 0; i < self->array_length; i++) {
      void* curr = self->array[i];

      if (curr) {
         callback(callback_data, curr);
      }
   }
}

void print_string(FILE* output, char* obj) {
   if (obj) {
      fprintf(output, "%s", obj);
   } else {
      fprintf(output, "<NULL>");
   }
}

void hash_set_dump(FILE* output, struct hash_table* self, hash_value2_fn callback) {
   int i;
   for (i = 0; i < self->array_length; i++) {
      fprintf(output, "[%d] = ", i);
      callback(output, self->array[i]);
      fprintf(output, "\n");
   }
}

void hash_map_dump(FILE* output, struct hash_table* self, hash_value2_fn key_callback, hash_value2_fn value_callback) {
   int i;
   for (i = 0; i < self->array_length; i++) {
      struct key_value* curr = (struct key_value*)self->array[i];
      if (self->array[i]) {
         fprintf(output, "[%d] = ", i);
         key_callback(output, curr->key);
         fprintf(output, " -> ");
         value_callback(output, curr->value);
         fprintf(output, "\n");
      } else {
         fprintf(output, "[%d] = <NULL>\n", i);
      }
   }
}



