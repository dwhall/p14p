/** \file
 *  \brief This file implements Python functions defined in main.py.
 */

#include "pm.h"

/** Raise an exception unless the given expression is true.
 *  This MUST be called from the C implementation of a 
 *  Python function, becuase it assumes the existance of
 *  - PmReturn_t retval
 *  \param expr Expression to evaluate.
 *  \param prReturn If expression is false, exception to raise. Must
 *                  be of type \ref PmReturn_t and typically is in the
 *                  \ref PmReturn_e enum.
 *  \param eStr Diagostic string to inclcude with the exception.
 *              Currently, not used.
 */
#define EXCEPTION_UNLESS(expr, prReturn, eStr) \
    do { \
        if (!(expr)) { \
            printf("Error: %s\n", eStr); \
            PM_RAISE(retval, prReturn); \
            return retval; \
        } \
    } while (C_FALSE)


/** Macro to ease calling getXxxx functions. This MUST be called from
 *  the C implementation of a Python function, becuase it assumes
 *  the existance of:
 *  - PmReturn_t retval
 *  - pPmFrame_t* ppframe
 *  \param u16_ndx Zero-based index of the desired parameter to extract.
 *  \param val Resulting value extracted.
 *  \param getFunc Function to call (must be from the getXxx family) to
 *         extract the requested parameter.
 *  \return Standard Python return value.
 */
#define GET_XXX(u16_ndx, val, getFunc) \
    do { \
        retval = getFunc(ppframe, u16_ndx, &val); \
        PM_RETURN_IF_ERROR(retval); \
    } while (C_FALSE)

/** Macro to ease calling the getUint16 function. This MUST be called from
 *  the C implementation of a Python function, becuase it assumes
 *  the existance of:
 *  - PmReturn_t retval
 *  - pPmFrame_t* ppframe
 *  \param u16_ndx Zero-based index of the desired parameter to extract.
 *  \param u16_val Resulting value extracted.
 *  \return Standard Python return value.
 */
#define GET_UINT16(u16_ndx, u16_val) GET_XXX(u16_ndx, u16_val, getUint16)

/** Macro to ease calling the getUint16 function. This MUST be called from
 *  the C implementation of a Python function, becuase it assumes
 *  the existance of:
 *  - PmReturn_t retval
 *  - pPmFrame_t* ppframe
 *  \param u16_ndx Zero-based index of the desired parameter to extract.
 *  \param i32_val Resulting value extracted.
 *  \return Standard Python return value.
 */
#define GET_INT32(u16_ndx, i32_val) GET_XXX(u16_ndx, i32_val, getInt32)

/** Macro to ease calling the getUint16 function. This MUST be called from
 *  the C implementation of a Python function, becuase it assumes
 *  the existance of:
 *  - PmReturn_t retval
 *  - pPmFrame_t* ppframe
 *  \param u16_ndx Zero-based index of the desired parameter to extract.
 *  \param u_bool Resulting boolean value extracted.
 *  \return Standard Python return value.
 */
#define GET_BOOL(u16_ndx, u_bool) GET_XXX(u16_ndx, u_bool, getBool)

/** Check the number of arguments passed to a Python function.
 *  Report an exception if the number is incorrect. This MUST be called from
 *  the C implementation of a Python function, becuase it assumes
 *  the existance of:
 *  - PmReturn_t retval
 *  - pPmFrame_t* ppframe
 *  @param numArgs Number of arguemnts expected.
 */
#define CHECK_NUM_ARGS(numArgs) \
    EXCEPTION_UNLESS(NATIVE_GET_NUM_ARGS() == numArgs, PM_RET_EX_TYPE, \
      "Incorrect number of arguments.")

/** Get an unsigned, 16-bit value from the arguments passed to a Python
 *  function. Raises errors as necessary.
 *  \param ppframe The Python stack frame containing user arguments.
 *  \param u16_ndx Zero-based index of the desired parameter to extract.
 *  \param pu16_val Pointer to resulting uint16 value extracted.
 *  \return Standard Python return value.
 */
PmReturn_t getUint16(pPmFrame_t *ppframe, uint16_t u16_ndx, uint16_t* pu16_val);

/** Get a signed, 32-bit value from the arguments passed to a Python
 *  function. Raises errors as necessary.
 *  \param ppframe The Python stack frame containing user arguments.
 *  \param u16_ndx Zero-based index of the desired parameter to extract.
 *  \param pi32_val Pointer to resulting int32 value extracted.
 *  \return Standard Python return value.
 */
PmReturn_t getInt32(pPmFrame_t *ppframe, uint16_t u16_ndx, int32_t* pi32_val);

/** Get a boolean value from the arguments passed to a Python
 *  function. Raises errors as necessary.
 *  \param ppframe The Python stack frame containing user arguments.
 *  \param u16_ndx Zero-based index of the desired parameter to extract.
 *  \param pu_bool Pointer to resulting boolean value extracted.
 *  \return Standard Python return value.
 */
PmReturn_t getBool(pPmFrame_t *ppframe, uint16_t u16_ndx, uint_t* pu_bool);
