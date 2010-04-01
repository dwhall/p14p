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
#define EXCEPTION_UNLESS(expr, prReturn, ...) \
    do { \
        if (!(expr)) { \
            printf("Error: " __VA_ARGS__); \
            PM_RAISE(retval, prReturn); \
            return retval; \
        } \
    } while (C_FALSE)

/** Call a C function which implements a Python routine. If the
 *  return value indicates an error, return. Because this macro
 *  assumes execution within the C implementation of a Python routine,
 *  is requires the existance of the variable
 *  <code>PmReturn_t retval</code>. The C function called must
 *  return <code>PmReturn_t retval</code>.
 *  @param func Call to a C function, not just the name of the function.
 *              Its return value will be tested.
 */
#define PM_CHECK_FUNCTION(func) \
    do { \
        retval = func; \
        PM_RETURN_IF_ERROR(retval); \
    } while (C_FALSE)

/** Macro to ease calling the getUint16 function. This MUST be called from
 *  the C implementation of a Python function, becuase it assumes
 *  the existance of:
 *  - PmReturn_t retval
 *  - pPmFrame_t* ppframe
 *  \param u8_ndx Zero-based index of the desired parameter to extract.
 *  \param u16_val Resulting value extracted.
 *  \return Standard Python return value.
 */
#define GET_UINT16(u8_ndx, u16_val) \
    PM_CHECK_FUNCTION(getUint16(ppframe, u8_ndx, &u16_val))

/** Macro to ease calling the getUint16 function. This MUST be called from
 *  the C implementation of a Python function, becuase it assumes
 *  the existance of:
 *  - PmReturn_t retval
 *  - pPmFrame_t* ppframe
 *  \param u8_ndx Zero-based index of the desired parameter to extract.
 *  \param i32_val Resulting value extracted.
 *  \return Standard Python return value.
 */
#define GET_INT32(u8_ndx, i32_val) \
    PM_CHECK_FUNCTION(getInt32(ppframe, u8_ndx, &i32_val))

/** Macro to ease calling the getUint16 function. This MUST be called from
 *  the C implementation of a Python function, becuase it assumes
 *  the existance of:
 *  - PmReturn_t retval
 *  - pPmFrame_t* ppframe
 *  \param u8_ndx Zero-based index of the desired parameter to extract.
 *  \param u_bool Resulting boolean value extracted.
 *  \return Standard Python return value.
 */
#define GET_BOOL(u8_ndx, u_bool) \
    PM_CHECK_FUNCTION(getBool(ppframe, u8_ndx, &u_bool))

/** Check the number of arguments passed to a Python function.
 *  Report an exception if the number is incorrect. This MUST be called from
 *  the C implementation of a Python function, becuase it assumes
 *  the existance of:
 *  - PmReturn_t retval
 *  - pPmFrame_t* ppframe
 *  @param u16_numArgs Number of arguemnts expected.
 */
#define CHECK_NUM_ARGS(u8_numArgs) \
    EXCEPTION_UNLESS(NATIVE_GET_NUM_ARGS() == u8_numArgs, PM_RET_EX_TYPE, \
      "Expected %u arguments, but received %u.", (uint16) u8_numArgs, \
      (uint16) NATIVE_GET_NUM_ARGS())

/** Get an integer from one of the arguments passed to a Python function,
 *  requiring that the integer lie winin a minimum and minimum value.
 *  Raises errors as necessary.
 *  \param ppframe The Python stack frame containing user arguments.
 *  \param u8_ndx Zero-based index of the desired parameter to extract.
 *  \param i32_min Minimum allowable value.
 *  \param i32_max Maximum allowable value.
 *  \param pi32_val Pointer to resulting int32 value extracted.
 *  \return Standard Python return value.
 */
PmReturn_t getRangedInt(pPmFrame_t *ppframe, uint8_t u8_ndx, 
  int32_t i32_min, int32_t i32_max, int32_t* pi32_val);

/** Get an unsigned, 16-bit value from the arguments passed to a Python
 *  function. Raises errors as necessary.
 *  \param ppframe The Python stack frame containing user arguments.
 *  \param u8_ndx Zero-based index of the desired parameter to extract.
 *  \param pu16_val Pointer to resulting uint16 value extracted.
 *  \return Standard Python return value.
 */
PmReturn_t getUint16(pPmFrame_t *ppframe, uint8_t u8_ndx, uint16_t* pu16_val);

/** Get a signed, 32-bit value from the arguments passed to a Python
 *  function. Raises errors as necessary.
 *  \param ppframe The Python stack frame containing user arguments.
 *  \param u8_ndx Zero-based index of the desired parameter to extract.
 *  \param pi32_val Pointer to resulting int32 value extracted.
 *  \return Standard Python return value.
 */
PmReturn_t getInt32(pPmFrame_t *ppframe, uint8_t u8_ndx, int32_t* pi32_val);

/** Get a boolean value from the arguments passed to a Python
 *  function. Raises errors as necessary.
 *  \param ppframe The Python stack frame containing user arguments.
 *  \param u8_ndx Zero-based index of the desired parameter to extract.
 *  \param pu_bool Pointer to resulting boolean value extracted.
 *  \return Standard Python return value.
 */
PmReturn_t getBool(pPmFrame_t *ppframe, uint8_t u8_ndx, uint_t* pu_bool);
