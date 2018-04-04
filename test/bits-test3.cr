#!../crisp

(def-type A
 (struct align32
  (i int)
  (short1 int32 16)
  (short2 int32 16)
 )
)

(def a stack
 (struct A
  (i 1)
  (short1 2)
  (short2 3)
 )
)

(print (str "a = ") a (str "\n"))
