#ifndef __STACK_H__
#define __STACK_H__

#include <assert.h>

struct stack {
   char* buffer;
   int buffer_length;
   int stack_top;
};

void stack_init(struct stack* stack, char* buffer, int buffer_length);
void* stack_alloc(struct stack* stack, int size);
int stack_decrease_top(struct stack* stack, int size);

#define STACK_ALLOC(alloc, type, stack, size) \
   alloc = (type)(stack.buffer + stack.stack_top); \
   stack.stack_top += (size); \
   assert(stack.stack_top <= stack.buffer_length);



#endif
