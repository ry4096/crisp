#!../crisp

(def a stack 45)

(while (> a 0)
  (print (str "a = ") a (str "\n"))
  (set a (- a 1)))

