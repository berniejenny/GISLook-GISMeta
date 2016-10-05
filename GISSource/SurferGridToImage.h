/*
 *  SurferGridToImage.h
 *  GISLook
 *
 *  Created by Bernhard Jenny on 19.05.08.
 *  Copyright 2008. All rights reserved.
 *
 */

#include <CoreFoundation/CoreFoundation.h>
#include <CoreFoundation/CFPlugInCOM.h>
#include <CoreServices/CoreServices.h>
#include <QuickLook/QuickLook.h>

#ifndef __SURFERGRID2IMAGE__
#define __SURFERGRID2IMAGE__

bool readSurferGridSize(FILE *fp, long *width, long *height);
CGImageRef readSurferGridImage(FILE * fp,
								  QLPreviewRequestRef preview,
								  QLThumbnailRequestRef thumbnail);

#endif
