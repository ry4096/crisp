#!../crisp

(def-type A
 (struct align32
  (a char 2)
  (b char 2)
  (c char 2)
  (d char 2)
  (e char 2)))

(def a stack (struct A (a 1) (b 2) (c 3) (d 4) (e 5)))

(print (str "(a a) = ") (to-int (a a)) (str "\n"))
(print (str "(a b) = ") (to-int (a b)) (str "\n"))
(print (str "(a c) = ") (to-int (a c)) (str "\n"))
(print (str "(a d) = ") (to-int (a d)) (str "\n"))
(print (str "(a e) = ") (to-int (a e)) (str "\n"))

(print (str "a = ") a (str "\n"))

(set-slot a a 11)
(set-slot a b 22)
(set-slot a c 23)

(print (str "a = ") a (str "\n"))
