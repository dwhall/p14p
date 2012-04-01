/*
# This file is Copyright 2006, 2007, 2009, 2010 Dean Hall.
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
#define __FILE_ID__ 0x15


/**
 * \file
 * \brief PyMite User API
 *
 * High-level functions to initialize and run PyMite
 */


#include "pm.h"


/** Number of millisecond-ticks to pass before scheduler is run */
#define PM_THREAD_TIMESLICE_MS  10


/** Stores the timer millisecond-ticks since system start */
volatile uint32_t pm_timerMsTicks = 0;

/** Stores tick timestamp of last scheduler run */
volatile uint32_t pm_lastRescheduleTimestamp = 0;

/** Most PyMite globals all in one convenient place */
volatile PmVmGlobal_t gVmGlobal;


/**
 * Loads the "__bt" module and sets the builtins dict (PM_PBUILTINS)
 * to point to __bt's attributes dict.
 * Creates "None" = None entry in builtins.
 *
 * When run, there should not be any other threads in the interpreter
 * thread list yet.
 *
 * @return  Return status
 */
static PmReturn_t
pm_loadBuiltins(void)
{
    PmReturn_t retval = PM_RET_OK;
    pPmObj_t pbimod;

    /* Import the builtins */
    retval = mod_import(PM_BI_STR, &pbimod);
    PM_RETURN_IF_ERROR(retval);

    /* Must interpret builtins' root code to set the attrs */
    C_ASSERT(gVmGlobal.threadList->length == 0);
    interp_addThread((pPmFunc_t)pbimod);
    retval = interpret(INTERP_RETURN_ON_NO_THREADS);
    PM_RETURN_IF_ERROR(retval);

    /* Builtins points to the builtins module's attrs dict */
    gVmGlobal.pbuiltins = ((pPmFunc_t)pbimod)->f_attrs;

    /* Set None, False and True */
    retval = dict_setItem(PM_PBUILTINS, (pPmObj_t)&pm_global_string_none, PM_NONE);
    PM_RETURN_IF_ERROR(retval);
    retval = dict_setItem(PM_PBUILTINS, (pPmObj_t)&pm_global_string_false, PM_FALSE);
    PM_RETURN_IF_ERROR(retval);
    retval = dict_setItem(PM_PBUILTINS, (pPmObj_t)&pm_global_string_true, PM_TRUE);
    PM_RETURN_IF_ERROR(retval);

    /* Deallocate builtins module */
    retval = heap_freeChunk((pPmObj_t)pbimod);

    return retval;
}


PmReturn_t
pm_init(uint8_t *heap_base, uint32_t heap_size)
{
    PmReturn_t retval;
    pPmObj_t pobj;

    /* Initialize the heap and the globals */
    retval = heap_init(heap_base, heap_size);
    PM_RETURN_IF_ERROR(retval);

    /* Clear the global struct */
    sli_memset((uint8_t *)&gVmGlobal, '\0', sizeof(PmVmGlobal_t));

    /* Set the PyMite release num (for debug and post mortem) */
    gVmGlobal.errVmRelease = PM_RELEASE;

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

    /* Load builtins module */
    retval = pm_loadBuiltins();
    PM_RETURN_IF_ERROR(retval);

    return retval;
}


PmReturn_t
pm_run(uint8_t const *modstr)
{
    PmReturn_t retval;
    pPmObj_t pmod;
    pPmObj_t pstring;
    uint8_t const *pmodstr = modstr;
    uint8_t objid1;
    uint8_t objid2;

    /* Import module from global struct */
    retval = string_new(&pmodstr, &pstring);
    PM_RETURN_IF_ERROR(retval);
    heap_gcPushTempRoot(pstring, &objid1);
    retval = mod_import(pstring, &pmod);
    PM_RETURN_IF_ERROR(retval);

    /* Put builtins module in the module's attrs dict */
    heap_gcPushTempRoot(pmod, &objid2);
    retval = dict_setItem((pPmObj_t)((pPmFunc_t)pmod)->f_attrs,
                          (pPmObj_t)&pm_global_string_bi,
                          PM_PBUILTINS);
    PM_RETURN_IF_ERROR(retval);

    /* Interpret the module's bcode */
    retval = interp_addThread((pPmFunc_t)pmod);
    heap_gcPopTempRoot(objid1);
    PM_RETURN_IF_ERROR(retval);
    retval = interpret(INTERP_RETURN_ON_NO_THREADS);

    return retval;
}


/* Warning: Can be called in interrupt context! */
PmReturn_t
pm_vmPeriodic(uint16_t usecsSinceLastCall)
{
    /*
     * Add the full milliseconds to pm_timerMsTicks and store additional
     * microseconds for the next run. Thus, usecsSinceLastCall must be
     * less than 2^16-1000 so it will not overflow usecResidual.
     */
    static uint16_t usecResidual = 0;

    C_ASSERT(usecsSinceLastCall < 64536);

    usecResidual += usecsSinceLastCall;
    while (usecResidual >= 1000)
    {
        usecResidual -= 1000;
        pm_timerMsTicks++;
    }

    /* Check if enough time has passed for a scheduler run */
    if ((pm_timerMsTicks - pm_lastRescheduleTimestamp)
        >= PM_THREAD_TIMESLICE_MS)
    {
        interp_setRescheduleFlag((uint8_t)1);
        pm_lastRescheduleTimestamp = pm_timerMsTicks;
    }
    return PM_RET_OK;
}
