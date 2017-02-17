/* ReadDEMB() - Read/store binary version of DEM */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include "efm.h"
#include "geo.h"
#include "binary.h"


extern	int		row,col;
 
 

void	ReadDEMB(char nam[255])
{
	int		i,value;
	FILE	*themap;
	long long	indexl,index2,counter;
 


	themap=fopen(nam,"rb");
	if(themap==NULL) {printf("ERROR, %s not found\n",nam);exit(-1);}


	/* read map file */
	HEptr=(HEAD *) calloc(1,sizeof(HEAD));
	fread(HEptr,sizeof(HEAD),1,themap);


	flow=(int *) calloc(maxcnt,sizeof(int));
	if(flow==NULL) {printf("ERROR, flow==NULL\n");exit(-1); }

	indexl=HEptr->counter;
	SCptr=(STOREC *) calloc(indexl+1,sizeof(STOREC));
	fread(SCptr,sizeof(STOREC),indexl+1,themap);
	fclose(themap);free(HEptr);

	for(index2=1;index2<=indexl;index2++) {
		counter=(long long)SCptr[index2].row*(long long)col;
		counter+=(long long)SCptr[index2].col;
		flow[counter]=SCptr[index2].value;
	}
	free(SCptr);
}
