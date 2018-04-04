// This implements the macros found in bits.h for uint.


(in-prefix bits.)

// Lower digits are ones. Upper digits are zeros.
(def lower-ones stack
 (function ((num-ones uint)) uint
  (- (<< 1 (to-int num-ones)) 1)))


// Upper digits are ones. Lower digits are zeros.
(def upper-ones stack
 (function ((num-zeros uint)) uint
  (bit-not (lower-ones num-zeros))))


// A window of width and offset are zeros. The digits before and after the window are zeros.
(def zero-window stack
 (function ((offset uint) (width uint)) uint
  (bit-or (<< (upper-ones width) (to-int offset)) (lower-ones offset))))


// Returns value with the bits inside the window set to zero.
(def set-window-zero stack
 (function ((value uint) (offset uint) (width uint)) uint
  (bit-and value (zero-window offset width))))


// Clips value by width, and shifts by offset.
(def align-window stack
 (function ((value uint) (offset uint) (width uint)) uint
  (<< (bit-and value (lower-ones width)) (to-int offset))))

(def set-window stack
 (function ((value1 uint) (offset uint) (width uint) (value2 uint)) uint
  (bit-or (set-window-zero value1 offset width) (align-window value2 offset width))))

(clear-prefixes)

