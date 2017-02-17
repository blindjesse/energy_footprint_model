/* RoadSkirt() - Adjusts the terminus of a road at a pad to
emulate skirting along the edge of a pad.  Emulates observed roads
on the SW Wyoming landscape.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include "efm.h"
#include "geo.h"
#include "roads.h"
#include "padpat.h"


extern	int		row,col;


/* return 1 if a road was drawn;  Else =0 cause something went wrong, so
Dopads3 (calls this routine) will use the centroid of a pad to find and
draw a line (via AssignRoad() ) */

int		RoadSkirt(int i,int j, int id, int patID,float centn,float cente) /* i,j are pad centroid, ID is padnum, patID is the pattern ID.
																		  centn and cente are n & e for the pad centroid - use these to
																		  convert the selected pattern to actual UTM's */
																		  
{

	int			k,count;
	int			r,c,seq;
	int			type;
	long long	indexl,index;
	RDSTORE		*Rptr;
	void		ConvToPtsF(int, int, float *, float *);
	float		sn,se,tn,te;
	void		Trig(float,float,float,float,int *,float*, float*,float *);
	PT			*ptr;
	float		n,e,disn,dise;
	void		pause();
	float		angle,compass,centangle,centcompass,dist,centdist;
	int			quad,centquad;
	float		mindist,minangle;
	int			hit;
	void		AssignRoad2(int i, int , int , float , float , int , int , float, float);
	float		saven,savee;
	char		ConvToRC2(float , float , int *, int *),tr;
	int			si,sj;
	int			FindNearest(int,int,int, int *, int *);


	/* Use the centroid to find the closest road */
	ConvToPtsF(i,j,&sn,&se);

	if(FORCEDRDflag==0) {
		type=FindNearest(i,j,id,&r,&c);
		if(type!=2) {printf("ERROR, type !=2 %d\n",type);exit(-1);}
	}else {
		type=2;r=FORCEDRDr;c=FORCEDRDc;
	}

	indexl=(long long)r * (long long)col;indexl+=(long long)c;
	if(rds[indexl]==-10) {  /* this is an anchor pt not stored in rdmaster and rindex - 
								i.e., this is an anchor pt created within this time interval. */
			tn=TEMPRptr[indexl].utmn; te=TEMPRptr[indexl].utme; 
	} else {
		index=rdmaster[indexl];		/* rdmaster contains the index for the ROADANCptr pointer */
		seq=rdindex[indexl];		/* rdindex is the sequential entry in ROADANCptr->RDSTORE  */
		if(index<=0) {
			printf("ERROR, rdmaster is <=0\n");exit(-1);
		}
		Rptr=ROADANCptr[index].aptr;
		tn=Rptr[seq].utmn;te=Rptr[seq].utme;
	}
	/* for each vertices of the pattern (source), derive distance and angle to the road pt (target).
	Do the same for each boundary vertices and use derived logic to select the new source pt. */

	Trig(sn,se,tn,te,&quad,&angle,&compass,&dist); /* printf("%d\n",patID); */
	centquad=quad;centangle=angle;centcompass=compass;centdist=dist;
	mindist=dist;minangle=90;hit=0;

	ptr=PATCptr[patID].aptr;
	disn=centn+PATCptr[patID].displn;dise=cente+PATCptr[patID].disple;
	count=PATCptr[patID].vertices;
	for(k=1;k<=count;k++) {
		n=disn-ptr[k].utmn; e=dise-ptr[k].utme;
		Trig(n,e,tn,te,&quad,&angle,&compass,&dist);
		/* if(loop==5) {
			printf("%f %f %f %f %d %f %f %f\n",n,e,tn,te,quad,angle,compass,dist);

		} */
		if(centquad==1) {
			if(dist<mindist) {
				if(angle<minangle){
					minangle=angle;
					hit=k;saven=n;savee=e;
				}
			}
			if((90.0-angle) <minangle){
					minangle=( 90.0-angle);
					hit=k;saven=n;savee=e;
			}
		}else {
			if(dist<mindist && compass >centcompass) {
				if(angle<minangle){
					minangle=angle;
					hit=k;saven=n;savee=e;
				}
				if((90.0-angle) <minangle){
					minangle=( 90.0-angle);
					hit=k;saven=n;savee=e;
				}
			}
		}
	}
/*	printf("%d\n",hit); */

	if(hit==0) {
		return( (int)0); /* results in call to AssignRoad() */
	}


	/* we now have coords for the source (along a pad) and the target (the selected road).  
	Pass this to AssignRoad2() to draw a line and exercise Drawlinemp() logic */
	tr=ConvToRC2(saven,savee,&si,&sj);
	if(!tr) {
		printf("ERROR in RoadSkirt\n");
		printf("%d %d %d %f %f %d %d %f %f\n",id,si,sj,saven,savee,r,c,tn,te);
		return( (int)0);  /* instead of exit(-1), lets see if we can continue despite this error.
						  noticed we can get too close to the edge of the AOI and this ConvToRC2
						  returns a non-valid coord. */
	}
	AssignRoad2(id,si,sj,saven,savee,r,c,tn,te);
	return( (int)1);  /* AOK */
}