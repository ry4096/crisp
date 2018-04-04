#!../crisp

(def-type A)
(set-type A
 (struct align32
  //(s symbol)
  (i int)
  (short1 int32 16)
  (short2 int32 16)
  (next (* A))
 )
)

(def a3 stack
 (struct A
  //(s (str a3))
  (i 3)
  (short1 3)
  (short2 3)
  (next 0)
 )
)

(def a2 stack
 (struct A
  //(s (str a2))
  (i 2)
  (short1 2)
  (short2 2)
  (next (& a3))
 )
)

(def a1 stack
 (struct A
  //(s (str a1))
  (i 1)
  (short1 1)
  (short2 1)
  (next (& a2))
 )
)

(def a-f stack
 (function ((n int)) A
  (struct A
   //(s (str a-f))
   (i n)
   (short1 n)
   (short2 n)
   (next 0))))

(print
 (str "a1 = ") a1 (str "\n")
 (str "a2 = ") a2 (str "\n")
 (str "a3 = ") a3 (str "\n")
 (str "(. a1 i) = ") (. a1 i) (str "\n")
 (str "(. a2 i) = ") (. a2 i) (str "\n")
 (str "(. a1 next i) = ") (. a1 next i) (str "\n")
 (str "(. (& a1) next i) = ") (. (& a1) next i) (str "\n")
 (str "(. a1 next next short1) = ") (. a1 next next short1) (str "\n")
 (str "(. a1 next next short2) = ") (. a1 next next short2) (str "\n")
 (str "(a-f 5) = ") (a-f 5) (str "\n")
 (str "((a-f 5) i) = ") ((a-f 5) i) (str "\n")
 (str "(. (a-f 5) i) = ") (. (a-f 5) i) (str "\n")
)

(set (. a1 next next i) 13)
(print (str "eval (set (. a1 next next i) 13)\n"))

(print (str "((. a1 next next)) = ") ((. a1 next next)) (str "\n"))

(set-slot ((. a1 next next)) short1 23)
(print (str "eval (set-slot ((. a1 next next)) short1 23)\n"))

(print (str "((. a1 next next)) = ") ((. a1 next next)) (str "\n"))



