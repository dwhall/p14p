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
 * Log
 * ---
 *
 * 2007/07/04   Introduce RPP and RPM
 */

#undef __FILE_ID__
#define __FILE_ID__ 0x19

#include "pm.h"

#ifdef HAVE_RPP

PmReturn_t
persist_init(void)
{
    PmReturn_t retval = PM_RET_OK;
    uint8_t i;

    for (i = 0; i < MEMSPACE_LAST; i++)
    {
        persist_checkSpace(i);
    }
    return retval;
}

PmReturn_t
persist_getUsedBytes(PmMemSpace_t memspace, uint16_t *pused)
{
    PmReturn_t retval = PM_RET_OK;
    uint8_t *paddr;

    paddr = (uint8_t*)PERSIST_OFFSET_USED;
    *pused  = plat_memGetByte(memspace, ((uint8_t const **)&paddr));
    *pused += plat_memGetByte(memspace, ((uint8_t const **)&paddr))<<8;

    return retval;
}


PmReturn_t
persist_getFreeBytes(PmMemSpace_t memspace, uint16_t *pfree)
{
    PmReturn_t retval = PM_RET_OK;
    uint16_t used;
    uint16_t size;
    uint8_t  writeable;

    persist_getUsedBytes(memspace, &used);

    retval = plat_memGetInfo(memspace, &size, &writeable);
    PM_RETURN_IF_ERROR(retval);

    *pfree = size - used;

    return retval;
}

PmReturn_t persist_checkSpace(PmMemSpace_t memspace)
{
    PmReturn_t retval = PM_RET_OK;
    /* Check for magic. */
    uint16_t magic, used;
    uint8_t *paddr;
    uint16_t size, i;
    uint8_t  writeable;

    retval = plat_memGetInfo(memspace, &size, &writeable);
    PM_RETURN_IF_ERROR(retval);

    if (size == 0 || writeable == 0)
    {
        return PM_RET_NO;
    }

    paddr  = (uint8_t*)PERSIST_OFFSET_MAGIC;
    magic  = plat_memGetByte(memspace, ((uint8_t const **)&paddr));
    magic += plat_memGetByte(memspace, ((uint8_t const **)&paddr))<<8;
    paddr  = (uint8_t*)PERSIST_OFFSET_USED;
    used   = plat_memGetByte(memspace, ((uint8_t const **)&paddr));
    used  += plat_memGetByte(memspace, ((uint8_t const **)&paddr))<<8;

    if (magic != PERSIST_MAGIC)
    {
        /* Format the memory space. */
        C_DEBUG_PRINT(VERBOSITY_MEDIUM, "Formatting space %i\n", memspace);
        paddr = (uint8_t*)PERSIST_OFFSET_MAGIC;
        plat_memSetByte(memspace, paddr++, PERSIST_MAGIC & 0xFF);
        plat_memSetByte(memspace, paddr++, PERSIST_MAGIC >> 8);
        /* Overwrite the rest with 0. Thereby setting used bytes and checksum
         * to 0 too.
         */
        for (i=2; i<size; i++)
        {
            plat_memSetByte(memspace, paddr++, 0);
        }
    }

    return retval;
}

PmReturn_t
persist_appendData(PmMemSpace_t memspace, uint8_t *pdata, uint16_t length)
{
    PmReturn_t retval = PM_RET_OK;
    uint16_t i;
    uint16_t used;
    uint8_t *paddr;

    persist_getUsedBytes(memspace, &used);
    paddr = (uint8_t*)(used+PERSIST_OFFSET_DATA);
    for (i=0; i<length; i++)
    {
        plat_memSetByte(memspace, paddr, *pdata);
        pdata++;
        paddr++;
    }
    
    /* Increase used byte count. */
    used += length;
    paddr = (uint8_t*)PERSIST_OFFSET_USED;
    plat_memSetByte(memspace, paddr++, used & 0xFF);
    plat_memSetByte(memspace, paddr++, used >> 8);
    
    return retval;
}

uint8_t
persist_memGetByte(PmMemSpace_t memspace, uint8_t const **paddr)
{
    uint8_t result;
    *paddr += PERSIST_OFFSET_DATA;
    result = plat_memGetByte(memspace, paddr);
    *paddr -= PERSIST_OFFSET_DATA;
    return result;
}

#endif /* HAVE_RPP */
