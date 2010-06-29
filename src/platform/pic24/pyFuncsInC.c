/** \file
 *  \brief This file implements Python functions defined in main.py.
 */

#include <pic24_all.h>
#include "pyToC.h"
#include "pyFuncsInC.h"
#include <stdio.h>

#undef __FILE_ID__
#define __FILE_ID__ 0x70

/// @name Helper functions
//@{
/** Store an int of private data to a Python class.
 *  @param ppframe Stack frame containing Python arguments.
 *                 Argument 0 should be the object.
 *  @param i32_val An int to store in the class.
 */
static PmReturn_t
putPyClassInt(pPmFrame_t *ppframe, int32_t i32_val)
{
    PmReturn_t retval = PM_RET_OK;

    pPmObj_t ppo_self;
    pPmObj_t ppo_attrs;
    pPmObj_t ppo_int;

    // Argument 0 is a pointer to the object.
    // Store port and pin in it.
    // Raise TypeError if address isn't an object
    ppo_self = NATIVE_GET_LOCAL(0);
    EXCEPTION_UNLESS(OBJ_GET_TYPE(ppo_self) == OBJ_TYPE_CLI, PM_RET_EX_TYPE, 
      "Argument 0 must be an class instance");
    ppo_attrs = (pPmObj_t)((pPmInstance_t)ppo_self)->cli_attrs;
    PM_CHECK_FUNCTION( int_new(i32_val, &ppo_int) );
    PM_CHECK_FUNCTION( dict_setItem(ppo_attrs, PM_NONE, ppo_int) );

    return retval;
}

/** Get an int of private data from a Python class.
 *  @param ppframe Stack frame containing Python arguments.
 *                 Argument 0 should be the object.
 *  @param pi32_val An int stored in the class is returned here.
 */
static PmReturn_t
getPyClassInt(pPmFrame_t *ppframe, int32_t* pi32_val)
{
    PmReturn_t retval = PM_RET_OK;
    pPmObj_t ppo_self;
    pPmObj_t ppo_attrs;
    pPmObj_t ppo_int;

    // Argument 0 is a pointer to the object.
    // Store port and pin in it.
    // Raise TypeError if address isn't an object
    ppo_self = NATIVE_GET_LOCAL(0);
    EXCEPTION_UNLESS(OBJ_GET_TYPE(ppo_self) == OBJ_TYPE_CLI, PM_RET_EX_TYPE, 
      "Argument 0 must be an class instance");
    ppo_attrs = (pPmObj_t) ((pPmInstance_t) ppo_self)->cli_attrs;
    PM_CHECK_FUNCTION( dict_getItem(ppo_attrs, PM_NONE, &ppo_int) );
    PM_CHECK_FUNCTION( getInt32(ppo_int, pi32_val) );

    return retval;
}
//@}

PmReturn_t
initIoConstPy(pPmFrame_t *ppframe)
{
    PmReturn_t retval = PM_RET_OK;

    initIoConst();
    CHECK_NUM_ARGS(0);
    NATIVE_SET_TOS(PM_NONE);

    return retval;
}

PmReturn_t
configDigitalPinPy(pPmFrame_t *ppframe)
{
    PmReturn_t retval = PM_RET_OK;
    uint16_t u16_port;
    uint16_t u16_pin;
    bool_t b_isInput;
    bool_t b_isOpenDrain;
    int16_t i16_pullDir;

    // Get the arguments
    CHECK_NUM_ARGS(6);
    GET_UINT16_ARG(1, &u16_port);
    GET_UINT16_ARG(2, &u16_pin);
    GET_BOOL_ARG(3, &b_isInput);
    GET_BOOL_ARG(4, &b_isOpenDrain);
    GET_INT16_ARG(5, &i16_pullDir);

    // Save the port and pin in theclass
    PM_CHECK_FUNCTION( putPyClassInt(ppframe, 
      (((int32_t) u16_port) << 16) | u16_pin) );

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
    int32_t i32_portPin;

    PM_CHECK_FUNCTION( getPyClassInt(ppframe, &i32_portPin) );
    *pu16_port = i32_portPin >> 16;
    *pu16_pin = i32_portPin & 0x00FF;

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
readDigitalValuePy(pPmFrame_t *ppframe)
{
    PmReturn_t retval = PM_RET_OK;
    uint16_t u16_port;
    uint16_t u16_pin;
    bool_t b_isHigh;
    bool_t b_isInput;

    // Get the arguments
    CHECK_NUM_ARGS(1);
    PM_CHECK_FUNCTION( getPyPortPin(ppframe, &u16_port, &u16_pin) );

    // Determine if this is an input or an output
    PM_CHECK_FUNCTION( getPinIsInput(u16_port, u16_pin, &b_isInput) );
    // Read the pin (if it's an input) or the port (if it's an output)
    if (b_isInput) {
        PM_CHECK_FUNCTION( readDigitalPin(u16_port, u16_pin, &b_isHigh) );
    } else {
        PM_CHECK_FUNCTION( readDigitalLatch(u16_port, u16_pin, &b_isHigh) );
    }

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

PmReturn_t
configAnalogPinPy(pPmFrame_t *ppframe)
{
    PmReturn_t retval = PM_RET_OK;
    uint16_t u16_analogPin;

    // Get the arguments
    CHECK_NUM_ARGS(2);
    GET_UINT16_ARG(1, &u16_analogPin);

    // Save the analog pin number in the class
    PM_CHECK_FUNCTION( putPyClassInt(ppframe, u16_analogPin) );

    PM_CHECK_FUNCTION( configAnalogPin(u16_analogPin) );
    NATIVE_SET_TOS(PM_NONE);

    return retval;
}

PmReturn_t
readAnalogValuePy(pPmFrame_t *ppframe)
{
    PmReturn_t retval = PM_RET_OK;
    int32_t i32_analogPin;
    uint16_t u16_analogPin;
    uint16_t u16_analogValue;

    // Get the arguments
    CHECK_NUM_ARGS(1);
    PM_CHECK_FUNCTION( getPyClassInt(ppframe, &i32_analogPin) );
    u16_analogPin = i32_analogPin;

    // Read analog value
    configADC1_ManualCH0(ADC_CH0_POS_SAMPLEA_AN0 + u16_analogPin, 31, C_TRUE);
    u16_analogValue = convertADC1();

    // Return value
//    NATIVE_SET_TOS(b_isHigh ? PM_TRUE : PM_FALSE);
    return retval;
}

