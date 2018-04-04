(in-prefix struct_test.)

(def-type A
 (struct align32
  (a char)
  (b int)
  (c float)
  (d longdouble)
  (e char)))

(def-type B
 (struct align32
  (a short)
  (b char)
  (c1 uint64 16)
  (c2 uint64 16)
  (c3 uint64 16)
  (c4 uint64 16)
  (d char)
  (e char)))

(def-type B2
 (struct align32
  (a short)
  (b char)
  (c1 uint32 16)
  (c2 uint32 16)
  (c3 uint32 16)
  (c4 uint32 16)
  (d char)
  (e char)))

(def-type C
 (struct align32
  (a longlong)
  (b char)
  (c long)
  (d char)
  (e longlong)))

(def-type D
 (struct align32
  (a char)
  (b C)))

(def-type E
 (struct align32
  (a char)
  (b longlong)))

(def-type F
 (struct align32
  (a int32 8)
  (b int32 8)
  (c int32 8)
  (d int32 8)))

(def-type G
 (struct align32
  (a int64 8)
  (b int64 8)
  (c int64 8)
  (d int64 8)
  (e int64 8)
  (f int64 8)
  (g int64 8)
  (h int64 8)))

(def-type H
 (struct align32
  (a int64 16)
  (b int64 16)
  (c int64 16)
  (d int64 16)))

(def-type I
 (struct align32
  (a int32 16)
  (b int32 16)
  (c int32 16)
  (d int32 16)))

(def-type J
 (struct align32
  (a int64 13)
  (b int64 13)
  (c int64 13)
  (d int64 13)))

(def-type K
 (struct align32
  (a int32)
  (b int64)))


(def-type L
 (struct align32
  (a uint64 1)
  (b uint64 9)
  (c uint64 3)
  (d uint64 16)
  (e uint64 17)
  (f uint64 3)
  (g uint64 6)
  (h uint64 9)))

(def handle stack
 (dl.open (cast str (str ./libstruct_test.so)) 1))

(dl.def-group handle
 (A.print A_print (c-fun ((* A)) void))
 (B.print B_print (c-fun ((* B)) void))
 (B2.print B2_print (c-fun ((* B2)) void))
 (C.print C_print (c-fun ((* C)) void))
 (D.print D_print (c-fun ((* D)) void))
 (E.print E_print (c-fun ((* E)) void))
 (F.print F_print (c-fun ((* F)) void))
 (G.print G_print (c-fun ((* G)) void))
 (H.print H_print (c-fun ((* H)) void))
 (I.print I_print (c-fun ((* I)) void))
 (J.print J_print (c-fun ((* J)) void))
 (K.print K_print (c-fun ((* K)) void))
 (L.print L_print (c-fun ((* L)) void))
)

(clear-prefixes)
