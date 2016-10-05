/*
 *  File.h
 *  GISLook
 *
 *  Created by Bernhard Jenny on 13.05.08.
 *  Copyright 2008. All rights reserved.
 *
 */

#include <Carbon/Carbon.h>
#include <stdio.h>

#ifndef __GISLOOK_FILE__
#define __GISLOOK_FILE__

bool setDefaultPathToParentDirectory(char *path);
char *getParentDirectory(char *path);
bool urlToPath(CFURLRef url, char *pathBuffer, int bufferLength);
FILE * openFile(CFURLRef url);
unsigned long getFileLength(char *path);
bool overreadWhiteChars(FILE *fp);
char *changeExtension(char*path, char *extension);

#endif