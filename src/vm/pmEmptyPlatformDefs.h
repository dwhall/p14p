/*
# This file is Copyright 2010 Dean Hall.
# This file is part of the PyMite VM.
# This file is licensed under the MIT License.
# See the LICENSE file for details.
*/


#ifndef __PM_EMPTY_PLATFORM_DEFS_H__
#define __PM_EMPTY_PLATFORM_DEFS_H__


/**
 * \file
 * \brief Empty platform-specific definitions
 *
 * This file #defines as blank any undefined platform-specific
 * definitions.
 */

/**
 * Define a processor-specific specifier for use in declaring the heap.
 * If not defined, make it empty.
 * See <code>pmHeap</code> in heap.c for its use, which is:<br>
 * <code>static PmHeap_t pmHeap PM_PLAT_HEAP_ATTR;</code>
 */
#if !defined(PM_PLAT_HEAP_ATTR) || defined(__DOXYGEN__)
#define PM_PLAT_HEAP_ATTR
#endif

#endif /* __PM_EMPTY_PLATFORM_DEFS_H__ */
