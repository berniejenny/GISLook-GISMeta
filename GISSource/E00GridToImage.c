/*
 *  E00GridToImage.c
 *  GISLook
 *
 *  Created by Bernhard Jenny on 06.06.08.
 *  Copyright 2008. All rights reserved.
 *
 */

#include "E00GridToImage.h"
#include "e00compr.h"
#include "ReadRaster.h"

unsigned char *scaleE00FloatToByte(float *floatBuffer, 
								float minVal, float maxVal,
								long width, long height,
								float voidValue) {
	
	const float diff = maxVal - minVal;
	unsigned char * grayBuffer;
	if (diff > 0.f) {
		long i, gridSize = width * height;
		grayBuffer = malloc(gridSize);
		if (grayBuffer == NULL)
			return NULL;
		for (i = 0; i < gridSize; i++) {
			float f = floatBuffer[i];
			if (f == voidValue)
				grayBuffer[i] = 255;
			else
				grayBuffer[i] = (unsigned char)((f - minVal) * 255 / diff);
		}
	} else {
		grayBuffer = calloc(width * height, 1);
	}
	
	return grayBuffer;
	
}

const char *readNextLine(E00ReadPtr hReadPtr) {
	return E00ReadNextLine(hReadPtr);
}

bool searchGRD (E00ReadPtr hReadPtr) {
	const char *pszLine;
	while((pszLine = readNextLine(hReadPtr)) != NULL) {
		if (strncmp(pszLine, "GRD", 3) == 0) {
			return TRUE;
		}
	}
	return FALSE;
}

void e00GridErrorHandler(CPLErr eErrClass, int err_no, const char *msg) {
}

bool readE00GridSize(char *path, long *width, long *height) {
	
	CPLSetErrorHandler( e00GridErrorHandler );
	
	const char *pszLine;
	E00ReadPtr hReadPtr = E00ReadOpen(path);
	bool res = hReadPtr != NULL;
	if (res)
		res = searchGRD(hReadPtr);
	if (res)
		res = (pszLine = readNextLine(hReadPtr)) != NULL;
	if (res)
		res = sscanf(pszLine, "%ld%ld", width, height) == 2;
	E00ReadClose(hReadPtr);
	return res;
}

bool isValidE00GridValue(float v, float limit) {
	return v > limit && v > -1e36;
}

CGImageRef readE00GridToImage(FILE *fp,
							char *path,
							QLPreviewRequestRef preview,
							QLThumbnailRequestRef thumbnail) {
	
	CPLSetErrorHandler( e00GridErrorHandler );
	
	long width, height;
	float voidValue;
	const char *pszLine;
	E00ReadPtr hReadPtr = E00ReadOpen(path);
	bool res = hReadPtr != NULL;
	if (res)
		res = searchGRD(hReadPtr);
	if (res)
		res = (pszLine = readNextLine(hReadPtr)) != NULL;
	if (res)
		res = sscanf(pszLine, "%ld%ld%*d%f", &width, &height, &voidValue) == 3;
	if (res)
		res = (pszLine = readNextLine(hReadPtr)) != NULL; // cell size
	if (res)
		res = (pszLine = readNextLine(hReadPtr)) != NULL; // south-west corner
	if (res)
		res = (pszLine = readNextLine(hReadPtr)) != NULL; // north-east corner
	if (!res) {
		E00ReadClose(hReadPtr);
		return NULL;
	}
	
	long rw = resampledWidth(width, height);
	long rh = resampledHeight(width, height);
	int sdist = sampleDist(width, height);
	float *floatBuffer = malloc(sizeof(float) * rw * rh);
	if (floatBuffer == NULL) {
		E00ReadClose(hReadPtr);
		return NULL;
	}
	
	unsigned long r, c;
	float minVal = 2147483647L;
	float maxVal = -2147483648.f;
	long pixelCounter = 0;
	pszLine = NULL;
	for (r = 0; r < height; r++) {
		
		// check whether we should cancel
		if (r % 10 == 0 && isCancelled(preview, thumbnail)) {
			free(floatBuffer);
			return NULL;
		}
		
		c = 0;
		while (c < width && (pszLine = readNextLine(hReadPtr)) != NULL) {
			float v[5];
			int nv = sscanf(pszLine, "%f%f%f%f%f", v, v+1, v+2, v+3, v+4);
			int i;
			for (i = 0; i < nv && c < width; i++, c++) {
				if (r % sdist == 0 && c % sdist == 0) {
					if (isValidE00GridValue(v[i], voidValue)) {
						floatBuffer[pixelCounter++] = v[i];
						if (v[i] < minVal)
							minVal = v[i];
						if (v[i] > maxVal)
							maxVal = v[i];
					} else
						floatBuffer[pixelCounter++] = voidValue;
				}
#ifdef CHECK_GRID_WRITE
				if (pixelCounter < 1 || pixelCounter > rw * rh) {
					printf("E00 Grid %ld \n", pixelCounter);
					free(floatBuffer);
					return NULL;
				}
#endif
			}				
		}
	}
	
	// scale values to 0..255
	unsigned char *grayBuffer = scaleE00FloatToByte(floatBuffer, minVal, maxVal, 
												 rw, rh, voidValue);	
	E00ReadClose(hReadPtr);
	free(floatBuffer);
	return createGrayScaleImage(grayBuffer, rw, rh);
}
