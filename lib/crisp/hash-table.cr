# Direct interface to the hash table used by crisp.

(import. handle)

(in-prefix crisp.)

(def-type hash-fn (c-fun (* int *) int))
(def-type hash-eq-fn (c-fun (* * *) int))
(def-type hash-value-fn (c-fun (*) void))
(def-type hash-value2-fn (c-fun (* *) void))

(def-type hash-vtable
 (struct align32
  (hash hash-fn)
  (eq hash-eq-fn)))

(def-type hash-obj (* (* hash-vtable)))

(dl.def-group main-handle
 (string-hash string_hash (c-fun (* int (* char)) int))
 (string-eq string_eq (c-fun (* (* char) (* char)) int))
 (hash-vtable-string hash_vtable_string (* hash-vtable))
 (hash-string-obj hash_string_obj hash-obj)
 (pointer-hash pointer_hash (c-fun (* int *) int))
 (pointer-eq pointer_eq (c-fun (* * *) int))
 (hash-vtable-pointer hash_vtable_pointer (* hash-vtable))
 (hash-pointer-obj hash_pointer_obj hash-obj))

(def-type key-value
 (struct align32
  (key *)
  (value *)))

(def-type hash-key-value-obj
 (struct align32
  (vtable (* hash-vtable))
  (hash-child hash-obj)))

(dl.def-group main-handle
 (key-value-left-hash key_value_left_hash (c-fun ((* hash-key-value-obj) int *) int))
 (key-value-left-eq key_value_left_eq (c-fun ((* hash-key-value-obj) (* key-value) *) int))
 (hash-vtabke-key-value-left hash_vtable_key_value_left (* hash-vtable))
 (key-value-hash key_value_hash (c-fun ((* hash-key-value-obj) int (* key-value)) int))
 (key-value-eq key_value_eq (c-fun ((* hash-key-value-obj) (* key-value) (* key-value)) int))
 (hash-vtable-key-value hash_vtable_key_value (* hash-vtable))
 (key-value key_value (c-fun (* *) (* key-value))))

(def-type hash-table
 (struct align32
  (array (* *))
  (array-length int)
  (num-elts int)
  (hasher hash-obj)))

(dl.def-group main-handle
 (hash-table-init hash_table_init (c-fun ((* hash-table) hash-obj int) void))
 (hash-table-destruct hash_table_destruct (c-fun ((* hash-table)) void))
 (hash-table-find-index hash_table_find_index (c-fun ((* hash-table) * hash-obj) int))
 (hash-table-slot- hash_table_slot_ (c-fun ((* hash-table) * hash-obj) (* *)))
 (hash-table-insert- hash_table_insert_ (c-fun ((* hash-table) * hash-obj) *))
 (hash-map-set- hash_map_set_ (c-fun ((* hash-table) * * hash-obj) *))
 (hash-set-find hash_set_find (c-fun ((* hash-table) * hash-obj) *))
 (hash-map-find hash_map_find (c-fun ((* hash-table) * hash-obj) (* key-value)))
 (hash-map-find-value hash_map_find_value (c-fun ((* hash-table) * hash-obj) *))
 (hash-table-resize hash_table_resize (c-fun ((* hash-table) int hash-obj) void))
 (hash-table-check-up-size hash_table_check_up_size (c-fun ((* hash-table) hash-obj) int))
 (hash-table-check-down-size hash_table_check_down_size (c-fun ((* hash-table) hash-obj) int))
 (hash-table-slot hash_table_slot (c-fun ((* hash-table) * hash-obj hash-obj) (* *)))
 (hash-set-slot hash_set_slot (c-fun ((* hash-table) * hash-obj) (* *)))
 (hash-map-slot hash_map_slot (c-fun ((* hash-table) * hash-obj) (* (* key-value))))
 (hash-map-intern hash_map_intern (c-fun ((* hash-table) * hash-obj) (* key-value)))
 (hash-table-insert hash_table_insert (c-fun ((* hash-table) * hash-obj) *))
 (hash-set-insert hash_set_insert (c-fun ((* hash-table) *) *))
 (hash-map-insert hash_map_insert (c-fun ((* hash-table) (* key-value)) (* key-value)))
 (hash-map-set hash_map_set (c-fun ((* hash-table) * *) *))
 (hash-table-rehash-index hash_table_rehash_index (c-fun ((* hash-table) int hash-obj) void))
 (hash-table-rehash-block hash_table_rehash_block (c-fun ((* hash-table) int hash-obj) void))
 (hash-table-remove hash_table_remove (c-fun ((* hash-table) * hash-obj hash-obj) *))
 (hash-set-remove hash_set_remove (c-fun ((* hash-table) * hash-obj) *))
 (hash-map-remove hash_map_remove (c-fun ((* hash-table) * hash-obj) *))
 (hash-set-for-each hash_set_for_each (c-fun ((* hash-table) hash-value-fn) void))
 (hash-set-for-each2 hash_set_for_each2 (c-fun ((* hash-table) (* hash-value2-fn)) void))
 (hash-set-for-each3 hash_set_for_each3 (c-fun ((* hash-table) hash-value2-fn *) void))
 (print-string print_string (c-fun (* str) void))
 (hash-set-dump hash_set_dump (c-fun (* (* hash-table) hash-value2-fn) void))
 (hash-map-dump hash_map_dump (c-fun (* (* hash-table) hash-value2-fn hash-value2-fn) void)))

(clear-prefixes)
