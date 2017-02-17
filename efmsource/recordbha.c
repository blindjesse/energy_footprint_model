/* RecordBHA() - Sets the BHA (bottom-hole area) consumed per AU-Proj (Assessment Unit-Project Area) combo.
*/



#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include "efm.h"
#include "plss.h"
#include "TBHA.h"

void	DeriveBHA ()
{
    int     i;
	float	mu,peryr,total;

    for(i=1;i<=AUSptr->auproj;i++) {
        if(FILptr[i].start >=2012 && FILptr[i].start <=2012+duration) {
			/* derive the average BHA per well */
			mu=FILptr[i].BHOLEsave/(float)FILptr[i].setmaxwells;
			/* derive the average BHA per yr */
			peryr = mu * (float)FILptr[i].numwells; 
			/* derive the total BHA expected given the duration */
			if(FILptr[i].start<=2012) {
				total=peryr* (float)duration;
			}else {
				 total=peryr* ( (2012+duration) - (FILptr[i].start-1));
			}

			/* you may purposely want less BHA than expected - this effectively means you want x no. of wells for a total of y BHA.
			Here we do away with the no. well target and concentrate on BHA.  SO, if expected BHA is > the specified total BHA, 
			we set total to the specified BHA; else we go with the expected to help end up with the same BHA among reps */
			if(total>FILptr[i].BHOLEsave)total=FILptr[i].BHOLEsave;

			fprintf(fileBH,"%d %d %f %f %f \n",time,i,mu,peryr,total);
			/* store these and decrement as wells are established */
			TrackBptr[i].peryr=peryr;
			TrackBptr[i].total=total;
			TrackBptr[i].thisyr=peryr;
			TrackBptr[i].thetotal=total;
			TrackBptr[i].active=1;
		}
    }
}

void	RecordBHA()
{
	int		i;

	for(i=1;i<=AUSptr->auproj;i++) {
		if(TrackBptr[i].active==1) {
			fprintf(fileBH,"%d %d %f \n",time,i,TrackBptr[i].thetotal);
		}
	}
}




