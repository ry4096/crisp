#include <stdlib.h>
#include "stack.h"

void stack_init(struct stack* stack, char* buffer, int buffer_length) {
   stack->buffer = buffer;
   stack->buffer_length = buffer_length;
   stack->stack_top = 0;
}

void* stack_alloc(struct stack* stack, int size) {
   int new_top = stack->stack_top + size;
   void* ptop = &stack->buffer[stack->stack_top];

   if (new_top > stack->buffer_length) {
      // Overflow.
      // Try to recover later?
      return NULL;
   }

   stack->stack_top = new_top;
   return ptop;
}

int stack_decrease_top(struct stack* stack, int size) {
   int new_top = stack->stack_top - size;

   if (new_top < 0) {
      // Underflow.
      return 0;
   }

   stack->stack_top = new_top;
   return 1;
}

