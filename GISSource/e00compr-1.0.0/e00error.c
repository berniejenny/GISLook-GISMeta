/**********************************************************************
 * $Id: e00error.c,v 1.6 2005/09/17 14:22:05 daniel Exp $
 *
 * Name:     e00error.c
 * Project:  Compressed E00 Read/Write library
 * Language: ANSI C
 * Purpose:  Error handling functions.
 * Author:   Daniel Morissette, dmorissette@dmsolutions.ca
 *
 * $Log: e00error.c,v $
 * Revision 1.6  2005/09/17 14:22:05  daniel
 * Switch to MIT license, update refs to website and email address, and
 * prepare for 1.0.0 release.
 *
 * Revision 1.5  1999/01/08 17:38:19  daniel
 * Changed E00Error argument "errno" to "nErrNo" to fix a warning with gcc
 *
 * Revision 1.4  1998/11/02 18:37:04  daniel
 * New file header, and added E00ErrorReset()
 *
 * Revision 1.1  1998/10/29 13:26:00  daniel
 * Initial revision
 *
 **********************************************************************
 * Copyright (c) 1998-2005, Daniel Morissette
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
 * 
 **********************************************************************/

#include <stdio.h>
#include <stdarg.h>

#include "e00compr.h"

/* static buffer to store the last error message.  We'll assume that error
 * messages cannot be longer than 2000 chars... which is quite reasonable
 * (that's 25 lines of 80 chars!!!)
 */
static char gszE00LastErrMsg[2000] = "";
static int  gnE00LastErrNo = 0;

static void (*gpfnE00ErrorHandler)(int, const char *) = NULL;

/**********************************************************************
 *                          E00Error()
 *
 * This function records an error code and displays the error message
 * to stderr.
 *
 * The error code can be accessed later using E00GetLastErrNo()
 **********************************************************************/
void    E00Error(int nErrNo, const char *pszFmt, ...)
{
    va_list args;

    gnE00LastErrNo = nErrNo;

    /* Expand the error message 
     */
    va_start(args, pszFmt);
    vsprintf(gszE00LastErrMsg, pszFmt, args);
    va_end(args);


    /* If the user provided his own error handling function, then call
     * it, otherwise print the error to stderr and return.
     */
    if (gpfnE00ErrorHandler != NULL)
    {
        gpfnE00ErrorHandler(nErrNo, gszE00LastErrMsg);
    }
    else
    {
        fprintf(stderr, "ERROR %d: %s\n", gnE00LastErrNo, gszE00LastErrMsg);
    }
}

/**********************************************************************
 *                          E00ErrorReset()
 *
 * Erase traces of previous errors.
 **********************************************************************/
void    E00ErrorReset()
{
    gnE00LastErrNo = 0;
    gszE00LastErrMsg[0] = '\0';
}

/**********************************************************************
 *                          E00GetLastErrorNo()
 *
 **********************************************************************/
int     E00GetLastErrorNo()
{
    return gnE00LastErrNo;
}

/**********************************************************************
 *                          E00GetLastErrorMsg()
 *
 **********************************************************************/
const char* E00GetLastErrorMsg()
{
    return gszE00LastErrMsg;
}

/**********************************************************************
 *                          E00SetErrorHandler()
 *
 * Let the library user specify his own error handler function.
 *
 * A valid error handler is a C function with the following prototype:
 *
 *     void MyErrorHandler(int nErrNo, const char *pszMsg)
 *
 * Pass NULL to come back to the default behavior.
 **********************************************************************/
void     E00SetErrorHandler(void (*pfnErrorHandler)(int, const char *))
{
    gpfnE00ErrorHandler = pfnErrorHandler;
}

