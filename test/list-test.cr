#!../crisp

(def-type List)
(set-type List
 (struct align32
  (value int)
  (next (* List))))

(def List.print stack
 (function ((self (* List))) void
  (begin
   (print (str "["))
   (while (not (= self 0))
    (print ((self) value))
    (set self ((self) next))
    (if (not (= self 0))
     (print (str ", "))))
   (print (str "]")))))

(def node3 stack
 (struct List
  (value 3)
  (next 0)))

(def node2 stack
 (struct List
  (value 2)
  (next (& node3))))

(def node1 stack
 (struct List
  (value 1)
  (next (& node2))))

(List.print (& node1))
(print (str "\n"))
