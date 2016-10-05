#ifndef __ERRORCODES
#define __ERRORCODES

enum {
			noError,
			unknownError,
			
			// user canceled
			cancelError,	
			
			// read or write error
			fileError,
			
			// not enough memory etc.
			memoryError,
			
			// external resource not found
			resourceError,
			
			// error related to QuickTime
			quickTimeError,
			
			// coordinate system projection datum ellipsoid
			projectionError,
			
			// error in internal structure of data sets and objects tree
			structureError,
			
			// received parameter with invalid value	
			parameterError,
			
			// a certain feature is not supported (for example a parameter found in a file)
			unsupportedFeatureError,
			
			// an error related to threading
			threadError,
			
			// something is not implemented yet
			notImplementedError
		};

// enum AlertID {exportErrorID = 1000 importErrorID unknownErrorID};

#endif