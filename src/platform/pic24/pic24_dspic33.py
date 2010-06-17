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
"""

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

