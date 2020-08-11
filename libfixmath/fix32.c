#include "fix32.h"
#include "int128.h"

#ifdef __GNUC__
// Count leading zeros, using processor-specific instruction if available.
#define clz(x) (__builtin_clzl(x) - (8 * sizeof(long) - 64))
#else
static uint8_t clz(uint64_t x)
{
	uint8_t result = 0;
	if (x == 0) return 64;
	while (!(x & 0xF000000000000000)) { result += 4; x <<= 4; }
	while (!(x & 0x8000000000000000)) { result += 1; x <<= 1; }
	return result;
}
#endif

/* Binary conversion functions for machines without 
 * FPO but somehow access to IEEE745 float binary data
 */
fix32_t fix32_from_float_bin(const void* value)
{
	int64_t fi = *(int64_t *)value;
	// get the fraction
	fix32_t q3232 = (fi & ((1 << 23) - 1)) | 1 << 23;
	// get the exponent
	int64_t expon = ((fi >> 23) & ((1 << 8) - 1)) - 127;
	// get the shift
	int32_t shift = 32 + expon - 23;
	// get the sign
	int64_t sign = (fi >> 31) & 1;

	if (shift >= 32 || shift <= -32)
	{
		q3232 = 0;
	}
	else
	{
		if (shift < 0)
		{
			q3232 >>= -shift;
		}
		else
		{
			q3232 <<= shift;
		}
	}

	if (sign)
	{
		int64_t int_part = (int32_t)((q3232 & 0xFFFFFFFF00000000) >> 32);
		int_part *= -1;

		uint32_t frac_part = (uint32_t)(q3232 & 0x00000000FFFFFFFF);
		if (frac_part)
		{
			frac_part = 0xFFFFFFFF - frac_part;
			q3232 &= 0xFFFFFFFF00000000;
			q3232 ^= frac_part;
			int_part -= 1;
		}
		q3232 &= 0x00000000FFFFFFFF;
		q3232 ^= int_part << 32;
	}

	return q3232;
}

static int32_t count_bits_pow2(uint64_t x)
{
	int32_t l = -33;
	for (uint64_t i = 0; i < 64; i++)
	{
		uint64_t test = 1ULL << i;
		if (x >= test)
		{
			l++;
		}
		else
		{
			break;
		}
	}
	return l;
}

uint32_t float_from_fix32_bin(fix32_t value)
{
	int64_t original_num = (int64_t)value;
	uint64_t sign = 0;
	if (original_num < 0)
	{
		sign = 1;
	}

	// remove the signed bit if it's set
	int64_t unsigned_ver = original_num < 0 ? -original_num : original_num;

	// calculate mantissa
	int lz = clz(unsigned_ver);
	uint64_t y = unsigned_ver << (lz + 1);

	// 33 --> because we use 64-bit fixed point num, the middle of it is at 32,
	// and +1 for the sign bit. Then 8 is the exponent bits, which is 8 for IEEE754
	uint64_t mantissa = y >> (33 + 8);

	// get the non-fractal bits, add the exponent bias ( 127 in IEEE754 ) to get exponent
	// uint64_t non_fractal = (unsigned_ver >> 32);
	uint64_t exp = count_bits_pow2(unsigned_ver) + 127;

	// construct the final IEEE754 float binary number
	// first add the last 23 bits (mantissa)
	uint32_t ret = mantissa;

	// add exponent
	ret |= (exp << 23);

	// add the sign if needed
	if (sign)
	{
		ret |= 0x80000000;
	}

	return ret;
}


/* Subtraction and addition with overflow detection.
 * The versions without overflow detection are inlined in the header.
 */
#ifndef FIXMATH_NO_OVERFLOW
fix32_t fix32_add(fix32_t a, fix32_t b)
{
	// Use unsigned integers because overflow with signed integers is
	// an undefined operation (http://www.airs.com/blog/archives/120).
	uint64_t _a = a, _b = b;
	uint64_t sum = _a + _b;

	// Overflow can only happen if sign of a == sign of b, and then
	// it causes sign of sum != sign of a.
	if (!((_a ^ _b) & 0x8000000000000000) && ((_a ^ sum) & 0x8000000000000000))
		return fix32_overflow;
	
	return sum;
}

fix32_t fix32_sub(fix32_t a, fix32_t b)
{
	uint64_t _a = a, _b = b;
	uint64_t diff = _a - _b;

	// Overflow can only happen if sign of a != sign of b, and then
	// it causes sign of diff != sign of a.
	if (((_a ^ _b) & 0x8000000000000000) && ((_a ^ diff) & 0x8000000000000000))
		return fix32_overflow;
	
	return diff;
}

/* Saturating arithmetic */
fix32_t fix32_sadd(fix32_t a, fix32_t b)
{
	fix32_t result = fix32_add(a, b);

	if (result == fix32_overflow)
		return (a >= 0) ? fix32_maximum : fix32_minimum;

	return result;
}	

fix32_t fix32_ssub(fix32_t a, fix32_t b)
{
	fix32_t result = fix32_sub(a, b);

	if (result == fix32_overflow)
		return (a >= 0) ? fix32_maximum : fix32_minimum;

	return result;
}
#endif



/* 64-bit implementation for fix32_mul. Simplest way to implement on a x64 
 * machines, and I don't have any need for different versions personally. If 
 * you do, please share. This is fast enough for me.
 */
fix32_t fix32_mul(fix32_t inArg0, fix32_t inArg1)
{
	// Each argument is divided to 32-bit parts.
	//					AB
	//			*	 CD
	// -----------
	//					BD	32 * 32 -> 64 bit products
	//				 CB
	//				 AD
	//				AC
	//			 |----| 64 bit product
	int64_t A = (inArg0 >> 32), C = (inArg1 >> 32);
	uint64_t B = (inArg0 & 0xFFFFFFFF), D = (inArg1 & 0xFFFFFFFF);
	
	int64_t AC = A*C;
	int64_t AD_CB = A*D + C*B;
	uint64_t BD = B*D;
	
	int64_t product_hi = AC + (AD_CB >> 32);
	
	// Handle carry from lower 32 bits to upper part of result.
	uint64_t ad_cb_temp = AD_CB << 32;
	uint64_t product_lo = BD + ad_cb_temp;
	if (product_lo < BD)
		product_hi++;
	
#ifndef FIXMATH_NO_OVERFLOW
	// The upper 17 bits should all be the same (the sign).
	if (product_hi >> 63 != product_hi >> 31)
		return fix32_overflow;
#endif
	
#ifdef FIXMATH_NO_ROUNDING
	return (product_hi << 32) | (product_lo >> 32);
#else
	// Subtracting 0x80000000 (= 0.5) and then using signed right shift
	// achieves proper rounding to result-1, except in the corner
	// case of negative numbers and lowest word = 0x80000000.
	// To handle that, we also have to subtract 1 for negative numbers.
	uint64_t product_lo_tmp = product_lo;
	product_lo -= 0x80000000;
	product_lo -= (uint64_t)product_hi >> 63;
	if (product_lo > product_lo_tmp)
		product_hi--;
	
	// Discard the lowest 16 bits. Note that this is not exactly the same
	// as dividing by 0x10000. For example if product = -1, result will
	// also be -1 and not 0. This is compensated by adding +1 to the result
	// and compensating this in turn in the rounding above.
	fix32_t result = (product_hi << 32) | (product_lo >> 32);
	result += 1;
	return result;
#endif
}


#ifndef FIXMATH_NO_OVERFLOW
/* Wrapper around fix32_mul to add saturating arithmetic. */
fix32_t fix32_smul(fix32_t inArg0, fix32_t inArg1)
{
	fix32_t result = fix32_mul(inArg0, inArg1);
	
	if (result == fix32_overflow)
	{
		if ((inArg0 >= 0) == (inArg1 >= 0))
			return fix32_maximum;
		else
			return fix32_minimum;
	}
	
	return result;
}
#endif

/*
 * 64-bit implementation of fix32_div. Only implemented this
 * one, if you need a 8-bit optimized version, please create 
 * and share.
 */
#if !defined(FIXMATH_OPTIMIZE_8BIT)

fix32_t fix32_div(fix32_t a, fix32_t b)
{
	// This uses a hardware 64/64 bit division multiple times, until we have
	// computed all the bits in (a<<33)/b. Usually this takes 1-3 iterations.
	
	if (b == 0)
			return fix32_minimum;
	
	uint64_t remainder = (a >= 0) ? a : (-a);
	uint64_t divider = (b >= 0) ? b : (-b);
	uint64_t quotient = 0;
	int bit_pos = 33;
	
	// Kick-start the division a bit.
	// This improves speed in the worst-case scenarios where N and D are large
	// It gets a lower estimate for the result by N/(D >> 33 + 1).
	if (divider & 0xFFF0000000000000)
	{
		uint64_t shifted_div = ((divider >> 33) + 1);
		quotient = remainder / shifted_div;
		remainder -= ((uint64_t)quotient * divider) >> 17;
	}
	
	// If the divider is divisible by 2^n, take advantage of it.
	while (!(divider & 0xF) && bit_pos >= 4)
	{
		divider >>= 4;
		bit_pos -= 4;
	}
	
	while (remainder && bit_pos >= 0)
	{
		// Shift remainder as much as we can without overflowing
		int shift = clz(remainder);
		if (shift > bit_pos) shift = bit_pos;
		remainder <<= shift;
		bit_pos -= shift;
		
		uint64_t div = remainder / divider;
		remainder = remainder % divider;
		quotient += div << bit_pos;

		#ifndef FIXMATH_NO_OVERFLOW
		if (div & ~(0xFFFFFFFFFFFFFFFF >> bit_pos))
				return fix32_overflow;
		#endif
		
		remainder <<= 1;
		bit_pos--;
	}
	
	#ifndef FIXMATH_NO_ROUNDING
	// Quotient is always positive so rounding is easy
	quotient++;
	#endif
	
	fix32_t result = quotient >> 1;
	
	// Figure out the sign of the result
	if ((a ^ b) & 0x8000000000000000)
	{
		#ifndef FIXMATH_NO_OVERFLOW
		if (result == fix32_minimum)
				return fix32_overflow;
		#endif
		
		result = -result;
	}
	
	return result;
}
#endif


#ifndef FIXMATH_NO_OVERFLOW
/* Wrapper around fix32_div to add saturating arithmetic. */
fix32_t fix32_sdiv(fix32_t inArg0, fix32_t inArg1)
{
	fix32_t result = fix32_div(inArg0, inArg1);
	
	if (result == fix32_overflow)
	{
		if ((inArg0 >= 0) == (inArg1 >= 0))
			return fix32_maximum;
		else
			return fix32_minimum;
	}
	
	return result;
}
#endif

fix32_t fix32_mod(fix32_t x, fix32_t y)
{
	#ifdef FIXMATH_OPTIMIZE_8BIT
		/* The reason we do this, rather than use a modulo operator
		 * is that if you don't have a hardware divider, this will result
		 * in faster operations when the angles are close to the bounds. 
		 */
		while(x >=  y) x -= y;
		while(x <= -y) x += y;
	#else
		/* Note that in C90, the sign of result of the modulo operation is
		 * undefined. in C99, it's the same as the dividend (aka numerator).
		 */
		x %= y;
	#endif

	return x;
}



fix32_t fix32_lerp8(fix32_t inArg0, fix32_t inArg1, uint8_t inFract)
{
	int128_t tempOut = int128_mul_i64_i64(inArg0, ((1 << 8) - inFract));
	tempOut = int128_add(tempOut, int128_mul_i64_i64(inArg1, inFract));
	tempOut = int128_shift(tempOut, -8);
	return (fix32_t)int128_lo(tempOut);
}

fix32_t fix32_lerp16(fix32_t inArg0, fix32_t inArg1, uint16_t inFract)
{
	int128_t tempOut = int128_mul_i64_i64(inArg0, (((int64_t)1 << 16) - inFract));
	tempOut = int128_add(tempOut, int128_mul_i64_i64(inArg1, inFract));
	tempOut = int128_shift(tempOut, -16);
	return (fix32_t)int128_lo(tempOut);
}

fix32_t fix32_lerp32(fix32_t inArg0, fix32_t inArg1, uint32_t inFract)
{
	int128_t tempOut;
	tempOut = ((int128_t)inArg0 * (0 - inFract));
	tempOut += ((int128_t)inArg1 * inFract);
	tempOut >>= 32;
	return (fix32_t)tempOut;
}