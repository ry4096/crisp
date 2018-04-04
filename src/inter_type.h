#ifndef INTER_TYPE_H_
#define INTER_TYPE_H_

#include <stdio.h>
#include <stdint.h>

#include "hash_table.h"
#include "parser.h"
#include "token.h"
#include "inter.h"
#include "error.h"

#define EVAL_MACRO


#define MACRO_ARG0 "arg0"

struct error_malformed_constant {
   struct error_vtable* vtable;
   struct ast_node* location;
   type expected_type;
};



void error_malformed_constant_print(FILE* output, struct error_malformed_constant* self);
extern struct error_vtable error_malformed_constant_vtable;
error error_malformed_constant(struct ast_node* location, type expected_type);

void type_print_void(FILE* output, type self);
int type_size_void(type self);
void type_print_val_void(FILE* output, type self, void* val);
void type_copy_val_void(type self, void* src, void* dest);

void type_print_char(FILE* output, type self);
int type_size_char(type self);
void type_print_val_char(FILE* output, type self, char* val);
void type_copy_val_char(type self, char* src, char* dest);
error type_parse_char(type self, struct ast_node_with_type* node, char* value);

void type_print_uchar(FILE* output, type self);
void type_copy_val_uchar(type self, unsigned char* src, unsigned char* dest);
error type_parse_uchar(type self, struct ast_node_with_type* node, unsigned char* value);


void type_print_short(FILE* output, type self);
int type_size_short(type self);
void type_print_val_short(FILE* output, type self, short* val);
void type_copy_val_short(type self, short* src, short* dest);
error type_parse_short(type self, struct ast_node_with_type* node, short* value);

void type_print_ushort(FILE* output, type self);
void type_print_val_ushort(FILE* output, type self, unsigned short* val);
void type_copy_val_ushort(type self, unsigned short* src, unsigned short* dest);
error type_parse_ushort(type self, struct ast_node_with_type* node, short* value);

void type_print_int(FILE* output, type self);
int type_size_int(type self);
void type_print_val_int(FILE* output, type self, int* val);
void type_copy_val_int(type self, int* src, int* dest);
error type_parse_int(type self, struct ast_node_with_type* node, int* value);

void type_print_uint(FILE* output, type self);
void type_print_val_uint(FILE* output, type self, unsigned int* val);
void type_copy_val_uint(type self, unsigned int* src, unsigned int* dest);
error type_parse_uint(type self, struct ast_node_with_type* node, unsigned int* value);

void type_print_long(FILE* output, type self);
int type_size_long(type self);
void type_print_val_long(FILE* output, type self, long* val);
void type_copy_val_long(type self, long* src, long* dest) ;
error type_parse_long(type self, struct ast_node_with_type* node, long* value);

void type_print_ulong(FILE* output, type self);
void type_print_val_ulong(FILE* output, type self, unsigned long* val);
void type_copy_val_ulong(type self, unsigned long* src, unsigned long* dest);
error type_parse_ulong(type self, struct ast_node_with_type* node, unsigned long* value);

void type_print_long_long(FILE* output, type self);
int type_size_long_long(type self);
void type_print_val_long_long(FILE* output, type self, long long* val);
void type_copy_val_long_long(type self, long long* src, long long* dest);
error type_parse_long_long(type self, struct ast_node_with_type* node, long long* value);

void type_print_ulong_long(FILE* output, type self);
void type_print_val_ulong_long(FILE* output, type self, unsigned long long* val);
void type_copy_val_ulong_long(type self, unsigned long long* src, unsigned long long* dest);
error type_parse_ulong_long(type self, struct ast_node_with_type* node, unsigned long long* value);

void type_print_float(FILE* output, type self);
int type_size_float(type self);
void type_print_val_float(FILE* output, type self, float* val);
void type_copy_val_float(type self, float* src, float* dest);
error type_parse_float(type self, struct ast_node_with_type* node, float* value);

void type_print_double(FILE* output, type self);
int type_size_double(type self);
void type_print_val_double(FILE* output, type self, double* val);
void type_copy_val_double(type self, double* src, double* dest);
error type_parse_double(type self, struct ast_node_with_type* node, double* value);

void type_print_long_double(FILE* output, type self);
int type_size_long_double(type self);
void type_print_val_long_double(FILE* output, type self, long double* val);
void type_copy_val_long_double(type self, long double* src, long double* dest);
error type_parse_long_double(type self, struct ast_node_with_type* node, long double* value);

void type_print_pointer(FILE* output, type self);
int type_match_pointer(type self, type other);
int type_size_pointer(type self);
void type_print_val_pointer(FILE* output, type self, void** val);
void type_copy_val_pointer(type self, void** src, void** dest);
void type_copy_val_refc(type self, void** src, void** dest);
void type_destruct_val_free(type self, void** val, struct inter* inter);
void type_destruct_val_refc(type self, void** val, struct inter* inter);
error type_parse_pointer(type self, struct ast_node_with_type* node, void** value);

void type_print_string(FILE* output, type self);
void type_print_string_auto(FILE* output, type self);
void type_print_string_refc(FILE* output, type self);
void type_print_symbol(FILE* output, type self);
void type_print_val_string(FILE* output, type self, char** val);
void type_copy_val_strdup(type self, char** src, char** dest);
void type_destruct_val_symbol(type self, char** val, struct inter* inter);
error type_string_check_call(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error type_string_eval_call(struct inter* inter, struct ast_node_with_type* root, void* value, int size);


extern struct type_vtable type_void_vtable;
extern struct type_vtable type_char_vtable;
extern struct type_vtable type_uchar_vtable;
extern struct type_vtable type_short_vtable;
extern struct type_vtable type_ushort_vtable;
extern struct type_vtable type_int_vtable;
extern struct type_vtable type_uint_vtable;
extern struct type_vtable type_long_vtable;
extern struct type_vtable type_ulong_vtable;
extern struct type_vtable type_long_long_vtable;
extern struct type_vtable type_ulong_long_vtable;
extern struct type_vtable type_float_vtable;
extern struct type_vtable type_double_vtable;
extern struct type_vtable type_long_double_vtable;
extern struct type_vtable type_pointer_vtable;
extern struct type_vtable type_string_vtable;
extern struct type_vtable type_string_auto_vtable;
extern struct type_vtable type_string_refc_vtable;
extern struct type_vtable type_symbol_vtable;

extern struct type_vtable* type_void;
extern struct type_vtable* type_char;
extern struct type_vtable* type_uchar;
extern struct type_vtable* type_short;
extern struct type_vtable* type_ushort;
extern struct type_vtable* type_int;
extern struct type_vtable* type_uint;
extern struct type_vtable* type_long;
extern struct type_vtable* type_ulong;
extern struct type_vtable* type_long_long;
extern struct type_vtable* type_ulong_long;
extern struct type_vtable* type_float;
extern struct type_vtable* type_double;
extern struct type_vtable* type_long_double;
extern struct type_vtable* type_pointer;
extern struct type_vtable* type_string;
extern struct type_vtable* type_string_auto;
extern struct type_vtable* type_string_refc;
extern struct type_vtable* type_symbol;


void type_type_print(FILE* output, type self);
void type_type_print_val(FILE* output, type self, type* val);
void type_type_copy_val(type self, type* src, type* dest);
void type_type_destruct_val(type self, type* val, struct inter* inter);
extern struct type_vtable type_type_vtable;
extern struct type_vtable* type_type;



struct type_pointer_to {
   struct type_vtable* vtable;
   type content_type;
};

void type_pointer_to_free(struct type_pointer_to* self, struct inter* inter);
void type_pointer_to_print(FILE* output, struct type_pointer_to* self);
void type_auto_pointer_to_print(FILE* output, struct type_pointer_to* self);
void type_refc_pointer_to_print(FILE* output, struct type_pointer_to* self);
int type_pointer_to_match(struct type_pointer_to* self, struct type_pointer_to* other);
void type_auto_pointer_to_copy_val(struct type_pointer_to* self, void** src, void** dest);
void type_auto_pointer_to_destruct_val(struct type_pointer_to* self, void** val, struct inter* inter);
void type_refc_pointer_to_destruct_val(struct type_pointer_to* self, void** val, struct inter* inter);
error type_pointer_to_check_call(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error type_pointer_to_eval_call(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
extern struct type_vtable type_pointer_to_vtable;
extern struct type_vtable type_auto_pointer_to_vtable;
extern struct type_vtable type_refc_pointer_to_vtable;
type type_pointer_to(struct type_vtable* vtable, type content_type);
int type_is_pointer(type self);



struct type_array {
   struct type_vtable* vtable;
   type content_type;
   int length;
};

void type_array_free(struct type_array* self, struct inter* inter);
void type_array_print(FILE* output, struct type_array* self);
int type_array_match(struct type_array* self, struct type_array* other);
int type_array_size(struct type_array* self);
void type_array_print_val(FILE* output, struct type_array* self, void* value);
void type_array_copy_val(struct type_array* self, char* src, char* dest);
void type_array_destruct_val(struct type_array* self, char* val, struct inter* inter);
error type_array_check_call(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error type_array_eval_call(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
extern struct type_vtable type_array_vtable;
type type_array(int length, type content_type);



struct type_function {
   struct type_vtable* vtable;
   int num_arguments;
   type* argument_types;
   type return_type;
   int accepts_var_args;
};

void type_function_free(struct type_function* self, struct inter* inter);
void type_function_print(FILE* output, struct type_function* self, char* tag);
void type_c_function_print(FILE* output, struct type_function* self);
void type_buf_function_print(FILE* output, struct type_function* self);
int type_function_match(struct type_function* self, struct type_function* other);
error inter_check_return_type(struct ast_node_with_type* root, type expected_type, type return_type);
error type_function_check_call(struct inter* inter, type expected_type, struct ast_node_with_type* root);
int type_function_args_size(struct ast_node_with_type* root);
error type_function_eval_call_void(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error type_function_eval_call_char(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error type_function_eval_call_int(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error type_function_eval_call_long(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error type_function_eval_call_long_long(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error type_function_eval_call_float(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error type_function_eval_call_double(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error type_function_eval_call_long_double(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error type_function_eval_call_pointer(struct inter* inter, struct ast_node_with_type* root, void* value, int size);

typedef void (*buf_function_t)(void* buffer, int size, void* output, int output_size);
error type_buf_function_eval_call(struct inter* inter, struct ast_node_with_type* root, void* value, int size);


extern struct type_vtable type_function_void_vtable;
extern struct type_vtable type_function_char_vtable;
extern struct type_vtable type_function_int_vtable;
extern struct type_vtable type_function_long_vtable;
extern struct type_vtable type_function_float_vtable;
extern struct type_vtable type_function_double_vtable;
extern struct type_vtable type_function_long_double_vtable;
extern struct type_vtable type_function_pointer_vtable;
extern struct type_vtable type_buf_function_vtable;
type type_function(struct type_vtable* vtable, int num_arguments, type* argument_types, type return_type, int accepts_var_args);


struct type_c_macro {
   struct type_vtable* vtable;
   type_check_call_fn type_check;
   type return_type;
};

void type_c_macro_free(struct type_c_macro* self, struct inter* inter);
void type_c_macro_print(FILE* output, struct type_c_macro* self);
int type_c_macro_match(struct type_c_macro* self, struct type_c_macro* other);
error type_c_macro_check_call(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error type_c_macro_eval_call(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
extern struct type_vtable type_c_macro_vtable;
type type_c_macro(type_check_call_fn type_check, type return_type);


struct type_code {
   struct type_vtable* vtable;
   type return_type;
};

void type_code_free(struct type_code* self, struct inter* inter);
void type_code_print(FILE* output, struct type_code* self);
int type_code_match(struct type_code* self, struct type_code* other);
void type_code_print_val(FILE* output, struct type_code* self, struct ast_node_with_type** value);
error type_code_check_call(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error type_code_eval_call(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
void type_code_unsafe_print(FILE* output, struct type_code* self);
error type_code_unsafe_eval_call(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
extern struct type_vtable type_code_vtable;
extern struct type_vtable type_code_unsafe_vtable;
type type_code(struct type_vtable* vtable, type return_type);



void type_quote_print(FILE* output, type self);
void type_quote_print_val(FILE* output, type self, struct ast_node_with_type** value);
void type_quote_destruct_val(type self, struct ast_node_with_type** value, struct inter* inter);
error type_quote_check_call(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error type_quote_eval_call(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
extern struct type_vtable type_quote_vtable;
extern struct type_vtable* type_quote;


struct error_struct_field_undefined {
   struct error_vtable* vtable;
   struct ast_node* location;
   struct type_struct* type;
};

void error_struct_field_undefined_print(FILE* output, struct error_struct_field_undefined* self);
extern struct error_vtable error_struct_field_undefined_vtable;
error error_struct_field_undefined(struct ast_node* location, struct type_struct* type);


struct type_struct_slot {
   char* name;
   int name_len;
   int offset;
   type type;
   int bit_width; // set to zero if not applicable.
   int bit_offset; 
};

struct type_struct_slot* type_struct_slot(char* name, int name_len, int offset, type type);
void type_struct_slot_free(struct type_struct_slot* self, struct inter* inter);

void type_struct_slot_print(FILE* output, struct type_struct_slot* self);
void type_struct_slot_print_val(FILE* output, struct type_struct_slot* self, void* value);
int type_struct_slot_match(struct type_struct_slot* slot1, struct type_struct_slot* slot2);

enum struct_packing_scheme {
   STRUCT_PACKED = 0,
   STRUCT_UNION,
   STRUCT_ALIGN32,
   NUM_STRUCT_PACKING_SCHEMES
};

#define ALIGN_MASK(mask, val) \
   (((val) + (mask)) & (~(mask)))

#define ALIGN(bits, val) \
   ALIGN_MASK((1<<(bits)) - 1, val)

// returns ceil(log_2(size))
int size_alignment(int size);

extern char* struct_packing_scheme_strs[];

struct type_struct {
   struct type_vtable* vtable;
   struct hash_table fields;
   int num_slots;
   struct type_struct_slot** slots_array;
   int size;
   enum struct_packing_scheme packing_scheme;
};

void type_struct_free(struct type_struct* self, struct inter* inter);
void type_struct_print(FILE* output, struct type_struct* self);
void type_struct_print_val(FILE* output, struct type_struct* self, void* value);
int type_struct_match(struct type_struct* self, struct type_struct* other);
void type_struct_copy_val(struct type_struct* self, char* src, char* dest);
void type_struct_destruct_val(struct type_struct* self, char* val, struct inter* inter);
int type_struct_size(struct type_struct* self);
error type_struct_check_call(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error type_struct_eval_call_lval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error type_struct_eval_call_rval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);

error type_struct_eval_call_bit_char(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error type_struct_eval_call_bit_uchar(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error type_struct_eval_call_bit_short(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error type_struct_eval_call_bit_ushort(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error type_struct_eval_call_bit_int(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error type_struct_eval_call_bit_uint(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error type_struct_eval_call_bit_long(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error type_struct_eval_call_bit_ulong(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error type_struct_eval_call_bit_long_long(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error type_struct_eval_call_bit_ulong_long(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error type_struct_eval_call_bit_long_long(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error type_struct_eval_call_bit_ulong_long(struct inter* inter, struct ast_node_with_type* root, void* value, int size);





extern struct type_vtable type_struct_vtable;


void type_prefix_print(FILE* output, void* self);
void type_prefix_print_val(FILE* output, void* self, struct prefix* value);
void type_prefix_copy_val(type self, struct prefix* src, struct prefix* dest);
void type_prefix_destruct_val(type self, struct prefix* value, struct inter* inter);
int type_prefix_size(void* self);
error type_prefix_check_call(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error type_prefix_eval_call(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
extern struct type_vtable type_prefix_vtable;
extern struct type_vtable* type_prefix;


struct local_var_binding {
   type var_type;
   char* var_alloc;
};


struct typed_value* local_var_binding(type var_type, char* alloc);
void local_var_binding_free(struct typed_value* self, struct inter* inter);
void type_local_var_binding_print(FILE* output, type self);
int type_local_var_binding_size(type self);
void type_local_var_binding_print_val(FILE* output, type self, struct local_var_binding* val);
extern struct type_vtable type_local_var_binding_vtable;
extern struct type_vtable* type_local_var_binding;

void local_var_binding_increment(struct ast_node_with_type* root, int increment);




struct type_script_function_arg {
   struct key_value* symbol;
   char* name;
   int name_len;
   type type;
};


struct type_script_function {
   struct type_vtable* vtable;
   int num_arguments;
   struct type_script_function_arg* arguments;
   type return_type;
   int destruct_args;
};


void type_script_function_arg_destruct(struct type_script_function_arg* self, struct inter* inter);
void type_script_function_arg_print(FILE* output, struct type_script_function_arg* self);
int type_script_function_arg_match(struct type_script_function_arg* arg1, struct type_script_function_arg* arg2);
void type_script_function_free(struct type_script_function* self, struct inter* inter);
void type_script_function_print_args(FILE* output, struct type_script_function* self);
void type_script_function_print(FILE* output, struct type_script_function* self);
void type_script_function_print_val(FILE* output, struct type_script_function* self, void* value);
int type_script_function_match(struct type_script_function* self, struct type_script_function* other);
error type_script_function_check_call(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error type_script_function_eval_call(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
extern struct type_vtable type_script_function_vtable;


struct type_macro {
   struct type_vtable* vtable;
   int num_arguments;
   int accepts_var_args;
   struct key_value** arg_symbols;
};

void type_macro_free(struct type_macro* self, struct inter* inter);
void type_macro_print_args(FILE* output, struct type_macro* self);
void type_macro_print(FILE* output, struct type_macro* self);
void type_macro_print_val(FILE* output, struct type_macro* self, void* value);
int type_macro_match(struct type_macro* self, struct type_macro* other);
error type_macro_check_call(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error type_macro_eval_call(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
extern struct type_vtable type_macro_vtable;

int type_is_ast_node(type self);


struct error_wrong_number_of_args {
   struct error_vtable* vtable;
   struct ast_node* location;
   int expected_args;
   int num_args;
};

void error_wrong_number_of_args_print(FILE* output, struct error_wrong_number_of_args* self);
extern struct error_vtable error_wrong_number_of_args_vtable;
error error_wrong_number_of_args(struct ast_node* location, int expected_args, int num_args);


struct error_cannot_cast {
   struct error_vtable* vtable;
   struct ast_node* location;
   type cast_type;
   type value_type;
};

void error_cannot_cast_print(FILE* output, struct error_cannot_cast* self);
extern struct error_vtable error_cannot_cast_vtable;
error error_cannot_cast(struct ast_node* location, type cast_type, type value_type);



struct error_at_node {
   struct error_vtable* vtable;
   struct ast_node* location;
};

void error_syntax_print(FILE* output, struct error_at_node* self);
extern struct error_vtable error_syntax_vtable;
error error_syntax(struct ast_node* location);

void error_unknown_type_print(FILE* output, struct error_at_node* self);
extern struct error_vtable error_unknown_type_vtable;
error error_unknown_type(struct ast_node* location);

void error_symbol_undefined_print(FILE* output, struct error_at_node* self);
extern struct error_vtable error_symbol_undefined_vtable;
error error_symbol_undefined(struct ast_node* location);

void error_char_expected_print(FILE* output, struct error_at_node* self);
extern struct error_vtable error_char_expected_vtable;
error error_char_expected(struct ast_node* location);

void error_lval_expected_print(FILE* output, struct error_at_node* self);
extern struct error_vtable error_lval_expected_vtable;
error error_lval_expected(struct ast_node* location);

void error_type_required_print(FILE* output, struct error_at_node* self);
extern struct error_vtable error_type_required_vtable;
error error_type_required(struct ast_node* location);

void error_field_name_expected_print(FILE* output, struct error_at_node* self);
extern struct error_vtable error_field_name_expected_vtable;
error error_field_name_expected(struct ast_node* location);

void error_field_previously_defined_print(FILE* output, struct error_at_node* self);
extern struct error_vtable error_field_previously_defined_vtable;
error error_field_previously_defined(struct ast_node* location);

void error_field_undefined_print(FILE* output, struct error_at_node* self);
extern struct error_vtable error_field_undefined_vtable;
error error_field_undefined(struct ast_node* location);



void error_packing_scheme_expected_print(FILE* output, struct error_at_node* self);
extern struct error_vtable error_packing_scheme_expected_vtable;
error error_packing_scheme_expected(struct ast_node* location);

void error_stack_overflow_print(FILE* output, struct error_at_node* self);
extern struct error_vtable error_stack_overflow_vtable;
error error_stack_overflow(struct ast_node* location);

void error_eval_macro_print(FILE* output, struct error_at_node* self);
extern struct error_vtable error_eval_macro_vtable;
error error_eval_macro(struct ast_node* location);





struct error_function_cannot_return_type {
   struct error_vtable* vtable;
   struct ast_node* location;
   type return_type;
};

void error_function_cannot_return_type_print(FILE* output, struct error_function_cannot_return_type* self);
extern struct error_vtable error_function_cannot_return_type_vtable;
error error_function_cannot_return_type(struct ast_node* location, type return_type);


type_constructor_fn type_query_ast_node(struct inter* inter, struct ast_node* node, error* err);
type type_build_ast_node(struct inter* inter, struct ast_node* node, error* err);
struct type_vtable* type_get_function_vtable(struct ast_node* location, type return_type, error* err);

type type_construct_void(struct inter* inter, struct ast_node* root, error* err);
type type_construct_char(struct inter* inter, struct ast_node* root, error* err);
type type_construct_uchar(struct inter* inter, struct ast_node* root, error* err);
type type_construct_short(struct inter* inter, struct ast_node* root, error* err);
type type_construct_ushort(struct inter* inter, struct ast_node* root, error* err);
type type_construct_int(struct inter* inter, struct ast_node* root, error* err);
type type_construct_uint(struct inter* inter, struct ast_node* root, error* err);
type type_construct_long(struct inter* inter, struct ast_node* root, error* err);
type type_construct_ulong(struct inter* inter, struct ast_node* root, error* err);
type type_construct_long_long(struct inter* inter, struct ast_node* root, error* err);
type type_construct_ulong_long(struct inter* inter, struct ast_node* root, error* err);
type type_construct_float(struct inter* inter, struct ast_node* root, error* err);
type type_construct_double(struct inter* inter, struct ast_node* root, error* err);
type type_construct_long_double(struct inter* inter, struct ast_node* root, error* err);
type type_construct_pointer(struct inter* inter, struct ast_node* root, error* err);
type type_construct_string(struct inter* inter, struct ast_node* root, error* err);
type type_construct_string_auto(struct inter* inter, struct ast_node* root, error* err);
type type_construct_string_refc(struct inter* inter, struct ast_node* root, error* err);
type type_construct_symbol(struct inter* inter, struct ast_node* root, error* err);
type type_construct_type(struct inter* inter, struct ast_node* root, error* err);

type type_construct_pointer_to_(struct inter* inter, struct ast_node* root, error* err, struct type_vtable* vtable);
type type_construct_pointer_to(struct inter* inter, struct ast_node* root, error* err);
type type_construct_auto_pointer_to(struct inter* inter, struct ast_node* root, error* err);
type type_construct_refc_pointer_to(struct inter* inter, struct ast_node* root, error* err);
type type_construct_array(struct inter* inter, struct ast_node* root, error* err);
type type_construct_c_function(struct inter* inter, struct ast_node* root, error* err);
type type_construct_buf_function(struct inter* inter, struct ast_node* root, error* err);
type type_construct_c_macro(struct inter* inter, struct ast_node* root, error* err);
type type_construct_code(struct inter* inter, struct ast_node* root, error* err);
type type_construct_code_unsafe(struct inter* inter, struct ast_node* root, error* err);
type type_construct_quote(struct inter* inter, struct ast_node* root, error* err);

struct error_bits_too_wide {
   struct error_vtable* vtable;
   struct ast_node* location;
   int bits;
   int type_size;
   type type;
};

void error_bits_too_wide_print(FILE* output, struct error_bits_too_wide* self);
extern struct error_vtable error_bits_too_wide_vtable;
error error_bits_too_wide(struct ast_node* location, int bits, int type_size, type type);

type type_construct_struct(struct inter* inter, struct ast_node* root, error* err);
type type_construct_prefix(struct inter* inter, struct ast_node* root, error* err);
type type_construct_script_function(struct inter* inter, struct ast_node* root, error* err);
type type_construct_script_function_(struct inter* inter, struct ast_node* args_node, struct ast_node* ret_type_node, error* err);

type type_construct_macro(struct inter* inter, struct ast_node* root, error* err);
type type_construct_macro_(struct inter* inter, struct ast_node* args_node, error* err);
 


struct error_type_mismatch {
   struct error_vtable* vtable;
   struct ast_node* location;
   type expected_type;
   type actual_type;
};

void error_type_mismatch_print(FILE* output, struct error_type_mismatch* err);
extern struct error_vtable error_type_mismatch_vtable;
error error_type_mismatch(struct ast_node* location, type expected_type, type actual_type);
error check_root_type(struct ast_node_with_type* root, type expected_type);

struct error_cannot_call {
   struct error_vtable* vtable;
   struct ast_node* location;
   type actual_type;
};

void error_cannot_call_print(FILE* output, struct error_cannot_call* err);
extern struct error_vtable error_cannot_call_vtable;
error error_cannot_call(struct ast_node* location, type actual_type);

struct error_incorrect_num_args {
   struct error_vtable* vtable;
   struct ast_node* location;
   type first_type;
   int actual_num_args;
};
void error_incorrect_num_args_print(FILE* output, struct error_incorrect_num_args* err);
extern struct error_vtable error_incorrect_num_args_vtable;
error error_incorrect_num_args(struct ast_node* location, type first_type, int actual_num_args);


error inter_type_check_constant(struct inter* inter, type expected_type, struct ast_node_with_type* root);
error inter_type_check_expr(struct inter* inter, type expected_type, struct ast_node_with_type* root);
void type_check_trivial(struct ast_node_with_type* root);
error type_check_trivial_call(struct inter* inter, type expected_type, struct ast_node_with_type* root);


error inter_eval_constant_primitive_lval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_eval_constant_char_rval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_eval_constant_uchar_rval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_eval_constant_short_rval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_eval_constant_ushort_rval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_eval_constant_int_rval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_eval_constant_uint_rval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_eval_constant_long_rval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_eval_constant_ulong_rval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_eval_constant_long_long_rval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_eval_constant_ulong_long_rval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_eval_constant_float_rval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_eval_constant_double_rval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_eval_constant_long_double_rval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_eval_constant_pointer_rval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);

extern struct eval_table inter_constant_char_eval_table;
extern struct eval_table inter_constant_uchar_eval_table;
extern struct eval_table inter_constant_short_eval_table;
extern struct eval_table inter_constant_ushort_eval_table;
extern struct eval_table inter_constant_int_eval_table;
extern struct eval_table inter_constant_uint_eval_table;
extern struct eval_table inter_constant_long_eval_table;
extern struct eval_table inter_constant_ulong_eval_table;
extern struct eval_table inter_constant_long_long_eval_table;
extern struct eval_table inter_constant_ulong_long_eval_table;
extern struct eval_table inter_constant_float_eval_table;
extern struct eval_table inter_constant_double_eval_table;
extern struct eval_table inter_constant_long_double_eval_table;
extern struct eval_table inter_constant_pointer_eval_table;

error inter_eval_constant_rval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_eval_constant_lval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);

struct eval_table inter_constant_eval_table;

error inter_eval_symbol_rval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_eval_symbol_lval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);

extern struct eval_table inter_symbol_eval_table;

error inter_eval_local_var_rval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_eval_local_var_lval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
extern struct eval_table inter_local_var_eval_table;





error inter_eval_call_matching_val(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_eval_call_rval_to_lval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_eval_call_lval_to_rval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);

extern struct eval_table inter_call_rval_eval_table;
extern struct eval_table inter_call_lval_eval_table;

error inter_eval_fallback_lval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_eval_fallback_rval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);







#ifndef EVAL_MACRO
error inter_eval_rval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_eval_lval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
error inter_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size);
#else
#define inter_eval_rval(inter,root,value,size) \
   (root)->eval_table.eval_rval((inter),(root),(value),(size))

#define inter_eval_lval(inter,root,value,size) \
   (root)->eval_table.eval_lval((inter),(root),(value),(size))

#define inter_eval(inter,root,value,size) \
   (root)->returns_lval \
      ? (root)->eval_table.eval_lval((inter),(root),(value),(size)) \
      : (root)->eval_table.eval_rval((inter),(root),(value),(size))
#endif





void inter_def_builtin_types(struct inter* inter);
void inter_undef_builtin_types(struct inter* inter);


#endif
