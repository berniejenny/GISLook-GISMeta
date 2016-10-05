/**********************************************************************
 *                          ex_avcwrite.c
 *
 * This example program illustrates the use of the AVCE00WriteOpen()
 * and the functions to use to write a binary coverage from E00 input.
 **********************************************************************/
#include <stdio.h>

#include "avc.h"

int main(int argc, char *argv[])
{
    FILE           *fpIn;
    AVCE00WritePtr hWritePtr;
    const char     *pszLine, *pszInFile, *pszOutCover;
    int            nStatus = 0;

    if (argc < 3)
    {
        printf("Usage: ex_avcwrite <e00_file> <cover_path>\n");
        return 1;
    }

    pszInFile = argv[1];
    pszOutCover = argv[2];

    /* Open input file */
    fpIn = fopen(pszInFile, "rt");

    if (fpIn)
    {
        /* Open output file */
        hWritePtr = AVCE00WriteOpen(pszOutCover, AVCCoverV7, AVC_DEFAULT_PREC);

        if (hWritePtr)
        {
            /* Read lines from input until we reach EOF */
            while((pszLine = CPLReadLine(fpIn)) != NULL)
            {
                if ((nStatus = CPLGetLastErrorNo()) == 0)
                    nStatus = AVCE00WriteNextLine(hWritePtr, pszLine);

                if (nStatus != 0)
                {
                    /* An error happened while converting the last 
                     * line... abort*/
                    break;
                }
            }
            /* Close output file. */
            AVCE00WriteClose(hWritePtr);
        }
        else
        {
            /* ERROR ... failed to open output file */
            nStatus = CPLGetLastErrorNo();
        }

        /* Close input file. */
        fclose(fpIn);
    }
    else
    {
        /* ERROR ... failed to open input file */
        nStatus = CPLGetLastErrorNo();
    }

    return nStatus;
}
