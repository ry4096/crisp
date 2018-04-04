#!../crisp

// example of conditional compilation using quasiquote.

//(def 3D stack 1)
(def 3D stack 0)

(: void ((quasiquote
  (begin-

   (def-type Point
    (struct align32
     (x int)
     (y int)
     (splice
      (if 3D
       (quote ((z int)))
       (quote ())))))

   (def print-point stack
    (function ((p (* Point))) void
     (print (str "<")
            ((p) x)
            (str ",")
            ((p) y)
            (splice
             (if 3D
              (quote ((str ",")
                      ((p) z)))
              (quote ())))
            (str ">"))))

   (def a stack
    (struct Point
     (x 1)
     (y 2)
     (splice
      (if 3D
       (quote ((z 3)))
       (quote ())))))

   (print (str "a = "))
   (print-point (& a))
   (print (str "\n"))

)))) // end quasiquote



