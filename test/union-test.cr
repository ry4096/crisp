#!../crisp

(def-type number
 (struct union
  (i int)
  (ui uint)
  (d double)
  (f float)))

(def a stack
 (array number
  (struct number (i 1))
  (struct number (ui 2))
  (struct number (d 3))
  (struct number (f 4))))

(print a)
