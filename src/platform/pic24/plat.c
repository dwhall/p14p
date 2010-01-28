/*
# This file is Copyright 2006, 2007, 2009 Dean Hall.
#
# This file is part of the Python-on-a-Chip program.
# Python-on-a-Chip is free software: you can redistribute it and/or modify
# it under the terms of the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1.
#
# Python-on-a-Chip is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# A copy of the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1
# is seen in the file COPYING up one directory from this.
*/


#undef __FILE_ID__
#define __FILE_ID__ 0x51


/** PyMite platform-specific routines for AVR target */


#include <stdio.h>
#include <include/pic24_all.h>

#include "pm.h"


PmReturn_t plat_init(void)
{
  configBasic(HELLO_MSG);
  return PM_RET_OK;
}


PmReturn_t
plat_deinit(void)
{

    return PM_RET_OK;
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
            b = **paddr;
            *paddr += 1;
            return b;

        /* For now, assume a large memory model, so all program memory
         * can be mapped into the PSV space. */
        case MEMSPACE_PROG:
//            printf("Flash data at %04x is %02x.\n", *paddr, **paddr);
//            if (*paddr < ((uint8_t*) 0x8000)) {
//              printf("Crashing now...\n");
//            }
            b = **paddr;
            *paddr += 1;
            return b;

        case MEMSPACE_EEPROM:  // Not yet implemented.
        case MEMSPACE_SEEPROM:
        case MEMSPACE_OTHER0:
        case MEMSPACE_OTHER1:
        case MEMSPACE_OTHER2:
        case MEMSPACE_OTHER3:
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

  /* Wait for character */
  while (!isCharReady1()) doHeartbeat();

  /* Return errors for Framing error or Overrun */
  if (U1STAbits.PERR || U1STAbits.FERR || U1STAbits.OERR) {
    PM_RAISE(retval, PM_RET_EX_IO);
    return retval;
  }
  *b = U1RXREG;

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
  outChar1(b);
  return PM_RET_OK;
}


/*
 * This operation is made atomic by temporarily disabling
 * the interrupts. The old state is restored afterwards.
 */
PmReturn_t
plat_getMsTicks(uint32_t *r_ticks)
{
    *r_ticks = 0;
    return PM_RET_OK;
}

#define PSTR(x) x
#define printf_P printf
#define puts_P puts

void
plat_reportError(PmReturn_t result)
{
    /* Print error */
    printf_P(PSTR("Error:     0x%02X\n"), result);
    printf_P(PSTR("  Release: 0x%02X\n"), gVmGlobal.errVmRelease);
    printf_P(PSTR("  FileId:  0x%02X\n"), gVmGlobal.errFileId);
    printf_P(PSTR("  LineNum: %d\n"), gVmGlobal.errLineNum);

    /* Print traceback */
    {
        pPmObj_t pframe;
        pPmObj_t pstr;
        PmReturn_t retval;

        puts_P(PSTR("Traceback (top first):"));

        /* Get the top frame */
        pframe = (pPmObj_t)gVmGlobal.pthread->pframe;

        /* If it's the native frame, print the native function name */
        if (pframe == (pPmObj_t)&(gVmGlobal.nativeframe))
        {

            /* The last name in the names tuple of the code obj is the name */
            retval = tuple_getItem((pPmObj_t)gVmGlobal.nativeframe.nf_func->
                                   f_co->co_names, -1, &pstr);
            if ((retval) != PM_RET_OK)
            {
                puts_P(PSTR("  Unable to get native func name."));
                return;
            }
            else
            {
                printf_P(PSTR("  %s() __NATIVE__\n"), ((pPmString_t)pstr)->val);
            }

            /* Get the frame that called the native frame */
            pframe = (pPmObj_t)gVmGlobal.nativeframe.nf_back;
        }

        /* Print the remaining frame stack */
        for (;
             pframe != C_NULL;
             pframe = (pPmObj_t)((pPmFrame_t)pframe)->fo_back)
        {
            /* The last name in the names tuple of the code obj is the name */
            retval = tuple_getItem((pPmObj_t)((pPmFrame_t)pframe)->
                                   fo_func->f_co->co_names, -1, &pstr);
            if ((retval) != PM_RET_OK) break;

            printf_P(PSTR("  %s()\n"), ((pPmString_t)pstr)->val);
        }
        puts_P(PSTR("  <module>."));
    }
}
