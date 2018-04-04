#!../crisp

# code-unsafe and begin- test.


(load (str ../lib/malloc.cr))

(def fibs-str heap (str heap "fibs: "))
(def st1 heap (str heap "Main branch\n"))
(def st2 heap (str heap "n=1 branch\n"))
(def st3 heap (str heap "n=0 branch\n"))
(def st4 heap (str heap "Fibs return\n"))
(def st5 heap (str heap "Fibs("))
(def st6 heap (str heap ") = "))

# n
(def (n int) stack)
(def (ret int) stack)
(def fibs heap
 (code-unsafe
  (: void
   (begin-
    #(print fibs-str n (char "\n"))
    (if n
     (if (- n 1)
      (begin-
       #(print st1)
       (def n stack (- n 1))
       (fibs)
       (undef n stack)
       (def fibs1-ret heap ret)

       (def n stack (- n 2))
       (fibs)
       (undef n stack)
       (set ret (+ ret fibs1-ret))
       (undef fibs1-ret heap)
       #(print st4)
       )

      (begin-
       #(print st2)
       (set ret 1)))
     (begin-
      #(print st3)
      (set ret 0)))))))
(undef ret stack)
(undef n stack)

(def (n int) stack)
(def print-fibs heap
 (code-unsafe
  (: void
   (begin-
    (def (ret int) stack)
    (fibs)
    (print st5 n st6 ret (char "\n"))
    (undef ret stack)))))
(undef n stack)


(def n stack 0) (: void (print-fibs)) (undef n stack)
(def n stack 1) (: void (print-fibs)) (undef n stack)
(def n stack 2) (: void (print-fibs)) (undef n stack)
(def n stack 3) (: void (print-fibs)) (undef n stack)
(def n stack 4) (: void (print-fibs)) (undef n stack)
(def n stack 5) (: void (print-fibs)) (undef n stack)
(def n stack 6) (: void (print-fibs)) (undef n stack)
(def n stack 7) (: void (print-fibs)) (undef n stack)
(def n stack 8) (: void (print-fibs)) (undef n stack)
(def n stack 9) (: void (print-fibs)) (undef n stack)
(def n stack 10) (: void (print-fibs)) (undef n stack)
(def n stack 11) (: void (print-fibs)) (undef n stack)
(def n stack 12) (: void (print-fibs)) (undef n stack)
(def n stack 13) (: void (print-fibs)) (undef n stack)
(def n stack 14) (: void (print-fibs)) (undef n stack)
(def n stack 15) (: void (print-fibs)) (undef n stack)
(def n stack 16) (: void (print-fibs)) (undef n stack)
(def n stack 17) (: void (print-fibs)) (undef n stack)
(def n stack 18) (: void (print-fibs)) (undef n stack)
(def n stack 19) (: void (print-fibs)) (undef n stack)
(def n stack 20) (: void (print-fibs)) (undef n stack)




(c.free fibs-str)
(c.free st1)
(c.free st2)
(c.free st3)
(c.free st4)
(c.free st5)
(c.free st6)
(undef fibs-str heap)
(undef st1 heap)
(undef st2 heap)
(undef st3 heap)
(undef st4 heap)
(undef st5 heap)
(undef st6 heap)
//(free-quote fibs)
//(free-quote print-fibs)
(undef fibs heap)
(undef print-fibs heap)
