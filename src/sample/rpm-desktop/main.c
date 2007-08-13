/*
 * RPM/RPP: A PyMite component
 * PyMite - A flyweight Python interpreter for 8-bit microcontrollers and more.
 * Copyright 2007 Philipp Adelt
 * Based on ipm-desktop/main.c: Copyright 2002 Dean Hall
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
 * RPM desktop target capsule
 *
 * Log
 * ---
 *
 * 2007/07/13   Creation
 */


#include "pm.h"

#define __FILE_ID__ 0

extern unsigned char usrlib_img[];


int main(void)
{
    PmReturn_t retval;

    retval = pm_init(MEMSPACE_PROG, usrlib_img);
    PM_RETURN_IF_ERROR(retval);

    /* Just run the interpreter loop forever without a thread. This
     * will eat CPU...
     * TODO Stop this from hogging CPU cycles
     */
    
    retval = pm_addThread((uint8_t*)"main"); 
    PM_RETURN_IF_ERROR(retval);

/*    retval = pm_addAutorunThreads();
    PM_RETURN_IF_ERROR(retval);
*/    
    retval = interpret(INTERP_LOOP_FOREVER);

    return (int)retval;
}
