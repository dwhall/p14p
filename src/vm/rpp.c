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
#include "string.h"
#ifdef TARGET_AVR
#include "avr/pgmspace.h"
#endif

#ifdef HAVE_RPP

/**
 * Remote PyMite Protocol
 *
 * Log
 * ---
 *
 * 2007/07/04   Introduce RPP and RPM
 */

/***************************************************************
 * Globals
 **************************************************************/

volatile PmRppFlags_t rpp_flags;
uint8_t  rpp_receive_buffer[RPP_MESSAGE_BUFFER_SIZE];
uint8_t  rpp_send_buffer[RPP_MESSAGE_BUFFER_SIZE];
uint8_t  rpp_receive_pos;
PmRppState_t rpp_state;
PmRppType_t rpp_last_message_type;

/***************************************************************
 * Prototypes (private functions)
 **************************************************************/

/***************************************************************
 * Functions
 **************************************************************/

PmReturn_t
rpp_handleIncomingByte(uint8_t data) {
    /* If a NACK is to be sent or a complete message is waiting, data is
     * ignored.
     */
    if (rpp_flags.messageReceiveComplete || rpp_flags.sendNACK)
    {
        return PM_RET_OK;
    }

    /* BUG if legit data follows a complete message shortly after, the message
     * is dropped. Solutions: double buffering or buffering of data while
     * the complete message is processed. The latter requires the buffer to be
     * copied after removal of a message and then reexamined.
     */

    /* TODO Discard data that lurked around too long without being completed */

    if (rpp_receive_pos == RPP_OFFSET_STARTSYMBOL)
    {
        /* Buffer is empty, first byte needs to be the start symbol */
        if (data != RPP_START_SYMBOL)
        {
            C_DEBUG_PRINT(VERBOSITY_LOW,
                "RPP rejected byte %i: Should be 0xFF\n", data);
            return PM_RET_OK;
        }

        /* Reset the buffer before the first byte is written */
        rpp_resetBuffer();

    }
    else if (rpp_receive_pos == RPP_OFFSET_SEQUENCENUMBER)
    {
        /* Check sequence number if we are already synced. */
        if (rpp_state.sequenceNumberKnown)
        {
            if (data != (rpp_state.lastSequenceNumber+1))
            {
                /* Wrong sequence number. */
                /* TODO Handle wrong sequence number. Ignored for now. */
                C_DEBUG_PRINT(VERBOSITY_LOW,
                    "RPP would reject message: Sequence number should be %i " \
                    "(received %i)\n", (rpp_state.lastSequenceNumber+1), data);
            }

        }
        else
        {
            /* Use this sequence number to synchronize */
            rpp_state.sequenceNumberKnown = 1;
            C_DEBUG_PRINT(VERBOSITY_HIGH,
                "RPP synchronizes with sequence number %i\n", data);
        }
        rpp_state.lastSequenceNumber = data;
    }
    else if (rpp_receive_pos == RPP_OFFSET_TYPE)
    {
        /* Check if type has a possible value. */
        if ((data == 0) || (((PmRppType_t)data) >= RPP_T_LAST))
        {
            /* Message type is not valid. */
            C_DEBUG_PRINT(VERBOSITY_LOW,
                "RPP rejects message: Message type %i unknown\n",
                data);
            rpp_resetBuffer();
            return PM_RET_OK;
        }
    }
    else if (rpp_receive_pos == RPP_OFFSET_DATALENGTH)
    {
        /* Check if length is small enough. */
        if (data > RPP_DATA_BUFFER_SIZE)
        {
            /* Too much data. This would not fit the buffer. */
            /* TODO Send a message stating problem with message length. */
            C_DEBUG_PRINT(VERBOSITY_MEDIUM,
                "RPP rejects message: Data length too big. (is: %i, max: %i)\n",
                data, RPP_DATA_BUFFER_SIZE);
            rpp_resetBuffer();
            return PM_RET_OK;
        }
    } else if (rpp_receive_pos ==
        rpp_receive_buffer[RPP_OFFSET_DATALENGTH]
        +RPP_OFFSET_DATASTART+1)
    {
        /* Last byte was received (MSB of CRC) */
        if ((rpp_receive_buffer[rpp_receive_pos-1]
            + (data<<8)) == rpp_state.receiveCrc)
        {
            /* Message is intact. */
            rpp_flags.messageReceiveComplete = 1;
            C_DEBUG_PRINT(VERBOSITY_HIGH,
                "Intact message with type %i and data length %i received\n",
                rpp_receive_buffer[RPP_OFFSET_TYPE],
                rpp_receive_buffer[RPP_OFFSET_DATALENGTH]);
        }
        else
        {
            C_DEBUG_PRINT(VERBOSITY_HIGH, "CRC expected=%X, received=%X.\n",
                rpp_state.receiveCrc, (rpp_receive_buffer[rpp_receive_pos-1]
                    + (data<<8)));
            /* Message is complete but CRCs do not match. */
            rpp_flags.sendNACK = 1;
            rpp_resetBuffer();
            return PM_RET_OK;
        }
        /* TODO Improve recovery by not throwing away all the data on error but only remove up to next START_SYMBOL. */
    }

    /* The position should never be beyond the last buffer byte here. */
    C_ASSERT(rpp_receive_pos < RPP_MESSAGE_BUFFER_SIZE);

    rpp_receive_buffer[rpp_receive_pos] = data;
    if (rpp_receive_pos > 0) {
        /* Incrementally calculate CRC data. Start with the second byte so
         * upon receiving the last byte of the CRC field, the first byte of the
         * CRC field has not been integrated into this CRC calculation yet.
         */
        plat_updateCrc16(&(rpp_state.receiveCrc),
            rpp_receive_buffer[rpp_receive_pos-1]);
    }
    rpp_receive_pos++;
    return PM_RET_OK;
}

PmReturn_t
rpp_sendNACK(void)
{
    PmReturn_t retval;

    C_DEBUG_PRINT(VERBOSITY_HIGH,
        "Sending NACK.\n");

    retval = rpp_sendPreambel(RPP_T_NACK);
    PM_RETURN_IF_ERROR(retval);
    retval = plat_putByte(0); /* Empty data field */
    PM_RETURN_IF_ERROR(retval);
    retval = rpp_sendCrc();
    PM_RETURN_IF_ERROR(retval);

    plat_enterCriticalSection();
    rpp_flags.sendNACK = 0;
    plat_exitCriticalSection();
    return retval;
}

PmReturn_t
rpp_sendACK(void)
{
    PmReturn_t retval;

    C_DEBUG_PRINT(VERBOSITY_HIGH,
        "Sending ACK.\n");

    retval = rpp_sendPreambel(RPP_T_ACK);
    PM_RETURN_IF_ERROR(retval);
    retval = plat_putByte(0); /* Empty data field */
    PM_RETURN_IF_ERROR(retval);
    retval = rpp_sendCrc();
    PM_RETURN_IF_ERROR(retval);

    return retval;
}

void
rpp_resetBuffer(void)
{
    rpp_receive_pos = 0;
    rpp_state.receiveCrc = RPP_CRC_INIT;
}

PmReturn_t
rpp_sendSychronizationHook(void)
{
    uint32_t timestamp;
    PmReturn_t retval;

    if (rpp_state.synchronizationOk)
    {
        /* This should not happen as caller should have checked. */
        return PM_RET_OK;
    }
    retval = plat_getMsTicks(&timestamp);
    PM_RETURN_IF_ERROR(retval);

    if ((timestamp-rpp_state.lastSychronizationSendTimestamp)
        > RPP_SYNC_RESEND_PERIOD)
    {
        /* Time for another attempt to tell our state and hope for ACK. */
        retval = rpp_sendMessageSync();
        PM_RETURN_IF_ERROR(retval);
        rpp_state.lastSychronizationSendTimestamp = timestamp;
    }
    return PM_RET_OK;
}

PmReturn_t
rpp_sendPreambel(PmRppType_t type)
{
    C_DEBUG_PRINT(VERBOSITY_HIGH,
        "Sending Preambel.\n");

    rpp_state.sendCrc = RPP_CRC_INIT;
    PmReturn_t retval;

    retval = plat_putByte(RPP_START_SYMBOL);
    PM_RETURN_IF_ERROR(retval);
    retval = plat_putByte(rpp_state.nextSequenceNumber);
    PM_RETURN_IF_ERROR(retval);
    retval = plat_putByte(type);
    PM_RETURN_IF_ERROR(retval);

    rpp_state.nextSequenceNumber++;
    return retval;
}

PmReturn_t
rpp_sendCrc(void)
{
    C_DEBUG_PRINT(VERBOSITY_HIGH,
        "Sending CRC.\n");

    /* Need to buffer the crc as sending the first byte will alter it. */
    uint16_t crc = rpp_state.sendCrc;
    PmReturn_t retval;

    retval = plat_putByte(crc & 0xFF);
    PM_RETURN_IF_ERROR(retval);
    retval = plat_putByte(crc>>8);
    PM_RETURN_IF_ERROR(retval);

    return retval;
}

PmReturn_t
rpp_sendMessageSync(void)
{
    uint8_t i;
    PmReturn_t retval;

    #ifdef TARGET_AVR
        const char* identification = (const char*)PSTR(RPM_IDENTIFICATION);
        uint8_t length = strlen_P(identification);
    #else
        char* identification = RPM_IDENTIFICATION;
        uint8_t length = strlen(identification);
    #endif

    retval = rpp_sendPreambel(RPP_T_SYNC);
    PM_RETURN_IF_ERROR(retval);

    retval = plat_putByte(19); /* data size */
    PM_RETURN_IF_ERROR(retval);

    /* data field start */

    retval = plat_putByte(1); /* reset on */
    PM_RETURN_IF_ERROR(retval);

    retval = plat_putByte(RPP_MESSAGE_BUFFER_SIZE);
    PM_RETURN_IF_ERROR(retval);

    retval = plat_putByte(sizeof(void*));
    PM_RETURN_IF_ERROR(retval);

    for(i = 0; i<length; i++)
    {
        #ifdef TARGET_AVR
            retval = plat_putByte(pgm_read_byte(identification+i));
        #else
            retval = plat_putByte(*(identification+i));
        #endif
        PM_RETURN_IF_ERROR(retval);
    }

    retval = rpp_sendCrc();
    PM_RETURN_IF_ERROR(retval);

    rpp_last_message_type = RPP_T_SYNC;

    return retval;
}

void
rpp_updateSendCrc(uint8_t data)
{
    plat_updateCrc16(&(rpp_state.sendCrc), data);
}

PmReturn_t
rpp_sendBufferedStart(PmRppType_t type)
{
    PmReturn_t retval;
    uint8_t i;

    rpp_state.bufferedSendType = type;
    rpp_state.bufferedSendPos = 0;

    if ((type == RPP_T_THREAD_PRINT_ITEM) || (type == RPP_T_THREAD_PRINT_EXPR))
    {
        /* These messages start with the address of the current thread. */
        for (i=0; i<sizeof(gVmGlobal.pthread); i++)
        {
            retval = rpp_sendBuffered((((int)gVmGlobal.pthread)>>(8*i)) & 0xFF);
            PM_RETURN_IF_ERROR(retval);
        }
    }
    return PM_RET_OK;
}

PmReturn_t
rpp_sendBuffered(uint8_t data)
{
    PmReturn_t retval;
    /* Note: This routine conveniently ignores the partners receive buffer size.
     * The partner could have advertised it in its synchronization message, but
     * we simply ignore it here.
     */
    if (rpp_state.bufferedSendPos >= sizeof(rpp_send_buffer))
    {
        /* The send buffer is full, send it out. */
        retval = rpp_sendBufferedFlush();
        PM_RETURN_IF_ERROR(retval);
    }
    rpp_send_buffer[rpp_state.bufferedSendPos] = data;
    rpp_state.bufferedSendPos++;
    return PM_RET_OK;
}

#if defined(TARGET_AVR)
PmReturn_t
rpp_sendBufferedString_P(const char* string)
{
    PmReturn_t retval;
    uint8_t i;
    uint8_t length = strlen_P(string);
    for (i = 0; i < length; i++)
    {
        retval = rpp_sendBuffered(pgm_read_byte(string+i));
        PM_RETURN_IF_ERROR(retval);
    }
    return PM_RET_OK;
}
#endif

PmReturn_t
rpp_sendBufferedString(uint8_t* string)
{
    PmReturn_t retval;
    uint8_t i;
    uint8_t length = strlen(string);
    for (i = 0; i < length; i++)
    {
        retval = rpp_sendBuffered(*(string+i));
        PM_RETURN_IF_ERROR(retval);
    }
    return PM_RET_OK;
}

PmReturn_t
rpp_sendBufferedFlush(void)
{
    PmReturn_t retval;
    uint8_t i;
    /* Start the message. */
    retval = rpp_sendPreambel(rpp_state.bufferedSendType);
    PM_RETURN_IF_ERROR(retval);

    /* Send out the amount of bytes. */
    retval = plat_putByte(rpp_state.bufferedSendPos);
    PM_RETURN_IF_ERROR(retval);

    /* Send buffered data. */
    for (i=0; i<rpp_state.bufferedSendPos; i++)
    {
        retval = plat_putByte(rpp_send_buffer[i]);
        PM_RETURN_IF_ERROR(retval);
    }

    /* Send checksum to finish message. */
    retval = rpp_sendCrc();
    PM_RETURN_IF_ERROR(retval);

    rpp_state.bufferedSendPos = 0;
    return retval;
}

PmReturn_t
rpp_sendBufferedFinish(void)
{
    PmReturn_t retval;

    /* Send out the rest of the message. In case the waiting data exactly fills
     * the buffer, send out an additional empty message as per protocol
     * specification for overlong messages.
     */
    if (rpp_state.bufferedSendPos >= sizeof(rpp_send_buffer))
    {
        retval = rpp_sendBufferedFlush();
        PM_RETURN_IF_ERROR(retval);
    }
    retval = rpp_sendBufferedFlush();
    PM_RETURN_IF_ERROR(retval);
    return retval;
}

#endif /* HAVE_RPP */
