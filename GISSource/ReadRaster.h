/*
 *  ReadRaster.h
 *  GISLook
 *
 *  Created by Bernhard Jenny on 17.05.08.
 *  Copyright 2008. All rights reserved.
 *
 */

#ifndef __READRASTER__
#define __READRASTER__

// #define CHECK_GRID_WRITE

#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>
#include <QuickLook/QuickLook.h>

int sampleDist (long width, long height);
long resampledWidth(long width, long height);
long resampledHeight(long width, long height);

short swapShort (short s);
void swapLong (long *b);
void swapFloat (float *b);
void swapDouble (double *b);

bool isCancelled(QLPreviewRequestRef preview,
				 QLThumbnailRequestRef thumbnail);

CGImageRef readRaster(QLPreviewRequestRef preview,
					  QLThumbnailRequestRef thumbnail,
					  CFURLRef url, 
					  CFStringRef contentTypeUTI);

CGImageRef createGrayScaleImage(unsigned char * grayPixels, 
								size_t width, 
								size_t heigth);

#endif