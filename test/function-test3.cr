#!../crisp

(def (A (function ((n int)) int)) heap)
(set A
 (function ((n int)) int
  (if (> n 0)
    (- n 1)
    n)))

(print (str "(A 4) = ") (A 4) (str "\n"))
(print (str "(A -4) = ") (A -4) (str "\n"))

//(free-quote A)
(undef A heap)
