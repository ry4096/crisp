#include <assert.h>
#include <dlfcn.h>
#include <malloc.h>
#include <string.h>


#include "inter.h"
#include "inter_type.h"
#include "inter_dl.h"


error inter_dl_open_check(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   error err;
   int num_children = dlist_root_length(&root->ast_node.children);
   if (num_children != 3) {
      return error_wrong_number_of_args(&root->ast_node, 2, num_children - 1);
   }
   
   struct ast_node_with_type* name_node = (struct ast_node_with_type*)dlist_root_nth(&root->ast_node.children, 1);
   struct ast_node_with_type* flags_node = (struct ast_node_with_type*)dlist_root_nth(&root->ast_node.children, 2);

   err = inter_type_check_expr(inter, &type_string, name_node);
   if (err) return err;

   return inter_type_check_expr(inter, &type_int, flags_node);
}

error inter_dl_open_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   error err;
   assert(dlist_root_length(&root->ast_node.children) == 3);

   struct ast_node_with_type* name_node = (struct ast_node_with_type*)dlist_root_nth(&root->ast_node.children, 1);
   struct ast_node_with_type* flags_node = (struct ast_node_with_type*)dlist_root_nth(&root->ast_node.children, 2);

   char* name;
   int flags;

   err = inter_eval_rval(inter, name_node, &name, sizeof(name));
   if (err) return err;

   err = inter_eval_rval(inter, flags_node, &flags, sizeof(flags));
   if (err) return err;

   *((void**)value) = dlopen(name, flags);

   return NULL;
}


error inter_dl_error_check(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   int num_children = dlist_root_length(&root->ast_node.children);
   if (num_children != 1) {
      return error_wrong_number_of_args(&root->ast_node, 0, num_children - 1);
   }
   
   return NULL;
}

error inter_dl_error_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   assert(dlist_root_length(&root->ast_node.children) == 1);

   *((char**)value) = dlerror();

   return NULL;
}


error inter_dl_sym_check(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   error err;
   int num_children = dlist_root_length(&root->ast_node.children);
   if (num_children != 3) {
      return error_wrong_number_of_args(&root->ast_node, 2, num_children - 1);
   }
   
   struct ast_node_with_type* handle_node = (struct ast_node_with_type*)dlist_root_nth(&root->ast_node.children, 1);
   struct ast_node_with_type* sym_node = (struct ast_node_with_type*)dlist_root_nth(&root->ast_node.children, 2);

   err = inter_type_check_expr(inter, &type_pointer, handle_node);
   if (err) return err;

   return inter_type_check_expr(inter, &type_string, sym_node);
}

error inter_dl_sym_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   error err;
   assert(dlist_root_length(&root->ast_node.children) == 3);

   struct ast_node_with_type* handle_node = (struct ast_node_with_type*)dlist_root_nth(&root->ast_node.children, 1);
   struct ast_node_with_type* sym_node = (struct ast_node_with_type*)dlist_root_nth(&root->ast_node.children, 2);

   void* handle;
   char* sym;

   err = inter_eval_rval(inter, handle_node, &handle, sizeof(handle));
   if (err) return err;

   err = inter_eval_rval(inter, sym_node, &sym, sizeof(sym));
   if (err) return err;

   *((void**)value) = dlsym(handle, sym);

   return NULL;
}


error inter_dl_def_group_check(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   error err;
   int num_children = dlist_root_length(&root->ast_node.children);
   if (num_children < 2) {
      return error_wrong_number_of_args(&root->ast_node, 1, num_children - 1);
   }
   
   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* handle_node = (struct ast_node_with_type*)first->ast_node.node.next;
   struct ast_node_with_type* def_node = (struct ast_node_with_type*)handle_node->ast_node.node.next;

   err = inter_type_check_expr(inter, &type_pointer, handle_node);
   if (err) return err;

   while (def_node != first) {
      if (def_node->ast_node.state != TOKEN_STATE_PUNCTUATION) {
         return error_syntax(&def_node->ast_node);
      }

      int curr_children = dlist_root_length(&def_node->ast_node.children);

      if (curr_children != 3) {
         return error_wrong_number_of_args(&def_node->ast_node, 3, curr_children);
      }

      struct ast_node_with_type* name_node = (struct ast_node_with_type*)def_node->ast_node.children.entry_node;
      struct ast_node_with_type* sym_node = (struct ast_node_with_type*)name_node->ast_node.node.next;
      struct ast_node_with_type* type_node = (struct ast_node_with_type*)sym_node->ast_node.node.next;

      if (name_node->ast_node.state != TOKEN_STATE_SYMBOL) {
         return error_syntax(&name_node->ast_node);
      }

      inter_cache_value_symbol(inter, name_node);

      type_node->type = type_build_ast_node(inter, &type_node->ast_node, &err);
      if (err) return err;

      type type_binding;
      TYPE_DEF_CELL_GET_TYPE(type_node->type, &type_node->ast_node, type_binding, err)
      if (err) return err;

      int type_size = (*type_binding)->type_size(type_binding);

      if (type_size != sizeof(void*)) {
         return error_cannot_cast(&def_node->ast_node, type_node->type, &type_pointer);
      }

      if (sym_node->ast_node.state != TOKEN_STATE_SYMBOL) {
         return error_syntax(&sym_node->ast_node);
      }

      def_node = (struct ast_node_with_type*)def_node->ast_node.node.next;
   }

   return NULL;
}

error inter_dl_def_group_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   error err;
   assert(dlist_root_length(&root->ast_node.children) >= 2);

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* handle_node = (struct ast_node_with_type*)first->ast_node.node.next;
   struct ast_node_with_type* def_node = (struct ast_node_with_type*)handle_node->ast_node.node.next;

   void* handle;

   err = inter_eval_rval(inter, handle_node, &handle, sizeof(handle));
   if (err) return err;

   while (def_node != first) {
      assert(dlist_root_length(&def_node->ast_node.children) == 3);

      struct ast_node_with_type* name_node = (struct ast_node_with_type*)def_node->ast_node.children.entry_node;
      struct ast_node_with_type* sym_node = (struct ast_node_with_type*)name_node->ast_node.node.next;
      struct ast_node_with_type* type_node = (struct ast_node_with_type*)sym_node->ast_node.node.next;

      struct typed_value* alloc = stack_alloc(&inter->stack, sizeof(struct typed_value) + sizeof(void*));
      alloc->type = (*type_node->type)->type_copy(type_node->type);

      void* val = dlsym(handle, sym_node->ast_node.text);

      if (!val) {
         return error_symbol_undefined(&sym_node->ast_node);
      }

      *((void**)alloc->val) = val;

      hash_map_sym_push_value(name_node->symbol, alloc);

      def_node = (struct ast_node_with_type*)def_node->ast_node.node.next;
   }

   return NULL;
}


error inter_dl_close_check(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   int num_children = dlist_root_length(&root->ast_node.children);
   if (num_children != 2) {
      return error_wrong_number_of_args(&root->ast_node, 1, num_children - 1);
   }
   
   struct ast_node_with_type* handle_node = (struct ast_node_with_type*)dlist_root_nth(&root->ast_node.children, 1);

   return inter_type_check_expr(inter, &type_pointer, handle_node);
}

error inter_dl_close_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   error err;
   assert(dlist_root_length(&root->ast_node.children) == 2);

   struct ast_node_with_type* handle_node = (struct ast_node_with_type*)dlist_root_nth(&root->ast_node.children, 1);

   void* handle;

   err = inter_eval_rval(inter, handle_node, &handle, sizeof(handle));
   if (err) return err;

   *((int*)value) = dlclose(handle);

   return NULL;
}



#define DO_MACROS(M) \
   M("dl.open", inter_dl_open_check, &type_pointer, inter_dl_open_eval) \
   M("dl.error", inter_dl_error_check, &type_string, inter_dl_error_eval) \
   M("dl.sym", inter_dl_sym_check, &type_pointer, inter_dl_sym_eval) \
   M("dl.def-group", inter_dl_def_group_check, &type_void, inter_dl_def_group_eval) \
   M("dl.close", inter_dl_close_check, &type_int, inter_dl_close_eval) \


//   value = (struct typed_value*)malloc(sizeof(struct typed_value) + sizeof(type_eval_call_fn)); \
//   assert(value); \


#define DEF_MACRO(name, check, return_type, eval) \
   name_sym = inter_intern_sym(inter, name, strlen(name)); \
   macro_type = type_c_macro(check, return_type); \
   value = (struct typed_value*)stack_alloc(&inter->stack, sizeof(struct typed_value) + sizeof(type_eval_call_fn)); \
   assert(value); \
   value->type = macro_type; \
   *((type_eval_call_fn*)value->val) = eval; \
   inter_def_value(inter, name_sym, strlen(name), value);


#define UNDEF_MACRO(name, check, return_type, eval) \
   name_sym = inter_intern_sym(inter, name, strlen(name)); \
   value = inter_undef_value(inter, name, strlen(name)); \
   typed_value_destruct(value, inter); \

//   free(value);


void inter_def_dl_funcs(struct inter* inter) {
   char* name_sym;
   type macro_type;
   struct typed_value* value;
   type* arguments;

   DO_MACROS(DEF_MACRO)
}

void inter_undef_dl_funcs(struct inter* inter) {
   char* name_sym;
   struct typed_value* value;

   DO_MACROS(UNDEF_MACRO)
}




