#include <stdio.h>
#include <stdint.h>

#include "bits.h"

// BITS_LOWER_ONES(num_ones)
// BITS_UPPER_ONES(num_zeros)
// BITS_ZERO_WINDOW(offset,width)
// BITS_SET_WINDOW_ZERO(value,offset,width)
// BITS_ALIGN_WINDOW(value,offset,width)
// BITS_SET_WINDOW(value1,offset,width,value2)

int main(int argc, char** argv) {

   printf("BITS_LOWER_ONES(0) = %u\n", BITS_LOWER_ONES(uint32_t,0));
   printf("BITS_LOWER_ONES(1) = %u\n", BITS_LOWER_ONES(uint32_t,1));
   printf("BITS_LOWER_ONES(2) = %u\n", BITS_LOWER_ONES(uint32_t,2));

   printf("BITS_UPPER_ONES(0) = %u\n", BITS_UPPER_ONES(uint32_t,0));
   printf("BITS_UPPER_ONES(1) = %u\n", BITS_UPPER_ONES(uint32_t,1));
   printf("BITS_UPPER_ONES(2) = %u\n", BITS_UPPER_ONES(uint32_t,2));

   printf("BITS_ZERO_WINDOW(0,0) = %u\n", BITS_ZERO_WINDOW(uint32_t,0,0));
   printf("BITS_ZERO_WINDOW(0,1) = %u\n", BITS_ZERO_WINDOW(uint32_t,0,1));
   printf("BITS_ZERO_WINDOW(0,2) = %u\n", BITS_ZERO_WINDOW(uint32_t,0,2));

   printf("BITS_ZERO_WINDOW(1,0) = %u\n", BITS_ZERO_WINDOW(uint32_t,1,0));
   printf("BITS_ZERO_WINDOW(1,1) = %u\n", BITS_ZERO_WINDOW(uint32_t,1,1));
   printf("BITS_ZERO_WINDOW(1,2) = %u\n", BITS_ZERO_WINDOW(uint32_t,1,2));

   printf("BITS_SET_WINDOW_ZERO(10,0,0) = %u\n", BITS_SET_WINDOW_ZERO(uint32_t,10,0,0));
   printf("BITS_SET_WINDOW_ZERO(10,0,1) = %u\n", BITS_SET_WINDOW_ZERO(uint32_t,10,0,1));
   printf("BITS_SET_WINDOW_ZERO(10,1,1) = %u\n", BITS_SET_WINDOW_ZERO(uint32_t,10,1,1));

   printf("BITS_ALIGN_WINDOW(5,0,3) = %u\n", BITS_ALIGN_WINDOW(uint32_t,5,0,3));
   printf("BITS_ALIGN_WINDOW(5,0,2) = %u\n", BITS_ALIGN_WINDOW(uint32_t,5,0,2));
   printf("BITS_ALIGN_WINDOW(5,1,3) = %u\n", BITS_ALIGN_WINDOW(uint32_t,5,1,3));

   printf("BITS_SET_WINDOW(10,0,0,1) = %u\n", BITS_SET_WINDOW(uint32_t,10,0,0,1));
   printf("BITS_SET_WINDOW(10,0,1,1) = %u\n", BITS_SET_WINDOW(uint32_t,10,0,1,1));
   printf("BITS_SET_WINDOW(10,1,1,1) = %u\n", BITS_SET_WINDOW(uint32_t,10,1,1,1));
   printf("BITS_SET_WINDOW(10,2,1,1) = %u\n", BITS_SET_WINDOW(uint32_t,10,2,1,1));

   printf("BITS_SET_WINDOW(0,0,8,1) = %u\n", BITS_SET_WINDOW(uint32_t,0,0,8,1));
   printf("BITS_SET_WINDOW(0,8,8,1) = %u\n", BITS_SET_WINDOW(uint32_t,0,8,8,1));
   printf("BITS_SET_WINDOW(0,16,8,1) = %u\n", BITS_SET_WINDOW(uint32_t,0,16,8,1));
   printf("BITS_SET_WINDOW(0,24,8,1) = %u\n", BITS_SET_WINDOW(uint32_t,0,24,8,1));

   printf("BITS_LOWER_ONES(32) = %u\n", BITS_LOWER_ONES(uint32_t,32));
   printf("BITS_UPPER_ONES(32) = %u\n", BITS_UPPER_ONES(uint32_t,32));

   printf("BITS_SET_WINDOW(0,0,32,1) = %u\n", BITS_SET_WINDOW(uint32_t,0,0,32,1));

   int64_t value = 0;

   value = 1;
   value <<= 62;
   printf("value = %lld\n", value);


   value = BITS_SET_WINDOW(int64_t,value,0,16,11);
   value = BITS_SET_WINDOW(int64_t,value,16,16,12);
   value = BITS_SET_WINDOW(int64_t,value,32,16,13);
   value = BITS_SET_WINDOW(int64_t,value,48,16,14);

   printf("value = (%d,%d,%d,%d)\n",
         (int)BITS_GET_WINDOW(int64_t,value,0,16),
         (int)BITS_GET_WINDOW(int64_t,value,16,16),
         (int)BITS_GET_WINDOW(int64_t,value,32,16),
         (int)BITS_GET_WINDOW(int64_t,value,48,16));

   return 0;
}
