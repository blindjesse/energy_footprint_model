/* ReadSGB() - Read/store the binary version Sage-Grouse Core areas.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include "efm.h"
#include "geo.h"
#include "binary.h"


extern	FILE	*themap;
extern	int		row,col;

 

void	ReadSGB()
{
	int			total,net;
	long long	indexl,index2,counter;



	/* read map file */
	HEptr=(HEAD *) calloc(1,sizeof(HEAD));
	fread(HEptr,sizeof(HEAD),1,themap);

	indexl=HEptr->counter;
	SCptr=(STOREC *) calloc(indexl+1,sizeof(STOREC));
	fread(SCptr,sizeof(STOREC),indexl+1,themap);
	fclose(themap);free(HEptr);

	total=net=0;MaxCore=0;
	for(index2=1;index2<=indexl;index2++) {
		counter=(long long)SCptr[index2].row*(long long)col;
		counter+=(long long)SCptr[index2].col;
		SGcore[counter]=(short)SCptr[index2].value;		/* set core ID */
		if(SCptr[index2].value>0) {
			if(SCptr[index2].value>MaxCore)MaxCore=SCptr[index2].value;
			total++;
			if(rds[counter]>0 || pads[counter]>0)net++;
		}
	}
	free(SCptr);

	/* printf("Proportion with SG core area = %f\n",(double)total/(double)(maxcnt)); */
	total-=net;

}
