/*
 * PyMite - A flyweight Python interpreter for 8-bit microcontrollers and more.
 * Copyright 2006 Dean Hall
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

/**
 * PyMite platform-specific routines for Desktop target
 *
 * Log
 * ---
 *
 * 2007/01/10   #75: Added time tick service for desktop (POSIX) and AVR.
 * 2006/12/26   #65: Create plat module with put and get routines
 */

#ifndef PLAT_H_
#define PLAT_H_

#if defined(TARGET_AVR)
#include "avr.h"
#include "util/crc16.h"
#elif defined(TARGET_DESKTOP)
#include "desktop.h"
#else
#endif

/***************************************************************
 * Prototypes
 **************************************************************/

/**
 * Initializes the platform as needed by the routines
 * in the platform implementation file.
 */
PmReturn_t plat_init(void);


/**
 * Return the byte at the given address in memspace.
 *
 * Increment the address (just like getc and read(1))
 * to make image loading work (recursive).
 *
 * PORT:    fill in getByte for each memspace in the system;
 *          call sys_error for invalid memspaces.
 *
 * @param   memspace memory space/type
 * @param   paddr ptr to address
 * @return  byte from memory.
 *          paddr - points to the next byte
 */
uint8_t plat_memGetByte(PmMemSpace_t memspace, uint8_t const **paddr);

#ifdef HAVE_RPP
/**
 * Write to the given address in memspace.
 *
 * PORT:    fill in setByte for each memspace in the system;
 *          call sys_error for invalid memspaces.
 *
 * @param   memspace memory space/type
 * @param   paddr ptr to address
 * @param   data the byte to write
 * @return  byte from memory.
 */
PmReturn_t plat_memSetByte(PmMemSpace_t memspace, uint8_t *paddr, uint8_t data);

/**
 * Get information about a specific memory space.
 *
 * PORT:    Fill in data for your memory spaces.
 *
 * @param   memspace memory space/type
 * @param   psize ptr the size is written to
 * @param   pwriteabe ptr to the variable that will carry 0 on r/o, 1 on r/w
 */
PmReturn_t plat_memGetInfo(PmMemSpace_t memspace, uint16_t *psize,
    uint8_t *pwriteable);

/**
 * Send the name of a memory space via RPP's buffered sending.
 *
 * PORT:    Fill in the name for your memory spaces.
 *
 * @param   memspace memory space/type
 */
PmReturn_t plat_memReportName(PmMemSpace_t memspace);
#endif

/**
 * Receives one byte from the default connection if one is available,
 * usually UART0 on a target device or stdio on the desktop.
 * If a byte is received and written to b, it returns PM_RET_OK.
 * This function does not block if no data is available. Instead it does not
 * alter b in any way and returns PM_RET_NO.
 *
 * This function is not available in every target. PLAT_HAVE_POLL_BYTE is only
 * defined if it is available - which is on desktop only at the moment. Other
 * targets feature an empty function that always return PM_RET_NO.
 *
 * If an error occurs, PM_RET_ERR ist returned.
 *
 * Note that the complementary solution to polling is an edge-triggered receive
 * interrupt.
 */
PmReturn_t plat_pollByte(uint8_t *b);

/**
 * Receives one byte from the default connection,
 * usually UART0 on a target device or stdio on the desktop
 */
PmReturn_t plat_getByte(uint8_t *b);

/**
 * Sends one byte out on the default connection,
 * usually UART0 on a target device or stdio on the desktop.
 *
 * If RPM/RPP is used, this should also update the CRC by calling
 * rpp_updateSendCrc().
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
 * If neccessary on a platform, enters a critical section by disabling
 * interrupts. Must not be nested e.g. on each plat_enterCriticalSection a
 * call to plat_exitCriticalSection must be called and not another
 * plat_enterCriticalSection. Ignored on other targets (notably Desktop).
 */
void plat_enterCriticalSection(void);

/**
 * If neccessary on a platform, exits a critical section by reenabling
 * interrupts. Ignored on other targets (notably Desktop).
 */
void plat_exitCriticalSection(void);

#if defined(HAVE_RPP)
#if defined(TARGET_AVR) 
/**
 * avr-libc's implementation is used.
 **/
#define plat_updateCrc16(pCrc, data) *pCrc = _crc16_update(*pCrc, data)

#else

/**     
 * Update a CRC16 polynomial with data. Call with the old CRC value (use
 * 0xFFFF the first time.
 */
void plat_updateCrc16(uint16_t *crc, uint8_t data);
#endif /* TARGET_AVR */

#endif /* HAVE_RPP */

#endif /* PLAT_H_ */
