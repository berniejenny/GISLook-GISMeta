/*
 *  ESRIShape.c
 *  GISLook
 *
 *  Created by Bernhard Jenny on 28.05.08.
 *  Copyright 2008. All rights reserved.
 *
 */

#include "ESRIShape.h"
#include "shapefil.h"
#include "ReadVector.h"


#define MAX_COORD 1.e20

void readPoint (SHPObject * psShape, CGContextRef cgContext, double r)
{
	if (psShape->nVertices > 0)
		drawCircle(cgContext, psShape->padfX[0], psShape->padfY[0], r);
}

void readMultiPoint (SHPObject * psShape, CGContextRef cgContext, double r)
{
	int n;
	for (n = 0; n < psShape->nVertices; n++) {
		drawCircle(cgContext, psShape->padfX[n], psShape->padfY[n], r);
	}
}

void readPolyline (SHPObject * psShape, CGContextRef cgContext)
{
	// Polylines may consist of several parts.
	// A part is a connected sequence of two or more points. Parts may or may not 
	// be connected to one another. Parts may or may not intersect each other.*/
	
	int	j, iPart;
	bool moveto = true;
	CGContextBeginPath(cgContext);
	for( j = 0, iPart = 1; j < psShape->nVertices; j++ )
	{
		// test for end of subline.
		if( iPart < psShape->nParts && psShape->panPartStart[iPart] == j )
		{
			moveto = true;
			iPart++;
		}
		double x = psShape->padfX[j];
		double y = psShape->padfY[j];
		if (!isValidQuartzCoord(x) || !isValidQuartzCoord(y))
			return;
		
		if (moveto) {
			CGContextMoveToPoint(cgContext, x, y);
			moveto = false;
		} else {
			CGContextAddLineToPoint(cgContext, x, y);
		}
	}
	
	CGContextStrokePath(cgContext);
}

void readPolygon (SHPObject * psShape, CGContextRef cgContext)
{
	/* A polygon consists of one or more rings. A ring is a connected sequence of four or more
	 points that form a closed, non-self-intersecting loop. A polygon may contain multiple
	 outer rings. The order of vertices or orientation for a ring indicates which side of the ring
	 is the interior of the polygon. The neighborhood to the right of an observer walking along
	 the ring in vertex order is the neighborhood inside the polygon. Vertices of rings defining
	 holes in polygons are in a counterclockwise direction. Vertices for a single, ringed
	 polygon are, therefore, always in clockwise order. The rings of a polygon are referred to
	 as its parts. 
	 */
	
	int	j, iPart;
	bool moveto = true;
	CGContextBeginPath(cgContext);
	for( j = 0, iPart = 1; j < psShape->nVertices; j++ )
	{
		// test for end of subline.
		if( iPart < psShape->nParts && psShape->panPartStart[iPart] == j )
		{
			CGContextClosePath(cgContext);
			moveto = true;
			iPart++;
		}
		
		// add point
		double x = psShape->padfX[j];
		double y = psShape->padfY[j];
		if (!isValidQuartzCoord(x) || !isValidQuartzCoord(y))
			return;
		
		if (moveto) {
			CGContextMoveToPoint(cgContext, x, y);
			moveto = false;
		} else {
			CGContextAddLineToPoint(cgContext, x, y);
		}
	}
	
	CGContextDrawPath(cgContext, kCGPathFillStroke);
}

bool readESRIShape(char *path,
				   double scale,
				   CGContextRef cgContext, 
				   QLPreviewRequestRef preview,
				   QLThumbnailRequestRef thumbnail) {
	
	int	nEntities, i;
	
	// Open the passed shapefile
	SHPHandle hSHP = SHPOpen (path, "rb" );
	if( hSHP == NULL )
		return FALSE;
	
	// get nbr of entities
	int shapeType;
	SHPGetInfo (hSHP, &nEntities, &shapeType, NULL, NULL);
	
	// figure out the size of dots
	double r = circleRadius(nEntities, scale);
	
	// draw shapes
	if (shapeType == SHPT_POLYGON || shapeType == SHPT_POLYGONZ || shapeType == SHPT_POLYGONM) {
		setColorForPolygons(cgContext, scale);
	} else {
		setColorForLines(cgContext, scale);
	}
	/*
	CGContextSetFillColorWithColor(cgContext, CGColorGetConstantColor(kCGColorBlack));
	CGColorRef strokeColor;
	float strokeWidth;
	if (shapeType == SHPT_POLYGON || shapeType == SHPT_POLYGONZ || shapeType == SHPT_POLYGONM) {
		strokeColor = CGColorGetConstantColor(kCGColorWhite);
		strokeWidth = LINE_WIDTH / scale / 2;
	} else {
		strokeColor = CGColorGetConstantColor(kCGColorBlack);
		strokeWidth = LINE_WIDTH / scale;
	}
	CGContextSetStrokeColorWithColor(cgContext, strokeColor);
	CGContextSetLineWidth(cgContext, strokeWidth);
	 */	
	for (i = 0; i < nEntities; i++)
	{
		if (i % 20 == 0 && isCancelled(preview, thumbnail))
			break;
		
		// read one shape
		SHPObject *psShape = SHPReadObject (hSHP, i);
		if (!psShape)
			continue;
		
		/* Decide for each shape how to convert it. This should also work fo
		 possible future shape files with mixed shape types. */ 
		switch (psShape->nSHPType)
		{
			case SHPT_NULL:				// SHPT_NULL indicates a null shape, without any geometric data.
				break;
				
			case SHPT_POINT:			// point
			case SHPT_POINTM:			// point with measure information (?)
			case SHPT_POINTZ:			// 3D point
				readPoint (psShape, cgContext, r);
				break;
				
			case SHPT_MULTIPOINT:		// multipoint
			case SHPT_MULTIPOINTM:		// multipoint with measure information (?)
			case SHPT_MULTIPOINTZ:		// 3D multipoint
				readMultiPoint (psShape, cgContext, r);
				break;
				
			case SHPT_ARC:				// polyline, possible in parts
			case SHPT_ARCM:				// polyline with measure information (?)
			case SHPT_ARCZ:				// 3D polyline
				readPolyline (psShape, cgContext);
			  	break;	
				
			case SHPT_POLYGON:			// polygon, possible in rings
			case SHPT_POLYGONM:			// with measure information (?)
			case SHPT_POLYGONZ:			// 3D polygon
				readPolygon (psShape, cgContext);
				break;
				
			case SHPT_MULTIPATCH:		// a kind of TIN. Not handled.
				break;
		}
		
		// kill the shape
		SHPDestroyObject (psShape);
		psShape = NULL;
	}
	
	// close the file
	SHPClose (hSHP);
	
	return TRUE;
}

void getExtension(SHPObject *shape, double *xmin, double *xmax, double *ymin, double *ymax) {
	if (!shape)
		return;
	SHPComputeExtents(shape);
	*xmin = shape->dfXMin;
	*xmax = shape->dfXMax;
	*ymin = shape->dfYMin;
	*ymax = shape->dfYMax;
}

bool readESRIShapeSize(char *path, 
					   double *x, 
					   double *y, 
					   double *width, 
					   double *height, 
					   QLPreviewRequestRef preview,
					   QLThumbnailRequestRef thumbnail) {
	
	// Open the passed shapefile
	SHPHandle hSHP = SHPOpen (path, "rb" );
	if( hSHP == NULL )
		return FALSE;
	
	// get bounding box
	double min[4];
	double max[4];
	SHPGetInfo (hSHP, NULL, NULL, min, max);
	
	*width = max[0] - min[0];
	*height = max[1] - min[1];
	*x = min[0];
	*y = min[1];
	
	if (!isfinite(*width) || !isfinite(*height)  || !isfinite(*x)  || !isfinite(*y) 
		|| (*width <= 0)
		|| (*height <= 0)
		|| (*width > MAX_COORD)
		|| (*height > MAX_COORD)
		|| *x < -MAX_COORD || *x > MAX_COORD
		|| *y < -MAX_COORD || *y > MAX_COORD) {
		
		int	nEntities, i;
		SHPGetInfo (hSHP, &nEntities, NULL, NULL, NULL);
		if (nEntities < 1) {
			// empty shape file
			*x = *y = *width = *height = 0;
			return TRUE;
		}
		double xmin = 0;
		double xmax = 0;
		double ymin = 0;
		double ymax = 0;
		for (i = 1; i < nEntities; i++) {
			SHPObject *shape = SHPReadObject (hSHP, 0);
			if (shape->nShapeId == -1 || shape->nShapeId == SHPT_NULL) {
				SHPDestroyObject (shape);
				continue;
			}
			getExtension(shape, &xmin, &xmax, &ymin, &ymax);
			SHPDestroyObject (shape);
			break;
		}
		
		for (; i < nEntities; i++)
		{
			if (i % 50 == 0 && isCancelled(preview, thumbnail))
				break;
			
			SHPObject *shape = SHPReadObject (hSHP, i);
			if (shape->nShapeId == -1 || shape->nShapeId == SHPT_NULL) {
				SHPDestroyObject (shape);
				continue;
			}
			
			SHPComputeExtents(shape);
			if (isfinite(shape->dfXMin) && shape->dfXMin < xmin)
				xmin = shape->dfXMin;
			if (isfinite(shape->dfXMax) && shape->dfXMax > xmax)
				xmax = shape->dfXMax;
			if (isfinite(shape->dfYMin) && shape->dfYMin < ymin)
				ymin = shape->dfYMin;
			if (isfinite(shape->dfYMax) && shape->dfYMax > ymax)
				ymax = shape->dfYMax;
			SHPDestroyObject (shape);
		}
		
		*width = xmax - xmin;
		*height = ymax - ymin;
		*x = xmin;
		*y = ymin;
	}
	
	// close the file
	SHPClose (hSHP);
	
	// width and height can be 0 when the file contains a single point
	return *width >= 0 && *height >= 0;
}