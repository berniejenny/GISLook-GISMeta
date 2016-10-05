/**********************************************************************
 * $Id: avcimport.c,v 1.9 2006/08/17 20:21:51 dmorissette Exp $
 *
 * Name:     avcimport.c
 * Project:  Arc/Info Vector coverage (AVC) BIN<->E00 conversion library
 * Language: ANSI C
 * Purpose:  Convert an E00 file to an Arc/Info binary coverage.
 * Author:   Daniel Morissette, dmorissette@dmsolutions.ca
 *
 **********************************************************************
 * Copyright (c) 1999-2005, Daniel Morissette
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
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
 * DEALINGS IN THE SOFTWARE.
 **********************************************************************
 *
 * $Log: avcimport.c,v $
 * Revision 1.9  2006/08/17 20:21:51  dmorissette
 * Updated email address
 *
 * Revision 1.8  2005/06/03 03:55:41  daniel
 * Update URL to website
 *
 * Revision 1.7  2005/06/03 03:49:59  daniel
 * Update email address, website url, and copyright dates
 *
 * Revision 1.6  2000/09/26 20:21:05  daniel
 * Added AVCCoverPC write
 *
 * Revision 1.5  2000/09/22 19:45:21  daniel
 * Switch to MIT-style license
 *
 * Revision 1.4  1999/12/24 07:18:34  daniel
 * Added PC Arc/Info coverages support
 *
 * Revision 1.3  1999/12/05 05:29:08  daniel
 * Updated usage info with build date
 *
 * Revision 1.2  1999/05/17 16:25:02  daniel
 * Use AVC_DEFAULT_PREC
 *
 * Revision 1.1  1999/05/11 02:11:49  daniel
 * Initial revision
 *
 **********************************************************************/

#include "avc.h"


static void ConvertCover(FILE *fpIn, const char *pszCoverName, 
                         AVCCoverType eCoverType);

/**********************************************************************
 *                          main()
 *
 * This program converts an Arc/Info vector coverage from the native
 * binary format to E00.
 **********************************************************************/
int main(int argc, char *argv[])
{
    const char  *pszCoverPath, *pszInFile;
    FILE        *fpIn;
    AVCCoverType eCoverType = AVCCoverV7;

/*---------------------------------------------------------------------
 *      Read program arguments.
 *--------------------------------------------------------------------*/
    if (argc >= 4)
    {
        if (EQUAL(argv[3], "V7"))
            eCoverType = AVCCoverV7;
        else if (EQUAL(argv[3], "PC"))
            eCoverType = AVCCoverPC;
        else
        {
            printf("\nERROR: Invalid coverage type '%s'\n\n", argv[3]);
            argc=0;  /* Force display of usage information */
        }
    }

    if (argc<3)
    {
        printf("\n");
        printf("AVCIMPORT - Version %s\n", AVC_VERSION);
        printf("     Convert an Arc/Info vector coverage from E00 to binary.\n");
        printf("     Copyright (c) 1999-2005, Daniel Morissette (dmorissette@mapgears.com)\n");
        printf("     AVCE00 web page:  http://avce00.maptools.org/\n");
        printf("\n");
        printf("Usage: avcimport <input_file> <coverage_path> [<coverage_type>]\n");
        printf("\n");
        printf("  <coverage_type> is either 'V7' (the default) to write a Unix V7.x coverage,\n");
        printf("                  or 'PC' to write a PC/Arc coverage.\n");
        printf("\n");
        return 1;
    }

    pszInFile   = argv[1];
    pszCoverPath = argv[2];


/*---------------------------------------------------------------------
 *     Open output file... passing "-" will take input from stdin
 *-------------------------------------------------------------------*/
    if (strcmp(pszInFile, "-") == 0)
    {
        fpIn = stdin;
    }
    else
    {
        fpIn = fopen(pszInFile, "rt");

        if (fpIn == NULL)
        {
            perror(CPLSPrintf("avcimport: Cannot open %s", pszInFile));
            return 1;
        }
    }

/*---------------------------------------------------------------------
 *     Convert the whole E00 file to a binary coverage 
 *-------------------------------------------------------------------*/
    ConvertCover(fpIn, pszCoverPath, eCoverType);

/*---------------------------------------------------------------------
 *     Close input file and exit.
 *-------------------------------------------------------------------*/
    if (strcmp(pszInFile, "-") != 0)
    {
        fclose(fpIn);
    }

    return 0;
}


/**********************************************************************
 *                          ConvertCover()
 *
 * Create a binary coverage from an E00 file.
 *
 * It would be possible to have an option for the precision... coming soon!
 **********************************************************************/
static void ConvertCover(FILE *fpIn, const char *pszCoverName, 
                         AVCCoverType eCoverType)
{
    AVCE00WritePtr hWriteInfo;
    const char *pszLine;

    hWriteInfo = AVCE00WriteOpen(pszCoverName, eCoverType, AVC_DEFAULT_PREC);

    if (hWriteInfo)
    {
        while (CPLGetLastErrorNo() == 0 &&
               (pszLine = CPLReadLine(fpIn) ) != NULL )
        {
            AVCE00WriteNextLine(hWriteInfo, pszLine);
        }

        AVCE00WriteClose(hWriteInfo);
    }
}
