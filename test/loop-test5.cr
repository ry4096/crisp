#!../crisp

(def a stack 0)

(forever
 (if (not (< a 10))
   (break)
   (print))
 (print (str "a = ") a (str "\n"))
 (set a (+ a 1))
 (if (% a 2)
   (continue)
   (print))
 (print (str "bonkers\n")))

