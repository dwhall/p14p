/*
 * PyMite - A flyweight Python interpreter for 8-bit microcontrollers and more.
 *
 * Remote PyMite Management/Remote Pymite Protocol
 * Copyright 2007 Philipp Adelt
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
 * Remote PyMite Management
 *
 * Log
 * ---
 *
 * 2007/07/04   Introduce RPP and RPM
 */

#ifdef HAVE_RPM

#ifndef RPM_H_
#define RPM_H_

/* RPM_IDENTIFICATION must be exactly 16 bytes long. */
#define RPM_IDENTIFICATION                  "PyMite is ready."

/* At the beginning of one of the memspaces there is a statically allocated
 * area for the autorun list. It is a comma-separated, 0-terminated list of
 * code image names. Access to the code images stored in the memspaces needs
 * to take into account that one memory space has this field at the start.
 */
#if defined(TARGET_DESKTOP)
#define RPM_AUTORUN_LIST_MEMSPACE           MEMSPACE_FILE   
#else
#define RPM_AUTORUN_LIST_MEMSPACE           MEMSPACE_EEPROM
#endif
/* Maximum amount of bytes reserved for the autorun list. Note that the list
 * must be 0-terminated.
 */
#define RPM_AUTORUN_LIST_LENGTH             50
#define RPM_AUTORUN_LIST_OFFSET             0

PmReturn_t rpm_init(void);

/**
 * Should be called by loop when rpp_flags.messageReceiveComplete is set.
 * Inspects the message and activates the appropriate domain-specific message
 * handler.
 * Resets the rpp_flags.messageReceiveComplete flag in an atomic manner.
 */
PmReturn_t rpm_handleMessage(void);

/**
 * Sends a list of thread addresses.
 */
PmReturn_t rpm_sendThreadList(void);

/**
 * Sends an eval'able string with memory space data.
 */
PmReturn_t rpm_sendMemList(void);

#endif /* RPP_H_ */

#endif /* HAVE_RPM */
