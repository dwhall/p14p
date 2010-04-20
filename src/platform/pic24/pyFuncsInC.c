/** \file
 *  \brief This file implements Python functions defined in main.py.
 */

#include <pic24_all.h>
#include "pyToC.h"
#include "pyFuncsInC.h"
#include <stdio.h>

#undef __FILE_ID__
#define __FILE_ID__ 0x70

PmReturn_t readBitsC(pPmFrame_t *ppframe)
{
    PmReturn_t retval = PM_RET_OK;
    pPmObj_t ppo;
    uint16_t u16;
    uint16_t* pu16_evenAddress;
    uint16_t u16_startBit;
    uint16_t u16_numBits;
    uint16_t u16_bitmask;
    uint16_t u16_value;

    // Get the arguments
    CHECK_NUM_ARGS(3);
    GET_UINT16(0, &u16);
    pu16_evenAddress = (uint16_t*) u16;
    GET_UINT16(1, &u16_startBit);
    GET_UINT16(2, &u16_numBits);

    // Check their values.
    /// \todo Allow an odd address for 8 bit values.
    EXCEPTION_UNLESS( !(((uint16_t) pu16_evenAddress) & 1), PM_RET_EX_VAL,
      "The address must be even.");
    EXCEPTION_UNLESS(u16_startBit <= 15, PM_RET_EX_VAL,
      "The start bit must be <= 15.");
    EXCEPTION_UNLESS(u16_numBits > 0, PM_RET_EX_VAL,
      "The number of bits must be > 0.");
    EXCEPTION_UNLESS(u16_startBit + u16_numBits <= 16, PM_RET_EX_VAL,
      "start bit + num bits <= 16.");

    // Form the bitmask
    u16_bitmask = (1 << u16_numBits) - 1;
    // Read the port and mask
    u16_value = (*pu16_evenAddress >> u16_numBits) & u16_bitmask;
    printf("Value at 0x%04X, bit(s) %d to %d = 0x%02X.\n", (uint16_t) pu16_evenAddress, 
      u16_startBit, u16_startBit + u16_numBits - 1, u16_value);

    retval = int_new(u16_value, &ppo);
    PM_RETURN_IF_ERROR(retval);
    NATIVE_SET_TOS(ppo);

    return retval;
}


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


/** Make sure the given pin is within bounds.
 *  @param u16_port Port, where 0 = A, 1 = B, etc.
 *  @param u16_pin  Pin of the given port; from 0 to 15.
 *  @return Returns C_FALSE if the port exceeds \ref NUM_DIGITAL_PORTS
 *          or if the pin > 15. Returns C_TRUE otherwise.
 */
inline static bool_t digitalPinInBounds(uint16_t u16_port, uint16_t u16_pin)
{
    // Check for an out-of-range port
    if (u16_port > NUM_DIGITAL_PORTS)
        return C_FALSE;
    // Check for an out-of-range pin
    if (u16_pin > 15)
        return C_FALSE;
    else
        return C_TRUE;
}

/** Determine if the given digitial I/O pin exists.
 *  @param u16_port Port, where 0 = A, 1 = B, etc.
 *  @param u16_pin  Pin of the given port; from 0 to 15.
 *  @return Returns C_TRUE if the pin exists, C_FALSE otherwise.
 *          Nonexistant ports or pins simply return C_FALSE.
 */
static bool_t digitalPinExists(uint16_t u16_port, uint16_t u16_pin)
{
    // Check for an out-of-range port
    if (!digitalPinInBounds(u16_port, u16_pin))
        return C_FALSE;
    // Check the map of pins.
    return getBit(u16_digitalPinPresent[u16_port], u16_pin);
}


/** Determine if the given digitial I/O open-drain pin exists.
 *  @param u16_port Port, where 0 = A, 1 = B, etc.
 *  @param u16_pin  Pin of the given port; from 0 to 15.
 *  @return Returns C_TRUE if the pin open-drain exists, C_FALSE otherwise.
 *          Nonexistant ports or pins simply return C_FALSE.
 */
static bool_t digitalOpenDrainPinExists(uint16_t u16_port, uint16_t u16_pin)
{
    // Check for an out-of-range port
    if (!digitalPinInBounds(u16_port, u16_pin))
        return C_FALSE;
    // Check the map of pins.
    return getBit(u16_digitalPinOpenDrainPresent[u16_port], u16_pin);
}


/** This values gives the number of words between I/O port control registers. */
// I can't make this a const, since only the linker knows these values.
static uint16_t u16_ioPortControlOffset;


void initIoConst(void) {
    u16_ioPortControlOffset = (uint16_t) (&TRISB - &TRISA);
}


PmReturn_t setPinIsInput(uint16_t u16_port, uint16_t u16_pin, bool_t b_isInput)
{
    PmReturn_t retval = PM_RET_OK;

    EXCEPTION_UNLESS(digitalPinExists(u16_port, u16_pin), PM_RET_EX_VAL,
      "Invalid pin %c%d.", (char) (u16_port + 'A'), u16_pin);
    // Make sure u16_ioPortControlOffset was initialized.
    ASSERT(u16_ioPortControlOffset);
    // Select input or output for the pin
    setBit((&TRISA) + u16_port*u16_ioPortControlOffset, u16_pin, b_isInput);
    return retval;
}


PmReturn_t setPinIsDigital(uint16_t u16_port, uint16_t u16_pin, 
  bool_t b_isDigital)
{
    PmReturn_t retval = PM_RET_OK;
    uint8_t u8_anPin;

    EXCEPTION_UNLESS(digitalPinExists(u16_port, u16_pin), PM_RET_EX_VAL,
      "Invalid pin %c%d.", (char) (u16_port + 'A'), u16_pin);

    // There are four possibilities for digital configuration:
    //                      | set as analog   | set as digital
    // ---------------------+-----------------+-----------------------------------------
    // has analog           | clear PCFG bit  | set PCFG bit
    // does not have analog | throw exception | do nothing (already digital)
    u8_anPin = anCnMap[u16_port*16 + u16_pin].u8_cnPin;
    if (u8_anPin != UNDEF_AN_PIN) {
        // Enable/disable analog input mode on this pin.
        // Each ADC handles 32 channels; some PIC24F / dsPIC33 parts have
        // two converters. If so, need to clear the corresponding bit on both.
        SET_EXTENDED_BIT(&AD1PCFGL, u8_anPin, b_isDigital);
#ifdef _AD2IF
        SET_EXTENDED_BIT(&AD2PCFGL, u8_anPin, b_isDigital);
#endif
    } else {
        // If analog is enabled on a pin without analog ability,
        // report an error. Otherwise, do nothing -- digital
        // is the default for a pin without analog ability.
        EXCEPTION_UNLESS(b_isDigital, PM_RET_EX_VAL,
          "Pin %c%d does not support analog functionality.", 
          (char) (u16_port + 'A'), u16_pin);
    }

    return retval;
}


PmReturn_t setPinIsOpenDrain(uint16_t u16_port, uint16_t u16_pin, bool_t b_isOpenDrain)
{
    PmReturn_t retval = PM_RET_OK;

    EXCEPTION_UNLESS(digitalPinExists(u16_port, u16_pin), PM_RET_EX_VAL,
      "Invalid pin %c%d.", (char) (u16_port + 'A'), u16_pin);
    // Make sure u16_ioPortControlOffset was initialized.
    ASSERT(u16_ioPortControlOffset);
    // There are four possibilities for open-drain configuration:
    //                          | set as open-drain  | set as normal (push/pull aka totem-pole)
    // -------------------------+--------------------+-----------------------------------------
    // has open-drain           | set OD bit         | clear OD bit
    // does not have open-drain | throw exception    | do nothing (already normal)
    if (digitalOpenDrainPinExists(u16_port, u16_pin)) {
        // Set the pin per the OD boolean.
        // PIC24F names this differently, so define around it.
        #if defined (_ODA0)  || defined (_ODA1)  || defined (_ODA2)  || defined (_ODA3)  || \
            defined (_ODA4)  || defined (_ODA5)  || defined (_ODA6)  || defined (_ODA7)  || \
            defined (_ODA8)  || defined (_ODA9)  || defined (_ODA10) || defined (_ODA11) || \
            defined (_ODA12) || defined (_ODA12) || defined (_ODA14) || defined (_ODA15)
        #define ODCA ODA
        #endif
        setBit((&ODCA) + u16_port*u16_ioPortControlOffset, u16_pin, b_isOpenDrain);
    } else {
        // If open-drain is enabled on a pin without OD ability,
        // report an error. Otherwise, do nothing -- open-drain
        // is already disabled for a pin without OD ability.
        EXCEPTION_UNLESS(!b_isOpenDrain, PM_RET_EX_VAL,
          "The pin %c%d has no open-drain ability.", (char) (u16_port + 'A'), u16_pin);
    }

    return retval;
}

PmReturn_t setPinPullDirection(uint16_t u16_port, uint16_t u16_pin, 
  int16_t i16_dir)
{
    PmReturn_t retval = PM_RET_OK;
    uint8_t u8_cnPin;

    EXCEPTION_UNLESS(digitalPinExists(u16_port, u16_pin), PM_RET_EX_VAL,
      "Invalid pin %c%d.", (char) (u16_port + 'A'), u16_pin);
    // Make sure u16_ioPortControlOffset was initialized.
    ASSERT(u16_ioPortControlOffset);

    // Detetrmine which (if any) CN bit exists on the given pin
    u8_cnPin = anCnMap[u16_port*16 + u16_pin].u8_cnPin;

    // For no pull, disable pull-ups and pull-downs if they exist
    if (i16_dir == 0) {
        if (u8_cnPin != UNDEF_CN_PIN) {
            SET_EXTENDED_BIT(&CNPU1, u8_cnPin, C_FALSE);
            #ifdef HAS_PULL_DOWNS
                SET_EXTENDED_BIT(&CNPD1, u8_cnPin, C_FALSE);
            #endif
        }
    // For pull-ups, disable pull-downs if they exist. Throw
    // an exception if pull-ups don't exist
    } else if (i16_dir > 0) {
        EXCEPTION_UNLESS(u8_cnPin != UNDEF_CN_PIN, PM_RET_EX_VAL,
          "Pull-ups do not exist on %c%d.", 
          (char) (u16_port + 'A'), u16_pin);
        SET_EXTENDED_BIT(&CNPU1, u8_cnPin, C_TRUE);
        #ifdef HAS_PULL_DOWNS
            SET_EXTENDED_BIT(&CNPD1, u8_cnPin, C_FALSE);
        #endif
    // For pull-downs, disable pull-ups. Throw an exception
    // if either don't exist.
    } else {
        ASSERT(i16_dir < 0);

        // Verify pull-downs exist then enable them
        #ifdef HAS_PULL_DOWNS
            SET_EXTENDED_BIT(&CNPD1, u8_cnPin, C_TRUE);
        #else
            EXCEPTION_UNLESS(0, PM_RET_EX_VAL,
              "Pull-downs do not exist on this chip.");
        #endif

        // Verify pull-ups exist then disable them
        EXCEPTION_UNLESS(u8_cnPin != UNDEF_CN_PIN, PM_RET_EX_VAL,
          "Pull-ups do not exist on %c%d.", 
          (char) (u16_port + 'A'), u16_pin);
        SET_EXTENDED_BIT(&CNPU1, u8_cnPin, C_FALSE);
    }

    return retval;
}

PmReturn_t configDigitalPinC(pPmFrame_t *ppframe)
{
    PmReturn_t retval = PM_RET_OK;
    uint16_t u16_port;
    uint16_t u16_pin;
    bool_t b_isInput;
    bool_t b_isOpenDrain;
    int16_t i16_pullDir;

    // Get the arguments
    CHECK_NUM_ARGS(5);
    GET_UINT16(0, &u16_port);
    GET_UINT16(1, &u16_pin);
    GET_BOOL(2, &b_isInput);
    GET_BOOL(3, &b_isOpenDrain);
    GET_INT16(4, &i16_pullDir);

    PM_CHECK_FUNCTION( setPinIsDigital(u16_port, u16_pin, C_TRUE) );
    PM_CHECK_FUNCTION( setPinIsInput(u16_port, u16_pin, b_isInput) );
    PM_CHECK_FUNCTION( setPinIsOpenDrain(u16_port, u16_pin, b_isOpenDrain) );
    PM_CHECK_FUNCTION( setPinPullDirection(u16_port, u16_pin, i16_pullDir) );
//    PM_CHECK_FUNCTION( unmapPin(u16_port, u16_pin) );
    return retval;
}
