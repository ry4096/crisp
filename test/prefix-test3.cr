#!../crisp -s


(def a heap -100)
(def b heap -200)
(def c heap -300)

(in-prefix a.prefix.)
(def a heap 1)
(def b heap 2)
(def c heap 3)

(def func heap
#(code-unsafe
#(quote
 (code
  (print (str "func:\n")
         (str "a = ") a (str "\n")
         (str "b = ") b (str "\n")
         (str "c = ") c (str "\n"))))

(clear-prefixes)

(: void (a.prefix.func))

(set a.prefix.a 45)

(: void (a.prefix.func))

(in-prefix a.prefix.)
(: void (func))

(clear-prefixes)

(undef a.prefix.a heap)
(undef a.prefix.b heap)
(undef a.prefix.c heap)
(undef a heap)
(undef b heap)
(undef c heap)
(undef a.prefix.func heap)
