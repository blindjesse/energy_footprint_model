/* ReInit() - Reinitialize global structures and variables before the next Monte Carlo replication.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include "efm.h"
#include "geo.h"
#include "roads.h"
#include "padalloc.h"
#include "fnames.h"
#include "plss.h"
#include "rdend.h"
#include "rdgrid.h"

extern	FILE	*themap;
extern float	jdum;

/* todo:  coreareacalcs,etc.  as these are turned on, need to check/update code to
accommodate multiple reps */

void	ReInit()
{
	PADINFO		*ptr,*oldptr;

	void		pause();
	int			i,j,k,id;
	long long	index;
	void		ReadRoadsB(int);
	void		ReadRdIDB(char *);
	void		ReadPadsB();
	void		SDCoreArea();
	void		ReadFillR(char *);
	char		tr;
	float		Ran22( float *);
	PADINFO		*ptr1,*ptr2;
	void		DoG();
	void		RdGrid(),PadRdOverlap();
	


	/* INIT TFILL */
	TFILL[0]=TFILL[1]=TFILL[2]=TFILL[3]=TFILL[4]=0;


	/* get rid of new pads - the end of the initial list is THEENDPTR */

	ptr=THEENDPTR->next_ptr; /* if(ptr!=NULL)printf("%d %d %d\n",ptr,ptr->next_ptr,ptr->id); */
	while(ptr!=NULL) {
		oldptr=ptr->next_ptr;
		free(ptr->Nwellptr);
		free(ptr);
		ptr=oldptr;
		/* if(ptr !=NULL)printf("%d %d\n",ptr,ptr->id); */
	}
	THEENDPTR->next_ptr=NULL;   /* specifically set */
	ENDPADIptr=THEENDPTR;		/* reset end pointer */


	/* Update pad info with padbase */
	ptr1=PADIptrBASE; ptr2=PADIptr;
	while(ptr1!=NULL) {
		ptr2->au=ptr1->au;ptr2->proj=ptr1->proj;
		ptr2->active=ptr1->active;ptr2->activedyn=ptr1->active;
		ptr2->reveg=ptr1->reveg;ptr2->end=ptr1->end;
		ptr2->duration=ptr1->duration;
		ptr2->yrssince=ptr1->yrssince;
		ptr2->nwells=ptr1->nwells;ptr2->activewells=ptr1->activewells;ptr2->inactivewells=ptr1->inactivewells;
		ptr2->change=ptr1->change;  /* this can be set to a 2 to stimulate checking of deactivated roads in CheckRdActive */
		for(j=1;j<=ptr1->nwells;j++) {
			ptr2->Nwellptr[j].yearbeg=ptr1->Nwellptr[j].yearbeg;
			ptr2->Nwellptr[j].yearend=ptr1->Nwellptr[j].yearend;
			ptr2->Nwellptr[j].status=ptr1->Nwellptr[j].status;
			ptr2->Nwellptr[j].bottom=ptr1->Nwellptr[j].bottom;
		}
		if(ptr1->id!=ptr2->id) {printf("ERROR, padinfo not matching mirror version\n");exit(-1);}
		ptr1=ptr1->next_ptr;ptr2=ptr2->next_ptr;
	}




	/* free, read roads */
	newline=0;	/* accession of new roads (lines) - see DrawSLine() */
	free(rds);maxcnt=0;themap=fopen(theroads,"rb");
	ReadRoadsB( (int)0);

	/* Also have to reinit rdsid */
	free(rdsid);ReadRdIDB(theroadsid);


	/* There is a bunch of road-related structs to deal with. */
	/* this is the initial init -> ROADANCptr = (ROADANCPTR *) calloc(duration+1,sizeof(ROADANCPTR)); */
	/* THis is the initial aptr init ->	ROADANCptr[1].aptr=(RDSTORE *) calloc(icnt+1,sizeof(RDSTORE));  where icnt is the number of vertices */
	/* ROADANCptr[1] is the initial vertices - save these.  Go thru this struct and deallocate aptr 
	   for i=2;i<=duration;, and reset number of entries.  */
	for(i=2;i<=duration+1;i++) {  /* this only works if roads are created every year.  ELSE, some ROADANCptr.aptr will not exist!! 
								SO, we create this pointer (size=1) in UpdateRoadAnchor Even for a year when no anchor pts
								were created.  */
		free(ROADANCptr[i].aptr);ROADANCptr[i].entries=0;
	}
	/* rdmaster contains the sequential order of the ROADANCptr ptr for the row,col.  rdindex[row,col] contains the
	   location of the vertices within the ROADANCptr[].aptr list.  Where rdmaster !=1, zero it out.  Where rdmaster !=1, zero
	   out rdindex */
	for(index=0;index<maxcnt;index++) {
		if(rdmaster[index]!=1) {
			rdmaster[index]=0;rdindex[index]=0;
		}
	}
   /* some checks */
	printf("No. of anchor pts = %d\n",ROADANCptr[1].entries);


	/* free, read pads grid */
	/* the active, inactive pad vars are wierd.  padinactive is used to set a struct that is used in output3.  Don't mess with
	padinactive without extracting the struct creation in readpadinfo.c */

	free(pads);padnum=0;padreveg=0;padactive=INITIALACTIVE;	padinactive=INITIALINACTIVE;dynamactive=padactive;dynaminactive=padinactive;
	themap=fopen(thepads,"rb");
	ReadPadsB();
	/* Now that we have reset padnum, init the activedyn field */
	ptr=PADIptr;
	for(i=1;i<=padnum;i++) {
		if(ptr->id!=i) { printf("Pad ID error %d %d\n",i,ptr->id);exit(-1); }
		ptr->activedyn=ptr->active;
		ptr=ptr->next_ptr;
	}



	/* free sagegrouse core area calcs, and re-calc */

	free(CAptr);SDCoreArea();
	/* just a reminder cause don't think this value is dynamically changed */
	coredisturbproportion=0.05;  /* 0.05 */

	


	for(i=1;i<=maxsec;i++) {
		SECPTSptr[i].counter=SECPTSptr[i].cnt;  /* reset no. of pts */
		SECPTSptr[i].spacelimit=0;
		SECPptr[i].counter=SECPptr[i].num;  /* original no. of pads in a section. To date, we dont mod this
											dynamically?? */
		for(k=1;k<=SECPTSptr[i].cnt;k++) {
			SECPTSptr[i].avail[k]=0;SECPTSptr[i].space[k]=0;
			SECPTSptr[i].nb[k]=SECPTSptr[i].n[k];SECPTSptr[i].eb[k]=SECPTSptr[i].e[k];
		}
	}


	for(i=1;i<=maxsec;i++) {
		if(SECPTSptr[i].cnt>0) {
			id=0;
			while(id<=0 || id> SECPTSptr[i].cnt) {
				id=(Ran22(&jdum)* (float)(SECPTSptr[i].cnt+1) );
			}
			id--;
			for(k=1;k<=SECPTSptr[i].cnt;k++) {
				id++;
				if(id>SECPTSptr[i].cnt)id=1;
				SECPTSptr[i].n[k]=SECPTSptr[i].nb[id];
				SECPTSptr[i].e[k]=SECPTSptr[i].eb[id];
				if(SECPTSptr[i].n[k]<=0 || SECPTSptr[i].e[k]<=0){
					printf("ERROR, invalid coords in shuffle  %d %d %d %f %f\n",i,k,id,SECPTSptr[i].n[k],SECPTSptr[i].e[k]);
					printf(" %d \n",SECPTSptr[i].cnt);
					printf("%f %f\n",SECPTSptr[i].nb[1],SECPTSptr[i].eb[1]);
					printf("%f %f\n",SECPTSptr[i].nb[2],SECPTSptr[i].eb[2]);
				} 
			}
		} /* if >0 */
	}



	for(i=1;i<=maxsec;i++) {
		SECGptr[i].priority=0;  /* reset the priority field */
		SECptr[i].tpads=SECBASEptr[i].tpads;SECptr[i].ipads=SECBASEptr[i].ipads;SECptr[i].apads=SECBASEptr[i].apads;
		SECptr[i].twells=SECBASEptr[i].twells;SECptr[i].iwells=SECBASEptr[i].iwells;SECptr[i].awells=SECBASEptr[i].awells;
		SECptr[i].padarea=SECBASEptr[i].padarea;SECptr[i].surface=SECBASEptr[i].surface;
		for(j=1;j<=SECptr[i].combos;j++) {
			SECptr[i].SEptr[j].tpads=SECBASEptr[i].SEptr[j].tpads;SECptr[i].SEptr[j].apads=SECBASEptr[i].SEptr[j].apads;
			SECptr[i].SEptr[j].ipads=SECBASEptr[i].SEptr[j].ipads;SECptr[i].SEptr[j].twells=SECBASEptr[i].SEptr[j].twells;
			SECptr[i].SEptr[j].awells=SECBASEptr[i].SEptr[j].awells;SECptr[i].SEptr[j].iwells=SECBASEptr[i].SEptr[j].iwells;
			SECptr[i].SEptr[j].padarea=SECBASEptr[i].SEptr[j].padarea;
			SECptr[i].SEptr[j].BHC=SECBASEptr[i].SEptr[j].BHC;SECptr[i].SEptr[j].BHA=SECBASEptr[i].SEptr[j].BHA;
		}
	}


	/* *************** reset max wells in FILptr */
	for(i=1;i<=AUSptr->auproj;i++) {
		FILptr[i].maxwells=FILptr[i].setmaxwells;
		FILptr[i].BHOLE=FILptr[i].BHOLEsave;
		if(FILptr[i].combofirst==2)FILptr[i].combofirst=1;
		for(j=2;j<=FILptr[i].num;j++) {
			FILptr[i].next_ptr[j-1].maxwells=FILptr[i].next_ptr[j-1].setmaxwells;
		}
	}


	/* ******************* clear the spec-use storage */
	for(i=1;i<=MAXSPECS;i++) SPECRECORDptr[i].cnt=0;



	DoG(); /* sets GStore & GPStore to initial values and output in Perform */

	maxrdend=MAXRDEND;  /* reinit to max no. of incoming rd ids */

	/* reinit the 10-m road/node grid and structs; requires padnum & maxrdend to be init first.
	Re-read rdverts.in in RdGrid() */
	free(grid10);
	free(RDNptr);  /* this is set in init so it will not be NULL */
	free(PADRptr); /* this is set in init so it will not be NULL */
	RdGrid();PadRdOverlap();
}
