#!../crisp

(def-type A
 (struct align32
  (i int)
  (a int)
 )
)

(print (str "(sizeof-type A) = ") (sizeof-type A) (str "\n"))

(def make-a stack
 (function ((n int)) A
  (struct A
   (i n)
   (a n)
  )
 )
)

(print (str "(make-a 10) = ") (make-a 10) (str "\n"))
(print (str "((make-a 10) i) = ") ((make-a 10) i) (str "\n"))

