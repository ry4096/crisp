#!../crisp

(def A heap
 (function ((a symbol) (b int) (c char)) void
  (print (str "Hi from A!: ") a b c)))

(A (str "HHHHHEEEEAA") 23 (char n))

(undef A heap)
