#!../crisp

(def a stack
 (code
  (begin
   (print (str "Hello World\n"))
   (print (str "Hello World\n"))
  )
 )
)

(a)

(def b stack
 (quote
  (begin-
   (print (str "Hello World\n"))
   (print (str "Hello World\n")))))

(: void (b))
