#include <stdio.h>

#define ALIGN_MASK(mask, val) \
   ((val) + (mask)) & (~(mask))

#define ALIGN(bits, val) \
   ALIGN_MASK((1<<(bits)) - 1, val)

int main() {
   int x;
   for(x = -16; x < 32; x++) {
      printf("align(%d) = %d\n", x, ALIGN(2, x));
   }
}


