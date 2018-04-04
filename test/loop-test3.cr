#!../crisp

(def a stack -1)

(do-while (and (>= a 0) (< a 10))
 (print (str "a = ") a (str "\n"))
 (set a (+ a 1)))

