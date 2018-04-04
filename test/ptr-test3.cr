#!../crisp

(def a stack (cast str-refc (str refc "Hello World!")))

(def b stack a)

(print
 (str "a = ") a (str " = ") (cast * a) (str "\n")
 (str "b = ") b (str " = ") (cast * b) (str "\n")
)

(def-type A
 (struct align32
  (s str-refc)
  (t str-refc)))

(def obj stack
 (struct A
  (s a)
  (t b)))

(print (str "obj = ") obj (str "\n"))

