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

#
# This is a sample application that calls functions in the AVR module.
#

"""__NATIVE__
#include <pic24_all.h>
#include <stdio.h>
"""




#
# Initialize the AVR's port A pin direction
#
def init():
    """__NATIVE__
    printf("Hello C world.\\n");

    return PM_RET_OK;
    """
    pass

#init()
import ipm
print "Welcome to PIC24 Python! Starting interactive mode."

import sys
while True:
	print sys.time()
ipm.ipm()
