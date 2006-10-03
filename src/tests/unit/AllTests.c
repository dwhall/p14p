#include <stdio.h>

#include "CuTest.h"

/* Need to declare this array since no PyMite application is linked */
const unsigned char usr_nat_fxn_table[] = {};

CuSuite* getTestDictSuite();

void RunAllTests(void)
{
    CuString *output = CuStringNew();
    CuSuite* suite = CuSuiteNew();

    CuSuiteAddSuite(suite, getTestDictSuite());

    CuSuiteRun(suite);
    CuSuiteSummary(suite, output);
    CuSuiteDetails(suite, output);
    printf("%s\n", output->buffer);
}

int main(void)
{
    RunAllTests();

    return 0;
}
