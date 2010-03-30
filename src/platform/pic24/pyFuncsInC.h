/** \file
 *  \brief This file implements Python functions defined in main.py.
 */

#include "pm.h"

/** Implements the Python \ref main::readBits function. See it for details. */
PmReturn_t readBitsC(pPmFrame_t *ppframe);

/** Look up a specific bit in a bitfield.
 *  @param u16_bitfield Bitfield to access.
 *  @param u_bit Bit in bitfield to access. Must be from
 *               0 to 15.
 *  @return C_TRUE if the bit is a 1, C_FALSE otherwise.
 */
uint_t getBit(uint16_t u16_bitfield, uint_t u_bit);

/** Set a specific bit in a bitfield.
 *  @param pu16_bitfield Pointer to bitfield to modify.
 *  @param u_bit Bit in bitfield to access. Must be from
 *               0 to 15.
 *  @param u_val True to set the bit, false to clear it.
 */
void setBit(volatile uint16_t* pu16_bitfield, uint_t u_bit, uint_t u_val);

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
