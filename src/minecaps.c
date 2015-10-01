#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "minecaps.h"

///The name of this module can be confusing: at first it was for the 2 Cap functions, because they
///didn't fit anywhere. Then I put every multifunctional algorythms here, which can be used at
///further projects as well.

/* Cap(...) function ***************************************************************************************

Description:    Limits the value of n between min and max
Parameters:         int             n,          (the value to be limited)
                    int             min,        (the minimum value allowed)
                    int             max,        (the maximum value allowed)
Returns:        the limited value (note: can't change parameter since it's passed as value)
********************************************************************************************************* */
int Cap(int n, int min, int max)
{
    if(n < min)
        return min;
    else if(n > max)
        return max;

    return n;
}

/* CapR(...) function **************************************************************************************

Description:    Limits the value of *n between min and max, and changes the value of *n
Parameters:         int             *n,         (the value to be limited)
                    int             min,        (the minimum value allowed)
                    int             max,        (the maximum value allowed)
Returns:        boolean value if *n needed to be changed && the limited value as parameter
********************************************************************************************************* */
bool CapR(int *n, int min, int max)
{
    if(*n < min)
    {
        *n = min;
        return true;
    }
    else if(*n > max)
    {
        *n = max;
        return true;
    }
    return false;
}

/* debugmalloc(...) function ******************************************************************************

Description:    Used for allocated memory and loaded files, checks if that pointer is valid, if it isn't,
                sends an error message and exits from application.
Parameters:         void            *p,         (the pointer to check)
                    char            *str,       (the error message to send if pointer is NULL)
Returns:        void
********************************************************************************************************* */
void debugmalloc(void * p, char * str)
{
    if(p==NULL)
    {
        printf("Allocation was unsuccesful. %s\n", str);
        exit(1);
    }
}

/* unicode_2_utf8(...) function ****************************************************************************

Description:    Used for converting a text from UNICODE to UTF8
Parameters:         Uint16 const    *be,        (the input text)
                    char            *ki,        (the output text)
Returns:        void
Source:         this function was copied from http://infoc.eet.bme.hu and not modified at all.
********************************************************************************************************* */
void unicode_2_utf8(Uint16 const *be, char *ki)
{
    int pb, pk;

    pk=0;
    for (pb=0; be[pb]!=0x0000; ++pb)
    {
        /* 00000000 0xxxxxxx    0x0000-0x007F   0xxxxxxx */
        if (be[pb]<=0x007F)
            ki[pk++] = be[pb];
        else
        /* 00000yyy yyxxxxxx    0x0080-0x07FF   110yyyyy 10xxxxxx */
        if (be[pb]<=0x07FF)
        {
            ki[pk++] = 0xC0 | be[pb]>>6;     /* 0xC0 = 11000000 */
            ki[pk++] = 0x80 | (be[pb]&0x3F); /* 0x80 = 10000000, 0x3F = 00111111 */
        }
        /* zzzzyyyy yyxxxxxx    0x0800-0xFFFF   1110zzzz 10yyyyyy 10xxxxxx */
        else
        {
            ki[pk++] = 0xE0 | be[pb]>>12;    /* 0xE0 = 11100000 */
            ki[pk++] = 0x80 | ((be[pb]>>6)&0x3F);
            ki[pk++] = 0x80 | (be[pb]&0x3F);
        }
    }

    ki[pk]=0;
}

/* utf8_2_unicode(...) function ****************************************************************************

Description:    Used for converting a text from UTF8 to UNICODE
Parameters:         char   const    *be,        (the input text)
                    Uint16          *ki,        (the output text)
Returns:        void
Source:         this function was copied from http://infoc.eet.bme.hu and not modified at all.
********************************************************************************************************* */
void utf8_2_unicode(char const *be, Uint16 *ki)
{
    int pb, pk;

    pk=0;
    for (pb=0; be[pb]!=0; ++pb)
    {
        if (be[pb]<0x80)
        {
            /* 00000000 0xxxxxxx    0x0000-0x007F    0xxxxxxx */
            ki[pk++] = be[pb];
        }
        else
        if (be[pb]>>5 == 6)
        {   /* 0x6 = 110 bin */
            /* 00000yyy yyxxxxxx    0x0080-0x07FF    110yyyyy 10xxxxxx */
            ki[pk++] = (be[pb]&0x1f)<<6 | (be[pb+1]&0x3f);
            pb+=1;              /* ket bajtot hasznaltunk */
        }
        else
        {
            /* zzzzyyyy yyxxxxxx    0x0800-0xFFFF   1110zzzz 10yyyyyy 10xxxxxx */
            ki[pk++] = (be[pb]&0x0f)<<12 | (be[pb+1]&0x3f)<<6 | (be[pb+2]&0x3f);
            pb+=2;              /* harom bajtot hasznaltunk */
        }
    }
    ki[pk]=0x0000;
}
