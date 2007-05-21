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

#undef __FILE_ID__
#define __FILE_ID__ 0x06

/**
 * VM Heap
 *
 * VM heap operations.
 * All of PyMite's dynamic memory is obtained from this heap.
 * The heap provides dynamic memory on demand.
 *
 * Log
 * ---
 *
 * 2007/02/02   #87: Redesign the heap
 * 2007/01/09   #75: Added thread type, fail correctly w/o GC (P.Adelt)
 * 2006/11/15   #53: Fix Win32/x86 build break
 * 2006/09/14   #27: Fix S16/U16 are 32-bits on DESKTOP
 * 2006/09/10   #20: Implement assert statement
 * 2006/08/29   #15 - All mem_*() funcs and pointers in the vm should use
 *              unsigned not signed or void
 * 2003/02/10   Started GC marking fxns.
 * 2002/08/31   Implemented freelist when chunk is deleted.
 * 2002/04/30   First.
 */

/***************************************************************
 * Includes
 **************************************************************/

#include "pm.h"


/***************************************************************
 * Constants
 **************************************************************/

/**
 * Static initial size of the heap.
 * A value should be provided by the makefile
 */
#ifndef HEAP_SIZE
#error HEAP_SIZE not defined by the build environment
#endif

/**
 * The maximum size a chunk can be.
 * The chunk size is limited by the od_size field in the object descriptor.
 * Currently, that field is 8 bits.  Since the heap allocator already rounds
 * requested chunk sizes up to the next multiple of four, the lower two bits
 * are alway zero.  So, we can shift this adjusted size value to the right
 * two places which allows larger effective sizes.
 * The maximum size is now (2^11 - 1 == 2047), but it must be a multiple of
 * four to maintain alignment on some 32-bit platforms, so it becomes 2044.
 */
#define HEAP_MAX_CHUNK_SIZE 2044

/** The minimum size a chunk can be */
#define HEAP_MIN_CHUNK_SIZE sizeof(PmHeapDesc_t)

/** Heap descriptor fields */
#define HD_RESERVED_BIT (uint16_t)(1 << 14)
#define HD_FREE_BIT (uint16_t)(1 << 15)
#define HD_SIZE_MASK (uint16_t)(0x3FFF)


/***************************************************************
 * Macros
 **************************************************************/

#define HD_GET_FREE(pchunk) ((pchunk)->hd & HD_FREE_BIT)

#define HD_SET_FREE(pchunk, free) \
            ((pchunk)->hd = (free) ? (pchunk)->hd | HD_FREE_BIT \
                                   : (pchunk)->hd & ~HD_FREE_BIT)

#define HD_GET_SIZE(pchunk) (((pchunk)->hd & HD_SIZE_MASK) << 2)

#define HD_SET_SIZE(pchunk, size) \
            do \
            { \
                (pchunk)->hd &= ~HD_SIZE_MASK; \
                (pchunk)->hd |= (((size) >> 2) & HD_SIZE_MASK); \
            } \
            while (0)


/***************************************************************
 * Types
 **************************************************************/

/**
 * The following is a diagram of the heap descriptor at the head of the chunk:
 *
 *                MSb          LSb
 *                7 6 5 4 3 2 1 0
 *      pchunk-> +-+-+-+-+-+-+-+-+
 *               |     S[9:2]    |     S := Size of the chunk (2 LSbs dropped)
 *               +-+-+-----------+     F := Chunk free bit (not in use)
 *               |F|R| S[15:10]  |     R := Bit reserved for future use
 *               +-+-+-----------+
 *               |     P(L)      |     P := hd_prev: Pointer to previous node
 *               |     P(H)      |     N := hd_next: Pointer to next node
 *               |     N(L)      |
 *               |     N(H)      |     Theoretical min size == 6
 *               +---------------+     Effective min size == 8
 *               | unused space  |     (12 on 32-bit MCUs)
 *               ...           ...
 *               | end chunk     |
 *               +---------------+
 */
typedef struct PmHeapChunk_s
{
    /** Heap descriptor */
    uint16_t hd;

    /** Ptr to prev heap chunk */
    struct PmHeapChunk_s *prev;

    /** Ptr to next heap chunk */
    struct PmHeapChunk_s *next;
} PmHeapDesc_t,
 *pPmHeapDesc_t;

typedef struct PmHeap_s
{
    /** Ptr to list of free chunks; sorted smallest to largest. */
    pPmHeapDesc_t pfreelist;

    /** The amount of heap space available in free list */
    uint16_t avail;

    /** Global declaration of heap. */
    uint8_t base[HEAP_SIZE] __attribute__((aligned(4)));

    /** Garbage collection mark value */
    uint8_t gcval;
} PmHeap_t,
 *pPmHeap_t;


/***************************************************************
 * Globals
 **************************************************************/

/** The PyMite heap */
static PmHeap_t pmHeap;


/***************************************************************
 * Functions
 **************************************************************/

/* Removes the given chunk from the free list; leaves list in sorted order */
static PmReturn_t
heap_unlinkFromFreelist(pPmHeapDesc_t pchunk)
{
    C_ASSERT(pchunk != C_NULL);

    if (pchunk->next != C_NULL)
    {
        pchunk->next->prev = pchunk->prev;
    }

    /* If pchunk was the first chunk in the free list, update the heap ptr */
    if (pchunk->prev == C_NULL)
    {
        pmHeap.pfreelist = pchunk->next;
    }
    else
    {
        pchunk->prev->next = pchunk->next;
    }

    return PM_RET_OK;
}


/* Inserts in order a chunk into the free list.  Caller adjusts heap state */
static PmReturn_t
heap_linkToFreelist(pPmHeapDesc_t pchunk)
{
    uint16_t size;
    pPmHeapDesc_t pscan;

    /* If free list is empty, add to head of list */
    if (pmHeap.pfreelist == C_NULL)
    {
        pmHeap.pfreelist = pchunk;
        pchunk->next = C_NULL;
        pchunk->prev = C_NULL;

        return PM_RET_OK;
    }

    /* Scan free list for insertion point */
    pscan = pmHeap.pfreelist;
    size = HD_GET_SIZE(pchunk);
    while ((HD_GET_SIZE(pscan) < size) && (pscan->next != C_NULL))
    {
        pscan = pscan->next;
    }

    /* 
     * Insert chunk after the scan chunk (next is NULL).
     * This is a slightly rare case where the last chunk in the free list 
     * is smaller than the chunk being freed.
     */
    if (size > HD_GET_SIZE(pscan))
    {
        pchunk->next = pscan->next;
        pscan->next = pchunk;
        pchunk->prev = pscan;
    }

    /* Insert chunk before the scan chunk */
    else
    {
        pchunk->next = pscan;
        pchunk->prev = pscan->prev;

        /* If chunk will be first item in free list */
        if (pscan->prev == C_NULL)
        {
            pmHeap.pfreelist = pchunk;
        }
        else
        {
            pscan->prev->next = pchunk;
        }
        pscan->prev = pchunk;
    }

    return PM_RET_OK;
}


/*
 * Initializes the heap state variables
 */
PmReturn_t
heap_init(void)
{
    pPmHeapDesc_t pchunk;

    /* Create one big chunk */
    pchunk = (pPmHeapDesc_t)pmHeap.base;
    HD_SET_FREE(pchunk, (uint8_t)1);
    HD_SET_SIZE(pchunk, HEAP_SIZE);
    pchunk->next = C_NULL;
    pchunk->prev = C_NULL;


    /* Init heap globals */
    pmHeap.pfreelist = pchunk;
    pmHeap.avail = HEAP_SIZE;
    pmHeap.gcval = (uint8_t)0;

    string_cacheInit();

    return PM_RET_OK;
}


/**
 * Obtains a chunk of memory from the free list
 *
 * Performs the Best Fit algorithm.
 * Iterates through the freelist to see if a chunk of suitable size exists.
 * Shaves a chunk to perfect size iff the remainder is greater than
 * the minimum chunk size.
 *
 * @param size Requested chunk size
 * @param r_pchunk Return ptr to chunk
 * @return Return status
 */
static PmReturn_t
heap_getChunkImpl(uint16_t size, uint8_t **r_pchunk)
{
    PmReturn_t retval;
    pPmHeapDesc_t pchunk;
    pPmHeapDesc_t premainderChunk;

    C_ASSERT(r_pchunk != C_NULL);

    /* Skip to the first chunk that can hold the requested size */
    pchunk = pmHeap.pfreelist;
    while ((pchunk != C_NULL) && (HD_GET_SIZE(pchunk) < size))
    {
        pchunk = pchunk->next;
    }

    /* No chunk of appropriate size was found, raise OutOfMemory exception */
    if (pchunk == C_NULL)
    {
        *r_pchunk = C_NULL;
        PM_RAISE(retval, PM_RET_EX_MEM);
        return retval;
    }

    /* Remove the chunk from the free list */
    retval = heap_unlinkFromFreelist(pchunk);
    PM_RETURN_IF_ERROR(retval);

    /* Check if a chunk should be carved from what is available */
    if (HD_GET_SIZE(pchunk) - size >= HEAP_MIN_CHUNK_SIZE)
    {
        /* Create the heap descriptor for the remainder chunk */
        premainderChunk = (pPmHeapDesc_t)((uint8_t *)pchunk + size);
        HD_SET_FREE(premainderChunk, 1);
        HD_SET_SIZE(premainderChunk, HD_GET_SIZE(pchunk) - size);

        /* Put the remainder chunk back in the free list */
        retval = heap_linkToFreelist(premainderChunk);
        PM_RETURN_IF_ERROR(retval);

        /* Convert the chunk from a heap descriptor to an object descriptor */
        HD_SET_FREE(pchunk, 0);
        OBJ_SET_SIZE(((pPmObj_t)pchunk), size);
    }
    else
    {
        /* Set chunk's type to none (overwrites size field's high byte) */
        HD_SET_FREE(pchunk, 0);
        OBJ_SET_TYPE((pPmObj_t)pchunk, OBJ_TYPE_NON);
    }

    /*
     * Set the chunk's GC mark so it will be collected on next GC cycle
     * if it is not reachable
     */
    OBJ_SET_GCVAL((pPmObj_t)pchunk, pmHeap.gcval);

    /* Reduce the amount of available memory */
    pmHeap.avail -= OBJ_GET_SIZE((pPmObj_t)pchunk);

    /* Return the chunk */
    *r_pchunk = (uint8_t *)pchunk;

    return retval;
}


/*
 * Allocates chunk of memory.
 * Filters out invalid sizes.
 * Rounds the size up to the next multiple of 4.
 * Obtains a chunk of at least the desired size.
 */
PmReturn_t
heap_getChunk(uint16_t requestedsize, uint8_t **r_pchunk)
{
    PmReturn_t retval;
    uint16_t adjustedsize;

    /* Ensure size request is valid */
    if (requestedsize > HEAP_MAX_CHUNK_SIZE)
    {
        PM_RAISE(retval, PM_RET_EX_MEM);
        return retval;
    }

    if (requestedsize < HEAP_MIN_CHUNK_SIZE)
    {
        requestedsize = HEAP_MIN_CHUNK_SIZE;
    }

    /*
     * Round up the size to a multiple of 4 bytes.
     * This maintains alignment on 32-bit platforms (required).
     */
    adjustedsize = ((requestedsize + 3) & ~3);

    /* Attempt to get a chunk */
    retval = heap_getChunkImpl(adjustedsize, r_pchunk);

    /* Ensure that the pointer is 4-byte aligned */
    C_ASSERT(((int)*r_pchunk & 3) == 0);

    return retval;
}


/* Releases chunk to the free list */
PmReturn_t
heap_freeChunk(pPmObj_t ptr)
{
    PmReturn_t retval;

    /* Ensure the chunk falls within the heap */
    C_ASSERT(((uint8_t *)ptr >= pmHeap.base)
             && ((uint8_t *)ptr < pmHeap.base + HEAP_SIZE));

    /* Insert the chunk into the freelist */
    OBJ_SET_FREE(ptr, 1);

    /* Clear type so that heap descriptor's size's upper byte is zero */
    OBJ_SET_TYPE(ptr, 0);
    retval = heap_linkToFreelist((pPmHeapDesc_t)ptr);
    PM_RETURN_IF_ERROR(retval);

    /* Increase heap available amount */
    pmHeap.avail += OBJ_GET_SIZE(ptr);
    return retval;
}


/* Returns, by reference, the number of bytes available in the heap */
PmReturn_t
heap_getAvail(uint16_t *r_avail)
{
    *r_avail = pmHeap.avail;
    return PM_RET_OK;
}
