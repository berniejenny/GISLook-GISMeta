/*
 *  SRTMToImage.c
 *  GISLook
 *
 *  Created by Bernhard Jenny on 17.05.08.
 *  Copyright 2008. All rights reserved.
 *
 
 Importer for SRTM (Shuttle Radar Topography Mission) HGT Format
 
 SRTM data are distributed in two levels: SRTM-1 with data sampled at one
 arc-second intervals in latitude and longitude, and SRTM-3 sampled at three
 arc-seconds.
 
 Data are divided into one by one degree latitude and longitude tiles in
 "geographic" projection, which is to say a raster presentation in no
 projection at all but easy to manipulate and mosaic.
 
 File names refer to the latitude and longitude of the lower left corner of
 the tile - e.g. N37W105 has its lower left corner at 37 degrees north
 latitude and 105 degrees west longitude.
 
 Height files have the extension .HGT and are signed two byte integers. The
 bytes are in Motorola "big-endian".
 
 Heights are in meters referenced to the WGS84 geoid. Data
 voids are assigned the value -32768.
 
 SRTM-3 files contain 1201 lines and 1201 samples. The one additional row of
 data at the north edge and one additional column of data at the west edge
 of each tile overlaps and duplicates data in the adjacent tile. SRTM-1
 files contain 3601 lines and 3601 samples, with similar overlap.
  
 */

#include "SRTMToImage.h"
#include "ReadRaster.h"

#define	SRTM30WIDTH			(40 * 60 * 2)
#define	SRTM30HEIGHT		(50 * 60 * 2)
#define	SRTM3SIZE			1201
#define	SRTM1SIZE			3601
#define SRTM30_FILE_SIZE	(SRTM30WIDTH * SRTM30HEIGHT * sizeof (short))
#define SRTM3_FILE_SIZE		(SRTM3SIZE * SRTM3SIZE * sizeof (short))
#define SRTM1_FILE_SIZE		(SRTM1SIZE * SRTM1SIZE * sizeof (short))
#define SRTM_NODATAVALUE	-32768

// some files use the undocumented value -9999 as void value.
// this is safe, since SRTM grids will not contain values of -9999
#define SRTM_ALTERNATIVE_NODATAVALUE	-9999

bool isValidSRTMValue(short s) {
	return s != SRTM_NODATAVALUE && s != SRTM_ALTERNATIVE_NODATAVALUE;
}

unsigned char *scaleSRTMToByte(short *shortBuffer, long gridSize, 
							   QLPreviewRequestRef preview,
							   QLThumbnailRequestRef thumbnail) {
	
	long i;
	
	// find min and max values
	short minVal = 32767;
	short maxVal = -32768;
	short s;
	for (i = 0; i < gridSize; i++) {
		
		// convert from big endian
#ifdef __LITTLE_ENDIAN__
		register short temp;
		s = shortBuffer[i];
		temp = (s & 0x00FF);
		shortBuffer[i] = s = ((s & 0xFF00) >> 0x08) | (temp << 0x08);
#else
		s = shortBuffer[i];
#endif
		
		if (!isValidSRTMValue(s))
			continue;					 
		if (s > maxVal)
			maxVal = s;
		if (s < minVal)
			minVal = s;
	}
	
	// check whether we should abort
	if (isCancelled(preview, thumbnail))
		return NULL;
	
	long diff = maxVal - minVal;
	if (diff <= 0)
		return calloc(gridSize, 1); // return an empty gray buffer
	
	// allocate buffer for gray pixels
	unsigned char * grayBuffer = malloc(gridSize);
	if (grayBuffer == NULL)
		return NULL;
	
	// scale shorts to unsigned chars
	for (i = 0; i < gridSize; i++) {
		if (!isValidSRTMValue(shortBuffer[i]))
			grayBuffer[i] = 255;
		else
			grayBuffer[i] = (unsigned char)((shortBuffer[i] - minVal) * 255 / diff);
	}
	
	return grayBuffer;
}

bool readSRTMSize(char *filePath, long *width, long *height) {
	
	unsigned long fileSize = getFileLength(filePath);
	if (fileSize == SRTM30_FILE_SIZE) {
		*width = SRTM30WIDTH;
		*height = SRTM30HEIGHT;
		return TRUE;
	}
	if (fileSize == SRTM3_FILE_SIZE) {
		*width = *height = SRTM3SIZE;
		return TRUE;
	}
	if (fileSize == SRTM1_FILE_SIZE) {
		*width = *height = SRTM1SIZE;
		return TRUE;
	}
	return FALSE;
	
}

CGImageRef readSRTMImage(FILE * fp,
						 char *filePath,
						 QLPreviewRequestRef preview,
						 QLThumbnailRequestRef thumbnail) {
	
	// compute the size of the grid from the file size
	long width, height;
	bool halfSize = false;
	unsigned long fileSize = getFileLength(filePath);
	if (fileSize == SRTM30_FILE_SIZE) {
		width = SRTM30WIDTH;
		height = SRTM30HEIGHT;
		halfSize = true;
	} else if (fileSize == SRTM3_FILE_SIZE)
		width = height = SRTM3SIZE;
	else if (fileSize == SRTM1_FILE_SIZE)
		width = height = SRTM1SIZE;
	else
		return NULL;
	
	// read the grid values
	short *shortBuffer = NULL;
	if (halfSize) {
		shortBuffer = (short *)malloc(width * height);
		short *lineBuffer = malloc(width * 2);
		int col, row, cell = 0;
		for (row = 0; row < height; row += 2) {
			if (fread (lineBuffer, 2, width, fp) != width) {
				free(lineBuffer);
				free(shortBuffer);
				return NULL;
			}
			fseek (fp , width * 2, SEEK_CUR);
			for (col = 0; col < width; col += 2) {
				shortBuffer[cell++] = lineBuffer[col];
			}
		}
		free(lineBuffer);
		width /= 2;
		height /= 2;
	} else {
		shortBuffer = (short *)malloc(2 * width * height);
		if (shortBuffer == NULL)
			return NULL;
		size_t read = fread (shortBuffer, 2, width * height, fp);
		if (read != width * height) {
			free(shortBuffer);
			return NULL;
		}
	}
	
	// check whether we should abort
	if (isCancelled(preview, thumbnail)) {
		free (shortBuffer);
		return NULL;
	}
	
	// convert to grayscale image
	unsigned char *grayBuffer = scaleSRTMToByte(shortBuffer, width * height, preview, thumbnail);
	free (shortBuffer);
	if (grayBuffer == NULL || isCancelled(preview, thumbnail)) {
		free (grayBuffer);
		return NULL;
	}
	
	return createGrayScaleImage(grayBuffer, width, height);

}