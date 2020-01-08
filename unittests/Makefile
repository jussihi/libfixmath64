# Makefile for running the unittests of libfixmath.
CC = gcc

# Basic CFLAGS for debugging
CFLAGS = -g -O0 -I../libfixmath -Wall -Wextra -Werror

# The files required for tests
FIX32_SRC = ../libfixmath/fix32.c ../libfixmath/fix32_sqrt.c ../libfixmath/fix32_str.c \
	../libfixmath/fix32_exp.c ../libfixmath/fix32.h

all: run_fix32_unittests run_fix32_exp_unittests run_fix32_str_unittests

clean:
	rm -f fix32_unittests_????
	rm -f fix32_str_unittests_default
	rm -f fix32_str_unittests_no_ctype
	rm -f fix32_exp_unittests

# The library is tested automatically under different compilations
# options.
#
# Test naming:
# r = rounding, n = no rounding
# o = overflow detection, n = no overflow detection
# 64 = int64_t math, 32 = int32_t math

run_fix32_unittests: \
	fix32_unittests_ro64 fix32_unittests_no64 \
	fix32_unittests_rn64 fix32_unittests_nn64
	$(foreach test, $^, \
	echo $(test) && \
	./$(test) > /dev/null && \
	) true

fix32_unittests_no64: DEFINES=-DFIXMATH_NO_ROUNDING
fix32_unittests_rn64: DEFINES=-DFIXMATH_NO_OVERFLOW
fix32_unittests_nn64: DEFINES=-DFIXMATH_NO_ROUNDING -DFIXMATH_NO_OVERFLOW
fix32_str_unittests_no_ctype: DEFINES=-DFIXMATH_NO_CTYPE

fix32_unittests_% : fix32_unittests.c $(FIX32_SRC)
	$(CC) $(CFLAGS) $(DEFINES) -o $@ $^ -lm


# Tests for the exponential function, run only in default config
run_fix32_exp_unittests: fix32_exp_unittests
	./fix32_exp_unittests > /dev/null

fix32_exp_unittests: fix32_exp_unittests.c $(FIX32_SRC)
	$(CC) $(CFLAGS) $(DEFINES) -o $@ $^ -lm

# Tests for string conversion, run only in default config and no ctype
run_fix32_str_unittests: fix32_str_unittests_default fix32_str_unittests_no_ctype
	./fix32_str_unittests_default > /dev/null
	./fix32_str_unittests_no_ctype > /dev/null

fix32_str_unittests_%: fix32_str_unittests.c $(FIX32_SRC)
	$(CC) $(CFLAGS) $(DEFINES) -o $@ $^ -lm

# Tests for literal macros, run only in default config
run_fix32_macros_unittests: fix32_macros_unittests
	./fix32_macros_unittests > /dev/null

fix32_macros_unittests: fix32_macros_unittests.c $(FIX32_SRC)
	$(CC) $(CFLAGS) $(DEFINES) -o $@ $^ -lm

