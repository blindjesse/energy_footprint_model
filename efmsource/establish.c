/* Establish() - Mediates the establishment of pads and wells.  Determines
if at least the specified number of wells can be located within the AU x PROJ combo.  If so, it calls Locate() 
to attempt to establish a pad and the corresponding wells.

SetThePad() records the establishment of a pad (updates global structures).

*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include "efm.h"
#include "geo.h"
#include "padpat.h"
#include "plss.h"
#include "reduce.h"
#include "TBHA.h"

extern	float	idum;

void Establish(int auproj)
{
	int		GetSpec(float,int,int),id;
	int		sid,entry;
	float	area;
	void	pause();
	char	SetWells(int,int *, int*, float *);
	char	sectr,tr;
	void	MasterUpdate(int,int,int,float,int);
	int		Locate(int,int,int),code;
	int		numfit,cnt,GetAllSpecs(float,int,int);
	char	Validate(int,int,float);
	void	FindRDCells(int,int);
	char	CheckDeadPads(int,int,int *, PADINFO **);
	int		thepid;
	PADINFO	*theptr;
	void	SetThePad(int,int,int,int, PADINFO *);
	char	used;


	/* this is where we loop thru the specific AU x PROJ to establish all the required wells for this time step */
	tr=1;
	while(tr) {
		/* select a section */

		if(WEIptr[1].remain>0) {
			if(SetWells(auproj,&sid,&entry,&area)) {  /* if true then at least 1 of the specs will fit.  entry is the index into WEIptr. */
				/* adjust WEIptr struct to eliminate selection of this section again in this time step */
				WEIptr[1].remain--;WEIptr[entry].used=1;

				/* attempt to max out this section given the specs */
				sectr=1;
				cnt=1;
				numfit=GetAllSpecs(area,auproj,WEIptr[entry].proj); /* try is set in this function */

				if(numfit<=0)sectr=0;

				SECTIONRD=1;   /* this is set to 0 inside Locate/NextStep when at least 1 valid road-to-pad spur is derived.
									   Because Locate loops thru all the pts in a section for each ID, if spur roads are not possible,
									   then SECTIONRD stays=1 after the first call to Locate. Exception:  If locate doesn't locate a
									   pad because of CheckPad(), SECTIONRD is set to 0 to allow the potential for subsequent specs to
									   be evaluate - e.g., those with smaller pads.  
									   ALSO, if checkpts() in Locate determines we have insufficient room to locate a pad, then 
									   SECTIONRD is set to 0.  If nothing is established due to pads not fitting or due to core-area
									   restrictions, SECTIONRD is set to 0.  We want to allow subsequent evaluations unless there are no
									   possible roads to all available pts. */
				/* if we need to evaluate this section, set the STORERDptr struct */
				if(sectr)FindRDCells(auproj,sid);

				while(sectr) {
					id=try[cnt];
					if(Validate(id,auproj,area)) {


						used=0;  /* if !used then call Locate below */

						/* if we want to re-use pads, then check to see if there is an available dead pad */
						if(REUSEPADS){  /* CheckDeadPads is in SelectSections */
							if(CheckDeadPads(auproj,sid,&thepid,&theptr) ) { /* tr if there is a dead pad never belonging to this au */
								SetThePad(auproj,sid,thepid,id,theptr); /* Establish the wells on this pad - id is the entry into TList */
								code=2;used=1;
							}
						}
					


						/* Locate will loop thru all the valid GRTS pts and try to apply the specs indexed by id */
						if(!used)code=Locate(auproj,sid,id);  /* auxproj, sid, and id is index into TList */

						if(code==1) {  /* no more pts OR couldn't fit anything, nothing happened, dont update, try next spec */
							cnt++; /* try next spec if there is another */
							if(cnt>numfit)sectr=0;  /* nothing fits due to spatial overlap or we are out of BHA */
							if(SECTIONRD==1)sectr=0;  /* can't find a valid road so don't evaluate other specs, if any */
							/* fprintf(diag,"%d %d %d code=1\n",sid,time,id); */
						} else {  /* else code==2 means success.  update area, then do a master update */

							area-=TList[id].acre * TList[id].wells; /* what we just consumed; area pertains to the focal section */
							if(area<=0){area=0;sectr=0;}  /* done with this section */
							MasterUpdate(auproj,sid,id,area,WEIptr[entry].proj); /* area here is BHA  - we used to decrement maxwells only in the first FILLRptr.
																				 In the new Track version, we don't touch maxwells or twells.  */

							if(TrackBptr[auproj].thisyr<=0)return;
							if(TrackBptr[auproj].thetotal<=0)return;

							/* if we want a recent road to be available, then we have to reset STORERDptr[].
							Is replaced by UpdateRdStore called in ProcessRds(). */
							/* FindRDCells(auproj,sid); */

						} /* if else code ==1 */
						/* because of overlap with pads or with nontraverse area, spacelimits may exceed counter;
						here we update counter as necessary */
						if(SECPTSptr[sid].counter>0 && SECPTSptr[sid].spacelimit>=SECPTSptr[sid].counter){
							SECPTSptr[sid].counter=0;sectr=0; /* done with this section */
							/* fprintf(diag,"%d %d turnedoff\n",sid,time,id); */
						}
					} else {
						cnt++; /* try next one if there is one */
						if(cnt>numfit)sectr=0;
					} /* Validate */
				} /* sectr */


			} /* we have a section; if SetWells is false, WEI..remain and ..used are set in SetWells()  */

		}else {  /* we're out of sections */
			tr=0;
		} /* else remain */
	} /* tr */

}

char Validate(int id, int auproj,float area)
{
	float	bh;
	void	pause();
	float	push;

	push=area;if(TIGHT==1)push+=10;  /* found up when TIGHT */
	bh=(float)TList[id].wells*TList[id].acre;  /* this is total BHarea required */
	if(push>=bh) {
		/* if(TList[id].wells<= TList[1].twells && bh<= FILptr[auproj].BHOLE) { */
		/* in the new track version, only thetotal matters. */
		if(TrackBptr[auproj].thetotal - bh >=0) {
			if(reducetrueBHA==1) {  /* BUT check to see if we'll exceed the specified BHa - adjusted using BHA time series */
				if(bh<=TList[1].BHa) {  
					return((char)1);
				}
			}else {
				return((char)1);
			}
		}
	}
	return((char)0);
}


int		GetAllSpecs(float area,int auproj,int proj)
{
	int		i,cnt;
	float	bh;
	float	push;

	push=area;if(TIGHT==1)push+=10;
	cnt=0;
	for(i=1;i<=TList[1].validn;i++) {
		if(TList[i].proj==proj) {
			bh=(float)TList[i].wells*TList[i].acre;  /* this is total BHarea required */
			if(push>=bh) {
				/* In the new track version, only the track total for the auproj matters */
			/*	if(TList[i].wells<= TList[1].twells && bh<= FILptr[auproj].BHOLE) {   */
				if(bh <=TrackBptr[auproj].thetotal) {
					if(reducetrueBHA==1) {  /* BUT check to see if we'll exceed the specified BHa - adjusted using BHA time series */
						if(bh<=TList[1].BHa) {  
							cnt++;try[cnt]=i;if(cnt>MAXTL){printf("ERROR cnt > MAXTL\n");exit(-1);}
						}
					}else {
						cnt++;try[cnt]=i;if(cnt>MAXTL){printf("ERROR cnt > MAXTL\n");exit(-1);}
					}
				}
			}
		}
	}
	return(cnt);
}

/* SetThePad() establishes wells on an existing pad - dups the essentials of SetPad().
id is entry into TLIST */
void	SetThePad(int auproj, int sid, int thepid, int id, PADINFO *theptr)
{
	int		proj,index;
	int		FindActualProj(int,int);


/*	fprintf(diag,"reused %d %d\n",time,thepid); */

	if(theptr->id !=thepid) {printf("ERROR, pids not the same in SetThePad\n");exit(-1);}
	theptr->activedyn=1;
	theptr->end=0;theptr->reveg=0;
	theptr->nwells+=TList[id].wells;  /* here we set actual no. of wells */
	theptr->activewells+=TList[id].wells;		/* no. of active wells */
	theptr->au=AUSptr->aulist[auproj];theptr->proj=AUSptr->projlist[auproj];  /* this is the combo project, not the original proj */
	theptr->change=1;

	/* Set the wells */
	if(theptr->nwells >MAXPADWELLS){printf("ERROR, old+new wells >MAXPADWELLS in SetThePad \n");exit(-1);}
	for(index=theptr->nwells - TList[id].wells+1;index<=theptr->nwells;index++) {
		theptr->Nwellptr[index].status=1;theptr->Nwellptr[index].yearbeg=time;
		theptr->Nwellptr[index].bottom=TList[id].acre;theptr->Nwellptr[index].au=AUSptr->aulist[auproj];
		theptr->Nwellptr[index].proj=AUSptr->projlist[auproj];   /* for combos, this is not the original project; instead it is the new combo project.  Is this correct?  */
	} 

	GStore[auproj].wells+=TList[id].wells;   /* Global tally of no. of established wells each year by auxproj combo */

	/* Global tally of no. of established wells and pads each yr by actual project */
	/* First find actual proj */
	proj=FindActualProj(auproj,sid);  /* Function will die if a valid proj is not found */
	GPStore[proj].wells+=TList[id].wells;
	/* We need to store acres consumed 'cause with the combinations across projects into an au we
	can't separate BHA among project areas FOR these combos. GStore[] above stores by au which is AOK,
	but project-level summaries need special processing. */
	GPStore[proj].area+=((float)TList[id].wells*TList[id].acre);
}



