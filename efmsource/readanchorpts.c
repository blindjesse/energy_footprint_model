/* ReadAnchorPts() - read and store the road anchor pts.
Called first to initialize, then called again to store info into global structs.  
*/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include "geo.h"
#include "efm.h"
#include "roads.h"

extern	FILE	*themap;
extern	int		row,col;


void	ReadAnchorPts(int code)
{
	int		ret,index,icnt;
	char	value1[255],value2[255],value[255];
	double	crap;
	RDSTORE	*Rptr;
	long long	indexl;

	if(code==1) {	/* initialize and tally no. of anchor pts */


		rdindex=(int *) calloc(maxcnt,sizeof(int));
		if(rdindex==NULL) {printf("ERROR, rdindex==NULL\n");exit(-1);}

		rdmaster=(int *) calloc(maxcnt,sizeof(int));
		if(rdmaster==NULL) {printf("ERROR, rdmaster==NULL\n");exit(-1);}

		ret=255;
		icnt=0;
		while(ret!=EOF) {
			ret=fscanf(themap,"%s %s %s\n",value,value1,value2);  /* the project cell index, UTMN, UTME */
			if(ret!=EOF) {
				crap=atof(value);indexl=(long long)crap;
				if(indexl>=maxcnt){printf("ERROR, >= invalid index in readanchorpts %d %d\n",indexl,maxcnt);exit(-1);}
				if(indexl<0){printf("ERROR, < invalid index in readanchorpts %d %d\n",indexl,maxcnt);exit(-1);}
				icnt++;		/* counts number of anchor pts */
				rdmaster[indexl]=1;rdindex[indexl]=icnt;
			}
		} /* end of while */
		fclose(themap);

		/* Initialize ROADANCptr and a RDSTORE ptr to store the incoming anchor pts; entry 1 is baseline.
		Thus, 1 + duration are needed, and to avoid using zero, do duration +2 */
		ROADANCptr = (ROADANCPTR *) calloc(duration+2,sizeof(ROADANCPTR));
		if(ROADANCptr==NULL){printf("ERROR, ROADANCptr==NULL\n");exit(-1);}

		ROADANCptr[1].aptr=(RDSTORE *) calloc(icnt+1,sizeof(RDSTORE));
		if(ROADANCptr[1].aptr==NULL) {printf("ERROR, ROADANCptr[1].aptr ==NULL\n");exit(-1);}

	} else {	/* read and store anchor pts in RDSTORE */

		Rptr=ROADANCptr[1].aptr;
		ret=255;
		icnt=0;
		while(ret!=EOF) {
			ret=fscanf(themap,"%s %s %s\n",value,value1,value2);
			if(ret!=EOF) {
				icnt++;		/* counts number of anchor pts */
				Rptr[icnt].utmn=atof(value1);Rptr[icnt].utme=atof(value2);
			}
		} /* end of while */
		fclose(themap);
		printf("No. of anchor pts = %d\n",icnt);
		ROADANCptr[1].entries=icnt;

		/* Double check to ensure that every road cell has an anchor pt */
		for(indexl=0;indexl<maxcnt;indexl++) {
			if(rds[indexl]>0) {
				if(rdmaster[indexl]!=1)printf("ERROR, rdmaster not set %d \n",indexl);
				if(rdindex[indexl]<=0) printf("ERROR, rdindex not set %d \n",indexl);
				index=rdindex[indexl];
				if(Rptr[index].utmn<=0)printf("ERROR, utmn of anchor pt not set %d\n",index);
				if(Rptr[index].utme<=0)printf("ERROR, utme of anchor pt not set %d\n",index);
			}
		}
		printf("All rd cells have anchor pts\n");	 
	}

}