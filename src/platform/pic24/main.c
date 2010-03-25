/*
# This file is Copyright 2007, 2009 Dean Hall.
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

/** @file
 *  @brief Initialize then run the Python VM on power-up.
 */


#include "pm.h"
#include "pic24_all.h"

extern unsigned char usrlib_img[];

int main(void)
{
    PmReturn_t retval;
    uint16_t u16_i;

    retval = pm_init(MEMSPACE_PROG, usrlib_img);
    printf("Python initialized; result was 0x%02x.\n", retval);
    PM_RETURN_IF_ERROR(retval);
    for (u16_i = 0; u16_i < NUM_DIGITAL_PORTS; u16_i++) {
      printf("Port %c = %04x, OD = %04x.\n", u16_i + 'A', u16_digitalPinPresent[u16_i],
        u16_digitalPinOpenDrainPresent[u16_i]);
    }

    printf("Running Python...\n");
    retval = pm_run((uint8_t *)"main");

    printf("\n\nPython finished, return of 0x%02x.\nResetting...\n\n", retval);
    // Wait for characters to finish printing before reset
    while (!IS_TRANSMIT_COMPLETE_UART1()) doHeartbeat();
    asm("reset");

    return (int)retval;
}
