/* FindLekDistance() - Determine which leks are within 4 miles (6437.376 meters) of the pad centroid (UTM coordinates) 
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include "efm.h"
#include "geo.h"
 
extern	int		row,col;


char	FindLekDistance(float n, float e)
{
 
	int		i;
	float	DISTANCE(float,float,float,float);
	float	dist;
	void	pause();
	char	tr;

	tr=0;
	LEKSptr[1].no=0; /* used as the master counter */
	for(i=1;i<=lek_n;i++) {
		dist=DISTANCE(n,e,LEKptr[i].n,LEKptr[i].e);
		if(dist<=6437.376){  /* this lek is within the distance, but is it in a core area */
			if(SGcore[LEKptr[i].r*col+LEKptr[i].c]>0) {
				LEKSptr[1].no++;
				LEKSptr[LEKSptr[1].no].lpt=i;  /* sequentially store the lekpt access code */
				tr=1;  /* we have lek pts */
			}
		}
	}
	return(tr);

}