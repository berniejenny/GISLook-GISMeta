/*
 *  ESRIASCIIGridToImage.c
 *  GISLook
 *
 *  Created by Bernhard Jenny on 18.05.08.
 *  Copyright 2008 Bernhard Jenny. All rights reserved.
 
 Header:
 ncols 3500
 nrows 2400
 xllcorner 637500.000 or xllcenter
 yllcorner 206000.000 or yllcenter
 cellsize 5.000
 NODATA_value -9999	this last line is optional
 
 */

#include "ESRIASCIIGridToImage.h"
#include "ReadRaster.h"
#include "File.h"

#define MAX(X,Y) ((X) > (Y) ? (X) : (Y))

bool readESRIASCIIGridSize(FILE *fp, long *width, long *height) {
	
	// read size of grid
	overreadWhiteChars(fp);
	if (tolower(getc(fp)) != 'n'
		|| tolower(getc(fp)) != 'c'
		|| tolower(getc(fp)) != 'o'
		|| tolower(getc(fp)) != 'l'
		|| tolower(getc(fp)) != 's')
		return FALSE;
	fscanf(fp, "%ld", width);
	overreadWhiteChars(fp);
	if (tolower(getc(fp)) != 'n'
		|| tolower(getc(fp)) != 'r'
		|| tolower(getc(fp)) != 'o'
		|| tolower(getc(fp)) != 'w'
		|| tolower(getc(fp)) != 's')
		return FALSE;
	fscanf(fp, "%ld", height);
	overreadWhiteChars(fp);
	
	// overread following three lines
	int i;
	for (i = 0; i < 3; i++) {
		int c;
		do {
			c = getc(fp);
		} while (c != '\n' && c != '\r' && c != EOF);
		overreadWhiteChars(fp);
	}
	
	return TRUE;
	
}

unsigned char *scaleFloatToByte(float *floatBuffer, 
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

unsigned char *readESRIASCIIGrid(FILE * fp, 
								 long width, long height,
								 QLPreviewRequestRef preview,
								 QLThumbnailRequestRef thumbnail, 
								 float voidValue) {
	
	long rw = resampledWidth(width, height);
	long rh = resampledHeight(width, height);
	int sdist = sampleDist(width, height);
	float *floatBuffer = malloc(sizeof(float) * rw * rh);
	if (floatBuffer == NULL)
		return NULL;
	
	// construct a string for fscanf that overreads unneeded values
	// e.g. "%f %*f %*f %*f %*f %*f"
	char *format = malloc(2 + 4 * (sdist - 1) + 1);
	if (format == NULL) {
		free(floatBuffer);
		return NULL;
	}
	strcpy(format, "%f");
	int i;
	for (i = 0; i < sdist - 1; i++) {
		strcpy(format + 2 + i * 4, " %*s"); // *s might possibly be faster than *f
	}

	// special scan format for end of lines
	char *lastFormat = NULL;
	if (width % sdist > 0) {
		lastFormat = malloc(2 + MAX(0, 4 * (width % sdist - 1)) + 1);
		if (lastFormat == NULL) {
			free(floatBuffer);
			free(format);
			return NULL;
		}
		strcpy(lastFormat, "%f");
		for (i = 0; i < width % sdist - 1; i++) {
			strcpy(lastFormat + 2 + i * 4, " %*s");
		}
	}
	
	unsigned long r, c;
	float f;
	float minVal = 2147483647L;
	float maxVal = -2147483648.f;
	long pixelCounter = 0;
		for (r = 0; r < height; r++) {
		
		// check whether we should cancel
		if (r % 10 == 0 && isCancelled(preview, thumbnail)) {
			free(floatBuffer);
			free(format);
			free(lastFormat);
			return NULL;
		}
		if (r % sdist == 0) {
			for (c = 0; c < rw; c++) {
				int success = fscanf(fp, c == rw - 1 && lastFormat ? lastFormat : format, &f);
				if (success == EOF) {
					free(floatBuffer);
					free(format);
					free(lastFormat);
					return NULL;
				}
				if (f != voidValue) {
					if (f < minVal)
						minVal = f;
					if (f > maxVal)
						maxVal = f;
				}

#ifdef CHECK_GRID_WRITE
				if (pixelCounter < 0 || pixelCounter >= rw * rh) {
					printf("ASCII Grid %ld \n", pixelCounter);
					free(floatBuffer);
					free(format);
					free(lastFormat);
					return NULL;
				}
#endif

				floatBuffer[pixelCounter++] = f;
			}
		} else {
			for (c = 0; c < width; c++) {
				if (EOF == fscanf(fp, "%*f")) {
					free(floatBuffer);
					free(format);
					free(lastFormat);
					return NULL;
				}
			}
		}
	}
	
	// scale values to 0..255
	unsigned char *grayBuffer = scaleFloatToByte(floatBuffer, minVal, maxVal, 
												 rw, rh, voidValue);	
	free(floatBuffer);
	return grayBuffer;
	
}

CGImageRef readESRIASCIIGridImage(FILE * fp, 
								  QLPreviewRequestRef preview,
								  QLThumbnailRequestRef thumbnail) {
	
	long width, height;
	float voidValue = NAN;
	
	if (!readESRIASCIIGridSize(fp, &width, &height) || width < 1 || height < 1)
		return NULL;
	
	// read optional "NODATA_value"
	int c = tolower(getc(fp));
	if (c == 'n') {
		do {
			c = getc(fp);
		} while (!isspace(c) && c != EOF);
		if (!fscanf(fp, "%f", &voidValue))
			return NULL;	
	} else {
		ungetc(c, fp);
	}
	unsigned char *grayBuffer = readESRIASCIIGrid(fp, width, height, preview, 
												  thumbnail, voidValue);
	long rw = resampledWidth(width, height);
	long rh = resampledHeight(width, height);
	return createGrayScaleImage(grayBuffer, rw, rh);
	
}