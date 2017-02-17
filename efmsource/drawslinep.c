/* DrawSLineP()- Draws a straight line between points (UTM coordinates), and is a modified version of DrawSLine() which draws
a straight line based on grid cells.  Compare with DrawSLinePMOD which is used to draw a straight line to help 
terminate a road generated in DrawLinemP().  */
 

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
extern	float	idum;

/* fprintf(linesf...) - records the vertices of new roads.
Used to call SetRdCells.  Based on previous logic, this means that ACTION defaulted to 2 */

void	DrawSLineP(float sn, float se, float tn, float te,int rcode)  /* rdcode is road type */
{
	int		dist;  /* distance in no. of cells */
	double		deltar,deltac;
	float	angle;
	int		displ;
	float	a,b;
	int		rdcode;
	float	n,e;
	int		ii,i,jj,j;
	float	Ran2(float *);
	int		one,zero;
	void	Record(float , float , int , int , int );

	rdcode=rcode;
	one=1;zero=0;

	a=sn-tn;b=se-te;
	dist = sqrt( (a*a) + (b*b) );
	deltar = fabs(sn-tn);deltac=fabs(se-te);

	i=sn;j=se;ii=tn;jj=te;


	/* quad 1 */
	if(ii<i && jj>=j) {
		if(deltar==0 ||deltac==0) {
			angle=0;
		} else {
			angle=(float)deltac/(float)deltar;  /* delta east /delta north */
		}
		angle=atan(angle);newline++;Record(sn,se,one,rdcode,(int)2); /* old example SetRdCells(sn,se,one,rdcode);	*/
		for(displ=30;displ<=dist;displ=displ+30) {	/* vertices every 30 m */
			n=displ*cos(angle);n=n*(-1.0);e=displ*sin(angle);
			n=sn+n;e=se+e;
			Record(n,e,zero,rdcode,(int)2);	
		} /* end of for displ */
			Record(tn,te,one,rdcode,(int)2); 

	} else if(ii>=i && jj>j) {		/* quad 2*/
		if(deltar==0 ||deltac==0) {
			angle=0;
		} else {
			angle=(float)deltar/(float)deltac;  /* delta north /delta east */
		}
		angle=atan(angle);newline++;
		Record(sn,se,one,rdcode,(int)2); 		
		for(displ=30;displ<=dist;displ=displ+30) {
			n=displ*sin(angle);e=displ*cos(angle);
			n=sn+n;e=se+e;
			Record(n,e,zero,rdcode,(int)2); 	
		} /* end of for displ */
		Record(tn,te,one,rdcode,(int)2);  

	} else if(ii>i && jj<=j) {	/* quad 3 */
		if(deltar==0 ||deltac==0) {
			angle=0;
		} else {
			angle=(float)deltac/(float)deltar;  /* delta east /delta north */
		}
		angle=atan(angle);newline++;
		Record(sn,se,one,rdcode,(int)2); 		
		for(displ=30;displ<=dist;displ=displ+30) {
			n=displ*cos(angle);e=displ*sin(angle);e=e*(-1.0);
			n=sn+n;e=se+e;
			Record(n,e,zero,rdcode,(int)2);  	
		} /* end of for displ */
		Record(tn,te,one,rdcode,(int)2);	

	} else if(ii<=i && jj<j) {		/* quad 4 */
		if(deltar==0 ||deltac==0) {
			angle=0;
		} else {
			angle=(float)deltar/(float)deltac;  /* delta north /delta east */
		}
		angle=atan(angle);newline++;
		Record(sn,se,one,rdcode,(int)2);			
		for(displ=30;displ<=dist;displ=displ+30) {
			n=displ*sin(angle);n=n*(-1.0);e=displ*cos(angle);e=e*(-1.0);
			n=sn+n;e=se+e;
			Record(n,e,zero,rdcode,(int)2);		
		} /* end of for displ */
		Record(tn,te,one,rdcode,(int)2);				
	}

}
