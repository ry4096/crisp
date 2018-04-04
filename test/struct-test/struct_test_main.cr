#!../../crisp

(import. struct_test)


(def offsetof stack
 (macro (type field)
  (quasiquote
   (to-int
    (& (((: (* (unquote type)) 0)) (unquote field)))))))

(def A_SLOTS stack
 (macro (M)
  (`
   (begin
    ((, M) A a char)
    ((, M) A b int)
    ((, M) A c float)
    ((, M) A d longdouble)
    ((, M) A e char)))))

(def B_SLOTS stack
 (macro (M)
  (`
   (begin
    ((, M) B a short)
    ((, M) B b char)
    ((, M) B d char)
    ((, M) B e char)))))

(def B2_SLOTS stack
 (macro (M)
  (`
   (begin
    ((, M) B2 a short)
    ((, M) B2 b char)
    ((, M) B2 d char)
    ((, M) B2 e char)))))

(def C_SLOTS stack
 (macro (M)
  (`
   (begin
    ((, M) C a longlong)
    ((, M) C b char)
    ((, M) C c long)
    ((, M) C d char)
    ((, M) C e longlong)))))

(def D_SLOTS stack
 (macro (M)
  (`
   (begin
    ((, M) D a char)
    ((, M) D b C)))))

(def E_SLOTS stack
 (macro (M)
  (`
   (begin
    ((, M) E a char)
    ((, M) E b longlong)))))

(def K_SLOTS stack
 (macro (M)
  (`
   (begin
    ((, M) K a int32)
    ((, M) K b int64)))))

(def PRINT_SLOT_OFFSET stack
 (macro (struct_type field field_type)
  (`
   (print
    (' (, field)) (str " : ") (' (, field_type))
    (str ", size=") (sizeof-type (, field_type))
    (str ", offset=") (offsetof (, struct_type) (, field))
    (str "\n")
   )
  )
 )
)



(use-prefix struct_test.)

(def a stack
 (struct A
  (a (char a))
  (b 2)
  (c 3.0)
  (d 4.0)
  (e (char e))))

(def b stack
 (struct B
  (a 1)
  (b (char b))
  (c1 13)
  (c2 23)
  (c3 33)
  (c4 43)
  (d (char d))
  (e (char e))))

(def b2 stack
 (struct B2
  (a 1)
  (b (char b))
  (c1 13)
  (c2 23)
  (c3 33)
  (c4 43)
  (d (char d))
  (e (char e))))

(def c stack
 (struct C
  (a 1)
  (b (char b))
  (c 3)
  (d (char d))
  (e 5)))

(def d stack
 (struct D
  (a (char a))
  (b
   (struct C
    (a 1)
    (b (char b))
    (c 3)
    (d (char d))
    (e 5)))))

(def e stack
 (struct E
  (a (char a))
  (b 1)))

(def f stack
 (struct F
  (a 1)
  (b 2)
  (c 3)
  (d 4)))

(def g stack
 (struct G
  (a 1)
  (b 2)
  (c 3)
  (d 4)
  (e 5)
  (f 6)
  (g 7)
  (h 8)))

(def h stack
 (struct H
  (a 1)
  (b 2)
  (c 3)
  (d 4)))

(def i stack
 (struct I
  (a 1)
  (b 2)
  (c 3)
  (d 4)))

(def j stack
 (struct J
  (a 1)
  (b 2)
  (c 3)
  (d 4)))

(def k stack
 (struct K
  (a 1)
  (b 2)))

(def l stack
 (struct L
  (a 1)
  (b 2)
  (c 3)
  (d 4)
  (e 5)
  (f 6)
  (g 7)
  (h 8)))

(print (str "a = ")) (A.print (& a)) (print (str "\n"))
(print (str "b = ")) (B.print (& b)) (print (str "\n"))
(print (str "b2 = ")) (B2.print (& b2)) (print (str "\n"))
(print (str "c = ")) (C.print (& c)) (print (str "\n"))
(print (str "d = ")) (D.print (& d)) (print (str "\n"))
(print (str "e = ")) (E.print (& e)) (print (str "\n"))
(print (str "f = ")) (F.print (& f)) (print (str "\n"))
(print (str "g = ")) (G.print (& g)) (print (str "\n"))
(print (str "h = ")) (H.print (& h)) (print (str "\n"))
(print (str "i = ")) (I.print (& i)) (print (str "\n"))
(print (str "j = ")) (J.print (& j)) (print (str "\n"))
(print (str "k = ")) (K.print (& k)) (print (str "\n"))
(print (str "l = ")) (L.print (& l)) (print (str "\n"))

(print
 (str "=============\n")
 (str "a = ") a (str "\n")
 (str "b = ") b (str "\n")
 (str "b2 = ") b2 (str "\n")
 (str "c = ") c (str "\n")
 (str "d = ") d (str "\n")
 (str "e = ") e (str "\n")
 (str "f = ") f (str "\n")
 (str "g = ") g (str "\n")
 (str "h = ") h (str "\n")
 (str "i = ") i (str "\n")
 (str "j = ") j (str "\n")
 (str "k = ") k (str "\n")
 (str "l = ") l (str "\n")
 (str "=============\n")
)


(print (str "A: ") (sizeof-type A) (str "\n"))
(A_SLOTS PRINT_SLOT_OFFSET)

(print (str "B: ") (sizeof-type B) (str "\n"))
(B_SLOTS PRINT_SLOT_OFFSET)

(print (str "B2: ") (sizeof-type B2) (str "\n"))
(B2_SLOTS PRINT_SLOT_OFFSET)

(print (str "C: ") (sizeof-type C) (str "\n"))
(C_SLOTS PRINT_SLOT_OFFSET)

(print (str "D: ") (sizeof-type D) (str "\n"))
(D_SLOTS PRINT_SLOT_OFFSET)

(print (str "E: ") (sizeof-type E) (str "\n"))
(E_SLOTS PRINT_SLOT_OFFSET)

(print (str "F: ") (sizeof-type F) (str "\n"))
(print (str "G: ") (sizeof-type G) (str "\n"))
(print (str "H: ") (sizeof-type H) (str "\n"))
(print (str "I: ") (sizeof-type I) (str "\n"))
(print (str "J: ") (sizeof-type J) (str "\n"))

(print (str "K: ") (sizeof-type K) (str "\n"))
(K_SLOTS PRINT_SLOT_OFFSET)

(print (str "L: ") (sizeof-type L) (str "\n"))
