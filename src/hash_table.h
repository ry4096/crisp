#ifndef HASH_TABLE_
#define HASH_TABLE_

typedef int (*hash_fn)(void* self, int seed, void* obj);
typedef int (*hash_eq_fn)(void* self, void* obj1, void* obj2);
typedef void (*hash_value_fn)(void* obj);
typedef void (*hash_value2_fn)(void* self, void* obj);

struct hash_vtable {
   hash_fn hash;
   hash_eq_fn eq;
};

typedef struct hash_vtable** hash_obj;

int string_hash(void* self, int seed, char* obj);
int string_eq(void* self, char* obj1, char* obj2);

extern struct hash_vtable hash_vtable_string;
extern struct hash_vtable* hash_string_obj;

int pointer_hash(void* self, int seed, void* obj);
int pointer_eq(void* self, void* obj1, void* obj2);

extern struct hash_vtable hash_vtable_pointer;
extern struct hash_vtable* hash_pointer_obj;

struct key_value {
   void* key;
   void* value;
};

// Used internally.
struct hash_key_value_obj {
   struct hash_vtable* vtable;
   hash_obj hash_child;
};

int key_value_left_hash(struct hash_key_value_obj* self, int seed, void* obj);
int key_value_left_eq(struct hash_key_value_obj* self, struct key_value* left, void* right);

extern struct hash_vtable hash_vtable_key_value_left;

int key_value_hash(struct hash_key_value_obj* self, int seed, struct key_value* obj);
int key_value_eq(struct hash_key_value_obj* self, struct key_value* left, struct key_value* right);

extern struct hash_vtable hash_vtable_key_value;

struct key_value* key_value(void* key, void* value);


struct hash_table {
   void** array;
   int array_length;
   int num_elts;
   hash_obj hasher;
};



void hash_table_init(struct hash_table* self, hash_obj hasher, int array_length);
void hash_table_destruct(struct hash_table* self);
int hash_table_find_index(struct hash_table* self, void* key, hash_obj query);
void** hash_table_slot_(struct hash_table* self, void* key, hash_obj query);
void* hash_table_insert_(struct hash_table* self, void* key, hash_obj query);
void* hash_map_set_(struct hash_table* self, void* key, void* value, hash_obj query);
void* hash_set_find(struct hash_table* self, void* key, hash_obj query);
struct key_value* hash_map_find(struct hash_table* self, void* key, hash_obj query);
void* hash_map_find_value(struct hash_table* self, void* key, hash_obj query);
void hash_table_resize(struct hash_table* self, int new_size, hash_obj query);
int hash_table_check_up_size(struct hash_table* self, hash_obj query);
int hash_table_check_down_size(struct hash_table* self, hash_obj query);
void** hash_table_slot(struct hash_table* self, void* key, hash_obj query, hash_obj internal);
void** hash_set_slot(struct hash_table* self, void* key, hash_obj query);
struct key_value** hash_map_slot(struct hash_table* self, void* key, hash_obj query);
struct key_value* hash_map_intern(struct hash_table* self, void* key, hash_obj query);
void* hash_table_insert(struct hash_table* self, void* key, hash_obj internal);
void* hash_set_insert(struct hash_table* self, void* key);
struct key_value* hash_map_insert(struct hash_table* self, struct key_value* key);
void* hash_map_set(struct hash_table* self, void* key, void* value);
void hash_table_rehash_index(struct hash_table* self, int i, hash_obj hasher);
void hash_table_rehash_block(struct hash_table* self, int i, hash_obj hasher);
void* hash_table_remove(struct hash_table* self, void* key, hash_obj query, hash_obj internal);
void* hash_set_remove(struct hash_table* self, void* key, hash_obj query);
void* hash_map_remove(struct hash_table* self, void* key, hash_obj query);
void hash_set_for_each(struct hash_table* self, hash_value_fn callback);
void hash_set_for_each2(struct hash_table* self, hash_value2_fn* callback);
void hash_set_for_each3(struct hash_table* self, hash_value2_fn callback, void* callback_data);
void print_string(FILE* output, char* obj);
void hash_set_dump(FILE* output, struct hash_table* self, hash_value2_fn callback);
void hash_map_dump(FILE* output, struct hash_table* self, hash_value2_fn key_callback, hash_value2_fn value_callback);

#endif
