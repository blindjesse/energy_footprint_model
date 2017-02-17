/* SetRdCells() - Determines if a new road point is already stored
as an anchor point.  If not, the point is saved as an anchor point.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include "efm.h"
#include "geo.h"
#include "roads.h"

extern	int		row,col;

/* code =1 if a terminal vertice, else 0.  These are used to decide which coord to use as the anchor
when there are multiple pts per cell.  Code=1 overrides a code=0 */
void	SetRdCells(float n, float e,int code, int type)
{
	char	ConvToRC2(float,float,int *, int *),tr;
	int		r,c;
	long long	indexl;


	tr=ConvToRC2(n,e,&r,&c);
	if(!tr) {
		printf("SetrdCells %f %f %d %d\n",n,e,r,c);exit(-1);
	}
	indexl=(long long)r * (long long)col;indexl+=(long long)c;
	if(rdindex[indexl]<=0) { /* if<=0, not previously set and no entry in ROADANCptr->RDptr[] */
		rds[indexl]=(-1)*type;	/* set negative road type; this is rectified later */
		fprintf(anchor,"%20d %f %f %d\n",indexl,n,e,code);newanchors++;
		TEMPRptr[indexl].utme=e;TEMPRptr[indexl].utmn=n;  /* within-time step temp storage - makes this pt accessible in
														  FindNearest() */
	}
	/* printf(" setrds %d %d %d\n",r,c,rdindex[indexl]); */
}