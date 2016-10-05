/*
 *  ESRIASCIIGridToImage.h
 *  GISLook
 *
 *  Created by Bernhard Jenny on 18.05.08.
 *  Copyright 2008. All rights reserved.
 *
 */

#include <CoreFoundation/CoreFoundation.h>
#include <CoreFoundation/CFPlugInCOM.h>
#include <CoreServices/CoreServices.h>
#include <QuickLook/QuickLook.h>

#ifndef __ESRIASCIIGRID2IMAGE__
#define __ESRIASCIIGRID2IMAGE__

bool readESRIASCIIGridSize(FILE *fp, long *width, long *height);
CGImageRef readESRIASCIIGridImage(FILE * fp,
						QLPreviewRequestRef preview,
						QLThumbnailRequestRef thumbnail);

#endif
