/*
# This file is Copyright 2006 Dean Hall.
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


/** @file
 *  @brief PyMite platform-specific routines for a PIC24/dsPIC33 target 
 */


#include <stdio.h>
#include <pic24_all.h>
#include "dataXferImpl.h"
#include "pm.h"

/** Number of milliseconds since the system
 *  was initialized.
 */
volatile uint32_t u32_ms = 0;

/** The number of milliseconds between timer interrupts. */
#define ISR_PERIOD  1    // in ms

/** Interrupt Service Routine for Timer2.
 *  Receives one interrupts per \ref ISR_PERIOD milliseconds.
 */
void _ISR
_T1Interrupt (void) {
    PmReturn_t retval;

    u32_ms++;
    _T1IF = 0;                 //clear the timer interrupt bit
    retval = pm_vmPeriodic(ISR_PERIOD * 1000);
    PM_REPORT_IF_ERROR(retval);
}

/** Configure timer 2 to produce interrupts every \ref ISR_PERIOD ms. */
static void
configTimer1(void) {
  // Configure the timer
  T1CON = T1_OFF | T1_IDLE_CON | T1_GATE_OFF
          | T1_SOURCE_INT
          | T1_PS_1_1;
  // Subtract 1 from ticks value assigned to PR2 because period is PRx + 1
  PR1 = msToU16Ticks(ISR_PERIOD, getTimerPrescale(T1CONbits)) - 1;
  TMR1  = 0;                       //clear timer2 value
  _T1IF = 0;                       //clear interrupt flag
  _T1IP = 1;                       //choose a priority
  _T1IE = 1;                       //enable the interrupt
  T1CONbits.TON = 1;               //turn on the timer
}


PmReturn_t plat_init(void)
{
  configBasic(HELLO_MSG);
  configTimer1();
  
  return PM_RET_OK;
}


PmReturn_t
plat_deinit(void)
{
    // Disable timer interrupts
    _T1IE = 0;

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

  // Check for interrupt-driven receive and if so, read from the
  // appropriate buffer.
  switch (__C30_UART) {
#if (NUM_UART_MODS >= 1) && (UART1_RX_INTERRUPT)
    case 1 :
        *b = inChar1();
        return retval;
#endif
#if (NUM_UART_MODS >= 2) && (UART2_RX_INTERRUPT)
    case 2 :
        *b = inChar2();
        return retval;
#endif
#if (NUM_UART_MODS >= 3) && (UART3_RX_INTERRUPT)
    case 3 :
        *b = inChar3();
        return retval;
#endif
#if (NUM_UART_MODS >= 4) && (UART4_RX_INTERRUPT)
    case 4 :
        *b = inChar4();
        return retval;
#endif
    }

/*  This values gives the number of words between UART SFR registers.
 *  For example, &U1MODE = 0x0220 and &U2MODE = 0x230, a difference of
 *  eight words. Per comments on \ref IO_PORT_CONTROL_OFFSET, this
 *  can't be defined as a static value, requiring a hand look-up.
 */
#define UART_SFR_SPACING 8

    // If we got here, then there's no interrupt-driven receive
    // for the selected port, or the port is invalid.
    C_ASSERT( (__C30_UART <= NUM_UART_MODS) && (__C30_UART > 0) );

    // Get a pointer to the desired UART status and receive registers
    volatile UxSTABITS* pUxSTABits = (UxSTABITS*) (&U1STA + (__C30_UART - 1)*UART_SFR_SPACING);
    volatile uint16_t* pUxRXREG = &U1RXREG + (__C30_UART - 1)*UART_SFR_SPACING;

    /* Wait for a character to be ready (URXDA, UART receive data available). */
    while (!pUxSTABits->URXDA)
        doHeartbeat();

    /* Return errors for parity error, framing error or overrun */
    if (pUxSTABits->PERR || pUxSTABits->FERR || pUxSTABits->OERR) {
        PM_RAISE(retval, PM_RET_EX_IO);
        return retval;
    }
    *b = *pUxRXREG;

    return retval;
}


/*
 * UART send char routine MUST send exactly and only the given char;
 * it should not translate \n to \r\n.
 * This is because the interactive interface uses binary transfers.
 */
PmReturn_t
plat_putByte(uint8_t u8_b)
{
  outChar(u8_b);
  // For the data transfer protocol, automatically escape outgoing
  // chars.
  if (u8_b == ((uint8_t) CMD_TOKEN))
    outChar(ESCAPED_CMD);
  return PM_RET_OK;
}


/** Return the number of milliseconds since the system
 *  was initialized.
 */
PmReturn_t
plat_getMsTicks(uint32_t *r_ticks)
{
    *r_ticks = u32_ms;
    return PM_RET_OK;
}

void
plat_reportError(PmReturn_t result)
{

#ifdef HAVE_DEBUG_INFO
#define LEN_FNLOOKUP 26
#define LEN_EXNLOOKUP 18

    uint8_t res;
    pPmFrame_t pframe;
    pPmObj_t pstr;
    PmReturn_t retval;
    uint16_t bcindex;
    uint16_t bcsum;
    uint16_t linesum;
    uint16_t len_lnotab;
    uint8_t const *plnotab;
    uint16_t i;

    /* This table should match src/vm/fileid.txt */
    char const * const fnlookup[LEN_FNLOOKUP] = {
        "<no file>",
        "codeobj.c",
        "dict.c",
        "frame.c",
        "func.c",
        "global.c",
        "heap.c",
        "img.c",
        "int.c",
        "interp.c",
        "pmstdlib_nat.c",
        "list.c",
        "main.c",
        "mem.c",
        "module.c",
        "obj.c",
        "seglist.c",
        "sli.c",
        "strobj.c",
        "tuple.c",
        "seq.c",
        "pm.c",
        "thread.c",
        "float.c",
        "class.c",
        "bytearray.c",
    };

    /* This table should match src/vm/pm.h PmReturn_t */
    char const * const exnlookup[LEN_EXNLOOKUP] = {
        "Exception",
        "SystemExit",
        "IoError",
        "ZeroDivisionError",
        "AssertionError",
        "AttributeError",
        "ImportError",
        "IndexError",
        "KeyError",
        "MemoryError",
        "NameError",
        "SyntaxError",
        "SystemError",
        "TypeError",
        "ValueError",
        "StopIteration",
        "Warning",
        "OverflowError",
    };

    /* Print traceback */
    printf("Traceback (most recent call first):\n");

    /* Get the top frame */
    pframe = gVmGlobal.pthread->pframe;

    /* If it's the native frame, print the native function name */
    if (pframe == (pPmFrame_t)&(gVmGlobal.nativeframe))
    {

        /* The last name in the names tuple of the code obj is the name */
        retval = tuple_getItem((pPmObj_t)gVmGlobal.nativeframe.nf_func->
                               f_co->co_names, -1, &pstr);
        if ((retval) != PM_RET_OK)
        {
            printf("  Unable to get native func name.\n");
            return;
        }
        else
        {
            printf("  %s() __NATIVE__\n", ((pPmString_t)pstr)->val);
        }

        /* Get the frame that called the native frame */
        pframe = (pPmFrame_t)gVmGlobal.nativeframe.nf_back;
    }

    /* Print the remaining frame stack */
    for (; pframe != C_NULL; pframe = pframe->fo_back)
    {
        /* The last name in the names tuple of the code obj is the name */
        retval = tuple_getItem((pPmObj_t)pframe->fo_func->f_co->co_names,
                               -1,
                               &pstr);
        if ((retval) != PM_RET_OK) break;

        /*
         * Get the line number of the current bytecode. Algorithm comes from:
         * http://svn.python.org/view/python/trunk/Objects/lnotab_notes.txt?view=markup
         */
        bcindex = pframe->fo_ip - pframe->fo_func->f_co->co_codeaddr;
        plnotab = pframe->fo_func->f_co->co_lnotab;
        len_lnotab = mem_getWord(MEMSPACE_PROG, &plnotab);
        bcsum = 0;
        linesum = pframe->fo_func->f_co->co_firstlineno;
        for (i = 0; i < len_lnotab; i += 2)
        {
            bcsum += mem_getByte(MEMSPACE_PROG, &plnotab);
            if (bcsum > bcindex) break;
            linesum += mem_getByte(MEMSPACE_PROG, &plnotab);
        }
        printf("  File \"%s\", line %d, in %s\n",
               ((pPmFrame_t)pframe)->fo_func->f_co->co_filename,
               linesum,
               ((pPmString_t)pstr)->val);
    }

    /* Print error */
    res = (uint8_t)result;
    if ((res > 0) && ((res - PM_RET_EX) < LEN_EXNLOOKUP))
    {
        printf("%s", exnlookup[res - PM_RET_EX]);
    }
    else
    {
        printf("Error code 0x%02X", result);
    }
    printf(" detected by ");

    if ((gVmGlobal.errFileId > 0) && (gVmGlobal.errFileId < LEN_FNLOOKUP))
    {
        printf("%s:", fnlookup[gVmGlobal.errFileId]);
    }
    else
    {
        printf("FileId 0x%02X line ", gVmGlobal.errFileId);
    }
    printf("%d\n", gVmGlobal.errLineNum);

#else /* HAVE_DEBUG_INFO */

    /* Print error */
    printf("Error:     0x%02X\n", result);
    printf("  Release: 0x%02X\n", gVmGlobal.errVmRelease);
    printf("  FileId:  0x%02X\n", gVmGlobal.errFileId);
    printf("  LineNum: %d\n", gVmGlobal.errLineNum);

    /* Print traceback */
    {
        pPmObj_t pframe;
        pPmObj_t pstr;
        PmReturn_t retval;

        printf("Traceback (top first):\n");

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
                printf("  Unable to get native func name.\n");
                return;
            }
            else
            {
                printf("  %s() __NATIVE__\n", ((pPmString_t)pstr)->val);
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

            printf("  %s()\n", ((pPmString_t)pstr)->val);
        }
        printf("  <module>.\n");
    }
#endif /* HAVE_DEBUG_INFO */
}


/** Very scary: v3.25 has some erroneous implementations in sprintf. To get the project to
 *  compile, I put the functions below in. A bit more info is at:
 *  http://www.microchip.com/forums/m535514.aspx
 *  Error messages which occur without this:
 *  c:/program files/microchip/mplabc30/v3.25/bin/bin/../../lib\libc-coff.a(snprintf_cdfFnopuxX.o)(.libc._snprintf_cdfFnopuxX+0x1c):fake: undefined reference to `assert'
 *  c:/program files/microchip/mplabc30/v3.25/bin/bin/../../lib\libc-coff.a(snprintf_cdfFnopuxX.o)(.libc._snprintf_cdfFnopuxX+0x20):fake: undefined reference to `alloc'
 *  c:/program files/microchip/mplabc30/v3.25/bin/bin/../../lib\libc-coff.a(snprintf.o)(.libc.snprintf+0x1c):fake: undefined reference to `assert'
 *  c:/program files/microchip/mplabc30/v3.25/bin/bin/../../lib\libc-coff.a(snprintf.o)(.libc.snprintf+0x20):fake: undefined reference to `alloc'
 *  As I understand it, this means these functions expect to call assert()
 *  (which is now a macro in the new library of 3.25) and alloc() (whatever that is) (!!!).
 *  Very scary. In the debugger, I can see that alloc is called; however, ASSERT(0) isn't called.
 *  Junk strings get printed.
 *
 *  Conclusion: snprintf is broken in 3.25 beyond my ability to fix it.
 */
#if __C30_VERSION__ == 325
void
assert(int i_expr)
{
   ASSERT(i_expr);
}

#include <stdlib.h>
void*
alloc(size_t s)
{
   return malloc(s);
}
#error The v3.25 C30 compiler does not work with this program. Please use v3.24 instead.
#endif
