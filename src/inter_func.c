
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <malloc.h>
#include <alloca.h>

#include "path.h"
#include "hash_table.h"
#include "hash_map_push.h"
#include "inter_type.h"
#include "inter_func.h"
#include "list.h"
#include "refc.h"
#include "bits.h"

#define LOGGING_OFF

#include "log.h"

//#define LOG_IF
//#define LOG_WHILE
//#define LOG_STACK_POS
//#define LOG_IMPORT

//#define DEBUG_SYMBOL "self"


char* tok_stack = "stack";
#define TOK_STACK_LEN 5

char* tok_heap = "heap";
#define TOK_HEAP_LEN 4

char* tok_refc = "refc";
#define TOK_REFC_LEN 4

char* tok_unquote = "unquote";
char* tok_unquote_abrv = ",";
char* tok_splice = "splice";
char* tok_splice_abrv = "@";

char* source_extension = ".cr";

error inter_func_check_args(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   error err;

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;

   struct ast_node_with_type* curr = (struct ast_node_with_type*)first->ast_node.node.next;
   while(curr != first) {
      err = inter_type_check_expr(inter, NULL, curr);
      if (err) return err;
      curr = (struct ast_node_with_type*)curr->ast_node.node.next;
   }

   return NULL;
}


error inter_check_alloc_node(struct ast_node_with_type* alloc_node) {
   if (alloc_node->ast_node.text_len == TOK_STACK_LEN &&
       !memcmp(alloc_node->ast_node.text, tok_stack, TOK_STACK_LEN)) {
   }
   else if (alloc_node->ast_node.text_len == TOK_HEAP_LEN &&
            !memcmp(alloc_node->ast_node.text, tok_heap, TOK_HEAP_LEN)) {
   }
   else if (alloc_node->ast_node.text_len == TOK_REFC_LEN &&
            !memcmp(alloc_node->ast_node.text, tok_refc, TOK_REFC_LEN)) {
   }
   else {
      return error_syntax(&alloc_node->ast_node);
   }

   return NULL;
}

void* inter_eval_alloc_node(struct inter* inter, struct ast_node_with_type* alloc_node, int alloc_size, error* err) {
   void* alloc;
   if (alloc_node->ast_node.text_len == TOK_STACK_LEN &&
       !memcmp(alloc_node->ast_node.text, tok_stack, TOK_STACK_LEN)) {
      alloc = stack_alloc(&inter->stack, alloc_size);
      LOG("inter_eval_alloc_node: stack: %d,%p,%p\n", alloc_size, alloc, &((struct typed_value*)alloc)->val);
      if (!alloc) {
         *err = error_stack_overflow(&alloc_node->ast_node);
      }
   }
   else if (alloc_node->ast_node.text_len == TOK_HEAP_LEN &&
            !memcmp(alloc_node->ast_node.text, tok_heap, TOK_HEAP_LEN)) {
      alloc = malloc(alloc_size);
      assert(alloc);
      // Can't recover meaningfully for now.
   }
   else if (alloc_node->ast_node.text_len == TOK_REFC_LEN &&
            !memcmp(alloc_node->ast_node.text, tok_refc, TOK_REFC_LEN)) {
      alloc = refc_alloc(alloc_size);
      assert(alloc);
      // Can't recover meaningfully for now.
   }
   else {
      assert(0);
   }

   return alloc;
}

void inter_do_free_alloc_node(struct inter* inter, struct ast_node_with_type* alloc_node, struct typed_value* alloc) {
   if (alloc_node->ast_node.text_len == TOK_STACK_LEN &&
       !memcmp(alloc_node->ast_node.text, tok_stack, TOK_STACK_LEN)) {
      int alloc_size = typed_value_size(alloc->type);
      typed_value_destruct(alloc, inter);
      stack_decrease_top(&inter->stack, alloc_size);
   }
   else if (alloc_node->ast_node.text_len == TOK_HEAP_LEN &&
            !memcmp(alloc_node->ast_node.text, tok_heap, TOK_HEAP_LEN)) {
      typed_value_destruct(alloc, inter);
      free(alloc);
   }
   else if (alloc_node->ast_node.text_len == TOK_REFC_LEN &&
            !memcmp(alloc_node->ast_node.text, tok_refc, TOK_REFC_LEN)) {
      if (!refc_dec(alloc)) {
         typed_value_destruct(alloc, inter);
         refc_free(alloc);
      }
   }
   else {
      assert(0);
   }
}



error inter_func_print_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   error err = NULL;
   assert(size == 0);
   
   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* curr = (struct ast_node_with_type*)first->ast_node.node.next;

   while(!err && curr != first) {
      int stack_top = inter->stack.stack_top;

      int arg_size = (*curr->type)->type_size(curr->type);
      void* val;

      err = inter_eval_lval(inter, curr, &val, arg_size);
      if (!err) {
         (*curr->type)->type_print_val(stdout, curr->type, val);

         if (!curr->returns_lval) {
            inter_destruct_val(inter, curr->type, val);
         }
      }

      inter->stack.stack_top = stack_top;

      curr = (struct ast_node_with_type*)curr->ast_node.node.next;
   }

   return err;
}


error inter_func_char_check(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   int num_children = dlist_root_length(&root->ast_node.children);

   root->returns_lval = 0;
   root->eval_table.eval_rval = inter_func_char_eval;
   root->eval_table.eval_lval = inter_eval_fallback_lval;

   if (num_children != 2) {
      return error_wrong_number_of_args(&root->ast_node, 1, num_children - 1);
   }

   struct ast_node_with_type* char_node = (struct ast_node_with_type*)dlist_root_nth(&root->ast_node.children, 1);
   if (char_node->ast_node.state != TOKEN_STATE_SYMBOL) {
      return error_syntax(&char_node->ast_node);
   }

   if (char_node->ast_node.text_len != 1) {
      return error_syntax(&char_node->ast_node);
   }

   char_node->type = NULL;
   char_node->returns_lval = 0;
   return NULL;
}

error inter_func_char_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   assert(dlist_root_length(&root->ast_node.children) == 2);
   struct ast_node_with_type* char_node = (struct ast_node_with_type*)dlist_root_nth(&root->ast_node.children, 1);
   assert(char_node->ast_node.state == TOKEN_STATE_SYMBOL);
   assert(char_node->ast_node.text_len == 1);
   assert(size == sizeof(char));
   *((char*)value) = char_node->ast_node.text[0];
   return NULL;
}

error inter_func_uchar_check(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   int num_children = dlist_root_length(&root->ast_node.children);

   root->returns_lval = 0;
   root->eval_table.eval_rval = inter_func_uchar_eval;
   root->eval_table.eval_lval = inter_eval_fallback_lval;

   if (num_children != 2) {
      return error_wrong_number_of_args(&root->ast_node, 1, num_children - 1);
   }

   struct ast_node_with_type* char_node = (struct ast_node_with_type*)dlist_root_nth(&root->ast_node.children, 1);
   if (char_node->ast_node.state != TOKEN_STATE_SYMBOL) {
      return error_syntax(&char_node->ast_node);
   }

   if (char_node->ast_node.text_len != 1) {
      return error_syntax(&char_node->ast_node);
   }

   char_node->type = NULL;
   char_node->returns_lval = 0;
   return NULL;
}

error inter_func_uchar_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   assert(dlist_root_length(&root->ast_node.children) == 2);
   struct ast_node_with_type* char_node = (struct ast_node_with_type*)dlist_root_nth(&root->ast_node.children, 1);
   assert(char_node->ast_node.state == TOKEN_STATE_SYMBOL);
   assert(char_node->ast_node.text_len == 1);
   assert(size == sizeof(char));
   *((unsigned char*)value) = char_node->ast_node.text[0];
   return NULL;
}

error inter_func_string_check(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   error err;
   int num_children = dlist_root_length(&root->ast_node.children);

   root->returns_lval = 0;
   root->eval_table.eval_rval = inter_func_string_eval;
   root->eval_table.eval_lval = inter_eval_fallback_lval;


   if (num_children != 3 && num_children != 2) {
      return error_wrong_number_of_args(&root->ast_node, 2, num_children - 1);
   }

   if (num_children == 3) {
      struct ast_node_with_type* alloc_node = (struct ast_node_with_type*)dlist_root_nth(&root->ast_node.children, 1);
      struct ast_node_with_type* string_node = (struct ast_node_with_type*)dlist_root_nth(&root->ast_node.children, 2);

      if (string_node->ast_node.state != TOKEN_STATE_SYMBOL) {
         return error_syntax(&string_node->ast_node);
      }

      err = inter_check_alloc_node(alloc_node);
      if (err) return err;

      if (alloc_node->ast_node.text_len == TOK_REFC_LEN &&
          !memcmp(alloc_node->ast_node.text, tok_refc, TOK_REFC_LEN)) {
         root->type = &type_string_refc;
      }
      else {
         root->type = &type_string;
      }
   }
   else {
      struct ast_node_with_type* string_node = (struct ast_node_with_type*)dlist_root_nth(&root->ast_node.children, 1);

      if (string_node->ast_node.state != TOKEN_STATE_SYMBOL) {
         return error_syntax(&string_node->ast_node);
      }

      root->type = &type_symbol;
   }

   return check_root_type(root, expected_type);
}

error inter_func_string_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   error err = NULL;

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* alloc_node = (struct ast_node_with_type*)first->ast_node.node.next;
   struct ast_node_with_type* string_node = (struct ast_node_with_type*)alloc_node->ast_node.node.next;

   if (string_node == first) {
      string_node = alloc_node;
      alloc_node = NULL;
   }

   if (alloc_node) {
      assert(size == sizeof(char*));
      assert(alloc_node->ast_node.state == TOKEN_STATE_SYMBOL);
      assert(string_node->ast_node.state == TOKEN_STATE_SYMBOL);

      char* alloc = (char*)inter_eval_alloc_node(inter, alloc_node, string_node->ast_node.text_len + 1, &err);
      if (err) return err;
 
      if (alloc != string_node->ast_node.text)
         memcpy(alloc, string_node->ast_node.text, string_node->ast_node.text_len);
      alloc[string_node->ast_node.text_len] = '\0';
      *((char**)value) = (char*)alloc;
      return NULL;
   }
   else {
      assert(size == sizeof(char*));
      assert(string_node->ast_node.state == TOKEN_STATE_SYMBOL);

      (*root->type)->type_copy_val(root->type, &string_node->ast_node.text, value);
      return NULL;
   }
}

error inter_func_if_check(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   error err;
   int num_children = dlist_root_length(&root->ast_node.children);

   root->returns_lval = 0;
   root->eval_table.eval_rval = inter_func_if_eval;
   root->eval_table.eval_lval = inter_eval_fallback_lval;

   if (num_children != 3 && num_children != 4) {
      return error_wrong_number_of_args(&root->ast_node, 3, num_children - 1);
   }

   if (num_children == 4) {
      struct ast_node_with_type* condition_node = (struct ast_node_with_type*)dlist_root_nth(&root->ast_node.children, 1);
      struct ast_node_with_type* true_node = (struct ast_node_with_type*)dlist_root_nth(&root->ast_node.children, 2);
      struct ast_node_with_type* false_node = (struct ast_node_with_type*)dlist_root_nth(&root->ast_node.children, 3);

      err = inter_type_check_expr(inter, &type_int, condition_node);
      if (err) return err;

      err = inter_type_check_expr(inter, expected_type, true_node);
      if (err) return err;

      err = inter_type_check_expr(inter, expected_type, false_node);
      if (err) return err;

      if (!expected_type) {
         if (!(*true_node->type)->type_match(true_node->type, false_node->type)) {
            return error_type_mismatch(&false_node->ast_node, true_node->type, false_node->type);
         }
      }

      root->type = (*true_node->type)->type_copy(true_node->type);

      if (true_node->returns_lval && false_node->returns_lval) {
         root->returns_lval = 1;
         root->eval_table.eval_rval = inter_eval_fallback_rval;
         root->eval_table.eval_lval = inter_func_if_2_eval;
      }
      else {
         root->returns_lval = 0;
         root->eval_table.eval_rval = inter_func_if_2_eval;
         root->eval_table.eval_lval = inter_eval_fallback_lval;
      }
   }
   else {
      struct ast_node_with_type* condition_node = (struct ast_node_with_type*)dlist_root_nth(&root->ast_node.children, 1);
      struct ast_node_with_type* true_node = (struct ast_node_with_type*)dlist_root_nth(&root->ast_node.children, 2);

      err = inter_type_check_expr(inter, &type_int, condition_node);
      if (err) return err;

      err = inter_type_check_expr(inter, &type_void, true_node);
      if (err) return err;

      root->type = &type_void;

      root->returns_lval = 0;
      root->eval_table.eval_rval = inter_func_if_1_eval;
      root->eval_table.eval_lval = inter_eval_fallback_lval;


      err = check_root_type(root, expected_type);
      if (err) return err;
   }

   return NULL;
}

error inter_func_if_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   error err;

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* condition_node = (struct ast_node_with_type*)first->ast_node.node.next;
   struct ast_node_with_type* true_node = (struct ast_node_with_type*)condition_node->ast_node.node.next;
   struct ast_node_with_type* false_node = (struct ast_node_with_type*)true_node->ast_node.node.next;

   if (false_node == first) {
      false_node = NULL;
   }

   int condition_res;
   err = inter_eval_rval(inter, condition_node, &condition_res, sizeof(condition_res));
   if (err) return err;

   struct ast_node_with_type* eval_node = condition_res ? true_node : false_node;

#ifdef LOG_IF
   if (condition_res) {
      LOG("if: %d.%d taking true branch\n",
            root->ast_node.line, root->ast_node.column);
   }
   else {
      LOG("if: %d.%d taking false branch\n",
            root->ast_node.line, root->ast_node.column);
   }
#endif

   if (eval_node) {
      if (root->returns_lval) {
         return inter_eval_lval(inter, eval_node, value, size);
      }
      else {
         return inter_eval_rval(inter, eval_node, value, size);
      }
   }
}

error inter_func_if_2_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   error err;

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* condition_node = (struct ast_node_with_type*)first->ast_node.node.next;
   struct ast_node_with_type* true_node = (struct ast_node_with_type*)condition_node->ast_node.node.next;
   struct ast_node_with_type* false_node = (struct ast_node_with_type*)true_node->ast_node.node.next;

   int condition_res;
   err = inter_eval_rval(inter, condition_node, &condition_res, sizeof(condition_res));
   if (err) return err;

   struct ast_node_with_type* eval_node = condition_res ? true_node : false_node;

   if (root->returns_lval) {
      return inter_eval_lval(inter, eval_node, value, size);
   }
   else {
      return inter_eval_rval(inter, eval_node, value, size);
   }
}

error inter_func_if_1_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   error err;

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* condition_node = (struct ast_node_with_type*)first->ast_node.node.next;
   struct ast_node_with_type* true_node = (struct ast_node_with_type*)condition_node->ast_node.node.next;

   int condition_res;
   err = inter_eval_rval(inter, condition_node, &condition_res, sizeof(condition_res));
   if (err) return err;

   if (root->returns_lval) {
      return inter_eval_lval(inter, true_node, value, size);
   }
   else {
      return inter_eval_rval(inter, true_node, value, size);
   }
}


error inter_func_defined_check(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   int num_children = dlist_root_length(&root->ast_node.children);

   root->returns_lval = 0;
   root->eval_table.eval_rval = inter_func_defined_eval;
   root->eval_table.eval_lval = inter_eval_fallback_lval;

   if (num_children != 2) {
      return error_wrong_number_of_args(&root->ast_node, 1, num_children - 1);
   }

   struct ast_node_with_type* sym_node = (struct ast_node_with_type*)dlist_root_nth(&root->ast_node.children, 1);

   if (sym_node->ast_node.state != TOKEN_STATE_SYMBOL) {
      return error_syntax(&sym_node->ast_node);
   }

   inter_cache_find_value_symbol(inter, sym_node);

   return NULL;
}

error inter_func_defined_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* sym_node = (struct ast_node_with_type*)first->ast_node.node.next;

   *((int*)value) = (sym_node->symbol != NULL);

   return NULL;
}




error inter_func_def_check(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   error err = NULL;
   int num_children = dlist_root_length(&root->ast_node.children);

   root->returns_lval = 0;
   root->eval_table.eval_rval = inter_func_def_eval;
   root->eval_table.eval_lval = inter_eval_fallback_lval;

   if (num_children != 4 && num_children != 3) {
      return error_wrong_number_of_args(&root->ast_node, 3, num_children - 1);
   }

   struct ast_node_with_type* name_node = (struct ast_node_with_type*)dlist_root_nth(&root->ast_node.children, 1);
   struct ast_node_with_type* type_node = NULL;
   struct ast_node_with_type* alloc_node = (struct ast_node_with_type*)dlist_root_nth(&root->ast_node.children, 2);
   struct ast_node_with_type* value_node = (struct ast_node_with_type*)dlist_root_nth(&root->ast_node.children, 3);

   type expected_value_type = NULL;

   if (name_node->ast_node.state != TOKEN_STATE_SYMBOL) {
      int num_name_children = dlist_root_length(&name_node->ast_node.children);

      if (num_name_children != 2) {
         return error_syntax(&name_node->ast_node);
      }

      type_node = (struct ast_node_with_type*)dlist_root_nth(&name_node->ast_node.children, 1);
      name_node = (struct ast_node_with_type*)dlist_root_nth(&name_node->ast_node.children, 0);
   }

   if (name_node->ast_node.state != TOKEN_STATE_SYMBOL) {
      return error_syntax(&name_node->ast_node);
   }

   inter_cache_value_symbol(inter, name_node);

   if (type_node) {
      expected_value_type = type_build_ast_node(inter, &type_node->ast_node, &err);
      if (err) return err;

      type_node->type = expected_value_type;
      type_node->returns_lval = 0;
   }

   err = inter_check_alloc_node(alloc_node);
   if (err) return err;

   if (num_children == 3) {
      if (expected_value_type == NULL) {
         return error_wrong_number_of_args(&root->ast_node, 3, num_children - 1);
      }
   }
   else {
      err = inter_type_check_expr(inter, expected_value_type, value_node);
      if (err) return err;
   }

   return NULL;
}

error inter_func_def_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   error err = NULL;

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* name_node = (struct ast_node_with_type*)first->ast_node.node.next;
   struct ast_node_with_type* alloc_node = (struct ast_node_with_type*)name_node->ast_node.node.next;
   struct ast_node_with_type* value_node = (struct ast_node_with_type*)alloc_node->ast_node.node.next;
   struct ast_node_with_type* type_node = NULL;

   if (value_node == first) {
      value_node = NULL;
   }

   if (name_node->ast_node.state != TOKEN_STATE_SYMBOL) {
      name_node = (struct ast_node_with_type*)name_node->ast_node.children.entry_node;
      type_node = (struct ast_node_with_type*)name_node->ast_node.node.next;
   }

   struct typed_value* alloc;
   if (value_node) {
      int alloc_size = typed_value_size(value_node->type);
      int value_size = (*value_node->type)->type_size(value_node->type);
      alloc = (struct typed_value*)inter_eval_alloc_node(inter, alloc_node, alloc_size, &err);
      if (err) return err;

      if (type_node) 
         alloc->type = (*type_node->type)->type_copy(type_node->type);
      else 
         alloc->type = (*value_node->type)->type_copy(value_node->type);

      err = inter_eval_rval(inter, value_node, alloc->val, value_size);
      if (err) return err;
   }
   else {
      assert(type_node);

      type type_binding;
      TYPE_DEF_CELL_GET_TYPE(type_node->type, &type_node->ast_node, type_binding, err)
      if (err) return err;

      int alloc_size = typed_value_size(type_binding);
      alloc = (struct typed_value*)inter_eval_alloc_node(inter, alloc_node, alloc_size, &err);
      if (err) return err;
      alloc->type = (*type_node->type)->type_copy(type_node->type);
   }

   assert(name_node->symbol);
   hash_map_sym_push_value(name_node->symbol, (void*)alloc);

   return NULL;
}


error inter_func_undef_check(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   error err;
   int num_children = dlist_root_length(&root->ast_node.children);

   root->returns_lval = 0;
   root->eval_table.eval_rval = inter_func_undef_eval;
   root->eval_table.eval_lval = inter_eval_fallback_lval;

   if (num_children != 3) {
      return error_wrong_number_of_args(&root->ast_node, 3, num_children - 1);
   }

   struct ast_node_with_type* name_node = (struct ast_node_with_type*)dlist_root_nth(&root->ast_node.children, 1);
   struct ast_node_with_type* alloc_node = (struct ast_node_with_type*)dlist_root_nth(&root->ast_node.children, 2);

   if (name_node->ast_node.state != TOKEN_STATE_SYMBOL) {
      return error_syntax(&name_node->ast_node);
   }

   inter_cache_value_symbol(inter, name_node);

   err = inter_check_alloc_node(alloc_node);
   if (err) return err;

   return NULL;
}

error inter_func_undef_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   assert(dlist_root_length(&root->ast_node.children) == 3);

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* name_node = (struct ast_node_with_type*)first->ast_node.node.next;
   struct ast_node_with_type* alloc_node = (struct ast_node_with_type*)name_node->ast_node.node.next;

   assert(name_node->symbol);
   struct typed_value* binding = (struct typed_value*)hash_map_sym_pop_value(name_node->symbol);

   if (binding == NULL) {
      return error_symbol_undefined(&name_node->ast_node);
   }
   else {
      inter_do_free_alloc_node(inter, alloc_node, binding);
   }

   return NULL;
}


error inter_func_set_check(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   error err;
   int num_children = dlist_root_length(&root->ast_node.children);

   root->returns_lval = 0;
   root->eval_table.eval_rval = inter_func_set_eval;
   root->eval_table.eval_lval = inter_eval_fallback_lval;

   if (num_children < 3) {
      return error_syntax(&root->ast_node);
   }

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* value_node = (struct ast_node_with_type*)first->ast_node.node.prev;
   struct ast_node_with_type* lval_node = (struct ast_node_with_type*)value_node->ast_node.node.prev;


   err = inter_type_check_expr(inter, NULL, lval_node);
   if (err) return err;

   if (!lval_node->returns_lval) {
      return error_lval_expected(&lval_node->ast_node);
   }

   type expected_val_type = lval_node->type;

   err = inter_type_check_expr(inter, expected_val_type, value_node);
   if (err) return err;

   lval_node = (struct ast_node_with_type*)lval_node->ast_node.node.prev;
   while (lval_node != first) {
      err = inter_type_check_expr(inter, expected_val_type, lval_node);
      if (err) return err;

      if (!lval_node->returns_lval) {
         return error_lval_expected(&lval_node->ast_node);
      }

      lval_node = (struct ast_node_with_type*)lval_node->ast_node.node.prev;
   }

   return NULL;
}

error inter_func_set_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   error err;
   assert(dlist_root_length(&root->ast_node.children) > 2);
   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* value_node = (struct ast_node_with_type*)first->ast_node.node.prev;
   struct ast_node_with_type* lval_node = (struct ast_node_with_type*)value_node->ast_node.node.prev;

   int value_size = (*value_node->type)->type_size(value_node->type);

   void* lval_place;
   err = inter_eval_lval(inter, lval_node, &lval_place, value_size);
   if (err) return err;

   err = inter_eval_rval(inter, value_node, lval_place, value_size);
   if (err) return err;

   while(lval_node != first) {

      void* other_lval_place;
      err = inter_eval_lval(inter, lval_node, &other_lval_place, value_size);
      if (err) return err;

      if (other_lval_place != lval_place)
         (*lval_node->type)->type_copy_val(lval_node->type, lval_place, other_lval_place);

      lval_node = (struct ast_node_with_type*)lval_node->ast_node.node.prev;
   }

   return NULL;
}


error inter_func_sum_check(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   error err;
   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* curr = (struct ast_node_with_type*)first->ast_node.node.next;

   if (curr == first) {
      return error_syntax(&root->ast_node);
   }

   err = inter_type_check_expr(inter, expected_type, curr);
   if (err) return err;

   type rest_expected_type = NULL;

   int is_primitive = 1;

   root->eval_table.eval_lval = inter_eval_fallback_lval;
   if (*curr->type == &type_char_vtable) {
      root->eval_table.eval_rval = inter_func_sum_eval_char;
   } else if (*curr->type == &type_uchar_vtable) {
      root->eval_table.eval_rval = inter_func_sum_eval_uchar;
   } else if (*curr->type == &type_short_vtable) {
      root->eval_table.eval_rval = inter_func_sum_eval_short;
   } else if (*curr->type == &type_ushort_vtable) {
      root->eval_table.eval_rval = inter_func_sum_eval_ushort;
   } else if (*curr->type == &type_int_vtable) {
      root->eval_table.eval_rval = inter_func_sum_eval_int;
   } else if (*curr->type == &type_uint_vtable) {
      root->eval_table.eval_rval = inter_func_sum_eval_uint;
   } else if (*curr->type == &type_long_vtable) {
      root->eval_table.eval_rval = inter_func_sum_eval_long;
   } else if (*curr->type == &type_ulong_vtable) {
      root->eval_table.eval_rval = inter_func_sum_eval_ulong;
   } else if (*curr->type == &type_long_long_vtable) {
      root->eval_table.eval_rval = inter_func_sum_eval_long_long;
   } else if (*curr->type == &type_ulong_long_vtable) {
      root->eval_table.eval_rval = inter_func_sum_eval_ulong_long;
   } else if (*curr->type == &type_float_vtable) {
      root->eval_table.eval_rval = inter_func_sum_eval_float;
   } else if (*curr->type == &type_double_vtable) {
      root->eval_table.eval_rval = inter_func_sum_eval_double;
   } else if (*curr->type == &type_long_double_vtable) {
      root->eval_table.eval_rval = inter_func_sum_eval_long_double;
   } else {
      is_primitive = 0;
   }

   if (is_primitive) {
      root->type = (*curr->type)->type_copy(curr->type);
      rest_expected_type = curr->type;
      curr = (struct ast_node_with_type*)curr->ast_node.node.next;
   }
   else if (*curr->type == &type_string_vtable ||
            *curr->type == &type_pointer_to_vtable) {
      root->type = (*curr->type)->type_copy(curr->type);

      curr = (struct ast_node_with_type*)curr->ast_node.node.next;
      if (curr != first) {
         err = inter_type_check_expr(inter, NULL, curr);
         if (err) return err;

         int arg_valid = 1;

         if (*curr->type == &type_char_vtable) {
            root->eval_table.eval_rval = inter_func_sum_eval_p_char;
         } else if (*curr->type == &type_uchar_vtable) {
            root->eval_table.eval_rval = inter_func_sum_eval_p_uchar;
         } else if (*curr->type == &type_short_vtable) {
            root->eval_table.eval_rval = inter_func_sum_eval_p_short;
         } else if (*curr->type == &type_ushort_vtable) {
            root->eval_table.eval_rval = inter_func_sum_eval_p_ushort;
         } else if (*curr->type == &type_int_vtable) {
            root->eval_table.eval_rval = inter_func_sum_eval_p_int;
         } else if (*curr->type == &type_uint_vtable) {
            root->eval_table.eval_rval = inter_func_sum_eval_p_uint;
         } else if (*curr->type == &type_long_vtable) {
            root->eval_table.eval_rval = inter_func_sum_eval_p_long;
         } else if (*curr->type == &type_ulong_vtable) {
            root->eval_table.eval_rval = inter_func_sum_eval_p_ulong;
         } else if (*curr->type == &type_long_long_vtable) {
            root->eval_table.eval_rval = inter_func_sum_eval_p_long_long;
         } else if (*curr->type == &type_ulong_long_vtable) {
            root->eval_table.eval_rval = inter_func_sum_eval_p_ulong_long;
         } else {
            return error_type_mismatch(&curr->ast_node, NULL, curr->type);
         }

         rest_expected_type = curr->type;
         curr = (struct ast_node_with_type*)curr->ast_node.node.next;
      }
   }
   else {
      return error_type_mismatch(&curr->ast_node, NULL, curr->type);
   }

   while (curr != first) {
      err = inter_type_check_expr(inter, rest_expected_type, curr);
      if (err) return err;

      curr = (struct ast_node_with_type*)curr->ast_node.node.next;
   }

   return NULL;
}


error inter_func_sub_check(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   error err;
   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* curr = (struct ast_node_with_type*)first->ast_node.node.next;

   if (curr == first) {
      return error_syntax(&root->ast_node);
   }

   err = inter_type_check_expr(inter, expected_type, curr);
   if (err) return err;

   type rest_expected_type = NULL;

   int is_primitive = 1;

   root->eval_table.eval_lval = inter_eval_fallback_lval;
   if (*curr->type == &type_char_vtable) {
      root->eval_table.eval_rval = inter_func_sub_eval_char;
   } else if (*curr->type == &type_uchar_vtable) {
      root->eval_table.eval_rval = inter_func_sub_eval_uchar;
   } else if (*curr->type == &type_short_vtable) {
      root->eval_table.eval_rval = inter_func_sub_eval_short;
   } else if (*curr->type == &type_ushort_vtable) {
      root->eval_table.eval_rval = inter_func_sub_eval_ushort;
   } else if (*curr->type == &type_int_vtable) {
      root->eval_table.eval_rval = inter_func_sub_eval_int;
   } else if (*curr->type == &type_uint_vtable) {
      root->eval_table.eval_rval = inter_func_sub_eval_uint;
   } else if (*curr->type == &type_long_vtable) {
      root->eval_table.eval_rval = inter_func_sub_eval_long;
   } else if (*curr->type == &type_ulong_vtable) {
      root->eval_table.eval_rval = inter_func_sub_eval_ulong;
   } else if (*curr->type == &type_long_long_vtable) {
      root->eval_table.eval_rval = inter_func_sub_eval_long_long;
   } else if (*curr->type == &type_ulong_long_vtable) {
      root->eval_table.eval_rval = inter_func_sub_eval_ulong_long;
   } else if (*curr->type == &type_float_vtable) {
      root->eval_table.eval_rval = inter_func_sub_eval_float;
   } else if (*curr->type == &type_double_vtable) {
      root->eval_table.eval_rval = inter_func_sub_eval_double;
   } else if (*curr->type == &type_long_double_vtable) {
      root->eval_table.eval_rval = inter_func_sub_eval_long_double;
   } else {
      is_primitive = 0;
   }

   if (is_primitive) {
      root->type = (*curr->type)->type_copy(curr->type);
      rest_expected_type = curr->type;
      curr = (struct ast_node_with_type*)curr->ast_node.node.next;
   }
   else if (*curr->type == &type_string_vtable ||
            *curr->type == &type_pointer_to_vtable) {
      root->type = (*curr->type)->type_copy(curr->type);

      curr = (struct ast_node_with_type*)curr->ast_node.node.next;
      if (curr != first) {
         err = inter_type_check_expr(inter, NULL, curr);
         if (err) return err;

         int arg_valid = 1;

         if (*curr->type == &type_char_vtable) {
            root->eval_table.eval_rval = inter_func_sub_eval_p_char;
         } else if (*curr->type == &type_uchar_vtable) {
            root->eval_table.eval_rval = inter_func_sub_eval_p_uchar;
         } else if (*curr->type == &type_short_vtable) {
            root->eval_table.eval_rval = inter_func_sub_eval_p_short;
         } else if (*curr->type == &type_ushort_vtable) {
            root->eval_table.eval_rval = inter_func_sub_eval_p_ushort;
         } else if (*curr->type == &type_int_vtable) {
            root->eval_table.eval_rval = inter_func_sub_eval_p_int;
         } else if (*curr->type == &type_uint_vtable) {
            root->eval_table.eval_rval = inter_func_sub_eval_p_uint;
         } else if (*curr->type == &type_long_vtable) {
            root->eval_table.eval_rval = inter_func_sub_eval_p_long;
         } else if (*curr->type == &type_ulong_vtable) {
            root->eval_table.eval_rval = inter_func_sub_eval_p_ulong;
         } else if (*curr->type == &type_long_long_vtable) {
            root->eval_table.eval_rval = inter_func_sub_eval_p_long_long;
         } else if (*curr->type == &type_ulong_long_vtable) {
            root->eval_table.eval_rval = inter_func_sub_eval_p_ulong_long;
         }
         else {
            return error_type_mismatch(&curr->ast_node, NULL, curr->type);
         }

         rest_expected_type = curr->type;
         curr = (struct ast_node_with_type*)curr->ast_node.node.next;
      }
   }
   else {
      return error_type_mismatch(&curr->ast_node, NULL, curr->type);
   }

   while (curr != first) {
      err = inter_type_check_expr(inter, rest_expected_type, curr);
      if (err) return err;

      curr = (struct ast_node_with_type*)curr->ast_node.node.next;
   }

   return NULL;
}





error inter_func_mul_check(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   error err;
   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* curr = (struct ast_node_with_type*)first->ast_node.node.next;

   if (curr == first) {
      return error_syntax(&root->ast_node);
   }

   err = inter_type_check_expr(inter, expected_type, curr);
   if (err) return err;

   root->eval_table.eval_lval = inter_eval_fallback_lval;
   if (*curr->type == &type_char_vtable) {
      root->eval_table.eval_rval = inter_func_mul_eval_char;
   } else if (*curr->type == &type_uchar_vtable) {
      root->eval_table.eval_rval = inter_func_mul_eval_uchar;
   } else if (*curr->type == &type_short_vtable) {
      root->eval_table.eval_rval = inter_func_mul_eval_short;
   } else if (*curr->type == &type_ushort_vtable) {
      root->eval_table.eval_rval = inter_func_mul_eval_ushort;
   } else if (*curr->type == &type_int_vtable) {
      root->eval_table.eval_rval = inter_func_mul_eval_int;
   } else if (*curr->type == &type_uint_vtable) {
      root->eval_table.eval_rval = inter_func_mul_eval_uint;
   } else if (*curr->type == &type_long_vtable) {
      root->eval_table.eval_rval = inter_func_mul_eval_long;
   } else if (*curr->type == &type_ulong_vtable) {
      root->eval_table.eval_rval = inter_func_mul_eval_ulong;
   } else if (*curr->type == &type_long_long_vtable) {
      root->eval_table.eval_rval = inter_func_mul_eval_long_long;
   } else if (*curr->type == &type_ulong_long_vtable) {
      root->eval_table.eval_rval = inter_func_mul_eval_ulong_long;
   } else if (*curr->type == &type_float_vtable) {
      root->eval_table.eval_rval = inter_func_mul_eval_float;
   } else if (*curr->type == &type_double_vtable) {
      root->eval_table.eval_rval = inter_func_mul_eval_double;
   } else if (*curr->type == &type_long_double_vtable) {
      root->eval_table.eval_rval = inter_func_mul_eval_long_double;
   } else {
      return error_type_mismatch(&curr->ast_node, NULL, curr->type);
   }

   root->type = (*curr->type)->type_copy(curr->type);
 
   curr = (struct ast_node_with_type*)curr->ast_node.node.next;
   while (curr != first) {
      err = inter_type_check_expr(inter, root->type, curr);
      if (err) return err;

      curr = (struct ast_node_with_type*)curr->ast_node.node.next;
   }

   return NULL;
}


error inter_func_div_check(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   error err;
   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* curr = (struct ast_node_with_type*)first->ast_node.node.next;

   if (curr == first) {
      return error_syntax(&root->ast_node);
   }

   err = inter_type_check_expr(inter, expected_type, curr);
   if (err) return err;

   root->eval_table.eval_lval = inter_eval_fallback_lval;
   if (*curr->type == &type_char_vtable) {
      root->eval_table.eval_rval = inter_func_div_eval_char;
   } else if (*curr->type == &type_uchar_vtable) {
      root->eval_table.eval_rval = inter_func_div_eval_uchar;
   } else if (*curr->type == &type_short_vtable) {
      root->eval_table.eval_rval = inter_func_div_eval_short;
   } else if (*curr->type == &type_ushort_vtable) {
      root->eval_table.eval_rval = inter_func_div_eval_ushort;
   } else if (*curr->type == &type_int_vtable) {
      root->eval_table.eval_rval = inter_func_div_eval_int;
   } else if (*curr->type == &type_uint_vtable) {
      root->eval_table.eval_rval = inter_func_div_eval_uint;
   } else if (*curr->type == &type_long_vtable) {
      root->eval_table.eval_rval = inter_func_div_eval_long;
   } else if (*curr->type == &type_ulong_vtable) {
      root->eval_table.eval_rval = inter_func_div_eval_ulong;
   } else if (*curr->type == &type_long_long_vtable) {
      root->eval_table.eval_rval = inter_func_div_eval_long_long;
   } else if (*curr->type == &type_ulong_long_vtable) {
      root->eval_table.eval_rval = inter_func_div_eval_ulong_long;
   } else if (*curr->type == &type_float_vtable) {
      root->eval_table.eval_rval = inter_func_div_eval_float;
   } else if (*curr->type == &type_double_vtable) {
      root->eval_table.eval_rval = inter_func_div_eval_double;
   } else if (*curr->type == &type_long_double_vtable) {
      root->eval_table.eval_rval = inter_func_div_eval_long_double;
   } else {
      return error_type_mismatch(&curr->ast_node, NULL, curr->type);
   }

   root->type = (*curr->type)->type_copy(curr->type);
 
   curr = (struct ast_node_with_type*)curr->ast_node.node.next;
   while (curr != first) {
      err = inter_type_check_expr(inter, root->type, curr);
      if (err) return err;

      curr = (struct ast_node_with_type*)curr->ast_node.node.next;
   }

   return NULL;
}



error inter_func_mod_check(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   error err;
   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* curr = (struct ast_node_with_type*)first->ast_node.node.next;

   if (curr == first) {
      return error_syntax(&root->ast_node);
   }

   err = inter_type_check_expr(inter, expected_type, curr);
   if (err) return err;

   root->eval_table.eval_lval = inter_eval_fallback_lval;
   if (*curr->type == &type_char_vtable) {
      root->eval_table.eval_rval = inter_func_mod_eval_char;
   } else if (*curr->type == &type_uchar_vtable) {
      root->eval_table.eval_rval = inter_func_mod_eval_uchar;
   } else if (*curr->type == &type_short_vtable) {
      root->eval_table.eval_rval = inter_func_mod_eval_short;
   } else if (*curr->type == &type_ushort_vtable) {
      root->eval_table.eval_rval = inter_func_mod_eval_ushort;
   } else if (*curr->type == &type_int_vtable) {
      root->eval_table.eval_rval = inter_func_mod_eval_int;
   } else if (*curr->type == &type_uint_vtable) {
      root->eval_table.eval_rval = inter_func_mod_eval_uint;
   } else if (*curr->type == &type_long_vtable) {
      root->eval_table.eval_rval = inter_func_mod_eval_long;
   } else if (*curr->type == &type_ulong_vtable) {
      root->eval_table.eval_rval = inter_func_mod_eval_ulong;
   } else if (*curr->type == &type_long_long_vtable) {
      root->eval_table.eval_rval = inter_func_mod_eval_long_long;
   } else if (*curr->type == &type_ulong_long_vtable) {
      root->eval_table.eval_rval = inter_func_mod_eval_ulong_long;
   } else {
      return error_type_mismatch(&curr->ast_node, NULL, curr->type);
   }

   root->type = (*curr->type)->type_copy(curr->type);
 
   curr = (struct ast_node_with_type*)curr->ast_node.node.next;
   while (curr != first) {
      err = inter_type_check_expr(inter, root->type, curr);
      if (err) return err;

      curr = (struct ast_node_with_type*)curr->ast_node.node.next;
   }

   return NULL;
}


error inter_func_bit_and_check(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   error err;
   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* curr = (struct ast_node_with_type*)first->ast_node.node.next;

   if (curr == first) {
      return error_syntax(&root->ast_node);
   }

   err = inter_type_check_expr(inter, expected_type, curr);
   if (err) return err;

   root->eval_table.eval_lval = inter_eval_fallback_lval;
   if (*curr->type == &type_char_vtable) {
      root->eval_table.eval_rval = inter_func_bit_and_eval_char;
   } else if (*curr->type == &type_uchar_vtable) {
      root->eval_table.eval_rval = inter_func_bit_and_eval_uchar;
   } else if (*curr->type == &type_short_vtable) {
      root->eval_table.eval_rval = inter_func_bit_and_eval_short;
   } else if (*curr->type == &type_ushort_vtable) {
      root->eval_table.eval_rval = inter_func_bit_and_eval_ushort;
   } else if (*curr->type == &type_int_vtable) {
      root->eval_table.eval_rval = inter_func_bit_and_eval_int;
   } else if (*curr->type == &type_uint_vtable) {
      root->eval_table.eval_rval = inter_func_bit_and_eval_uint;
   } else if (*curr->type == &type_long_vtable) {
      root->eval_table.eval_rval = inter_func_bit_and_eval_long;
   } else if (*curr->type == &type_ulong_vtable) {
      root->eval_table.eval_rval = inter_func_bit_and_eval_ulong;
   } else if (*curr->type == &type_long_long_vtable) {
      root->eval_table.eval_rval = inter_func_bit_and_eval_long_long;
   } else if (*curr->type == &type_ulong_long_vtable) {
      root->eval_table.eval_rval = inter_func_bit_and_eval_ulong_long;
   } else {
      return error_type_mismatch(&curr->ast_node, NULL, curr->type);
   }

   root->type = (*curr->type)->type_copy(curr->type);
 
   curr = (struct ast_node_with_type*)curr->ast_node.node.next;
   while (curr != first) {
      err = inter_type_check_expr(inter, root->type, curr);
      if (err) return err;

      curr = (struct ast_node_with_type*)curr->ast_node.node.next;
   }

   return NULL;
}


error inter_func_bit_or_check(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   error err;
   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* curr = (struct ast_node_with_type*)first->ast_node.node.next;

   if (curr == first) {
      return error_syntax(&root->ast_node);
   }

   err = inter_type_check_expr(inter, expected_type, curr);
   if (err) return err;

   root->eval_table.eval_lval = inter_eval_fallback_lval;
   if (*curr->type == &type_char_vtable) {
      root->eval_table.eval_rval = inter_func_bit_or_eval_char;
   } else if (*curr->type == &type_uchar_vtable) {
      root->eval_table.eval_rval = inter_func_bit_or_eval_uchar;
   } else if (*curr->type == &type_short_vtable) {
      root->eval_table.eval_rval = inter_func_bit_or_eval_short;
   } else if (*curr->type == &type_ushort_vtable) {
      root->eval_table.eval_rval = inter_func_bit_or_eval_ushort;
   } else if (*curr->type == &type_int_vtable) {
      root->eval_table.eval_rval = inter_func_bit_or_eval_int;
   } else if (*curr->type == &type_uint_vtable) {
      root->eval_table.eval_rval = inter_func_bit_or_eval_uint;
   } else if (*curr->type == &type_long_vtable) {
      root->eval_table.eval_rval = inter_func_bit_or_eval_long;
   } else if (*curr->type == &type_ulong_vtable) {
      root->eval_table.eval_rval = inter_func_bit_or_eval_ulong;
   } else if (*curr->type == &type_long_long_vtable) {
      root->eval_table.eval_rval = inter_func_bit_or_eval_long_long;
   } else if (*curr->type == &type_ulong_long_vtable) {
      root->eval_table.eval_rval = inter_func_bit_or_eval_ulong_long;
   } else {
      return error_type_mismatch(&curr->ast_node, NULL, curr->type);
   }

   root->type = (*curr->type)->type_copy(curr->type);
 
   curr = (struct ast_node_with_type*)curr->ast_node.node.next;
   while (curr != first) {
      err = inter_type_check_expr(inter, root->type, curr);
      if (err) return err;

      curr = (struct ast_node_with_type*)curr->ast_node.node.next;
   }

   return NULL;
}


error inter_func_bit_not_check(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   error err;
   int num_children = dlist_root_length(&root->ast_node.children);

   if (num_children != 2) {
      return error_wrong_number_of_args(&root->ast_node, 1, num_children - 1);
   }

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* arg = (struct ast_node_with_type*)first->ast_node.node.next;

   err = inter_type_check_expr(inter, expected_type, arg);
   if (err) return err;

   root->eval_table.eval_lval = inter_eval_fallback_lval;
   if (*arg->type == &type_char_vtable) {
      root->eval_table.eval_rval = inter_func_bit_not_eval_char;
   } else if (*arg->type == &type_uchar_vtable) {
      root->eval_table.eval_rval = inter_func_bit_not_eval_uchar;
   } else if (*arg->type == &type_short_vtable) {
      root->eval_table.eval_rval = inter_func_bit_not_eval_short;
   } else if (*arg->type == &type_ushort_vtable) {
      root->eval_table.eval_rval = inter_func_bit_not_eval_ushort;
   } else if (*arg->type == &type_int_vtable) {
      root->eval_table.eval_rval = inter_func_bit_not_eval_int;
   } else if (*arg->type == &type_uint_vtable) {
      root->eval_table.eval_rval = inter_func_bit_not_eval_uint;
   } else if (*arg->type == &type_long_vtable) {
      root->eval_table.eval_rval = inter_func_bit_not_eval_long;
   } else if (*arg->type == &type_ulong_vtable) {
      root->eval_table.eval_rval = inter_func_bit_not_eval_ulong;
   } else if (*arg->type == &type_long_long_vtable) {
      root->eval_table.eval_rval = inter_func_bit_not_eval_long_long;
   } else if (*arg->type == &type_ulong_long_vtable) {
      root->eval_table.eval_rval = inter_func_bit_not_eval_ulong_long;
   } else {
      return error_type_mismatch(&arg->ast_node, NULL, arg->type);
   }

   root->type = (*arg->type)->type_copy(arg->type);
   return NULL;
}


error inter_func_shift_left_check(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   error err;
   int num_children = dlist_root_length(&root->ast_node.children);

   if (num_children != 3) {
      return error_wrong_number_of_args(&root->ast_node, 2, num_children - 1);
   }

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* arg = (struct ast_node_with_type*)first->ast_node.node.next;
   struct ast_node_with_type* index_node = (struct ast_node_with_type*)arg->ast_node.node.next;

   err = inter_type_check_expr(inter, expected_type, arg);
   if (err) return err;

   root->eval_table.eval_lval = inter_eval_fallback_lval;
   if (*arg->type == &type_char_vtable) {
      root->eval_table.eval_rval = inter_func_shift_left_eval_char;
   } else if (*arg->type == &type_uchar_vtable) {
      root->eval_table.eval_rval = inter_func_shift_left_eval_uchar;
   } else if (*arg->type == &type_short_vtable) {
      root->eval_table.eval_rval = inter_func_shift_left_eval_short;
   } else if (*arg->type == &type_ushort_vtable) {
      root->eval_table.eval_rval = inter_func_shift_left_eval_ushort;
   } else if (*arg->type == &type_int_vtable) {
      root->eval_table.eval_rval = inter_func_shift_left_eval_int;
   } else if (*arg->type == &type_uint_vtable) {
      root->eval_table.eval_rval = inter_func_shift_left_eval_uint;
   } else if (*arg->type == &type_long_vtable) {
      root->eval_table.eval_rval = inter_func_shift_left_eval_long;
   } else if (*arg->type == &type_ulong_vtable) {
      root->eval_table.eval_rval = inter_func_shift_left_eval_ulong;
   } else if (*arg->type == &type_long_long_vtable) {
      root->eval_table.eval_rval = inter_func_shift_left_eval_long_long;
   } else if (*arg->type == &type_ulong_long_vtable) {
      root->eval_table.eval_rval = inter_func_shift_left_eval_ulong_long;
   } else {
      return error_type_mismatch(&arg->ast_node, NULL, arg->type);
   }

   root->type = (*arg->type)->type_copy(arg->type);

   return inter_type_check_expr(inter, &type_int, index_node);
}

error inter_func_shift_right_check(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   error err;
   int num_children = dlist_root_length(&root->ast_node.children);

   if (num_children != 3) {
      return error_wrong_number_of_args(&root->ast_node, 2, num_children - 1);
   }

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* arg = (struct ast_node_with_type*)first->ast_node.node.next;
   struct ast_node_with_type* index_node = (struct ast_node_with_type*)arg->ast_node.node.next;

   err = inter_type_check_expr(inter, expected_type, arg);
   if (err) return err;

   root->eval_table.eval_lval = inter_eval_fallback_lval;
   if (*arg->type == &type_char_vtable) {
      root->eval_table.eval_rval = inter_func_shift_right_eval_char;
   } else if (*arg->type == &type_uchar_vtable) {
      root->eval_table.eval_rval = inter_func_shift_right_eval_uchar;
   } else if (*arg->type == &type_short_vtable) {
      root->eval_table.eval_rval = inter_func_shift_right_eval_short;
   } else if (*arg->type == &type_ushort_vtable) {
      root->eval_table.eval_rval = inter_func_shift_right_eval_ushort;
   } else if (*arg->type == &type_int_vtable) {
      root->eval_table.eval_rval = inter_func_shift_right_eval_int;
   } else if (*arg->type == &type_uint_vtable) {
      root->eval_table.eval_rval = inter_func_shift_right_eval_uint;
   } else if (*arg->type == &type_long_vtable) {
      root->eval_table.eval_rval = inter_func_shift_right_eval_long;
   } else if (*arg->type == &type_ulong_vtable) {
      root->eval_table.eval_rval = inter_func_shift_right_eval_ulong;
   } else if (*arg->type == &type_long_long_vtable) {
      root->eval_table.eval_rval = inter_func_shift_right_eval_long_long;
   } else if (*arg->type == &type_ulong_long_vtable) {
      root->eval_table.eval_rval = inter_func_shift_right_eval_ulong_long;
   } else {
      return error_type_mismatch(&arg->ast_node, NULL, arg->type);
   }

   root->type = (*arg->type)->type_copy(arg->type);

   return inter_type_check_expr(inter, &type_int, index_node);
}






error inter_func_gt_check(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   error err;

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   assert(first);
   struct ast_node_with_type* curr = (struct ast_node_with_type*)first->ast_node.node.next;

   if (first == curr) {
      return error_syntax(&root->ast_node);
   }

   err = inter_type_check_expr(inter, NULL, curr);
   if (err) return err;

   type arg_types = curr->type;

   root->eval_table.eval_lval = inter_eval_fallback_lval;
   if (curr->type == &type_char) {
      root->eval_table.eval_rval = inter_func_gt_eval_char;
   } else if (curr->type == &type_uchar) {
      root->eval_table.eval_rval = inter_func_gt_eval_uchar;
   } else if (curr->type == &type_short) {
      root->eval_table.eval_rval = inter_func_gt_eval_short;
   } else if (curr->type == &type_ushort) {
      root->eval_table.eval_rval = inter_func_gt_eval_ushort;
   } else if (curr->type == &type_int) {
      root->eval_table.eval_rval = inter_func_gt_eval_int;
   } else if (curr->type == &type_uint) {
      root->eval_table.eval_rval = inter_func_gt_eval_uint;
   } else if (curr->type == &type_long) {
      root->eval_table.eval_rval = inter_func_gt_eval_long;
   } else if (curr->type == &type_ulong) {
      root->eval_table.eval_rval = inter_func_gt_eval_ulong;
   } else if (curr->type == &type_long_long) {
      root->eval_table.eval_rval = inter_func_gt_eval_long_long;
   } else if (curr->type == &type_ulong_long) {
      root->eval_table.eval_rval = inter_func_gt_eval_ulong_long;
   } else if (curr->type == &type_float) {
      root->eval_table.eval_rval = inter_func_gt_eval_float;
   } else if (curr->type == &type_double) {
      root->eval_table.eval_rval = inter_func_gt_eval_double;
   } else if (curr->type == &type_long_double) {
      root->eval_table.eval_rval = inter_func_gt_eval_long_double;
   } else {
      return error_type_mismatch(&root->ast_node, NULL, curr->type);
   }

   curr = (struct ast_node_with_type*)curr->ast_node.node.next;

   while (curr != first) {
      err = inter_type_check_expr(inter, arg_types, curr);
      if (err) return err;

      curr = (struct ast_node_with_type*)curr->ast_node.node.next;
   }

   return NULL;
}

error inter_func_g_check(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   error err;

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   assert(first);
   struct ast_node_with_type* curr = (struct ast_node_with_type*)first->ast_node.node.next;

   if (first == curr) {
      return error_syntax(&root->ast_node);
   }

   err = inter_type_check_expr(inter, NULL, curr);
   if (err) return err;

   type arg_types = curr->type;

   root->eval_table.eval_lval = inter_eval_fallback_lval;
   if (curr->type == &type_char) {
      root->eval_table.eval_rval = inter_func_g_eval_char;
   } else if (curr->type == &type_uchar) {
      root->eval_table.eval_rval = inter_func_g_eval_uchar;
   } else if (curr->type == &type_short) {
      root->eval_table.eval_rval = inter_func_g_eval_short;
   } else if (curr->type == &type_ushort) {
      root->eval_table.eval_rval = inter_func_g_eval_ushort;
   } else if (curr->type == &type_int) {
      root->eval_table.eval_rval = inter_func_g_eval_int;
   } else if (curr->type == &type_uint) {
      root->eval_table.eval_rval = inter_func_g_eval_uint;
   } else if (curr->type == &type_long) {
      root->eval_table.eval_rval = inter_func_g_eval_long;
   } else if (curr->type == &type_ulong) {
      root->eval_table.eval_rval = inter_func_g_eval_ulong;
   } else if (curr->type == &type_long_long) {
      root->eval_table.eval_rval = inter_func_g_eval_long_long;
   } else if (curr->type == &type_ulong_long) {
      root->eval_table.eval_rval = inter_func_g_eval_ulong_long;
   } else if (curr->type == &type_float) {
      root->eval_table.eval_rval = inter_func_g_eval_float;
   } else if (curr->type == &type_double) {
      root->eval_table.eval_rval = inter_func_g_eval_double;
   } else if (curr->type == &type_long_double) {
      root->eval_table.eval_rval = inter_func_g_eval_long_double;
   } else {
      return error_type_mismatch(&root->ast_node, NULL, curr->type);
   }

   curr = (struct ast_node_with_type*)curr->ast_node.node.next;

   while (curr != first) {
      err = inter_type_check_expr(inter, arg_types, curr);
      if (err) return err;

      curr = (struct ast_node_with_type*)curr->ast_node.node.next;
   }

   return NULL;
}

error inter_func_lt_check(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   error err;

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   assert(first);
   struct ast_node_with_type* curr = (struct ast_node_with_type*)first->ast_node.node.next;

   if (first == curr) {
      return error_syntax(&root->ast_node);
   }

   err = inter_type_check_expr(inter, NULL, curr);
   if (err) return err;

   type arg_types = curr->type;

   root->eval_table.eval_lval = inter_eval_fallback_lval;
   if (curr->type == &type_char) {
      root->eval_table.eval_rval = inter_func_lt_eval_char;
   } else if (curr->type == &type_uchar) {
      root->eval_table.eval_rval = inter_func_lt_eval_uchar;
   } else if (curr->type == &type_short) {
      root->eval_table.eval_rval = inter_func_lt_eval_short;
   } else if (curr->type == &type_ushort) {
      root->eval_table.eval_rval = inter_func_lt_eval_ushort;
   } else if (curr->type == &type_int) {
      root->eval_table.eval_rval = inter_func_lt_eval_int;
   } else if (curr->type == &type_uint) {
      root->eval_table.eval_rval = inter_func_lt_eval_uint;
   } else if (curr->type == &type_long) {
      root->eval_table.eval_rval = inter_func_lt_eval_long;
   } else if (curr->type == &type_ulong) {
      root->eval_table.eval_rval = inter_func_lt_eval_ulong;
   } else if (curr->type == &type_long_long) {
      root->eval_table.eval_rval = inter_func_lt_eval_long_long;
   } else if (curr->type == &type_ulong_long) {
      root->eval_table.eval_rval = inter_func_lt_eval_ulong_long;
   } else if (curr->type == &type_float) {
      root->eval_table.eval_rval = inter_func_lt_eval_float;
   } else if (curr->type == &type_double) {
      root->eval_table.eval_rval = inter_func_lt_eval_double;
   } else if (curr->type == &type_long_double) {
      root->eval_table.eval_rval = inter_func_lt_eval_long_double;
   } else {
      return error_type_mismatch(&root->ast_node, NULL, curr->type);
   }

   curr = (struct ast_node_with_type*)curr->ast_node.node.next;

   while (curr != first) {
      err = inter_type_check_expr(inter, arg_types, curr);
      if (err) return err;

      curr = (struct ast_node_with_type*)curr->ast_node.node.next;
   }

   return NULL;
}

error inter_func_l_check(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   error err;

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   assert(first);
   struct ast_node_with_type* curr = (struct ast_node_with_type*)first->ast_node.node.next;

   if (first == curr) {
      return error_syntax(&root->ast_node);
   }

   err = inter_type_check_expr(inter, NULL, curr);
   if (err) return err;

   type arg_types = curr->type;

   root->eval_table.eval_lval = inter_eval_fallback_lval;
   if (curr->type == &type_char) {
      root->eval_table.eval_rval = inter_func_l_eval_char;
   } else if (curr->type == &type_uchar) {
      root->eval_table.eval_rval = inter_func_l_eval_uchar;
   } else if (curr->type == &type_short) {
      root->eval_table.eval_rval = inter_func_l_eval_short;
   } else if (curr->type == &type_ushort) {
      root->eval_table.eval_rval = inter_func_l_eval_ushort;
   } else if (curr->type == &type_int) {
      root->eval_table.eval_rval = inter_func_l_eval_int;
   } else if (curr->type == &type_uint) {
      root->eval_table.eval_rval = inter_func_l_eval_uint;
   } else if (curr->type == &type_long) {
      root->eval_table.eval_rval = inter_func_l_eval_long;
   } else if (curr->type == &type_ulong) {
      root->eval_table.eval_rval = inter_func_l_eval_ulong;
   } else if (curr->type == &type_long_long) {
      root->eval_table.eval_rval = inter_func_l_eval_long_long;
   } else if (curr->type == &type_ulong_long) {
      root->eval_table.eval_rval = inter_func_l_eval_ulong_long;
   } else if (curr->type == &type_float) {
      root->eval_table.eval_rval = inter_func_l_eval_float;
   } else if (curr->type == &type_double) {
      root->eval_table.eval_rval = inter_func_l_eval_double;
   } else if (curr->type == &type_long_double) {
      root->eval_table.eval_rval = inter_func_l_eval_long_double;
   } else {
      return error_type_mismatch(&root->ast_node, NULL, curr->type);
   }

   curr = (struct ast_node_with_type*)curr->ast_node.node.next;

   while (curr != first) {
      err = inter_type_check_expr(inter, arg_types, curr);
      if (err) return err;

      curr = (struct ast_node_with_type*)curr->ast_node.node.next;
   }

   return NULL;
}


#define FOLD_BODY(TYPE, OP) \
   err = inter_eval_rval(inter, curr, value, sizeof(TYPE)); \
   if (err) return err; \
   curr = (struct ast_node_with_type*)curr->ast_node.node.next; \
   while (curr != first) { \
      TYPE curr_val; \
      err = inter_eval_rval(inter, curr, &curr_val, sizeof(TYPE)); \
      if (err) return err; \
      *((TYPE*)value) OP curr_val; \
      curr = (struct ast_node_with_type*)curr->ast_node.node.next; \
   }



#define FOLD_P_BODY(CURR_TYPE, OP) \
   while (curr != first) { \
      CURR_TYPE curr_val; \
      err = inter_eval_rval(inter, curr, &curr_val, sizeof(CURR_TYPE)); \
      if (err) return err; \
      *((char**)value) OP (curr_val*elt_size); \
      curr = (struct ast_node_with_type*)curr->ast_node.node.next; \
   }

#define FOLD_P(OP) \
   int elt_size;  \
   if (*root->type == &type_pointer_to_vtable) { \
      struct type_pointer_to* root_type = (struct type_pointer_to*)root->type; \
      elt_size = (*root_type->content_type)->type_size(root_type->content_type); \
   } else if (*root->type == &type_string_vtable) { \
      elt_size = 1; \
   } else { \
      assert(0); \
   } \
   err = inter_eval_rval(inter, curr, value, sizeof(char*)); \
   if (err) return err; \
   curr = (struct ast_node_with_type*)curr->ast_node.node.next; \
   if (curr == first) { \
   } else if (*curr->type == &type_char_vtable) { \
      FOLD_P_BODY(char, OP) \
   } else if (*curr->type == &type_uchar_vtable) { \
      FOLD_P_BODY(unsigned char, OP) \
   } else if (*curr->type == &type_short_vtable) { \
      FOLD_P_BODY(short, OP) \
   } else if (*curr->type == &type_ushort_vtable) { \
      FOLD_P_BODY(unsigned short, OP) \
   } else if (*curr->type == &type_int_vtable) { \
      FOLD_P_BODY(int, OP) \
   } else if (*curr->type == &type_uint_vtable) { \
      FOLD_P_BODY(unsigned int, OP) \
   } else if (*curr->type == &type_long_vtable) { \
      FOLD_P_BODY(long, OP) \
   } else if (*curr->type == &type_ulong_vtable) { \
      FOLD_P_BODY(unsigned long, OP) \
   } else if (*curr->type == &type_long_long_vtable) { \
      FOLD_P_BODY(long long, OP) \
   } else if (*curr->type == &type_ulong_long_vtable) { \
      FOLD_P_BODY(unsigned long long, OP) \
   } else { \
      assert(0); \
   } \


#define DEF_FOLD(OPNAME,OP,TYPENAME,TYPE) \
error inter_func_##OPNAME##_eval_##TYPENAME(struct inter* inter, struct ast_node_with_type* root, void* value, int size) { \
   error err; \
   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node; \
   assert(first); \
   struct ast_node_with_type* curr = (struct ast_node_with_type*)first->ast_node.node.next; \
   FOLD_BODY(TYPE,OP) \
   return NULL; \
}

#define DEF_UNARY_FOLD(OPNAME,OP,UNARY_OP,TYPENAME,TYPE) \
error inter_func_##OPNAME##_eval_##TYPENAME(struct inter* inter, struct ast_node_with_type* root, void* value, int size) { \
   error err; \
   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node; \
   assert(first); \
   struct ast_node_with_type* curr = (struct ast_node_with_type*)first->ast_node.node.next; \
   if (first == (struct ast_node_with_type*)curr->ast_node.node.next) { \
      err = inter_eval_rval(inter, curr, value, size); \
      if (err) return err; \
      *((TYPE*)value) = UNARY_OP *((TYPE*)value); \
   } \
   else { \
      FOLD_BODY(TYPE,OP) \
   } \
   return NULL; \
}


#define UNARY_ASG(TYPE, UNARY_OP) \
   *((TYPE*)value) = UNARY_OP *((TYPE*)value);


#define DEF_UNARY(OPNAME,UNARY_OP,TYPENAME,TYPE) \
error inter_func_##OPNAME##_eval_##TYPENAME(struct inter* inter, struct ast_node_with_type* root, void* value, int size) { \
   error err; \
   assert(dlist_root_length(&root->ast_node.children) == 2); \
   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node; \
   struct ast_node_with_type* arg = (struct ast_node_with_type*)first->ast_node.node.next; \
   err = inter_eval_rval(inter, arg, value, size); \
   if (err) return err; \
   UNARY_ASG(TYPE, UNARY_OP) \
   return NULL; \
}

#define SHIFT_ASG(TYPE, SHIFT_OP) \
   *((TYPE*)value) = *((TYPE*)value) SHIFT_OP index;

#define DEF_SHIFT(OPNAME,SHIFT_OP,TYPENAME,TYPE) \
error inter_func_##OPNAME##_eval_##TYPENAME(struct inter* inter, struct ast_node_with_type* root, void* value, int size) { \
   error err; \
   int index; \
   assert(dlist_root_length(&root->ast_node.children) == 3); \
   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node; \
   struct ast_node_with_type* arg = (struct ast_node_with_type*)first->ast_node.node.next; \
   struct ast_node_with_type* index_node = (struct ast_node_with_type*)arg->ast_node.node.next; \
   err = inter_eval_rval(inter, index_node, &index, sizeof(int)); \
   if (err) return err; \
   err = inter_eval_rval(inter, arg, value, size); \
   if (err) return err; \
   SHIFT_ASG(TYPE, SHIFT_OP) \
   return NULL; \
}





#define DEF_P_FOLD(OPNAME,OP,TYPENAME,TYPE) \
error inter_func_##OPNAME##_eval_p_##TYPENAME(struct inter* inter, struct ast_node_with_type* root, void* value, int size) { \
   error err; \
   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node; \
   assert(first); \
   struct ast_node_with_type* curr = (struct ast_node_with_type*)first->ast_node.node.next; \
   int elt_size; \
   if (*root->type == &type_pointer_to_vtable) { \
      struct type_pointer_to* root_type = (struct type_pointer_to*)root->type; \
      elt_size = (*root_type->content_type)->type_size(root_type->content_type); \
   } else if (*root->type == &type_string_vtable) { \
      elt_size = 1; \
   } else { \
      assert(0); \
   } \
   err = inter_eval_rval(inter, curr, value, sizeof(char*)); \
   if (err) return err; \
   curr = (struct ast_node_with_type*)curr->ast_node.node.next; \
   FOLD_P_BODY(TYPE,OP) \
   return NULL; \
}


// inter_func_sum_eval
DEF_FOLD(sum,+=,char,char)
DEF_FOLD(sum,+=,uchar,unsigned char)
DEF_FOLD(sum,+=,short,short)
DEF_FOLD(sum,+=,ushort,unsigned short)
DEF_FOLD(sum,+=,int,int)
DEF_FOLD(sum,+=,uint,unsigned int)
DEF_FOLD(sum,+=,long,long)
DEF_FOLD(sum,+=,ulong,unsigned long)
DEF_FOLD(sum,+=,long_long,long long)
DEF_FOLD(sum,+=,ulong_long,unsigned long long)
DEF_FOLD(sum,+=,float,float)
DEF_FOLD(sum,+=,double,double)
DEF_FOLD(sum,+=,long_double,long double)
DEF_P_FOLD(sum,+=,char,char)
DEF_P_FOLD(sum,+=,uchar,unsigned char)
DEF_P_FOLD(sum,+=,short,short)
DEF_P_FOLD(sum,+=,ushort,unsigned short)
DEF_P_FOLD(sum,+=,int,int)
DEF_P_FOLD(sum,+=,uint,unsigned int)
DEF_P_FOLD(sum,+=,long,long)
DEF_P_FOLD(sum,+=,ulong,unsigned long)
DEF_P_FOLD(sum,+=,long_long,long long)
DEF_P_FOLD(sum,+=,ulong_long,unsigned long long)



error inter_func_sum_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   error err;
   assert(dlist_root_length(&root->ast_node.children) > 1);

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* curr = (struct ast_node_with_type*)first->ast_node.node.next;

   if (*root->type == &type_char_vtable) {
      FOLD_BODY(char, +=)
   } else if (*root->type == &type_uchar_vtable) {
      FOLD_BODY(unsigned char, +=)
   } else if (*root->type == &type_short_vtable) {
      FOLD_BODY(short, +=)
   } else if (*root->type == &type_ushort_vtable) {
      FOLD_BODY(unsigned short, +=)
   } else if (*root->type == &type_int_vtable) {
      FOLD_BODY(int, +=)
   } else if (*root->type == &type_uint_vtable) {
      FOLD_BODY(unsigned int, +=)
   } else if (*root->type == &type_long_vtable) {
      FOLD_BODY(long, +=)
   } else if (*root->type == &type_ulong_vtable) {
      FOLD_BODY(unsigned long, +=)
   } else if (*root->type == &type_long_long_vtable) {
      FOLD_BODY(long long, +=)
   } else if (*root->type == &type_ulong_long_vtable) {
      FOLD_BODY(unsigned long long, +=)
   } else if (*root->type == &type_float_vtable) {
      FOLD_BODY(float, +=)
   } else if (*root->type == &type_double_vtable) {
      FOLD_BODY(double, +=)
   } else if (*root->type == &type_long_double_vtable) {
      FOLD_BODY(long double, +=)
   } else if (*root->type == &type_pointer_to_vtable ||
              *root->type == &type_string_vtable) {
      FOLD_P(+=)
   }
   else {
      assert(0);
   }

   return NULL;
}

#define FOLD_SUB_BODY(TYPE) \
   err = inter_eval_rval(inter, curr, value, sizeof(TYPE)); \
   if (err) return err; \
   curr = (struct ast_node_with_type*)curr->ast_node.node.next; \
   if (curr == first) { \
      *((TYPE*)value) = -*((TYPE*)value); \
   } \
   else { \
      while (curr != first) { \
         TYPE curr_val; \
         err = inter_eval_rval(inter, curr, &curr_val, sizeof(TYPE)); \
         if (err) return err; \
         *((TYPE*)value) -= curr_val; \
         curr = (struct ast_node_with_type*)curr->ast_node.node.next; \
      } \
   }



// inter_func_sub_eval
DEF_UNARY_FOLD(sub,-=,-,char,char)
DEF_UNARY_FOLD(sub,-=,-,uchar,unsigned char)
DEF_UNARY_FOLD(sub,-=,-,short,short)
DEF_UNARY_FOLD(sub,-=,-,ushort,unsigned short)
DEF_UNARY_FOLD(sub,-=,-,int,int)
DEF_UNARY_FOLD(sub,-=,-,uint,unsigned int)
DEF_UNARY_FOLD(sub,-=,-,long,long)
DEF_UNARY_FOLD(sub,-=,-,ulong,unsigned long)
DEF_UNARY_FOLD(sub,-=,-,long_long,long long)
DEF_UNARY_FOLD(sub,-=,-,ulong_long,unsigned long long)
DEF_UNARY_FOLD(sub,-=,-,float,float)
DEF_UNARY_FOLD(sub,-=,-,double,double)
DEF_UNARY_FOLD(sub,-=,-,long_double,long double)
DEF_P_FOLD(sub,-=,char,char)
DEF_P_FOLD(sub,-=,uchar,unsigned char)
DEF_P_FOLD(sub,-=,short,short)
DEF_P_FOLD(sub,-=,ushort,unsigned short)
DEF_P_FOLD(sub,-=,int,int)
DEF_P_FOLD(sub,-=,uint,unsigned int)
DEF_P_FOLD(sub,-=,long,long)
DEF_P_FOLD(sub,-=,ulong,unsigned long)
DEF_P_FOLD(sub,-=,long_long,long long)
DEF_P_FOLD(sub,-=,ulong_long,unsigned long long)



error inter_func_sub_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   error err;
   assert(dlist_root_length(&root->ast_node.children) > 1);

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* curr = (struct ast_node_with_type*)first->ast_node.node.next;

   if (*root->type == &type_char_vtable) {
      FOLD_SUB_BODY(char)
   } else if (*root->type == &type_uchar_vtable) {
      FOLD_SUB_BODY(unsigned char)
   } else if (*root->type == &type_short_vtable) {
      FOLD_SUB_BODY(short)
   } else if (*root->type == &type_ushort_vtable) {
      FOLD_SUB_BODY(unsigned short)
   } else if (*root->type == &type_int_vtable) {
      FOLD_SUB_BODY(int)
   } else if (*root->type == &type_uint_vtable) {
      FOLD_SUB_BODY(unsigned int)
   } else if (*root->type == &type_long_vtable) {
      FOLD_SUB_BODY(long)
   } else if (*root->type == &type_ulong_vtable) {
      FOLD_SUB_BODY(unsigned long)
   } else if (*root->type == &type_long_long_vtable) {
      FOLD_SUB_BODY(long long)
   } else if (*root->type == &type_ulong_long_vtable) {
      FOLD_SUB_BODY(unsigned long long)
   } else if (*root->type == &type_float_vtable) {
      FOLD_SUB_BODY(float)
   } else if (*root->type == &type_double_vtable) {
      FOLD_SUB_BODY(double)
   } else if (*root->type == &type_long_double_vtable) {
      FOLD_SUB_BODY(long double)
   } else if (*root->type == &type_pointer_to_vtable ||
              *root->type == &type_string_vtable) {
      FOLD_P(-=)
   }
   else {
      assert(0);
   }

   return NULL;
}

// inter_func_mul_eval
DEF_FOLD(mul,*=,char,char)
DEF_FOLD(mul,*=,uchar,unsigned char)
DEF_FOLD(mul,*=,short,short)
DEF_FOLD(mul,*=,ushort,unsigned short)
DEF_FOLD(mul,*=,int,int)
DEF_FOLD(mul,*=,uint,unsigned int)
DEF_FOLD(mul,*=,long,long)
DEF_FOLD(mul,*=,ulong,unsigned long)
DEF_FOLD(mul,*=,long_long,long long)
DEF_FOLD(mul,*=,ulong_long,unsigned long long)
DEF_FOLD(mul,*=,float,float)
DEF_FOLD(mul,*=,double,double)
DEF_FOLD(mul,*=,long_double,long double)

error inter_func_mul_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   error err;
   assert(dlist_root_length(&root->ast_node.children) > 1);

   union number_t curr_num;
   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* curr = (struct ast_node_with_type*)first->ast_node.node.next;

   if (*root->type == &type_char_vtable) {
      FOLD_BODY(char, *=)
   } else if (*root->type == &type_uchar_vtable) {
      FOLD_BODY(unsigned char, *=)
   } else if (*root->type == &type_short_vtable) {
      FOLD_BODY(short, *=)
   } else if (*root->type == &type_ushort_vtable) {
      FOLD_BODY(unsigned short, *=)
   } else if (*root->type == &type_int_vtable) {
      FOLD_BODY(int, *=)
   } else if (*root->type == &type_uint_vtable) {
      FOLD_BODY(unsigned int, *=)
   } else if (*root->type == &type_long_vtable) {
      FOLD_BODY(long, *=)
   } else if (*root->type == &type_ulong_vtable) {
      FOLD_BODY(unsigned long, *=)
   } else if (*root->type == &type_long_long_vtable) {
      FOLD_BODY(long long, *=)
   } else if (*root->type == &type_ulong_long_vtable) {
      FOLD_BODY(unsigned long long, *=)
   } else if (*root->type == &type_float_vtable) {
      FOLD_BODY(float, *=)
   } else if (*root->type == &type_double_vtable) {
      FOLD_BODY(double, *=)
   } else if (*root->type == &type_long_double_vtable) {
      FOLD_BODY(long double, *=)
   } else {
      assert(0);
   }

   return NULL;
}


// inter_func_div_eval
DEF_FOLD(div,/=,char,char)
DEF_FOLD(div,/=,uchar,unsigned char)
DEF_FOLD(div,/=,short,short)
DEF_FOLD(div,/=,ushort,unsigned short)
DEF_FOLD(div,/=,int,int)
DEF_FOLD(div,/=,uint,unsigned int)
DEF_FOLD(div,/=,long,long)
DEF_FOLD(div,/=,ulong,unsigned long)
DEF_FOLD(div,/=,long_long,long long)
DEF_FOLD(div,/=,ulong_long,unsigned long long)
DEF_FOLD(div,/=,float,float)
DEF_FOLD(div,/=,double,double)
DEF_FOLD(div,/=,long_double,long double)

error inter_func_div_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   error err;
   assert(dlist_root_length(&root->ast_node.children) > 1);

   union number_t curr_num;
   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* curr = (struct ast_node_with_type*)first->ast_node.node.next;

   if (*root->type == &type_char_vtable) {
      FOLD_BODY(char, /=)
   } else if (*root->type == &type_uchar_vtable) {
      FOLD_BODY(unsigned char, /=)
   } else if (*root->type == &type_short_vtable) {
      FOLD_BODY(short, /=)
   } else if (*root->type == &type_ushort_vtable) {
      FOLD_BODY(unsigned short, /=)
   } else if (*root->type == &type_int_vtable) {
      FOLD_BODY(int, /=)
   } else if (*root->type == &type_uint_vtable) {
      FOLD_BODY(unsigned int, /=)
   } else if (*root->type == &type_long_vtable) {
      FOLD_BODY(long, /=)
   } else if (*root->type == &type_ulong_vtable) {
      FOLD_BODY(unsigned long, /=)
   } else if (*root->type == &type_long_long_vtable) {
      FOLD_BODY(long long, /=)
   } else if (*root->type == &type_ulong_long_vtable) {
      FOLD_BODY(unsigned long long, /=)
   } else if (*root->type == &type_float_vtable) {
      FOLD_BODY(float, /=)
   } else if (*root->type == &type_double_vtable) {
      FOLD_BODY(double, /=)
   } else if (*root->type == &type_long_double_vtable) {
      FOLD_BODY(long double, /=)
   } else {
      assert(0);
   }

   return NULL;
}


// inter_func_mod_eval
DEF_FOLD(mod,%=,char,char)
DEF_FOLD(mod,%=,uchar,unsigned char)
DEF_FOLD(mod,%=,short,short)
DEF_FOLD(mod,%=,ushort,unsigned short)
DEF_FOLD(mod,%=,int,int)
DEF_FOLD(mod,%=,uint,unsigned int)
DEF_FOLD(mod,%=,long,long)
DEF_FOLD(mod,%=,ulong,unsigned long)
DEF_FOLD(mod,%=,long_long,long long)
DEF_FOLD(mod,%=,ulong_long,unsigned long long)

error inter_func_mod_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   error err;
   assert(dlist_root_length(&root->ast_node.children) > 1);

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* curr = (struct ast_node_with_type*)first->ast_node.node.next;

   if (*root->type == &type_char_vtable) {
      FOLD_BODY(char, %=)
   } else if (*root->type == &type_uchar_vtable) {
      FOLD_BODY(unsigned char, %=)
   } else if (*root->type == &type_short_vtable) {
      FOLD_BODY(short, %=)
   } else if (*root->type == &type_ushort_vtable) {
      FOLD_BODY(unsigned short, %=)
   } else if (*root->type == &type_int_vtable) {
      FOLD_BODY(int, %=)
   } else if (*root->type == &type_uint_vtable) {
      FOLD_BODY(unsigned int, %=)
   } else if (*root->type == &type_long_vtable) {
      FOLD_BODY(long, %=)
   } else if (*root->type == &type_ulong_vtable) {
      FOLD_BODY(unsigned long, %=)
   } else if (*root->type == &type_long_long_vtable) {
      FOLD_BODY(long long, %=)
   } else if (*root->type == &type_ulong_long_vtable) {
      FOLD_BODY(unsigned long long, %=)
   } else {
      assert(0);
   }

   return NULL;
}


// inter_func_bit_and_eval
DEF_FOLD(bit_and,&=,char,char)
DEF_FOLD(bit_and,&=,uchar,unsigned char)
DEF_FOLD(bit_and,&=,short,short)
DEF_FOLD(bit_and,&=,ushort,unsigned short)
DEF_FOLD(bit_and,&=,int,int)
DEF_FOLD(bit_and,&=,uint,unsigned int)
DEF_FOLD(bit_and,&=,long,long)
DEF_FOLD(bit_and,&=,ulong,unsigned long)
DEF_FOLD(bit_and,&=,long_long,long long)
DEF_FOLD(bit_and,&=,ulong_long,unsigned long long)

error inter_func_bit_and_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   error err;
   assert(dlist_root_length(&root->ast_node.children) > 1);

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* curr = (struct ast_node_with_type*)first->ast_node.node.next;

   if (*root->type == &type_char_vtable) {
      FOLD_BODY(char, &=)
   } else if (*root->type == &type_uchar_vtable) {
      FOLD_BODY(unsigned char, &=)
   } else if (*root->type == &type_short_vtable) {
      FOLD_BODY(short, &=)
   } else if (*root->type == &type_ushort_vtable) {
      FOLD_BODY(unsigned short, &=)
   } else if (*root->type == &type_int_vtable) {
      FOLD_BODY(int, &=)
   } else if (*root->type == &type_uint_vtable) {
      FOLD_BODY(unsigned int, &=)
   } else if (*root->type == &type_long_vtable) {
      FOLD_BODY(long, &=)
   } else if (*root->type == &type_ulong_vtable) {
      FOLD_BODY(unsigned long, &=)
   } else if (*root->type == &type_long_long_vtable) {
      FOLD_BODY(long long, &=)
   } else if (*root->type == &type_ulong_long_vtable) {
      FOLD_BODY(unsigned long long, &=)
   } else {
      assert(0);
   }

   return NULL;
}


// inter_func_bit_or_eval
DEF_FOLD(bit_or,|=,char,char)
DEF_FOLD(bit_or,|=,uchar,unsigned char)
DEF_FOLD(bit_or,|=,short,short)
DEF_FOLD(bit_or,|=,ushort,unsigned short)
DEF_FOLD(bit_or,|=,int,int)
DEF_FOLD(bit_or,|=,uint,unsigned int)
DEF_FOLD(bit_or,|=,long,long)
DEF_FOLD(bit_or,|=,ulong,unsigned long)
DEF_FOLD(bit_or,|=,long_long,long long)
DEF_FOLD(bit_or,|=,ulong_long,unsigned long long)

error inter_func_bit_or_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   error err;
   assert(dlist_root_length(&root->ast_node.children) > 1);

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* curr = (struct ast_node_with_type*)first->ast_node.node.next;

   if (*root->type == &type_char_vtable) {
      FOLD_BODY(char, |=)
   } else if (*root->type == &type_uchar_vtable) {
      FOLD_BODY(unsigned char, |=)
   } else if (*root->type == &type_short_vtable) {
      FOLD_BODY(short, |=)
   } else if (*root->type == &type_ushort_vtable) {
      FOLD_BODY(unsigned short, |=)
   } else if (*root->type == &type_int_vtable) {
      FOLD_BODY(int, |=)
   } else if (*root->type == &type_uint_vtable) {
      FOLD_BODY(unsigned int, |=)
   } else if (*root->type == &type_long_vtable) {
      FOLD_BODY(long, |=)
   } else if (*root->type == &type_ulong_vtable) {
      FOLD_BODY(unsigned long, |=)
   } else if (*root->type == &type_long_long_vtable) {
      FOLD_BODY(long long, |=)
   } else if (*root->type == &type_ulong_long_vtable) {
      FOLD_BODY(unsigned long long, |=)
   } else {
      assert(0);
   }

   return NULL;
}


// inter_func_bit_not_eval
DEF_UNARY(bit_not,~,char,char)
DEF_UNARY(bit_not,~,uchar,unsigned char)
DEF_UNARY(bit_not,~,short,short)
DEF_UNARY(bit_not,~,ushort,unsigned short)
DEF_UNARY(bit_not,~,int,int)
DEF_UNARY(bit_not,~,uint,unsigned int)
DEF_UNARY(bit_not,~,long,long)
DEF_UNARY(bit_not,~,ulong,unsigned long)
DEF_UNARY(bit_not,~,long_long,long long)
DEF_UNARY(bit_not,~,ulong_long,unsigned long long)

error inter_func_bit_not_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   error err;
   assert(dlist_root_length(&root->ast_node.children) == 2);

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* arg = (struct ast_node_with_type*)first->ast_node.node.next;

   err = inter_eval_rval(inter, arg, value, size);
   if (err) return err;

   if (*root->type == &type_char_vtable) {
      UNARY_ASG(char, ~)
   } else if (*root->type == &type_uchar_vtable) {
      UNARY_ASG(unsigned char, ~)
   } else if (*root->type == &type_short_vtable) {
      UNARY_ASG(short, ~)
   } else if (*root->type == &type_ushort_vtable) {
      UNARY_ASG(unsigned short, ~)
   } else if (*root->type == &type_int_vtable) {
      UNARY_ASG(int, ~)
   } else if (*root->type == &type_uint_vtable) {
      UNARY_ASG(unsigned int, ~)
   } else if (*root->type == &type_long_vtable) {
      UNARY_ASG(long, ~)
   } else if (*root->type == &type_ulong_vtable) {
      UNARY_ASG(unsigned long, ~)
   } else if (*root->type == &type_long_long_vtable) {
      UNARY_ASG(long long, ~)
   } else if (*root->type == &type_ulong_long_vtable) {
      UNARY_ASG(unsigned long long, ~)
   } else {
      assert(0);
   }

   return NULL;
}


// inter_func_shift_left_eval
DEF_SHIFT(shift_left,<<,char,char)
DEF_SHIFT(shift_left,<<,uchar,unsigned char)
DEF_SHIFT(shift_left,<<,short,short)
DEF_SHIFT(shift_left,<<,ushort,unsigned short)
DEF_SHIFT(shift_left,<<,int,int)
DEF_SHIFT(shift_left,<<,uint,unsigned int)
DEF_SHIFT(shift_left,<<,long,long)
DEF_SHIFT(shift_left,<<,ulong,unsigned long)
DEF_SHIFT(shift_left,<<,long_long,long long)
DEF_SHIFT(shift_left,<<,ulong_long,unsigned long long)

error inter_func_shift_left_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   error err;
   int index;
   assert(dlist_root_length(&root->ast_node.children) == 3);

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* arg = (struct ast_node_with_type*)first->ast_node.node.next;
   struct ast_node_with_type* index_node = (struct ast_node_with_type*)arg->ast_node.node.next;

   err = inter_eval_rval(inter, index_node, &index, sizeof(int));
   if (err) return err;
   err = inter_eval_rval(inter, arg, value, size);
   if (err) return err;

   if (*root->type == &type_char_vtable) {
      SHIFT_ASG(char, <<)
   } else if (*root->type == &type_uchar_vtable) {
      SHIFT_ASG(unsigned char, <<)
   } else if (*root->type == &type_short_vtable) {
      SHIFT_ASG(short, <<)
   } else if (*root->type == &type_ushort_vtable) {
      SHIFT_ASG(unsigned short, <<)
   } else if (*root->type == &type_int_vtable) {
      SHIFT_ASG(int, <<)
   } else if (*root->type == &type_uint_vtable) {
      SHIFT_ASG(unsigned int, <<)
   } else if (*root->type == &type_long_vtable) {
      SHIFT_ASG(long, <<)
   } else if (*root->type == &type_ulong_vtable) {
      SHIFT_ASG(unsigned long, <<)
   } else if (*root->type == &type_long_long_vtable) {
      SHIFT_ASG(long long, <<)
   } else if (*root->type == &type_ulong_long_vtable) {
      SHIFT_ASG(unsigned long long, <<)
   } else {
      assert(0);
   }

   return NULL;
}


// inter_func_shift_right_eval
DEF_SHIFT(shift_right,>>,char,char)
DEF_SHIFT(shift_right,>>,uchar,unsigned char)
DEF_SHIFT(shift_right,>>,short,short)
DEF_SHIFT(shift_right,>>,ushort,unsigned short)
DEF_SHIFT(shift_right,>>,int,int)
DEF_SHIFT(shift_right,>>,uint,unsigned int)
DEF_SHIFT(shift_right,>>,long,long)
DEF_SHIFT(shift_right,>>,ulong,unsigned long)
DEF_SHIFT(shift_right,>>,long_long,long long)
DEF_SHIFT(shift_right,>>,ulong_long,unsigned long long)

error inter_func_shift_right_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   error err;
   int index;
   assert(dlist_root_length(&root->ast_node.children) == 3);

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* arg = (struct ast_node_with_type*)first->ast_node.node.next;
   struct ast_node_with_type* index_node = (struct ast_node_with_type*)arg->ast_node.node.next;

   err = inter_eval_rval(inter, index_node, &index, sizeof(int));
   if (err) return err;
   err = inter_eval_rval(inter, arg, value, size);
   if (err) return err;

   if (*root->type == &type_char_vtable) {
      SHIFT_ASG(char, >>)
   } else if (*root->type == &type_uchar_vtable) {
      SHIFT_ASG(unsigned char, >>)
   } else if (*root->type == &type_short_vtable) {
      SHIFT_ASG(short, >>)
   } else if (*root->type == &type_ushort_vtable) {
      SHIFT_ASG(unsigned short, >>)
   } else if (*root->type == &type_int_vtable) {
      SHIFT_ASG(int, >>)
   } else if (*root->type == &type_uint_vtable) {
      SHIFT_ASG(unsigned int, >>)
   } else if (*root->type == &type_long_vtable) {
      SHIFT_ASG(long, >>)
   } else if (*root->type == &type_ulong_vtable) {
      SHIFT_ASG(unsigned long, >>)
   } else if (*root->type == &type_long_long_vtable) {
      SHIFT_ASG(long long, >>)
   } else if (*root->type == &type_ulong_long_vtable) {
      SHIFT_ASG(unsigned long long, >>)
   } else {
      assert(0);
   }

   return NULL;
}








#define CMP_BODY(TYPE, OP) \
   TYPE prev_val; \
   TYPE curr_val; \
   err = inter_eval_rval(inter, curr, &curr_val, sizeof(TYPE)); \
   if (err) return err; \
   curr = (struct ast_node_with_type*)curr->ast_node.node.next; \
   while (curr != first) { \
      prev_val = curr_val; \
      err = inter_eval_rval(inter, curr, &curr_val, sizeof(TYPE)); \
      if (err) return err; \
      if (!(prev_val OP curr_val)) { \
         *((int*)value) = 0; \
         return NULL; \
      } \
      curr = (struct ast_node_with_type*)curr->ast_node.node.next; \
   } \
   *((int*)value) = 1; \
   return NULL; 
   
#define DEF_CMP(OP_NAME,OP,TYPENAME,TYPE) \
error inter_func_##OP_NAME##_eval_##TYPENAME(struct inter* inter, struct ast_node_with_type* root, void* value, int size) { \
   error err; \
   assert(dlist_root_length(&root->ast_node.children) > 1); \
   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node; \
   struct ast_node_with_type* curr = (struct ast_node_with_type*)first->ast_node.node.next; \
   assert(curr != first); \
   CMP_BODY(TYPE,OP) \
}

#define DEF_CMPS(OP_NAME,OP) \
   DEF_CMP(OP_NAME,OP,char,char) \
   DEF_CMP(OP_NAME,OP,uchar,unsigned char) \
   DEF_CMP(OP_NAME,OP,short,short) \
   DEF_CMP(OP_NAME,OP,ushort,unsigned short) \
   DEF_CMP(OP_NAME,OP,int,int) \
   DEF_CMP(OP_NAME,OP,uint,unsigned int) \
   DEF_CMP(OP_NAME,OP,long,long) \
   DEF_CMP(OP_NAME,OP,ulong,unsigned long) \
   DEF_CMP(OP_NAME,OP,long_long,long long) \
   DEF_CMP(OP_NAME,OP,ulong_long,long long) \
   DEF_CMP(OP_NAME,OP,float,float) \
   DEF_CMP(OP_NAME,OP,double,double) \
   DEF_CMP(OP_NAME,OP,long_double,long double)



#define DEF_CMP_EVAL(NAME, OP) \
error inter_func_##NAME##_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) { \
   error err; \
   assert(dlist_root_length(&root->ast_node.children) > 1); \
   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node; \
   struct ast_node_with_type* curr = (struct ast_node_with_type*)first->ast_node.node.next; \
   assert(curr != first); \
   if (*curr->type == &type_char_vtable) { \
      CMP_BODY(char, OP) \
   } else if (*curr->type == &type_uchar_vtable) { \
      CMP_BODY(unsigned char, OP) \
   } else if (*curr->type == &type_short_vtable) { \
      CMP_BODY(short, OP) \
   } else if (*curr->type == &type_ushort_vtable) { \
      CMP_BODY(unsigned short, OP) \
   } else if (*curr->type == &type_int_vtable) { \
      CMP_BODY(int, OP) \
   } else if (*curr->type == &type_uint_vtable) { \
      CMP_BODY(unsigned int, OP) \
   } else if (*curr->type == &type_long_vtable) { \
      CMP_BODY(long, OP) \
   } else if (*curr->type == &type_ulong_vtable) { \
      CMP_BODY(unsigned long, OP) \
   } else if (*curr->type == &type_long_long_vtable) { \
      CMP_BODY(long long, OP) \
   } else if (*curr->type == &type_ulong_long_vtable) { \
      CMP_BODY(unsigned long long, OP) \
   } else if (*curr->type == &type_float_vtable) { \
      CMP_BODY(float, OP) \
   } else if (*curr->type == &type_double_vtable) { \
      CMP_BODY(double, OP) \
   } else if (*curr->type == &type_long_double_vtable) { \
      CMP_BODY(long double, OP) \
   } else { \
      assert(0); \
   } \
   return NULL; \
} 


DEF_CMPS(gt, >=)
DEF_CMP_EVAL(gt, >=)

DEF_CMPS(g, >)
DEF_CMP_EVAL(g, >)

DEF_CMPS(lt, <=)
DEF_CMP_EVAL(lt, <=)

DEF_CMPS(l, <)
DEF_CMP_EVAL(l, <)


// This check also works for neq.
error inter_func_eq_check(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   error err;

   root->returns_lval = 0;
   root->eval_table.eval_rval = inter_func_eq_eval;
   root->eval_table.eval_lval = inter_eval_fallback_lval;

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* curr = (struct ast_node_with_type*)first->ast_node.node.next;

   if (first == curr) {
      return error_syntax(&root->ast_node);
   }

   type arg_types = NULL;

   while (curr != first) {
      err = inter_type_check_expr(inter, arg_types, curr);
      if (err) return err;

      if (arg_types == NULL) {
         arg_types = curr->type;
      }

      curr = (struct ast_node_with_type*)curr->ast_node.node.next;
   }

   return NULL;
}


error inter_func_eq_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   error err;
   assert(dlist_root_length(&root->ast_node.children) > 1);

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* first_arg = (struct ast_node_with_type*)first->ast_node.node.next;
   assert(first_arg != first);

   int arg_size = (*first_arg->type)->type_size(first_arg->type);

   int stack_top = inter->stack.stack_top;

   void* first_p;
   void* curr_p;

   int all_equal = 1;

   err = inter_eval_lval(inter, first_arg, &first_p, arg_size);
   if (!err) {
      struct ast_node_with_type* curr = (struct ast_node_with_type*)first_arg->ast_node.node.next;

      while (all_equal && curr != first) {

         int stack_top = inter->stack.stack_top;

         err = inter_eval_lval(inter, curr, &curr_p, arg_size);
         if (err) break;

         if (memcmp(first_p, curr_p, arg_size)) {
            all_equal = 0;
         }

         if (!curr->returns_lval) {
            inter_destruct_val(inter, curr->type, curr_p);
         }

         inter->stack.stack_top = stack_top;

         curr = (struct ast_node_with_type*)curr->ast_node.node.next;
      }

      *((int*)value) = all_equal;

      if (!first_arg->returns_lval) {
         inter_destruct_val(inter, first_arg->type, first_p);
      }
   }

   inter->stack.stack_top = stack_top;
   return err;
}


error inter_func_neq_check(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   error err;

   root->returns_lval = 0;
   root->eval_table.eval_rval = inter_func_neq_eval;
   root->eval_table.eval_lval = inter_eval_fallback_lval;

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* curr = (struct ast_node_with_type*)first->ast_node.node.next;

   if (first == curr) {
      return error_syntax(&root->ast_node);
   }

   type arg_types = NULL;

   while (curr != first) {
      err = inter_type_check_expr(inter, arg_types, curr);
      if (err) return err;

      if (arg_types == NULL) {
         arg_types = curr->type;
      }

      curr = (struct ast_node_with_type*)curr->ast_node.node.next;
   }

   return NULL;
}



error inter_func_neq_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   error err;
   int num_children = dlist_root_length(&root->ast_node.children);
   assert(num_children > 1);

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* curr = (struct ast_node_with_type*)first->ast_node.node.next;
   struct ast_node_with_type* curr_;

   int num_args = num_children - 1;
   int arg_size = (*curr->type)->type_size(curr->type);
   void** args = alloca(sizeof(void*)*num_args);
   int i = 0;
   int j;

   int stack_start = inter->stack.stack_top;

   while (curr != first) {
      assert (i < num_args);

      err = inter_eval_lval(inter, curr, args + i, arg_size);
      if (err) goto done;

      i++;
      curr = (struct ast_node_with_type*)curr->ast_node.node.next;
   }

   for (i = 1; i < num_args; i++) {
      for (j = 0; j < i; j++) {
         if (!memcmp(args[i], args[j], arg_size)) {
            *((int*)value) = 0;
            goto done;
         }
      }
   }
   *((int*)value) = 1;

done:

   curr_ = (struct ast_node_with_type*)first->ast_node.node.next;
   i = 0;
   while (curr_ != curr) {
      if (!curr_->returns_lval) {
         inter_destruct_val(inter, curr_->type, args[i]);
      }

      i++;
      curr_ = (struct ast_node_with_type*)curr_->ast_node.node.next;
   }

   inter->stack.stack_top = stack_start;
   return err;
}


error inter_func_and_check(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   error err;

   root->returns_lval = 0;
   root->eval_table.eval_rval = inter_func_and_eval;
   root->eval_table.eval_lval = inter_eval_fallback_lval;

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* curr = (struct ast_node_with_type*)first->ast_node.node.next;

   while (curr != first) {
      err = inter_type_check_expr(inter, NULL, curr);
      if (err) return err;

      if (*curr->type != &type_char_vtable &&
          *curr->type != &type_uchar_vtable &&
          *curr->type != &type_short_vtable &&
          *curr->type != &type_ushort_vtable &&
          *curr->type != &type_int_vtable &&
          *curr->type != &type_uint_vtable &&
          *curr->type != &type_long_vtable &&
          *curr->type != &type_ulong_vtable &&
          *curr->type != &type_long_long_vtable &&
          *curr->type != &type_ulong_long_vtable &&
          *curr->type != &type_float_vtable &&
          *curr->type != &type_double_vtable &&
          *curr->type != &type_long_double_vtable &&
          *curr->type != &type_pointer_vtable &&
          *curr->type != &type_string_vtable &&
          *curr->type != &type_pointer_to_vtable) {
         return error_type_mismatch(&curr->ast_node, NULL, curr->type);
      }

      curr = (struct ast_node_with_type*)curr->ast_node.node.next;
   }

   return NULL;
}

error inter_func_or_check(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   error err;

   root->returns_lval = 0;
   root->eval_table.eval_rval = inter_func_or_eval;
   root->eval_table.eval_lval = inter_eval_fallback_lval;

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* curr = (struct ast_node_with_type*)first->ast_node.node.next;

   while (curr != first) {
      err = inter_type_check_expr(inter, NULL, curr);
      if (err) return err;

      if (*curr->type != &type_char_vtable &&
          *curr->type != &type_uchar_vtable &&
          *curr->type != &type_short_vtable &&
          *curr->type != &type_ushort_vtable &&
          *curr->type != &type_int_vtable &&
          *curr->type != &type_uint_vtable &&
          *curr->type != &type_long_vtable &&
          *curr->type != &type_ulong_vtable &&
          *curr->type != &type_long_long_vtable &&
          *curr->type != &type_ulong_long_vtable &&
          *curr->type != &type_float_vtable &&
          *curr->type != &type_double_vtable &&
          *curr->type != &type_long_double_vtable &&
          *curr->type != &type_pointer_vtable &&
          *curr->type != &type_string_vtable &&
          *curr->type != &type_pointer_to_vtable) {
         return error_type_mismatch(&curr->ast_node, NULL, curr->type);
      }

      curr = (struct ast_node_with_type*)curr->ast_node.node.next;
   }

   return NULL;
}



#define LOGIC_CURR(TYPE, TERMINATE, TERM_RES) \
   TYPE curr_val; \
   err = inter_eval_rval(inter, curr, &curr_val, sizeof(TYPE)); \
   if (err) return err; \
   if (TERMINATE) { \
      *((int*)value) = TERM_RES; \
      return NULL; \
   } \

error inter_func_and_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   error err;

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* curr = (struct ast_node_with_type*)first->ast_node.node.next;

   while (curr != first) {
      if (*curr->type == &type_char_vtable) {
         LOGIC_CURR(char, !curr_val, 0)
      } else if (*curr->type == &type_uchar_vtable) {
         LOGIC_CURR(unsigned char, !curr_val, 0)
      } else if (*curr->type == &type_short_vtable) {
         LOGIC_CURR(short, !curr_val, 0)
      } else if (*curr->type == &type_ushort_vtable) {
         LOGIC_CURR(unsigned short, !curr_val, 0)
      } else if (*curr->type == &type_int_vtable) {
         LOGIC_CURR(int, !curr_val, 0)
      } else if (*curr->type == &type_uint_vtable) {
         LOGIC_CURR(unsigned int, !curr_val, 0)
      } else if (*curr->type == &type_long_vtable) {
         LOGIC_CURR(long, !curr_val, 0)
      } else if (*curr->type == &type_ulong_vtable) {
         LOGIC_CURR(unsigned long, !curr_val, 0)
      } else if (*curr->type == &type_long_long_vtable) {
         LOGIC_CURR(long long, !curr_val, 0)
      } else if (*curr->type == &type_ulong_long_vtable) {
         LOGIC_CURR(unsigned long long, !curr_val, 0)
      } else if (*curr->type == &type_float_vtable) {
         LOGIC_CURR(float, !curr_val, 0)
      } else if (*curr->type == &type_double_vtable) {
         LOGIC_CURR(double, !curr_val, 0)
      } else if (*curr->type == &type_long_double_vtable) {
         LOGIC_CURR(long double, !curr_val, 0)
      } else if (*curr->type == &type_string_vtable ||
                 *curr->type == &type_pointer_vtable ||
                 *curr->type == &type_pointer_to_vtable) {
         LOGIC_CURR(void*, !curr_val, 0)
      } else {
         assert(0);
      }

      curr = (struct ast_node_with_type*)curr->ast_node.node.next;
   }

   *((int*)value) = 1;
   return NULL;
}


error inter_func_or_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   error err;

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* curr = (struct ast_node_with_type*)first->ast_node.node.next;

   while (curr != first) {

      if (*curr->type == &type_char_vtable) {
         LOGIC_CURR(char, curr_val, 1)
      } else if (*curr->type == &type_uchar_vtable) {
         LOGIC_CURR(unsigned char, curr_val, 1)
      } else if (*curr->type == &type_short_vtable) {
         LOGIC_CURR(short, curr_val, 1)
      } else if (*curr->type == &type_ushort_vtable) {
         LOGIC_CURR(unsigned short, curr_val, 1)
      } else if (*curr->type == &type_int_vtable) {
         LOGIC_CURR(int, curr_val, 1)
      } else if (*curr->type == &type_uint_vtable) {
         LOGIC_CURR(unsigned int, curr_val, 1)
      } else if (*curr->type == &type_long_vtable) {
         LOGIC_CURR(long, curr_val, 1)
      } else if (*curr->type == &type_ulong_vtable) {
         LOGIC_CURR(unsigned long, curr_val, 1)
      } else if (*curr->type == &type_long_long_vtable) {
         LOGIC_CURR(long long, curr_val, 1)
      } else if (*curr->type == &type_ulong_long_vtable) {
         LOGIC_CURR(unsigned long long, curr_val, 1)
      } else if (*curr->type == &type_float_vtable) {
         LOGIC_CURR(float, curr_val, 1)
      } else if (*curr->type == &type_double_vtable) {
         LOGIC_CURR(double, curr_val, 1)
      } else if (*curr->type == &type_long_double_vtable) {
         LOGIC_CURR(long double, curr_val, 1)
      } else if (*curr->type == &type_string_vtable ||
                 *curr->type == &type_pointer_vtable ||
                 *curr->type == &type_pointer_to_vtable) {
         LOGIC_CURR(void*, curr_val, 1)
      } else {
         assert(0);
      }

      curr = (struct ast_node_with_type*)curr->ast_node.node.next;
   }

   *((int*)value) = 0;
   return NULL;
}


error inter_func_not_check(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   error err;

   int num_children = dlist_root_length(&root->ast_node.children);

   root->returns_lval = 0;
   root->eval_table.eval_rval = inter_func_not_eval;
   root->eval_table.eval_lval = inter_eval_fallback_lval;

   if (num_children != 2) {
      return error_wrong_number_of_args(&root->ast_node, 1, num_children - 1);
   }

   struct ast_node_with_type* value_node = (struct ast_node_with_type*)dlist_root_nth(&root->ast_node.children, 1);

   err = inter_type_check_expr(inter, NULL, value_node);
   if (err) return err;

   if (*value_node->type != &type_char_vtable &&
       *value_node->type != &type_uchar_vtable &&
       *value_node->type != &type_short_vtable &&
       *value_node->type != &type_ushort_vtable &&
       *value_node->type != &type_int_vtable &&
       *value_node->type != &type_uint_vtable &&
       *value_node->type != &type_long_vtable &&
       *value_node->type != &type_ulong_vtable &&
       *value_node->type != &type_long_long_vtable &&
       *value_node->type != &type_ulong_long_vtable &&
       *value_node->type != &type_float_vtable &&
       *value_node->type != &type_double_vtable &&
       *value_node->type != &type_long_double_vtable &&
       *value_node->type != &type_pointer_vtable &&
       *value_node->type != &type_string_vtable &&
       *value_node->type != &type_pointer_to_vtable) {
      return error_type_mismatch(&value_node->ast_node, NULL, value_node->type);
   }

   return NULL;
}

#define NOT_CURR(TYPE) \
   TYPE val; \
   err = inter_eval_rval(inter, value_node, &val, sizeof(val)); \
   if (err) return err; \
   *((int*)value) = !val;

error inter_func_not_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   error err;
   assert(dlist_root_length(&root->ast_node.children) == 2);

   struct ast_node_with_type* value_node = (struct ast_node_with_type*)dlist_root_nth(&root->ast_node.children, 1);

   if (*value_node->type == &type_char_vtable) {
      NOT_CURR(char)
   } else if (*value_node->type == &type_uchar_vtable) {
      NOT_CURR(unsigned char)
   } else if (*value_node->type == &type_short_vtable) {
      NOT_CURR(short)
   } else if (*value_node->type == &type_ushort_vtable) {
      NOT_CURR(unsigned short)
   } else if (*value_node->type == &type_int_vtable) {
      NOT_CURR(int)
   } else if (*value_node->type == &type_uint_vtable) {
      NOT_CURR(unsigned int)
   } else if (*value_node->type == &type_long_vtable) {
      NOT_CURR(long)
   } else if (*value_node->type == &type_ulong_vtable) {
      NOT_CURR(unsigned long)
   } else if (*value_node->type == &type_long_long_vtable) {
      NOT_CURR(long long)
   } else if (*value_node->type == &type_ulong_long_vtable) {
      NOT_CURR(unsigned long long)
   } else if (*value_node->type == &type_string_vtable ||
              *value_node->type == &type_pointer_vtable ||
              *value_node->type == &type_pointer_to_vtable) {
      NOT_CURR(void*)
   } else {
      assert(0);
   }

   return NULL;
}


error inter_func_addr_of_check(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   error err;
   int num_children = dlist_root_length(&root->ast_node.children);

   root->returns_lval = 0;
   root->eval_table.eval_rval = inter_func_addr_of_eval;
   root->eval_table.eval_lval = inter_eval_fallback_lval;

   if (num_children != 2) {
      return error_wrong_number_of_args(&root->ast_node, 1, num_children - 1);
   }

   struct ast_node_with_type* lval_node = (struct ast_node_with_type*)dlist_root_nth(&root->ast_node.children, 1);

   if (expected_type != NULL) {
      if (*expected_type == &type_string_vtable) {
         err = inter_type_check_expr(inter, &type_char, lval_node);
         if (err) return err;
         root->type = &type_string;
      }
      else {
         err = inter_type_check_expr(inter, NULL, lval_node);
         if (err) return err;
         root->type = type_pointer_to(&type_pointer_to_vtable, (*lval_node->type)->type_copy(lval_node->type));
         if (!(*expected_type)->type_match(expected_type, root->type)) {
            return error_type_mismatch(&root->ast_node, expected_type, root->type);
         }
      }
   }
   else {
      err = inter_type_check_expr(inter, NULL, lval_node);
      if (err) return err;
      root->type = type_pointer_to(&type_pointer_to_vtable, (*lval_node->type)->type_copy(lval_node->type));
   }

   return NULL;
}

error inter_func_addr_of_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   assert(dlist_root_length(&root->ast_node.children) == 2);
   struct ast_node_with_type* lval_node = (struct ast_node_with_type*)dlist_root_nth(&root->ast_node.children, 1);

   return inter_eval_lval(inter, lval_node, value, size);
}



error inter_func_array_check(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   error err = NULL;
   int num_children = dlist_root_length(&root->ast_node.children);

   root->returns_lval = 0;
   root->eval_table.eval_rval = inter_func_array_eval;
   root->eval_table.eval_lval = inter_eval_fallback_lval;

   if (num_children < 2) {
      return error_wrong_number_of_args(&root->ast_node, 1, num_children - 1);
   }

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* type_node = (struct ast_node_with_type*)first->ast_node.node.next;

   type_node->type = type_build_ast_node(inter, &type_node->ast_node, &err);
   if (err) return err;

   type content_type_binding;
   TYPE_DEF_CELL_GET_TYPE(type_node->type, &type_node->ast_node, content_type_binding, err)
   if (err) return err;

   type_node->returns_lval = 0;

   int array_length = num_children - 2;

   struct type_array* array_type = (struct type_array*)type_array(array_length, (*content_type_binding)->type_copy(content_type_binding));
   root->type = (type)array_type;
   root->returns_lval = 0;

   err = check_root_type(root, expected_type);
   if (err) return err;

   struct ast_node_with_type* curr = (struct ast_node_with_type*)type_node->ast_node.node.next;

   while (curr != first) {
      err = inter_type_check_expr(inter, content_type_binding, curr);
      if (err) return err;
      curr = (struct ast_node_with_type*)curr->ast_node.node.next;
   }

   return NULL;
}

error inter_func_array_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   error err;
   assert(dlist_root_length(&root->ast_node.children) > 1);

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* type_node = (struct ast_node_with_type*)first->ast_node.node.next;
   struct ast_node_with_type* curr = (struct ast_node_with_type*)type_node->ast_node.node.next;

   struct type_array* type = (struct type_array*)root->type;

   int elt_size = (*type->content_type)->type_size(type->content_type);

   char* buf_pos = value;

   while (curr != first) {
      assert(buf_pos - (char*)value < size);
      err = inter_eval_rval(inter, curr, buf_pos, elt_size);
      if (err) return err;
      buf_pos += elt_size;
      curr = (struct ast_node_with_type*)curr->ast_node.node.next;
   }

   return NULL;
}

error inter_func_array_length_check(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   error err;

   root->returns_lval = 0;
   root->eval_table.eval_rval = inter_func_array_length_eval;
   root->eval_table.eval_lval = inter_eval_fallback_lval;

   int num_children = dlist_root_length(&root->ast_node.children);
   if (num_children != 2) {
      return error_wrong_number_of_args(&root->ast_node, 1, num_children - 1);
   }

   struct ast_node_with_type* array_node = (struct ast_node_with_type*)dlist_root_nth(&root->ast_node.children, 1);

   err = inter_type_check_expr(inter, NULL, array_node);
   if (err) return err;

   assert(array_node->type);
   if (*array_node->type != &type_array_vtable) {
      return error_type_mismatch(&array_node->ast_node, NULL, array_node->type);
   }
   
   return NULL;
}

// Nothing is actually evaluated here. The length
// is extracted from the type information is the
// syntax tree.
error inter_func_array_length_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   assert(dlist_root_length(&root->ast_node.children) == 2);

   struct ast_node_with_type* array_node = (struct ast_node_with_type*)dlist_root_nth(&root->ast_node.children, 1);

   assert(array_node->type);
   assert(*array_node->type == &type_array_vtable);

   struct type_array* array_type = (struct type_array*)array_node->type;

   *((int*)value) = array_type->length;
   return NULL;
}


error inter_func_sizeof_type_check(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   error err = NULL;

   root->returns_lval = 0;
   root->eval_table.eval_rval = inter_func_sizeof_eval;
   root->eval_table.eval_lval = inter_eval_fallback_lval;

   int num_children = dlist_root_length(&root->ast_node.children);
   if (num_children != 2) {
      return error_wrong_number_of_args(&root->ast_node, 1, num_children - 1);
   }

   struct ast_node_with_type* type_node = (struct ast_node_with_type*)dlist_root_nth(&root->ast_node.children, 1);

   type_node->type = type_build_ast_node(inter, &type_node->ast_node, &err);
   if (err) return err;

   err = type_def_cell_bind_type(inter, type_node);
   return err;
}

error inter_func_sizeof_value_check(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   error err;

   root->returns_lval = 0;
   root->eval_table.eval_rval = inter_func_sizeof_eval;
   root->eval_table.eval_lval = inter_eval_fallback_lval;

   int num_children = dlist_root_length(&root->ast_node.children);
   if (num_children != 2) {
      return error_wrong_number_of_args(&root->ast_node, 1, num_children - 1);
   }

   struct ast_node_with_type* type_node = (struct ast_node_with_type*)dlist_root_nth(&root->ast_node.children, 1);

   err = inter_type_check_expr(inter, NULL, type_node);
   return err;
}


error inter_func_sizeof_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   assert(dlist_root_length(&root->ast_node.children) == 2);

   struct ast_node_with_type* type_node = (struct ast_node_with_type*)dlist_root_nth(&root->ast_node.children, 1);

   assert(type_node->type);

   *((int*)value) = (*type_node->type)->type_size(type_node->type);
   return NULL;
}


error inter_func_type_check(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   error err = NULL;

   root->returns_lval = 0;
   root->eval_table.eval_rval = inter_func_type_eval;
   root->eval_table.eval_lval = inter_eval_fallback_lval;

   int num_children = dlist_root_length(&root->ast_node.children);
   if (num_children != 2) {
      return error_wrong_number_of_args(&root->ast_node, 1, num_children - 1);
   }

   struct ast_node_with_type* type_node = (struct ast_node_with_type*)dlist_root_nth(&root->ast_node.children, 1);

   type_node->type = type_build_ast_node(inter, &type_node->ast_node, &err);
   if (err) return err;

   err = type_def_cell_bind_type(inter, type_node);
   return err;
}

error inter_func_typeof_check(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   error err;

   root->returns_lval = 0;
   root->eval_table.eval_rval = inter_func_type_eval;
   root->eval_table.eval_lval = inter_eval_fallback_lval;

   int num_children = dlist_root_length(&root->ast_node.children);
   if (num_children != 2) {
      return error_wrong_number_of_args(&root->ast_node, 1, num_children - 1);
   }

   struct ast_node_with_type* type_node = (struct ast_node_with_type*)dlist_root_nth(&root->ast_node.children, 1);

   err = inter_type_check_expr(inter, NULL, type_node);
   return err;
}


error inter_func_type_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   assert(dlist_root_length(&root->ast_node.children) == 2);

   struct ast_node_with_type* type_node = (struct ast_node_with_type*)dlist_root_nth(&root->ast_node.children, 1);

   assert(type_node->type);

   *((type*)value) = (*type_node->type)->type_copy(type_node->type);
   return NULL;
}



error inter_func_type_size_check(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   error err;

   root->returns_lval = 0;
   root->eval_table.eval_rval = inter_func_type_size_eval;
   root->eval_table.eval_lval = inter_eval_fallback_lval;

   int num_children = dlist_root_length(&root->ast_node.children);
   if (num_children != 2) {
      return error_wrong_number_of_args(&root->ast_node, 1, num_children - 1);
   }

   struct ast_node_with_type* type_node = (struct ast_node_with_type*)dlist_root_nth(&root->ast_node.children, 1);

   err = inter_type_check_expr(inter, &type_type, type_node);
   return err;
}


error inter_func_type_size_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   error err;
   assert(dlist_root_length(&root->ast_node.children) == 2);

   struct ast_node_with_type* type_node = (struct ast_node_with_type*)dlist_root_nth(&root->ast_node.children, 1);

   type type_val;
   err = inter_eval_rval(inter, type_node, &type_val, sizeof(type_val));
   if (err) return err;

   assert(type_val);

   *((int*)value) = (*type_val)->type_size(type_val);

   inter_destruct_val(inter, type_node->type, &type_val);
   return NULL;
}



error inter_func_cast_check(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   error err = NULL;
   int num_children = dlist_root_length(&root->ast_node.children);

   root->returns_lval = 0;
   root->eval_table.eval_rval = inter_func_cast_eval;
   root->eval_table.eval_lval = inter_eval_fallback_lval;

   if (num_children != 3) {
      return error_wrong_number_of_args(&root->ast_node, 2, num_children - 1);
   }

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* type_node = (struct ast_node_with_type*)first->ast_node.node.next;
   struct ast_node_with_type* value_node = (struct ast_node_with_type*)type_node->ast_node.node.next;

   type_node->type = type_build_ast_node(inter, &type_node->ast_node, &err);
   if (err) return err;

   type cast_type;
   TYPE_DEF_CELL_GET_TYPE(type_node->type, &type_node->ast_node, cast_type, err)
   if (err) return err;

   root->type = (*cast_type)->type_copy(cast_type);

   err = inter_type_check_expr(inter, NULL, value_node);
   if (err) return err;

   root->returns_lval = value_node->returns_lval;
   if (root->returns_lval) {
      root->eval_table.eval_rval = inter_eval_fallback_rval;
      root->eval_table.eval_lval = inter_func_cast_eval;
   }

   int value_size = (*value_node->type)->type_size(value_node->type);
   int cast_size = (*cast_type)->type_size(cast_type);

   if (value_size != cast_size) {
      return error_cannot_cast(&root->ast_node, cast_type, value_node->type);
   }

   // Set the value node's type to the root type so the proper
   // functions will be called for copying the value.
   (*value_node->type)->type_free(value_node->type, inter);
   value_node->type = (*root->type)->type_copy(root->type);

   return NULL;
}

error inter_func_cast_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   assert(dlist_root_length(&root->ast_node.children) > 1);

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* type_node = (struct ast_node_with_type*)first->ast_node.node.next;
   struct ast_node_with_type* value_node = (struct ast_node_with_type*)type_node->ast_node.node.next;

   return inter_eval(inter, value_node, value, size);
}


error inter_func_context_check(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   error err = NULL;

   int num_children = dlist_root_length(&root->ast_node.children);

   root->returns_lval = 0;
   root->eval_table.eval_rval = inter_func_context_eval;
   root->eval_table.eval_lval = inter_eval_fallback_lval;

   if (num_children != 3) {
      return error_wrong_number_of_args(&root->ast_node, 2, num_children - 1);
   }

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* type_node = (struct ast_node_with_type*)first->ast_node.node.next;
   struct ast_node_with_type* value_node = (struct ast_node_with_type*)type_node->ast_node.node.next;

   type_node->type = type_build_ast_node(inter, &type_node->ast_node, &err);
   if (err) return err;

   type context_type;
   TYPE_DEF_CELL_GET_TYPE(type_node->type, &type_node->ast_node, context_type, err)
   if (err) return err;

   root->type = (*context_type)->type_copy(context_type);

   err = check_root_type(root, expected_type);
   if (err) return err;

   type_node->type = NULL;
   type_node->returns_lval = 0;

   err = inter_type_check_expr(inter, context_type, value_node);
   if (err) return err;

   root->returns_lval = value_node->returns_lval;
   if (root->returns_lval) {
      root->eval_table.eval_rval = inter_eval_fallback_rval;
      root->eval_table.eval_lval = inter_func_context_eval;
   }

   return NULL;
}


error inter_func_context_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   assert(dlist_root_length(&root->ast_node.children) == 3);
   assert(root->type);

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* type_node = (struct ast_node_with_type*)first->ast_node.node.next;
   struct ast_node_with_type* value_node = (struct ast_node_with_type*)type_node->ast_node.node.next;

   return inter_eval(inter, value_node, value, size);
}


error inter_func_to_char_check(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   error err;

   int num_children = dlist_root_length(&root->ast_node.children);
   if (num_children != 2) {
      return error_wrong_number_of_args(&root->ast_node, 1, num_children - 1);
   }

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* value_node = (struct ast_node_with_type*)first->ast_node.node.next;

   err = inter_type_check_expr(inter, NULL, value_node);
   if (err) return err;

   if (*value_node->type == &type_char_vtable ||
       *value_node->type == &type_uchar_vtable ||
       *value_node->type == &type_short_vtable ||
       *value_node->type == &type_ushort_vtable ||
       *value_node->type == &type_int_vtable ||
       *value_node->type == &type_uint_vtable ||
       *value_node->type == &type_long_vtable ||
       *value_node->type == &type_ulong_vtable ||
       *value_node->type == &type_long_long_vtable ||
       *value_node->type == &type_ulong_long_vtable ||
       *value_node->type == &type_float_vtable ||
       *value_node->type == &type_double_vtable ||
       *value_node->type == &type_long_double_vtable) {
   }
   else {
      return error_type_mismatch(&value_node->ast_node, NULL, value_node->type);
   }

   return NULL;
}

error inter_func_to_uchar_check(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   error err;

   int num_children = dlist_root_length(&root->ast_node.children);
   if (num_children != 2) {
      return error_wrong_number_of_args(&root->ast_node, 1, num_children - 1);
   }

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* value_node = (struct ast_node_with_type*)first->ast_node.node.next;

   err = inter_type_check_expr(inter, NULL, value_node);
   if (err) return err;

   if (*value_node->type == &type_char_vtable ||
       *value_node->type == &type_uchar_vtable ||
       *value_node->type == &type_short_vtable ||
       *value_node->type == &type_ushort_vtable ||
       *value_node->type == &type_int_vtable ||
       *value_node->type == &type_uint_vtable ||
       *value_node->type == &type_long_vtable ||
       *value_node->type == &type_ulong_vtable ||
       *value_node->type == &type_long_long_vtable ||
       *value_node->type == &type_ulong_long_vtable ||
       *value_node->type == &type_float_vtable ||
       *value_node->type == &type_double_vtable ||
       *value_node->type == &type_long_double_vtable) {
   }
   else {
      return error_type_mismatch(&value_node->ast_node, NULL, value_node->type);
   }

   return NULL;
}




error inter_func_to_int_check(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   error err;

   int num_children = dlist_root_length(&root->ast_node.children);
   if (num_children != 2) {
      return error_wrong_number_of_args(&root->ast_node, 1, num_children - 1);
   }

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* value_node = (struct ast_node_with_type*)first->ast_node.node.next;

   err = inter_type_check_expr(inter, NULL, value_node);
   if (err) return err;

   if (*value_node->type == &type_char_vtable ||
       *value_node->type == &type_uchar_vtable ||
       *value_node->type == &type_short_vtable ||
       *value_node->type == &type_ushort_vtable ||
       *value_node->type == &type_int_vtable ||
       *value_node->type == &type_uint_vtable ||
       *value_node->type == &type_long_vtable ||
       *value_node->type == &type_ulong_vtable ||
       *value_node->type == &type_long_long_vtable ||
       *value_node->type == &type_ulong_long_vtable ||
       *value_node->type == &type_float_vtable ||
       *value_node->type == &type_double_vtable ||
       *value_node->type == &type_long_double_vtable ||
       *value_node->type == &type_pointer_vtable) {
   }
   else if (type_is_pointer(value_node->type)) {
      (*value_node->type)->type_free(value_node->type, inter);
      value_node->type = &type_pointer;
   }
   else {
      return error_type_mismatch(&value_node->ast_node, NULL, value_node->type);
   }

   return NULL;
}


#define CONVERT_VAL(FROM_TYPE, TO_TYPE) \
   FROM_TYPE val; \
   err = inter_eval_rval(inter, value_node, &val, sizeof(val)); \
   if (err) return err; \
   *((TO_TYPE*)value) = (TO_TYPE)val;

error inter_func_to_char_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   error err;
   assert(dlist_root_length(&root->ast_node.children) == 2);

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* value_node = (struct ast_node_with_type*)first->ast_node.node.next;

   if (*value_node->type == &type_char_vtable) {
      return inter_eval_rval(inter, value_node, value, size);
   } else if (*value_node->type == &type_uchar_vtable) {
      CONVERT_VAL(unsigned char, char)
   } else if (*value_node->type == &type_short_vtable) {
      CONVERT_VAL(short, char)
   } else if (*value_node->type == &type_ushort_vtable) {
      CONVERT_VAL(unsigned short, char)
   } else if (*value_node->type == &type_int_vtable) {
      CONVERT_VAL(int, char)
   } else if (*value_node->type == &type_uint_vtable) {
      CONVERT_VAL(unsigned int, char)
   } else if (*value_node->type == &type_long_vtable) {
      CONVERT_VAL(long, char)
   } else if (*value_node->type == &type_ulong_vtable) {
      CONVERT_VAL(unsigned long, char)
   } else if (*value_node->type == &type_long_long_vtable) {
      CONVERT_VAL(long long, char)
   } else if (*value_node->type == &type_ulong_long_vtable) {
      CONVERT_VAL(unsigned long long, char)
   } else if (*value_node->type == &type_float_vtable) {
      CONVERT_VAL(float, char)
   } else if (*value_node->type == &type_double_vtable) {
      CONVERT_VAL(double, char)
   } else if (*value_node->type == &type_long_double_vtable) {
      CONVERT_VAL(long double, char)
   } else {
      assert(0);
   }

   return NULL;
}

error inter_func_to_uchar_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   error err;
   assert(dlist_root_length(&root->ast_node.children) == 2);

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* value_node = (struct ast_node_with_type*)first->ast_node.node.next;

   if (*value_node->type == &type_char_vtable) {
      CONVERT_VAL(char, unsigned char)
   } else if (*value_node->type == &type_uchar_vtable) {
      return inter_eval_rval(inter, value_node, value, size);
   } else if (*value_node->type == &type_short_vtable) {
      CONVERT_VAL(short, unsigned char)
   } else if (*value_node->type == &type_ushort_vtable) {
      CONVERT_VAL(unsigned short, unsigned char)
   } else if (*value_node->type == &type_int_vtable) {
      CONVERT_VAL(int, unsigned char)
   } else if (*value_node->type == &type_uint_vtable) {
      CONVERT_VAL(unsigned int, unsigned char)
   } else if (*value_node->type == &type_long_vtable) {
      CONVERT_VAL(long, unsigned char)
   } else if (*value_node->type == &type_ulong_vtable) {
      CONVERT_VAL(unsigned long, unsigned char)
   } else if (*value_node->type == &type_long_long_vtable) {
      CONVERT_VAL(long long, unsigned char)
   } else if (*value_node->type == &type_ulong_long_vtable) {
      CONVERT_VAL(unsigned long long, unsigned char)
   } else if (*value_node->type == &type_float_vtable) {
      CONVERT_VAL(float, unsigned char)
   } else if (*value_node->type == &type_double_vtable) {
      CONVERT_VAL(double, unsigned char)
   } else if (*value_node->type == &type_long_double_vtable) {
      CONVERT_VAL(long double, unsigned char)
   } else {
      assert(0);
   }

   return NULL;
}

error inter_func_to_short_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   error err;
   assert(dlist_root_length(&root->ast_node.children) == 2);

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* value_node = (struct ast_node_with_type*)first->ast_node.node.next;

   if (*value_node->type == &type_char_vtable) {
      CONVERT_VAL(char, short)
   } else if (*value_node->type == &type_uchar_vtable) {
      CONVERT_VAL(unsigned char, short)
   } else if (*value_node->type == &type_short_vtable) {
      return inter_eval_rval(inter, value_node, value, size);
   } else if (*value_node->type == &type_ushort_vtable) {
      CONVERT_VAL(unsigned short, short)
   } else if (*value_node->type == &type_int_vtable) {
      CONVERT_VAL(int, short)
   } else if (*value_node->type == &type_uint_vtable) {
      CONVERT_VAL(unsigned int, short)
   } else if (*value_node->type == &type_long_vtable) {
      CONVERT_VAL(long, short)
   } else if (*value_node->type == &type_ulong_vtable) {
      CONVERT_VAL(unsigned long, short)
   } else if (*value_node->type == &type_long_long_vtable) {
      CONVERT_VAL(long long, short)
   } else if (*value_node->type == &type_ulong_long_vtable) {
      CONVERT_VAL(unsigned long long, short)
   } else if (*value_node->type == &type_float_vtable) {
      CONVERT_VAL(float, short)
   } else if (*value_node->type == &type_double_vtable) {
      CONVERT_VAL(double, short)
   } else if (*value_node->type == &type_long_double_vtable) {
      CONVERT_VAL(long double, short)
   } else {
      assert(0);
   }

   return NULL;
}

error inter_func_to_ushort_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   error err;
   assert(dlist_root_length(&root->ast_node.children) == 2);

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* value_node = (struct ast_node_with_type*)first->ast_node.node.next;

   if (*value_node->type == &type_char_vtable) {
      CONVERT_VAL(char, unsigned short)
   } else if (*value_node->type == &type_uchar_vtable) {
      CONVERT_VAL(unsigned char, unsigned short)
   } else if (*value_node->type == &type_short_vtable) {
      CONVERT_VAL(short, unsigned short)
   } else if (*value_node->type == &type_ushort_vtable) {
      return inter_eval_rval(inter, value_node, value, size);
   } else if (*value_node->type == &type_int_vtable) {
      CONVERT_VAL(int, unsigned short)
   } else if (*value_node->type == &type_uint_vtable) {
      CONVERT_VAL(unsigned int, unsigned short)
   } else if (*value_node->type == &type_long_vtable) {
      CONVERT_VAL(long, unsigned short)
   } else if (*value_node->type == &type_ulong_vtable) {
      CONVERT_VAL(unsigned long, unsigned short)
   } else if (*value_node->type == &type_long_long_vtable) {
      CONVERT_VAL(long long, unsigned short)
   } else if (*value_node->type == &type_ulong_long_vtable) {
      CONVERT_VAL(unsigned long long, unsigned short)
   } else if (*value_node->type == &type_float_vtable) {
      CONVERT_VAL(float, unsigned short)
   } else if (*value_node->type == &type_double_vtable) {
      CONVERT_VAL(double, unsigned short)
   } else if (*value_node->type == &type_long_double_vtable) {
      CONVERT_VAL(long double, unsigned short)
   } else {
      assert(0);
   }

   return NULL;
}

error inter_func_to_int_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   error err;
   assert(dlist_root_length(&root->ast_node.children) == 2);

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* value_node = (struct ast_node_with_type*)first->ast_node.node.next;

   if (*value_node->type == &type_char_vtable) {
      CONVERT_VAL(char, int)
   } else if (*value_node->type == &type_uchar_vtable) {
      CONVERT_VAL(unsigned char, int)
   } else if (*value_node->type == &type_short_vtable) {
      CONVERT_VAL(short, int)
   } else if (*value_node->type == &type_ushort_vtable) {
      CONVERT_VAL(unsigned short, int)
   } else if (*value_node->type == &type_int_vtable) {
      return inter_eval_rval(inter, value_node, value, size);
   } else if(value_node->type == &type_uint) {
      CONVERT_VAL(unsigned int, int)
   } else if (*value_node->type == &type_long_vtable) {
      CONVERT_VAL(long, int)
   } else if (*value_node->type == &type_ulong_vtable) {
      CONVERT_VAL(unsigned long, int)
   } else if (*value_node->type == &type_long_long_vtable) {
      CONVERT_VAL(long long, int)
   } else if (*value_node->type == &type_ulong_long_vtable) {
      CONVERT_VAL(unsigned long long, int)
   } else if (*value_node->type == &type_float_vtable) {
      CONVERT_VAL(float, int)
   } else if (*value_node->type == &type_double_vtable) {
      CONVERT_VAL(double, int)
   } else if (*value_node->type == &type_long_double_vtable) {
      CONVERT_VAL(long double, int)
   } else if (*value_node->type == &type_pointer_vtable) {
      CONVERT_VAL(void*, int)
   } else {
      assert(0);
   }

   return NULL;
}

error inter_func_to_uint_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   error err;
   assert(dlist_root_length(&root->ast_node.children) == 2);

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* value_node = (struct ast_node_with_type*)first->ast_node.node.next;

   if (*value_node->type == &type_char_vtable) {
      CONVERT_VAL(char, unsigned int)
   } else if (*value_node->type == &type_uchar_vtable) {
      CONVERT_VAL(unsigned char, unsigned int)
   } else if (*value_node->type == &type_short_vtable) {
      CONVERT_VAL(short, unsigned int)
   } else if (*value_node->type == &type_ushort_vtable) {
      CONVERT_VAL(unsigned short, unsigned int)
   } else if (*value_node->type == &type_int_vtable) {
      CONVERT_VAL(unsigned int, unsigned int)
   } else if(value_node->type == &type_uint) {
      return inter_eval_rval(inter, value_node, value, size);
   } else if (*value_node->type == &type_long_vtable) {
      CONVERT_VAL(long, unsigned int)
   } else if (*value_node->type == &type_ulong_vtable) {
      CONVERT_VAL(unsigned long, unsigned int)
   } else if (*value_node->type == &type_long_long_vtable) {
      CONVERT_VAL(long long, unsigned int)
   } else if (*value_node->type == &type_ulong_long_vtable) {
      CONVERT_VAL(unsigned long long, unsigned int)
   } else if (*value_node->type == &type_float_vtable) {
      CONVERT_VAL(float, unsigned int)
   } else if (*value_node->type == &type_double_vtable) {
      CONVERT_VAL(double, unsigned int)
   } else if (*value_node->type == &type_long_double_vtable) {
      CONVERT_VAL(long double, unsigned int)
   } else if (*value_node->type == &type_pointer_vtable) {
      CONVERT_VAL(void*, unsigned int)
   } else {
      assert(0);
   }

   return NULL;
}

error inter_func_to_long_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   error err;
   assert(dlist_root_length(&root->ast_node.children) == 2);

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* value_node = (struct ast_node_with_type*)first->ast_node.node.next;

   if (*value_node->type == &type_char_vtable) {
      CONVERT_VAL(char, long)
   } else if (*value_node->type == &type_uchar_vtable) {
      CONVERT_VAL(unsigned char, long)
   } else if (*value_node->type == &type_short_vtable) {
      CONVERT_VAL(short, long)
   } else if (*value_node->type == &type_ushort_vtable) {
      CONVERT_VAL(unsigned short, long)
   } else if (*value_node->type == &type_int_vtable) {
      CONVERT_VAL(int, long)
   } else if (*value_node->type == &type_uint_vtable) {
      CONVERT_VAL(unsigned int, long)
   } else if (*value_node->type == &type_long_vtable) {
      return inter_eval_rval(inter, value_node, value, size);
   } else if (*value_node->type == &type_ulong_vtable) {
      CONVERT_VAL(unsigned long, long)
   } else if (*value_node->type == &type_long_long_vtable) {
      CONVERT_VAL(long long, long)
   } else if (*value_node->type == &type_ulong_long_vtable) {
      CONVERT_VAL(unsigned long long, long)
   } else if (*value_node->type == &type_float_vtable) {
      CONVERT_VAL(float, long)
   } else if (*value_node->type == &type_double_vtable) {
      CONVERT_VAL(double, long)
   } else if (*value_node->type == &type_long_double_vtable) {
      CONVERT_VAL(long double, long)
   } else if (*value_node->type == &type_pointer_vtable) {
      CONVERT_VAL(void*, long)
   } else {
      assert(0);
   }

   return NULL;
}

error inter_func_to_ulong_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   error err;
   assert(dlist_root_length(&root->ast_node.children) == 2);

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* value_node = (struct ast_node_with_type*)first->ast_node.node.next;

   if (*value_node->type == &type_char_vtable) {
      CONVERT_VAL(char, unsigned long)
   } else if (*value_node->type == &type_uchar_vtable) {
      CONVERT_VAL(unsigned char, unsigned long)
   } else if (*value_node->type == &type_short_vtable) {
      CONVERT_VAL(short, unsigned long)
   } else if (*value_node->type == &type_ushort_vtable) {
      CONVERT_VAL(unsigned short, unsigned long)
   } else if (*value_node->type == &type_int_vtable) {
      CONVERT_VAL(int, unsigned long)
   } else if (*value_node->type == &type_uint_vtable) {
      CONVERT_VAL(unsigned int, unsigned long)
   } else if (*value_node->type == &type_long_vtable) {
      CONVERT_VAL(long, unsigned long)
   } else if (*value_node->type == &type_ulong_vtable) {
      return inter_eval_rval(inter, value_node, value, size);
   } else if (*value_node->type == &type_long_long_vtable) {
      CONVERT_VAL(long long, unsigned long)
   } else if (*value_node->type == &type_ulong_long_vtable) {
      CONVERT_VAL(unsigned long long, unsigned long)
   } else if (*value_node->type == &type_float_vtable) {
      CONVERT_VAL(float, unsigned long)
   } else if (*value_node->type == &type_double_vtable) {
      CONVERT_VAL(double, unsigned long)
   } else if (*value_node->type == &type_long_double_vtable) {
      CONVERT_VAL(long double, unsigned long)
   } else if (*value_node->type == &type_pointer_vtable) {
      CONVERT_VAL(void*, unsigned long)
   } else {
      assert(0);
   }

   return NULL;
}

error inter_func_to_long_long_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   error err;
   assert(dlist_root_length(&root->ast_node.children) == 2);

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* value_node = (struct ast_node_with_type*)first->ast_node.node.next;

   if (*value_node->type == &type_char_vtable) {
      CONVERT_VAL(char, long long)
   } else if (*value_node->type == &type_uchar_vtable) {
      CONVERT_VAL(unsigned char, long long)
   } else if (*value_node->type == &type_short_vtable) {
      CONVERT_VAL(short, long long)
   } else if (*value_node->type == &type_ushort_vtable) {
      CONVERT_VAL(unsigned short, long long)
   } else if (*value_node->type == &type_int_vtable) {
      CONVERT_VAL(int, long long)
   } else if (*value_node->type == &type_uint_vtable) {
      CONVERT_VAL(unsigned int, long long)
   } else if (*value_node->type == &type_long_vtable) {
      CONVERT_VAL(long, long long)
   } else if (*value_node->type == &type_ulong_vtable) {
      CONVERT_VAL(unsigned long, long long)
   } else if (*value_node->type == &type_long_long_vtable) {
      return inter_eval_rval(inter, value_node, value, size);
   } else if (*value_node->type == &type_ulong_long_vtable) {
      CONVERT_VAL(unsigned long long, long long)
   } else if (*value_node->type == &type_float_vtable) {
      CONVERT_VAL(float, long long)
   } else if (*value_node->type == &type_double_vtable) {
      CONVERT_VAL(double, long long)
   } else if (*value_node->type == &type_long_double_vtable) {
      CONVERT_VAL(long double, long long)
   } else {
      assert(0);
   }

   return NULL;
}

error inter_func_to_ulong_long_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   error err;
   assert(dlist_root_length(&root->ast_node.children) == 2);

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* value_node = (struct ast_node_with_type*)first->ast_node.node.next;

   if (*value_node->type == &type_char_vtable) {
      CONVERT_VAL(char, unsigned long long)
   } else if (*value_node->type == &type_uchar_vtable) {
      CONVERT_VAL(unsigned char, unsigned long long)
   } else if (*value_node->type == &type_short_vtable) {
      CONVERT_VAL(short, unsigned long long)
   } else if (*value_node->type == &type_ushort_vtable) {
      CONVERT_VAL(unsigned short, unsigned long long)
   } else if (*value_node->type == &type_int_vtable) {
      CONVERT_VAL(int, unsigned long long)
   } else if (*value_node->type == &type_uint_vtable) {
      CONVERT_VAL(unsigned int, unsigned long long)
   } else if (*value_node->type == &type_long_vtable) {
      CONVERT_VAL(long, unsigned long long)
   } else if (*value_node->type == &type_ulong_vtable) {
      CONVERT_VAL(unsigned long, unsigned long long)
   } else if (*value_node->type == &type_long_long_vtable) {
      CONVERT_VAL(long long, unsigned long long)
   } else if (*value_node->type == &type_ulong_long_vtable) {
      return inter_eval_rval(inter, value_node, value, size);
   } else if (*value_node->type == &type_float_vtable) {
      CONVERT_VAL(float, unsigned long long)
   } else if (*value_node->type == &type_double_vtable) {
      CONVERT_VAL(double, unsigned long long)
   } else if (*value_node->type == &type_long_double_vtable) {
      CONVERT_VAL(long double, unsigned long long)
   } else {
      assert(0);
   }

   return NULL;
}


error inter_func_to_float_check(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   error err;
   int num_children = dlist_root_length(&root->ast_node.children);
   if (num_children != 2) {
      return error_wrong_number_of_args(&root->ast_node, 1, num_children - 1);
   }

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* value_node = (struct ast_node_with_type*)first->ast_node.node.next;

   err = inter_type_check_expr(inter, NULL, value_node);
   if (err) return err;

   if (*value_node->type == &type_char_vtable ||
       *value_node->type == &type_uchar_vtable ||
       *value_node->type == &type_short_vtable ||
       *value_node->type == &type_ushort_vtable ||
       *value_node->type == &type_int_vtable ||
       *value_node->type == &type_uint_vtable ||
       *value_node->type == &type_long_vtable ||
       *value_node->type == &type_ulong_vtable ||
       *value_node->type == &type_long_long_vtable ||
       *value_node->type == &type_ulong_long_vtable ||
       *value_node->type == &type_float_vtable ||
       *value_node->type == &type_double_vtable ||
       *value_node->type == &type_long_double_vtable) {
   }
   else {
      return error_type_mismatch(&value_node->ast_node, NULL, value_node->type);
   }

   return NULL;
}


error inter_func_to_float_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   error err;
   assert(dlist_root_length(&root->ast_node.children) == 2);

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* value_node = (struct ast_node_with_type*)first->ast_node.node.next;

   if (*value_node->type == &type_char_vtable) {
      CONVERT_VAL(char, float)
   } else if (*value_node->type == &type_uchar_vtable) {
      CONVERT_VAL(unsigned char, float)
   } else if (*value_node->type == &type_short_vtable) {
      CONVERT_VAL(short, float)
   } else if (*value_node->type == &type_ushort_vtable) {
      CONVERT_VAL(unsigned short, float)
   } else if (*value_node->type == &type_int_vtable) {
      CONVERT_VAL(int, float)
   } else if (*value_node->type == &type_uint_vtable) {
      CONVERT_VAL(unsigned int, float)
   } else if (*value_node->type == &type_long_vtable) {
      CONVERT_VAL(long, float)
   } else if (*value_node->type == &type_ulong_vtable) {
      CONVERT_VAL(unsigned long, float)
   } else if (*value_node->type == &type_long_long_vtable) {
      CONVERT_VAL(long long, float)
   } else if (*value_node->type == &type_ulong_long_vtable) {
      CONVERT_VAL(unsigned long long, float)
   } else if (*value_node->type == &type_float_vtable) {
      return inter_eval_rval(inter, value_node, value, size);
   } else if (*value_node->type == &type_double_vtable) {
      CONVERT_VAL(double, float)
   } else if (*value_node->type == &type_long_double_vtable) {
      CONVERT_VAL(long double, float)
   } else {
      assert(0);
   }

   return NULL;
}


error inter_func_to_double_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   error err;
   assert(dlist_root_length(&root->ast_node.children) == 2);

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* value_node = (struct ast_node_with_type*)first->ast_node.node.next;

   if (*value_node->type == &type_char_vtable) {
      CONVERT_VAL(char, double)
   } else if (*value_node->type == &type_uchar_vtable) {
      CONVERT_VAL(unsigned char, double)
   } else if (*value_node->type == &type_short_vtable) {
      CONVERT_VAL(short, double)
   } else if (*value_node->type == &type_ushort_vtable) {
      CONVERT_VAL(unsigned short, double)
   } else if (*value_node->type == &type_int_vtable) {
      CONVERT_VAL(int, double)
   } else if (*value_node->type == &type_uint_vtable) {
      CONVERT_VAL(unsigned int, double)
   } else if (*value_node->type == &type_long_vtable) {
      CONVERT_VAL(long, double)
   } else if (*value_node->type == &type_ulong_vtable) {
      CONVERT_VAL(unsigned long, double)
   } else if (*value_node->type == &type_long_long_vtable) {
      CONVERT_VAL(long long, double)
   } else if (*value_node->type == &type_ulong_long_vtable) {
      CONVERT_VAL(unsigned long long, double)
   } else if (*value_node->type == &type_float_vtable) {
      CONVERT_VAL(float, double)
   } else if (*value_node->type == &type_double_vtable) {
      return inter_eval_rval(inter, value_node, value, size);
   } else if (*value_node->type == &type_long_double_vtable) {
      CONVERT_VAL(long double, double)
   } else {
      assert(0);
   }

   return NULL;
}


error inter_func_to_long_double_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   error err;
   assert(dlist_root_length(&root->ast_node.children) == 2);

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* value_node = (struct ast_node_with_type*)first->ast_node.node.next;

   if (*value_node->type == &type_char_vtable) {
      CONVERT_VAL(char, long double)
   } else if (*value_node->type == &type_uchar_vtable) {
      CONVERT_VAL(unsigned char, long double)
   } else if (*value_node->type == &type_short_vtable) {
      CONVERT_VAL(short, long double)
   } else if (*value_node->type == &type_ushort_vtable) {
      CONVERT_VAL(unsigned short, long double)
   } else if (*value_node->type == &type_int_vtable) {
      CONVERT_VAL(int, long double)
   } else if (*value_node->type == &type_uint_vtable) {
      CONVERT_VAL(unsigned int, long double)
   } else if (*value_node->type == &type_long_vtable) {
      CONVERT_VAL(long, long double)
   } else if (*value_node->type == &type_ulong_vtable) {
      CONVERT_VAL(unsigned long, long double)
   } else if (*value_node->type == &type_long_long_vtable) {
      CONVERT_VAL(long long, long double)
   } else if (*value_node->type == &type_ulong_long_vtable) {
      CONVERT_VAL(unsigned long long, long double)
   } else if (*value_node->type == &type_float_vtable) {
      CONVERT_VAL(float, long double)
   } else if (*value_node->type == &type_double_vtable) {
      CONVERT_VAL(double, long double)
   } else if (*value_node->type == &type_long_double_vtable) {
      return inter_eval_rval(inter, value_node, value, size);
   } else {
      assert(0);
   }

   return NULL;
}


error inter_func_to_pointer_check(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   error err;
   int num_children = dlist_root_length(&root->ast_node.children);
   if (num_children != 2) {
      return error_wrong_number_of_args(&root->ast_node, 1, num_children - 1);
   }

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* value_node = (struct ast_node_with_type*)first->ast_node.node.next;

   err = inter_type_check_expr(inter, NULL, value_node);
   if (err) return err;


   if (*value_node->type == &type_int_vtable ||
       *value_node->type == &type_uint_vtable ||
       *value_node->type == &type_long_vtable ||
       *value_node->type == &type_ulong_vtable ||
       *value_node->type == &type_pointer_vtable) {
   }
   else if (type_is_pointer(value_node->type)) {
      (*value_node->type)->type_free(value_node->type, inter);
      value_node->type = &type_pointer;
   }
   else {
      return error_type_mismatch(&value_node->ast_node, NULL, value_node->type);
   }

   return NULL;
}


error inter_func_to_pointer_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   error err;
   assert(dlist_root_length(&root->ast_node.children) == 2);

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* value_node = (struct ast_node_with_type*)first->ast_node.node.next;

   if (*value_node->type == &type_int_vtable) {
      CONVERT_VAL(int, void*)
   } else if (*value_node->type == &type_uint_vtable) {
      CONVERT_VAL(unsigned int, void*)
   } else if (*value_node->type == &type_long_vtable) {
      CONVERT_VAL(long, void*)
   } else if (*value_node->type == &type_ulong_vtable) {
      CONVERT_VAL(unsigned long, void*)
   } else if (*value_node->type == &type_pointer_vtable) {
      return inter_eval_rval(inter, value_node, value, size);
   } else {
      assert(0);
   }

   return NULL;
}

error inter_func_code_check(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   error err;
   int num_children = dlist_root_length(&root->ast_node.children);

   root->returns_lval = 0;
   root->eval_table.eval_rval = inter_func_code_eval;
   root->eval_table.eval_lval = inter_eval_fallback_lval;

   if (num_children != 2) {
      return error_wrong_number_of_args(&root->ast_node, 1, num_children - 1);
   }

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* code_node = (struct ast_node_with_type*)first->ast_node.node.next;

   err = inter_type_check_expr(inter, NULL, code_node);
   if (err) return err;

   root->type = type_code(&type_code_vtable, (*code_node->type)->type_copy(code_node->type));

   // Type check occurs again everytime code_node is called.
   ast_node_with_type_free_types(code_node, inter);

   return check_root_type(root, expected_type);
}


error inter_func_code_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   assert(dlist_root_length(&root->ast_node.children) == 2);

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* code_node = (struct ast_node_with_type*)first->ast_node.node.next;

   *((struct ast_node_with_type**)value) = ast_node_with_type_copy_tree(code_node);

   return NULL;
}


error inter_func_code_unsafe_check(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   error err;
   int num_children = dlist_root_length(&root->ast_node.children);

   root->returns_lval = 0;
   root->eval_table.eval_rval = inter_func_code_eval;
   root->eval_table.eval_lval = inter_eval_fallback_lval;

   if (num_children != 2) {
      return error_wrong_number_of_args(&root->ast_node, 1, num_children - 1);
   }

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* code_node = (struct ast_node_with_type*)first->ast_node.node.next;

   err = inter_type_check_expr(inter, NULL, code_node);
   if (err) return err;

   root->type = type_code(&type_code_unsafe_vtable, (*code_node->type)->type_copy(code_node->type));

   return check_root_type(root, expected_type);
}

error inter_func_code_unsafe_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   assert(dlist_root_length(&root->ast_node.children) == 2);

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* code_node = (struct ast_node_with_type*)first->ast_node.node.next;

   *((struct ast_node_with_type**)value) = (struct ast_node_with_type*)refc_copy(code_node);

   return NULL;
}

error inter_func_quote_check(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   int num_children = dlist_root_length(&root->ast_node.children);

   root->returns_lval = 0;
   root->eval_table.eval_rval = inter_func_quote_eval;
   root->eval_table.eval_lval = inter_eval_fallback_lval;

   if (num_children != 2) {
      return error_wrong_number_of_args(&root->ast_node, 1, num_children - 1);
   }

   return NULL;
}


error inter_func_quote_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   assert(dlist_root_length(&root->ast_node.children) == 2);

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* quote_node = (struct ast_node_with_type*)first->ast_node.node.next;

   *((struct ast_node_with_type**)value) = ast_node_with_type_copy_tree(quote_node);

   return NULL;
}


error inter_func_free_quote_check(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   error err;
   int num_children = dlist_root_length(&root->ast_node.children);

   root->returns_lval = 0;
   root->eval_table.eval_rval = inter_func_free_quote_eval;
   root->eval_table.eval_lval = inter_eval_fallback_lval;

   if (num_children != 2) {
      return error_wrong_number_of_args(&root->ast_node, 1, num_children - 1);
   }

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* code_node = (struct ast_node_with_type*)first->ast_node.node.next;

   err = inter_type_check_expr(inter, NULL, code_node);
   if (err) return err;

   if (!type_is_ast_node(code_node->type)) {
      return error_type_mismatch(&code_node->ast_node, NULL, code_node->type);
   }

   // Change type of argument to pointer to prevent refc copy when evaluating.
   (*code_node->type)->type_free(code_node->type, inter);
   code_node->type = &type_pointer;

   return NULL;
}


error inter_func_free_quote_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   error err;
   assert(dlist_root_length(&root->ast_node.children) == 2);

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* code_node = (struct ast_node_with_type*)first->ast_node.node.next;

   struct ast_node_with_type* code_val;
   err = inter_eval_rval(inter, code_node, &code_val, sizeof(code_val));
   if (err) return err;

   ast_node_with_type_rec_free(code_val, inter); 

   return NULL;
}


error inter_func_begin_check(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   error err;
   int num_children = dlist_root_length(&root->ast_node.children);

   root->returns_lval = 0;
   root->eval_table.eval_rval = inter_func_begin_eval;
   root->eval_table.eval_lval = inter_eval_fallback_lval;

   if (num_children < 2) {
      return error_syntax(&root->ast_node);
   }

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* last_node = (struct ast_node_with_type*)first->ast_node.node.prev;
   struct ast_node_with_type* curr = (struct ast_node_with_type*)first->ast_node.node.next;

   err = inter_type_check_expr(inter, expected_type, last_node);
   if (err) return err;

   root->type = (*last_node->type)->type_copy(last_node->type);
   root->returns_lval = last_node->returns_lval;
   if (root->returns_lval) {
      root->eval_table.eval_rval = inter_eval_fallback_rval;
      root->eval_table.eval_lval = inter_func_begin_eval;
   }

   while (curr != last_node) {
      err = inter_type_check_expr(inter, NULL, curr);
      if (err) return err;

      curr = (struct ast_node_with_type*)curr->ast_node.node.next;
   }

   return NULL;
}

error inter_func_begin_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   error err;
   assert(dlist_root_length(&root->ast_node.children) > 1);

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* last_node = (struct ast_node_with_type*)first->ast_node.node.prev;
   struct ast_node_with_type* curr = (struct ast_node_with_type*)first->ast_node.node.next;

   while (curr != last_node) {
      int type_size = (*curr->type)->type_size(curr->type);
      void* value;
      err = inter_eval_lval(inter, curr, &value, type_size);
      if (err) return err;

      if (!curr->returns_lval) {
         inter_destruct_val(inter, curr->type, value);
         stack_decrease_top(&inter->stack, type_size);
      }

      curr = (struct ast_node_with_type*)curr->ast_node.node.next;
   }

   return inter_eval(inter, last_node, value, size);
}


// begin_ defers typechecking of the body to evaluation time.
error inter_func_begin__check(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   int num_children = dlist_root_length(&root->ast_node.children);

   root->returns_lval = 0;
   root->eval_table.eval_rval = inter_func_begin__eval;
   root->eval_table.eval_lval = inter_eval_fallback_lval;

   if (num_children < 2) {
      return error_syntax(&root->ast_node);
   }

   if (expected_type == NULL) {
      return error_type_required(&root->ast_node);
   }

   root->type = (*expected_type)->type_copy(expected_type);

   return NULL;
}

// Any kind of evaluation that changes the type bindings at evaluation
// time must make a copy of the code tree because
// bad things will happen if recursion is used. 
error inter_func_begin__eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   error err = NULL;
   assert(dlist_root_length(&root->ast_node.children) > 1);

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* last_node = (struct ast_node_with_type*)first->ast_node.node.prev;
   struct ast_node_with_type* curr = (struct ast_node_with_type*)first->ast_node.node.next;

   while (!err && curr != last_node) {
      struct ast_node_with_type* curr_copy = ast_node_with_type_copy_tree(curr);

      err = inter_type_check_expr(inter, NULL, curr_copy);
      if (!err) {
         int type_size = (*curr_copy->type)->type_size(curr_copy->type);
         void* value;
         err = inter_eval_lval(inter, curr_copy, &value, type_size);

         if (!err) {
            if (!curr->returns_lval) {
               inter_destruct_val(inter, curr_copy->type, value);
               stack_decrease_top(&inter->stack, type_size);
            }
         }
      }

      ast_node_with_type_rec_free(curr_copy, inter);

      curr = (struct ast_node_with_type*)curr->ast_node.node.next;
   }

   if (!err) {
      struct ast_node_with_type* last_node_copy = ast_node_with_type_copy_tree(last_node);

      err = inter_type_check_expr(inter, root->type, last_node_copy);
      if (!err) {
         err = inter_eval_rval(inter, last_node_copy, value, size);
      }

      ast_node_with_type_rec_free(last_node_copy, inter);
   }

   return err;
}

error inter_do_file(struct inter* inter, struct ast_node_with_type* root, struct token_reader* reader, int interactive) {

   union parser_ret parser_ret;

   struct inter_conv_str str_conv;
   str_conv.fn = (string_conv_fn)inter_conv_str_fn;
   str_conv.inter = inter;

   dlist_init(&root->ast_node.node);

   for (;;) {
      root->type = NULL;
      root->returns_lval = 0;
      root->symbol = NULL;
      root->eval_table.eval_rval = NULL;
      root->eval_table.eval_lval = NULL;

      int ret = ast_node_root(reader, &str_conv.fn, sizeof(struct ast_node_with_type),
                              &root->ast_node, &parser_ret);

      if (reader->curr == EOF) {
         if (root->ast_node.filename)
            inter_release_sym(inter, root->ast_node.filename);
         inter_release_sym(inter, root->ast_node.text);
         break;
      }

#ifdef DEBUG_SYMBOL
      void** value = (void**)hash_map_find_top_value(&inter->values_table, DEBUG_SYMBOL, NULL);
      LOG("inter_do_file: %s = ")
      if (!value) {
         LOG("NULL\n");
      }
      else {
         LOG("%p,%p\n", value, value[1])
      }
#endif

      if (parser_ret.code != PARSER_SUCCESS) {
         if (interactive) {
            parser_error_print(stderr, &parser_ret);
         }
         else {
            return error_syntax(&root->ast_node);
         }
      }
      else if (root->ast_node.state == TOKEN_STATE_BLANK_LINE) {
      }
      else {
         error err;

         err = inter_type_check_expr(inter, NULL, root);
         if (err) {
            if (interactive) {
               (*err)->error_print(stderr, err);
               (*err)->error_free(err);
            }
            else {
               return err;
            }
         }
         else {
            int type_size = (*root->type)->type_size(root->type);

            void* value;
            err = inter_eval_lval(inter, root, &value, type_size);
            if (err) {
               if (interactive) {
                  (*err)->error_print(stderr, err);
                  (*err)->error_free(err);
               }
               else {
                  return err;
               }
            }
            else {
               if (interactive) {
                  (*root->type)->type_print_val(stdout, root->type, value);
                  printf(" : ");
                  (*root->type)->type_print(stdout, root->type);
                  printf("\n");
               }

               if (!root->returns_lval) {
                  inter_destruct_val(inter, root->type, value);
                  stack_decrease_top(&inter->stack, type_size);
               }
            }
         }
      }

      ast_node_with_type_free_children(root, inter);
      inter_release_sym(inter, root->ast_node.text);
      if (root->ast_node.filename) {
         inter_release_sym(inter, root->ast_node.filename);
      }
      if (root->type) {
         (*root->type)->type_free(root->type, inter);
      }
   }

   return NULL;
}

error inter_do_filename(struct inter* inter, struct ast_node* location, char* filename, int filename_len, int interactive) {
   error err;

   FILE* file = fopen(filename, "r");
   if (file == NULL) {
      return error_cannot_open_file(location, (char*)refc_copy(filename), inter);
   }

   struct filepath_node script_path_node;
   filepath_from_str(&script_path_node.filepath, inter, filename, filename_len);
   dlist_root_push(&inter->script_path_stack, &script_path_node.node);

   struct ast_node_with_type file_root;
   struct token_reader reader;
   char buffer[TOKEN_BUF_LEN];

   token_reader_init(&reader, file, filename, buffer, TOKEN_BUF_LEN - 1);

   err = inter_do_file(inter, &file_root, &reader, interactive);

   struct filepath_node* node = (struct filepath_node*)dlist_root_pop(&inter->script_path_stack);
   assert(node == &script_path_node);

   filepath_destruct(&script_path_node.filepath, inter);
   
   fclose(file);
   return err;
}

error inter_func_load_file_check(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   int num_children = dlist_root_length(&root->ast_node.children);

   root->returns_lval = 0;
   root->eval_table.eval_rval = inter_func_load_file_eval;
   root->eval_table.eval_lval = inter_eval_fallback_lval;

   if (num_children != 2) {
      return error_wrong_number_of_args(&root->ast_node, 1, num_children - 1);
   }
   
   struct ast_node_with_type* file_node = (struct ast_node_with_type*)dlist_root_nth(&root->ast_node.children, 1);

   return inter_type_check_expr(inter, &type_pointer, file_node);
}

error inter_func_load_file_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   error err;
   assert(dlist_root_length(&root->ast_node.children) == 2);

   struct ast_node_with_type* file_node = (struct ast_node_with_type*)dlist_root_nth(&root->ast_node.children, 1);

   FILE* file;
   err = inter_eval_rval(inter, file_node, &file, sizeof(file));
   if (err) return err;

   struct ast_node_with_type file_root;
   struct token_reader reader;
   char buffer[TOKEN_BUF_LEN];

   token_reader_init(&reader, file, NULL, buffer, TOKEN_BUF_LEN - 1);

   err = inter_do_file(inter, &file_root, &reader, 0);

   inter_destruct_val(inter, file_node->type, &file);

   return err;
}


void error_cannot_open_file_print(FILE* output, struct error_cannot_open_file* err) {
   if (err->location) {
      fprintf(output, "%s: %d.%d: ",
            err->location->filename, err->location->line, err->location->column);
   }

   fprintf(output, "Cannot open file: %s\n", err->file_name);
}

void error_cannot_open_file_free(struct error_cannot_open_file* err) {
   inter_release_sym(err->inter, err->file_name);
   free(err);
}

struct error_vtable error_cannot_open_file_vtable = {
   .error_print = (error_print_fn)error_cannot_open_file_print,
   .error_free = (error_free_fn)error_cannot_open_file_free
};

error error_cannot_open_file(struct ast_node* location, char* file_name, struct inter* inter) {
   struct error_cannot_open_file* self = (struct error_cannot_open_file*)malloc(sizeof(struct error_cannot_open_file));
   assert(self);
   self->vtable = &error_cannot_open_file_vtable;
   self->location = location;
   self->file_name = (char*)refc_copy(file_name);
   self->inter = inter;
   return (error)self;
}


error inter_func_load_check(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   int num_children = dlist_root_length(&root->ast_node.children);

   root->returns_lval = 0;
   root->eval_table.eval_rval = inter_func_load_eval;
   root->eval_table.eval_lval = inter_eval_fallback_lval;

   if (num_children != 2) {
      return error_wrong_number_of_args(&root->ast_node, 1, num_children - 1);
   }
   
   struct ast_node_with_type* file_name_node = (struct ast_node_with_type*)dlist_root_nth(&root->ast_node.children, 1);

   return inter_type_check_expr(inter, &type_symbol, file_name_node);
}

error inter_func_load_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   error err;
   assert(dlist_root_length(&root->ast_node.children) == 2);

   struct ast_node_with_type* file_name_node = (struct ast_node_with_type*)dlist_root_nth(&root->ast_node.children, 1);

   char* file_name;
   err = inter_eval_rval(inter, file_name_node, &file_name, sizeof(file_name));
   if (err) return err;

   err = inter_do_filename(inter, &root->ast_node, file_name, strlen(file_name), 0);

   inter_destruct_val(inter, file_name_node->type, &file_name);

   return err;
}


error inter_func_import_path_check(struct ast_node* root) {
   if (root->state == TOKEN_STATE_SYMBOL)
      return NULL;

   struct ast_node* first = (struct ast_node*)root->children.entry_node;
   if (!first) {
      return error_syntax(root);
   }

   struct ast_node* curr = first;
   do {
      if (curr->state != TOKEN_STATE_SYMBOL) {
         return error_syntax(curr);
      }

      curr = (struct ast_node*)curr->node.next;
   } while (curr != first);

   return NULL;
}

void inter_func_parse_import_path(struct inter* inter, struct ast_node* root, struct filepath* filepath) {
   if (root->state == TOKEN_STATE_SYMBOL) {
      filepath->filename_sym = (char*)refc_copy(root->text);
      filepath->extension = source_extension;
      dlist_root_init(&filepath->path);
      return;
   }

   struct ast_node* first = (struct ast_node*)root->children.entry_node;
   struct ast_node* last = (struct ast_node*)first->node.prev;

   assert(last->state == TOKEN_STATE_SYMBOL);

   filepath->filename_sym = (char*)refc_copy(last->text);
   filepath->extension = source_extension;
   dlist_root_init(&filepath->path);

   struct ast_node* curr = first;
   while (curr != last) {
      assert(curr->state == TOKEN_STATE_SYMBOL);
      path_append(&filepath->path, curr->text);

      curr = (struct ast_node*)curr->node.next;
   }

   path_reduce(&filepath->path, inter);
}


error inter_func_import_check(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   error err;
   int num_children = dlist_root_length(&root->ast_node.children);

   if (num_children <= 1) {
      return error_wrong_number_of_args(&root->ast_node, 1, num_children - 1);
   }

   struct ast_node* first = (struct ast_node*)root->ast_node.children.entry_node;
   struct ast_node* curr = (struct ast_node*)first->node.next;

   while (curr != first) {
      err = inter_func_import_path_check(curr);
      if (err) return err;

      curr = (struct ast_node*)curr->node.next;
   }

   return NULL;
}


error inter_func_import_abs_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   error err = NULL;
   assert(dlist_root_length(&root->ast_node.children) > 1);

   struct ast_node* first = (struct ast_node*)root->ast_node.children.entry_node;
   struct ast_node* curr = (struct ast_node*)first->node.next;

   while (!err && curr != first) {
      struct filepath_node script_path_node;

      inter_func_parse_import_path(inter, curr, &script_path_node.filepath);

      dlist_root_push(&inter->script_path_stack, &script_path_node.node);

      char* filename = (char*)refc_copy(filepath_sym(&script_path_node.filepath, inter));

      LOG("import_abs: %s\n", filename)

      char** import_slot = (char**)hash_set_slot(&inter->import_set, filename, NULL);

      if (!*import_slot) {
         FILE* file = fopen(filename, "r");
         if (!file) {
            err = error_cannot_open_file(curr, filename, inter);
         }
         else {
            struct ast_node_with_type sub_root;
            struct token_reader reader;
            char buffer[TOKEN_BUF_LEN];
   
            token_reader_init(&reader, file, filename, buffer, TOKEN_BUF_LEN - 1);
   
            err = inter_do_file(inter, &sub_root, &reader, 0);
   
            fclose(file);
         }

         if (!err) {
            *import_slot = (char*)refc_copy(filename);
         }
         else {
            inter->import_set.num_elts--;
         }
      }

      inter_release_sym(inter, filename);

      struct filepath_node* node = (struct filepath_node*)dlist_root_pop(&inter->script_path_stack);
      assert(node == &script_path_node);

      filepath_destruct(&script_path_node.filepath, inter);

      curr = (struct ast_node*)curr->node.next;
   }

   return err;
}


error inter_func_import_rel_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   error err = NULL;
   assert(dlist_root_length(&root->ast_node.children) > 1);

   struct filepath_node* top = (struct filepath_node*)inter->script_path_stack.entry_node;
   assert(top);

   struct ast_node* first = (struct ast_node*)root->ast_node.children.entry_node;
   struct ast_node* curr = (struct ast_node*)first->node.next;

   while (!err && curr != first) {
      struct filepath_node script_path_node;
      struct filepath relative_path;

      inter_func_parse_import_path(inter, curr, &relative_path);
      script_path_node.filepath.filename_sym = relative_path.filename_sym;
      script_path_node.filepath.extension = relative_path.extension;
      path_copy(&top->filepath.path, &script_path_node.filepath.path);
      path_merge(&script_path_node.filepath.path, &relative_path.path, inter);

      dlist_root_push(&inter->script_path_stack, &script_path_node.node);

      char* filename = (char*)refc_copy(filepath_sym(&script_path_node.filepath, inter));


      char** import_slot = (char**)hash_set_slot(&inter->import_set, filename, NULL);

#ifdef LOG_IMPORT
      LOG("import_rel: %p = %s -> (%p) = %p, %d\n", filename, filename, import_slot, *import_slot, ((void**)import_slot) - inter->import_set.array)
      LOG("import_rel: num_imports=%d\n", inter->import_set.num_elts)
#endif

      if (!*import_slot) {
         *import_slot = (char*)refc_copy(filename);

         FILE* file = fopen(filename, "r");
         if (!file) {
            err = error_cannot_open_file(curr, (char*)refc_copy(filename), inter);
         }
         else {
            struct ast_node_with_type sub_root;
            struct token_reader reader;
            char buffer[TOKEN_BUF_LEN];
   
            token_reader_init(&reader, file, filename, buffer, TOKEN_BUF_LEN - 1);
   
            err = inter_do_file(inter, &sub_root, &reader, 0);
   
            fclose(file);
         }

         if (err) {
            hash_set_remove(&inter->import_set, filename, NULL);
            refc_dec(filename);
            assert(refc_count(filename) > 0);
         }
      }

      inter_release_sym(inter, filename);

      struct filepath_node* node = (struct filepath_node*)dlist_root_pop(&inter->script_path_stack);
      assert(node == &script_path_node);

      filepath_destruct(&script_path_node.filepath, inter);

      curr = (struct ast_node*)curr->node.next;
   }

   return err;
}




error inter_func_struct_check(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   error err = NULL;
   int num_children = dlist_root_length(&root->ast_node.children);

   root->returns_lval = 0;
   root->eval_table.eval_rval = inter_func_struct_eval;
   root->eval_table.eval_lval = inter_eval_fallback_lval;

   if (num_children < 2) {
      return error_wrong_number_of_args(&root->ast_node, 2, num_children - 1);
   }

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* type_node = (struct ast_node_with_type*)first->ast_node.node.next;

   type_node->type = type_build_ast_node(inter, &type_node->ast_node, &err);
   if (err) return err;

   type type_binding;
   TYPE_DEF_CELL_GET_TYPE(type_node->type, &type_node->ast_node, type_binding, err)
   if (err) return err;

   root->type = (*type_binding)->type_copy(type_binding);

   if (*type_binding != &type_struct_vtable) {
      return error_type_mismatch(&type_node->ast_node, NULL, root->type);
   }

   err = check_root_type(root, expected_type);
   if (err) return err;

   struct type_struct* struct_type = (struct type_struct*)type_binding;

   struct ast_node_with_type* curr = (struct ast_node_with_type*)type_node->ast_node.node.next;
   
   while (curr != first) {

      if (dlist_root_length(&curr->ast_node.children) != 2) {
         return error_syntax(&curr->ast_node);
      }

      struct ast_node_with_type* name_node = (struct ast_node_with_type*)dlist_root_nth(&curr->ast_node.children, 0);
      struct ast_node_with_type* value_node = (struct ast_node_with_type*)dlist_root_nth(&curr->ast_node.children, 1);

      if (name_node->ast_node.state != TOKEN_STATE_SYMBOL) {
         return error_syntax(&name_node->ast_node);
      }

      struct type_struct_slot* slot = (struct type_struct_slot*)hash_map_find_value(
            &struct_type->fields,
            name_node->ast_node.text,
            NULL);

      if (!slot) {
         return error_field_undefined(&name_node->ast_node);
      }

      err = inter_type_check_expr(inter, slot->type, value_node);
      if (err) return err;

      curr = (struct ast_node_with_type*)curr->ast_node.node.next;
   }
   
   return NULL;
}


#define BIT_SHIFT_SLOT(TYPE) \
   TYPE set_val; \
   TYPE old_val; \
   TYPE final_val; \
   err = inter_eval_rval(inter, value_node, &set_val, sizeof(TYPE)); \
   if (err) return err; \
   old_val = *((TYPE*)slot_location); \
   final_val = BITS_SET_WINDOW(TYPE,old_val, slot->bit_offset, slot->bit_width, set_val); \
   *((TYPE*)slot_location) = final_val;


error inter_func_struct_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   error err = NULL;
   assert(dlist_root_length(&root->ast_node.children) >= 2);

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   assert(first);
   struct ast_node_with_type* type_node = (struct ast_node_with_type*)first->ast_node.node.next;

   struct type_struct* struct_type = (struct type_struct*)root->type;

   struct ast_node_with_type* curr = (struct ast_node_with_type*)type_node->ast_node.node.next;

   while (curr != first) {

      assert(dlist_root_length(&curr->ast_node.children) == 2);

      struct ast_node_with_type* name_node = (struct ast_node_with_type*)curr->ast_node.children.entry_node;
      struct ast_node_with_type* value_node = (struct ast_node_with_type*)name_node->ast_node.node.next;

      assert(name_node->ast_node.state == TOKEN_STATE_SYMBOL);

      struct type_struct_slot* slot = hash_map_find_value(
            &struct_type->fields,
            name_node->ast_node.text,
            NULL);
      assert(slot);

      type slot_type;
      TYPE_DEF_CELL_GET_TYPE(slot->type, &name_node->ast_node, slot_type, err)
      if (err) return err;

      void* slot_location = (char*)value + slot->offset;
      int slot_size = (*slot_type)->type_size(slot_type);

      if (slot->bit_width == 0) {
         err = inter_eval_rval(inter, value_node, slot_location, slot_size);
         if (err) return err;
      }
      else {
         if (*slot_type == &type_char_vtable) {
            BIT_SHIFT_SLOT(char)
         } else if (*slot_type == &type_uchar_vtable) {
            BIT_SHIFT_SLOT(unsigned char)
         } else if (*slot_type == &type_short_vtable) {
            BIT_SHIFT_SLOT(short)
         } else if (*slot_type == &type_ushort_vtable) {
            BIT_SHIFT_SLOT(unsigned short)
         } else if (*slot_type == &type_int_vtable) {
            BIT_SHIFT_SLOT(int)
         } else if (*slot_type == &type_uint_vtable) {
            BIT_SHIFT_SLOT(unsigned int)
         } else if (*slot_type == &type_long_vtable) {
            BIT_SHIFT_SLOT(long)
         } else if (*slot_type == &type_ulong_vtable) {
            BIT_SHIFT_SLOT(unsigned long)
         } else if (*slot_type == &type_long_long_vtable) {
            BIT_SHIFT_SLOT(long long)
         } else if (*slot_type == &type_ulong_long_vtable) {
            BIT_SHIFT_SLOT(unsigned long long)
         } else {
            assert(0);
         }
      }

      curr = (struct ast_node_with_type*)curr->ast_node.node.next;
   }

   return NULL;
}



error inter_func_set_slot_check(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   error err;
   int num_children = dlist_root_length(&root->ast_node.children);

   root->returns_lval = 0;
   root->eval_table.eval_rval = inter_func_set_slot_eval;
   root->eval_table.eval_lval = inter_eval_fallback_lval;

   if (num_children != 4) {
      return error_wrong_number_of_args(&root->ast_node, 3, num_children - 1);
   }

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* struct_node = (struct ast_node_with_type*)first->ast_node.node.next;
   struct ast_node_with_type* field_node = (struct ast_node_with_type*)struct_node->ast_node.node.next;
   struct ast_node_with_type* value_node = (struct ast_node_with_type*)field_node->ast_node.node.next;

   if (field_node->ast_node.state != TOKEN_STATE_SYMBOL) {
      return error_syntax(&field_node->ast_node);
   }

   err = inter_type_check_expr(inter, NULL, struct_node);
   if (err) return err;

   struct type_struct* struct_node_type = (struct type_struct*)struct_node->type;

   if (struct_node_type->vtable != &type_struct_vtable) {
      return error_type_mismatch(&struct_node->ast_node, NULL, struct_node->type);
   }

   struct type_struct_slot* slot = (struct type_struct_slot*)hash_map_find_value(
         &struct_node_type->fields,
         field_node->ast_node.text,
         NULL);

   if (!slot) {
      return error_struct_field_undefined(&field_node->ast_node, struct_node_type);
   }

   type slot_type;
   TYPE_DEF_CELL_GET_TYPE(slot->type, &field_node->ast_node, slot_type, err)
   if (err) return err;

   err = inter_type_check_expr(inter, slot_type, value_node);
   if (err) return err;

   field_node->constant.i = slot->offset;

   if (slot->bit_width != 0) {
      struct_node->constant.bit_section.width = slot->bit_width;
      struct_node->constant.bit_section.offset = slot->bit_offset;

      if (*slot_type == &type_char_vtable) {
         root->eval_table.eval_rval = inter_func_set_slot_eval_bit_char;
      } else if (*slot_type == &type_uchar_vtable) {
         root->eval_table.eval_rval = inter_func_set_slot_eval_bit_uchar;
      } else if (*slot_type == &type_short_vtable) {
         root->eval_table.eval_rval = inter_func_set_slot_eval_bit_short;
      } else if (*slot_type == &type_ushort_vtable) {
         root->eval_table.eval_rval = inter_func_set_slot_eval_bit_ushort;
      } else if (*slot_type == &type_int_vtable) {
         root->eval_table.eval_rval = inter_func_set_slot_eval_bit_int;
      } else if (*slot_type == &type_uint_vtable) {
         root->eval_table.eval_rval = inter_func_set_slot_eval_bit_uint;
      } else if (*slot_type == &type_long_vtable) {
         root->eval_table.eval_rval = inter_func_set_slot_eval_bit_long;
      } else if (*slot_type == &type_ulong_vtable) {
         root->eval_table.eval_rval = inter_func_set_slot_eval_bit_ulong;
      } else if (*slot_type == &type_long_long_vtable) {
         root->eval_table.eval_rval = inter_func_set_slot_eval_bit_long_long;
      } else if (*slot_type == &type_ulong_long_vtable) {
         root->eval_table.eval_rval = inter_func_set_slot_eval_bit_ulong_long;
      } else {
         assert(0);
      }
   }

   return NULL;
}

error inter_func_set_slot_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   error err;
   assert(dlist_root_length(&root->ast_node.children) == 4);

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* struct_node = (struct ast_node_with_type*)first->ast_node.node.next;
   struct ast_node_with_type* field_node = (struct ast_node_with_type*)struct_node->ast_node.node.next;
   struct ast_node_with_type* value_node = (struct ast_node_with_type*)field_node->ast_node.node.next;

   int offset = field_node->constant.i;
   int struct_size = (*struct_node->type)->type_size(struct_node->type);

   void* struct_place;
   err = inter_eval_lval(inter, struct_node, &struct_place, struct_size);
   if (err) return err;

   int value_size = (*value_node->type)->type_size(value_node->type);

   void* slot_location = ((char*)struct_place + offset);
   return inter_eval_rval(inter, value_node, slot_location, value_size);
}

#define DEF_FUNC_SET_SLOT_EVAL_BIT(TYPENAME, TYPE) \
error inter_func_set_slot_eval_bit_##TYPENAME(struct inter* inter, struct ast_node_with_type* root, void* value, int size) { \
   error err; \
   assert(dlist_root_length(&root->ast_node.children) == 4); \
   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node; \
   struct ast_node_with_type* struct_node = (struct ast_node_with_type*)first->ast_node.node.next; \
   struct ast_node_with_type* field_node = (struct ast_node_with_type*)struct_node->ast_node.node.next; \
   struct ast_node_with_type* value_node = (struct ast_node_with_type*)field_node->ast_node.node.next; \
   int offset = field_node->constant.i; \
   int struct_size = (*struct_node->type)->type_size(struct_node->type); \
   int bit_width = struct_node->constant.bit_section.width; \
   int bit_offset = struct_node->constant.bit_section.offset; \
   void* struct_place; \
   err = inter_eval_lval(inter, struct_node, &struct_place, struct_size); \
   if (err) return err; \
   TYPE set_val; \
   err = inter_eval_rval(inter, value_node, &set_val, sizeof(TYPE)); \
   if (err) return err; \
   TYPE* slot_location = (TYPE*)((char*)struct_place + offset); \
   *((TYPE*)slot_location) = BITS_SET_WINDOW(TYPE,*((TYPE*)slot_location), bit_offset, bit_width, set_val); \
   return NULL; \
}

DEF_FUNC_SET_SLOT_EVAL_BIT(char, char)
DEF_FUNC_SET_SLOT_EVAL_BIT(uchar, unsigned char)
DEF_FUNC_SET_SLOT_EVAL_BIT(short, short)
DEF_FUNC_SET_SLOT_EVAL_BIT(ushort, unsigned short)
DEF_FUNC_SET_SLOT_EVAL_BIT(int, int)
DEF_FUNC_SET_SLOT_EVAL_BIT(uint, unsigned int)
DEF_FUNC_SET_SLOT_EVAL_BIT(long, long)
DEF_FUNC_SET_SLOT_EVAL_BIT(ulong, unsigned long)
DEF_FUNC_SET_SLOT_EVAL_BIT(long_long, long long)
DEF_FUNC_SET_SLOT_EVAL_BIT(ulong_long, unsigned long long)



error inter_func_dot_check(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   error err;
   int num_children = dlist_root_length(&root->ast_node.children);

   root->returns_lval = 1;
   root->eval_table.eval_rval = inter_eval_fallback_rval;
   root->eval_table.eval_lval = inter_func_dot_eval_lval;

   if (num_children < 2) {
      return error_wrong_number_of_args(&root->ast_node, 2, num_children - 1);
   }

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* start_node = (struct ast_node_with_type*)first->ast_node.node.next;
   struct ast_node_with_type* curr_field = (struct ast_node_with_type*)start_node->ast_node.node.next;

   first->constant.bit_section.width = 0;

   err = inter_type_check_expr(inter, NULL, start_node);
   if (err) return err;

   type start_type = start_node->type;

   if (*start_type == &type_struct_vtable &&
       !start_node->returns_lval) {
      root->returns_lval = 0;
      root->eval_table.eval_rval = inter_func_dot_eval_rval;
      root->eval_table.eval_lval = inter_eval_fallback_lval;
   }

   if (type_is_pointer(start_type)) {
      struct type_pointer_to* start_type_ = (struct type_pointer_to*)start_type;
      start_type = start_type_->content_type;
   }

   if (*start_type != &type_struct_vtable) {
      return error_type_mismatch(&start_node->ast_node, NULL, start_node->type);
   }

   struct type_struct* start_struct_type = (struct type_struct*)start_type;

   type curr_type = start_type;
   struct type_struct* curr_struct_type = start_struct_type;

   for (;;) {
      struct type_struct_slot* slot = (struct type_struct_slot*)hash_map_find_value(
            &curr_struct_type->fields,
            curr_field->ast_node.text,
            NULL);

      if (!slot) {
         return error_struct_field_undefined(&curr_field->ast_node, curr_struct_type);
      }

      TYPE_DEF_CELL_GET_TYPE(slot->type, &curr_field->ast_node, curr_type, err)
      if (err) return err;

      curr_field->type = (*curr_type)->type_copy(curr_type);
      curr_field->constant.i = slot->offset;

      struct ast_node_with_type* next_field = (struct ast_node_with_type*)curr_field->ast_node.node.next;

      if (next_field == first) {
         root->type = (*curr_type)->type_copy(curr_type);

         if (slot->bit_width) {
            // Storing the bit offset info inside of the first node.
            first->constant.bit_section.offset = slot->bit_offset;
            first->constant.bit_section.width = slot->bit_width;

            root->returns_lval = 0;
            root->eval_table.eval_rval = inter_func_dot_eval_rval;
            root->eval_table.eval_lval = inter_eval_fallback_lval;
         }

         break;
      }

      if (slot->bit_width) {
         return error_lval_expected(&curr_field->ast_node);
      }

      if (type_is_pointer(curr_type)) {
         struct type_pointer_to* curr_type_ = (struct type_pointer_to*)curr_type;

         TYPE_DEF_CELL_GET_TYPE(curr_type_->content_type, &curr_field->ast_node, curr_type, err)
         if (err) return err;
      }

      if (*curr_type != &type_struct_vtable) {
         return error_type_mismatch(&curr_field->ast_node, NULL, curr_field->type);
      }

      curr_struct_type = (struct type_struct*)curr_type;

      curr_field = next_field;
   }

   return check_root_type(root, expected_type);
}


error inter_func_dot_eval_lval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   error err;
   assert(dlist_root_length(&root->ast_node.children) >= 2);

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* start_node = (struct ast_node_with_type*)first->ast_node.node.next;
   struct ast_node_with_type* curr_field = (struct ast_node_with_type*)start_node->ast_node.node.next;

   assert(start_node->returns_lval ||
          type_is_pointer(start_node->type));

   int start_size = (*start_node->type)->type_size(start_node->type);
   int stack_top = inter->stack.stack_top;

   char* start_val;
   err = inter_eval_lval(inter, start_node, &start_val, start_size);
   if (!err) {
      char* curr_val = start_val;

      if (*start_node->type != &type_struct_vtable)
         curr_val = *((char**)curr_val);

      for (;;) {
         curr_val += curr_field->constant.i;

         struct ast_node_with_type* next_field = (struct ast_node_with_type*)curr_field->ast_node.node.next;

         if (next_field == first)
            break;

         if (*curr_field->type != &type_struct_vtable) {
            curr_val = *((char**)curr_val);
         }

         curr_field = next_field;
      }

      *((char**)value) = curr_val;

      if (!start_node->returns_lval) {
         assert(type_is_pointer(start_node->type));

         // If this destruct call leads to the destruction
         // of the place pointed to by value,
         // something bad will happen.
         // Functions that returns auto_pointer or refc_pointer
         // could cause this.

         inter_destruct_val(inter, start_node->type, start_val);
      }
   }

   inter->stack.stack_top = stack_top;
   return err;
}


#define DOT_BIT_SHIFT_SLOT(TYPE) \
   *((TYPE*)value) = BITS_GET_WINDOW(TYPE, *((TYPE*)value),first->constant.bit_section.offset, first->constant.bit_section.width);


error inter_func_dot_eval_rval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   error err;
   assert(dlist_root_length(&root->ast_node.children) >= 2);

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* start_node = (struct ast_node_with_type*)first->ast_node.node.next;
   struct ast_node_with_type* curr_field = (struct ast_node_with_type*)start_node->ast_node.node.next;

   int struct_size = (*start_node->type)->type_size(start_node->type);
   int stack_top = inter->stack.stack_top;

   char* start_val;
   err = inter_eval_lval(inter, start_node, &start_val, struct_size);
   if (!err) {
      char* curr_val = start_val;

      if (*start_node->type != &type_struct_vtable)
         curr_val = *((char**)curr_val);

      for (;;) {
         curr_val += curr_field->constant.i;

         struct ast_node_with_type* next_field = (struct ast_node_with_type*)curr_field->ast_node.node.next;

         if (next_field == first)
            break;

         if (*curr_field->type != &type_struct_vtable) {
            curr_val = *((char**)curr_val);
         }

         curr_field = next_field;
      }

      (*root->type)->type_copy_val(root->type, curr_val, value);

      if (*root->type == &type_char_vtable) {
         DOT_BIT_SHIFT_SLOT(char)
      } else if (*root->type == &type_uchar_vtable) {
         DOT_BIT_SHIFT_SLOT(unsigned char)
      } else if (*root->type == &type_short_vtable) {
         DOT_BIT_SHIFT_SLOT(short)
      } else if (*root->type == &type_ushort_vtable) {
         DOT_BIT_SHIFT_SLOT(unsigned short)
      } else if (*root->type == &type_int_vtable) {
         DOT_BIT_SHIFT_SLOT(int)
      } else if (*root->type == &type_uint_vtable) {
         DOT_BIT_SHIFT_SLOT(unsigned int)
      } else if (*root->type == &type_long_vtable) {
         DOT_BIT_SHIFT_SLOT(long)
      } else if (*root->type == &type_ulong_vtable) {
         DOT_BIT_SHIFT_SLOT(unsigned long)
      } else if (*root->type == &type_long_long_vtable) {
         DOT_BIT_SHIFT_SLOT(long long)
      } else if (*root->type == &type_ulong_long_vtable) {
         DOT_BIT_SHIFT_SLOT(unsigned long long)
      } else {
         assert(0);
      }

      if (!start_node->returns_lval) {
         inter_destruct_val(inter, start_node->type, start_val);
      }
   }

   inter->stack.stack_top = stack_top;
   return err;
}



error inter_func_def_type_check(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   error err = NULL;
   int num_children = dlist_root_length(&root->ast_node.children);

   root->returns_lval = 0;
   root->eval_table.eval_rval = inter_func_def_type_eval;
   root->eval_table.eval_lval = inter_eval_fallback_lval;

   if (num_children != 3 && num_children != 2) {
      return error_wrong_number_of_args(&root->ast_node, 2, num_children - 1);
   }
   
   struct ast_node_with_type* type_name_node = (struct ast_node_with_type*)dlist_root_nth(&root->ast_node.children, 1);
   struct ast_node_with_type* type_node = NULL;

   if (num_children == 3) {
      type_node = (struct ast_node_with_type*)dlist_root_nth(&root->ast_node.children, 2);
   }

   if (type_name_node->ast_node.state != TOKEN_STATE_SYMBOL) {
      return error_syntax(&type_name_node->ast_node);
   }

   inter_cache_type_symbol(inter, type_name_node);

   if (type_node) {
      type type = type_build_ast_node(inter, &type_node->ast_node, &err);
      if (err) return err;

      type_node->type = type;
   }

   return NULL;
}

error inter_func_def_type_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   error err = NULL;
   assert(dlist_root_length(&root->ast_node.children) == 2 ||
          dlist_root_length(&root->ast_node.children) == 3);

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* type_name_node = (struct ast_node_with_type*)first->ast_node.node.next;
   struct ast_node_with_type* type_node = (struct ast_node_with_type*)type_name_node->ast_node.node.next;
   if (type_node == first) {
      type_node = NULL;
   }

   assert(type_name_node->ast_node.state == TOKEN_STATE_SYMBOL);
   assert(type_name_node->symbol);

   type type_val = NULL;

   if (type_node) {
      assert(type_node->type);

      TYPE_DEF_CELL_GET_TYPE(type_node->type, &type_node->ast_node, type_val, err)
      if (err) return err;

      type_val = (*type_val)->type_copy(type_val);
   }

   struct type_def_cell* cell = type_def_cell(type_name_node->ast_node.text, type_val);

   hash_map_sym_push_value(type_name_node->symbol, (void*)cell);

   return NULL;
}


error inter_func_set_type_check(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   error err = NULL;
   int num_children = dlist_root_length(&root->ast_node.children);

   root->returns_lval = 0;
   root->eval_table.eval_rval = inter_func_set_type_eval;
   root->eval_table.eval_lval = inter_eval_fallback_lval;

   if (num_children != 3) {
      return error_wrong_number_of_args(&root->ast_node, 2, num_children - 1);
   }
   
   struct ast_node_with_type* type_name_node = (struct ast_node_with_type*)dlist_root_nth(&root->ast_node.children, 1);
   struct ast_node_with_type* type_node = (struct ast_node_with_type*)dlist_root_nth(&root->ast_node.children, 2);

   if (type_name_node->ast_node.state != TOKEN_STATE_SYMBOL) {
      return error_syntax(&type_name_node->ast_node);
   }

   inter_cache_type_symbol(inter, type_name_node);

   type type = type_build_ast_node(inter, &type_node->ast_node, &err);
   if (err) return err;

   type_node->type = type;

   return NULL;
}

error inter_func_set_type_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   error err = NULL;
   assert(dlist_root_length(&root->ast_node.children) == 3);

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* type_name_node = (struct ast_node_with_type*)first->ast_node.node.next;
   struct ast_node_with_type* type_node = (struct ast_node_with_type*)type_name_node->ast_node.node.next;

   assert(type_name_node->ast_node.state == TOKEN_STATE_SYMBOL);
   assert(type_node->type);
   assert(type_name_node->symbol);

   type type_val;
   TYPE_DEF_CELL_GET_TYPE(type_node->type, &type_node->ast_node, type_val, err)
   if (err) return err;

   type_val = (*type_val)->type_copy(type_val);

   struct type_def_cell* cell = (struct type_def_cell*)hash_map_sym_top_value(type_name_node->symbol);

   if (!cell) {
      return error_symbol_undefined(&type_name_node->ast_node);
   }

   if (cell->value) {
      return error_type_already_bound(&type_name_node->ast_node, cell);
   }

   cell->value = type_val;
   return NULL;
}


error inter_func_undef_type_check(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   int num_children = dlist_root_length(&root->ast_node.children);

   root->returns_lval = 0;
   root->eval_table.eval_rval = inter_func_undef_type_eval;
   root->eval_table.eval_lval = inter_eval_fallback_lval;

   if (num_children != 2) {
      return error_wrong_number_of_args(&root->ast_node, 1, num_children - 1);
   }
   
   struct ast_node_with_type* type_name_node = (struct ast_node_with_type*)dlist_root_nth(&root->ast_node.children, 1);

   if (type_name_node->ast_node.state != TOKEN_STATE_SYMBOL) {
      return error_syntax(&type_name_node->ast_node);
   }

   inter_cache_type_symbol(inter, type_name_node);

   return NULL;
}

error inter_func_undef_type_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   assert(dlist_root_length(&root->ast_node.children) == 2);

   struct ast_node_with_type* type_name_node = (struct ast_node_with_type*)dlist_root_nth(&root->ast_node.children, 1);

   assert(type_name_node->ast_node.state == TOKEN_STATE_SYMBOL);
   assert(type_name_node->symbol);

   struct type_def_cell* type_val = (struct type_def_cell*)hash_map_sym_pop_value(type_name_node->symbol);

   assert(type_val->vtable == &type_def_cell_vtable);

   if (type_val == NULL) {
      return error_unknown_type(&type_name_node->ast_node);
   }

   type_def_cell_unbind_free(inter, type_val);

   return NULL;
}

error inter_func_def_type_cons_check(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   int num_children = dlist_root_length(&root->ast_node.children);

   root->returns_lval = 0;
   root->eval_table.eval_rval = inter_func_def_type_cons_eval;
   root->eval_table.eval_lval = inter_eval_fallback_lval;

   if (num_children != 3) {
      return error_wrong_number_of_args(&root->ast_node, 2, num_children - 1);
   }
   
   struct ast_node_with_type* name_node = (struct ast_node_with_type*)dlist_root_nth(&root->ast_node.children, 1);
   struct ast_node_with_type* handle_node = (struct ast_node_with_type*)dlist_root_nth(&root->ast_node.children, 2);

   if (name_node->ast_node.state != TOKEN_STATE_SYMBOL) {
      return error_syntax(&name_node->ast_node);
   }

   inter_cache_type_cons_symbol(inter, name_node);

   return inter_type_check_expr(inter, &type_pointer, handle_node);
}

error inter_func_def_type_cons_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   error err;
   assert(dlist_root_length(&root->ast_node.children) == 3);

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* name_node = (struct ast_node_with_type*)first->ast_node.node.next;
   struct ast_node_with_type* handle_node = (struct ast_node_with_type*)name_node->ast_node.node.next;

   assert(name_node->ast_node.state == TOKEN_STATE_SYMBOL);

   type_constructor_fn handle;
   err = inter_eval_rval(inter, handle_node, &handle, sizeof(handle));
   if (err) return err;

   assert(name_node->symbol);
   hash_map_sym_push_value(name_node->symbol, (void*)handle);
   return NULL;
}


error inter_func_undef_type_cons_check(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   int num_children = dlist_root_length(&root->ast_node.children);

   root->returns_lval = 0;
   root->eval_table.eval_rval = inter_func_undef_type_cons_eval;
   root->eval_table.eval_lval = inter_eval_fallback_lval;

   if (num_children != 2) {
      return error_wrong_number_of_args(&root->ast_node, 1, num_children - 1);
   }
   
   struct ast_node_with_type* name_node = (struct ast_node_with_type*)dlist_root_nth(&root->ast_node.children, 1);

   if (name_node->ast_node.state != TOKEN_STATE_SYMBOL) {
      return error_syntax(&name_node->ast_node);
   }

   inter_cache_type_cons_symbol(inter, name_node);

   return NULL;
}

error inter_func_undef_type_cons_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   assert(dlist_root_length(&root->ast_node.children) == 2);

   struct ast_node_with_type* name_node = (struct ast_node_with_type*)dlist_root_nth(&root->ast_node.children, 1);

   assert(name_node->ast_node.state == TOKEN_STATE_SYMBOL);

   assert(name_node->symbol);

   type_constructor_fn ret = (type_constructor_fn)hash_map_sym_pop_value(name_node->symbol);
   
   if (!ret) {
      return error_unknown_type(&name_node->ast_node);
   }

   return NULL;
}


error inter_func_def_enum_check(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   error err;
   int num_children = dlist_root_length(&root->ast_node.children);

   root->returns_lval = 0;
   root->eval_table.eval_rval = inter_func_def_enum_eval;
   root->eval_table.eval_lval = inter_eval_fallback_lval;

   if (num_children < 3) {
      return error_wrong_number_of_args(&root->ast_node, 2, num_children - 1);
   }
   
   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* name_node = (struct ast_node_with_type*)first->ast_node.node.next;
   struct ast_node_with_type* alloc_node = (struct ast_node_with_type*)name_node->ast_node.node.next;
   struct ast_node_with_type* curr = (struct ast_node_with_type*)alloc_node->ast_node.node.next;

   if (name_node->ast_node.state != TOKEN_STATE_SYMBOL) {
      return error_syntax(&name_node->ast_node);
   }

   inter_cache_type_symbol(inter, name_node);

   inter_check_alloc_node(alloc_node);

   while (curr != first) {

      if (curr->ast_node.state == TOKEN_STATE_SYMBOL) {
         inter_cache_value_symbol(inter, curr);
      }
      else {
         int curr_children = dlist_root_length(&curr->ast_node.children);

         if (curr_children != 2) {
            return error_wrong_number_of_args(&curr->ast_node, 2, curr_children);
         }

         struct ast_node_with_type* curr_name = (struct ast_node_with_type*)curr->ast_node.children.entry_node;
         struct ast_node_with_type* curr_val = (struct ast_node_with_type*)curr_name->ast_node.node.next;

         if (curr_name->ast_node.state != TOKEN_STATE_SYMBOL) {
            return error_syntax(&curr_name->ast_node);
         }

         inter_cache_value_symbol(inter, curr_name);

         err = inter_type_check_expr(inter, &type_int, curr_val);
         if (err) return err;
      }

      curr = (struct ast_node_with_type*)curr->ast_node.node.next;
   }

   return NULL;
}


error inter_func_def_enum_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   error err = NULL;
   assert(dlist_root_length(&root->ast_node.children) >= 3);

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* name_node = (struct ast_node_with_type*)first->ast_node.node.next;
   struct ast_node_with_type* alloc_node = (struct ast_node_with_type*)name_node->ast_node.node.next;
   struct ast_node_with_type* curr = (struct ast_node_with_type*)alloc_node->ast_node.node.next;

   assert(name_node->ast_node.state == TOKEN_STATE_SYMBOL);

   struct type_def_cell* cell = type_def_cell(name_node->ast_node.text, &type_int);
   hash_map_sym_push_value(name_node->symbol, cell);

   int enum_val = 0;

   while (curr != first) {
      struct ast_node_with_type* curr_name;

      if (curr->ast_node.state == TOKEN_STATE_SYMBOL) {
         curr_name = curr;
      }
      else {
         assert(dlist_root_length(&curr->ast_node.children) == 2);

         curr_name = (struct ast_node_with_type*)curr->ast_node.children.entry_node;
         struct ast_node_with_type* val_node = (struct ast_node_with_type*)curr_name->ast_node.node.next;

         err = inter_eval_rval(inter, val_node, &enum_val, sizeof(int));
         if (err) return err;
      }

      struct typed_value* alloc = (struct typed_value*)inter_eval_alloc_node(
            inter, alloc_node, sizeof(struct typed_value) + sizeof(int), &err);
      if (err) return err;

      alloc->type = &type_int;
      *((int*)alloc->val) = enum_val;

      hash_map_sym_push_value(curr_name->symbol, alloc);

      enum_val++;
      curr = (struct ast_node_with_type*)curr->ast_node.node.next;
   }

   return NULL;
}






error inter_func_prefix_make_check(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   int num_children = dlist_root_length(&root->ast_node.children);

   root->returns_lval = 0;
   root->eval_table.eval_rval = inter_func_prefix_make_eval;
   root->eval_table.eval_lval = inter_eval_fallback_lval;

   if (num_children != 2) {
      return error_wrong_number_of_args(&root->ast_node, 1, num_children - 1);
   }
   
   struct ast_node_with_type* prefix_node = (struct ast_node_with_type*)dlist_root_nth(&root->ast_node.children, 1);

   return inter_type_check_expr(inter, &type_string, prefix_node);
}

error inter_func_prefix_make_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   error err;
   assert(dlist_root_length(&root->ast_node.children) == 2);

   struct ast_node_with_type* prefix_node = (struct ast_node_with_type*)dlist_root_nth(&root->ast_node.children, 1);

   char* str;
   err = inter_eval_rval(inter, prefix_node, &str, sizeof(str));
   if (err) return err;

   prefix_init((struct prefix*)value, (char*)refc_copy(str), strlen(str), inter);
   return NULL;
}


error inter_func_current_prefix_check(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   root->returns_lval = 1;
   root->eval_table.eval_rval = inter_eval_fallback_rval;
   root->eval_table.eval_lval = inter_func_current_prefix_eval;

   int num_children = dlist_root_length(&root->ast_node.children);
   if (num_children != 1) {
      return error_wrong_number_of_args(&root->ast_node, 0, num_children - 1);
   }
   
   return NULL;
}

error inter_func_current_prefix_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   assert(dlist_root_length(&root->ast_node.children) == 1);

   *((struct prefix***)value) = &inter->current_prefix;
   return NULL;
}

error inter_func_in_prefix_check(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   int num_children = dlist_root_length(&root->ast_node.children);

   root->returns_lval = 0;
   root->eval_table.eval_rval = inter_func_in_prefix_eval;
   root->eval_table.eval_lval = inter_eval_fallback_lval;

   if (num_children != 2) {
      return error_wrong_number_of_args(&root->ast_node, 1, num_children - 1);
   }

   struct ast_node_with_type* prefix_node = (struct ast_node_with_type*)dlist_root_nth(&root->ast_node.children, 1);

   if (prefix_node->ast_node.state != TOKEN_STATE_SYMBOL) {
      return error_syntax(&prefix_node->ast_node);
   }

   return NULL;
}

error inter_func_in_prefix_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   assert(dlist_root_length(&root->ast_node.children) == 2);

   struct ast_node_with_type* prefix_node = (struct ast_node_with_type*)dlist_root_nth(&root->ast_node.children, 1);

   assert(prefix_node->ast_node.state == TOKEN_STATE_SYMBOL);

   if (inter->current_prefix) {
      prefix_free(inter->current_prefix, inter);
   }

   inter->current_prefix = (struct prefix*)refc_alloc(sizeof(struct prefix));
   assert(inter->current_prefix);
   prefix_init(inter->current_prefix, (char*)refc_copy(prefix_node->ast_node.text), prefix_node->ast_node.text_len, inter);
   return NULL;
}

error inter_func_use_prefix_check(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   int num_children = dlist_root_length(&root->ast_node.children);

   root->returns_lval = 0;
   root->eval_table.eval_rval = inter_func_use_prefix_eval;
   root->eval_table.eval_lval = inter_eval_fallback_lval;

   if (num_children != 2) {
      return error_wrong_number_of_args(&root->ast_node, 1, num_children - 1);
   }

   struct ast_node_with_type* prefix_node = (struct ast_node_with_type*)dlist_root_nth(&root->ast_node.children, 1);

   if (prefix_node->ast_node.state != TOKEN_STATE_SYMBOL) {
      return error_syntax(&prefix_node->ast_node);
   }

   return NULL;
}

error inter_func_use_prefix_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   assert(dlist_root_length(&root->ast_node.children) == 2);

   struct ast_node_with_type* prefix_node = (struct ast_node_with_type*)dlist_root_nth(&root->ast_node.children, 1);

   assert(prefix_node->ast_node.state == TOKEN_STATE_SYMBOL);

   struct prefix* prefix = (struct prefix*)refc_alloc(sizeof(struct prefix));
   assert(prefix);
   prefix_init(prefix, (char*)refc_copy(prefix_node->ast_node.text), prefix_node->ast_node.text_len, inter);
   dlist_root_push(&inter->prefix_list, &prefix->node);

   return NULL;
}


error inter_func_clear_prefixes_check(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   int num_children = dlist_root_length(&root->ast_node.children);

   root->returns_lval = 0;
   root->eval_table.eval_rval = inter_func_clear_prefixes_eval;
   root->eval_table.eval_lval = inter_eval_fallback_lval;

   if (num_children != 1) {
      return error_wrong_number_of_args(&root->ast_node, 0, num_children - 1);
   }
   
   return NULL;
}

error inter_func_clear_prefixes_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   assert(dlist_root_length(&root->ast_node.children) == 1);

   inter_clear_prefixes(inter);

   return NULL;
}


error inter_func_script_function_check(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   error err = NULL;
   int num_children = dlist_root_length(&root->ast_node.children);

   root->returns_lval = 0;
   root->eval_table.eval_rval = inter_func_script_function_eval;
   root->eval_table.eval_lval = inter_eval_fallback_lval;

   if (num_children != 4) {
      return error_wrong_number_of_args(&root->ast_node, 3, num_children - 1);
   }

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* args_node = (struct ast_node_with_type*)first->ast_node.node.next;
   struct ast_node_with_type* ret_type_node = (struct ast_node_with_type*)args_node->ast_node.node.next;
   struct ast_node_with_type* code_node = (struct ast_node_with_type*)ret_type_node->ast_node.node.next;

   struct type_script_function* func_type = (struct type_script_function*)type_construct_script_function_(inter, &args_node->ast_node, &ret_type_node->ast_node, &err);
   if (err) return err;

   root->type = (type)func_type;

   err = check_root_type(root, expected_type);
   if (err) return err;

   int stack_top = inter->stack.stack_top;
   int i;
   int j;
   for (i = func_type->num_arguments - 1; i >= 0; i--) {
      struct type_script_function_arg* arg = func_type->arguments + i;

      type arg_type;
      TYPE_DEF_CELL_GET_TYPE(arg->type, &root->ast_node, arg_type, err)
      if (err) goto pop_args;

      struct typed_value* binding = (struct typed_value*)stack_alloc(&inter->stack, typed_value_size(arg_type));
      assert(binding);
      binding->type = arg_type;
      hash_map_sym_push_value(arg->symbol, (void*)binding);
   }

   i++;

   err = inter_type_check_expr(inter, func_type->return_type, code_node);

pop_args:
   for (j = i; j < func_type->num_arguments; j++) {
      struct type_script_function_arg* arg = func_type->arguments + j;
      hash_map_sym_pop_value(arg->symbol);
   }

   inter->stack.stack_top = stack_top;
   return err;
}

error inter_func_script_function_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   assert(dlist_root_length(&root->ast_node.children) == 4);

   struct ast_node_with_type* code_node = (struct ast_node_with_type*)dlist_root_nth(&root->ast_node.children, 3);

   *((struct ast_node_with_type**)value) = (struct ast_node_with_type*)refc_copy(code_node);

   return NULL;
}


error inter_func_free_check(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   error err;
   int num_children = dlist_root_length(&root->ast_node.children);

   root->returns_lval = 0;
   root->eval_table.eval_rval = inter_func_free_eval;
   root->eval_table.eval_lval = inter_eval_fallback_lval;

   if (num_children != 2) {
      return error_syntax(&root->ast_node);
   }

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* value_node = (struct ast_node_with_type*)first->ast_node.node.next;

   err = inter_type_check_expr(inter, NULL, value_node);
   if (err) return err;

   if (!type_is_pointer(value_node->type)) {
      return error_type_mismatch(&value_node->ast_node, NULL, value_node->type);
   }

   // Prevent smart copying
   (*value_node->type)->type_free(value_node->type, inter);
   value_node->type = &type_pointer;

   return NULL;
}

error inter_func_free_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   error err;
   assert(dlist_root_length(&root->ast_node.children) == 2);

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* value_node = (struct ast_node_with_type*)first->ast_node.node.next;

   void* arg;
   err = inter_eval_rval(inter, value_node, &arg, sizeof(arg));
   if (err) return err;

   free(arg);

   return NULL;
}


void error_break_print(FILE* output, struct error_at_node* self) {
   fprintf(output, "%s: %d.%d: Break.\n",
         self->location->filename, self->location->line, self->location->column);
}

struct error_vtable error_break_vtable = {
   .error_print = (error_print_fn)error_break_print,
   .error_free = (error_free_fn)free
};

error error_break(struct ast_node* location) {
   struct error_at_node* self = malloc(sizeof(struct error_at_node));
   assert(self);
   self->vtable = &error_break_vtable;
   self->location = location;
   return (error)self;
}


void error_continue_print(FILE* output, struct error_at_node* self) {
   fprintf(output, "%s: %d.%d: Continue.\n",
         self->location->filename, self->location->line, self->location->column);
}

struct error_vtable error_continue_vtable = {
   .error_print = (error_print_fn)error_continue_print,
   .error_free = (error_free_fn)free
};

error error_continue(struct ast_node* location) {
   struct error_at_node* self = malloc(sizeof(struct error_at_node));
   assert(self);
   self->vtable = &error_continue_vtable;
   self->location = location;
   return (error)self;
}


error inter_func_break_check(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   error err = NULL;
   int num_children = dlist_root_length(&root->ast_node.children);

   root->returns_lval = 0;
   root->eval_table.eval_rval = inter_func_break_eval;
   root->eval_table.eval_lval = inter_eval_fallback_lval;

   if (num_children != 1) {
      return error_syntax(&root->ast_node);
   }

   return NULL;
}

error inter_func_continue_check(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   error err = NULL;
   int num_children = dlist_root_length(&root->ast_node.children);

   root->returns_lval = 0;
   root->eval_table.eval_rval = inter_func_continue_eval;
   root->eval_table.eval_lval = inter_eval_fallback_lval;

   if (num_children != 1) {
      return error_syntax(&root->ast_node);
   }

   return NULL;
}


error inter_func_break_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   assert(dlist_root_length(&root->ast_node.children) == 1);

   return error_break(&root->ast_node);
}

error inter_func_continue_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   assert(dlist_root_length(&root->ast_node.children) == 1);

   return error_continue(&root->ast_node);
}




error inter_func_forever_check(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   error err;
   int num_children = dlist_root_length(&root->ast_node.children);

   root->returns_lval = 0;
   root->eval_table.eval_rval = inter_func_forever_eval;
   root->eval_table.eval_lval = inter_eval_fallback_lval;

   if (num_children < 2) {
      return error_syntax(&root->ast_node);
   }

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* curr = (struct ast_node_with_type*)first->ast_node.node.next;

   while (curr != first) {
      err = inter_type_check_expr(inter, NULL, curr);
      if (err) return err;

      curr = (struct ast_node_with_type*)curr->ast_node.node.next;
   }

   return NULL;
}

error inter_func_forever_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   error err;
   assert(dlist_root_length(&root->ast_node.children) > 1);

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* start = (struct ast_node_with_type*)first->ast_node.node.next;
   struct ast_node_with_type* curr = start;

   for (;;) {
      int curr_size = (*curr->type)->type_size(curr->type);
      void* curr_value;
      err = inter_eval_lval(inter, curr, &curr_value, curr_size);
      if (!err) {
         if (!curr->returns_lval) {
            inter_destruct_val(inter, curr->type, curr_value);
            stack_decrease_top(&inter->stack, curr_size);
         }

         curr = (struct ast_node_with_type*)curr->ast_node.node.next;
         if (curr == first)
            curr = start;
      }
      else {
         if (*err == &error_break_vtable) {
            (*err)->error_free(err);
            break;
         }
         else if (*err == &error_continue_vtable) {
            (*err)->error_free(err);
            curr = start;
         }
         else {
            return err;
         }
      }
   }

   return NULL;
}

error inter_func_while__check(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   error err;
   int num_children = dlist_root_length(&root->ast_node.children);

   if (num_children < 3) {
      return error_syntax(&root->ast_node);
   }

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* cond_node = (struct ast_node_with_type*)first->ast_node.node.next;
   struct ast_node_with_type* curr = (struct ast_node_with_type*)cond_node->ast_node.node.next;

   err = inter_type_check_expr(inter, &type_int, cond_node);
   if (err) return err;

   while (curr != first) {
      err = inter_type_check_expr(inter, NULL, curr);
      if (err) return err;

      curr = (struct ast_node_with_type*)curr->ast_node.node.next;
   }

   return NULL;
}



error inter_func_while_check(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   root->returns_lval = 0;
   root->eval_table.eval_rval = inter_func_while_eval;
   root->eval_table.eval_lval = inter_eval_fallback_lval;

   return inter_func_while__check(inter, expected_type, root);
}

error inter_func_do_while_check(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   root->returns_lval = 0;
   root->eval_table.eval_rval = inter_func_do_while_eval;
   root->eval_table.eval_lval = inter_eval_fallback_lval;

   return inter_func_while__check(inter, expected_type, root);
}


error inter_func_while_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   error err;
   assert(dlist_root_length(&root->ast_node.children) > 1);

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* cond_node = (struct ast_node_with_type*)first->ast_node.node.next;
   struct ast_node_with_type* start = (struct ast_node_with_type*)cond_node->ast_node.node.next;
   struct ast_node_with_type* curr = first;

   for (;;) {
      if (curr == first) {
         curr = start;

         int cond_res;
         err = inter_eval_rval(inter, cond_node, &cond_res, sizeof(int));
         if (err) return err;

#ifdef LOG_WHILE
         LOG("inter_eval_while %s:%d.%d: condition: %d\n",
               root->ast_node.filename, root->ast_node.line, root->ast_node.column, cond_res)
#endif

         if (!cond_res)
            break;
      }

#ifdef LOG_WHILE
      LOG("inter_eval_while at %s:%d.%d: ",
            curr->ast_node.filename, curr->ast_node.line, curr->ast_node.column)
      ast_node_print(LOG_FILE, &curr->ast_node);
      LOG("\n")
#endif

      int curr_size = (*curr->type)->type_size(curr->type);
      void* curr_value;
      err = inter_eval_lval(inter, curr, &curr_value, curr_size);
      if (!err) {
         if (!curr->returns_lval) {
            inter_destruct_val(inter, curr->type, curr_value);
            stack_decrease_top(&inter->stack, curr_size);
         }
         curr = (struct ast_node_with_type*)curr->ast_node.node.next;
      }
      else {
         if (*err == &error_break_vtable) {
            (*err)->error_free(err);
            break;
         }
         else if (*err == &error_continue_vtable) {
            (*err)->error_free(err);
            curr = first;
         }
         else {
            return err;
         }
      }
   }

   return NULL;
}

error inter_func_do_while_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   error err;
   assert(dlist_root_length(&root->ast_node.children) > 1);

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* cond_node = (struct ast_node_with_type*)first->ast_node.node.next;
   struct ast_node_with_type* start = (struct ast_node_with_type*)cond_node->ast_node.node.next;
   struct ast_node_with_type* curr = start;

   for (;;) {
      if (curr == first) {
         curr = start;

         int cond_res;
         err = inter_eval_rval(inter, cond_node, &cond_res, sizeof(int));
         if (err) return err;

         if (!cond_res)
            break;
      }

      int curr_size = (*curr->type)->type_size(curr->type);
      void* curr_value;
      err = inter_eval_lval(inter, curr, &curr_value, curr_size);
      if (!err) {
         if (!curr->returns_lval) {
            inter_destruct_val(inter, curr->type, curr_value);
            stack_decrease_top(&inter->stack, curr_size);
         }

         curr = (struct ast_node_with_type*)curr->ast_node.node.next;
      }
      else {
         if (*err == &error_break_vtable) {
            (*err)->error_free(err);
            break;
         }
         else if (*err == &error_continue_vtable) {
            (*err)->error_free(err);
            curr = first;
         }
         else {
            return err;
         }
      }
   }

   return NULL;
}


error inter_func_for_check(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   error err;
   int num_children = dlist_root_length(&root->ast_node.children);

   root->returns_lval = 0;
   root->eval_table.eval_rval = inter_func_for_eval;
   root->eval_table.eval_lval = inter_eval_fallback_lval;

   if (num_children < 5) {
      return error_syntax(&root->ast_node);
   }

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* init_node = (struct ast_node_with_type*)first->ast_node.node.next;
   struct ast_node_with_type* cond_node = (struct ast_node_with_type*)init_node->ast_node.node.next;
   struct ast_node_with_type* inc_node = (struct ast_node_with_type*)cond_node->ast_node.node.next;
   struct ast_node_with_type* curr = (struct ast_node_with_type*)inc_node->ast_node.node.next;

   err = inter_type_check_expr(inter, NULL, init_node);
   if (err) return err;

   err = inter_type_check_expr(inter, &type_int, cond_node);
   if (err) return err;

   err = inter_type_check_expr(inter, NULL, inc_node);
   if (err) return err;

   while (curr != first) {
      err = inter_type_check_expr(inter, NULL, curr);
      if (err) return err;

      curr = (struct ast_node_with_type*)curr->ast_node.node.next;
   }

   return NULL;
}

error inter_func_for_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   error err;
   assert(dlist_root_length(&root->ast_node.children) > 1);

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* init_node = (struct ast_node_with_type*)first->ast_node.node.next;
   struct ast_node_with_type* cond_node = (struct ast_node_with_type*)init_node->ast_node.node.next;
   struct ast_node_with_type* inc_node = (struct ast_node_with_type*)cond_node->ast_node.node.next;
   struct ast_node_with_type* start = (struct ast_node_with_type*)inc_node->ast_node.node.next;
   struct ast_node_with_type* curr = start;

   int cond_res;
   void* curr_value;
   int curr_size;

   curr_size = (*init_node->type)->type_size(init_node->type);
   err = inter_eval_lval(inter, init_node, &curr_value, curr_size);
   if (err) return err;

   if (!init_node->returns_lval) {
      inter_destruct_val(inter, init_node->type, curr_value);
      stack_decrease_top(&inter->stack, curr_size);
   }

   err = inter_eval_rval(inter, cond_node, &cond_res, sizeof(int));
   if (err) return err;

   if (!cond_res)
      return NULL;

   for (;;) {
      curr_size = (*curr->type)->type_size(curr->type);
      err = inter_eval_lval(inter, curr, &curr_value, curr_size);
      if (!err) {
         if (!curr->returns_lval) {
            inter_destruct_val(inter, curr->type, curr_value);
            stack_decrease_top(&inter->stack, curr_size);
         }

         curr = (struct ast_node_with_type*)curr->ast_node.node.next;
      }
      else {
         if (*err == &error_break_vtable) {
            (*err)->error_free(err);
            break;
         }
         else if (*err == &error_continue_vtable) {
            (*err)->error_free(err);
            curr = first;
         }
         else {
            return err;
         }
      }

      if (curr == first) {
         curr = start;

         curr_size = (*inc_node->type)->type_size(inc_node->type);
         err = inter_eval_lval(inter, inc_node, &curr_value, curr_size);
         if (err) return err;

         if (!inc_node->returns_lval) {
            inter_destruct_val(inter, inc_node->type, curr_value);
            stack_decrease_top(&inter->stack, curr_size);
         }

         err = inter_eval_rval(inter, cond_node, &cond_res, sizeof(int));
         if (err) return err;

         if (!cond_res)
            break;
      }
   }

   return NULL;
}


error inter_func_let_check(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   error err = NULL;
   int num_children = dlist_root_length(&root->ast_node.children);

   root->returns_lval = 0;
   root->eval_table.eval_rval = inter_func_let_eval;
   root->eval_table.eval_lval = inter_eval_fallback_lval;

   if (num_children < 2) {
      return error_syntax(&root->ast_node);
   }

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* last_node = (struct ast_node_with_type*)first->ast_node.node.prev;
   struct ast_node_with_type* curr = (struct ast_node_with_type*)first->ast_node.node.next;

   int stack_top = inter->stack.stack_top;

   while (curr != last_node) {
      struct ast_node_with_type* name_node;
      struct ast_node_with_type* type_node = NULL;
      struct ast_node_with_type* value_node;

      int curr_children = dlist_root_length(&curr->ast_node.children);

      if (curr->ast_node.state != TOKEN_STATE_PUNCTUATION ||
          (curr_children != 2 && curr_children != 1)) {
         err = error_syntax(&curr->ast_node);
         goto pop_args;
      }

      name_node = (struct ast_node_with_type*)curr->ast_node.children.entry_node;

      if (curr_children == 2) {
         value_node = (struct ast_node_with_type*)name_node->ast_node.node.next;
      }
      else {
         value_node = NULL;
      }

      if (name_node->ast_node.state == TOKEN_STATE_PUNCTUATION) {
         if (dlist_root_length(&name_node->ast_node.children) != 2) {
            err = error_syntax(&name_node->ast_node);
            goto pop_args;
         }

         type_node = (struct ast_node_with_type*)dlist_root_nth(&name_node->ast_node.children, 1);
         name_node = (struct ast_node_with_type*)dlist_root_nth(&name_node->ast_node.children, 0);
      }

      if (name_node->ast_node.state != TOKEN_STATE_SYMBOL) {
         err = error_syntax(&name_node->ast_node);
         goto pop_args;
      }

      if (!type_node && !value_node) {
         err = error_syntax(&curr->ast_node);
         goto pop_args;
      }

      if (type_node) {
         name_node->type = type_build_ast_node(inter, &type_node->ast_node, &err);
         if (err) goto pop_args;
      }

      if (value_node) {
         err = inter_type_check_expr(inter, name_node->type, value_node);
         if (err) goto pop_args;

         if (!name_node->type)
            name_node->type = (*value_node->type)->type_copy(value_node->type);
      }

      err = type_def_cell_bind_type(inter, name_node);
      if (err) goto pop_args;

      struct typed_value* binding = (struct typed_value*)stack_alloc(&inter->stack, typed_value_size(name_node->type));
      assert(binding);
      binding->type = name_node->type; // Skip ref count inc. Don't call typed_value_destruct.
      inter_cache_value_symbol(inter, name_node);
      hash_map_sym_push_value(name_node->symbol, (void*)binding);

      curr = (struct ast_node_with_type*)curr->ast_node.node.next;
   }

   err = inter_type_check_expr(inter, expected_type, last_node);
   if (err) goto pop_args;

   root->type = (*last_node->type)->type_copy(last_node->type);
   root->returns_lval = last_node->returns_lval;
   if (root->returns_lval) {
      root->eval_table.eval_rval = inter_eval_fallback_rval;
      root->eval_table.eval_lval = inter_func_let_eval;
   }

   err = check_root_type(root, expected_type);

pop_args:
   curr = (struct ast_node_with_type*)curr->ast_node.node.prev;
   while(curr != first) {

      struct ast_node_with_type* name_node = (struct ast_node_with_type*)curr->ast_node.children.entry_node;
      assert(name_node);

      if (name_node->ast_node.state == TOKEN_STATE_PUNCTUATION) {
         name_node = (struct ast_node_with_type*)name_node->ast_node.children.entry_node;
      }

      assert(name_node);
      assert(name_node->ast_node.state == TOKEN_STATE_SYMBOL);
      assert(name_node->symbol);

      hash_map_sym_pop_value(name_node->symbol);
      curr = (struct ast_node_with_type*)curr->ast_node.node.prev;
   }

   inter->stack.stack_top = stack_top;

   return err;
}


error inter_func_let_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   error err;
   assert(dlist_root_length(&root->ast_node.children) >= 2);

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* last_node = (struct ast_node_with_type*)first->ast_node.node.prev;
   struct ast_node_with_type* curr = (struct ast_node_with_type*)first->ast_node.node.next;

   int stack_top = inter->stack.stack_top;

   while (curr != last_node) {
      struct ast_node_with_type* name_node;
      struct ast_node_with_type* value_node;

      name_node = (struct ast_node_with_type*)curr->ast_node.children.entry_node;
      value_node = (struct ast_node_with_type*)name_node->ast_node.node.next;
      
      if (value_node == name_node) {
         value_node = NULL;
      }

      if (name_node->ast_node.state == TOKEN_STATE_PUNCTUATION) {
         name_node = (struct ast_node_with_type*)name_node->ast_node.children.entry_node;
      }

      assert(name_node->symbol);
      assert(name_node->type);

      int value_size = (*name_node->type)->type_size(name_node->type);
      struct typed_value* alloc = (struct typed_value*)stack_alloc(&inter->stack, sizeof(struct typed_value) + value_size);
      assert(alloc);
      alloc->type = name_node->type;

      if (value_node) {
         err = inter_eval_rval(inter, value_node, alloc->val, value_size);
         if (err) goto pop_args;
      }

      struct list* cell = (struct list*)alloca(sizeof(struct list));
      cell->value = (void*)alloc;
      cell->rest = name_node->symbol->value;
      name_node->symbol->value = (void*)cell;

      curr = (struct ast_node_with_type*)curr->ast_node.node.next;
   }

   err = inter_eval(inter, last_node, value, size);

pop_args:

   curr = (struct ast_node_with_type*)curr->ast_node.node.prev;

   while (curr != first) {
      struct ast_node_with_type* name_node = (struct ast_node_with_type*)curr->ast_node.children.entry_node;

      if (name_node->ast_node.state == TOKEN_STATE_PUNCTUATION) {
         name_node = (struct ast_node_with_type*)name_node->ast_node.children.entry_node;
      }

      assert(name_node->ast_node.state == TOKEN_STATE_SYMBOL);
      assert(name_node->symbol);

      struct list* cell = (struct list*)name_node->symbol->value;
      assert(cell);
      assert(cell->value);

      struct typed_value* alloc = (struct typed_value*)cell->value;
      inter_destruct_val(inter, name_node->type, alloc->val);

      name_node->symbol->value = (void*)cell->rest;

      curr = (struct ast_node_with_type*)curr->ast_node.node.prev;
   }

   inter->stack.stack_top = stack_top;

   return err;
}


error inter_func_macro_check(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   error err = NULL;
   int num_children = dlist_root_length(&root->ast_node.children);

   root->returns_lval = 0;
   root->eval_table.eval_rval = inter_func_macro_eval;
   root->eval_table.eval_lval = inter_eval_fallback_lval;

   if (num_children != 3) {
      return error_wrong_number_of_args(&root->ast_node, 2, num_children - 1);
   }

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* args_node = (struct ast_node_with_type*)first->ast_node.node.next;
   struct ast_node_with_type* value_node = (struct ast_node_with_type*)args_node->ast_node.node.next;

   root->type = type_construct_macro_(inter, &args_node->ast_node, &err);
   if (err) return err;

   struct type_macro* macro_type = (struct type_macro*)root->type;

   err = check_root_type(root, expected_type);
   if (err) return err;

   int stack_top = inter->stack.stack_top;

   char* arg0_str = inter_intern_sym(inter, MACRO_ARG0, strlen(MACRO_ARG0));
   struct key_value* arg0_symbol = inter_make_value_binding(inter, arg0_str, strlen(arg0_str));
   struct typed_value* arg_alloc = (struct typed_value*)stack_alloc(&inter->stack, sizeof(struct typed_value) + sizeof(void*));
   assert(arg_alloc);
   arg_alloc->type = &type_quote;
   *((struct ast_node_with_type**)arg_alloc->val) = NULL;

   hash_map_sym_push_value(arg0_symbol, arg_alloc);

   int i;
   for (i = 0; i < macro_type->num_arguments; i++) {
      arg_alloc = (struct typed_value*)stack_alloc(&inter->stack, sizeof(struct typed_value) + sizeof(void*));
      assert(arg_alloc);
      arg_alloc->type = &type_quote;
      *((struct ast_node_with_type**)arg_alloc->val) = NULL;

      hash_map_sym_push_value(macro_type->arg_symbols[i], arg_alloc); 
   }

   err = inter_type_check_expr(inter, &type_quote, value_node);

reset_stack:

   for (i = macro_type->num_arguments - 1; i >= 0; i--) {
      hash_map_sym_pop_value(macro_type->arg_symbols[i]);
   }

   hash_map_sym_pop_value(arg0_symbol);

   inter->stack.stack_top = stack_top;

   return err;
}


error inter_func_macro_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   assert(dlist_root_length(&root->ast_node.children) == 3);

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* args_node = (struct ast_node_with_type*)first->ast_node.node.next;
   struct ast_node_with_type* value_node = (struct ast_node_with_type*)args_node->ast_node.node.next;

   *((struct ast_node_with_type**)value) = (struct ast_node_with_type*)refc_copy(value_node);
   return NULL;
}





void error_splice_outside_list_print(FILE* output, struct error_at_node* self) {
   fprintf(output, "%s:%d.%d: Splice outside of a list.\n",
         self->location->filename, self->location->line, self->location->column);
}

struct error_vtable error_splice_outside_list_vtable = {
   .error_print = (error_print_fn)error_splice_outside_list_print,
   .error_free = (error_free_fn)free
};

error error_splice_outside_list(struct ast_node* location) {
   struct error_at_node* alloc = malloc(sizeof(struct error_at_node));
   assert(alloc);
   alloc->vtable = &error_splice_outside_list_vtable;
   alloc->location = location;
   return (error)alloc;
}


void error_cannot_splice_atom_print(FILE* output, struct error_cannot_splice_atom* self) {
   fprintf(output, "%s:%d.%d: Cannot splice atom: ",
         self->location->filename, self->location->line, self->location->column);
   ast_node_print(output, &self->quote->ast_node);
   fprintf(output, "\n");
}

void error_cannot_splice_atom_free(struct error_cannot_splice_atom* self) {
   if (self->quote)
      ast_node_with_type_rec_free(self->quote, self->inter);

   free(self);
}

struct error_vtable error_cannot_splice_atom_vtable = {
   .error_print = (error_print_fn)error_cannot_splice_atom_print,
   .error_free = (error_free_fn)error_cannot_splice_atom_free
};

error error_cannot_splice_atom(struct ast_node* location, struct ast_node_with_type* quote, struct inter* inter) {
   struct error_cannot_splice_atom* self = (struct error_cannot_splice_atom*)malloc(sizeof(struct error_cannot_splice_atom));
   assert(self);
   self->vtable = &error_cannot_splice_atom_vtable;
   self->location = location;
   self->quote = quote;
   self->inter = inter;
   return (error)self;
}


// These symbols would ideally by saved to global variables,
// but I don't want to rule out there being multiple symbol tables yet.
void quasiquote_syms_init(struct quasiquote_syms* syms, struct inter* inter) {
   syms->unquote = (char*)refc_copy(inter_intern_sym(inter, tok_unquote, strlen(tok_unquote)));
   syms->unquote_abrv = (char*)refc_copy(inter_intern_sym(inter, tok_unquote_abrv, strlen(tok_unquote_abrv)));
   syms->splice = (char*)refc_copy(inter_intern_sym(inter, tok_splice, strlen(tok_splice)));
   syms->splice_abrv = (char*)refc_copy(inter_intern_sym(inter, tok_splice_abrv, strlen(tok_splice_abrv)));
}

void quasiquote_syms_destruct(struct quasiquote_syms* syms, struct inter* inter) {
   inter_release_sym(inter, syms->unquote);
   inter_release_sym(inter, syms->unquote_abrv);
   inter_release_sym(inter, syms->splice);
   inter_release_sym(inter, syms->splice_abrv);
}

error inter_func_quasiquote_check_rec(
      struct inter* inter,
      struct quasiquote_syms* syms,
      struct ast_node_with_type* root,
      int level) {
   error err;

   if (root->ast_node.state != TOKEN_STATE_PUNCTUATION)
      return NULL;

   int num_children = dlist_root_length(&root->ast_node.children);

   if (num_children == 0)
      return NULL;

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;

   if (first->ast_node.text == syms->unquote ||
       first->ast_node.text == syms->unquote_abrv) {
      if (num_children != 2) {
         return error_wrong_number_of_args(&root->ast_node, 1, num_children - 1);
      }

      struct ast_node_with_type* arg_node = (struct ast_node_with_type*)first->ast_node.node.next;
      return inter_type_check_expr(inter, &type_quote, arg_node);
   }

   if (first->ast_node.text == syms->splice ||
       first->ast_node.text == syms->splice_abrv) {
      if (num_children != 2) {
         return error_wrong_number_of_args(&root->ast_node, 1, num_children - 1);
      }

      if (level == 0) {
         return error_splice_outside_list(&root->ast_node);
      }

      struct ast_node_with_type* arg_node = (struct ast_node_with_type*)first->ast_node.node.next;
      return inter_type_check_expr(inter, &type_quote, arg_node);
   }

   struct ast_node_with_type* curr = first;
   do {
      err = inter_func_quasiquote_check_rec(inter, syms, curr, level + 1);
      if (err) return err;

      curr = (struct ast_node_with_type*)curr->ast_node.node.next;
   } while (curr != first);

   return NULL;
}

error inter_func_quasiquote_check(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   error err = NULL;

   struct quasiquote_syms syms;
   quasiquote_syms_init(&syms, inter);

   int num_children = dlist_root_length(&root->ast_node.children);

   root->returns_lval = 0;
   root->eval_table.eval_rval = inter_func_quasiquote_eval;
   root->eval_table.eval_lval = inter_eval_fallback_lval;

   if (num_children != 2) {
      err = error_wrong_number_of_args(&root->ast_node, 1, num_children - 1);
      goto done;
   }

   struct ast_node_with_type* arg_node = (struct ast_node_with_type*)dlist_root_nth(&root->ast_node.children, 1);

   err = inter_func_quasiquote_check_rec(inter, &syms, arg_node, 0);

done:
   quasiquote_syms_destruct(&syms, inter);
   return err;
}

error inter_func_quasiquote_eval_rec(
      struct inter* inter,
      struct quasiquote_syms* syms,
      struct ast_node_with_type* root,
      struct ast_node_with_type* adopter) {
   error err;

   // atom node
   if (root->ast_node.state != TOKEN_STATE_PUNCTUATION) {
      struct ast_node_with_type* copy = ast_node_with_type_copy(root);
      dlist_root_append(&adopter->ast_node.children, &copy->ast_node.node);
      return NULL;
   }

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;

   // empty list
   if (first == NULL) {
      struct ast_node_with_type* copy = ast_node_with_type_copy(root);
      dlist_root_append(&adopter->ast_node.children, &copy->ast_node.node);
      return NULL;
   }

   // eval unquote
   if (first->ast_node.text == syms->unquote ||
       first->ast_node.text == syms->unquote_abrv) {
      struct ast_node_with_type* arg_node = (struct ast_node_with_type*)first->ast_node.node.next;
      struct ast_node_with_type* quote;

      err = inter_eval_rval(inter, arg_node, &quote, sizeof(quote));
      if (err) return err;

      assert(quote);

      if (quote->ast_node.node.root) {
         // This is a reference to a node already in a tree.
         // Make a copy that can be put somewhere else.
         struct ast_node_with_type* quote_copy = ast_node_with_type_copy_tree(quote);
         ast_node_with_type_rec_free(quote, inter);
         quote = quote_copy;
      }

      dlist_root_append(&adopter->ast_node.children, &quote->ast_node.node);
      return NULL;
   }

   // eval splice
   if (first->ast_node.text == syms->splice ||
       first->ast_node.text == syms->splice_abrv) {
      struct ast_node_with_type* arg_node = (struct ast_node_with_type*)first->ast_node.node.next;
      struct ast_node_with_type* quote;

      err = inter_eval_rval(inter, arg_node, &quote, sizeof(quote));
      if (err) return err;

      assert(quote);

      if (quote->ast_node.state != TOKEN_STATE_PUNCTUATION) {
         return error_cannot_splice_atom(&arg_node->ast_node, quote, inter);
      }

      struct ast_node_with_type* child_first = (struct ast_node_with_type*)quote->ast_node.children.entry_node;
      if (child_first) {
         struct ast_node_with_type* child_curr = child_first;
         do {
            struct ast_node_with_type* curr_copy = ast_node_with_type_copy_tree(child_curr);
            dlist_root_append(&adopter->ast_node.children, &curr_copy->ast_node.node);

            child_curr = (struct ast_node_with_type*)child_curr->ast_node.node.next;
         } while (child_curr != child_first);
      }

      ast_node_with_type_rec_free(quote, inter);
      return NULL;
   }

   // recursively decend on list.
   struct ast_node_with_type* root_copy = ast_node_with_type_copy(root);
   dlist_root_append(&adopter->ast_node.children, &root_copy->ast_node.node);

   struct ast_node_with_type* curr = first;
   do {
      err = inter_func_quasiquote_eval_rec(inter, syms, curr, root_copy);
      if (err) return err;

      curr = (struct ast_node_with_type*)curr->ast_node.node.next;
   } while (curr != first);

   return NULL;
}


error inter_func_quasiquote_eval(struct inter* inter, struct ast_node_with_type* root, void* value, int size) {
   error err = NULL;
   assert(dlist_root_length(&root->ast_node.children) == 2);

   struct quasiquote_syms syms;
   quasiquote_syms_init(&syms, inter);

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   struct ast_node_with_type* arg_node = (struct ast_node_with_type*)first->ast_node.node.next;

   // atom node
   if (arg_node->ast_node.state != TOKEN_STATE_PUNCTUATION) {
      *((struct ast_node_with_type**)value) = ast_node_with_type_copy(arg_node);
      goto done;
   }

   struct ast_node_with_type* arg_first = (struct ast_node_with_type*)arg_node->ast_node.children.entry_node;

   // empty list
   if (arg_first == NULL) {
      *((struct ast_node_with_type**)value) = ast_node_with_type_copy(arg_node);
      goto done;
   }

   // eval unquote
   if (arg_first->ast_node.text == syms.unquote ||
       arg_first->ast_node.text == syms.unquote_abrv) {
      struct ast_node_with_type* unquote_arg = (struct ast_node_with_type*)arg_first->ast_node.node.next;

      struct ast_node_with_type* quote;
      err = inter_eval_rval(inter, unquote_arg, &quote, sizeof(quote));
      if (err) goto done;

      if (quote->ast_node.node.root) {
         struct ast_node_with_type* quote_copy = ast_node_with_type_copy_tree(quote);
         ast_node_with_type_rec_free(quote, inter);
         quote = quote_copy;
      }

      *((struct ast_node_with_type**)value) = quote;
      goto done;
   }

   // eval splice, error. splice outside of a list.
   // This should be caught in the type check.
   assert(arg_first->ast_node.text != syms.splice &&
          arg_first->ast_node.text != syms.splice_abrv);

   // recursively decend on list.
   struct ast_node_with_type* arg_copy = ast_node_with_type_copy(arg_node);
   struct ast_node_with_type* arg_curr = arg_first;

   do {
      err = inter_func_quasiquote_eval_rec(inter, &syms, arg_curr, arg_copy);
      if (err) goto done_arg_copy;

      arg_curr = (struct ast_node_with_type*)arg_curr->ast_node.node.next;
   } while (arg_curr != arg_first);

   *((struct ast_node_with_type**)value) = arg_copy;

done_arg_copy:
   if (err)
      ast_node_with_type_rec_free(arg_copy, inter);

done:
   quasiquote_syms_destruct(&syms, inter);
   return err;
}








#define DO_MACROS(M) \
   M("print", inter_func_check_args, &type_void, inter_func_print_eval) \
   M("char", inter_func_char_check, &type_char, inter_func_char_eval) \
   M("uchar", inter_func_uchar_check, &type_uchar, inter_func_uchar_eval) \
   M("str", inter_func_string_check, NULL, inter_func_string_eval) \
   M("if", inter_func_if_check, NULL, inter_func_if_eval) \
   M("defined", inter_func_defined_check, &type_int, inter_func_defined_eval) \
   M("def", inter_func_def_check, &type_void, inter_func_def_eval) \
   M("undef", inter_func_undef_check, &type_void, inter_func_undef_eval) \
   M("set", inter_func_set_check, &type_void, inter_func_set_eval) \
   M("+", inter_func_sum_check, NULL, inter_func_sum_eval) \
   M("-", inter_func_sub_check, NULL, inter_func_sub_eval) \
   M("*", inter_func_mul_check, NULL, inter_func_mul_eval) \
   M("/", inter_func_div_check, NULL, inter_func_div_eval) \
   M("%", inter_func_mod_check, NULL, inter_func_mod_eval) \
   M("bit-and", inter_func_bit_and_check, NULL, inter_func_bit_and_eval) \
   M("bit-or", inter_func_bit_or_check, NULL, inter_func_bit_or_eval) \
   M("bit-not", inter_func_bit_not_check, NULL, inter_func_bit_not_eval) \
   M("<<", inter_func_shift_left_check, NULL, inter_func_shift_left_eval) \
   M(">>", inter_func_shift_right_check, NULL, inter_func_shift_right_eval) \
   M(">=", inter_func_gt_check, &type_int, inter_func_gt_eval) \
   M(">", inter_func_g_check, &type_int, inter_func_g_eval) \
   M("<=", inter_func_lt_check, &type_int, inter_func_lt_eval) \
   M("<", inter_func_l_check, &type_int, inter_func_l_eval) \
   M("=", inter_func_eq_check, &type_int, inter_func_eq_eval) \
   M("!=", inter_func_eq_check, &type_int, inter_func_neq_eval) \
   M("and", inter_func_and_check, &type_int, inter_func_and_eval) \
   M("or", inter_func_or_check, &type_int, inter_func_or_eval) \
   M("not", inter_func_not_check, &type_int, inter_func_not_eval) \
   M("&", inter_func_addr_of_check, NULL, inter_func_addr_of_eval) \
   M("array", inter_func_array_check, NULL, inter_func_array_eval) \
   M("array.length", inter_func_array_length_check, &type_int, inter_func_array_length_eval) \
   M("sizeof-type", inter_func_sizeof_type_check, &type_int, inter_func_sizeof_eval) \
   M("sizeof-value", inter_func_sizeof_value_check, &type_int, inter_func_sizeof_eval) \
   M("type", inter_func_type_check, &type_type, inter_func_type_eval) \
   M("typeof", inter_func_typeof_check, &type_type, inter_func_type_eval) \
   M("type.size", inter_func_type_size_check, &type_int, inter_func_type_size_eval) \
   M("cast", inter_func_cast_check, NULL, inter_func_cast_eval) \
   M(":", inter_func_context_check, NULL, inter_func_context_eval) \
   M("to-char", inter_func_to_char_check, &type_char, inter_func_to_char_eval) \
   M("to-uchar", inter_func_to_uchar_check, &type_uchar, inter_func_to_uchar_eval) \
   M("to-short", inter_func_to_char_check, &type_short, inter_func_to_short_eval) \
   M("to-ushort", inter_func_to_char_check, &type_ushort, inter_func_to_ushort_eval) \
   M("to-int", inter_func_to_int_check, &type_int, inter_func_to_int_eval) \
   M("to-uint", inter_func_to_int_check, &type_uint, inter_func_to_uint_eval) \
   M("to-long", inter_func_to_int_check, &type_long, inter_func_to_long_eval) \
   M("to-ulong", inter_func_to_int_check, &type_ulong, inter_func_to_ulong_eval) \
   M("to-longlong", inter_func_to_char_check, &type_long_long, inter_func_to_long_long_eval) \
   M("to-ulonglong", inter_func_to_char_check, &type_ulong_long, inter_func_to_ulong_long_eval) \
   M("to-float", inter_func_to_float_check, &type_float, inter_func_to_float_eval) \
   M("to-double", inter_func_to_float_check, &type_double, inter_func_to_double_eval) \
   M("to-longdouble", inter_func_to_float_check, &type_long_double, inter_func_to_long_double_eval) \
   M("to-*", inter_func_to_pointer_check, &type_pointer, inter_func_to_pointer_eval) \
   M("code", inter_func_code_check, NULL, inter_func_code_eval) \
   M("code-unsafe", inter_func_code_unsafe_check, NULL, inter_func_code_unsafe_eval) \
   M("quote", inter_func_quote_check, &type_quote, inter_func_quote_eval) \
   M("'", inter_func_quote_check, &type_quote, inter_func_quote_eval) \
   M("free-quote", inter_func_free_quote_check, &type_void, inter_func_free_quote_eval) \
   M("begin", inter_func_begin_check, NULL, inter_func_begin_eval) \
   M("begin-", inter_func_begin__check, NULL, inter_func_begin__eval) \
   M("load-file", inter_func_load_file_check, &type_void, inter_func_load_file_eval) \
   M("load", inter_func_load_check, &type_void, inter_func_load_eval) \
   M("import", inter_func_import_check, &type_void, inter_func_import_abs_eval) \
   M("import.", inter_func_import_check, &type_void, inter_func_import_rel_eval) \
   M("struct", inter_func_struct_check, NULL, inter_func_struct_eval) \
   M("set-slot", inter_func_set_slot_check, &type_void, inter_func_set_slot_eval) \
   M(".", inter_func_dot_check, NULL, inter_func_dot_eval_lval) \
   M("def-type", inter_func_def_type_check, &type_void, inter_func_def_type_eval) \
   M("set-type", inter_func_set_type_check, &type_void, inter_func_set_type_eval) \
   M("undef-type", inter_func_undef_type_check, &type_void, inter_func_undef_type_eval) \
   M("def-type-cons", inter_func_def_type_cons_check, &type_void, inter_func_def_type_cons_eval) \
   M("undef-type-cons", inter_func_undef_type_cons_check, &type_void, inter_func_undef_type_cons_eval) \
   M("def-enum", inter_func_def_enum_check, &type_void, inter_func_def_enum_eval) \
   M("prefix.make", inter_func_prefix_make_check, &type_prefix, inter_func_prefix_make_eval) \
   M("current-prefix", inter_func_current_prefix_check, type_pointer_to(&type_pointer_to_vtable, &type_prefix), inter_func_current_prefix_eval) \
   M("in-prefix", inter_func_in_prefix_check, &type_void, inter_func_in_prefix_eval) \
   M("use-prefix", inter_func_use_prefix_check, &type_void, inter_func_use_prefix_eval) \
   M("clear-prefixes", inter_func_clear_prefixes_check, &type_void, inter_func_clear_prefixes_eval) \
   M("function", inter_func_script_function_check, NULL, inter_func_script_function_eval) \
   M("free", inter_func_free_check, &type_void, inter_func_free_eval) \
   M("break", inter_func_break_check, &type_void, inter_func_break_eval) \
   M("continue", inter_func_continue_check, &type_void, inter_func_continue_eval) \
   M("forever", inter_func_forever_check, &type_void, inter_func_forever_eval) \
   M("while", inter_func_while_check, &type_void, inter_func_while_eval) \
   M("do-while", inter_func_do_while_check, &type_void, inter_func_do_while_eval) \
   M("for", inter_func_for_check, &type_void, inter_func_for_eval) \
   M("let", inter_func_let_check, NULL, inter_func_let_eval) \
   M("macro", inter_func_macro_check, NULL, inter_func_macro_eval) \
   M("quasiquote", inter_func_quasiquote_check, &type_quote, inter_func_quasiquote_eval) \
   M("`", inter_func_quasiquote_check, &type_quote, inter_func_quasiquote_eval) \


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
   value = inter_undef_value(inter, name_sym, strlen(name)); \
   typed_value_destruct(value, inter); \



void inter_def_std_funcs(struct inter* inter) {
   char* name_sym;
   type macro_type;
   struct typed_value* value;
   type* arguments;

   DO_MACROS(DEF_MACRO)

   // stdint conversions
   DEF_MACRO("to-int8", inter_func_to_char_check, &type_char, inter_func_to_char_eval)
   DEF_MACRO("to-uint8", inter_func_to_uchar_check, &type_uchar, inter_func_to_uchar_eval)

   if (sizeof(short) == 2) {
      DEF_MACRO("to-int16", inter_func_to_char_check, &type_short, inter_func_to_short_eval)
      DEF_MACRO("to-uint16", inter_func_to_char_check, &type_ushort, inter_func_to_ushort_eval)
   }
   else if (sizeof(int) == 2) {
      DEF_MACRO("to-int16", inter_func_to_int_check, &type_int, inter_func_to_int_eval)
      DEF_MACRO("to-uint16", inter_func_to_int_check, &type_uint, inter_func_to_uint_eval)
   }
   else {
      assert(0);
   }

   if (sizeof(int) == 4) {
      DEF_MACRO("to-int32", inter_func_to_int_check, &type_int, inter_func_to_int_eval)
      DEF_MACRO("to-uint32", inter_func_to_int_check, &type_uint, inter_func_to_uint_eval)
   }
   else if (sizeof(long) == 4) {
      DEF_MACRO("to-int32", inter_func_to_int_check, &type_long, inter_func_to_long_eval)
      DEF_MACRO("to-uint32", inter_func_to_int_check, &type_ulong, inter_func_to_ulong_eval)
   }
   else {
      assert(0);
   }

   if (sizeof(int) == 8) {
      DEF_MACRO("to-int64", inter_func_to_int_check, &type_int, inter_func_to_int_eval)
      DEF_MACRO("to-uint64", inter_func_to_int_check, &type_uint, inter_func_to_uint_eval)
   }
   else if (sizeof(long) == 8) {
      DEF_MACRO("to-int64", inter_func_to_int_check, &type_long, inter_func_to_long_eval)
      DEF_MACRO("to-uint64", inter_func_to_int_check, &type_ulong, inter_func_to_ulong_eval)
   }
   else if (sizeof(long long) == 8) {
      DEF_MACRO("to-int64", inter_func_to_int_check, &type_long_long, inter_func_to_long_long_eval)
      DEF_MACRO("to-uint64", inter_func_to_int_check, &type_ulong_long, inter_func_to_ulong_long_eval)
   }
   else {
      assert(0);
   }
}

void inter_undef_std_funcs(struct inter* inter) {
   char* name_sym;
   struct typed_value* value;

   DO_MACROS(UNDEF_MACRO)

   UNDEF_MACRO("to-int8", NULL, NULL, NULL)
   UNDEF_MACRO("to-uint8", NULL, NULL, NULL)
   UNDEF_MACRO("to-int16", NULL, NULL, NULL)
   UNDEF_MACRO("to-uint16", NULL, NULL, NULL)
   UNDEF_MACRO("to-int32", NULL, NULL, NULL)
   UNDEF_MACRO("to-uint32", NULL, NULL, NULL)
   UNDEF_MACRO("to-int64", NULL, NULL, NULL)
   UNDEF_MACRO("to-uint64", NULL, NULL, NULL)
}




