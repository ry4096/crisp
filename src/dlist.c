#include <assert.h>
#include <stdio.h>

#include "dlist.h"


void dlist_init(struct dlist* node) {
   node->root = NULL;
   node->next = NULL;
   node->prev = NULL;
}

void dlist_remove_neighbors(struct dlist* node) {
   node->prev->next = node->next;
   node->next->prev = node->prev;
}

void dlist_remove(struct dlist* node) {
   assert(node);
   if(node == node->root->entry_node) {
      if(node->next != node)
         node->root->entry_node = node->next;
      else
         node->root->entry_node = NULL;
   }

   node->root = NULL;
   dlist_remove_neighbors(node);
}

void dlist_insert_before(struct dlist* node, struct dlist* other) {
   assert(node);
   assert(other);
   node->next = other;
   node->prev = other->prev;
   other->prev->next = node;
   other->prev = node;
   node->root = other->root;
}

int dlist_length(struct dlist* node) {
   int length = 1;
   struct dlist* curr = node->next;
   while(curr != node) {
      length++;
      curr = curr->next;
   }
   return length;
}

struct dlist* dlist_nth(struct dlist* node, int index) {
   if (index >= 0) {
      while(index--) {
         node = node->next;
      }
   }
   else {
      while(index++) {
         node = node->prev;
      }
   }
   return node;
}

void dlist_root_adopt(struct dlist_root* root, struct dlist* node) {
   node->next = node->prev = node;
   node->root = root;
   root->entry_node = node;
}

void dlist_root_append(struct dlist_root* root, struct dlist* node) {
   assert(node);
   assert(root);
   if(root->entry_node)
      dlist_insert_before(node, root->entry_node);
   else
      dlist_root_adopt(root, node);
}

void dlist_root_push(struct dlist_root* root, struct dlist* node) {
   dlist_root_append(root, node);
   root->entry_node = node;
}

void dlist_root_init(struct dlist_root* root) {
   root->entry_node = NULL;
}

struct dlist* dlist_root_pop(struct dlist_root* root) {
   assert(!dlist_root_empty(root));
   struct dlist* entry_node = root->entry_node;
   dlist_remove(entry_node);
   return entry_node;
}

struct dlist* dlist_root_pop_back(struct dlist_root* root) {
   assert(!dlist_root_empty(root));
   struct dlist* back_node = root->entry_node->prev;
   dlist_remove(back_node);
   return back_node;
}

int dlist_root_empty(struct dlist_root* root) {
   return root->entry_node == NULL;
}

void dlist_root_take_nodes(
      struct dlist_root* taker,
      struct dlist_root* takee) {
   assert(dlist_root_empty(taker));
   taker->entry_node = takee->entry_node;
   struct dlist* start = takee->entry_node;
   if(start) {
      struct dlist* curr = start;
      do {
         curr->root = taker;
         curr = curr->next;
      } while(curr != start);
   }
   takee->entry_node = NULL;
}

int dlist_root_length(struct dlist_root* root) {
   if(!root->entry_node) {
      return 0;
   }
   else {
      return dlist_length(root->entry_node);
   }
}

void dlist_root_swap_nodes(
      struct dlist_root* self,
      struct dlist_root* other) {
   if (self == other)
      return;

   int other_len = dlist_root_length(other);
   int i;

   while (!dlist_root_empty(self)) {
      dlist_root_append(other, dlist_root_pop(self));
   }

   for (i = 0; i < other_len; i++) {
      dlist_root_append(self, dlist_root_pop(other));
   }
}

struct dlist* dlist_root_nth(struct dlist_root* root, int index) {
   assert(root->entry_node);
   return dlist_nth(root->entry_node, index);
}

