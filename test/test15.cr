#!../crisp

# Tests recursion.

(def (fibs (function ((n int)) int)) stack)
(set fibs
 (function ((n int)) int
  (if (< n 2)
   n
   (+ (fibs (- n 1)) (fibs (- n 2))))))

(def fibs-print stack
 (function ((n int)) void
  (print (str "Fibs(") n (str ") = ") (fibs n) (str "\n"))))

(fibs-print 0)
(fibs-print 1)
(fibs-print 2)
(fibs-print 3)
(fibs-print 4)
(fibs-print 5)
(fibs-print 6)
(fibs-print 7)
(fibs-print 8)
(fibs-print 9)
(fibs-print 10)
(fibs-print 11)
(fibs-print 12)
(fibs-print 13)
(fibs-print 14)
(fibs-print 15)
(fibs-print 16)
(fibs-print 17)
(fibs-print 18)
(fibs-print 19)
(fibs-print 20)

#(free-quote fibs-print)
#(free-quote fibs)
#(undef fibs-print heap)
#(undef fibs heap)
