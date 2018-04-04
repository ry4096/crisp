#!../crisp

(def n stack 5)
(def a stack
 (quote
// (code
  (if (= n 0)
   (print (str "done\n"))
   (: void
    (begin-
     (print (str "step ") n (str "\n"))
     (set n (- n 1))
     (: void (a)))))))

(: void (a))
