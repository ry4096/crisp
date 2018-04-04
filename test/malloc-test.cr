#!../crisp

(load (str ../lib/malloc.cr))

(def s stack (str heap "Hello World!\n"))
(print s)
(c.free s)
(undef s stack)
