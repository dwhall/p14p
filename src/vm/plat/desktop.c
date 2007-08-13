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

#undef __FILE_ID__
#define __FILE_ID__ 0x50
/**
 * PyMite platform-specific routines for Desktop target
 *
 * Log
 * ---
 *
 * 2007/01/10   #75: Added time tick service for desktop (POSIX) and AVR. (P.Adelt)
 * 2006/12/26   #65: Create plat module with put and get routines
 */

/* PyMite build process uses -ansi which disables certain features that
 * in turn disable features needed for signal processing. To work around
 * this, temporarily disable the corresponding #define. This is not
 * needed for Cygwin but for Linux. The -ansi option of GCC is explained
 * here: http://gcc.gnu.org/onlinedocs/gcc-4.0.3/gcc/C-Dialect-Options.html
 */
#ifdef __STRICT_ANSI__
#undef __STRICT_ANSI__
#include <stdio.h>
#define __STRICT_ANSI__
#else
#include <stdio.h>
#endif
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include "../pm.h"
#ifdef HAVE_RPP
    #include "../rpp.h"
#endif

/***************************************************************
 * Globals
 **************************************************************/

/**
 * This file is created if it does not exist and acts as a byte-addressable
 * user storage just like embedded targets' EEPROMs.
 */
#define PERSIST_FILENAME "pymite-persistent"

/**
 * Size of the file in bytes.
 */
#define PERSIST_SIZE     4096

FILE *plat_pPersistFile;

/***************************************************************
 * Prototypes
 **************************************************************/

void plat_sigalrm_handler(int signal);

/***************************************************************
 * Functions
 **************************************************************/

/* Desktop target shall use stdio for I/O routines. */
PmReturn_t
plat_init(void)
{
    PmReturn_t retval = PM_RET_OK;

    /* Let POSIX' SIGALRM fire every full millisecond. */
    /*
     * #67 Using sigaction complicates the use of getchar (below),
     * so signal() is used instead.
     */
    /* FIXME Cygwin dies with the following error, so a non-signal solution is
     * used at the moment.
     *
     * *** fatal error - called with threadlist_ix -1
     *
    signal(SIGALRM, plat_sigalrm_handler);
    ualarm(1000, 1000);
    */

    /* Open the persisten storage file. If neccessary, it is created and the
     * size is set to PERSIST_SIZE bytes.
     */
    plat_pPersistFile = fopen(PERSIST_FILENAME, "rb+");
    if (plat_pPersistFile == NULL)
    {
        /* Opening an existing file failed, try to create a new one. */
        plat_pPersistFile = fopen(PERSIST_FILENAME, "wb+");
        if (plat_pPersistFile == NULL)
        {
            PM_RAISE(retval, PM_RET_EX_IO);
            return retval;
        }
        /* Increase file size. */
        fseek(plat_pPersistFile, PERSIST_SIZE-1, SEEK_SET);
        fputc(0, plat_pPersistFile);
        
    }
    else
    {
        /* Check if file is big enough. */
        fseek(plat_pPersistFile, 0, SEEK_END);
        if (ftell(plat_pPersistFile) < PERSIST_SIZE) {
            /* Increase file size. */
            fseek(plat_pPersistFile, PERSIST_SIZE-1, SEEK_SET);
            fputc(0, plat_pPersistFile);
        }
        
    }

    return retval;
}


void
plat_sigalrm_handler(int signal)
{
    PmReturn_t retval;
    retval = pm_vmPeriodic(1000);
    PM_REPORT_IF_ERROR(retval);
}


/*
 * Gets a byte from the address in the designated memory space
 * Post-increments *paddr.
 */
uint8_t
plat_memGetByte(PmMemSpace_t memspace, uint8_t const **paddr)
{
    uint8_t b = 0;

    switch (memspace)
    {
        case MEMSPACE_RAM:
        case MEMSPACE_PROG:
            b = **paddr;
            *paddr += 1;
            return b;
        case MEMSPACE_FILE:
            fseek(plat_pPersistFile, (int)(uint8_t*)*paddr, SEEK_SET);
            *paddr += 1;
            return fgetc(plat_pPersistFile);
        case MEMSPACE_EEPROM:
        case MEMSPACE_SEEPROM:
        case MEMSPACE_OTHER0:
        case MEMSPACE_OTHER1:
        case MEMSPACE_OTHER2:
        default:
            return 0;
    }
}


/* Desktop target shall use stdio for I/O routines */
PmReturn_t
plat_getByte(uint8_t *b)
{
    int c;
    PmReturn_t retval = PM_RET_OK;

    c = getchar();
    *b = c & 0xFF;

    if (c == EOF)
    {
        PM_RAISE(retval, PM_RET_EX_IO);
    }

    return retval;
}


/* Desktop target shall use stdio for I/O routines */
PmReturn_t
plat_putByte(uint8_t b)
{
    int i;
    PmReturn_t retval = PM_RET_OK;

    i = putchar(b);
    fflush(stdout);

    if ((i != b) || (i == EOF))
    {
        PM_RAISE(retval, PM_RET_EX_IO);
    }

    #ifdef HAVE_RPP
        rpp_updateSendCrc(b);
    #endif

    return retval;
}


PmReturn_t
plat_getMsTicks(uint32_t *r_ticks)
{
    *r_ticks = pm_timerMsTicks;

    return PM_RET_OK;
}


void
plat_reportError(PmReturn_t result)
{
    printf("Error:     0x%02X\n", result);
    printf("  Release: 0x%02X\n", gVmGlobal.errVmRelease);
    printf("  FileId:  0x%02X\n", gVmGlobal.errFileId);
    printf("  LineNum: %d\n", gVmGlobal.errLineNum);
}

/**
 * Sets STDIO to non-blocking I/O and tries to read a byte.
 */
PmReturn_t
plat_pollByte(uint8_t *b)
{
    int c;
    ssize_t bytesRead;

    if (fcntl(fileno(stdin), F_SETFL, fcntl(fileno(stdin), F_GETFL) | O_NONBLOCK)) {
        /* Something failed */
        return PM_RET_ERR;
    }

    bytesRead = read(fileno(stdin), &c, 1);
    if (bytesRead == 0) {
        /* No data available. */
        return PM_RET_NO;
    }
    if (bytesRead == -1) {
        /* Some special condition occurred with no data read */
        switch (errno) {
            case 0:
                /* Undocumented case but does happen at least in Cygwin. */
                return PM_RET_NO;
            case (EAGAIN):
                /* Simply means that no data was available */
                return PM_RET_NO;
            default:
                return PM_RET_ERR;
        }
    }

    *b = c & 0xFF;

    /* FIXME Ugly hack to reduce system load. */
/*    usleep(10000);*/

    return PM_RET_OK;
}

/**
 * Desktop does not use interrupts and does not need critical section isolation.
 */
void
plat_enterCriticalSection(void)
{
}

void plat_exitCriticalSection(void)
{
}

#ifdef HAVE_RPP

PmReturn_t
plat_memSetByte(PmMemSpace_t memspace, uint8_t *paddr, uint8_t data)
{
    switch (memspace)
    {
        case MEMSPACE_FILE:
            fseek(plat_pPersistFile, (int)(uint8_t*)paddr, SEEK_SET);
            fputc(data, plat_pPersistFile);
            break;
        case MEMSPACE_PROG:
            /* Program memory not available as user storage. */
        case MEMSPACE_RAM:
            /* RAM not available as user storage. */
        case MEMSPACE_EEPROM:
        case MEMSPACE_SEEPROM:
        case MEMSPACE_OTHER0:
        case MEMSPACE_OTHER1:
        case MEMSPACE_OTHER2:
        default:
            break;
    }
    return PM_RET_OK;
}

PmReturn_t
plat_memGetInfo(PmMemSpace_t memspace, uint16_t *psize, uint8_t *pwriteable)
{
    switch (memspace)
    {
        case MEMSPACE_FILE:
            *psize = PERSIST_SIZE;
            *pwriteable = 1;
            break;
        case MEMSPACE_RAM:
            /* RAM not available as byte-addressable user storage, but
             * writeability hints that RAM objects can be created.
             */
            *psize = 0;
            *pwriteable = 1;
            break;
        case MEMSPACE_PROG:
            /* Program memory not available as user storage. */
        case MEMSPACE_EEPROM:
        case MEMSPACE_SEEPROM:
        case MEMSPACE_OTHER0:
        case MEMSPACE_OTHER1:
        case MEMSPACE_OTHER2:
        default:
            *psize = 0;
            *pwriteable = 0;
            break;
    }
    return PM_RET_OK;
}

PmReturn_t
plat_memReportName(PmMemSpace_t memspace)
{
    switch (memspace)
    {
        case MEMSPACE_FILE:
            rpp_sendBufferedString(MEM_NAME_FILE);
            break;
        case MEMSPACE_PROG:
            rpp_sendBufferedString(MEM_NAME_PROG);
            break;
        case MEMSPACE_RAM:
            rpp_sendBufferedString(MEM_NAME_RAM);
            break;
        case MEMSPACE_EEPROM:
            rpp_sendBufferedString(MEM_NAME_EEPROM);
            break;
        case MEMSPACE_SEEPROM:
            rpp_sendBufferedString(MEM_NAME_SEEPROM);
            break;
        case MEMSPACE_OTHER0:
            rpp_sendBufferedString(MEM_NAME_OTHER0);
            break;
        case MEMSPACE_OTHER1:
            rpp_sendBufferedString(MEM_NAME_OTHER1);
            break;
        case MEMSPACE_OTHER2:
            rpp_sendBufferedString(MEM_NAME_OTHER2);
            break;
        default:
            break;
    }
    return PM_RET_OK;
}


/**
 * Implementation based on avr-libc's equivalent C function in util/crc16.h at
 * http://www.nongnu.org/avr-libc/user-manual/group__util__crc.html .
 *
 * License:
 * Copyright (c) 2002, 2003, 2004  Marek Michalkiewicz
 * Copyright (c) 2005, Joerg Wunsch
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in
 *   the documentation and/or other materials provided with the
 *   distribution.
 *
 * * Neither the name of the copyright holders nor the names of
 *   contributors may be used to endorse or promote products derived
 *   from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 **/
void
plat_updateCrc16(uint16_t *pCrc, uint8_t data)
{
    int i;
    uint16_t crc = *pCrc;

    crc ^= data;
    for (i = 0; i < 8; ++i)
    {
        if (crc & 1)
            crc = (crc >> 1) ^ 0xA001;
        else
            crc = (crc >> 1);
    }
    *pCrc = crc;
}
#endif
