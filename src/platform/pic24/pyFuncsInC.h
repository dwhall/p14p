/** \file
 *  \brief This file implements Python functions defined in main.py.
 * 
 *  \section pinConfiguration Pin configuration
 *  The PIC24 and dsPIC33 processors support a rich and varied I/O
 *  ability. Before configuring a pin, the I/O system must first be
 *  initialized by a call to \ref initIoConst. Next, each pin
 *  most be configured. The high-level functions 
 *  \ref configDigitalPinC, \ref configAnalogPinC simplify this
 *  process. \todo more here
 *
 *  \subsection manualPinConfiguration Manual pin configuration
 *  Manual I/O pin configuration requires making the following
 *  choices:
 *  - First, use \ref setPinIsInput to configure a pin as either
 *    an input or an output.
 *    - For inputs, use \ref setPinIsAnalogInput to configure the
 *      pin as an analog or digital input.
 *    - For outputs, use \ref setPinIsAnalogInput to configure the
 *      pin as a digital input. While digital output functions
 *      correctly when the pin is configured as both an analog
 *      input and a digitial output, doing so slows the rise and
 *      fall times of the output. This occurs because analog input
 *      use a capacitor to sample and hold the analog voltage, which
 *      loads the pin, slowing its edge rates.
 *    - In either case, note that the device reset state of a pin
 *      is with analog input capability enabled; when configuring
 *      a pin for digital I/O, this must be changed for the pin to
 *      operate correctly.
 *  - Second, use \ref setPinPullDirection to
 *    select either a weak pull-up, a weak pull-down, or no pull.
 *    Note that only the PIC24F family supports pull-downs (see the
 *    24F FRM section 12.6). In addition, the availablity of pull-ups
 *    and pull-downs varies based on the chip and pin; only pins with
 *    change notification capability support pull-ups or pull-downs.
 *    Pins with this ability are labeled CNx on the pinout for the chip.
 *    - For analog inputs, the manual does not specify the
 *      required pull; however, they should be configured with no
 *      pull.
 *    - For digital outputs, the manual states that no pull should be
 *      enabled (see e.g. the PIC24HJ32GP202 manual, section 10.3,
 *      grey box beginning with "Note:"). However, if the pin is
 *      configured as an open-drain output (see next item), this
 *      is expected and should be supported.
 *  - Third, for digital outputs only, select open-drain or standard
 *    operation using \ref setPinIsOpenDrain. Recall that open-drain 
 *    (also known as open-collector) operation
 *    configures the digital output to only drive the pin low but allow
 *    it to float when high; therefore, a pull-up must be connected either
 *    externally or internally. In contrast, standard drivers (also known
 *    as push/pull or totem-pole drivers) active drive the pin either
 *    high or low.
 *  - Finally, any remappable peripherals which take control of the
 *    I/O pin must be unmapped to make use of the pin. The
 *    \ref unmapPin function does this; it can still be called for
 *    chips which do not have remappable I/O. In this case, the
 *    function does nothing.
 *    
 */

#include "pm.h"

/** Initialize constants for this module. */
void initIoConst(void);

/** Set an I/O pin to be either an input or an output. Setting this
 *  pin as an output implies that it is a digital outp0ut. In contrast,
 *  configuring this pint to be an input allows it to be used as either
 *  a digital input or an analog input.
 *  \param u16_port I/O port (A = 0, B = 1, etc.)
 *  \param u16_pin  Pin on the I/O port (from 0 to 15)
 *  \param b_isInput True to select the pin as an input, false as an output.
 */
PmReturn_t setPinIsInput(uint16_t u16_port, uint16_t u16_pin, bool_t b_isInput);

/** Specify the direction (input or output) for an I/O pin.
 *  \param u16_port I/O port (A = 0, B = 1, etc.)
 *  \param u16_pin  Pin on the I/O port (from 0 to 15)
 *  \param b_isOpenDrain True to select an open-drain driver for the
 *                       pin, false to select a standard push-pull (totem-pole) driver.
 */
PmReturn_t setPinIsOpenDrain(uint16_t u16_port, uint16_t u16_pin, bool_t b_isOpenDrain);

/** Specify the pull direction (up, down, or none) for an I/O pin.
 *  \param u16_port I/O port (A = 0, B = 1, etc.)
 *  \param u16_pin  Pin on the I/O port (from 0 to 15)
 *  \param i16_dir  Pull direction: 0 = none, negative = pull down, 
 *                    positive = pull up.
 */
PmReturn_t setPinPullDirection(uint16_t u16_port, uint16_t u16_pin, 
  int16_t i16_dir);

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

/** Set a specific bit in an extended (> 16 bit) bitfield.
 *  @param pu16_bitfield Pointer to bitfield to modify.
 *  @param u16_bit Bit in bitfield to access.
 *  @param b_val True to set the bit, false to clear it.
 */
#define SET_EXTENDED_BIT(pu16_bitfield, u16_bit, b_val) \
    setBit(pu16_bitfield + (u16_bit >> 4), u16_bit & 0x000F, b_val)

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
