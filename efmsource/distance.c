/* Distance() - Derives eculidean distance between 2 points.
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>


float	DISTANCE(float n1,float e1, float cn, float ce)
{
	float	dist;

	dist= sqrt(((n1-cn) * (n1-cn) + (e1-ce) * (e1-ce)));
	return(dist);
}
