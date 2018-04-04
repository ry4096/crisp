#!../crisp

(def a stack (cast str-auto (str heap "Hello World!")))

(def b stack a)

(print
 (str "a = ") a (str " = ") (cast * a) (str "\n")
 (str "b = ") b (str " = ") (cast * b) (str "\n")
)

(def-type A
 (struct align32
  (s str-auto)
  (t str-auto)))

(def obj stack
 (struct A
  (s a)
  (t b)))

(print (str "obj = ") obj (str "\n"))

