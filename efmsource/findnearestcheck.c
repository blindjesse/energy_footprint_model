/* FindNearestCheck() - Modified version of FindNearest() but has the ability to skip previously
selected locations, and the starting distance for a search can be specified.  Used to find a road that
doesn't traverse a nontraverse area. 
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


/* NOTE - we expanded the search for -10 rd cells.  However, other netative numbers may occur (when dealing with
spur roads).  Leave as is but remember to change if spur roads are again used.
i and j are the starting location, */
int		FindNearestCheck(int i, int j, int *r, int *c)
{
	/* return r, c of nearest road.
	return 1 for pad (not used anymore), 2 for road */

	char	Valid(int,int);
   	int		ii,jj,type;
	int		br,er,bc,ec;
	char	tr;
	float	dist;
	void	CheckDist(int,int,int,float);
	int		icnt;
	long long	indexl;
	void	pause();
	char	UseIt(int,int);  /* checks to see if this r,c should be skipped */


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
				if(UseIt(br,jj)) {
					indexl=(long long)br * (long long)col;indexl+=(long long)jj;
					if(rds[indexl]>0 || rds[indexl]==-10) {  /* this assumes that all new roads are type=10.  This allows roads not
														 stored in rdmaster, rdindex, and corresponding structs to be accessed.
														 See NOTE in roads.h regarding TEMPRptr */
						dist=sqrt( (float)(i-ii)*(float)(i-ii) + (float)(j-jj)*(float)(j-jj));
						CheckDist(br,jj,(int)2,dist);
					}
				}
			} /* end of if Valid */
		} /* across the top */
		jj=bc;
		for(ii=br;ii<=er;ii++) { /* across LHS */
			if(Valid(ii,bc)) {
				if(UseIt(ii,bc)) {
					indexl=(long long)ii * (long long)col;indexl+=(long long)bc;
					if(rds[indexl]>0 || rds[indexl]==-10) {
						dist=sqrt( (float)(i-ii)*(float)(i-ii) + (float)(j-jj)*(float)(j-jj));
						CheckDist(ii,bc,(int)2,dist);
					}
				}
			} /* end of if Valid */
		} /* across the LHS */

		jj=ec;
		for(ii=br;ii<=er;ii++) { /* across RHS */
			if(Valid(ii,ec)) {
				if(UseIt(ii,ec)) {
					indexl=(long long)ii * (long long)col;indexl+=(long long)ec;
					if(rds[indexl]>0 || rds[indexl]==-10) {
						dist=sqrt( (float)(i-ii)*(float)(i-ii) + (float)(j-jj)*(float)(j-jj));
						CheckDist(ii,ec,(int)2,dist);
					}
				}
			} /* end of if Valid */
		} /* across the RHS */

		ii=er;
		for(jj=bc;jj<=ec;jj++) { /* across the bottom */
			if(Valid(er,jj)) {
				if(UseIt(er,jj)) {
					indexl=(long long)er * (long long)col;indexl+=(long long)jj;
					if(rds[indexl]>0 || rds[indexl]==-10) {
						dist=sqrt( (float)(i-ii)*(float)(i-ii) + (float)(j-jj)*(float)(j-jj));
						CheckDist(er,jj,(int)2,dist);
					}
				}
			} /* end of if Valid */
		} /* across the bottom */

		/* if a hit, then start to increment icnt, but continue search.  This was implemented cause
		you can have the first hit on a diagonal but this may not be the closest road/pad.  Continuing the search
		may find a closer road that is orthogonal to the source pt and closer than the first hit on a
		diagonal from the source pt. 12 hits is a useful threshold. */
		if(savetype!=-1) {
			icnt++;  
			if(icnt>12) {
				*r=nearr;*c=nearc;return(savetype);
			}
		}
	} /* end of while */
}


/* If the r and c are in the OLDRDptr list, then DO NOT use it.  THis r and c were previously 
selected and they didn't work. */
char	UseIt(int r, int c)
{
	int		i;
	char	tr;

	tr=1;
	for(i=1;i<=OLDRDptr[1].no;i++) {
		if(r==OLDRDptr[i].r && c==OLDRDptr[i].c) {
			tr=0;return(tr);
		}
	}
	return(tr);
}
