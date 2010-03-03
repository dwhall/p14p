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
PmReturn_t readBitsC(pPmFrame_t *ppframe);
"""

## Read bits from evenAddress.
#  \todo Python returns PmInt_t values, which are 32-bit signed integers.
#        Need to check this for negative values, >16 bit values.
#  \param evenAddress The word address to read from. Must be even.
#  \param startBit The starting bit to read from; must be from 0 to 15.
#  \param numBits The number of bits to read, must be > 0. 
#                 Also, startBit + numBits must be <= 16.
def readBits(evenAddress, startBit, numBits):
    """__NATIVE__
    return readBitsC(ppframe);
    """
    pass

print readBits(10, 3, 2)

import sys
print "Welcome to PIC24 Python! Free space:", sys.heap()
print "Starting interactive mode."
import ipm
ipm.ipm()
