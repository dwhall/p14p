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
 * 2007/01/10   Added time tick service for desktop (POSIX) and AVR. (P.Adelt)
 * 2006/12/26   #65: Create plat module with put and get routines
 */

#include <stdio.h>
#define __USE_POSIX199309
#include <time.h>
#include "../pm.h"

/***************************************************************
 * Globals
 **************************************************************/

struct timespec plat_starttime;
uint32_t plat_timePerTickUsec;
uint32_t plat_lastThreadSwitchTicks = 0;
/** amount of ticks to pass between two thread switches */
uint32_t plat_threadSwitchTicks;

 
/***************************************************************
 * Functions
 **************************************************************/

/* Desktop target shall use stdio for I/O routines.
 * Init timer resolution. */
PmReturn_t
plat_init(void)
{
	struct timespec timeres;
	clock_getres(CLOCK_REALTIME, &timeres);
	plat_timePerTickUsec = (1000000UL*timeres.tv_sec)+(timeres.tv_nsec/1000);
	 
	clock_gettime(CLOCK_REALTIME, &plat_starttime);
	
	plat_threadSwitchTicks = ((1000000UL/THREAD_RESCHEDULE_FREQUENCY)
		/PLAT_TIME_PER_TICK_USEC);
    return PM_RET_OK;
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

    if ((i != b) || (i == EOF))
    {
        PM_RAISE(retval, PM_RET_EX_IO);
    }

    return retval;
}

uint32_t plat_getTicks(void)
{
	struct timespec currtime;
	clock_gettime(CLOCK_REALTIME, &currtime);
	return ((unsigned long long)(currtime.tv_sec-plat_starttime.tv_sec))
		*1000000ULL+(unsigned long long)((currtime.tv_nsec-plat_starttime.tv_nsec)/1000)
		/ plat_timePerTickUsec;
}

uint8_t plat_switchThreads(void)
{
	uint32_t currentTicks = plat_getTicks();
	
	if (currentTicks % plat_threadSwitchTicks == 0) {
		plat_lastThreadSwitchTicks = currentTicks;
		return 1;
	}
	
	return 0;

}
