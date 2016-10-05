/*
 *  SurferGridToImage.c
 *  GISLook
 *
 *  Created by Bernhard Jenny on 19.05.08.
 *  Copyright 2008. All rights reserved.
 *
 */

#include "SurferGridToImage.h"
#include "ReadRaster.h"

double swapSurferDouble(double d) {
#ifdef __LITTLE_ENDIAN__
	return d;
#else
	return CFSwapInt64(d);
#endif
}

float swapSurferFloat(float f) {
#ifdef __LITTLE_ENDIAN__
	return f;
#else
	return CFSwapInt32(f);
#endif
}


unsigned char *readSurferBinary7(FILE * fp,
						   long *width, long *height,
						   bool onlyReadSize,
						   QLPreviewRequestRef preview,
						   QLThumbnailRequestRef thumbnail) {
	
	double 	west, south, dH, dV, zmin, zmax;
	long	rows, cols;
	long	tagID, tagSize;
	double	rotation;	// rotation is not used in Surfer 7 and 8
	double	blank;
	bool	gridSectionRead = false;
	
	rewind(fp);
	const long tagHeaderSize = sizeof (long) + sizeof (long);
	const long gridSectionSize = tagHeaderSize + 2 * sizeof (long) + 8 * sizeof (double);
	
	// loop through all tags until grid data is found.
	for (;;)
	{
		if (fread (&tagID, 4, 1, fp) != 1)
			return NULL;
		tagID = CFSwapInt32BigToHost(tagID);
		if (fread (&tagSize, 4, 1, fp) != 1)
			return NULL;
		tagSize = CFSwapInt32LittleToHost(tagSize);
		switch (tagID)
		{
			case 'GRID':	// grid section: information about the grid
			{
				if (fread (&rows, 4, 1, fp) != 1)	// rows
					return NULL;
				rows = CFSwapInt32LittleToHost(rows);
				*height = rows;
				if (fread (&cols, 4, 1, fp) != 1)	// cols
					return NULL;
				cols = CFSwapInt32LittleToHost(cols);
				*width = cols;
				if (onlyReadSize)
					return (void*)TRUE;
				if (fread (&west, 8, 1, fp) != 1)	// west
					return NULL;
				west = swapSurferDouble(west);
				if (fread (&south, 8, 1, fp) != 1)	// south
					return NULL;
				south = swapSurferDouble(south);
				if (fread (&dH, 8, 1, fp) != 1)		// horizontal distance between pts
					return NULL;
				dH = swapSurferDouble (dH);
				if (fread (&dV, 8, 1, fp) != 1)		// vertical distance between pts
					return NULL;
				dV = swapSurferDouble (dV);
				if (fread (&zmin, 8, 1, fp) != 1)	// z min
					return NULL;
				zmin = swapSurferDouble (zmin);
				if (fread (&zmax, 8, 1, fp) != 1)	// z max
					return NULL;
				zmax = swapSurferDouble (zmax);
				if (fread (&rotation, 8, 1, fp) != 1)// rotation. not used in vers. 7
					return NULL;
				rotation = swapSurferDouble (rotation);
				if (fread (&blank, 8, 1, fp) != 1)	// blank value
					return NULL;
				blank = swapSurferDouble (blank);
				gridSectionRead = true;
				if (tagSize > gridSectionSize)		// test if data is longer then what was read.
					if (fseek (fp, tagSize - gridSectionSize, SEEK_CUR) != 0)
						return NULL;
				break;
			}
				
			case 'DATA': // data section: the grid as double values.
			{
				if (!gridSectionRead)	// grid section must preceed data section.
					return NULL;
				// test grid information values
				if (cols < 1L || rows < 1L)
					return NULL;
				
				long rw = resampledWidth(cols, rows);
				long rh = resampledHeight(cols, rows);
				int sdist = sampleDist(cols, rows);
				
				// initialize grid
				float diff = zmax - zmin;
				if (zmin >= zmax)
					return calloc(rw * rh, 1);
				double * bufIterator;
				double * lineBuffer = nil;
				unsigned char *grayBuffer = malloc(rw * rh);
				
				// read data
				lineBuffer = malloc(cols * sizeof(double));
				long col, row;
				for (row = 0; row < rows; row += sdist)
				{
					// check whether we should cancel
					if (row % 10 == 0 && isCancelled(preview, thumbnail)) {
						free(lineBuffer);
						free(grayBuffer);
						return NULL;
					}
					
					// read one line
					if (fread (lineBuffer, sizeof(double) * cols, 1, fp) != 1) {
						free(grayBuffer);
						free(lineBuffer);
						return NULL;
					}
					
					// copy samples from line to grid
					bufIterator = lineBuffer;
					
					long cell = rw * (rh - 1 - row / sdist);
#ifdef CHECK_GRID_WRITE	
					if (cell < 0 || cell >= rw * rh) {
						printf("Surfer 7 %ld \n", cell);
						free(grayBuffer);
						free(lineBuffer);
						return NULL;
					}
#endif
					
					for (col = 0; col < cols; col += sdist, bufIterator += sdist) {
						double z = swapSurferDouble(*bufIterator);
						if (z >= blank)
							grayBuffer[cell++] = 255;
						else
							grayBuffer[cell++] = (z - zmin) / diff * 255.f;
					}
					
					// overread lines
					if (row + sdist < rows)
						fseek ( fp , cols * (sdist - 1) * sizeof(double), SEEK_CUR);
				}
				free(lineBuffer);
				return grayBuffer;
			}	
			default:	// overread tag
			{	
				if (fseek (fp, tagSize, SEEK_CUR) != 0)
					return NULL;
			}
		}
	}
	
	return NULL;
	
}

bool readSurferBinary7Size(FILE * fp, long *width, long *height) {
	
	return (((void*)TRUE) == readSurferBinary7(fp, width, height, true, NULL, NULL));
	
}
	
bool readSurferBinary6Size(FILE * fp, long *width, long *height) {
	
	short	rows, cols;
	if (fread (&cols, sizeof(short), 1, fp) != 1)
		return FALSE;
	*width = CFSwapInt16LittleToHost(cols);
	if (fread (&rows, sizeof(short), 1, fp) != 1)
		return FALSE;
	*height = CFSwapInt16LittleToHost(rows);
	return TRUE;
	
}

unsigned char *readSurferBinary6(FILE * fp,
						   long *width, long *height,
						   QLPreviewRequestRef preview,
						   QLThumbnailRequestRef thumbnail) {
	
	double 	west, south, east, north, zmin, zmax;
	short	rows, cols;
	float * bufIterator;
	float * lineBuffer = nil;
	
	if (fread (&cols, sizeof(short), 1, fp) != 1)
		return NULL;
	cols = CFSwapInt16LittleToHost(cols);
	if (fread (&rows, sizeof(short), 1, fp) != 1)
		return NULL;
	rows = CFSwapInt16LittleToHost(rows);
	if (fread (&west, sizeof(double), 1, fp) != 1)
		return NULL;
	west = swapSurferDouble (west);
	if (fread (&east, sizeof(double), 1, fp) != 1)
		return NULL;
	east = swapSurferDouble (east);
	if (fread (&south, sizeof(double), 1, fp) != 1)
		return NULL;
	south = swapSurferDouble (south);
	if (fread (&north, sizeof(double), 1, fp) != 1)
		return NULL;
	north = swapSurferDouble (north);
	if (fread (&zmin, sizeof(double), 1, fp) != 1)
		return NULL;
	zmin = swapSurferDouble (zmin);
	if (fread (&zmax, sizeof(double), 1, fp) != 1)
		return NULL;
	zmax = swapSurferDouble (zmax);
	
	// check if the found values are ok.
	if (cols <= 0 || rows <= 0)
		return NULL;
	
	long rw = resampledWidth(cols, rows);
	long rh = resampledHeight(cols, rows);
	int sdist = sampleDist(cols, rows);
	
	// initialize grid
	float diff = zmax - zmin;
	if (zmin >= zmax)
		return calloc(rw * rh, 1);
	unsigned char *grayBuffer = malloc(rw * rh);
	if (grayBuffer == NULL)
		return NULL;
	
	// read grid
	lineBuffer = malloc(sizeof(float) * cols);		
	if (lineBuffer == NULL) {
		free (grayBuffer);
		return NULL;
	}
	
	long row, col;
	for (row = 0; row < rows; row += sdist) {
		// check whether we should cancel
		if (row % 10 == 0 && isCancelled(preview, thumbnail)) {
			free(lineBuffer);
			free(grayBuffer);
			return NULL;
		}
		
		// read one line
		if (fread (lineBuffer, sizeof(float) * cols, 1, fp) != 1) {
			free(grayBuffer);
			free(lineBuffer);
			return NULL;
		}
		
		// copy samples
		bufIterator = lineBuffer;
		long cell = rw * (rh - 1 - row / sdist);
		for (col = 0; col < cols; col += sdist, bufIterator += sdist) {
			float z = swapSurferFloat(*bufIterator);
#ifdef CHECK_GRID_WRITE			
			if (cell < 0 || cell >= rw * rh) {
				printf("Surfer 6 %ld \n", cell);
				free(lineBuffer);
			}
#endif			
			if ((z > zmax || z < zmin))
				grayBuffer[cell++] = 255;
			else
				grayBuffer[cell++] = (z - zmin) / diff * 255.f;
		}
		
		// overread lines
		if (row + sdist < rows)
			fseek ( fp , cols * (sdist - 1) * sizeof(float) , SEEK_CUR );
	}
	
	free(lineBuffer);
	*width = cols;
	*height = rows;
	return grayBuffer;
	
}

bool readSurferASCIISize(FILE *fp, long *width, long *height) {
	
	return fscanf (fp, "%lu", width) == 1 && fscanf (fp, "%lu", height) == 1;
	
}

unsigned char *readSurferASCIIGrid(FILE *fp,
						 long *width, long *height,
						 QLPreviewRequestRef preview,
						 QLThumbnailRequestRef thumbnail) {
	
	float	z;
	double 	west, south, east, north, zmin, zmax;
	long	rows, cols;
	
	if (fscanf (fp, "%lu", &cols) == EOF)	// Kolonnen
		return NULL;
	if (fscanf (fp, "%lu", &rows) == EOF)	// Reihen
		return NULL;
	if (fscanf (fp, "%lf", &west) == EOF)		// Westen
		return NULL;
	if (fscanf (fp, "%lf", &east) == EOF)		// Osten
		return NULL;
	if (fscanf (fp, "%lf", &south) == EOF)		// süden
		return NULL;
	if (fscanf (fp, "%lf", &north) == EOF)		// Norden
		return NULL;
	if (fscanf (fp, "%lf", &zmin) == EOF)		// zmin
		return NULL;
	if (fscanf (fp, "%lf", &zmax) == EOF)		// zmax
		return NULL;
	
	long rw = resampledWidth(cols, rows);
	long rh = resampledHeight(cols, rows);
	int sdist = sampleDist(cols, rows);
	
	float diff = zmax - zmin;
	if (zmin >= zmax)
		return calloc(rw * rh, 1);
	unsigned char *grid = malloc(rw * rh);
	long row, col;
	for (row = 0; row < rows; row++)
	{
		// check whether we should cancel
		if (row % 10 == 0 && isCancelled(preview, thumbnail)) {
			free(grid);
			return NULL;
		}
		long cell = rw * (rh - 1 - row / sdist);
		for (col = 0; col < cols; col++)
		{
			if (row % sdist == 0 && col % sdist == 0) {
				if (fscanf (fp, "%f", &z) != 1) {
					free(grid);
					return NULL;
				}
				
#ifdef CHECK_GRID_WRITE
				if (cell < 0 || cell >= rw * rh) {
				   printf("Surfer ASCII %ld \n", cell);
					free(grid);
				}
#endif
				if ((z > zmax || z < zmin))
					grid[cell++] = 255;
				else
					grid[cell++] = (z - zmin) / diff * 255.f;
			} else {
				fscanf (fp, "%*f");
			}
		}
	}
	
	*width = cols;
	*height = rows;
	return grid;
	
}

bool readSurferGridSize(FILE *fp, long *width, long *height) {
	
	long l;
	if (fread (&l, sizeof (long), 1, fp) != 1)
		return FALSE;
	l = CFSwapInt32LittleToHost(l);
	switch (l) {
		case 'AASD':
			return readSurferASCIISize(fp, width, height);
		case 'BBSD':
			return readSurferBinary6Size(fp, width, height);
		case 'BRSD':
			return readSurferBinary7Size(fp, width, height);
		default:
			return FALSE;
	}
	
}

CGImageRef readSurferGridImage(FILE * fp,
							   QLPreviewRequestRef preview,
							   QLThumbnailRequestRef thumbnail) {
	
	long l;
	if (fread (&l, sizeof (long), 1, fp) != 1)
		return NULL;
	l = CFSwapInt32LittleToHost(l);
	unsigned char *grayBuffer = NULL;
	long width, height;
	switch (l)
	{
		case 'AASD':
			grayBuffer = readSurferASCIIGrid(fp, &width, &height, preview, thumbnail);
			break;
		case 'BBSD':
			grayBuffer = readSurferBinary6(fp, &width, &height, preview, thumbnail);
			break;
		case 'BRSD':
			grayBuffer = readSurferBinary7(fp, &width, &height, false, preview, thumbnail);
			break;
	}
	
	long rw = resampledWidth(width, height);
	long rh = resampledHeight(width, height);
	return createGrayScaleImage(grayBuffer, rw, rh);
	
}