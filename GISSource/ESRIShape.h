/*
 *  ESRIShape.h
 *  GISLook
 *
 *  Created by Bernhard Jenny on 28.05.08.
 *  Copyright 2008. All rights reserved.
 *
 */

#include <CoreFoundation/CoreFoundation.h>
#include <CoreFoundation/CFPlugInCOM.h>
#include <CoreServices/CoreServices.h>
#include <QuickLook/QuickLook.h>

#ifndef __ESRISHAPE__
#define __ESRISHAPE__

bool readESRIShape(char *path,
				   double scale,
				   CGContextRef cgContext, 
				   QLPreviewRequestRef preview,
				   QLThumbnailRequestRef thumbnail);

bool readESRIShapeSize(char *path, 
					   double *x, 
					   double *y, 
					   double *width, 
					   double *height, 
					   QLPreviewRequestRef preview,
					   QLThumbnailRequestRef thumbnail);

#endif
