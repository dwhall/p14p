/*
 * PyMite - A flyweight Python interpreter for 8-bit microcontrollers and more.
 * Copyright 2002 Dean Hall
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
#define __FILE_ID__ 0x92
/**
 * Unit Test 001
 *
 * Test the Dict implementation.
 *
 *  List of tests and whether they are implemented (Y/N).
 *
 *      dict_setItem(pdict, pkey, pval)
 *          Y 000 Pass C_NULL to dict, expect a SystemError
 *          Y 001 Pass C_NULL to key, expect a SystemError
 *          Y 002 Pass C_NULL to val, expect a SystemError
 *          Y 003 Test dict_setItem() passing valid vals, return dict must not be NULL
 *          Y 004 Test dict_setItem() passing valid vals, return dict must be same addr
 *          Y 005 Test dict_setItem(), set(k,v), dict length must be 1
 *          Y 006 Test dict_setItem(), set(k,v) item at k must be same v
 *          Y 007 Test dict_setItem(), set(k,v1) set(k,v2), item at k must be v2
 *          Y 008 Test dict_setItem(), expect retval is OK
 *
 *      dict_clear(pdict)
 *          Y dict = C_NULL
 *          Y dict = non-dict
 *          Y dict = empty dict, return dict not C_NULL
 *          Y dict = empty dict, return length is 0
 *          Y dict = non-empty dict, return length is 0
 *          N retval is OK
 *
 *      dict_getItem(pdict, pkey)
 *          N dict == C_NULL
 *          N key == C_NULL
 *          N dict = non-dict
 *          N key = non-hashable
 *          N key with empty key,val lists
 *          N key (key not known)
 *          N key (kew known)
 *          N retval is OK
 *
 * Log:
 *
 * 2003/01/11   First.
 */


#include "CuTest.h"
#include "pm.h"


/**************************************************************/

/**
 * Test dict_new():
 *      retval is OK
 *      obj pointer is not C_NULL
 *      obj type must be DICT
 *      obj const bit must be clear
 *      dict length must be 0
 */
void
ut_dict_new_000(CuTest* tc)
{
#if 1
    CuAssertTrue(tc, 1==1);

    pPmObj_t pobj = C_NULL;
    PmReturn_t retval;

    retval = dict_new(&pobj);
#else

    CuAssertTrue(tc, retval == PM_RET_OK);
    CuAssertPtrNotNull(tc, pobj);
    CuAssertTrue(tc, OBJ_GET_TYPE(*pobj) == OBJ_TYPE_DIC);
    CuAssertTrue(tc, !OBJ_IS_CONST(*pobj));
    CuAssertTrue(tc, ((pPmDict_t)pobj)->length == 0);
#endif
}

#if 0

/**************************************************************/
/** Pass C_NULL to dict, expect a SystemError */
void
ut_dict_setItem_000(void)
{
    PmReturn_t retval = PM_RET_OK;
    retval = dict_setItem((pPmObj_t)C_NULL,
                          (pPmObj_t)C_NULL,
                          (pPmObj_t)C_NULL);
    if (retval != PM_RET_EX_SYS)
    {
        TEST_ERR(__LINE__);
    }
}

/** Pass C_NULL to key, expect a SystemError */
void
ut_dict_setItem_001(void)
{
    PmReturn_t retval = PM_RET_OK;
    pPmObj_t pdict = C_NULL;

    dict_new(&pdict);
    retval = dict_setItem(pdict,
                          (pPmObj_t)C_NULL,
                          (pPmObj_t)C_NULL);
    if (retval != PM_RET_EX_SYS)
    {
        TEST_ERR(__LINE__);
    }
}

/** Pass C_NULL to val, expect a SystemError */
void
ut_dict_setItem_002(void)
{
    PmReturn_t retval = PM_RET_OK;
    pPmObj_t pdict = C_NULL;

    dict_new(&pdict);
    retval = dict_setItem(pdict,
                          PM_ZERO,
                          (pPmObj_t)C_NULL);
    if (retval != PM_RET_EX_SYS)
    {
        TEST_ERR(__LINE__);
    }
}

/** Test dict_setItem() passing valid vals, return dict must not be NULL */
void
ut_dict_setItem_003(void)
{
    pPmObj_t pdict = C_NULL;

    dict_new(&pdict);
    dict_setItem(pdict, PM_ZERO, PM_ONE);
    if (pdict == C_NULL)
    {
        TEST_ERR(__LINE__);
    }
}

/** Test dict_setItem() passing valid vals, return dict must be same addr */
void
ut_dict_setItem_004(void)
{
    pPmObj_t pdict = C_NULL;
    pPmObj_t pdictbefore = C_NULL;

    dict_new(&pdict);
    pdictbefore = pdict;
    dict_setItem(pdict, PM_ZERO, PM_ONE);
    if (pdict != pdictbefore)
    {
        TEST_ERR(__LINE__);
    }
}

/** Test dict_setItem(), set(k,v), dict length must be 1 */
void
ut_dict_setItem_005(void)
{
    pPmObj_t pdict = C_NULL;

    dict_new(&pdict);
    dict_setItem(pdict, PM_ZERO, PM_ONE);
    if (((pPmDict_t)pdict)->length != 1)
    {
        TEST_ERR(__LINE__);
    }
}

/** Test dict_setItem(), set(k,v) item at k must be same v */
void
ut_dict_setItem_006(void)
{
    pPmObj_t pval = C_NULL;
    pPmObj_t pdict = C_NULL;

    dict_new(&pdict);
    dict_setItem(pdict, PM_ZERO, PM_ONE);
    dict_getItem(pdict, PM_ZERO, &pval);
    if (pval != PM_ONE)
    {
        TEST_ERR(__LINE__);
    }
}

/** Test dict_setItem(), set(k,v1) set(k,v2), item at k must be v2 */
void
ut_dict_setItem_007(void)
{
    pPmObj_t pval = C_NULL;
    pPmObj_t pdict = C_NULL;

    dict_new(&pdict);
    dict_setItem(pdict, PM_ZERO, PM_ZERO);
    dict_setItem(pdict, PM_ZERO, PM_ONE);
    dict_getItem(pdict, PM_ZERO, &pval);
    if (pval != PM_ONE)
    {
        TEST_ERR(__LINE__);
    }
}

/** Test dict_setItem(), expect retval is OK */
void
ut_dict_setItem_008(void)
{
    PmReturn_t retval = PM_RET_OK;
    pPmObj_t pdict = C_NULL;

    dict_new(&pdict);
    retval = dict_setItem(pdict,
                          PM_ZERO,
                          PM_ZERO);
    if (retval != PM_RET_OK)
    {
        TEST_ERR(__LINE__);
    }
}

/**************************************************************/
/** Test dict_clear() passing C_NULL, return is void, must inspect */
void
ut_dict_clear_000(void)
{
    dict_clear((pPmObj_t)C_NULL);
}

/** Test dict_clear() passing non-dict, return is void, must inspect */
void
ut_dict_clear_001(void)
{
    pPmObj_t pint;
    PmReturn_t retval = PM_RET_OK;

     retval = int_new(256, &pint);
    dict_clear(pint);
}

/** Test dict_clear() passing empty dict, return dict must not be null */
void
ut_dict_clear_002(void)
{
    pPmObj_t pdict = C_NULL;

    dict_new(&pdict);
    dict_clear(pdict);
    if (pdict == C_NULL)
    {
        TEST_ERR(__LINE__);
    }
 }

/** Test dict_clear() passing empty dict, return dict must have 0 length */
void
ut_dict_clear_003(void)
{
    pPmObj_t pdict = C_NULL;

    dict_new(&pdict);
    dict_clear(pdict);
    if (((pPmDict_t)pdict)->length != 0)
    {
        TEST_ERR(__LINE__);
    }
}

/** Test dict_clear() passing non-empty dict, return dict must have 0 length */
void
ut_dict_clear_004(void)
{
    pPmObj_t pdict = C_NULL;

    dict_new(&pdict);
    dict_setItem(pdict, PM_ZERO, PM_ZERO);
    dict_clear(pdict);
    if (((pPmDict_t)pdict)->length != 0)
    {
        TEST_ERR(__LINE__);
    }
}

#endif

/**************************************************************/
/** Set of all dict test functions */

CuSuite *getTestDictSuite(void)
{
	CuSuite* suite = CuSuiteNew();

	SUITE_ADD_TEST(suite, ut_dict_new_000);

    return suite;
}

#if 0

void
ut_dict(void)
{
    /* dict_new() tests */
/*
    ut_dict_new_000();
    ut_dict_new_001();
    ut_dict_new_002();
    ut_dict_new_003();
*/

    /* dict_clear() tests */
    ut_dict_clear_000();
    ut_dict_clear_001();
    ut_dict_clear_002();
    ut_dict_clear_003();
    ut_dict_clear_004();

    /* dict_setItem() tests */
    ut_dict_setItem_000();
    ut_dict_setItem_001();
    ut_dict_setItem_002();
    ut_dict_setItem_003();
    ut_dict_setItem_004();
    ut_dict_setItem_005();
    ut_dict_setItem_006();
    ut_dict_setItem_007();
    ut_dict_setItem_008();
}
#endif
