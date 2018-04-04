
(in-prefix ftest5.)

(def f stack
 (function ((x int) (y double)) int
  (+ x (to-int y))))

(clear-prefixes)
