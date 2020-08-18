//#include "../libfixmath/fix32.h"
//#include <stdio.h>
//#include <math.h>
//#include <stdbool.h>
//#include "unittests.h"
//
//#define delta(a,b) (((a)>=(b)) ? (a)-(b) : (b)-(a))
//
//int main()
//{
//    int status = 0;
//    {
//        COMMENT("Testing fix32_exp() corner cases");
//        TEST(fix32_exp(0) == fix32_one);
//        TEST(fix32_exp(fix32_minimum) == 0);
//        TEST(fix32_exp(fix32_maximum) == fix32_maximum);
//    }
//    
//    {
//        COMMENT("Testing fix32_exp() accuracy over -4..4");
//        
//        fix32_t max_delta = -1;
//        fix32_t worst = 0;
//        fix32_t sum = 0;
//        int count = 0;
//        fix32_t a;
//        
//        // the original implementation took forever to run this, so I made the 
//        // range a little shorter
//        for (a = fix32_from_dbl(-0.4); a < fix32_from_dbl(0.4); a += 63)
//        {
//            fix32_t result = fix32_exp(a);
//            fix32_t resultf = fix32_from_dbl(exp(fix32_to_dbl(a)));
//            
//            fix32_t d = delta(result, resultf);
//            if (d > max_delta)
//            {
//                max_delta = d;
//                worst = a;
//            }
//            
//            sum += d;
//            count++;
//        }
//        
//        printf("Worst delta %ld with input %ld\n", max_delta, worst);
//        printf("Average delta %0.2f\n", (float)sum / count);
//        
//        TEST(max_delta < 200);
//    }
//    
//    {
//        COMMENT("Testing fix32_exp() accuracy over full range");
//        
//        float max_delta = -1;
//        fix32_t worst = 0;
//        float sum = 0;
//        int count = 0;
//        fix32_t a;
//        
//        // Test the whole range of results 0..32768 with a bit less samples
//        for (a = -772243; a < 681391; a += 113)
//        {
//            fix32_t result = fix32_exp(a);
//            fix32_t resultf = fix32_from_dbl(exp(fix32_to_dbl(a)));
//            
//            fix32_t d1 = delta(result, resultf);
//            
//            if (d1 > 0) d1--; // Forgive +-1 for the fix32_t inaccuracy
//            
//            float d = (float)d1 / resultf * 100;
//            
//            if (resultf < 1000) continue; // Percentages can explode when result is almost 0.
//            
//            if (d > max_delta)
//            {
//                max_delta = d;
//                worst = a;
//            }
//            
//            sum += d;
//            count++;
//        }
//        
//        printf("Worst delta %0.4f%% with input %ld\n", max_delta, worst);
//        printf("Average delta %0.4f%%\n", sum / count);
//        
//        TEST(max_delta < 1);
//    }
//    
//    {
//        COMMENT("Testing fix32_log() accuracy over full range");
//        
//        fix32_t max_delta = -1;
//        fix32_t worst = 0;
//        fix32_t sum = 0;
//        int count = 0;
//        fix32_t a;
//        
//        for (a = 100; a > 0 && a < fix32_maximum - 7561; a += 7561)
//        {
//            fix32_t result = fix32_log(a);
//            fix32_t resultf = fix32_from_dbl(log(fix32_to_dbl(a)));
//            
//            fix32_t d = delta(result, resultf);
//            if (d > max_delta)
//            {
//                max_delta = d;
//                worst = a;
//            }
//            
//            sum += d;
//            count++;
//        }
//        
//        printf("Worst delta %ld with input %ld\n", max_delta, worst);
//        printf("Average delta %0.2f\n", (float)sum / count);
//        
//        TEST(max_delta < 20);
//    }
//    
//    if (status != 0)
//    fprintf(stdout, "\n\nSome tests FAILED!\n");
//
//    return status;
//}
