#!../crisp


(def a stack (array int 1 2 3 4 5 6))

(print
 (str "a = ") a (str "\n")
 (str "(a) = ") (a) (str "\n")
 (str "(& (a)) = ") (& (a)) (str "\n")
 (str "(+ (& (a)) 0) = ") (+ (& (a)) 0) (str "\n")
 (str "((+ (& (a)) 0)) = ") ((+ (& (a)) 0)) (str "\n")
 (str "(a 0) = ") (a 0) (str "\n")
 (str "(+ (& (a)) 2) = ") (+ (& (a)) 2) (str "\n")
 (str "((+ (& (a)) 2)) = ") ((+ (& (a)) 2)) (str "\n")
 (str "(a 2) = ") (a 2) (str "\n")
)

(undef a stack)
