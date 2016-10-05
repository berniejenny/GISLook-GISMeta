/*
 *  SRTMToImage.h
 *  GISLook
 *
 *  Created by Bernhard Jenny on 17.05.08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
*/

#include <CoreFoundation/CoreFoundation.h>
#include <CoreFoundation/CFPlugInCOM.h>
#include <CoreServices/CoreServices.h>
#include <QuickLook/QuickLook.h>

#ifndef __SRTM2IMAGE__
#define __SRTM2IMAGE__

bool readSRTMSize(char *filePath, long *width, long *height);
CGImageRef readSRTMImage(FILE * fp,
						 char *filePath,
						QLPreviewRequestRef preview,
						QLThumbnailRequestRef thumbnail);

#endif