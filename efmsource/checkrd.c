/* CheckRd() - Attempt to find a rd that doesn't traverse unacceptable areas. 
A new road can not traverse areas in the nontraverse map - e.g.,
1) a lek buffer (0.6 mile buffer)
2) NSO
3) large river
4) a reservoir
5) a town
6) road types = 1, 2, and 7 (interstates and exchanges)
7) steep slopes
8) rail road tracks
9) a mine
10) a power pant
11) ag lands.


Processing summary:

1 Find the nearest road 
2 Draw a straight line 
3 See if this line traverses any nontraverse areas
  3a If not, return the coords, and further processing determines elevation change 
  3b IF so, record this roads row & column, find another closest road, and repeat nontraverse test 

4 Repeat 1-3 50 times until a suitable road target is located; return the coords  

5 If after 50 times a suitable road is not located, return a code to indicate that the pad
	should not be established.
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
extern	float	grain;

/* n and e are coords of the proposed pad */
char CheckRd(float n, float e,int auproj, int sid)
{

	int		FindNearestCheck(int ,int,int *,int *);
	char	ConvToRC2(float,float, int *, int *),piss;
	void	ConvToPts(int,int, float *,float *);
	char	DrawSLinePCheck(float, float , float , float,int *, int *);  
	int		r,c,type,ii,jj,id;
	float	tn,te,dist;
	char	tr;
	int		rr,cc;
	long long	indexl;
	float	DISTANCE(float,float,float,float);
	int		maxNUM;
	char	theflag;


	theflag=0;
	/* if(loop==2 && auproj==164 && sid==22731)theflag=1; */


	maxNUM=50;  /* This maxes out at 50 - set in Init() */

	/* Find the nearest road */
	/* draw a straight line */
	/* See if this line traverses any notraverse */
	/* If not, return and let FindNearest() do its thing */
	/* IF so, record this road r & c, find another closest road, and repeat.
	Do this x times until a suitable road target is located.  Record the coords 
	of an acceptable road target, return the coords.  If after x times no
	suitable road is located, return a code to indicate that the pad
	should not be established. */

	FORCEDRDflag=0;  /* initialize */
	OLDRDptr[1].no=0;  /* Initialize every entry into this routine */
	piss=ConvToRC2(n,e,&r,&c);
	if(!piss) {
		printf("in checkrd %f %f %d %d\n",n,e,r,c);exit(-1);
	}

	type=FindNearestCheck(r,c,&ii,&jj); 
	if(type!=2) {printf("ERROR, type !=2 in checkrds %d\n",type);exit(-1);}
	ConvToPts(ii,jj,&tn,&te);
	/* returns 0 if the road hits a nontraverse; else 1 = AOK to draw using standard logic */
	tr=DrawSLinePCheck(n, e, tn, te,&rr,&cc);   /* rr and cc is where we hit nontraverse */
	if(theflag)fprintf(checkrd,"%f, %f, %f, %f, %d\n",n,e,tn,te,(int)tr);
	if(tr) {
		return(tr);  /* use standard logic to derive a road */
	}else { /* We need to continue the search for an acceptable road */ 
		/* store old road pts - FindNearestCheck will skip over these pts */
		OLDRDptr[1].no++;
		if(OLDRDptr[1].no>maxNUM) {tr=0;return(tr);}  /* we tried, but no beef, so do not establish the pad */
		OLDRDptr[OLDRDptr[1].no].r=ii;OLDRDptr[OLDRDptr[1].no].c=jj;  /* do not use this road target again */
	}
	dist=DISTANCE((float)r,(float)c,(float)ii,(float)jj);dist=dist*grain;

	while(OLDRDptr[1].no<=maxNUM && dist<4500) {
			type=FindNearestCheck(r,c,&ii,&jj);
			ConvToPts(ii,jj,&tn,&te);
			tr=DrawSLinePCheck(n, e, tn, te,&rr,&cc); 
		if(theflag)fprintf(checkrd,"%f, %f, %f, %f, %d\n",n,e,tn,te,(int)tr);
			if(tr) {  /* found a target, save the coordinates */

		/* printf("olddist %d %f %f %f %d %d\n",OLDRDptr[1].no,dist,n,e,auproj,sid); */


				FORCEDRDr=ii; FORCEDRDc=jj;  FORCEDRDflag=1;
				return(tr);  /* FORCEDRDflag indicates use of pre-determine road target */
			}
			OLDRDptr[1].no++;
			if(OLDRDptr[1].no>maxNUM) {tr=0;return(tr);}  /* we tried, but no beef, so do not establish the pad */
			OLDRDptr[OLDRDptr[1].no].r=ii;OLDRDptr[OLDRDptr[1].no].c=jj;/* do not use this road target again */
			dist=DISTANCE((float)r,(float)c,(float)ii,(float)jj);dist=dist*grain;
	}
	tr=0;return(tr);

}