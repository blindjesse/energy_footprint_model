/* ReadPadsB() - Read/store the binary version of the gridded, initial pads.
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

/* pads need to be numbered using an accession strategy specific to this project 
- 'cause of the logic in place for how pads are handled */

void	ReadPadsB()
{
	long long	indexl,index2,counter;


	pads=(int *) calloc(maxcnt,sizeof(int));
	if(pads==NULL) {printf("ERROR, pads==NULL\n");exit(-1);}


	/* read map file */
	HEptr=(HEAD *) calloc(1,sizeof(HEAD));
	fread(HEptr,sizeof(HEAD),1,themap);

	indexl=HEptr->counter;
	SCptr=(STOREC *) calloc(indexl+1,sizeof(STOREC));
	fread(SCptr,sizeof(STOREC),indexl+1,themap);
	fclose(themap);free(HEptr);


	for(index2=1;index2<=indexl;index2++) {
		counter=(long long)SCptr[index2].row*(long long)col;
		counter+=(long long)SCptr[index2].col;
		if(SCptr[index2].value>0) {
			pads[counter]=SCptr[index2].value;
			if(SCptr[index2].value>padnum)padnum=SCptr[index2].value;
		}
	}
	free(SCptr);

	fclose(themap);
	printf("Initial no. of pads= %d\n",padnum);
}
