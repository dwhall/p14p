/*
# This file is Copyright 2003, 2006, 2007, 2009, 2010 Dean Hall.
#
# This file is part of the PyMite VM.
# The PyMite VM is free software: you can redistribute it and/or modify
# it under the terms of the GNU GENERAL PUBLIC LICENSE Version 2.
#
# The PyMite VM is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# A copy of the GNU GENERAL PUBLIC LICENSE Version 2
# is seen in the file COPYING in this directory.
*/


#undef __FILE_ID__
#define __FILE_ID__ 0x05


/**
 * \file
 * \brief VM Globals
 *
 * VM globals operations.
 * PyMite's global struct def and initial values.
 */


#include "pm.h"


extern unsigned char const *stdlib_img;

static uint8_t const *bistr = (uint8_t const *)"__bi";


/** Most PyMite globals all in one convenient place */
volatile PmVmGlobal_t gVmGlobal;


PmReturn_t
global_init(void)
{
    PmReturn_t retval;
    pPmObj_t pobj;

    /* Clear the global struct */
    sli_memset((uint8_t *)&gVmGlobal, '\0', sizeof(PmVmGlobal_t));

    /* Set the PyMite release num (for debug and post mortem) */
    gVmGlobal.errVmRelease = PM_RELEASE;

    /* Init empty builtins */
    gVmGlobal.builtins = C_NULL;

    /* Init native frame */
    OBJ_SET_SIZE(&gVmGlobal.nativeframe, sizeof(PmNativeFrame_t));
    OBJ_SET_TYPE(&gVmGlobal.nativeframe, OBJ_TYPE_NFM);
    gVmGlobal.nativeframe.nf_func = C_NULL;
    gVmGlobal.nativeframe.nf_stack = C_NULL;
    gVmGlobal.nativeframe.nf_active = C_FALSE;
    gVmGlobal.nativeframe.nf_numlocals = 0;

    /* Create empty threadList */
    retval = list_new(&pobj);
    gVmGlobal.threadList = (pPmList_t)pobj;

#ifdef HAVE_PRINT
    gVmGlobal.needSoftSpace = C_FALSE;
    gVmGlobal.somethingPrinted = C_FALSE;
#endif /* HAVE_PRINT */

    return retval;
}


PmReturn_t
global_setBuiltins(pPmFunc_t pmod)
{
    PmReturn_t retval = PM_RET_OK;
    pPmObj_t pkey = C_NULL;
    uint8_t const *pbistr = bistr;
    uint8_t objid;

    if (PM_PBUILTINS == C_NULL)
    {
        /* Need to load builtins first */
        global_loadBuiltins();
    }

    /* Put builtins module in the module's attrs dict */
    retval = string_new(&pbistr, &pkey);
    PM_RETURN_IF_ERROR(retval);

    heap_gcPushTempRoot(pkey, &objid);
    retval = dict_setItem((pPmObj_t)pmod->f_attrs, pkey, PM_PBUILTINS);
    heap_gcPopTempRoot(objid);

    return retval;
}


PmReturn_t
global_loadBuiltins(void)
{
    PmReturn_t retval = PM_RET_OK;
    pPmObj_t pkey = C_NULL;
    uint8_t const *nonestr = (uint8_t const *)"None";
    uint8_t const *falsestr = (uint8_t const *)"False";
    uint8_t const *truestr = (uint8_t const *)"True";
    pPmObj_t pstr = C_NULL;
    pPmObj_t pbimod;
    uint8_t const *pbistr = bistr;

    /* Import the builtins */
    retval = string_new(&pbistr, &pstr);
    PM_RETURN_IF_ERROR(retval);
    retval = mod_import(PM_BI_STR, &pbimod);
    PM_RETURN_IF_ERROR(retval);

    /* Must interpret builtins' root code to set the attrs */
    C_ASSERT(gVmGlobal.threadList->length == 0);
    interp_addThread((pPmFunc_t)pbimod);
    retval = interpret(INTERP_RETURN_ON_NO_THREADS);
    PM_RETURN_IF_ERROR(retval);

    /* Builtins points to the builtins module's attrs dict */
    gVmGlobal.builtins = ((pPmFunc_t)pbimod)->f_attrs;

    /* Set None manually */
    retval = string_new(&nonestr, &pkey);
    PM_RETURN_IF_ERROR(retval);
    retval = dict_setItem(PM_PBUILTINS, pkey, PM_NONE);
    PM_RETURN_IF_ERROR(retval);

    /* Set False manually */
    retval = string_new(&falsestr, &pkey);
    PM_RETURN_IF_ERROR(retval);
    retval = dict_setItem(PM_PBUILTINS, pkey, PM_FALSE);
    PM_RETURN_IF_ERROR(retval);

    /* Set True manually */
    retval = string_new(&truestr, &pkey);
    PM_RETURN_IF_ERROR(retval);
    retval = dict_setItem(PM_PBUILTINS, pkey, PM_TRUE);
    PM_RETURN_IF_ERROR(retval);

    /* Deallocate builtins module */
    retval = heap_freeChunk((pPmObj_t)pbimod);

    return retval;
}
