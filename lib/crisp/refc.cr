# Reference counting functions used by crisp.

(import. handle)

(in-prefix crisp.)

(dl.def-group main-handle
 (refc.alloc refc_alloc (c-fun (int) *))
 (refc.str refc_str (c-fun (str) str))
 (refc.dec refc_dec (c-fun (*) int))
 (refc.count refc_count (c-fun (*) int))
 (refc.dec-free refc_dec_free (c-fun (*) int))
 (refc.copy refc_copy (c-fun (*) *)))

(clear-prefixes)
