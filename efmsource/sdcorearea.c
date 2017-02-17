/* SDCoreArea() - Derive/store the amount of surface disturbance in sage-grouse core areas.  This provides general
information on the intensity of development within core areas.
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

 

void	SDCoreArea()
{


	int			i,j,id;
	char		tr;
	long long	indexl;

	FILE	*fnam;



	CAptr=(COREAREA *) calloc(MaxCore+1,sizeof(COREAREA));
	if(CAptr==NULL){printf("ERROR,CAptr==NULL in SDCoreArea\n");exit(-1); }

	for(indexl=0;indexl<maxcnt;indexl++) {
		if(SGcore[indexl]>0) {
			id=SGcore[indexl];
			if(id>MaxCore){printf("ERROR in sdcorearea ID > max\n");exit(-1);}
			CAptr[id].ha+=ha;	/* tally total size */
			CAptr[id].num++;

			/* tally surface disturbance */
			if(pads[indexl]>0) {
				CAptr[id].disturb+=ha;
			} else if(rds[indexl]>0){
				if(rds[indexl]>11){
					rds[indexl]=11;
				}
				CAptr[id].disturb+=(grain * rdwidth[rds[indexl]])/10000.0;
			}

		}
	}

	for(i=1;i<=MaxCore;i++) {
		CAptr[i].prop= CAptr[i].disturb/CAptr[i].ha;
	}

}