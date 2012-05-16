/*
# This file is Copyright 2007, 2009, 2010 Dean Hall.
#
# This file is part of the PyMite VM.
# The PyMite VM is free software: you can redistribute it and/or modify
# it under the terms of the GNU GENERAL PUBLIC LICENSE Version 2.
#
# The PyMite VM is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# A copy of the GNU GENERAL PUBLIC LICENSE Version 2
# is seen in the file COPYING in this directory.
*/


#ifndef __PLAT_H__
#define __PLAT_H__


/**
 * \file
 * \brief PyMite's Porting Interface
 */


/**
 * Initializes the platform as needed by the routines
 * in the platform implementation file.
 */
PmReturn_t plat_init(void);

/** De-initializes the platform after the VM is done running. */
PmReturn_t plat_deinit(void);

/**
 * Receives one byte from the default connection,
 * usually UART0 on a target device or stdio on the desktop
 */
PmReturn_t plat_getByte(uint8_t *b);


/**
 * Sends one byte out on the default connection,
 * usually UART0 on a target device or stdio on the desktop
 */
PmReturn_t plat_putByte(uint8_t b);


/**
 * Gets the number of timer ticks that have passed since system start.
 */
PmReturn_t plat_getMsTicks(uint32_t *r_ticks);


/**
 * Reports an exception or other error that caused the thread to quit
 */
void plat_reportError(PmReturn_t result);

/**
 * OPTIONAL: It is NOT necessary to implement this function on a platform.
 *           Implementing this function will allow the ability to load code
 *           from data storage areas including Flash, SEEPROM or a filesystem.
 * USAGE:    In plat_init(), register this function by calling::
 *           mod_setPlatLoadCodeObjectCallback(plat_loadCodeObject);
 * Finds a code object matching the given name.
 * The code object must be in addressable memory (RAM or Flash);
 * if it is not, the code object must be copied to RAM.
 * Used by mod_import() to import modules using a platform-specific method.
 * For more information on this function, see
 * docs/src/PlatformMarshalledModules.rst
 */
PmReturn_t plat_loadCodeObject(pPmObj_t pname, pPmObj_t *r_cob);
#endif /* __PLAT_H__ */
