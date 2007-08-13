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

#undef __FILE_ID__
#define __FILE_ID__ 0x17

#include "pm.h"
#ifdef TARGET_AVR
#include "avr/pgmspace.h"
#endif

/**
 * Remote PyMite Management
 *
 * Log
 * ---
 *
 * 2007/07/04   Introduce RPP and RPM
 */

#ifdef HAVE_RPM

/***************************************************************
 * Private protoypes
 **************************************************************/

void rpm_resetFlagIncomingMessage(void);
PmReturn_t rpm_handleAutorunList(void);
PmReturn_t rpm_handleAddBlock(void);

/***************************************************************
 * Globals
 **************************************************************/

/* When receiving overlong data, this counts the current segment number
 * from 1. Otherwise, it is 0.
 */
static uint8_t     rpm_olSegmentNumber;
static uint16_t    rpm_olTotalDataLength;

/***************************************************************
 * Functions
 **************************************************************/

PmReturn_t
rpm_init(void)
{
    PmReturn_t retval = PM_RET_OK;
    uint16_t used, i;
    uint8_t  t08;

    retval = persist_getUsedBytes(RPM_AUTORUN_LIST_MEMSPACE, &used);
    PM_RETURN_IF_ERROR(retval);
    
    t08 = 0;
    /* Let persistence reserve an area for the autorun list if neccessary. */
    if (used < RPM_AUTORUN_LIST_LENGTH)
    {
        C_DEBUG_PRINT(VERBOSITY_HIGH, "rpm_init creates the autorun list buffer"
            ". Used bytes in memspace %i until now were %i byte. Needed: %i\n",
            RPM_AUTORUN_LIST_MEMSPACE, used, RPM_AUTORUN_LIST_LENGTH);
        for (i = 0; i < (RPM_AUTORUN_LIST_LENGTH - used); i++)
        {
            retval = persist_appendData(RPM_AUTORUN_LIST_MEMSPACE, &t08, 1);
            PM_RETURN_IF_ERROR(retval);
        }
    }
    return retval;
}

void
rpm_resetFlagIncomingMessage(void)
{
    rpp_resetBuffer();
    plat_enterCriticalSection();
    rpp_flags.messageReceiveComplete = 0;
    plat_exitCriticalSection();
}

PmReturn_t
rpm_handleMessage(void)
{
    PmReturn_t retval;

    /* If an overlong message is being received, this holds the type. */
    static PmRppType_t olType;
    
    if (rpm_olSegmentNumber)
    {
        /* This should be the same type as the last message. */
        if (olType != (PmRppType_t)rpp_receive_buffer[RPP_OFFSET_TYPE])
        {
            /* Something went wrong - we didn't receive the last segment.
             * We don't handle this yet (TODO), but at least reset state.
             */
            rpm_olSegmentNumber = 0;
        }
    }
    else if (rpp_receive_buffer[RPP_OFFSET_DATALENGTH] == RPP_DATA_BUFFER_SIZE)
    {
        /* Message is the first segment of an overlong message. */
        olType = (PmRppType_t)rpp_receive_buffer[RPP_OFFSET_TYPE];
        rpm_olTotalDataLength = rpp_receive_buffer[RPP_OFFSET_DATASTART]
            + (rpp_receive_buffer[RPP_OFFSET_DATASTART+1]<<8);
    }

    switch ((PmRppType_t)rpp_receive_buffer[RPP_OFFSET_TYPE])
    {
        case RPP_T_SYNC:
            /* Incoming sync messages are acknowledged and answered. */
            rpm_resetFlagIncomingMessage();
            retval = rpp_sendACK();
            PM_RETURN_IF_ERROR(retval);

            retval = rpp_sendMessageSync();
            PM_RETURN_IF_ERROR(retval);
            break;
        case RPP_T_ACK:
            rpm_resetFlagIncomingMessage();
            switch (rpp_last_message_type)
            {
                case RPP_T_SYNC:
                    /* This is the response to our sync message. */
                    rpp_state.synchronizationOk = 1;
                    break;
                default:
                    break;
            }
            break;
        case RPP_T_THREAD_LIST:
            rpm_resetFlagIncomingMessage();
            retval = rpm_sendThreadList();
            PM_RETURN_IF_ERROR(retval);
            break;
        case RPP_T_MEM_LIST:
            rpm_resetFlagIncomingMessage();
            retval = rpm_sendMemList();
            PM_RETURN_IF_ERROR(retval);
            break;
        case RPP_T_MEM_ADD_BLOCK:
            retval = rpm_handleAddBlock();
            rpm_resetFlagIncomingMessage();
            PM_RETURN_IF_ERROR(retval);
            break;
        case RPP_T_AUTORUN_LIST:
            retval = rpm_handleAutorunList();
            rpm_resetFlagIncomingMessage();
            PM_RETURN_IF_ERROR(retval);
            break;
        case RPP_T_THREAD_START:
            /* Enforce 0-termination of string inside receive buffer. */
            rpp_receive_buffer[RPP_OFFSET_DATASTART+
                rpp_receive_buffer[RPP_OFFSET_DATALENGTH]] = 0;
            retval = pm_addThread(rpp_receive_buffer+RPP_OFFSET_DATASTART);
            PM_RETURN_IF_ERROR(retval);
            rpm_resetFlagIncomingMessage();
            retval = rpp_sendACK();
            break;
        default:
            rpm_resetFlagIncomingMessage();
            retval = rpp_sendACK();
            PM_RETURN_IF_ERROR(retval);
            break;
    }

    if (rpm_olSegmentNumber &&
        (rpp_receive_buffer[RPP_OFFSET_DATALENGTH] != RPP_DATA_BUFFER_SIZE))
    {
        /* This was the last segment. */
        rpm_olSegmentNumber = 0;
    }
    
    return PM_RET_OK;
}

PmReturn_t
rpm_sendThreadList(void)
{
    PmReturn_t retval;
    uint16_t threadCount = gVmGlobal.threadList->length;
    uint16_t i,b;
    pPmObj_t pThread;

    retval = rpp_sendBufferedStart(RPP_T_THREAD_LIST);
    PM_RETURN_IF_ERROR(retval);

    /* Iterate over thread list. */
    for (i=0; i < threadCount; i++)
    {
        retval = list_getItem((pPmObj_t)gVmGlobal.threadList, i, (pPmObj_t*)&pThread);
        PM_RETURN_IF_ERROR(retval);
        for (b=0; b < sizeof(pPmThread_t); b++)
        {
            retval = rpp_sendBuffered(((int)pThread) >> (8*b));
            PM_RETURN_IF_ERROR(retval);
        }
    }
    return rpp_sendBufferedFinish();
}

PmReturn_t
rpm_sendMemList(void)
{
    PmReturn_t retval;
    uint16_t size;
    uint8_t i,k,writeable, bytesWritten;
    uint8_t buffer[30];

    retval = rpp_sendBufferedStart(RPP_T_MEM_LIST);
    PM_RETURN_IF_ERROR(retval);

    retval = rpp_sendBuffered('{');
    PM_RETURN_IF_ERROR(retval);
    /* Iterate over memory spaces. */
    for (i=0; i < MEMSPACE_LAST; i++)
    {
        plat_memGetInfo(i, &size, &writeable);
        #ifdef TARGET_AVR
        bytesWritten = snprintf_P((char *)&buffer, sizeof(buffer),
            PSTR(" %u:(\""), i);
        #else
        /* This does not use snprintf because glibc's snprintf is only
         * included for compiles without strict-ansi.
         */
        bytesWritten =
            sprintf((void *)&buffer, " %u:(\"", i);
        #endif /* !TARGET_AVR */

        for (k = (uint8_t)0; k < bytesWritten; k++)
        {
            retval = rpp_sendBuffered(buffer[k]);
            PM_RETURN_IF_ERROR(retval);
        }

        plat_memReportName(i);

        #ifdef TARGET_AVR
        bytesWritten = snprintf_P((char *)&buffer, sizeof(buffer),
            PSTR("\",%u,%u),"), size, writeable );
        #else
        /* This does not use snprintf because glibc's snprintf is only
         * included for compiles without strict-ansi.
         */
        bytesWritten =
            sprintf((void *)&buffer, "\",%u,%u),", size, writeable);
        #endif /* !TARGET_AVR */

        for (k = (uint8_t)0; k < bytesWritten; k++)
        {
            retval = rpp_sendBuffered(buffer[k]);
            PM_RETURN_IF_ERROR(retval);
        }

    }
    retval = rpp_sendBuffered('}');
    return rpp_sendBufferedFinish();
}

PmReturn_t
rpm_handleAutorunList(void)
{
    PmReturn_t retval = PM_RET_OK;
    uint8_t* plist;
    uint8_t data;
    uint8_t i;
    
    if (rpp_receive_buffer[RPP_OFFSET_DATALENGTH] == 0) {
        /* This is a request for the list, so send it. */
        retval = rpp_sendBufferedStart(RPP_T_AUTORUN_LIST);
        PM_RETURN_IF_ERROR(retval);

        plist = (uint8_t*)RPM_AUTORUN_LIST_OFFSET;
        data = persist_memGetByte(RPM_AUTORUN_LIST_MEMSPACE,
            (uint8_t const **)&plist);
        while (data)
        {
            retval = rpp_sendBuffered(data);
            PM_RETURN_IF_ERROR(retval);
            data = persist_memGetByte(RPM_AUTORUN_LIST_MEMSPACE,
                (uint8_t const **)&plist);
        }
        retval = rpp_sendBufferedFinish();
        PM_RETURN_IF_ERROR(retval);
    }
    else
    {
        /* This is a new list to be stored. */
        plist = (uint8_t*)RPM_AUTORUN_LIST_OFFSET;
        for (i=0; i<rpp_receive_buffer[RPP_OFFSET_DATALENGTH]; i++)
        {
            persist_memSetByte(RPM_AUTORUN_LIST_MEMSPACE, plist,
                rpp_receive_buffer[RPP_OFFSET_DATASTART+i]);
            plist++;
            /* Need to reserve one byte for 0-termination. */
            if (i >= RPM_AUTORUN_LIST_LENGTH-2)
            {
                break;
            }
        }
        /* Write 0-termination. */
        persist_memSetByte(RPM_AUTORUN_LIST_MEMSPACE, plist, 0);
        retval = rpp_sendACK();
    }
    
    return retval;
}

PmReturn_t rpm_handleAddBlock(void)
{
    PmReturn_t retval = PM_RET_OK;
    static PmMemSpace_t memspace;
    /* pstart will hold the address in memspace of the first byte. */
    static uint16_t startAddr;
    
    C_DEBUG_PRINT(VERBOSITY_HIGH, "handleAddBlock 1 rpm_olSegmentNumber=%i\n",
        rpm_olSegmentNumber);
    if (rpm_olSegmentNumber < 2)
    {
        /* Either the first or the only segment, so remember start address. */
        switch (rpm_olSegmentNumber)
        {
            case 0: 
                memspace = rpp_receive_buffer[RPP_OFFSET_DATASTART];
                break;
            case 1: 
                memspace = rpp_receive_buffer[RPP_OFFSET_DATASTART+2];
                break;
        }
        retval = persist_getUsedBytes(memspace, &startAddr);
        startAddr += PERSIST_OFFSET_DATA;
        C_DEBUG_PRINT(VERBOSITY_HIGH, "handleAddBlock 2 retval=%i startAddr=%i\n",
            retval, startAddr);
        PM_RETURN_IF_ERROR(retval);
    }

    C_DEBUG_PRINT(VERBOSITY_HIGH, "handleAddBlock 3 rpm_olSegmentNumber=%i\n",
        rpm_olSegmentNumber);

    if (rpm_olSegmentNumber == 1)
    {
        /* This is the first segment of an overlong message. */
        /* Copy neither the TDL field not the memory space byte. */
        retval = persist_appendData(memspace,
            rpp_receive_buffer+RPP_OFFSET_DATASTART+3, RPP_DATA_BUFFER_SIZE-3);
        PM_RETURN_IF_ERROR(retval);
    }
    else if (rpm_olSegmentNumber == 0)
    {
        /* Single-segment message. */
        memspace = rpp_receive_buffer[RPP_OFFSET_DATASTART];
        /* Do not copy the memspace byte. */
        retval = persist_appendData(memspace,
            rpp_receive_buffer+RPP_OFFSET_DATASTART+1,
            rpp_receive_buffer[RPP_OFFSET_DATALENGTH]-1);
        C_DEBUG_PRINT(VERBOSITY_HIGH, "handleAddBlock 4 memspace=%i von=%p laenge=%i rpp_r_b=%p\n",
            memspace, 
            rpp_receive_buffer+RPP_OFFSET_DATASTART+1,
            rpp_receive_buffer[RPP_OFFSET_DATALENGTH]-1,
            rpp_receive_buffer);
        PM_RETURN_IF_ERROR(retval);
    }
    else
    {
        /* A segment after the first. */
        /* No memory space byte in the following segments. */
        retval = persist_appendData(memspace, rpp_receive_buffer+RPP_OFFSET_DATASTART,
            rpp_receive_buffer[RPP_OFFSET_DATALENGTH]);
        PM_RETURN_IF_ERROR(retval);
    }
    
    if (rpp_receive_buffer[RPP_OFFSET_DATALENGTH] < RPP_DATA_BUFFER_SIZE)
    {
        uint8_t const *paddr;
        /* Must be a single-segment or the last segment, so try to add the new
         * data as an image.
         */
        /* TODO This is a shameless abuse of the message and should be refactored. */
        C_DEBUG_PRINT(VERBOSITY_HIGH, "handleAddBlock 5 startaddr=%i\n",
            startAddr);
        paddr = (uint8_t const*)(unsigned int)startAddr;
        retval = img_findInMem(memspace, &paddr);
        if (retval != PM_RET_OK)
        {
            C_DEBUG_PRINT(VERBOSITY_HIGH, "img_findInMem returned with"
                "retval=%X startaddr=%i\n", retval, startAddr);
        }
    }

    retval = rpp_sendACK();
    return retval;
}

#endif /* HAVE_RPM */
