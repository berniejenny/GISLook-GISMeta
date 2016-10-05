/**********************************************************************
 * $Id: e00conv.c,v 1.7 2005/09/17 14:22:05 daniel Exp $
 *
 * Name:     e00conv.c
 * Project:  Compressed E00 Read/Write library
 * Language: ANSI C
 * Purpose:  Command-line program that uses the Compressed E00 
 *           Read/Write library to convert a E00 file from one level
 *           of compression to another.
 *           (Note: the current version only writes uncompressed E00)
 * Author:   Daniel Morissette, dmorissette@dmsolutions.ca
 *
 * $Log: e00conv.c,v $
 * Revision 1.7  2005/09/17 14:22:05  daniel
 * Switch to MIT license, update refs to website and email address, and
 * prepare for 1.0.0 release.
 *
 * Revision 1.6  1999/02/25 18:29:51  daniel
 * Now use CPL error handling and memory allocations
 *
 * Revision 1.5  1998/11/13 15:40:38  daniel
 * Added support for wirting compressed files.
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
#include <ctype.h>

#include "e00compr.h"

/**********************************************************************
 *                          main()
 *
 **********************************************************************/
int main(int argc, char *argv[])
{
    E00ReadPtr  hReadPtr;
    E00WritePtr hWritePtr;
    const char  *pszInFile = NULL, *pszOutFile = NULL;
    const char  *pszLine;
    int         nStatus = 0, nComprLevel = E00_COMPR_NONE;

    /* ----------------------------------------------------------------
     * Parse input parameters
     * ---------------------------------------------------------------*/

    if (argc >= 4)
    {
        /* 4th param is compression level (NONE, PARTIAL or FULL)
         * Default is NONE.  
         *
         * We simply check the first char: N, P or F
         */
        switch(toupper(argv[3][0]))
        {
          case 'F':
            nComprLevel = E00_COMPR_FULL;
            break;
          case 'P':
            nComprLevel = E00_COMPR_PARTIAL;
            break;
          case 'N':
          default:
            nComprLevel = E00_COMPR_NONE;
        }
    }

    if (argc >= 3)
    {
        /* Input and output file names
         */
        pszInFile = argv[1];
        pszOutFile = argv[2];
    }
    else
    {
        printf("\n");
        printf("E00CONV - Version %s\n", E00COMPR_VERSION);
        printf("    Converts Arc/Info E00 files from one level of compression to another.\n");
        printf("    Copyright (c) 1999-2005, Daniel Morissette (dmorissette@dmsolutions.ca)\n");
        printf("    E00COMPR web page:  http://avce00.maptools.org/\n");
        printf("\n");
        printf("Usage: e00conv <input_file> <output_file> [NONE|PARTIAL|FULL]\n");
        printf("\n");
        return 1;
    }

    /* ----------------------------------------------------------------
     * Open files and proceed with the conversion
     * ---------------------------------------------------------------*/
    hReadPtr = E00ReadOpen(pszInFile);

    if (hReadPtr)
    {
        hWritePtr = E00WriteOpen(pszOutFile, nComprLevel);

        if (hWritePtr)
        {
            /* Read lines from input until we reach EOF 
             */
            while((pszLine = E00ReadNextLine(hReadPtr)) != NULL)
            {
                if ((nStatus = CPLGetLastErrorNo()) == 0)
                    nStatus = E00WriteNextLine(hWritePtr, pszLine);

                if (nStatus != 0)
                {
                    /* An error happened while converting the last 
                     * line... abort*/
                    break;
                }
            }

            /* Close files.
             */
            E00WriteClose(hWritePtr);
        }
        else
            nStatus = CPLGetLastErrorNo();

        E00ReadClose(hReadPtr);
    }
    else
        nStatus = CPLGetLastErrorNo();

    return nStatus;
}

