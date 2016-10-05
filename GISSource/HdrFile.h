#ifndef __HDRFILE__
#define __HDRFILE__

#include "File.h"

#define bil 1
#define bip 2
#define bsq 3
#define esriBinary 4


bool readHeaderFile(FILE *f, 
					unsigned long *nrows,
					unsigned long *ncols,
					unsigned long *nbands,
					short *nbits,
					bool *byteOrderMotorola,
					int *layout,
					unsigned long *skipBytes,
					unsigned long *bandRowBytes,
					unsigned long *totalRowBytes,
					unsigned long *bandGapBytes,
					float *noDataValue);

#endif