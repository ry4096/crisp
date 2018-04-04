#!../crisp

/*
(def a stack
 (macro (one)
  (begin
   (print (str "one = ") one (str "\n"))
   (` (+ (, one) 2)))))
   */

(def a stack
 (macro (one)
//  (` (+ (, one) 2))
//  (` (+ 1 2))
  one
//  (` (, one))
 )
)

(print (a 1) (str "\n"))


