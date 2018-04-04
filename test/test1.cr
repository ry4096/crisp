#!../crisp

# Nested arrays.

(def a heap
 (array
  (array 3 int)
  (array int 1 2 3)
  (array int 4 5 6)
  (array int 7 8 9)
  (array int 17 28 39)
 )
)

a
(undef a heap)
