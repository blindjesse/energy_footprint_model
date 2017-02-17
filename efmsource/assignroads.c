/* AssignRoads() - Initiates finding the closest, flattest road to a new pad.  
Calls DrawLinemP() to establish a road.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include "efm.h"
#include "geo.h"
#include "roads.h"


extern	int		row,col;


void	AssignRoad2(int id, int i, int j, float n, float e, int ii, int jj, float tn, float te)
{
	void	DrawLinemP(int,int , int , int , int,float,float,float,float );

	/* Allocate the interim smoothing struct */
	RDVERTSptr = (RDVERTS *) calloc(5000,sizeof(RDVERTS));  /* hard coded to 5000 - see if this works */
	NOVERTS=0;

	DrawLinemP(id,i,j,ii,jj,n,e,tn,te);

}

void	AssignRoad(int i, int j,int id,float then, float thee)
{
	int		ii,jj,type,index,seq;
	int		FindNearest(int,int,int, int *, int *);
	/* void	DrawSLine(int,int,int,int);	*/	/* original straight line based on grid cells */
	void	DrawSLineP(float,float,float,float,int);	/* straight line based on points */
	void	ConvToPtsF(int, int, float *, float *);
	void	DrawLinemP(int,int , int , int , int,float,float,float,float );
	/* void	DrawLinem(int , int , int , int ); */
	float	n,e,tn,te;
	RDSTORE	*Rptr; 
	long long	indexl;
	void		pause();


	if(FORCEDRDflag==0) {
		type=FindNearest(i,j,id,&ii,&jj); /* find nearest rd based on grid cells */
		if(type!=2) {printf("ERROR, type !=2 %d\n",type);exit(-1);}
	}else {
		type=2;ii=FORCEDRDr;jj=FORCEDRDc;  /* used the rd coords we previously found */
	}

	/* If type ==2, then we are dealing with a road, and we should have a utm coord associated with this road.
	Otherwise, we are dealing with a pad and we use the ii,jj coords to derive a UTM coord. (NOT USED ANYMORE) */


	/* Allocate the interim smoothing struct */
	RDVERTSptr = (RDVERTS *) calloc(5000,sizeof(RDVERTS));  /* hard coded to 5000 - see if this works */
	NOVERTS=0;

	/* ConvToPtsF(i,j,&n,&e); n and e are now passed from DoPads -  actual coords of source */
	n=then;e=thee;


	if(type==2) {
		indexl=(long long)ii * (long long)col;indexl+=(long long)jj;
		if(rds[indexl]==-10) {  /* this is an anchor pt not stored in rdmaster and rindex - 
								i.e., this is an anchor pt created within this time interval. */
			DrawLinemP(id,i,j,ii,jj,n,e,TEMPRptr[indexl].utmn,TEMPRptr[indexl].utme);
		} else {
			index=rdmaster[indexl];		/* rdmaster contains the index for the ROADANCptr pointer */
			seq=rdindex[indexl];		/* rdindex is the sequential entry in ROADANCptr->RDSTORE  */
			if(index<=0) {
				printf("ERROR, rdmaster is <=0\n");exit(-1);
			}
			Rptr=ROADANCptr[index].aptr;
			/*		DrawSLineP(n,e,Rptr[seq].utmn,Rptr[seq].utme,(int)10); */
			DrawLinemP(id,i,j,ii,jj,n,e,Rptr[seq].utmn,Rptr[seq].utme);
		}
	}else {
		ConvToPtsF(ii,jj,&tn,&te);			/* coords of target pad */
/*		DrawSLineP(n,e,tn,te,(int)10); */
		DrawLinemP(id,i,j,ii,jj,n,e,tn,te);
	}
}


/* NOTE - we expanded the search for -10 rd cells.  However, other negative numbers may occur (when dealing with
spur roads).  Leave as is but remember to change if spur roads are again used */
int		FindNearest(int i,int j,int id,int *r,int *c)
{
	/* return r, c of nearest pad or road.
	return 1 for pad, 2 for road */

	char	Valid(int,int);
   	int		ii,jj,type;
	int		br,er,bc,ec;
	char	tr;
	float	dist;
	void	CheckDist(int,int,int,float);
	int		icnt;
	long long	indexl;
	void	pause();

	tr=1;
	br=er=i;bc=ec=j;
	savetype=-1;icnt=0;
	roaddistance=9999999;


	/* need to save results & determine nearest after evaluating all orientations */
	while(tr) {
		br--;er++;bc--;ec++;
		ii=br;
		for(jj=bc;jj<=ec;jj++) { /* across the top */
			if(Valid(br,jj)) {
				indexl=(long long)br * (long long)col;indexl+=(long long)jj;
				if(rds[indexl]>0 || rds[indexl]==-10) {  /* this assumes that all new roads are type=10.  This allows roads not
														 stored in rdmaster, rdindex, and corresponding structs to be accessed.
														 See NOTE in roads.h regarding TEMPRptr */
					dist=sqrt( (float)(i-ii)*(float)(i-ii) + (float)(j-jj)*(float)(j-jj));
					CheckDist(br,jj,(int)2,dist);
				/*	*r=br;*c=jj;return ( (int)2); */
				}
/*	No longer due we include pads - need the stupid road lines to connect!!			if(pads[indexl]>0 && pads[indexl]!=id) {
						dist=sqrt( (i-ii)*(i-ii) + (j-jj)*(j-jj));
						CheckDist(br,jj,(int)1,dist);
				} */
			} /* end of if Valid */
		} /* across the top */
		jj=bc;
		for(ii=br;ii<=er;ii++) { /* across LHS */
			if(Valid(ii,bc)) {
				indexl=(long long)ii * (long long)col;indexl+=(long long)bc;
				if(rds[indexl]>0 || rds[indexl]==-10) {
					dist=sqrt( (float)(i-ii)*(float)(i-ii) + (float)(j-jj)*(float)(j-jj));
					CheckDist(ii,bc,(int)2,dist);
					/* *r=ii;*c=bc;return ( (int)2); */
				}
		/*		if(pads[indexl]>0 && pads[indexl]!=id)  {
						dist=sqrt( (i-ii)*(i-ii) + (j-jj)*(j-jj));
						CheckDist(ii,bc,(int)1,dist);
				} */
			} /* end of if Valid */
		} /* across the LHS */

		jj=ec;
		for(ii=br;ii<=er;ii++) { /* across RHS */
			if(Valid(ii,ec)) {
				indexl=(long long)ii * (long long)col;indexl+=(long long)ec;
				if(rds[indexl]>0 || rds[indexl]==-10) {
					dist=sqrt( (float)(i-ii)*(float)(i-ii) + (float)(j-jj)*(float)(j-jj));
					CheckDist(ii,ec,(int)2,dist);
					/* *r=ii;*c=ec;return ( (int)2); */
				}
		/*		if(pads[indexl]>0 && pads[indexl]!=id) {
						dist=sqrt( (i-ii)*(i-ii) + (j-jj)*(j-jj));
						CheckDist(ii,ec,(int)1,dist);
				} */
			} /* end of if Valid */
		} /* across the RHS */

		ii=er;
		for(jj=bc;jj<=ec;jj++) { /* across the bottom */
			if(Valid(er,jj)) {
				indexl=(long long)er * (long long)col;indexl+=(long long)jj;
				if(rds[indexl]>0 || rds[indexl]==-10) {
					dist=sqrt( (float)(i-ii)*(float)(i-ii) + (float)(j-jj)*(float)(j-jj));
					CheckDist(er,jj,(int)2,dist);
					/* *r=er;*c=jj;return ( (int)2); */
				}
		/*		if(pads[indexl]>0 && pads[indexl]!=id) {
						dist=sqrt( (i-ii)*(i-ii) + (j-jj)*(j-jj));
						CheckDist(er,jj,(int)1,dist);
				} */
			} /* end of if Valid */
		} /* across the bottom */

		/* if a hit, then start to increment icnt, but continue search.  This was implemented cause
		you can have the first hit on a diagonal but this may not be the closest road/pad.  Continuing the search
		may find a closer road/pad that is orthogonal to the source pt and closer than the first hit on a
		diagonal from the source pt. 12 hits was picked as a useful threshold. */
		if(savetype!=-1) {
			icnt++;  
			if(icnt>12) {
				*r=nearr;*c=nearc;return(savetype);
			}
		}
	} /* end of while */
}

void	CheckDist(int i,int j,int id,float dist)
{
	if(savetype==-1) {
		roaddistance=dist;
		nearr=i;nearc=j;savetype=id;
	}else {
		if(dist<roaddistance) {
			roaddistance=dist;
			nearr=i;nearc=j;savetype=id;
		}
	}
}