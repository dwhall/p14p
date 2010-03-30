/** \file
 *  \brief This file provides a set of functions to ease calling
 *         C functions from Python.
 */

#include "pyToC.h"
#include <limits.h>

#undef __FILE_ID__
#define __FILE_ID__ 0x71

PmReturn_t getUint16(pPmFrame_t *ppframe, uint16_t u16_ndx, uint16_t* pu16_val) {
    PmReturn_t retval = PM_RET_OK;
    pPmObj_t ppo;
    int32_t i32;

    // Raise TypeError if address isn't an int
    ppo = NATIVE_GET_LOCAL(u16_ndx);
    EXCEPTION_UNLESS(OBJ_GET_TYPE(ppo) == OBJ_TYPE_INT, PM_RET_EX_TYPE, 
      "Expected an int");

    // Raise a ValueError if address is < 0 or > 65535
    i32 = ((pPmInt_t) ppo)->val;
    EXCEPTION_UNLESS((i32 >= 0) && (i32 <= UINT_MAX), PM_RET_EX_VAL, 
        "Value must be between 0 and 65535.");

    // Assignment is now safe
    *pu16_val = (uint16_t) i32;

    return retval;
}

PmReturn_t getInt32(pPmFrame_t *ppframe, uint16_t u16_ndx, int32_t* pi32_val) {
    PmReturn_t retval = PM_RET_OK;
    pPmObj_t ppo;

    // Raise TypeError if address isn't an int
    ppo = NATIVE_GET_LOCAL(u16_ndx);
    EXCEPTION_UNLESS(OBJ_GET_TYPE(ppo) == OBJ_TYPE_INT, PM_RET_EX_TYPE, 
      "Expected an int");

    // Assignment is now safe
    *pi32_val = ((pPmInt_t) ppo)->val;

    return retval;
}

PmReturn_t getBool(pPmFrame_t *ppframe, uint16_t u16_ndx, uint_t* pu_bool) {
    PmReturn_t retval = PM_RET_OK;
    pPmObj_t ppo;

    // Raise TypeError if address isn't an int
    ppo = NATIVE_GET_LOCAL(u16_ndx);
    EXCEPTION_UNLESS(OBJ_GET_TYPE(ppo) == OBJ_TYPE_BOOL, PM_RET_EX_TYPE, 
      "Expected a bool");
    
    *pu_bool = ((pPmBoolean_t) ppo)->val ? C_TRUE : C_FALSE;

    return retval;
}
