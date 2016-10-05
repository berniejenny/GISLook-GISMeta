/*
 *  E00.c
 *  GISLook
 *
 *  Created by Bernhard Jenny on 03.06.08.
 *  Copyright 2008. All rights reserved.
 *
 */

#include <stdio.h>
#include "e00compr.h"
#include "avc.h"
#include "E00.h"
#include "File.h"
#include "ReadVector.h"

const char *nextLine(E00ReadPtr e00Ptr, AVCE00ReadPtr avcPtr) {
	if (e00Ptr)
		return E00ReadNextLine(e00Ptr);
	return AVCE00ReadNextLine(avcPtr);
}					 

// returns true if lineString is the header of a double precision block
bool isDoublePrecision (const char * lineString)
{
	int i;
	sscanf (lineString, "%*s%d", &i);	// read precision flag from string
	return (i == 3);
}

int readE00Coords (E00ReadPtr e00Ptr, AVCE00ReadPtr avcPtr, double *x1, double *y1, double *x2, double *y2)
{
	const char * lineString = nextLine(e00Ptr, avcPtr);
	if (!lineString)
		return 0;
	return sscanf (lineString, "%lf%lf%lf%lf", x1, y1, x2, y2) / 2;
}

void readARC(E00ReadPtr e00Ptr, 
			 AVCE00ReadPtr avcPtr, 
			 bool doublePrecision, 
			 CGContextRef cgContext,
			 QLPreviewRequestRef preview,
			 QLThumbnailRequestRef thumbnail) {
	
	const char *lineString;
	int	coverageNbr, coverageID, fromNode, toNode;
	int	leftPolygon, rightPolygon, nbrCoordinates;
	int arcCounter = 0;
	while (TRUE)
	{
		// read the first line of the next arc
		lineString = nextLine(e00Ptr, avcPtr);
		if (!lineString)
			return;
		
		// extract information about the next arc
		if (sscanf (lineString, "%d%d%d%d%d%d%d", 	
					&coverageNbr, 
					&coverageID,
					&fromNode,
					&toNode,
					&leftPolygon,
					&rightPolygon,
					&nbrCoordinates) != 7)
			return;
		
		if (coverageNbr == -1)
			break;
		
		int i;
		double x1, y1, x2, y2;
		bool moveto = true;
		CGContextBeginPath(cgContext);
		for (i = 0; i < nbrCoordinates; i += 1 + !doublePrecision) {
			int npoints = readE00Coords(e00Ptr, avcPtr, &x1, &y1, &x2, &y2);
			if (npoints >= 1 && isValidQuartzCoord(x1) && isValidQuartzCoord(y1))
				if (moveto) {
					CGContextMoveToPoint(cgContext, x1, y1);
					moveto = false;
				} else
					CGContextAddLineToPoint(cgContext, x1, y1);
			if (npoints == 2 && isValidQuartzCoord(x2) && isValidQuartzCoord(y2))
				CGContextAddLineToPoint(cgContext, x2, y2);
		}
		CGContextStrokePath(cgContext);
		
		if (arcCounter++ % 20 == 0 && isCancelled(preview, thumbnail))
			break;
	}			
}

bool readARCExtension(E00ReadPtr e00Ptr, 
					  AVCE00ReadPtr avcPtr, 
					  bool doublePrecision, 
					  QLPreviewRequestRef preview,
					  QLThumbnailRequestRef thumbnail,
					  double *x, 
					  double *y, 
					  double *width, 
					  double *height) {
	
	const char *lineString;
	int	coverageNbr, coverageID, fromNode, toNode;
	int	leftPolygon, rightPolygon, nbrCoordinates;
	int arcCounter = 0;
	double xmin = MAXFLOAT;
	double ymin = MAXFLOAT;
	double xmax = -MAXFLOAT;
	double ymax = -MAXFLOAT;
	
	while (TRUE)
	{
		// read the first line of the next arc
		lineString = nextLine(e00Ptr, avcPtr);
		if (!lineString)
			return FALSE;
		
		// extract information about the next arc
		if (sscanf (lineString, "%d%d%d%d%d%d%d", 	
					&coverageNbr, 
					&coverageID,
					&fromNode,
					&toNode,
					&leftPolygon,
					&rightPolygon,
					&nbrCoordinates) != 7)
			return FALSE;
		
		if (coverageNbr == -1) // -1 indicates end of ARC section
			break;
		
		int i;
		double x1, y1, x2, y2;
		for (i = 0; i < nbrCoordinates; i += 1 + !doublePrecision) {
			int npoints = readE00Coords(e00Ptr, avcPtr, &x1, &y1, &x2, &y2);
			if (npoints >= 1) {
				xmin = fmin (xmin, x1);
				ymin = fmin (ymin, y1);
				xmax = fmax (xmax, x1);
				ymax = fmax (ymax, y1);
			}
			if (npoints == 2) {
				xmin = fmin (xmin, x2);
				ymin = fmin (ymin, y2);
				xmax = fmax (xmax, x2);
				ymax = fmax (ymax, y2);
			}
		}
		
		if (arcCounter++ % 20 == 0 && isCancelled(preview, thumbnail))
			break;
	}
	
	*x = xmin;
	*y = ymin;
	*width = xmax - xmin;
	*height = ymax - ymin;
	return TRUE;
}

void readLAB(E00ReadPtr e00Ptr, 
			 AVCE00ReadPtr avcPtr, 
			 bool doublePrecision, 
			 CGContextRef cgContext, 
			 double scale,
			 QLPreviewRequestRef preview,
			 QLThumbnailRequestRef thumbnail) {
	
	// read the points
	int npoints = 0;
	float *pts = NULL;
	while (TRUE)
	{
		const char *lineString;
		int coverageID;
		double x, y;
		
		lineString = nextLine(e00Ptr, avcPtr);
		if (!lineString)
			return;
		
		if (sscanf (lineString, "%d%*d%lf%lf", &coverageID, &x, &y) != 3)
			return;
		if (coverageID < 0)
			break;
		
		// overread label box
		double x1, y1, x2, y2;
		readE00Coords(e00Ptr, avcPtr, &x1, &y1, &x2, &y2);
		if (doublePrecision)
			readE00Coords(e00Ptr, avcPtr, &x1, &y1, &x2, &y2);
		
		++npoints;
		pts = realloc(pts, sizeof(float) * npoints * 2);
		if (pts == NULL)
			return;
		pts[npoints * 2 - 2] = x;
		pts[npoints * 2 - 1] = y;
		
		if (npoints % 20 == 0 && isCancelled(preview, thumbnail))
			break;
	}	
	
	// draw the points
	double r = circleRadius(npoints, scale);
	int i;
	for (i = 0; i < npoints; i++) {
		drawCircle(cgContext, pts[i * 2], pts[i * 2 + 1], r);
	}
	free(pts);
	
}

void readLABExtension(E00ReadPtr e00Ptr, 
					  AVCE00ReadPtr avcPtr, 
					  bool doublePrecision, 
					  QLPreviewRequestRef preview,
					  QLThumbnailRequestRef thumbnail,
					  double *x, 
					  double *y, 
					  double *width, 
					  double *height) {
	
	double xmin = MAXFLOAT;
	double ymin = MAXFLOAT;
	double xmax = -MAXFLOAT;
	double ymax = -MAXFLOAT;
	
	int npoints = 0;
	// read the points
	while (TRUE)
	{
		const char *lineString;
		int coverageID;
		double x, y;
		
		lineString = nextLine(e00Ptr, avcPtr);
		if (!lineString)
			return;
		
		if (sscanf (lineString, "%d%*d%lf%lf", &coverageID, &x, &y) != 3)
			return;
		if (coverageID < 0) // -1 indicates end of LAB section
			break;
		
		xmin = fmin (xmin, x);
		ymin = fmin (ymin, y);
		xmax = fmax (xmax, x);
		ymax = fmax (ymax, y);
		
		// overread label box
		double x1, y1, x2, y2;
		readE00Coords(e00Ptr, avcPtr, &x1, &y1, &x2, &y2);
		if (doublePrecision)
			readE00Coords(e00Ptr, avcPtr, &x1, &y1, &x2, &y2);
		
		if ((++npoints) % 20 == 0 && isCancelled(preview, thumbnail))
			break;
	}	
	
	*x = xmin;
	*y = ymin;
	*width = xmax - xmin;
	*height = ymax - ymin;
}

void e00ErrorHandler(CPLErr eErrClass, int err_no, const char *msg) {
}

void closeE00(E00ReadPtr e00Ptr, 
			  AVCE00ReadPtr avcPtr) {
	if (e00Ptr)
		E00ReadClose(e00Ptr);
	if (avcPtr)
		AVCE00ReadClose(avcPtr);
}

bool readE00(char *path,
			 double scale,
			 CGContextRef cgContext, 
			 QLPreviewRequestRef preview,
			 QLThumbnailRequestRef thumbnail) {
	
	CPLSetErrorHandler( e00ErrorHandler );
	E00ReadPtr e00Ptr = NULL;
	AVCE00ReadPtr avcPtr = NULL;
	if (path[strlen(path) - 2] == '0') {
		if ((e00Ptr = E00ReadOpen(path)) == NULL)
			return FALSE;
	} else {
		if ((avcPtr = AVCE00ReadOpen(path)) == NULL)
			return FALSE;
	}	
	// draw shapes
	setColorForLines(cgContext, scale);
	
	const char *pszLine;
	int lineCounter = 0;
	while((pszLine = nextLine(e00Ptr, avcPtr)) != NULL)
	{
		if (strncmp(pszLine, "ARC", 3) == 0) {
			bool doublePrecision = isDoublePrecision(pszLine);
			readARC(e00Ptr, avcPtr, doublePrecision, cgContext, preview, thumbnail);
			break;
		}
		if (strncmp(pszLine, "LAB", 3) == 0) {
			bool doublePrecision = isDoublePrecision(pszLine);
			readLAB(e00Ptr, avcPtr, doublePrecision, cgContext, scale, preview, thumbnail);
			break;
		}
		
		if (lineCounter++ % 20 == 0 && isCancelled(preview, thumbnail))
			break;
	}
	
	closeE00(e00Ptr, avcPtr);
	return TRUE;
}

bool readBND(E00ReadPtr e00Ptr, 
			 AVCE00ReadPtr avcPtr,
			 double *x, 
			 double *y, 
			 double *width, 
			 double *height) {
	
	int i;
	int length = 0;
	double xmax, ymax;
	for (i = 0; i < 4; i++) {
		const char * line = nextLine(e00Ptr, avcPtr);
		if (line == NULL)
			return FALSE;
		int type, l;
		// XMIN              8-1   14-1  18 5 60-1  -1  -1-1                   1-
		if (sscanf(line, "%*s%d%*d%*d%*d%*d%*d%d", &type, &l) != 2)
			return FALSE;
		switch (l) {
			case 40:
				length += 14;
				break;
			case 50:
				length += type == 2 ? 6 : 11;
				break;
			case 60:
				length += type == 4 ? 14 : 24;
				break;
			default:
				return FALSE;		
		}
	}
	const char *line = nextLine(e00Ptr, avcPtr);
	if (line == NULL)
		return NULL;
	bool success;
	if (length > strlen(line)) {
		char combLine[1024];
		strncpy (combLine, line, 1024);
		line = nextLine(e00Ptr, avcPtr);
		strncpy(combLine + strlen(combLine), line, 1024 - strlen(combLine));
		success = (sscanf (combLine, "%lf%lf%lf%lf", x, y, &xmax, &ymax) == 4);
	} else {
		success = (sscanf (line, "%lf%lf%lf%lf", x, y, &xmax, &ymax) == 4);
	}
	
	*width = xmax - *x;
	*height = ymax - *y;
	
	return success;
}

void rewindE00(E00ReadPtr e00Ptr, 
			   AVCE00ReadPtr avcPtr) {
	if (e00Ptr)
		E00ReadRewind(e00Ptr);
	if (avcPtr)
		AVCE00ReadRewind(avcPtr);
}

bool readE00Size(char *path, 
				 double *x, 
				 double *y, 
				 double *width, 
				 double *height, 
				 QLPreviewRequestRef preview,
				 QLThumbnailRequestRef thumbnail) {
	
	CPLSetErrorHandler( e00ErrorHandler );
	E00ReadPtr e00Ptr = NULL;
	AVCE00ReadPtr avcPtr = NULL;
	if (path[strlen(path) - 2] == '0') {
		if ((e00Ptr = E00ReadOpen(path)) == NULL)
			return FALSE;
	} else {
		if ((avcPtr = AVCE00ReadOpen(path)) == NULL)
			return FALSE;
	}
	
	// search for name of data set
	const char *pszLine = nextLine(e00Ptr, avcPtr);
	const char * namePtr = strrchr(pszLine, '/');
	if (namePtr == NULL)
		namePtr =  strrchr(pszLine, '\\');
	char name[1024];
	if (sscanf(namePtr, "%*1c%1023s", name) != 1)
		name[0] = '\0';
	if (strlen(name) <= 4) {
		closeE00(e00Ptr, avcPtr);
		return FALSE;
	}
	
	// search for a line starting with "name.BND"
	*width = *height = 0;
	
	// only search for BND section in E00 files. bnd files of coverages are not read.
	if (e00Ptr != NULL) {
		
		int lineCounter = 0;
		char *bnd = changeExtension(name, "BND");
		int bndLength = strlen(bnd);
		while((pszLine = nextLine(e00Ptr, avcPtr)) != NULL)
		{
			if (strncmp(pszLine, bnd, bndLength) == 0) {
				if (!readBND(e00Ptr, avcPtr, x, y, width, height))
					break;
				free(bnd);
				closeE00(e00Ptr, avcPtr);
				return TRUE;
			}
			if (lineCounter++ % 20 == 0 && isCancelled(preview, thumbnail)) {
				free(bnd);
				closeE00(e00Ptr, avcPtr);
				return FALSE;
			}
		}
		free(bnd);
		
		// search for a line containing ".BND "
		rewindE00(e00Ptr, avcPtr);
		lineCounter = 0;
		while((pszLine = nextLine(e00Ptr, avcPtr)) != NULL)
		{
			if (strstr(pszLine, ".BND ") != NULL) {
				if (!readBND(e00Ptr, avcPtr, x, y, width, height))
					break;
				closeE00(e00Ptr, avcPtr);
				return TRUE;
			}
			if (lineCounter++ % 20 == 0 && isCancelled(preview, thumbnail)) {
				closeE00(e00Ptr, avcPtr);
				return FALSE;
			}
		}
	}
	
	// search for ARC section and parse extension of the arcs
	rewindE00(e00Ptr, avcPtr);
	int lineCounter = 0;
	while((pszLine = nextLine(e00Ptr, avcPtr)) != NULL)
	{
		if (strncmp(pszLine, "ARC", 3) == 0) {
			bool doublePrecision = isDoublePrecision(pszLine);
			readARCExtension(e00Ptr, avcPtr, doublePrecision, 
							 preview, thumbnail, x, y, width, height);
			closeE00(e00Ptr, avcPtr);
			return TRUE;
		}
		if (lineCounter++ % 20 == 0 && isCancelled(preview, thumbnail)) {
			closeE00(e00Ptr, avcPtr);
			return FALSE;
		}
	}
	
	// search for LAB section and parse extension of the points
	rewindE00(e00Ptr, avcPtr);
	lineCounter = 0;
	while((pszLine = nextLine(e00Ptr, avcPtr)) != NULL)
	{
		if (strncmp(pszLine, "LAB", 3) == 0) {
			bool doublePrecision = isDoublePrecision(pszLine);
			readLABExtension(e00Ptr, avcPtr, doublePrecision, 
							 preview, thumbnail, x, y, width, height);
			closeE00(e00Ptr, avcPtr);
			return TRUE;
		}
		if (lineCounter++ % 20 == 0 && isCancelled(preview, thumbnail)) {
			closeE00(e00Ptr, avcPtr);
			return FALSE;
		}
	}
	
	closeE00(e00Ptr, avcPtr);
	return FALSE;
}