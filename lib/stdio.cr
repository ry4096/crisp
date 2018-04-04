(in-prefix c.)

# Expand.

(def stdio.handle stack (dl.open (cast str 0) 1))

(def-type FILE)

(dl.def-group stdio.handle
 (stdin-p stdin (* (* FILE)))
 (stdout-p stdout (* (* FILE)))
 (stderr-p stderr (* (* FILE)))

 (fwrite fwrite (c-fun (* int int (* FILE)) int))
 (fread fread (c-fun (* int int (* FILE)) int))
 (fprintf fprintf (c-fun ((* FILE) str ...) int))
 (printf printf (c-fun (str ...) int))
)

(def stdin stack (stdin-p))
(def stdout stack (stdout-p))
(def stderr stack (stderr-p))

(clear-prefixes)
