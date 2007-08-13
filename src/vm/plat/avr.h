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
 * PyMite platform-specific routines for AVR target
 *
 * Log
 * ---
 *
 * 2007/07/04   Platform-dependant definitions
 */

/**
 * If defined signals that polling receiving is not needed.
 */
 #define PLAT_RECEIVE_BY_INTERRUPT

/**
  * If defined, plat_pollByte() allows non-blocking polling for incoming byte.
  */
#undef PLAT_HAVE_POLL_BYTE
