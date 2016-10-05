/**********************************************************************
 *                          ex_writecb.c
 *
 * This example program illustrates the use of the E00WriteCallbackOpen()
 * and associated compressed E00 write functions.
 **********************************************************************/
#include <stdio.h>

#include "e00compr.h"

static int myWriteNextLine(void *pRefData, const char *pszLine);

int main(int argc, char *argv[])
{
    E00ReadPtr  hReadPtr;
    E00WritePtr hWritePtr = NULL;
    const char  *pszLine;
    FILE        *fp;
    int         nStatus = 0;

    /* Open input file */
    hReadPtr = E00ReadOpen("test1.e00");

    if (hReadPtr)
    {
        /* Open output file, and if succesful then initialize the writer
         * using the myWriteNextLine callback method.
         * For the purposes of this example, fp will be used directly
         * as reference data for the callback.
         */
        if ((fp = fopen("test2.e00", "wt")) != NULL)
        {
            hWritePtr = E00WriteCallbackOpen((void *)fp,
                                             myWriteNextLine,
                                             E00_COMPR_FULL);
        }

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

        /* Close the physical output file */
        if (fp)
            fclose(fp);

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


/**********************************************************************
 *                          myWriteNextLine()
 *
 * My own implementation of the ReadWriteNextLine() function to test the 
 * E00WriteCallbackOpen() functions.
 * 
 * This function writes a line to the E00 file.  The line that is passed
 * as argument won't be terminated by a '\n', it is the responsibility
 * of this function to add one if needed.
 *
 * myWriteNextLine() should return a positive value on success (the number
 * of chars written, as printf() does) or -1 if an error happened...
 **********************************************************************/
static int myWriteNextLine(void *pRefData, const char *pszLine)
{
    int nStatus;

    /* For the purpose of this example, pRefData is the actual FILE *
     * but it can be whatever you want...
     */

    nStatus =  fprintf((FILE *)pRefData, "%s\n", pszLine);

    return nStatus;
}
