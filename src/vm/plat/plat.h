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
 * 2007/01/10   Added time tick service for desktop (POSIX) and AVR.
 * 2006/12/26   #65: Create plat module with put and get routines
 */

#ifndef PLAT_H_
#define PLAT_H_

/***************************************************************
 * Prototypes
 **************************************************************/

/**
 * Initializes the platform as needed by the routines
 * in the platform implementation file.
 */
PmReturn_t plat_init(void);


/**
 * Receives one byte from the default connection,
 * usually UART0 on a target device or stdio on the desktop
 */
PmReturn_t plat_getByte(uint8_t *b);


/**
 * Sends one byte out on the default connection,
 * usually UART0 on a target device or stdio on the desktop
 */
PmReturn_t plat_putByte(uint8_t b);

/**
 * Get the number of timer ticks that have passed since system start.
 * PLAT_TIME_PER_TICK_USEC is the number of full microseconds that
 * pass between two timer ticks.
 * On embedded targets, this operatoin is made atomic by disabling the
 * interrupts. It unconditionally enables them afterwards!
 */
uint32_t plat_getTicks(void);
#ifdef TARGET_AVR

/* PORT BEGIN: Adapt to the timer configured in avr.c:plat_init() */
/* Hint: 1,000,000 µs/s * 256 T/C0 clock cycles per tick * 8 CPU clocks per
 * T/C0 clock cycle / x,000,000 CPU clock cycles per second -> µs per tick
 */
#define PLAT_TIME_PER_TICK_USEC (1000000ULL*256ULL*8ULL/F_CPU)
/* PORT END */

#else /* !TARGET_AVR */

extern uint32_t plat_timePerTickUsec;
#define PLAT_TIME_PER_TICK_USEC (plat_timePerTickUsec)

#endif /* !TARGET_AVR */

/**
 * Can be used to check if enough time has passed to switch threads.
 * Depends on THREAD_RESCHEDULE_FREQUENCY to be set.
 * On targets without timer interrupt (like desktop), this will calculate
 * on demand.
 */
#ifdef TARGET_AVR

extern volatile uint8_t plat_switchThreads;
#define PLAT_SWITCH_THREADS plat_switchThreads
#define THREAD_SWITCH_TICKS ((1000000UL/THREAD_RESCHEDULE_FREQUENCY) \
		/PLAT_TIME_PER_TICK_USEC)

#else /* !TARGET_AVR */

uint8_t plat_switchThreads(void);
#define PLAT_SWITCH_THREADS plat_switchThreads()
#endif /* !TARGET_AVR */

#endif /* PLAT_H_ */
