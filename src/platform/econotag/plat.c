/** @file
 * PyMite - A flyweight Python interpreter for 8-bit and larger microcontrollers.
 * Copyright 2002 Dean Hall.  All rights reserved.
 * PyMite is offered through one of two licenses: commercial or open-source.
 * See the LICENSE file at the root of this package for licensing details.
 *
 * some sections based on code (C) COPYRIGHT 2008 STMicroelectronics
 */

#undef __FILE_ID__
#define __FILE_ID__ 0x52

#include <stdio.h>

#include "pm.h"


PmReturn_t
plat_init(void)
{

#ifdef DEBUG
    debug();
#endif

    return PM_RET_OK;
}

int fputc(int ch, FILE *f)
{
    plat_putByte((uint8_t) ch);
    return ch;
}

/* TODO: disable the peripherals and interrupts */
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
	return 0;
}


PmReturn_t
plat_getByte(uint8_t *b)
{
	return 0;
}


PmReturn_t
plat_putByte(uint8_t b)
{
    return PM_RET_OK;
}


PmReturn_t
plat_getMsTicks(uint32_t *r_ticks)
{
    return PM_RET_OK;
}


void
plat_reportError(PmReturn_t result)
{

}