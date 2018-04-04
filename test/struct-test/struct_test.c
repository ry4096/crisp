#include <stdio.h>

#include "struct_test.h"




void A_print(struct A* self) {
   printf("{(a %c) (b %d) (c %f) (d %llf) (e %c)}",
         self->a, self->b, self->c, self->d, self->e);
}

void B_print(struct B* self) {
   printf("{(a %hd) (b %c) (c1 %u) (c2 %u) (c3 %u) (c4 %u) (d %c) (e %c)}",
         self->a, self->b,
         (int)self->c1, (int)self->c2, (int)self->c3, (int)self->c4,
         self->d, self->e);
}

void B2_print(struct B2* self) {
   printf("{(a %hd) (b %c) (c1 %u) (c2 %u) (c3 %u) (c4 %u) (d %c) (e %c)}",
         self->a, self->b,
         (int)self->c1, (int)self->c2, (int)self->c3, (int)self->c4,
         self->d, self->e);
}



void C_print(struct C* self) {
   printf("{(a %lld) (b %c) (c %ld) (d %c) (e %lld)}",
         self->a, self->b, self->c, self->d, self->e);
}

void D_print(struct D* self) {
   printf("{(a %c) (b ", self->a);
   C_print(&self->b);
   printf(")}");
}

void E_print(struct E* self) {
   printf("{(a %c) (b %lld)}", self->a, self->b);
}

void F_print(struct F* self) {
   printf("{(a %u) (b %u) (c %u) (d %u)}",
         self->a, self->b, self->c, self->d);
}

void G_print(struct G* self) {
   printf("{(a %u) (b %u) (c %u) (d %u) (e %u) (f %u) (g %u) (h %u)}",
         self->a, self->b, self->c, self->d,
         self->e, self->f, self->g, self->h);
}

void H_print(struct H* self) {
   printf("{(a %u) (b %u) (c %u) (d %u)}",
         self->a, self->b, self->c, self->d);
}

void I_print(struct I* self) {
   printf("{(a %u) (b %u) (c %u) (d %u)}",
         self->a, self->b, self->c, self->d);
}

void J_print(struct J* self) {
   printf("{(a %u) (b %u) (c %u) (d %u)}",
         self->a, self->b, self->c, self->d);
}

void K_print(struct K* self) {
   printf("{(a %u) (b %llu)",
         self->a, self->b);
}

void L_print(struct L* self) {
   printf("{(a %u) (b %u) (c %u) (d %u) (e %u) (f %u) (g %u) (h %u)}",
         (int)self->a, (int)self->b, (int)self->c, (int)self->d,
         (int)self->e, (int)self->f, (int)self->g, (int)self->h);
}



