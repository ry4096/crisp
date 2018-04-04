#ifndef ERROR_H_
#define ERROR_H_

typedef void (*error_print_fn)(FILE* output, void* self);
typedef void (*error_free_fn)(void* self);

struct error_vtable {
   error_print_fn error_print;
   error_free_fn error_free;
};

typedef struct error_vtable** error;


#endif
