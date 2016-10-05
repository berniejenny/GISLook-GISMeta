/**********************************************************************
 *                          ex_read.c
 *
 * This example program illustrates the use of the E00ReadOpen()
 * and associated compressed E00 read functions.
 **********************************************************************/

#include <stdio.h>

#include "e00compr.h"

int main(int argc, char *argv[])
{
    E00ReadPtr  hReadPtr;
    const char  *pszLine;

    /* Open input */
    hReadPtr = E00ReadOpen("test.e00");

    if (hReadPtr)
    {
        /* Read lines from input until we reach EOF */
        while((pszLine = E00ReadNextLine(hReadPtr)) != NULL)
        {
            if (E00GetLastErrorNo() == 0)
                printf("%s\n", pszLine);
            else
            {
                /* An error happened while reading the last line... */
                break;
            }
        }

        /* Close input file */
        E00ReadClose(hReadPtr);
    }
    else
    {
        /* ERROR ... failed to open input file */
    }

    return 0;
}
