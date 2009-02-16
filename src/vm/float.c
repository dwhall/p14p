/*
 * PyMite - A flyweight Python interpreter for 8-bit microcontrollers and more.
 * Copyright 2009 Dean Hall
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#undef __FILE_ID__
#define __FILE_ID__ 0x17

/**
 * Float Object Type
 *
 * Float object type operations.
 *
 * Log
 * ---
 *
 * 2009/02/15   First
 */

 
#include "pm.h"


#ifdef HAVE_FLOAT

PmReturn_t
float_new(float f, pPmObj_t *r_pf)
{
    PmReturn_t retval = PM_RET_OK;

    retval = heap_getChunk(sizeof(PmFloat_t), (uint8_t **)r_pf);
    PM_RETURN_IF_ERROR(retval);
    OBJ_SET_TYPE(*r_pf, OBJ_TYPE_FLT);
    ((pPmFloat_t)*r_pf)->val = f;
    return retval;
}


#ifdef HAVE_PRINT
PmReturn_t
float_print(pPmObj_t pf)
{
    uint8_t tBuffer[10/*guessing*/];
    uint8_t bytesWritten;
    uint8_t i;
    PmReturn_t retval = PM_RET_OK;

    C_ASSERT(pf != C_NULL);

    /* Raise TypeError if obj is not an float */
    if (OBJ_GET_TYPE(pf) != OBJ_TYPE_FLT)
    {
        PM_RAISE(retval, PM_RET_EX_TYPE);
        return retval;
    }

    /* This does not use snprintf because glibc's snprintf is only
     * included for compiles without strict-ansi.
     */
    bytesWritten =
        sprintf((void *)&tBuffer, "%f", ((pPmFloat_t)pf)->val);

    /* Sanity check */
    C_ASSERT(bytesWritten != 0);
    C_ASSERT(bytesWritten < sizeof(tBuffer));

    for (i = (uint8_t)0; i < bytesWritten; i++)
    {
        retval = plat_putByte(tBuffer[i]);
        PM_RETURN_IF_ERROR(retval);
    }
    return PM_RET_OK;
}


PmReturn_t
float_negative(pPmObj_t pf, pPmObj_t *r_pf)
{
    PmReturn_t retval;

    /* Raise TypeError if obj is not an int */
    if (OBJ_GET_TYPE(pf) != OBJ_TYPE_FLT)
    {
        PM_RAISE(retval, PM_RET_EX_TYPE);
        return retval;
    }

    /* Create new int obj */
    return float_new(-((pPmFloat_t)pf)->val, r_pf);
}

#endif /* HAVE_PRINT */
#endif /* HAVE_FLOAT */
