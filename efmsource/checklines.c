/* CheckLines - This is modified version of DrawSLine, but designed to count no. of road cells between
two pts.
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include "efm.h"
#include "geo.h"

extern	int		row,col;
extern	float	grain;


/* number is the number of cells checked,  icnt is the no. of rd cells */
void	CheckLines(int i, int j, int ii, int jj,int *icnt,int *number)
{
	int		dist;  /* distance in no. of cells */
	int		deltar,deltac;
	float	angle;
	int		displ,r,c,quad;
	float	a,b;
	int		cnt,cells;
	long long	indexl;



	a=i-ii;b=j-jj;
	dist = sqrt( (a*a) + (b*b) );
	deltar = abs(i-ii);deltac=abs(j-jj);

	cnt=cells=0;

	/* quad 1 */
	if(ii<i && jj>=j) {
		if(deltar==0 ||deltac==0) {
			angle=0;
		} else {
			angle=(float)deltac/(float)deltar;  /* delta east /delta north */
		}
		angle=atan(angle);
		for(displ=1;displ<=dist;displ++) {
			r=displ*cos(angle);r=r*(-1.0);c=displ*sin(angle);
			if(r!=0 || c!=0 ) {
				r=i+r;c=j+c;
				indexl=(long long)r * (long long)col;indexl+=(long long)c;
				if(rds[indexl]>0 && pads[indexl]<=0)cnt++;   cells++;
			}
		} /* end of for displ */

	} else if(ii>=i && jj>j) {		/* quad 2*/
		if(deltar==0 ||deltac==0) {
			angle=0;
		} else {
			angle=(float)deltar/(float)deltac;  /* delta north /delta east */
		}
		angle=atan(angle);
		for(displ=1;displ<=dist;displ++) {
			r=displ*sin(angle);c=displ*cos(angle);
			if(r!=0 || c!=0 ) {
				r=i+r;c=j+c;
				indexl=(long long)r * (long long)col;indexl+=(long long)c;
				if(rds[indexl]>0 && pads[indexl]<=0)cnt++;   cells++;
			}
		} /* end of for displ */

	} else if(ii>i && jj<=j) {	/* quad 3 */
		if(deltar==0 ||deltac==0) {
			angle=0;
		} else {
			angle=(float)deltac/(float)deltar;  /* delta east /delta north */
		}
		angle=atan(angle);
		for(displ=1;displ<=dist;displ++) {
			r=displ*cos(angle);c=displ*sin(angle);c=c*(-1.0);
			if(r!=0 || c!=0 ) {
				r=i+r;c=j+c; 
				indexl=(long long)r * (long long)col;indexl+=(long long)c;
				if(rds[indexl]>0 && pads[indexl]<=0)cnt++;   cells++;
			}
		} /* end of for displ */

	} else if(ii<=i && jj<j) {		/* quad 4 */
		if(deltar==0 ||deltac==0) {
			angle=0;
		} else {
			angle=(float)deltar/(float)deltac;  /* delta north /delta east */
		}
		angle=atan(angle);
		for(displ=1;displ<=dist;displ++) {
			r=displ*sin(angle);r=r*(-1.0);c=displ*cos(angle);c=c*(-1.0);
			if(r!=0 || c!=0 ) {
				r=i+r;c=j+c;
				indexl=(long long)r * (long long)col;indexl+=(long long)c;
				if(rds[indexl]>0 && pads[indexl]<=0)cnt++;   cells++;
			}
		} /* end of for displ */
	}
	*icnt=cnt;*number=cells;
}

