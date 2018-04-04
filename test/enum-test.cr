#!../crisp

(def-enum fruit stack
 apples
 oranges
 bananas)

(def (a fruit) stack apples)

(print (str "apples = ") a (str "\n"))

(set a oranges)
(print (str "oranges = ") a (str "\n"))

(set a bananas)
(print (str "bananas = ") a (str "\n"))

(def-enum test stack
 (one 1)
 two
 three
 four
 (neg-one (- 0 1))
 zero)

(print (str   "one = ") one
       (str "\ntwo = ") two
       (str "\nthree = ") three
       (str "\nfour = ") four
       (str "\nneg-one = ") neg-one
       (str "\nzero = ") zero (str "\n"))
