#!../crisp

# Struct test.

(def-type test-struct
 (struct packed
  (c char)
  (i int)
  (f float)
  (d double)))
   
(def a heap (struct test-struct
              (c (char a))
              (i 89)
              (f 79.0)
              (d 102.0)))
(print a)
(set (a d) 99.9)
(print (a d))
(set (a f) (: float 23.1))
(print a)
(set (a c) (char f))
(print a)

(undef a heap)
(undef-type test-struct)
