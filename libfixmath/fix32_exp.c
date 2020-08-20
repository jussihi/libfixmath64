#include "fix32.h"
#include <stdbool.h>


fix32_t fix32_exp(fix32_t inValue) {
	if(inValue == 0          ) return fix32_one;
	if(inValue == fix32_one  ) return fix32_e;
	if(inValue >= 92288378626I64) return fix32_maximum;	//fix32_from_dbl(ln(fix32_to_dbl(fix32_maximum))) = fix32_from_dbl(ln(2147483648)) = fix32_from_dbl(21.487562597) = 92288378625
	if(inValue <= -98242467570I64) return 0;			//fix32_from_dbl(ln(fix32_to_dbl(fix32_epsilon))) = fix32_from_dbl(ln(0.00000000023283064365)) = fix32_from_dbl(-22.1807097779) = -95265423098
														//fix32_from_dbl(ln(0.5*fix32_to_dbl(fix32_epsilon))) = fix32_from_dbl(ln(0.000000000116415321825)) = fix32_from_dbl(-22.87385) = -98242467570

                        
	/* The algorithm is based on the power series for exp(x):
	 * http://en.wikipedia.org/wiki/Exponential_function#Formal_definition
	 * 
	 * From term n, we get term n+1 by multiplying with x/n.
	 * When the sum term drops to zero, we can stop summing.
	 */
            
	// The power-series converges much faster on positive values
	// and exp(-x) = 1/exp(x).
	bool neg = (inValue < 0);
	if (neg) inValue = -inValue;

	fix32_t result = inValue + fix32_one;
	fix32_t term = inValue;

	uint_fast8_t i;
	for (i = 2; i < 30; i++)
	{
		term = fix32_mul(term, fix32_div(inValue, fix32_from_int(i)));
		result += term;

		if ((term < 500) && ((i > 15) || (term < 20)))
			break;
	}

	if (neg)
		result = fix32_div(fix32_one, result);

	return result;
}



fix32_t fix32_ln(fix32_t inValue)
{
	fix32_t guess = fix32_from_int(2);
	fix32_t delta;
	int scaling = 0;
	int count = 0;
	
	if (inValue <= 0)
		return fix32_minimum;
	
	// Bring the value to the most accurate range (1 < x < 100)
	const fix32_t e_to_fourth = 234497268814;
	while (inValue > fix32_from_int(100))
	{
		inValue = fix32_div(inValue, e_to_fourth);
		scaling += 4;
	}
	
	while (inValue < fix32_one)
	{
		inValue = fix32_mul(inValue, e_to_fourth);
		scaling -= 4;
	}
	
	do
	{
		// Solving e(x) = y using Newton's method
		// f(x) = e(x) - y
		// f'(x) = e(x)
		fix32_t e = fix32_exp(guess);
		delta = fix32_div(inValue - e, e);
		
		// It's unlikely that logarithm is very large, so avoid overshooting.
		if (delta > fix32_from_int(3))
			delta = fix32_from_int(3);
		
		guess += delta;
	} while ((count++ < 10)
		&& ((delta > 1) || (delta < -1)));
	
	return guess + fix32_from_int(scaling);
}



static inline fix32_t fix32_rs(fix32_t x)
{
	#ifdef FIXMATH_NO_ROUNDING
		return (x >> 1);
	#else
		fix32_t y = (x >> 1) + (x & 1);
		return y;
	#endif
}

/**
 * This assumes that the input value is >= 1.
 * 
 * Note that this is only ever called with inValue >= 1 (because it has a wrapper to check. 
 * As such, the result is always less than the input. 
 */
static fix32_t fix32__log2_inner(fix32_t x)
{
	fix32_t result = 0;
	
	while(x >= fix32_from_int(2))
	{
		result++;
		x = fix32_rs(x);
	}

	if(x == 0) return (result << 32);

	uint_fast8_t i;
	for(i = 32; i > 0; i--)
	{
		x = fix32_mul(x, x);
		result <<= 1;
		if(x >= fix32_from_int(2))
		{
			result |= 1;
			x = fix32_rs(x);
		}
	}
	#ifndef FIXMATH_NO_ROUNDING
		x = fix32_mul(x, x);
		if(x >= fix32_from_int(2)) result++;
	#endif
	
	return result;
}



/**
 * calculates the log base 2 of input.
 * Note that negative inputs are invalid! (will return fix32_overflow, since there are no exceptions)
 * 
 * i.e. 2 to the power output = input.
 * It's equivalent to the log or ln functions, except it uses base 2 instead of base 10 or base e.
 * This is useful as binary things like this are easy for binary devices, like modern microprocessros, to calculate.
 * 
 * This can be used as a helper function to calculate powers with non-integer powers and/or bases.
 */
fix32_t fix32_log2(fix32_t x)
{
	// Note that a negative x gives a non-real result.
	// If x == 0, the limit of log2(x)  as x -> 0 = -infinity.
	// log2(-ve) gives a complex result.
	if (x <= 0) return fix32_overflow;

	// If the input is less than one, the result is -log2(1.0 / in)
	if (x < fix32_one)
	{
		// Note that the inverse of this would overflow.
		// This is the exact answer for log2(1.0 / 4294967296)
		if (x == 1) return fix32_from_int(-32);

		fix32_t inverse = fix32_div(fix32_one, x);
		return -fix32__log2_inner(inverse);
	}

	// If input >= 1, just proceed as normal.
	// Note that x == fix32_one is a special case, where the answer is 0.
	return fix32__log2_inner(x);
}

/**
 * This is a wrapper for fix32_log2 which implements saturation arithmetic.
 */
fix32_t fix32_slog2(fix32_t x)
{
	fix32_t retval = fix32_log2(x);
	// The only overflow possible is when the input is negative.
	if(retval == fix32_overflow)
		return fix32_minimum;
	return retval;
}

fix32_t fix32_pow2(fix32_t x)
{
	const static fix32_t LN2 =	    0x00000000B17217F7;
	const static fix32_t Log2Max =  0x0000001F00000000;
	const static fix32_t Log2Min = -0x0000002000000000;

	if (x == 0)
		return fix32_one;

	// Avoid negative arguments by exploiting that exp(-x) = 1/exp(x).
	bool neg = x < 0;
	if (neg)
		x = -x;

	if (x == fix32_one)
	{
		return neg ? 0x0000000080000000 : 0x0000000200000000;
		//return neg ? fix32_from_double(0.5) : fix32_from_int(2);
	}
	if (x >= Log2Max)
	{
		return neg ? fix32_epsilon : fix32_maximum;
	}
	if (x <= Log2Min)
	{
		return neg ? fix32_maximum : 0;
	}

	/* The algorithm is based on the power series for exp(x):
	 * http://en.wikipedia.org/wiki/Exponential_function#Formal_definition
	 *
	 * From term n, we get term n+1 by multiplying with x/n.
	 * When the sum term drops to zero, we can stop summing.
	 */

	fix32_t int_part = fix32_floor(x);
	int integerPart = (int)(int_part >> 32);
	// Take fractional part of exponent
	x = x & 0x00000000FFFFFFFF;

	fix32_t result = fix32_one;
	fix32_t term = fix32_one;
	int i = 1;
	while (term != 0)
	{
		term = fix32_mul(term, fix32_mul(LN2, fix32_div(x, fix32_from_int(i))));
		result += term;
		i++;
	}

	result = result << integerPart;

	if (neg)
		result = fix32_div(fix32_one, result);

	return result;
}

fix32_t fix32_pow(fix32_t b, fix32_t exp)
{
	if (b == fix32_one || exp == 0)
		return fix32_one;

	if (b == 0)
		return 0;

	fix32_t log2 = fix32_log2(b);
	return fix32_pow2(fix32_mul(exp, log2));
}

fix32_t fix32_spow(fix32_t b, fix32_t exp)
{
	if (b == fix32_one || exp == 0)
		return fix32_one;

	if (b == 0)
		return 0;

	fix32_t log2 = fix32_slog2(b);
	return fix32_pow2(fix32_smul(exp, log2));
}