#!../crisp

(def point-template stack
 (macro (name type)
  (` (def-type (, name)
      (struct align32
       (x (, type))
       (y (, type))
       (z (, type)))))))

(point-template Point.int int)
(point-template Point.double double)

(def a stack
 (struct Point.int
  (x 1)
  (y 2)
  (z 3)))

(def b stack
 (struct Point.double
  (x 1.1)
  (y 2.2)
  (z 3.3)))

(print (str "a = ") a (str "\n")
       (str "b = ") b (str "\n"))

//(free-quote point-template)

