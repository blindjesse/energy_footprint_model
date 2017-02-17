/* ReadSurfB() - Read/store the binary version of surface ownership.
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


void	ReadSurfB()
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

	total=net=0;
	for(index2=1;index2<=indexl;index2++) {
		counter=(long long)SCptr[index2].row*(long long)col;
		counter+=(long long)SCptr[index2].col;
		if(SCptr[index2].value>0)surf[counter]=(char)SCptr[index2].value;
	}
	free(SCptr);
}
