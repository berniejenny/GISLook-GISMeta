/*
 *  BILToImage.cpp
 *  GISLook
 *
 *  Created by Bernhard Jenny on 25.05.08.
 *  Copyright 2008. All rights reserved.
 *
 */

#include "BILToImage.h"
#include "File.h"
#include "HdrFile.h"
#include "ReadRaster.h"

bool readBILSize(char *path, long *width, long *height) {
	
	unsigned long nrows, ncols, nbands;
	short nbits;
	bool byteOrderMotorola;
	int layout;
	unsigned long skipBytes, bandRowBytes, totalRowBytes, bandGapBytes;
	float noDataValue;
	
	// read header file
	char * hdrPath = changeExtension(path, "hdr");
	if (hdrPath == NULL)
		return FALSE;
	FILE *hdrfp = fopen(hdrPath, "r");
	free(hdrPath);
	if (hdrfp == NULL)
		return FALSE;
	bool headerRead = readHeaderFile(hdrfp,
									 &nrows,
									 &ncols,
									 &nbands,
									 &nbits,
									 &byteOrderMotorola,
									 &layout,
									 &skipBytes,
									 &bandRowBytes,
									 &totalRowBytes,
									 &bandGapBytes,
									 &noDataValue);
	fclose(hdrfp);
	if (!headerRead)
		return FALSE;
	
	*width = ncols;
	*height = nrows;
	return TRUE;
}

unsigned char * read8BitGrid(FILE *fp,
							 unsigned long nrows, 
							 unsigned long ncols, 
							 unsigned long nbands, 
							 short nbits, 
							 bool byteOrderMotorola, 
							 int layout,
							 unsigned long  skipBytes,
							 unsigned long bandRowBytes, 
							 unsigned long  totalRowBytes, 
							 unsigned long bandGapBytes,
							 float noDataValue,
							 QLPreviewRequestRef preview,
							 QLThumbnailRequestRef thumbnail) {
	
	long rw = resampledWidth(ncols, nrows);
	long rh = resampledHeight(ncols, nrows);
	int sdist = sampleDist(ncols, nrows);
	
	unsigned char *grid = malloc(rw * rh);
	unsigned char *line = malloc(ncols);
	size_t rowLength = (ncols * nbits) / 8L;
	
	unsigned long col, row;
	long cell = 0;
	unsigned char minVal = 255;
	unsigned char maxVal = 0;
	
	for (row = 0; row < nrows; row += sdist) {
		
		// check whether we should abort
		if (row % 30 == 0 && isCancelled(preview, thumbnail)) {
			free(line);
			free(grid);
			return NULL;			
		}
		
		if (fread (line, 1, ncols, fp) != ncols) {
			free(line);
			free(grid);
			return NULL;			
		}
		
		for (col = 0; col < ncols; col += sdist) {
			short s = line[col];
			if (s == noDataValue)
				grid[cell++] = 255;
			else {
				if (s > maxVal)
					maxVal = s;
				if (s < minVal)
					minVal = s;
				grid[cell++] = s;
			}
		}
		
		int r;
		for (r = 0; r <= sdist; r++) {
			// overread empty row ends in bil files
			if (fseek (fp, bandRowBytes - rowLength, SEEK_CUR) != 0
				// overread following bands
				|| fseek (fp, bandRowBytes * (nbands - 1L), SEEK_CUR) != 0
				// overread empty data at end of bands
				|| fseek (fp, totalRowBytes - bandRowBytes * nbands, SEEK_CUR) != 0) 
			{
				free(line);
				free(grid);
				return NULL;
			}
			
			// overread next lines
			if (r + 1 < sdist)
				fseek (fp , ncols, SEEK_CUR);
		}
	}
	free (line);
	
	// scale to gray values
	short diff = maxVal - minVal;
	long gridSize = rw*rh;
	if (diff <= 0)
		return grid; // return an empty gray buffer
	long i;
	for (i = 0; i < gridSize; i++) {
		short v = grid[i];
		if (v != 255)
			grid[i] = (unsigned char)((v - minVal) * 255 / diff);
	}
	
	return grid;
	
}

unsigned char * read16BitGrid(FILE *fp,
							  unsigned long nrows, 
							  unsigned long ncols, 
							  unsigned long nbands, 
							  short nbits, 
							  bool byteOrderMotorola, 
							  int layout,
							  unsigned long  skipBytes,
							  unsigned long bandRowBytes, 
							  unsigned long  totalRowBytes, 
							  unsigned long bandGapBytes,
							  float noDataValue, 
							  bool swap,
							  QLPreviewRequestRef preview,
							  QLThumbnailRequestRef thumbnail) {
	
	short voidValue = -32768;
	
	long rw = resampledWidth(ncols, nrows);
	long rh = resampledHeight(ncols, nrows);
	int sdist = sampleDist(ncols, nrows);
	
	short *grid = malloc(sizeof(short) * rw * rh);
	short *line = malloc(sizeof(short) * ncols);
	size_t rowLength = (ncols * nbits) / 8L;
	
	short minVal = 32767;
	short maxVal = -32768;
	
	unsigned long col, row;
	long cell = 0;
	for (row = 0; row < nrows; row += sdist) {
		
		// check whether we should abort
		if (row % 30 == 0 && isCancelled(preview, thumbnail)) {
			free(line);
			free(grid);
			return NULL;			
		}
		
		if (fread (line, 1, 2 * ncols, fp) != 2 * ncols) {
			free(line);
			free(grid);
			return NULL;			
		}
		
		for (col = 0; col < ncols; col += sdist) {
			short s = line[col];
			if (swap)
				s = swapShort(s);
			
			if (s == noDataValue)
				grid[cell++] = voidValue;
			else {
				if (s > maxVal)
					maxVal = s;
				if (s < minVal)
					minVal = s;
				grid[cell++] = s;
			}
		}
		
		int r;
		for (r = 0; r <= sdist; r++) {
			// overread empty row ends in bil files
			if (fseek (fp, bandRowBytes - rowLength, SEEK_CUR) != 0) {
				free(line);
				free(grid);
				return NULL;
			}
			
			// overread following bands
			if (layout != bsq && fseek (fp, bandRowBytes * (nbands - 1L), SEEK_CUR) != 0) {
				free(line);
				free(grid);
				return NULL;
			}
			
			// overread empty data at end of bands
			if (fseek (fp, totalRowBytes - bandRowBytes * nbands, SEEK_CUR) != 0) {
				free(line);
				free(grid);
				return NULL;
			}
			
			// overread next lines
			if (r + 1 < sdist)
				fseek (fp , ncols * 2, SEEK_CUR);
		}
	}
	free (line);
	
	// scale to gray values
	long diff = maxVal - minVal;
	long gridSize = rw*rh;
	if (diff <= 0) {
		free (grid);
		return calloc(gridSize, 1); // return an empty gray buffer
	}
	
	// allocate buffer for gray pixels
	unsigned char * grayBuffer = malloc(gridSize);
	if (grayBuffer == NULL) {
		free (grid);
		return NULL;
	}
	
	// scale shorts to unsigned chars
	long i;
	for (i = 0; i < gridSize; i++) {
		long v = grid[i];
		if (v == voidValue)
			grayBuffer[i] = 255;
		else
			grayBuffer[i] = (unsigned char)((v - minVal) * 255 / diff);
	}
	
	free (grid);
	return grayBuffer;
	
}

CGImageRef readBILImage(FILE *fp,
						char *path,
						QLPreviewRequestRef preview,
						QLThumbnailRequestRef thumbnail) {
	
	unsigned long nrows, ncols, nbands;
	short nbits;
	bool byteOrderMotorola;
	int layout;
	unsigned long skipBytes, bandRowBytes, totalRowBytes, bandGapBytes;
	float noDataValue;
	
	// read header file
	char * hdrPath = changeExtension(path, "hdr");
	if (hdrPath == NULL)
		return NULL;
	FILE *hdrfp = fopen(hdrPath, "r");
	free(hdrPath);
	if (hdrfp == NULL)
		return NULL;
	bool headerRead = readHeaderFile(hdrfp,
									 &nrows,
									 &ncols,
									 &nbands,
									 &nbits,
									 &byteOrderMotorola,
									 &layout,
									 &skipBytes,
									 &bandRowBytes,
									 &totalRowBytes,
									 &bandGapBytes,
									 &noDataValue);
	fclose(hdrfp);
	if (!headerRead)
		return NULL;
	
	if (nbands > 1 && layout == bip)
		return NULL;
	
	// read grid file
	unsigned char *grayBuffer = NULL;
#ifdef __LITTLE_ENDIAN__
	bool swap = byteOrderMotorola;		
#else
	bool swap = !byteOrderMotorola;
#endif
	
	switch (nbits)
	{
		case 1:	// 1 byte contains 8 values
		{
			
		}
			break;
		case 4:	// 1 byte contains two 4 bit values
		{
			
		}
			break;
		case 8:
		{
			grayBuffer = read8BitGrid(fp, nrows, ncols, nbands, nbits, 
									  byteOrderMotorola, layout, skipBytes,
									  bandRowBytes, totalRowBytes, bandGapBytes,
									  noDataValue,
									  preview,
									  thumbnail);
		}
			break;
		case 16:
		{
			grayBuffer = read16BitGrid(fp, nrows, ncols, nbands, nbits, 
									   byteOrderMotorola, layout, skipBytes,
									   bandRowBytes, totalRowBytes, bandGapBytes,
									   noDataValue, swap,
									   preview,
									   thumbnail);
		}
			break;
		case 32:
		{
		}
			break;
	}
	
	long rw = resampledWidth(ncols, nrows);
	long rh = resampledHeight(ncols, nrows);
	
	return createGrayScaleImage(grayBuffer, rw, rh);;
	
}
