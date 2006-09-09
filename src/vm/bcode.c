#undef __FILE_ID__
#define __FILE_ID__ 0x14
/**
 * Bytecode implementations
 *
 * @author      Dean Hall
 * @copyright   Copyright 2006 Dean Hall.  All rights reserved.
 *
 * Log
 * ---
 *
 * 2006/09/03   First.
 */

#include "py.h"


/*
 * The implementation for the COMPARE_OP bytecode
 */
PyReturn_t
bcode_compare_op(pPyObj_t pobj1, pPyObj_t pobj2, U16 comptype, pPyObj_t *r_pobj)
{
    U8 r;
    S32 a;
    S32 b;
    PyReturn_t retval = PY_RET_OK;

    /* Comparison of two integers is the most common */
    if ((pobj1->od.od_type == OBJ_TYPE_INT) &&
        (pobj2->od.od_type == OBJ_TYPE_INT))
    {
        a = ((pPyInt_t)pobj2)->val;
        b = ((pPyInt_t)pobj1)->val;

        switch (comptype)
        {
            case COMP_LT: r = (a <  b); break;
            case COMP_LE: r = (a <= b); break;
            case COMP_EQ: r = (a == b); break;
            case COMP_NE: r = (a != b); break;
            case COMP_GT: r = (a >  b); break;
            case COMP_GE: r = (a >= b); break;
            case COMP_IS: r = (pobj1 == pobj2); break;
            case COMP_IS_NOT: r = (pobj1 != pobj2); break;
            default: PY_ERR(ERR_ARG);
        }
        *r_pobj = (r) ? PY_TRUE : PY_FALSE;
    }
    else if (comptype == COMP_EQ)
    {
        if (obj_compare(pobj1, pobj2) == C_SAME)
        {
            *r_pobj = PY_TRUE;
        }
        else
        {
            *r_pobj = PY_FALSE;
        }
    }
    else if (comptype == COMP_NE)
    {
        if (obj_compare(pobj1, pobj2) == C_DIFFER)
        {
            *r_pobj = PY_TRUE;
        }
        else
        {
            *r_pobj = PY_FALSE;
        }
    }

    /* Compare to see if pobj2 is instance of Exception or subclass of pobj1 */
    else if (comptype == COMP_EXN)
    {
        /*pobj1==w==exc*/
        /*
        if (pobj2 == instance_t)
            pobj2 = pobj2->class
        if (pobj2->type == class && pobj1->type == class)
            return obj_isSubclass(pobj2, pobj1)
        else
            return ???
        */
    }

    /* Other forms of compare are not implemented: IN, NOT_IN, etc */
    else
    {
        PY_ERR(__LINE__);
    }
    
    return retval;
}