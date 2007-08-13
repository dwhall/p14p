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
 * Persistence module
 *
 * This module provides access to multiple memory spaces as defined in mem.h
 * with additional functions from plat.h. It handles a continous block of data
 * in each of these spaces and does bookkeeping on the amount of bytes used.
 * 
 * At the beginning of each block, a magic number is used to identify a
 * formatted memory space. Following this comes a 16bit word containing the
 * remaining free bytes in this space followed by a CRC16 of the following
 * data section (only the used part is calculated).
 *
 * TODO CRC checking is completely missing.
 *
 * Log
 * ---
 *
 * 2007/07/04   Introduce RPP and RPM
 */


#ifndef PERSIST_H_
#define PERSIST_H_

#define PERSIST_MAGIC           0xBEEF
#define PERSIST_OFFSET_MAGIC    0
#define PERSIST_OFFSET_USED     2
#define PERSIST_OFFSET_CRC      4
#define PERSIST_OFFSET_DATA     6

#ifdef HAVE_RPP

/**
 * Uses persist_checkSpace() on all memory spaces that have a size > 0 and
 * are writeable.
 */
PmReturn_t persist_init(void);

/**
 * Returns the number of used bytes.
 * @param pused ptr to the variable to writeto
 */
PmReturn_t persist_getUsedBytes(PmMemSpace_t memspace, uint16_t *pused);

/**
 * Returns the number of remaining bytes.
 * @param pfree ptr to the variable to write to
 */
PmReturn_t persist_getFreeBytes(PmMemSpace_t memspace, uint16_t *pfree);

/**
 * Checks for magic at the start and checks CRC (unimplemented). Formats
 * memory space if check fails.
 */
PmReturn_t persist_checkSpace(PmMemSpace_t memspace);

/**
 * Appends a block of data to the end of the memory space.
 * @param memspace target memory space
 * @param pdata ptr to the start of the data to be appended
 * @param length number of bytes to append
 */
PmReturn_t persist_appendData(PmMemSpace_t memspace, uint8_t *pdata,
    uint16_t length);

/**
 * Access to the data field via persist_mem*-functions. Adds the data start
 * offset of the persistance data structure at the beginning of each memory
 * space so the user can address from 0.
 */ 
uint8_t persist_memGetByte(PmMemSpace_t memspace, uint8_t const **paddr);

#define persist_memSetByte(memspace, paddr, data) plat_memSetByte( \
    (memspace), (paddr)+PERSIST_OFFSET_DATA, (data));

#endif /* HAVE_RPP */

#endif /* PERSIST_H_ */
