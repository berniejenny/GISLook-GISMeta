/*
 *  BILToImage.h
 *  GISLook
 *
 *  Created by Bernhard Jenny on 25.05.08.
 *  Copyright 2008. All rights reserved.
 *
 */

#include <CoreFoundation/CoreFoundation.h>
#include <CoreFoundation/CFPlugInCOM.h>
#include <CoreServices/CoreServices.h>
#include <QuickLook/QuickLook.h>

#ifndef __BIL2IMAGE__
#define __BIL2IMAGE__

bool readBILSize(char *path, long *width, long *height);
CGImageRef readBILImage(FILE * fp,
								   char *path,
								   QLPreviewRequestRef preview,
								   QLThumbnailRequestRef thumbnail);

#endif

