#!../crisp

(def a stack
 (function ((self int) (other int)) void
  (print (str "self = ") self
         (str "\nother = ") other (str "\n"))))

(def b stack
 (function ((other int) (self int)) void
  (a other self)))

(a 1 2)
(b 1 2)

#(free-quote b)
#(free-quote a)
