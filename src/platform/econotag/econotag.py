# This file is Copyright 2010 Dean Hall.
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

"""__NATIVE__
#include "mc1322x.h"
#include "board.h"
"""


def set_channel(n):
    """__NATIVE__
    PmReturn_t retval = PM_RET_OK;
    pPmObj_t pn;
    uint8_t n;

    /* Raise TypeError if wrong number of args */
    if (NATIVE_GET_NUM_ARGS() != 1)
    {
        PM_RAISE(retval, PM_RET_EX_TYPE);
        return retval;
    }

    /* Raise TypeError if arg is not an int */
    pn = NATIVE_GET_LOCAL(0);
    if (OBJ_GET_TYPE(pn) != OBJ_TYPE_INT)
    {
        PM_RAISE(retval, PM_RET_EX_TYPE);
        return retval;
    }

    /* Raise ValueError if arg is not within 0..15 inclusive */
    n = (uint8_t)(((pPmInt_t)pn)->val);
    if (n >= (uint8_t)16)
    {
        PM_RAISE(retval, PM_RET_EX_VAL);
        return retval;
    }

    maca_off();
    set_channel(n);
    maca_on();

    NATIVE_SET_TOS(PM_NONE);
    return retval;
    """
    pass


def set_power(n):
    """__NATIVE__
    PmReturn_t retval = PM_RET_OK;
    pPmObj_t pn;
    uint8_t n;

    /* Raise TypeError if wrong number of args */
    if (NATIVE_GET_NUM_ARGS() != 1)
    {
        PM_RAISE(retval, PM_RET_EX_TYPE);
        return retval;
    }

    /* Raise TypeError if arg is not an int */
    pn = NATIVE_GET_LOCAL(0);
    if (OBJ_GET_TYPE(pn) != OBJ_TYPE_INT)
    {
        PM_RAISE(retval, PM_RET_EX_TYPE);
        return retval;
    }

    /* Raise ValueError if arg is not within 0..18 inclusive */
    n = (uint8_t)(((pPmInt_t)pn)->val);
    if (n > (uint8_t)18)
    {
        PM_RAISE(retval, PM_RET_EX_VAL);
        return retval;
    }

    maca_off();
    set_power(n);
    maca_on();

    NATIVE_SET_TOS(PM_NONE);
    return retval;
    """
    pass


def check_maca():
    """__NATIVE__
    PmReturn_t retval = PM_RET_OK;

    /* Raise TypeError if wrong number of args */
    if (NATIVE_GET_NUM_ARGS() != 0)
    {
        PM_RAISE(retval, PM_RET_EX_TYPE);
        return retval;
    }

    check_maca();

    NATIVE_SET_TOS(PM_NONE);
    return retval;
    """
    pass


def tx_packet(p):
    """__NATIVE__
    PmReturn_t retval = PM_RET_OK;
	volatile packet_t *p;
	pPmObj_t ppkt;
	pPmBytearray_t pba;
	pPmString_t pstr;
	pPmInt_t pn;

    /* Raise TypeError if wrong number of args */
    if (NATIVE_GET_NUM_ARGS() != 1)
    {
        PM_RAISE(retval, PM_RET_EX_TYPE);
        return retval;
    }

    ppkt = NATIVE_GET_LOCAL(0);

    /* If object is an instance, get the thing it is containing */
    if (OBJ_GET_TYPE(ppkt) == OBJ_TYPE_CLI)
    {
        retval = dict_getItem((pPmObj_t)((pPmInstance_t)ppkt)->cli_attrs,
                              PM_NONE,
                              (pPmObj_t *)&pba);
        PM_RETURN_IF_ERROR(retval);
        ppkt = (pPmObj_t)pba;
    }

    /* Raise TypeError if arg is not a string or bytearray */
    if ((OBJ_GET_TYPE(ppkt) != OBJ_TYPE_STR)
        && (OBJ_GET_TYPE(ppkt) != OBJ_TYPE_BYA))
    {
        NATIVE_SET_TOS(PM_NONE);
        PM_RAISE(retval, PM_RET_EX_TYPE);
        return retval;
    }

    p = get_free_packet();
    if (!p)
    {
        /* TODO: raise an IoError (when VM can catch exceptions) */
        NATIVE_SET_TOS(PM_NONE);
        return retval;
    }

    /* Copy bytes from string arg to packet */
    p->offset = 0;
    if (OBJ_GET_TYPE(ppkt) == OBJ_TYPE_STR)
    {
        pstr = (pPmString_t)ppkt;
        sli_memcpy((unsigned char *)&(p->data[0]),
                   (unsigned char *)&(pstr->val[0]),
                   (unsigned int)pstr->length);
        p->length = pstr->length;
    }

    /* Copy bytes from bytearray arg to packet */
    else
    {
        pba = (pPmBytearray_t)ppkt;
        sli_memcpy((unsigned char *)&(p->data[0]),
                   (unsigned char *)&(pba->val->val[0]),
                   (unsigned int)pba->length);
        p->length = pba->length;
    }

    retval = int_new(p->length, &pn);
    NATIVE_SET_TOS(pn);

    tx_packet(p);

    return retval;
    """
    pass


def rx_packet():
    """__NATIVE__
    PmReturn_t retval = PM_RET_OK;
	volatile packet_t *p;
	pPmBytes_t pb;
	pPmBytearray_t pba;
	pPmInt_t pn;

	p = rx_packet();
	if (!p)
	{
        NATIVE_SET_TOS(PM_NONE);
        return retval;
	}

    /* Allocate a bytearray */
    retval = int_new(p->length, &pn);
    if (retval != PM_RET_OK)
    {
        free_packet(p);
        return retval;
    }
    retval = bytearray_new(pn, &pba);
    if (retval != PM_RET_OK)
    {
        free_packet(p);
        return retval;
    }

    /* Copy packet payload to bytearray */
    sli_memcpy((unsigned char *)&(pba->val->val[0]),
               (unsigned char *)&(p->data[0]),
               (unsigned int)p->length);

    free_packet(p);
    NATIVE_SET_TOS(pba);
    return retval;
    """
    pass


# :mode=c:
