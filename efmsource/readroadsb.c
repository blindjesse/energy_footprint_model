/* ReadRoadsB() - Read/store the binary version of road types, for the 30-m gridded initial roads */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include "efm.h"
#include "geo.h"
#include "binary.h"


extern	FILE	*themap,*outmap;
extern	int		row,col;
extern	float	grain;
extern	float	utme,utmn;

void	ReadRoadsB(int code)
{

	char		valuec[255],st[255];
	int			total;
	long long	indexl,index2,counter;
	FILE		*fnam;

	




	HEptr=(HEAD *) calloc(1,sizeof(HEAD));
	fread(HEptr,sizeof(HEAD),1,themap);
	row=HEptr->row;col=HEptr->col;
	grain=HEptr->grain;utmn=HEptr->ULN;utme=HEptr->ULE;

	total=0;
	maxcnt=(long long)row * (long long)col;
	rds=(int *) calloc(maxcnt,sizeof(int));
	if(rds==NULL) {printf("ERROR, rds==NULL\n");exit(-1); }


	indexl=HEptr->counter;
	SCptr=(STOREC *) calloc(indexl+1,sizeof(STOREC));
	fread(SCptr,sizeof(STOREC),indexl+1,themap);
	fclose(themap);free(HEptr);


	for(index2=1;index2<=indexl;index2++) {
		counter=(long long)SCptr[index2].row*(long long)col;
		counter+=(long long)SCptr[index2].col;
		rds[counter]=SCptr[index2].value;
		if(rds[counter]>0)total++;
		if(rds[counter]>11){
			printf("Warning, rds >11 in readroadsB %d\n",rds[counter]);
			rds[counter]=11;
		}

	}
	free(SCptr);
}
