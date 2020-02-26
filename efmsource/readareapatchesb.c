/* ReadAreaPatchesB() - Read/store binary version of area patch IDS.
This information is used to expedite finding very long and convoluted road paths;
e.g., whenever the closest road to a new pad is many kilometers away and surrounded 
by areas where roads are prohibited (SG core areas, steep slopes, water/urban features).
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include "binary.h"
#include "geo.h"
#include "curve.h"


extern	int		row,col;


void	ReadAreaPatchesB(char nam[255])
{
	long long	indexl,index2,counter;
	FILE		*fnam;
	int			mp;

	mp=0;
	fnam=fopen(nam,"rb");
	patchid=(int *) calloc(maxcnt,sizeof(int));
	if(patchid==NULL) {printf("ERROR, patchid==NULL\n");exit(-1);}


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
			patchid[counter]=SCptr[index2].value;
			if(SCptr[index2].value>mp)mp=SCptr[index2].value;
		}
	}
	free(SCptr);

	printf("Max AreaPatchID= %d\n",mp);
}
