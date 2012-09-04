/*
# This file is Copyright 2002 Dean Hall.
# This file is part of the PyMite VM.
# This file is licensed under the MIT License.
# See the LICENSE file for details.
*/


#ifndef __SLI_H__
#define __SLI_H__


/**
 * \file
 * \brief Standard Library Interface
 *
 * PyMite requires a few functions from a few different
 * standard C libraries (memory, string, etc).
 * If your microcontroller has these libraries,
 * set the constant to 1 for each library available.
 * This will cause a macro to be defined which wraps
 * the function for use by PyMite.
 * Otherwise, leave the constant as 0, and PyMite will
 * use the function defined in sli.c
 * Some of the functions in sli.c will need to be ported
 * to the target system.
 */


/**
 * If the compiler has string.h, set HAVE_STRING to 1;
 * otherwise, leave it 0 and the sli functions will be used.
 */
#define HAVE_STRING_H 0


/*
 * This section creates a macro or a function prototype
 * for each library based on the corresponding constant.
 * For example, if HAVE_STRING_H is defined to non-zero,
 * the system <string.h> file will be included,
 * and a macro "sli_strcmp" will be created to wrap the strcmp()
 * function.  But if HAVE_STRING is zero, the sli_strcmp()
 * prototype will be declared and sli_strcmp() must be
 * implemented in sli.c
 */

#if HAVE_STRING_H

#include <string.h>

#define sli_memcpy(to, from, n) memcpy((to), (from), (n))
#define sli_strcmp(s1, s2)      strcmp((s1),(s2))
#define sli_strlen(s)           strlen(s)
#define sli_strncmp(s1, s2, n)  strncmp((s1),(s2),(n))

#else

/**
 * Copies a block of memory in RAM.
 *
 * @param   to The destination address.
 * @param   from The source address.
 * @param   n The number of bytes to copy.
 * @return  The initial pointer value of the destination
 * @see     mem_copy
 */
void *sli_memcpy(unsigned char *to, unsigned char const *from, unsigned int n);

/**
 * Compares two strings.
 *
 * @param   s1 Ptr to string 1.
 * @param   s2 Ptr to string 2.
 * @return  value that is less then, equal to or greater than 0
 *          depending on whether s1's encoding is
 *          less than, equal to, or greater than s2's.
 */
int sli_strcmp(char const *s1, char const *s2);

/**
 * Obtain string length.
 *
 * @param   s ptr to string.
 * @return  number of bytes in string.
 */
int sli_strlen(char const *s);

/**
 * Compare strings for a specific length.
 *
 * @param   s1 ptr to string 1.
 * @param   s2 ptr to string 2.
 * @param   n number of chars to compare
 * @return  value that is less then, equal to or greater than 0
 *          depending on whether s1's encoding is
 *          less than, equal to, or greater than s2's.
 */
int sli_strncmp(char const *s1, char const *s2, unsigned int n);

#endif /* HAVE_STRING_H */

/**
 * Copy a value repeatedly into a block of memory
 *
 * @param   dest the destination address.
 * @param   val the value.
 * @param   n the number of bytes to copy.
 * @return  Nothing
 * @see     memset
 */
void sli_memset(unsigned char *dest, const char val, unsigned int n);

/**
 * Prints a string to stdout (using plat_putByte)
 *
 * @param s Pointer to the C string to print
 */
void sli_puts(uint8_t * s);

/**
 * Formats a 32-bit signed int as a decimal value.
 *
 * @param value the 32-bit signed value
 * @param buf a pointer to where the formatted string goes
 * @param buflen the length of the given buffer in bytes
 * @return a pointer to the string. 
 */
PmReturn_t sli_ltoa10(int32_t value, uint8_t *buf, uint8_t buflen);

/**
 * Formats an 8-bit int as a hexadecimal value.
 *
 * @param value the 8-bit value
 * @param buf a pointer to where the formatted string goes
 * @param buflen the length of the given buffer in bytes
 * @param upperCase when zero, hex chars rendered lowercase, else uppercase
 * @return Always PM_RET_OK
 */
PmReturn_t sli_btoa16(uint8_t value, uint8_t *buf, uint8_t buflen, uint8_t upperCase);

/**
 * Formats a 32-bit signed int as a hexadecimal value.
 *
 * @param value the 32-bit signed value
 * @param buf a pointer to where the formatted string goes
 * @param buflen the length of the given buffer in bytes
 * @param upperCase when zero, hex chars rendered lowercase, else uppercase
 * @return Always PM_RET_OK
 */
PmReturn_t sli_ltoa16(int32_t value, uint8_t *buf, uint8_t buflen, uint8_t upperCase);

/**
 * Formats a pointer as a hexadecimal value.
 *
 * @param value the pointer
 * @param buf a pointer to where the formatted string goes
 * @param buflen the length of the given buffer in bytes
 * @param upperCase when zero, hex chars rendered lowercase, else uppercase
 * @return Always PM_RET_OK
 */
PmReturn_t sli_ptoa16(intptr_t value, uint8_t *buf, uint8_t buflen, uint8_t upperCase);

/**
 * Formats a 32-bit (single-precision) float as an ascii string.
 *
 * @param f the float value
 * @param buf a pointer to where the formatted string goes
 * @param buflen the size of the buffer
 * @return Status
 */
PmReturn_t sli_ftoa(float f, uint8_t *buf, uint8_t buflen);

#endif /* __SLI_H__ */
