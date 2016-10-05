#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>
#include <QuickLook/QuickLook.h>
#include "ReadRaster.h"

/* -----------------------------------------------------------------------------
 Generate a thumbnail for file
 
 This function's job is to create thumbnail for designated file as fast as possible
 ----------------------------------------------------------------------------- */

OSStatus GenerateThumbnailForURL(void *thisInterface, 
								 QLThumbnailRequestRef thumbnail, 
								 CFURLRef url, 
								 CFStringRef contentTypeUTI, 
								 CFDictionaryRef options, 
								 CGSize maxSize)
{
	
	if (isVector (contentTypeUTI) && readVector(NULL, thumbnail, url, contentTypeUTI))
		return noErr;
	
	CGImageRef image = readRaster(NULL, thumbnail, url, contentTypeUTI);
	if (image != NULL)
		QLThumbnailRequestSetImage(thumbnail, image, NULL);
	
	return noErr;
	
}

void CancelThumbnailGeneration(void* thisInterface, QLThumbnailRequestRef thumbnail)
{
    // implement only if supported
}
