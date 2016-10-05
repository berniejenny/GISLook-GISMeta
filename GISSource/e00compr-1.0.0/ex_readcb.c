/**********************************************************************
 *                          ex_readcb.c
 *
 * This example program illustrates the use of the E00ReadCallbackOpen()
 * and associated compressed E00 read functions.
 **********************************************************************/

#include <stdio.h>

#include "e00compr.h"

static const char *myReadNextLine(void *pRefData);
static void       myReadRewind(void *pRefData);

int main(int argc, char *argv[])
{
    E00ReadPtr  hReadPtr;
    const char  *pszLine;
    FILE        *fp;

    /* Open input file */
    if ((fp = fopen("test.e00", "rt")) == NULL)
    {
        /* Error */
        printf("Cannot open input file test.e00\n");
        return 1;
    }

    /* Initialize reader */
    hReadPtr = E00ReadCallbackOpen((void*)fp, 
                                   myReadNextLine, myReadRewind);

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

        fclose(fp);
    }
    else
    {
        /* ERROR ... file is not a valid E00 */
    }

    return 0;
}



/**********************************************************************
 *                          myReadNextLine()
 *
 * My own implementation of the ReadNextLine() function to test the 
 * E00ReadCallbackOpen() functions.
 * 
 * This function must return a reference to static buffer with the next
 * line of input, or NULL when it reaches EOF.
 **********************************************************************/
static const char *myReadNextLine(void *pRefData)
{
    FILE *fp;
    static char szBuf[256];

    fp = (FILE *)pRefData;

    if (fgets(szBuf, 255, fp) == NULL)
        return NULL;

    szBuf[255] = '\0';

    return szBuf;
}

/**********************************************************************
 *                          myReadRewind()
 *
 * Callback function to rewind the file being read by myReadNextLine()
 **********************************************************************/
static void myReadRewind(void *pRefData)
{
    rewind((FILE *)pRefData);
}


