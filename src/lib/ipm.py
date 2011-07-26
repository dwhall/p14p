# This file is Copyright 2007, 2009, 2010 Dean Hall.
#
# This file is part of the Python-on-a-Chip program.
# Python-on-a-Chip is free software: you can redistribute it and/or modify
# it under the terms of the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1.
#
# Python-on-a-Chip is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# A copy of the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1
# is seen in the file COPYING in this directory.

## @file
#  @copybrief ipm_target

## @package ipm_target
#  @brief Provides PyMite's interactive interface for the target.


##
# Receives a bytestream over the platform's standard connection.
# Converts the byte stream into an object using marshal's load function.
# Returns the resulting object on the stack.
# Note: Read and load are performed in this function so that the pmMarshal
# module does not have to be loaded into RAM.
#
def _read_and_load():
    """__NATIVE__
    PmReturn_t retval;
    uint16_t size;
    uint8_t *pchunk;
    pPmString_t pstr;
    pPmObj_t pobj;
    uint16_t i;
    uint8_t b;
    uint8_t objid;

    /* Get the size (little endien) */
    retval = plat_getByte(&b);
    PM_RETURN_IF_ERROR(retval);
    size = b;
    retval = plat_getByte(&b);
    PM_RETURN_IF_ERROR(retval);
    size |= (b << 8);

    /* Allocate space to receive bytes */
    retval = heap_getChunk(sizeof(PmString_t) + size, &pchunk);
    PM_RETURN_IF_ERROR(retval);
    pstr = (pPmString_t)pchunk;
    OBJ_SET_TYPE(pstr, OBJ_TYPE_STR);
    pstr->length = size;
    /* NOTE: Assumes USE_STRING_CACHE is non-zero in strobj.h */
    pstr->next = C_NULL;

    /* Get the payload bytes */
    for(i = 0; i < size; i++)
    {
        retval = plat_getByte(&b);
        PM_RETURN_IF_ERROR(retval);
        pstr->val[i] = b;
    }
    pstr->val[i] = '\\0';

    /* Load the marshaled string to an object. */
    heap_gcPushTempRoot((pPmObj_t)pstr, &objid);
    retval = marshal_load(pstr->val, pstr->length, &pobj);
    heap_gcPopTempRoot(objid);
    PM_RETURN_IF_ERROR(retval);
    NATIVE_SET_TOS(pobj);

    /* Free the string */
    retval = heap_freeChunk((pPmObj_t)pstr);

    return retval;
    """
    pass


def x04():
    """__NATIVE__
    NATIVE_SET_TOS(PM_NONE);
    return plat_putByte(0x04);
    """
    pass


##
# Runs the target device-side interactive session.
#
def ipm(g={}):
    while 1:
        co = _read_and_load()
        rv = eval(co, g)
        x04()

    # Execution should never reach here
    # The while loop (above) probably caught a StopIteration, accidentally
    assert False

# :mode=c:
