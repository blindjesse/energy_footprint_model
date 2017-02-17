/* DrawSLinePCheck()- Draws a straight line between points (UTM coordinates).  Modified version of DrawSLineP(), but here
we determine if a straight line traverses nontraversable areas.  Helps to eliminate a road crossing lakes, river, ....  
See CheckRds() for an explanation.  This routine does not store any points - it just checks to see if a road 
traverses a nontraverse cell.  Used for a quick check to see if a simple, straight line-road is possible.
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

char	DrawSLinePCheck(float ssn, float sse, float ttn, float tte,int *rr, int *cc)  
{
	int		dist;  /* distance in no. of cells */
	double	deltar,deltac;
	float	angle,angle1;
	int		displ;
	float	a,b;
	int		rdcode;
	float	n,e;
	int		ii,i,jj,j;
	int		r,c;
	char	ConvToRC2(float ,float, int *, int *),piss;
	char	tr;
	void	ferrr(int,float,float,float,float,float,float,int,int);
	void	pause();
	/* void	Rec(float,float); */
	float	sn,se,tn,te;



	sn=ssn;se=sse;tn=ttn;te=tte;

	/* sn=4567055;se=496559;
	tn=4566185;te=496558;
	printf("drawsl\n");  */

 




	/* Check the ending pts first.  Don't think the end pts are actually checked in the following logic */
	piss=ConvToRC2(tn,te,&r,&c);
	if(!piss) {
		printf("in drawslinepcheck %f %f %f %f %d %d\n",sn,se,tn,te,r,c);exit(-1);
	}
	if(nontraverse[r*col+c]>0){*rr=r;*cc=c;tr=0;return(tr);} 


	tr=1;
	a=sn-tn;b=se-te;
	dist = sqrt( (a*a) + (b*b) );
	deltar = fabs(sn-tn);deltac=fabs(se-te);

	i=sn;j=se;ii=tn;jj=te;


	/* quad 1 */
	if(ii<i && jj>=j) {
		if(deltar==0 ||deltac==0) {
			angle=0;
		} else {
			angle=deltac/deltar;  /* delta east /delta north */
		}
		angle=atan(angle);
		for(displ=30;displ<=dist;displ=displ+30) {	/* vertices every 30 m */
			n=displ*cos(angle);n=n*(-1.0);e=displ*sin(angle);
			n=sn+n;e=se+e;
			piss=ConvToRC2(n,e,&r,&c);
			if(!piss) {*rr=r;*cc=c;tr=0;return(tr);} 
				if(!piss)ferrr((int)1,sn,se,tn,te,n,e,r,c);
			if(nontraverse[r*col+c]>0){*rr=r;*cc=c;tr=0;return(tr);} 
		} /* end of for displ */

	} else if(ii>=i && jj>j) {		/* quad 2*/
		if(deltar==0 ||deltac==0) {
			angle=0;
		} else {
			angle=deltar/deltac;  /* delta north /delta east */
		}
		angle=atan(angle);
		for(displ=30;displ<=dist;displ=displ+30) {
			n=displ*sin(angle);e=displ*cos(angle);
			n=sn+n;e=se+e;
			piss=ConvToRC2(n,e,&r,&c);
			if(!piss) {*rr=r;*cc=c;tr=0;return(tr);} 
						if(!piss)ferrr((int)2,sn,se,tn,te,n,e,r,c);
			if(nontraverse[r*col+c]>0){*rr=r;*cc=c;tr=0;return(tr);}  
		} /* end of for displ */

	} else if(ii>i && jj<=j) {	/* quad 3 */
		if(deltar==0 ||deltac==0) {
			angle=0;
		} else {
			angle=deltac/deltar;  /* delta east /delta north */
		}
		angle=atan(angle);
		for(displ=30;displ<=dist;displ=displ+30) {
			n=displ*cos(angle);e=displ*sin(angle);e=e*(-1.0);
			n=sn+n;e=se+e;
			piss=ConvToRC2(n,e,&r,&c);
			if(!piss) {*rr=r;*cc=c;tr=0;return(tr);} 
						if(!piss)ferrr((int)3,sn,se,tn,te,n,e,r,c);
			if(nontraverse[r*col+c]>0) {*rr=r;*cc=c;tr=0;return(tr);} 
		} /* end of for displ */

	} else if(ii<=i && jj<j) {		/* quad 4 */
		if(deltar==0 ||deltac==0) {
			angle=0;
		} else {
			angle=deltar/deltac;  /* delta north /delta east */
		}
		angle1=angle;
		angle=atan(angle);
		for(displ=30;displ<=dist;displ=displ+30) {
			n=displ*sin(angle);n=n*(-1.0);e=displ*cos(angle);e=e*(-1.0);
			n=sn+n;e=se+e;
		/*	if(padnum==31236){printf("%f %f %d %f %f %f %f\n",n,e,displ,sn,se,tn,te);
			printf("%f %f %f %f\n",deltar,deltac,angle1,angle);pause();  } */

/*			Rec(n,e); */
			piss=ConvToRC2(n,e,&r,&c);
			if(!piss) {*rr=r;*cc=c;tr=0;return(tr);} 
						if(!piss)ferrr((int)4,sn,se,tn,te,n,e,r,c);
			if(nontraverse[r*col+c]>0) {*rr=r;*cc=c;tr=0;return(tr);} 
		} /* end of for displ */
	}
	return(tr);

}
void	ferrr(int code, float sn, float se, float tn, float te,float n, float e, int r, int c)
{
	printf("in Drawslinepcheck %d %f %f %f %f %f %f %d %d\n",code,sn,se,tn,te,n,e,r,c);
	printf("padnum %d\n",padnum);exit(-1);
}

 /*  void	Rec(float n, float e)
{
	fprintf(linesf,"%f, %f,1,1,1,1\n",n,e);
}  */