(in-prefix c.)

(def malloc.handle stack
 (dl.open (cast str 0) 1))

(dl.def-group malloc.handle
 (malloc malloc (c-fun (int) *))
 (free free (c-fun (*) void))
 (calloc calloc (c-fun (int int) *))
 (realloc realloc (c-fun (* int) *))
)

(clear-prefixes)
