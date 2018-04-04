#!../crisp

(let (a 1)
     (b (+ 2 a))
     ((c float) 23.4)
     ((d char))
 (begin
  (set d (char d))
  (print (str   "a = ") a 
         (str "\nb = ") b
         (str "\nc = ") c
         (str "\nd = ") d
         (str "\n"))))

(print
 (str "let = ")
 (let (a 45) ((b long) (+ (to-long a) 2))
  b)
 (str "\n"))

(def a stack (array int 1 3 5 7 9))

(print
 (str "lval let = ")
 ((let (i 2) (p (& (a)))
  (+ p i)) 2)
 (str "\n"))
