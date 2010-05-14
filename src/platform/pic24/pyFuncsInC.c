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
    uint16_t u16;
    uint16_t* pu16_evenAddress;
    uint16_t u16_startBit;
    uint16_t u16_numBits;
    uint16_t u16_bitmask;
    uint16_t u16_value;

    // Get the arguments
    CHECK_NUM_ARGS(3);
    GET_UINT16(0, &u16);
    pu16_evenAddress = (uint16_t*) u16;
    GET_UINT16(1, &u16_startBit);
    GET_UINT16(2, &u16_numBits);

    // Check their values.
    /// \todo Allow an odd address for 8 bit values.
    EXCEPTION_UNLESS( !(((uint16_t) pu16_evenAddress) & 1), PM_RET_EX_VAL,
      "The address must be even.");
    EXCEPTION_UNLESS(u16_startBit <= 15, PM_RET_EX_VAL,
      "The start bit must be <= 15.");
    EXCEPTION_UNLESS(u16_numBits > 0, PM_RET_EX_VAL,
      "The number of bits must be > 0.");
    EXCEPTION_UNLESS(u16_startBit + u16_numBits <= 16, PM_RET_EX_VAL,
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


