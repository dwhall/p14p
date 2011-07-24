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


#ifndef __PM_H__
#define __PM_H__


/**
 * \file
 * \brief PyMite Header
 *
 * Include things that are needed by nearly everything.
 */


#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdio.h>


/**
 * Value indicating the release of PyMite
 *
 * This value should be incremented for every public release.
 * It helps locate a defect when used in conjunction with a fileID
 * and line number.
 */
#define PM_RELEASE 10


/** null for C code */
#define C_NULL 0

/** false for C code */
#define C_FALSE 0

/** true for C code */
#define C_TRUE 1

/** Comparison result is that items are the same */
#define C_SAME (int8_t)0

/** Comparison result is that items differ */
#define C_DIFFER (int8_t)-1

/** PORT inline for C code */
#define INLINE __inline__


/* Convenience macros for global objects */
#define PM_PBUILTINS (pPmObj_t)(gVmGlobal.pbuiltins)
#define PM_NONE (pPmObj_t)&pm_global_none
#define PM_FALSE (pPmObj_t)&pm_global_false
#define PM_TRUE (pPmObj_t)&pm_global_true
#define PM_ZERO (pPmObj_t)&pm_global_zero
#define PM_ONE (pPmObj_t)&pm_global_one
#define PM_NEGONE (pPmObj_t)&pm_global_negone
#define PM_CODE_STR (pPmObj_t)&pm_global_string_code
#define PM_INIT_STR (pPmObj_t)&pm_global_string_init
#define PM_GENERATOR_STR (pPmObj_t)&pm_global_string_generator
#define PM_NEXT_STR (pPmObj_t)&pm_global_string_next
#define PM_EXCEPTION_STR (pPmObj_t)&pm_global_string_exception
#define PM_BYTEARRAY_STR (pPmObj_t)&pm_global_string_bytearray
#define PM_MD_STR (pPmObj_t)&pm_global_string_md
#define PM_BI_STR (pPmObj_t)&pm_global_string_bi


/**
 * Returns an exception error code and stores debug data
 *
 * This macro must be used as an rval statement.  That is, it must
 * be used after an assignment such as "retval = " or a return statement
 */
#define PM_RAISE(retexn, exn) \
    do \
    { \
        retexn = (exn); \
        gVmGlobal.errFileId = __FILE_ID__; \
        gVmGlobal.errLineNum = (uint16_t)__LINE__; \
    } while (0)

/** Breaks from the block if retval is not PM_RET_OK */
#define PM_BREAK_IF_ERROR(retval) if ((retval) != PM_RET_OK) break

/** Returns an error code if it is not PM_RET_OK */
#define PM_RETURN_IF_ERROR(retval) if ((retval) != PM_RET_OK) return (retval)

/** Pops a temp root and returns an error code if it is not PM_RET_OK */
#define PM_POP_ROOT_AND_RETURN_IF_ERROR(retval, id) \
    do \
    { \
        if ((retval) != PM_RET_OK) \
        { \
            heap_gcPopTempRoot(id); \
            return (retval); \
        } \
    } while (0)

/** Prints an error message if argument is not PM_RET_OK */
#define PM_REPORT_IF_ERROR(retval) if ((retval) != PM_RET_OK) \
                                       plat_reportError(retval)

/** Jumps to a label if argument is not PM_RET_OK */
#define PM_GOTO_IF_ERROR(retval, target) if ((retval) != PM_RET_OK) goto target

#if __DEBUG__
/** If the boolean expression fails, return the ASSERT error code */
#define C_ASSERT(boolexpr) \
    do \
    { \
        if (!((boolexpr))) \
        { \
            gVmGlobal.errFileId = __FILE_ID__; \
            gVmGlobal.errLineNum = (uint16_t)__LINE__; \
            return PM_RET_ASSERT_FAIL; \
        } \
    } \
    while (0)

#else
/** Assert statements are removed from production code */
#define C_ASSERT(boolexpr)
#endif

/** Use as the first argument to C_DEBUG_PRINT for low volume messages */
#define VERBOSITY_LOW 1

/** Use as the first argument to C_DEBUG_PRINT for medium volume messages */
#define VERBOSITY_MEDIUM 2

/** Use as the first argument to C_DEBUG_PRINT for high volume messages */
#define VERBOSITY_HIGH 3

#if __DEBUG__

/** To be used to set DEBUG_PRINT_VERBOSITY to a value so no prints occur */
#define VERBOSITY_OFF 0

/** Sets the level of verbosity to allow in debug prints */
#define DEBUG_PRINT_VERBOSITY VERBOSITY_OFF

/** Prints a debug message when the verbosity is within the set value */
#define C_DEBUG_PRINT(v, f, ...) \
    do \
    { \
        if (DEBUG_PRINT_VERBOSITY >= (v)) \
        { \
            printf("PM_DEBUG: " f, ## __VA_ARGS__); \
        } \
    } \
    while (0)

#else
#define C_DEBUG_PRINT(...)
#endif


/**
 * Return values for system functions
 * to report status, errors, exceptions, etc.
 * Normally, functions which use these values
 * should propagate the same return value
 * up the call tree to the interpreter.
 */
typedef enum PmReturn_e
{
    /* general status return values */
    PM_RET_OK = 0,              /**< Everything is ok */
    PM_RET_NO = 0xFF,           /**< General "no result" */
    PM_RET_ERR = 0xFE,          /**< General failure */
    PM_RET_STUB = 0xFD,         /**< Return val for stub fxn */
    PM_RET_ASSERT_FAIL = 0xFC,  /**< Assertion failure */
    PM_RET_FRAME_SWITCH = 0xFB, /**< Frame pointer was modified */

    /* return vals that indicate an exception occured */
    PM_RET_EX = 0xE0,           /**< General exception */
    PM_RET_EX_EXIT = 0xE1,      /**< System exit */
    PM_RET_EX_IO = 0xE2,        /**< Input/output error */
    PM_RET_EX_ZDIV = 0xE3,      /**< Zero division error */
    PM_RET_EX_ASSRT = 0xE4,     /**< Assertion error */
    PM_RET_EX_ATTR = 0xE5,      /**< Attribute error */
    PM_RET_EX_IMPRT = 0xE6,     /**< Import error */
    PM_RET_EX_INDX = 0xE7,      /**< Index error */
    PM_RET_EX_KEY = 0xE8,       /**< Key error */
    PM_RET_EX_MEM = 0xE9,       /**< Memory error */
    PM_RET_EX_NAME = 0xEA,      /**< Name error */
    PM_RET_EX_SYNTAX = 0xEB,    /**< Syntax error */
    PM_RET_EX_SYS = 0xEC,       /**< System error */
    PM_RET_EX_TYPE = 0xED,      /**< Type error */
    PM_RET_EX_VAL = 0xEE,       /**< Value error */
    PM_RET_EX_STOP = 0xEF,      /**< Stop iteration */
    PM_RET_EX_WARN = 0xF0,      /**< Warning */
    PM_RET_EX_OFLOW = 0xF1,     /**< Overflow */
} PmReturn_t;


/* WARNING: The order of the following includes is critical */
#include "plat.h"
#include "pmfeatures.h"
#include "pmEmptyPlatformDefs.h"
#include "sli.h"
#include "obj.h"
#include "seq.h"
#include "tuple.h"
#include "strobj.h"
#include "heap.h"
#include "int.h"
#include "seglist.h"
#include "list.h"
#include "dict.h"
#include "codeobj.h"
#include "func.h"
#include "module.h"
#include "frame.h"
#include "interp.h"
#include "class.h"
#include "thread.h"
#include "float.h"
#include "plat_interface.h"
#include "bytearray.h"
#include "marshal.h"


/** Pointer to a native function used for lookup tables in interp.c */
typedef PmReturn_t (* pPmNativeFxn_t)(pPmFrame_t *);

/**
 * Native Code Object
 *
 * A Code Object that executes a native C function instead of bytecode.
 * This struct holds the number of arguments the function expects on the
 * argument stack and a pointer to the C function to execute.
 */
typedef struct PmNo_s
{
    PmObjDesc_t const od;
    int8_t no_argcount;
    pPmNativeFxn_t no_func;
} PmNo_t, *pPmNo_t;


/**
 * A Module Entry holds the module's name and a pointer to its code object.
 * The array of module entries that lists the modules built in to the VM
 * is initialized in pm_generated_codeobjs.c
 */
typedef struct PmModuleEntry_s
{
    pPmString_t pnm;
    pPmCo_t pco;
} PmModuleEntry_t, *pPmModuleEntry;


/**
 * This struct contains ALL of PyMite's globals
 */
typedef struct PmVmGlobal_s
{
    /** Dict for builtins */
    pPmDict_t pbuiltins;

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


/** Object descriptor declaration macro (used in output of pmCoCreator.py) */
#define PM_DECLARE_OD(type, size) \
    ((((type) << OD_TYPE_SHIFT) & OD_TYPE_MASK) \
     | (((size) << OD_SIZE_SHIFT) & OD_SIZE_MASK))

/** String object declaration macro (used by output of pmCoCreator.py) */
#if USE_STRING_CACHE
#define PM_DECLARE_STRING_TYPE(n) \
    typedef struct PmString ## n ## _s \
    { \
        PmObjDesc_t od; \
        int16_t length; \
        struct PmString_s *next; \
        uint8_t val[n+1]; \
    } PmString ## n ## _t
#else
#define PM_DECLARE_STRING_TYPE(n) \
    typedef struct PmString ## n ## _s \
    { \
        PmObjDesc_t od; \
        int16_t length; \
        uint8_t val[n]; \
    } PmString ## n ## _t
#endif /* USE_STRING_CACHE */

/** Tuple object declaration macro (used by output of pmCoCreator.py) */
#define PM_DECLARE_TUPLE_TYPE(n) \
    typedef struct PmTuple ## n ## _s \
    { \
        PmObjDesc_t od; \
        int16_t length; \
        pPmObj_t val[n]; \
    } PmTuple ## n ## _t

/** Used by the code generator in pmCoCreator.py */
#ifdef HAVE_DEBUG_INFO
#define PM_REFERENCE_LNOTAB(s) (pPmString_t)&(s)
#else
#define PM_REFERENCE_LNOTAB(s) C_NULL
#endif


/* Types and globals that were generated by pmCoCreator.py */
#include "pm_generated_types.h"
extern PmNone_t PM_PLAT_PROGMEM pm_global_none;
extern PmInt_t PM_PLAT_PROGMEM pm_global_negone;
extern PmInt_t PM_PLAT_PROGMEM pm_global_zero;
extern PmInt_t PM_PLAT_PROGMEM pm_global_one;
extern PmInt_t PM_PLAT_PROGMEM pm_global_two;
extern PmInt_t PM_PLAT_PROGMEM pm_global_three;
extern PmInt_t PM_PLAT_PROGMEM pm_global_four;
extern PmInt_t PM_PLAT_PROGMEM pm_global_five;
extern PmInt_t PM_PLAT_PROGMEM pm_global_five;
extern PmInt_t PM_PLAT_PROGMEM pm_global_six;
extern PmInt_t PM_PLAT_PROGMEM pm_global_seven;
extern PmInt_t PM_PLAT_PROGMEM pm_global_eight;
extern PmInt_t PM_PLAT_PROGMEM pm_global_nine;
extern PmBoolean_t PM_PLAT_PROGMEM pm_global_true;
extern PmBoolean_t PM_PLAT_PROGMEM pm_global_false;
extern PmString4_t PM_PLAT_PROGMEM pm_global_string_bi;
extern PmString4_t PM_PLAT_PROGMEM pm_global_string_md;
extern PmString4_t PM_PLAT_PROGMEM pm_global_string_code;
extern PmString8_t PM_PLAT_PROGMEM pm_global_string_init;
extern PmString4_t PM_PLAT_PROGMEM pm_global_string_next;
extern PmTuple0_t PM_PLAT_PROGMEM pm_global_empty_tuple;
extern PmString0_t PM_PLAT_PROGMEM pm_global_empty_string;
extern PmString9_t PM_PLAT_PROGMEM pm_global_string_generator;
extern PmString9_t PM_PLAT_PROGMEM pm_global_string_exception;
extern PmString9_t PM_PLAT_PROGMEM pm_global_string_bytearray;
extern PmString4_t PM_PLAT_PROGMEM pm_global_string_none;
extern PmString5_t PM_PLAT_PROGMEM pm_global_string_false;
extern PmString4_t PM_PLAT_PROGMEM pm_global_string_true;
extern PmString8_t PM_PLAT_PROGMEM pm_global_string_code_attr;

extern PmInt_t PM_PLAT_PROGMEM * const pm_global_module_table_len_ptr;
extern PmModuleEntry_t PM_PLAT_PROGMEM pm_global_module_table[];

extern volatile PmVmGlobal_t gVmGlobal;
extern volatile uint32_t pm_timerMsTicks;


/**
 * Initializes the PyMite virtual machine and indexes the user's application
 * image.  The VM heap and globals are reset.  The argument, pusrimg, may be
 * null for interactive sessions.
 *
 * @param heap_base The address where the contiguous heap begins
 * @param heap_size The size in bytes (octets) of the given heap.
 *             Must be a multiple of four.
 * @param memspace      Memory space in which the user image is located
 * @param pusrimg       Address of the user image in the memory space
 * @return Return status
 */
PmReturn_t pm_init(uint8_t *heap_base, uint32_t heap_size);

/**
 * Executes the named module
 *
 * @param modstr        Name of module to run
 * @return Return status
 */
PmReturn_t pm_run(uint8_t const *modstr);

/**
 * Needs to be called periodically by the host program.
 * For the desktop target, it is periodically called using a signal.
 * For embedded targets, it needs to be called periodically. It should
 * be called from a timer interrupt.
 *
 * @param usecsSinceLastCall Microseconds (not less than those) that passed
 *                           since last call. This must be <64535.
 * @return Return status
 */
PmReturn_t pm_vmPeriodic(uint16_t usecsSinceLastCall);

#ifdef __cplusplus
}
#endif

#endif /* __PM_H__ */
