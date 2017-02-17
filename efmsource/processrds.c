/* ProcessRds() */

/* This module is called after the vertices for each new road-line has been determined.  It smooths the line, outputs
smoothed vertices to RDlines#.csv, then assesses what is stored as anchor pts.
Then performs the global anchor point storage which follows
the previous logic.  After all this, RDVERTS is deallocated, 
and it begins again with the next newline. 



DrawLinemp() is the module used to create a line.  But, DrawSLineP is called 
when <=3 pts to draw a straight line, and DrawSLinePMOD is called to
cap-off the end of a line. See comments in DrawLinemp().


    Road vertices for a new road are first stored in RDVERTS.
    There are 3 types of vertices:  This follows the logic of previous versions.
	1) Vertices of terminal anchor points.  These can not modified.  They are stored
	   as anchor pts inside the model.  They are output to RDlines#.csv.
	2) Vertices along the line that are close to the center of a cell.  	   
	   These vertices are output to RDlines#.csv.  These
	   are stored as anchor pts inside the model but can be modified later.  This is old
	   logic, not sure it is appropriate, but we'll use it for now.  
	3) Vertices along the line that are more-or-less on the edge of a cell.  They
	   are output to RDlines#.csv but are not stored as anchor pts inside the model.

	NOTE - With the old logic, >1 pts in a cell tended have the most centered pt
	being stored as an anchor (modifiable).  However, a cell may originally only have 1 pt falling within, but
	the pt is not saved as even a modifiable anchor cell (is real close to the edge of a cell)?  
	Wonder if this results in roads not touching each other in such a cell (that is, 
	the cell becomes an anchor pt for a new road, 	but the pt in that cell used for a 
	previous road was not stored; thus the origin vertices of 
	the new road will not exactly match a pt on the first road?). ALSO, if a cell has a modifiable
	anchor pt which is later modified when a new road has the same cell as an anchor pt, 
	will roads also not connect?  OR, if a new road uses the modifiable cell pt as a terminal
	anchor, will that pt then become a non modifiable anchor pt - so no worries??
	
	 Previously, you'll see a CODE value - 1 is anchor pt, do not modify; - 0 an anchor point
     along a line that is not a terminal vertices.

	 An ACTION value was used to store pts in tanc.  1 was for points along a line that
	 were not to be stored as an anchor pt (not written to tanc).  2 was to store pts as anchor pts (write to tanc).
	 The CODE value would indicate if an existing pt in a cell would be over written (CODE=1) or
	 not (CODE=0).  Remember, only 1 pt per cell is stored as an anchor pt.

	 RDVERTS is used to smooth a road before global storage and output of road vertices.  This
	 smooths funky lines, and sets 'real' anchor pts that may be used for other lines.  Doing
	 this processing within should ensure that roads are physically attached all the way thru
	 a simulation.  

	 RDVERTS has the action and code values along with other key info.  After smoothing, we
	 re-assess which pts are stored as anchor pts.  The original code=1 terminal pts are 
	 not smoothed and are always stored as is.  The assessment is to determine if the
	 code and action values (i.e., storage logic) of code=0 and action=1 or 2
	 pts change.  All smoothed pts are output to RDlines#.csv.  It's the anchor pt storage
	 and the setting of rds & rdindex that is re-adjusted as necessary.  The distance
	 from the new coords to the centroid of the corresponding cell is evaluated.  If
	 <= .5*grain, then it is suitable as an anchor pt; else, it is not stored as an anchor pt.
	 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include "efm.h"
#include "geo.h"
#include "roads.h"
#include "rdend.h"
#include "rdgrid.h"

extern	int	col;
float	grain;



void	ProcessRds()
{
	int			beg,end,i;
	float		n,e,cn,ce;
	FILE		*fnam;
	char		ConvToRC2(float,float,int *, int *),tr;
	int			r,c;
	long long	indexl;
	void		pause();
	void		ConvToPtsF(int , int , float *, float *);
	float		dist,DISTANCE(float, float,float,float);
	int			smoothi;
	float		smoothf;
	int			checkline;  /* used to determine first and last coord of a new road segment; used to update endpts.dat */
	void		RdGridUp();
	void		UpdateRdStore(long long, float, float); /* sets RDSTOREptr set in FindRdCells() and used in checknewrds - eliminates call to exp1 in establish() everytime a new
														road is added to a section */

	/* this is set up to smooth using 3 pt step size */


	/* printf(" smooth %d %d\n",NOVERTS,loop); */
	if(NOVERTS<=0) return;  /* should never happen, but in case */


	smoothi=3;smoothf=smoothi;

	if(NOVERTS<=smoothi) {  /* set all necessary attributes and skip down to the global storage code */
		/* set index2.   Set .anchor based on action value. */
		for(i=1;i<=NOVERTS;i++) {
			if(RDVERTSptr[i].action==2)RDVERTSptr[i].anchor=1;
			RDVERTSptr[i].index2=RDVERTSptr[i].index;
		}
	
	
	/* >smooth pts */
	}else {
		/* perform smoothing */
		beg=1;end=smoothi;
		while(end<=NOVERTS) {
			e=0;n=0;
			for(i=beg;i<=end;i++) {
				e+=RDVERTSptr[i].e;
				n+=RDVERTSptr[i].n;
			}
			n=n/smoothf;e=e/smoothf;
			RDVERTSptr[beg+1].n=n;RDVERTSptr[beg+1].e=e;
			tr=ConvToRC2(n,e,&r,&c); 
			if(!tr) {
				printf("in processrds %f %f %d %d\n",n,e,r,c);exit(-1);
			}
			indexl=(long long)r * (long long)col;indexl+=(long long)c;
			RDVERTSptr[beg+1].index2=indexl;
			RDVERTSptr[beg+1].r=r;RDVERTSptr[beg+1].c=c;
			beg++;end++;
		}
		/* set index2, r and c for first and last pt - yep, same as the original, but just in case!  */
		tr=ConvToRC2(RDVERTSptr[1].n,RDVERTSptr[1].e,&r,&c); 
		if(!tr) {
				printf("in processrds %f %f %d %d\n",n,e,r,c);exit(-1);
		}
		indexl=(long long)r * (long long)col;indexl+=(long long)c;
		RDVERTSptr[1].index2=indexl;RDVERTSptr[1].r=r;RDVERTSptr[1].c=c;
		tr=ConvToRC2(RDVERTSptr[NOVERTS].n,RDVERTSptr[NOVERTS].e,&r,&c); 
		if(!tr) {
				printf("in processrds %f %f %d %d\n",n,e,r,c);exit(-1);
		}
		indexl=(long long)r * (long long)col;indexl+=(long long)c;
		RDVERTSptr[NOVERTS].index2=indexl;
		RDVERTSptr[NOVERTS].r=r;RDVERTSptr[NOVERTS].c=c;



		/* assess new anchor status */
		for(i=1;i<=NOVERTS;i++) {
			if(RDVERTSptr[i].code==0) {  /* if ==1, set anchor pt automatically, else check */
				ConvToPtsF(RDVERTSptr[i].r,RDVERTSptr[i].c, &cn, &ce);  /* centroid of this cell */	
				dist=DISTANCE(RDVERTSptr[i].n,RDVERTSptr[i].e,cn,ce);
				if(dist<=0.5*grain)RDVERTSptr[i].anchor=1;  /* this is an anchor pt; effectively resets ACTION code to 2 */ 
			}else {
				RDVERTSptr[i].anchor=1;  /* sets the terminal vertices */
			}
		}

	} /* end of if <=3 */


	/* NOTE - when adding output to FILE=croads, it looked like this is the only place where
	data are written to rdlines#, despite numerous - fprintf(linesf - statements.  Thus, thinking
	that this should be the only place where we write to rdsid[] */
	/* do global storage */
		checkline=0;  /* checkline is how we determine the first and last coord of a new road segment.  We use this to
					  update endpts.dat - opened as an append file */
		for(i=1;i<=NOVERTS;i++) {
			if(checkline==0){
				checkline=RDVERTSptr[i].newline;
				fprintf(endpts,"%7.4f %6.4f %d\n",RDVERTSptr[i].n,RDVERTSptr[i].e,RDVERTSptr[i].newline+MAXRDEND);
			}
			if(checkline!=RDVERTSptr[i].newline) {  /* write the previous record */
				if(i<=1){printf("ERROR, we have a problem writting to endpts in processrds.c\n");exit(-1);}
				fprintf(endpts,"%7.4f %6.4f %d\n",RDVERTSptr[i-1].n,RDVERTSptr[i-1].e,RDVERTSptr[i-1].newline+MAXRDEND);
				checkline=RDVERTSptr[i].newline;
			}

		/* All pts go to RDlines#.csv - 5 is the 1-sided buffer distance(m);  1 is CCODE.
		CCODE is for quick rasterization of all rds without regard to type. */
			fprintf(linesf,"%f, %f, %d, %d, 5, 1\n",RDVERTSptr[i].n,RDVERTSptr[i].e,RDVERTSptr[i].newline,RDVERTSptr[i].time);
		/* MEUS	fprintf(croads,"%f, %f, %d, %d, 1\n",RDVERTSptr[i].n,RDVERTSptr[i].e,RDVERTSptr[i].newline+MAXRDEND,RDVERTSptr[i].time);  *//* here the 1 means active */

			/* this is the old SetRds() function.  This would occur if ACTION==2 */
			/* Here, only pts with .anchor==1 are stored as anchor pts - corresponding row,col is
			set in rds[]; info is output to tanc for later storage in rdindex[], and in ROADANCPTR[].aptr via
			UpdateAnchorPts(). */
			if(RDVERTSptr[i].anchor==1) {
				if(rdindex[RDVERTSptr[i].index2]<=0) { /* if<=0, not previously set and no entry in ROADANCptr->RDptr[] */
					rds[RDVERTSptr[i].index2]=(-1)*RDVERTSptr[i].type;	/* set negative road type; this is rectified later */
					fprintf(anchor,"%20d %f %f %d\n",RDVERTSptr[i].index2,RDVERTSptr[i].n,RDVERTSptr[i].e,RDVERTSptr[i].code);
					UpdateRdStore(RDVERTSptr[i].index2,RDVERTSptr[i].n,RDVERTSptr[i].e); /* this usurps the need to call FindRdCells() in Establish everytime
																						 a new road is added to a section - may save time! */
					newanchors++;  /* used in updateanchor pts */
					TEMPRptr[RDVERTSptr[i].index2].utme=RDVERTSptr[i].e;
					TEMPRptr[RDVERTSptr[i].index2].utmn=RDVERTSptr[i].n;  /* within-time step temp storage - makes this pt accessible in
															  FindNearest() */

					/* This is the only place where we set rdsid[] - I think this should be the only place.
					MAXRDEND is the max ID of roads in roadbase.shp.  */
					rdsid[RDVERTSptr[i].index2]=RDVERTSptr[i].newline+MAXRDEND;
				}
			}

		}

		/* flush the final coords */
		fprintf(endpts,"%7.4f %6.4f %d\n",RDVERTSptr[NOVERTS].n,RDVERTSptr[NOVERTS].e,RDVERTSptr[NOVERTS].newline+MAXRDEND);
		maxrdend=MAXRDEND+newline;  /* update the master endpt counter */

		/* Update the 10-m road/node grid with this new line.  NOVERTS is global; maxrdend is global and set before this call */
		RdGridUp();



/* debug purposes */
	/* fnam=fopen("crap","w");
	   for(i=1;i<=NOVERTS;i++) {
		fprintf(fnam,"%d %d %d %d %d %f %f %d %d %f %f %d\n",i,RDVERTSptr[i].newline,RDVERTSptr[i].time,
			RDVERTSptr[i].index,RDVERTSptr[i].index2,RDVERTSptr[i].n,RDVERTSptr[i].e,
			RDVERTSptr[i].code,RDVERTSptr[i].action,
			RDVERTSptr[i].oldn,RDVERTSptr[i].olde,RDVERTSptr[i].anchor);
	}
	fclose(fnam);
	  pause(); */
}



