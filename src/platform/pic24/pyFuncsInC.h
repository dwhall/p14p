/** \file
 *  \brief This file implements Python functions defined in main.py.
 * 
 */

#include "pm.h"

/** @name Utility functions
 */
//@{

/** Implements the Python \ref main::readBits function. See it for details. */
PmReturn_t readBitsC(pPmFrame_t *ppframe);

/** Look up a specific bit in a bitfield.
 *  @param u16_bitfield Bitfield to access.
 *  @param u16_bit Bit in bitfield to access. Must be from
 *               0 to 15.
 *  @return True if the bit is a 1, false otherwise.
 */
bool_t getBit(uint16_t u16_bitfield, uint16_t u16_bit);

/** Look up a specific bit in an extended (> 16 bit) bitfield.
 *  @param u16_bitfield Bitfield to access.
 *  @param u16_bit Bit in bitfield to access
 *  @return True if the bit is a 1, false otherwise.
 */
#define GET_EXTENDED_BIT(u_bitfield, u16_bit) \
    getBit(*(((uint16_t*) &(u_bitfield)) + ((u16_bit) >> 4)), (u16_bit) & 0x000F)

/** Set a specific bit in a bitfield.
 *  @param pu16_bitfield Pointer to bitfield to modify.
 *  @param u16_bit Bit in bitfield to access. Must be from
 *               0 to 15.
 *  @param b_val True to set the bit, false to clear it.
 */
void setBit(volatile uint16_t* pu16_bitfield, uint16_t u16_bit, bool_t b_val);

/** Set a specific bit in an extended (> 16 bit) bitfield.
 *  @param p_bitfield Pointer to bitfield to modify.
 *  @param u16_bit Bit in bitfield to access.
 *  @param b_val True to set the bit, false to clear it.
 */
#define SET_EXTENDED_BIT(p_bitfield, u16_bit, b_val) \
    setBit(((uint16_t*) (p_bitfield)) + ((u16_bit) >> 4), (u16_bit) & 0x000F, b_val)
//@}


/** @name Initialization, read/write, and high-level pin configuration functions
 *  These functions set up the I/O system, read and write to a pin, and
 *  allow configuring an I/O pin using a single function call.
 */
//@{
/** Calls \ref initIoConst from Python. */
PmReturn_t initIoConstPy(pPmFrame_t *ppframe);

/** Implements the Python \ref main.configDigitalPin function. 
 *  See it for details.
 */
PmReturn_t configDigitalPinPy(pPmFrame_t *ppframe);

/** Implements the Python \ref digitalIo.set function.
 *  The \ref setDigitalPin function does the work.
 */
PmReturn_t setDigitalPinPy(pPmFrame_t *ppframe);

/** Implements the Python \ref digitalIo.get function.
 *  The \ref readDigitalValue function does the work.
 */
PmReturn_t
readDigitalValuePy(pPmFrame_t *ppframe);

/** Implements the Python \ref digitalIo.getPin function.
 *  The \ref readDigitalPin function does the work.
 */
PmReturn_t readDigitalPinPy(pPmFrame_t *ppframe);

/** Implements the Python \ref digitalIo.getLatch function.
 *  The \ref readDigitalLatch function does the work.
 */
PmReturn_t readDigitalLatchPy(pPmFrame_t *ppframe);
//@}
