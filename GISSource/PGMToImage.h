/*
 *  PGMToImage.h
 *  GISLook
 *
 *  Created by Bernhard Jenny on 13.05.08.
 *  Copyright 2008. All rights reserved.
 *
 */

#include <CoreFoundation/CoreFoundation.h>
#include <CoreFoundation/CFPlugInCOM.h>
#include <CoreServices/CoreServices.h>
#include <QuickLook/QuickLook.h>

#ifndef __PGM2IMAGE__
#define __PGM2IMAGE__

bool readPGMSize(FILE *fp, long *width, long *height);
CGImageRef readPGMImage(FILE * fp,
						QLPreviewRequestRef preview,
						QLThumbnailRequestRef thumbnail);

#endif