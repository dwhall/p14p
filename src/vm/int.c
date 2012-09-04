/*
# This file is Copyright 2002 Dean Hall.
# This file is part of the PyMite VM.
# This file is licensed under the MIT License.
# See the LICENSE file for details.
*/


#undef __FILE_ID__
#define __FILE_ID__ 0x08


/**
 * \file
 * \brief Integer Object Type
 *
 * Integer object type operations.
 */

#include <limits.h>

#include "pm.h"


PmReturn_t
int_dup(pPmObj_t pint, pPmObj_t *r_pint)
{
    PmReturn_t retval = PM_RET_OK;

    /* Allocate new int */
    retval = heap_getChunk(sizeof(PmInt_t), (uint8_t **)r_pint);
    PM_RETURN_IF_ERROR(retval);

    /* Copy value */
    OBJ_SET_TYPE(*r_pint, OBJ_TYPE_INT);
    ((pPmInt_t)*r_pint)->val = ((pPmInt_t)pint)->val;
    return retval;
}


PmReturn_t
int_new(int32_t n, pPmObj_t *r_pint)
{
    PmReturn_t retval = PM_RET_OK;

    /* If n is 0,1,-1, return static int objects from global struct */
    if (n == 0)
    {
        *r_pint = PM_ZERO;
        return PM_RET_OK;
    }
    if (n == 1)
    {
        *r_pint = PM_ONE;
        return PM_RET_OK;
    }
    if (n == -1)
    {
        *r_pint = PM_NEGONE;
        return PM_RET_OK;
    }

    /* Else create and return new int obj */
    retval = heap_getChunk(sizeof(PmInt_t), (uint8_t **)r_pint);
    PM_RETURN_IF_ERROR(retval);
    OBJ_SET_TYPE(*r_pint, OBJ_TYPE_INT);
    ((pPmInt_t)*r_pint)->val = n;
    return retval;
}


PmReturn_t
int_positive(pPmObj_t pobj, pPmObj_t *r_pint)
{
    PmReturn_t retval;

    /* Raise TypeError if obj is not an int */
    if (OBJ_GET_TYPE(pobj) != OBJ_TYPE_INT)
    {
        PM_RAISE(retval, PM_RET_EX_TYPE);
        return retval;
    }

    /* Create new int obj */
    return int_new(((pPmInt_t)pobj)->val, r_pint);
}


PmReturn_t
int_negative(pPmObj_t pobj, pPmObj_t *r_pint)
{
    PmReturn_t retval;

    /* Raise TypeError if obj is not an int */
    if (OBJ_GET_TYPE(pobj) != OBJ_TYPE_INT)
    {
        PM_RAISE(retval, PM_RET_EX_TYPE);
        return retval;
    }

    /* Create new int obj */
    return int_new(-((pPmInt_t)pobj)->val, r_pint);
}


PmReturn_t
int_bitInvert(pPmObj_t pobj, pPmObj_t *r_pint)
{
    PmReturn_t retval;

    /* Raise TypeError if obj is not an int */
    if (OBJ_GET_TYPE(pobj) != OBJ_TYPE_INT)
    {
        PM_RAISE(retval, PM_RET_EX_TYPE);
        return retval;
    }

    /* Create new int obj */
    return int_new(~((pPmInt_t)pobj)->val, r_pint);
}


PmReturn_t
int_print(pPmObj_t pint)
{
    PmReturn_t retval = PM_RET_OK;
    uint8_t buf[12];

    C_ASSERT(pint != C_NULL);

    /* Raise TypeError if obj is not an int */
    if (OBJ_GET_TYPE(pint) != OBJ_TYPE_INT)
    {
        PM_RAISE(retval, PM_RET_EX_TYPE);
        return retval;
    }

    retval = sli_ltoa10(((pPmInt_t)pint)->val, buf, sizeof(buf));
    PM_RETURN_IF_ERROR(retval);
    sli_puts(buf);

    return retval;
}


PmReturn_t
int_printHex(pPmObj_t pint)
{
    uint8_t buf[9];
    PmReturn_t retval = PM_RET_OK;

    C_ASSERT(OBJ_GET_TYPE(pint) == OBJ_TYPE_INT);

    /* Print the integer object */
    retval = sli_ltoa16(((pPmInt_t)pint)->val, buf, sizeof(buf), 1);
    sli_puts(buf);
    return retval;
}


PmReturn_t
int_pow(pPmObj_t px, pPmObj_t py, pPmObj_t *r_pn)
{
    int32_t x;
    int32_t y;
    int32_t n;
    PmReturn_t retval;

    /* Raise TypeError if args aren't ints */
    if ((OBJ_GET_TYPE(px) != OBJ_TYPE_INT)
        || (OBJ_GET_TYPE(py) != OBJ_TYPE_INT))
    {
        PM_RAISE(retval, PM_RET_EX_TYPE);
        return retval;
    }

    x = ((pPmInt_t)px)->val;
    y = ((pPmInt_t)py)->val;

    /* Raise Value error if exponent is negative */
    if (y < 0)
    {
        PM_RAISE(retval, PM_RET_EX_VAL);
        return retval;
    }

    /* Calculate x raised to y */
    n = 1;
    while (y > 0)
    {
        n = n * x;
        y--;
    }
    retval = int_new(n, r_pn);

    return retval;
}


PmReturn_t
int_divmod(pPmObj_t px, pPmObj_t py, uint8_t op, pPmObj_t *r_pxopy)
{
    int32_t x;
    int32_t y;
    int32_t xdivy;
    int32_t xmody;
    int32_t xopy;
    PmReturn_t retval = PM_RET_OK;

    /* Raise TypeError if args aren't ints */
    if ((OBJ_GET_TYPE(px) != OBJ_TYPE_INT)
        || (OBJ_GET_TYPE(py) != OBJ_TYPE_INT))
    {
        PM_RAISE(retval, PM_RET_EX_TYPE);
        return retval;
    }

    x = ((pPmInt_t)px)->val;
    y = ((pPmInt_t)py)->val;

    /* Raise ZeroDivisionError if denominator is zero */
    if (y == 0)
    {
        PM_RAISE(retval, PM_RET_EX_ZDIV);
        return retval;
    }

    /* Issue #167: Make overflow silent until exceptions can be caught */
    /* (-sys.maxint-1)/-1 is the only overflow case. */
    /* TODO:  enable the overflow for Issue #169 */
    /*
    if ((y == -1) && (op == '/') && (x < 0)
        && ((uint32_t)x == (0 - (uint32_t)x)))
    {
        PM_RAISE(retval, PM_RET_EX_OFLOW);
        return retval;
    }
    */

    /* Shortcut when denominator is one or negative one */
    if (y == 1)
    {
        xdivy = x;
        xmody = 0;
    }
    else if (y == -1)
    {
        xdivy = -x;
        xmody = 0;
    }

    else
    {
        xdivy = x / y;
        xmody = x - xdivy * y;

        /*
         * If the remainder is non-0 and the signs of x and y differ,
         * C89 doesn't define whether xdivy is now the floor or the
         * ceiling of the infinitely precise quotient.  We want the floor,
         * and we have it iff the remainder's sign matches y's.
         */
        if ((xmody != 0) && ((y ^ xmody) < 0))
        {
            xmody += y;
            --xdivy;
            C_ASSERT(xmody && ((y ^ xmody) >= 0));
        }
    }
    xopy = (op == '/') ? xdivy : xmody;
    return int_new(xopy, r_pxopy);
}
