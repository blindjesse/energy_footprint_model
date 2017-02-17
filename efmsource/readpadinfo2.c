/* ReadPadInfo2() - Duplicates Read PadInfo but saves baseline information 
used in reinit() to re-initialize the initial pads at the beginning of a 
subsequent Monte Carlo replications. */



#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include "efm.h"
#include "geo.h"


extern	FILE	*themap;


void	ReadPadInfo2(char thefile[255])
{
	PADINFO		*ptr;

	int		ret,nwells,i;
	int		active,id,mid,begin,end;
	char	valuen[255],valuee[255],value1[255],value2[255],value3[255],value4[255];
	int		sid,activewells,inactivewells,fmin,owner,au,proj;


	themap=fopen(thefile,"r");


	ret=255;

	while(ret!=EOF) {  /* see fixpawell.exe for definition of input */
		ret=fscanf(themap,"%d %d %d %s %s %d %d %d %d %d %d %d %d %d %d %s %s\n",&mid,&id,&sid,value1,value2,&begin,&end,&nwells,&active,&activewells,&inactivewells,&fmin,&owner,&au,&proj,valuen,valuee);
		if(ret!=EOF) {
		
			if(PADIptrBASE==NULL) {
				PADIptrBASE=(PADINFO *) calloc(1,sizeof(PADINFO));
				ENDPADIptrBASE=PADIptrBASE;
				ENDPADIptrBASE->Nwellptr=(NWELLS *) calloc(MAXPADWELLS+1,sizeof(NWELLS));
			}else {
				ENDPADIptrBASE->next_ptr=(PADINFO *) calloc(1,sizeof(PADINFO));
				ENDPADIptrBASE=ENDPADIptrBASE->next_ptr;
				ENDPADIptrBASE->Nwellptr=(NWELLS *) calloc(MAXPADWELLS+1,sizeof(NWELLS));
			}
			if(ENDPADIptrBASE==NULL) {printf("ERROR, ENDPADIptrBASE==NULL\n");exit(-1);}
			ptr=ENDPADIptrBASE;

			ptr->au=au;ptr->proj=proj;ptr->fmin=fmin;ptr->owner=owner;
			ptr->id=id;ptr->begin=begin;ptr->active=active;ptr->end=end;
			ptr->ha=atof(value1);ptr->mid=mid;ptr->nwells=nwells;
			ptr->utme=atof(valuee);ptr->utmn=atof(valuen);
			ptr->activewells=activewells;ptr->SID2=sid;
			ptr->inactivewells=inactivewells;
			strcpy(ptr->type,value2);
			if(ptr->active==0)ptr->change=2;  /* init ptr->change to 2 to allow road-deactivation processing in CheckRds().
											  This 2 is cleared first time thru CheckRdActive so we don't unnecessarily process the
											  pad as having changed. */

			ptr->activedyn=active;  /* this is the dynamic field */
			ptr->duration=end-begin;ptr->yrssince=0;

			/* set end to 0 if pad is still active */
			if(ptr->active==1)ptr->end=0;

			/* clean up */
			if(ptr->ha==0)ptr->ha=1;

			if(nwells>MAXPADWELLS){printf("ERROR, too many wells in readpadinfo2.c \n");exit(-1);}
			/* read well info if any */
			for(i=1;i<=nwells;i++) { 
				fscanf(themap,"%d %d %d %d %d %s %s %s %d %d %d %s\n",&mid,&id,&begin,&end,&active,value1,value2,value3,&au,&proj,&sid,value4); 
				ptr->Nwellptr[i].yearbeg=begin;ptr->Nwellptr[i].yearend=end; ptr->Nwellptr[i].au=au;ptr->Nwellptr[i].proj=proj;
				ptr->Nwellptr[i].status=active;ptr->Nwellptr[i].bottom=atof(value4);
				ptr->Nwellptr[i].setend=end;
			}
		} /* if ret */
	}/* end of while */

	if(ENDPADIptrBASE->id !=padnum){printf("ERROR, last PID and padnum !=\n");exit(-1);}
	fclose(themap);
}