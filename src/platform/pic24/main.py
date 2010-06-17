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

# Demo some of the PIC hardware functions
import pic24_dspic33 as pic
pic.configDigitalPin(1, 1, False, False, 0)

import sys
print "Welcome to PIC24 Python! Free space:", sys.heap()
print "Starting interactive mode."
import ipm
ipm.ipm()
