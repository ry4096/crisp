#!../crisp

(def (a int) stack)

(for (set a 0) (< a 40) (set a (+ a 1))
 (print (str "a = ") a (str "\n")))

