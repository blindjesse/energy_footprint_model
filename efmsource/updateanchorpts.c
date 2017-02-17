/* UpdateAnchorPts() - Process and store road-anchor points generated for the current yr.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include "geo.h"
#include "efm.h"
#include "roads.h"

extern	int		row,col;
extern	float	grain;

void	UpdateAnchorPts()
{
	int			ret,index,i,j,id,k,code;
	char		value1[255],value2[255];
	RDSTORE		*Rptr;
	long long	indexl;
	void		CheckCore(int,int),CheckCoreI(long long);



	if(newanchors<=0){
		/* to satisfy processing in REINIT, we'll create a dummy ROADANCptr even though no road anchors were created */
		ROADANCptr[loop+1].aptr=(RDSTORE *) calloc(1,sizeof(RDSTORE));
		fclose(anchor);
		return;
	}


	/* read the new anchor pts and filter out duplicates */
	Newptr=(ROADSTOREN *) calloc(newanchors+1,sizeof(ROADSTOREN));
	if(Newptr==NULL){printf("ERROR, Newptr==NULL\n");exit(-1);}
	fclose(anchor);



	anchor=fopen("tanc","r");
	j=0;
	for(i=1;i<=newanchors;i++) {
		fscanf(anchor,"%d %s %s %d\n",&id,value1,value2,&code);
		if(rdmaster[id]==0){	/* new anchor pt */
			if(rdindex[id]==0) {
				j++;
				rdindex[id]=j;
				Newptr[j].code=code;
				Newptr[j].utmn=atof(value1);
				Newptr[j].utme=atof(value2);
				Newptr[j].id=(long long) id;
			}else {
				if(code==1) {
					/* replace entry */
					k=rdindex[id];
					Newptr[k].code=code;
					Newptr[k].utmn=atof(value1);
					Newptr[k].utme=atof(value2);
				}
			} /* rdindex */
		} /* rdmaster */
	} /* for i */
	fclose(anchor);


	/* Now set ROADANCptr[t].aptr, where t is the duration loop + 1.  +1 because
	   ROADANCptr[1] is the initial, baseline road vertices */
	ROADANCptr[loop+1].aptr=(RDSTORE *) calloc(j+1,sizeof(RDSTORE));
	Rptr=ROADANCptr[loop+1].aptr;ROADANCptr[loop+1].entries=j;
	if(Rptr==NULL) {printf("ERROR, RDptr in UpdateAnchorPts ==NULL\n");exit(-1);}

	for(i=1;i<=j;i++) {
		if(rdindex[Newptr[i].id]!=i) {  /* checks to ensure that the rdindex accession number 
										matches the accession value in ROADANCptr[].aptr */
			printf("ERROR, misalignment in UpdateAnchorPt\n");exit(-1);
		}
		rdmaster[Newptr[i].id]=loop+1;Rptr[i].utmn=Newptr[i].utmn;Rptr[i].utme=Newptr[i].utme;
		CheckCoreI(Newptr[i].id);   /* update disturbance amount if within a core area */
	}
	free(Newptr);
	/* new roads are set to negative road type in SetRdCells().  This eliminates them from consideration in FindNearest()
	since an anchor pt has not been set until now.  Clean up roads here */
	for(indexl=0;indexl<maxcnt;indexl++) {
		if(rds[indexl]!=-9999) {
			if(rds[indexl]<0)rds[indexl]=(-1)*rds[indexl];
		}
	}
}

/* check to see if road is in core area.
If so, then first determine if pad is overlapping.  If so, don't double count the road.
If not, then count the road */
void CheckCore(int r, int c)  /* this version requires r & c */
{
	int		id;
	long long	indexl;

	indexl=(long long)r * (long long)col;indexl+=(long long)c;
	if(SGcore[indexl]>0) {
		if(pads[indexl]>0)return;
		id=SGcore[indexl];
		CAptr[id].disturb+=(grain * rdwidth[10])/10000.0;
		CAptr[id].prop=CAptr[id].disturb/CAptr[id].ha;
	}
}
void CheckCoreI(long long indexl)  /* this version requires row,col index */
{
	int		id;

	if(SGcore[indexl]>0) {
		if(pads[indexl]>0)return;
		id=SGcore[indexl];
		CAptr[id].disturb+=(grain * rdwidth[10])/10000.0;
		CAptr[id].prop=CAptr[id].disturb/CAptr[id].ha;
	}
}