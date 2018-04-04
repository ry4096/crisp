#!../crisp

(def f stack (quote 47))
(def g stack (quote 48))

(def tests stack
 (array quote
  (quote (quasiquote (1 2 3)))
  (quote (quasiquote (a b c)))
  (quote (quasiquote (a b c (unquote f))))
  (quote (quasiquote (a b c (unquote (quasiquote (unquote f))))))
  (' (` (a b c (, (` (, f))))))
  (quote (quasiquote (1 2 (unquote f) 3 4 5)))
  (quote (quasiquote (1 2 (unquote f) 3 4 5 ((unquote g)))))
  (quote (quasiquote (1 2 ((unquote f) 3 ((4) 5) ((unquote g))))))
  (quote (quasiquote (((unquote f) 1))))
  (quote (quasiquote (1 2 3 (splice (quote (4 5 6 7))) 8 9 10)))
 )
)

(def (i int) stack)

(for (set i 0) (< i (array.length tests)) (set i (+ i 1))
 (let (code (tests i))
      (res (: quote (code)))
  (begin
   (print code (str " = ") res (str "\n"))
   //(free-quote res)
  )
 )
)

//(for (set i 0) (< i (array.length tests)) (set i (+ i 1))
// (free-quote (tests i)))

//(free-quote f)
//(free-quote g)
