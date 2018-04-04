#ifndef LIST_H_
#define LIST_H_

struct list {
   void* value;
   struct list* rest;
};

typedef void (*list_for_each_fn)(void* obj, void* value);


void list_init(struct list* self, void* value, struct list* rest);
struct list* list_new(void* value, struct list* rest);
void list_for_each(struct list* self, list_for_each_fn* callback);
void list_for_each2(struct list* self, list_for_each_fn callback, void* callback_data);
void list_free(struct list* self);
struct list* list_push(struct list** handle, void* value);
void* list_pop(struct list** handle);
void* list_nth(struct list* self, int index);


#endif
