#!../crisp
   
(def NULL heap (cast * 0))
(def LAZY heap 1)

(def main-handle heap (dl.open (cast str NULL) LAZY))

(def printf heap (cast (c-fun (str ...) int) (dl.sym main-handle (cast str (str printf)))))
(printf (cast str (str "Hello there world number %d!\n")) 5)
(print (dl.close main-handle) (char "\n"))

(undef printf heap)
(undef main-handle heap)
(undef LAZY heap)
(undef NULL heap)
