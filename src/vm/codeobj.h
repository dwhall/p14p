/*
# This file is Copyright 2003, 2006, 2007, 2009, 2010 Dean Hall.
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


#ifndef __CODEOBJ_H__
#define __CODEOBJ_H__


/**
 * \file
 * \brief CodeObj Type
 *
 * CodeObj type header.
 */


/* Masks for co_flags (from Python's code.h) */
#define CO_OPTIMIZED 0x01
#define CO_NEWLOCALS 0x02
#define CO_VARARGS 0x04
#define CO_VARKEYWORDS 0x08
#define CO_NESTED 0x10
#define CO_GENERATOR 0x20
#define CO_NOFREE 0x40


/**
 * Code Object
 *
 * A static constant definition of a code object.
 * A code object is normally defined in program memory to conserve RAM.
 * Functions, classes, methods and modules are all based on a code object.
 */
typedef struct PmCo_s
{
    PmObjDesc_t const od;
    PmString_t const * const co_name;
    PmString_t const * const co_filename;
    PmString_t const * const co_code;
    PmString_t const * const co_lnotab;
    PmTuple_t const * const co_names;
    PmTuple_t const * const co_consts;
    PmTuple_t const * const co_cellvars;
    uint16_t const co_firstlineno;
    uint8_t const co_argcount;
    uint8_t const co_flags;
    uint8_t const co_stacksize;
    uint8_t const co_nlocals;
    uint8_t const co_nfreevars;
} PmCo_t,
 *pPmCo_t;


#endif /* __CODEOBJ_H__ */
