/* Trig() - Performs the trigonometry behind delineating a road path from an existing road to
a new pad.  Derives the path angle between 2 cells. 
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>



void	Trig(float sn,float se, float tn,float te,int *thequad, float *theangle,float *thecompass, float *dist)  /* s means source, t means target */
{

	double		deltan,deltae,angle,distance,totangle;
	int			quad;
	double		rads;


	rads=3.14159/180.0;

	distance= sqrt ( (sn-tn) * (sn-tn) + (se-te) * (se-te) );
	deltan=fabs(sn-tn);deltae=fabs(se-te);

	/* angle is the angle from the source to the target relative to the cross hairs (draw a cross and call it cross hairs).
	quad 1 angle is relative to 90 deg, quad 2 is relative to 180, quad3 is relative to 270, and quad 4
	is relative to 360.  The compass bearing (with True north =0/360) is totangle. 
	totangle is the 360-degree (polar?) angle where 0 and 360 are true north.
	angle is adjusted depending on the quad to derive totangle. */

	quad=-1;

	/* quad 1  - up to the right */
	if(tn>=sn && te>=se) {
		quad=1;
		if(deltan==0 ||deltae==0) {
			angle=0;
		} else {
			angle=deltan/deltae;  /* delta north /delta east */
		}
		angle=atan(angle);	angle=angle*(1.0/rads);totangle=90-angle;

	} else if(tn<=sn && te>se) {		/* quad 2 - down to the right*/
		quad=2;
		if(deltan==0 ||deltae==0) {
			angle=0;
		} else {
			angle=deltae/deltan;  /* delta east /delta north */
		}
		/* if you go with deltan/deltae you'd get the angle between 90 and 180 */
		angle=atan(angle); angle=angle*(1.0/rads); totangle= 180 - angle;


	} else if(tn<=sn && te<=se) {	/* quad 3  - down to the left */
		quad=3;
		if(deltan==0 ||deltae==0) {
			angle=0;
		} else {
			angle=deltan/deltae;  /* delta east /delta north */
		}
		angle=atan(angle);angle=angle*(1.0/rads); totangle= 270 - angle;



	} else if(tn>=sn && te<se) {		/* quad 4 - up and to the left */
		quad=4;
		if(deltan==0 ||deltae==0) {
			angle=0;
		} else {
			angle=deltae/deltan;  /* delta east /delta north */
		}
		angle=atan(angle);angle=angle*(1.0/rads); totangle= 360 - angle;
	}

	/* for simplicity, use brutt force to figure out totangle when n=n or e=e */
	if(sn==tn) {
		if(se>te) {
			angle=270;totangle=270;quad=0;
		}else {
			angle=90;totangle=90;quad=0;
		}
	}
	if(se==te) {
		if(sn>tn) {
			angle=180;totangle=180;quad=0;
		}else {
			angle=0;totangle=0;quad=0;
		}
	}

	if(sn==tn && se == te) {  /* exact same pt. do something with this */
	}

	if(quad==-1) {printf("ERROR, quad error\n");exit(-1);}
	/* printf("quad %d, distance %f, angle %f polar angle %f\n",quad,distance,angle,totangle); */
	*thequad=quad;*theangle=angle;*thecompass=totangle;*dist=distance;
}