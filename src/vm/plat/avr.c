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
#define __FILE_ID__ 0x51

/**
 * PyMite platform-specific routines for AVR target
 *
 * Log
 * ---
 *
 * 2007/07/04   Introduce RPM/RPP
 * 2007/01/31   #86: Move platform-specific code to the platform impl file
 * 2007/01/10   #75: Added time tick service for desktop (POSIX) and AVR. (P.Adelt)
 * 2006/12/26   #65: Create plat module with put and get routines
 */


#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#ifdef HAVE_RPP
  #include <util/crc16.h>
#endif

#include "../pm.h"


/***************************************************************
 * Configuration
 **************************************************************/

/**
 * When defined, the AVR target configures Timer/Counter0 to generate an
 * overflow interrupt to call pm_vmPeriodic().
 * If you configure T/C0 yourself, disable this define and be sure to
 * periodically call pm_vmPeriodic(usec)!
 * Has no meaning on non-AVR.
 */
#define AVR_DEFAULT_TIMER_SOURCE


/***************************************************************
 * Constants
 **************************************************************/
 
#ifdef AVR_DEFAULT_TIMER_SOURCE

/* Hint: 1,000,000 µs/s * 256 T/C0 clock cycles per tick * 8 CPU clocks per
 * T/C0 clock cycle / x,000,000 CPU clock cycles per second -> µs per tick
 */
#define PLAT_TIME_PER_TICK_USEC (1000000ULL*256ULL*8ULL/F_CPU)

#endif /* AVR_DEFAULT_TIMER_SOURCE */

/***************************************************************
 * Globals
 **************************************************************/

uint8_t plat_avr_sreg_i;

/***************************************************************
 * Functions
 **************************************************************/


/*
 * AVR target shall use stdio for I/O routines.
 * The UART or USART must be configured for the interactive interface to work.
 */
PmReturn_t
plat_init(void)
{
    /* PORT BEGIN: Set these UART/USART SFRs properly for your AVR */

    /* Set the baud rate register */
    UBRR0 = (F_CPU / (16UL * UART_BAUD)) - 1;

    /* Enable the transmit and receive pins */
#ifdef __AVR_AT90CAN128__
    UCSR0B = _BV(TXEN0) | _BV(RXEN0) | _BV(RXCIE0);
#else
    UCSR0A = _BV(TXEN) | _BV(RXEN);
#endif
    /* PORT END */

#ifdef AVR_DEFAULT_TIMER_SOURCE
    /* PORT BEGIN: Configure a timer that fits your needs. */
    /* Use T/C0 in synchronous mode, aim for a tick rate of
     * several hundred Hz */
#if __AVR_ATmega103__ || __AVR_ATmega128__
    /* set T/C0 to use synchronous clock */
    ASSR &= ~(1<<AS0);
    /* set prescaler to /8 */
    TCCR0 &= ~0x07;
    TCCR0 |= (1<<CS01);
#elif __AVR_AT90CAN128__
    /* set prescaler to /8 */
    TCCR0A &= ~0x07;
    TCCR0A |= (1<<CS01);
#else
#error No timer configuration is implemented for this AVR.
#endif
    TIMSK0 |= 1<<TOIE0;
#endif /* AVR_DEFAULT_TIMER_SOURCE */
    /* PORT END */
    
    /* Enable interrupts. */
    sei();

    return PM_RET_OK;
}

#ifdef AVR_DEFAULT_TIMER_SOURCE
ISR(TIMER0_OVF_vect)
{
    /* TODO Find a clever way to handle bad return code, maybe use
     * PM_REPORT_IF_ERROR(retval) when that works on AVR inside an
     * interrupt.
     */
    pm_vmPeriodic(PLAT_TIME_PER_TICK_USEC);
}
#endif


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
            b = **paddr;
            *paddr += 1;
            return b;

        case MEMSPACE_PROG:
            b = pgm_read_byte(*paddr);
            *paddr += 1;
            return b;

        case MEMSPACE_EEPROM:
            b = eeprom_read_byte(*paddr);
            *paddr += 1;
            return b;

        case MEMSPACE_SEEPROM:
        case MEMSPACE_OTHER0:
        case MEMSPACE_OTHER1:
        case MEMSPACE_OTHER2:
        case MEMSPACE_FILE:
        default:
            return 0;
    }
}


/*
 * UART receive char routine MUST return exactly and only the received char;
 * it should not translate \n to \r\n.
 * This is because the interactive interface uses binary transfers.
 */
PmReturn_t
plat_getByte(uint8_t *b)
{
    PmReturn_t retval = PM_RET_OK;

    /* PORT BEGIN: Set these UART/USART SFRs properly for your AVR */
    /* Loop until serial receive is complete */
    loop_until_bit_is_set(UCSR0A, RXC0);

    /* If a framing error or data overrun occur, raise an IOException */
    if (UCSR0A & (_BV(FE) | _BV(DOR)))
    {
        PM_RAISE(retval, PM_RET_EX_IO);
        return retval;
    }
    *b = UDR0;
    /* PORT END */

    return retval;
}


/*
 * UART send char routine MUST send exactly and only the given char;
 * it should not translate \n to \r\n.
 * This is because the interactive interface uses binary transfers.
 */
PmReturn_t
plat_putByte(uint8_t b)
{
    /* PORT BEGIN: Set these UART/USART SFRs properly for your AVR */
    /* Loop until serial data reg is empty (from previous transfer) */
    loop_until_bit_is_set(UCSR0A, UDRE);

    /* Put the byte to send into the serial data register */
    UDR0 = b;
    /* PORT END */

    #ifdef HAVE_RPP
        rpp_updateSendCrc(b);
    #endif

    return PM_RET_OK;
}


/*
 * This operation is made atomic by temporarily disabling
 * the interrupts. The old state is restored afterwards.
 */
PmReturn_t
plat_getMsTicks(uint32_t *r_ticks)
{
    plat_enterCriticalSection();
    *r_ticks = pm_timerMsTicks;
    plat_exitCriticalSection();
    return PM_RET_OK;
}


void
plat_reportError(PmReturn_t result)
{
    /* TODO #97: Implement plat_reportError for AVR */
}

void
plat_enterCriticalSection(void)
{
    plat_avr_sreg_i = SREG & (1<<SREG_I);
    cli();
}

/* Reenable Interrupts if they have been turned off when entering. */
void plat_exitCriticalSection(void)
{
    if (plat_avr_sreg_i)
    {
        sei();
    }
}

#ifdef HAVE_RPP

PmReturn_t
plat_memSetByte(PmMemSpace_t memspace, uint8_t *paddr, uint8_t data)
{
    switch (memspace)
    {
        case MEMSPACE_EEPROM:
            eeprom_write_byte (paddr, data);
            break;
        case MEMSPACE_PROG:
            /* Program memory not available as user storage. */
        case MEMSPACE_RAM:
            /* RAM not available as user storage. */
        case MEMSPACE_SEEPROM:
        case MEMSPACE_OTHER0:
        case MEMSPACE_OTHER1:
        case MEMSPACE_OTHER2:
        case MEMSPACE_FILE:
        default:
            return PM_RET_ERR;
    }
    return PM_RET_OK;
}

PmReturn_t
plat_memGetInfo(PmMemSpace_t memspace, uint16_t *psize, uint8_t *pwriteable)
{
    switch (memspace)
    {
        case MEMSPACE_EEPROM:
            #if __AVR_ATmega128__ || __AVR_AT90CAN128__
            /* Note: There are lots of other AVR parts with 4 kByte EEPROM. */
            *psize = 4096;
            #else
            /* Seems like all AVRs that could run PyMite offer at least
             * 2 kByte EEPROM.
             */
            *psize = 2048;
            #endif
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
        case MEMSPACE_SEEPROM:
        case MEMSPACE_OTHER0:
        case MEMSPACE_OTHER1:
        case MEMSPACE_OTHER2:
        case MEMSPACE_FILE:
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
            rpp_sendBufferedString_P(PSTR(MEM_NAME_FILE));
            break;
        case MEMSPACE_PROG:
            rpp_sendBufferedString_P(PSTR(MEM_NAME_PROG));
            break;
        case MEMSPACE_RAM:
            rpp_sendBufferedString_P(PSTR(MEM_NAME_RAM));
            break;
        case MEMSPACE_EEPROM:
            rpp_sendBufferedString_P(PSTR(MEM_NAME_EEPROM));
            break;
        case MEMSPACE_SEEPROM:
            rpp_sendBufferedString_P(PSTR(MEM_NAME_SEEPROM));
            break;
        case MEMSPACE_OTHER0:
            rpp_sendBufferedString_P(PSTR(MEM_NAME_OTHER0));
            break;
        case MEMSPACE_OTHER1:
            rpp_sendBufferedString_P(PSTR(MEM_NAME_OTHER1));
            break;
        case MEMSPACE_OTHER2:
            rpp_sendBufferedString_P(PSTR(MEM_NAME_OTHER2));
            break;
        default:
            /* MEMSPACE_LAST */
            break;
    }
    return PM_RET_OK;
}


/* PORT BEGIN: See rpp.c for other possibilities to receive RPP data. If
 * you change the way data is received, also visit avr.h and check if
 * PLAT_RECEIVE_BY_INTERRUPT needs to be undef'd. */
ISR(USART0_RX_vect) {
    rpp_handleIncomingByte(UDR0);
}
/* PORT END */
#endif /* HAVE_RPP */
