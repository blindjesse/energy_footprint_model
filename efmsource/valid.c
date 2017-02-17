/* Valid() - Determines if a row (r) and column (c) is within the range
of the AOI; this range is stored as row and col and is set when reading the road layer.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>

extern	int		row,col;

char Valid(int r, int c)
{
	if(r>=0 && r<row && c>=0 && c<col) return ( (int)1);
	return( (int)0);
}