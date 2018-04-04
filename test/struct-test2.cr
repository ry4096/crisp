#!../crisp

(def-type s
 (struct packed
  (i int)
  (f float)
  (d double)
  (pi (* int))))

(def is stack
 (array int 10 11 12 13 14))

(def S stack
 (struct s
  (i 3)
  (f 1)
  (d 2)
  (pi (& (is)))))

(def a stack (S i))

(print a (char "\n"))

(print ((+ (S pi) 2)) (char "\n"))

(print (* 34 (S i)) (str "\n"))

