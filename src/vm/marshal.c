/*
# This file is Copyright 2011 Dean Hall.
#
# This file is part of the PyMite VM.
# The PyMite VM is free software: you can redistribute it and/or modify
# it under the terms of the GNU GENERAL PUBLIC LICENSE Version 2.
#
# The PyMite VM is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# A copy of the GNU GENERAL PUBLIC LICENSE Version 2
# is seen in the file COPYING in this directory.
*/

#undef __FILE_ID__
#define __FILE_ID__ 0x1A


#include "pm.h"


static PmReturn_t dump_obj(pPmObj_t po, uint16_t *r_size, uint8_t **r_pdest);
static PmReturn_t load_obj(uint8_t *ps, uint16_t *offset, uint16_t const maxlen, pPmObj_t *r_po);


static
PmReturn_t
load_int(uint8_t *ps, uint16_t *offset, uint16_t const maxlen, pPmObj_t *r_po)
{
    int32_t val;

    switch(ps[*offset])
    {
        case 'B':
            if (((*offset) + 2) > maxlen) return PM_RET_ERR;
            /* Casting is important for sign-extension */
            val = (int32_t)(int8_t)ps[*offset + 1];
            (*offset) += 2;
            break;

        case 'H':
            if (((*offset) + 3) > maxlen) return PM_RET_ERR;
            /* Casting is important for sign-extension */
            val = (int32_t)(int8_t)ps[*offset + 1];
            val = ps[*offset + 1] | (int32_t)(int16_t)(ps[*offset + 2] << 8);
            (*offset) += 3;
            break;

        case 'I':
            if (((*offset) + 5) > maxlen) return PM_RET_ERR;
            /* Casting is important for sign-extension */
            val = ps[*offset + 1] | (ps[*offset + 2] << 8)
                  | (ps[*offset + 3] << 16) | (int32_t)(ps[*offset + 4] << 24);
            (*offset) += 5;
            break;
    }
    return int_new(val, r_po);
}


static
PmReturn_t
load_flt(uint8_t *ps, uint16_t *offset, uint16_t const maxlen, pPmObj_t *r_po)
{
    union
    {
        char c[4];
        float f;
    }
    v;

    if (((*offset) + 5) > maxlen) return PM_RET_ERR;
    v.c[0] = ps[*offset + 1];
    v.c[1] = ps[*offset + 2];
    v.c[2] = ps[*offset + 3];
    v.c[3] = ps[*offset + 4];
    (*offset) += 5;

    return float_new(v.f, r_po);
}


static
PmReturn_t
load_str(uint8_t *ps, uint16_t *offset, uint16_t const maxlen, pPmObj_t *r_po)
{
    uint16_t len;
    uint8_t const *cstr;

    if (ps[*offset] == 's')
    {
        len = ps[*offset + 1];
        if (((*offset) + 2 + len) > maxlen) return PM_RET_ERR;
        cstr = &ps[*offset + 2];
        (*offset) += 2 + len;
    }
    else
    {
        len = ps[*offset + 1] + (ps[*offset + 2] << 8);
        if (((*offset) + 3 + len) > maxlen) return PM_RET_ERR;
        cstr = &ps[*offset + 3];
        (*offset) += 3 + len;
    }
    return string_newWithLen(&cstr, len, r_po);
}


static
PmReturn_t
load_tup(uint8_t *ps, uint16_t *offset, uint16_t const maxlen, pPmObj_t *r_po)
{
    uint16_t len;
    pPmObj_t pobj;
    PmReturn_t retval;
    uint8_t objid;
    uint16_t i;

    if (ps[*offset] == '(')
    {
        len = ps[*offset + 1];
        if (((*offset) + 2) > maxlen) return PM_RET_ERR;
        (*offset) += 2;
    }
    else
    {
        len = ps[*offset + 1] + (ps[*offset + 2] << 8);
        if (((*offset) + 3) > maxlen) return PM_RET_ERR;
        (*offset) += 3;
    }

    retval = tuple_new(len, r_po);
    PM_RETURN_IF_ERROR(retval);

    heap_gcPushTempRoot(*r_po, &objid);
    for (i = 0; i < len; i++)
    {
        retval = load_obj(ps, offset, maxlen, &pobj);
        PM_BREAK_IF_ERROR(retval);
        ((pPmTuple_t)*r_po)->val[i] = pobj;
    }
    heap_gcPopTempRoot(objid);

    return retval;
}


static
PmReturn_t
load_lst(uint8_t *ps, uint16_t *offset, uint16_t const maxlen, pPmObj_t *r_po)
{
    uint16_t len;
    pPmObj_t pobj;
    PmReturn_t retval;
    uint8_t objid, objid2;
    uint16_t i;

    if (ps[*offset] == '[')
    {
        len = ps[*offset + 1];
        if (((*offset) + 2) > maxlen) return PM_RET_ERR;
        (*offset) += 2;
    }
    else
    {
        len = ps[*offset + 1] + (int16_t)(ps[*offset + 2] << 8);
        if (((*offset) + 3) > maxlen) return PM_RET_ERR;
        (*offset) += 3;
    }

    retval = list_new(r_po);
    PM_RETURN_IF_ERROR(retval);

    heap_gcPushTempRoot(*r_po, &objid);
    for (i = 0; i < len; i++)
    {
        retval = load_obj(ps, offset, maxlen, &pobj);
        PM_BREAK_IF_ERROR(retval);

        heap_gcPushTempRoot(pobj, &objid2);
        retval = list_append(*r_po, pobj);
        heap_gcPopTempRoot(objid2);
        PM_RETURN_IF_ERROR(retval);
    }
    heap_gcPopTempRoot(objid);

    return retval;
}


static
PmReturn_t
load_dic(uint8_t *ps, uint16_t *offset, uint16_t const maxlen, pPmObj_t *r_po)
{
    uint16_t len;
    pPmObj_t pkey;
    pPmObj_t pval;
    PmReturn_t retval;
    uint8_t objid, objid2, objid3;
    uint16_t i;

    if (ps[*offset] == '{')
    {
        len = ps[*offset + 1];
        if (((*offset) + 2) > maxlen) return PM_RET_ERR;
        (*offset) += 2;
    }
    else
    {
        len = ps[*offset + 1] + (int16_t)(ps[*offset + 2] << 8);
        if (((*offset) + 3) > maxlen) return PM_RET_ERR;
        (*offset) += 3;
    }

    retval = dict_new(r_po);
    PM_RETURN_IF_ERROR(retval);

    heap_gcPushTempRoot(*r_po, &objid);
    for (i = 0; i < len; i++)
    {
        retval = load_obj(ps, offset, maxlen, &pkey);
        PM_POP_ROOT_AND_RETURN_IF_ERROR(retval, objid);

        heap_gcPushTempRoot(pkey, &objid2);
        retval = load_obj(ps, offset, maxlen, &pval);
        PM_POP_ROOT_AND_RETURN_IF_ERROR(retval, objid);

        heap_gcPushTempRoot(pval, &objid3);
        retval = dict_setItem(*r_po, pkey, pval);
        PM_POP_ROOT_AND_RETURN_IF_ERROR(retval, objid);
    }
    heap_gcPopTempRoot(objid);

    return retval;
}


static
PmReturn_t
load_cob(uint8_t *ps, uint16_t *offset, uint16_t const maxlen, pPmObj_t *r_po)
{
    PmReturn_t retval;
    pPmObj_t pobj;
    uint8_t objid;

    if (((*offset) + 1) > maxlen) return PM_RET_ERR;
    (*offset) += 1;

    retval = co_new(r_po);
    PM_RETURN_IF_ERROR(retval);

    heap_gcPushTempRoot(*r_po, &objid);
    retval = load_str(ps, offset, maxlen, &pobj);
    PM_POP_ROOT_AND_RETURN_IF_ERROR(retval, objid);
    ((pPmCob_t)*r_po)->co_code = (pPmString_t)pobj;
    retval = load_str(ps, offset, maxlen, &pobj);
    PM_POP_ROOT_AND_RETURN_IF_ERROR(retval, objid);
    ((pPmCob_t)*r_po)->co_lnotab = (pPmString_t)pobj;
    retval = load_tup(ps, offset, maxlen, &pobj);
    PM_POP_ROOT_AND_RETURN_IF_ERROR(retval, objid);
    ((pPmCob_t)*r_po)->co_names = (pPmTuple_t)pobj;
    retval = load_tup(ps, offset, maxlen, &pobj);
    PM_POP_ROOT_AND_RETURN_IF_ERROR(retval, objid);
    ((pPmCob_t)*r_po)->co_consts = (pPmTuple_t)pobj;
    retval = load_tup(ps, offset, maxlen, &pobj);
    PM_POP_ROOT_AND_RETURN_IF_ERROR(retval, objid);
    ((pPmCob_t)*r_po)->co_cellvars = (pPmTuple_t)pobj;
    heap_gcPopTempRoot(objid);

    if (((*offset) + 7) > maxlen) return PM_RET_ERR;
    ((pPmCob_t)*r_po)->co_firstlineno = ps[*offset] + (ps[*offset + 1] << 8);
    ((pPmCob_t)*r_po)->co_argcount = ps[*offset + 2];
    ((pPmCob_t)*r_po)->co_flags = ps[*offset + 3];
    ((pPmCob_t)*r_po)->co_stacksize = ps[*offset + 4];
    ((pPmCob_t)*r_po)->co_nlocals = ps[*offset + 5];
    ((pPmCob_t)*r_po)->co_nfreevars = ps[*offset + 6];
    (*offset) += 7;

    return retval;
}


static
PmReturn_t
load_obj(uint8_t *ps, uint16_t *offset, uint16_t const maxlen, pPmObj_t *r_po)
{
    PmReturn_t retval;

    switch(ps[*offset])
    {
        case 'N': (*offset) += 1; *r_po = (pPmObj_t)&pm_global_none; return PM_RET_OK;
        case 'F': (*offset) += 1; *r_po = (pPmObj_t)&pm_global_false; return PM_RET_OK;
        case 'T': (*offset) += 1; *r_po = (pPmObj_t)&pm_global_true; return PM_RET_OK;
        case '0': (*offset) += 1; *r_po = (pPmObj_t)&pm_global_zero; return PM_RET_OK;
        case '1': (*offset) += 1; *r_po = (pPmObj_t)&pm_global_one; return PM_RET_OK;
        case '2': (*offset) += 1; *r_po = (pPmObj_t)&pm_global_two; return PM_RET_OK;
        case '3': (*offset) += 1; *r_po = (pPmObj_t)&pm_global_three; return PM_RET_OK;
        case '4': (*offset) += 1; *r_po = (pPmObj_t)&pm_global_four; return PM_RET_OK;
        case '5': (*offset) += 1; *r_po = (pPmObj_t)&pm_global_five; return PM_RET_OK;
        case '6': (*offset) += 1; *r_po = (pPmObj_t)&pm_global_six; return PM_RET_OK;
        case '7': (*offset) += 1; *r_po = (pPmObj_t)&pm_global_seven; return PM_RET_OK;
        case '8': (*offset) += 1; *r_po = (pPmObj_t)&pm_global_eight; return PM_RET_OK;
        case '9': (*offset) += 1; *r_po = (pPmObj_t)&pm_global_nine; return PM_RET_OK;
        case 'B':
        case 'H':
        case 'I': return load_int(ps, offset, maxlen, r_po);
#ifdef HAVE_FLOAT
        case 'r': return load_flt(ps, offset, maxlen, r_po);
#endif
        case 's':
        case 'S': return load_str(ps, offset, maxlen, r_po);
        case '(':
        case ')': return load_tup(ps, offset, maxlen, r_po);
        case '[':
        case ']': return load_lst(ps, offset, maxlen, r_po);
        case '{':
        case '}': return load_dic(ps, offset, maxlen, r_po);
        case 'C': return load_cob(ps, offset, maxlen, r_po);
    }
    PM_RAISE(retval, PM_RET_EX_VAL);
    return retval;
}


PmReturn_t
marshal_load(uint8_t *ps, uint16_t len, pPmObj_t *r_po)
{
    uint16_t offset = 0;
    return load_obj(ps, &offset, len, r_po);
}


/* Dump Routines */


static
PmReturn_t
dump_int(pPmObj_t po, uint16_t *r_size, uint8_t **r_pdest)
{
    int32_t v = ((pPmInt_t)po)->val;

    if ((v >= 0) && (v <= 9))
    {
        *r_size += 1;
        if (r_pdest != C_NULL)
        {
            (*r_pdest)[0] = '0' + (uint8_t)v;
            *r_pdest += 1;
        }
        return PM_RET_OK;
    }

    if ((v >= -128) && (v < 128))
    {
        *r_size += 2;
        if (r_pdest != C_NULL)
        {
            (*r_pdest)[0] = 'B';
            (*r_pdest)[1] = (uint8_t)v;
            *r_pdest += 2;
        }
        return PM_RET_OK;
    }

    if ((v >= -32768) && (v < 32768))
    {
        *r_size += 3;
        if (r_pdest != C_NULL)
        {
            (*r_pdest)[0] = 'H';
            (*r_pdest)[1] = (uint8_t)(v & 0xFF);
            (*r_pdest)[2] = (uint8_t)((v >> 8) & 0xFF);
            *r_pdest += 3;
        }
        return PM_RET_OK;
    }

    *r_size += 5;
    if (r_pdest != C_NULL)
    {
        (*r_pdest)[0] = 'I';
        (*r_pdest)[1] = (uint8_t)(v & (uint8_t)0xFF);
        (*r_pdest)[2] = (uint8_t)((v >> (uint8_t)8) & (uint8_t)0xFF);
        (*r_pdest)[3] = (uint8_t)((v >> (uint8_t)16) & (uint8_t)0xFF);
        (*r_pdest)[4] = (uint8_t)((v >> (uint8_t)24) & (uint8_t)0xFF);
        *r_pdest += 5;
    }
    return PM_RET_OK;
}


static
PmReturn_t
dump_flt(pPmObj_t po, uint16_t *r_size, uint8_t **r_pdest)
{
    union
    {
        char c[4];
        float f;
    }
    v;

    v.f = ((pPmFloat_t)po)->val;

    *r_size += 5;
    if (r_pdest != C_NULL)
    {
        (*r_pdest)[0] = 'r';
        (*r_pdest)[1] = (uint8_t)(v.c[0]);
        (*r_pdest)[2] = (uint8_t)(v.c[1]);
        (*r_pdest)[3] = (uint8_t)(v.c[2]);
        (*r_pdest)[4] = (uint8_t)(v.c[3]);
        *r_pdest += 5;
    }
    return PM_RET_OK;
}


static
PmReturn_t
dump_str(pPmObj_t po, uint16_t *r_size, uint8_t **r_pdest)
{
    uint16_t len;
    uint8_t *cstr;

    len = ((pPmString_t)po)->length;
    if (len < 256)
    {
        *r_size += 2 + len;
        if (r_pdest != C_NULL)
        {
            (*r_pdest)[0] = 's';
            (*r_pdest)[1] = (uint8_t)(len & (uint8_t)0xFF);
            *r_pdest += 2;
        }
    }
    else
    {
        *r_size += 3 + len;
        if (r_pdest != C_NULL)
        {
            (*r_pdest)[0] = 'S';
            (*r_pdest)[1] = (uint8_t)(len & (uint8_t)0xFF);
            (*r_pdest)[2] = (uint8_t)((len >> (uint8_t)8) & (uint8_t)0xFF);
            *r_pdest += 3;
        }
    }
    if (r_pdest != C_NULL)
    {
        cstr = ((pPmString_t)po)->val;
        sli_memcpy(*r_pdest, cstr, len);
        *r_pdest += len;
    }

    return PM_RET_OK;
}


static
PmReturn_t
dump_tup(pPmObj_t po, uint16_t *r_size, uint8_t **r_pdest)
{
    uint16_t n;
    uint16_t len;
    PmReturn_t retval = PM_RET_OK;

    len = ((pPmTuple_t)po)->length;
    if (len < 256)
    {
        *r_size += 2;
        if (r_pdest != C_NULL)
        {
            (*r_pdest)[0] = '(';
            (*r_pdest)[1] = (uint8_t)(len & (uint8_t)0xFF);
            *r_pdest += 2;
        }
    }
    else
    {
        *r_size += 3;
        if (r_pdest != C_NULL)
        {
            (*r_pdest)[0] = ')';
            (*r_pdest)[1] = (uint8_t)(len & (uint8_t)0xFF);
            (*r_pdest)[2] = (uint8_t)((len >> (uint8_t)8) & (uint8_t)0xFF);
            *r_pdest += 3;
        }
    }

    for (n = 0; n < len; n++)
    {
        retval = dump_obj(((pPmTuple_t)po)->val[n], r_size, r_pdest);
        PM_RETURN_IF_ERROR(retval);
    }
    return retval;
}


static
PmReturn_t
dump_cob(pPmObj_t po, uint16_t *r_size, uint8_t **r_pdest)
{
    PmReturn_t retval;

    /* type identifier */
    *r_size += 1;
    if (r_pdest != C_NULL)
    {
        (*r_pdest)[0] = 'C';
        *r_pdest += 1;
    }

    retval = dump_str((pPmObj_t)((pPmCo_t)po)->co_code, r_size, r_pdest);
    PM_RETURN_IF_ERROR(retval);
    retval = dump_str((pPmObj_t)((pPmCo_t)po)->co_lnotab, r_size, r_pdest);
    PM_RETURN_IF_ERROR(retval);
    retval = dump_tup((pPmObj_t)((pPmCo_t)po)->co_names, r_size, r_pdest);
    PM_RETURN_IF_ERROR(retval);
    retval = dump_tup((pPmObj_t)((pPmCo_t)po)->co_consts, r_size, r_pdest);
    PM_RETURN_IF_ERROR(retval);
    retval = dump_tup((pPmObj_t)((pPmCo_t)po)->co_cellvars, r_size, r_pdest);
    PM_RETURN_IF_ERROR(retval);

    /* firstlineno, argcount, flags, stacksize, nlocals, nfreevars */
    *r_size += 7;
    if (r_pdest != C_NULL)
    {
        (*r_pdest)[0] = (uint8_t)(((pPmCo_t)po)->co_firstlineno & 0xFF);
        (*r_pdest)[1] = (uint8_t)((((pPmCo_t)po)->co_firstlineno >> 8) & 0xFF);
        (*r_pdest)[2] = ((pPmCo_t)po)->co_argcount;
        (*r_pdest)[3] = ((pPmCo_t)po)->co_flags;
        (*r_pdest)[4] = ((pPmCo_t)po)->co_stacksize;
        (*r_pdest)[5] = ((pPmCo_t)po)->co_nlocals;
        (*r_pdest)[6] = ((pPmCo_t)po)->co_nfreevars;
        *r_pdest += 7;
    }
    return retval;
}


static
PmReturn_t
dump_lst(pPmObj_t po, uint16_t *r_size, uint8_t **r_pdest)
{
    uint16_t n;
    uint16_t len;
    pPmObj_t pitem;
    PmReturn_t retval = PM_RET_OK;

    len = ((pPmList_t)po)->length;
    if (len < 256)
    {
        *r_size += 2;
        if (r_pdest != C_NULL)
        {
            (*r_pdest)[0] = '[';
            (*r_pdest)[1] = (uint8_t)(len & (uint8_t)0xFF);
            *r_pdest += 2;
        }
    }
    else
    {
        *r_size += 3;
        if (r_pdest != C_NULL)
        {
            (*r_pdest)[0] = ']';
            (*r_pdest)[1] = (uint8_t)(len & (uint8_t)0xFF);
            (*r_pdest)[2] = (uint8_t)((len >> (uint8_t)8) & (uint8_t)0xFF);
            *r_pdest += 3;
        }
    }

    for (n = 0; n < len; n++)
    {
        retval = list_getItem(po, n, &pitem);
        PM_RETURN_IF_ERROR(retval);
        retval = dump_obj(pitem, r_size, r_pdest);
        PM_RETURN_IF_ERROR(retval);
    }
    return retval;
}


static
PmReturn_t
dump_dic(pPmObj_t po, uint16_t *r_size, uint8_t **r_pdest)
{
    uint16_t n;
    uint16_t len;
    pPmObj_t pitem;
    pSeglist_t pkeys;
    pSeglist_t pvals;
    PmReturn_t retval;

    len = ((pPmDict_t)po)->length;
    if (len < 256)
    {
        *r_size += 2;
        if (r_pdest != C_NULL)
        {
            (*r_pdest)[0] = '{';
            (*r_pdest)[1] = (uint8_t)(len & (uint8_t)0xFF);
            *r_pdest += 2;
        }
    }
    else
    {
        *r_size += 3;
        if (r_pdest != C_NULL)
        {
            (*r_pdest)[0] = '}';
            (*r_pdest)[1] = (uint8_t)(len & (uint8_t)0xFF);
            (*r_pdest)[2] = (uint8_t)((len >> (uint8_t)8) & (uint8_t)0xFF);
            *r_pdest += 3;
        }
    }

    pkeys = ((pPmDict_t)po)->d_keys;
    pvals = ((pPmDict_t)po)->d_vals;

    retval = PM_RET_OK;
    for (n = 0; n < len; n++)
    {
        retval = seglist_getItem(pkeys, n, &pitem);
        PM_RETURN_IF_ERROR(retval);
        retval = dump_obj(pitem, r_size, r_pdest);
        PM_RETURN_IF_ERROR(retval);

        retval = seglist_getItem(pvals, n, &pitem);
        PM_RETURN_IF_ERROR(retval);
        retval = dump_obj(pitem, r_size, r_pdest);
        PM_RETURN_IF_ERROR(retval);
    }

    return retval;
}


/* Determines the size that the serialized object will occupy.
 * Raises an exception if the type (or contained type) is not serializable.
 * This function is recursive.
 */
static
PmReturn_t
dump_obj(pPmObj_t po, uint16_t *r_size, uint8_t **r_pdest)
{
    uint8_t t;
    PmReturn_t retval;

    t = OBJ_GET_TYPE(po);
    switch(t)
    {
        case OBJ_TYPE_NON:
            *r_size += 1;
            if (r_pdest != C_NULL)
            {
                (*r_pdest)[0] = 'N';
                (*r_pdest)++;
            }
            return PM_RET_OK;

        case OBJ_TYPE_BOOL:
            *r_size += 1;
            if (r_pdest != C_NULL)
            {
                (*r_pdest)[0] = ((pPmBoolean_t)po)->val ? 'T' : 'F';
                (*r_pdest)++;
            }
            return PM_RET_OK;

#ifdef HAVE_FLOAT
        case OBJ_TYPE_FLT: return dump_flt(po, r_size, r_pdest);
#endif
        case OBJ_TYPE_INT: return dump_int(po, r_size, r_pdest);
        case OBJ_TYPE_STR: return dump_str(po, r_size, r_pdest);
        case OBJ_TYPE_TUP: return dump_tup(po, r_size, r_pdest);
        case OBJ_TYPE_COB: return dump_cob(po, r_size, r_pdest);
        case OBJ_TYPE_LST: return dump_lst(po, r_size, r_pdest);
        case OBJ_TYPE_DIC: return dump_dic(po, r_size, r_pdest);
        default: PM_RAISE(retval, PM_RET_EX_TYPE); return retval;
    }
}


PmReturn_t
marshal_dump(pPmObj_t po, pPmObj_t *r_ps)
{
    PmReturn_t retval;
    uint16_t size;
    uint8_t *pchunk;
    uint8_t *pdest;
    pPmString_t pstr;

    /* Recursive size and type analysis */
    size = 0;
    retval = dump_obj(po, &size, C_NULL);
    PM_RETURN_IF_ERROR(retval);

    /* Create string to hold the serialized data */
    retval = heap_getChunk(sizeof(PmString_t) + size, &pchunk);
    PM_RETURN_IF_ERROR(retval);
    pstr = (pPmString_t)pchunk;

    /* Init the string obj */
    OBJ_SET_TYPE(pstr, OBJ_TYPE_STR);
    pstr->length = size;
    pstr->next = C_NULL;
    pdest = (uint8_t *)&(pstr->val);

    /* Now serialize the object into the string's bytes */
    retval = dump_obj(po, &size, &pdest);
    *r_ps = (pPmObj_t)pstr;
    return retval;
}
