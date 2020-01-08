#include "fix32.h"

/* The square root algorithm is quite directly from
 * http://en.wikipedia.org/wiki/Methods_of_computing_square_roots#Binary_numeral_system_.28base_2.29
 * An important difference is that it is split to two parts
 * in order to use only 64-bit operations.
 *
 * Note that for negative numbers we return -sqrt(-inValue).
 * Not sure if someone relies on this behaviour, but not going
 * to break it for now. It doesn't slow the code much overall.
 */
fix32_t fix32_sqrt(fix32_t inValue)
{
	uint8_t  neg = (inValue < 0);
	uint64_t num = (neg ? -inValue : inValue);
	uint64_t result = 0;
	uint64_t bit;
	uint8_t  n;
	
	bit = (uint64_t)1 << 62;
	
	while (bit > num) bit >>= 2;
	
	// The main part is executed twice, in order to avoid
	// > 64 bit values in computations.
	for (n = 0; n < 2; n++)
	{
		// First we get the top 24 bits of the answer.
		while (bit)
		{
			if (num >= result + bit)
			{
				num -= result + bit;
				result = (result >> 1) + bit;
			}
			else
			{
				result = (result >> 1);
			}
			bit >>= 2;
		}
		
		if (n == 0)
		{
			// Then process it again to get the lowest bits.
			if (num > 4294967295)
			{
				// The remainder 'num' is too large to be shifted left
				// by 32, so we have to add 1 to result manually and
				// adjust 'num' accordingly.
				// num = a - (result + 0.5)^2
				//	 = num + result^2 - (result + 0.5)^2
				//	 = num - result - 0.5
				num -= result;
				num = (num << 32) - 0x80000000;
				result = (result << 32) + 0x80000000;
			}
			else
			{
				num <<= 32;
				result <<= 32;
			}
			
			bit = 1 << 30;
		}
	}

#ifndef FIXMATH_NO_ROUNDING
	// Finally, if next bit would have been 1, round the result upwards.
	if (num > result)
	{
		result++;
	}
#endif
	
	return (neg ? -(fix32_t)result : (fix32_t)result);
}
