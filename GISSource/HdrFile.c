#include "HdrFile.h"
#include <float.h>
#include <string.h>
#include <math.h>
#include "ReadRaster.h"
#include "strlwr.h"

bool readHeaderFile(FILE *f, 
					unsigned long *nrows,
					unsigned long *ncols,
					unsigned long *nbands,
					short *nbits,
					bool *byteOrderMotorola,
					int *layout,
					unsigned long *skipBytes,
					unsigned long *bandRowBytes,
					unsigned long *totalRowBytes,
					unsigned long *bandGapBytes,
					float *noDataValue) {
	
	// default values from ESRI ArcView help.
	*nrows = 0;
	*ncols = 0;
	*nbands	= 1;
	*nbits = 8;
	*byteOrderMotorola = false;
	*layout = bil;
	*skipBytes = 0;
	*bandRowBytes = 0;
	*totalRowBytes = *nbands * *bandRowBytes;
	*bandGapBytes = 0;
	*noDataValue = -9999;
	// bil, bip and bsq does not officially have a nodata value
	// -9999 is some kind of standard
	
	// longest possible string at beginning of row is 13 chars long
	char	c20[21];
	while (true)
	{
		if (fscanf(f, "%13s", c20) == EOF)
			break;	// reached end of file.
		strlwr (c20);
		
		if (strcmp(c20, "nrows") == 0)
		{
			if (fscanf(f, "%lu", nrows) == EOF)
				return FALSE;
			continue;
		}
		if (strcmp(c20, "ncols") == 0)
		{
			if (fscanf(f, "%lu", ncols) == EOF)
				return FALSE;
			continue;
		}
		if (strcmp(c20, "nbands") == 0)
		{
			if (fscanf(f, "%lu", nbands) == EOF)
				return FALSE;
			continue;
		}
		if (strcmp(c20, "nbits") == 0)
		{
			if (fscanf(f, "%hi", nbits) == EOF)
				return FALSE;
			continue;
		}
		if (strcmp(c20, "byteorder") == 0)
		{
			// Band-Header-ReadFile: M oder I
			// Binary-Grid-ReadFile: msbfirst oder lsbfirst
			if (fscanf(f, "%1s", c20) == EOF)
				return FALSE;
			*byteOrderMotorola = (c20[0] == 'M' || c20[0] == 'm');
			continue;
		}
		if (strcmp(c20, "layout") == 0)
		{
			if (fscanf(f, "%3s", c20) == EOF)
				return FALSE;
			strlwr (c20);
			if (strcmp(c20, "bil") == 0)
				*layout = bil;
			else if (strcmp(c20, "bip") == 0)
				*layout = bip;
			else if (strcmp(c20, "bsq") == 0)
				*layout = bsq;
			continue;
		}
		if (strcmp(c20, "skipbytes") == 0)
		{
			if (fscanf(f, "%lu", skipBytes) == EOF)
				return FALSE;
			continue;
		}
		/*
		 if (strcmp(c20, "ulxmap") == 0)		// center of upper left pixel
		 {
		 if (fscanf(f, "%lf", &x_) == EOF)
		 return FALSE;
		 continue;
		 }
		 if (strcmp(c20, "xllcorner") == 0)	// aus binary-grid header. Center of lower left pixel
		 {
		 if (fscanf(f, "%lf", &x_) == EOF)
		 return FALSE;
		 continue;
		 }
		 if (strcmp(c20, "ulymap") == 0)		// center of upper left pixel
		 {
		 if (fscanf(f, "%lf", &y_) == EOF)
		 return FALSE;
		 yIsLowerCorner = false;
		 continue;
		 }
		 if (strcmp(c20, "yllcorner") == 0)	// aus binary-grid header. Center of lower left pixel
		 {
		 if (fscanf(f, "%lf", &y_) == EOF)
		 return FALSE;
		 yIsLowerCorner = true;
		 continue;
		 }
		 if (strcmp(c20, "xdim") == 0)
		 {
		 if (fscanf(f, "%lf", &xdim) == EOF)
		 return FALSE;
		 xdim = ::fabs(xdim);
		 continue;
		 }
		 if (strcmp(c20, "ydim") == 0)
		 {
		 if (fscanf(f, "%lf", &ydim) == EOF)
		 return FALSE;
		 ydim = ::fabs(ydim);
		 continue;
		 }
		 */
		if (strcmp(c20, "cellsize") == 0)	// aus Esri-binary-grid header. 
			// Mit cellsize lässt sich ein binary grid header eindeutig identifizieren.
			// Deshalb werden bei dieser Gelegenheit weitere Werte gesetzt.
		{
			double xdim;
			if (fscanf(f, "%lf", &xdim) == EOF)
				return FALSE;
			//ydim = xdim = fabs(xdim);	// dx == dy
			*nbits = 32;		// binary grids sind 4 bytes
			*layout = esriBinary;
			//yIsLowerCorner = true;		// ESRI binary header uses lower left value (yllcorner)
			continue;
		}
		if (strcmp(c20, "bandrowbytes") == 0)
		{
			if (fscanf(f, "%lu", bandRowBytes) == EOF)
				return FALSE;
			continue;
		}
		if (strcmp(c20, "totalrowbytes") == 0)
		{
			if (fscanf(f, "%lu", totalRowBytes) == EOF)
				return FALSE;
			continue;
		}
		if (strcmp(c20, "bandgapbytes") == 0)
		{
			if (fscanf(f, "%lu", bandGapBytes) == EOF)
				return FALSE;
			continue;
		}
		if (strncmp(c20, "nodata", strlen("nodata")) == 0)	
			// nicht in ESRI-Dokumentation, taucht aber in GTOPO30 auf. In binary Grid als "nodata_value".
		{
			if (fscanf(f, "%f", noDataValue) == EOF)
				return FALSE;
			continue;
		}
		
		// found unsupported tag: spool to end of line
		while (true)
		{
			int c = getc (f);
			if (c == '\n' || c == '\r' || c == EOF)
				break;
		}
	}
	
	// bandRowBytes is only defined for BIL files
	if (*layout != bil)
		bandRowBytes = 0L;
	else if (*bandRowBytes == 0)
		*bandRowBytes = *ncols * *nbits / 8L + (unsigned long)((*ncols * *nbits % 8L) > 0);
	
	if (*totalRowBytes == 0) {
		if (*layout == bil)
			*totalRowBytes = *nbands * *bandRowBytes;
		else if (*layout == bip)
			*totalRowBytes = *ncols * *nbands * *nbits / 8L + (unsigned long)((*ncols * *nbands * *nbits) % 8L > 0L);
	}	
	if (*layout != bsq)
		*bandGapBytes = 0L;
	
	return TRUE;
}