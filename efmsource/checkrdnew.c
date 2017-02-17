/* CheckRdNew() - Similar to CheckRd() but uses the road coordinates stored in STORERDptr[]. 
*/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include "efm.h"
#include "geo.h"
#include "roads.h"

extern	float	grain;

/* n and e are coords of the proposed pad */
char CheckRdNew(float n, float e,int auproj, int sid,int ptk)
{

	char	ConvToRC2(float,float, int *, int *),piss;
	char	DrawSLinePCheck(float, float , float , float,int *, int *);  
	int		r,c;
	float	tn,te,dist,maxdist;
	char	tr;
	int		rr,cc,ii,jj;
	long long	indexl;
	float	DISTANCE(float,float,float,float);
	int		i;

	FORCEDRDflag=0;  /* initialize */
	OLDRDptr[1].no=0;  /* Initialize every entry into this routine */
	maxdist=999999.0;
	piss=ConvToRC2(n,e,&r,&c);  /* GRTS pt coords */
	if(!piss) {
		printf("in checkrdnew %f %f %d %d\n",n,e,r,c);exit(-1);
	}
	
	for(i=1;i<=STORERDptr[1].no;i++) {
		if(STORERDptr[i].list[ptk]==0) {  /* if first time thru then=0; If tried it before and this road pt doesn't work
										  for this GRTS pt then =1.  BUT, we reset this struct while looping thru a section
										  if a road was created. SO, we may speed things up when we cycle thru the
										  GRTS pts and most do not work, but we had some pad overlap
										  or core-area issues which resets SECTIONRD to 0.  In such case, so we will loop
										  thru the pts again for the next set of specs. */
			rr=STORERDptr[i].r;cc=STORERDptr[i].c;  /* coords of a road */
			tn=STORERDptr[i].n;te=STORERDptr[i].e;
			tr=DrawSLinePCheck(n, e, tn, te,&ii,&jj);   /* ii and jj is where we hit nontraverse */
			if(tr) {
				dist=DISTANCE((float)r,(float)c,(float)rr,(float)cc);dist=dist*grain;
				if(dist<maxdist) {
					maxdist=dist;
					FORCEDRDr=rr; FORCEDRDc=cc;  FORCEDRDflag=1;
				}
			}else {
				STORERDptr[i].list[ptk]=1;  /* this road doesn't work for the point indexed as ptk */
			}
		} /* if ptk !=0 */
	}
	if(FORCEDRDflag==1)return(FORCEDRDflag);
	tr=0;return(tr);
}