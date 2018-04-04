#ifndef PATH_H_
#define PATH_H_

#include "dlist.h"
#include "refc.h"

struct inter;

struct path_node {
   struct dlist node;
   char* sym;
};

struct filepath {
   char* filename_sym;
   char* extension; // hard coded extension.
   struct dlist_root path;
};

struct filepath_node {
   struct dlist node;
   struct filepath filepath;
};

struct path_node* path_node(char* sym);
void path_node_free(struct path_node* self, struct inter* inter);
void path_destruct(struct dlist_root* self, struct inter* inter);
void path_append(struct dlist_root* self, char* sym);
void path_push(struct dlist_root* self, char* sym);
void path_pop_back(struct dlist_root* self, struct inter* inter);
void path_pop(struct dlist_root* self, struct inter* inter);
void path_reduce(struct dlist_root* self, struct inter* inter);
int path_starts_dot_dot(struct dlist_root* self);
int path_ends_dot_dot(struct dlist_root* self);
void path_merge(struct dlist_root* start, struct dlist_root* end, struct inter* inter);
void path_copy(struct dlist_root* self, struct dlist_root* clone);
int path_length(struct dlist_root* self);
int path_write(struct dlist_root* self, char* buffer);
char* path_str(struct dlist_root* self);
char* path_sym(struct dlist_root* self, struct inter* inter);
void path_from_str(struct dlist_root* self, struct inter* inter, char* buffer, int buf_len);
void path_print(FILE* output, struct dlist_root* self);


int filepath_has_path(struct filepath* self);
void filepath_from_str(struct filepath* self, struct inter* inter, char* buffer, int buf_len);
int filepath_length(struct filepath* self);
int filepath_write(struct filepath* self, char* buffer);
char* filepath_str(struct filepath* self);
char* filepath_sym(struct filepath* self, struct inter* inter);
void filepath_print(FILE* output, struct filepath* self);
void filepath_clone(struct filepath* src, struct filepath* dest);
void filepath_destruct(struct filepath* self, struct inter* inter);
struct filepath_node* filepath_node(char* filename_sym, char* extension);
void filepath_node_free(struct filepath_node* self, struct inter* inter);


#endif
