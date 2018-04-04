#!../crisp

# begin- test.
# begin- is like begin but each term is type checked at evaluation time.
# begin should be used instead because begin- is slower.


(def func heap
 (quote
   (begin-
    (print n))))
(def n stack 4)


(: void (func))
(undef n stack)
//(free-quote func)
(undef func heap)
