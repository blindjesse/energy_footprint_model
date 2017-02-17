/* DeActivateWells() - Randomly deactivates wells using empirical probabilities derived rom a retrospective assessment of 
wells in SW Wyoming. 
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include "efm.h"
#include "geo.h"
#include "plss.h"

extern	float	idum;


void	DeActivateWells()
{

	void	Tally(char),pause();
	float	Ran2(float *);
	double	delta;


	PADINFO		*ptr;
	int			j;
	float		prob;
	

	Tally((char)0);  /* do initial tally */

	ptr=PADIptr;
	while(ptr!=NULL) {
		if(ptr->activedyn==1) {
			for(j=1;j<=ptr->nwells;j++) {
				if(ptr->Nwellptr[j].status==1) {


					if(ptr->Nwellptr[j].yearbeg==time) {
						prob=0.000000000110205; /*0.110205;  0.20*;   0.049 - mod this to assume we are only dealing with 'good' wells.  */
						if(Ran2(&idum)<prob) {
							ptr->change=1;
							ptr->Nwellptr[j].yearend=time;
							ptr->Nwellptr[j].status=0;
							ptr->activewells--;
							if(ptr->activewells<=0){ptr->activewells=0;ptr->activedyn=0;ptr->end=time;}
							/* there are active pads without active OG wells - so adjust activedyn here */
						}
					} else {
						/* if old wells, then consider closing them down */
						    delta=0.0051778379; /* 0.0051778379;  0.00248*   delta=0.0049;*/ if(time-ptr->Nwellptr[j].yearbeg>=30)delta=0.1;
							if(time-ptr->Nwellptr[j].yearbeg>=50)delta=0.3;if(time-ptr->Nwellptr[j].yearbeg>=60)delta=0.5;
							if(time-ptr->Nwellptr[j].yearbeg>=80)delta=0.8;
							if(Ran2(&idum)<delta) {
								ptr->change=1;
								ptr->Nwellptr[j].yearend=time;
								ptr->Nwellptr[j].status=0;
								ptr->activewells--;
								if(ptr->activewells<=0){ptr->activewells=0;ptr->activedyn=0;ptr->end=time;}
							}

					}
				} /* if status==1 */
			} /* for j */

			/* here we check to see if 1) we have an active pad but do not have any active wells.  This is the AI (Active Injector) issue...  If
			we don't do anything about this, the pad remains active forever.  Alternatively, we may need to recode
			these pads differently (maybe even include AI wells in the database?? ) */
			if(ptr->activedyn==1 && ptr->activewells==0) {
				/* use the start date as the basis for shutting the pad down */
				delta=0.0051778379; /* similar to wells - what should be used? */ 
				if(time-ptr->begin>=30)delta=0.1;
				if(time-ptr->begin>=50)delta=0.3;if(time-ptr->begin>=60)delta=0.5;
				if(time-ptr->begin>=80)delta=0.8;
				if(Ran2(&idum)<delta) {
					ptr->change=1;
					ptr->activewells=0;ptr->activedyn=0;ptr->end=time;
				}
			}


		} /* end of if ptr-> active */
		ptr=ptr->next_ptr;
	}

	Tally((char)1);
}



void	Tally(char code)
{
	PADINFO		*ptr;
	double		total,thisyr,old,pads;
	int			j;

	total=0;thisyr=0;old=0;pads=0;
	ptr=PADIptr;
	while(ptr!=NULL) {
		if(ptr->activedyn==1)pads++;
		total+=ptr->activewells;
		for(j=1;j<=ptr->nwells;j++) {
			if(ptr->Nwellptr[j].status==1) {
				if(ptr->Nwellptr[j].yearbeg==time)thisyr++;
				if(ptr->Nwellptr[j].yearbeg <time-30)old++;
			}
		}
		ptr=ptr->next_ptr;
	}
	/* time, active pads, total active wells, wells this year, old wells */ 
	if(code==0) fprintf(mrecord,"%d %f %f %f %f ",time,pads,total,thisyr,old);
	if(code==1)fprintf(mrecord,"%f %f %f %f\n",pads,total,thisyr,old);
}