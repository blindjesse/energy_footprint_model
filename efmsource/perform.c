/* Perform() - Records well allocation by AU X Proj combo.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include "efm.h"
#include "padalloc.h"
#include "plss.h"



void	Perform()
{
	int			i;
	int			sumwells,sumpads;
	int			au;
	int			*sumpadsm,*sumwellsm;

	/* do something with GPStore - don't want to output in 2012.. */

	if(time==2012) {
		wellscumu=0;padscumu=0;POUT=0;
		/* Also, figure out how many sets are included in the output */
		for(i=1;i<=AUSptr->auproj;i++) {
			if(FILptr[i].setmaxwells>0)POUT++;
		}
		fprintf(sumrates,"%d\n",POUT);
		fprintf(sumratesp,"%d\n",AUSptr->projects);  /* project summary  - record max number of projects. */
	}

	sumpadsm=(int *) calloc(AUSptr->aus+1,sizeof(int));
	sumwellsm=(int *) calloc(AUSptr->aus+1,sizeof(int));

	sumwells=0;sumpads=0;
    /* No. of wells & pads established each time step by au x proj and BHOLE remaining to consume */
	fprintf(sumrates,"%d ",time);
	for(i=1;i<=AUSptr->auproj;i++) {
		/* only output for au x proj combos that are active -that is, with setmaxwells>0 */
		if(FILptr[i].setmaxwells>0) fprintf(sumrates,"%4d %5d %5d  %7.0f ",i,GStore[i].wells,GStore[i].pads,FILptr[i].BHOLE);  /* set in SetPad() */

		sumpads+=GStore[i].pads;sumwells+=GStore[i].wells;

		au=AUSptr->aulist[i];
		sumpadsm[au]+=GStore[i].pads;sumwellsm[au]+=GStore[i].wells;  /* this sums by au NOT by au x proj */

		GStore[i].wells=0;GStore[i].pads=0;
	}
	fprintf(sumrates,"\n");

	fprintf(ratef,"%d ",time);
	for(i=1;i<=AUSptr->aus;i++) {
		fprintf(ratef,"%d %d %d ",i,sumpadsm[i],sumwellsm[i]);
	}
	if(time>2012){wellscumu+=sumwells;padscumu+=sumpads;}
	fprintf(ratef,"%d %d %f %f\n",sumpads,sumwells,padscumu,wellscumu);  /* sum for the year and cumulative sum */
	free(sumpadsm);free(sumwellsm);

	if(time>2012) {  /* nothing to show at time zero */
		fprintf(sumratesp,"%d ",time);
		for(i=1;i<=AUSptr->projects;i++) {
			fprintf(sumratesp,"%d %d %d %d ",i,GPStore[i].pads,GPStore[i].wells,(int)GPStore[i].area);
			GPStore[i].pads=0;GPStore[i].wells=0;GPStore[i].area=0;
		}
		fprintf(sumratesp,"\n");
	}
}	

