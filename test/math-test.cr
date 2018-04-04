#!../crisp

(load (str ../lib/math.cr))

(def (theta double) heap 23.1)

(print (str "sin(") theta (str ") = ") (c.sin theta) (str "\n"))
(print (str "sinf(") (to-float theta) (str ") = ") (c.sinf (to-float theta)) (str "\n"))
(print (str "sinl(") (to-longdouble theta) (str ") = ") (c.sinl (to-longdouble theta)) (str "\n"))

(undef theta heap)

(dl.close c.math.handle)
