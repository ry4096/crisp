#!../crisp


(def-type bonz int)

(def (a bonz) stack 34)

(def g stack
 (function ((b bonz)) void
  (print (str "a: ") b (str "\n"))))

(g a)


(def-type B
 (struct align32
  (value int)))

(def b stack (struct B (value 1)))

(print (str "b: ") b (str "\n"))


(def-type C
 (struct align32
  (value bonz)))

(def c stack
 (struct C
  (value 23)))

(print (str "c: ") c (str "\n"))


(def-type D)
(set-type D
 (struct align32
  (value int)
  (next (* D))))

(def d stack
 (struct D
  (value 123)
  (next (cast (* D) 0))))

(print (str "d: ") d (str "\n"))



/*
(undef-type D)
(undef-type C)
(undef-type B)
(undef-type bonz)
*/

//(free-quote g)

