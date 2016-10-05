/*
 *  Color.c
 *  GISLook
 *
 *  Created by Bernhard Jenny on 12.05.08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "Color.h"

long RGBToLong (long red, long green, long blue)
{
	// PPC
	/*
	 const long color =  (0xFF000000 & (red << 24)) |
	 (0x00FF0000 & (green << 16)) |
	 (0x0000FF00 & (blue << 8)) |
	 0x000000FF;
	 */
	
	// intel
	const long color =  (0x00FF0000 & (blue << 16)) |
	(0x0000FF00 & (green << 8)) |
	(0x000000FF & red) |
	0xFF000000;
	
	return color;
}

