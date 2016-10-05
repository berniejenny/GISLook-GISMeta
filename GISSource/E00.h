/*
 *  E00.h
 *  GISLook
 *
 *  Created by Bernhard Jenny on 03.06.08.
 *  Copyright 2008. All rights reserved.
 *
 */

#include <CoreFoundation/CoreFoundation.h>
#include <CoreFoundation/CFPlugInCOM.h>
#include <CoreServices/CoreServices.h>
#include <QuickLook/QuickLook.h>

#ifndef __E00__
#define __E00__

bool readE00(char *path,
				   double scale,
				   CGContextRef cgContext, 
				   QLPreviewRequestRef preview,
				   QLThumbnailRequestRef thumbnail);

bool readE00Size(char *path, 
					   double *x, 
					   double *y, 
					   double *width, 
					   double *height, 
					   QLPreviewRequestRef preview,
					   QLThumbnailRequestRef thumbnail);

#endif

