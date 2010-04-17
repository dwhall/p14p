/** \file
 *  \brief This file implements Python functions defined in main.py.
 */

#include "pm.h"

/** Initialize constants for this module. */
void initDigitalIoConst(void);

/** Specify the direction (input or output) for an I/O pin.
 *  \param u16_port I/O port (A = 0, B = 1, etc.)
 *  \param u16_pin  Pin on the I/O port (from 0 to 15)
 *  \param b_isInput True to select the pin as an input, false as an output.
 */
PmReturn_t configPinDirection(uint16_t u16_port, uint16_t u16_pin, bool_t b_isInput);

/** Specify the direction (input or output) for an I/O pin.
 *  \param u16_port I/O port (A = 0, B = 1, etc.)
 *  \param u16_pin  Pin on the I/O port (from 0 to 15)
 *  \param b_isOpenDrain True to select an open-drain driver for the
 *                       pin, false to select a standard push-pull (totem-pole) driver.
 */
PmReturn_t configPinOpenDrain(uint16_t u16_port, uint16_t u16_pin, bool_t b_isOpenDrain);

/** Implements the Python \ref main::readBits function. See it for details. */
PmReturn_t readBitsC(pPmFrame_t *ppframe);

/** Look up a specific bit in a bitfield.
 *  @param u16_bitfield Bitfield to access.
 *  @param u16_bit Bit in bitfield to access. Must be from
 *               0 to 15.
 *  @return C_TRUE if the bit is a 1, C_FALSE otherwise.
 */
bool_t getBit(uint16_t u16_bitfield, uint16_t u16_bit);

/** Set a specific bit in a bitfield.
 *  @param pu16_bitfield Pointer to bitfield to modify.
 *  @param u16_bit Bit in bitfield to access. Must be from
 *               0 to 15.
 *  @param b_val True to set the bit, false to clear it.
 */
void setBit(volatile uint16_t* pu16_bitfield, uint16_t u16_bit, bool_t b_val);

/** Implements the Python \ref main::configDigitalPin function. 
 *  See it for details. Implementation:
 *  -# Check to see if the port/pin exists.
 *  -# If the pin has analog capability, turn it off.
 *  -# Select the pin to be either an input or an output.
 *  -# Check and configure open-drain for the pin.
 *  -# Check and configure pull-ups/pull-downs for the pin.
 *  \todo Need to also remove any peripheral outputs mapped to
 *  this pin if it's a remappable pin.
 */
PmReturn_t configDigitalPinC(pPmFrame_t *ppframe);
