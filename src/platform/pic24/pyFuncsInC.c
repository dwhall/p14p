/** \file
 *  \brief This file implements Python functions defined in main.py.
 */

#include <pic24_all.h>
#include "pyToC.h"
#include "pyFuncsInC.h"
#include <stdio.h>

#undef __FILE_ID__
#define __FILE_ID__ 0x70

bool_t getBit(uint16_t u16_bitfield, uint16_t u16_bit)
{
    ASSERT(u16_bit < 16);
    return (u16_bitfield & (1 << u16_bit)) ? C_TRUE : C_FALSE;
}


void setBit(volatile uint16_t* pu16_bitfield, uint16_t u16_bit, bool_t b_val)
{
    ASSERT(u16_bit < 16);
    if (b_val)
        *pu16_bitfield |= 1 << u16_bit;
    else
        *pu16_bitfield &= ~(1 << u16_bit);
}

/** C strings to convert to Python strings. */
static const char* psz_port = "port";
static const char* psz_pin = "pin";

/** Python strings to hold C strings above. */
static pPmObj_t ppo_portStr;
static pPmObj_t ppo_pinStr;

PmReturn_t
initIoConstPy(pPmFrame_t *ppframe)
{
    PmReturn_t retval = PM_RET_OK;

    initIoConst();
    CHECK_NUM_ARGS(0);
    PM_CHECK_FUNCTION( string_new(&psz_port, &ppo_portStr) );
    PM_CHECK_FUNCTION( string_new(&psz_pin, &ppo_pinStr) );
    NATIVE_SET_TOS(PM_NONE);

    return retval;
}

PmReturn_t configDigitalPinPy(pPmFrame_t *ppframe)
{
    PmReturn_t retval = PM_RET_OK;
    uint16_t u16_port;
    uint16_t u16_pin;
    bool_t b_isInput;
    bool_t b_isOpenDrain;
    int16_t i16_pullDir;
    pPmObj_t ppo_self;
    pPmObj_t ppo_attrs;
    pPmObj_t ppo_portVal;
    pPmObj_t ppo_pinVal;

    // Get the arguments
    CHECK_NUM_ARGS(6);
    GET_UINT16_ARG(1, &u16_port);
    GET_UINT16_ARG(2, &u16_pin);
    GET_BOOL_ARG(3, &b_isInput);
    GET_BOOL_ARG(4, &b_isOpenDrain);
    GET_INT16_ARG(5, &i16_pullDir);

    // Argument 0 is a pointer to the object.
    // Store port and pin in it.
    // Raise TypeError if address isn't an object
    ppo_self = NATIVE_GET_LOCAL(0);
    EXCEPTION_UNLESS(OBJ_GET_TYPE(ppo_self) == OBJ_TYPE_CLI, PM_RET_EX_TYPE, 
      "Argument 0 must be an class instance");
    ppo_attrs = (pPmObj_t)((pPmInstance_t)ppo_self)->cli_attrs;
    PM_CHECK_FUNCTION( int_new(u16_port, &ppo_portVal) );
    PM_CHECK_FUNCTION( dict_setItem(ppo_attrs, ppo_portStr, ppo_portVal) );
    PM_CHECK_FUNCTION( int_new(u16_port, &ppo_pinVal) );
    PM_CHECK_FUNCTION( dict_setItem(ppo_attrs, ppo_pinStr, ppo_pinVal) );

    PM_CHECK_FUNCTION( configDigitalPin(u16_port, u16_pin, b_isInput, 
        b_isOpenDrain, i16_pullDir) );
    NATIVE_SET_TOS(PM_NONE);

    return retval;
}

/** Get the port and pin from a digital I/O object.
 *  @param ppframe Stack frame containing Python arguments.
 *                 Argument 0 should be the object.
 *  @param pu16_port The port number taken from the object is
 *                   stored here.
 *  @param pu16_pin  The pin number taken from the object is
 *                   stored here.
 */
static PmReturn_t
getPyPortPin(pPmFrame_t *ppframe, uint16_t* pu16_port, uint16_t* pu16_pin)
{
    PmReturn_t retval = PM_RET_OK;
    pPmObj_t ppo_self;
    pPmObj_t ppo_attrs;
    pPmObj_t ppo_portVal;
    pPmObj_t ppo_pinVal;

    // Argument 0 is a pointer to the object.
    // Store port and pin in it.
    // Raise TypeError if address isn't an object
    ppo_self = NATIVE_GET_LOCAL(0);
    EXCEPTION_UNLESS(OBJ_GET_TYPE(ppo_self) == OBJ_TYPE_CLI, PM_RET_EX_TYPE, 
      "Argument 0 must be an class instance");
    ppo_attrs = (pPmObj_t) ((pPmInstance_t) ppo_self)->cli_attrs;
    PM_CHECK_FUNCTION( dict_getItem(ppo_attrs, ppo_portStr, &ppo_portVal) );
    PM_CHECK_FUNCTION( dict_getItem(ppo_attrs, ppo_pinStr, &ppo_pinVal) );
    PM_CHECK_FUNCTION( getUint16(ppo_portVal, pu16_port) );
    PM_CHECK_FUNCTION( getUint16(ppo_pinVal, pu16_pin) );

    return retval;
}

PmReturn_t
setDigitalPinPy(pPmFrame_t *ppframe)
{
    PmReturn_t retval = PM_RET_OK;
    uint16_t u16_port;
    uint16_t u16_pin;
    bool_t b_isHigh;

    // Get the arguments
    CHECK_NUM_ARGS(2);
    PM_CHECK_FUNCTION( getPyPortPin(ppframe, &u16_port, &u16_pin) );
    GET_BOOL_ARG(1, &b_isHigh);

    // Call the function with these arguments
    PM_CHECK_FUNCTION( setDigitalPin(u16_port, u16_pin, b_isHigh) );

    // Return nothing
    NATIVE_SET_TOS(PM_NONE);
    return retval;
}

PmReturn_t
readDigitalPinPy(pPmFrame_t *ppframe)
{
    PmReturn_t retval = PM_RET_OK;
    uint16_t u16_port;
    uint16_t u16_pin;
    bool_t b_isHigh;

    // Get the arguments
    CHECK_NUM_ARGS(1);
    PM_CHECK_FUNCTION( getPyPortPin(ppframe, &u16_port, &u16_pin) );

    // Call the function with these arguments
    PM_CHECK_FUNCTION( readDigitalPin(u16_port, u16_pin, &b_isHigh) );

    // Return bool
    NATIVE_SET_TOS(b_isHigh ? PM_TRUE : PM_FALSE);
    return retval;
}
PmReturn_t
readDigitalLatchPy(pPmFrame_t *ppframe)
{
    PmReturn_t retval = PM_RET_OK;
    uint16_t u16_port;
    uint16_t u16_pin;
    bool_t b_isHigh;

    // Get the arguments
    CHECK_NUM_ARGS(1);
    PM_CHECK_FUNCTION( getPyPortPin(ppframe, &u16_port, &u16_pin) );

    // Call the function with these arguments
    PM_CHECK_FUNCTION( readDigitalLatch(u16_port, u16_pin, &b_isHigh) );

    // Return bool
    NATIVE_SET_TOS(b_isHigh ? PM_TRUE : PM_FALSE);
    return retval;
}
