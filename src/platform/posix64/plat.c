/*
# This file is Copyright 2006, 2007, 2009 Dean Hall.
#
# This file is part of the Python-on-a-Chip program.
# Python-on-a-Chip is free software: you can redistribute it and/or modify
# it under the terms of the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1.
#
# Python-on-a-Chip is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# A copy of the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1
# is seen in the file COPYING up one directory from this.
*/


#undef __FILE_ID__
#define __FILE_ID__ 0x70


/** PyMite platform-specific routines for Desktop target */


#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "pm.h"


void
_sigalrm_handler(int signal)
{
    PmReturn_t retval;
    retval = pm_vmPeriodic(1000);
    PM_REPORT_IF_ERROR(retval);
}


/* Desktop target shall use stdio for I/O routines. */
PmReturn_t
plat_init(void)
{
    /* Let POSIX' SIGALRM fire every full millisecond. */
    /*
     * #67 Using sigaction complicates the use of getchar (below),
     * so signal() is used instead.
     */
/*#ifndef __DEBUG__*/
/* This block was ifndef'd because of a verbal warning that signals cause trouble when debugging */
    signal(SIGALRM, _sigalrm_handler);
    ualarm(1000, 1000);
/*#endif*/

    mod_setPlatLoadCodeObjectFunctionPointer(plat_loadCodeObject);

    return PM_RET_OK;
}


/* Disables the peripherals and interrupts */
PmReturn_t
plat_deinit(void)
{
    /* Cancel alarm and set the alarm handler to the default */
    ualarm(0, 0);
    signal(SIGALRM, SIG_DFL);

    return PM_RET_OK;
}


/* Desktop target shall use stdio for I/O routines */
PmReturn_t
plat_getByte(uint8_t *b)
{
    int c;
    PmReturn_t retval = PM_RET_OK;

    c = getchar();
    *b = c & 0xFF;

    if (c == EOF)
    {
        PM_RAISE(retval, PM_RET_EX_IO);
    }

    return retval;
}


/* Desktop target shall use stdio for I/O routines */
PmReturn_t
plat_putByte(uint8_t b)
{
    int i;
    PmReturn_t retval = PM_RET_OK;

    i = putchar(b);
    fflush(stdout);

    if ((i != b) || (i == EOF))
    {
        PM_RAISE(retval, PM_RET_EX_IO);
    }

    return retval;
}


PmReturn_t
plat_getMsTicks(uint32_t *r_ticks)
{
    *r_ticks = pm_timerMsTicks;

    return PM_RET_OK;
}


void
plat_reportError(PmReturn_t result)
{

#ifdef HAVE_DEBUG_INFO
#define LEN_FNLOOKUP 26
#define LEN_EXNLOOKUP 18

    uint8_t res;
    pPmFrame_t pframe;
    pPmObj_t pstr;
    pPmObj_t pfnstr;
    PmReturn_t retval;
    uint16_t bcaddr;
    uint16_t lineno;
    uint16_t len_lnotab;
    uint16_t lnotab_index;
    uint8_t b1, b2;
    uint16_t i;

    /* This table should match src/vm/fileid.txt */
    char const * const fnlookup[LEN_FNLOOKUP] = {
        "<no file>",
        "codeobj.c",
        "dict.c",
        "frame.c",
        "func.c",
        "global.c",
        "heap.c",
        "img.c",
        "int.c",
        "interp.c",
        "pm_generated_objs.c",
        "list.c",
        "main.c",
        "mem.c",
        "module.c",
        "obj.c",
        "seglist.c",
        "sli.c",
        "strobj.c",
        "tuple.c",
        "seq.c",
        "pm.c",
        "thread.c",
        "float.c",
        "class.c",
        "bytearray.c",
    };

    /* This table should match src/vm/pm.h PmReturn_t */
    char const * const exnlookup[LEN_EXNLOOKUP] = {
        "Exception",
        "SystemExit",
        "IoError",
        "ZeroDivisionError",
        "AssertionError",
        "AttributeError",
        "ImportError",
        "IndexError",
        "KeyError",
        "MemoryError",
        "NameError",
        "SyntaxError",
        "SystemError",
        "TypeError",
        "ValueError",
        "StopIteration",
        "Warning",
        "OverflowError",
    };

    /* Print traceback */
    printf("Traceback (most recent call first):\n");

    /* Get the top frame */
    pframe = gVmGlobal.pthread->pframe;

    /* If it's the native frame, print the native function name */
    if (pframe == (pPmFrame_t)&(gVmGlobal.nativeframe))
    {
        /* Get the native func's name */
        retval = co_getName((pPmObj_t)gVmGlobal.nativeframe.nf_func->f_co, &pstr);
        if ((retval) != PM_RET_OK)
        {
            printf("  Unable to get native func name.\n");
            return;
        }
        else
        {
            printf("  %s() __NATIVE__\n", ((pPmString_t)pstr)->val);
        }

        /* Get the frame that called the native frame */
        pframe = (pPmFrame_t)gVmGlobal.nativeframe.nf_back;
    }

    /* Print the remaining frame stack */
    for (; pframe != C_NULL; pframe = pframe->fo_back)
    {
        /* The last name in the names tuple of the code obj is the name */
        retval = co_getName((pPmObj_t)pframe->fo_func->f_co, &pstr);
        if ((retval) != PM_RET_OK) break;

        /*
         * Get the line number of the current bytecode. Algorithm comes from:
         * http://svn.python.org/view/python/trunk/Objects/lnotab_notes.txt?view=markup
         */
        lnotab_index = 0;
        bcaddr = 0;
        co_getLnotabLen((pPmObj_t)pframe->fo_func->f_co, &len_lnotab);
        co_getFirstlineno((pPmObj_t)pframe->fo_func->f_co, &lineno);
        for (i = 0; i < len_lnotab; i += 2)
        {
            co_getLnotabAtOffset((pPmObj_t)pframe->fo_func->f_co,
                                 lnotab_index++, &b1);
            bcaddr += b1;
            if (bcaddr > pframe->fo_ip) break;

            co_getLnotabAtOffset((pPmObj_t)pframe->fo_func->f_co,
                                 lnotab_index++, &b2);
            lineno += b2;
        }

        co_getFileName((pPmObj_t)((pPmFrame_t)pframe)->fo_func->f_co, &pfnstr);
        printf("  File \"%s\", line %d, in %s\n",
               ((pPmString_t)pfnstr)->val,
               lineno,
               ((pPmString_t)pstr)->val);
    }

    /* Print error */
    if ((gVmGlobal.errFileId > 0) && (gVmGlobal.errFileId < LEN_FNLOOKUP))
    {
        printf("%s:", fnlookup[gVmGlobal.errFileId]);
    }
    else
    {
        printf("FileId 0x%02X line ", gVmGlobal.errFileId);
    }
    printf("%d detects a ", gVmGlobal.errLineNum);

    res = (uint8_t)result;
    if ((res > 0) && ((res - PM_RET_EX) < LEN_EXNLOOKUP))
    {
        printf("%s\n", exnlookup[res - PM_RET_EX]);
    }
    else
    {
        printf("Error code 0x%02X\n", result);
    }


#else /* HAVE_DEBUG_INFO */

    /* Print error */
    printf("Error:     0x%02X\n", result);
    printf("  Release: 0x%02X\n", gVmGlobal.errVmRelease);
    printf("  FileId:  0x%02X\n", gVmGlobal.errFileId);
    printf("  LineNum: %d\n", gVmGlobal.errLineNum);

    /* Print traceback */
    {
        pPmObj_t pframe;
        pPmObj_t pstr;
        PmReturn_t retval;

        printf("Traceback (top first):\n");

        /* Get the top frame */
        pframe = (pPmObj_t)gVmGlobal.pthread->pframe;

        /* If it's the native frame, print the native function name */
        if (pframe == (pPmObj_t)&(gVmGlobal.nativeframe))
        {

            /* The last name in the names tuple of the code obj is the name */
            retval = tuple_getItem((pPmObj_t)gVmGlobal.nativeframe.nf_func->
                                   f_co->co_names, -1, &pstr);
            if ((retval) != PM_RET_OK)
            {
                printf("  Unable to get native func name.\n");
                return;
            }
            else
            {
                printf("  %s() __NATIVE__\n", ((pPmString_t)pstr)->val);
            }

            /* Get the frame that called the native frame */
            pframe = (pPmObj_t)gVmGlobal.nativeframe.nf_back;
        }

        /* Print the remaining frame stack */
        for (;
             pframe != C_NULL;
             pframe = (pPmObj_t)((pPmFrame_t)pframe)->fo_back)
        {
            /* Get the func's name */
            retval = co_getName((pPmObj_t)((pPmFrame_t)pframe)->fo_func->f_co, &pstr);
            PM_BREAK_IF_ERROR(retval);

            printf("  %s()\n", ((pPmString_t)pstr)->val);
        }
        printf("  <module>.\n");
    }
#endif /* HAVE_DEBUG_INFO */
}


/*
 * The Posix64 platform looks in the current directory of the filesystem
 * to find a module of the matching name (with .pmm extension).
 * If one is found, it is assumed to contain a marshalled code object.
 */
PmReturn_t plat_loadCodeObject(pPmObj_t pname, pPmObj_t *r_cob)
{
    char cwd[FILENAME_MAX];
    char *ext = PM_MARSHAL_FILE_EXTENSION;
    char *pchar;
    uint16_t len;
    FILE *fp;
    long fsize;
    struct stat stbuf;
    int fd;
    uint8_t *pchunk;
    PmReturn_t retval;
    uint8_t objid;

    *r_cob = C_NULL;

    /* Get the current directory or return failure */
    if (getcwd(cwd, FILENAME_MAX) == C_NULL)
    {
        return PM_RET_NO;
    }

    /* Scan to end of path */
    for (pchar = cwd; *pchar != '\0'; pchar++);

    /* Return failure if there is not enough room to hold the filename */
    len = ((pchar - cwd) + ((pPmString_t)pname)->length + sizeof(ext));
    if (len >= FILENAME_MAX)
    {
        return PM_RET_NO;
    }

    /* Append directory seperator and given name to the path */
    *pchar = '/'; pchar++;
    sli_memcpy(pchar, ((pPmString_t)pname)->val, ((pPmString_t)pname)->length);
    pchar += ((pPmString_t)pname)->length;

    /* Append p14p's marshal file extension and terminate */
    sli_memcpy(pchar, ext, sizeof(ext));
    pchar += sizeof(ext);
    *pchar = '\0';

    /* Return failure if the file won't open */
    fd = open(cwd, O_RDONLY);
    if (fd == -1)
    {
        return PM_RET_NO;
    }
    fp = fdopen(fd, "rb");
    if (fp == C_NULL)
    {
        close(fd);
        return PM_RET_NO;
    }

    /* Get the size of the file */
    if (fstat(fd, &stbuf) == -1) 
    {
        fclose(fp);
        close(fd);
        return PM_RET_NO;
    }
    fsize = stbuf.st_size;

    /* Return failure if file is too big or an exception if not enough memory */
    if ((fsize + sizeof(PmObjDesc_t)) > 65535) return PM_RET_NO;
    retval = heap_getChunk((uint16_t)(fsize + sizeof(PmObjDesc_t)), &pchunk);
    PM_RETURN_IF_ERROR(retval);

    heap_gcPushTempRoot((pPmObj_t)pchunk, &objid);
    {
        /*
         * Read the file into the buffer
         * (offset by sizeof(OD) so GC doesn't clobber the data)
         */
        if (fsize != fread(pchunk + sizeof(PmObjDesc_t), 1, fsize, fp))
        {
            fclose(fp);
            close(fd);
            heap_freeChunk((pPmObj_t)pchunk);
            heap_gcPopTempRoot(objid);
            return PM_RET_NO;
        }
        fclose(fp);
        close(fd);

        /* Un-marshal the file contents */
        retval = marshal_load(pchunk + sizeof(PmObjDesc_t), fsize, r_cob);
        PM_RETURN_IF_ERROR(retval);
    }
    heap_gcPopTempRoot(objid);
    heap_freeChunk((pPmObj_t)pchunk);

    /* Raise exception if it is not a Code Object */
    if (OBJ_GET_TYPE(*r_cob) != OBJ_TYPE_COB)
    {
        PM_RAISE(retval, PM_RET_EX_TYPE);
        return retval;
    }
    return PM_RET_OK;
}
