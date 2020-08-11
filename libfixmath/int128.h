#ifndef __libfixmath_int64_h__
#define __libfixmath_int64_h__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

/* No int128_t support
#ifndef FIXMATH_NO_64BIT
static inline  int64_t int64_const(int32_t hi, uint32_t lo) { return (((int64_t)hi << 32) | lo); }
static inline  int64_t int64_from_int32(int32_t x) { return (int64_t)x; }
static inline  int32_t int64_hi(int64_t x) { return (x >> 32); }
static inline uint32_t int64_lo(int64_t x) { return (x & ((1ULL << 32) - 1)); }

static inline int64_t int64_add(int64_t x, int64_t y)   { return (x + y);  }
static inline int64_t int64_neg(int64_t x)              { return (-x);     }
static inline int64_t int64_sub(int64_t x, int64_t y)   { return (x - y);  }
static inline int64_t int64_shift(int64_t x, int8_t y)  { return (y < 0 ? (x >> -y) : (x << y)); }

static inline int64_t int64_mul_i32_i32(int32_t x, int32_t y) { return (x * y);  }
static inline int64_t int64_mul_i64_i32(int64_t x, int32_t y) { return (x * y);  }

static inline int64_t int64_div_i64_i32(int64_t x, int32_t y) { return (x / y);  }

static inline int int64_cmp_eq(int64_t x, int64_t y) { return (x == y); }
static inline int int64_cmp_ne(int64_t x, int64_t y) { return (x != y); }
static inline int int64_cmp_gt(int64_t x, int64_t y) { return (x >  y); }
static inline int int64_cmp_ge(int64_t x, int64_t y) { return (x >= y); }
static inline int int64_cmp_lt(int64_t x, int64_t y) { return (x <  y); }
static inline int int64_cmp_le(int64_t x, int64_t y) { return (x <= y); }
#else*/

typedef struct {
	 int64_t hi;
	uint64_t lo;
} _int128_t;

static inline _int128_t int128_const(int64_t hi, uint64_t lo) { return (_int128_t){ hi, lo }; }
static inline _int128_t int128_from_int64(int64_t x) { return (_int128_t){ (x < 0 ? -1 : 0), x }; }
static inline   int64_t int128_hi(_int128_t x) { return x.hi; }
static inline  uint64_t int128_lo(_int128_t x) { return x.lo; }

static inline int int128_cmp_eq(_int128_t x, _int128_t y) { return ((x.hi == y.hi) && (x.lo == y.lo)); }
static inline int int128_cmp_ne(_int128_t x, _int128_t y) { return ((x.hi != y.hi) || (x.lo != y.lo)); }
static inline int int128_cmp_gt(_int128_t x, _int128_t y) { return ((x.hi > y.hi) || ((x.hi == y.hi) && (x.lo >  y.lo))); }
static inline int int128_cmp_ge(_int128_t x, _int128_t y) { return ((x.hi > y.hi) || ((x.hi == y.hi) && (x.lo >= y.lo))); }
static inline int int128_cmp_lt(_int128_t x, _int128_t y) { return ((x.hi < y.hi) || ((x.hi == y.hi) && (x.lo <  y.lo))); }
static inline int int128_cmp_le(_int128_t x, _int128_t y) { return ((x.hi < y.hi) || ((x.hi == y.hi) && (x.lo <= y.lo))); }

static inline _int128_t int128_add(_int128_t x, _int128_t y) {
	_int128_t ret;
	ret.hi = x.hi + y.hi;
	ret.lo = x.lo + y.lo;
	if((ret.lo < x.lo) || (ret.hi < y.hi))
		ret.hi++;
	return ret;
}

static inline _int128_t int128_neg(_int128_t x) {
	_int128_t ret;
	ret.hi = ~x.hi;
	ret.lo = ~x.lo + 1;
	if(ret.lo == 0)
		ret.hi++;
	return ret;
}

static inline _int128_t int128_sub(_int128_t x, _int128_t y) {
	return int128_add(x, int128_neg(y));
}

static inline _int128_t int128_shift(_int128_t x, int8_t y) {
	_int128_t ret;
	if(y > 0) {
		if(y >= 64)
			return (_int128_t){ 0, 0 };
		ret.hi = (x.hi << y) | (x.lo >> (64 - y));
		ret.lo = (x.lo << y);
	} else {
		y = -y;
		if(y >= 64)
			return (_int128_t){ 0, 0 };
		ret.lo = (x.lo >> y) | (x.hi << (64 - y));
		ret.hi = (x.hi >> y);
	}
	return ret;
}

static inline _int128_t int128_mul_i64_i64(int64_t x, int64_t y) {
	 int32_t hi[2] = { (x >> 32), (y >> 32) };
	uint32_t lo[2] = { (x & 0xFFFFFFFF), (y & 0xFFFFFFFF) };

	 int64_t r_hi = hi[0] * hi[1];
	 int64_t r_md = (hi[0] * lo[1]) + (hi[1] * lo[0]);
	uint64_t r_lo = lo[0] * lo[1];

	r_hi += (r_md >> 32);
	r_lo += (r_md << 32);

	return (_int128_t){ r_hi, r_lo };
}

static inline _int128_t int128_mul_i64_i32(_int128_t x, int64_t y) {
	int neg = ((x.hi ^ y) < 0);
	if(x.hi < 0)
		x = int128_neg(x);
	if(y < 0)
		y = -y;

	uint64_t _x[4] = { (x.hi >> 32), (x.hi & 0xFFFFFFFF), (x.lo >> 32), (x.lo & 0xFFFFFFFF) };
	uint64_t _y[2] = { (y >> 32), (y & 0xFFFFFFFF) };

	uint64_t r[4];
	r[0] = (_x[0] * _y[0]);
	r[1] = (_x[1] * _y[0]) + (_x[0] * _y[1]);
	r[2] = (_x[1] * _y[1]) + (_x[2] * _y[0]);
	r[3] = (_x[2] * _y[0]) + (_x[1] * _y[1]);

	_int128_t ret;
	ret.lo = r[0] + (r[1] << 32);
	ret.hi = (r[3] << 32) + r[2] + (r[1] >> 32);
	return (neg ? int128_neg(ret) : ret);
}

static inline _int128_t int128_div_i128_i64(_int128_t x, int64_t y) {
	int neg = ((x.hi ^ y) < 0);
	if(x.hi < 0)
		x = int128_neg(x);
	if(y < 0)
		y = -y;

	_int128_t ret = { (x.hi / y) , (x.lo / y) };
	x.hi = x.hi % y;
	x.lo = x.lo % y;

	_int128_t _y = int128_from_int64(y);

	_int128_t i;
	for(i = int128_from_int64(1); int128_cmp_lt(_y, x); _y = int128_shift(_y, 1), i = int128_shift(i, 1));

	while(x.hi) {
		_y = int128_shift(_y, -1);
		 i = int128_shift(i, -1);
		if(int64_cmp_ge(x, _y)) {
			x = int128_sub(x, _y);
			ret = int128_add(ret, i);
		}
	}

	ret = int128_add(ret, int128_from_int64(x.lo / y));
	return (neg ? int128_neg(ret) : ret);
}

#define int128_t _int128_t

//#endif

#ifdef __cplusplus
}
#endif

#endif
