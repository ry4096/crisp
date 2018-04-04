#include <string.h>
#include <assert.h>
#include <malloc.h>


#include "path.h"
#include "dlist.h"
#include "refc.h"
#include "inter.h"

#define LOGGING_OFF
#include "log.h"

#ifdef WINDOWS
#define PATH_DELIM '\\'
#else
#define PATH_DELIM '/'
#endif

#define DOT "."
#define DOT_DOT ".."


struct path_node* path_node(char* sym) {
   struct path_node* alloc = (struct path_node*)malloc(sizeof(struct path_node));
   assert(alloc);
   alloc->sym = (char*)refc_copy(sym);
   return alloc;
}

void path_node_free(struct path_node* self, struct inter* inter) {
   LOG("path_node_free: %s\n", self->sym)
   inter_release_sym(inter, self->sym);
   free(self);
}

void path_destruct(struct dlist_root* self, struct inter* inter) {
   while(!dlist_root_empty(self)) {
      struct path_node* curr = (struct path_node*)dlist_root_pop(self);
      path_node_free(curr, inter);
   }
}

void path_append(struct dlist_root* self, char* sym) {
   struct path_node* new_node = path_node(sym);
   dlist_root_append(self, &new_node->node);
}

void path_push(struct dlist_root* self, char* sym) {
   struct path_node* new_node = path_node(sym);
   dlist_root_push(self, &new_node->node);
}

void path_pop_back(struct dlist_root* self, struct inter* inter) {
   struct path_node* back = (struct path_node*)dlist_root_pop_back(self);
   path_node_free(back, inter);
}

void path_pop(struct dlist_root* self, struct inter* inter) {
   struct path_node* front = (struct path_node*)dlist_root_pop(self);
   path_node_free(front, inter);
}

void path_reduce(struct dlist_root* self, struct inter* inter) {
   struct path_node* first = (struct path_node*)self->entry_node;
   struct path_node* curr;
   struct path_node* prev;
   struct path_node* next;

#ifdef LOGGING
   LOG("path_reduce: before: ")
   path_print(LOG_FILE, self);
   LOG("\n")
#endif

   if (first) {
      curr = first;
      for (;;) {
         next = (struct path_node*)curr->node.next;
         if (!strcmp(curr->sym, DOT)) {
            dlist_remove(&curr->node);
            path_node_free(curr, inter);
            if (next == curr) break;
   
            curr = next;
         }
         else if (!strcmp(curr->sym, DOT_DOT) && curr != first) {
            prev = (struct path_node*)curr->node.prev;
            if (first == prev) {
               first = next;
            }
            dlist_remove(&prev->node);
            dlist_remove(&curr->node);
            path_node_free(prev, inter);
            path_node_free(curr, inter);
            if (next == prev) break;
   
            curr = next;
         }
         else {
            if (next == first) break;
            curr = next;
         }
      }
   }

#ifdef LOGGING
   LOG("path_reduce: after: ")
   path_print(LOG_FILE, self);
   LOG("\n")
#endif
}

int path_starts_dot_dot(struct dlist_root* self) {
   struct path_node* first = (struct path_node*)self->entry_node;

   return first && !strcmp(first->sym, DOT_DOT);
}

int path_ends_dot_dot(struct dlist_root* self) {
   struct path_node* first = (struct path_node*)self->entry_node;

   if (first) {
      struct path_node* last = (struct path_node*)first->node.prev;
      return !strcmp(last->sym, DOT_DOT);
   }
   else {
      return 0;
   }
}

// start ends up having the whole thing. end becomes empty.
void path_merge(struct dlist_root* start, struct dlist_root* end, struct inter* inter) {
   while (!dlist_root_empty(start) && path_starts_dot_dot(end) && !path_ends_dot_dot(start)) {
      path_pop_back(start, inter);
      path_pop(end, inter);
   }

   while (!dlist_root_empty(end)) {
      dlist_root_append(start, dlist_root_pop(end));
   }
}

void path_copy(struct dlist_root* self, struct dlist_root* clone) {
   dlist_root_init(clone);
   struct path_node* first = (struct path_node*)self->entry_node;
   if (first) {
      struct path_node* curr = first;
      do {
         path_append(clone, curr->sym);
         curr = (struct path_node*)curr->node.next;
      } while (curr != first);
   }
}

void path_append_str(struct dlist_root* self, struct inter* inter, char* buffer, int length) {
   LOG("path_append_str: length=%d\n", length)
   assert(length >= 0);

   char* sym = inter_intern_sym(inter, buffer, length);
   LOG("path_append_str: sym=%s\n", sym)

   struct path_node* node = path_node(sym);
   dlist_root_append(self, &node->node);
}


int path_length(struct dlist_root* self) {
   int length = 0;

   struct path_node* first = (struct path_node*)self->entry_node;
   if (first) {
      length += strlen(first->sym);

      struct path_node* curr = (struct path_node*)first->node.next;

      while (curr != first) {
         length += 1 + strlen(curr->sym);
         curr = (struct path_node*)curr->node.next;
      }
   }

   return length;
}

int path_write(struct dlist_root* self, char* buffer) {
   int length;
   int buf_pos = 0;

   struct path_node* first = (struct path_node*)self->entry_node;
   if (first) {
      length = strlen(first->sym);
      memcpy(buffer, first->sym, length);
      buf_pos = length;

      struct path_node* curr = (struct path_node*)first->node.next;

      while (curr != first) {
         buffer[buf_pos++] = PATH_DELIM;
         length = strlen(curr->sym);
         memcpy(buffer + buf_pos, curr->sym, length);
         buf_pos += length;

         curr = (struct path_node*)curr->node.next;
      }
   }

   buffer[buf_pos] = '\0';

   return buf_pos;
}

char* path_str(struct dlist_root* self) {
   int length = path_length(self);
   char* alloc = (char*)malloc(length + 1);
   assert(alloc);
   int pos = path_write(self, alloc);
   assert(pos == length + 1);
   return alloc;
}

char* path_sym(struct dlist_root* self, struct inter* inter) {
   int length = path_length(self);
   char* alloc = (char*)refc_alloc(length + 1);
   assert(alloc);
   refc_dec(alloc);
   int pos = path_write(self, alloc);
   assert(pos == length + 1);

   char** slot = (char**)hash_set_slot(&inter->sym_table, alloc, NULL);
   if (!*slot) {
      *slot = alloc;
      return alloc;
   }
   else {
      refc_free(alloc);
      return *slot;
   }
}

void path_from_str(struct dlist_root* self, struct inter* inter, char* buffer, int buf_len) {
   LOG("path_from_str: %s, %d\n", buffer, buf_len)

   dlist_root_init(self);

   assert(buf_len >= 0);

   char* copy = malloc(buf_len + 1);
   assert(copy);
   memcpy(copy, buffer, buf_len);
   copy[buf_len] = 0;

   char delims[2];
   delims[0] = PATH_DELIM;
   delims[1] = '\0';

   char* saveptr;
   char* tok;
   for (tok = strtok_r(copy, delims, &saveptr); tok; tok = strtok_r(NULL, delims, &saveptr)) {
      path_append_str(self, inter, tok, strlen(tok));
   }

   free(copy);
}

void path_print(FILE* output, struct dlist_root* self) {
   struct path_node* first = (struct path_node*)self->entry_node;
   if (first) {
      fprintf(output, "%s", first->sym);

      struct path_node* curr = (struct path_node*)first->node.next;

      while (curr != first) {
         fprintf(output, "%c%s", PATH_DELIM, curr->sym);
         curr = (struct path_node*)curr->node.next;
      }
   }
}




int filepath_has_path(struct filepath* self) {
   return !dlist_root_empty(&self->path);
}

void filepath_from_str(struct filepath* self, struct inter* inter, char* buffer, int buf_len) {
   char* last_slash = strrchr(buffer, PATH_DELIM);
   int path_len;
   char* filename;
   int filename_len;

   if (last_slash == NULL) {
      filename = buffer;
      path_len = 0;
      filename_len = buf_len;
      dlist_root_init(&self->path);
   }
   else {
      filename = last_slash + 1;
      path_len = last_slash - buffer;
      filename_len = buf_len - 1 - path_len;
      path_from_str(&self->path, inter, buffer, path_len);
   }

   self->filename_sym = (char*)refc_copy(inter_intern_sym(inter, filename, filename_len));
   self->extension = NULL;
}

int filepath_length(struct filepath* self) {
   int length = 0;

   if (filepath_has_path(self)) {
      length += path_length(&self->path) + 1;
   }

   length += strlen(self->filename_sym);

   if (self->extension) {
      length += strlen(self->extension);
   }

   return length;
}

int filepath_write(struct filepath* self, char* buffer) {
   int pos = 0;
   int sym_len = strlen(self->filename_sym);

   if (filepath_has_path(self)) {
      pos = path_write(&self->path, buffer);
      buffer[pos++] = PATH_DELIM;
   }

   memcpy(buffer + pos, self->filename_sym, sym_len);
   pos += sym_len;

   if (self->extension) {
      int extension_len = strlen(self->extension);
      memcpy(buffer + pos, self->extension, extension_len);
      pos += extension_len;
   }

   buffer[pos] = '\0';
   return pos;
}

char* filepath_str(struct filepath* self) {
   int length = filepath_length(self);
   char* alloc = (char*)malloc(length + 1);
   assert(alloc);
   int pos = filepath_write(self, alloc);
   assert(pos == length);
   return alloc;
}

char* filepath_sym(struct filepath* self, struct inter* inter) {
   int length = filepath_length(self);
   char* alloc = (char*)refc_alloc(length + 1);
   assert(alloc);
   refc_dec(alloc);
   int pos = filepath_write(self, alloc);
   assert(pos == length);

   char** slot = (char**)hash_set_slot(&inter->sym_table, alloc, NULL);
   if (!*slot) {
      *slot = alloc;
      return alloc;
   }
   else {
      refc_free(alloc);
      return *slot;
   }
}

void filepath_print(FILE* output, struct filepath* self) {
   if (filepath_has_path(self)) {
      path_print(output, &self->path);
      fprintf(output, "%c", PATH_DELIM);
   }

   fprintf(output, "%s", self->filename_sym);

   if (self->extension) {
      fprintf(output, "%s", self->extension);
   }
}


void filepath_clone(struct filepath* src, struct filepath* dest) {
   dest->filename_sym = src->filename_sym;
   dest->extension = src->extension;
   dest->path.entry_node = src->path.entry_node;
   dlist_root_init(&dest->path);
}

void filepath_destruct(struct filepath* self, struct inter* inter) {
   inter_release_sym(inter, self->filename_sym);
   path_destruct(&self->path, inter);
}

struct filepath_node* filepath_node(char* filename_sym, char* extension) {
   struct filepath_node* node = (struct filepath_node*)malloc(sizeof(struct filepath_node));
   assert(node);

   node->filepath.filename_sym = (char*)refc_copy(filename_sym);
   node->filepath.extension = extension;
   dlist_root_init(&node->filepath.path);

   return node;
}

void filepath_node_free(struct filepath_node* self, struct inter* inter) {
   filepath_destruct(&self->filepath, inter);
   free(self);
}




