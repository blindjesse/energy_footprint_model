/* ReadLekGridB() - Read/store binary version of sage-grouse leks.  Data
are estimated perimeters provided by WY Game & Fish.
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

/* lek perimeter grid */

void	ReadLekGridB(char nam[255])
{
	long long	indexl,index2,counter;
	FILE		*fnam;


	fnam=fopen(nam,"rb");

	lekperim=(int *) calloc(maxcnt,sizeof(int));
	if(lekperim==NULL) {printf("ERROR, lekperim==NULL\n");exit(-1);}

	/* read map file */
	HEptr=(HEAD *) calloc(1,sizeof(HEAD));
	fread(HEptr,sizeof(HEAD),1,fnam);

	indexl=HEptr->counter;
	SCptr=(STOREC *) calloc(indexl+1,sizeof(STOREC));
	fread(SCptr,sizeof(STOREC),indexl+1,fnam);
	fclose(fnam);free(HEptr);


	for(index2=1;index2<=indexl;index2++) {
		counter=(long long)SCptr[index2].row*(long long)col;
		counter+=(long long)SCptr[index2].col;
		if(SCptr[index2].value>0) {
			lekperim[counter]=SCptr[index2].value;
		}
	}
	free(SCptr);
}
