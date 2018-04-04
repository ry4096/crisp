#!../crisp


(def (local1 int) heap)
(def (local2 double) heap)

(def f stack
 (function ((n int) (v double)) double
  (begin
   (def local1 stack (+ n 2))
   (def local2 stack (to-double n))
   (set v (+ v local2 (to-double local1)))
   (undef local2 stack)
   (undef local1 stack)
   v)))

(undef local2 heap)
(undef local1 heap)

(print (f 2 3))
