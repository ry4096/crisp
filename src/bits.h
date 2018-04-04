#ifndef BITS_H_
#define BITS_H_


// Lower digits are ones. Upper digits are zeros.
// Ex: LOWER_ONES(4) = 00001111
#define BITS_LOWER_ONES(type, num_ones) \
   ((((type)1) << (num_ones)) - 1)

// Upper digits are ones. Lower digits are zeros.
// Ex: UPPER_ONES(4) = 11110000
#define BITS_UPPER_ONES(type, num_zeros) \
   (~BITS_LOWER_ONES(type, num_zeros))

// A window of width and offset are zeros. The digits before and after the window are zeros.
// Ex: ZERO_WINDOW(2,3) = 11100011
#define BITS_ZERO_WINDOW(type,offset,width) \
  ((BITS_UPPER_ONES(type, width) << offset) | BITS_LOWER_ONES(type, offset))


// Returns value with the bits inside the window set to zero.
#define BITS_SET_WINDOW_ZERO(type,value,offset,width) \
   ((value) & BITS_ZERO_WINDOW(type,offset,width))

// Clips value by width, and shifts by offset.
#define BITS_ALIGN_WINDOW(type,value,offset,width) \
   (((value) & BITS_LOWER_ONES(type,width)) << offset)

// Returns value1 with the bits inside the window set to value2.
#define BITS_SET_WINDOW(type,value1,offset,width,value2) \
   (BITS_SET_WINDOW_ZERO(type,value1,offset,width) | BITS_ALIGN_WINDOW(type,value2,offset,width))

// Extracts the bits inside the window of value.
#define BITS_GET_WINDOW(type,value,offset,width) \
   (((value) >> offset) & BITS_LOWER_ONES(type,width))

#endif
