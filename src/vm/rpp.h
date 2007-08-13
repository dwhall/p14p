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
 * Remote PyMite Protocol
 *
 * Handle incoming and outgoing data to conform to a protocol that multiplexes
 * several message types into one single order-preserving (FIFO) byte-stream
 * channels per direction.
 *
 * Porting note: To enable receiving data, do one of the following:
 * a) Implement plat.h's plat_pollByte and define PLAT_HAVE_POLL_BYTE or
 * b) implement some receive interrupt to call rpp_handleIncomingByte() with
 *    the received byte. rpp_handleIncomingByte() is promised to be as fast as
 *    possible.
 *
 * Solution a) works as RPP places a hook into the interpreter-loop to look for
 * bytes that have been queued since the last run.
 *
 * Log
 * ---
 *
 * 2007/07/04   Introduce RPP and RPM
 */


#ifndef RPP_H_
#define RPP_H_

/***************************************************************
 * Defines
 **************************************************************/

/**
 * Size of raw receive buffer that eats RAM and determines the size of the
 * maximum message data field length.
 */
#define RPP_MESSAGE_BUFFER_SIZE             50

/**
 * Amount of milliseconds that at least pass between two send attempts for
 * the synchronization message.
 */
#define RPP_SYNC_RESEND_PERIOD              2000

/**
 * Maximum amount of bytes that fit in the data section of a received message.
 */
#define RPP_DATA_BUFFER_SIZE (RPP_MESSAGE_BUFFER_SIZE-6)

#define RPP_START_SYMBOL                    0xFF
#define RPP_CRC_INIT                        0xFFFF

/* Offset into the message buffer for fixed position fields.
 */
#define RPP_OFFSET_STARTSYMBOL              0
#define RPP_OFFSET_SEQUENCENUMBER           1
#define RPP_OFFSET_TYPE                     2
#define RPP_OFFSET_DATALENGTH               3
#define RPP_OFFSET_DATASTART                4

/* If RPP is in use, send via RPP. Otherwise use plat_putByte()
 */
#ifdef HAVE_RPP
#define SEND_BYTE(data) rpp_sendBuffered(data)
#else
#define SEND_BYTE(data) plat_putByte(data)
#endif

/***************************************************************
 * Types
 **************************************************************/

/**
 * Message Types
 */
typedef enum PmRppType_e
{
    /** Invalid block type */
    RPP_T_INVALID = 0,

    /** Acknowledge */
    RPP_T_ACK = 1,

    /** Negative Acknowledge (checksum was wrong) */
    RPP_T_NACK = 2,

    /** Synchronization message */
    RPP_T_SYNC = 3,

    /** Data that is output via bytecode PRINT_ITEM */
    RPP_T_THREAD_PRINT_ITEM = 4,

    /** Data that is output via bytecode PRINT_EXPR */
    RPP_T_THREAD_PRINT_EXPR = 5,

    /** When sent to the target, requests the list of available threads.
     *  When sent from the target, carries a list of thread information of all
     *  available threads.
     */
    RPP_T_THREAD_LIST = 6,

    /** When sent to the target, requests the list of available memory spaces.
     *  When sent from the target, carries a list of tuples as eval'able text.
     */
    RPP_T_MEM_LIST = 7,

    /**
     * Sent by the desktop to append a block of data to a memory space. If
     * the message is overlong, the first two bytes of the first segment
     * contain a 16bit-number indicating the total amount of bytes to be
     * received. This is followed by a byte indicating the memory space.
     * Following bytes are data to be stored.   
     */
    RPP_T_MEM_ADD_BLOCK = 8,

    /**
     * When received with an empty data field, answer with the autorun list.
     * When received with a non-empty data field, overwrite the autorun list
     * with the data. The desktop sends a single zero character to erase the
     * list.
     */
    RPP_T_AUTORUN_LIST = 9,

    /**
     * When received with the name of a module, a thread is started from that
     * module.
     */
    RPP_T_THREAD_START = 10,

    /** Unused type to mark the upper limit of message type numbers */
    RPP_T_LAST
} PmRppType_t, *pPmRppType_t;


/**
 * Used to signal received message and status message send request
 * between receive routine and interpreter loop hooks.
 *
 * NOTE that this might be written to inside interrupt routines, so guarding
 * writes or even reads to such struct might be neccessary.
 */
typedef struct PmRppFlags_s
{
    /**
     * Set by receive routine to have handler hook trigger message receive
     * handler. While this is non-zero, received bytes will be ignored.
     */
    uint8_t messageReceiveComplete:1;

    /**
     * Set by receive routine to have handler hook send a NACK packet. Is
     * set if complete message with wrong checksum is received.
     * While this is non-zero, received bytes will be ignored.
     */
    uint8_t sendNACK:1;

} PmRppFlags_t,
 *pPmRppFlags_t;

typedef struct PmRppState_s
{
    /**
     * Set if another message was correctly received before. If the
     * connection was reset (partner signalled reset), this flag will be
     * reset too.
     */
    uint8_t sequenceNumberKnown:1;

    /**
     * The last sequence number received. Invalid data in here of
     * sequenceNumberKnown is not set.
     */
    uint8_t lastSequenceNumber;

    /**
     * The next sequence number to be sent.
     */
    uint8_t nextSequenceNumber;

    /**
     * Set when the ACK following the synchronization message with reset=1
     * is received. Until then, no other messages are processed.
     */
    uint8_t synchronizationOk:1;

    /**
     * Last system timestamp the synchronization message was sent. Used to
     * resend the message until an ACK is received. Unused after
     * synchronizationOk is set.
     */
    uint32_t lastSychronizationSendTimestamp;

    uint16_t receiveCrc;

    /**
     * This is incrementally updated on each plat_putByte() call. It is reset
     * when rpp_send_preamble() starts a new message.
     */
    uint16_t sendCrc;

    /**
     * The type of the last started buffered send operation. Is used to start
     * subsequent messages while sending buffered.
     */
    PmRppType_t bufferedSendType;

    /**
     * Amount of bytes that are waiting in rpp_send_buffer to be sent.
     */
    uint8_t bufferedSendPos;
    
} PmRppState_t,
 *pPmRppState_t;

/***************************************************************
 * Prototypes
 **************************************************************/

/**
 * Should be called for each incoming byte. Checks for complete message and
 * signals handling of the message. Data received before message is handled will
 * be ignored.
 */
PmReturn_t rpp_handleIncomingByte(uint8_t data);

/**
 * Sends an ACK message.
 */
PmReturn_t rpp_sendACK(void);

/**
 * Sends a NACK message and resets the rpp_flags.messageReceiveComplete flag in
 * an atomic manner.
 */
PmReturn_t rpp_sendNACK(void);

/**
 * Checks if it is time to send the synchronization message again. Should be
 * called periodically until rpp_state.synchronizationOk is set.
 */
PmReturn_t rpp_sendSychronizationHook(void);

/**
 * Assemble and send a sync message.
 */
PmReturn_t rpp_sendMessageSync(void);

/**
 * Start a new message and send start symbol, sequence number and type.
 * Also resets the sending CRC first.
 */
PmReturn_t rpp_sendPreambel(PmRppType_t type);

/**
 * Finish sending of a message by sending the CRC field.
 */
PmReturn_t rpp_sendCrc(void);

/**
 * Called to update the incremental crc for an outgoing message. Needs to be
 * called by any platform's putByte-function on sending a byte.
 */
void rpp_updateSendCrc(uint8_t data);

/**
 * Start sending a message with type.
 */
PmReturn_t rpp_sendBufferedStart(PmRppType_t type);

/**
 * Buffer data for sending. If the send buffer is full, it is flushed by sending
 * out all data and a new message is started too.
 */
PmReturn_t rpp_sendBuffered(uint8_t data);

/**
 * Flush the current buffer by sending the waiting data.
 */
PmReturn_t rpp_sendBufferedFlush(void);

/**
 * Complete sending of the message.
 */
PmReturn_t rpp_sendBufferedFinish(void);

/**
 * Reset the buffer position to empty, reset checksum.
 */
void rpp_resetBuffer(void);

#if defined(TARGET_AVR)
/**
 * Send a string from program memory via buffered sending.
 * Only available on AVR.
 */
PmReturn_t rpp_sendBufferedString_P(const char* string);
#endif

/**
 * Send a string from program memory via buffered sending.
 */
PmReturn_t rpp_sendBufferedString(uint8_t* string);

/***************************************************************
 * Globals
 **************************************************************/

extern volatile PmRppFlags_t rpp_flags;
extern uint8_t  rpp_receive_buffer[RPP_MESSAGE_BUFFER_SIZE];
extern PmRppState_t rpp_state;
extern PmRppType_t rpp_last_message_type;
#endif /* RPP_H_ */
