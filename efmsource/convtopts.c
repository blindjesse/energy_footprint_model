/* ConvToPts() - Different methods to convert row and column to UTM coordinates.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>



extern	float	grain;
extern	float	utmn,utme;
extern	float	idum;


void	ConvToPts(int r, int c, float *n, float *e)	 
{
	float		cordr,cordc;
	float		Ran2(float *);

	/* randomly select an area +/- .25 around the center of the cell */
	cordr = (Ran2(&idum)*.5)+.25;cordc=(Ran2(&idum)*.5)+.25;
	cordr=cordr*grain;cordc=cordc*grain;

	cordr = (utmn - (r*grain)) - cordr;
	cordc = (utme + (c*grain))+cordc;

	*n = cordr;*e = cordc;
}

/* deterministic conversion to center of a cell */
void	ConvToPtsF(int r, int c, float *n, float *e)	 
{
	float		cordr,cordc;

	/* Actual conversion from r, c to UTM */
	cordr = 0.5;cordc=0.5;
	cordr=cordr*grain;cordc=cordc*grain;

	cordr = (utmn - (r*grain)) - cordr;
	cordc = (utme + (c*grain))+cordc;

	*n = cordr;*e = cordc;
}

/* deterministic conversion to upper left of a cell */
void	ConvToPtsL(int r, int c, float *n, float *e)
{
	float		cordr,cordc;

	cordr = (utmn - (r*grain));
	cordc = (utme + (c*grain));

	*n = cordr;*e = cordc;
}


