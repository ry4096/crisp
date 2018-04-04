# crisp
A LISP dialect meant to interface well with C.
Types like structs, arrays, and pointers are all native types in Crisp.
It is:

* Statically typed.
* Interpreted but reasonably fast.
* Dynamically scoped.
* Has no gc. Reference counting is used for types and syntax trees, but that's it.
* Has defmacro-style macros.
* Unsafe. You can do pointer arithmetic and perform casts.
* Has C++ like destructors.

The benchmarks I've done so far have it about the same speed as the Lua interpreter. It's written in C with a little assembly for the foriegn function interface. You should be able to define an interface to a C library without having to write any glue code in C. Just open the shared library and provide the function prototypes in Crisp.

Crisp is beta and I'm still expanding the standard library. A reference for the language is also needed.

Licence: MIT

Contact: ry4096@gmail.com

