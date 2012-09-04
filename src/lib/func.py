# This file is Copyright 2009 Dean Hall.
# This file is part of the Python-on-a-Chip libraries.
# This software is licensed under the MIT License.
# See the LICENSE file for details.

## @file
#  @copybrief func

## @package func
#  @brief Provides PyMite's func module.


##
# Returns the names tuple of the function/module object
#
def co_names(f):
    """__NATIVE__
    PmReturn_t retval = PM_RET_OK;
    pPmObj_t pfunc;

    /* If wrong number of args, raise TypeError */
    if (NATIVE_GET_NUM_ARGS() != 1)
    {
        PM_RAISE(retval, PM_RET_EX_TYPE);
        return retval;
    }

    pfunc = NATIVE_GET_LOCAL(0);
    NATIVE_SET_TOS((pPmObj_t)((pPmFunc_t)pfunc)->f_co->co_names);

    return retval;
    """
    pass


##
# Returns the constants tuple of the function/module object
#
def co_consts(f):
    """__NATIVE__
    PmReturn_t retval = PM_RET_OK;
    pPmObj_t pfunc;

    /* If wrong number of args, raise TypeError */
    if (NATIVE_GET_NUM_ARGS() != 1)
    {
        PM_RAISE(retval, PM_RET_EX_TYPE);
        return retval;
    }

    pfunc = NATIVE_GET_LOCAL(0);
    NATIVE_SET_TOS((pPmObj_t)((pPmFunc_t)pfunc)->f_co->co_consts);

    return retval;
    """
    pass


# :mode=c:
