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
PmReturn_t configDigitalPinC(pPmFrame_t *ppframe);
"""

## Read bits from evenAddress.
#  \todo Python returns PmInt_t values, which are 32-bit signed integers.
#        Need to check this for negative values, >16 bit values.
#  \param evenAddress The word address to read from. Must be even.
#  \param startBit The starting bit to read from; must be from 0 to 15.
#  \param numBits The number of bits to read, must be > 0. 
#                 Also, startBit + numBits must be <= 16.
def readBits(evenAddress, startBit, numBits):
# Uncommenting lines below causes VM to not start.
#    """__NATIVE__
#    return readBitsC(ppframe);
#    """
    pass

## Configures a pin for digital operation.
#  Parameters:
#  @param port The port, specified as a one-letter string,
#              which must be from "A" to "G".
#  @param pin  The pin of the port to configure. Must be
#              a number between 0 and 15.
#  @param isInput True to configure the pin as an input,
#              false to configure the pin as an output.
#  @param isOpenDrain True to configure the pin's output
#              drivers to be 
#              <a href="http://en.wikipedia.org/wiki/Open_collector">open drain</a>,
#              false to configure the pin's output drivers
#              as a standrard
#              <a href="http://en.wikipedia.org/wiki/Totem_pole_output">push-pull</a>
#              output. <em>IMPORTANT</em>: Not all pins
#              have open-drain ability; therefore, the only
#              valid selection for this parameter may be false.
#              All pins have standard, push-pull drivers.
#  @param pullDir A value > 0 to enable a 
#              <a href="http://en.wikipedia.org/wiki/Pull-up_resistor">pull-up resistor</a>
#              on the pin, a value < 0 to enable a pull-down
#              resistor on the pin, or 0 to disable both.
#              <em>IMPORTANT</em>: Not all pins have pull-up
#              or pull-down capability. Valid values for
#              some pins are 0 (neither pull-up nor pull-down
#              resistors are available), or >=0 (only
#              pull-up resistors are available).
def configDigitalPin(port, pin, isInput, isOpenDrain, pullDir):
    """__NATIVE__
    return configDigitalPinC(ppframe);
    """
    pass


# Start of main code
#readBits(10, 3, 2)
#configDigitalPin(1, 1, False, False, 0)

import sys
print "Welcome to PIC24 Python! Free space:", sys.heap()
print "Starting interactive mode."
import ipm
ipm.ipm()
