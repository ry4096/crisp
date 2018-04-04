#!../crisp

(def-type A
 (struct align32
  (i int)
  (short1 int32 16)
  (short2 int32 16)))

(def make-a stack
 (function ((n int)) A
  (struct A
   (i n)
   (short1 n)
   (short2 n))))

(print (str "(make-a 10) = ") (make-a 10) (str "\n"))
(print (str "((make-a 10) i) = ") ((make-a 10) i) (str "\n"))

