/******************************************************************************
 * Copyright (c) 1998, Frank Warmerdam
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 ******************************************************************************
 *
 * cpl_port.h
 *
 * Include file providing low level portability services for CPL.  This
 * should be the first include file for any CPL based code.  It provides the
 * following:
 *
 * o Includes some standard system include files, such as stdio, and stdlib.
 *
 * o Defines CPL_C_START, CPL_C_END macros.
 *
 * o Ensures that some other standard macros like NULL are defined.
 *
 * o Defines some portability stuff like CPL_MSB, or CPL_LSB.
 *
 * o Ensures that core types such as GBool, GInt32, GInt16, GUInt32, 
 *   GUInt16, and GByte are defined.
 *
 * $Log: cpl_port.h,v $
 * Revision 1.1  1999/08/27 14:09:55  daniel
 * Update CPL files
 *
 * Revision 1.13  1999/05/20 02:54:38  warmerda
 * Added API documentation
 *
 * Revision 1.12  1999/05/14 20:35:03  warmerda
 * added some more swapping macros
 *
 * Revision 1.11  1999/05/13 19:19:06  warmerda
 * Only use dbmalloc if DEBUG is set.
 *
 * Revision 1.10  1999/03/02 21:08:11  warmerda
 * autoconf switch
 *
 * Revision 1.9  1999/02/17 01:41:17  warmerda
 * Added NULL.
 *
 * Revision 1.8  1999/02/02 21:32:38  warmerda
 * Added CPL_{MSB,LSB}WORD{16,32} macros.
 *
 * Revision 1.7  1999/02/02 19:02:36  warmerda
 * Removed duplicates of base types, and CPL_LSB
 *
 * Revision 1.6  1999/01/28 18:36:06  warmerda
 * Ensure WIN32 is defined on Windows.
 *
 * Revision 1.5  1999/01/28 05:26:12  danmo
 * Added byte swapping macros.
 *
 * Revision 1.4  1998/12/15 19:05:30  warmerda
 * added errno.h
 *
 * Revision 1.3  1998/12/14 04:50:07  warmerda
 * Added DBMALLOC support
 *
 * Revision 1.2  1998/12/04 21:38:40  danmo
 * Changed str*casecmp() to str*icmp() for WIN32
 *
 * Revision 1.1  1998/12/03 18:26:02  warmerda
 * New
 *
 */

#ifndef CPL_BASE_H_INCLUDED
#define CPL_BASE_H_INCLUDED

/**
 * \file cpl_port.h
 *
 * Core portability definitions for CPL.
 *
 */

/* ==================================================================== */
/*      We will use WIN32 as a standard windows define.                 */
/* ==================================================================== */
#if defined(_WIN32) && !defined(WIN32)
#  define WIN32
#endif

/* ==================================================================== */
/*	Standard include files.						*/
/* ==================================================================== */

#include "cpl_config.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>

#if defined(HAVE_LIBDBMALLOC) && defined(HAVE_DBMALLOC_H) && defined(DEBUG)
#  define DBMALLOC
#  include <dbmalloc.h>
#endif

/* ==================================================================== */
/*      Base portability stuff ... this stuff may need to be            */
/*      modified for new platforms.                                     */
/* ==================================================================== */

/*---------------------------------------------------------------------
 *        types for 16 and 32 bits integers, etc...
 *--------------------------------------------------------------------*/
#if UINT_MAX == 65535
typedef long            GInt32;
typedef unsigned long   GUInt32;
#else
typedef int             GInt32;
typedef unsigned int    GUInt32;
#endif

typedef short           GInt16;
typedef unsigned short  GUInt16;
typedef unsigned char   GByte;
typedef int             GBool;


/* ==================================================================== */
/*      Other standard services.                                        */
/* ==================================================================== */
#ifdef __cplusplus
#  define CPL_C_START		extern "C" {
#  define CPL_C_END		}
#else
#  define CPL_C_START
#  define CPL_C_END
#endif

/* #  define CPL_DLL     __declspec(dllexport) */

#define CPL_DLL

#ifndef NULL
#  define NULL	0
#endif

#ifndef FALSE
#  define FALSE	0
#endif

#ifndef TRUE
#  define TRUE	1
#endif

#ifndef MAX
#  define MIN(a,b)      ((a<b) ? a : b)
#  define MAX(a,b)      ((a>b) ? a : b)
#endif

#ifndef NULL
#define NULL 0
#endif

#ifndef ABS
#  define ABS(x)        ((x<0) ? (-1*(x)) : x)
#endif

#ifndef EQUAL
#ifdef WIN32
#  define EQUALN(a,b,n)           (strnicmp(a,b,n)==0)
#  define EQUAL(a,b)              (stricmp(a,b)==0)
#else
#  define EQUALN(a,b,n)           (strncasecmp(a,b,n)==0)
#  define EQUAL(a,b)              (strcasecmp(a,b)==0)
#endif
#endif

/*---------------------------------------------------------------------
 *                         CPL_LSB and CPL_MSB
 * Only one of these 2 macros should be defined and specifies the byte 
 * ordering for the current platform.  
 * This should be defined in the Makefile, but if it is not then
 * the default is CPL_LSB (Intel ordering, LSB first).
 *--------------------------------------------------------------------*/
#if defined(WORDS_BIGENDIAN) && !defined(CPL_MSB) && !defined(CPL_LSB)
#  define CPL_MSB
#endif

#if ! ( defined(CPL_LSB) || defined(CPL_MSB) )
#define CPL_LSB
#endif

/*---------------------------------------------------------------------
 *        Little endian <==> big endian byte swap macros.
 *--------------------------------------------------------------------*/

#define CPL_SWAP16(x) \
        ((GUInt16)( \
            (((GUInt16)(x) & 0x00ffU) << 8) | \
            (((GUInt16)(x) & 0xff00U) >> 8) ))

#define CPL_SWAP32(x) \
        ((GUInt32)( \
            (((GUInt32)(x) & (GUInt32)0x000000ffUL) << 24) | \
            (((GUInt32)(x) & (GUInt32)0x0000ff00UL) <<  8) | \
            (((GUInt32)(x) & (GUInt32)0x00ff0000UL) >>  8) | \
            (((GUInt32)(x) & (GUInt32)0xff000000UL) >> 24) ))

#define CPL_SWAP32PTR(x) \
{								\
    GByte	byTemp, *pabyData = (GByte *) (x);		\
								\
    byTemp = pabyData[0];					\
    pabyData[0] = pabyData[3];					\
    pabyData[3] = byTemp;					\
    byTemp = pabyData[1];					\
    pabyData[1] = pabyData[2];					\
    pabyData[2] = byTemp;					\
}                                                                    
                                                            
#define CPL_SWAP64PTR(x) \
{								\
    GByte	byTemp, *pabyData = (GByte *) (x);		\
								\
    byTemp = pabyData[0];					\
    pabyData[0] = pabyData[7];					\
    pabyData[7] = byTemp;					\
    byTemp = pabyData[1];					\
    pabyData[1] = pabyData[6];					\
    pabyData[6] = byTemp;					\
    byTemp = pabyData[2];					\
    pabyData[2] = pabyData[5];					\
    pabyData[5] = byTemp;					\
    byTemp = pabyData[3];					\
    pabyData[3] = pabyData[4];					\
    pabyData[4] = byTemp;					\
}                                                                    
                                                            

/* Until we have a safe 64 bits integer data type defined, we'll replace
m * this version of the CPL_SWAP64() macro with a less efficient one.
 */
/*
#define CPL_SWAP64(x) \
        ((uint64)( \
            (uint64)(((uint64)(x) & (uint64)0x00000000000000ffULL) << 56) | \
            (uint64)(((uint64)(x) & (uint64)0x000000000000ff00ULL) << 40) | \
            (uint64)(((uint64)(x) & (uint64)0x0000000000ff0000ULL) << 24) | \
            (uint64)(((uint64)(x) & (uint64)0x00000000ff000000ULL) << 8) | \
            (uint64)(((uint64)(x) & (uint64)0x000000ff00000000ULL) >> 8) | \
            (uint64)(((uint64)(x) & (uint64)0x0000ff0000000000ULL) >> 24) | \
            (uint64)(((uint64)(x) & (uint64)0x00ff000000000000ULL) >> 40) | \
            (uint64)(((uint64)(x) & (uint64)0xff00000000000000ULL) >> 56) ))
*/

#define	CPL_SWAPDOUBLE(p) {                             \
        double _tmp = *(double *)(p);                     \
	((GByte *)(p))[0] = ((GByte *)&_tmp)[7];          \
	((GByte *)(p))[1] = ((GByte *)&_tmp)[6];          \
	((GByte *)(p))[2] = ((GByte *)&_tmp)[5];          \
	((GByte *)(p))[3] = ((GByte *)&_tmp)[4];          \
	((GByte *)(p))[4] = ((GByte *)&_tmp)[3];          \
	((GByte *)(p))[5] = ((GByte *)&_tmp)[2];          \
	((GByte *)(p))[6] = ((GByte *)&_tmp)[1];          \
	((GByte *)(p))[7] = ((GByte *)&_tmp)[0];          \
}

#ifdef CPL_MSB
#  define CPL_MSBWORD16(x)	(x)
#  define CPL_LSBWORD16(x)	CPL_SWAP16(x)
#  define CPL_MSBWORD32(x)	(x)
#  define CPL_LSBWORD32(x)	CPL_SWAP32(x)
#  define CPL_MSBPTR32(x)	
#  define CPL_LSBPTR32(x)	CPL_SWAP32PTR(x)
#else
#  define CPL_LSBWORD16(x)	(x)
#  define CPL_MSBWORD16(x)	CPL_SWAP16(x)
#  define CPL_LSBWORD32(x)	(x)
#  define CPL_MSBWORD32(x)	CPL_SWAP32(x)
#  define CPL_LSBPTR32(x)	
#  define CPL_MSBPTR32(x)	CPL_SWAP32PTR(x)
#endif

#endif /* ndef CPL_BASE_H_INCLUDED */
