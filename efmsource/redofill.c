/* ReDoFill() - Resets TList[]] using the default build-out design specifications. 
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include "efm.h"
#include "geo.h"
#include "padalloc.h"
#include "binary.h"
#include "padpat.h"
#include "plss.h"
#include "reduce.h"
#include "TBHA.h"

extern	float	idum;



void ReDoFill(int auproj)
{
	int		j,cnt,sid,combo,id;
	float	Ran2(float *);
	int		twells,awells,nsects;
	void	pause();
	float	bhole,area;
	int		iyr;
	FILLRATE	*ptr;
	char	value1[255],tr;



	
		cnt=0;
		TList[1].validn=0;  /* init; Use this as a counter into TList for valid specs for this au x proj combo */
		TList[1].twells=0;

		/* determine if wells remain, and the annual allotment */
		twells=0;awells=0;  /* total remaining */

		ptr=FILptr;


		twells=FILptr[auproj].maxwells;awells=FILptr[auproj].numwells;  /* Have to use FILptr[] here 'cause only FILptr[1] is updated in REINIT */
		if(ptr[auproj].prob>0) {if(Ran2(&idum) <ptr[auproj].prob)awells++;}

		if(reducetrue) {  /* if reduction is on, reduce awells to see if we have any to establish.  Below, we go thru
							  a similar process to determine awells then use it to determine BHA */
			if(notenforced[auproj]!=1) {
				awells=(float)awells * rfactor;if(awells<=0)awells=0;
			}
		}


			/* in the new TRACK version, only BHA matters, but we also check to see if awells is nonzero */
		/*	if(twells>0 && awells>0 && FILptr[i].BHOLE>0) { */
		if(awells>0 && TrackBptr[auproj].thetotal>0) {
			cnt++;
			TList[1].auproj=auproj;TList[1].validn=1;
			TList[cnt].num=1;TList[cnt].padpat=ptr[auproj].padpat;TList[cnt].horizontal=ptr[auproj].horizontal;
			TList[cnt].wells=ptr[auproj].wells;
			TList[cnt].acre=ptr[auproj].acre;
			if(awells>twells)awells=twells;
			TList[cnt].numwells=awells;
			TList[1].twells=awells;  /* grand total no. of wells for this yr across all specs */
			TList[cnt].padn=awells/TList[cnt].wells;  /* this means nothing in this case */
			TList[cnt].proj=ptr[auproj].proj;
			/* Derive the annual allotment of BH consumption; adjust numwells if reduction is on.  */
			/* ptr[i] actually should be FILptr[i] since even when switching between different strategies all BOLEsave2 and setmaxwells2 are assumed to
			be the same.  Thus even though this should be FILptr[i], ptr[i] will also work */
			TList[1].BHa=(ptr[auproj].BHOLEsave2/(float)ptr[auproj].setmaxwells2)*(float)ptr[auproj].numwells;  /* should work for all permutations of fillrates */
			if(reducetrue) {  /* if reduction is on  - this adjusts the annual max BHa to consume BUT assumes that this BHa makes
								  as much sense as adjusting the no. of wells.  That is, if we actually want fewer wells then we also want
								  less BHa consumed this year.  Remember, the well reduction was designed to fit a future pattern (of total no. of wells
								  per year). When we do Hwells, we assume here that a reduction in BHa consumed parallels the need to reduce the number of wells
								  in situations where we aren't using Hwells.  For the most part, this BHa adjustment is necessary to produce
								  comparable experiments. The latter means that the BHa consumed when using vertical and directional wells will equate
								  to the BHA consumed when using Hwells.  */
				if(notenforced[auproj]!=1) {
					if(reducetrueBHA==1) {  /* if we're using BHA targets */
						/* don't adjust awells, rely on adjusting only TList[1].BHA */
						TList[1].BHa=(ptr[auproj].BHOLEsave2/(float)ptr[auproj].setmaxwells2)*(float)ptr[auproj].numwells; 
						TList[1].BHa=TList[1].BHa * BHAfactor;
						if(TList[1].BHa<0)TList[1].BHa=0;
				/* Above we adjust awells then use it to determine if we get to here.  
				  Reset storage of twell and well to max values to allow BHA to regulate the time series */
						twells=FILptr[auproj].maxwells;awells=FILptr[auproj].numwells;
						if(awells>twells)twells=awells;
						TList[1].twells=awells;TList[1].numwells=awells;
					}else { /* here we are only using a timeseries of wells */
						awells=(float)ptr[auproj].numwells * rfactor;if(awells<=0)awells=0;
						TList[1].BHa=(ptr[auproj].BHOLEsave2/(float)ptr[auproj].setmaxwells2)*(float)awells;					
					}
				}
			}
		}

		if(cnt>0) {  /* see if there are more well specs; doesn't influence totals */
			for(j=2;j<=ptr[auproj].num;j++) {
				twells=0;awells=0;
				/* if(ValidT(ptr[auproj].next_ptr[j-1].start,ptr[auproj].next_ptr[j-1].stop)){ */
					twells=ptr[auproj].next_ptr[j-1].maxwells;  /* these 2 aren't used ??? */
					awells=ptr[auproj].next_ptr[j-1].numwells;
					if(ptr[auproj].next_ptr[j-1].prob>0) {if(Ran2(&idum) <ptr[auproj].next_ptr[j-1].prob)awells++;}
					cnt++;
					if(cnt>MAXTL){printf("ERROR, TLIST too large \n");exit(-1);}
					TList[1].auproj=auproj;TList[1].validn++;
					TList[cnt].num=j;TList[cnt].padpat=ptr[auproj].next_ptr[j-1].padpat;
					TList[cnt].wells=ptr[auproj].next_ptr[j-1].wells;TList[cnt].horizontal=ptr[auproj].next_ptr[j-1].horizontal;
					TList[cnt].acre=ptr[auproj].next_ptr[j-1].acre;
					TList[cnt].proj=ptr[auproj].next_ptr[j-1].proj;
				/* } */
			}
		} /* if cnt */

}
