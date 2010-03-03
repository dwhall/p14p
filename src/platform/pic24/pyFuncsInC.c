/** \file
 *  \brief This file implements Python functions defined in main.py.
 */

#include <pic24_all.h>
#include "pm.h"
#include <stdio.h>

#undef __FILE_ID__
#define __FILE_ID__ 0x0B

/** Raise an exception if the given expression is false.
 *  This MUST be called from the C implementation of a 
 *  Python function, becuase it assumes the existance of
 *  - PmReturn_t retval
 *  \param expr Expression to evaluate.
 *  \param prReturn If expression is false, exception to raise. Must
 *                  be of type \ref PmReturn_t and typically is in the
 *                  \ref PmReturn_e enum.
 *  \param eStr Diagostic string to inclcude with the exception.
 *              Currently, not used.
 */
#define EXCEPTION_ASSERT(expr, prReturn, eStr) \
    do { \
        if (!(expr)) { \
            printf("Error: %s\n", eStr); \
            PM_RAISE(retval, prReturn); \
            return retval; \
        } \
    } while (C_FALSE)


/** Get an unsigned, 16-bit value from the arguments passed to a Python
 *  function. Raises errors as necessary.
 *  \param ppframe The Python stack frame containing user arguments.
 *  \param u16_ndx Zero-based index of the desired parameter to extract.
 *  \param pu16_val Pointer to resulting uint16 value extracted.
 *  \return Standard Python return value.
 */
PmReturn_t getUint16(pPmFrame_t *ppframe, uint16_t u16_ndx, uint16_t* pu16_val) {
    PmReturn_t retval = PM_RET_OK;
    pPmObj_t ppo;
    int32_t i32;

    // Raise TypeError if address isn't an int
    ppo = NATIVE_GET_LOCAL(u16_ndx);
    EXCEPTION_ASSERT(OBJ_GET_TYPE(ppo) == OBJ_TYPE_INT, PM_RET_EX_TYPE, 
      "Expected an int");

    // Raise a ValueError if address is < 0 or > 65535
    i32 = ((pPmInt_t) ppo)->val;
    EXCEPTION_ASSERT((i32 >= 0) && (i32 <= 65535), PM_RET_EX_VAL, 
        "Value must be between 0 and 65535.");

    // Assignment is now safe
    *pu16_val = (uint16_t) i32;

    return retval;
}

/** Macro to ease calling \ref getUint16. This MUST be called from
 *  the C implementation of a Python function, becuase it assumes
 *  the existance of:
 *  - PmReturn_t retval
 *  - pPmFrame_t* ppframe
 *  \param u16_ndx Zero-based index of the desired parameter to extract.
 *  \param u16_val Resulting uint16 value extracted.
 */
#define GET_UINT16(u16_ndx, u16_val) \
    do { \
        retval = getUint16(ppframe, u16_ndx, &u16_val); \
        PM_RETURN_IF_ERROR(retval); \
    } while (C_FALSE)

/** Implements the Python \ref readBits function. See it for details. */
PmReturn_t readBitsC(pPmFrame_t *ppframe)
{
    PmReturn_t retval = PM_RET_OK;
    pPmObj_t ppo;
    uint16_t u16;
    uint16_t* pu16_evenAddress;
    uint16_t u16_startBit;
    uint16_t u16_numBits;
    uint16_t u16_bitmask;
    uint16_t u16_value;

    // Raise TypeError if wrong number of args
    EXCEPTION_ASSERT(NATIVE_GET_NUM_ARGS() == 3, PM_RET_EX_TYPE,
      "Incorrect number of arguments.");

    // Get the arguments
    GET_UINT16(0, u16);
    pu16_evenAddress = (uint16_t*) u16;
    GET_UINT16(1, u16_startBit);
    GET_UINT16(2, u16_numBits);

    // Check their values.
    /// \todo Allow an odd addres for 8 bit values.
    EXCEPTION_ASSERT( !(((uint16_t) pu16_evenAddress) & 1), PM_RET_EX_VAL,
      "The address must be even.");
    EXCEPTION_ASSERT(u16_startBit <= 15, PM_RET_EX_VAL,
      "The start bit must be <= 15.");
    EXCEPTION_ASSERT(u16_numBits > 0, PM_RET_EX_VAL,
      "The number of bits must be > 0.");
    EXCEPTION_ASSERT(u16_startBit + u16_numBits <= 16, PM_RET_EX_VAL,
      "start bit + num bits <= 16.");

    // Form the bitmask
    u16_bitmask = (1 << u16_numBits) - 1;
    // Read the port and mask
    u16_value = (*pu16_evenAddress >> u16_numBits) & u16_bitmask;
    printf("Value at 0x%04X, bit(s) %d to %d = 0x%02X.\n", (uint16_t) pu16_evenAddress, 
      u16_startBit, u16_startBit + u16_numBits - 1, u16_value);

    retval = int_new(u16_value, &ppo);
    PM_RETURN_IF_ERROR(retval);
    NATIVE_SET_TOS(ppo);

    return retval;
}
