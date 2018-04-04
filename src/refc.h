#ifndef REFC_H_
#define REFC_H_

//#define REFC_SAFETY_STAMP 1337


// REFC_OFFSET is the number of ints stored before the data.
#ifdef REFC_SAFETY_STAMP
#define REFC_OFFSET 2
#else
#define REFC_OFFSET 1
#endif


#define REFC_SIZE(data_size) \
   ((data_size) + sizeof(int)*REFC_OFFSET)


#define REFC_ALLOC_TO_HANDLE(alloc) \
   ((void*)(((int*)alloc) + REFC_OFFSET))


#define REFC_HANDLE_TO_ALLOC(handle) \
   ((void*)(((int*)handle) - REFC_OFFSET))


#define REFC_HANDLE_TO_COUNT(handle) \
   ((void*)(((int*)handle) - 1))



void* refc_alloc(size_t size);
char* refc_str(char* buff);

#define REFC_MACROS


int refc_inc(void* handle);
int refc_dec(void* handle);
int refc_count(void* handle);
void refc_free(void* handle);
int refc_dec_free(void* handle);
void* refc_copy(void* handle);

#ifdef REFC_MACROS

#define refc_inc(handle) \
   (++*(int*)REFC_HANDLE_TO_COUNT(handle))

#define refc_dec(handle) \
   (--*(int*)REFC_HANDLE_TO_COUNT(handle))

#define refc_count(handle) \
   (*(int*)REFC_HANDLE_TO_COUNT(handle))

#define refc_free(handle) \
   free(REFC_HANDLE_TO_ALLOC(handle))

#define refc_dec_free(handle) \
   if (!refc_dec(handle)) \
      refc_free(handle);

#define refc_copy(handle) \
   (refc_inc(handle), handle)

#endif

#ifdef REFC_SAFETY_STAMP
int refc_check_stamp(void* handle);
#endif



#endif
