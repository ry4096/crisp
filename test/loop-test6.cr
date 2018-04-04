#!../crisp

(load (str ../lib/stdio.cr))

(def a stack 45)

(while (> a 0)
  (c.printf (cast str (str "a = %d\n")) a)
  (set a (- a 1)))

