/** \file
 *  \brief This file implements Python functions defined in main.py.
 */

#include <pic24_all.h>
#include "pyToC.h"
#include "pyFuncsInC.h"
#include "outputcompare_multiservo.h"
#include "dataXfer.h"
#ifdef HAS_REMAPPABLE_PINS
#include <pps.h>
#endif
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
    EXCEPTION_UNLESS(NATIVE_GET_NUM_ARGS() > 1, PM_RET_EX_TYPE,
      "Expected at least 1 argument.");
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
    EXCEPTION_UNLESS(NATIVE_GET_NUM_ARGS() >= 1, PM_RET_EX_TYPE,
      "Expected at least 1 argument.");
    ppo_self = NATIVE_GET_LOCAL(0);
    EXCEPTION_UNLESS(OBJ_GET_TYPE(ppo_self) == OBJ_TYPE_CLI, PM_RET_EX_TYPE, 
      "Argument 0 must be an class instance");
    ppo_attrs = (pPmObj_t) ((pPmInstance_t) ppo_self)->cli_attrs;
    PM_CHECK_FUNCTION( dict_getItem(ppo_attrs, PM_NONE, &ppo_int) );
    PM_CHECK_FUNCTION( getInt32(ppo_int, pi32_val) );

    return retval;
}

/** Get a list of private data from a Python class.
 *  @param ppframe Stack frame containing Python arguments.
 *                 Argument 0 should be the object.
 *  @param pppo_list The list stored in the class is returned here.
 */
static PmReturn_t
getPyClassList(pPmFrame_t *ppframe, pPmObj_t* pppo_list)
{
    PmReturn_t retval = PM_RET_OK;
    pPmObj_t ppo_self;
    pPmObj_t ppo_attrs;

    // Argument 0 is a pointer to the object.
    // Store port and pin in it.
    // Raise TypeError if address isn't an object
    EXCEPTION_UNLESS(NATIVE_GET_NUM_ARGS() >= 1, PM_RET_EX_TYPE,
      "Expected at least 1 argument.");
    ppo_self = NATIVE_GET_LOCAL(0);
    EXCEPTION_UNLESS(OBJ_GET_TYPE(ppo_self) == OBJ_TYPE_CLI, PM_RET_EX_TYPE, 
      "Argument 0 must be an class instance");
    ppo_attrs = (pPmObj_t) ((pPmInstance_t) ppo_self)->cli_attrs;
    PM_CHECK_FUNCTION( dict_getItem(ppo_attrs, PM_NONE, pppo_list) );
    EXCEPTION_UNLESS(OBJ_GET_TYPE(*pppo_list) == OBJ_TYPE_LST, PM_RET_EX_TYPE,
      "Private class object not a list");

    return retval;
}
//@}

/** The offset in words between successive OC control registers. 
 *  For example, &OC1RS = 0x0180 and &OC2RS = 0x0186, a three
 *  byte difference. */
#define OC_CONTROL_OFFSET 3

PmReturn_t
configDigitalPinPy(pPmFrame_t *ppframe)
{
    PmReturn_t retval = PM_RET_OK;
    uint16_t u16_port;
    uint16_t u16_pin;
    bool_t b_isInput;
    bool_t b_isOpenDrain = C_FALSE;
    int16_t i16_pullDir = 0;

    // Get the arguments
    EXCEPTION_UNLESS(NATIVE_GET_NUM_ARGS() >= 4, PM_RET_EX_TYPE,
      "Expected at least 4 arguments, but received %u.",
      (uint16_t) NATIVE_GET_NUM_ARGS());
    EXCEPTION_UNLESS(NATIVE_GET_NUM_ARGS() <= 6, PM_RET_EX_TYPE,
      "Too many arguments");
    GET_UINT16_ARG(1, &u16_port);
    GET_UINT16_ARG(2, &u16_pin);
    GET_BOOL_ARG(3, &b_isInput);
    if (NATIVE_GET_NUM_ARGS() >= 5)
        GET_BOOL_ARG(4, &b_isOpenDrain);
    if (NATIVE_GET_NUM_ARGS() >= 6)
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

/** Read an analog code from the ADC and return it.
 *  @param ppframe Stack frame of Python arguments 
 *      passed to the function
 *  @param pu16_analogCode Code read from the ADC.
 */
static PmReturn_t
readAnalogCode(pPmFrame_t *ppframe, uint16_t* pu16_analogCode)
{
    PmReturn_t retval = PM_RET_OK;
    int32_t i32_analogPin;
    uint16_t u16_analogPin;

    // Get the arguments
    CHECK_NUM_ARGS(1);
    PM_CHECK_FUNCTION( getPyClassInt(ppframe, &i32_analogPin) );
    ASSERT( (i32_analogPin >= 0) && (i32_analogPin < 32) );
    u16_analogPin = i32_analogPin;

    // Read analog value
    configADC1_ManualCH0(ADC_CH0_POS_SAMPLEA_AN0 + u16_analogPin, 31, C_TRUE);
    *pu16_analogCode = convertADC1();

    return retval;
}

PmReturn_t
readAnalogCodePy(pPmFrame_t *ppframe)
{
    PmReturn_t retval = PM_RET_OK;
    uint16_t u16_analogCode;
    pPmObj_t ppo_analogCode;
        
    CHECK_NUM_ARGS(1);
    PM_CHECK_FUNCTION( readAnalogCode(ppframe, &u16_analogCode) );
    PM_CHECK_FUNCTION( int_new(u16_analogCode, &ppo_analogCode) );
    NATIVE_SET_TOS(ppo_analogCode);
    return retval;
}

PmReturn_t
readAnalogFloatPy(pPmFrame_t *ppframe, float f_scale)
{
    PmReturn_t retval = PM_RET_OK;
    uint16_t u16_analogCode;
    pPmObj_t ppo_analogCode;
        
    CHECK_NUM_ARGS(1);
    PM_CHECK_FUNCTION( readAnalogCode(ppframe, &u16_analogCode) );
    PM_CHECK_FUNCTION( float_new(f_scale*u16_analogCode, &ppo_analogCode) );
    NATIVE_SET_TOS(ppo_analogCode);
    return retval;
}

PmReturn_t
configPwmPy(pPmFrame_t *ppframe)
{
    PmReturn_t retval = PM_RET_OK;
    uint32_t u32_freq;
    bool_t b_isTimer2;
    uint16_t u16_oc;
    int16_t i16_ocPin;
    uint16_t u16_pr;

    // Get the arguments and error check them
    CHECK_NUM_ARGS(5);
    GET_UINT32_ARG(1, &u32_freq);
    GET_BOOL_ARG(2, &b_isTimer2);
    GET_UINT16_ARG(3, &u16_oc);
    GET_INT16_ARG(4, &i16_ocPin);

    PM_CHECK_FUNCTION( configPwm(u32_freq, b_isTimer2, u16_oc, i16_ocPin) );

    // Save the timer and OC number in the class
    u16_pr = b_isTimer2 ? PR2 : PR3;
    PM_CHECK_FUNCTION( putPyClassInt(ppframe, u16_oc | (((uint32_t) u16_pr) << 16)) );
    NATIVE_SET_TOS(PM_NONE);
    return retval;
}

/** A macro to access an Output Compare control register.
 *  @param u16_reg Control register to set
 *  @param u16_n   Offset to set: the n in OCnRS, for example.
 */
#define OC_REG(u16_reg, u16_n) \
  ((volatile uint16_t*) &u16_reg)[(u16_n - 1)*OC_CONTROL_OFFSET]

PmReturn_t
configPwm(uint32_t u32_freq, bool_t b_isTimer2, uint16_t u16_oc, 
  int16_t i16_ocPin)
{
    PmReturn_t retval = PM_RET_OK;
    uint32_t u32_counts;
    uint16_t u16_prescale;
    uint16_t u16_t2con;
    uint16_t u16_counts;

    EXCEPTION_UNLESS((u16_oc < NUM_OC_MODS) && (u16_oc > 0), PM_RET_EX_VAL,
      "Requested OC module %d does not exist", u16_oc);

    // Check and remap pins if possible
#ifdef HAS_REMAPPABLE_PINS
    EXCEPTION_UNLESS(i16_ocPin >= 0, PM_RET_EX_VAL,
      "Invalid pin RP%d.", i16_ocPin);
    EXCEPTION_UNLESS(digitalPinExists(PORT_B_INDEX + (i16_ocPin >> 4), 
      i16_ocPin & 0xF), PM_RET_EX_VAL,
      "Invalid pin RP%d.", i16_ocPin);
    // Make the selected pin an output.
    PM_CHECK_FUNCTION( 
      configDigitalPin(PORT_B_INDEX + (i16_ocPin >> 4), 
      i16_ocPin & 0xF, C_FALSE, C_FALSE, 0) );
    // Register RPOR0 has the _RP0R bitfield, with _RPnR bitfields
    // every 8 bits. So, cast this as a uint8_t to easily access
    // the nth _RPnR bitfield. Then assign it to the appropriate
    // OC peripheral.
    ((volatile uint8_t*) &RPOR0)[i16_ocPin] = OUT_FN_PPS_OC1 + u16_oc - 1;
#else
    EXCEPTION_UNLESS(i16_ocPin < 0, PM_RET_EX_VAL,
      "Remapping not possible on this device.");
#endif

    // Start with no PWM signal
    OC_REG(OC1RS, u16_oc) = 0;

    // Determine prescale and counts for timer
    EXCEPTION_UNLESS(u32_freq <= FCY, PM_RET_EX_VAL,
      "Frequency %ld too high", u32_freq);
    u32_counts = FCY/u32_freq;
    u16_prescale = u32_counts >> 16;
    EXCEPTION_UNLESS(u16_prescale <= 256, PM_RET_EX_VAL,
      "Frequency %ld too low", u32_freq);
    u16_t2con = 0;
    if (u16_prescale > 64)
    {
        u16_t2con = T2_PS_1_256;
        u16_counts = (u32_counts >> 8) - 1;
    } else if (u16_prescale > 8)
    {
        u16_t2con = T2_PS_1_64;
        u16_counts = (u32_counts >> 6) - 1;
    } else if (u16_prescale > 0)
    {
        u16_t2con = T2_PS_1_8;
        u16_counts = (u32_counts >> 3) - 1;
    } else {
        u16_t2con = T2_PS_1_1;
        u16_prescale = 0;
        u16_counts = u32_counts - 1;
    }

    // Configure timer with count and prescale
    if (b_isTimer2)
    {
        T2CON = T2_OFF | T2_IDLE_CON | T2_GATE_OFF
          | T2_32BIT_MODE_OFF
          | T2_SOURCE_INT
          | u16_t2con;
        TMR2 = 0;
        PR2 = u16_counts;
        OC_REG(OC1CON, u16_oc) = OC_TIMER2_SRC | OC_PWM_FAULT_PIN_DISABLE;
        T2CONbits.TON = 1;
    } else {
        T3CON = T3_OFF | T3_IDLE_CON | T3_GATE_OFF
          | T3_SOURCE_INT
          | u16_t2con;
        PR3 = 0;
        PR3 = u16_counts;
        OC_REG(OC1CON, u16_oc) = OC_TIMER3_SRC | OC_PWM_FAULT_PIN_DISABLE;
        T3CONbits.TON = 1;
    }

    NATIVE_SET_TOS(PM_NONE);
    return retval;
}

/** Get the PR2/3 value and the OC peripheral in use from the
 *  Python PWM object.
 *  @param ppframe Stack frame containing Python arguments.
 *                 Argument 0 should be the object.
 *  @param pu16_oc Output compare module to use.
 *  @param pu16_prn The value of the PR2/3 register. The register
 *      passed is based on which timer (Timer 2 or 3) the OC
 *      module was configured to use.
 */
static PmReturn_t
getPyOcPrn(pPmFrame_t *ppframe, uint16_t* pu16_oc, uint16_t* pu16_prn)
{
    PmReturn_t retval = PM_RET_OK;
    int32_t i32_prnOc;

    PM_CHECK_FUNCTION( getPyClassInt(ppframe, &i32_prnOc) );
    *pu16_prn = i32_prnOc >> 16;
    *pu16_oc = i32_prnOc & 0x00FF;

    return retval;
}

PmReturn_t
setPwmTimePy(pPmFrame_t *ppframe)
{
    PmReturn_t retval = PM_RET_OK;
    uint16_t u16_timeUs;
    uint16_t u16_oc;
    uint16_t u16_prn;

    CHECK_NUM_ARGS(2);
    GET_UINT16_ARG(1, &u16_timeUs);
    PM_CHECK_FUNCTION(getPyOcPrn(ppframe, &u16_oc, &u16_prn) );
    PM_CHECK_FUNCTION(setPwmTime(u16_timeUs, u16_oc, u16_prn) );

    NATIVE_SET_TOS(PM_NONE);
    return retval;
}

PmReturn_t
setPwmTime(uint16_t u16_timeUs, uint16_t u16_oc, uint16_t u16_prn)
{
    PmReturn_t retval = PM_RET_OK;
    uint16_t u16_counts;
    uint16_t u16_pre;

    ASSERT(u16_oc <= NUM_OC_MODS);
    // Determine which timer is in use in order to convert time to ticks
    u16_pre = (u16_prn == PR2) ? getTimerPrescale(T2CONbits) : 
      getTimerPrescale(T3CONbits);
    u16_counts = usToU16Ticks(u16_timeUs, u16_pre);
    // Make sure this doesn't exceed the PR value.
    EXCEPTION_UNLESS(u16_counts <= u16_prn, PM_RET_EX_VAL, "Time exceeds PRx time.");
    // Update PWM
    OC_REG(OC1RS, u16_oc) = u16_counts;

    return retval;
}

PmReturn_t
setPwmRatioPy(pPmFrame_t *ppframe)
{
    PmReturn_t retval = PM_RET_OK;
    float f_ratio;
    uint16_t u16_oc;
    uint16_t u16_prn;

    CHECK_NUM_ARGS(2);
    GET_FLOAT_ARG(1, &f_ratio);
    PM_CHECK_FUNCTION(getPyOcPrn(ppframe, &u16_oc, &u16_prn) );
    EXCEPTION_UNLESS( (f_ratio <= 1.0) && (f_ratio >= 0.0), PM_RET_EX_VAL,
      "Invalid ratio.");
    // Update PWM
    OC_REG(OC1RS, u16_oc) = (((uint32_t) u16_prn) + 1)*f_ratio;

    NATIVE_SET_TOS(PM_NONE);
    return retval;
}

PmReturn_t
configMultiServoPy(pPmFrame_t *ppframe)
{
    PmReturn_t retval = PM_RET_OK;
    bool_t b_isTimer2;
    uint16_t u16_oc;

    // Get the arguments and error check them
    CHECK_NUM_ARGS(3);
    GET_BOOL_ARG(1, &b_isTimer2);
    GET_UINT16_ARG(2, &u16_oc);
    PM_CHECK_FUNCTION(initMultiServo() );

    NATIVE_SET_TOS(PM_NONE);
    return retval;
}

PmReturn_t
setServoPulseWidthPy(pPmFrame_t *ppframe)
{
    PmReturn_t retval = PM_RET_OK;
    uint16_t u16_servo;
    uint16_t u16_pwUs;

    // Get the arguments and error check them
    CHECK_NUM_ARGS(3);
    GET_UINT16_ARG(1, &u16_servo);
    GET_UINT16_ARG(2, &u16_pwUs);
    PM_CHECK_FUNCTION(setServoPulseWidth(u16_servo, u16_pwUs) );

    NATIVE_SET_TOS(PM_NONE);
    return retval;
}

/*  Data transfer implementation notes:
 *  - Use a single, 4-byte buffer to hold send and receive data.
 *    Eventually, modify the code so it doesn't need a pointer to
 *    the data to send/receive. Sends should accept a pointer to the
 *    data to send; receives should make use of a single receive buffer.
 *  - On a receive, wrap the value into a Python int then place it at
 *    the appropriate index in the list.
 *  - Provide a non-blocking receiveAll function to receive everything
 *    until the receive buffer is empty. Provide a blocking version too.  
 */

/** A buffer for all reads and writes between Python and the dataXfer
 *  code. */
static int32_t i32_xferReceiveInt;

PmReturn_t
initDataXferPy(pPmFrame_t *ppframe)
{
    PmReturn_t retval = PM_RET_OK;
    pPmObj_t ppo_self;
    pPmObj_t ppo_attrs;
    pPmObj_t ppo_list;
    uint16_t u16_index;

    // Make sure we were passed self
    EXCEPTION_UNLESS(NATIVE_GET_NUM_ARGS() >= 1, PM_RET_EX_TYPE,
      "Expected at least 1 argument, but received %u.",
      (uint16_t) NATIVE_GET_NUM_ARGS());
    EXCEPTION_UNLESS(NATIVE_GET_NUM_ARGS() <= 2, PM_RET_EX_TYPE,
      "Expected at most 2 arguments, but received %u.",
      (uint16_t) NATIVE_GET_NUM_ARGS());

    // Argument 0 is a pointer to the object.
    // Raise TypeError if address isn't an object
    ppo_self = NATIVE_GET_LOCAL(0);
    EXCEPTION_UNLESS(OBJ_GET_TYPE(ppo_self) == OBJ_TYPE_CLI, PM_RET_EX_TYPE, 
      "Argument 0 must be an class instance");
    
    // Create a list of size NUM_XFER_VARS for the class or use the given list
    // Argument 1 (optional) is a list
    if (NATIVE_GET_NUM_ARGS() >= 2)
    {
        ppo_list = NATIVE_GET_LOCAL(1);
        EXCEPTION_UNLESS(OBJ_GET_TYPE(ppo_list) == OBJ_TYPE_LST, PM_RET_EX_TYPE, 
          "Argument 1 must be a list");
    } else {
        PM_CHECK_FUNCTION( list_new(&ppo_list) );
    }

    // Save the list in the object's private data
    ppo_attrs = (pPmObj_t)((pPmInstance_t)ppo_self)->cli_attrs;
    PM_CHECK_FUNCTION( dict_setItem(ppo_attrs, PM_NONE, ppo_list) );

    // Fill the list with PM_NONEs until it's at least NUM_XFER_VARS in size
    for (u16_index = ((pPmList_t) ppo_list)->length; 
      u16_index < NUM_XFER_VARS; u16_index++)
    {
        PM_CHECK_FUNCTION( list_append(ppo_list, PM_NONE) );
    }

    // Configure all vars to receive data into the i32_xferReceiveInt
    // buffer.
    initDataXfer();
    for (u16_index = 0; u16_index < NUM_XFER_VARS; u16_index++)
    {
        specifyVar(u16_index, &i32_xferReceiveInt, 
          sizeof(i32_xferReceiveInt), C_TRUE, "%d", "", "");
    }

    // Return nothing
    NATIVE_SET_TOS(PM_NONE);
    return retval;
}

PmReturn_t
receiveDataXferPy(pPmFrame_t *ppframe)
{
    PmReturn_t retval = PM_RET_OK;
    bool_t b_isBlocking = C_TRUE;
    RECEIVE_ERROR re;
    uint8_t au8_c[2];
    uint16_t u16_index;
    pPmObj_t ppo_list;
    pPmObj_t ppo_int;
    pPmObj_t ppo_str;

    // Extract and validate args
    EXCEPTION_UNLESS(NATIVE_GET_NUM_ARGS() <= 2, PM_RET_EX_TYPE,
      "Too many arguments");
    PM_CHECK_FUNCTION( getPyClassList(ppframe, &ppo_list) );
    if (NATIVE_GET_NUM_ARGS() >= 2)
    {
        GET_BOOL_ARG(1, &b_isBlocking);
    }

    // Feed state machine if characters are ready or if we're in the middle
    // of receiving data.
    while (isCharReady1() || b_isBlocking)
    {
        // Receive a char
        PM_CHECK_FUNCTION( plat_getByte(au8_c) );
        // Step state machine, no timeout
        // TODO: Add a timeout
        re = stepReceiveMachine(au8_c[0]);
        // Transform any errors to exceptions
        EXCEPTION_UNLESS(re == ERR_NONE, PM_RET_EX_VAL,
          "Data transfer error: %s", getReceiveErrorString());
        // Process any data received
        if (isReceiveMachineData())
        {
            // Wrap the value in an int
            PM_CHECK_FUNCTION( int_new(i32_xferReceiveInt, &ppo_int) );
            // Place it in the list
            u16_index = getReceiveMachineIndex();
            PM_CHECK_FUNCTION( list_setItem(ppo_list, u16_index, ppo_int) );
            // Return it
            PM_CHECK_FUNCTION( int_new(u16_index, &ppo_int) );
            NATIVE_SET_TOS(ppo_int);
            return retval;
        }
        // Process any characters received
        if (isReceiveMachineChar())
        {
            // Convert the char to a string
            au8_c[1] = 0;
            PM_CHECK_FUNCTION( string_new(au8_c, &ppo_str) );
            NATIVE_SET_TOS(ppo_str);
            return retval;
        }
    }

    // Nothing to return -- no data or characters received.
    NATIVE_SET_TOS(PM_NONE);
    return retval;
}

PmReturn_t
readDataXferPy(pPmFrame_t *ppframe)
{
    PmReturn_t retval = PM_RET_OK;
    pPmObj_t ppo_list;
    pPmObj_t ppo_listItem;
    uint16_t u16_index;

    // Make sure we were passed self and index
    CHECK_NUM_ARGS(2);

    // Extract and validate args
    PM_CHECK_FUNCTION( getPyClassList(ppframe, &ppo_list) );
    GET_UINT16_ARG(1, &u16_index);
    EXCEPTION_UNLESS(u16_index < NUM_XFER_VARS, PM_RET_EX_VAL,
      "Index exceeds list size");

    // Return requested list item
    PM_CHECK_FUNCTION( list_getItem(ppo_list, u16_index, &ppo_listItem) );
    NATIVE_SET_TOS(ppo_listItem);
    return retval;
}

PmReturn_t
writeDataXferPy(pPmFrame_t *ppframe)
{
    PmReturn_t retval = PM_RET_OK;
    uint16_t u16_index;

    // Make sure we were passed self, index, val
    CHECK_NUM_ARGS(3);

    // Extract and validate args
    GET_UINT16_ARG(1, &u16_index);
    EXCEPTION_UNLESS(u16_index < NUM_XFER_VARS, PM_RET_EX_VAL,
      "Index exceeds list size");
    GET_INT32_ARG(2, &i32_xferReceiveInt);

    // Send the data
    sendVar(u16_index);

    // Nothing to return
    NATIVE_SET_TOS(PM_NONE);
    return retval;
}
