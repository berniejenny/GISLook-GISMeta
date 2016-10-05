/**********************************************************************
 * $Id: avcexport.c,v 1.9 2006/08/17 20:21:51 dmorissette Exp $
 *
 * Name:     avcexport.c
 * Project:  Arc/Info Vector coverage (AVC) BIN->E00 conversion library
 * Language: ANSI C
 * Purpose:  Convert an Arc/Info binary coverage to E00.
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
 * $Log: avcexport.c,v $
 * Revision 1.9  2006/08/17 20:21:51  dmorissette
 * Updated email address
 *
 * Revision 1.8  2005/06/03 03:55:41  daniel
 * Update URL to website
 *
 * Revision 1.7  2005/06/03 03:49:59  daniel
 * Update email address, website url, and copyright dates
 *
 * Revision 1.6  2000/09/26 20:21:04  daniel
 * Added AVCCoverPC write
 *
 * Revision 1.5  1999/12/05 05:28:59  daniel
 * Updated usage info with build date
 *
 * Revision 1.4  1999/05/11 02:11:35  daniel
 * Renamed from avcconv.c to avcexport.c
 *
 * Revision 1.3  1999/02/25 03:34:48  daniel
 * Fixed the Revsion field in the "usage" output
 *
 * Revision 1.2  1999/02/25 03:32:25  daniel
 * Added support for output_file arg.
 *
 **********************************************************************/

#include "avc.h"


static void ConvertCover(const char *pszFname, FILE *fpOut);

/**********************************************************************
 *                          main()
 *
 * This program converts an Arc/Info vector coverage from the native
 * binary format to E00.
 **********************************************************************/
int main(int argc, char *argv[])
{
    const char  *pszCoverPath, *pszOutFile;
    FILE        *fpOut;

/*---------------------------------------------------------------------
 *      Read program arguments.
 *--------------------------------------------------------------------*/
    if (argc<3)
    {
        printf("\n");
        printf("AVCEXPORT - Version %s\n", AVC_VERSION);
        printf("     Convert an Arc/Info vector coverage from binary to E00.\n");
        printf("     Copyright (c) 1999-2005, Daniel Morissette (dmorissette@mapgears.com)\n");
        printf("     AVCE00 web page:  http://avce00.maptools.org/\n");
        printf("\n");
        printf("Usage: avcexport <coverage_path> <output_file>\n");
        printf("\n");
        return 1;
    }

    pszCoverPath = argv[1];
    pszOutFile   = argv[2];

/*---------------------------------------------------------------------
 *     Open output file... passing "-" will send output to stdout
 *-------------------------------------------------------------------*/
    if (strcmp(pszOutFile, "-") == 0)
    {
        fpOut = stdout;
    }
    else
    {
        fpOut = fopen(pszOutFile, "wt");

        if (fpOut == NULL)
        {
            perror(CPLSPrintf("avcexport: Cannot create %s", pszOutFile));
            return 1;
        }
    }

/*---------------------------------------------------------------------
 *     Convert the whole coverage to E00
 *-------------------------------------------------------------------*/
    ConvertCover(pszCoverPath, fpOut);

/*---------------------------------------------------------------------
 *     Close output file and exit.
 *-------------------------------------------------------------------*/
    if (strcmp(pszOutFile, "-") != 0)
    {
        fclose(fpOut);
    }

    return 0;
}


/**********************************************************************
 *                          ConvertCover()
 *
 * Convert a complete coverage to E00.
 **********************************************************************/
static void ConvertCover(const char *pszFname, FILE *fpOut)
{
    AVCE00ReadPtr hReadInfo;
    const char *pszLine;

    hReadInfo = AVCE00ReadOpen(pszFname);

    if (hReadInfo)
    {
        while ((pszLine = AVCE00ReadNextLine(hReadInfo)) != NULL)
        {
            fprintf(fpOut, "%s\n", pszLine);
        }

        AVCE00ReadClose(hReadInfo);
    }
}
