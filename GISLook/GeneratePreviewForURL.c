#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>
#include <QuickLook/QuickLook.h>
#include "ReadRaster.h"
#include "ReadVector.h"

/* -----------------------------------------------------------------------------
 Generate a preview for file
 
 This function's job is to create preview for designated file
 ----------------------------------------------------------------------------- */

OSStatus GeneratePreviewForURL(void *thisInterface, 
							   QLPreviewRequestRef preview, 
							   CFURLRef url, 
							   CFStringRef contentTypeUTI, 
							   CFDictionaryRef options)
{	
	
	if (isVector (contentTypeUTI) && readVector(preview, NULL, url, contentTypeUTI))
		return noErr;
	
	CGImageRef image = readRaster(preview, NULL, url, contentTypeUTI);
	if (image == NULL)
		return noErr;
	CGSize size = CGSizeMake(CGImageGetWidth(image), CGImageGetHeight(image));
	// specify size in points (pass false for isBitmap). Finder will display 
	// previews at a larger size.
	CGContextRef cgContext = QLPreviewRequestCreateContext(preview, size, false, NULL);
	if(cgContext) {
		// draw the image
		CGRect crt = CGRectMake (0.f, 0.f, size.width, size.height);
		CGContextDrawImage (cgContext, crt, image);
		QLPreviewRequestFlushContext(preview, cgContext);
		CGImageRelease (image);
		CGContextRelease(cgContext);
	}
	
	return noErr;
	
}

void CancelPreviewGeneration(void* thisInterface, QLPreviewRequestRef preview)
{
    // implement only if supported
}
