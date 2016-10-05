/*
 *  ESRIBinaryGridToImage.c
 *  GISLook
 *
 *  Created by Bernhard Jenny on 19.05.08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "ESRIBinaryGridToImage.h"
#include "ReadRaster.h"
#include "File.h"
#include "strlwr.h"

void overreadLine(FILE *fp) {
	int c;
	do {
		c = getc(fp);
	} while (c != '\n' && c != '\r' && c != EOF);
}

unsigned char *scaleESRIBinaryToByte(float *fgrid, 
									 float voidValue,
									 bool bigEndian,
									 long gridSize, 
									 QLPreviewRequestRef preview,
									 QLThumbnailRequestRef thumbnail) {
	
	long i;
	
	// find min and max values
	float minVal = MAXFLOAT;
	float maxVal = -MAXFLOAT;
	float f;
	for (i = 0; i < gridSize; i++) {
		
		// convert endianes
		
#ifdef __LITTLE_ENDIAN__
		if (bigEndian)
			swapFloat(fgrid+i);
#else
		if (!bigEndian)
			swapFloat(fgrid+i);
#endif
		f = fgrid[i];
		if (f != voidValue) {
			if (f > maxVal)
				maxVal = f;
			if (f < minVal)
				minVal = f;
		}
	}
	
	// check whether we should abort
	if (isCancelled(preview, thumbnail))
		return NULL;
	
	float diff = maxVal - minVal;
	if (diff <= 0.f)
		return calloc(gridSize, 1); // return an empty gray buffer
	
	// allocate buffer for gray pixels
	unsigned char * grayBuffer = malloc(gridSize);
	if (grayBuffer == NULL)
		return NULL;
	
	// scale floats to unsigned chars
	for (i = 0; i < gridSize; i++) {
		if (fgrid[i] == voidValue)
			grayBuffer[i] = 255;
		else
			grayBuffer[i] = (unsigned char)((fgrid[i] - minVal) * 255.f / diff);
	}
	
	return grayBuffer;
}

bool readHeader(FILE *fp, 
				long *cols, 
				long *rows, 
				float *voidValue, 
				bool *bigEndian) {
	
	if (fp == NULL)
		return FALSE;
	char tag[13];
	bool hasCols = false;
	bool hasRows = false;
	bool hasVoidValue = false;
	bool hasEndianes = false;
	for (;;) {
		if (fscanf(fp, "%12s", tag) == EOF)
			break;
		strlwr(tag);
		if (strcmp (tag, "ncols") == 0) {
			if (fscanf(fp, "%ld", cols) != 1)
				break;
			hasCols = TRUE;
		} else if (strcmp (tag, "nrows") == 0) {
			if (fscanf(fp, "%ld", rows) != 1)
				break;
			hasRows = TRUE;
		} else if (strcmp (tag, "nodata_value") == 0) {
			if (fscanf(fp, "%f", voidValue) != 1)
				break;
			hasVoidValue = TRUE;
		} else if (strcmp (tag, "byteorder") == 0) {
			if (fscanf(fp, "%12s", tag) == EOF)
				break;
			*bigEndian = (strcmp (strlwr(tag), "msbfirst") == 0);
			hasEndianes = TRUE;
		} else {
			overreadLine(fp);
		}
	}
	
	if (!hasVoidValue)
		*voidValue = NAN;
	if (!hasEndianes)
		*bigEndian = FALSE;
	return hasCols && hasRows;
	
}

bool readESRIBinaryGridSize(char *path, long *width, long *height) {
	
	float voidValue;
	bool bigEndian;
	
	// construct path with "hdr" extension
	char * hdrPath = changeExtension(path, "hdr");
	if (hdrPath == NULL)
		return NULL;
	FILE *hdrfp = fopen(hdrPath, "r");
	free(hdrPath);
	bool headerRead = readHeader(hdrfp, width, height, &voidValue, &bigEndian);
	fclose(hdrfp);
	return headerRead;
	
}

CGImageRef readESRIBinaryGridImage(FILE *fp,
								   char *path,
								   QLPreviewRequestRef preview,
								   QLThumbnailRequestRef thumbnail) {
	
	long width, height;
	float voidValue;
	bool bigEndian;
	
	// construct path with "hdr" extension
	char * hdrPath = changeExtension(path, "hdr");
	if (hdrPath == NULL)
		return NULL;
	FILE *hdrfp = fopen(hdrPath, "r");
	free(hdrPath);
	bool headerRead = readHeader(hdrfp, &width, &height, &voidValue, &bigEndian);
	fclose(hdrfp);
	if (!headerRead)
		return NULL;
	
	long rw = resampledWidth(width, height);
	long rh = resampledHeight(width, height);
	int sdist = sampleDist(width, height);
	
	float *fgrid = (float *)malloc(sizeof(float) * rw * rh);
	if (fgrid == NULL)
		return NULL;
	float *frow = (float *)malloc(sizeof(float) * width);
	if (frow == NULL) {
		free(fgrid);
		return NULL;
	}
	int c, r;
	long cell = 0;
	for (r = 0; r < height; r += sdist) {

#ifdef CHECK_GRID_WRITE
		if (cell < 0 || cell >= rw * rh) {
			printf("ESRI Binary %ld \n", cell);
			free(fgrid);
			free(frow);
			return NULL;
		}
#endif
		
		// test for abort
		if ((r % 20) == 0 && isCancelled(preview, thumbnail)) {
			free(fgrid);
			free(frow);
			return NULL;
		}
		
		// read one row
		size_t read = fread (frow, sizeof(float), width, fp);
		if (read != width) {
			free(fgrid);
			free(frow);
			return NULL;
		}
		
		// copy samples
		for (c = 0; c < width; c += sdist) {
			fgrid[cell++] = frow[c];
		}
		
		// overread lines
		if (r + sdist < height)
			fseek ( fp , width * (sdist - 1) * sizeof(float) , SEEK_CUR );
	}
	
	// convert to grayscale image
	unsigned char *grayBuffer = scaleESRIBinaryToByte(fgrid,
													  voidValue, 
													  bigEndian,
													  rw * rh, 
													  preview, thumbnail);
	free (fgrid);
	free(frow);
	if (grayBuffer == NULL || isCancelled(preview, thumbnail)) {
		free (grayBuffer);
		return NULL;
	}
	
	return createGrayScaleImage(grayBuffer, rw, rh);
	
}