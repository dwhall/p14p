/*
# This file is Copyright 2009 Dean Hall.
# This file is part of the PyMite VM.
# This file is licensed under the MIT License.
# See the LICENSE file for details.
*/


#undef __FILE_ID__
#define __FILE_ID__ 0x18


/**
 * \file
 * \brief Class Object Type
 *
 * Class object type operations.
 */


#include "pm.h"


#ifdef HAVE_AUTOBOX
static uint8_t const *liststr = (uint8_t const *)"list";
static uint8_t const *dictstr = (uint8_t const *)"dict";
static uint8_t const *stringstr = (uint8_t const *)"string";
static uint8_t const *autoboxstr = (uint8_t const *)"_Autobox";
static uint8_t const *objstr = (uint8_t const *)"obj";
#endif


PmReturn_t
class_new(pPmObj_t pattrs, pPmObj_t pbases, pPmObj_t pname, pPmObj_t *r_pclass)
{
    PmReturn_t retval = PM_RET_OK;
    uint8_t *pchunk;
    pPmObj_t pobj;

    /* Ensure types */
    if ((OBJ_GET_TYPE(pattrs) != OBJ_TYPE_DIC)
        || (OBJ_GET_TYPE(pbases) != OBJ_TYPE_TUP)
        || (OBJ_GET_TYPE(pname) != OBJ_TYPE_STR))
    {
        PM_RAISE(retval, PM_RET_EX_TYPE);
        return retval;
    }

    /* Allocate a class obj */
    retval = heap_getChunk(sizeof(PmClass_t), &pchunk);
    PM_RETURN_IF_ERROR(retval);
    pobj = (pPmObj_t)pchunk;
    OBJ_SET_TYPE(pobj, OBJ_TYPE_CLO);

    /* Class has no access to its CO */
    ((pPmClass_t)pobj)->cl_attrs = (pPmDict_t)pattrs;
    ((pPmClass_t)pobj)->cl_bases = (pPmTuple_t)pbases;

    *r_pclass = pobj;

    return retval;
}


/* Returns an instance of the class by reference */
PmReturn_t
class_instantiate(pPmObj_t pclass, pPmObj_t *r_pobj)
{
    PmReturn_t retval = PM_RET_OK;
    uint8_t *pchunk;
    pPmObj_t pobj;
    pPmObj_t pattrs;
    uint8_t objid;

    /* Allocate a class instance */
    retval = heap_getChunk(sizeof(PmInstance_t), &pchunk);
    PM_RETURN_IF_ERROR(retval);
    pobj = (pPmObj_t)pchunk;
    OBJ_SET_TYPE(pobj, OBJ_TYPE_CLI);

    /* Set the instance's fields */
    ((pPmInstance_t)pobj)->cli_class = (pPmClass_t)pclass;
    ((pPmInstance_t)pobj)->cli_attrs = C_NULL;

    /* Create the attributes dict */
    heap_gcPushTempRoot(pobj, &objid);
    retval = dict_new(&pattrs);
    heap_gcPopTempRoot(objid);
    ((pPmInstance_t)pobj)->cli_attrs = (pPmDict_t)pattrs;

    /* TODO: Store pclass in __class__ attr */

    *r_pobj = pobj;
    return retval;
}


#ifdef HAVE_AUTOBOX
PmReturn_t
class_autobox(pPmObj_t *pobj)
{
    PmReturn_t retval = PM_RET_OK;
    pPmObj_t pmodule, pstr, pclass, pwrapped, pmodcache;

    uint8_t const *pliststr = liststr;
    uint8_t const *pdictstr = dictstr;
    uint8_t const *pstringstr = stringstr;

    uint8_t const *pAutoboxstr = autoboxstr;
    uint8_t const *pobjstr = objstr;

    /* Load the appropriate module name,
     * or do nothing if we have a non-boxable type
     */
    if (OBJ_GET_TYPE(*pobj) == OBJ_TYPE_LST) {
        retval = string_new(&pliststr, &pstr);
        PM_RETURN_IF_ERROR(retval);
    } else if (OBJ_GET_TYPE(*pobj) == OBJ_TYPE_DIC) {
        retval = string_new(&pdictstr, &pstr);
        PM_RETURN_IF_ERROR(retval);
    } else if (OBJ_GET_TYPE(*pobj) == OBJ_TYPE_STR) {
        retval = string_new(&pstringstr, &pstr);
        PM_RETURN_IF_ERROR(retval);
    } else {
        return retval;
    }

    /** first, try to get the module from the cache */
    retval = dict_getItem(PM_PBUILTINS, PM_MD_STR, &pmodcache);
    PM_RETURN_IF_ERROR(retval);

    retval = dict_getItem(pmodcache, pstr, &pmodule);
    PM_RETURN_IF_ERROR(retval);

    if (!((retval == PM_RET_OK) && (OBJ_GET_TYPE(pmodule) == OBJ_TYPE_MOD)))
    {
        PM_RAISE(retval, PM_RET_EX_SYS);
        return retval;
    }

    /* grab the class from within the loaded module */
    retval = string_new(&pAutoboxstr, &pstr);
    PM_RETURN_IF_ERROR(retval);
    retval = dict_getItem((pPmObj_t) ((pPmFunc_t)pmodule)->f_attrs, pstr, &pclass);
    PM_RETURN_IF_ERROR(retval);

    /* instantiate instance of (type)._Autobox */
    retval = class_instantiate(pclass, &pwrapped);
    PM_RETURN_IF_ERROR(retval);

    /* store object as _Autobox().obj */
    retval = string_new(&pobjstr, &pstr);
    PM_RETURN_IF_ERROR(retval);
    retval = dict_setItem((pPmObj_t)((pPmInstance_t)pwrapped)->cli_attrs,
                          pstr, *pobj);
    PM_RETURN_IF_ERROR(retval);

    /** replace old object with new instance in place */
    *pobj = pwrapped;

    return retval;
}
#endif


PmReturn_t
class_method(pPmObj_t pinstance, pPmObj_t pfunc, pPmObj_t *r_pmeth)
{
    PmReturn_t retval = PM_RET_OK;
    uint8_t *pchunk;
    pPmMethod_t pmeth;
    pPmObj_t pattrs;
    uint8_t objid;

    /* Allocate a method */
    retval = heap_getChunk(sizeof(PmMethod_t), &pchunk);
    PM_RETURN_IF_ERROR(retval);
    OBJ_SET_TYPE(pchunk, OBJ_TYPE_MTH);

    /* Set method fields */
    pmeth = (pPmMethod_t)pchunk;
    pmeth->m_instance = (pPmInstance_t)pinstance;
    pmeth->m_func = (pPmFunc_t)pfunc;
    pmeth->m_attrs = C_NULL;

    /* Create the attributes dict */
    heap_gcPushTempRoot((pPmObj_t)pmeth, &objid);
    retval = dict_new(&pattrs);
    heap_gcPopTempRoot(objid);
    pmeth->m_attrs = (pPmDict_t)pattrs;

    *r_pmeth = (pPmObj_t)pmeth;
    return retval;
}


PmReturn_t
class_getAttr(pPmObj_t pobj, pPmObj_t pname, pPmObj_t *r_pobj)
{
    PmReturn_t retval;
    uint16_t i;
    pPmObj_t pparent;

    /* If the given obj is an instance, check its attrs */
    if (OBJ_GET_TYPE(pobj) == OBJ_TYPE_CLI)
    {
        retval = dict_getItem((pPmObj_t)((pPmInstance_t)pobj)->cli_attrs, pname,
                              r_pobj);
        if (retval == PM_RET_OK)
        {
            return retval;
        }

        /* Otherwise, check the instance's class */
        pobj = (pPmObj_t)((pPmInstance_t)pobj)->cli_class;
    }

    C_ASSERT(OBJ_GET_TYPE(pobj) == OBJ_TYPE_CLO);

    retval = dict_getItem((pPmObj_t)((pPmClass_t)pobj)->cl_attrs, pname,
                          r_pobj);

    /* If attr is not found, search parent(s) */
    if ((retval == PM_RET_EX_KEY) && (((pPmClass_t)pobj)->cl_bases != C_NULL))
    {
        for (i = 0; i < ((pPmClass_t)pobj)->cl_bases->length; i++)
        {
            pparent = ((pPmClass_t)pobj)->cl_bases->val[i];
            retval = class_getAttr(pparent, pname, r_pobj);
            if (retval == PM_RET_OK)
            {
                break;
            }
        }
    }

    return retval;
}


uint8_t /* boolean */
class_isSubclass(pPmObj_t ptest_class, pPmObj_t pbase_class)
{
    uint8_t i;
    uint8_t retval;

    retval = C_FALSE;

    if (ptest_class == pbase_class)
    {
        return C_TRUE;
    }

    /* Recursively check if test class has a matching base class */
    if (((pPmClass_t)ptest_class)->cl_bases != C_NULL)
    {
        for (i = 0; i < ((pPmClass_t)ptest_class)->cl_bases->length; i++)
        {
            retval = class_isSubclass(((pPmClass_t)ptest_class)->cl_bases->val[i],
                                        pbase_class);
            if (retval)
            {
                break;
            }
        }
    }
    return retval;
}
