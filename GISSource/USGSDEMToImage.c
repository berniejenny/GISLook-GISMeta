/*
 *  USGSDEMToImage.c
 *  GISLook
 *
 *  Created by Bernhard Jenny on 18.05.08.
 *  Copyright 2008. All rights reserved.
 *
 */

#include "USGSDEMToImage.h"
#include "ReadRaster.h"

#define MIN(a,b) ((a)>(b)?(b):(a))

#define MAX(a,b) ((a)<(b)?(b):(a))

#define USGSDEM_NODATA	-32767

bool read_dem_double(FILE *fp, double *d)
{
	char buffer[32];
	
	if (fscanf (fp, "%24c", buffer) == EOF)
		return FALSE;
	// replace D by E that is used by Fortran D24.15 double format
	if (buffer[20] == 'D' || buffer[20] == 'd')
		buffer[20] = 'E';
	return sscanf (buffer,"%lf",d) == 1;
}

bool read_dem_I6(FILE *fp, int *i) {
	return fscanf(fp, "%6d", i) == 1;	
}

unsigned char *scaleUSGSDEMToByte(float *floatBuffer, long gridSize,
								  float minVal, float maxVal) {
	
	long i;
	float diff = maxVal - minVal;
	if (diff <= 0)
		return calloc(gridSize, 1); // return an empty gray buffer
	
	// allocate buffer for gray pixels
	unsigned char * grayBuffer = malloc(gridSize);
	if (grayBuffer == NULL)
		return NULL;
	
	// scale to unsigned chars
	for (i = 0; i < gridSize; i++) {
		if (!finite(floatBuffer[i]))
			grayBuffer[i] = 255;
		else
			grayBuffer[i] = (unsigned char)((floatBuffer[i] - minVal) * 255 / diff);
	}
	
	return grayBuffer;
}

typedef struct {
    double	x;
    double	y;
} DPoint2;

static double DConvert( FILE *fp, int nCharCount )

{
    char	szBuffer[100];
    int		i;
	
    VSIFRead( szBuffer, nCharCount, 1, fp );
    szBuffer[nCharCount] = '\0';
	
    for( i = 0; i < nCharCount; i++ )
    {
        if( szBuffer[i] == 'D' )
            szBuffer[i] = 'E';
    }
	
    return atof(szBuffer);
}

bool isUSGSDEMFile(FILE *fp) {
	
	// check ground planimetric reference system. Possible values >= 0
	int i;
	fseek (fp, 156, SEEK_SET);
	for (i = 0; i < 5; i++) {
		if (getc(fp) != ' ')
			return FALSE;
	}
	if (fscanf(fp, "%1d", &i) != 1 || i < 0)
		return FALSE;
	
	// check for regular elevation pattern. Possible value = 1.
	fseek (fp, 150, SEEK_SET);
	for (i = 0; i < 5; i++) {
		if (getc(fp) != ' ')
			return FALSE;
	}
	return	fscanf(fp, "%1d", &i) == 1 && i == 1;
}

bool readUSGSDEMSize(FILE *fp, long *width, long *height) {
	
	if (!isUSGSDEMFile(fp))
		return FALSE;
	
	int nDataStartOffset = 0;
	// check for version of DEM format
    fseek(fp, 864, SEEK_SET);
	
    // Read DEM into matrix
    fscanf(fp, "%ld", height);
    fscanf(fp, "%ld", width);
    bool bNewFormat = ((*height!=1)||(*width!=1));
    if (bNewFormat)
    {
        fseek(fp, 1024, SEEK_SET); 	// New Format
		int		i, j;
        fscanf(fp, "%d", &i);
        fscanf(fp, "%d", &j);
        if ((i!=1)||(j!=1))			// File OK?
        {
            fseek(fp, 893, 0); 	// Undocumented Format (39109h1.dem)
            fscanf(fp, "%d", &i);
            fscanf(fp, "%d", &j);
            if ((i!=1)||(j!=1))			// File OK?
            {
				return FALSE;
            }
            else
                nDataStartOffset = 893;
        }
        else
            nDataStartOffset = 1024;
    }
    else
        nDataStartOffset = 864;
	
	// read coordinates of corners
	fseek(fp, 546, SEEK_SET);
	DPoint2	corners[4];
	DPoint2	extent_min, extent_max;
	int i;
    for (i = 0; i < 4; i++)
    {
        corners[i].x = DConvert(fp, 24);
        corners[i].y = DConvert(fp, 24);
    }
    
    // find absolute extents of raw vales
    extent_min.x = MIN(corners[0].x, corners[1].x);
    extent_max.x = MAX(corners[2].x, corners[3].x);
    extent_min.y = MIN(corners[0].y, corners[3].y);
    extent_max.y = MAX(corners[1].y, corners[2].y);
	
	// read number of columns (vertical profiles)
	fseek(fp, 858, SEEK_SET);
	int nProfiles;
    fscanf(fp, "%d", &nProfiles);
	
	// read spatial resolution (cell size)
	double 	dxdelta, dydelta;
	fseek(fp, 816, 0);
    dxdelta = DConvert(fp, 12);
    dydelta = DConvert(fp, 12);
    //fVRes = DConvert(fp, 12);
	
	// read coordinate system
	fseek(fp, 156, SEEK_SET);
	int		nCoordSystem;
    fscanf(fp, "%d", &nCoordSystem);
	if (nCoordSystem == 1)	// UTM
    {
        // expand extents modulus the pixel size.
        extent_min.y = floor(extent_min.y/dydelta) * dydelta;
        extent_max.y = ceil(extent_max.y/dydelta) * dydelta;
	}
	
	*height = (int) ((extent_max.y - extent_min.y)/dydelta + 1.5);
	*width = nProfiles;
	
	return TRUE;
}

CGImageRef readUSGSDEMImage(FILE * fp, 
							QLPreviewRequestRef preview,
							QLThumbnailRequestRef thumbnail) {
	
	int		bad = FALSE;
	
	if (!isUSGSDEMFile(fp))
		return NULL;
	
	int nDataStartOffset = 0;
	// check for version of DEM format
    fseek(fp, 864, SEEK_SET);
	
    long width, height;
    fscanf(fp, "%ld", &height);
    fscanf(fp, "%ld", &width);
    bool bNewFormat = ((height!=1)||(width!=1));
    if (bNewFormat)
    {
        fseek(fp, 1024, SEEK_SET); 	// New Format
		int		i, j;
        fscanf(fp, "%d", &i);
        fscanf(fp, "%d", &j);
        if ((i!=1)||(j!=1))			// File OK?
        {
            fseek(fp, 893, 0); 	// Undocumented Format (39109h1.dem)
            fscanf(fp, "%d", &i);
            fscanf(fp, "%d", &j);
            if ((i!=1)||(j!=1))			// File OK?
            {
				return NULL;
            }
            else
                nDataStartOffset = 893;
        }
        else
            nDataStartOffset = 1024;
    }
    else
        nDataStartOffset = 864;
	
	// read coordinates of corners
	fseek(fp, 546, SEEK_SET);
	DPoint2	corners[4];
	DPoint2	extent_min, extent_max;
	int i;
    for (i = 0; i < 4; i++)
    {
        corners[i].x = DConvert(fp, 24);
        corners[i].y = DConvert(fp, 24);
    }
    
    // find absolute extents of raw vales
    extent_min.x = MIN(corners[0].x, corners[1].x);
    extent_max.x = MAX(corners[2].x, corners[3].x);
    extent_min.y = MIN(corners[0].y, corners[3].y);
    extent_max.y = MAX(corners[1].y, corners[2].y);
	
	// read number of columns (vertical profiles)
	fseek(fp, 858, SEEK_SET);
	int nProfiles;
    fscanf(fp, "%d", &nProfiles);
	
	// read spatial resolution (cell size)
	double 	dxdelta, dydelta, fVRes;
	fseek(fp, 816, 0);
    dxdelta = DConvert(fp, 12);
    dydelta = DConvert(fp, 12);
    fVRes = DConvert(fp, 12);
	
	// read coordinate system
	fseek(fp, 156, SEEK_SET);
	int	nCoordSystem;
	double adfGeoTransform[6];
    fscanf(fp, "%d", &nCoordSystem);
	if (nCoordSystem == 1)	// UTM
    {
        // expand extents modulus the pixel size.
        extent_min.y = floor(extent_min.y/dydelta) * dydelta;
        extent_max.y = ceil(extent_max.y/dydelta) * dydelta;
		// Forceably compute X extents based on first profile and pixelsize.
        fseek(fp, nDataStartOffset, SEEK_SET);
        int njunk;
		fscanf(fp, "%d", &njunk);
        fscanf(fp, "%d", &njunk);
        fscanf(fp, "%d", &njunk);
        fscanf(fp, "%d", &njunk);
        double dxStart = DConvert(fp, 24);
        
        adfGeoTransform[0] = dxStart - dxdelta/2.0;
        adfGeoTransform[1] = dxdelta;
        adfGeoTransform[2] = 0.0;
        adfGeoTransform[3] = extent_max.y + dydelta/2.0;
        adfGeoTransform[4] = 0.0;
        adfGeoTransform[5] = -dydelta;
	} else {
		// Translate extents from arc-seconds to decimal degrees.
        adfGeoTransform[0] = (extent_min.x - dxdelta/2.0) / 3600.0;
        adfGeoTransform[1] = dxdelta / 3600.0;
        adfGeoTransform[2] = 0.0;
        adfGeoTransform[3] = (extent_max.y + dydelta/2.0) / 3600.0;
        adfGeoTransform[4] = 0.0;
        adfGeoTransform[5] = (-dydelta) / 3600.0;		
	}
	
	height = (int) ((extent_max.y - extent_min.y)/dydelta + 1.5);
	width = nProfiles;
	if (width < 1 || height < 1)
		return NULL;
	
	long rw = resampledWidth(width, height);
	long rh = resampledHeight(width, height);
	int sdist = sampleDist(width, height);
	
	float *grid = malloc(sizeof(float) * rw * rh);
	for (i = rw * rh; i >= 0; i--)
		grid[i] = NAN;
	
	fseek(fp, nDataStartOffset, SEEK_SET);
	
    double dfYMin = adfGeoTransform[3] + (height-0.5) * adfGeoTransform[5];
	
	//Read all the profiles
	float minVal = FLT_MAX;
	float maxVal = -FLT_MAX;
    for(i = 0; i < width; i++)
    {
        int	njunk, nCPoints, lygap;
        double	djunk, dxStart, dyStart, dfElevOffset;
		
        fscanf(fp, "%d", &njunk);
        fscanf(fp, "%d", &njunk);
        fscanf(fp, "%d", &nCPoints);
        fscanf(fp, "%d", &njunk);
		
        dxStart = DConvert(fp, 24);
        dyStart = DConvert(fp, 24);
        dfElevOffset = DConvert(fp, 24);
        djunk = DConvert(fp, 24);
        djunk = DConvert(fp, 24);
		
		if (nCoordSystem == 0)
            dyStart = dyStart / 3600.0;
		
        lygap = (int)((dfYMin - dyStart)/adfGeoTransform[5]+ 0.5);
		
		int j;
        for (j=lygap; j < (nCPoints+(int)lygap); j++)
        {
            int	iY = height - j - 1;
            int nElev;
			
            fscanf(fp, "%d", &nElev);
            if (iY < 0 || iY >= height )
                bad = TRUE;
            else if ((i % sdist == 0) && (iY % sdist == 0) && (nElev != USGSDEM_NODATA)){
				float v = (float)(nElev * fVRes + dfElevOffset);
				if (v < minVal)
					minVal = v;
				if (v > maxVal)
					maxVal = v;
				grid[i / sdist + iY / sdist * rw] = v;
			}
			
			// test for abort
			if (((i + iY * width) % 50 == 0) && isCancelled(preview, thumbnail)) {
				free(grid);
				return NULL;
			}
			
        }
    }
	
	unsigned char *grayBuffer = scaleUSGSDEMToByte(grid, rw * rh, minVal, maxVal);
	free (grid);
	if (grayBuffer == NULL)
		return NULL;
	
	return createGrayScaleImage(grayBuffer, rw, rh);
}