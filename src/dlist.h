#ifndef __DLIST_H__
#define __DLIST_H__

struct dlist_root {
   struct dlist* entry_node;
};

struct dlist {
   struct dlist_root* root;
   struct dlist* next;
   struct dlist* prev;
};

void dlist_init(struct dlist* node);
void dlist_remove(struct dlist* node);
void dlist_insert_before(struct dlist* node, struct dlist* other);
int dlist_length(struct dlist* node);
struct dlist* dlist_nth(struct dlist* node, int index);
void dlist_root_adopt(struct dlist_root* root, struct dlist* node);
void dlist_root_append(struct dlist_root* root, struct dlist* node);
void dlist_root_push(struct dlist_root* root, struct dlist* node);
void dlist_root_init(struct dlist_root* root);
struct dlist* dlist_root_pop(struct dlist_root* root);
struct dlist* dlist_root_pop_back(struct dlist_root* root);
int dlist_root_empty(struct dlist_root* root);
void dlist_root_take_nodes(struct dlist_root* taker, struct dlist_root* takee);
int dlist_root_length(struct dlist_root* root);
void dlist_root_swap_nodes(struct dlist_root* self, struct dlist_root* other);
struct dlist* dlist_root_nth(struct dlist_root* root, int index);
 


#endif
