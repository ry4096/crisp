#!../crisp


(def handle stack (dl.open (cast str 0) 1))

(dl.def-group handle
 (printf printf (c-fun (symbol ...) int))
 (stdout stdout (* *))
 (fprintf fprintf (c-fun (* symbol ...) int))
)

(printf (str "hello world!\n"))
(fprintf (stdout) (str "Hello World!\n"))

