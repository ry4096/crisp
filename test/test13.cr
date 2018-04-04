#!../crisp

# Tests the code-unsafe type.
# code and code-unsafe are chunks of unevaluated code.
# code is type checked in its context every time it is called.
# code-unsafe is typed checked when it is defined
# and it is assumed all dynamically bound values
# will have the same type when it is called.

# code-unsafe is faster than code but may cause
# a crash if one of the dynamically bound values
# has a different type.

# Neither code nor code-unsafe should be used
# like this. Use function.


(def (n int) stack)
(def (ret int) stack)
(def (temp int) stack)
(def (fibs (code-unsafe void)) heap)
# n ret temp
(set fibs
 (code-unsafe
  (if (< n 2)
   (set ret n)
   (begin

    (def n stack (- n 1))
    (def (ret int) stack)
    (def (temp int) stack)
    (fibs)
    (undef temp stack)
    (set temp ret)
    (undef ret stack)
    (undef n stack)

    (def n stack (- n 2))
    (def (ret int) stack)
    (def (temp int) stack)
    (fibs)
    (undef temp stack)
    (set temp (+ temp ret))
    (undef ret stack)
    (undef n stack)
    (set ret temp)))))
(undef temp stack)
(undef ret stack)
(undef n stack)

(def (n int) stack)
(def (ret int) stack)
(def (temp int) stack)
(def fibs-print heap
 (code-unsafe
  (begin
   (def (ret int) stack)
   (def (temp int) stack)
   (fibs)
   (undef temp stack)
   (print (str "Fibs(") n (str ") = ") ret (str "\n"))
   (undef ret stack))))
(undef temp stack)
(undef ret stack)
(undef n stack)

(def n stack 0) (fibs-print) (undef n stack)
(def n stack 1) (fibs-print) (undef n stack)
(def n stack 2) (fibs-print) (undef n stack)
(def n stack 3) (fibs-print) (undef n stack)
(def n stack 4) (fibs-print) (undef n stack)
(def n stack 5) (fibs-print) (undef n stack)
(def n stack 6) (fibs-print) (undef n stack)
(def n stack 7) (fibs-print) (undef n stack)
(def n stack 8) (fibs-print) (undef n stack)
(def n stack 9) (fibs-print) (undef n stack)
(def n stack 10) (fibs-print) (undef n stack)
(def n stack 11) (fibs-print) (undef n stack)
(def n stack 12) (fibs-print) (undef n stack)
(def n stack 13) (fibs-print) (undef n stack)
(def n stack 14) (fibs-print) (undef n stack)
(def n stack 15) (fibs-print) (undef n stack)
(def n stack 16) (fibs-print) (undef n stack)
(def n stack 17) (fibs-print) (undef n stack)
(def n stack 18) (fibs-print) (undef n stack)
(def n stack 19) (fibs-print) (undef n stack)
(def n stack 20) (fibs-print) (undef n stack)

//(free-quote fibs)
//(free-quote fibs-print)
(undef fibs heap)
(undef fibs-print heap)
