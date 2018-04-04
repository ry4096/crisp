#!../crisp

(def show-expr stack
 (macro (expr)
  (`
   (print
    (' (, expr)) (str " => ") (, expr)
    (str " : ") (typeof (, expr)) (str "\n")))))


// Use this if the expr affects the stack and is incompatible with let.
(def show-void-expr stack
 (macro (expr)
  (`
   (begin
    (, expr)
    (print
     (' (, expr)) (str " : ") (typeof (, expr)) (str "\n"))))))

(show-void-expr (def a stack (str refc "Hello World")))
(show-void-expr (print a))
(show-expr (type str))
(show-expr (type (array 5 str)))
(show-expr a)
(show-expr (a 2))
(show-expr (typeof (a 2)))

(show-expr
 (def-type A
  (struct align32
   (a int)
   (b char)
   (c float)
   (d str-refc))))

(show-void-expr
 (def b stack
  (struct A
   (a 1)
   (b (char b))
   (c 3.0)
   (d a))))

(show-expr b)
(show-expr (b a))
(show-expr (b d))
(show-expr (. b d))
(show-expr (& (. b d)))
(show-expr (& (. b c)))
(show-expr (typeof (& (. b c))))
(show-expr (typeof (. b c)))
(show-expr (typeof b))
(show-expr (type A))


(def-type A2 A)

(show-void-expr
 (def c stack
  (struct A2
   (a 1)
   (b (char b))
   (c 3.0)
   (d a))))

(show-expr c)
(show-expr (= b c))
(show-expr (= (typeof b) (typeof c)))
(show-expr (type A2))
(show-expr (= (type A) (type A2)))
(show-expr (type.size (typeof c)))
(show-expr (type.size (typeof (c d))))


