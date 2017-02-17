/* CheckRdActive() - Determines if we have an oil/gas road that should be set to inactive since
a pad became inactive.  Called in DataRecord(); thus, this  evaluation only occurs during a time-interval dump. 

Deactivation Logic:

1: When deactivating a pad, if many==0 then rdid is the only road connected to the pad.  Goto step2.

2: If RDNODE[rdid] only is connected to one other road, then count=1 (sometimes 0 cause of a diagonal connection); if there
is only 1 pad connected by rdid, then many=0; else if count>1 and many==1 DONOT deactivate.

*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include "efm.h"
#include "geo.h"
#include "plss.h"
#include "rdend.h"
#include "rdgrid.h"

extern		int	row,col;

void	CheckRdActive()
{
	PADINFO		*ptr;
	int			*store;
	long long	indexl;
	int			r1,c1,r2,c2;
	void		pause();
	int				i;
	char		cmd[255],number[255],nam[255],clear;
	FILE		*fnam,*storeit,*fin;
	int			id,action,PID,centroidr,centroidc,ret,thetime,rdid;
	char		RDRD(int);







/* newline+MAXRDEND (set in init) needs to equal maxrdend */
	if(maxrdend !=MAXRDEND+newline) {printf("ERROR maxrdend != constant+newline %d %d\n",maxrdend,MAXRDEND+newline);exit(-1);}


	store=(int *) calloc(newline+1+MAXRDEND,sizeof(int));
	store1=(STORE1 *) calloc(newline+1+MAXRDEND,sizeof(STORE1));


	/* This determines if we should deactivate any roads */
	ptr=PADIptr;
	while(ptr!=NULL) {
		if(ptr->change >=1 && ptr->activedyn==0) {  /* if a change and if the pad was deactivated */
			if(PADRptr[ptr->id].many==0) {  /* if only 1 rd */
				id=PADRptr[ptr->id].rdid;
				if(RDNptr[id].count<=1 && RDNptr[id].many==0) { /* if only 1 node and only 1 pad */
					store[id]=2;store1[id].PID=ptr->id;  /* deactivate */
					store1[id].centroidr=ptr->centroidr;store1[id].centroidc=ptr->centroidc;
				}
			}
		}
		if(ptr->change==2)ptr->change=0; /* ==2 means it was originally a deactivated pad.  We init ptr->change with a 2 so we can check on the rds.
										 But, we don't want to process these pads in DataRecord(), so first time thru we clear ptr->change. */
		ptr=ptr->next_ptr;
	}

	/* do this again BUT for active pads.  If we have a deactivated road from above that is on/near an active pad, then clear store[] */


/* we may have some old deactivated roads that have been extended - determine if we need to change previously deactivated roads to active */
	/* create the main output - used to activate, deactivate roads  - roadfo_repnumber_time*/
	strcpy(cmd,"roadfo");sprintf(nam,"%d",repnumber);strcat(cmd,nam);sprintf(nam,"%d",time);strcat(cmd,nam);
	fnam=fopen(cmd,"w");


	/* change the name of storeit */
	strcpy(nam,"if EXIST storet (del storet)");system(nam);
	strcpy(nam,"if EXIST storeit (copy storeit storet)");system(nam);

	fin=fopen("storet","r");
	if(fin!=NULL) {
		ret=255;
		storeit=fopen("storeit","w");  /* open a fresh version of storeit to store a new list of all the deactivated roads */
		while(ret!=EOF) {
			ret=fscanf(fin,"%d %d %d %d %d %d\n",&id,&action,&PID,&centroidr,&centroidc,&thetime);
			if(ret!=EOF) {
				clear=0;

				if(RDRD(PID)) {  /* active pad with a deactivated road? If tr, then set clear=1 */
					clear=1;
				}else { /* otherwise, continue with standard checks */
					clear=1;
					if(PADRptr[PID].many==0) {  /* if only 1 rd */
						rdid=PADRptr[PID].rdid;
						if(RDNptr[rdid].count<=1 && RDNptr[rdid].many==0) { /* and if only 1 node and only 1 pad */
							clear=0;  /* then it remains a deactivated road */
							fprintf(storeit,"%d %d %d %d %d %d\n",id,action,PID,centroidr,centroidc,thetime); 
						}
					}
				} /* if RDRD else */

				if(clear==1){
					fprintf(fnam,"%d 1 %d 0\n",id,PID);  /* this indicates activation  - fnam is roadfo###### */
					store[id]=0;  /* in case this was set above, clear it */
				}
			}
		}
		fclose(fin);fclose(storeit);
	}

	storeit=fopen("storeit","a");  /* append storeit with info from store[]==2 */
	for(indexl=1;indexl<newline+1+MAXRDEND;indexl++) {  /* indexl = rd ID, 0 means deactivated, store1[] is PID */
		if(store[indexl]==2){
			fprintf(fnam,"%d 0 %d %d\n",indexl,store1[indexl].PID,time);  /* this indicates deactivation */
			fprintf(storeit,"%d 0 %d %d %d %d\n",indexl,store1[indexl].PID,store1[indexl].centroidr,store1[indexl].centroidc,time);
		}
	}
	fclose(fnam);fclose(storeit);
	free(store);free(store1);
}

/* determine if we have an active pad with a deactivated road.  This may happen when we
re-use dead pads */
char	RDRD(int pid)
{
	PADINFO  *ptr;
	int		i;

	ptr=PADIptr;
	for(i=1;i<=pid-1;i++) {
		ptr=ptr->next_ptr;
	}
	if(ptr->id!=pid) {printf("ERROR in RDRD\n");exit(-1);}
	if(ptr->activedyn==1) {
		/* We have an active pad with a deactivated road; set the road to active */
		return((char)1);
	}
	return((char)0);  /* continue the standard checks */
}








