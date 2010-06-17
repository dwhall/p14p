/** \file
 *  \brief This file implements Python functions defined in main.py.
 */

#include <pic24_all.h>
#include "pyToC.h"
#include "pyFuncsInC.h"
#include <stdio.h>

#undef __FILE_ID__
#define __FILE_ID__ 0x70

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


