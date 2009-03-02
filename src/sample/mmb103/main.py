# PyMite - A flyweight Python interpreter for 8-bit microcontrollers and more.
# Copyright 2002 Dean Hall
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
#

#
# This is a sample application that calls native functions.
#


"""__NATIVE__
#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include "libmmb103.h"
"""


#
# Do this at the very start to show that PyMite is running
#
import sys
print sys.heap() # (1762,3328)

s = "It's alive!"
print s

import mmb
print sys.heap() # (1322, 3328)


def lcd_print(ps):
    """__NATIVE__
    pPmObj_t ps = C_NULL;
    uint8_t *s = C_NULL;
    PmReturn_t retval;

    /* If wrong number of args, throw type exception */
    if (NATIVE_GET_NUM_ARGS() != 1)
    {
        PM_RAISE(retval, PM_RET_EX_TYPE);
        return retval;
    }

    /* Get the arg, throw type exception if needed */
    ps = NATIVE_GET_LOCAL(0);
    if (OBJ_GET_TYPE(ps) != OBJ_TYPE_STR)
    {
        PM_RAISE(retval, PM_RET_EX_TYPE);
        return retval;
    }

    /* Get a pointer to the string */
    s = ((pPmString_t)ps)->val;

    /* Print the string on the mmb's lcd */
    mmb_lcd_print_str(s);

    /* Return none obj on stack */
    NATIVE_SET_TOS(PM_NONE);

    return PM_RET_OK;
    """
    pass


def lcd_set_line(n):
    """__NATIVE__
    pPmObj_t pn = C_NULL;
    uint8_t n;
    PmReturn_t retval;

    /* If wrong number of args, throw type exception */
    if (NATIVE_GET_NUM_ARGS() != 1)
    {
        PM_RAISE(retval, PM_RET_EX_TYPE);
        return retval;
    }

    /* Get the arg, throw type exception if needed */
    pn = NATIVE_GET_LOCAL(0);
    if (OBJ_GET_TYPE(pn) != OBJ_TYPE_INT)
    {
        PM_RAISE(retval, PM_RET_EX_TYPE);
        return retval;
    }

    /* Get the line number and set the cursor to that line */
    n = (uint8_t)((pPmInt_t)pn)->val;
    mmb_lcd_set_cursor(n, 0);

    /* Return none obj on stack */
    NATIVE_SET_TOS(PM_NONE);

    return PM_RET_OK;
    """
    pass


################################################################################
# Duplicating ipm functions here so I do not have to import ipm (spend heap)   #
################################################################################

#
# Receives an image over the platform's standard connection.
# Returns the image in a string object
#
def _getImg():
    """__NATIVE__
    PmReturn_t retval;
    uint8_t imgType;
    uint16_t imgSize;
    uint8_t *pchunk;
    pPmString_t pimg;
    uint16_t i;
    uint8_t b;

    /* Get the image type (skip any trash at beginning) */
    do
    {
        retval = plat_getByte(&imgType);
        PM_RETURN_IF_ERROR(retval);
    }
    while (imgType != OBJ_TYPE_CIM);

    /* Quit if a code image type was not received */
    if (imgType != OBJ_TYPE_CIM)
    {
        PM_RAISE(retval, PM_RET_EX_STOP);
        return retval;
    }

    /* Get the image size (little endien) */
    retval = plat_getByte(&b);
    PM_RETURN_IF_ERROR(retval);
    imgSize = b;
    retval = plat_getByte(&b);
    PM_RETURN_IF_ERROR(retval);
    imgSize |= (b << 8);

    /* Get space for String obj */
    retval = heap_getChunk(sizeof(PmString_t) + imgSize, &pchunk);
    PM_RETURN_IF_ERROR(retval);
    pimg = (pPmString_t)pchunk;

    /* Set the string object's fields */
    OBJ_SET_TYPE(pimg, OBJ_TYPE_STR);
    pimg->length = imgSize;

    /* Start the image with the bytes that have already been received */
    i = 0;
    pimg->val[i++] = imgType;
    pimg->val[i++] = imgSize & 0xFF;
    pimg->val[i++] = (imgSize >> 8) & 0xFF;

    /* Get the remaining bytes in the image */
    for(; i < imgSize; i++)
    {
        retval = plat_getByte(&b);
        PM_RETURN_IF_ERROR(retval);

        pimg->val[i] = b;
    }

    /* Return the image as a string object on the stack */
    NATIVE_SET_TOS((pPmObj_t)pimg);
    return retval;
    """
    pass


print '~'
#
# Runs the target device-side interactive session.
#
#def ipm():
g = {}
while 1:
    # Wait for a code image, make a code object from it
    # and evaluate the code object.
    rv = eval(Co(_getImg()), g)

    # Send a byte to indicate completion of evaluation
    print '\x04',



# :mode=c:
