/*
 *  E00GridToImage.h
 *  GISLook
 *
 *  Created by Bernhard Jenny on 06.06.08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include <CoreFoundation/CoreFoundation.h>
#include <CoreFoundation/CFPlugInCOM.h>
#include <CoreServices/CoreServices.h>
#include <QuickLook/QuickLook.h>

#ifndef __E00GRID2IMAGE__
#define __E00GRID2IMAGE__

bool readE00GridSize(char *path, long *width, long *height);

CGImageRef readE00GridToImage(FILE * fp,
								   char *path,
								   QLPreviewRequestRef preview,
								   QLThumbnailRequestRef thumbnail);

#endif


