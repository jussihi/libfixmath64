#include <stdio.h>

#define COMMENT(x) printf("\n----" x "----\n");
#define STR(x) #x
#define STR2(x) STR(x)
#define TEST(x) \
    if (!(x)) { \
        fflush(stdout); \
        fflush(stderr); \
        fprintf(stderr, "\033[31;1mFAILED:\033[22;39m " __FILE__ ":" STR2(__LINE__) " " #x "\n"); \
        status = 1; \
    } else { \
        fflush(stdout); \
        fflush(stderr); \
        printf("\033[32;1mOK:\033[22;39m " #x "\n"); \
    }

#define TEST_FLOAT_ERROR(x, result, thres) \
    if (abs(fix32_to_float(result) - fix32_to_float((x))) > thres) { \
        fflush(stdout); \
        fflush(stderr); \
        fprintf(stderr, "\033[31;1mFAILED:\033[22;39m " __FILE__ ":" STR2(__LINE__) " " #x "\n"); \
        status = 1; \
    } else { \
        fflush(stdout); \
        fflush(stderr); \
        printf("\033[32;1mOK:\033[22;39m " #x " = %f \n", fix32_to_float((x))); \
    }

#define TEST_DOUBLE_ERROR(x, result, thres) \
    if (abs(fix32_to_dbl(result) - fix32_to_dbl((x))) > thres) { \
        fflush(stdout); \
        fflush(stderr); \
        fprintf(stderr, "\033[31;1mFAILED:\033[22;39m " __FILE__ ":" STR2(__LINE__) " " #x " = %f != %f\n", fix32_to_dbl((x)), fix32_to_dbl(result)); \
        status = 1; \
    } else { \
        fflush(stdout); \
        fflush(stderr); \
        printf("\033[32;1mOK:\033[22;39m " #x " = %f \n", fix32_to_dbl((x))); \
    }
