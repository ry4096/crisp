(in-prefix c.)

(def math.handle stack (dl.open (cast str (str "libm.so")) 1))

(def (M_E double) stack 2.7182818284590452354)
(def (M_LOG2E double) stack 1.4426950408889634074)
(def (M_LOG10E double) stack 0.43429448190325182765)
(def (M_LN2 double) stack 0.69314718055994530942)
(def (M_LN10 double) stack 2.30258509299404568402)
(def (M_PI double) stack 3.14159265358979323846)
(def (M_PI_2 double) stack 1.57079632679489661923)
(def (M_PI_4 double) stack 0.78539816339744830962)
(def (M_1_PI double) stack 0.31830988618379067154)
(def (M_2_PI double) stack 0.63661977236758134308)
(def (M_2_SQRTPI double) stack 1.12837916709551257390)
(def (M_SQRT2 double) stack 1.41421356237309504880)
(def (M_SQRT1_2 double) stack 0.70710678118654752440)

(dl.def-group math.handle
 (acos acos (c-fun (double) double))
 (acosf acosf (c-fun (float) float))
 (acosl acosl (c-fun (longdouble) longdouble))
 (asin asin (c-fun (double) double))
 (asinf asinf (c-fun (float) float))
 (asinl asinf (c-fun (longdouble) longdouble))
 (atan atan (c-fun (double) double))
 (atanf atanf (c-fun (float) float))
 (atanl atanl (c-fun (longdouble) longdouble))
 (atan2 atan2 (c-fun (double double) double))
 (atan2f atan2f (c-fun (float float) float))
 (atan2l atan2l (c-fun (longdouble longdouble) longdouble))
 (cos cos (c-fun (double) double))
 (cosf cosf (c-fun (float) float))
 (cosl cosl (c-fun (longdouble) longdouble))
 (sin sin (c-fun (double) double))
 (sinf sinf (c-fun (float) float))
 (sinl sinl (c-fun (longdouble) longdouble))
 (tan tan (c-fun (double) double))
 (tanf tanf (c-fun (float) float))
 (tanl tanl (c-fun (longdouble) longdouble))
 (acosh acosh (c-fun (double) double))
 (acoshf acoshf (c-fun (float) float))
 (acoshl acoshl (c-fun (longdouble) longdouble))
 (asinh asinh (c-fun (double) double))
 (asinhf asinhf (c-fun (float) float))
 (asinhl asinhl (c-fun (longdouble) longdouble))
 (atanh atanh (c-fun (double) double))
 (atanhf atanhf (c-fun (float) float))
 (atanhl atanhl (c-fun (longdouble) longdouble))
 (cosh cosh (c-fun (double) double))
 (coshf coshf (c-fun (float) float))
 (coshl coshl (c-fun (longdouble) longdouble))
 (sinh sinh (c-fun (double) double))
 (sinhf sinhf (c-fun (float) float))
 (sinhl sinhl (c-fun (longdouble) longdouble))
 (tanh tanh (c-fun (double) double))
 (tanhf tanhf (c-fun (float) float))
 (tanhl tanhl (c-fun (longdouble) longdouble))
 (exp exp (c-fun (double) double))
 (expf expf (c-fun (float) float))
 (expl expl (c-fun (longdouble) longdouble))
 (log log (c-fun (double) double))
 (logf logf (c-fun (float) float))
 (logl logl (c-fun (longdouble) longdouble))
 (exp2 exp2 (c-fun (double) double))
 (exp2f exp2f (c-fun (float) float))
 (exp2l exp2l (c-fun (longdouble) longdouble))
 (log2 log2 (c-fun (double) double))
 (log2f log2f (c-fun (float) float))
 (log2l log2l (c-fun (longdouble) longdouble))
 (frexp frexp (c-fun (double (* int)) double))
 (frexpf frexpf (c-fun (float (* int)) float))
 (frexpl frexpl (c-fun (longdouble (* int)) longdouble))
 (ldexp ldexp (c-fun (double int) double))
 (ldexpf ldexpf (c-fun (float int) float))
 (ldexpl ldexpl (c-fun (longdouble int) longdouble))
 (log10 log10 (c-fun (double) double))
 (log10f log10f (c-fun (float) float))
 (log10l log10l (c-fun (longdouble) longdouble))
 (modf modf (c-fun (double (* double)) double))
 (modff modff (c-fun (float (* float)) float))
 (modfl modfl (c-fun (longdouble (* longdouble)) longdouble))
 (pow pow (c-fun (double double) double))
 (powf powf (c-fun (float float) float))
 (powl powl (c-fun (longdouble longdouble) longdouble))
 (sqrt sqrt (c-fun (double) double))
 (sqrtf sqrtf (c-fun (float) float))
 (sqrtl sqrtl (c-fun (longdouble) longdouble))
 (hypot hypot (c-fun (double double) double))
 (hypotf hypotf (c-fun (float float) float))
 (hypotl hypotl (c-fun (longdouble longdouble) longdouble))
 (cbrt cbrt (c-fun (double) double))
 (cbrtf cbrtf (c-fun (float) float))
 (cbrtl cbrtl (c-fun (longdouble) longdouble))
 (ceil ceil (c-fun (double) double))
 (ceilf ceilf (c-fun (float) float))
 (ceill ceill (c-fun (longdouble) longdouble))
 (fabs fabs (c-fun (double) double))
 (fabsf fabsf (c-fun (float) float))
 (fabsl fabsl (c-fun (longdouble) longdouble))
 (floor floor (c-fun (double) double))
 (floorf floorf (c-fun (float) float))
 (floorl floorl (c-fun (longdouble) longdouble))
 (fmod fmod (c-fun (double double) double))
 (fmodf fmodf (c-fun (float float) float))
 (fmodl fmodl (c-fun (longdouble longdouble) longdouble))
 (drem drem (c-fun (double double) double))
 (dremf dremf (c-fun (float float) float))
 (dreml dreml (c-fun (longdouble longdouble) longdouble))
 (remainder remainder (c-fun (double double) double))
 (remainderf remainderf (c-fun (float float) float))
 (remainderl remainderl (c-fun (longdouble longdouble) longdouble))
 (copysign copysign (c-fun (double double) double))
 (copysignf copysignf (c-fun (float float) float))
 (copysignl copysignl (c-fun (longdouble longdouble) longdouble))
 (nan nan (c-fun (str) double))
 (nanf nanf (c-fun (str) float))
 (nanl nanl (c-fun (str) longdouble))
 (j0 j0 (c-fun (double) double))
 (j0f j0f (c-fun (float) float))
 (j0l j0l (c-fun (longdouble) longdouble))
 (j1 j1 (c-fun (double) double))
 (j1f j1f (c-fun (float) float))
 (j1l j1l (c-fun (longdouble) longdouble))
 (jn jn (c-fun (int double) double))
 (jnf jnf (c-fun (int float) float))
 (jnl jnl (c-fun (int longdouble) longdouble))
 (y0 y0 (c-fun (double) double))
 (y0f y0f (c-fun (float) float))
 (y0l y0l (c-fun (longdouble) longdouble))
 (y1 y1 (c-fun (double) double))
 (y1f y1f (c-fun (float) float))
 (y1l y1l (c-fun (longdouble) longdouble))
 (yn yn (c-fun (int double) double))
 (ynf ynf (c-fun (int float) float))
 (ynl ynl (c-fun (int longdouble) longdouble))
 (erf erf (c-fun (double) double))
 (erff erff (c-fun (float) float))
 (erfl erfl (c-fun (longdouble) longdouble))
 (erfc erfc (c-fun (double) double))
 (erfcf erfcf (c-fun (float) float))
 (erfcl erfcl (c-fun (longdouble) longdouble))
 (lgamma lgamma (c-fun (double) double))
 (lgammaf lgammaf (c-fun (float) float))
 (lgammal lgammal (c-fun (longdouble) longdouble))
 (lgamma_r lgamma_r (c-fun (double (* int)) double))
 (lgammaf_r lgammaf_r (c-fun (float (* int)) float))
 (lgammal_r lgammal_r (c-fun (longdouble (* int)) longdouble))
 (tgamma tgamma (c-fun (double) double))
 (tgammaf tgammaf (c-fun (float) float))
 (tgammal tgammal (c-fun (longdouble) longdouble))
 (gamma gamma (c-fun (double) double))
 (gammaf gammaf (c-fun (float) float))
 (gammal gammal (c-fun (longdouble) longdouble))
 (rint rint (c-fun (double) double))
 (rintf rintf (c-fun (float) float))
 (rintl rintl (c-fun (longdouble) longdouble))
 (nearbyint nearbyint (c-fun (double) double))
 (nearbyintf nearbyintf (c-fun (float) float))
 (nearbyintl nearbyintl (c-fun (longdouble) longdouble))
 (ilogb ilogb (c-fun (double) double))
 (ilogbf ilogbf (c-fun (float) float))
 (ilogbl ilogbl (c-fun (longdouble) longdouble))
 (round round (c-fun (double) double))
 (roundf roundf (c-fun (float) float))
 (roundl roundl (c-fun (longdouble) longdouble))
 (trunc trunc (c-fun (double) double))
 (truncf truncf (c-fun (float) float))
 (truncl truncl (c-fun (longdouble) longdouble))
 (remquo remquo (c-fun (double double (* int)) double))
 (remquof remquof (c-fun (float float (* int)) float))
 (remquol remquol (c-fun (longdouble longdouble (* int)) longdouble))
 (fdim fdim (c-fun (double double) double))
 (fdimf fdimf (c-fun (float float) float))
 (fdiml fdiml (c-fun (longdouble longdouble) longdouble))
 (fmax fmax (c-fun (double double) double))
 (fmaxf fmaxf (c-fun (float float) float))
 (fmaxl fmaxl (c-fun (longdouble longdouble) longdouble))
 (fmin fmin (c-fun (double double) double))
 (fminf fminf (c-fun (float float) float))
 (fminl fminl (c-fun (longdouble longdouble) longdouble))
 (fma fma (c-fun (double double double) double))
 (fmaf fmaf (c-fun (float float float) float))
 (fmal fmal (c-fun (longdouble longdouble longdouble) longdouble)))

(clear-prefixes)
