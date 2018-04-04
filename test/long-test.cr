#!../crisp
   
(def NULL heap (cast * 0))
(def LAZY heap 1)

(def main-handle heap (dl.open (cast str NULL) LAZY))

(dl.def-group main-handle
 (strtol strtol (c-fun (str (* str) int) long))
 (strtoll strtoll (c-fun (str (* str) int) longlong))
 (strtod strtod (c-fun (str (* str)) double))
 (strtold strtold (c-fun (str (* str)) longdouble))
)

(def num-str stack (str heap "1111111111111111111"))
(def a heap (strtol num-str (cast str 0) 10))
(def b heap (strtoll num-str (cast str 0) 10))
(def c heap (strtod num-str (cast str 0)))
(def d heap (strtold num-str (cast str 0)))

(print num-str (str " = ") a (str "\n"))
(print num-str (str " = ") b (str "\n"))
(print num-str (str " = ") c (str "\n"))
(print num-str (str " = ") d (str "\n"))

(dl.close main-handle)


(free num-str)
(undef a heap)
(undef b heap)
(undef c heap)
(undef d heap)
(undef main-handle heap)
(undef NULL heap)
(undef LAZY heap)
