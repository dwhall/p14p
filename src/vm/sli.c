/*
# This file is Copyright 2002 Dean Hall.
# This file is part of the PyMite VM.
# This file is licensed under the MIT License.
# See the LICENSE file for details.
*/


#undef __FILE_ID__
#define __FILE_ID__ 0x11


/**
 * \file
 * \brief Standard Library Interface
 *
 * PyMite requires a few functions from a few different
 * standard C libraries (memory, string, etc).
 */


#include "pm.h"


/** use Duff's Device or simple for-loop for memcpy. */
#define USE_DUFFS_DEVICE    0


#if !HAVE_STRING_H

void *
sli_memcpy(unsigned char *to, unsigned char const *from, unsigned int n)
{
    unsigned char *tobak;

    /* Store init value of to */
    tobak = to;

#if USE_DUFFS_DEVICE
    if (n > 0)
    {
        switch (n & 0x7)
            do
            {
            case 0:
                *to++ = *from++;
            case 7:
                *to++ = *from++;
            case 6:
                *to++ = *from++;
            case 5:
                *to++ = *from++;
            case 4:
                *to++ = *from++;
            case 3:
                *to++ = *from++;
            case 2:
                *to++ = *from++;
            case 1:
                *to++ = *from++;
            }
            while ((n -= 8) > 0);
    }
#else
    for (; n > 0; n--)
    {
        *to = *from;
        from++;
        to++;
    }
#endif /* USE_DUFFS_DEVICE */
    return tobak;
}


int
sli_strlen(char const *s)
{
    char const *si = s;
    int len = 0;

    while (*si++)
    {
        len++;
    }
    return len;
}


int
sli_strcmp(char const *s1, char const *s2)
{
    /* While not at either strings' end and they're same */
    while ((*s1 != C_NULL) && (*s2 != C_NULL) && (*s1 == *s2))
    {
        s1++;
        s2++;
    }

    /* Return string difference */
    return *s1 - *s2;
}


int
sli_strncmp(char const *s1, char const *s2, unsigned int n)
{
    unsigned int i = 0;

    if (n == 0)
    {
        return 0;
    }

    /* Scan n bytes in string */
    for (i = 0; i < n; i++)
    {
        /* If bytes differ, return difference */
        if (s1[i] != s2[i])
        {
            return s1[i] - s2[i];
        }
    }
    return 0;
}

#endif /* HAVE_STRING_H */


/*
 * This function is moved outside of HAVE_STRING_H because the one in string.h
 * will not accept a null value for the second arg
 */
void
sli_memset(unsigned char *dest, char const val, unsigned int n)
{
    unsigned int i;

    for (i = 0; i < n; i++)
    {
        *dest = (unsigned char)val;
        dest++;
    }
}

void
sli_puts(uint8_t * s)
{
    uint8_t *ps = s;
    uint8_t c;

    c = *ps;
    ps++;
    while (c != '\0')
    {
        plat_putByte(c);
        c = *ps;
        ps++;
    }
}


PmReturn_t
sli_ltoa10(int32_t value, uint8_t *buf, uint8_t buflen)
{
    int32_t const decimal_places[] = { 1000000000, 100000000, 10000000, 1000000,
                                       100000, 10000, 1000, 100, 10, 1 };
    int32_t decimal_place;
    int32_t number;
    uint8_t c;
    uint8_t printed_one = C_FALSE;
    uint8_t i;
    uint8_t j;
    PmReturn_t retval = PM_RET_OK;

    C_ASSERT(buflen >= 12);

    number = value;
    if (number == 0)
    {
        buf[0] = '0';
        buf[1] = '\0';
        return retval;
    }

    /* Special case (can't convert it to positive value) */
    if (number == -2147483648)
    {
        sli_memcpy(buf, (unsigned char *)"-2147483648", 11);
        return PM_RET_OK;
    }

    j = 0;
    if (number < 0)
    {
        buf[0] = '-';
        j++;
        number = -number;
    }

    for (i = 0; i < 10; i++)
    {
        decimal_place = decimal_places[i];
        c = '0';
        while (number >= decimal_place)
        {
            number -= decimal_place;
            c++;
        }
        if ((c != '0') || printed_one)
        {
            buf[j++] = c;
            printed_one = C_TRUE;
        }
    }
    buf[j] = '\0';

    return retval;
}

char const * const hexChars = "0123456789abcdef";

/* MUST show leading zeros because callers don't keep track */
PmReturn_t
sli_btoa16(uint8_t value, uint8_t *buf, uint8_t buflen, uint8_t upperCase)
{
    C_ASSERT(buflen >= 3);

    if (upperCase) upperCase = 'A' - 'a';

    buf[0] = (value >> 4) > 9 
             ? hexChars[value >> 4] + upperCase
             : hexChars[value >> 4];
    buf[1] = (value & 0x0F) > 9
             ? hexChars[value & 0x0F] + upperCase
             : hexChars[value & 0x0F];
    buf[2] = '\0';

    return PM_RET_OK;
}


/* Does NOT show leading zeroes */
PmReturn_t
sli_ltoa16(int32_t value, uint8_t *buf, uint8_t buflen, uint8_t upperCase)
{
    int8_t i;
    uint8_t j = 0;
    uint8_t showZero = C_FALSE;
    uint8_t nibble;

    C_ASSERT(buflen >= 9);

    if (upperCase) upperCase = 'A' - 'a';

    for (i = 28; i >= 0; i -= 4)
    {
        nibble = ((value >> i) & 0xF);
        if ((nibble == 0) && !showZero) continue;
        buf[j++] = (nibble > 9) 
                   ? hexChars[nibble] + upperCase
                   : hexChars[nibble];
        showZero = C_TRUE;
    }
    buf[j] = '\0';

    return PM_RET_OK;
}


PmReturn_t
sli_ptoa16(intptr_t value, uint8_t *buf, uint8_t buflen, uint8_t upperCase)
{
    PmReturn_t retval;
    int8_t i;
    int8_t j;

    C_ASSERT(buflen >= 2 * sizeof(intptr_t) + 1);

    /* Print the hex value, most significant byte first */
    for (j = 0, i = 8 * sizeof(intptr_t) - 8; i >= 0; i -= 8, j += 2)
    {
        retval = sli_btoa16((value >> i) & 0xFF, &buf[j], buflen - j, upperCase);
        PM_BREAK_IF_ERROR(retval);
    }

    return retval;
}


typedef union {
    int32_t L;
    float F;
} LF_t;


/* The buf MUST be at least 15 bytes long */
PmReturn_t
sli_ftoa(float f, uint8_t *buf, uint8_t buflen)
{
    uint32_t mantissa, int_part, frac_part;
    int16_t exp2;
    LF_t x;
    uint8_t *p;
    int8_t adj = 0;
    PmReturn_t retval = PM_RET_OK;

    C_ASSERT(buflen >= 15);

    if (f == 0.0)
    {
        buf[0] = '0';
        buf[1] = '.';
        buf[2] = '0';
        buf[3] = '\0';
        return PM_RET_OK;
    }
    x.F = f;

    exp2 = (0xFF & (x.L >> 23)) - 127;
    mantissa = (x.L & 0xFFFFFF) | 0x800000;
    frac_part = 0;
    int_part = 0;
    p = buf;

    /* Adjust large exponents using the approximation: 2**10 == k*10**3 */
    while (exp2 >= 31)
    {
        /* Reduce the binary exponent here (incr the decimal exponent below) */
        exp2 -=  10;
        adj++;

        /*
         * To use the approximation above, the mantissa must be multiplied by k
         * where k = 1.024 ~= (1 + 12583/(2**19))
         * Divide first to avoid integer overflow (the mantissa is 24 bits)
         */
        mantissa += ((mantissa >> 6) * 12583) >> 13;
    }

    if (exp2 < -23)
    {
        // Unable to handle large negative exponents at this time
        *p++ = '?';
        return PM_RET_OK;
    }
    else if (exp2 >= 23)
    {
        int_part = mantissa << (exp2 - 23);
    }
    else if (exp2 >= 0)
    {
        int_part = mantissa >> (23 - exp2);
        frac_part = (mantissa << (exp2 + 1)) & 0xFFFFFF;
    }
    else /* if (exp2 < 0) */
    {
        frac_part = (mantissa & 0xFFFFFF) >> -(exp2 + 1);
    }

    if (x.L < 0)
    {
        *p++ = '-';
    }

    if (int_part == 0)
    {
        *p++ = '0';
    }
    else
    {
        retval = sli_ltoa10(int_part, p, buflen - (p - buf));
        PM_RETURN_IF_ERROR(retval);
        while (*p) p++;
    }
    *p++ = '.';

    if (frac_part == 0)
    {
        *p++ = '0';
    }
    else
    {
        char m, max;

        max = buflen - (p - buf) - 1;
        if (max > 6)
        {
            max = 6;
        }

        /* Print fractional part */
        for (m = 0; m < max; m++)
        {
            frac_part *= 10;
            *p++ = '0' + (frac_part >> 24);
            frac_part &= 0xFFFFFF;
        }

        /* Remove ending zeroes */
        //for (--p; p[0] == '0' && p[-1] != '.'; --p);
        //++p;
    }

    /*
     * If the exponent is large (adjustment took place above),
     * normalize the string to scientific notation
     */
    if (adj != 0)
    {
        uint8_t i;

        /* Shift chars to make room for the new decimal point */
        i = (p - buf + 1);
        i = (i > (buflen - 1)) ? buflen - 1 : i;
        for (; i > 1; i--)
        {
            buf[i] = buf[i-1];
        }

        /* Find the index of the old decimal point */
        for (i = 6; (buf[i] != '.') && (i < 15); i++);

        /* Set the new decimal point (normalized) */
        buf[1] = '.';

        /*
         * Adjust the decimal exponent (3 decimal places for every 10 bits)
         * and add the amount for the normalization
         */
        p = &buf[8];
        *p++ = 'e';
        *p++ = '+';
        retval = sli_ltoa10(3 * adj + (i - 2), p, buflen - (p - buf));
        PM_RETURN_IF_ERROR(retval);
        while (*p) p++;
    }

    *p = '\0';

    return PM_RET_OK;
}
