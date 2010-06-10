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
 *  @brief Run a series of PIC-specific unit tests.
 *
 *  Note that these test routines rely on local versions of CuTest.c and
 *  CuTest.h. This is because some of the defines in CuTest.h have been
 *  modified to accomodate a limited RAM space (e.g. original max string
 *  size was 8K, etc.).
 */


#include "pm.h"
#include "pic24_all.h"
#include "CuTestSmall.h"

/* Need to declare this array since no PyMite application is linked */
const unsigned char usr_nat_fxn_table[] = {};

// I don't understand why this is necessary, since the same prototype exists
// in pic24_pyports.h.
__STATIC__ bool_t digitalOpenDrainPinExists(uint16_t u16_port, uint16_t u16_pin);

/** A series of tests on the existance of digital IO pins.
  * @param tc Test object.
  */
void test_gpioExists(CuTest* tc)
{
    // The PIC24HJxxGP504 is a 44 pin chip with 3 IO ports
    CuAssertTrue(tc, NUM_DIGITAL_PORTS == 3);

    // It has remappable pins
#ifndef HAS_REMAPPABLE_PINS
    CuAssertTrue(tc, 0);
#endif

    // Spot check simple port / pin existance
    CuAssertTrue(tc, !digitalPinInBounds(PORT_A_INDEX, 16));
    CuAssertTrue(tc, !digitalPinInBounds(PORT_D_INDEX, 0));
    CuAssertTrue(tc,  digitalPinInBounds(PORT_C_INDEX, 15));

    // Check specific port / pin combos:
    // Port A
    CuAssertTrue(tc,  digitalPinExists(PORT_A_INDEX, 0));
    CuAssertTrue(tc,  digitalPinExists(PORT_A_INDEX, 1));
    CuAssertTrue(tc,  digitalPinExists(PORT_A_INDEX, 2));
    CuAssertTrue(tc,  digitalPinExists(PORT_A_INDEX, 3));
    CuAssertTrue(tc,  digitalPinExists(PORT_A_INDEX, 4));
    CuAssertTrue(tc, !digitalPinExists(PORT_A_INDEX, 5));
    CuAssertTrue(tc, !digitalPinExists(PORT_A_INDEX, 6));
    CuAssertTrue(tc,  digitalPinExists(PORT_A_INDEX, 7));
    CuAssertTrue(tc,  digitalPinExists(PORT_A_INDEX, 8));
    CuAssertTrue(tc,  digitalPinExists(PORT_A_INDEX, 9));
    CuAssertTrue(tc,  digitalPinExists(PORT_A_INDEX, 10));
    CuAssertTrue(tc, !digitalPinExists(PORT_A_INDEX, 11));
    CuAssertTrue(tc, !digitalPinExists(PORT_A_INDEX, 12));
    CuAssertTrue(tc, !digitalPinExists(PORT_A_INDEX, 13));
    CuAssertTrue(tc, !digitalPinExists(PORT_A_INDEX, 14));
    CuAssertTrue(tc, !digitalPinExists(PORT_A_INDEX, 15));
    // Port B
    CuAssertTrue(tc,  digitalPinExists(PORT_B_INDEX, 0));
    CuAssertTrue(tc,  digitalPinExists(PORT_B_INDEX, 1));
    CuAssertTrue(tc,  digitalPinExists(PORT_B_INDEX, 2));
    CuAssertTrue(tc,  digitalPinExists(PORT_B_INDEX, 3));
    CuAssertTrue(tc,  digitalPinExists(PORT_B_INDEX, 4));
    CuAssertTrue(tc,  digitalPinExists(PORT_B_INDEX, 5));
    CuAssertTrue(tc,  digitalPinExists(PORT_B_INDEX, 6));
    CuAssertTrue(tc,  digitalPinExists(PORT_B_INDEX, 7));
    CuAssertTrue(tc,  digitalPinExists(PORT_B_INDEX, 8));
    CuAssertTrue(tc,  digitalPinExists(PORT_B_INDEX, 9));
    CuAssertTrue(tc,  digitalPinExists(PORT_B_INDEX, 10));
    CuAssertTrue(tc,  digitalPinExists(PORT_B_INDEX, 11));
    CuAssertTrue(tc,  digitalPinExists(PORT_B_INDEX, 12));
    CuAssertTrue(tc,  digitalPinExists(PORT_B_INDEX, 13));
    CuAssertTrue(tc,  digitalPinExists(PORT_B_INDEX, 14));
    CuAssertTrue(tc,  digitalPinExists(PORT_B_INDEX, 15));
    // Port C
    CuAssertTrue(tc,  digitalPinExists(PORT_C_INDEX, 0));
    CuAssertTrue(tc,  digitalPinExists(PORT_C_INDEX, 1));
    CuAssertTrue(tc,  digitalPinExists(PORT_C_INDEX, 2));
    CuAssertTrue(tc,  digitalPinExists(PORT_C_INDEX, 3));
    CuAssertTrue(tc,  digitalPinExists(PORT_C_INDEX, 4));
    CuAssertTrue(tc,  digitalPinExists(PORT_C_INDEX, 5));
    CuAssertTrue(tc,  digitalPinExists(PORT_C_INDEX, 6));
    CuAssertTrue(tc,  digitalPinExists(PORT_C_INDEX, 7));
    CuAssertTrue(tc,  digitalPinExists(PORT_C_INDEX, 8));
    CuAssertTrue(tc,  digitalPinExists(PORT_C_INDEX, 9));
    CuAssertTrue(tc, !digitalPinExists(PORT_C_INDEX, 10));
    CuAssertTrue(tc, !digitalPinExists(PORT_C_INDEX, 11));
    CuAssertTrue(tc, !digitalPinExists(PORT_C_INDEX, 12));
    CuAssertTrue(tc, !digitalPinExists(PORT_C_INDEX, 13));
    CuAssertTrue(tc, !digitalPinExists(PORT_C_INDEX, 14));
    CuAssertTrue(tc, !digitalPinExists(PORT_C_INDEX, 15));
}

/** A series of tests on the existance of digital IO pins.
  * Note: Microchip header files define _ODCA0-4, but the data sheet
  * claims these don't exist. So, the test below fail. For now,
  * ignore this. The same is true of pins on ports B and C.
  *
  * It looks like the header files defined the ODC bits if the
  * corresponding LATx/Rx bits exist, which is incorrect.
  *
  * Therefore, these tell will always fail. Aargh.
  *
  * @param tc Test object.
  */
void test_gpioOdExists(CuTest* tc)
{
    // Test open-drain config
    // Port A
#if 0   // Test everything we can be commenting these out for now
    CuAssertTrue(tc, !digitalOpenDrainPinExists(PORT_A_INDEX, 0));
    CuAssertTrue(tc, !digitalOpenDrainPinExists(PORT_A_INDEX, 1));
    CuAssertTrue(tc, !digitalOpenDrainPinExists(PORT_A_INDEX, 2));
    CuAssertTrue(tc, !digitalOpenDrainPinExists(PORT_A_INDEX, 3));
    CuAssertTrue(tc, !digitalOpenDrainPinExists(PORT_A_INDEX, 4));
#endif
    CuAssertTrue(tc, !digitalOpenDrainPinExists(PORT_A_INDEX, 5));
    CuAssertTrue(tc, !digitalOpenDrainPinExists(PORT_A_INDEX, 6));
    CuAssertTrue(tc,  digitalOpenDrainPinExists(PORT_A_INDEX, 7));
    CuAssertTrue(tc,  digitalOpenDrainPinExists(PORT_A_INDEX, 8));
    CuAssertTrue(tc,  digitalOpenDrainPinExists(PORT_A_INDEX, 9));
    CuAssertTrue(tc,  digitalOpenDrainPinExists(PORT_A_INDEX, 10));
    CuAssertTrue(tc, !digitalOpenDrainPinExists(PORT_A_INDEX, 11));
    CuAssertTrue(tc, !digitalOpenDrainPinExists(PORT_A_INDEX, 12));
    CuAssertTrue(tc, !digitalOpenDrainPinExists(PORT_A_INDEX, 13));
    CuAssertTrue(tc, !digitalOpenDrainPinExists(PORT_A_INDEX, 14));
    CuAssertTrue(tc, !digitalOpenDrainPinExists(PORT_A_INDEX, 15));
    // Port B
#if 0
    CuAssertTrue(tc, !digitalOpenDrainPinExists(PORT_B_INDEX, 0));
    CuAssertTrue(tc, !digitalOpenDrainPinExists(PORT_B_INDEX, 1));
    CuAssertTrue(tc, !digitalOpenDrainPinExists(PORT_B_INDEX, 2));
    CuAssertTrue(tc, !digitalOpenDrainPinExists(PORT_B_INDEX, 3));
    CuAssertTrue(tc, !digitalOpenDrainPinExists(PORT_B_INDEX, 4));
#endif
    CuAssertTrue(tc,  digitalOpenDrainPinExists(PORT_B_INDEX, 5));
    CuAssertTrue(tc,  digitalOpenDrainPinExists(PORT_B_INDEX, 6));
    CuAssertTrue(tc,  digitalOpenDrainPinExists(PORT_B_INDEX, 7));
    CuAssertTrue(tc,  digitalOpenDrainPinExists(PORT_B_INDEX, 8));
    CuAssertTrue(tc,  digitalOpenDrainPinExists(PORT_B_INDEX, 9));
    CuAssertTrue(tc,  digitalOpenDrainPinExists(PORT_B_INDEX, 10));
    CuAssertTrue(tc,  digitalOpenDrainPinExists(PORT_B_INDEX, 11));
    CuAssertTrue(tc, !digitalOpenDrainPinExists(PORT_B_INDEX, 12));
    CuAssertTrue(tc, !digitalOpenDrainPinExists(PORT_B_INDEX, 13));
    CuAssertTrue(tc, !digitalOpenDrainPinExists(PORT_B_INDEX, 14));
    CuAssertTrue(tc, !digitalOpenDrainPinExists(PORT_B_INDEX, 15));
    // Port C
#if 0
    CuAssertTrue(tc, !digitalOpenDrainPinExists(PORT_C_INDEX, 0));
    CuAssertTrue(tc, !digitalOpenDrainPinExists(PORT_C_INDEX, 1));
    CuAssertTrue(tc, !digitalOpenDrainPinExists(PORT_C_INDEX, 2));
#endif
    CuAssertTrue(tc,  digitalOpenDrainPinExists(PORT_C_INDEX, 3));
    CuAssertTrue(tc,  digitalOpenDrainPinExists(PORT_C_INDEX, 4));
    CuAssertTrue(tc,  digitalOpenDrainPinExists(PORT_C_INDEX, 5));
    CuAssertTrue(tc,  digitalOpenDrainPinExists(PORT_C_INDEX, 6));
    CuAssertTrue(tc,  digitalOpenDrainPinExists(PORT_C_INDEX, 7));
    CuAssertTrue(tc,  digitalOpenDrainPinExists(PORT_C_INDEX, 8));
    CuAssertTrue(tc,  digitalOpenDrainPinExists(PORT_C_INDEX, 9));
    CuAssertTrue(tc, !digitalOpenDrainPinExists(PORT_C_INDEX, 10));
    CuAssertTrue(tc, !digitalOpenDrainPinExists(PORT_C_INDEX, 11));
    CuAssertTrue(tc, !digitalOpenDrainPinExists(PORT_C_INDEX, 12));
    CuAssertTrue(tc, !digitalOpenDrainPinExists(PORT_C_INDEX, 13));
    CuAssertTrue(tc, !digitalOpenDrainPinExists(PORT_C_INDEX, 14));
    CuAssertTrue(tc, !digitalOpenDrainPinExists(PORT_C_INDEX, 15));
    
    // For future testing, include one failure; fixing this should
    // allow including all the unincluded code above.
    CuAssertTrue(tc, !digitalOpenDrainPinExists(PORT_A_INDEX, 0));
}

/** A series of tests on the existance of digital IO pins.
  * @param tc Test object.
  */
void test_gpio(CuTest* tc)
{
}

/** Run a series of tests on general-purpose I/O functions.
    @return A suite of tests.

 */

CuSuite* getSuite_testGpio() {
    CuSuite* suite = CuSuiteNew();
    SUITE_ADD_TEST(suite, test_gpioExists);
    SUITE_ADD_TEST(suite, test_gpioOdExists);
    return suite;
}


int main(void)
{
    CuString* output = CuStringNew();
    CuSuite*  suite = CuSuiteNew();

    // Before testing, I/O constants must be set up.
    initIoConst();

    // Add all suites to be tested
    CuSuiteAddSuite(suite, getSuite_testGpio());

    // Run the tests and report the results
    CuSuiteRun(suite);
    CuSuiteSummary(suite, output);
    CuSuiteDetails(suite, output);
    puts(output->buffer);

    /*
     * Zero failures indicates an all-good exit code.
     * Any errors gives a non-zero exit code that should stop the build
     * when this is called by the Makefile
     */
    return suite->failCount;
}
