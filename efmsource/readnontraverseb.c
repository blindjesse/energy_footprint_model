/* ReadNonTraverseB() - Read/store the layer inidcating areas where oil/gas development is prohibited.
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

 

void	ReadNonTraverseB(char nam[255])
{
	long long	indexl,index2,counter;


	nontraverse=(int *) calloc(maxcnt,sizeof(int));
	themap=fopen(nam,"rb");

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
		nontraverse[counter]=SCptr[index2].value;		/* =1 if non traverse, else <=0 */
		if(nontraverse[counter]>1)printf("ERROR nontraverse value >1 %d\n",nontraverse[counter]);
	}
	free(SCptr);
}
