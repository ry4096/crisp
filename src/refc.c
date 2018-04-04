#include <malloc.h>
#include <assert.h>
#include <string.h>

#include "refc.h"

#define LOGGING_OFF

#include "log.h"



void* refc_alloc(size_t size) {
   int* alloc = (int*)malloc(REFC_SIZE(size));
   assert(alloc);
   if (alloc) {
#ifdef REFC_SAFETY_STAMP
      alloc[0] = REFC_SAFETY_STAMP;
      alloc[1] = 1;
#else
      alloc[0] = 1;
#endif
      void* handle = REFC_ALLOC_TO_HANDLE(alloc);
      LOG("refc_alloc: %p, %d\n", handle, size)
      return handle;
   }
   else {
      return NULL;
   }
}


char* refc_str(char* buff) {
   int len = strlen(buff);
   char* alloc = (char*)refc_alloc(len + 1);
   assert(alloc);
   if (alloc) {
      memcpy(alloc, buff, len);
      alloc[len] = '\0';
      return alloc;
   }
   else {
      return NULL;
   }
}


#ifdef REFC_MACROS
#undef refc_inc
#undef refc_dec
#undef refc_count
#undef refc_free
#undef refc_dec_free
#undef refc_copy
#endif

int refc_inc(void* handle) {
   int* ptr = REFC_HANDLE_TO_COUNT(handle);
   int ret = ++*ptr;
   LOG("refc_inc: %p, %d\n", handle, ret)
   return ret;
}

int refc_dec(void* handle) {
   int* ptr = REFC_HANDLE_TO_COUNT(handle);
   assert(*ptr > 0);
   int ret = --*ptr;
   LOG("refc_dec: %p, %d\n", handle, ret)
   return ret;
}


int refc_count(void* handle) {
   int* ptr = REFC_HANDLE_TO_COUNT(handle);
   return *ptr;
}

void refc_free(void* handle) {
   void* alloc = REFC_HANDLE_TO_ALLOC(handle);
   int* ptr = REFC_HANDLE_TO_COUNT(handle);
   LOG("refc_free: %p, %d\n", handle, *ptr)
   assert(*ptr == 0);
   free(alloc);
}

int refc_dec_free(void* handle) {
   if (refc_dec(handle) == 0) {
      refc_free(handle);
   }
}

void* refc_copy(void* handle) {
   refc_inc(handle);
   return handle;
}



#ifdef REFC_SAFETY_STAMP

int refc_check_stamp(void* handle) {
   int* alloc = REFC_HANDLE_TO_ALLOC(handle);
   return *alloc == REFC_SAFETY_STAMP;
}

#endif



