#include "../libfixmath/fix32.h"
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include "unittests.h"
#include <math.h>
#include <inttypes.h>

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

#ifdef FIXMATH_NO_ROUNDING
const fix32_t max_delta = 1;
#else
const fix32_t max_delta = 0;
#endif


void PrintFloat(float f)
{
	printf("%f \n", f);
}
void PrintFix64f(fix32_t fx)
{
	float f = fix32_to_float(fx);
	printf("%f \n", f);
}
void PrintFix64(fix32_t fx)
{
	double f = fix32_to_dbl(fx);
	printf("%f \n", f);
}

void PrintFix64_Long(fix32_t fx)
{
	double f = fix32_to_dbl(fx);
	printf("%.20f \n", f);
}

int main()
{
  int status = 0;

  {
	  COMMENT("Testing basic float");
	  //printf("%#018"PRIx64"\n", fix32_from_dbl(0.5));
	  printf("%#018"PRIx64"\n", fix32_from_dbl(1/3.141592653589));
	  PrintFix64_Long(fix32_epsilon);
	  PrintFix64(fix32_maximum);
	  PrintFix64(fix32_minimum);
  }

  {
	  COMMENT("Testing math operation");
	  TEST_DOUBLE_ERROR(fix32_pow2(fix32_from_dbl(24.4894616)), fix32_from_dbl(23553883.31583202), 0.01f);
	  TEST_DOUBLE_ERROR(fix32_pow(fix32_from_dbl(3.456546), fix32_from_dbl(11.246556)), fix32_from_dbl(1142539.57167091), 0.01f);
	  TEST_DOUBLE_ERROR(fix32_spow(fix32_from_dbl(312.456546), fix32_from_dbl(11.246556)), fix32_maximum, 0.01f);
	  TEST_DOUBLE_ERROR(fix32_spow(fix32_from_dbl(-312.456546), fix32_from_dbl(1100.246556)), fix32_minimum, 0.01f);
	  TEST_DOUBLE_ERROR(fix32_sqrt(fix32_from_dbl(3661.56684)), fix32_from_dbl(60.51088199), 0.01f);
	  TEST_DOUBLE_ERROR(fix32_exp(fix32_from_dbl(3.56684)), fix32_from_dbl(35.40453785787), 0.01f);
	  TEST_DOUBLE_ERROR(fix32_ln(fix32_from_dbl(45.24)), fix32_from_dbl(3.81198165), 0.01f);
	  TEST_DOUBLE_ERROR(fix32_log2(fix32_from_dbl(45.24)), fix32_from_dbl(5.499527), 0.01f);
	  TEST_DOUBLE_ERROR(fix32_slog2(fix32_from_dbl(-43.0)), fix32_minimum, 0.01f);
  }

  //{
	 // COMMENT("Testing overflow float");
	 // TEST_DOUBLE_ERROR(fix32_add(fix32_maximum, fix32_one), fix32_maximum, 0.01f);
	 // TEST_DOUBLE_ERROR(fix32_mul(fix32_from_dbl(3661.56684), fix32_from_dbl(89378.654563)), fix32_overflow, 0.01f);
	 // TEST_DOUBLE_ERROR(fix32_div(fix32_from_dbl(3661.56684), fix32_from_dbl(89378.654563)), fix32_overflow, 0.01f);
  //}

  {
	  COMMENT("Testing lerp");
	  TEST_DOUBLE_ERROR(fix32_lerp8(fix32_from_dbl(0.0), fix32_from_dbl(100.0), 76), fix32_from_dbl(29.8), 0.01f);
	  TEST_DOUBLE_ERROR(fix32_lerp16(fix32_from_dbl(0.0), fix32_from_dbl(100.0), 37484), fix32_from_dbl(57.196044921875), 0.01f);
	  TEST_DOUBLE_ERROR(fix32_lerp32(fix32_from_dbl(0.0), fix32_from_dbl(100.0), 4698323), fix32_from_dbl(0.109391356818377971649169921875), 0.01f);
  }

  {
	  COMMENT("Testing basic multiplication");
	  TEST(fix32_mul(fix32_from_int(2), fix32_from_int(1)) == fix32_from_int(2));
	  TEST(fix32_mul(fix32_from_int(5), fix32_from_int(5)) == fix32_from_int(25));
	  TEST(fix32_mul(fix32_from_int(-5), fix32_from_int(5)) == fix32_from_int(-25));
	  TEST(fix32_mul(fix32_from_int(-5), fix32_from_int(-5)) == fix32_from_int(25));
	  TEST(fix32_mul(fix32_from_int(5), fix32_from_int(-5)) == fix32_from_int(-25));
  }

  {
	  COMMENT("Testing trigonometric functions");
	  const int TRIG_TEST_SAMPLES = 10000;
	  double max_err = 0;
	  double max_err_angle = 0;
	  for (int i = 0; i < TRIG_TEST_SAMPLES; ++i)
	  {
		  double angle = 3.1415926 * 2 * (double)i / (double)TRIG_TEST_SAMPLES;
		  double dResult = sin(angle);
		  fix32_t fResult = fix32_sin(fix32_from_dbl(angle));
		  double err = fabs(fix32_to_dbl(fResult) - dResult);
		  if (err > max_err)
		  {
			  max_err = err;
			  max_err_angle = angle;
		  }
	  }
	  printf("[sin]: max error: %.10f, when angle = %.10f\n", max_err, max_err_angle);

	  max_err = 0;
	  max_err_angle = 0;
	  for (int i = 0; i < TRIG_TEST_SAMPLES; ++i)
	  {
		  double angle = 3.1415926 * 2 * (double)i / (double)TRIG_TEST_SAMPLES;
		  double dResult = cos(angle);
		  fix32_t fResult = fix32_cos(fix32_from_dbl(angle));
		  double err = fabs(fix32_to_dbl(fResult) - dResult);
		  if (err > max_err)
		  {
			  max_err = err;
			  max_err_angle = angle;
		  }
	  }
	  printf("[cos]: max error: %.10f, when angle = %.10f\n", max_err, max_err_angle);

	  max_err = 0;
	  max_err_angle = 0;
	  for (int i = 0; i < TRIG_TEST_SAMPLES; ++i)
	  {
		  double angle = -3.141 * 0.5 + 3.1415926 * (double)i / (double)(TRIG_TEST_SAMPLES+10);
		  double dResult = tan(angle);
		  fix32_t fResult = fix32_tan(fix32_from_dbl(angle));
		  double err = fabs(fix32_to_dbl(fResult) - dResult);
		  if (err > max_err)
		  {
			  max_err = err;
			  max_err_angle = angle;
		  }
	  }
	  printf("[tan]: max error: %.10f, when angle = %.10f\n", max_err, max_err_angle);

	  max_err = 0;
	  max_err_angle = 0;
	  for (int i = 0; i < TRIG_TEST_SAMPLES; ++i)
	  {
		  double angle = -1 + 2.0 * (double)i / (double)TRIG_TEST_SAMPLES;
		  double dResult = asin(angle);
		  fix32_t fResult = fix32_asin(fix32_from_dbl(angle));
		  double err = fabs(fix32_to_dbl(fResult) - dResult);
		  if (err > max_err)
		  {
			  max_err = err;
			  max_err_angle = angle;
		  }
	  }
	  printf("[asin]: max error: %.10f, when value = %.10f\n", max_err, max_err_angle);

	  max_err = 0;
	  max_err_angle = 0;
	  for (int i = 0; i < TRIG_TEST_SAMPLES; ++i)
	  {
		  double angle = -1 + 2.0 * (double)i / (double)TRIG_TEST_SAMPLES;
		  double dResult = acos(angle);
		  fix32_t fResult = fix32_acos(fix32_from_dbl(angle));
		  double err = fabs(fix32_to_dbl(fResult) - dResult);
		  if (err > max_err)
		  {
			  max_err = err;
			  max_err_angle = angle;
		  }
	  }
	  printf("[acos]: max error: %.10f, when value = %.10f\n", max_err, max_err_angle);
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
          
          printf("\n%.15f / %.15f = %.25f\n", fix32_to_dbl(a), fix32_to_dbl(b), fix32_to_dbl(result));
          printf("%.15f / %.15f = %.25f\n", fa, fb, (fa / fb));
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
