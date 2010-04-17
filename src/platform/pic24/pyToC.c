/** \file
 *  \brief This file provides a set of functions to ease calling
 *         C functions from Python.
 */

#include "pyToC.h"
#include <limits.h>

#undef __FILE_ID__
#define __FILE_ID__ 0x71

PmReturn_t getInt32(pPmFrame_t *ppframe, uint8_t u8_ndx, int32_t* pi32_val) {
    PmReturn_t retval = PM_RET_OK;
    pPmObj_t ppo;

    // Raise TypeError if address isn't an int
    ppo = NATIVE_GET_LOCAL(u8_ndx);
    EXCEPTION_UNLESS(OBJ_GET_TYPE(ppo) == OBJ_TYPE_INT, PM_RET_EX_TYPE, 
      "Argument %u must be an int", (uint16_t) u8_ndx);

    // Get the value, now that we know it's an int
    *pi32_val = ((pPmInt_t) ppo)->val;

    return retval;
}

PmReturn_t getRangedInt(pPmFrame_t *ppframe, uint8_t u8_ndx, 
  int32_t i32_min, int32_t i32_max, int32_t* pi32_val) {
    PmReturn_t retval = PM_RET_OK;

    // Get the int32 from the Python arguments passed to this function
    retval = getInt32(ppframe, u8_ndx, pi32_val);
    PM_RETURN_IF_ERROR(retval);

    // Raise a ValueError if address is < min or > max
    EXCEPTION_UNLESS((*pi32_val >= i32_min) && 
      (*pi32_val <= i32_max), PM_RET_EX_VAL, 
        "Argument %u value must be between %ld and %ld.", 
        (uint16_t) u8_ndx, i32_min, i32_max);

    return retval;
}

PmReturn_t getUint16(pPmFrame_t *ppframe, uint8_t u8_ndx, uint16_t* pu16_val) {
    PmReturn_t retval = PM_RET_OK;
    int32_t i32;

    retval = getRangedInt(ppframe, u8_ndx, 0, 65535, &i32);
    PM_RETURN_IF_ERROR(retval);
    *pu16_val = (uint16_t) i32;
    return retval;
}

PmReturn_t getBool(pPmFrame_t *ppframe, uint8_t u8_ndx, bool_t* pb_bool) {
    PmReturn_t retval = PM_RET_OK;
    pPmObj_t ppo;

    // Raise TypeError if address isn't an int
    ppo = NATIVE_GET_LOCAL(u8_ndx);
    EXCEPTION_UNLESS(OBJ_GET_TYPE(ppo) == OBJ_TYPE_BOOL, PM_RET_EX_TYPE, 
      "Argument %u must be a bool", (uint16_t) u8_ndx);
    
    *pb_bool = ((pPmBoolean_t) ppo)->val;

    return retval;
}
