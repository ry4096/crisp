#!../crisp

# quote test

(def a heap (quote (print a)))
a
a
(: void (a))
(: void (a))
(: void (a))
(: void (a))
//(free-quote a)
(undef a heap)
