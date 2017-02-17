/* ReadRdIDB() - Read/store the binary version of road IDs, for 30-m gridded initial roads.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include "efm.h"
#include "geo.h"
#include "binary.h"


extern	int		row,col;


void	ReadRdIDB(char nam[255])
{
	long long	indexl,index2,counter;
	FILE		*fnam;
	int			max;


	fnam=fopen(nam,"rb");

	rdsid=(int *) calloc(row*col,sizeof(int));
	if(rdsid==NULL) {printf("ERROR, rdsid==NULL\n");exit(-1);}

	/* read map file */
	HEptr=(HEAD *) calloc(1,sizeof(HEAD));
	fread(HEptr,sizeof(HEAD),1,fnam);


	indexl=HEptr->counter;
	SCptr=(STOREC *) calloc(indexl+1,sizeof(STOREC));
	fread(SCptr,sizeof(STOREC),indexl+1,fnam);
	fclose(fnam);free(HEptr);


	max=0;
	for(index2=1;index2<=indexl;index2++) {
		counter=(long long)SCptr[index2].row*(long long)col;
		counter+=(long long)SCptr[index2].col;
		rdsid[counter]=SCptr[index2].value;	
		if(rdsid[counter]>max)max=rdsid[counter];
	}
	free(SCptr);
	printf("Max initial road ID %d\n",max);
}
