#!../crisp

(import. (.. lib malloc))

(use-prefix c.)

(def a stack (cast (auto* int) (malloc (sizeof-type int))))
(set (a) 4)

(def b stack a)

(print
 (str "a = ") (a) (str " = ") a (str "\n")
 (str "b = ") (b) (str " = ") b (str "\n")
)

(print (str "eval (set (a) 5)\n"))
(set (a) 5)

(print
 (str "a = ") (a) (str " = ") a (str "\n")
 (str "b = ") (b) (str " = ") b (str "\n")
)


(def-type A
 (struct align32
  (s (auto* int))
  (t (auto* int))))

(def obj stack
 (struct A
  (s a)
  (t b)))

(print (str "obj = ") obj (str "\n"))

