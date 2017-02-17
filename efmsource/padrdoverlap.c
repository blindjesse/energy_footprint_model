/* PadRdOverlapGrid() - Determines if a pad overlaps a road using a 10-m grain size.
Initial pads often overlap oil/gas roads.  New pads are allowed to overlap
oil/gas roads, but not other types of roads.  Primariy used to
determine overlap of an inactive pad with a road.  If there is overlap, then
the road is evaluated (elsewhere) to determine if it also should be deactivated.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include "efm.h"
#include "geo.h"
#include "roads.h"
#include "rdgrid.h"


extern		int	row,col;

void	PadRdOverlap()
{

	int			i,j,r,c,id,k,l;
	float		n,e;
	int			t1,t2,t3;

	char		ConvToRC10(double,double,int *, int *);
	void		ConvToPts(int , int , float *, float *);
	char		ConvToRC2(float,float,int *, int *);
	void		ConvToPtsF10(int , int , float *, float *);
	char		Valid(int,int);



	PADRptr= (PADRD *) calloc(padnum+1,sizeof(PADRD));

	/* if there is a pad, convert to 10-m grid and determine if there is overlap with a road */
	for(i=0;i<row;i++) {
		for(j=0;j<col;j++) {
			if(pads[i*col+j]>0) {
				id=pads[i*col+j];
				ConvToPts(i, j, &n, &e);  /* 30-m r,c to n, e conversion */
				if(ConvToRC10((double)n,(double)e,&r, &c)) {  /* convert to 10-m r c */
					if(grid10[r*col10+c]>0) {  /* =-9999 if no road, = -1 if node; else rdid */
						if(PADRptr[id].rdid==0) {
							PADRptr[id].rdid=grid10[r*col10+c];
						}else {
							if(PADRptr[id].rdid!=grid10[r*col10+c])PADRptr[id].many=1;
						}
					}
				}
			}
		}
	}
	/* go thru road grid; convert to 30-m res and determine if there is overlap with a pad */
	for(i=0;i<row10;i++) {
		for(j=0;j<col10;j++) {
			if(grid10[i*col10+j]>0) {
				ConvToPtsF10(i, j, &n,&e);
				if(ConvToRC2(n,e,&r, &c)) {  /* convert to 30-m r c */
					if(pads[r*col+c]>0) {
						id=pads[r*col+c];

						/* we interpert this processing to interpret/set no. of pads on a road */
						if(RDNptr[grid10[i*col10+j]].padid==0) {
							RDNptr[grid10[i*col10+j]].padid=id;
						}else {
							/* this road contains many pads */
							if(RDNptr[grid10[i*col10+j]].padid!=id)RDNptr[grid10[i*col10+j]].many=1;
						}

						/* here we're back to dealing with no. of roads connected to a pad */
						if(PADRptr[id].rdid==0) {
							PADRptr[id].rdid=grid10[i*col10+j];
						}else {
							if(PADRptr[id].rdid!=grid10[i*col10+j])PADRptr[id].many=1;
						}
					}
				}
			}
		}
	}

	/* go thru road grid; convert to 30-m res, see if there is a nearby pad with rdid=0.  At this point we are
	only dealing with pads that do not overlap a road */
	for(i=0;i<row10;i++) {
		for(j=0;j<col10;j++) {
			if(grid10[i*col10+j]>0) {
				ConvToPtsF10(i, j, &n,&e);
				if(ConvToRC2(n,e,&r, &c)) {  /* convert to 30-m r c */
					/* see if there is a pad nearby with rdid=0 */
                    for(k=r-1;k<=r+1;k++) {
						for(l=c-1;l<=c+1;l++) {
							if(Valid(k,l)) {  /* 30-m check */
								if(pads[k*col+l]>0) {
									id=pads[k*col+l];
									if(PADRptr[id].rdid==0) {
										PADRptr[id].rdid=grid10[i*col10+j];

										/* we interpert this processing to interpret/set no. of pads on a road */
										if(RDNptr[grid10[i*col10+j]].padid==0) {
											RDNptr[grid10[i*col10+j]].padid=id;
										}else {
											/* this road contains many pads */
											if(RDNptr[grid10[i*col10+j]].padid!=id)RDNptr[grid10[i*col10+j]].many=1;
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
}

 
/* deterministic conversion to center of a 10-m cell */
void	ConvToPtsF10(int r, int c, float *n, float *e)	 
{
	float		cordr,cordc;

	/* Actual conversion from r, c to UTM */
	cordr = 0.5;cordc=0.5;
	cordr=cordr*grain10;cordc=cordc*grain10;

	cordr = (utmnup10 - (r*grain10)) - cordr;
	cordc = (utme10 + (c*grain10))+cordc;

	*n = cordr;*e = cordc;
}