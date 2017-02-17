/* ProcessCorePad() - This implements the WY Executive order for determining surface disturbance of new developments
within sage-grouse core areas; also known as Swing the Cat.... 
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
extern	FILE	*outmap;

 
/* n & e , r anc c of the proposed pad,trlek - if true then we have lek pts to deal with */
char	ProcessCorePad(float n, float e,int r,int c, char trlek)
{
	char		*tgrid;
	int			i,j,l,m;
	char		Valid(int, int),tr;
	long long	indexl;
	int			disp;
	double		area,disturb;
	void		pause();
	char		ConvToRC2(float,float,int *,int *),piss;
	float		DISTANCE(float,float,float,float);
	float		r1,c1,r2,c2,dist;


	tr=1;
	tgrid=(char *) calloc(maxcnt,sizeof(char));
	disp=(int)(6437.376/grain);
	disp--;

	 
	tgrid[r*col+c]=5;
	area=0;
	r2=r;c2=c;
	/* create a 4 mile buffer around the pt but only include area within a core */
	for(i=r-disp;i<=r+disp;i++) {
		for(j=c-disp;j<=c+disp;j++){
			if(Valid(i,j)) {
				r1=i;c1=j;dist=DISTANCE(r1,c1,r2,c2);dist=dist*grain;
				if(dist<=6437+30) {
					indexl=(long long)i * (long long)col;indexl+=(long long)j;
					if(SGcore[indexl]>0) {
						if(sage[indexl]>=5 && sage[indexl]<=100 && tgrid[indexl]==0)area+=ha;
						if(tgrid[indexl]==0)tgrid[indexl]=1;
					}
				}
			}
		}
	}

	/* if we have lek pts to deal with */
	if(trlek) {
		for(l=1;l<=LEKSptr[1].no;l++) {
			r=LEKptr[LEKSptr[l].lpt].r;c=LEKptr[LEKSptr[l].lpt].c;tgrid[r*col+c]=2;
			r2=r;c2=c;
			for(i=r-disp;i<=r+disp;i++) {
				for(j=c-disp;j<=c+disp;j++){
					if(Valid(i,j)) {
						r1=i;c1=j;dist=DISTANCE(r1,c1,r2,c2);dist=dist*grain;
						if(dist<=6437+30) {
							indexl=(long long)i * (long long)col;indexl+=(long long)j;
							if(SGcore[indexl]>0) {
								if(sage[indexl]>=5 && sage[indexl]<=100 && tgrid[indexl]==0)area+=ha;
								if(tgrid[indexl]==0)tgrid[indexl]=1;
							}
						}
					}
				}
			}
		}
	}

	disturb=0;  /* we are assuming that pads and roads eliminated suitable SG habitat?? */
	for(indexl=0;indexl<maxcnt;indexl++) {
		if(tgrid[indexl]>0) {
			if(pads[indexl]>0) {
				disturb+=ha;
			}else if(rds[indexl]>0) {
				disturb+=((rdwidth[rds[indexl]]*grain)/10000.0);
			}
		}
	}
	if( (disturb+7.0)/area >=0.05) tr=0;

	free(tgrid);
	return(tr);

}