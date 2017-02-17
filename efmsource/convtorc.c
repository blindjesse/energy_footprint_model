/* ConvToRC() - Different methods to convert UTM coordinates to row and column values.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include "efm.h"
#include "geo.h"

extern	int		row,col;
extern	float	utmn,utme,grain;
FILE	*outmap;

void	ConvToRC(float n, float e, int *r, int *c)
{
	int		i,j;
	char	Valid(int,int);
	char	tr;

	tr=1;
	i=(utmn-n)/grain;
	j=(e-utme)/grain;
	if(!Valid(i,j)) {
		printf("ERROR, ConvToRC conversion not valid %f %f %d %d\n",n,e,i,j); 
		exit (-1); 
	
	}
	*r=i;*c=j;
}


char	ConvToRC2(float n, float e, int *r, int *c)
{
	int		i,j;
	char	Valid(int,int);
	char	tr;
	void	pause();
	int		k,l;

	tr=1;
	i=(utmn-n)/grain;
	j=(e-utme)/grain;
	if(!Valid(i,j)) {
		printf("ERROR, ConvToRC conversion not valid %f %f %d %d\n",n,e,i,j); 
		tr=0; 
	}
	*r=i;*c=j;
	return(tr);
}