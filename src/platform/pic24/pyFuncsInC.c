/** \file
 *  \brief This file implements Python functions defined in main.py.
 */

#include <pic24_all.h>
#include "pyToC.h"
#include "pyFuncsInC.h"
#include <stdio.h>

#undef __FILE_ID__
#define __FILE_ID__ 0x70

PmReturn_t readBitsC(pPmFrame_t *ppframe)
{
    PmReturn_t retval = PM_RET_OK;
    pPmObj_t ppo;
    uint16_t u16_evenAddress;
    uint16_t* pu16_evenAddress;
    uint16_t u16_bit;
    uint16_t u16_bitmask;
    uint16_t u16_value;

    // Get the arguments
    CHECK_NUM_ARGS(2);
    GET_UINT16(0, &u16_evenAddress);
    GET_UINT16(1, &u16_bit);

    // Check their values.
    /// \todo Allow an odd address for 8 bit values.
    EXCEPTION_UNLESS( !(u16_evenAddress & 1), PM_RET_EX_VAL,
      "The address must be even.");
    pu16_evenAddress = (uint16_t*) u16_evenAddress;
    EXCEPTION_UNLESS(u16_bit <= 15, PM_RET_EX_VAL,
      "The bit must be <= 15.");

    // Read the port and mask
    u16_value = getBit(*pu16_evenAddress, u16_bit);
    printf("Value at 0x%04X, bit %d = %d.\n", (uint16_t) pu16_evenAddress, 
      u16_bit, u16_value);

    retval = int_new(u16_value, &ppo);
    PM_RETURN_IF_ERROR(retval);
    NATIVE_SET_TOS(ppo);

    return retval;
}


bool_t getBit(uint16_t u16_bitfield, uint16_t u16_bit)
{
    ASSERT(u16_bit < 16);
    return (u16_bitfield & (1 << u16_bit)) ? C_TRUE : C_FALSE;
}


void setBit(volatile uint16_t* pu16_bitfield, uint16_t u16_bit, bool_t b_val)
{
    ASSERT(u16_bit < 16);
    if (b_val)
        *pu16_bitfield |= 1 << u16_bit;
    else
        *pu16_bitfield &= ~(1 << u16_bit);
}


