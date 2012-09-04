# This file is Copyright 2009 Dean Hall.
# This file is part of the Python-on-a-Chip libraries.
# This software is licensed under the MIT License.
# See the LICENSE file for details.

## @file
#  @copybrief dict

## @package dict
#  @brief Provides PyMite's dict module.

def clear(d):
    """__NATIVE__
    pPmObj_t pd;
    PmReturn_t retval = PM_RET_OK;

    /* Raise TypeError if it's not a dict or wrong number of args, */
    pd = NATIVE_GET_LOCAL(0);
    if ((OBJ_GET_TYPE(pd) != OBJ_TYPE_DIC) || (NATIVE_GET_NUM_ARGS() != 1))
    {
        PM_RAISE(retval, PM_RET_EX_TYPE);
        return retval;
    }

    /* Clear the contents of the dict */
    retval = dict_clear(pd);
    PM_RETURN_IF_ERROR(retval);

    NATIVE_SET_TOS(PM_NONE);

    return retval;
    """
    pass


def keys(d):
    """__NATIVE__
    pPmObj_t pd;
    pPmObj_t pl;
    pPmObj_t pk;
    pSeglist_t psl;
    uint16_t i;
    PmReturn_t retval = PM_RET_OK;
    uint8_t objid;

    /* Raise TypeError if it's not a dict or wrong number of args, */
    pd = NATIVE_GET_LOCAL(0);
    if ((OBJ_GET_TYPE(pd) != OBJ_TYPE_DIC) || (NATIVE_GET_NUM_ARGS() != 1))
    {
        PM_RAISE(retval, PM_RET_EX_TYPE);
        return retval;
    }

    /* Create empty list */
    retval = list_new(&pl);
    PM_RETURN_IF_ERROR(retval);

    /* Iterate through the keys seglist */
    psl = ((pPmDict_t)pd)->d_keys;
    for (i = 0; i < ((pPmDict_t)pd)->length; i++)
    {
        /* Get the key and append it to the list */
        retval = seglist_getItem(psl, i, &pk);
        PM_RETURN_IF_ERROR(retval);
        heap_gcPushTempRoot(pl, &objid);
        retval = list_append(pl, pk);
        heap_gcPopTempRoot(objid);
        PM_RETURN_IF_ERROR(retval);
    }

    /* Return the list of keys to the caller */
    NATIVE_SET_TOS(pl);

    return retval;
    """
    pass


def has_key(d, k):
    return k in d


def values(d):
    """__NATIVE__
    pPmObj_t pd;
    pPmObj_t pl;
    pPmObj_t pv;
    pSeglist_t psl;
    uint16_t i;
    PmReturn_t retval = PM_RET_OK;
    uint8_t objid;

    /* Raise TypeError if it's not a dict or wrong number of args, */
    pd = NATIVE_GET_LOCAL(0);
    if ((OBJ_GET_TYPE(pd) != OBJ_TYPE_DIC) || (NATIVE_GET_NUM_ARGS() != 1))
    {
        PM_RAISE(retval, PM_RET_EX_TYPE);
        return retval;
    }

    /* Create empty list */
    retval = list_new(&pl);
    PM_RETURN_IF_ERROR(retval);

    /* Iterate through the values seglist */
    psl = ((pPmDict_t)pd)->d_vals;
    for (i = 0; i < ((pPmDict_t)pd)->length; i++)
    {
        /* Get the value and append it to the list */
        retval = seglist_getItem(psl, i, &pv);
        PM_RETURN_IF_ERROR(retval);
        heap_gcPushTempRoot(pl, &objid);
        retval = list_append(pl, pv);
        heap_gcPopTempRoot(objid);
        PM_RETURN_IF_ERROR(retval);
    }

    /* Return the list of values to the caller */
    NATIVE_SET_TOS(pl);

    return retval;
    """
    pass


def update(d1, d2):
    # Updates dict d1 with the contents of d2.  Returns None
    """__NATIVE__
    pPmObj_t pd1;
    pPmObj_t pd2;
    PmReturn_t retval;

    /* Raise TypeError if wrong number of args, */
    if (NATIVE_GET_NUM_ARGS() != 2)
    {
        PM_RAISE(retval, PM_RET_EX_TYPE);
        return retval;
    }

    pd1 = NATIVE_GET_LOCAL(0);
    pd2 = NATIVE_GET_LOCAL(1);
    retval = dict_update(pd1, pd2, C_FALSE);

    NATIVE_SET_TOS(PM_NONE);
    return retval;
    """
    pass


# :mode=c:
