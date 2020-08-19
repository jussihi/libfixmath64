#ifndef __libfixmath_fix32_h__
#define __libfixmath_fix32_h__

#ifdef __cplusplus
extern "C"
{
#endif

/* These options may let the optimizer to remove some calls to the functions.
 * Refer to http://gcc.gnu.org/onlinedocs/gcc/Function-Attributes.html
 */
#ifndef FIXMATH_FUNC_ATTRS
# ifdef __GNUC__
#   if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ > 6)
#     define FIXMATH_FUNC_ATTRS __attribute__((leaf, nothrow, const))
#   else
#     define FIXMATH_FUNC_ATTRS __attribute__((nothrow, const))
#   endif
# else
#   define FIXMATH_FUNC_ATTRS
# endif
#endif

#include <stdint.h>

typedef int64_t fix32_t;

static const fix32_t FOUR_DIV_PI = 0x145F306DD;               /*!< Fix32 value of 4/PI */
static const fix32_t _FOUR_DIV_PI2 = 0xFFFFFFFF983f4277;       /*!< Fix32 value of -4/PI² */
static const fix32_t X4_CORRECTION_COMPONENT = 0x3999999A;    /*!< Fix32 value of 0.225 */
static const fix32_t PI_DIV_4 = 0xC90FDAA2;                   /*!< Fix32 value of PI/4 */
static const fix32_t THREE_PI_DIV_4 = 0x25B2F8FE6;            /*!< Fix32 value of 3PI/4 */

static const fix32_t fix32_maximum  = 0x7FFFFFFFFFFFFFFF;     /*!< the maximum value of fix32_t */
static const fix32_t fix32_minimum  = 0x8000000000000000;     /*!< the maximum value of fix32_t */
static const fix32_t fix32_overflow = 0x8000000000000000;     /*!< the value used to indicate overflows when FIXMATH_NO_OVERFLOW is not specified */
static const fix32_t fix32_epsilon = 0x0000000000000001;      /*!< the min absolute value of fix32_t */

static const fix32_t fix32_pi = 13493037704;                  /*!< fix32_t value of pi */		//0x 0000 0003 243F 6A88
static const fix32_t fix32_pi_over_2 = 6746518852;            /*!< fix32_t value of pi */		//0x 0000 0001 921F B544
static const fix32_t fix32_e  = 11674931555;                  /*!< fix32_t value of e */
static const fix32_t fix32_one = 0x0000000100000000;          /*!< fix32_t value of 1 */

/* Conversion functions between fix32_t and float/integer.
 * These are inlined to allow compiler to optimize away constant numbers
 */
static inline fix32_t fix32_from_int(int a)     { return a * fix32_one; }
static inline float   fix32_to_float(fix32_t a) { return (float)a / fix32_one; }
static inline double  fix32_to_dbl(fix32_t a)   { return (double)a / fix32_one; }

static inline int fix32_to_int(fix32_t a)
{
#ifdef FIXMATH_NO_ROUNDING
	return (a >> 32);
#else
	if (a >= 0)
		return (a + (fix32_one >> 1)) / fix32_one;
	return (a - (fix32_one >> 1)) / fix32_one;
#endif
}

static inline fix32_t fix32_from_float(float a)
{
	float temp = a * fix32_one;
#ifndef FIXMATH_NO_ROUNDING
	temp += (temp >= 0) ? 0.5f : -0.5f;
#endif
	return (fix32_t)temp;
}

static inline fix32_t fix32_from_dbl(double a)
{
	double temp = a * fix32_one;
#ifndef FIXMATH_NO_ROUNDING
	temp += (double)((temp >= 0) ? 0.5f : -0.5f);
#endif
	return (fix32_t)temp;
}

/* Converts a binary IEEE745 float to a fixed point q31.32 integer
 * without FPO
 */
extern fix32_t fix32_from_float_bin(const void* value);

/* Converts a q31.32 integer back to binary IEEE754 floating point
 * It returns uint32_t that contains the 32-bit float value
 */
extern uint32_t float_from_fix32_bin(fix32_t value);

/* Macro for defining fix32_t constant values.
	 The functions above can't be used from e.g. global variable initializers,
	 and their names are quite long also. This macro is useful for constants
	 springled alongside code, e.g. F32(1.234).

	 Note that the argument is evaluated multiple times, and also otherwise
	 you should only use this for constant values. For runtime-conversions,
	 use the functions above.
*/
#define F32(x) ((fix32_t)(((x) >= 0) ? ((x) * 4294967296.0 + 0.5) : ((x) * 4294967296.0 - 0.5)))

static inline fix32_t fix32_abs(fix32_t x)
	{ return (x < 0 ? -x : x); }
static inline fix32_t fix32_floor(fix32_t x)
	{ return (x & 0xFFFFFFFF00000000ULL); }
static inline fix32_t fix32_ceil(fix32_t x)
	{ return (x & 0xFFFFFFFF00000000ULL) + (x & 0x00000000FFFFFFFFULL ? fix32_one : 0); }
static inline fix32_t fix32_min(fix32_t x, fix32_t y)
	{ return (x < y ? x : y); }
static inline fix32_t fix32_max(fix32_t x, fix32_t y)
	{ return (x > y ? x : y); }
static inline fix32_t fix32_clamp(fix32_t x, fix32_t lo, fix32_t hi)
	{ return fix32_min(fix32_max(x, lo), hi); }

/* Subtraction and addition with (optional) overflow detection. */
#ifdef FIXMATH_NO_OVERFLOW

static inline fix32_t fix32_add(fix32_t inArg0, fix32_t inArg1) { return (inArg0 + inArg1); }
static inline fix32_t fix32_sub(fix32_t inArg0, fix32_t inArg1) { return (inArg0 - inArg1); }

#else

extern fix32_t fix32_add(fix32_t a, fix32_t b) FIXMATH_FUNC_ATTRS;
extern fix32_t fix32_sub(fix32_t a, fix32_t b) FIXMATH_FUNC_ATTRS;

/* Saturating arithmetic */
extern fix32_t fix32_sadd(fix32_t a, fix32_t b) FIXMATH_FUNC_ATTRS;
extern fix32_t fix32_ssub(fix32_t a, fix32_t b) FIXMATH_FUNC_ATTRS;

#endif

/*! Multiplies the two given fix16_t's and returns the result.
*/
extern fix32_t fix32_mul(fix32_t inArg0, fix32_t inArg1) FIXMATH_FUNC_ATTRS;

/*! Divides the first given fix16_t by the second and returns the result.
*/
extern fix32_t fix32_div(fix32_t inArg0, fix32_t inArg1) FIXMATH_FUNC_ATTRS;

#ifndef FIXMATH_NO_OVERFLOW
/*! Performs a saturated multiplication (overflow-protected) of the two given fix32_t's and returns the result.
*/
extern fix32_t fix32_smul(fix32_t inArg0, fix32_t inArg1) FIXMATH_FUNC_ATTRS;

/*! Performs a saturated division (overflow-protected) of the first fix32_t by the second and returns the result.
*/
extern fix32_t fix32_sdiv(fix32_t inArg0, fix32_t inArg1) FIXMATH_FUNC_ATTRS;
#endif

/*! Divides the first given fix32_t by the second and returns the result.
*/
extern fix32_t fix32_mod(fix32_t x, fix32_t y) FIXMATH_FUNC_ATTRS;



/* Returns the linear interpolation: (inArg0 * (1 - inFract)) + (inArg1 * inFract):
 */
extern fix32_t fix32_lerp8(fix32_t inArg0, fix32_t inArg1, uint8_t inFract) FIXMATH_FUNC_ATTRS;
extern fix32_t fix32_lerp16(fix32_t inArg0, fix32_t inArg1, uint16_t inFract) FIXMATH_FUNC_ATTRS;
extern fix32_t fix32_lerp32(fix32_t inArg0, fix32_t inArg1, uint32_t inFract) FIXMATH_FUNC_ATTRS;



/*! Returns the sine of the given fix32_t.
*/
extern fix32_t fix32_sin_parabola(fix32_t inAngle) FIXMATH_FUNC_ATTRS;

/*! Returns the sine of the given fix32_t.
*/
extern fix32_t fix32_sin(fix32_t inAngle) FIXMATH_FUNC_ATTRS;

/*! Returns the cosine of the given fix32_t.
*/
extern fix32_t fix32_cos(fix32_t inAngle) FIXMATH_FUNC_ATTRS;

/*! Returns the tangent of the given fix32_t.
*/
extern fix32_t fix32_tan(fix32_t inAngle) FIXMATH_FUNC_ATTRS;

/*! Returns the arcsine of the given fix32_t.
*/
extern fix32_t fix32_asin(fix32_t inValue) FIXMATH_FUNC_ATTRS;

/*! Returns the arccosine of the given fix32_t.
*/
extern fix32_t fix32_acos(fix32_t inValue) FIXMATH_FUNC_ATTRS;

/*! Returns the arctangent of the given fix32_t.
*/
extern fix32_t fix32_atan(fix32_t inValue) FIXMATH_FUNC_ATTRS;

/*! Returns the arctangent of inY/inX.
*/
extern fix32_t fix32_atan2(fix32_t inY, fix32_t inX) FIXMATH_FUNC_ATTRS;

static const fix32_t fix32_rad_to_deg_mult = 246101626061;
static inline fix32_t fix32_rad_to_deg(fix32_t radians)
	{ return fix32_mul(radians, fix32_rad_to_deg_mult); }

static const fix32_t fix32_deg_to_rad_mult = 74947179;
static inline fix32_t fix32_deg_to_rad(fix32_t degrees)
	{ return fix32_mul(degrees, fix32_deg_to_rad_mult); }



/*! Returns the square root of the given fix32_t.
*/
extern fix32_t fix32_sqrt(fix32_t inValue) FIXMATH_FUNC_ATTRS;

/*! Returns the square of the given fix32_t.
*/
static inline fix32_t fix32_sq(fix32_t x)
	{ return fix32_mul(x, x); }

/*! Returns the exponent (e^) of the given fix32_t.				<---------------------------------------  !!! Might give wrong returns values !!!
*/
extern fix32_t fix32_exp(fix32_t inValue) FIXMATH_FUNC_ATTRS;

/*! Returns the natural logarithm of the given fix32_t.
 */
extern fix32_t fix32_log(fix32_t inValue) FIXMATH_FUNC_ATTRS;

/*! Returns the base 2 logarithm of the given fix32_t.
 */
extern fix32_t fix32_log2(fix32_t x) FIXMATH_FUNC_ATTRS;

/*! Returns the saturated base 2 logarithm of the given fix32_t.
 */
extern fix32_t fix32_slog2(fix32_t x) FIXMATH_FUNC_ATTRS;

/*! Returns 2 raised to the specified power.
 */
extern fix32_t fix32_pow2(fix32_t x) FIXMATH_FUNC_ATTRS;

/*! Returns a specified number raised to the specified power.
 */
extern fix32_t fix32_pow(fix32_t b, fix32_t exp) FIXMATH_FUNC_ATTRS;

/*! Returns the saturated version of fix32_pow.
 */
extern fix32_t fix32_spow(fix32_t b, fix32_t exp) FIXMATH_FUNC_ATTRS;

#ifdef __cplusplus
}
#include "fix16.hpp"
#endif

#endif
