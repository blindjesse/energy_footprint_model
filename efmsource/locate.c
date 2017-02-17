/* Locate() - Picks a point in a selected section and attempts to establish a pad.
Locate() performs some initial checks, especially related to sage-grouse core area
restrictions.  If a location passes these checks, NextStep() is called to perform
additional checks on the ability to locate a pad without overlapping other pads
and non-developable areas, and the ability to find a road that would connect the
new pad with an existing road network.
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
#include "roads.h"


extern int		col;
extern	float	idum;

/* a section may loose all of its points inside of this processing, so
double check to terminate */

/* go thru Locate until a pt results in the establishment of a well/pad, then go back to Establish to
select the next spec and come back to Locate until the section is consumed or annual allotment
has been achieved. */

int	Locate(int auproj,int sid, int tlistid) /* tlistid is index into TList */
{
	int		i,ii,deltar,deltac;
	float	n,e;
	float	displ,newdispl;
	char	ConvToRC2(float , float , int *, int *),piss;
	void 	ConvToPtsFSECG(int,int, float *, float *);
	int		r,c,acore;
	char	CheckPts(float ,float, int, int, float,int,float,int),tr;/* dec 2015 */
	char	UsedPt,code,NextStep(int,int,int,float,float,int,int,int,int,float,int,int);
	int		hsuccess;
	void	SGVGrid();
	float	factor;



	/* If no pts, return */
	if(SECPTSptr[sid].counter<=0) return((int)1);  /* code==1 means no more pts */

	/* If tlistid points to a horizontal well, then special processing occurs.
	If we can't fit a Hwell, then return code==1 and we'll go onto the next spec for this section.
	If we establish a Hwell, then return2 and Validate() in establish will determine if we have
	enough room for another Hwell.  If so we come back here and try for the second one.  If we lack the
	right space for a Hwell, then we return 1 and go onto the next spec, if any. */
	if(TList[tlistid].horizontal==1) {  /* obselete and will ever be set to 1 */
		/* hsuccess=Horizontal(sid,auproj,tlistid); */ /* returns 2 if a pad/well was established; else 1  */ /* 26086 - 7184 */
		/* printf("hsuccess %d\n",hsuccess); */
		/* return(hsuccess); */ 
	}

	UsedPt=0;
	/* pick a pt in this section */
	for(i=1;i<=SECPTSptr[sid].cnt;i++) {
		if(SECPTSptr[sid].avail[i]==0 && SECPTSptr[sid].space[i]<3) {
			n=SECPTSptr[sid].n[i];e=SECPTSptr[sid].e[i];


			ii=i;  /* ii can be changed to -1 to accommodate centering Core pads - BUT currently centering isn't done */

			/* set the displacement */
			displ=TList[tlistid].acre* (float)TList[tlistid].wells;
			displ=displ/2.47;displ=displ*10000.0;displ=sqrt(displ);displ=displ/2.0;
			newdispl=displ;    /* init dynamic displ */

			/* Check for SGCore Area,  Convert n,e to r, c */
			piss=ConvToRC2(n,e,&r,&c);
			if(!piss) {  /* if not true, then error */
				printf("CONV error- Locate %f %f %d %d\n",n,e,r,c);exit(-1); 
			}

			acore=0;	factor=0;	/* initialize */
			if(SGcore[r*col+c]>0) {
				/* newdispl=1653.0;  */
				/* used to determine distance from lek perimeter accounting for pad size */
				factor=TList[tlistid].padpat;

				acore=1;  /* 640 acre spacing (=804 meter radius, added 6 cause seems like pads are too close); acore==1 will ensure that we check to see if
							 this pt is within 965 meters of a lek perimeter. */
				/* fprintf(diag,"%d %d core\n",sid,time); */
			}

			/* check on displacement from other pts and roads (Basic CheckPts check)  */
			/* zone, section, entry #, spacing in meters, acore=1 if within a core area, else =0 */
			/* In terms of within a core (when acore==1), CheckPts will determine if the pt is too close (<=965m)
				to the perimeter of a lek.  If so, tr=0. */
			tr=CheckPts(n,e,r,c,newdispl,acore,factor,auproj);  /* in Locatefunc.c */ /* dec 2015 */

			if( tr==1 && acore==1) {  /* this forces locating 1 pad in a core section */
				if(SECGptr[sid].ha1 >240) {
					if(SECptr[sid].tpads>0)tr=0;
				}else {
					tr=0;
				}
			}

			if(!tr) {
				if(acore==1) SECPTSptr[sid].space[i]=3;
				if(SECPTSptr[sid].space[i]>=3)SECPTSptr[sid].spacelimit++;/* tally of non-usable pts due to spacing in this section */
				SECTIONRD=0; /* allow evaluation of subsequent specs for this section */
				/* got to next point */
			} else {
				/* the last argument indicates this is not a horizontal well */
				code=NextStep(ii,sid,acore,n,e,r,c,TList[tlistid].padpat,TList[tlistid].wells,TList[tlistid].acre,auproj,(int)0);
				if(code==2)return((int)2); /* success; else try next point */
			}
		} /* valid pt */
	} /* for i */
	if(UsedPt==0)return((int)1);  /* nothing happened, nothing was set, use code==1 to jump out of this section */
}


/* continuation of the old DoPads2 */
char	NextStep(int ptk, int sid, int acore,float n, float e, int r, int c,int padpat, int wells,float acre,int auproj,int horiz)
{
	char	FindLekDistance(float,float),trlek;  /* determines the leks within 4 miles (6437 meters) from a disturbance pt */
	char	ProcessCorePad(float,float,int,int,char);  /* swings the cat */
	char	CheckPad(int,float,float,int,int,int *);
	int		returnid;
	char	rd,CheckRd(float,float,int,int),CheckRdNew(float,float,int,int,int);
	char	trcore,padfit;
	int		SetPad(int,int,int,int,float,float,int,float,int,int,int,int *, int *, int *, float *, float *);
	int		RoadSkirt(int,int,int,int,float,float),hit;
	void	AssignRoad(int,int,int,float,float);
	float	Ran2(float *);
	int		therow,thecol,theid;
	float	then,thee;
	int		loopcode;

	char	Curve(int, int, int *, float * , float *); 
	void	ProcessRds(), ClearRDS();
	void	DeriveRd(int,float,float,float,float);
	int		numcells;
	float	dn,de;



		trcore=1;
		if(acore==1) {  /* special processing if we are in a core area */
			trlek=FindLekDistance(n,e);  /* determine if any leks within a 4 mile buffer. trlek==1 if at least 1 lek pt within 4 miles of this pt.
			If trlek, the accession of the lek pts are recorded in LEKSptr */
			/* ProcessCorePad does the 4 mile buffer around the pt, and around
			effected lek pts.  Then it tallies suitable habitat (>=5% sage)
			and tallies extant disturbance, then determines the % disturbance. */
			/* trcore =0 if we can't place a pad (>=5% disturbance) */
			trcore=ProcessCorePad(n,e,r,c,trlek); 
			/* Must set space vars here if trcore==0 */
			if(!trcore) {
				if(ptk==-1) {  /* if core area */
				}else {
					SECPTSptr[sid].space[ptk]=3; /* can't use this pt due to spacing */
					if(SECPTSptr[sid].space[ptk]>=3)SECPTSptr[sid].spacelimit++;/* tally of non-usable pts due to spacing in this section */
				}
				SECTIONRD=0;  /* allow evaluation of subsequent specs */
				return ((char)1); /* nothing happened */
			}
		}

		/* trcore will be 1 if not in a core area. trcore is reset if in a core area - 
		will be 1 if we can place a pad in a core area, will be 0 if we can't */
		if(trcore) {  
			/* First check to see if the pad overlaps something it shouldn't */

			padfit=CheckPad(padpat,n,e,r,c,&returnid);  /* returns the pattern id that works */

			if(!padfit) { /* this is tricky cause maybe a pad of a different type (due to size) would work? need to re-arrange checks */
				if(ptk==-1) {
				}else {
					SECPTSptr[sid].space[ptk]=0; /* can't use this pt due to spacing */
					if(SECPTSptr[sid].space[ptk]>=3)SECPTSptr[sid].spacelimit++;/* tally of non-usable pts due to spacing in this section */
				}
				SECTIONRD=0;  /* we abort here cause of pad size; setting sectionRD=0 allows subsequent specs to be evaluated.  Perhaps
							  there are no valid roads possible in this section, but we don't really know until all pts in this section have
							  been evaluated.  SO, set this to 0 to allow other specs to be evaluated */
				return ((char)1); /* nothing happened */
			}else {

				/* Check to see if the shortest, straight road is possible.
				If not, then see if a path from another existing road is acceptable.  If not,
				then do not establish this pad.  CheckRd calls FindNearestCheck and
				DrawSLinepCheck to determine if a road target traverses a nontraverse and to
				find a road target that doesn't.  Starts with the shortest, then expands
				outward for a total of 100 hits or the distance from the pt to a target road
				is >4500 meters (derived from experimentation). */

				/* rd=CheckRd(n,e,auproj,sid); */

				rd=CheckRdNew(n,e,auproj,sid,ptk);  /* uses STORERDptr[] method.  ptk can not be -1!! */
				if(!rd && CURVEACTIVE==1)rd=Curve(r,c,&numcells,&dn,&de);  /* CURVE */


				if (rd==0){  /* can't find a road target that doesn't hit a nontraverse area */
					/* update spacelimits */ 
					if(ptk==-1) {
					}else {
						/* when using curve, if a road was not found, then turn the whole section off - */
						if(CURVEACTIVE==1) SECPTSptr[sid].counter=0;

						SECPTSptr[sid].space[ptk]=0; /* can't use this pt due to spacing */
						if(SECPTSptr[sid].space[ptk]>=3)SECPTSptr[sid].spacelimit++;/* tally of non-usable pts due to spacing in this section */
					}
					return ((char)1); /* nothing happened */

/*  to examine how this works, do the following */
								/*	padnum++;
									acore=PadSize2(i,id,padnum,padpat,n,e,&therow,&thecol,&theid,&then,&thee); */

				}else {   /* we have a suitable road target.  Establish the pad.
							If a road target isn't the shortest as defined by FindNearestCheck,
							then FORCEDRDflag ==1 and coords of the road target are 
							FORCEDRDr and FORCEDRDc. */
					SECTIONRD=0;  /* if we have at least 1 pt that has a valid road spur, then set SECTIONRD to 0; meaning
								  we may apply other specs to this section */
					padnum++;padactive++;dynamactive++;

					/* PadSize returns 0 if no road overlap; else =1 so no need to draw a road */
					if(	SetPad(r,c,padnum,returnid,n,e,wells,acre,auproj,sid,horiz,&therow,&thecol,&theid,&then,&thee) ==0) { 
							if(rd==2 && CURVEACTIVE==1) {
								RDVERTSptr = (RDVERTS *) calloc(5000,sizeof(RDVERTS));  /* hard coded to 5000 - see if this works */
								NOVERTS=0;
								/* printf("Here before DRD\n"); */
								DeriveRd(numcells,n,e,dn,de);  /* pad ne, road ne */
								ProcessRds();  /* Smooth output to RDlines#.csv, then globally store anchor pts */
								ClearRDS(); NEWrd=NULL;RDptr=NULL;ENDRptr=NULL; 
								free(RDVERTSptr);NOVERTS=0;
							}else if(Ran2(&idum)<.7) {
								/* Use RoadSkirt() to find a road to an edge cell (skirts the edge of a pad)   - 
								RoadSkirt(row and column of pad centroid, padnum, the pattern ID, the n and e of the pad centroid) */

								hit=RoadSkirt(therow,thecol,padnum,theid,then,thee);  

								/* the following will draw a line to the pad centroid */
								if(hit==0) {

									AssignRoad(therow,thecol,padnum,then,thee);    /* pass the n,e of the selected pad vertices to assignroad */

								}
							}else {  /* draw to pad centroid */

								AssignRoad(therow,thecol,padnum,then,thee);  

							}
						}
						if(ptk!=-1) {
							/* deactive the pt */
							SECPTSptr[sid].avail[ptk]=1; 
							/* decrement the no. of pts in this section */
							SECPTSptr[sid].counter--;
						}
						return((char)2);  /* code==2 means success */ 
				} /* if (rd) */
			} /* if padfit */
		} /* if trcore */
		return((char)1);
}
