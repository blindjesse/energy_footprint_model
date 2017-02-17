/* ReadSageB() - Read/store the binary version of the %sagebrush cover map.
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


void	ReadSageB()
{
	int		total,net;
	long long	indexl,index2,counter;



	/* printf("NOTE sagebrush==0 is reset to -1\n"); */
	sage=(char *) calloc(maxcnt,sizeof(char));
	if(sage==NULL) {printf("ERROR, sage==NULL\n");exit(-1);}

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

		/* Quick fix to alter sage input.  Here we want zeros to be -1s */
		if(SCptr[index2].value==0)SCptr[index2].value=-1;
		if(SCptr[index2].value<0)SCptr[index2].value=-1;	/* 0 is valid, use this to avoid setting -9999 into a char */
		sage[counter]=(char)SCptr[index2].value;

		if(SCptr[index2].value>0) {
			total++;
			if(rds[counter]>0 || pads[counter]>0)net++;
		}
	}
	free(SCptr);

	totalsagearea = (double)total * ha;   /* use this in proportion assessment of surface disturbance in agebrush */

	total-=net;

}
