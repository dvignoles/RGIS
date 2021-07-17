/******************************************************************************

GHAAS Command Line Library V1.0
Global Hydrological Archive and Analysis System
Copyright 1994-2021, UNH - ASRC/CUNY

cmMath.c

bfekete@gc.cuny.edu

*******************************************************************************/

#include <math.h>
#include <string.h>
#include <cm.h>

bool CMmathEqualValues (double var0,double var1) {
	if (fabs (var0) + fabs (var1) == (double) 0.0) return (true);
	return (fabs (var0 - var1) / (fabs (var0) + fabs (var1)) < CMmathEpsilon);
}

bool CMmathIsInteger (const char *string) {
	size_t i, len = strlen (string);
	
	for (i = 0;i < len; ++i)
		if (string [i] == '-' || (string [i] >= '0' && string [i] <= '9')) continue; else break;
	if (i == len) return (true);
	return (false);
}
