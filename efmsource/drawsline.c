/* DrawSLine()- Draws a straight line between 2 locations.  This is
part of the road-pathway processing.
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include "efm.h"
#include "geo.h"
#include "rdend.h"

extern	int		row,col;
extern	float	grain;



void	DrawSLine(int i, int j, int ii, int jj,int rdcode)
{
	int		dist;  /* distance in no. of cells */
	int		deltar,deltac;
	float	angle;
	int		displ,r,c,quad;
	float	a,b;
	int		k,l,lastr,lastc;
	char	Diag(int,int);
	void	ConvToPts(int,int,float *,float *);
	float	n,e;
	void	CheckCore(int,int);   /* updates core-area disturbance due to roads */
	long long	indexl;
	void		pause();

	printf("DrawSLINE is not functional\n");pause();

	a=i-ii;b=j-jj;
	dist = sqrt( (a*a) + (b*b) );
	deltar = abs(i-ii);deltac=abs(j-jj);



	/* quad 1 */
	if(ii<i && jj>=j) {
		if(deltar==0 ||deltac==0) {
			angle=0;
		} else {
			angle=(float)deltac/(float)deltar;  /* delta east /delta north */
		}
		lastr=i;lastc=j;
		angle=atan(angle);newline++;ConvToPts(lastr,lastc,&n,&e);fprintf(linesf,"%f, %f, %d, %d, 5\n",n,e,newline,time);
		fprintf(croads,"%f, %f, %d, %d, 1\n",n,e,newline+MAXRDEND,time);
		for(displ=1;displ<=dist;displ++) {
			r=displ*cos(angle);r=r*(-1.0);c=displ*sin(angle);
			if(r!=0 || c!=0 ) {
				r=i+r;c=j+c;
				indexl=(long long)r * (long long)col;indexl+=(long long)c;
				rds[indexl]=rdcode;CheckCore(r,c);
				k=r-lastr;l=c-lastc;
				if(k<0 && l!=0) {
					indexl=(long long)r * (long long)col;indexl+=(long long)(c-1);
					rds[indexl]=rdcode;CheckCore(r,c-1);
				}
				if(lastr !=r && lastc!=c){
					ConvToPts(r,c,&n,&e);fprintf(linesf,"%f, %f, %d, %d, 5\n",n,e,newline,time);
					fprintf(croads,"%f, %f, %d, %d, 1\n",n,e,newline+MAXRDEND,time);
				}
				lastr=r;lastc=c;
			}
			/*printf("r c displ %d %d %d %f\n",r,c,displ,angle/rads);  */
		} /* end of for displ */
		if(Diag(lastr,lastc)) {
			indexl=(long long)lastr * (long long)col;indexl+=(long long)(lastc+1);
			rds[indexl]=rdcode;ConvToPts(lastr,lastc+1,&n,&e);fprintf(linesf,"%f, %f, %d, %d, 5\n",n,e,newline,time);
			fprintf(croads,"%f, %f, %d, %d, 1\n",n,e,newline+MAXRDEND,time);
			CheckCore(lastr,lastc+1);
		}
		/* printf("%d %d %d %d\n",i,j,ii,jj); */
	} else if(ii>=i && jj>j) {		/* quad 2*/
		if(deltar==0 ||deltac==0) {
			angle=0;
		} else {
			angle=(float)deltar/(float)deltac;  /* delta north /delta east */
		}
		angle=atan(angle);
		lastr=i;lastc=j;newline++;ConvToPts(lastr,lastc,&n,&e);fprintf(linesf,"%f, %f, %d, %d, 5\n",n,e,newline,time);
		fprintf(croads,"%f, %f, %d, %d, 1\n",n,e,newline+MAXRDEND,time);
		for(displ=1;displ<=dist;displ++) {
			r=displ*sin(angle);c=displ*cos(angle);
			if(r!=0 || c!=0 ) {
				r=i+r;c=j+c;
				indexl=(long long)r * (long long)col;indexl+=(long long)c;
				rds[indexl]=rdcode;CheckCore(r,c);
				k=r-lastr;l=c-lastc;
				if(k>0 && l!=0) {
					indexl=(long long)r * (long long)col;indexl+=(long long)(c-1);
					rds[indexl]=rdcode; CheckCore(r,c-1);
				}
				if(lastr !=r && lastc!=c){
					ConvToPts(r,c,&n,&e);fprintf(linesf,"%f, %f, %d, %d, 5\n",n,e,newline,time);
					fprintf(croads,"%f, %f, %d, %d, 1\n",n,e,newline+MAXRDEND,time);
				}
				lastr=r;lastc=c;
			}
			/* printf("r c displ %d %d %d %f\n",r,c,displ,angle/rads); */
		} /* end of for displ */
		if(Diag(lastr,lastc)) {
			indexl=(long long)lastr * (long long)col;indexl+=(long long)(lastc+1);
			rds[indexl]=rdcode;ConvToPts(lastr,lastc+1,&n,&e);fprintf(linesf,"%f, %f, %d, %d, 5\n",n,e,newline,time);
			fprintf(croads,"%f, %f, %d, %d, 1\n",n,e,newline+MAXRDEND,time);
			CheckCore(lastr,lastc+1);
		}
		/* printf("%d %d %d %d\n",i,j,ii,jj); */
	} else if(ii>i && jj<=j) {	/* quad 3 */
		if(deltar==0 ||deltac==0) {
			angle=0;
		} else {
			angle=(float)deltac/(float)deltar;  /* delta east /delta north */
		}
		angle=atan(angle);
		lastr=i;lastc=j;newline++;ConvToPts(lastr,lastc,&n,&e);fprintf(linesf,"%f, %f, %d, %d, 5\n",n,e,newline,time);
		fprintf(croads,"%f, %f, %d, %d, 1\n",n,e,newline+MAXRDEND,time);
		for(displ=1;displ<=dist;displ++) {
			r=displ*cos(angle);c=displ*sin(angle);c=c*(-1.0);
			if(r!=0 || c!=0 ) {
				r=i+r;c=j+c;
				indexl=(long long)r * (long long)col;indexl+=(long long)c;
				rds[indexl]=rdcode;CheckCore(r,c);
				k=r-lastr;l=lastc-c;
				if(k>0 && l!=0) {
					indexl=(long long)r * (long long)col;indexl+=(long long)(c+1);
					rds[indexl]=rdcode; CheckCore(r,c+1);
				}
				if(lastr !=r && lastc!=c){
					ConvToPts(r,c,&n,&e);fprintf(linesf,"%f, %f, %d, %d, 5\n",n,e,newline,time);
					fprintf(croads,"%f, %f, %d, %d, 1\n",n,e,newline+MAXRDEND,time);
				}
				lastr=r;lastc=c;
			}
			/* printf("r c displ %d %d %d %f\n",r,c,displ,angle/rads); */
		} /* end of for displ */
		if(Diag(lastr,lastc)) {
			indexl=(long long)lastr * (long long)col;indexl+=(long long)(lastc-1);
			rds[indexl]=rdcode;ConvToPts(lastr,lastc-1,&n,&e);fprintf(linesf,"%f, %f, %d, %d, 5\n",n,e,newline,time);
			fprintf(croads,"%f, %f, %d, %d, 1\n",n,e,newline+MAXRDEND,time);
			CheckCore(lastr,lastc-1);
		}
		/* printf("%d %d %d %d\n",i,j,ii,jj); */
	} else if(ii<=i && jj<j) {		/* quad 4 */
		if(deltar==0 ||deltac==0) {
			angle=0;
		} else {
			angle=(float)deltar/(float)deltac;  /* delta north /delta east */
		}
		angle=atan(angle);
		lastr=i;lastc=j;newline++;ConvToPts(lastr,lastc,&n,&e);fprintf(linesf,"%f, %f, %d, %d, 5\n",n,e,newline,time);
		fprintf(croads,"%f, %f, %d, %d, 1\n",n,e,newline+MAXRDEND,time);
		for(displ=1;displ<=dist;displ++) {
			r=displ*sin(angle);r=r*(-1.0);c=displ*cos(angle);c=c*(-1.0);
			if(r!=0 || c!=0 ) {
				r=i+r;c=j+c;
				indexl=(long long)r * (long long)col;indexl+=(long long)c;
				rds[indexl]=rdcode;CheckCore(r,c);
				k=r-lastr;l=lastc-c;
				if(k<0 && l!=0) {
					indexl=(long long)r * (long long)col;indexl+=(long long)(c+1);
					rds[indexl]=rdcode; CheckCore(r,c+1);
				}
				if(lastr !=r && lastc!=c){
					ConvToPts(r,c,&n,&e);fprintf(linesf,"%f, %f, %d, %d, 5\n",n,e,newline,time);
					fprintf(croads,"%f, %f, %d, %d, 1\n",n,e,newline+MAXRDEND,time);
				}
				lastr=r;lastc=c;
			}
			/* printf("r c displ %d %d %d %f\n",r,c,displ,angle/rads); */
		} /* end of for displ */
		if(Diag(lastr,lastc)) {
			indexl=(long long)lastr * (long long)col;indexl+=(long long)(lastc-1);
			rds[indexl]=rdcode;ConvToPts(lastr,lastc-1,&n,&e);fprintf(linesf,"%f, %f, %d, %d, 5\n",n,e,newline,time);
			fprintf(croads,"%f, %f, %d, %d, 1\n",n,e,newline+MAXRDEND,time);
			CheckCore(lastr,lastc-1);
		}
		/* printf("%d %d %d %d\n",i,j,ii,jj); */
	}
}

/* this should square-off the end of a new road with an existing road */
char	Diag(r,c)
{
	int		i,j,hit;
	char	Valid(int,int);
	long long	indexl;


	hit=0;
	i=r-1;j=c;
	if(Valid(i,j)) {
		indexl=(long long)i * (long long)col;indexl+=(long long)j;
		if(rds[indexl]>0)hit++;
	}
	i=r+1;j=c;
	if(Valid(i,j)) {
		indexl=(long long)i * (long long)col;indexl+=(long long)j;
		if(rds[indexl]>0)hit++;
	}
	i=r;j=c-1;
	if(Valid(i,j)) {
		indexl=(long long)i * (long long)col;indexl+=(long long)j;
		if(rds[indexl]>0)hit++;
	}
	i=r;j=c+1;
	if(Valid(i,j)) {
		indexl=(long long)i * (long long)col;indexl+=(long long)j;
		if(rds[indexl]>0)hit++;
	}

	if(hit<=1) return( (int)1);
	return( (int)0);
}



