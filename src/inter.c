#include <string.h>
#include <malloc.h>
#include <assert.h>

#include "hash_table.h"
#include "hash_map_push.h"
#include "inter.h"
#include "list.h"
#include "refc.h"

#define LOGGING_OFF
#include "log.h"

// Tune these to optimize.
#define INTER_INIT_TYPE_CONS_SIZE 31
#define INTER_INIT_TYPE_DEF_SIZE 7
#define INTER_INIT_VALUE_SIZE 127
#define INTER_INIT_IMPORT_SIZE 63


void type_print(FILE* output, type type) {
   if (type) {
      (*type)->type_print(output, type);
   }
   else {
      fprintf(output, "(NULL)");
   }
}


void type_free_(struct inter* inter, type type) {
   (*type)->type_free(type, inter);
}


#ifndef INTER_DESTRUCT_VAL_MACRO

void inter_destruct_val(struct inter* inter, type val_type, void* val) {
   if ((*val_type)->type_destruct_val) {
      (*val_type)->type_destruct_val(val_type, val, inter);
   }
}

#endif


// Same as inter_destruct_val, but handles the case when the type is a type_def_cell
void inter_destruct_val2(struct inter* inter, type val_type, void* val) {
   type type_binding;
   error err = NULL;
   TYPE_DEF_CELL_GET_TYPE(val_type, NULL, type_binding, err)
   assert(!err);

   inter_destruct_val(inter, type_binding, val);
}




void* malloc_fn(void* self, int size) {
   return malloc(size);
}

alloc_fn malloc_fn_ = &malloc_fn;

void* typed_value_get_val(struct typed_value* self) {
   return (void*)self->val;
}

void typed_value_destruct(struct typed_value* self, struct inter* inter) {
   assert(self);

   inter_destruct_val2(inter, self->type, self->val);

   (*(self->type))->type_free(self->type, inter);
}

void typed_value_destruct_(struct inter* inter, struct typed_value* self) {
   typed_value_destruct(self, inter);
}

void typed_value_print(FILE* output, struct typed_value* self) {
   (*(self->type))->type_print_val(output, self->type, (void*)self->val);
   fprintf(output, " : ");
   (*(self->type))->type_print(output, self->type);
}

int typed_value_size(type type) {
   return sizeof(struct typed_value) + (*type)->type_size(type);
}

struct typed_value* typed_value(type type, alloc_fn* alloc) {
   struct typed_value* self = (struct typed_value*)(*alloc)(alloc, typed_value_size(type));
   assert(self);
   self->type = type;
   return self;
}


int symbol_hash(void* obj, int seed, struct symbol* sym) {
   int hash = seed;
   hash = string_hash(NULL, hash, sym->prefix->prefix);
   hash = string_hash(NULL, hash, sym->ending);
   return hash;
}

int sym_string_eq(void* obj, char* string, struct symbol* sym) {
   return !strncmp(string, sym->prefix->prefix, sym->prefix->prefix_len) &&
          !strcmp(string + sym->prefix->prefix_len, sym->ending);;
}

struct hash_vtable hash_vtable_sym_string = {
   .hash = (hash_fn)symbol_hash,
   .eq = (hash_eq_fn)sym_string_eq
};
struct hash_vtable* hash_sym_string_obj = &hash_vtable_sym_string;


struct key_value* sym_key_value(char* sym_key, void* value) {
   return key_value(refc_copy(sym_key), value);
}

void sym_key_value_free(struct key_value* self, struct inter* inter, list_for_each_fn callback) {
   inter_release_sym(inter, self->key);
   struct list* list = (struct list*)self->value;

   while (list) {
      void* value = list_pop(&list);
      if (callback)
         callback(inter, value);
   }

   free(self);
}

struct key_value* inter_mapping_intern(struct hash_table* map, char* sym_key) {
   struct key_value** slot = hash_map_slot(map, sym_key, NULL);

   if (!*slot) {
      *slot = sym_key_value(sym_key, NULL);
   }

   return *slot;
}

void inter_mapping_push_value(struct hash_table* map, char* sym_key, void* value) {
   struct key_value* binding = inter_mapping_intern(map, sym_key); 
   hash_map_sym_push_value(binding, value);
}


void inter_mapping_destruct_keys_only(struct hash_table* map, struct inter* inter) {
   int i;
   for (i = 0; i < map->array_length; i++) {
      struct key_value* curr = (struct key_value*)map->array[i];
      if (curr) {
         inter_release_sym(inter, curr->key);
         free(curr);
      }
   }

   hash_table_destruct(map);
}

void inter_mapping_destruct(struct hash_table* map, struct inter* inter, list_for_each_fn callback) {
   int i;
   for (i = 0; i < map->array_length; i++) {
      struct key_value* curr = (struct key_value*)map->array[i];
      if (curr) {
         sym_key_value_free(curr, inter, callback);
      }
   }

   hash_table_destruct(map);
}



void type_free_nothing(type self, struct inter* inter) {
}

int type_match_static(type self, type other) {
   return self == other;
}

type type_copy_static(type self) {
   return self;
}




void error_type_unbound_print(FILE* output, struct error_type_unbound* self) {
   fprintf(output, "%s:%d.%d: Type %s unbound.\n",
         self->location->filename, self->location->line, self->location->column,
         self->cell->key);
}

struct error_vtable error_type_unbound_vtable = {
   .error_print = (error_print_fn)error_type_unbound_print,
   .error_free = (error_free_fn)free
};

error error_type_unbound(struct ast_node* location, struct type_def_cell* cell) {
   struct error_type_unbound* alloc = (struct error_type_unbound*)malloc(sizeof(struct error_type_unbound));
   assert(alloc);
   alloc->vtable = &error_type_unbound_vtable;
   alloc->location = location;
   alloc->cell = cell;
   return (error)alloc;
}


void error_type_already_bound_print(FILE* output, struct error_type_unbound* self) {
   fprintf(output, "%s:%d.%d: Type %s already bound.\n",
         self->location->filename, self->location->line, self->location->column,
         self->cell->key);
}

struct error_vtable error_type_already_bound_vtable = {
   .error_print = (error_print_fn)error_type_already_bound_print,
   .error_free = (error_free_fn)free
};

error error_type_already_bound(struct ast_node* location, struct type_def_cell* cell) {
   struct error_type_unbound* alloc = (struct error_type_unbound*)malloc(sizeof(struct error_type_unbound));
   assert(alloc);
   alloc->vtable = &error_type_already_bound_vtable;
   alloc->location = location;
   alloc->cell = cell;
   return (error)alloc;
}





void type_def_cell_print(FILE* output, struct type_def_cell* self) {
   fprintf(output, "%s", self->key);
}

int type_def_cell_match(struct type_def_cell* self, type other) {
   assert(0);
   return 0;

/*
   if ((void*)self == (void*)other)
      return 1;

   type binding = (type)hash_map_sym_top_value(&self->key_value);
   if (!binding)
      return 0;

   if (*other == &type_def_cell_vtable) {
      struct type_def_cell* other_ = (struct type_def_cell*)other;
      type other_binding = (type)hash_map_sym_top_value(&other_->key_value);
      if (!other_binding)
         return 0;

      return (*binding)->type_match(binding, other_binding);
   }

   return (*binding)->type_match(binding, other);
   */
}

void type_def_cell_copy_val(struct type_def_cell* self, void* src, void* dest) {
   assert(0);
}

void type_def_cell_destruct_val(struct type_def_cell* self, void* val, struct inter* inter) {
   assert(0);
}

int type_def_cell_size(struct type_def_cell* self) {
   /*
   type binding = (type)hash_map_sym_top_value(&self->key_value);
   if (!binding)
      return 0;

   return (*binding)->type_size(binding);
   */
   assert(0);
   return 0;
}


#ifndef TYPE_DEF_CELL_GET_TYPE_MACRO
#if 0
error type_def_cell_get_type(type self, struct ast_node* location, type* ret) {
   struct type_def_cell* parent = NULL;
   error err = NULL;

   for (;;) {
      if (!self) {
         if (parent) {
            *ret = NULL;
            err = error_type_unbound(location, parent);
         }
         else {
            *ret = self;
         }
         break;
      }

      if (*self != &type_def_cell_vtable) {
         *ret = self;
         break;
      }

      parent = (struct type_def_cell*)self;
      self = parent->value;
   }

   return err;
}

#else

error type_def_cell_get_type(type self, struct ast_node* location, type* ret) {
   if (!self || *self != &type_def_cell_vtable) {
      *ret = self;
      return NULL;
   }

   struct type_def_cell* cell = (struct type_def_cell*)self;
   if (!cell->value) {
      return error_type_unbound(location, cell);
   }

   assert(*cell->value != &type_def_cell_vtable);

   *ret = cell->value;
   return NULL;
}

#endif

#endif

error type_def_cell_bind_type(struct inter* inter, struct ast_node_with_type* node) {
   error err = NULL;
   type bind_type;

   TYPE_DEF_CELL_GET_TYPE(node->type, &node->ast_node, bind_type, err)
   if (err) return err;

   if (node->type && node->type != bind_type) {
      (*node->type)->type_free(node->type, inter);

      if (bind_type)
         node->type = (*bind_type)->type_copy(bind_type);
      else
         node->type = NULL;
   }

   return err;
}

void type_def_cell_unbind(struct type_def_cell* self, struct inter* inter) {
   if (self->value) {
      (*self->value)->type_free(self->value, inter);
      self->value = NULL;
   }
}

void type_def_cell_free(struct type_def_cell* self, struct inter* inter) {
   if (refc_dec(self))
      return;

   inter_release_sym(inter, self->key);

   if (self->value) {
      (*self->value)->type_free(self->value, inter);
   }
   
   refc_free(self);
}

void type_def_cell_unbind_free(struct inter* inter, struct type_def_cell* self) {
   type_def_cell_unbind(self, inter);

   if (refc_dec(self))
      return;

   inter_release_sym(inter, self->key);

   refc_free(self);
}

void type_def_cell_print_val(FILE* output, struct type_def_cell* self, void* value) {
   /*
   type binding = (type)hash_map_sym_top_value(&self->key_value);
   if (!binding) {
      fprintf(output, "<%s UNBOUND>", self->key_value.key);
   }

   (*binding)->type_print_val(output, binding, value);
   */
   assert(0);
}

error type_def_cell_parse_constant(struct type_def_cell* self, struct ast_node_with_type* node, void* value) {
   /*
   type binding = (type)hash_map_sym_top_value(&self->key_value);
   if (!binding) {
      return error_type_unbound(&node->ast_node, self);
   }

   return (*binding)->type_parse_constant(binding, node, value);
   */
   assert(0);
   return NULL;
}

/*
error type_def_cell_check_call(struct inter* inter, type expected_type, struct ast_node_with_type* root) {
   struct type_def_cell* self = (struct type_def_cell*)root->type;
   assert(self);
   assert(self->vtable == &type_def_cell_vtable);

   type binding = (type)hash_map_sym_top_value(&self->key_value);
   if (!binding)
      return error_type_unbound(&root->ast_node, self);

   if (!(*binding)->type_check_call)
      return error_cannot_call(&root->ast_node, binding);

   return (*binding)->type_check_call(inter, expected_type, root);
}
*/

struct type_vtable type_def_cell_vtable = {
   .type_free = (type_free_fn)type_def_cell_free,
   .type_copy = (type_copy_fn)refc_copy,
   .type_print = (type_print_fn)type_def_cell_print,
   .type_match = (type_match_fn)type_def_cell_match,
   .type_size = (type_size_fn)type_def_cell_size,
   .type_print_val = (type_print_val_fn)type_def_cell_print_val,
   .type_copy_val = (type_copy_val_fn)type_def_cell_copy_val,
   .type_destruct_val = (type_destruct_val_fn)type_def_cell_destruct_val,
   .type_parse_constant = (type_parse_constant_fn)type_def_cell_parse_constant,
   .type_check_call = NULL,
   .type_eval_call = NULL
};

struct type_def_cell* type_def_cell(char* key, type value) {
   struct type_def_cell* self = (struct type_def_cell*)refc_alloc(sizeof(struct type_def_cell));
   assert(self);
   self->vtable = &type_def_cell_vtable;
   self->key = (char*)refc_copy(key);
   self->value = value;
   return self;
}







char* prefix_find_sym(struct prefix* self, char* text, int text_len) {
   struct symbol sym;
   sym.prefix = self;
   sym.ending = text;

   return (char*)hash_set_find(&self->inter->sym_table, &sym, &hash_sym_string_obj);
}

char* prefix_make_sym(struct prefix* self, char* text, int text_len) {
   struct symbol sym;
   sym.prefix = self;
   sym.ending = text;

   char** slot = (char**)hash_set_slot(&self->inter->sym_table, &sym, &hash_sym_string_obj);
   if (!*slot) {
      int string_len = self->prefix_len + text_len;
      char* alloc = refc_alloc(string_len + 1);
      refc_dec(alloc); // Must start zero.
      assert(alloc);
      memcpy(alloc, self->prefix, self->prefix_len);
      memcpy(alloc + self->prefix_len, text, text_len);
      alloc[string_len] = '\0';
      *slot = alloc;
   }

   return *slot;
}


struct key_value* prefix_find_type_cons_binding(struct prefix* self, char* name, int name_len) {
   char* sym = prefix_find_sym(self, name, name_len);
   if (!sym) return NULL;
   return hash_map_find(&self->inter->type_constructor_table, sym, NULL);
}

struct key_value* prefix_make_type_cons_binding(struct prefix* self, char* name, int name_len) {
   char* sym = prefix_make_sym(self, name, name_len);
   return inter_mapping_intern(&self->inter->type_constructor_table, sym);
}

struct key_value* prefix_find_type_binding(struct prefix* self, char* name, int name_len) {
   char* sym = prefix_find_sym(self, name, name_len);
   if (!sym) return NULL;
   return hash_map_find(&self->inter->type_def_table, sym, NULL);
}

struct key_value* prefix_make_type_binding(struct prefix* self, char* name, int name_len) {
   char* sym = prefix_make_sym(self, name, name_len);
   return inter_mapping_intern(&self->inter->type_def_table, sym);
}

struct key_value* prefix_find_value_binding(struct prefix* self, char* name, int name_len) {
   char* sym = prefix_find_sym(self, name, name_len);
   if (!sym) return NULL;
   return hash_map_find(&self->inter->values_table, sym, NULL);
}

struct key_value* prefix_make_value_binding(struct prefix* self, char* name, int name_len) {
   char* sym = prefix_make_sym(self, name, name_len);
   return inter_mapping_intern(&self->inter->values_table, sym);
}

type_constructor_fn prefix_query_type_cons(struct prefix* self, char* name, int name_len) {
   struct key_value* binding = prefix_find_type_cons_binding(self, name, name_len);
   if (binding) {
      return (type_constructor_fn)hash_map_sym_top_value(binding);
   }
   return NULL;
}

type prefix_query_type(struct prefix* self, char* name, int name_len) {
   struct key_value* binding = prefix_find_type_binding(self, name, name_len);
   if (binding) {
      return (type)hash_map_sym_top_value(binding);
   }
   return NULL;
}

void prefix_def_type_cons(struct prefix* self, char* name, int name_len, type_constructor_fn type_cons) {
   struct key_value* binding = prefix_make_type_cons_binding(self, name, name_len);
   hash_map_sym_push_value(binding, type_cons);
}

void prefix_def_type(struct prefix* self, char* name, int name_len, type type) {
   struct key_value* binding = prefix_make_type_binding(self, name, name_len);
   hash_map_sym_push_value(binding, type);
}

type_constructor_fn prefix_undef_type_cons(struct prefix* self, char* name, int name_len) {
   struct key_value* binding = prefix_find_type_cons_binding(self, name, name_len);
   if (binding) {
      return (type_constructor_fn)hash_map_sym_pop_value(binding);
   }
   return NULL;
}

type prefix_undef_type(struct prefix* self, char* name, int name_len) {
   struct key_value* binding = prefix_find_type_binding(self, name, name_len);
   if (binding) {
      return (type)hash_map_sym_pop_value(binding);
   }
   return NULL;
}

struct typed_value* prefix_get_value(struct prefix* self, char* name, int name_len) {
   struct key_value* binding = prefix_find_value_binding(self, name, name_len);
   if (binding) {
      return (struct typed_value*)hash_map_sym_top_value(binding);
   }
   return NULL;
}

void prefix_def_value(struct prefix* self, char* name, int name_len, struct typed_value* value) {
   struct key_value* binding = prefix_make_value_binding(self, name, name_len);
   hash_map_sym_push_value(binding, value);
}

struct typed_value* prefix_undef_value(struct prefix* self, char* name, int name_len) {
   struct key_value* binding = prefix_find_value_binding(self, name, name_len);
   if (binding) {
      return (struct typed_value*)hash_map_sym_pop_value(binding);
   }
   return NULL;
}

void prefix_init(struct prefix* self, char* prefix, int prefix_len, struct inter* inter) {
   self->prefix = prefix;
   self->prefix_len = prefix_len;
   self->inter = inter;
}

void prefix_destruct(struct prefix* self, struct inter* inter) {
   inter_release_sym(inter, self->prefix);
}

void prefix_free(struct prefix* self, struct inter* inter) {
   if (!refc_dec(self)) {
      prefix_destruct(self, inter);
      refc_free(self);
   }
}


struct key_value* inter_find_type_cons_binding(struct inter* inter, char* name, int name_len) {
   struct key_value* binding;
   if (inter->current_prefix) {
      binding = prefix_find_type_cons_binding(inter->current_prefix, name, name_len);
      if (binding && binding->value) return binding;
   }

   binding = hash_map_find(&inter->type_constructor_table, name, NULL);
   if (binding && binding->value) return binding;

   struct prefix* first = (struct prefix*)inter->prefix_list.entry_node;

   if (first) {
      struct prefix* curr = first;
      do {
         binding = prefix_find_type_cons_binding(curr, name, name_len);
         if (binding && binding->value) return binding;

         curr = (struct prefix*)curr->node.next;
      } while (curr != first);
   }

   return NULL;
}

struct key_value* inter_make_type_cons_binding(struct inter* inter, char* name, int name_len) {
   if (inter->current_prefix) {
      return prefix_make_type_cons_binding(inter->current_prefix, name, name_len);
   }
   else {
      return inter_mapping_intern(&inter->type_constructor_table, name);
   }
}


struct key_value* inter_find_type_binding(struct inter* inter, char* name, int name_len) {
   struct key_value* binding;
   if (inter->current_prefix) {
      binding = prefix_find_type_binding(inter->current_prefix, name, name_len);
      if (binding && binding->value) return binding;
   }

   binding = hash_map_find(&inter->type_def_table, name, NULL);
   if (binding && binding->value) return binding;

   struct prefix* first = (struct prefix*)inter->prefix_list.entry_node;

   if (first) {
      struct prefix* curr = first;
      do {
         binding = prefix_find_type_binding(curr, name, name_len);
         if (binding && binding->value) return binding;

         curr = (struct prefix*)curr->node.next;
      } while (curr != first);
   }

   return NULL;
}

struct key_value* inter_make_type_binding(struct inter* inter, char* name, int name_len) {
   if (inter->current_prefix) {
      return prefix_make_type_binding(inter->current_prefix, name, name_len);
   }
   else {
      return inter_mapping_intern(&inter->type_def_table, name);
   }
}


struct key_value* inter_find_value_binding(struct inter* inter, char* name, int name_len) {
   struct key_value* binding;
   if (inter->current_prefix) {
      binding = prefix_find_value_binding(inter->current_prefix, name, name_len);
      if (binding && binding->value) return binding;
   }

   binding = hash_map_find(&inter->values_table, name, NULL);
   if (binding && binding->value) return binding;

   struct prefix* first = (struct prefix*)inter->prefix_list.entry_node;

   if (first) {
      struct prefix* curr = first;
      do {
         binding = prefix_find_value_binding(curr, name, name_len);
         if (binding && binding->value) return binding;

         curr = (struct prefix*)curr->node.next;
      } while (curr != first);
   }

   return NULL;
}


struct key_value* inter_make_value_binding(struct inter* inter, char* name, int name_len) {
   if (inter->current_prefix) {
      return prefix_make_value_binding(inter->current_prefix, name, name_len);
   }
   else {
      return inter_mapping_intern(&inter->values_table, name);
   }
}


type_constructor_fn inter_query_type_cons(struct inter* inter, char* name, int name_len) {
   type_constructor_fn val = (type_constructor_fn)hash_map_find_top_value(&inter->type_constructor_table, name, NULL);
   if (val) return val;

   if (inter->current_prefix) {
      val = prefix_query_type_cons(inter->current_prefix, name, name_len);
      if (val) return val;
   }

   struct prefix* first = (struct prefix*)inter->prefix_list.entry_node;

   if (first) {
      struct prefix* curr = first;
      do {
         val = prefix_query_type_cons(curr, name, name_len);
         if (val) return val;

         curr = (struct prefix*)curr->node.next;
      } while (curr != first);
   }

   return NULL;
}

type inter_query_type(struct inter* inter, char* name, int name_len) {
   type val = (type)hash_map_find_top_value(&inter->type_def_table, name, NULL);
   if (val) return val;

   if (inter->current_prefix) {
      val = prefix_query_type(inter->current_prefix, name, name_len);
      if (val) return val;
   }

   struct prefix* first = (struct prefix*)inter->prefix_list.entry_node;

   if (first) {
      struct prefix* curr = first;
      do {
         val = prefix_query_type(curr, name, name_len);
         if (val) return val;

         curr = (struct prefix*)curr->node.next;
      } while (curr != first);
   }

   return NULL;
}

void inter_def_type_cons(struct inter* inter, char* name, int name_len, type_constructor_fn type_cons) {
   if (inter->current_prefix) {
      prefix_def_type_cons(inter->current_prefix, name, name_len, type_cons);
   }
   else {
      inter_mapping_push_value(&inter->type_constructor_table, name, type_cons);
   }
}

void inter_def_type(struct inter* inter, char* name, int name_len, type type) {
   if (inter->current_prefix) {
      prefix_def_type(inter->current_prefix, name, name_len, type);
   }
   else {
      inter_mapping_push_value(&inter->type_def_table, name, (void*)type);
   }
}

type_constructor_fn inter_undef_type_cons(struct inter* inter, char* name, int name_len) {
   if (inter->current_prefix) {
      type_constructor_fn val = prefix_undef_type_cons(inter->current_prefix, name, name_len);
      if (val) return val;
   }

   return (type_constructor_fn)hash_map_pop_value_nf(&inter->type_constructor_table, name, NULL);
}

type inter_undef_type(struct inter* inter, char* name, int name_len) {
   if (inter->current_prefix) {
      type val = prefix_undef_type(inter->current_prefix, name, name_len);
      if (val) return val;
   }

   return (type)hash_map_pop_value_nf(&inter->type_def_table, name, NULL);
}

struct typed_value* inter_get_value(struct inter* inter, char* name, int name_len) {
   struct typed_value* val = (struct typed_value*)hash_map_find_top_value(&inter->values_table, name, NULL);
   if (val) return val;

   if (inter->current_prefix) {
      val = prefix_get_value(inter->current_prefix, name, name_len);
      if (val) return val;
   }

   struct prefix* first = (struct prefix*)inter->prefix_list.entry_node;

   if (first) {
      struct prefix* curr = first;
      do {
         val = prefix_get_value(curr, name, name_len);
         if (val) return val;

         curr = (struct prefix*)curr->node.next;
      } while (curr != first);
   }

   return NULL;
}

void inter_def_value(struct inter* inter, char* name, int name_len, struct typed_value* value) {
   if (inter->current_prefix) {
      prefix_def_value(inter->current_prefix, name, name_len, value);
   }
   else {
      inter_mapping_push_value(&inter->values_table, name, value);
   }
}

struct typed_value* inter_undef_value(struct inter* inter, char* name, int name_len) {
   if (inter->current_prefix) {
      struct typed_value* val = prefix_undef_value(inter->current_prefix, name, name_len);
      if (val) return val;
   }

   return (struct typed_value*)hash_map_pop_value_nf(&inter->values_table, name, NULL);
}


#if 0
void inter_undef_all(struct inter* inter) {
   // Values table
   radix_tree_value_fn callback = (radix_tree_value_fn)typed_value_destruct_;
   radix_tree_for_values(&inter->values_table, &callback);
   radix_tree_delete_all_children(&inter->values_table);

   // Type cons table
   radix_tree_delete_all_children(&inter->type_constructor_table);

   // Type def table.
   callback = (radix_tree_value_fn)type_free_;
   radix_tree_for_values(&inter->type_def_table, &callback);
   radix_tree_delete_all_children(&inter->type_def_table);
}
#endif


void inter_free_value(struct inter* inter, char* name, int name_len) {
   struct typed_value* value = inter_undef_value(inter, name, name_len);
   if (value) {
      typed_value_destruct(value, inter);
      free(value);
   }
}

void inter_pop_value(struct inter* inter, char* name, int name_len) {
   struct typed_value* value = inter_undef_value(inter, name, name_len);
   if (value) {
      int size = typed_value_size(value->type);
      typed_value_destruct(value, inter);
      stack_decrease_top(&inter->stack, size);
   }
}

void inter_cache_type_cons_symbol(struct inter* inter, struct ast_node_with_type* node) {
   if (!node->symbol) {
      node->symbol = inter_make_type_cons_binding(inter, node->ast_node.text, node->ast_node.text_len);
   }
}

void inter_cache_type_symbol(struct inter* inter, struct ast_node_with_type* node) {
   if (!node->symbol) {
      node->symbol = inter_make_type_binding(inter, node->ast_node.text, node->ast_node.text_len);
   }
}

void inter_cache_value_symbol(struct inter* inter, struct ast_node_with_type* node) {
   if (!node->symbol) {
      node->symbol = inter_make_value_binding(inter, node->ast_node.text, node->ast_node.text_len);
   }
}

void inter_cache_find_type_cons_symbol(struct inter* inter, struct ast_node_with_type* node) {
   if (!node->symbol) {
      node->symbol = inter_find_type_cons_binding(inter, node->ast_node.text, node->ast_node.text_len);
   }
}

void inter_cache_find_type_symbol(struct inter* inter, struct ast_node_with_type* node) {
   if (!node->symbol) {
      node->symbol = inter_find_type_binding(inter, node->ast_node.text, node->ast_node.text_len);
   }
}

void inter_cache_find_value_symbol(struct inter* inter, struct ast_node_with_type* node) {
   if (!node->symbol) {
      node->symbol = inter_find_value_binding(inter, node->ast_node.text, node->ast_node.text_len);
   }
}



void inter_init(struct inter* inter, void* stack_buffer, int stack_size) {
   hash_table_init(&inter->sym_table, &hash_string_obj, 0);

   hash_table_init(&inter->type_constructor_table, &hash_pointer_obj, INTER_INIT_TYPE_CONS_SIZE);
   hash_table_init(&inter->type_def_table, &hash_pointer_obj, INTER_INIT_TYPE_DEF_SIZE);
   hash_table_init(&inter->values_table, &hash_pointer_obj, INTER_INIT_VALUE_SIZE);

   stack_init(&inter->stack, stack_buffer, stack_size);

   inter->current_prefix = NULL;
   dlist_root_init(&inter->prefix_list);

   dlist_root_init(&inter->script_path_stack);
   hash_table_init(&inter->import_set, &hash_pointer_obj, INTER_INIT_IMPORT_SIZE);
}


void inter_clear_prefixes(struct inter* inter) {
   if (inter->current_prefix) {
      prefix_free(inter->current_prefix, inter);
      inter->current_prefix = NULL;
   }

   while (!dlist_root_empty(&inter->prefix_list)) {
      struct prefix* prefix = (struct prefix*)dlist_root_pop(&inter->prefix_list);
      prefix_free(prefix, inter);
   }
}


void inter_destruct(struct inter* inter) {

   inter_mapping_destruct(&inter->values_table, inter, (list_for_each_fn)typed_value_destruct_);
   inter_mapping_destruct(&inter->type_constructor_table, inter, NULL);
   inter_mapping_destruct(&inter->type_def_table, inter, (list_for_each_fn)type_def_cell_unbind_free);

   hash_set_for_each3(&inter->import_set, (hash_value2_fn)inter_release_sym, inter);
   hash_table_destruct(&inter->import_set);

   inter_clear_prefixes(inter);

   //hash_set_dump(stderr, &inter->sym_table, (hash_value2_fn)print_string);
   //hash_set_for_each(&inter->sym_table, (hash_value_fn)refc_free);
   hash_table_destruct(&inter->sym_table);
}


#ifdef REFC_SAFETY_STAMP

void inter_validate_syms(struct inter* inter) {
   int i;
   for (i = 0; i < inter->sym_table.array_length; i++) {
      char* curr = inter->sym_table.array[i];

      if (curr) {
         assert(refc_check_stamp(curr));
      }
   }
}
#endif


char* inter_intern_sym(struct inter* inter, char* buffer, int length) {
   char* ret;
   char** slot = (char**)hash_set_slot(&inter->sym_table, buffer, NULL);
   if (!*slot) {
      char* copy = (char*)refc_alloc(length + 1);
      refc_dec(copy);
      memcpy(copy, buffer, length);
      copy[length] = '\0';
      *slot = copy;
      ret = copy;
   }
   else {
      ret = *slot;
   }

   LOG("inter_intern_sym: %s : %p\n", ret, ret)
   return ret;
}


void inter_release_sym(struct inter* inter, char* buffer) {
   //hash_set_dump(stderr, &inter->sym_table, (hash_value2_fn)print_string);

#ifdef REFC_SAFETY_STAMP
   //assert(refc_check_stamp(buffer));
   //inter_validate_syms(inter);
#endif

   char* sym = hash_set_find(&inter->sym_table, buffer, NULL);
   if (sym) {
      int count = refc_dec(sym);
      LOG("inter_release_sym: %s : %p, %d\n", buffer, buffer, count)
      if (count == 0) {
         hash_set_remove(&inter->sym_table, sym, NULL);
         refc_free(sym);
      }
   }
   else {
      assert(0);
   }
}


char* inter_conv_str_fn(struct inter_conv_str* self, char* buffer, int length) {
   buffer[length] = '\0';
   return (char*)refc_copy(inter_intern_sym(self->inter, buffer, length));
}


void inter_del_str_fn(struct inter_del_str* self, char* buffer, int length) {
   inter_release_sym(self->inter, buffer);
}

void inter_push_abs_path_str(struct inter* inter, char* buffer, int buf_len) {
   LOG("inter_push_abs_path_str: %s, %d\n", buffer, buf_len)
   struct filepath_node* node = (struct filepath_node*)malloc(sizeof(struct filepath_node));
   assert(node);
   filepath_from_str(&node->filepath, inter, buffer, buf_len);
   path_reduce(&node->filepath.path, inter);
   dlist_root_push(&inter->script_path_stack, &node->node);
}

void inter_push_abs_path(struct inter* inter, struct filepath* filepath) {
   struct filepath_node* node = (struct filepath_node*)malloc(sizeof(struct filepath_node));
   assert(node);
   filepath_clone(filepath, &node->filepath);
   dlist_root_push(&inter->script_path_stack, &node->node);
}

void inter_push_relative_path(struct inter* inter, struct filepath* filepath) {
   assert(!dlist_root_empty(&inter->script_path_stack));

   struct filepath_node* top = (struct filepath_node*)inter->script_path_stack.entry_node;
   struct filepath_node* node = filepath_node(filepath->filename_sym, NULL);

   struct dlist_root filepath_copy;
   path_copy(&top->filepath.path, &node->filepath.path);
   path_copy(&filepath->path, &filepath_copy);
   path_merge(&node->filepath.path, &filepath_copy, inter);
   dlist_root_push(&inter->script_path_stack, &node->node);

   /*
   if (!top->filepath.has_path) {
      if (!filepath->has_path) {
      }
      else {
         path_copy(&filepath->path, &node->filepath.path);
      }
   }
   else {
      if (!filepath->has_path) {
         path_copy(&top->filepath.path, &node->filepath.path);
      }
      else {
         struct dlist_root filepath_copy;
         path_copy(&top->filepath.path, &node->filepath.path);
         path_copy(&filepath->path, &filepath_copy);
         path_merge(&node->filepath.path, &filepath_copy, inter);
      }
   }
   */
}

void ast_node_with_type_free_types(struct ast_node_with_type* root, struct inter* inter) {
   if (root->type) {
      (*root->type)->type_free(root->type, inter);
      root->type = NULL;
   }
   root->returns_lval = 0;

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   if (first) {
      struct ast_node_with_type* curr = first;
      do {
         ast_node_with_type_free_types(curr, inter);

         curr = (struct ast_node_with_type*)curr->ast_node.node.next;
      } while (curr != first);
   }
}


void ast_node_with_type_free(struct ast_node_with_type* root, struct inter* inter) {
   if (refc_dec(root))
      return;

   inter_release_sym(inter, root->ast_node.text);
   if (root->ast_node.filename) {
      inter_release_sym(inter, root->ast_node.filename);
   }

   if (root->type) {
      (*root->type)->type_free(root->type, inter);
   }

   refc_free(root);
}

void ast_node_with_type_rec_free(struct ast_node_with_type* root, struct inter* inter) {
   if (refc_dec(root))
      return;

   inter_release_sym(inter, root->ast_node.text);
   if (root->ast_node.filename) {
      inter_release_sym(inter, root->ast_node.filename);
   }

   if (root->type) {
      (*root->type)->type_free(root->type, inter);
   }

   while(!dlist_root_empty(&root->ast_node.children)) {
      struct ast_node_with_type* curr = (struct ast_node_with_type*)dlist_root_pop(&root->ast_node.children);
      ast_node_with_type_rec_free(curr, inter);
   }

   LOG("ast_node_with_type_rec_free: count=%d\n", refc_count(root))

   refc_free(root);
}

void ast_node_with_type_free_children(struct ast_node_with_type* root, struct inter* inter) {
   while(!dlist_root_empty(&root->ast_node.children)) {
      struct ast_node_with_type* curr = (struct ast_node_with_type*)dlist_root_pop(&root->ast_node.children);
      ast_node_with_type_rec_free(curr, inter);
   }
}

void ast_node_with_type_write_to(struct ast_node_with_type* root, struct inter* inter, struct ast_node_with_type* dest) {
#ifdef LOGGING
   LOG("ast_node_with_type_write_to: root=")
   ast_node_print(LOG_FILE, &root->ast_node);
   LOG(", dest=")
   ast_node_print(LOG_FILE, &dest->ast_node);
   LOG("\n")
#endif


   if (dest->ast_node.filename != root->ast_node.filename) {
      if (dest->ast_node.filename)
         inter_release_sym(inter, dest->ast_node.filename);

      if (root->ast_node.filename)
         dest->ast_node.filename = (char*)refc_copy(root->ast_node.filename);
      else
         dest->ast_node.filename = 0;
   }

   dest->ast_node.line = root->ast_node.line;
   dest->ast_node.column = root->ast_node.column;

   if (dest->ast_node.text != root->ast_node.text) {
      if (dest->ast_node.text)
         inter_release_sym(inter, dest->ast_node.text);

      if (root->ast_node.text)
         dest->ast_node.text = (char*)refc_copy(root->ast_node.text);
      else
         dest->ast_node.text = NULL;
   }

   dest->ast_node.text_len = root->ast_node.text_len;
   dest->ast_node.state = root->ast_node.state;

   if (dest->type) {
      (*dest->type)->type_free(dest->type, inter);
   }

   if (root->type)
      dest->type = (*root->type)->type_copy(root->type);
   else
      dest->type = NULL;

   dest->returns_lval = root->returns_lval;
   dest->symbol = root->symbol;
   dest->constant = root->constant;
   dest->eval_table = root->eval_table;
}


// Copies a node. Copy has no children.
struct ast_node_with_type* ast_node_with_type_copy(struct ast_node_with_type* root) {
   struct ast_node_with_type* alloc = (struct ast_node_with_type*)refc_alloc(sizeof(struct ast_node_with_type));
   assert(alloc);

   if (root->ast_node.filename)
      alloc->ast_node.filename = (char*)refc_copy(root->ast_node.filename);
   else
      alloc->ast_node.filename = 0;

   alloc->ast_node.line = root->ast_node.line;
   alloc->ast_node.column = root->ast_node.column;

   if (root->ast_node.text)
      alloc->ast_node.text = (char*)refc_copy(root->ast_node.text);
   else
      alloc->ast_node.text = NULL;

   alloc->ast_node.text_len = root->ast_node.text_len;
   alloc->ast_node.state = root->ast_node.state;

   dlist_root_init(&alloc->ast_node.children);
   dlist_init(&alloc->ast_node.node);

   if (root->type)
      alloc->type = (*root->type)->type_copy(root->type);
   else
      alloc->type = NULL;

   alloc->returns_lval = root->returns_lval;
   alloc->symbol = root->symbol;
   alloc->constant = root->constant;
   alloc->eval_table = root->eval_table;

   return alloc;
}

// Recursively copies all children.
struct ast_node_with_type* ast_node_with_type_copy_tree(struct ast_node_with_type* root) {
   struct ast_node_with_type* root_copy = ast_node_with_type_copy(root);

   struct ast_node_with_type* first = (struct ast_node_with_type*)root->ast_node.children.entry_node;
   if (first) {
      struct ast_node_with_type* curr = first;
      do {
         struct ast_node_with_type* curr_copy = ast_node_with_type_copy_tree(curr);
         dlist_root_append(&root_copy->ast_node.children, &curr_copy->ast_node.node);

         curr = (struct ast_node_with_type*)curr->ast_node.node.next;
      } while (curr != first);
   }

   return root_copy;
}


// Overwrite location information.
// Applied to code generated by macro expansion.
void ast_node_with_type_write_location(struct ast_node_with_type* location, struct inter* inter, struct ast_node_with_type* tree) {

   if (tree->ast_node.filename != location->ast_node.filename) {
      if (tree->ast_node.filename)
         inter_release_sym(inter, tree->ast_node.filename);

      if (location->ast_node.filename)
         tree->ast_node.filename = (char*)refc_copy(location->ast_node.filename);
      else
         tree->ast_node.filename = NULL;
   }

   tree->ast_node.line = location->ast_node.line;
   tree->ast_node.column = location->ast_node.column;

   struct ast_node_with_type* first = (struct ast_node_with_type*)tree->ast_node.children.entry_node;
   if (first) {
      struct ast_node_with_type* curr = first;
      do {
         ast_node_with_type_write_location(location, inter, curr);

         curr = (struct ast_node_with_type*)curr->ast_node.node.next;
      } while (curr != first);
   }
}

