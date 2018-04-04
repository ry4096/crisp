#!../crisp

(import. (.. lib crisp refc))

(use-prefix crisp.)

(def a stack (cast (refc* int) (refc.alloc (sizeof-type int))))
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
  (s (refc* int))
  (t (refc* int))))

(def obj stack
 (struct A
  (s a)
  (t b)))

(print (str "obj = ") obj (str "\n"))

