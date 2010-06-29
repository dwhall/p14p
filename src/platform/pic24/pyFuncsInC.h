/** \file
 *  \brief This file implements Python functions defined in main.py.
 * 
 */

#include "pm.h"


/** @name Initialization, read/write, and high-level pin configuration functions
 *  These functions set up the I/O system, read and write to a pin, and
 *  allow configuring an I/O pin using a single function call. They wrap a
 *  series of C functions from pic24_pyports.h into the
 *  pic24_dspic33::digital_io Python class. 
 */
//@{
/** Calls \ref initIoConst from Python. */
PmReturn_t
initIoConstPy(pPmFrame_t *ppframe);

/** Implements the Python \ref pic24_dspic33::digital_io::__init__ function. 
 *  The \ref configDigitalPin function does the work.
 */
PmReturn_t
configDigitalPinPy(pPmFrame_t *ppframe);

/** Implements the Python \ref pic24_dspic33::analog_input::__init__ function. 
 *  The \ref configAnalogPin function does the work.
 */
PmReturn_t
configAnalogPinPy(pPmFrame_t *ppframe);

/** Implements the Python \ref pic24_dspic33::digital_io::set function.
 *  The \ref setDigitalPin function does the work.
 */
PmReturn_t
setDigitalPinPy(pPmFrame_t *ppframe);

/** Implements the Python \ref pic24_dspic33::digital_io::get function.
 */
PmReturn_t
readDigitalValuePy(pPmFrame_t *ppframe);

/** Implements the Python \ref pic24_dspic33::digital_io::getPin function.
 *  The \ref readDigitalPin function does the work.
 */
PmReturn_t readDigitalPinPy(pPmFrame_t *ppframe);

/** Implements the Python \ref pic24_dspic33::digital_io::getLatch function.
 *  The \ref readDigitalLatch function does the work.
 */
PmReturn_t readDigitalLatchPy(pPmFrame_t *ppframe);
//@}
