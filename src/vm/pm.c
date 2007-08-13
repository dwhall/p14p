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
#define __FILE_ID__ 0x15

/**
 * PyMite User API
 *
 * High-level functions to initialize and run PyMite
 *
 * Log
 * ---
 *
 * 2007/01/09   #75: Refactored for green thread support (P.Adelt)
 * 2006/09/16   #16: Create pm_init() that does the initial housekeeping
 */

#include "pm.h"


/** Number of millisecond-ticks to pass before scheduler is run */
#define PM_THREAD_TIMESLICE_MS  10

extern unsigned char stdlib_img[];

/* Stores the timer millisecond-ticks since system start */
volatile uint32_t pm_timerMsTicks = 0;

/* Stores tick timestamp of last scheduler run */
volatile uint32_t pm_lastRescheduleTimestamp = 0;

PmReturn_t
pm_init(PmMemSpace_t memspace, uint8_t *pusrimg)
{
    PmReturn_t retval;
    uint8_t const *pimg;

    /* Initialize the hardware platform */
    retval = plat_init();
    PM_RETURN_IF_ERROR(retval);

    /* Initialize the heap and the globals */
    retval = heap_init();
    PM_RETURN_IF_ERROR(retval);

    retval = global_init();
    PM_RETURN_IF_ERROR(retval);
    
    #ifdef HAVE_RPP
    retval = persist_init();
    PM_RETURN_IF_ERROR(retval);
    #endif /* HAVE_RPP */

    #ifdef HAVE_RPM
    retval = rpm_init();
    PM_RETURN_IF_ERROR(retval);
    #endif /* HAVE_RPM */

    /* Load std image info */
    pimg = (uint8_t *)&stdlib_img;
    retval = img_findInMem(MEMSPACE_PROG, &pimg);
    PM_RETURN_IF_ERROR(retval);
    /* Also look in target dependant non-volatile memory. */
    /* PORT BEGIN */
    pimg = (uint8_t const*)(PERSIST_OFFSET_DATA+RPM_AUTORUN_LIST_OFFSET+RPM_AUTORUN_LIST_LENGTH);
    #if defined(TARGET_DESKTOP)
    retval = img_findInMem(MEMSPACE_FILE, &pimg);
    PM_RETURN_IF_ERROR(retval);
    #elif defined(TARGET_AVR)
    retval = img_findInMem(MEMSPACE_EEPROM, &pimg);
    /* For now, there does not need to be a valid image at the beginning of the
     * EEPROM space.
     */
    /* PM_RETURN_IF_ERROR(retval); */
    #endif
    /* PORT END */

    /* Load usr image info if given */
    if (pusrimg != C_NULL)
    {
        pimg = pusrimg;
        retval = img_findInMem(memspace, &pimg);
    }

    return retval;
}

PmReturn_t
pm_addThread(uint8_t const *modstr)
{
    PmReturn_t retval;
    pPmObj_t pmod;
    pPmObj_t pstring;
    uint8_t const *pmodstr = modstr;

    /* Import module from global struct */
    retval = string_new(&pmodstr, &pstring);
    PM_RETURN_IF_ERROR(retval);
    retval = mod_import(pstring, &pmod);
    PM_RETURN_IF_ERROR(retval);

    /* Load builtins into thread */
    retval = global_setBuiltins((pPmFunc_t)pmod);
    PM_RETURN_IF_ERROR(retval);
    
    /* Add thread to the list. */
    retval = interp_addThread((pPmFunc_t)pmod);

    return retval;
}

PmReturn_t
pm_run(uint8_t const *modstr)
{
    PmReturn_t retval;
    
    retval = pm_addThread(modstr);
    PM_RETURN_IF_ERROR(retval);

    /* Interpret the module's bcode */
    retval = interpret(INTERP_RETURN_ON_NO_THREADS); 

    return retval;
}


/* Warning: Can be called in interrupt/signal context! */
PmReturn_t
pm_vmPeriodic(uint16_t usecsSinceLastCall)
{
    /*
     * Add the full milliseconds to pm_timerMsTicks and store additional
     * microseconds for the next run. Thus, usecsSinceLastCall must be
     * less than 2^16-1000 so it will not overflow usecResidual.
     */
    static uint16_t usecResidual = 0;

    C_ASSERT(usecsSinceLastCall < 64536);

    usecResidual += usecsSinceLastCall;
    while (usecResidual >= 1000)
    {
        usecResidual -= 1000;
        pm_timerMsTicks++;
    }

    /* Check if enough time has passed for a scheduler run */
    if ((pm_timerMsTicks - pm_lastRescheduleTimestamp)
        >= PM_THREAD_TIMESLICE_MS)
    {
        interp_setRescheduleFlag((uint8_t)1);
        pm_lastRescheduleTimestamp = pm_timerMsTicks;
    }
    return PM_RET_OK;
}

PmReturn_t
pm_addAutorunThreads(void)
{
    PmReturn_t retval = PM_RET_OK;
    uint8_t buffer[30];
    uint8_t c, i = 0;
    uint16_t bytesRead = 0;
    uint8_t const *paddr;
    
    /* Copy the next module name into buffer */
    paddr = (uint8_t const *)(unsigned int)(RPM_AUTORUN_LIST_OFFSET);
    c = persist_memGetByte(RPM_AUTORUN_LIST_MEMSPACE, &paddr);
    bytesRead++;
    while (bytesRead < RPM_AUTORUN_LIST_LENGTH)
    {
        if (c == 0) {
            buffer[i] = 0;
            if (i > 0)
            {
                retval = pm_addThread((uint8_t*)&buffer);
            }
            break;
        }
        else if (c == ',')
        {
            buffer[i] = 0;
            retval = pm_addThread((uint8_t*)&buffer);
            PM_RETURN_IF_ERROR(retval);
        }
        else
        {
            buffer[i] = c;
            i++;
        }
        c = persist_memGetByte(RPM_AUTORUN_LIST_MEMSPACE, &paddr);
        bytesRead++;
    }
    return retval;
}
