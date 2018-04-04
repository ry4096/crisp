#ifndef SHIFT_H_
#define SHIFT_H_

//#define SHIFT_API __attribute__((cdecl))
//#define SHIFT_API _cdecl
#define SHIFT_API

void SHIFT_API call_with_buffer(void (*fn)(), void* buffer, int length);
char SHIFT_API call_with_buffer_char(char (*fn)(), void* buffer, int length);
short SHIFT_API call_with_buffer_short(short (*fn)(), void* buffer, int length);
int SHIFT_API call_with_buffer_int(int (*fn)(), void* buffer, int length);
long SHIFT_API call_with_buffer_long(long (*fn)(), void* buffer, int length);
long long SHIFT_API call_with_buffer_long_long(long long (*fn)(), void* buffer, int length);
float SHIFT_API call_with_buffer_float(float (*fn)(), void* buffer, int length);
double SHIFT_API call_with_buffer_double(double (*fn)(), void* buffer, int length);
long double SHIFT_API call_with_buffer_long_double(long double (*fn)(), void* buffer, int length);
void* SHIFT_API call_with_buffer_pointer(void* (*fn)(), void* buffer, int length);


char SHIFT_API call_fn_char(char (*fn)());
int SHIFT_API call_fn_int(int (*fn)());
float SHIFT_API call_fn_float(float (*fn)());
double SHIFT_API call_fn_double(double (*fn)());
void* SHIFT_API call_fn_pointer(void* (*fn)());
void SHIFT_API call_fn(void (*fn)());



#endif
