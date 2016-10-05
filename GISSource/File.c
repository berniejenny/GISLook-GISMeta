/*
 *  File.c
 *  GISLook
 *
 *  Created by Bernhard Jenny on 13.05.08.
 *  Copyright 2008. All rights reserved.
 *
 */

#include "File.h"
#include <sys/stat.h>

/* Sets the default path for access with standard C file accessors.
 * Return false on failure.
 */
bool setDefaultPathToParentDirectory(char *path) {
	
	char *pch=strrchr(path,'/');
	if (pch == NULL)
		return FALSE;
	size_t parentPathLength = pch-path+1;
	char *parentPath = malloc(parentPathLength+1);
	if (parentPath == NULL)
		return FALSE;
	strncpy(parentPath, path, parentPathLength);
	int res = chdir (parentPath);
	free(parentPath);
	return (res == 0);

}

/* Returns the path to the parent directory of a file. The returned path must
 be released with free();
 */
char *getParentDirectory(char *path) {
	
	char *pch=strrchr(path,'/');
	if (pch == NULL)
		return NULL;
	size_t parentPathLength = pch-path+1;
	char *parentPath = malloc(parentPathLength+1);
	if (parentPath == NULL)
		return NULL;
	strncpy(parentPath, path, parentPathLength);
	return parentPath;	
}

FILE * openFile(CFURLRef url) {
	
	unsigned char pathChar[10240];
	if (!CFURLGetFileSystemRepresentation(url, true, pathChar, 10240))
        return NULL;
	return fopen((char*)pathChar, "r");
	
}

/* Returns the length of the file (only data fork). */
unsigned long getFileLength(char *path) {
	
    struct stat fileStats;
	return  0 == stat (path, &fileStats) ? fileStats.st_size : 0;

}

// reads over white chars. Returns false if an error occurs.
bool overreadWhiteChars(FILE *fp)
{
    int c;
	do {
		c = getc (fp);
    } while (isspace(c));
	
	if (ungetc (c, fp) == EOF)
		return false;
	return true;
}

// returned path must be released with free()
char *changeExtension(char *path, char *extension) {
	
	if (extension == NULL || path == NULL)
		return NULL;
	size_t extLength = strlen(extension);
	if (extLength != 3)
		return NULL;
	size_t pathLength = strlen(path);
	if (pathLength < 5 || path[pathLength - 4] != '.')
		return NULL;
	char *newPath = malloc(pathLength + 1);
	strcpy(newPath, path);
	
	newPath[pathLength - 3] = extension[0];
	newPath[pathLength - 2] = extension[1];
	newPath[pathLength - 1] = extension[2];
	
	return newPath;
}