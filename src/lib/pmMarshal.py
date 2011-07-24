#! /usr/bin/env python

# This file is Copyright 2011 Dean Hall.
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


# Marshals some Python objects to a serial format
# and unmarshals the serial format to a Python object.
# The serialization format used here differs from that of Python's.
# See here for p14p's marshal format: docs/src/MarshalFormat.rst
# And here for Python's marshal format:
# http://docs.python.org/library/marshal.html
# http://daeken.com/python-marshal-format


def loads(s):
    """__NATIVE__
    /* Returns a new object described by the serialzed data. */
    pPmObj_t ps;
    pPmObj_t po;
    PmReturn_t retval = PM_RET_OK;

    /* TypeError if not one argument */
    if (NATIVE_GET_NUM_ARGS() != 1)
    {
        PM_RAISE(retval, PM_RET_EX_TYPE);
    }

    /* TypeError if s is not a String */
    ps = NATIVE_GET_LOCAL(0);
    if (OBJ_GET_TYPE(ps) != OBJ_TYPE_STR)
    {
        PM_RAISE(retval, PM_RET_EX_TYPE);
        return retval;
    }

    retval = marshal_load(((pPmString_t)ps)->val, ((pPmString_t)ps)->length, &po);
    NATIVE_SET_TOS(po);
    return retval;
    """
    pass


def dumps(o):
    """__NATIVE__
    /* Returns a string that is the serialized representation of the argument */
    pPmObj_t po;
    pPmObj_t ps;
    PmReturn_t retval = PM_RET_OK;

    /* TypeError if not one argument */
    if (NATIVE_GET_NUM_ARGS() != 1)
    {
        PM_RAISE(retval, PM_RET_EX_TYPE);
    }

    /* pmMarshal_dump will return TypeError if s is not a serializable type */
    po = NATIVE_GET_LOCAL(0);
    retval = marshal_dump(po, &ps);
    NATIVE_SET_TOS(ps);
    return retval;
    """
    pass

# :mode=c:
