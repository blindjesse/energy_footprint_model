/* DrawSLinePMOD()- Draws a straight line between 2 points (UTM coordinates), and is a modified version of DrawSLineP().   This version
draws a straight line to help terminate a road initiated in DrawLinemP().  Anchor pts and road vertices are handled in DrawLinemP. 
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

void	DrawSLinePMOD(float sn, float se, float tn, float te)
{
	int		dist;  /* distance in no. of cells */
	double	deltar,deltac;
	float	angle;
	int		displ;
	float	a,b;
	float	n,e;
	int		ii,i,jj,j,r,c;
	char	ConvToRC2(float,float,int *, int *),tr;
	void	ferr(int,float,float,int,int);

	a=sn-tn;b=se-te;
	dist = sqrt( (a*a) + (b*b) );
	deltar = fabs(sn-tn);deltac=fabs(se-te);

	i=sn;j=se;ii=tn;jj=te;


	/* quad 1 */
	if(ii<i && jj>=j) {
		if(deltar==0 ||deltac==0) {
			angle=0;
		} else {
			angle= (float)deltac/(float)deltar;  /* delta east /delta north */
		}
		angle=atan(angle);
		for(displ=30;displ<=dist;displ=displ+30) {	/* vertices every 30 m */
			n=displ*cos(angle);n=n*(-1.0);e=displ*sin(angle);
			n=sn+n;e=se+e;
			tr=ConvToRC2(n,e,&r,&c);if(!tr) ferr((int)1,n,e,r,c);
			ENDRptr->r=r;ENDRptr->c=c;ENDRptr->hit=1;ENDRptr->quad=1;
				ENDRptr->nextptr=(RDS *) calloc(1,sizeof(RDS));
				ENDRptr=ENDRptr->nextptr;
				if(ENDRptr==NULL){printf("ERROR, ENDRptr==NULL\n");exit(-1);}
		} /* end of for displ */
			tr=ConvToRC2(tn,te,&r,&c);if(!tr) ferr((int)1,tn,te,r,c);
			ENDRptr->r=r;ENDRptr->c=c;ENDRptr->hit=1;ENDRptr->quad=1;
				ENDRptr->nextptr=(RDS *) calloc(1,sizeof(RDS));
				ENDRptr=ENDRptr->nextptr;
				if(ENDRptr==NULL){printf("ERROR, ENDRptr==NULL\n");exit(-1);}

	} else if(ii>=i && jj>j) {		/* quad 2*/
		if(deltar==0 ||deltac==0) {
			angle=0;
		} else {
			angle=(float)deltar/(float)deltac;  /* delta north /delta east */
		}
		angle=atan(angle);	
		for(displ=30;displ<=dist;displ=displ+30) {
			n=displ*sin(angle);e=displ*cos(angle);
			n=sn+n;e=se+e;
			tr=ConvToRC2(n,e,&r,&c);if(!tr) ferr((int)2,n,e,r,c);
			ENDRptr->r=r;ENDRptr->c=c;ENDRptr->hit=1;ENDRptr->quad=2;
				ENDRptr->nextptr=(RDS *) calloc(1,sizeof(RDS));
				ENDRptr=ENDRptr->nextptr;
				if(ENDRptr==NULL){printf("ERROR, ENDRptr==NULL\n");exit(-1);}
		} /* end of for displ */
			tr=ConvToRC2(tn,te,&r,&c);if(!tr) ferr((int)2,tn,te,r,c);
			ENDRptr->r=r;ENDRptr->c=c;ENDRptr->hit=1;ENDRptr->quad=2;
				ENDRptr->nextptr=(RDS *) calloc(1,sizeof(RDS));
				ENDRptr=ENDRptr->nextptr;
				if(ENDRptr==NULL){printf("ERROR, ENDRptr==NULL\n");exit(-1);}	

	} else if(ii>i && jj<=j) {	/* quad 3 */
		if(deltar==0 ||deltac==0) {
			angle=0;
		} else {
			angle=(float)deltac/(float)deltar;  /* delta east /delta north */
		}
		angle=atan(angle);
		for(displ=30;displ<=dist;displ=displ+30) {
			n=displ*cos(angle);e=displ*sin(angle);e=e*(-1.0);
			n=sn+n;e=se+e;
			tr=ConvToRC2(n,e,&r,&c);if(!tr) ferr((int)3,n,e,r,c);
			ENDRptr->r=r;ENDRptr->c=c;ENDRptr->hit=1;ENDRptr->quad=3;
				ENDRptr->nextptr=(RDS *) calloc(1,sizeof(RDS));
				ENDRptr=ENDRptr->nextptr;
				if(ENDRptr==NULL){printf("ERROR, ENDRptr==NULL\n");exit(-1);}
		} /* end of for displ */
			tr=ConvToRC2(tn,te,&r,&c);if(!tr) ferr((int)3,tn,te,r,c);
			ENDRptr->r=r;ENDRptr->c=c;ENDRptr->hit=1;ENDRptr->quad=3;
				ENDRptr->nextptr=(RDS *) calloc(1,sizeof(RDS));
				ENDRptr=ENDRptr->nextptr;
				if(ENDRptr==NULL){printf("ERROR, ENDRptr==NULL\n");exit(-1);}	

	} else if(ii<=i && jj<j) {		/* quad 4 */
		if(deltar==0 ||deltac==0) {
			angle=0;
		} else {
			angle=(float)deltar/(float)deltac;  /* delta north /delta east */
		}
		angle=atan(angle);
		for(displ=30;displ<=dist;displ=displ+30) {
			n=displ*sin(angle);n=n*(-1.0);e=displ*cos(angle);e=e*(-1.0);
			n=sn+n;e=se+e;
			tr=ConvToRC2(n,e,&r,&c);if(!tr) ferr((int)4,n,e,r,c);
			ENDRptr->r=r;ENDRptr->c=c;ENDRptr->hit=1;ENDRptr->quad=4;
				ENDRptr->nextptr=(RDS *) calloc(1,sizeof(RDS));
				ENDRptr=ENDRptr->nextptr;
				if(ENDRptr==NULL){printf("ERROR, ENDRptr==NULL\n");exit(-1);}
		} /* end of for displ */
			tr=ConvToRC2(tn,te,&r,&c);if(!tr) ferr((int)4,tn,te,r,c);
			ENDRptr->r=r;ENDRptr->c=c;ENDRptr->hit=1;ENDRptr->quad=4;
				ENDRptr->nextptr=(RDS *) calloc(1,sizeof(RDS));
				ENDRptr=ENDRptr->nextptr;
				if(ENDRptr==NULL){printf("ERROR, ENDRptr==NULL\n");exit(-1);}			
	}
}

void	ferr(int code, float tn, float te, int r, int c)
{
	printf("ConvToRC2 error in drawslinepmod, quad= %d\n",code);
	printf("%f %f %d %d\n",tn,te,r,c);
	exit(-1);
}