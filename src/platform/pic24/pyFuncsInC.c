/** \file
 *  \brief This file implements Python functions defined in main.py.
 */

#include <pic24_all.h>
#include "pyToC.h"
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
    GET_UINT16(0, u16);
    pu16_evenAddress = (uint16_t*) u16;
    GET_UINT16(1, u16_startBit);
    GET_UINT16(2, u16_numBits);

    // Check their values.
    /// \todo Allow an odd addres for 8 bit values.
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

uint_t getBit(uint16_t u16_bitfield, uint_t u_bit)
{
    ASSERT(u_bit < 16);
    return (u16_bitfield & (1 << u_bit)) ? C_TRUE : C_FALSE;
}

void setBit(volatile uint16_t* pu16_bitfield, uint_t u_bit, uint_t u_val)
{
    ASSERT(u_bit < 16);
    if (u_val)
        *pu16_bitfield |= 1 << u_bit;
    else
        *pu16_bitfield &= ~(1 << u_bit);
}

/** Determine if the given digitial I/O port/pin exists.
 *  @param u_port Port, where 0 = A, 1 = B, etc.
 *  @param u_pin  Pin of the given port; from 0 to 15.
 *  @return Returns C_TRUE if the port/pin exists, C_FALSE otherwise.
 *          Nonexistant ports or pins simply return C_FALSE.
 */
static uint_t digitalPortPinExists(uint_t u_port, uint_t u_pin)
{
    // Check for an out-of-range port
    if (u_port > NUM_DIGITAL_PORTS)
        return C_FALSE;
    // Check for an out-of-range pin
    if (u_pin > 15)
        return C_FALSE;
    // Otherwise, check the map of pins.
    return getBit(u16_digitalPinPresent[u_port], u_pin);
}


/** Determine if the given digitial I/O open-drain port/pin exists.
 *  @param u_port Port, where 0 = A, 1 = B, etc.
 *  @param u_pin  Pin of the given port; from 0 to 15.
 *  @return Returns C_TRUE if the port/pin open-drain exists, C_FALSE otherwise.
 *          Nonexistant ports or pins simply return C_FALSE.
 */
static uint_t digitalPortPinOpenDrainExists(uint_t u_port, uint_t u_pin)
{
    // Check for an out-of-range port
    if (u_port > NUM_DIGITAL_PORTS)
        return C_FALSE;
    // Check for an out-of-range pin
    if (u_pin > 15)
        return C_FALSE;
    // Otherwise, check the map of pins.
    return getBit(u16_digitalPinOpenDrainPresent[u_port], u_pin);
}


PmReturn_t configDigitalPinC(pPmFrame_t *ppframe)
{
    PmReturn_t retval = PM_RET_OK;
    uint16_t u16_port;
    uint16_t u16_pin;
    uint_t u_isInput;
    uint_t u_isOpenDrain;
    uint16 u16_offset;
    int32_t i32_pullDir;

    // Get the arguments
    u16_port = 0;
    CHECK_NUM_ARGS(5);
    GET_UINT16(0, u16_port);
    GET_UINT16(1, u16_pin);
    GET_BOOL(2, u_isInput);
    GET_BOOL(3, u_isOpenDrain);
    GET_INT32(4, i32_pullDir);

    // 1. Check to see if the port/pin exists.
    EXCEPTION_UNLESS(digitalPortPinExists(u16_port, u16_pin), PM_RET_EX_VAL,
      "Invalid port or pin.");

    // 3. Select the pin to be either an input or an output.
    u16_offset = (uint16) (&TRISB - &TRISA);
    setBit((&TRISA) + u16_port*u16_offset, u16_pin, u_isInput);

    // 4. Check and configure open-drain for the pin.
    // First, see if the pin exists.
    if (digitalPortPinOpenDrainExists(u16_port, u16_pin)) {
        // Set the pin per the OD boolean.
        // PIC24F names this differently, so define around it.
        #if defined (_ODA0)  || defined (_ODA1)  || defined (_ODA2)  || defined (_ODA3) || \
            defined (_ODA4)  || defined (_ODA5)  || defined (_ODA6)  || defined (_ODA7) || \
            defined (_ODA8)  || defined (_ODA9)  || defined (_ODA10) || defined (_ODA11) || \
            defined (_ODA12) || defined (_ODA12) || defined (_ODA14) || defined (_ODA15)
        #define ODCA ODA
        #endif
        setBit((&ODCA) + u16_port*u16_offset, u16_pin, u_isOpenDrain);
    } else {
        // If open-drain is enabled on a pin without OD ability,
        // report an error. Otherwise, do nothing -- open-drain
        // is already disabled for a pin without OD ability.
        EXCEPTION_UNLESS(!u_isOpenDrain, PM_RET_EX_VAL,
          "The port/pin selected has no open-drain ability.");
    }

    return retval;
}
