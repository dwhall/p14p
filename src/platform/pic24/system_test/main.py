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
#  @brief System test for the pic24/dspic33.
#

"""__NATIVE__
#include <pic24_all.h>
#include "pyToC.h"

/** Macro to convert a number to a string.
 *  Typical usage: <code>TOSTRING(__LINE__)</code>
 */
#define TOSTRING(x) _TOSTRING(x)
// A helper macro used by \\ref TOSTRING.
#define _TOSTRING(x) #x

#define EXCEPTION_ASSERT(a) EXCEPTION_UNLESS(a, PM_RET_EX_VAL, __FILE__ ":" TOSTRING(__LINE__) " - " #a "\\n")
"""

## Check results of pin config
def testConfigDigitalPin1():
    """__NATIVE__
    PmReturn_t retval = PM_RET_OK;

    EXCEPTION_ASSERT(_PCFG3 == 1);
    EXCEPTION_ASSERT(_TRISB1 == 0);
    EXCEPTION_ASSERT(_ODCB1 == 0);
    EXCEPTION_ASSERT(_CN5PUE == 0);
    return retval;
    """
    pass

## Check results of pin config
def testConfigDigitalPin2():
    """__NATIVE__
    PmReturn_t retval = PM_RET_OK;

    EXCEPTION_ASSERT(_PCFG3 == 1);
    EXCEPTION_ASSERT(_TRISB1 == 1);
    EXCEPTION_ASSERT(_ODCB1 == 1);
    EXCEPTION_ASSERT(_CN5PUE == 1);
    return retval;
    """
    pass

## Check results of analog pin config
def testConfigAnalogPin():
    """__NATIVE__
    PmReturn_t retval = PM_RET_OK;

    EXCEPTION_ASSERT(_PCFG0 == 0);
    EXCEPTION_ASSERT(_TRISA0 == 1);
    EXCEPTION_ASSERT(_CN2PUE == 0);
    return retval;
    """
    pass

import pic24_dspic33 as pic

# Test digital I/O briefly
# ------------------------
#                    port pin isInput isOpenDrain pullDir
dio = pic.digital_io(1,   1,  False,  False,      0)
testConfigDigitalPin1()
#                    port pin isInput isOpenDrain pullDir
dio = pic.digital_io(1,   1,  True,   True,       1)
testConfigDigitalPin2()

#                    port pin isInput isOpenDrain pullDir
dio = pic.digital_io(1,   1,  False,  False,      0)
dio.set(True)
assert(dio.get())
assert(dio.getPin())
assert(dio.getLatch())

# Test analog input
# -----------------
# Set up a digital input, then change to analog to
# verify that the analog config reset everything
# properly.
dio = pic.digital_io(0,   0,  False,  False,      1)
ain = pic.analog_input(0)
testConfigAnalogPin()
#print ain.getCode()

print "All tests passed.\n"
