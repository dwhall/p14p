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

#ifndef __OBJ_H__
#define __OBJ_H__

/**
 * Object Type
 *
 * Object type header.
 *
 * Log
 * ---
 *
 * 2007/03/16   #99: Design a way for ipm to be able to receive images larger
 *              than HEAP_MAX_CHUNK_SIZE
 * 2007/01/17   #76: Print will differentiate on strings and print tuples
 * 2007/01/09   #75: OBJ_TYPE_THR for thread objects (P.Adelt)
 * 2007/01/09   #75: Printing support (P.Adelt)
 * 2006/09/20   #35: Macroize all operations on object descriptors
 * 2006/08/31   #9: Fix BINARY_SUBSCR for case stringobj[intobj]
 * 2006/08/29   #15 - All mem_*() funcs and pointers in the vm should use
 *              unsigned not signed or void
 * 2002/12/16   OD's order changed to size-first to assist GC.
 * 2002/05/04   First.
 */


/***************************************************************
 * Constants
 **************************************************************/

#define OD_MARK_BIT (uint16_t)(1 << 14)
#define OD_FREE_BIT (uint16_t)(1 << 15)
#define OD_SIZE_MASK (uint16_t)(0x01FF)
#define OD_TYPE_MASK (uint16_t)(0x3E00)
#define OD_TYPE_SHIFT 9

/***************************************************************
 * Macros
 **************************************************************/

#define OBJ_GET_GCVAL(pobj) ((pobj)->od & OD_MARK_BIT)

#define OBJ_SET_GCVAL(pobj, gcval) \
            (pobj)->od = (gcval) ? (pobj)->od | OD_MARK_BIT \
                                 : (pobj)->od & ~OD_MARK_BIT

#define OBJ_GET_FREE(pobj) ((pobj)->od & OD_FREE_BIT)

#define OBJ_SET_FREE(pobj, free) \
            (pobj)->od = (free) ? (pobj)->od | OD_FREE_BIT \
                                : (pobj)->od & ~OD_FREE_BIT

/*
 * #99: od_size bits are shifted because size is a scaled value
 * True size is always a multiple of 4, so the lower two bits are ignored
 * and two more significant bits are gained.
 */
#define OBJ_GET_SIZE(pobj) (((pobj)->od & OD_SIZE_MASK) << 2)

#define OBJ_SET_SIZE(pobj, size) \
            do \
            { \
                (pobj)->od &= ~OD_SIZE_MASK; \
                (pobj)->od |= (((size) >> 2) & OD_SIZE_MASK); \
            } \
            while (0)

#define OBJ_GET_TYPE(pobj) ((((pobj)->od) & OD_TYPE_MASK) >> OD_TYPE_SHIFT)

#define OBJ_SET_TYPE(pobj, type) \
            do \
            { \
                (pobj)->od &= ~OD_TYPE_MASK; \
                (pobj)->od |= (((type) << OD_TYPE_SHIFT) & OD_TYPE_MASK); \
            } \
            while (0)


/***************************************************************
 * Types
 **************************************************************/

/**
 * Object type enum
 *
 * These values go in the od_type fields of the obj descriptor.
 * Be sure these values correspond to those in the image creator
 * tool.
 * The hashable types are grouped together for convenience.
 *
 * WARNING: od_type must be at most 5 bits! (must be < 0x20)
 */
typedef enum PmType_e
{
    OBJ_TYPE_HASHABLE_MIN = 0x00,

    /** None */
    OBJ_TYPE_NON = 0x00,

    /** Signed integer */
    OBJ_TYPE_INT = 0x01,

    /** Floating point 32b */
    OBJ_TYPE_FLT = 0x02,

    /** String */
    OBJ_TYPE_STR = 0x03,

    /** Tuple (immutable sequence) */
    OBJ_TYPE_TUP = 0x04,

    /** Code obj */
    OBJ_TYPE_COB = 0x05,

    /** Module obj */
    OBJ_TYPE_MOD = 0x06,

    /** Class obj */
    OBJ_TYPE_CLO = 0x07,

    /** Function obj (callable) */
    OBJ_TYPE_FXN = 0x08,

    /** Class instance */
    OBJ_TYPE_CLI = 0x09,

    /** Code image in static memory */
    OBJ_TYPE_CIM = 0x0A,

    /** Native function image */
    OBJ_TYPE_NIM = 0x0B,

    /** Native function object */
    OBJ_TYPE_NOB = 0x0C,

    /** Thread */
    OBJ_TYPE_THR = 0x0D,

    /** Exception object */
    OBJ_TYPE_EXN = 0x0E,

    /* All types after this are not hashable */
    OBJ_TYPE_HASHABLE_MAX = 0x0E,

    /** List (mutable sequence) */
    OBJ_TYPE_LST = 0x10,

    /** Dictionary (hash table) */
    OBJ_TYPE_DIC = 0x11,

    /* All types after this are not accessible to the user */
    OBJ_TYPE_ACCESSIBLE_MAX = 0x11,

    /** Frame type */
    OBJ_TYPE_FRM = 0x12,

    /** Block type (for,while,try,etc) */
    OBJ_TYPE_BLK = 0x13,

    /** Segment (within a seglist) */
    OBJ_TYPE_SEG = 0x14,

    /** Seglist */
    OBJ_TYPE_SGL = 0x15,

    /** Sequence iterator */
    OBJ_TYPE_SQI = 0x16,
} PmType_t, *pPmType_t;


/**
 * Object Descriptor
 *
 * All active PyMite "objects" must have this at the top of their struct.
 * (CodeObj, Frame, Dict, List, Tuple, etc.).
 *
 * The following is a diagram of the object descriptor:
 *
 *              MSb           LSb
 *               7 6 5 4 3 2 1 0
 *     pchunk-> +-+-+-+-+-+-+-+-+     S := Size of the chunk (2 LSbs dropped)
 *              |     S[9:2]    |     F := Free bit
 *              +-+-+---------+-+     M := GC Mark bit
 *              |F|M|    T    |S|     T := Object type (PyMite specific)
 *              +-+-+---------+-+
 *              | object data   |
 *              ...           ...
 *              | end data      |     Theoretical min size == 2
 *              +---------------+     Effective min size == 8
 *                                    (due to pmHeapDesc_t)
 *
 * Macros are used to get and set field values.
 * Using macros eliminates declaring bit fields which fails on some compilers.
 */
typedef uint16_t PmObjDesc_t, *pPmObjDesc_t;

/**
 * Object
 *
 * The abstract empty object type for PyMite.
 */
typedef struct PmObj_s
{
    /** Object descriptor */
    PmObjDesc_t od;
} PmObj_t,
 *pPmObj_t;


/***************************************************************
 * Prototypes
 **************************************************************/

/**
 * Loads an object from an image in memory.
 * Return pointer to object.
 * Leave add pointing one byte past end of obj.
 *
 * The following lists the simple object types
 * and their image structures:
 * -None:
 *      -type:      int8_t - OBJ_TYPE_NON
 *
 * -Int:
 *      -type:      int8_t - OBJ_TYPE_INT
 *      -value:     int32_t - signed integer value
 *
 * -Float:
 *      -type:      int8_t - OBJ_TYPE_FLOAT
 *      -value:     float32_t - 32-bit floating point value
 *
 * -Slice (is this allowed in img?):
 *      -type:      int8_t - OBJ_TYPE_SLICE
 *      -index1:    int16_t - first index.
 *      -index2:    int16_t - second index.
 *
 * @param   memspace memory space/type
 * @param   paddr ptr to ptr to obj
 *          return by reference: paddr pts to
 *          first byte after obj
 * @param   r_pobj Return arg, the loaded object.
 * @return  Return status
 */
PmReturn_t obj_loadFromImg(PmMemSpace_t memspace,
                           uint8_t const **paddr, pPmObj_t *r_pobj);

/**
 * Finds the boolean value of the given object.
 *
 * @param   pobj Ptr to object to test.
 * @return  Nonzero value if object is False.
 */
int8_t obj_isFalse(pPmObj_t pobj);

/**
 * Returns the boolean true if the item is in the object
 *
 * @param   pobj Ptr to container object
 * @param   pitem Ptr to item
 */
PmReturn_t obj_isIn(pPmObj_t pobj, pPmObj_t pitem);

/**
 * Compares two objects for equality.
 *
 * @param   pobj1 Ptr to first object.
 * @param   pobj2 Ptr to second object.
 * @return  C_SAME if the items are equivalent, C_DIFFER otherwise.
 */
int8_t obj_compare(pPmObj_t pobj1, pPmObj_t pobj2);

/**
 * Print an object, thereby using objects helpers.
 *
 * @param   pobj Ptr to object for printing.
 * @param   marshallString Only has influence on the way strings are printed.
 *                         If 0, just output the string bytewise. Otherwise,
 *                         surround with single quotes and escape unprintable
 *                         characters.
 * @return  Return status
 */
PmReturn_t obj_print(pPmObj_t pobj, uint8_t marshallString);

#endif /* __OBJ_H__ */
