# This file is Copyright 2007, 2009 Dean Hall.
#
# This file is part of the Python-on-a-Chip program.
# Python-on-a-Chip is free software: you can redistribute it and/or modify
# it under the terms of the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1.
# 
# Python-on-a-Chip is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# A copy of the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1
# is seen in the file COPYING up one directory from this.

## @file
#  @brief PIC24/dsPIC33-specific Python functions
#

"""__NATIVE__
#include <pic24_all.h>
#include <stdio.h>
"""

def init():
	pass


## Read bits from evenAddress.
#  \todo Python returns PmInt_t values, which are 32-bit signed integers.
#        Need to check this for negative values, >16 bit values.
#  \param evenAddress The word address to read from. Must be even.
#  \param startBit The starting bit to read from; must be from 0 to 15.
#  \param numBits The number of bits to read, must be > 0. 
#                 Also, startBit + numBits must be <= 16.
def readBits(evenAddress, startBit, numBits):
    """__NATIVE__
    PmReturn_t retval = PM_RET_OK;
    pPmObj_t ppo_evenAddress;
	pPmObj_t ppo_startBit;
	pPmObj_t ppo_numBits;
	pPmObj_t ppo_value;
	uint16_t* pu16_evenAddress;
	uint16_t u16_startBit;
	uint16_t u16_numBits;
	uint16_t u16_bitmask;
	uint16_t u16_value;

    /* Raise TypeError if wrong number of args */
    if (NATIVE_GET_NUM_ARGS() != 3)
    {
        PM_RAISE(retval, PM_RET_EX_TYPE);
        return retval;
    }

    // Raise TypeError if args are not the right type.
    // Otherwise, extract their values.
    ppo_evenAddress = NATIVE_GET_LOCAL(0);
    if (OBJ_GET_TYPE(ppo_evenAddress) != OBJ_TYPE_INT)
    {
        PM_RAISE(retval, PM_RET_EX_TYPE);
        return retval;
    }
	pu16_evenAddress = (uint16_t*) ((pPmInt_t) ppo_evenAddress)->val;

    ppo_startBit = NATIVE_GET_LOCAL(1);
    if (OBJ_GET_TYPE(ppo_startBit) != OBJ_TYPE_INT)
    {
        PM_RAISE(retval, PM_RET_EX_TYPE);
        return retval;
    }
	u16_startBit = ((pPmInt_t) ppo_startBit)->val;

    ppo_numBits = NATIVE_GET_LOCAL(2);
    if (OBJ_GET_TYPE(ppo_numBits) != OBJ_TYPE_INT)
    {
        PM_RAISE(retval, PM_RET_EX_TYPE);
        return retval;
    }
	u16_numBits = ((pPmInt_t) ppo_numBits)->val;

	// Check these args:
	// 1. The address must be even.
	C_ASSERT( !(((uint16_t) pu16_evenAddress) & 1) );
	// 2. The start bit must be <= 15
	C_ASSERT(u16_startBit <= 15);
	// 3. The number of bits must be > 0
	C_ASSERT(u16_numBits > 0);
	// 4. start bit + num bits <= 16
	C_ASSERT(u16_startBit + u16_numBits <= 16);

	// Form the bitmask
	u16_bitmask = (1 << u16_numBits) - 1;
	// Read the port and mask
	u16_value = (*pu16_evenAddress >> u16_numBits) & u16_bitmask;
//	printf("Value at 0x%04X, bit(s) %d to %d = 0x%02X.\\n", pu16_evenAddress, 
//	  u16_startBit, u16_startBit + u16_numBits - 1, u16_value);

    retval = int_new(u16_value, &ppo_value);
    NATIVE_SET_TOS(ppo_value);

    return retval;
    """
    pass

# print readBits(2, 3, 2)

import sys
print "Welcome to PIC24 Python! Free space:", sys.heap()
print "Starting interactive mode."
import ipm
ipm.ipm()
