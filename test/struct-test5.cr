#!../crisp

(def-type A
 (struct align32
  (a short)
  (b char)
  (c1 uint64 16)
  (c2 uint64 16)
  (c3 uint64 16)
  (c4 uint64 16)
  (d char)
  (e char)))

(def a stack
 (struct A
  (a 1)
  (b (char b))
  (c1 11)
  (c2 12)
  (c3 13)
  (c4 14)
  (d (char d))
  (e (char e))))

(print (str "a = ") a (str "\n"))
