#!../crisp -s

(in-prefix a.prefix.)
(def a heap 1)
(def b heap 2)
(def c heap 3)

(in-prefix other.prefix.)
(def a heap 4)
(def b heap 5)
(def c heap 6)

(in-prefix a.prefix.)
(print (str "a.prefix.a = ") a (char "\n"))
(print (str "a.prefix.b = ") b (char "\n"))
(print (str "a.prefix.c = ") c (char "\n"))

(in-prefix other.prefix.)
(print (str "other.prefix.a = ") a (char "\n"))
(print (str "other.prefix.b = ") b (char "\n"))
(print (str "other.prefix.c = ") c (char "\n"))

(clear-prefixes)

(use-prefix a.prefix.)
(print (str "a.prefix.a = ") a (char "\n"))
(print (str "a.prefix.b = ") b (char "\n"))
(print (str "a.prefix.c = ") c (char "\n"))

(use-prefix other.prefix.)
(print (str "other.prefix.a = ") a (char "\n"))
(print (str "other.prefix.b = ") b (char "\n"))
(print (str "other.prefix.c = ") c (char "\n"))

(in-prefix a.prefix.)
(undef a heap)
(undef b heap)
(undef c heap)

(in-prefix other.prefix.)
(undef a heap)
(undef b heap)
(undef c heap)

(clear-prefixes)
