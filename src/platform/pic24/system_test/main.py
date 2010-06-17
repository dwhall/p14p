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
"""

## Check results of pin config
def testConfigDigitalPin1():
    """__NATIVE__
    PmReturn_t retval = PM_RET_OK;

    EXCEPTION_UNLESS(_PCFG3 == 0, PM_RET_EX_VAL, "");
    EXCEPTION_UNLESS(_TRISB1 == 0, PM_RET_EX_VAL, "");
    EXCEPTION_UNLESS(_ODCB1 == 0, PM_RET_EX_VAL, "");
    EXCEPTION_UNLESS(_CN5PUE == 0, PM_RET_EX_VAL, "");
    return retval;
    """
    pass

## Check results of pin config
def testConfigDigitalPin2():
    """__NATIVE__
    PmReturn_t retval = PM_RET_OK;

    EXCEPTION_UNLESS(_PCFG3 == 0, PM_RET_EX_VAL, "");
    EXCEPTION_UNLESS(_TRISB1 == 1, PM_RET_EX_VAL, "");
    EXCEPTION_UNLESS(_ODCB1 == 1, PM_RET_EX_VAL, "");
    EXCEPTION_UNLESS(_CN5PUE == 1, PM_RET_EX_VAL, "");
    return retval;
    """
    pass

import pic24_dspic33 as pic

#                    port pin isInput isOpenDrain pullDir
pic.configDigitalPin(1,   1,  False,  False,      0)
testConfigDigitalPin1()
#                    port pin isInput isOpenDrain pullDir
#pic.configDigitalPin(1,   1,  True,   True,       1)
#testConfigDigitalPin2()

print "All tess passed.\n"
while True:
    a = 0
