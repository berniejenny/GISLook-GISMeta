/*
 *  PGMToImage.c
 *  GISLook
 *
 *  Created by Bernhard Jenny on 13.05.08.
 *  Copyright 2008. All rights reserved.
 *
 */

#include "PGMToImage.h"
#include "ReadRaster.h"
#include "File.h"

inline bool readLong (FILE *fp, long *l)
{
	return fscanf(fp, "%ld", l) == 1;
	/*
	long n;
	unsigned char c;
	
	do {
		c = getc(fp);
		if ( c=='#' ) {
			do {
				c = getc(fp);
			} while ( (c!='\n') && (c!='\r') );
		}
	} while ( isspace (c) );
	if ( ! isdigit (c) ) {
		return FALSE;
	}
	n = 0;
	do {
		n = n*10 + c-'0';
		c = getc (fp);
	} while ( isdigit (c) );
	*l = n;
	return TRUE;*/
}

// reads over white chars. Returns false if an error occurs.
bool overreadWhiteCharsAndComments(FILE *fp) {
	
    int c;
	do {
		c = getc (fp);
    } while (isspace(c));
	
	if (c == '#') {
		do {
			c = getc (fp);
		} while (c != '\n' && c != '\r');
		return TRUE;
	} else 
		return (ungetc (c, fp) != EOF);
	
}

bool readPGMSize(FILE *fp, long *width, long *height) {
	
	fseek(fp, 0, SEEK_SET);
	fgetc(fp);
	fgetc(fp);
	
	// read number of columns and rows
	if (!overreadWhiteCharsAndComments(fp))
		return FALSE;
	if (!readLong(fp, width) || *width < 1)
		return FALSE;
	
	if (!overreadWhiteCharsAndComments(fp))
		return FALSE;
	if (!readLong(fp, height) || *height < 0)
		return FALSE;
	
	return overreadWhiteCharsAndComments(fp);
	
}

unsigned char *scaleToByte(unsigned short *shortBuffer, 
						   long minVal, long maxVal, 
						   long width, long height) {
	
	
	unsigned char * grayBuffer = malloc(width * height);
	if (grayBuffer == NULL) {
		return NULL;
	}

	long pixelCounter = 0;
	long c, r;
	const long diff = maxVal - minVal;
	if (diff > 0) {
		for (r = 0; r < height; r++) {
			for (c = 0; c < width; c++) {
				long l = shortBuffer[pixelCounter];
				l = (l - minVal) * 255 / diff;
				grayBuffer[pixelCounter++] = (unsigned char)l;
			}
		}
	}
	
	return grayBuffer;
}

unsigned char *readPGMASCII(FILE * fp, 
							long width, long height,
							QLPreviewRequestRef preview,
							QLThumbnailRequestRef thumbnail) {
	
	long rw = resampledWidth(width, height);
	long rh = resampledHeight(width, height);
	int sdist = sampleDist(width, height);
	unsigned short *shortBuffer = malloc(2 * rw * rh);
	if (shortBuffer == NULL)
		return NULL;
	
	unsigned long r, c;
	long l;
	long minVal = 2147483647L;
	long maxVal = -2147483647 - 1; //-2147483648L;
	long pixelCounter = 0;
	for (r = 0; r < height; r++) {
		
		// check whether we should cancel
		if (r % 10 == 0 && isCancelled(preview, thumbnail)) {
			free (shortBuffer);
			return NULL;;
		}
		
		for (c = 0; c < width; c++) {
			if (!readLong(fp, &l))
				break;
			if (r % sdist == 0 && c % sdist == 0) {
				if (l < minVal)
					minVal = l;
				if (l > maxVal)
					maxVal = l;

#ifdef CHECK_GRID_WRITE
				if (pixelCounter < 0 || pixelCounter >= rw * rh) {
					printf("PGM ASCII %ld \n", pixelCounter);
					free(shortBuffer);
					return NULL;
				}
#endif
				
				shortBuffer[pixelCounter++] = (unsigned short)l;
			}
		}
	}
	
	// scale values to 0..255
	unsigned char *grayBuffer = scaleToByte(shortBuffer, minVal, maxVal, rw, rh);
		
	free(shortBuffer);
	return grayBuffer;
	
}

unsigned char *readPGMBinary8Bit(FILE * fp, 
							long width, long height,
							QLPreviewRequestRef preview,
							QLThumbnailRequestRef thumbnail) {
	
	if (!overreadWhiteChars(fp))
		return NULL;
	
	int sdist = sampleDist(width, height);
	
	if (sdist == 1) {
		unsigned char *grayBuffer = malloc(width * height);
		if (grayBuffer == NULL)
			return NULL;
		if (fread (grayBuffer, 1, width * height, fp) != width * height) {
			free(grayBuffer);
			return NULL;
		}
		return grayBuffer;
	} else {
		long rw = resampledWidth(width, height);
		long rh = resampledHeight(width, height);
		unsigned char *grayBuffer = malloc(rw * rh);
		unsigned char *lineBuffer = malloc(width);
		long c, r;
		long cell = 0;
		for (r = 0; r < height; r+= sdist) {
			
			// check whether we should cancel
			if (r % 10 == 0 && isCancelled(preview, thumbnail)) {
				free (grayBuffer);
				free(lineBuffer);
				return NULL;;
			}
			
			// read one row
			size_t read = fread (lineBuffer, 1, width, fp);
			if (read != width) {
				free(grayBuffer);
				free(lineBuffer);
				return NULL;
			}

#ifdef CHECK_GRID_WRITE
			if (cell >= rw * rh) {
				printf("PGM Binary %ld \n", cell);
				free(grayBuffer);
				free(lineBuffer);
				return NULL;
			}
#endif
			// copy samples
			for (c = 0; c < width; c += sdist) {
				grayBuffer[cell++] = lineBuffer[c];
			}
			
			// overread lines
			if (r + sdist < height)
				fseek ( fp , width * (sdist - 1), SEEK_CUR );			
		}
		free(lineBuffer);
		return grayBuffer;
	}
	return NULL;
	
}

unsigned char *readPGMBinary16Bit(FILE * fp, 
								 long width, long height,
								 QLPreviewRequestRef preview,
								 QLThumbnailRequestRef thumbnail) {

	return NULL;

}

CGImageRef readPGMImage(FILE * fp, 
						QLPreviewRequestRef preview,
						QLThumbnailRequestRef thumbnail) {
	
	long width, height;
	
	// make sure the first two bytes are 'P2'
	if (fgetc(fp) != 'P')
		return FALSE;
	int c = fgetc(fp);
	if (c != '2' && c != '5')
		return FALSE;
	
	bool isASCII = (c == '2');
	
	if (!readPGMSize(fp, &width, &height))	{
		return NULL;
	}
	
	// read the maximum value in the grid
	long max;
	if (!readLong(fp, &max))
		return NULL;
	// the maximum in binary files is required to be in the range 0..65536
	if (!isASCII && (max < 0 || max > 65536)) {
		return NULL;
	}
	
	// read the grid
	unsigned char *grayBuffer = NULL;
	if (isASCII) {
		grayBuffer = readPGMASCII(fp, width, height, preview, thumbnail);
	} else if (max < 256) {
		grayBuffer = readPGMBinary8Bit(fp, width, height, preview, thumbnail);
	} else {
		grayBuffer = readPGMBinary16Bit(fp, width, height, preview, thumbnail);
	}
	
	// convert to grayscale
	long rw = resampledWidth(width, height);
	long rh = resampledHeight(width, height);
	return createGrayScaleImage(grayBuffer, rw, rh);
	
}