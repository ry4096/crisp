#include <malloc.h>
#include <assert.h>

#include "list.h"


void list_init(struct list* self, void* value, struct list* rest) {
   self->value = value;
   self->rest = rest;
}

struct list* list_new(void* value, struct list* rest) {
   struct list* self = (struct list*)malloc(sizeof(struct list));
   assert(self);
   list_init(self, value, rest);
   return self;
}

void list_for_each(struct list* self, list_for_each_fn* callback) {
   while(self) {
      (*callback)(callback, self->value);
      self = self->rest;
   }
}

void list_for_each2(struct list* self, list_for_each_fn callback, void* callback_data) {
   while(self) {
      callback(callback_data, self->value);
      self = self->rest;
   }
}

void list_free(struct list* self) {
   struct list* next;
   while(self) {
      next = self->rest;
      free(self);
      self = next;
   }
}

struct list* list_push(struct list** handle, void* value) {
   struct list* cell = list_new(value, *handle);
   *handle = cell;
   return cell;
}

void* list_pop(struct list** handle) {
   struct list* top = *handle;
   if (!top)
      return NULL;
   *handle = top->rest;
   void* value = top->value;
   free(top);
   return value;
}

void* list_nth(struct list* self, int index) {
   while(self && index > 0) {
      index--;
      self = self->rest;
   }
   if (!self)
      return NULL;
   return self->value;
}
