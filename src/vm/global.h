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


#ifndef __GLOBAL_H__
#define __GLOBAL_H__


/**
 * \file
 * \brief VM Globals
 *
 * VM globals header.
 */


/** The global root PmGlobals Dict object */
#define PM_PBUILTINS    (pPmObj_t)(gVmGlobal.builtins)

/** The global None object */
#define PM_NONE         (pPmObj_t)&pm_global_none

/** The global False object */
#define PM_FALSE        (pPmObj_t)&pm_global_false

/** The global True object */
#define PM_TRUE         (pPmObj_t)&pm_global_true

/** The global integer 0 object */
#define PM_ZERO         (pPmObj_t)&pm_global_zero

/** The global integer 1 object */
#define PM_ONE          (pPmObj_t)&pm_global_one

/** The global integer -1 object */
#define PM_NEGONE       (pPmObj_t)&pm_global_negone

/** The global string "code" */
#define PM_CODE_STR     (pPmObj_t)&pm_global_string_code

/** The global string "__init__" */
#define PM_INIT_STR     (pPmObj_t)&pm_global_string_init

/** The global string "Generator" */
#define PM_GENERATOR_STR (pPmObj_t)&pm_global_string_generator
/** The global string "next" */
#define PM_NEXT_STR (pPmObj_t)&pm_global_string_next

/** The global string "Exception" */
#define PM_EXCEPTION_STR (pPmObj_t)&pm_global_string_exception

/** The global string "bytearray" */
#define PM_BYTEARRAY_STR (pPmObj_t)&pm_global_string_bytearray

/** The global string "__md" */
#define PM_MD_STR (pPmObj_t)&pm_global_string_md

/** The global string "__bi" */
#define PM_BI_STR (pPmObj_t)&pm_global_string_bi


/**
 * This struct contains ALL of PyMite's globals
 */
typedef struct PmVmGlobal_s
{
    /** Dict for builtins */
    pPmDict_t builtins;

    /** The single native frame.  Static alloc so it won't be GC'd */
    PmNativeFrame_t nativeframe;

    /** PyMite release value for when an error occurs */
    uint8_t errVmRelease;

    /** PyMite source file ID number for when an error occurs */
    uint8_t errFileId;

    /** Line number for when an error occurs */
    uint16_t errLineNum;

    /** Thread list */
    pPmList_t threadList;

    /** Ptr to current thread */
    pPmThread_t pthread;

    /** Remembers when a space is needed before printing the next object */
    uint8_t needSoftSpace;

    /** Remembers when something has printed since the last newline */
    uint8_t somethingPrinted;

    /** Flag to trigger rescheduling */
    uint8_t reschedule;
} PmVmGlobal_t,
 *pPmVmGlobal_t;


extern volatile PmVmGlobal_t gVmGlobal;


/**
 * Initializes the global struct
 *
 * @return Return status
 */
PmReturn_t global_init(void);

/**
 * Sets the builtins dict into the given module's attrs.
 *
 * If not yet done, loads the "__bt" module via global_loadBuiltins().
 * Restrictions described in that functions documentation apply.
 *
 * @param pmod Module whose attrs receive builtins
 * @return Return status
 */
PmReturn_t global_setBuiltins(pPmFunc_t pmod);

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
PmReturn_t global_loadBuiltins(void);

#endif /* __GLOBAL_H__ */
