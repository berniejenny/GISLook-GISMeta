/*
 *  ReadRaster.c
 *  GISLook
 *
 *  Created by Bernhard Jenny on 17.05.08.
 *  Copyright 2008. All rights reserved.
 *
 */

#include "ReadRaster.h"
//#include "ArcInfoGridToImage.h"
#include "BILToImage.h"
#include "E00GridToImage.h"
#include "ESRIASCIIGridToImage.h"
#include "ESRIBinaryGridToImage.h"
#include "PGMToImage.h"
#include "SRTMToImage.h"
#include "SurferGridToImage.h"
#include "USGSDEMToImage.h"
#include "File.h"

const double  MAX_GRID_SIZE = 2000; // must be double

int sampleDist (long width, long height) {
	if (width > MAX_GRID_SIZE || height > MAX_GRID_SIZE) {
		int h = ceil(width / MAX_GRID_SIZE);
		int v = ceil(height / MAX_GRID_SIZE);
		return h > v ? h : v;
	} else
		return 1;
}

long resampledWidth(long width, long height) {
	long sdist = sampleDist(width, height);
	return width / sdist + (width % sdist > 0);
}

long resampledHeight(long width, long height) {
	long sdist = sampleDist(width, height);
	return height / sdist + (height % sdist > 0);
}

short swapShort (short s) {
	return (short)(((s << 8) & 0xFF00) | ((s >> 8) & 0xFF));	
}

void swapLong (long *b) {
	register long temp;
	
	temp = (*b & 0x000000FF);
	temp = ((*b & 0x0000FF00) >> 0x08) | (temp << 0x08);
	temp = ((*b & 0x00FF0000) >> 0x10) | (temp << 0x08);
	*b = ((*b & 0xFF000000) >> 0x18) | (temp << 0x08);
}

void swapFloat (float *b) {
	swapLong ((long*)b);
}

void swapDouble (double *b) {
	double 	temp = *b;
	char * 				csource 	= (char*)&temp;
	char * 				cdest 		= (char*)b;
	cdest[0] = csource[7];
	cdest[1] = csource[6];
	cdest[2] = csource[5];
	cdest[3] = csource[4];
	cdest[4] = csource[3];
	cdest[5] = csource[2];
	cdest[6] = csource[1];
	cdest[7] = csource[0];
}

bool isCancelled(QLPreviewRequestRef preview,
				 QLThumbnailRequestRef thumbnail) {
	if (preview && QLPreviewRequestIsCancelled(preview))
		return TRUE;
	if (thumbnail && QLThumbnailRequestIsCancelled(thumbnail))
		return TRUE;
	return FALSE;
}

CGImageRef readRaster(QLPreviewRequestRef preview,
					  QLThumbnailRequestRef thumbnail,
					  CFURLRef url, 
					  CFStringRef contentTypeUTI) {
	
	FILE *fp = openFile(url);
	if (fp == NULL) {
		return NULL;
	}
	/*
	CFStringRef ESRI_ARCIINFO_GRID_UTI = CFSTR("com.esri.arcinfogrid");
	CFStringRef ESRI_ASCII_GRID_UTI = CFSTR("com.esri.asciigrid");
	CFStringRef ESRI_BINARY_GRID_UTI = CFSTR("com.esri.binarygrid");
	CFStringRef SURFER_UTI = CFSTR("com.goldensoftware.surfer.grid");
	CFStringRef SRTM_UTI = CFSTR("gov.nasa.srtm");	
	CFStringRef DEM_UTI = CFSTR("gov.usgs.dem");
	*/
	CFStringRef PGM_UTI = CFSTR("net.sourceforge.netpbm.pgm");
	
	CGImageRef image = NULL;
	
	char path[10240];
	if (!CFURLGetFileSystemRepresentation(url, true, (unsigned char*)path, 10240)) {
		fclose(fp);
        return NULL;
	}
	
	// only read PGM files with a pgm extension
	if (UTTypeConformsTo (contentTypeUTI, PGM_UTI)) {
		image = readPGMImage(fp, preview, thumbnail);
	}
	
	// try the unequivocal binary formats first 
	if (!image) {
		fseek (fp, 0 , SEEK_SET);
		image = readSRTMImage(fp, path, preview, thumbnail);
	}
	
	// surfer grids are quickly identified by the first 4 bytes
	if (!image) {
		fseek (fp, 0 , SEEK_SET);
		image = readSurferGridImage(fp, preview, thumbnail);
	}
	
	// read ascii formats
	if (!image) {
		fseek (fp, 0 , SEEK_SET);
		image = readESRIASCIIGridImage(fp, preview, thumbnail);
	}
	
	// e00 is ascii
	if (!image) {
		fseek (fp, 0 , SEEK_SET);
		image = readE00GridToImage(fp, path, preview, thumbnail);
	}
	
	// USGS DEM is also ascii
	if (!image) {
		fseek (fp, 0 , SEEK_SET);
		image = readUSGSDEMImage(fp, preview, thumbnail);
	}

	// read formats with external header files after all other formats to 
	// avoid naming conflicts. E.g. grid.pgm with a grid.hdr file would be treated
	// as a ESRI binary grid.
	if (!image) {
		fseek (fp, 0 , SEEK_SET);
		image = readESRIBinaryGridImage(fp, path, preview, thumbnail);
	}
	
	if (!image) {
		fseek (fp, 0 , SEEK_SET);
		image = readBILImage(fp, path, preview, thumbnail);
	}
	
	/*
	if (UTTypeConformsTo (contentTypeUTI, ESRI_ARCIINFO_GRID_UTI)) {
	
	 } else if (UTTypeConformsTo (contentTypeUTI, ESRI_ASCII_GRID_UTI)) {
		
		image = readESRIASCIIGridImage(fp, preview, thumbnail);
		
		
	} else if (UTTypeConformsTo (contentTypeUTI, ESRI_BINARY_GRID_UTI)) {
		
		char path[1024];
		if (urlToPath(url, path, 1024))
			image = readESRIBinaryGridImage(fp, path, preview, thumbnail);
		
	} else if (UTTypeConformsTo (contentTypeUTI, PGM_UTI)) {
		
		image = readPGMImage(fp, preview, thumbnail);
		
	} else if (UTTypeConformsTo (contentTypeUTI, SRTM_UTI)) {
		
		char path[1024];
		if (urlToPath(url, path, 1024))
			image = readSRTMImage(fp, path, preview, thumbnail);
		
	} else if (UTTypeConformsTo (contentTypeUTI, SURFER_UTI)) {
		
		image = readSurferGridImage(fp, preview, thumbnail);
		
	} else if (UTTypeConformsTo (contentTypeUTI, DEM_UTI)) {
		
		image = readUSGSDEMImage(fp, preview, thumbnail);
		
	}*/
	
	fclose(fp);
	return image;
	
}

void ProviderReleaseData (void *info, 
						  const void *data, 
						  size_t size ) {
	unsigned char *grayBuffer = info;
	free(grayBuffer);
}


CGImageRef createGrayScaleImage(unsigned char * grayPixels, 
								size_t width, 
								size_t height) {
	
	if (grayPixels == NULL || width <= 0 || height <= 0)
		return NULL;
	
	// brighten dark values with a square-root shaped gradation curve
	unsigned char lut [256];
	int i;
	for (i = 0; i < 256; i++) {
		lut[i] = sqrt(i / 255.) * 255;
	}
	int pixelCount = width * height;
	for (i = 0; i < pixelCount; i++) {
		grayPixels[i] = lut[grayPixels[i]];
	}
	
	CGDataProviderRef prov = CGDataProviderCreateWithData (grayPixels, grayPixels, 
														   width * height, 
														   ProviderReleaseData);
	if (prov == NULL)
		return NULL;
	CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceGray();
	if (colorSpace == NULL) {
		CGDataProviderRelease (prov);
		return NULL;
	}
	CGImageRef image = CGImageCreate (width, height, 8, 8, 
									  width, colorSpace, 
									  kCGImageAlphaNone,
									  prov, NULL, 0,
									  kCGRenderingIntentDefault);
	CGDataProviderRelease (prov);
	CGColorSpaceRelease (colorSpace);
	
	return image;
}