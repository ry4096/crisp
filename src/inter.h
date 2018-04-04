#ifndef __INTER_H__
#define __INTER_H__

#include "list.h"
#include "hash_table.h"
#include "stack.h"
#include "error.h"
#include "parser.h"
#include "dlist.h"
#include "path.h"

typedef struct type_vtable** type;

struct ast_node_with_type;
struct inter;

struct typed_value {
   type type;
   char val[0];
};

void type_print(FILE* output, type type);
void type_free_(struct inter* inter, type type);

//#define INTER_DESTRUCT_VAL_MACRO

#ifdef INTER_DESTRUCT_VAL_MACRO

#define inter_destruct_val(inter, val_type, val) \
   if ((*(val_type))->type_destruct_val) { \
      (*(val_type))->type_destruct_val((val_type), (val), (inter)); \
   }

#else

void inter_destruct_val(struct inter* inter, type val_type, void* val);

#endif



void inter_destruct_val2(struct inter* inter, type val_type, void* val);

typedef void* (*alloc_fn)(void* self, int size);

void* malloc_fn(void* self, int size);
extern alloc_fn malloc_fn_;

void* typed_value_get_val(struct typed_value* self);
void typed_value_destruct(struct typed_value* self, struct inter* inter);
void typed_value_destruct_(struct inter* inter, struct typed_value* self);
void typed_value_print(FILE* output, struct typed_value* self);
int typed_value_size(type type);
struct typed_value* typed_value(type type, alloc_fn* alloc);



struct prefix {
   struct dlist node;
   char* prefix;
   int prefix_len;
   struct inter* inter;
};

struct symbol {
   struct prefix* prefix;
   char* ending;
};


int symbol_hash(void* obj, int seed, struct symbol* sym);
int sym_string_eq(void* obj, char* string, struct symbol* sym);
extern struct hash_vtable hash_vtable_sym_string;
extern struct hash_vtable* hash_sym_string_obj;

struct key_value* sym_key_value(char* sym_key, void* value);
void sym_key_value_free(struct key_value* self, struct inter* inter, list_for_each_fn callback);
struct key_value* inter_mapping_intern(struct hash_table* map, char* sym_key);
void inter_mapping_push_value(struct hash_table* map, char* sym_key, void* value);
void inter_mapping_destruct(struct hash_table* map, struct inter* inter, list_for_each_fn callback);
void inter_mapping_destruct_keys_only(struct hash_table* map, struct inter* inter);


struct inter {
   // maps names to type constructors
   struct hash_table sym_table;

   struct hash_table type_constructor_table;
   struct hash_table type_def_table;
   struct hash_table values_table;

   struct stack stack;

   struct prefix* current_prefix;
   struct dlist_root prefix_list;

   struct dlist_root script_path_stack;
   struct hash_table import_set;
};


typedef type (*type_constructor_fn)(struct inter* self, struct ast_node* root, error* err);
// type_query_fn has been eliminated and replaced with struct inter*
//typedef type_constructor_fn (*type_query_fn)(void* self, char* type_name, int type_name_len);



typedef void (*type_free_fn)(type self, struct inter* inter);
typedef type (*type_copy_fn)(type self);
typedef void (*type_print_fn)(FILE* output, type self);
typedef int (*type_match_fn)(type self, type other);
typedef int (*type_size_fn)(type self);
typedef void (*type_print_val_fn)(FILE* output, type self, void* val);
typedef void (*type_copy_val_fn)(type self, void* src, void* dest);
typedef void (*type_destruct_val_fn)(type self, void* src, struct inter* inter);
typedef error (*type_parse_constant_fn)(type self, struct ast_node_with_type* node, void* value);
typedef error (*type_check_call_fn)(struct inter* inter, type expected_type, struct ast_node_with_type* root);
typedef error (*type_eval_call_fn)(struct inter* inter, struct ast_node_with_type* root, void* value, int size);

struct bit_section {
   char width;
   char offset;
};

union number_t {
   char c;
   unsigned char uc;
   short s;
   unsigned short us;
   int i;
   unsigned int u;
   long l;
   unsigned long ul;
   long long ll;
   unsigned long long ull;
   float f;
   double d;
   long double ld;
   void* p;
   struct bit_section bit_section; // Used for bit fields in structs.
};

struct eval_table {
   type_eval_call_fn eval_rval;
   type_eval_call_fn eval_lval;
};

struct ast_node_with_type {
   struct ast_node ast_node;
   type type;
   int returns_lval;
   struct key_value* symbol;
   union number_t constant;
   struct eval_table eval_table;
};


struct type_vtable {
   type_free_fn type_free;
   type_copy_fn type_copy;
   type_print_fn type_print;
   type_match_fn type_match;
   type_size_fn type_size;
   type_print_val_fn type_print_val;
   type_copy_val_fn type_copy_val;
   type_destruct_val_fn type_destruct_val;
   type_parse_constant_fn type_parse_constant;
   type_check_call_fn type_check_call;
   type_eval_call_fn type_eval_call;
};


void type_free_nothing(type self, struct inter* inter);
int type_match_static(type self, type other);
type type_copy_static(type self);



struct type_def_cell {
   struct type_vtable* vtable;
   char* key;
   type value;
};


struct error_type_unbound {
   struct error_vtable* vtable;
   struct ast_node* location;
   struct type_def_cell* cell;
};

void error_type_unbound_print(FILE* output, struct error_type_unbound* self);
extern struct error_vtable error_type_unbound_vtable;
error error_type_unbound(struct ast_node* location, struct type_def_cell* cell);

void error_type_already_bound_print(FILE* output, struct error_type_unbound* self);
extern struct error_vtable error_type_already_bound_vtable;
error error_type_already_bound(struct ast_node* location, struct type_def_cell* cell);




void type_def_cell_print(FILE* output, struct type_def_cell* self);
int type_def_cell_match(struct type_def_cell* self, type other);
void type_def_cell_copy_val(struct type_def_cell* self, void* src, void* dest);
void type_def_cell_destruct_val(struct type_def_cell* self, void* val, struct inter* inter);
int type_def_cell_size(struct type_def_cell* self);
void type_def_cell_copy_val(struct type_def_cell* self, void* src, void* dest);

#define TYPE_DEF_CELL_GET_TYPE_MACRO

#ifdef TYPE_DEF_CELL_GET_TYPE_MACRO

#define TYPE_DEF_CELL_GET_TYPE(self, location, ret, err) \
   if (!(self) || *(self) != &type_def_cell_vtable) \
      ret = self; \
   else if (((struct type_def_cell*)self)->value == NULL) \
      err = error_type_unbound(location, (struct type_def_cell*)self); \
   else \
      ret = ((struct type_def_cell*)self)->value;
      

#else

error type_def_cell_get_type(type self, struct ast_node* location, type* ret);

#define TYPE_DEF_CELL_GET_TYPE(self, location, ret, err) \
   err = type_def_cell_get_type(self, location, &ret);

#endif



error type_def_cell_bind_type(struct inter* inter, struct ast_node_with_type* node);
void type_def_cell_unbind(struct type_def_cell* self, struct inter* inter);
void type_def_cell_free(struct type_def_cell* self, struct inter* inter);
void type_def_cell_unbind_free(struct inter* inter, struct type_def_cell* self);
void type_def_cell_print_val(FILE* output, struct type_def_cell* self, void* value);
error type_def_cell_parse_constant(struct type_def_cell* self, struct ast_node_with_type* node, void* value);
extern struct type_vtable type_def_cell_vtable;
struct type_def_cell* type_def_cell(char* key, type value);



char* prefix_find_sym(struct prefix* self, char* text, int text_len);
char* prefix_make_sym(struct prefix* self, char* text, int text_len);

struct key_value* prefix_find_type_cons_binding(struct prefix* self, char* name, int name_len);
struct key_value* prefix_make_type_cons_binding(struct prefix* self, char* name, int name_len);
struct key_value* prefix_find_types_binding(struct prefix* self, char* name, int name_len);
struct key_value* prefix_make_types_binding(struct prefix* self, char* name, int name_len);
struct key_value* prefix_find_values_binding(struct prefix* self, char* name, int name_len);
struct key_value* prefix_make_values_binding(struct prefix* self, char* name, int name_len);

type_constructor_fn prefix_query_type_cons(struct prefix* self, char* name, int name_len);
type prefix_query_type(struct prefix* self, char* name, int name_len);
void prefix_def_type_cons(struct prefix* self, char* name, int name_len, type_constructor_fn type_cons);
void prefix_def_type(struct prefix* self, char* name, int name_len, type type);
type_constructor_fn prefix_undef_type_cons(struct prefix* self, char* name, int name_len);
type prefix_undef_type(struct prefix* self, char* name, int name_len);
struct typed_value* prefix_get_value(struct prefix* self, char* name, int name_len);
void prefix_def_value(struct prefix* self, char* name, int name_len, struct typed_value* value);
struct typed_value* prefix_undef_value(struct prefix* self, char* name, int name_len);
void prefix_init(struct prefix* self, char* prefix, int prefix_len, struct inter* inter);
void prefix_destruct(struct prefix* self, struct inter* inter);
void prefix_free(struct prefix* self, struct inter* inter);



struct key_value* inter_find_type_cons_binding(struct inter* inter, char* name, int name_len);
struct key_value* inter_make_type_cons_binding(struct inter* inter, char* name, int name_len);
struct key_value* inter_find_type_binding(struct inter* inter, char* name, int name_len);
struct key_value* inter_make_type_binding(struct inter* inter, char* name, int name_len);
struct key_value* inter_find_value_binding(struct inter* inter, char* name, int name_len);
struct key_value* inter_make_value_binding(struct inter* inter, char* name, int name_len);

type_constructor_fn inter_query_type_cons(struct inter* inter, char* name, int name_len);
type inter_query_type(struct inter* inter, char* name, int name_len);
void inter_def_type_cons(struct inter* inter, char* name, int name_len, type_constructor_fn type_cons);
void inter_def_type(struct inter* inter, char* name, int name_len, type type);
type_constructor_fn inter_undef_type_cons(struct inter* inter, char* name, int name_len);
type inter_undef_type(struct inter* inter, char* name, int name_len);
struct typed_value* inter_get_value(struct inter* inter, char* name, int name_len);
void inter_def_value(struct inter* inter, char* name, int name_len, struct typed_value* value);
struct typed_value* inter_undef_value(struct inter* inter, char* name, int name_len);
//void inter_undef_all(struct inter* inter);
void inter_free_value(struct inter* inter, char* name, int name_len);
void inter_pop_value(struct inter* inter, char* name, int name_len);
void inter_cache_type_cons_symbol(struct inter* inter, struct ast_node_with_type* node);
void inter_cache_type_symbol(struct inter* inter, struct ast_node_with_type* node);
void inter_cache_value_symbol(struct inter* inter, struct ast_node_with_type* node);
void inter_cache_find_type_cons_symbol(struct inter* inter, struct ast_node_with_type* node);
void inter_cache_find_type_symbol(struct inter* inter, struct ast_node_with_type* node);
void inter_cache_find_value_symbol(struct inter* inter, struct ast_node_with_type* node);



void inter_init(struct inter* inter, void* stack_buffer, int stack_size);
void inter_clear_prefixes(struct inter* inter);
void inter_destruct(struct inter* inter);

char* inter_intern_sym(struct inter* inter, char* buffer, int length);
void inter_release_sym(struct inter* inter, char* buffer);

struct inter_conv_str {
   string_conv_fn fn;
   struct inter* inter;
};

char* inter_conv_str_fn(struct inter_conv_str* self, char* buffer, int length);

struct inter_del_str {
   string_del_fn fn;
   struct inter* inter;
};

void inter_del_str_fn(struct inter_del_str* self, char* buffer, int length);


void inter_push_abs_path_str(struct inter* inter, char* buffer, int buf_len);
void inter_push_abs_path(struct inter* inter, struct filepath* filepath);
void inter_push_relative_path(struct inter* inter, struct filepath* filepath);



void ast_node_with_type_free_types(struct ast_node_with_type* root, struct inter* inter);
void ast_node_with_type_free(struct ast_node_with_type* root, struct inter* inter);
void ast_node_with_type_rec_free(struct ast_node_with_type* root, struct inter* inter);
void ast_node_with_type_free_children(struct ast_node_with_type* root, struct inter* inter);
void ast_node_with_type_write_to(struct ast_node_with_type* root, struct inter* inter, struct ast_node_with_type* dest);
struct ast_node_with_type* ast_node_with_type_copy(struct ast_node_with_type* root);
struct ast_node_with_type* ast_node_with_type_copy_tree(struct ast_node_with_type* root);
void ast_node_with_type_write_location(struct ast_node_with_type* location, struct inter* inter, struct ast_node_with_type* tree);




#endif

