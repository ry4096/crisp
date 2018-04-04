#ifndef STRUCT_TEST_H_
#define STRUCT_TEST_H_

#include <stdint.h>


// Tests compatible alignment of struct fields.

struct A {
   char a;
   int b;
   float c;
   long double d;
   char e;
};

struct B {
   short a;
   char b;
   uint64_t c1 :16;
   uint64_t c2 :16;
   uint64_t c3 :16;
   uint64_t c4 :16;
   char d;
   char e;
};

struct B2 {
   short a;
   char b;
   uint32_t c1 :16;
   uint32_t c2 :16;
   uint32_t c3 :16;
   uint32_t c4 :16;
   char d;
   char e;
};

struct C {
   long long a;
   char b;
   long c;
   char d;
   long long e;
};

struct D {
   char a;
   struct C b;
};

struct E {
   char a;
   long long b;
};

struct F {
   int32_t a : 8;
   int32_t b : 8;
   int32_t c : 8;
   int32_t d : 8;
};

struct G {
   int64_t a : 8;
   int64_t b : 8;
   int64_t c : 8;
   int64_t d : 8;
   int64_t e : 8;
   int64_t f : 8;
   int64_t g : 8;
   int64_t h : 8;
};

struct H {
   int64_t a : 16;
   int64_t b : 16;
   int64_t c : 16;
   int64_t d : 16;
};

struct I {
   int32_t a : 16;
   int32_t b : 16;
   int32_t c : 16;
   int32_t d : 16;
};

struct J {
   int64_t a : 13;
   int64_t b : 13;
   int64_t c : 13;
   int64_t d : 13;
};

struct K {
   int32_t a;
   int64_t b;
};

struct L {
   uint64_t a : 1;
   uint64_t b : 9;
   uint64_t c : 3;
   uint64_t d : 16;
   uint64_t e : 17;
   uint64_t f : 3;
   uint64_t g : 6;
   uint64_t h : 9;
};





void A_print(struct A* self);
void B_print(struct B* self);
void B2_print(struct B2* self);
void C_print(struct C* self);
void D_print(struct D* self);
void E_print(struct E* self);
void F_print(struct F* self);
void G_print(struct G* self);
void H_print(struct H* self);
void I_print(struct I* self);
void J_print(struct J* self);
void K_print(struct K* self);
void L_print(struct L* self);


#endif
