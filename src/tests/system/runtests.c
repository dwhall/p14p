/*
# This file is Copyright 2011 Dean Hall.
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

#include "pm.h"
#include "stdlib.h"


#define HEAP_SIZE 0x2000


typedef struct atypical_settings_s
{
    uint32_t heap_size;
    PmReturn_t expected_retval;
    char *module_name;
} atypical_settings_t;


/* Typical tests use HEAP_SIZE and return PM_RET_OK */
char * typical_tests[] = {
    "t002", "t003", "t009", "t010", "t013",
    "t017", "t020", "t026", "t047", "t049",
    "t050", "t051", "t054", "t056", "t058",
    "t076", "t078", "t080", "t090", "t094",
    "t095", "t096", "t102", "t103", "t104",
    "t108a","t110", "t113", "t135", "t138",
    "t140", "t141", "t142", "t147", "t150",
    "t153", "t157", "t158", "t160", "t162",
    "t164", "t174", "t187", "t205", "t207",
    "t213", "t230", "t232", "t233", "t234",
    "t235", "t236", "t242", "t243", "t244",
    "t245", "t256", "t259", "t281", "t284",
    "t290", "t291", "t329", "t347a","t349",
    "t370", "t374", "t376", "t377", "t378d",
    "t384",
    };
uint16_t num_typical_tests = sizeof(typical_tests) / sizeof(int8_t *);


/* Atypical tests may use a different heap size or raise an exception */
atypical_settings_t atypical_tests[] = {
    {HEAP_SIZE, PM_RET_EX_ATTR, "t114"},
    {HEAP_SIZE, PM_RET_EX_ZDIV, "t119"},
    {HEAP_SIZE, PM_RET_EX_TYPE, "t132"},
    {HEAP_SIZE, PM_RET_EX_VAL, "t136"},
    {HEAP_SIZE, PM_RET_EX_ZDIV, "t148"},
    {HEAP_SIZE, PM_RET_EX_ATTR, "t149"},
    {HEAP_SIZE, PM_RET_EX_TYPE, "t190"},
    {HEAP_SIZE, PM_RET_EX_TYPE, "t202"},
    {HEAP_SIZE, PM_RET_EX_ASSRT, "t248"},
    {HEAP_SIZE, PM_RET_EX_TYPE, "t258"},
    {HEAP_SIZE, PM_RET_EX_TYPE, "t321"},
    {HEAP_SIZE, PM_RET_EX_TYPE, "t334"},
    {HEAP_SIZE, PM_RET_EX_NAME, "t350a"},
    {HEAP_SIZE, PM_RET_OK, "t352"}, /* TODO: set to PM_RET_EX_OFLOW for Issue #169 */

    /* Test formerly bad stack sizes */
    {0x10000, PM_RET_OK, "t380"},
    {0x20008, PM_RET_OK, "t380"},
    {0x30004, PM_RET_OK, "t380"},
    {0x4000C, PM_RET_OK, "t380"},
};
uint16_t num_atypical_tests = sizeof(atypical_tests) / sizeof(atypical_settings_t);


/* Replicate pm_run()'s functionality to allow two initial threads. */
int t075(void)
{
    uint8_t heap[HEAP_SIZE];
    PmReturn_t retval;
    pPmObj_t pmodA;
    pPmObj_t pmodB;
    pPmObj_t pstring;
    uint8_t const *pmodstrA = (uint8_t const *)"t075a";
    uint8_t const *pmodstrB = (uint8_t const *)"t075b";

    retval = pm_init(heap, HEAP_SIZE);
    PM_RETURN_IF_ERROR(retval);

    /* Import modules from global struct */
    retval = string_new(&pmodstrA, &pstring);
    PM_RETURN_IF_ERROR(retval);
    retval = mod_import(pstring, &pmodA);
    PM_RETURN_IF_ERROR(retval);

    retval = string_new(&pmodstrB, &pstring);
    PM_RETURN_IF_ERROR(retval);
    retval = mod_import(pstring, &pmodB);
    PM_RETURN_IF_ERROR(retval);

    /* Interpret the module's bcode */
    retval = interp_addThread((pPmFunc_t)pmodA);
    retval = interp_addThread((pPmFunc_t)pmodB);
    PM_RETURN_IF_ERROR(retval);
    retval = interpret(INTERP_RETURN_ON_NO_THREADS);

    return retval;
}


int main(void)
{
    uint8_t heap[HEAP_SIZE];
    uint8_t *aheap;
    PmReturn_t retval;
    uint16_t i;

    retval = plat_init();
    PM_RETURN_IF_ERROR(retval);

    /* Run the typical tests */
    for (i = 0; i < num_typical_tests; i++)
    {
        retval = pm_init(heap, HEAP_SIZE);
        PM_RETURN_IF_ERROR(retval);

        retval = pm_run((uint8_t *)typical_tests[i]);
        if (retval != PM_RET_OK)
        {
            fprintf(stderr, "Error: %s\n", typical_tests[i]);
            return retval;
        }
    }


    /* Run the atypical tests */
    for (i = 0; i < num_atypical_tests; i++)
    {
        aheap = malloc(atypical_tests[i].heap_size);
        retval = pm_init(aheap, atypical_tests[i].heap_size);
        PM_RETURN_IF_ERROR(retval);

        retval = pm_run((uint8_t *)atypical_tests[i].module_name);
        free(aheap);
        if (retval != atypical_tests[i].expected_retval)
        {
            fprintf(stderr, "Error: %s\n", atypical_tests[i].module_name);
            fprintf(stderr, "got retval = 0x%0X\n", retval);
            return retval;
        }
    }

    /* Run the bizarre tests */
    retval = t075();
    PM_RETURN_IF_ERROR(retval);

    plat_deinit();
    puts("Done.");
    return PM_RET_OK;
 }
