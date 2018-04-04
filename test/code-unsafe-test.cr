#!../crisp

(def n stack 5)

/*
(def a stack
 (code-unsafe
  (if (= n 0)
   (print (str "done\n"))
   (: void
    (begin-
     (print (str "step ") n (str "\n"))
     (set n (- n 1))
     (: void (a)))))))

(: void (a))
*/

/*
(def b stack
 (code-unsafe
  (print (str "n = ") n (str "\n"))))

(b)
*/

/*
(def (c (code-unsafe void)) stack)
(set c
 (code-unsafe
  (if (= n 0)
   (print (str "done\n"))
   (begin
    (print (str "n = ") n (str "\n"))
    (set n (- n 1))
    (c)))))
(c)
*/

(def d stack
 (code-unsafe
  (if (= n 0)
   (print (str "done\n"))
   (: void
    (begin- 
     (print (str "n = ") n (str "\n"))
     (set n (- n 1))
     (d))))))
(d)

