/* locatefunc.c -  Collection of functions used to evaluate pad locations, and
to set global structures with a new pad once one has been determined.
Functions are primarily called by Locate().
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include "efm.h"
#include "geo.h"
#include "padpat.h"
#include "plss.h"
#include "rdgrid.h"

extern	float	grain;
extern	int		row,col;
extern	float	idum;


char	CheckPad(int padpat,float n, float e, int i, int j, int *returnid)
{
	char	Valid(int,int);
	float	Ran2(float *);
	int		r,c,ii,jj,k;
	int		modi,modj;
	char	tr;
	long long	indexl;
	int		delta,attempts;
	void	pause();
	void	OutPads();




	for(attempts=1;attempts<=10;attempts++) {

	/* Here is where we figure out the padpattern sequence to select from.  padpat comes from FILR... */
		delta=SEQptr[padpat].delta;

		tr=1; 
		while(tr) {
			tr=0;
			k=Ran2(&idum)*(delta+1);	/* randomly select a pattern */
			if(k<=0 || k>delta)tr=1;
			/* make sure the pattern actually exists.  numpat can be the max ID of a pad pattern and NOT
			be the same as the number of pad patterns, OR NOT?? */
			k=SEQptr[padpat].beg-1+k;
			if(k<SEQptr[padpat].beg || k>SEQptr[padpat].end)tr=1;
			if(tr==0) {
				if(PADPptr[k].r==0 || PADPptr[k].c==0)tr=1;
			}
		}

		ii=PADPptr[k].r;jj=PADPptr[k].c;
		modi=(float)ii * 0.5;modj=(float)jj*0.5; 


		/* see if the pad overlaps stuff it shouldn't */
		tr=1;
		for(r=0;r<ii;r++) {
			for(c=0;c<jj;c++){
				if(Valid(r+(i-modi),c+(j-modj))) {
					indexl=(long long)(i+r-modi) * (long long)col;indexl+=(long long)(c+j-modj);
					if(PADPptr[k].aptr[r*jj+c]>0) {	
						if(nontraverse[indexl]>0) tr=0;
						if(rds[indexl]>=1 && rds[indexl]<=4)tr=0;
						if(rds[indexl]==7)tr=0;
						if(pads[indexl]>0)tr=0; /* dec 2015  */
					}
				}
			}
		} /* end of for r */
		if(tr) {
			*returnid=k;return(tr);
		}
	} /* for attempts */
	tr=0;return(tr);  /* this location doesn't work after trying 10 pad shapes */
}



/* action = 1 if we are in a core area and need to determine if
this pt is within 965 meters of a lek (0.6 mile buffer requirement);
else action = 0.  In this version, we check to see if we are within 965 meters
of the boundary of an occupied lek that is within a core area - using the lekp grid. */
char CheckPts(float n, float e, int r, int c, float displ,int action,float factor, int auproj) /* dec 2015 */
{
	float		dist;
	int			i,j;
	char		tr;
	float		adjust;
	char		CheckNear(int,int, float , float,int ); /* dec 2015 */
	char		CheckNearInfra(int,int, float , float);
	float		DISTANCE(float,float,float,float);
	char		Valid(int,int);
	int			dis;


 /* check for too close to infrastructre and adjacent pads */
		adjust=1.2;
		if(TIGHT==1){tr=1;}else{tr=CheckNearInfra(r,c,(float)200.0,(float)1.0);}
		if(TIGHT==111)fprintf(checkp,"CNearI %d %d\n",auproj,(int)tr);
		if(tr) {   /* if not too close to infrastructure, check for proximity to pads */
			if(TIGHT==1){tr=1;}else{tr=CheckNear(r,c, displ, adjust,auproj);} /* dec 2015 */
			if(TIGHT==111)fprintf(checkp,"CNear %d %d\n",auproj,(int)tr);
		}
		if(tr==1 && action==1) {  /* if not too close to infrastructre and to existing pads, AND if action==1 (in core area)
				  then check to see if this pt is within 965 meters (0.6mile buffer) of a lek perimeter that
				  is within a core area.  If so, can't use this pt (tr=0). NOTE - the lek perimeters have been buffered
				  according to their location (inside, outside of core).  Thus, the 0.6mile buffer is already contained in
				  lekperim[]. So, check to see if we overlap a lek.  */

			/* for(i=1;i<=lek_n;i++) {
				dist=DISTANCE(n,e,LEKptr[i].n,LEKptr[i].e);
				if(dist<=965.0)tr=0;
			} */
			dist=sqrt(factor*10000.0);  /* assume 1ha increase per 1 unit increase in factor */
			dist=dist/grain;dist+=2.5;
			dis=(int)dist;
			for(i=r-dis;i<=r+dis;i++) {
				for(j=c-dis;j<=c+dis;j++) {
					if(Valid(i,j)) {
						if(lekperim[i*col+j]>0 && SGcore[i*col+j]>0) {tr=0;return(tr);}
					}
				}
			}

		}
		return(tr);
}


/* returns 0 if too close to existing pads.  2 options. Any pad or if TIGHT==1 then a pad of the same AU x Proj combo */
char	CheckNear(int r,int c, float displ, float adjust,int auproj) /* dec 2015 */
{
	int			dis,dis1;
	int			k,l;
	float		displcheck;
	char		Valid(int,int);
	long long	index;
	float		dist;
	void		FindPadAU(int,int *, int *);  /* dec 2015 */
	int			aut,au,proj,projt; /* dec 2015 */


	/* Pick up the AU & proj of the new pad/wells */
	au=AUSptr->aulist[auproj]; /* dec 2015 */
	proj=AUSptr->projlist[auproj];

	/* search within adjust times displ */
	displcheck=adjust*displ;
	dis=displcheck/grain;

	/* start nearby, work outwards */
	for(dis1=1;dis1<=dis;dis1++){
		k=r-dis1;  /* upper */
		for(l=c-dis1;l<=c+dis1;l++) {
			if(Valid(k,l)) {
				dist= sqrt( (float)((k-r)*(k-r)) + (float)((l-c) * (l-c)) );dist=dist*grain;  /* translate to meters */
				index=(long long)k * (long long) col;index+=(long long)l;
				if(pads[index]>0){
					if(dist<=displ) {
						if(TIGHT==0)return((char)0); /* else base spacing on AU x Proj combo */
						FindPadAU(pads[index],&aut,&projt); /* printf("aut au %d %d %d %d\n",aut,au,projt,proj); */ /* dec 2015 */
						if(aut==au && projt==proj)return((char)0);
					}
				}
			}
		}
		k=r+dis1;	/* lower */
		for(l=c-dis1;l<=c+dis1;l++) {
			if(Valid(k,l)) {
				dist= sqrt( (float)((k-r)*(k-r)) + (float)((l-c) * (l-c)) );dist=dist*grain;  /* translate to meters */
				index=(long long)k * (long long) col;index+=(long long)l;
				if(pads[index]>0){
					if(dist<=displ) {
						if(TIGHT==0)return((char)0); /* else base spacing on AU x Proj combo */
						FindPadAU(pads[index],&aut,&projt);/* dec 2015 */
						if(aut==au && projt==proj)return((char)0);
					}
				}
			}
		}
		l=c-dis1;  /* LHS */
		for(k=r-dis1;k<=r+dis1;k++) {
			if(Valid(k,l)) {
				dist= sqrt( (float)((k-r)*(k-r)) + (float)((l-c) * (l-c)) );dist=dist*grain;  /* translate to meters */
				index=(long long)k * (long long) col;index+=(long long)l;
				if(pads[index]>0){
					if(dist<=displ) {
						if(TIGHT==0)return((char)0); /* else base spacing on AU x Proj combo */
						FindPadAU(pads[index],&aut,&projt);/* dec 2015 */
						if(aut==au && projt==proj)return((char)0);
					}
				}
			}
		}
		l=c+dis1;  /* RHS */
		for(k=r-dis1;k<=r+dis1;k++) {
			if(Valid(k,l)) {
				dist= sqrt( (float)((k-r)*(k-r)) + (float)((l-c) * (l-c)) );dist=dist*grain;  /* translate to meters */
				index=(long long)k * (long long) col;index+=(long long)l;
				if(pads[index]>0){
					if(dist<=displ) {
						if(TIGHT==0)return((char)0); /* else base spacing on AU x Proj combo */
						FindPadAU(pads[index],&aut,&projt);/* dec 2015 */
						if(aut==au && projt==proj)return((char)0);
					}
				}
			}
		}
	}
	return( (char)1); /* if not returned before here, then no pads nearby */
}


/* returns 0 if too close to existing infrastructure */
char	CheckNearInfra(int r,int c, float displ, float adjust)
{
	int			dis,dis1;
	int			k,l;
	float		displcheck;
	char		Valid(int,int);
	long long	index;
	float		dist;

	/* search within adjust times displ */
	displcheck=adjust*displ;
	dis=displcheck/grain;

	/* start nearby, work outwards */
	for(dis1=1;dis1<=dis;dis1++){
		k=r-dis1;  /* upper */
		for(l=c-dis1;l<=c+dis1;l++) {
			if(Valid(k,l)) {
				dist= sqrt( (float)((k-r)*(k-r)) + (float)((l-c) * (l-c)) );dist=dist*grain;  /* translate to meters */
				index=(long long)k * (long long) col;index+=(long long)l;
				if(rds[index]==1 || rds[index]==2 || rds[index]==7){
					if(dist<=displ)return( (char)0);
				}
			}
		}
		k=r+dis1;	/* lower */
		for(l=c-dis1;l<=c+dis1;l++) {
			if(Valid(k,l)) {
				dist= sqrt( (float)((k-r)*(k-r)) + (float)((l-c) * (l-c)) );dist=dist*grain;  /* translate to meters */
				index=(long long)k * (long long) col;index+=(long long)l;
				if(rds[index]==1 || rds[index]==2 || rds[index]==7){
					if(dist<displ) {
						if(dist<=displ)return( (char)0);
					}
				}
			}
		}
		l=c-dis1;  /* LHS */
		for(k=r-dis1;k<=r+dis1;k++) {
			if(Valid(k,l)) {
				dist= sqrt( (float)((k-r)*(k-r)) + (float)((l-c) * (l-c)) );dist=dist*grain;  /* translate to meters */
				index=(long long)k * (long long) col;index+=(long long)l;
				if(rds[index]==1 || rds[index]==2 || rds[index]==7){
					if(dist<displ) {
						if(dist<=displ)return( (char)0);
					}
				}
			}
		}
		l=c+dis1;  /* RHS */
		for(k=r-dis1;k<=r+dis1;k++) {
			if(Valid(k,l)) {
				dist= sqrt( (float)((k-r)*(k-r)) + (float)((l-c) * (l-c)) );dist=dist*grain;  /* translate to meters */
				index=(long long)k * (long long) col;index+=(long long)l;
				if(rds[index]==1 || rds[index]==2 || rds[index]==7){
					if(dist<displ) {
						if(dist<=displ)return( (char)0);
					}
				}
			}
		}
	}
	return( (char)1); /* if not returned before here, then no rds nearby */
}

/* place i,j in center of pad object, then create a new pad */
/* returns roadoverlap=0 if the pad doesn't overlap a road; else 1 to indciate pad overlaps a road so there is no
need to create a road.

*therow, *thecol... redundant but n and e of the pad is returned. 
returnid is the pattern ID derived in CheckPad; id is the padid */
int	SetPad(int i,int j,int id, int returnid,float n, float e, int wells,float acre, int auproj,int sid,int horiz,int *therow,int *thecol,int *theid, float *then, float *thee)
{
	char	Valid(int,int);
	int		r,c,ii,jj,k,index,proj;
	int		modi,modj;
	float	size;
	char	tr;
	PADINFO	*ptr;
	int		code;
	int		saver,savec;		/* records r & c of pad for storage */
	void	ConvToPtsF(int , int , float *, float *);
	long long	indexl;
	int		roadoverlap;
	void	pause();
	void	OutPads();
	void	SetGV(float,float,int,int);
	int		iused;
	float	used;
	int		FindActualProj(int,int);
	int		rdid;



	
	k=returnid;
	saver=savec=0;
	size=0;


	/* n and east are passed - use passed r and c (i & j )  */

	ii=PADPptr[k].r;jj=PADPptr[k].c;
	modi=(float)ii * 0.5;modj=(float)jj*0.5; 


	/* create the pad grid */
	roadoverlap=0;
	for(r=0;r<ii;r++) {
		for(c=0;c<jj;c++){
			if(Valid(r+(i-modi),c+(j-modj))) {
				indexl=(long long)(i+r-modi) * (long long)col;indexl+=(long long)(c+j-modj);
				if(PADPptr[k].aptr[r*jj+c]>0) {	
					pads[indexl]=id;size++;
					
					if(rds[indexl]>0)roadoverlap=1;  /* pad overlaps a road - so no need to create a road */
					
					saver=i+(r-modi);
					savec=c+(j-modj);


					/* update core area disturbance */
					if(SGcore[indexl]>0) {
						code=SGcore[indexl];
						CAptr[code].disturb+=ha;CAptr[code].prop=CAptr[code].disturb/CAptr[code].ha;
					}

				}
			}
		}
	}
	size=size*ha;



	ENDPADIptr->next_ptr=(PADINFO *) calloc(1,sizeof(PADINFO));
	ENDPADIptr=ENDPADIptr->next_ptr;
	if(ENDPADIptr==NULL){printf("ERROR, ENDPADIPTR==NULL in PadSize\n");exit(-1);}
	ptr=ENDPADIptr;
	ptr->Nwellptr=(NWELLS *) calloc(MAXPADWELLS+1,sizeof(NWELLS));
	ptr->id=id;ptr->begin=time;ptr->ha=size;ptr->active=1;ptr->reveg=0;ptr->activedyn=1;
	ptr->nwells=wells;  /* here we set actual no. of wells */
	ptr->activewells=wells;		/* no. of active wells */
	ptr->patID=k;
	ptr->centroidr=i;ptr->centroidc=j;		/* do we really need to store these? */
	
	/* for combos, projlist[auproj] really doesn't tell us anything - this is not the
	
	actual project of this section */
	ptr->au=AUSptr->aulist[auproj];ptr->proj=AUSptr->projlist[auproj];
	ptr->SID2=sid;
	ptr->change=1;

	if(wells>MAXPADWELLS){printf("ERROR, nwells >MAXPADEWLLS in SetPad\n");exit(-1);}
	for(index=1;index<=wells;index++) {
		ptr->Nwellptr[index].status=1;ptr->Nwellptr[index].yearbeg=time;
		ptr->Nwellptr[index].bottom=acre;ptr->Nwellptr[index].au=AUSptr->aulist[auproj];
		ptr->Nwellptr[index].proj=AUSptr->projlist[auproj];  /* for combos, this is not the original project; instead it is the new combo project.  Is this correct? */
	}

	/* Output centroid of newly established pad. patbnd.cor contains displacement info to move
	grid-based centroid so that pts, lines, polygon best matches the grid-based version of a pad.
	This translation occurs in createpad.exe. The logic for this translation depends on using the
	above approach for deriving the centroid of the grid-based pad. */
	/* ConvToPtsF(i,j,&n, &e);	*/				/* convert centroid to utms,record, and store in struct  */
	fprintf(padpatID,"%d %d %d %f %f %d\n",id,k,time,n,e,wells);	/* pad ID, pattern ID,time, n and e of the centroid  */
	/* fprintf(cpads,"%d %d %d %f %f %d\n",id,k,time,n,e,wells); */	/* pad ID, pattern ID,time, n and e of the centroid  */

	/* fprintf(centroidrecord,"%d %d %f %f\n",time,id,n,e);  */  /* REMOVED THIS- record centroids as they happen */

	ptr->utme=e;ptr->utmn=n;

	/* MOD - return, r,c,n,e, and pattern id */
	*therow=i;*thecol=j;*theid=k;*then=n;*thee=e;

	/* Use the centroid to assign fedmin & ownership to this pad */
	indexl=( (long long)i * (long long) col);indexl+=(long long)j;

	ptr->fmin= fedmin[indexl];
	ptr->owner=surf[indexl];

	GStore[auproj].wells+=wells;   /* Global tally of no. of established wells each year by auxproj combo */
	GStore[auproj].pads++;

	/* Global tally of no. of established wells and pads each yr by actual project */
	/* First find actual proj */
	proj=FindActualProj(auproj,sid);  /* Function will die if a valid proj is not found */
	GPStore[proj].wells+=wells;GPStore[proj].pads++;
	/* We need to store acres consumed 'cause with the combinations across projects into an au we
	can't separate BHA among project areas FOR these combos. GStore[] above stores by au which is AOK,
	but project-level summaries need special processing. */
	GPStore[proj].area+=((float)wells*acre);
	

	
	/* THis is a new pad in the section, so update the section-pad struct */
	if(SECPptr[sid].counter+1>MAXSECPAD) {printf("ERROR, too many pads in a section\n");exit(-1);}
	SECPptr[sid].counter++;
	SECPptr[sid].padid[SECPptr[sid].counter]=ptr->id;

	/* update secgrid cells unless this is a horizontal well */
	if(horiz==0) {
		used=(((float)wells*acre)/2.47)/0.2601;  /* no. of 51 x 51 m cells to set */
		iused=used;
	/*	SetGV(n,e,sid,iused);  */  /* save time and don't do this until we're ready to use this info */
	}

	return(roadoverlap);
	/* printf("Pid & codes %d %d %d %d \n",id,(int)ptr->fmin,(int)ptr->owner,(int)ptr->area);  */
}


void	FindPadAU(int pid,int *aut,int *projt)  /* dec 2015 */
{
	PADINFO		*ptr;

	ptr=PADIptr;
	while(ptr!=NULL) {
		if(ptr->id==pid) {
			*aut=ptr->au;*projt=ptr->proj;
			return;
		}
		ptr=ptr->next_ptr;
	}
	printf("WARNING - cound't find the AU of the pid\n");
	*aut=-1;*projt=-1;
}