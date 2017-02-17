/* SimDevelop()-  Loop thru each AU x PROJ (Assessment unit x Project area) combo 1 at a time, 
determine if we have wells to establish.  If so, determine if we have Bottom-hole area remaining
and room to establish pads. If we have available sections and pts, then weight available sections
via SelectSections().  Set TList[] which is a global but temporary copy of the build-out designs.
Calls Establish() to initiate the establishment of wells and pads.

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



void SimDevelop()
{
	int		i,j,cnt,sid,combo,id;
	float	Ran2(float *);
	int		twells,awells,nsects;
	char	ValidT(int,int);
	void	pause();
	float	bhole,area;
	char	SelectSections(int);
	void	Establish(int);
	int		CheckFill(),usefil,iyr;
	FILLRATE	*ptr;
	char	value1[255],tr;
	void	ReDoFill(int);
	char	validuse,useit;
	char	VSpec(char);



	/* Randomly determine if we weight sections by priority==2 */
	clusteron=0;
	if(Ran2(&idum)<0.4) {clusteron=1;}


	/* randomly invoke reuse of pads using user-provided probability */
	REUSEPADS=0;
	if(Ran2(&idum) <reusepadsprob){REUSEPADS=1; }


	usefil=CheckFill(); /* returns 0 to use the default, else entry into FILSTptr[] */
	if(usefil>0) {
		TFILL[usefil]++;
	}else {
		TFILL[1]++;
	}


	/* ********************************************* loop thru au x proj combos.  Apply the different specs if >1  */
	for(i=1;i<=AUSptr->auproj;i++) {		
		cnt=0;
		TList[1].validn=0;  /* init; Use this as a counter into TList for valid specs for this au x proj combo */
		TList[1].twells=0;

		/* determine if wells remain, and the annual allotment */
		twells=0;awells=0;  /* total remaining */
		validuse=0;

		if(ValidT(FILptr[i].start,FILptr[i].stop)){  /* assume that the MOD fillers will have the same master start,stop as the default */
			validuse=1; /* all this does is indicate that this spec is to be considered given the time interval; reset below before
										   going on to evaluate additional specs. */

			/* first time used; adjust totals based on what has been consumed up to this point in time */
			if(FILptr[i].combofirst==1) {  /* we need to adjust max wells and BH to reflect what has been consumed by this point in time */
				FILptr[i].combofirst=2;  /* gets reset to 1 in REINIT */
				bhole=FILptr[i].BHOLE;twells=FILptr[i].maxwells;
				for(j=1;j<=AUSptr->auproj;j++) {  /* cycle thru all the combined specs & adjust targets */
					if(FILptr[i].list[j]>0) {
						id=j;
						bhole-=FILptr[id].BHOLEsave-FILptr[id].BHOLE;  /* total request to consume minus what remains */
						twells-=FILptr[id].setmaxwells-FILptr[id].maxwells;  /* no. of wells consumed */
					}
				}
				FILptr[i].BHOLE=bhole;if(FILptr[i].BHOLE<=0)FILptr[i].BHOLE=0;  /* these are adjusted values based on what has been consumed to date */
				FILptr[i].maxwells=twells;if(FILptr[i].maxwells<=0)FILptr[i].maxwells=0;

				/* adjust the following the first time a combo is used */
				FILptr[i].BHOLEsave2=bhole;FILptr[i].setmaxwells2=twells;
			}

			ptr=FILptr;
			if(usefil>0)ptr=FILSTptr[usefil].PTR;

			twells=FILptr[i].maxwells;awells=FILptr[i].numwells;  /* Have to use FILptr[] here 'cause only FILptr[1] is updated in REINIT.
																  BUT with the Track verison, this may not mean/do anything */

			if(reducetrue) {  /* if reduction is on, reduce awells to see if we have any to establish.  Below, we go thru
							  a similar process to determine awells then use it to determine BHA */
				if(notenforced[i]!=1) {
					awells=(float)awells * rfactor;if(awells<=0)awells=0;
				}
			}


			/* in the new TRACK version, only BHA matters, but we also check to see if awells is nonzero */
		/*	if(twells>0 && awells>0 && FILptr[i].BHOLE>0) { */
			validuse=0;
			if(awells>0 && TrackBptr[i].thetotal>0) {
				validuse=1;  /* reset here - a double check before evaluating additional specs */
				TList[1].auproj=i;TList[1].twells=awells;  /* grand total no. of wells for this yr across all specs */
				/* do the following here and below.  Doing it here may have no effect given the Track version, but to be on the safe side */
				TList[1].BHa=(ptr[i].BHOLEsave2/(float)ptr[i].setmaxwells2)*(float)ptr[i].numwells;  /* should work for all permutations of fillrates */

				/* we may not use the first spec given the puse value */
				useit=VSpec(ptr[i].puse);
				if(useit) {
					cnt++;
					TList[1].validn=1;
					TList[cnt].num=1;TList[cnt].padpat=ptr[i].padpat;TList[cnt].horizontal=ptr[i].horizontal;
					TList[cnt].wells=ptr[i].wells;
					TList[cnt].acre=ptr[i].acre;
					if(awells>twells)awells=twells;
					TList[cnt].numwells=awells;
					TList[cnt].padn=awells/TList[cnt].wells;  /* this means nothing in this case */
					TList[cnt].proj=ptr[i].proj;
					/* Derive the annual allotment of BH consumption; adjust numwells if reduction is on.  */
					/* ptr[i] actually should be FILptr[i] since even when switching between different strategies all BOLEsave2 and setmaxwells2 are assumed to
					be the same.  Thus even though this should be FILptr[i], ptr[i] will also work */
					TList[1].BHa=(ptr[i].BHOLEsave2/(float)ptr[i].setmaxwells2)*(float)ptr[i].numwells;  /* should work for all permutations of fillrates */
					if(reducetrue) {  /* if reduction is on  - this adjusts the annual max BHa to consume BUT assumes that this BHa makes
								  as much sense as adjusting the no. of wells.  That is, if we actually want fewer wells then we also want
								  less BHa consumed this year.  Remember, the well reduction was designed to fit a future pattern (of total no. of wells
								  per year). When we do Hwells, we assume here that a reduction in BHa consumed parallels the need to reduce the number of wells
								  in situations where we aren't using Hwells.  For the most part, this BHa adjustment is necessary to produce
								  comparable experiments. The latter means that the BHa consumed when using vertical and directional wells will equate
								  to the BHA consumed when using Hwells.  */
						if(notenforced[i]!=1) {
							if(reducetrueBHA==1) {  /* if we're using BHA targets */
								/* don't adjust awells, rely on adjusting only TList[1].BHA */
								TList[1].BHa=(ptr[i].BHOLEsave2/(float)ptr[i].setmaxwells2)*(float)ptr[i].numwells; 
								TList[1].BHa=TList[1].BHa * BHAfactor;
								if(TList[1].BHa<0)TList[1].BHa=0;
								/* Above we adjust awells then use it to determine if we get to here.  
								Reset storage of twell and well to max values to allow BHA to regulate the time series */
								twells=FILptr[i].maxwells;awells=FILptr[i].numwells;
								if(awells>twells)twells=awells;
								TList[1].twells=awells;TList[1].numwells=awells;
							}else { /* here we are only using a timeseries of wells */
								awells=(float)ptr[i].numwells * rfactor;if(awells<=0)awells=0;
								TList[1].BHa=(ptr[i].BHOLEsave2/(float)ptr[i].setmaxwells2)*(float)awells;					
							} /* if reducetrue */
						} /* if notenforced */
					} /* if reducetrue */
				} /* if useit */
			} /* if awells and thetotal */
		} /* if Valid */
		if(validuse) {  /* valid timewise, see if there are more/any well specs; doesn't influence totals */
			for(j=2;j<=ptr[i].num;j++) {
				twells=0;awells=0;
				if(ValidT(ptr[i].next_ptr[j-1].start,ptr[i].next_ptr[j-1].stop)){
					twells=ptr[i].next_ptr[j-1].maxwells;  /* these 2 aren't used ??? */
					awells=ptr[i].next_ptr[j-1].numwells;
					if(ptr[i].next_ptr[j-1].prob>0) {if(Ran2(&idum) <ptr[i].next_ptr[j-1].prob)awells++;}
					/*	if(twells>0 && awells>0) {  */
					useit=VSpec(ptr[i].next_ptr[j-1].puse);
					if(useit) {
						cnt++;
						if(cnt>MAXTL){printf("ERROR, TLIST too large \n");exit(-1);}
						TList[1].auproj=i;TList[1].validn++;
						TList[cnt].num=TList[1].validn;TList[cnt].padpat=ptr[i].next_ptr[j-1].padpat;
						TList[cnt].wells=ptr[i].next_ptr[j-1].wells;TList[cnt].horizontal=ptr[i].next_ptr[j-1].horizontal;
						TList[cnt].acre=ptr[i].next_ptr[j-1].acre;
						TList[cnt].proj=ptr[i].next_ptr[j-1].proj;
					}/* if useit */
 				} /* if Valid */
			}/* for j= */
		} /* if validuse */
		

		/* if cnt <=0, then nothing to do in this au x proj combo based on specs; else determine if sections & pts are
		available */
		if(cnt>0) {


			TIGHT=0;CURVEACTIVE=0;
			tr=1;
			while(tr >0) {
				/* adjust annual BHA where necessary */
				if(loop>1) {
					if(FILptr[i].start==time) {
							TrackBptr[i].thisyr=TrackBptr[i].peryr;
					}else {
						if(TrackBptr[i].thisyr>0) {
							TrackBptr[i].thisyr+=TrackBptr[i].peryr;
						}else {
							TrackBptr[i].thisyr=TrackBptr[i].peryr;
						}
					}
				}else {
					TrackBptr[i].thisyr=TrackBptr[i].peryr;
				}

			/* if(tr==2)printf("%d %f %f\n",i,TrackBptr[i].thisyr,TrackBptr[i].thetotal); */


			/* SelectSections determines if any of the specs can fit in this section.  If so, 
				derives a list (WEIptr) of sections and applies weights for selection prob. Returns true if there is anything to check
				out, else false - nothing will fit */
				if(SelectSections(i)) {  /* i = au x proj combo.  TList is set with specs.
									SelectSections actually just weights sections and sets WEIptr[].
									Use j=SelectSect() to select sections from WEIptr[] - routine
									is inside SetWells(). */

					Establish(i);		/* start the process of establishing wells/pads */

				}
				/* this is where we attempt to flush remaining wells, if necessary */
				if(loop==duration && TrackBptr[i].thetotal>0) {
					tr=tr+1;TIGHT=1;CURVEACTIVE=1;if(tr>2)tr=0;
					/* use the default filler specs */
					if(tr>0 && usefil>0)ReDoFill(i); /* dups some/most of the above to reset TList for this auproj */
					/* if(tr>0)printf("Flushing for auproj %d\n",i); */
				}else {
					tr=0;
				}
			} /* end of tr */
				
		} /* cnt - determination that there are possibly available sections */
	} /* for i (each auproj) */
}



/* if valid times for these specs */
char	ValidT(int start,int stop)
{
	if(start<=time && time <=stop) return((int)1);
	return((int)0);
}


/* this determines the filler specs to use every time step. */
int		CheckFill()
{
	float	Ran2(float *);
	float	urv;
	int		i;


	if(nfillers<=0) return((int)0);
	urv=Ran2(&idum);
	for(i=1;i<=nfillers;i++) {
		if(urv<=FILSTptr[i].prob) {
			if(TFILL[i]<FILLmax[i]) {
				if(i==1)return((int)0);
				return(i);
			}
		}
	}
	/* random selection didn't work, so brutt force it */
	for(i=1;i<=nfillers;i++) {
		if(TFILL[i]<FILLmax[i]) {
			if(i==1)return( (int)0);
			return(i);
		}
	}
	printf("ERROR, no return in CheckFill()");
	return( (int)0);
}

/* evaluates the puse value to determine if the spec is used this time step */
char	VSpec(char puse)
{
	float	Ran2(float *);
	float	rval;

	if(puse==1)return((char)1);
	if(puse==0)return((char)0);
	rval=(float)puse;rval=rval*0.1;
	if(Ran2(&idum)<=rval)return((char)1);
	return((char)0);
}