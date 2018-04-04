#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <alloca.h>

#include "hash_table.h"
#include "hash_map_push.h"
#include "shift.h"
#include "parser.h"
#include "token.h"
#include "inter_type.h"
#include "list.h"
#include "refc.h"
#include "bits.h"


#define LOGGING_OFF
#include "log.h"

//#define LOG_TYPE_CHECK
//#define LOG_EVAL
//#define LOG_FUN_CALL
//#define LOG_LOCAL
//#define LOG_STACK
//#define LOG_MACRO

//#define DEBUG_SYMBOL "c.acosf"

#define TYPE_COPY_VAL(type, src, dest, size) \
   (*type)->type_copy_val(type, src, dest);




#ifdef DEBUG_SYMBOL



void debug_callback(struct key_value* key_val) {
   char* key = (char*)key_val->key;
   struct list* cell = (struct list*)key_val->value;
   key[length] = '\0';
   printf("%s = ", key);
   if (cell->value) {
      typed_value_print(stdout, (struct typed_value*)cell->value);
   }
   else {
      printf("nothing");
   }
   printf("\n");
}

#if 1
void debug_symbol(struct inter* inter) {
   struct typed_value* debug_value = hash_map_find_top_value(&inter->values_table, DEBUG_SYMBOL, NULL);
   LOG("inter_debug_sym: %s = ", DEBUG_SYMBOL)
   if (!debug_value) {
      LOG("undefined\n")
   }
   else {
      struct type_function* debug_type = (struct type_function*)debug_value->type;
      LOG(", %p, %d = [", debug_type->argument_types, debug_type->num_arguments)
      int i;
      for (i = 0; i < debug_type->num_arguments; i++) {
         LOG("%p ", debug_type->argument_types[i])
      }
      LOG("], ")
      typed_value_print(LOG_FILE, debug_value);
   }
   LOG("\n")
}
#elif 0
void debug_symbol(struct inter* inter) {
   hash_value_fn callback = (hash_value_fn)debug_callback;

   printf("begin dump:\n");
   hash_set_for_each(&inter->values_table, callback);
   printf("end dump:\n");
}
#endif


#endif





void error_malformed_constant_print(FILE* output, struct error_malformed_constant* self) {
   fprintf(output, "%s: %d.%d: Malformed constant for type ",
         self->location->filename, self->location->line, self->location->column);
   (*self->expected_type)->type_print(output, self->expected_type);
   fprintf(output, "\n");
}

struct error_vtable error_malformed_constant_vtable = {
   .error_print = (error_print_fn)error_malformed_constant_print,
   .error_free = (error_free_fn)free
};

error error_malformed_constant(struct ast_node* location, type expected_type) {
   struct error_malformed_constant* self = (struct error_malformed_constant*)malloc(sizeof(struct error_malformed_constant));
   assert(self);
   self->vtable = &error_malformed_constant_vtable;
   self->location = location;
   self->expected_type = expected_type;
   return (error)self;
}

void type_print_void(FILE* output, type self) {
   fprintf(output, "void");
}

int type_size_void(type self) {
   return 0;
}

void type_print_val_void(FILE* output, type self, void* val) {
   fprintf(output, "(void)");
}

void type_copy_val_void(type self, void* src, void* dest) {
}

void type_print_char(FILE* output, type self) {
   fprintf(output, "char");
}

int type_size_char(type self) {
   return sizeof(char);
}

void type_print_val_char(FILE* output, type self, char* val) {
   fprintf(output, "%c", *val);
}

void type_copy_val_char(type self, char* src, char* dest) {
   *dest = *src;
}

// Parse numerically.
error type_parse_char(type self, struct ast_node_with_type* node, char* value) {
   char* endptr;
   char* start = node->ast_node.text;
   int base = 10;

   if (node->ast_node.text_len == 0) {
      return error_symbol_undefined(&node->ast_node);
   }
   
   if (start[0] == '0') {
      if (node->ast_node.text_len > 1 && start[1] == 'x') {
         base = 16;
         start += 2;
      }
      else {
         base = 8;
         start += 1;
      }
   }

   *value = node->constant.c = (char)strtol(start, &endptr, base);
   if (endptr != node->ast_node.text + node->ast_node.text_len) {
      return error_symbol_undefined(&node->ast_node);
   }

   return NULL;
}


void type_print_uchar(FILE* output, type self) {
   fprintf(output, "uchar");
}

void type_copy_val_uchar(type self, unsigned char* src, unsigned char* dest) {
   *dest = *src;
}

// Parse numerically.
error type_parse_uchar(type self, struct ast_node_with_type* node, unsigned char* value) {
   char* endptr;
   char* start = node->ast_node.text;
   int base = 10;

   if (node->ast_node.text_len == 0) {
      return error_symbol_undefined(&node->ast_node);
   }
   
   if (start[0] == '0') {
      if (node->ast_node.text_len > 1 && start[1] == 'x') {
         base = 16;
         start += 2;
      }
      else {
         base = 8;
         start += 1;
      }
   }

   *value = node->constant.uc = (unsigned char)strtol(start, &endptr, base);
   if (endptr != node->ast_node.text + node->ast_node.text_len) {
      return error_symbol_undefined(&node->ast_node);
   }

   return NULL;
}






void type_print_short(FILE* output, type self) {
   fprintf(output, "short");
}

int type_size_short(type self) {
   return sizeof(short);
}

void type_print_val_short(FILE* output, type self, short* val) {
   fprintf(output, "%hd", *val);
}

void type_copy_val_short(type self, short* src, short* dest) {
   *dest = *src;
}

error type_parse_short(type self, struct ast_node_with_type* node, short* value) {
   char* endptr;
   char* start = node->ast_node.text;
   int base = 10;

   if (node->ast_node.text_len == 0) {
      return error_symbol_undefined(&node->ast_node);
   }
   
   if (start[0] == '0') {
      if (node->ast_node.text_len > 1 && start[1] == 'x') {
         base = 16;
         start += 2;
      }
      else {
         base = 8;
         start += 1;
      }
   }

   *value = node->constant.s = (short)strtol(start, &endptr, base);
   if (endptr != node->ast_node.text + node->ast_node.text_len) {
      return error_symbol_undefined(&node->ast_node);
   }

   return NULL;
}

void type_print_ushort(FILE* output, type self) {
   fprintf(output, "ushort");
}

void type_print_val_ushort(FILE* output, type self, unsigned short* val) {
   fprintf(output, "%hu", *val);
}

void type_copy_val_ushort(type self, unsigned short* src, unsigned short* dest) {
   *dest = *src;
}

error type_parse_ushort(type self, struct ast_node_with_type* node, short* value) {
   char* endptr;
   char* start = node->ast_node.text;
   int base = 10;

   if (node->ast_node.text_len == 0) {
      return error_symbol_undefined(&node->ast_node);
   }
   
   if (start[0] == '0') {
      if (node->ast_node.text_len > 1 && start[1] == 'x') {
         base = 16;
         start += 2;
      }
      else {
         base = 8;
         start += 1;
      }
   }

   *value = node->constant.us = (unsigned short)strtoul(start, &endptr, base);
   if (endptr != node->ast_node.text + node->ast_node.text_len) {
      return error_symbol_undefined(&node->ast_node);
   }

   return NULL;
}




void type_print_int(FILE* output, type self) {
   fprintf(output, "int");
}

int type_size_int(type self) {
   return sizeof(int);
}

void type_print_val_int(FILE* output, type self, int* val) {
   fprintf(output, "%d", *val);
}

void type_copy_val_int(type self, int* src, int* dest) {
   *dest = *src;
}

error type_parse_int(type self, struct ast_node_with_type* node, int* value) {
   char* endptr;
   char* start = node->ast_node.text;
   int base = 10;

   if (node->ast_node.text_len == 0) {
      return error_symbol_undefined(&node->ast_node);
   }
   
   if (start[0] == '0') {
      if (node->ast_node.text_len > 1 && start[1] == 'x') {
         base = 16;
         start += 2;
      }
      else {
         base = 8;
         start += 1;
      }
   }

   *value = node->constant.i = (int)strtol(start, &endptr, base);
   if (endptr != node->ast_node.text + node->ast_node.text_len) {
      return error_symbol_undefined(&node->ast_node);
   }

   return NULL;
}

void type_print_uint(FILE* output, type self) {
   fprintf(output, "uint");
}

void type_print_val_uint(FILE* output, type self, unsigned int* val) {
   fprintf(output, "%u", *val);
}

void type_copy_val_uint(type self, unsigned int* src, unsigned int* dest) {
   *dest = *src;
}

error type_parse_uint(type self, struct ast_node_with_type* node, unsigned int* value) {
   char* endptr;
   char* start = node->ast_node.text;
   int base = 10;

   if (node->ast_node.text_len == 0) {
      return error_symbol_undefined(&node->ast_node);
   }
   
   if (start[0] == '0') {
      if (node->ast_node.text_len > 1 && start[1] == 'x') {
         base = 16;
         start += 2;
      }
      else {
         base = 8;
         start += 1;
      }
   }

   *value = node->constant.u = (unsigned int)strtoul(start, &endptr, base);
   if (endptr != node->ast_node.text + node->ast_node.text_len) {
      return error_symbol_undefined(&node->ast_node);
   }

   return NULL;
}





void type_print_long(FILE* output, type self) {
   fprintf(output, "long");
}

int type_size_long(type self) {
   return sizeof(long);
}

void type_print_val_long(FILE* output, type self, long* val) {
   fprintf(output, "%ld", *val);
}
 
void type_copy_val_long(type self, long* src, long* dest) {
   *dest = *src;
}

error type_parse_long(type self, struct ast_node_with_type* node, long* value) {
   char* endptr;
   char* start = node->ast_node.text;
   int base = 10;

   if (node->ast_node.text_len == 0) {
      return error_symbol_undefined(&node->ast_node);
   }
   
   if (start[0] == '0') {
      if (node->ast_node.text_len > 1 && start[1] == 'x') {
         base = 16;
         start += 2;
      }
      else {
         base = 8;
         start += 1;
      }
   }

   *value = node->constant.l = strtol(start, &endptr, base);
   if (endptr != node->ast_node.text + node->ast_node.text_len) {
      return error_symbol_undefined(&node->ast_node);
   }

   return NULL;
}

void type_print_ulong(FILE* output, type self) {
   fprintf(output, "ulong");
}

void type_print_val_ulong(FILE* output, type self, unsigned long* val) {
   fprintf(output, "%lu", *val);
}

void type_copy_val_ulong(type self, unsigned long* src, unsigned long* dest) {
   *dest = *src;
}

error type_parse_ulong(type self, struct ast_node_with_type* node, unsigned long* value) {
   char* endptr;
   char* start = node->ast_node.text;
   int base = 10;

   if (node->ast_node.text_len == 0) {
      return error_symbol_undefined(&node->ast_node);
   }
   
   if (start[0] == '0') {
      if (node->ast_node.text_len > 1 && start[1] == 'x') {
         base = 16;
         start += 2;
      }
      else {
         base = 8;
         start += 1;
      }
   }

   *value = node->constant.ul = strtoul(start, &endptr, base);
   if (endptr != node->ast_node.text + node->ast_node.text_len) {
      return error_symbol_undefined(&node->ast_node);
   }

   return NULL;
}




void type_print_long_long(FILE* output, type self) {
   fprintf(output, "longlong");
}

int type_size_long_long(type self) {
   return sizeof(long long);
}

void type_print_val_long_long(FILE* output, type self, long long* val) {
   fprintf(output, "%lld", *val);
}

void type_copy_val_long_long(type self, long long* src, long long* dest) {
   *dest = *src;
}

error type_parse_long_long(type self, struct ast_node_with_type* node, long long* value) {
   char* endptr;
   char* start = node->ast_node.text;
   int base = 10;

   if (node->ast_node.text_len == 0) {
      return error_symbol_undefined(&node->ast_node);
   }
   
   if (start[0] == '0') {
      if (node->ast_node.text_len > 1 && start[1] == 'x') {
         base = 16;
         start += 2;
      }
      else {
         base = 8;
         start += 1;
      }
   }

   *value = node->constant.ll = strtoll(start, &endptr, base);
   if (endptr != node->ast_node.text + node->ast_node.text_len) {
      return error_symbol_undefined(&node->ast_node);
   }

   return NULL;
}


void type_print_ulong_long(FILE* output, type self) {
   fprintf(output, "ulonglong");
}

void type_print_val_ulong_long(FILE* output, type self, unsigned long long* val) {
   fprintf(output, "%llu", *val);
}

void type_copy_val_ulong_long(type self, unsigned long long* src, unsigned long long* dest) {
   *dest = *src;
}

error type_parse_ulong_long(type self, struct ast_node_with_type* node, unsigned long long* value) {
   char* endptr;
   char* start = node->ast_node.text;
   int base = 10;

   if (node->ast_node.text_len == 0) {
      return error_symbol_undefined(&node->ast_node);
   }
   
   if (start[0] == '0') {
      if (node->ast_node.text_len > 1 && start[1] == 'x') {
         base = 16;
         start += 2;
      }
      else {
         base = 8;
         start += 1;
      }
   }

   *value = node->constant.ull = strtoull(start, &endptr, base);
   if (endptr != node->ast_node.text + node->ast_node.text_len) {
      return error_symbol_undefined(&node->ast_node);
   }

   return NULL;
}




void type_print_float(FILE* output, type self) {
   fprintf(output, "float");
}

int type_size_float(type self) {
   return sizeof(float);
}

void type_print_val_float(FILE* output, type self, float* val) {
   fprintf(output, "%f", *val);
}

void type_copy_val_float(type self, float* src, float* dest) {
   *dest = *src;
}

error type_parse_float(type self, struct ast_node_with_type* node, float* value) {
   char* endptr;
   *value = node->constant.f = strtof(node->ast_node.text, &endptr);
   if (endptr != node->ast_node.text + node->ast_node.text_len)
      return error_symbol_undefined(&node->ast_node);

   return NULL;
}


void type_print_double(FILE* output, type self) {
   fprintf(output, "double");
}

int type_size_double(type self) {
   return sizeof(double);
}

void type_print_val_double(FILE* output, type self, double* val) {
   fprintf(output, "%lf", *val);
}

void type_copy_val_double(type self, double* src, double* dest) {
   *dest = *src;
}

error type_parse_double(type self, struct ast_node_with_type* node, double* value) {
   char* endptr;
   *value = node->constant.d = strtod(node->ast_node.text, &endptr);
   if (endptr != node->ast_node.text + node->ast_node.text_len) {
      return error_symbol_undefined(&node->ast_node);
   }

   return NULL;
}


void type_print_long_double(FILE* output, type self) {
   fprintf(output, "longdouble");
}

int type_size_long_double(type self) {
   return sizeof(long double);
}

void type_print_val_long_double(FILE* output, type self, long double* val) {
   fprintf(output, "%llf", *val);
}

void type_copy_val_long_double(type self, long double* src, long double* dest) {
   *dest = *src;
}

error type_parse_long_double(type self, struct ast_node_with_type* node, long double* value) {
   char* endptr;
   *value = node->constant.ld = strtold(node->ast_node.text, &endptr);
   if (endptr != node->ast_node.text + node->ast_node.text_len) {
      return error_symbol_undefined(&node->ast_node);
   }

   return NULL;
}


void type_print_pointer(FILE* output, type self) {
   fprintf(output, "void*");
}

int type_match_pointer(type self, type other) {
   return *other == &type_pointer_vtable ||
          *other == &type_string_vtable ||
          *other == &type_pointer_to_vtable;
}

int type_size_pointer(type self) {
   return sizeof(void*);
}

void type_print_val_pointer(FILE* output, type self, void** val) {
   fprintf(output, "%p", *val);
}

void type_copy_val_pointer(type self, void** src, void** dest) {
   *dest = *src;
}

void type_copy_val_refc(type self, void** src, void** dest) {
   if (*src)
      *dest = refc_copy(*src);
   else
      *dest = *src;
}

void type_destruct_val_free(type self, void** val, struct inter* inter) {
   if (*val)
      free(*val);
}

void type_destruct_val_refc(type self, void** val, struct inter* inter) {
   if (*val)
      refc_dec_free(*val);
}

error type_parse_pointer(type self, struct ast_node_with_type* node, void** value) {
   char* endptr;
   char* start = node->ast_node.text;
   int base = 10;

   if (node->ast_node.text_len == 0) {
      return error_symbol_undefined(&node->ast_node);
   }
   
   if (start[0] == '0') {
      if (node->ast_node.text_len > 1 && start[1] == 'x') {
         base = 16;
         start += 2;
      }
      else {
         base = 8;
         start += 1;
      }
   }

   *value = node->constant.p = (void*)strtol(start, &endptr, base);
   if (endptr != node->ast_node.text + node->ast_node.text_len) {
      return error_symbol_undefined(&node->ast_node);
   }

   return NULL;
}

void type_print_string(FILE* output, type self) {
   fprintf(output, "str");
}

void type_print_string_auto(FILE* output, type self) {
   fprintf(output, "str-auto");
}

void type_print_string_refc(FILE* output, type self) {
   fprintf(output, "str-refc");
}

void type_print_symbol(FILE* output, type self) {
   fprintf(output, "symbol");
}


void type_print_val_string(FILE* output, type self, char** val) {
   if (*val) {
      fprintf(output, "%s", *val);
   }
   else {
      fprintf(output, "NULL");
   }
}

void type_copy_val_strdup(type self, char** src, char** dest) {
   if (*src) 
      *dest = strdup(*src);
   else
      *dest = *src;
}

void type_destruct_val_symbol(type self, char** val, struct inter* inter) {
   if (*val) {
      inter_release_sym(inter, *val);
   }
}


error type_string_check_call(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   error err;
   root->type = &type_char;
   root->returns_lval = 1;

   err = check_root_type(root, expected_type);
   if (err) return err;

   int num_children = dlist_root_length(&root->ast_node.children);

   if (num_children == 1) {
      return NULL;
   }
   else if (num_children == 2) {
      struct ast_node_with_type* index_node = (struct ast_node_with_type*)dlist_root_nth(&root->ast_node.children, 1);
      return inter_type_check_expr(inter, &type_int, index_node);
   }
   else {
      return error_wrong_number_of_args(&root->ast_node, 1, num_children - 1);
   }
}

error type_string_eval_call(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   error err = NULL;

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;

   assert(first != NULL);

   struct ast_node_with_type* second = (struct ast_node_with_type*)first->ast_node.node.next;

   char* str;
   err = inter_eval_rval(inter, first, &str, sizeof(char*));
   if (err) goto done;

   if (second == first) {
      *((char**)value) = str;
   }
   else {
      int index;
      err = inter_eval_rval(inter, second, &index, sizeof(int));
      if (err) goto destruct_str;

      *((char**)value) = str + index;
   }

destruct_str:
   inter_destruct_val(inter, first->type, &str);
done:
   return err;
}





struct type_vtable type_void_vtable = {
   .type_free = type_free_nothing,
   .type_copy = type_copy_static,
   .type_print = type_print_void,
   .type_match = type_match_static,
   .type_size = type_size_void,
   .type_print_val = (type_print_val_fn)type_print_val_void,
   .type_copy_val = (type_copy_val_fn)type_copy_val_void,
   .type_destruct_val = NULL,
   .type_parse_constant = (type_parse_constant_fn)NULL,
   .type_check_call = NULL,
   .type_eval_call = NULL
};

struct type_vtable type_char_vtable = {
   .type_free = type_free_nothing,
   .type_copy = type_copy_static,
   .type_print = type_print_char,
   .type_match = type_match_static,
   .type_size = type_size_char,
   .type_print_val = (type_print_val_fn)type_print_val_char,
   .type_copy_val = (type_copy_val_fn)type_copy_val_char,
   .type_destruct_val = NULL,
   .type_parse_constant = (type_parse_constant_fn)type_parse_char,
   .type_check_call = NULL,
   .type_eval_call = NULL
};

struct type_vtable type_uchar_vtable = {
   .type_free = type_free_nothing,
   .type_copy = type_copy_static,
   .type_print = type_print_uchar,
   .type_match = type_match_static,
   .type_size = type_size_char,
   .type_print_val = (type_print_val_fn)type_print_val_char,
   .type_copy_val = (type_copy_val_fn)type_copy_val_uchar,
   .type_destruct_val = NULL,
   .type_parse_constant = (type_parse_constant_fn)type_parse_uchar,
   .type_check_call = NULL,
   .type_eval_call = NULL
};

struct type_vtable type_short_vtable = {
   .type_free = type_free_nothing,
   .type_copy = type_copy_static,
   .type_print = type_print_short,
   .type_match = type_match_static,
   .type_size = type_size_short,
   .type_print_val = (type_print_val_fn)type_print_val_short,
   .type_copy_val = (type_copy_val_fn)type_copy_val_short,
   .type_destruct_val = NULL,
   .type_parse_constant = (type_parse_constant_fn)type_parse_short,
   .type_check_call = NULL,
   .type_eval_call = NULL
};

struct type_vtable type_ushort_vtable = {
   .type_free = type_free_nothing,
   .type_copy = type_copy_static,
   .type_print = type_print_ushort,
   .type_match = type_match_static,
   .type_size = type_size_short,
   .type_print_val = (type_print_val_fn)type_print_val_ushort,
   .type_copy_val = (type_copy_val_fn)type_copy_val_ushort,
   .type_destruct_val = NULL,
   .type_parse_constant = (type_parse_constant_fn)type_parse_ushort,
   .type_check_call = NULL,
   .type_eval_call = NULL
};

struct type_vtable type_int_vtable = {
   .type_free = type_free_nothing,
   .type_copy = type_copy_static,
   .type_print = type_print_int,
   .type_match = type_match_static,
   .type_size = type_size_int,
   .type_print_val = (type_print_val_fn)type_print_val_int,
   .type_copy_val = (type_copy_val_fn)type_copy_val_int,
   .type_destruct_val = NULL,
   .type_parse_constant = (type_parse_constant_fn)type_parse_int,
   .type_check_call = NULL,
   .type_eval_call = NULL
};

struct type_vtable type_uint_vtable = {
   .type_free = type_free_nothing,
   .type_copy = type_copy_static,
   .type_print = type_print_uint,
   .type_match = type_match_static,
   .type_size = type_size_int,
   .type_print_val = (type_print_val_fn)type_print_val_uint,
   .type_copy_val = (type_copy_val_fn)type_copy_val_uint,
   .type_destruct_val = NULL,
   .type_parse_constant = (type_parse_constant_fn)type_parse_uint,
   .type_check_call = NULL,
   .type_eval_call = NULL
};

struct type_vtable type_long_vtable = {
   .type_free = type_free_nothing,
   .type_copy = type_copy_static,
   .type_print = type_print_long,
   .type_match = type_match_static,
   .type_size = type_size_long,
   .type_print_val = (type_print_val_fn)type_print_val_long,
   .type_copy_val = (type_copy_val_fn)type_copy_val_long,
   .type_destruct_val = NULL,
   .type_parse_constant = (type_parse_constant_fn)type_parse_long,
   .type_check_call = NULL,
   .type_eval_call = NULL
};

struct type_vtable type_ulong_vtable = {
   .type_free = type_free_nothing,
   .type_copy = type_copy_static,
   .type_print = type_print_ulong,
   .type_match = type_match_static,
   .type_size = type_size_long,
   .type_print_val = (type_print_val_fn)type_print_val_ulong,
   .type_copy_val = (type_copy_val_fn)type_copy_val_ulong,
   .type_destruct_val = NULL,
   .type_parse_constant = (type_parse_constant_fn)type_parse_ulong,
   .type_check_call = NULL,
   .type_eval_call = NULL
};

struct type_vtable type_long_long_vtable = {
   .type_free = type_free_nothing,
   .type_copy = type_copy_static,
   .type_print = type_print_long_long,
   .type_match = type_match_static,
   .type_size = type_size_long_long,
   .type_print_val = (type_print_val_fn)type_print_val_long_long,
   .type_copy_val = (type_copy_val_fn)type_copy_val_long_long,
   .type_destruct_val = NULL,
   .type_parse_constant = (type_parse_constant_fn)type_parse_long_long,
   .type_check_call = NULL,
   .type_eval_call = NULL
};

struct type_vtable type_ulong_long_vtable = {
   .type_free = type_free_nothing,
   .type_copy = type_copy_static,
   .type_print = type_print_ulong_long,
   .type_match = type_match_static,
   .type_size = type_size_long_long,
   .type_print_val = (type_print_val_fn)type_print_val_ulong_long,
   .type_copy_val = (type_copy_val_fn)type_copy_val_ulong_long,
   .type_destruct_val = NULL,
   .type_parse_constant = (type_parse_constant_fn)type_parse_ulong_long,
   .type_check_call = NULL,
   .type_eval_call = NULL
};

struct type_vtable type_float_vtable = {
   .type_free = type_free_nothing,
   .type_copy = type_copy_static,
   .type_print = type_print_float,
   .type_match = type_match_static,
   .type_size = type_size_float,
   .type_print_val = (type_print_val_fn)type_print_val_float,
   .type_copy_val = (type_copy_val_fn)type_copy_val_float,
   .type_destruct_val = NULL,
   .type_parse_constant = (type_parse_constant_fn)type_parse_float,
   .type_check_call = NULL,
   .type_eval_call = NULL
};

struct type_vtable type_double_vtable = {
   .type_free = type_free_nothing,
   .type_copy = type_copy_static,
   .type_print = type_print_double,
   .type_match = type_match_static,
   .type_size = type_size_double,
   .type_print_val = (type_print_val_fn)type_print_val_double,
   .type_copy_val = (type_copy_val_fn)type_copy_val_double,
   .type_destruct_val = NULL,
   .type_parse_constant = (type_parse_constant_fn)type_parse_double,
   .type_check_call = NULL,
   .type_eval_call = NULL
};

struct type_vtable type_long_double_vtable = {
   .type_free = type_free_nothing,
   .type_copy = type_copy_static,
   .type_print = type_print_long_double,
   .type_match = type_match_static,
   .type_size = type_size_long_double,
   .type_print_val = (type_print_val_fn)type_print_val_long_double,
   .type_copy_val = (type_copy_val_fn)type_copy_val_long_double,
   .type_destruct_val = NULL,
   .type_parse_constant = (type_parse_constant_fn)type_parse_long_double,
   .type_check_call = NULL,
   .type_eval_call = NULL
};

struct type_vtable type_pointer_vtable = {
   .type_free = type_free_nothing,
   .type_copy = type_copy_static,
   .type_print = type_print_pointer,
   .type_match = type_match_pointer,
   .type_size = type_size_pointer,
   .type_print_val = (type_print_val_fn)type_print_val_pointer,
   .type_copy_val = (type_copy_val_fn)type_copy_val_pointer,
   .type_destruct_val = NULL,
   .type_parse_constant = (type_parse_constant_fn)type_parse_pointer,
   .type_check_call = NULL,
   .type_eval_call = NULL
};

struct type_vtable type_string_vtable = {
   .type_free = type_free_nothing,
   .type_copy = type_copy_static,
   .type_print = type_print_string,
   .type_match = type_match_static,
   .type_size = type_size_pointer,
   .type_print_val = (type_print_val_fn)type_print_val_string,
   .type_copy_val = (type_copy_val_fn)type_copy_val_pointer,
   .type_destruct_val = NULL,
   .type_parse_constant = (type_parse_constant_fn)NULL,
   .type_check_call = (type_check_call_fn)type_string_check_call,
   .type_eval_call = (type_eval_call_fn)type_string_eval_call 
};

struct type_vtable type_string_auto_vtable = {
   .type_free = type_free_nothing,
   .type_copy = type_copy_static,
   .type_print = type_print_string_auto,
   .type_match = type_match_static,
   .type_size = type_size_pointer,
   .type_print_val = (type_print_val_fn)type_print_val_string,
   .type_copy_val = (type_copy_val_fn)type_copy_val_strdup,
   .type_destruct_val = (type_destruct_val_fn)type_destruct_val_free,
   .type_parse_constant = (type_parse_constant_fn)NULL,
   .type_check_call = (type_check_call_fn)type_string_check_call,
   .type_eval_call = (type_eval_call_fn)type_string_eval_call 
};

struct type_vtable type_string_refc_vtable = {
   .type_free = type_free_nothing,
   .type_copy = type_copy_static,
   .type_print = type_print_string_refc,
   .type_match = type_match_static,
   .type_size = type_size_pointer,
   .type_print_val = (type_print_val_fn)type_print_val_string,
   .type_copy_val = (type_copy_val_fn)type_copy_val_refc,
   .type_destruct_val = (type_destruct_val_fn)type_destruct_val_refc,
   .type_parse_constant = (type_parse_constant_fn)NULL,
   .type_check_call = (type_check_call_fn)type_string_check_call,
   .type_eval_call = (type_eval_call_fn)type_string_eval_call 
};

struct type_vtable type_symbol_vtable = {
   .type_free = type_free_nothing,
   .type_copy = type_copy_static,
   .type_print = type_print_symbol,
   .type_match = type_match_static,
   .type_size = type_size_pointer,
   .type_print_val = (type_print_val_fn)type_print_val_string,
   .type_copy_val = (type_copy_val_fn)type_copy_val_refc,
   .type_destruct_val = (type_destruct_val_fn)type_destruct_val_symbol,
   .type_parse_constant = (type_parse_constant_fn)NULL,
   .type_check_call = (type_check_call_fn)type_string_check_call,
   .type_eval_call = (type_eval_call_fn)type_string_eval_call 
};


struct type_vtable* type_void = &type_void_vtable;
struct type_vtable* type_char = &type_char_vtable;
struct type_vtable* type_uchar = &type_uchar_vtable;
struct type_vtable* type_short = &type_short_vtable;
struct type_vtable* type_ushort = &type_ushort_vtable;
struct type_vtable* type_int = &type_int_vtable;
struct type_vtable* type_uint = &type_uint_vtable;
struct type_vtable* type_long = &type_long_vtable;
struct type_vtable* type_ulong = &type_ulong_vtable;
struct type_vtable* type_long_long = &type_long_long_vtable;
struct type_vtable* type_ulong_long = &type_ulong_long_vtable;
struct type_vtable* type_float = &type_float_vtable;
struct type_vtable* type_double = &type_double_vtable;
struct type_vtable* type_long_double = &type_long_double_vtable;
struct type_vtable* type_pointer = &type_pointer_vtable;
struct type_vtable* type_string = &type_string_vtable;
struct type_vtable* type_string_auto = &type_string_auto_vtable;
struct type_vtable* type_string_refc = &type_string_refc_vtable;
struct type_vtable* type_symbol = &type_symbol_vtable;


void type_type_print(FILE* output, type self) {
   fprintf(output, "type");
}

void type_type_print_val(FILE* output, type self, type* val) {
   if (*val) {
      (**val)->type_print(output, *val);
   }
   else {
      fprintf(output, "NULL");
   }
}

void type_type_copy_val(type self, type* src, type* dest) {
   if (*src) {
      *dest = (**src)->type_copy(*src);
   }
   else {
      *dest = *src;
   }
}

void type_type_destruct_val(type self, type* val, struct inter* inter) {
   if (*val) {
      (**val)->type_free(*val, inter);
   }
}


struct type_vtable type_type_vtable = {
   .type_free = type_free_nothing,
   .type_copy = type_copy_static,
   .type_print = type_type_print,
   .type_match = type_match_static,
   .type_size = type_size_pointer,
   .type_print_val = (type_print_val_fn)type_type_print_val,
   .type_copy_val = (type_copy_val_fn)type_type_copy_val,
   .type_destruct_val = (type_destruct_val_fn)type_type_destruct_val,
   .type_parse_constant = (type_parse_constant_fn)NULL,
   .type_check_call = (type_check_call_fn)NULL,
   .type_eval_call = (type_eval_call_fn)NULL
};

struct type_vtable* type_type = &type_type_vtable;



void type_pointer_to_free(struct type_pointer_to* self, struct inter* inter) {
   if (!refc_dec(self)) {
      (*(self->content_type))->type_free(self->content_type, inter);
      refc_free(self);
   }
}

void type_pointer_to_print(FILE* output, struct type_pointer_to* self) {
   fprintf(output, "(* ");
   (*(self->content_type))->type_print(output, self->content_type);
   fprintf(output, ")");
}

void type_auto_pointer_to_print(FILE* output, struct type_pointer_to* self) {
   fprintf(output, "(auto* ");
   (*(self->content_type))->type_print(output, self->content_type);
   fprintf(output, ")");
}

void type_refc_pointer_to_print(FILE* output, struct type_pointer_to* self) {
   fprintf(output, "(refc* ");
   (*(self->content_type))->type_print(output, self->content_type);
   fprintf(output, ")");
}


int type_pointer_to_match(struct type_pointer_to* self, struct type_pointer_to* other) {
   if (self->vtable != other->vtable)
      return 0;

   error err = NULL;
   type self_type;
   type other_type;

   TYPE_DEF_CELL_GET_TYPE(self->content_type, NULL, self_type, err)
   if (err) {
      assert(0);
      free(err);
      return 0;
   }

   TYPE_DEF_CELL_GET_TYPE(other->content_type, NULL, other_type, err)
   if (err) {
      assert(0);
      free(err);
      return 0;
   }

   assert(self_type);
   assert(other_type);

   return (*self_type)->type_match(self_type, other_type);
}

void type_auto_pointer_to_copy_val(struct type_pointer_to* self, void** src, void** dest) {
   if (src == dest) {
   } else if (*src) {
      int size = (*self->content_type)->type_size(self->content_type);
      *dest = malloc(size);
      assert(*dest);

      type content_type;
      error err = NULL;
      TYPE_DEF_CELL_GET_TYPE(self->content_type, NULL, content_type, err)
      assert(!err);

      TYPE_COPY_VAL(content_type, *src, *dest, size)
   }
   else {
      *dest = *src;
   }
}

void type_auto_pointer_to_destruct_val(struct type_pointer_to* self, void** val, struct inter* inter) {
   if (*val) {
      inter_destruct_val2(inter, self->content_type, *val);
      free(*val);
   }
}

void type_refc_pointer_to_destruct_val(struct type_pointer_to* self, void** val, struct inter* inter) {
   if (*val) {
      if (!refc_dec(*val)) {
         inter_destruct_val2(inter, self->content_type, *val);
         refc_free(*val);
      }
   }
}


error type_pointer_to_check_call(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   error err;
   root->type = NULL;
   root->returns_lval = 1;


   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   assert(first);
   struct type_pointer_to* first_type = (struct type_pointer_to*)first->type;

   root->type = (*first_type->content_type)->type_copy(first_type->content_type);

   err = type_def_cell_bind_type(inter, root);
   if (err) return err;

   err = check_root_type(root, expected_type);
   if (err) return err;

   int num_children = dlist_root_length(&root->ast_node.children);

   if (num_children == 1) {
      return NULL;
   }
   else if (num_children == 2) {
      struct ast_node_with_type* index_node = (struct ast_node_with_type*)dlist_root_nth(&root->ast_node.children, 1);
      return inter_type_check_expr(inter, &type_int, index_node);
   }
   else {
      return error_wrong_number_of_args(&root->ast_node, 1, num_children - 1);
   }
}

error type_pointer_to_eval_call(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   error err = NULL;

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   assert(first != NULL);
   struct type_pointer_to* first_type = (struct type_pointer_to*)first->type;
   type content_type;

   TYPE_DEF_CELL_GET_TYPE(first_type->content_type, &root->ast_node, content_type, err)
   if (err) return err;

   int elt_size = (*content_type)->type_size(content_type);

   struct ast_node_with_type* second = (struct ast_node_with_type*)first->ast_node.node.next;

   int stack_top = inter->stack.stack_top;
   char** ptr;
   err = inter_eval_lval(inter, first, &ptr, sizeof(char*));
   if (!err) {
      if (second == first) {
         *((void**)value) = (void*)(*ptr);
      }
      else {
         int index;
         err = inter_eval_rval(inter, second, &index, sizeof(int));
         if (!err) {
            *((void**)value) = (void*)((*ptr) + index*elt_size);
         }
      }

      if (!first->returns_lval) {
         inter_destruct_val(inter, first->type, ptr);
      }
   }

   inter->stack.stack_top = stack_top;
   return err;
}


struct type_vtable type_pointer_to_vtable = {
   .type_free = (type_free_fn)type_pointer_to_free,
   .type_copy = (type_copy_fn)refc_copy,
   .type_print = (type_print_fn)type_pointer_to_print,
   .type_match = (type_match_fn)type_pointer_to_match,
   .type_size = type_size_pointer,
   .type_print_val = (type_print_val_fn)type_print_val_pointer,
   .type_copy_val = (type_copy_val_fn)type_copy_val_pointer,
   .type_destruct_val = NULL,
   .type_parse_constant = (type_parse_constant_fn)type_parse_pointer,
   .type_check_call = (type_check_call_fn)type_pointer_to_check_call,
   .type_eval_call = (type_eval_call_fn)type_pointer_to_eval_call
};

struct type_vtable type_auto_pointer_to_vtable = {
   .type_free = (type_free_fn)type_pointer_to_free,
   .type_copy = (type_copy_fn)refc_copy,
   .type_print = (type_print_fn)type_auto_pointer_to_print,
   .type_match = (type_match_fn)type_pointer_to_match,
   .type_size = type_size_pointer,
   .type_print_val = (type_print_val_fn)type_print_val_pointer,
   .type_copy_val = (type_copy_val_fn)type_auto_pointer_to_copy_val,
   .type_destruct_val = (type_destruct_val_fn)type_auto_pointer_to_destruct_val,
   .type_parse_constant = (type_parse_constant_fn)type_parse_pointer,
   .type_check_call = (type_check_call_fn)type_pointer_to_check_call,
   .type_eval_call = (type_eval_call_fn)type_pointer_to_eval_call
};

struct type_vtable type_refc_pointer_to_vtable = {
   .type_free = (type_free_fn)type_pointer_to_free,
   .type_copy = (type_copy_fn)refc_copy,
   .type_print = (type_print_fn)type_refc_pointer_to_print,
   .type_match = (type_match_fn)type_pointer_to_match,
   .type_size = type_size_pointer,
   .type_print_val = (type_print_val_fn)type_print_val_pointer,
   .type_copy_val = (type_copy_val_fn)type_copy_val_refc,
   .type_destruct_val = (type_destruct_val_fn)type_refc_pointer_to_destruct_val,
   .type_parse_constant = (type_parse_constant_fn)type_parse_pointer,
   .type_check_call = (type_check_call_fn)type_pointer_to_check_call,
   .type_eval_call = (type_eval_call_fn)type_pointer_to_eval_call
};



type type_pointer_to(struct type_vtable* vtable, type content_type) {
   struct type_pointer_to* self = (struct type_pointer_to*)refc_alloc(sizeof(struct type_pointer_to));
   assert(self);
   self->vtable = vtable;
   self->content_type = content_type;
   return (type)self;
}


int type_is_pointer(type self) {
   return *self == &type_pointer_vtable ||
          *self == &type_string_vtable ||
          *self == &type_string_auto_vtable ||
          *self == &type_string_refc_vtable ||
          *self == &type_pointer_to_vtable ||
          *self == &type_auto_pointer_to_vtable ||
          *self == &type_refc_pointer_to_vtable;
}

void type_array_free(struct type_array* self, struct inter* inter) {
   if (!refc_dec(self)) {
      (*(self->content_type))->type_free(self->content_type, inter);
      refc_free(self);
   }
}

void type_array_print(FILE* output, struct type_array* self) {
   fprintf(output, "(array %d ", self->length);
   (*(self->content_type))->type_print(output, self->content_type);
   fprintf(output, ")");
}

int type_array_match(struct type_array* self, struct type_array* other) {
   return self->vtable == other->vtable &&
          self->length == other->length &&
          (*(self->content_type))->type_match(self->content_type, other->content_type);
}

int type_array_size(struct type_array* self) {
   return self->length*(*self->content_type)->type_size(self->content_type);
}

void type_array_print_val(FILE* output, struct type_array* self, void* value) {
   int i;
   int elt_size = (*self->content_type)->type_size(self->content_type);

   fprintf(output, "[");
   for (i = 0; i < self->length; i++) {
      if (i > 0) {
         fprintf(output, " ");
      }
      void* elt = ((char*)value) + elt_size*i;
      (*self->content_type)->type_print_val(output, self->content_type, elt);
   }
   fprintf(output, "]");
}

void type_array_copy_val(struct type_array* self, char* src, char* dest) {
   int i;

   if (src == dest) {
      return;
   }

   int content_size = (*self->content_type)->type_size(self->content_type);

   type content_type;
   error err = NULL;
   TYPE_DEF_CELL_GET_TYPE(self->content_type, NULL, content_type, err)
   assert(!err);

   for (i = 0; i < self->length; i++) {
      int offset = i * content_size;
      TYPE_COPY_VAL(content_type, src + offset, dest + offset, content_size)
   }
}

void type_array_destruct_val(struct type_array* self, char* val, struct inter* inter) {
   type content_type;
   error err = NULL;
   TYPE_DEF_CELL_GET_TYPE(self->content_type, NULL, content_type, err)
   assert(!err);

   if ((*content_type)->type_destruct_val) {
      int i;
      int content_size = (*content_type)->type_size(content_type);

      for (i = 0; i < self->length; i++) {
         int offset = i * content_size;
         (*content_type)->type_destruct_val(content_type, val + offset, inter);
      }
   }
}


error type_array_check_call(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   error err;
   root->type = NULL;
   root->returns_lval = 1;

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   assert(first);
   struct type_array* first_type = (struct type_array*)first->type;

   root->type = (*first_type->content_type)->type_copy(first_type->content_type);

   err = type_def_cell_bind_type(inter, root);
   if (err) return err;

   err = check_root_type(root, expected_type);
   if (err) return err;

   int num_children = dlist_root_length(&root->ast_node.children);

   if (num_children == 1) {
      return NULL;
   }
   else if (num_children == 2) {
      struct ast_node_with_type* index_node = (struct ast_node_with_type*)dlist_root_nth(&root->ast_node.children, 1);
      return inter_type_check_expr(inter, &type_int, index_node);
   }
   else {
      return error_wrong_number_of_args(&root->ast_node, 1, num_children - 1);
   }
}

error type_array_eval_call(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   error err;

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   assert(first != NULL);
   struct type_array* first_type = (struct type_array*)first->type;
   int elt_size = (*first_type->content_type)->type_size(first_type->content_type);

   struct ast_node_with_type* second = (struct ast_node_with_type*)first->ast_node.node.next;

   int array_size = (*first->type)->type_size(first->type);
   int stack_start = inter->stack.stack_top;
   char* ptr;
   err = inter_eval_lval(inter, first, &ptr, array_size);
   if (!err) {
      if (second == first) {
         *((void**)value) = (void*)ptr;
      }
      else {
         int index;
         err = inter_eval_rval(inter, second, &index, sizeof(int));
         if (!err) {
            *((void**)value) = (void*)(ptr + index*elt_size);
         }
      }

      if (!first->returns_lval) {
         inter_destruct_val(inter, first->type, ptr);
      }
   }

   inter->stack.stack_top = stack_start;
   return err;
}



struct type_vtable type_array_vtable = {
   .type_free = (type_free_fn)type_array_free,
   .type_copy = (type_copy_fn)refc_copy,
   .type_print = (type_print_fn)type_array_print,
   .type_match = (type_match_fn)type_array_match,
   .type_size = (type_size_fn)type_array_size,
   .type_print_val = (type_print_val_fn)type_array_print_val,
   .type_copy_val = (type_copy_val_fn)type_array_copy_val,
   .type_destruct_val = (type_destruct_val_fn)type_array_destruct_val,
   .type_parse_constant = NULL,
   .type_check_call = (type_check_call_fn)type_array_check_call,
   .type_eval_call = (type_eval_call_fn)type_array_eval_call
};

type type_array(int length, type content_type) {
   struct type_array* self = (struct type_array*)refc_alloc(sizeof(struct type_array));
   assert(self);
   self->vtable = &type_array_vtable;
   self->length = length;
   self->content_type = content_type;
   return (type)self;
}


void type_function_free(struct type_function* self, struct inter* inter) {
   if (!refc_dec(self)) {
      int index = 0;
      for(index = 0; index < self->num_arguments; index++) {
         type arg_type = self->argument_types[index];
         (*arg_type)->type_free(arg_type, inter);
      }
      if (self->argument_types)
         free(self->argument_types);
      (*(self->return_type))->type_free(self->return_type, inter);
      refc_free(self);
   }
}

void type_function_print(FILE* output, struct type_function* self, char* tag) {
   int index;
   fprintf(output, "(%s (", tag);
   for(index = 0; index < self->num_arguments; index++) {
      type arg_type = self->argument_types[index];
      if (index > 0) {
         fprintf(output, " ");
      }
      (*arg_type)->type_print(output, arg_type);
   }
   if (self->accepts_var_args) {
      fprintf(output, " ...");
   }
   fprintf(output, ") ");
   (*(self->return_type))->type_print(output, self->return_type);
   fprintf(output, ")");
}

void type_c_function_print(FILE* output, struct type_function* self) {
   type_function_print(output, self, "c-fun");
}

void type_buf_function_print(FILE* output, struct type_function* self) {
   type_function_print(output, self, "buf-fun");
}


int type_function_match(struct type_function* self, struct type_function* other) {
   int index;
   int match = self->vtable == other->vtable &&
               self->num_arguments == other->num_arguments;
   if (!match) return 0;

   match = (*(self->return_type))->type_match(self->return_type, other->return_type);
   if (!match) return 0;

   for(index = 0; index < self->num_arguments; index++) {
      type self_arg = self->argument_types[index];
      type other_arg = other->argument_types[index];
      match = (*self_arg)->type_match(self_arg, other_arg);
      if (!match) return 0;
   }

   match = self->accepts_var_args == other->accepts_var_args;
   return match;
}

error inter_check_return_type(struct ast_node_with_type* root, type expected_type, type return_type) {
   assert(return_type);
   root->type = (*return_type)->type_copy(return_type);
   return check_root_type(root, expected_type);
}

error type_function_check_call(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   error err;

   root->type = NULL;
   root->returns_lval = 0;

   int num_children = dlist_root_length(&root->ast_node.children);

   assert(num_children > 0);

   struct ast_node_with_type* first = (struct ast_node_with_type*)dlist_root_nth(&root->ast_node.children, 0);

   struct type_function* first_type = (struct type_function*)first->type;

   err = inter_check_return_type(root, expected_type, first_type->return_type);
   if (err) return err;

   int num_arguments = num_children - 1;
   if (num_arguments < first_type->num_arguments ||
       (num_arguments > first_type->num_arguments &&
        !first_type->accepts_var_args)) {
      return error_incorrect_num_args(&root->ast_node, (type)first_type, num_arguments);
   }

   int arg_num = 0;
   struct ast_node_with_type* arg_node = (struct ast_node_with_type*)first->ast_node.node.next;

   while (arg_node != first) {

      type arg_expected_type = (arg_num < first_type->num_arguments) 
         ? first_type->argument_types[arg_num]
         : NULL;

      err = inter_type_check_expr(inter, arg_expected_type, arg_node);
      if(err) return err;

      arg_node = (struct ast_node_with_type*)arg_node->ast_node.node.next;
      arg_num++;
   }

   return NULL;
}


int type_function_args_size(struct ast_node_with_type* root) {
   int size = 0;
   int num_children = dlist_root_length(&root->ast_node.children);
   assert(num_children > 0);
   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* curr = (struct ast_node_with_type*)first->ast_node.node.next;
   struct type_function* first_type = (struct type_function*)first->type;

   while (curr != first) {
      size += (*curr->type)->type_size(curr->type);

      curr = (struct ast_node_with_type*)curr->ast_node.node.next;
   }
   
   return size;
}


#define TYPE_FUNCTION_EVAL_CALL_DEF(TYPE, TYPENAME) \
error type_function_eval_call_##TYPENAME(struct inter* inter, struct ast_node_with_type* root, void* value, int size) { \
   error err; \
   int num_children = dlist_root_length(&root->ast_node.children); \
   assert(num_children > 0); \
   int num_args = num_children - 1; \
   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node; \
   struct type_function* first_type = (struct type_function*)first->type; \
   struct ast_node_with_type* arg_node = (struct ast_node_with_type*)first->ast_node.node.next; \
   int stack_start = inter->stack.stack_top; \
   int args_size = type_function_args_size(root); \
   char* args = stack_alloc(&inter->stack, args_size); \
   assert(args); \
   char* args_pos = args; \
   while (arg_node != first) { \
      int arg_size = (*arg_node->type)->type_size(arg_node->type); \
      void* arg_alloc = args_pos; \
      args_pos += arg_size; \
      err = inter_eval_rval(inter, arg_node, arg_alloc, arg_size); \
      if (err) break; \
      arg_node = (struct ast_node_with_type*)arg_node->ast_node.node.next; \
   } \
   if (!err) { \
      void* function_handle; \
      err = inter_eval_rval(inter, first, &function_handle, sizeof(function_handle)); \
      if (!err) { \
         *((TYPE*)value) = call_with_buffer_##TYPENAME((TYPE (*)())function_handle, args, args_size); \
      } \
   } \
   struct ast_node_with_type* arg_node_ = (struct ast_node_with_type*)first->ast_node.node.next; \
   args_pos = args; \
   while (arg_node_ != first) { \
      int arg_size = (*arg_node_->type)->type_size(arg_node_->type); \
      void* arg_alloc = args_pos; \
      args_pos += arg_size; \
      inter_destruct_val(inter, arg_node_->type, arg_alloc); \
      arg_node_ = (struct ast_node_with_type*)arg_node_->ast_node.node.next; \
   } \
   inter->stack.stack_top = stack_start; \
   return err; \
}


TYPE_FUNCTION_EVAL_CALL_DEF(char, char)
TYPE_FUNCTION_EVAL_CALL_DEF(short, short)
TYPE_FUNCTION_EVAL_CALL_DEF(int, int)
TYPE_FUNCTION_EVAL_CALL_DEF(long, long)
TYPE_FUNCTION_EVAL_CALL_DEF(long long, long_long)
TYPE_FUNCTION_EVAL_CALL_DEF(float, float)
TYPE_FUNCTION_EVAL_CALL_DEF(double, double)
TYPE_FUNCTION_EVAL_CALL_DEF(long double, long_double)
TYPE_FUNCTION_EVAL_CALL_DEF(void*, pointer)

error type_function_eval_call_void(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   error err;
   int num_children = dlist_root_length(&root->ast_node.children);
   assert(num_children > 0);
   int num_args = num_children - 1;
   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct type_function* first_type = (struct type_function*)first->type;
   struct ast_node_with_type* arg_node = (struct ast_node_with_type*)first->ast_node.node.next;
   struct ast_node_with_type* arg_node_;
   int stack_start = inter->stack.stack_top;
   int args_size = type_function_args_size(root);
   char* args = stack_alloc(&inter->stack, args_size);
   assert(args);
   char* args_pos = args;
   while (arg_node != first) {
      int arg_size = (*arg_node->type)->type_size(arg_node->type);
      void* arg_alloc = (void*)args_pos;
      args_pos += arg_size;
      err = inter_eval_rval(inter, arg_node, arg_alloc, arg_size);
      if (err) goto pop_args;
      arg_node = (struct ast_node_with_type*)arg_node->ast_node.node.next;
   }
   void* function_handle;
   err = inter_eval_rval(inter, first, &function_handle, sizeof(function_handle));
   if (err) goto pop_args;
   call_with_buffer((void (*)())function_handle, args, args_size);
pop_args:
   arg_node_ = (struct ast_node_with_type*)first->ast_node.node.next;
   args_pos = args;
   while (arg_node_ != arg_node) {
      int arg_size = (*arg_node_->type)->type_size(arg_node_->type);
      void* arg_alloc = args_pos;
      args_pos += arg_size;
      inter_destruct_val(inter, arg_node_->type, arg_alloc);
      arg_node_ = (struct ast_node_with_type*)arg_node_->ast_node.node.next;
   }
   inter->stack.stack_top = stack_start;
   return err;
}


error type_buf_function_eval_call(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   error err;
   int num_children = dlist_root_length(&root->ast_node.children);
   assert(num_children > 0);
   int num_args = num_children - 1;
   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct type_function* first_type = (struct type_function*)first->type;
   struct ast_node_with_type* arg_node = (struct ast_node_with_type*)first->ast_node.node.next;
   struct ast_node_with_type* arg_node_;
   int stack_start = inter->stack.stack_top;
   int args_size = type_function_args_size(root);
   char* args = stack_alloc(&inter->stack, args_size);
   assert(args);
   char* args_pos = args;
   while (arg_node != first) {
      int arg_size = (*arg_node->type)->type_size(arg_node->type);
      void* arg_alloc = args_pos;
      args_pos += arg_size;
      err = inter_eval_rval(inter, arg_node, arg_alloc, arg_size);
      if (err) goto pop_args;
      arg_node = (struct ast_node_with_type*)arg_node->ast_node.node.next;
   }
   buf_function_t function_handle;
   err = inter_eval_rval(inter, first, &function_handle, sizeof(function_handle));
   if (err) goto pop_args;
   function_handle(args, args_size, value, size);
pop_args:
   arg_node_ = (struct ast_node_with_type*)first->ast_node.node.next;
   args_pos = args;
   while (arg_node_ != arg_node) {
      int arg_size = (*arg_node_->type)->type_size(arg_node_->type);
      void* arg_alloc = args_pos;
      args_pos += arg_size;
      inter_destruct_val(inter, arg_node_->type, arg_alloc);
      arg_node_ = (struct ast_node_with_type*)arg_node_->ast_node.node.next;
   }
   inter->stack.stack_top = stack_start;
   return err;
}


struct type_vtable type_function_void_vtable = {
   .type_free = (type_free_fn)type_function_free,
   .type_copy = (type_copy_fn)refc_copy,
   .type_print = (type_print_fn)type_c_function_print,
   .type_match = (type_match_fn)type_function_match,
   .type_size = type_size_pointer,
   .type_print_val = (type_print_val_fn)type_print_val_pointer,
   .type_copy_val = (type_copy_val_fn)type_copy_val_pointer,
   .type_destruct_val = NULL,
   .type_parse_constant = NULL,
   .type_check_call = (type_check_call_fn)type_function_check_call,
   .type_eval_call = (type_eval_call_fn)type_function_eval_call_void
};

struct type_vtable type_function_char_vtable = {
   .type_free = (type_free_fn)type_function_free,
   .type_copy = (type_copy_fn)refc_copy,
   .type_print = (type_print_fn)type_c_function_print,
   .type_match = (type_match_fn)type_function_match,
   .type_size = type_size_pointer,
   .type_print_val = (type_print_val_fn)type_print_val_pointer,
   .type_copy_val = (type_copy_val_fn)type_copy_val_pointer,
   .type_destruct_val = NULL,
   .type_parse_constant = NULL,
   .type_check_call = (type_check_call_fn)type_function_check_call,
   .type_eval_call = (type_eval_call_fn)type_function_eval_call_char
};

struct type_vtable type_function_short_vtable = {
   .type_free = (type_free_fn)type_function_free,
   .type_copy = (type_copy_fn)refc_copy,
   .type_print = (type_print_fn)type_c_function_print,
   .type_match = (type_match_fn)type_function_match,
   .type_size = type_size_pointer,
   .type_print_val = (type_print_val_fn)type_print_val_pointer,
   .type_copy_val = (type_copy_val_fn)type_copy_val_pointer,
   .type_destruct_val = NULL,
   .type_parse_constant = NULL,
   .type_check_call = (type_check_call_fn)type_function_check_call,
   .type_eval_call = (type_eval_call_fn)type_function_eval_call_short
};

struct type_vtable type_function_int_vtable = {
   .type_free = (type_free_fn)type_function_free,
   .type_copy = (type_copy_fn)refc_copy,
   .type_print = (type_print_fn)type_c_function_print,
   .type_match = (type_match_fn)type_function_match,
   .type_size = type_size_pointer,
   .type_print_val = (type_print_val_fn)type_print_val_pointer,
   .type_copy_val = (type_copy_val_fn)type_copy_val_pointer,
   .type_destruct_val = NULL,
   .type_parse_constant = NULL,
   .type_check_call = (type_check_call_fn)type_function_check_call,
   .type_eval_call = (type_eval_call_fn)type_function_eval_call_int
};

struct type_vtable type_function_long_vtable = {
   .type_free = (type_free_fn)type_function_free,
   .type_copy = (type_copy_fn)refc_copy,
   .type_print = (type_print_fn)type_c_function_print,
   .type_match = (type_match_fn)type_function_match,
   .type_size = type_size_pointer,
   .type_print_val = (type_print_val_fn)type_print_val_pointer,
   .type_copy_val = (type_copy_val_fn)type_copy_val_pointer,
   .type_destruct_val = NULL,
   .type_parse_constant = NULL,
   .type_check_call = (type_check_call_fn)type_function_check_call,
   .type_eval_call = (type_eval_call_fn)type_function_eval_call_long
};

struct type_vtable type_function_long_long_vtable = {
   .type_free = (type_free_fn)type_function_free,
   .type_copy = (type_copy_fn)refc_copy,
   .type_print = (type_print_fn)type_c_function_print,
   .type_match = (type_match_fn)type_function_match,
   .type_size = type_size_pointer,
   .type_print_val = (type_print_val_fn)type_print_val_pointer,
   .type_copy_val = (type_copy_val_fn)type_copy_val_pointer,
   .type_destruct_val = NULL,
   .type_parse_constant = NULL,
   .type_check_call = (type_check_call_fn)type_function_check_call,
   .type_eval_call = (type_eval_call_fn)type_function_eval_call_long_long
};

struct type_vtable type_function_float_vtable = {
   .type_free = (type_free_fn)type_function_free,
   .type_copy = (type_copy_fn)refc_copy,
   .type_print = (type_print_fn)type_c_function_print,
   .type_match = (type_match_fn)type_function_match,
   .type_size = type_size_pointer,
   .type_print_val = (type_print_val_fn)type_print_val_pointer,
   .type_copy_val = (type_copy_val_fn)type_copy_val_pointer,
   .type_destruct_val = NULL,
   .type_parse_constant = NULL,
   .type_check_call = (type_check_call_fn)type_function_check_call,
   .type_eval_call = (type_eval_call_fn)type_function_eval_call_float
};

struct type_vtable type_function_double_vtable = {
   .type_free = (type_free_fn)type_function_free,
   .type_copy = (type_copy_fn)refc_copy,
   .type_print = (type_print_fn)type_c_function_print,
   .type_match = (type_match_fn)type_function_match,
   .type_size = type_size_pointer,
   .type_print_val = (type_print_val_fn)type_print_val_pointer,
   .type_copy_val = (type_copy_val_fn)type_copy_val_pointer,
   .type_destruct_val = NULL,
   .type_parse_constant = NULL,
   .type_check_call = (type_check_call_fn)type_function_check_call,
   .type_eval_call = (type_eval_call_fn)type_function_eval_call_double
};

struct type_vtable type_function_long_double_vtable = {
   .type_free = (type_free_fn)type_function_free,
   .type_copy = (type_copy_fn)refc_copy,
   .type_print = (type_print_fn)type_c_function_print,
   .type_match = (type_match_fn)type_function_match,
   .type_size = type_size_pointer,
   .type_print_val = (type_print_val_fn)type_print_val_pointer,
   .type_copy_val = (type_copy_val_fn)type_copy_val_pointer,
   .type_destruct_val = NULL,
   .type_parse_constant = NULL,
   .type_check_call = (type_check_call_fn)type_function_check_call,
   .type_eval_call = (type_eval_call_fn)type_function_eval_call_long_double
};

struct type_vtable type_function_pointer_vtable = {
   .type_free = (type_free_fn)type_function_free,
   .type_copy = (type_copy_fn)refc_copy,
   .type_print = (type_print_fn)type_c_function_print,
   .type_match = (type_match_fn)type_function_match,
   .type_size = type_size_pointer,
   .type_print_val = (type_print_val_fn)type_print_val_pointer,
   .type_copy_val = (type_copy_val_fn)type_copy_val_pointer,
   .type_destruct_val = NULL,
   .type_parse_constant = NULL,
   .type_check_call = (type_check_call_fn)type_function_check_call,
   .type_eval_call = (type_eval_call_fn)type_function_eval_call_pointer
};

struct type_vtable type_buf_function_vtable = {
   .type_free = (type_free_fn)type_function_free,
   .type_copy = (type_copy_fn)refc_copy,
   .type_print = (type_print_fn)type_buf_function_print,
   .type_match = (type_match_fn)type_function_match,
   .type_size = type_size_pointer,
   .type_print_val = (type_print_val_fn)type_print_val_pointer,
   .type_copy_val = (type_copy_val_fn)type_copy_val_pointer,
   .type_destruct_val = NULL,
   .type_parse_constant = NULL,
   .type_check_call = (type_check_call_fn)type_function_check_call,
   .type_eval_call = (type_eval_call_fn)type_buf_function_eval_call
};




type type_function(struct type_vtable* vtable, int num_arguments, type* argument_types, type return_type, int accepts_var_args) {
   struct type_function* self = (struct type_function*)refc_alloc(sizeof(struct type_function));
   assert(self);
   self->vtable = vtable;
   self->num_arguments = num_arguments;
   self->argument_types = argument_types;
   self->return_type = return_type;
   self->accepts_var_args = accepts_var_args;
   return (type)self;
}

void type_c_macro_free(struct type_c_macro* self, struct inter* inter) {
   if (!refc_dec(self)) {
      if (self->return_type) {
         (*self->return_type)->type_free(self->return_type, inter);
      }
      refc_free(self);
   }
}

void type_c_macro_print(FILE* output, struct type_c_macro* self) {
   fprintf(output, "(c-macro %p ", self->type_check);
   if (self->return_type) {
      (*self->return_type)->type_print(output, self->return_type);
   }
   else {
      fprintf(output, "NULL");
   }
   fprintf(output, ")");
}

int type_c_macro_match(struct type_c_macro* self, struct type_c_macro* other) {
   return self->vtable == other->vtable &&
          self->type_check == other->type_check &&
          (*self->return_type)->type_match(self->return_type, other->return_type);
}


error type_c_macro_check_call(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   error err;
   root->type = NULL;
   root->returns_lval = 0;

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;

   assert(first != NULL);

   struct type_c_macro* first_type = (struct type_c_macro*)first->type;

   if (first_type->return_type) {
      err = inter_check_return_type(root, expected_type, first_type->return_type);
      if (err) return err;
   }

   return first_type->type_check(inter, expected_type, root);
}

error type_c_macro_eval_call(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   error err;

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;

   assert(first != NULL);

   type_eval_call_fn function_handle;
   err = inter_eval_rval(inter, first, &function_handle, sizeof(type_eval_call_fn));
   if (err)
      return err;

   return function_handle(inter, root, value, size);
}


struct type_vtable type_c_macro_vtable = {
   .type_free = (type_free_fn)type_c_macro_free,
   .type_copy = (type_copy_fn)refc_copy,
   .type_print = (type_print_fn)type_c_macro_print,
   .type_match = (type_match_fn)type_c_macro_match,
   .type_size = type_size_pointer,
   .type_print_val = (type_print_val_fn)type_print_val_pointer,
   .type_copy_val = (type_copy_val_fn)type_copy_val_pointer,
   .type_destruct_val = NULL,
   .type_parse_constant = NULL,
   .type_check_call = (type_check_call_fn)type_c_macro_check_call,
   .type_eval_call = (type_eval_call_fn)type_c_macro_eval_call
};

type type_c_macro(type_check_call_fn type_check, type return_type) {
   struct type_c_macro* self = (struct type_c_macro*)refc_alloc(sizeof(struct type_c_macro));
   assert(self);
   self->vtable = &type_c_macro_vtable;
   self->type_check = type_check;
   self->return_type = return_type;
   return (type)self;
}

void type_code_free(struct type_code* self, struct inter* inter) {
   if (!refc_dec(self)) {
      (*self->return_type)->type_free(self->return_type, inter);
      refc_free(self);
   }
}

void type_code_print(FILE* output, struct type_code* self) {
   fprintf(output, "(code ");
   (*self->return_type)->type_print(output, self->return_type);
   fprintf(output, ")");
}

int type_code_match(struct type_code* self, struct type_code* other) {
   return self->vtable == other->vtable &&
          (*self->return_type)->type_match(self->return_type, other->return_type);
}

void type_code_print_val(FILE* output, struct type_code* self, struct ast_node_with_type** value) {
   ast_node_print(output, &(*value)->ast_node);
}

error type_code_check_call(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   root->type = NULL;
   root->returns_lval = 0;

   int num_children = dlist_root_length(&root->ast_node.children);

   if (num_children != 1) {
      return error_wrong_number_of_args(&root->ast_node, 0, num_children - 1);
   }

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct type_code* first_type = (struct type_code*)first->type;

   return inter_check_return_type(root, expected_type, first_type->return_type);
}


error type_code_eval_call(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   error err;

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;

   assert(first != NULL);

   struct ast_node_with_type* code_root;
   err = inter_eval_rval(inter, first, &code_root, sizeof(code_root));
   if (!err) {
      struct type_code* first_type = (struct type_code*)first->type;

      // Create a copy of the tree to store types unique to this evaluation context.
      struct ast_node_with_type* code_copy = ast_node_with_type_copy_tree(code_root);

      err = inter_type_check_expr(inter, first_type->return_type, code_copy);
      if (!err) {
         err = inter_eval_rval(inter, code_copy, value, size);
      }

      ast_node_with_type_rec_free(code_copy, inter);
   }

   ast_node_with_type_rec_free(code_root, inter);
   return err;
}


void type_code_unsafe_print(FILE* output, struct type_code* self) {
   fprintf(output, "(code-unsafe ");
   (*self->return_type)->type_print(output, self->return_type);
   fprintf(output, ")");
}

error type_code_unsafe_eval_call(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   error err;

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;

   assert(first != NULL);

   struct ast_node_with_type* code_root;
   err = inter_eval_rval(inter, first, &code_root, sizeof(code_root));
   if (!err) {
      err = inter_eval(inter, code_root, value, size);
   }

   ast_node_with_type_rec_free(code_root, inter);
   return err;
}


struct type_vtable type_code_vtable = {
   .type_free = (type_free_fn)type_code_free,
   .type_copy = (type_copy_fn)refc_copy,
   .type_print = (type_print_fn)type_code_print,
   .type_match = (type_match_fn)type_code_match,
   .type_size = type_size_pointer,
   .type_print_val = (type_print_val_fn)type_code_print_val,
   .type_copy_val = (type_copy_val_fn)type_copy_val_refc,
   .type_destruct_val = (type_destruct_val_fn)type_quote_destruct_val,
   .type_parse_constant = NULL,
   .type_check_call = (type_check_call_fn)type_code_check_call,
   .type_eval_call = (type_eval_call_fn)type_code_eval_call
};

struct type_vtable type_code_unsafe_vtable = {
   .type_free = (type_free_fn)type_code_free,
   .type_copy = (type_copy_fn)refc_copy,
   .type_print = (type_print_fn)type_code_unsafe_print,
   .type_match = (type_match_fn)type_code_match,
   .type_size = type_size_pointer,
   .type_print_val = (type_print_val_fn)type_code_print_val,
   .type_copy_val = (type_copy_val_fn)type_copy_val_refc,
   .type_destruct_val = (type_destruct_val_fn)type_quote_destruct_val,
   .type_parse_constant = NULL,
   .type_check_call = (type_check_call_fn)type_code_check_call,
   .type_eval_call = (type_eval_call_fn)type_code_unsafe_eval_call
};


type type_code(struct type_vtable* vtable, type return_type) {
   struct type_code* self = (struct type_code*)refc_alloc(sizeof(struct type_code));
   assert(self);
   self->vtable = vtable;
   self->return_type = return_type;
   return (type)self;
}



void type_quote_print(FILE* output, type self) {
   fprintf(output, "quote");
}

void type_quote_print_val(FILE* output, type self, struct ast_node_with_type** value) {
   ast_node_print(output, &(*value)->ast_node);
}

// There's something wrong here. Check it out. 
void type_quote_destruct_val(type self, struct ast_node_with_type** value, struct inter* inter) {
   if (*value) {
      ast_node_with_type_rec_free(*value, inter);
   }
}

error type_quote_check_call(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   root->type = NULL;
   root->returns_lval = 0;

   int num_children = dlist_root_length(&root->ast_node.children);

   if (num_children != 1) {
      return error_wrong_number_of_args(&root->ast_node, 0, num_children - 1);
   }

   if (expected_type == NULL) {
      return error_type_required(&root->ast_node);
   }

   root->type = (*expected_type)->type_copy(expected_type);
   return NULL;
}

error type_quote_eval_call(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   error err;

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   assert(first->type);

   assert(first != NULL);

   struct ast_node_with_type* quote_root;
   err = inter_eval_rval(inter, first, &quote_root, sizeof(quote_root));
   if (!err) {
      struct ast_node_with_type* quote_copy = ast_node_with_type_copy_tree(quote_root);

      err = inter_type_check_expr(inter, root->type, quote_copy);
      if (!err) {
         err = inter_eval_rval(inter, quote_copy, value, size);
      }

      ast_node_with_type_rec_free(quote_copy, inter);
   }

   ast_node_with_type_rec_free(quote_root, inter);
   return err;
}

struct type_vtable type_quote_vtable = {
   .type_free = (type_free_fn)type_free_nothing,
   .type_copy = (type_copy_fn)type_copy_static,
   .type_print = (type_print_fn)type_quote_print,
   .type_match = (type_match_fn)type_match_static,
   .type_size = type_size_pointer,
   .type_print_val = (type_print_val_fn)type_quote_print_val,
   .type_copy_val = (type_copy_val_fn)type_copy_val_refc,
   .type_destruct_val = (type_destruct_val_fn)type_quote_destruct_val,
   .type_parse_constant = NULL,
   .type_check_call = (type_check_call_fn)type_quote_check_call,
   .type_eval_call = (type_eval_call_fn)type_quote_eval_call
};


struct type_vtable* type_quote = &type_quote_vtable;


void error_struct_field_undefined_print(FILE* output, struct error_struct_field_undefined* self) {
   fprintf(output, "%s: %d.%d: Struct field %s undefined in ",
         self->location->filename, self->location->line, self->location->column, self->location->text);
   type_struct_print(output, self->type);
   fprintf(output, "\n");
}

struct error_vtable error_struct_field_undefined_vtable = {
   .error_print = (error_print_fn)error_struct_field_undefined_print,
   .error_free = (error_free_fn)free
};

error error_struct_field_undefined(struct ast_node* location, struct type_struct* type) {
   struct error_struct_field_undefined* self = (struct error_struct_field_undefined*)malloc(sizeof(struct error_struct_field_undefined));
   assert(self);
   self->vtable = &error_struct_field_undefined_vtable;
   self->location = location;
   self->type = type;
   return (error)self;
}

struct type_struct_slot* type_struct_slot(char* name, int name_len, int offset, type type) {
   struct type_struct_slot* self = (struct type_struct_slot*)malloc(sizeof(struct type_struct_slot));
   assert(self);
   self->name = (char*)refc_copy(name);
   self->name_len = name_len;
   self->offset = offset;
   self->type = type;
   self->bit_width = 0;
   self->bit_offset = 0;
   return self;
}

void type_struct_slot_free(struct type_struct_slot* self, struct inter* inter) {
   inter_release_sym(inter, self->name);
   (*self->type)->type_free(self->type, inter);
   free(self);
}

void type_struct_slot_print(FILE* output, struct type_struct_slot* self) {
   fprintf(output, "(");
   token_print(output, self->name, self->name_len);
   fprintf(output, " ");
   (*self->type)->type_print(output, self->type);
   fprintf(output, ")");
}


#define TYPE_STRUCT_SLOT_PRINT_VAL_BIT(TYPE) \
   TYPE val = *((TYPE*)slot_addr); \
   val = BITS_GET_WINDOW(TYPE, val, self->bit_offset, self->bit_width); \
   (*slot_type)->type_print_val(output, slot_type, &val); \
   

void type_struct_slot_print_val(FILE* output, struct type_struct_slot* self, void* struct_value) {
   void* slot_addr = ((char*)struct_value) + self->offset;

   type slot_type;
   error err = NULL;
   TYPE_DEF_CELL_GET_TYPE(self->type, NULL, slot_type, err)
   assert(!err);

   fprintf(output, "(");
   token_print(output, self->name, self->name_len);
   fprintf(output, " ");

   if (self->bit_width == 0) {
      (*slot_type)->type_print_val(output, slot_type, slot_addr);
   } else {
      if (*slot_type == &type_char_vtable) {
         TYPE_STRUCT_SLOT_PRINT_VAL_BIT(char)
      } else if (*slot_type == &type_uchar_vtable) {
         TYPE_STRUCT_SLOT_PRINT_VAL_BIT(unsigned char)
      } else if (*slot_type == &type_short_vtable) {
         TYPE_STRUCT_SLOT_PRINT_VAL_BIT(short)
      } else if (*slot_type == &type_ushort_vtable) {
         TYPE_STRUCT_SLOT_PRINT_VAL_BIT(unsigned short)
      } else if (*slot_type == &type_int_vtable) {
         TYPE_STRUCT_SLOT_PRINT_VAL_BIT(int)
      } else if (*slot_type == &type_uint_vtable) {
         TYPE_STRUCT_SLOT_PRINT_VAL_BIT(unsigned int)
      } else if (*slot_type == &type_long_vtable) {
         TYPE_STRUCT_SLOT_PRINT_VAL_BIT(long)
      } else if (*slot_type == &type_ulong_vtable) {
         TYPE_STRUCT_SLOT_PRINT_VAL_BIT(unsigned long)
      } else if (*slot_type == &type_long_long_vtable) {
         TYPE_STRUCT_SLOT_PRINT_VAL_BIT(long long)
      } else if (*slot_type == &type_ulong_long_vtable) {
         TYPE_STRUCT_SLOT_PRINT_VAL_BIT(unsigned long long)
      } else {
         assert(0);
      }
   }
   fprintf(output, ")");
}

int type_struct_slot_match(struct type_struct_slot* slot1, struct type_struct_slot* slot2) {
   if (strcmp(slot1->name, slot2->name) ||
       slot1->offset != slot2->offset)
      return 0;

   if (slot1->type == slot2->type)
      return 1;

   error err = NULL;
   type slot1_type;
   type slot2_type;
   TYPE_DEF_CELL_GET_TYPE(slot1->type, NULL, slot1_type, err)
   if (err) {
      assert(0);
      free(err);
      return 0;
   }

   TYPE_DEF_CELL_GET_TYPE(slot2->type, NULL, slot2_type, err)
   if (err) {
      assert(0);
      free(err);
      return 0;
   }

   return (*slot1_type)->type_match(slot1_type, slot2_type);
}

int size_alignment(int size) {
   int bits = 0;
   while (size > 1) {
      bits++;
      size >>= 1;
   }
   return bits;
}

char* struct_packing_scheme_strs[] = {
   "packed",
   "union",
   "align32"
};

void type_struct_free(struct type_struct* self, struct inter* inter) {
   if (!refc_dec(self)) {
      int i;
      for (i = 0; i < self->num_slots; i++) {
         if (self->slots_array[i])
            type_struct_slot_free(self->slots_array[i], inter);
      }
      if (self->slots_array) {
         free(self->slots_array);
      }
      inter_mapping_destruct_keys_only(&self->fields, inter);
      refc_free(self);
   }
}

void type_struct_print(FILE* output, struct type_struct* self) {
   fprintf(output, "(struct %s", struct_packing_scheme_strs[self->packing_scheme]);
   int i;
   for (i = 0; i < self->num_slots; i++) {
      fprintf(output, " ");
      type_struct_slot_print(output, self->slots_array[i]);
   }
   fprintf(output, ")");
}

void type_struct_print_val(FILE* output, struct type_struct* self, void* value) {
   fprintf(output, "{");
   int i;
   for (i = 0; i < self->num_slots; i++) {
      if (i > 0)
         fprintf(output, " ");
      type_struct_slot_print_val(output, self->slots_array[i], value);
   }
   fprintf(output, "}");
}

int type_struct_match(struct type_struct* self, struct type_struct* other) {

   if (self == other)
      return 1;

   if (self->vtable != other->vtable ||
       self->num_slots != other->num_slots ||
       self->size != other->size ||
       self->packing_scheme != other->packing_scheme) {
      return 0;
   }

   int i;
   for (i = 0; i < self->num_slots; i++) {
      if (!type_struct_slot_match(self->slots_array[i], other->slots_array[i])) {
         return 0;
      }
   }

   return 1;
}

// This copies certain fields multiples times if they are bit slots.
// Unions will be copied weirdly too.
void type_struct_copy_val(struct type_struct* self, char* src, char* dest) {
   int i;
   
   if (src == dest)
      return;

   for (i = 0; i < self->num_slots; i++) {
      struct type_struct_slot* slot = self->slots_array[i];

      type slot_type;
      error err = NULL;
      TYPE_DEF_CELL_GET_TYPE(slot->type, NULL, slot_type, err)
      assert(!err);

      int slot_size = (*slot_type)->type_size(slot_type);

      TYPE_COPY_VAL(slot_type, src + slot->offset, dest + slot->offset, slot_size)
   }
}

// This will absolutely not work on unions.
void type_struct_destruct_val(struct type_struct* self, char* val, struct inter* inter) {
   int i;

   for (i = 0; i < self->num_slots; i++) {
      struct type_struct_slot* slot = self->slots_array[i];

      inter_destruct_val2(inter, slot->type, val + slot->offset);
   }
}

int type_struct_size(struct type_struct* self) {
   return self->size;
}

error type_struct_check_call(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   error err = NULL;
   root->type = NULL;
   root->eval_table.eval_rval = inter_eval_fallback_rval;
   root->eval_table.eval_lval = type_struct_eval_call_lval;
   root->returns_lval = 1;

   int num_children = dlist_root_length(&root->ast_node.children);

   if (num_children != 2) {
      return error_wrong_number_of_args(&root->ast_node, 1, num_children - 1);
   }

   struct ast_node_with_type* first = (struct ast_node_with_type*)dlist_root_nth(&root->ast_node.children, 0);
   struct ast_node_with_type* field_node = (struct ast_node_with_type*)dlist_root_nth(&root->ast_node.children, 1);

   if (!first->returns_lval) {
      root->eval_table.eval_rval = type_struct_eval_call_rval;
      root->eval_table.eval_lval = inter_eval_fallback_lval;
      root->returns_lval = 0;
   }

   if (field_node->ast_node.state != TOKEN_STATE_SYMBOL) {
      return error_syntax(&field_node->ast_node);
   }

   struct type_struct* first_type = (struct type_struct*)first->type;

   struct type_struct_slot* slot = hash_map_find_value(
         &first_type->fields, field_node->ast_node.text, NULL);

   if (!slot) {
      return error_struct_field_undefined(&field_node->ast_node, first_type);
   }

   type slot_type;
   TYPE_DEF_CELL_GET_TYPE(slot->type, &field_node->ast_node, slot_type, err)
   if (err) return err;

   field_node->constant.i = slot->offset;

   root->type = (*slot_type)->type_copy(slot_type);

   err = check_root_type(root, expected_type);
   if (err) return err;

   if (slot->bit_width) {
      // override default.
      first->constant.bit_section.width = slot->bit_width;
      first->constant.bit_section.offset = slot->bit_offset;

      root->returns_lval = 0;
      root->eval_table.eval_lval = inter_eval_fallback_lval;

      if (*root->type == &type_char_vtable) {
         root->eval_table.eval_rval = type_struct_eval_call_bit_char;
      } else if (*root->type == &type_uchar_vtable) {
         root->eval_table.eval_rval = type_struct_eval_call_bit_uchar;
      } else if (*root->type == &type_short_vtable) {
         root->eval_table.eval_rval = type_struct_eval_call_bit_short;
      } else if (*root->type == &type_ushort_vtable) {
         root->eval_table.eval_rval = type_struct_eval_call_bit_ushort;
      } else if (*root->type == &type_int_vtable) {
         root->eval_table.eval_rval = type_struct_eval_call_bit_int;
      } else if (*root->type == &type_uint_vtable) {
         root->eval_table.eval_rval = type_struct_eval_call_bit_uint;
      } else if (*root->type == &type_long_vtable) {
         root->eval_table.eval_rval = type_struct_eval_call_bit_long;
      } else if (*root->type == &type_ulong_vtable) {
         root->eval_table.eval_rval = type_struct_eval_call_bit_ulong;
      } else if (*root->type == &type_long_long_vtable) {
         root->eval_table.eval_rval = type_struct_eval_call_bit_long_long;
      } else if (*root->type == &type_ulong_long_vtable) {
         root->eval_table.eval_rval = type_struct_eval_call_bit_ulong_long;
      } else {
         assert(0);
      }
   }

   return NULL;
}



// called when the struct is an lval.
error type_struct_eval_call_lval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   error err;

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   assert(first != NULL);
   assert(first->returns_lval);

   struct ast_node_with_type* field_node = (struct ast_node_with_type*)first->ast_node.node.next;
   assert(field_node != first);
   assert(field_node->ast_node.state == TOKEN_STATE_SYMBOL);

   int offset = field_node->constant.i;

   int struct_size = (*first->type)->type_size(first->type);
   int stack_top = inter->stack.stack_top;
   void* struct_value;
   err = inter_eval_lval(inter, first, &struct_value, struct_size);
   if (!err) {
      *((void**)value) = (void*)((char*)struct_value + offset);
   }

   inter->stack.stack_top = stack_top;
   return err;
}


// struct node may be lval or rval.
error type_struct_eval_call_rval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   error err;

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   assert(first != NULL);

   struct ast_node_with_type* field_node = (struct ast_node_with_type*)first->ast_node.node.next;
   assert(field_node != first);
   assert(field_node->ast_node.state == TOKEN_STATE_SYMBOL);

   int offset = field_node->constant.i;

   int struct_size = (*first->type)->type_size(first->type);
   int stack_top = inter->stack.stack_top;
   void* struct_value;
   err = inter_eval_lval(inter, first, &struct_value, struct_size);
   if (!err) {
      TYPE_COPY_VAL(root->type, (char*)struct_value + offset, value, size)
   }

   if (!first->returns_lval) {
      inter_destruct_val(inter, first->type, struct_value);
   }

   inter->stack.stack_top = stack_top;
   return err;
}





#define DEF_TYPE_STRUCT_EVAL_CALL_BIT(TYPE_NAME, TYPE) \
error type_struct_eval_call_bit_##TYPE_NAME(struct inter* inter, struct ast_node_with_type* root, void* value, int size) { \
   error err; \
   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node; \
   assert(first); \
   struct ast_node_with_type* field_node = (struct ast_node_with_type*)first->ast_node.node.next; \
   int offset = field_node->constant.i; \
   int bit_width = first->constant.bit_section.width; \
   int bit_offset = first->constant.bit_section.offset; \
   int struct_size = (*first->type)->type_size(first->type); \
   void* struct_value; \
   err = inter_eval_lval(inter, first, &struct_value, struct_size); \
   if (err) return err; \
   TYPE* location = (TYPE*)((char*)struct_value + offset); \
   *((TYPE*)value) = BITS_GET_WINDOW(TYPE,*location, bit_offset, bit_width); \
   return NULL; \
}


DEF_TYPE_STRUCT_EVAL_CALL_BIT(char,char)
DEF_TYPE_STRUCT_EVAL_CALL_BIT(uchar,unsigned char)
DEF_TYPE_STRUCT_EVAL_CALL_BIT(short,short)
DEF_TYPE_STRUCT_EVAL_CALL_BIT(ushort,unsigned short)
DEF_TYPE_STRUCT_EVAL_CALL_BIT(int,int)
DEF_TYPE_STRUCT_EVAL_CALL_BIT(uint,unsigned int)
DEF_TYPE_STRUCT_EVAL_CALL_BIT(long,long)
DEF_TYPE_STRUCT_EVAL_CALL_BIT(ulong,unsigned long)
DEF_TYPE_STRUCT_EVAL_CALL_BIT(long_long,long long)
DEF_TYPE_STRUCT_EVAL_CALL_BIT(ulong_long,unsigned long long)


struct type_vtable type_struct_vtable = {
   .type_free = (type_free_fn)type_struct_free,
   .type_copy = (type_copy_fn)refc_copy,
   .type_print = (type_print_fn)type_struct_print,
   .type_match = (type_match_fn)type_struct_match,
   .type_size = (type_size_fn)type_struct_size,
   .type_print_val = (type_print_val_fn)type_struct_print_val,
   .type_copy_val = (type_copy_val_fn)type_struct_copy_val,
   .type_destruct_val = (type_destruct_val_fn)type_struct_destruct_val,
   .type_parse_constant = NULL,
   .type_check_call = (type_check_call_fn)type_struct_check_call,
   .type_eval_call = (type_eval_call_fn)type_struct_eval_call_lval
};

void type_prefix_print(FILE* output, void* self) {
   fprintf(output, "prefix");
}

void type_prefix_print_val(FILE* output, void* self, struct prefix* value) {
   fprintf(output, "(prefix ");
   token_print(output, value->prefix, value->prefix_len);
   fprintf(output, ")");
}

void type_prefix_copy_val(type self, struct prefix* src, struct prefix* dest) {
   prefix_init(dest, (char*)refc_copy(src->prefix), src->prefix_len, src->inter);
}

void type_prefix_destruct_val(type self, struct prefix* value, struct inter* inter) {
   prefix_destruct(value, inter);
}

int type_prefix_size(void* self) {
   return sizeof(struct prefix);
}

error type_prefix_check_call(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   error err;
   root->type = NULL;
   root->returns_lval = 1;

   int num_children = dlist_root_length(&root->ast_node.children);

   if (num_children != 2) {
      return error_wrong_number_of_args(&root->ast_node, 1, num_children - 1);
   }

   struct ast_node_with_type* first = (struct ast_node_with_type*)dlist_root_nth(&root->ast_node.children, 0);
   struct ast_node_with_type* sym_node = (struct ast_node_with_type*)dlist_root_nth(&root->ast_node.children, 1);


   if (first->ast_node.state != TOKEN_STATE_SYMBOL) {
      return error_syntax(&first->ast_node);
   }

   if (sym_node->ast_node.state != TOKEN_STATE_SYMBOL) {
      return error_syntax(&sym_node->ast_node);
   }

   err = inter_type_check_expr(inter, &type_prefix, first);
   if (err) return err;
   
   void* stack_place = inter->stack.buffer + inter->stack.stack_top;
   struct prefix* prefix;

   err = inter_eval_lval(inter, first, &prefix, sizeof(struct prefix));
   if (err) return err;

   assert(prefix != stack_place);

   struct typed_value* value = prefix_get_value(prefix, sym_node->ast_node.text, sym_node->ast_node.text_len);

   if (!value) {
      return error_symbol_undefined(&sym_node->ast_node);
   }

   root->type = (*value->type)->type_copy(value->type);
   return check_root_type(root, expected_type);
}

error type_prefix_eval_call(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   error err;

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   assert(first != NULL);
   assert(first->ast_node.state == TOKEN_STATE_SYMBOL);

   struct ast_node_with_type* sym_node = (struct ast_node_with_type*)first->ast_node.node.next;
   assert(sym_node != first);
   assert(sym_node->ast_node.state == TOKEN_STATE_SYMBOL);

   int stack_top = inter->stack.stack_top;
   struct prefix* prefix;
   err = inter_eval_lval(inter, first, &prefix, sizeof(struct prefix));
   if (!err) {
      struct typed_value* sym_val = prefix_get_value(prefix, sym_node->ast_node.text, sym_node->ast_node.text_len);

      *((void**)value) = sym_val->val;

      if (!first->returns_lval) {
         inter_destruct_val(inter, first->type, prefix);
      }
   }

   inter->stack.stack_top = stack_top;
   return err;
}

struct type_vtable type_prefix_vtable = {
   .type_free = (type_free_fn)type_free_nothing,
   .type_copy = (type_copy_fn)type_copy_static,
   .type_print = (type_print_fn)type_prefix_print,
   .type_match = (type_match_fn)type_match_static,
   .type_size = (type_size_fn)type_prefix_size,
   .type_print_val = (type_print_val_fn)type_prefix_print_val,
   .type_copy_val = (type_copy_val_fn)type_prefix_copy_val,
   .type_destruct_val = (type_destruct_val_fn)type_prefix_destruct_val,
   .type_parse_constant = NULL,
   .type_check_call = (type_check_call_fn)type_prefix_check_call,
   .type_eval_call = (type_eval_call_fn)type_prefix_eval_call
};

struct type_vtable* type_prefix = &type_prefix_vtable;



struct typed_value* local_var_binding(type var_type, char* alloc) {
   struct typed_value* self = (struct typed_value*)malloc(typed_value_size(var_type));
   assert(self);
   self->type = &type_local_var_binding;
   struct local_var_binding* local_binding = (struct local_var_binding*)self->val;
   local_binding->var_type = (*var_type)->type_copy(var_type);
   local_binding->var_alloc = alloc;
   return self;
}

void local_var_binding_free(struct typed_value* self, struct inter* inter) {
   struct local_var_binding* local_binding = (struct local_var_binding*)self->val;
   (*local_binding->var_type)->type_free(local_binding->var_type, inter);
   free(self);
}

void type_local_var_binding_print(FILE* output, type self) {
   fprintf(output, "local-var-binding");
}

int type_local_var_binding_size(type self) {
   return sizeof(struct local_var_binding);
}

void type_local_var_binding_print_val(FILE* output, type self, struct local_var_binding* val) {
   fprintf(output, "(local-var-binding ");
   (*val->var_type)->type_print(output, val->var_type);
   fprintf(output, " %p)", val->var_alloc);
}

struct type_vtable type_local_var_binding_vtable = {
   .type_free = type_free_nothing,
   .type_copy = type_copy_static,
   .type_print = type_local_var_binding_print,
   .type_match = type_match_static,
   .type_size = type_local_var_binding_size,
   .type_print_val = (type_print_val_fn)type_local_var_binding_print_val,
   .type_copy_val = (type_copy_val_fn)NULL, // not implemented.
   .type_destruct_val = NULL,
   .type_parse_constant = NULL,
   .type_check_call = NULL,
   .type_eval_call = NULL
};

struct type_vtable* type_local_var_binding = &type_local_var_binding_vtable;

void local_var_binding_increment(struct ast_node_with_type* root, int increment) {
   if (root->ast_node.state == TOKEN_STATE_SYMBOL) {
      if (!root->symbol) return;

      struct typed_value* sym_binding = (struct typed_value*)hash_map_sym_top_value(root->symbol);

      if (!sym_binding) return;

      if (*sym_binding->type == &type_local_var_binding_vtable)
         root->constant.i += increment;

      return;
   }

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   if (first) {
      struct ast_node_with_type* curr = first;
      do {
         local_var_binding_increment(curr, increment);
         curr = (struct ast_node_with_type*)curr->ast_node.node.next;
      } while (curr != first);
   }
}


void type_script_function_arg_destruct(struct type_script_function_arg* self, struct inter* inter) {
   inter_release_sym(inter, self->name);
   (*self->type)->type_free(self->type, inter);
}

void type_script_function_arg_print(FILE* output, struct type_script_function_arg* self) {
   fprintf(output, "(");
   token_print(output, self->name, self->name_len);
   fprintf(output, " ");
   (*self->type)->type_print(output, self->type);
   fprintf(output, ")");
}

int type_script_function_arg_match(struct type_script_function_arg* arg1, struct type_script_function_arg* arg2) {
   return arg1->symbol == arg2->symbol &&
          (*arg1->type)->type_match(arg1->type, arg2->type);
}

void type_script_function_free(struct type_script_function* self, struct inter* inter) {
   if (!refc_dec(self)) {
      int i;
      if (self->arguments) {
         for (i = 0; i < self->num_arguments; i++) {
            type_script_function_arg_destruct(self->arguments + i, inter);
         }
         free(self->arguments);
      }
      (*self->return_type)->type_free(self->return_type, inter);
      refc_free(self);
   }
}

void type_script_function_print_args(FILE* output, struct type_script_function* self) {
   int i;
   fprintf(output, "(");
   for (i = 0; i < self->num_arguments; i++) {
      if (i > 0)
         fprintf(output, " ");
      type_script_function_arg_print(output, self->arguments + i);
   }
   fprintf(output, ")");
}

void type_script_function_print(FILE* output, struct type_script_function* self) {
   fprintf(output, "(function ");
   type_script_function_print_args(output, self);
   fprintf(output, " ");
   (*self->return_type)->type_print(output, self->return_type);
   fprintf(output, ")");
}

void type_script_function_print_val(FILE* output, struct type_script_function* self, void* value) {
   fprintf(output, "(function ");
   type_script_function_print_args(output, self);
   fprintf(output, " ");
   (*self->return_type)->type_print(output, self->return_type);
   fprintf(output, " ");
   ast_node_print(output, *((struct ast_node**)value));
   fprintf(output, ")");
}

int type_script_function_match(struct type_script_function* self, struct type_script_function* other) {
   if (self->vtable != other->vtable ||
       self->num_arguments != other->num_arguments ||
       !(*self->return_type)->type_match(self->return_type, other->return_type)) {
      return 0;
   }

   int i;
   for (i = 0; i < self->num_arguments; i++) {
      if (!type_script_function_arg_match(self->arguments + i, other->arguments + i))
         return 0;
   }

   return 1;
}

error type_script_function_check_call(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   error err = NULL;
   root->type = NULL;
   root->returns_lval = 0;

   int num_children = dlist_root_length(&root->ast_node.children);

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct type_script_function* first_type = (struct type_script_function*)first->type;

   if (num_children - 1 != first_type->num_arguments) {
      return error_wrong_number_of_args(&root->ast_node, first_type->num_arguments, num_children - 1);
   }

   struct ast_node_with_type* curr = (struct ast_node_with_type*)first->ast_node.node.next;

   int i = 0;
   while (curr != first) {

      struct type_script_function_arg* arg = first_type->arguments + i;

      type arg_type;
      TYPE_DEF_CELL_GET_TYPE(arg->type, &curr->ast_node, arg_type, err)
      if (err) return err;

      if (arg_type != arg->type) {
         (*arg->type)->type_free(arg->type, inter);
         arg->type = (*arg_type)->type_copy(arg_type);
      }

      err = inter_type_check_expr(inter, arg_type, curr);
      if (err) return err;

      i++;
      curr = (struct ast_node_with_type*)curr->ast_node.node.next;
   }

   return inter_check_return_type(root, expected_type, first_type->return_type);
}

error type_script_function_eval_call(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   error err = NULL;

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   assert(first != NULL);
   struct type_script_function* first_type = (struct type_script_function*)first->type;

   struct ast_node_with_type* curr = (struct ast_node_with_type*)first->ast_node.node.prev;

   struct list* arg_values = (struct list*)alloca(sizeof(struct list)*first_type->num_arguments);

   int stack_top = inter->stack.stack_top;

   struct type_script_function_arg* arguments = first_type->arguments;

   int i = first_type->num_arguments - 1;
   int j;
   while (curr != first) {
      assert(i >= 0);

      struct type_script_function_arg* arg = arguments + i;

      type arg_type;
      TYPE_DEF_CELL_GET_TYPE(arg->type, &curr->ast_node, arg_type, err)
      if (err) goto restore_stack;

      int arg_size = (*arg_type)->type_size(arg_type);

      struct typed_value* alloc;
      //alloc = stack_alloc(&inter->stack, sizeof(struct typed_value) + arg_size);
      STACK_ALLOC(alloc, struct typed_value*, inter->stack, sizeof(struct typed_value) + arg_size);
      assert(alloc);

      alloc->type = arg_type;

      err = inter_eval_rval(inter, curr, alloc->val, arg_size);
      if (err) goto restore_stack;

      struct list* cell = arg_values + i;
      cell->value = (void*)alloc;
      cell->rest = (struct list*)arg->symbol->value;

      i--;
      curr = (struct ast_node_with_type*)curr->ast_node.node.prev;
   }

   assert(i == -1);

   struct ast_node_with_type** code_root;
   err = inter_eval_lval(inter, first, &code_root, sizeof(struct ast_node_with_type*));
   if (err) goto restore_stack;

   // Now bind the arguments.
   for (j = 0; j < first_type->num_arguments; j++) {
      arguments[j].symbol->value = (void*)(arg_values + j);
   }

#ifdef LOG_FUN_CALL
   LOG("script_function_eval: %s:%d.%d: code: %p,%d:",
         root->ast_node.filename, root->ast_node.line, root->ast_node.column,
         code_root, refc_count(code_root))
   ast_node_print(LOG_FILE, &code_root->ast_node);
   LOG("\n")
#endif

   err = inter_eval_rval(inter, *code_root, value, size);
   if (!first->returns_lval)
      ast_node_with_type_rec_free(*code_root, inter);

pop_args:

   for (j = 0; j < first_type->num_arguments; j++) {
      struct type_script_function_arg* arg = arguments + j;
      struct list* cell = (struct list*)arg->symbol->value;
      assert(cell);
      assert(cell->value);

      arg->symbol->value = (void*)cell->rest;
   }

restore_stack:

   if (first_type->destruct_args) {
      for (j = i + 1; j < first_type->num_arguments; j++) {
         struct list* cell = arg_values + j;
         struct typed_value* arg_alloc = (struct typed_value*)cell->value;

         inter_destruct_val(inter, arg_alloc->type, arg_alloc->val);
      }
   }

   inter->stack.stack_top = stack_top;
   return err;
}

struct type_vtable type_script_function_vtable = {
   .type_free = (type_free_fn)type_script_function_free,
   .type_copy = (type_copy_fn)refc_copy,
   .type_print = (type_print_fn)type_script_function_print,
   .type_match = (type_match_fn)type_script_function_match,
   .type_size = (type_size_fn)type_size_pointer,
   .type_print_val = (type_print_val_fn)type_script_function_print_val,
   .type_copy_val = (type_copy_val_fn)type_copy_val_refc,
   .type_destruct_val = (type_destruct_val_fn)type_quote_destruct_val,
   .type_parse_constant = NULL,
   .type_check_call = (type_check_call_fn)type_script_function_check_call,
   .type_eval_call = (type_eval_call_fn)type_script_function_eval_call
};



void type_macro_free(struct type_macro* self, struct inter* inter) {
   if (!refc_dec(self)) {
      if (self->arg_symbols) {
         free(self->arg_symbols);
      }
      refc_free(self);
   }
}

void type_macro_print_args(FILE* output, struct type_macro* self) {
   int i;
   char* fmt;

   fprintf(output, "(");

   for (i = 0; i < self->num_arguments; i++) {
      fmt = i == 0 ? "%s" : " %s";
      fprintf(output, fmt, (char*)self->arg_symbols[i]->key);
   }

   if (self->accepts_var_args) {
      fmt = i == 0 ? "%s" : " %s";
      fprintf(output, fmt, "...");
   }

   fprintf(output, ")");
}

void type_macro_print(FILE* output, struct type_macro* self) {
   int i;
   fprintf(output, "(macro ");

   type_macro_print_args(output, self);

   fprintf(output, ")");
}

void type_macro_print_val(FILE* output, struct type_macro* self, void* value) {
   fprintf(output, "(macro ");
   type_macro_print_args(output, self);
   fprintf(output, " ");
   ast_node_print(output, *((struct ast_node**)value));
   fprintf(output, ")");
}

int type_macro_match(struct type_macro* self, struct type_macro* other) {
   if (self->vtable != other->vtable ||
       self->num_arguments != other->num_arguments ||
       self->accepts_var_args != other->accepts_var_args) {
      return 0;
   }

   int i;
   for (i = 0; i < self->num_arguments; i++) {
      if (self->arg_symbols[i] != other->arg_symbols[i]) {
         return 0;
      }
   }

   return 1;
}

error type_macro_check_call(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   error err;
   root->type = NULL;
   root->returns_lval = 0;

   char* arg0_str = inter_intern_sym(inter, MACRO_ARG0, strlen(MACRO_ARG0));
   struct key_value* arg0_symbol = inter_make_value_binding(inter, arg0_str, strlen(arg0_str));

   int num_children = dlist_root_length(&root->ast_node.children);

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct type_macro* first_type = (struct type_macro*)first->type;

   if ((!first_type->accepts_var_args && num_children - 1 != first_type->num_arguments) ||
       (first_type->accepts_var_args && num_children - 1 < first_type->num_arguments)) {
      return error_wrong_number_of_args(&root->ast_node, first_type->num_arguments, num_children - 1);
   }

   struct ast_node_with_type* macro_code;
   err = inter_eval_rval(inter, first, &macro_code, sizeof(macro_code));
   if (err) return err;
   assert(macro_code);

   struct ast_node_with_type* curr = (struct ast_node_with_type*)first->ast_node.node.next;

   int stack_top = inter->stack.stack_top;

   struct typed_value* arg_alloc = (struct typed_value*)stack_alloc(&inter->stack, sizeof(struct typed_value) + sizeof(void*));
   assert(arg_alloc);
   arg_alloc->type = &type_quote;
   *((struct ast_node_with_type**)arg_alloc->val) = first;

   hash_map_sym_push_value(arg0_symbol, arg_alloc);

   int i;
   for (i = 0; i < first_type->num_arguments; i++) {

      assert(curr != first);

      struct key_value* arg_symbol = first_type->arg_symbols[i];

      arg_alloc = (struct typed_value*)stack_alloc(&inter->stack, sizeof(struct typed_value) + sizeof(void*));
      assert(arg_alloc);
      arg_alloc->type = &type_quote;
      *((struct ast_node_with_type**)arg_alloc->val) = curr;

      hash_map_sym_push_value(arg_symbol, arg_alloc);

      curr = (struct ast_node_with_type*)curr->ast_node.node.next;
   }

#ifdef LOG_MACRO
   LOG("type_macro_check_call: macro_code: ")
   ast_node_print(LOG_FILE, &macro_code->ast_node);
   LOG("\n")
#endif

   struct ast_node_with_type* quote;
   err = inter_eval_rval(inter, macro_code, &quote, sizeof(quote));

   inter_destruct_val(inter, first->type, &macro_code);

   if (quote->ast_node.node.root) {
      // Don't mess with the original if it's already linked in a tree.
      struct ast_node_with_type* quote_copy = ast_node_with_type_copy_tree(quote);
      ast_node_with_type_rec_free(quote, inter);
      quote = quote_copy;
   }

#ifdef LOG_MACRO
   LOG("type_macro_check_call: expanded: ")
   ast_node_print(LOG_FILE, &quote->ast_node);
   LOG("\n")
#endif

   for (i = first_type->num_arguments - 1; i >= 0; i--) {
      hash_map_sym_pop_value(first_type->arg_symbols[i]);
   }

   hash_map_sym_pop_value(arg0_symbol);

   inter->stack.stack_top = stack_top;
   if (err) return err;

   ast_node_with_type_write_location(root, inter, quote);
   dlist_root_swap_nodes(&root->ast_node.children, &quote->ast_node.children);
   ast_node_with_type_write_to(quote, inter, root);
   ast_node_with_type_rec_free(quote, inter);

   // Finally type_check the replaced root.
   return inter_type_check_expr(inter, expected_type, root);
}

error type_macro_eval_call(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   // This should not occur normally.
   // During type checking a macro replaces itself with some other kind of expression.

   return error_eval_macro(&root->ast_node);
}

struct type_vtable type_macro_vtable = {
   .type_free = (type_free_fn)type_macro_free,
   .type_copy = (type_copy_fn)refc_copy,
   .type_print = (type_print_fn)type_macro_print,
   .type_match = (type_match_fn)type_macro_match,
   .type_size = (type_size_fn)type_size_pointer,
   .type_print_val = (type_print_val_fn)type_macro_print_val,
   .type_copy_val = (type_copy_val_fn)type_copy_val_refc,
   .type_destruct_val = (type_destruct_val_fn)type_quote_destruct_val,
   .type_parse_constant = NULL,
   .type_check_call = (type_check_call_fn)type_macro_check_call,
   .type_eval_call = (type_eval_call_fn)type_macro_eval_call
};



int type_is_ast_node(type self) {
   return *self == &type_code_vtable ||
          *self == &type_quote_vtable ||
          *self == &type_code_unsafe_vtable ||
          *self == &type_script_function_vtable ||
          *self == &type_macro_vtable;
}


void error_wrong_number_of_args_print(FILE* output, struct error_wrong_number_of_args* self) {
   fprintf(output, "%s: %d,%d: Incorrect number of arguments: Expected %d. Got %d.\n",
         self->location->filename, self->location->line, self->location->column,
         self->expected_args, self->num_args);
}

struct error_vtable error_wrong_number_of_args_vtable = {
   .error_print = (error_print_fn)error_wrong_number_of_args_print,
   .error_free = (error_free_fn)free
};

error error_wrong_number_of_args(struct ast_node* location, int expected_args, int num_args) {
   struct error_wrong_number_of_args* self = malloc(sizeof(struct error_wrong_number_of_args));
   assert(self);
   self->vtable = &error_wrong_number_of_args_vtable;
   self->location = location;
   self->expected_args = expected_args;
   self->num_args = num_args;
   return (error)self;
}

void error_syntax_print(FILE* output, struct error_at_node* self) {
   fprintf(output, "%s: %d,%d: Syntax error.\n", self->location->filename, self->location->line, self->location->column);
}

struct error_vtable error_syntax_vtable = {
   .error_print = (error_print_fn)error_syntax_print,
   .error_free = (error_free_fn)free
};

error error_syntax(struct ast_node* location) {
   struct error_at_node* self = malloc(sizeof(struct error_at_node));
   assert(self);
   self->vtable = &error_syntax_vtable;
   self->location = location;
   return (error)self;
}


void error_unknown_type_print(FILE* output, struct error_at_node* self) {
   fprintf(output, "%s: %d,%d: Unknown Type: ",
         self->location->filename, self->location->line, self->location->column);
   ast_node_print(output, self->location);
   fprintf(output, "\n");
}

struct error_vtable error_unknown_type_vtable = {
   .error_print = (error_print_fn)error_unknown_type_print,
   .error_free = (error_free_fn)free
};

error error_unknown_type(struct ast_node* location) {
   struct error_at_node* self = malloc(sizeof(struct error_at_node));
   assert(self);
   self->vtable = &error_unknown_type_vtable;
   self->location = location;
   return (error)self;
}


void error_symbol_undefined_print(FILE* output, struct error_at_node* self) {
   fprintf(output, "%s: %d,%d: Symbol Undefined: %s\n",
         self->location->filename, self->location->line, self->location->column, self->location->text);
}

struct error_vtable error_symbol_undefined_vtable = {
   .error_print = (error_print_fn)error_symbol_undefined_print,
   .error_free = (error_free_fn)free
};

error error_symbol_undefined(struct ast_node* location) {
   struct error_at_node* self = malloc(sizeof(struct error_at_node));
   assert(self);
   self->vtable = &error_symbol_undefined_vtable;
   self->location = location;
   return (error)self;
}


void error_char_expected_print(FILE* output, struct error_at_node* self) {
   fprintf(output, "%s: %d,%d: Character expected: %s\n",
         self->location->filename, self->location->line, self->location->column, self->location->text);
}

struct error_vtable error_char_expected_vtable = {
   .error_print = (error_print_fn)error_char_expected_print,
   .error_free = (error_free_fn)free
};

error error_char_expected(struct ast_node* location) {
   struct error_at_node* self = malloc(sizeof(struct error_at_node));
   assert(self);
   self->vtable = &error_char_expected_vtable;
   self->location = location;
   return (error)self;
}


void error_lval_expected_print(FILE* output, struct error_at_node* self) {
   fprintf(output, "%s: %d.%d: Expected lval.\n",
         self->location->filename, self->location->line, self->location->column);
}

struct error_vtable error_lval_expected_vtable = {
   .error_print = (error_print_fn)error_lval_expected_print,
   .error_free = (error_free_fn)free
};

error error_lval_expected(struct ast_node* location) {
   struct error_at_node* self = malloc(sizeof(struct error_at_node));
   assert(self);
   self->vtable = &error_lval_expected_vtable;
   self->location = location;
   return (error)self;
}


void error_type_required_print(FILE* output, struct error_at_node* self) {
   fprintf(output, "%s: %d.%d: Type required.\n",
         self->location->filename, self->location->line, self->location->column);
}

struct error_vtable error_type_required_vtable = {
   .error_print = (error_print_fn)error_type_required_print,
   .error_free = (error_free_fn)free
};

error error_type_required(struct ast_node* location) {
   struct error_at_node* self = malloc(sizeof(struct error_at_node));
   assert(self);
   self->vtable = &error_type_required_vtable;
   self->location = location;
   return (error)self;
}


void error_field_name_expected_print(FILE* output, struct error_at_node* self) {
   fprintf(output, "%s: %d.%d: Field name expected.\n",
         self->location->filename, self->location->line, self->location->column);
}

struct error_vtable error_field_name_expected_vtable = {
   .error_print = (error_print_fn)error_field_name_expected_print,
   .error_free = (error_free_fn)free
};

error error_field_name_expected(struct ast_node* location) {
   struct error_at_node* self = malloc(sizeof(struct error_at_node));
   assert(self);
   self->vtable = &error_field_name_expected_vtable;
   self->location = location;
   return (error)self;
}


void error_field_previously_defined_print(FILE* output, struct error_at_node* self) {
   fprintf(output, "%s: %d.%d: Field previously defined: %s.\n",
         self->location->filename, self->location->line, self->location->column, self->location->text);
}

struct error_vtable error_field_previously_defined_vtable = {
   .error_print = (error_print_fn)error_field_previously_defined_print,
   .error_free = (error_free_fn)free
};

error error_field_previously_defined(struct ast_node* location) {
   struct error_at_node* self = malloc(sizeof(struct error_at_node));
   assert(self);
   self->vtable = &error_field_previously_defined_vtable;
   self->location = location;
   return (error)self;
}


void error_field_undefined_print(FILE* output, struct error_at_node* self) {
   fprintf(output, "%s: %d.%d: Field previously defined: %s.\n",
         self->location->filename, self->location->line, self->location->column, self->location->text);
}

struct error_vtable error_field_undefined_vtable = {
   .error_print = (error_print_fn)error_field_undefined_print,
   .error_free = (error_free_fn)free
};

error error_field_undefined(struct ast_node* location) {
   struct error_at_node* self = malloc(sizeof(struct error_at_node));
   assert(self);
   self->vtable = &error_field_undefined_vtable;
   self->location = location;
   return (error)self;
}


void error_packing_scheme_expected_print(FILE* output, struct error_at_node* self) {
   fprintf(output, "%s: %d.%d: Struct packing scheme name expected.\n",
         self->location->filename, self->location->line, self->location->column);
}

struct error_vtable error_packing_scheme_expected_vtable = {
   .error_print = (error_print_fn)error_packing_scheme_expected_print,
   .error_free = (error_free_fn)free
};

error error_packing_scheme_expected(struct ast_node* location) {
   struct error_at_node* self = malloc(sizeof(struct error_at_node));
   assert(self);
   self->vtable = &error_packing_scheme_expected_vtable;
   self->location = location;
   return (error)self;
}


void error_stack_overflow_print(FILE* output, struct error_at_node* self) {
   fprintf(output, "%s: %d.%d: Stack overflow.\n",
         self->location->filename, self->location->line, self->location->column);
}

struct error_vtable error_stack_overflow_vtable = {
   .error_print = (error_print_fn)error_stack_overflow_print,
   .error_free = (error_free_fn)free
};

error error_stack_overflow(struct ast_node* location) {
   struct error_at_node* self = malloc(sizeof(struct error_at_node));
   assert(self);
   self->vtable = &error_stack_overflow_vtable;
   self->location = location;
   return (error)self;
}


void error_eval_macro_print(FILE* output, struct error_at_node* self) {
   fprintf(output, "%s:%d.%d: Attempted to call macro at eval time.\n",
         self->location->filename, self->location->line, self->location->column);
}

struct error_vtable error_eval_macro_vtable = {
   .error_print = (error_print_fn)error_eval_macro_print,
   .error_free = (error_free_fn)free,
};

error error_eval_macro(struct ast_node* location) {
   struct error_at_node* alloc = (struct error_at_node*)malloc(sizeof(struct error_at_node));
   assert(alloc);
   alloc->vtable = &error_eval_macro_vtable;
   alloc->location = location;
   return (error)alloc;
}





void error_function_cannot_return_type_print(FILE* output, struct error_function_cannot_return_type* self) {
   fprintf(output, "%s: %d.%d: Function cannot return type ",
      self->location->filename, self->location->line, self->location->column);
   (*self->return_type)->type_print(output, self->return_type);
   fprintf(output, "\n");
}

struct error_vtable error_function_cannot_return_type_vtable = {
   .error_print = (error_print_fn)error_function_cannot_return_type_print,
   .error_free = (error_free_fn)free
};

error error_function_cannot_return_type(struct ast_node* location, type return_type) {
   struct error_function_cannot_return_type* self = (struct error_function_cannot_return_type*)malloc(sizeof(struct error_function_cannot_return_type));
   assert(self);
   self->vtable = &error_function_cannot_return_type_vtable;
   self->location = location;
   self->return_type = return_type;
   return (error)self;
}

type_constructor_fn type_query_ast_node(struct inter* inter, struct ast_node* node, error* err) {
   struct ast_node* type_node;

   if (node->state == TOKEN_STATE_PUNCTUATION) {
      if (node->children.entry_node == NULL) {
         *err = error_syntax(node);
         return NULL;
      }

      type_node = (struct ast_node*)node->children.entry_node;
   }
   else {
      type_node = node;
   }

   return inter_query_type_cons(inter, type_node->text, type_node->text_len);
}

type type_build_ast_node(struct inter* inter, struct ast_node* node, error* err) {
   type_constructor_fn type_cons = type_query_ast_node(inter, node, err);
   if (type_cons != NULL) {
      return type_cons(inter, node, err);
   }

   if (node->state != TOKEN_STATE_SYMBOL) {
      *err = error_unknown_type(node);
      return NULL;
   }

   type type = inter_query_type(inter, node->text, node->text_len);
   if (!type) {
      *err = error_unknown_type(node);
      return NULL;
   }

   return (*type)->type_copy(type);
}

struct type_vtable* type_get_function_vtable(struct ast_node* location, type return_type, error* err) {
   if (*return_type == &type_void_vtable) {
      return &type_function_void_vtable;
   }
   else if (*return_type == &type_char_vtable) {
      return &type_function_char_vtable;
   }
   else if (*return_type == &type_short_vtable ||
            *return_type == &type_ushort_vtable) {
      return &type_function_short_vtable;
   }
   else if (*return_type == &type_int_vtable ||
            *return_type == &type_uint_vtable) {
      return &type_function_int_vtable;
   }
   else if (*return_type == &type_long_vtable ||
            *return_type == &type_ulong_vtable) {
      return &type_function_long_vtable;
   }
   else if (*return_type == &type_long_long_vtable ||
            *return_type == &type_ulong_long_vtable) {
      return &type_function_long_long_vtable;
   }
   else if (*return_type == &type_float_vtable) {
      return &type_function_float_vtable;
   }
   else if (*return_type == &type_double_vtable) {
      return &type_function_double_vtable;
   }
   else if (*return_type == &type_long_double_vtable) {
      return &type_function_long_double_vtable;
   }
   else if (*return_type == &type_pointer_vtable ||
            *return_type == &type_pointer_to_vtable ||
            *return_type == &type_string_vtable) {
      return &type_function_pointer_vtable;
   }
   else {
      *err = error_function_cannot_return_type(location, return_type);
      return NULL;
   }
}



type type_construct_void(struct inter* inter, struct ast_node* root, error* err) {
   return &type_void;
}

type type_construct_char(struct inter* inter, struct ast_node* root, error* err) {
   return &type_char;
}

type type_construct_uchar(struct inter* inter, struct ast_node* root, error* err) {
   return &type_uchar;
}

type type_construct_short(struct inter* inter, struct ast_node* root, error* err) {
   return &type_short;
}

type type_construct_ushort(struct inter* inter, struct ast_node* root, error* err) {
   return &type_ushort;
}

type type_construct_int(struct inter* inter, struct ast_node* root, error* err) {
   return &type_int;
}

type type_construct_uint(struct inter* inter, struct ast_node* root, error* err) {
   return &type_uint;
}

type type_construct_long(struct inter* inter, struct ast_node* root, error* err) {
   return &type_long;
}

type type_construct_ulong(struct inter* inter, struct ast_node* root, error* err) {
   return &type_ulong;
}

type type_construct_long_long(struct inter* inter, struct ast_node* root, error* err) {
   return &type_long_long;
}

type type_construct_ulong_long(struct inter* inter, struct ast_node* root, error* err) {
   return &type_ulong_long;
}

type type_construct_float(struct inter* inter, struct ast_node* root, error* err) {
   return &type_float;
}

type type_construct_double(struct inter* inter, struct ast_node* root, error* err) {
   return &type_double;
}

type type_construct_long_double(struct inter* inter, struct ast_node* root, error* err) {
   return &type_long_double;
}

type type_construct_pointer(struct inter* inter, struct ast_node* root, error* err) {
   return &type_pointer;
}

type type_construct_string(struct inter* inter, struct ast_node* root, error* err) {
   return &type_string;
}

type type_construct_string_auto(struct inter* inter, struct ast_node* root, error* err) {
   return &type_string_auto;
}

type type_construct_string_refc(struct inter* inter, struct ast_node* root, error* err) {
   return &type_string_refc;
}

type type_construct_symbol(struct inter* inter, struct ast_node* root, error* err) {
   return &type_symbol;
}

type type_construct_type(struct inter* inter, struct ast_node* root, error* err) {
   return &type_type;
}





type type_construct_pointer_to_(struct inter* inter, struct ast_node* root, error* err, struct type_vtable* vtable) {
   int num_children = dlist_root_length(&root->children);
   if (num_children == 0) {
      return &type_pointer;
   }

   if (num_children != 2) {
      *err = error_wrong_number_of_args(root, 1, num_children - 1);
      return NULL;
   }
   struct ast_node* content_node = (struct ast_node*)dlist_root_nth(&root->children, 1);
   type content_type = type_build_ast_node(inter, content_node, err);
   if (*err) return NULL;

   return type_pointer_to(vtable, content_type);
}


type type_construct_pointer_to(struct inter* inter, struct ast_node* root, error* err) {
   return type_construct_pointer_to_(inter, root, err, &type_pointer_to_vtable);
}

type type_construct_auto_pointer_to(struct inter* inter, struct ast_node* root, error* err) {
   return type_construct_pointer_to_(inter, root, err, &type_auto_pointer_to_vtable);
}

type type_construct_refc_pointer_to(struct inter* inter, struct ast_node* root, error* err) {
   return type_construct_pointer_to_(inter, root, err, &type_refc_pointer_to_vtable);
}


type type_construct_array(struct inter* inter, struct ast_node* root, error* err) {
   int num_children = dlist_root_length(&root->children);
   if (num_children != 3) {
      *err = error_wrong_number_of_args(root, 2, num_children - 1);
      return NULL;
   }
   struct ast_node_with_type* length_node = (struct ast_node_with_type*)dlist_root_nth(&root->children, 1);
   struct ast_node* content_node = (struct ast_node*)dlist_root_nth(&root->children, 2);

   *err = inter_type_check_expr(inter, &type_int, length_node);
   if (*err) return NULL;

   int length;
   *err = inter_eval_rval(inter, length_node, &length, sizeof(length));
   if (*err) return NULL;

   type content_type = type_build_ast_node(inter, content_node, err);
   if (*err) return NULL;

   return type_array(length, content_type);
}

type type_construct_c_function(struct inter* inter, struct ast_node* root, error* err) {
   int num_children = dlist_root_length(&root->children);
   if (num_children != 3) {
      *err = error_wrong_number_of_args(root, 2, num_children - 1);
      return NULL;
   }
   struct ast_node* args_node = (struct ast_node*)dlist_root_nth(&root->children, 1);
   struct ast_node* ret_node = (struct ast_node*)dlist_root_nth(&root->children, 2);
   int accepts_var_args = 0;
   int num_args = dlist_root_length(&args_node->children);
   if (num_args > 0) {
      struct ast_node* last_arg = (struct ast_node*)dlist_root_nth(&args_node->children, -1);
      if (!strcmp(last_arg->text, "...")) {
         num_args--;
         accepts_var_args = 1;
      }
   }
   type* argument_types = malloc(sizeof(type)*num_args);
   assert(argument_types);

   int arg_index;
   int index2;
   type arg_type;
   struct ast_node* curr = (struct ast_node*)args_node->children.entry_node;
   for(arg_index = 0; arg_index < num_args; arg_index++, curr = (struct ast_node*)curr->node.next) {
      arg_type = type_build_ast_node(inter, curr, err);
      if (*err) goto clean_up_argument_types;
      argument_types[arg_index] = arg_type;
   }

   type return_type = type_build_ast_node(inter, ret_node, err);
   if (*err) goto clean_up_argument_types;

   struct type_vtable* function_vtable = type_get_function_vtable(ret_node, return_type, err);
   if (*err) goto clean_up_argument_types;

   return type_function(function_vtable, num_args, argument_types, return_type, accepts_var_args);

clean_up_argument_types:
   for(index2 = 0; index2 < arg_index; index2++) {
      arg_type = argument_types[index2];
      (*arg_type)->type_free(arg_type, inter);
   }
   free(argument_types);
   return NULL;
}


type type_construct_buf_function(struct inter* inter, struct ast_node* root, error* err) {
   int num_children = dlist_root_length(&root->children);
   if (num_children != 3) {
      *err = error_wrong_number_of_args(root, 2, num_children - 1);
      return NULL;
   }
   struct ast_node* args_node = (struct ast_node*)dlist_root_nth(&root->children, 1);
   struct ast_node* ret_node = (struct ast_node*)dlist_root_nth(&root->children, 2);
   int accepts_var_args = 0;
   int num_args = dlist_root_length(&args_node->children);
   if (num_args > 0) {
      struct ast_node* last_arg = (struct ast_node*)dlist_root_nth(&args_node->children, -1);
      if (!strcmp(last_arg->text, "...")) {
         num_args--;
         accepts_var_args = 1;
      }
   }
   type* argument_types = malloc(sizeof(type)*num_args);
   assert(argument_types);

   int arg_index;
   int index2;
   type arg_type;
   struct ast_node* curr = (struct ast_node*)args_node->children.entry_node;
   for(arg_index = 0; arg_index < num_args; arg_index++, curr = (struct ast_node*)curr->node.next) {
      arg_type = type_build_ast_node(inter, curr, err);
      if (*err) goto clean_up_argument_types;
      argument_types[arg_index] = arg_type;
   }

   type return_type = type_build_ast_node(inter, ret_node, err);
   if (*err) goto clean_up_argument_types;

   struct type_vtable* function_vtable = &type_buf_function_vtable;

   return type_function(function_vtable, num_args, argument_types, return_type, accepts_var_args);

clean_up_argument_types:
   for(index2 = 0; index2 < arg_index; index2++) {
      arg_type = argument_types[index2];
      (*arg_type)->type_free(arg_type, inter);
   }
   free(argument_types);
   return NULL;
}

type type_construct_c_macro(struct inter* inter, struct ast_node* root, error* err) {
   int num_children = dlist_root_length(&root->children);

   if (num_children != 3) {
      *err = error_syntax(root);
      return NULL;
   }

   struct ast_node_with_type* type_check_node = (struct ast_node_with_type*)dlist_root_nth(&root->children, 1);
   struct ast_node_with_type* return_type_node = (struct ast_node_with_type*)dlist_root_nth(&root->children, 2);

   type_check_call_fn type_check;
   *err = inter_type_check_expr(inter, &type_pointer, type_check_node);
   if (*err)
      return NULL;

   type_check_call_fn function_handle;
   *err = inter_eval_rval(inter, type_check_node, &function_handle, sizeof(function_handle));
   if (*err)
      return NULL;

   type return_type = type_build_ast_node(inter, &return_type_node->ast_node, err);
   if (*err)
      return NULL;

   return type_c_macro(type_check, return_type);
}

type type_construct_code(struct inter* inter, struct ast_node* root, error* err) {
   int num_children = dlist_root_length(&root->children);

   if (num_children != 2) {
      *err = error_wrong_number_of_args(root, 1, num_children - 1);
      return NULL;
   }

   struct ast_node_with_type* return_type_node = (struct ast_node_with_type*)dlist_root_nth(&root->children, 1);

   type return_type = type_build_ast_node(inter, &return_type_node->ast_node, err);
   if (*err) return NULL;

   return type_code(&type_code_vtable, return_type);
}


type type_construct_code_unsafe(struct inter* inter, struct ast_node* root, error* err) {
   int num_children = dlist_root_length(&root->children);

   if (num_children != 2) {
      *err = error_wrong_number_of_args(root, 1, num_children - 1);
      return NULL;
   }

   struct ast_node_with_type* return_type_node = (struct ast_node_with_type*)dlist_root_nth(&root->children, 1);

   type return_type = type_build_ast_node(inter, &return_type_node->ast_node, err);
   if (*err) return NULL;

   return type_code(&type_code_unsafe_vtable, return_type);
}


type type_construct_quote(struct inter* inter, struct ast_node* root, error* err) {
   return &type_quote;
}



void error_bits_too_wide_print(FILE* output, struct error_bits_too_wide* self) {
   fprintf(output, "%s:%d.%d: Bits too large for type. Bits=%d. Type ",
         self->location->filename, self->location->line, self->location->column, self->bits);
   (*self->type)->type_print(output, self->type);
   fprintf(output, " has size %d.\n", self->type_size);
}

struct error_vtable error_bits_too_wide_vtable = {
   .error_print = (error_print_fn)error_bits_too_wide_print,
   .error_free = (error_free_fn)free
};

error error_bits_too_wide(struct ast_node* location, int bits, int type_size, type type) {
   struct error_bits_too_wide* alloc = (struct error_bits_too_wide*)malloc(sizeof(struct error_bits_too_wide));
   assert(alloc);
   alloc->vtable = &error_bits_too_wide_vtable;
   alloc->location = location;
   alloc->bits = bits;
   alloc->type_size = type_size;
   alloc->type = type;
   return (error)alloc;
}


type type_construct_struct(struct inter* inter, struct ast_node* root, error* err) {

   int num_children = dlist_root_length(&root->children);

   if (num_children < 2) {
      *err = error_wrong_number_of_args(root, 1, num_children - 1);
      return NULL;
   }

   struct ast_node* first = (struct ast_node*)root->children.entry_node;
   struct ast_node* packing_scheme_node = (struct ast_node*)first->node.next;

   if (packing_scheme_node->state != TOKEN_STATE_SYMBOL) {
      *err = error_packing_scheme_expected(packing_scheme_node);
      return NULL;
   }

   int packing_scheme;
   for (packing_scheme = 0; packing_scheme < NUM_STRUCT_PACKING_SCHEMES; packing_scheme++) {
      if (!strncmp(
            struct_packing_scheme_strs[packing_scheme],
            packing_scheme_node->text,
            packing_scheme_node->text_len)) {
         break;
      }
   }

   if (packing_scheme == NUM_STRUCT_PACKING_SCHEMES) {
      *err = error_packing_scheme_expected(packing_scheme_node);
      return NULL;
   }

   struct type_struct* self = (struct type_struct*)refc_alloc(sizeof(struct type_struct));
   assert(self);
   self->vtable = &type_struct_vtable;
   self->packing_scheme = packing_scheme;
   hash_table_init(&self->fields, &hash_pointer_obj, 0);
   self->num_slots = num_children - 2;
   int i = 0;
   int j;

   assert(self->num_slots >= 0);

   if (self->num_slots == 0) {
      self->slots_array = NULL;
   }
   else {
      self->slots_array = calloc(self->num_slots, sizeof(type));
      assert(self->slots_array);

      int prev_offset = 0;
      int offset = 0;
      int struct_size = 0;

      type bits_type = NULL;
      int bits_size;
      int bits_offset;

      struct ast_node* curr = (struct ast_node*)packing_scheme_node->node.next;
   
      while(curr != first) {
         assert(i < self->num_slots);
         int num_curr_children = dlist_root_length(&curr->children);
   
         if (num_curr_children != 2 && num_curr_children != 3) {
            *err = error_syntax(curr);
            goto clean_up;
         }
   
         struct ast_node* curr_name_node = (struct ast_node*)dlist_root_nth(&curr->children, 0);
         struct ast_node* curr_type_node = (struct ast_node*)dlist_root_nth(&curr->children, 1);
         struct ast_node_with_type* bits_node = NULL;
         int num_bits = 0;

         if (curr_name_node->state != TOKEN_STATE_SYMBOL) {
            *err = error_field_name_expected(curr_name_node);
            goto clean_up;
         }

         if (num_curr_children == 3) {
            bits_node = (struct ast_node_with_type*)dlist_root_nth(&curr->children, 2);
         }
 
         type curr_type = type_build_ast_node(inter, curr_type_node, err);
         if (*err) goto clean_up;

         type curr_type_binding;
         TYPE_DEF_CELL_GET_TYPE(curr_type, curr_type_node, curr_type_binding, *err)
         if (*err) goto clean_up;
   
         int curr_size = (*curr_type_binding)->type_size(curr_type_binding);
         int align_bits = size_alignment(curr_size);

         switch (packing_scheme) {
            case STRUCT_ALIGN32:
               if (align_bits > 2) align_bits = 2;
               offset = ALIGN(align_bits, offset);
               break;
         }

         struct type_struct_slot* slot = type_struct_slot(curr_name_node->text, curr_name_node->text_len, offset, curr_type);

         struct key_value* binding = inter_mapping_intern(&self->fields, curr_name_node->text);

         if (binding->value != NULL) {
            *err = error_field_previously_defined(curr_name_node);
            type_struct_slot_free(slot, inter);
            goto clean_up;
         }

         binding->value = slot;
         self->slots_array[i] = slot;

         int alloc_slot = 0;

         if (bits_node) {

            *err = inter_type_check_expr(inter, &type_int, bits_node);
            if (*err) goto clean_up;

            *err = inter_eval_rval(inter, bits_node, &num_bits, sizeof(int));
            if (*err) goto clean_up;

            if (*curr_type_binding != &type_char_vtable &&
                *curr_type_binding != &type_uchar_vtable &&
                *curr_type_binding != &type_short_vtable &&
                *curr_type_binding != &type_ushort_vtable &&
                *curr_type_binding != &type_int_vtable &&
                *curr_type_binding != &type_uint_vtable &&
                *curr_type_binding != &type_long_vtable &&
                *curr_type_binding != &type_ulong_vtable &&
                *curr_type_binding != &type_long_long_vtable &&
                *curr_type_binding != &type_ulong_long_vtable) {
               *err = error_type_mismatch(curr_type_node, NULL, curr_type_binding);
               goto clean_up;
            }

            if (!bits_type ||
                !(*bits_type)->type_match(bits_type, curr_type_binding)) {
               alloc_slot = 1;
               bits_type = curr_type_binding;
               bits_size = curr_size*8;
               bits_offset = 0;
            }

            if (num_bits >= bits_size) {
               *err = error_bits_too_wide(&bits_node->ast_node, num_bits, bits_size, bits_type);
               goto clean_up;
            }

            if (num_bits <= 0) {
               *err = error_syntax(&bits_node->ast_node);
               goto clean_up;
            }

            if (bits_offset + num_bits > bits_size) {
               alloc_slot = 1;
            }

            if (!alloc_slot) {
               slot->bit_width = num_bits;
               slot->bit_offset = bits_offset;
               slot->offset = prev_offset;
               bits_offset += num_bits;
            }
            else {
               slot->bit_width = num_bits;
               slot->bit_offset = bits_offset = 0;
               slot->offset = offset;
               bits_offset += num_bits;
            }
         }
         else {
            alloc_slot = 1;
            bits_type = NULL;
            bits_size = 0;
            bits_offset = 0;
         }
   
         if (alloc_slot) {
            prev_offset = offset;
            switch(packing_scheme) {
               case STRUCT_PACKED:
                  offset += curr_size;
                  struct_size = offset;
                  break;
   
               case STRUCT_UNION:
                  if (struct_size < curr_size)
                     struct_size = curr_size;
                  break;
   
               case STRUCT_ALIGN32:
                  offset += curr_size;
                  struct_size = offset;
                  break;
      
               default:
                  assert(0);
            }
         }
   
         i++;
         curr = (struct ast_node*)curr->node.next;
      }

      self->size = struct_size;
   }


   return (type)self;

clean_up:
   type_struct_free(self, inter);
   return NULL;
}

type type_construct_prefix(struct inter* inter, struct ast_node* root, error* err) {
   return &type_prefix;
}


type type_construct_script_function(struct inter* inter, struct ast_node* root, error* err) {
   int num_children = dlist_root_length(&root->children);

   if (num_children != 3) {
      *err = error_wrong_number_of_args(root, 2, num_children - 1);
      return NULL;
   }

   struct ast_node* first = (struct ast_node*)root->children.entry_node;
   struct ast_node* args_node = (struct ast_node*)first->node.next;
   struct ast_node* ret_type_node = (struct ast_node*)args_node->node.next;

   return type_construct_script_function_(inter, args_node, ret_type_node, err);
}

type type_construct_script_function_(struct inter* inter, struct ast_node* args_node, struct ast_node* ret_type_node, error* err) {

   if (args_node->state != TOKEN_STATE_PUNCTUATION) {
      *err = error_syntax(args_node);
      return NULL;
   }

   type ret_type = type_build_ast_node(inter, ret_type_node, err);
   if (*err) return NULL;

   int num_args = dlist_root_length(&args_node->children);

   struct type_script_function_arg* arguments = malloc(sizeof(struct type_script_function_arg)*num_args);
   assert(arguments);
   int i = 0;
   int j;

   int destruct_args = 0;

   struct ast_node* arg_node = (struct ast_node*)args_node->children.entry_node;
   while (i < num_args) {
      if (dlist_root_length(&arg_node->children) != 2) {
         *err = error_syntax(arg_node);
         goto clean_args;
      }

      struct ast_node* arg_name_node = (struct ast_node*)arg_node->children.entry_node;
      struct ast_node* arg_type_node = (struct ast_node*)arg_name_node->node.next;

      if (arg_name_node->state != TOKEN_STATE_SYMBOL) {
         *err = error_syntax(arg_name_node);
         goto clean_args;
      }

      type arg_type = type_build_ast_node(inter, arg_type_node, err);
      if (*err) goto clean_args;

      struct key_value* arg_symbol = inter_make_value_binding(inter, arg_name_node->text, arg_name_node->text_len);
      struct type_script_function_arg* arg = arguments + i;

      arg->symbol = arg_symbol;
      arg->name = (char*)refc_copy(arg_name_node->text);
      assert(arg->name);
      arg->name_len = arg_name_node->text_len;
      arg->type = arg_type;

      error temp_err = NULL;
      type arg_type_binding;
      TYPE_DEF_CELL_GET_TYPE(arg_type, arg_type_node, arg_type_binding, temp_err)
      if (temp_err) {
         (*temp_err)->error_free(temp_err);
         destruct_args = 1;
      }
      else {
         destruct_args = destruct_args || ((*arg_type_binding)->type_destruct_val != NULL);
      }

      i++;
      arg_node = (struct ast_node*)arg_node->node.next;
   }

   struct type_script_function* ret = (struct type_script_function*)refc_alloc(sizeof(struct type_script_function));
   assert(ret);
   ret->vtable = &type_script_function_vtable;
   ret->num_arguments = num_args;
   ret->arguments = arguments;
   ret->return_type = ret_type;
   ret->destruct_args = destruct_args;

   return (type)ret;

clean_args:
   (*ret_type)->type_free(ret_type, inter);
   for (j = 0; j < i; j++) {
      type_script_function_arg_destruct(arguments + j, inter);
   }
   free(arguments);
   return NULL;
}



type type_construct_macro(struct inter* inter, struct ast_node* root, error* err) {
   int num_children = dlist_root_length(&root->children);

   if (num_children != 2) {
      *err = error_wrong_number_of_args(root, 1, num_children - 1);
      return NULL;
   }

   struct ast_node* first = (struct ast_node*)root->children.entry_node;
   struct ast_node* args_node = (struct ast_node*)first->node.next;

   return type_construct_macro_(inter, args_node, err);
}

type type_construct_macro_(struct inter* inter, struct ast_node* args_node, error* err) {

   if (args_node->state != TOKEN_STATE_PUNCTUATION) {
      *err = error_syntax(args_node);
      return NULL;
   }

   int num_arguments = dlist_root_length(&args_node->children);
   int accepts_var_args = 0;

   struct ast_node_with_type* first_arg = (struct ast_node_with_type*)args_node->children.entry_node;
   struct ast_node_with_type* last_arg = first_arg;

   if (first_arg != NULL) {
      last_arg = (struct ast_node_with_type*)first_arg->ast_node.node.prev;
      if (!strcmp(last_arg->ast_node.text, "...")) {
         num_arguments--;
         accepts_var_args = 1;
      }
   }

   struct key_value** arg_symbols = NULL;

   if (num_arguments > 0) {
      arg_symbols = malloc(sizeof(struct key_value*)*num_arguments);
      assert(arg_symbols);

      int i = 0;
      struct ast_node_with_type* curr = first_arg;
      while (i < num_arguments) {

         if (curr->ast_node.state != TOKEN_STATE_SYMBOL) {
            *err = error_syntax(&curr->ast_node);
            goto clean_up_arg_symbols;
         }

         inter_cache_value_symbol(inter, curr);
         arg_symbols[i] = curr->symbol;

         i++;
         curr = (struct ast_node_with_type*)curr->ast_node.node.next;
      }
   }

   struct type_macro* ret = (struct type_macro*)refc_alloc(sizeof(struct type_macro));
   ret->vtable = &type_macro_vtable;
   ret->num_arguments = num_arguments;
   ret->accepts_var_args = accepts_var_args;
   ret->arg_symbols = arg_symbols;

   return (type)ret;

clean_up_arg_symbols:

   free(arg_symbols);
   return NULL;
}









void error_type_mismatch_print(FILE* output, struct error_type_mismatch* err) {
   fprintf(output, "%s: %d,%d: Type Mismatch for ",
         err->location->filename, err->location->line, err->location->column);
   ast_node_print(output, err->location);

   if (err->expected_type) {
      fprintf(output, ": Expected ");
      (*err->expected_type)->type_print(output, err->expected_type);
      fprintf(output, ".");
   }
   if (err->actual_type) {
      fprintf(output, " Found ");
      (*err->actual_type)->type_print(output, err->actual_type);
      fprintf(output, ".");
   }
   fprintf(output, "\n");
}

struct error_vtable error_type_mismatch_vtable = {
   .error_print = (error_print_fn)error_type_mismatch_print,
   .error_free = (error_free_fn)free
};

error error_type_mismatch(struct ast_node* location, type expected_type, type actual_type) {
   struct error_type_mismatch* self = (struct error_type_mismatch*)malloc(sizeof(struct error_type_mismatch));
   assert(self);
   self->vtable = &error_type_mismatch_vtable;
   self->location = location;
   self->expected_type = expected_type;
   self->actual_type = actual_type;
   return (error)self;
}

error check_root_type(struct ast_node_with_type* root, type expected_type) {
   if (expected_type != NULL &&
       !(*expected_type)->type_match(expected_type, root->type)) {
      return error_type_mismatch(&root->ast_node, expected_type, root->type);
   }
   return NULL;
}

void error_cannot_call_print(FILE* output, struct error_cannot_call* err) {
   fprintf(output, "%s: %d,%d: Cannot call value %s. Type: ",
         err->location->filename, err->location->line, err->location->column, err->location->text);
   (*err->actual_type)->type_print(output, err->actual_type);
   fprintf(output, "\n");
}

struct error_vtable error_cannot_call_vtable = {
   .error_print = (error_print_fn)error_cannot_call_print,
   .error_free = (error_free_fn)free
};

error error_cannot_call(struct ast_node* location, type actual_type) {
   struct error_cannot_call* self = (struct error_cannot_call*)malloc(sizeof(struct error_cannot_call));
   assert(self);
   self->vtable = &error_cannot_call_vtable;
   self->location = location;
   self->actual_type = actual_type;
   return (error)self;
}


void error_incorrect_num_args_print(FILE* output, struct error_incorrect_num_args* err) {
   fprintf(output, "%s: %d,%d: Incorrect number of arguments. %d given for calling type ",
         err->location->filename, err->location->line, err->location->column, err->actual_num_args);
   (*err->first_type)->type_print(output, err->first_type);
   fprintf(output, "\n");
}

struct error_vtable error_incorrect_num_args_vtable = {
   .error_print = (error_print_fn)error_incorrect_num_args_print,
   .error_free = (error_free_fn)free
};

error error_incorrect_num_args(struct ast_node* location, type first_type, int actual_num_args) {
   struct error_incorrect_num_args* self = (struct error_incorrect_num_args*)malloc(sizeof(struct error_incorrect_num_args));
   assert(self);
   self->vtable = &error_incorrect_num_args_vtable;
   self->location = location;
   self->first_type = first_type;
   self->actual_num_args = actual_num_args;
   return (error)self;
}


void error_cannot_cast_print(FILE* output, struct error_cannot_cast* self) {
   fprintf(output, "%s: %d.%d: Cannot cast type ",
         self->location->filename, self->location->line, self->location->column);
   (*self->value_type)->type_print(output, self->value_type);
   fprintf(output, " to type ");
   (*self->cast_type)->type_print(output, self->cast_type);
   fprintf(output, "\n");
}

struct error_vtable error_cannot_cast_vtable = {
   .error_print = (error_print_fn)error_cannot_cast_print,
   .error_free = (error_free_fn)free
};

error error_cannot_cast(struct ast_node* location, type cast_type, type value_type) {
   struct error_cannot_cast* self = (struct error_cannot_cast*)malloc(sizeof(struct error_cannot_cast));
   assert(self);
   self->vtable = &error_cannot_cast_vtable;
   self->location = location;
   self->cast_type = cast_type;
   self->value_type = value_type;
   return (error)self;
}




error inter_type_check_constant(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   error err;
   root->returns_lval = 0;
   if (expected_type == NULL) {
      union number_t sample_num;
      err = type_parse_int(&type_int, root, &sample_num.i);
      if (err) {
         (*err)->error_free(err);
         err = type_parse_double(&type_double, root, &sample_num.d);
         if (err) {
            (*err)->error_free(err);
            return error_symbol_undefined(&root->ast_node);
         }
         else {
            root->type = &type_double;
            return NULL;
         }
      }
      else {
         root->type = &type_int;
         return NULL;
      }
   }
   else if ((*expected_type)->type_parse_constant == NULL) {
      return error_symbol_undefined(&root->ast_node);
   }
   else {
      root->type = (*expected_type)->type_copy(expected_type);
      void* stack_place = inter->stack.buffer + inter->stack.stack_top;
      return (*root->type)->type_parse_constant(root->type, root, stack_place);
   }
}

#ifdef LOGGING
int inter_log_nesting_level = 0;
#endif

error inter_type_check_expr(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   error err = NULL;

#ifdef LOG_TYPE_CHECK
   int space_i;
   for (space_i = 0; space_i < inter_log_nesting_level; space_i++) {
      LOG(" ")
   }
   LOG("type_check_expr start %s:%d.%d: ",
         root->ast_node.filename, root->ast_node.line, root->ast_node.column)
   ast_node_print(LOG_FILE, &root->ast_node);
   LOG("\n")
   inter_log_nesting_level++;
#endif

   root->type = NULL;
   root->returns_lval = 0;

   TYPE_DEF_CELL_GET_TYPE(expected_type, &root->ast_node, expected_type, err)
   if (err) goto done;

   assert(!expected_type || (*expected_type != &type_def_cell_vtable));

   if (root->ast_node.state == TOKEN_STATE_SYMBOL) {
      inter_cache_find_value_symbol(inter, root);
      struct typed_value* binding = root->symbol == NULL
         ? NULL
         : (struct typed_value*)hash_map_sym_top_value(root->symbol);

      if (binding == NULL) {
         err = inter_type_check_constant(inter, expected_type, root);
         if (err) goto done;

         err = type_def_cell_bind_type(inter, root);
         if (err) goto done;

         if (*root->type == &type_char_vtable)
            root->eval_table = inter_constant_char_eval_table;
         else if (*root->type == &type_uchar_vtable)
            root->eval_table = inter_constant_uchar_eval_table;
         else if (*root->type == &type_short_vtable)
            root->eval_table = inter_constant_short_eval_table;
         else if (*root->type == &type_ushort_vtable)
            root->eval_table = inter_constant_ushort_eval_table;
         else if (*root->type == &type_int_vtable)
            root->eval_table = inter_constant_int_eval_table;
         else if (*root->type == &type_uint_vtable)
            root->eval_table = inter_constant_uint_eval_table;
         else if (*root->type == &type_long_vtable)
            root->eval_table = inter_constant_long_eval_table;
         else if (*root->type == &type_ulong_vtable)
            root->eval_table = inter_constant_ulong_eval_table;
         else if (*root->type == &type_long_long_vtable)
            root->eval_table = inter_constant_long_long_eval_table;
         else if (*root->type == &type_ulong_long_vtable)
            root->eval_table = inter_constant_ulong_long_eval_table;
         else if (*root->type == &type_float_vtable)
            root->eval_table = inter_constant_float_eval_table;
         else if (*root->type == &type_double_vtable)
            root->eval_table = inter_constant_double_eval_table;
         else if (*root->type == &type_long_double_vtable)
            root->eval_table = inter_constant_long_double_eval_table;
         else if (*root->type == &type_pointer_vtable)
            root->eval_table = inter_constant_pointer_eval_table;
         else
            root->eval_table = inter_constant_eval_table;

         goto done;
      }

      // No local variables for now
#if 0
      if (*binding->type == &type_local_var_binding_vtable) {
         struct local_var_binding* local_binding = (struct local_var_binding*)binding->val;
         root->type = (*local_binding->var_type)->type_copy(local_binding->var_type);
         root->returns_lval = 1;
         char* stack_top = inter->stack.buffer + inter->stack.stack_top;
         root->constant.i = stack_top - local_binding->var_alloc;
         root->eval_table = inter_local_var_eval_table;
         goto done;
      }
#endif

      root->type = (*binding->type)->type_copy(binding->type);
      root->returns_lval = 1;
      root->eval_table = inter_symbol_eval_table;

      err = type_def_cell_bind_type(inter, root);
      if (err) goto done;

      err = check_root_type(root, expected_type);
      goto done;
   }

   int num_children = dlist_root_length(&root->ast_node.children);

   if (num_children == 0) {
      err = error_syntax(&root->ast_node);
      goto done;
   }

   struct ast_node_with_type* first = (struct ast_node_with_type*)dlist_root_nth(&root->ast_node.children, 0);
   err = inter_type_check_expr(inter, NULL, first);
   assert(err || first->type);
   if (err) {
      goto done;
   }

   if ((*first->type)->type_check_call) {
      err = (*first->type)->type_check_call(inter, expected_type, root);

      if (!root->eval_table.eval_rval) {
#if 1
         if (root->returns_lval) {
            root->eval_table.eval_rval = inter_eval_fallback_rval;
            root->eval_table.eval_lval = (*first->type)->type_eval_call;
         }
         else { 
            root->eval_table.eval_rval = (*first->type)->type_eval_call;
            root->eval_table.eval_lval = inter_eval_fallback_lval;
         }
#else
         if (root->returns_lval) {
            root->eval_table = inter_call_lval_eval_table;
         }
         else { 
            root->eval_table = inter_call_rval_eval_table;
         }
#endif
      }

      if (err) goto done;

      err = type_def_cell_bind_type(inter, root);
      goto done;
   }

   err = error_cannot_call(&first->ast_node, first->type);

done:
#ifdef LOG_TYPE_CHECK
   inter_log_nesting_level--;
   for (space_i = 0; space_i < inter_log_nesting_level; space_i++) {
      LOG(" ")
   }
   LOG("type_check_expr end %s:%d.%d: ",
         root->ast_node.filename, root->ast_node.line, root->ast_node.column)
   ast_node_print(LOG_FILE, &root->ast_node);
   LOG(" => ")
   if (root->type == NULL) {
      LOG("NULL");
   }
   else {
      (*root->type)->type_print(LOG_FILE, root->type);
   }
   LOG("\n")
#endif
   return err;
}


// Set the type field of this node and all of its children to null.
void type_check_trivial(struct ast_node_with_type* root) {
   root->type = NULL;
   root->returns_lval = 0;

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;

   if (first) {
      struct ast_node_with_type* curr = first;
      do {
         type_check_trivial(root);
         curr = (struct ast_node_with_type*)curr->ast_node.node.next;
      } while(curr != first);
   }
}

error type_check_trivial_call(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;

   assert(first);

   struct ast_node_with_type* curr = (struct ast_node_with_type*)first->ast_node.node.next;

   while(curr != first) {
      type_check_trivial(curr);
      struct ast_node_with_type* curr = (struct ast_node_with_type*)curr->ast_node.node.next;
   }

   return NULL;
}


error inter_eval_constant_primitive_lval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   *((void**)value) = (void*)(&root->constant);
   return NULL;
}

#define DEF_EVAL_CONSTANT(TYPENAME,TYPE,FIELD_NAME) \
error inter_eval_constant_##TYPENAME##_rval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) { \
   assert(size == sizeof(TYPE)); \
   *((TYPE*)value) = root->constant.FIELD_NAME; \
   return NULL; \
}




DEF_EVAL_CONSTANT(char,char,c)
DEF_EVAL_CONSTANT(uchar,unsigned char,uc)
DEF_EVAL_CONSTANT(short,short,s)
DEF_EVAL_CONSTANT(ushort,unsigned short,us)
DEF_EVAL_CONSTANT(int,int,i)
DEF_EVAL_CONSTANT(uint,unsigned int,u)
DEF_EVAL_CONSTANT(long,long,l)
DEF_EVAL_CONSTANT(ulong,unsigned long,ul)
DEF_EVAL_CONSTANT(long_long,long long,ll)
DEF_EVAL_CONSTANT(ulong_long,unsigned long long,ull)
DEF_EVAL_CONSTANT(float,float,f)
DEF_EVAL_CONSTANT(double,double,d)
DEF_EVAL_CONSTANT(long_double,long double,ld)
DEF_EVAL_CONSTANT(pointer,void*,p)

struct eval_table inter_constant_char_eval_table = {
   .eval_rval = (type_eval_call_fn)inter_eval_constant_char_rval,
   .eval_lval = (type_eval_call_fn)inter_eval_constant_primitive_lval
};
struct eval_table inter_constant_uchar_eval_table = {
   .eval_rval = (type_eval_call_fn)inter_eval_constant_uchar_rval,
   .eval_lval = (type_eval_call_fn)inter_eval_constant_primitive_lval
};
struct eval_table inter_constant_short_eval_table = {
   .eval_rval = (type_eval_call_fn)inter_eval_constant_short_rval,
   .eval_lval = (type_eval_call_fn)inter_eval_constant_primitive_lval
};
struct eval_table inter_constant_ushort_eval_table = {
   .eval_rval = (type_eval_call_fn)inter_eval_constant_ushort_rval,
   .eval_lval = (type_eval_call_fn)inter_eval_constant_primitive_lval
};
struct eval_table inter_constant_int_eval_table = {
   .eval_rval = (type_eval_call_fn)inter_eval_constant_int_rval,
   .eval_lval = (type_eval_call_fn)inter_eval_constant_primitive_lval
};
struct eval_table inter_constant_uint_eval_table = {
   .eval_rval = (type_eval_call_fn)inter_eval_constant_uint_rval,
   .eval_lval = (type_eval_call_fn)inter_eval_constant_primitive_lval
};
struct eval_table inter_constant_long_eval_table = {
   .eval_rval = (type_eval_call_fn)inter_eval_constant_long_rval,
   .eval_lval = (type_eval_call_fn)inter_eval_constant_primitive_lval
};
struct eval_table inter_constant_ulong_eval_table = {
   .eval_rval = (type_eval_call_fn)inter_eval_constant_ulong_rval,
   .eval_lval = (type_eval_call_fn)inter_eval_constant_primitive_lval
};
struct eval_table inter_constant_long_long_eval_table = {
   .eval_rval = (type_eval_call_fn)inter_eval_constant_long_long_rval,
   .eval_lval = (type_eval_call_fn)inter_eval_constant_primitive_lval
};
struct eval_table inter_constant_ulong_long_eval_table = {
   .eval_rval = (type_eval_call_fn)inter_eval_constant_ulong_long_rval,
   .eval_lval = (type_eval_call_fn)inter_eval_constant_primitive_lval
};
struct eval_table inter_constant_float_eval_table = {
   .eval_rval = (type_eval_call_fn)inter_eval_constant_float_rval,
   .eval_lval = (type_eval_call_fn)inter_eval_constant_primitive_lval
};
struct eval_table inter_constant_double_eval_table = {
   .eval_rval = (type_eval_call_fn)inter_eval_constant_double_rval,
   .eval_lval = (type_eval_call_fn)inter_eval_constant_primitive_lval
};
struct eval_table inter_constant_long_double_eval_table = {
   .eval_rval = (type_eval_call_fn)inter_eval_constant_long_double_rval,
   .eval_lval = (type_eval_call_fn)inter_eval_constant_primitive_lval
};
struct eval_table inter_constant_pointer_eval_table = {
   .eval_rval = (type_eval_call_fn)inter_eval_constant_pointer_rval,
   .eval_lval = (type_eval_call_fn)inter_eval_constant_primitive_lval
};


error inter_eval_constant_rval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   assert((*root->type)->type_parse_constant);
   return (*root->type)->type_parse_constant(root->type, root, value);
}
error inter_eval_constant_lval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   error err;
   void* alloc = stack_alloc(&inter->stack, size);
   assert(alloc);
   err = (*root->type)->type_parse_constant(root->type, root, alloc);
   if (err) return err;
   *((void**)value) = alloc;
   return NULL;
}

struct eval_table inter_constant_eval_table = {
   .eval_rval = (type_eval_call_fn)inter_eval_constant_rval,
   .eval_lval = (type_eval_call_fn)inter_eval_constant_lval
};




error inter_eval_symbol_rval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   assert(root->symbol);
   struct typed_value* binding = (struct typed_value*)hash_map_sym_top_value(root->symbol);

   if (!binding) {
      return error_symbol_undefined(&root->ast_node);
   }

   TYPE_COPY_VAL(root->type, binding->val, value, size)
   return NULL;
}

error inter_eval_symbol_lval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   assert(root->symbol);
   struct typed_value* binding = (struct typed_value*)hash_map_sym_top_value(root->symbol);

   if (!binding) {
      return error_symbol_undefined(&root->ast_node);
   }

   *((void**)value) = (void*)binding->val;
   return NULL;
}

struct eval_table inter_symbol_eval_table = {
   .eval_rval = (type_eval_call_fn)inter_eval_symbol_rval,
   .eval_lval = (type_eval_call_fn)inter_eval_symbol_lval
};

error inter_eval_local_var_rval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   char* stack_top = inter->stack.buffer + inter->stack.stack_top;
   char* var_alloc = stack_top - root->constant.i;
#ifdef LOG_LOCAL
   LOG("inter_eval_local_var_rval: %s.%d at %p\n", root->ast_node.text, root->constant.i, var_alloc)
#endif

   TYPE_COPY_VAL(root->type, var_alloc, value, size)
   return NULL;
}
error inter_eval_local_var_lval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   char* stack_top = inter->stack.buffer + inter->stack.stack_top;
   char* var_alloc = stack_top - root->constant.i;
#ifdef LOG_LOCAL
   LOG("inter_eval_local_var_lval: %s.%d at %p\n", root->ast_node.text, root->constant.i, var_alloc)
#endif
   *((char**)value) = var_alloc;
   return NULL;
}

struct eval_table inter_local_var_eval_table = {
   .eval_rval = (type_eval_call_fn)inter_eval_local_var_rval,
   .eval_lval = (type_eval_call_fn)inter_eval_local_var_lval
};



error inter_eval_call_matching_val(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   assert(first);
   assert((*first->type)->type_eval_call);

   return (*first->type)->type_eval_call(inter, root, value, size);
}
error inter_eval_call_rval_to_lval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   error err;
   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   assert(first);
   assert((*first->type)->type_eval_call);

   void* alloc = stack_alloc(&inter->stack, size);
   assert(alloc);
   err = (*first->type)->type_eval_call(inter, root, alloc, size);
   if (err) return err;
   *((void**)value) = alloc;
   return NULL;
}
error inter_eval_call_lval_to_rval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   error err;
   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   assert(first);
   assert((*first->type)->type_eval_call);

   void* ptr;
   err = (*first->type)->type_eval_call(inter, root, &ptr, size);
   if (err) return err;

   TYPE_COPY_VAL(root->type, ptr, value, size)
   return NULL;
}

struct eval_table inter_call_rval_eval_table = {
   .eval_rval = (type_eval_call_fn)inter_eval_call_matching_val,
   .eval_lval = (type_eval_call_fn)inter_eval_call_rval_to_lval
};

struct eval_table inter_call_lval_eval_table = {
   .eval_rval = (type_eval_call_fn)inter_eval_call_lval_to_rval,
   .eval_lval = (type_eval_call_fn)inter_eval_call_matching_val
};


error inter_eval_fallback_lval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   error err;
   void* alloc = stack_alloc(&inter->stack, size);
   assert(alloc);
   err = root->eval_table.eval_rval(inter, root, alloc, size);
   if (err) return err;
   *((void**)value) = alloc;
   return NULL;
}

error inter_eval_fallback_rval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   error err;
   void* ptr;
   err = root->eval_table.eval_lval(inter, root, &ptr, size);
   if (err) return err;

   TYPE_COPY_VAL(root->type, ptr, value, size)
   return NULL;
}



#ifndef EVAL_MACRO


error inter_eval_rval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   error err;

#ifdef LOG_STACK
   LOG("inter stack pos: %d,%p\n", inter->stack.stack_top, inter->stack.buffer + inter->stack.stack_top)
#endif

#ifdef LOG_EVAL
   int space_i;
   for (space_i = 0; space_i < inter_log_nesting_level; space_i++) {
      LOG(" ")
   }
   LOG("inter_eval_rval start %s:%d.%d: ",
         root->ast_node.filename, root->ast_node.line, root->ast_node.column)
   ast_node_print(LOG_FILE, &root->ast_node);
   LOG("\n")
   inter_log_nesting_level++;
#endif

#ifdef DEBUG_SYMBOL
      debug_symbol(inter);
#endif

   err = root->eval_table.eval_rval(inter, root, value, size);

done:
#ifdef LOG_EVAL
   inter_log_nesting_level--;
   for (space_i = 0; space_i < inter_log_nesting_level; space_i++) {
      LOG(" ")
   }
   LOG("inter_eval_rval end %s:%d.%d: ",
         root->ast_node.filename, root->ast_node.line, root->ast_node.column)
   ast_node_print(LOG_FILE, &root->ast_node);
   LOG(" => ")
   if (!err)
      (*root->type)->type_print_val(LOG_FILE, root->type, value);
   else
      (*err)->error_print(LOG_FILE, err);
   LOG("\n")
#endif
   return err;
}


error inter_eval_lval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   error err;

#ifdef LOG_STACK
   LOG("inter stack pos: %d,%p\n", inter->stack.stack_top, inter->stack.buffer + inter->stack.stack_top)
#endif

#ifdef LOG_EVAL
   int space_i;
   for (space_i = 0; space_i < inter_log_nesting_level; space_i++) {
      LOG(" ")
   }
   LOG("inter_eval_lval start %s:%d.%d: ",
         root->ast_node.filename, root->ast_node.line, root->ast_node.column)
   ast_node_print(LOG_FILE, &root->ast_node);
   LOG("\n")
#endif

#ifdef DEBUG_SYMBOL
      debug_symbol(inter);
#endif

   err = root->eval_table.eval_lval(inter, root, value, size);

done:
#ifdef LOG_EVAL
   for (space_i = 0; space_i < inter_log_nesting_level; space_i++) {
      LOG(" ")
   }
   LOG("inter_eval_lval done %s:%d.%d: ",
         root->ast_node.filename, root->ast_node.line, root->ast_node.column)
   ast_node_print(LOG_FILE, &root->ast_node);
   LOG(" => ")
   if (!err)
      (*root->type)->type_print_val(LOG_FILE, root->type, *((void**)value));
   else
      (*err)->error_print(LOG_FILE, err);
   LOG("\n")
#endif
   return err;
}


error inter_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   if (root->returns_lval) {
      return inter_eval_lval(inter, root, value, size);
   }
   else {
      return inter_eval_rval(inter, root, value, size);
   }
}

#endif // ifndef EVAL_MACRO



#define DO_TYPES(M) \
   M("void", type_construct_void) \
   M("char", type_construct_char) \
   M("uchar", type_construct_uchar) \
   M("short", type_construct_short) \
   M("ushort", type_construct_ushort) \
   M("int", type_construct_int) \
   M("uint", type_construct_uint) \
   M("long", type_construct_long) \
   M("ulong", type_construct_ulong) \
   M("longlong", type_construct_long_long) \
   M("ulonglong", type_construct_ulong_long) \
   M("float", type_construct_float) \
   M("double", type_construct_double) \
   M("longdouble", type_construct_long_double) \
   M("void*", type_construct_pointer) \
   M("str", type_construct_string) \
   M("str-auto", type_construct_string_auto) \
   M("str-refc", type_construct_string_refc) \
   M("symbol", type_construct_symbol) \
   M("type", type_construct_type) \
   M("*", type_construct_pointer_to) \
   M("auto*", type_construct_auto_pointer_to) \
   M("refc*", type_construct_refc_pointer_to) \
   M("array", type_construct_array) \
   M("c-fun", type_construct_c_function) \
   M("buf-fun", type_construct_buf_function) \
   M("c-macro", type_construct_c_macro) \
   M("code", type_construct_code) \
   M("code-unsafe", type_construct_code_unsafe) \
   M("quote", type_construct_quote) \
   M("struct", type_construct_struct) \
   M("prefix", type_construct_prefix) \
   M("function", type_construct_script_function) \
   M("macro", type_construct_macro) \


#define DEF_TYPE(key, value) \
   key_sym = inter_intern_sym(inter, key, strlen(key)); \
   inter_def_type_cons(inter, key_sym, strlen(key_sym), value);

#define UNDEF_TYPE(key, value) \
   key_sym = inter_intern_sym(inter, key, strlen(key)); \
   inter_undef_type_cons(inter, key, strlen(key));


#define AUTO_DETECT_STDINT_TYPE(BYTES, TYPE_STR, UNSIGNED_TYPE_STR) \
   if (sizeof(char) == BYTES) { \
      DEF_TYPE(TYPE_STR, type_construct_char) \
      DEF_TYPE(UNSIGNED_TYPE_STR, type_construct_uchar) \
   } \
   else if (sizeof(int) == BYTES) { \
      DEF_TYPE(TYPE_STR, type_construct_int) \
      DEF_TYPE(UNSIGNED_TYPE_STR, type_construct_uint) \
   } \
   else if (sizeof(short) == BYTES) { \
      DEF_TYPE(TYPE_STR, type_construct_short) \
      DEF_TYPE(UNSIGNED_TYPE_STR, type_construct_ushort) \
   } \
   else if (sizeof(long) == BYTES) { \
      DEF_TYPE(TYPE_STR, type_construct_long) \
      DEF_TYPE(UNSIGNED_TYPE_STR, type_construct_ulong) \
   } \
   else if (sizeof(long long) == BYTES) { \
      DEF_TYPE(TYPE_STR, type_construct_long_long) \
      DEF_TYPE(UNSIGNED_TYPE_STR, type_construct_ulong_long) \
   } \
   else { \
      assert(0); \
   }


void inter_def_builtin_types(struct inter* inter) {
   char* key;
   int len;
   char* key_sym;
   DO_TYPES(DEF_TYPE)


   // stdint types.
   AUTO_DETECT_STDINT_TYPE(1, "int8", "uint8")
   AUTO_DETECT_STDINT_TYPE(2, "int16", "uint16")
   AUTO_DETECT_STDINT_TYPE(4, "int32", "uint32")
   AUTO_DETECT_STDINT_TYPE(8, "int64", "uint64")
}

void inter_undef_builtin_types(struct inter* inter) {
   char* key_sym;
   DO_TYPES(UNDEF_TYPE)

   UNDEF_TYPE("int8", NULL)
   UNDEF_TYPE("uint8", NULL)
   UNDEF_TYPE("int16", NULL)
   UNDEF_TYPE("uint16", NULL)
   UNDEF_TYPE("int32", NULL)
   UNDEF_TYPE("uint32", NULL)
   UNDEF_TYPE("int64", NULL)
   UNDEF_TYPE("uint64", NULL)
}
