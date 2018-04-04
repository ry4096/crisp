#!../crisp

(import.
 (.. lib crisp hash-table)
 (.. lib stdio)
 (.. lib malloc)
)

(in-prefix hash-test.)
(use-prefix crisp.)

(def (table hash-table) stack)

(hash-table-init (& table) hash-string-obj 63)

(hash-set-insert (& table) (str heap one))
(hash-set-insert (& table) (str heap two))
(hash-set-insert (& table) (str heap three))
(hash-set-insert (& table) (str heap four))
(hash-set-insert (& table) (str heap five))

(hash-set-dump c.stdout (& table) print-string)

(hash-set-for-each (& table) c.free)

(hash-table-destruct (& table))
