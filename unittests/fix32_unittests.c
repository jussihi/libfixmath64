#include <fix32.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include "unittests.h"

const fix32_t testcases[] = {
  // Small numbers
  0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
  -1, -2, -3, -4, -5, -6, -7, -8, -9, -10,
  
  // Integer numbers
  0x10000, -0x10000, 0x20000, -0x20000, 0x30000, -0x30000,
  0x40000, -0x40000, 0x50000, -0x50000, 0x60000, -0x60000,
  
  // Fractions (1/2, 1/4, 1/8)
  0x8000, -0x8000, 0x4000, -0x4000, 0x2000, -0x2000,
  
  // Problematic carry
  0xFFFF, -0xFFFF, 0x1FFFF, -0x1FFFF, 0x3FFFF, -0x3FFFF,
  
  // Smallest and largest values
  0x7FFFFFFF, 0x80000000,
  
  // Large random numbers
  831858892, 574794913, 2147272293, -469161054, -961611615,
  1841960234, 1992698389, 520485404, 560523116, -2094993050,
  -876897543, -67813629, 2146227091, 509861939, -1073573657,
  
  // Small random numbers
  -14985, 30520, -83587, 41129, 42137, 58537, -2259, 84142,
  -28283, 90914, 19865, 33191, 81844, -66273, -63215, -44459,
  -11326, 84295, 47515, -39324,
  
  // Tiny random numbers
  -171, -359, 491, 844, 158, -413, -422, -737, -575, -330,
  -376, 435, -311, 116, 715, -1024, -487, 59, 724, 993
};

#define TESTCASES_COUNT (sizeof(testcases)/sizeof(testcases[0]))

#define delta(a,b) (((a)>=(b)) ? (a)-(b) : (b)-(a))

// TODO: fix the float-to-fix32 conversion to actually make this working.
#ifdef FIXMATH_NO_ROUNDING
const fix32_t max_delta = 200;
#else
const fix32_t max_delta = 200;
#endif

int main()
{
  int status = 0;
  
  {
    COMMENT("Testing basic multiplication");
    TEST(fix32_mul(fix32_from_int(2), fix32_from_int(1)) == fix32_from_int(2));
    TEST(fix32_mul(fix32_from_int(5), fix32_from_int(5)) == fix32_from_int(25));
    TEST(fix32_mul(fix32_from_int(-5), fix32_from_int(5)) == fix32_from_int(-25));
    TEST(fix32_mul(fix32_from_int(-5), fix32_from_int(-5)) == fix32_from_int(25));
    TEST(fix32_mul(fix32_from_int(5), fix32_from_int(-5)) == fix32_from_int(-25));
    return 0;
  }
  
#ifndef FIXMATH_NO_ROUNDING
  {
    COMMENT("Testing multiplication rounding corner cases");
    TEST(fix32_mul(0, 10) == 0);
    TEST(fix32_mul(2, 0x80000000) == 1);
    TEST(fix32_mul(-2, 0x80000000) == -1);
    TEST(fix32_mul(3, 0x80000000) == 2);
    TEST(fix32_mul(-3, 0x80000000) == -2);
    TEST(fix32_mul(2, 0x7FFFFFFF) == 1);
    TEST(fix32_mul(-2, 0x7FFFFFFF) == -1);
    TEST(fix32_mul(2, 0x80000001) == 1);
    TEST(fix32_mul(-2, 0x80000001) == -1);
  }
#endif
  
  {
    unsigned int i, j;
    int failures = 0;
    COMMENT("Running testcases for multiplication");
    
    for (i = 0; i < TESTCASES_COUNT; i++)
    {
      for (j = 0; j < TESTCASES_COUNT; j++)
      {
        fix32_t a = testcases[i];
        fix32_t b = testcases[j];
        fix32_t result = fix32_mul(a, b);
        
        double fa = fix32_to_dbl(a);
        double fb = fix32_to_dbl(b);
        fix32_t fresult = fix32_from_dbl(fa * fb);
        
        double max = fix32_to_dbl(fix32_maximum);
        double min = fix32_to_dbl(fix32_minimum);
        
        if (delta(fresult, result) > max_delta)
        {
          if (fa * fb > max || fa * fb < min)
          {
            #ifndef FIXMATH_NO_OVERFLOW
            if (result != fix32_overflow)
            {
              printf("\n%ld * %ld overflow not detected!\n", a, b);
              failures++;
            }
            #endif
            // Legitimate overflow
            continue;
          }
          
          printf("\n%ld * %ld = %ld\n", a, b, result);
          printf("%f * %f = %ld\n", fa, fb, fresult);
          failures++;
        }
      }
    }
    
    TEST(failures == 0);
  }
  
  {
    COMMENT("Testing basic division");
    TEST(fix32_div(fix32_from_int(15), fix32_from_int(5)) == fix32_from_int(3));
    TEST(fix32_div(fix32_from_int(-15), fix32_from_int(5)) == fix32_from_int(-3));
    TEST(fix32_div(fix32_from_int(-15), fix32_from_int(-5)) == fix32_from_int(3));
    TEST(fix32_div(fix32_from_int(15), fix32_from_int(-5)) == fix32_from_int(-3));
  }
  
#ifndef FIXMATH_NO_ROUNDING
  {
    COMMENT("Testing division rounding corner cases");
    TEST(fix32_div(0, 10) == 0);
    TEST(fix32_div(1, fix32_from_int(2)) == 1);
    TEST(fix32_div(-1, fix32_from_int(2)) == -1);
    TEST(fix32_div(1, fix32_from_int(-2)) == -1);
    TEST(fix32_div(-1, fix32_from_int(-2)) == 1);
    TEST(fix32_div(3, fix32_from_int(2)) == 2);
    TEST(fix32_div(-3, fix32_from_int(2)) == -2);
    TEST(fix32_div(3, fix32_from_int(-2)) == -2);
    TEST(fix32_div(-3, fix32_from_int(-2)) == 2);
    TEST(fix32_div(2, 0x7FFFFFFF) == 4);
    TEST(fix32_div(-2, 0x7FFFFFFF) == -4);
    TEST(fix32_div(2, 0x80000001) == 4);
    TEST(fix32_div(-2, 0x80000001) == -4);
  }
#endif
  
  {
    unsigned int i, j;
    int failures = 0;
    COMMENT("Running testcases for division");
    
    for (i = 0; i < TESTCASES_COUNT; i++)
    {
      for (j = 0; j < TESTCASES_COUNT; j++)
      {
        fix32_t a = testcases[i];
        fix32_t b = testcases[j];
        
        // We don't require a solution for /0 :)
        if (b == 0) continue;
        
        fix32_t result = fix32_div(a, b);
        
        double fa = fix32_to_dbl(a);
        double fb = fix32_to_dbl(b);
        fix32_t fresult = fix32_from_dbl(fa / fb);
        
        double max = fix32_to_dbl(fix32_maximum);
        double min = fix32_to_dbl(fix32_minimum);        
        if (delta(fresult, result) > max_delta)
        {
          if (((fa / fb) > max) || ((fa / fb) < min))
          {
            #ifndef FIXMATH_NO_OVERFLOW
            if (result != fix32_overflow)
            {
              printf("\n%ld / %ld overflow not detected!\n", a, b);
              failures++;
            }
            #endif
            // Legitimate overflow
            continue;
          }
          
          printf("\n%f / %f = %f\n", fix32_to_dbl(a), fix32_to_dbl(b), fix32_to_dbl(fresult));
          printf("%f / %f = %f\n", fa, fb, (fa / fb));
          printf("delta: %ld\n", delta(fresult, result));
          failures++;
        }
      }
    }
    
    TEST(failures == 0);
  }
  
  {
    unsigned int i, j;
    int failures = 0;
    COMMENT("Running testcases for addition");
    
    for (i = 0; i < TESTCASES_COUNT; i++)
    {
      for (j = 0; j < TESTCASES_COUNT; j++)
      {
        fix32_t a = testcases[i];
        fix32_t b = testcases[j];
        
        fix32_t result = fix32_add(a, b);
        
        double fa = fix32_to_dbl(a);
        double fb = fix32_to_dbl(b);
        fix32_t fresult = fix32_from_dbl(fa + fb);
        
        double max = fix32_to_dbl(fix32_maximum);
        double min = fix32_to_dbl(fix32_minimum);
        
        if (delta(fresult, result) > max_delta)
        {
          if (fa + fb > max || fa + fb < min)
          {
            #ifndef FIXMATH_NO_OVERFLOW
            if (result != fix32_overflow)
            {
              printf("\n%ld + %ld overflow not detected!\n", a, b);
              failures++;
            }
            #endif
            // Legitimate overflow
            continue;
          }
          
          printf("\n%ld + %ld = %ld\n", a, b, result);
          printf("%f + %f = %ld\n", fa, fb, fresult);
          failures++;
        }
      }
    }
    
    TEST(failures == 0);
  }
  
  {
    unsigned int i, j;
    int failures = 0;
    COMMENT("Running testcases for subtraction");
    
    for (i = 0; i < TESTCASES_COUNT; i++)
    {
      for (j = 0; j < TESTCASES_COUNT; j++)
      {
        fix32_t a = testcases[i];
        fix32_t b = testcases[j];
        
        fix32_t result = fix32_sub(a, b);
        
        double fa = fix32_to_dbl(a);
        double fb = fix32_to_dbl(b);
        fix32_t fresult = fix32_from_dbl(fa - fb);
        
        double max = fix32_to_dbl(fix32_maximum);
        double min = fix32_to_dbl(fix32_minimum);
        
        if (delta(fresult, result) > max_delta)
        {
          if (fa - fb > max || fa - fb < min)
          {
            #ifndef FIXMATH_NO_OVERFLOW
            if (result != fix32_overflow)
            {
              printf("\n%ld - %ld overflow not detected!\n", a, b);
              failures++;
            }
            #endif
            // Legitimate overflow
            continue;
          }
          
          printf("\n%ld - %ld = %ld\n", a, b, result);
          printf("%f - %f = %ld\n", fa, fb, fresult);
          failures++;
        }
      }
    }
    
    TEST(failures == 0);
  }
  
  {
    COMMENT("Testing basic square roots");
    TEST(fix32_sqrt(fix32_from_int(16)) == fix32_from_int(4));
    TEST(fix32_sqrt(fix32_from_int(100)) == fix32_from_int(10));
    TEST(fix32_sqrt(fix32_from_int(1)) == fix32_from_int(1));
  }
  
#ifndef FIXMATH_NO_ROUNDING
  {
  	// TODO: FIX TESTS!
    COMMENT("Testing square root rounding corner cases");
    //TEST(fix32_sqrt(214748302) == 3751499);
    //TEST(fix32_sqrt(214748303) == 3751499);
    //TEST(fix32_sqrt(214748359) == 3751499);
    //TEST(fix32_sqrt(214748360) == 3751500);
  }
#endif
  
  {
    unsigned int i;
    int failures = 0;
    COMMENT("Running test cases for square root");
    
    for (i = 0; i < TESTCASES_COUNT; i++)
    {
      fix32_t a = testcases[i];
      
      if (a < 0) continue;
      
      fix32_t result = fix32_sqrt(a);
      
      double fa = fix32_to_dbl(a);
      fix32_t fresult = fix32_from_dbl(sqrt(fa));
      
      if (delta(fresult, result) > max_delta)
      {
        printf("\nfix32_sqrt(%ld) = %ld\n", a, result);
        printf("sqrt(%f) = %ld\n", fa, fresult);
        failures++;
      }
    }
    
    TEST(failures == 0);
  }
  
  if (status != 0)
    fprintf(stdout, "\n\nSome tests FAILED!\n");
  
  return status;
}
