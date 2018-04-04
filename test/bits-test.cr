#!../crisp

(def-type A
 (struct align32
  (a int32 8)
  (b int32 8)
  (c int32 8)
  (d int32 8)
  (e int32 8)))

(def a stack (struct A (a 1) (b 2) (c 3) (d 4) (e 5)))

(print (str "(a a) = ") (a a) (str "\n"))
(print (str "(a b) = ") (a b) (str "\n"))
(print (str "(a c) = ") (a c) (str "\n"))
(print (str "(a d) = ") (a d) (str "\n"))
(print (str "(a e) = ") (a e) (str "\n"))

(print (str "a = ") a (str "\n"))

(set-slot a a 11)
(set-slot a b 22)
(set-slot a c 23)

(print (str "a = ") a (str "\n"))
