#!../crisp

(def-type s
 (struct packed
  (i int)
  (f float)
  (d double)))

(def-type d
 (struct packed
  (ad s)
  (add (* s))))

(def aa stack
 (struct s
  (i 1)
  (f 1)
  (d 1)))

(def a stack
 (array d
  (struct d
   (ad (struct s
        (i 1)
        (f 2.0)
        (d 3.0)))
   (add (& aa)))
  (struct d
   (ad (struct s
        (i 4)
        (f 4.0)
        (d 4.0)))
   (add (& aa)))))

(print a)

