#include "fix32.h"
#include <stdbool.h>
#ifndef FIXMATH_NO_CTYPE
#include <ctype.h>
#else
static inline int isdigit(int c)
{
    return c >= '0' && c <= '9';
}

static inline int isspace(int c)
{
    return c == ' ' || c == '\r' || c == '\n' || c == '\t' || c == '\v' || c == '\f';
}
#endif

static const uint32_t scales[10] = {
    /* 5 decimals is enough for full fix32_t precision */
    1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000
};

static char *itoa_loop(char *buf, uint32_t scale, uint32_t value, bool skip)
{
    while (scale)
    {
        unsigned digit = (value / scale);
    
        if (!skip || digit || scale == 1)
        {
            skip = false;
            *buf++ = '0' + digit;
            value %= scale;
        }
        
        scale /= 10;
    }
    return buf;
}

void fix32_to_str(fix32_t value, char *buf, int decimals)
{
    uint64_t uvalue = (value >= 0) ? value : -value;
    if (value < 0)
        *buf++ = '-';

    /* Separate the integer and decimal parts of the value */
    unsigned intpart = uvalue >> 32;
    uint64_t fracpart = uvalue & 0xFFFFFFFF;
    uint64_t scale = scales[decimals];
    fracpart = fix32_mul(fracpart, scale);
    
    if (fracpart >= scale)
    {
        /* Handle carry from decimal part */
        intpart++;
        fracpart -= scale;    
    }
    
    /* Format integer part */
    buf = itoa_loop(buf, 1000000000, intpart, true);
    
    /* Format decimal part (if any) */
    if (scale != 1)
    {
        *buf++ = '.';
        buf = itoa_loop(buf, scale / 10, fracpart, false);
    }
    
    *buf = '\0';
}

fix32_t fix32_from_str(const char *buf)
{
    while (isspace(*buf))
        buf++;
    
    /* Decode the sign */
    bool negative = (*buf == '-');
    if (*buf == '+' || *buf == '-')
        buf++;

    /* Decode the integer part */
    uint64_t intpart = 0;
    int count = 0;
    while (isdigit(*buf))
    {
        intpart *= 10;
        intpart += *buf++ - '0';
        count++;
    }
    
    if (count == 0 || count > 10
        || intpart > 2147483646 || (!negative && intpart > 2147483647))
        return fix32_overflow;
    
    fix32_t value = intpart << 32;
    
    /* Decode the decimal part */
    if (*buf == '.' || *buf == ',')
    {
        buf++;
        
        uint32_t fracpart = 0;
        uint32_t scale = 1;
        while (isdigit(*buf) && scale < 100000000)
        {
            scale *= 10;
            fracpart *= 10;
            fracpart += *buf++ - '0';
        }
        
        value += fix32_div(fracpart, scale);
    }
    
    /* Verify that there is no garbage left over */
    while (*buf != '\0')
    {
        if (!isdigit(*buf) && !isspace(*buf))
            return fix32_overflow;
        
        buf++;
    }
    
    return negative ? -value : value;
}

