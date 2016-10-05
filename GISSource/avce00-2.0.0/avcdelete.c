/**********************************************************************
 * $Id: avcdelete.c,v 1.6 2006/08/17 20:21:51 dmorissette Exp $
 *
 * Name:     avcdelete.c
 * Project:  Arc/Info Vector coverage (AVC) BIN<->E00 conversion library
 * Language: ANSI C
 * Purpose:  Delete an Arc/Info binary coverage.
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
 * $Log: avcdelete.c,v $
 * Revision 1.6  2006/08/17 20:21:51  dmorissette
 * Updated email address
 *
 * Revision 1.5  2005/06/03 03:55:41  daniel
 * Update URL to website
 *
 * Revision 1.4  2005/06/03 03:49:59  daniel
 * Update email address, website url, and copyright dates
 *
 * Revision 1.3  2000/09/26 20:21:04  daniel
 * Added AVCCoverPC write
 *
 * Revision 1.2  1999/12/05 05:28:49  daniel
 * Updated usage info with build date
 *
 * Revision 1.1  1999/08/23 17:15:48  daniel
 * *** empty log message ***
 *
 **********************************************************************/

#include "avc.h"

/**********************************************************************
 *                          main()
 *
 * This program properly deletes an Arc/Info vector coverage and 
 * associated INFO tables.
 **********************************************************************/
int main(int argc, char *argv[])
{
    const char  *pszCoverPath;

/*---------------------------------------------------------------------
 *      Read program arguments.
 *--------------------------------------------------------------------*/
    if (argc != 2)
    {
        printf("\n");
        printf("AVCDELETE - Version %s\n", AVC_VERSION);
        printf("     Delete an Arc/Info vector coverage and associated INFO files\n");
        printf("     Copyright (c) 1999-2005, Daniel Morissette (dmorissette@mapgears.com)\n");
        printf("     AVCE00 web page:  http://avce00.maptools.org/\n");
        printf("\n");
        printf("Usage: avcdelete <coverage_path>\n");
        printf("\n");
        return 1;
    }

    pszCoverPath = argv[1];

/*---------------------------------------------------------------------
 *     Delete requested coverage
 *--------------------------------------------------------------------*/

    return AVCE00DeleteCoverage(pszCoverPath);
}
