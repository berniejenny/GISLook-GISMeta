/**********************************************************************
 *                          ex_write.c
 *
 * This example program illustrates the use of the E00WriteOpen()
 * and associated compressed E00 write functions.
 **********************************************************************/
#include <stdio.h>

#include "e00compr.h"

int main(int argc, char *argv[])
{
    E00ReadPtr  hReadPtr;
    E00WritePtr hWritePtr;
    const char  *pszLine;
    int         nStatus = 0;

    /* Open input file */
    hReadPtr = E00ReadOpen("test1.e00");

    if (hReadPtr)
    {
        /* Open output file */
        hWritePtr = E00WriteOpen("test2.e00", E00_COMPR_FULL);

        if (hWritePtr)
        {
            /* Read lines from input until we reach EOF */
            while((pszLine = E00ReadNextLine(hReadPtr)) != NULL)
            {
                if ((nStatus = E00GetLastErrorNo()) == 0)
                    nStatus = E00WriteNextLine(hWritePtr, pszLine);

                if (nStatus != 0)
                {
                    /* An error happened while converting the last 
                     * line... abort*/
                    break;
                }
            }
            /* Close output file. */
            E00WriteClose(hWritePtr);
        }
        else
        {
            /* ERROR ... failed to open output file */
            nStatus = E00GetLastErrorNo();
        }

        /* Close input file. */
        E00ReadClose(hReadPtr);
    }
    else
    {
        /* ERROR ... failed to open input file */
        nStatus = E00GetLastErrorNo();
    }

    return nStatus;
}
