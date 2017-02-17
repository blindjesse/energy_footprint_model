/* DumpPads () - Outputs pad information including their wells.  This
information is stored as dumppYYYY and is used to
create vector shapefiles of simulated pads.
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include "efm.h"
#include "geo.h"
#include "plss.h"


void	DumpPads()
{

	int			i,j;
	PADINFO		*ptr;
	int			n;
	char		nam[255],number[255];

	/* output
	pad#, 	au, proj, sid2, years	yeare	active	newlls	nactivewells, lease, fedmin, maxent pob
	for each active well -> yearb,yeare, bh
	*/

	strcpy(nam,"dumpp");sprintf(number,"%d",repnumber);strcat(nam,number);sprintf(number,"%d",time);strcat(nam,number);
	dumppads=fopen(nam,"w");		/* records pad allocation by zone - used in perform() */

	ptr=PADIptr;
	while(ptr!=NULL) {
		fprintf(dumppads,"%d %d %d %d %d %d %d %d %d %d %d %f\n",ptr->id,ptr->au,ptr->proj,ptr->SID2,ptr->begin,ptr->end,ptr->activedyn,ptr->nwells,
			ptr->activewells,SECGptr[ptr->SID2].lease,SECGptr[ptr->SID2].fed1,SECGptr[ptr->SID2].prob);
		n=0;
		for(j=1;j<=ptr->nwells;j++) {
			if(ptr->Nwellptr[j].status==1) {
				fprintf(dumppads,"%d %d %f\n",ptr->Nwellptr[j].yearbeg,ptr->Nwellptr[j].yearend,ptr->Nwellptr[j].bottom);
				n++;
			}
		}
		if(n!=ptr->activewells) {printf("ERROR, n and active wells not the same in dumppads %d %d %d\n",n,ptr->activewells,ptr->id);exit(-1);}
		ptr=ptr->next_ptr;
	}
	fclose(dumppads);
}


