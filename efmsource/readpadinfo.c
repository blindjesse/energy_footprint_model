/* ReadPadInfo() - Read/store information about each existing pad.
Information consists of the ID of the pad, the year the pad was created, the year the pad
was deactivated (if necessary), the no. of wells on the pad, the no. of
wells that are active and inactive, surface ownership type, and the coded AU and project area,
*/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include "efm.h"
#include "geo.h"


extern	FILE	*themap;


void	ReadPadInfo(char thefile[255])
{
	PADINFO		*ptr;

	int		ret,nwells,i,rr,cc;
	int		active,id,mid,begin,end;
	char	valuen[255],valuee[255],value1[255],value2[255],value3[255],value4[255];
	int		sid,activewells,inactivewells,fmin,owner,au,proj;
	int		ta,ti;
	void	ReadPadInfo2(char *);
	char	ConvToRC2(float , float , int *, int *);

	themap=fopen(thefile,"r");


	ret=255;ta=0;ti=0;

	while(ret!=EOF) {  /* see fixpawell.exe for definition of input */
		ret=fscanf(themap,"%d %d %d %s %s %d %d %d %d %d %d %d %d %d %d %s %s\n",&mid,&id,&sid,value1,value2,&begin,&end,&nwells,&active,&activewells,&inactivewells,&fmin,&owner,&au,&proj,valuen,valuee);
		if(ret!=EOF) {
		
			if(PADIptr==NULL) {
				PADIptr=(PADINFO *) calloc(1,sizeof(PADINFO));
				ENDPADIptr=PADIptr;
				ENDPADIptr->Nwellptr=(NWELLS *) calloc(MAXPADWELLS+1,sizeof(NWELLS));
			}else {
				ENDPADIptr->next_ptr=(PADINFO *) calloc(1,sizeof(PADINFO));
				ENDPADIptr=ENDPADIptr->next_ptr;
				ENDPADIptr->Nwellptr=(NWELLS *) calloc(MAXPADWELLS+1,sizeof(NWELLS));
			}
			if(ENDPADIptr==NULL) {printf("ERROR, ENDPADIptr==NULL\n");exit(-1);}
			ptr=ENDPADIptr;

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

			/* we need to use the centroid r c so set here */
			if( ConvToRC2(ptr->utmn, ptr->utme, &rr, &cc) ) {
				ptr->centroidr=rr;ptr->centroidc=cc;
			}else {
				printf("ERROR in conversion in readpadinfo\n");exit(-1);
			}

			/* set end to 0 if pad is still active */
			if(ptr->active==1)ptr->end=0;



			if(active==1){
				padactive++;dynamactive++;
			}else {
				padinactive++;dynaminactive++;
			}
			/* clean up */
			if(ptr->ha==0)ptr->ha=1;
			ta+=activewells;

			if(nwells>MAXPADWELLS){printf("ERROR, too many wells in readpadinfo.c \n");exit(-1);}
			/* read well info if any */
			for(i=1;i<=nwells;i++) { 
				fscanf(themap,"%d %d %d %d %d %s %s %s %d %d %d %s\n",&mid,&id,&begin,&end,&active,value1,value2,value3,&au,&proj,&sid,value4); 
				ptr->Nwellptr[i].yearbeg=begin;ptr->Nwellptr[i].yearend=end; ptr->Nwellptr[i].au=au;ptr->Nwellptr[i].proj=proj;
				ptr->Nwellptr[i].status=active;ptr->Nwellptr[i].bottom=atof(value4);
				ptr->Nwellptr[i].setend=end;

				if(active==1)ti++;
			}




		} /* if ret */
	}/* end of while */
	printf("No. of active, inactive pads %d %d \n",padactive,padinactive);

	if(ENDPADIptr->id !=padnum){printf("ERROR, last PID and padnum !=\n");exit(-1);}

	fclose(themap);

	/* save the endpointer - the is the last valid entry for the initial pad data.  Used in ReInit() */
	THEENDPTR=ENDPADIptr;
	INITIALACTIVE = padactive;
	INITIALINACTIVE=padinactive;
	dynamactive=padactive;dynaminactive=padinactive;   /* these can change during a sim */


	/* save original info */
	ReadPadInfo2(thefile);
}