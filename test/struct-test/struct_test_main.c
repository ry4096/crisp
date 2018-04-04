#include <stdio.h>
#include <stddef.h>

#include "struct_test.h"


#define A_SLOTS(M) \
   M(struct A, a, char, "a", "char") \
   M(struct A, b, int, "b", "int") \
   M(struct A, c, float, "c", "float") \
   M(struct A, d, long double, "d", "long double") \
   M(struct A, e, char, "e", "char")

#define B_SLOTS(M) \
   M(struct B, a, short, "a", "short") \
   M(struct B, b, char, "b", "char") \
   M(struct B, d, char, "d", "char") \
   M(struct B, e, char, "e", "char")

#define B2_SLOTS(M) \
   M(struct B2, a, short, "a", "short") \
   M(struct B2, b, char, "b", "char") \
   M(struct B2, d, char, "d", "char") \
   M(struct B2, e, char, "e", "char")


#define C_SLOTS(M) \
   M(struct C, a, long long, "a", "long long") \
   M(struct C, b, char, "b", "char") \
   M(struct C, c, long, "c", "long") \
   M(struct C, d, char, "d", "char") \
   M(struct C, e, long long, "e", "long long")

#define D_SLOTS(M) \
   M(struct D, a, char, "a", "char") \
   M(struct D, b, struct C, "b", "struct C")

#define E_SLOTS(M) \
   M(struct E, a, char, "a", "char") \
   M(struct E, b, long long, "b", "long long")


#define K_SLOTS(M) \
   M(struct K, a, int32_t, "a", "int32_t") \
   M(struct K, b, int64_t, "b", "int64_t")


#define PRINT_SLOT(struct_type, field, field_type, field_str, field_type_str) \
   printf("%s : %s, size=%d, offset=%d\n", \
         field_str, field_type_str, \
         sizeof(field_type), offsetof(struct_type, field));



int main(int argc, char** argv) {

   struct A a;
   a.a = 'a';
   a.b = 2;
   a.c = 3.0;
   a.d = 4.0;
   a.e = 'e';

   struct B b;
   b.a = 1;
   b.b = 'b';
   b.c1 = 13;
   b.c2 = 23;
   b.c3 = 33;
   b.c4 = 43;
   b.d = 'd';
   b.e = 'e';

   struct B2 b2;
   b2.a = 1;
   b2.b = 'b';
   b2.c1 = 13;
   b2.c2 = 23;
   b2.c3 = 33;
   b2.c4 = 43;
   b2.d = 'd';
   b2.e = 'e';

   struct C c;
   c.a = 1;
   c.b = 'b';
   c.c = 3;
   c.d = 'd';
   c.e = 5;

   struct D d;
   d.a = 'a';
   d.b.a = 1;
   d.b.b = 'b';
   d.b.c = 3;
   d.b.d = 'd';
   d.b.e = 5;

   struct E e;
   e.a = 'a';
   e.b = 2;

   struct F f;
   f.a = 1;
   f.b = 2;
   f.c = 3;
   f.d = 4;


   struct G g;
   g.a = 1;
   g.b = 2;
   g.c = 3;
   g.d = 4;
   g.e = 5;
   g.f = 6;
   g.g = 7;
   g.h = 8;


   struct H h;
   h.a = 1;
   h.b = 2;
   h.c = 3;
   h.d = 4;

   struct I i;
   i.a = 1;
   i.b = 2;
   i.c = 3;
   i.d = 4;

   struct J j;
   j.a = 1;
   j.b = 2;
   j.c = 3;
   j.d = 4;

   struct K k;
   k.a = 1;
   k.b = 2;

   struct L l;
   l.a = 1;
   l.b = 2;
   l.c = 3;
   l.d = 4;
   l.e = 5;
   l.f = 6;
   l.g = 7;
   l.h = 8;



   printf("a = "); A_print(&a); printf("\n");
   printf("b = "); B_print(&b); printf("\n");
   printf("b2 = "); B2_print(&b2); printf("\n");
   printf("c = "); C_print(&c); printf("\n");
   printf("d = "); D_print(&d); printf("\n");
   printf("e = "); E_print(&e); printf("\n");
   printf("f = "); F_print(&f); printf("\n");
   printf("g = "); G_print(&g); printf("\n");
   printf("h = "); H_print(&h); printf("\n");
   printf("i = "); I_print(&i); printf("\n");
   printf("j = "); J_print(&j); printf("\n");
   printf("k = "); K_print(&k); printf("\n");
   printf("l = "); L_print(&l); printf("\n");

   int8_t* f_data = (int8_t*)&f;
   printf("f_data = (%d,%d,%d,%d)\n",
         (int)f_data[0], (int)f_data[1], (int)f_data[2], (int)f_data[3]);

   int8_t* g_data = (int8_t*)&g;
   printf("g_data = (%d,%d,%d,%d,%d,%d,%d,%d)\n",
         (int)g_data[0], (int)g_data[1], (int)g_data[2], (int)g_data[3],
         (int)g_data[4], (int)g_data[5], (int)g_data[6], (int)g_data[7]);

   int16_t* h_data = (int16_t*)&f;
   printf("h_data = (%d,%d,%d,%d)\n",
         (int)h_data[0], (int)h_data[1], (int)h_data[2], (int)h_data[3]);

   int16_t* i_data = (int16_t*)&i;
   printf("i_data = (%d,%d,%d,%d)\n",
         (int)i_data[0], (int)i_data[1], (int)i_data[2], (int)i_data[3]);


   printf("struct A: %d\n", sizeof(struct A));
   A_SLOTS(PRINT_SLOT)

   printf("struct B: %d\n", sizeof(struct B));
   B_SLOTS(PRINT_SLOT)

   printf("struct B2: %d\n", sizeof(struct B2));
   B2_SLOTS(PRINT_SLOT)

   printf("struct C: %d\n", sizeof(struct C));
   C_SLOTS(PRINT_SLOT)

   printf("struct D: %d\n", sizeof(struct D));
   D_SLOTS(PRINT_SLOT)

   printf("struct E: %d\n", sizeof(struct E));
   E_SLOTS(PRINT_SLOT)

   printf("struct F: %d\n", sizeof(struct F));
   printf("struct G: %d\n", sizeof(struct G));
   printf("struct H: %d\n", sizeof(struct H));
   printf("struct I: %d\n", sizeof(struct I));
   printf("struct J: %d\n", sizeof(struct J));

   printf("struct K: %d\n", sizeof(struct K));
   K_SLOTS(PRINT_SLOT)

   printf("struct L: %d\n", sizeof(struct L));

   return 0;
}
