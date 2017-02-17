/* SelectSection() - Regulates the selection of PLSS sections within an AU-Proj area for development.
First checks to see if an active pad can be used for 'new' wells.  If not, then
checks to see if there is room (bottom-hole area) available for the type of
well(s) that is being evaluated for establishment.  If the section can be developed,
Calls SelSectOpts()to determine the weighted score of the section.  Sums and
relativize weights of all processed sections.  These weights
are used in the random selection of sections.
*/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include "efm.h"
#include "geo.h"
#include "padalloc.h"
#include "binary.h"
#include "padpat.h"
#include "plss.h"

extern	float	kdum;

char SelectSections(int auproj)
{
	int		nsects,j,sid,id,k,cnt,option;
	float	area,maxscore,savemaxscore;
	void	GetSecInfo(int,int,int,float *);
	double	total,temp,grand;
	void	pause();
	char	MinHole(float,int);

	int		secscore;
	void	OrderWgts(int,double);
	char	tr;
	char	CheckDeadPads(int,int,int *, PADINFO **);
	int		thepid;
	PADINFO	*theptr;
	void	SelSecOpts(int,int,int,float *,double *);
	char	DEAD;


	grand=0;savemaxscore=0;SET=0;
	WEIptr[1].num=0; WEIptr[1].remain=0; cnt=0; /* init */
	nsects=AUSptr->nsection[auproj];  /* no. of sections in this au x proj combo; i is the au x proj accession code */
	for(j=1;j<=nsects;j++) {
		sid=AUSptr->SLIST[auproj].sid2[j]; /* i is the au x proj, j is the accession */
		if(sid==0){ printf("ERROR sid=0 in SelectSections %d %d\n",auproj,j);exit(-1);}
		GetSecInfo(sid,auproj,AUSptr->SLIST[auproj].proj[j],&area);  /* get BH info for this section, for this auproj=au x proj combo */


		tr=0;
		if(area>0) {
			tr=1;DEAD=0;
			if(SECPTSptr[sid].counter<=0)tr=0;
			if(REUSEPADS) {  /* if we're reusing pads, check to see if there is a dead pad that is not this au and if previous wells
							 on the dead pad have never belonged to this au. */
				if(CheckDeadPads(auproj,sid,&thepid,&theptr)) {  /* returns tr=1 if we have an available dead pad.  If no dead pads that
												 we can use, then don't adjust the value of tr - let the above
												 logic determine if we can use this sid.  Do nothing with thepid and theptr here...  */ 
					tr=1;
					/* we can have sufficient area with ###.counter<=0 and an available dead pad, BUT ###.cnt may be =0 cause
					we didn't generate any GRTS pts for this section.  Thus, check on ###.cnt and reset tr if necessary */
					if(SECPTSptr[sid].cnt<=0)tr=0;
					DEAD=tr;
				}
			}
		}

		if(tr)  {  /* consider this sid to be a viable option */



			if(MinHole(area,AUSptr->SLIST[auproj].proj[j])) {  /* if at least 1 spec can fit in this section - pass original project code
															   which is primarily used for combos */
				if(SECGptr[sid].prob>0.00) {  /* an attempt to only use areas with geophys prob >0.01 */


				/* I think all of the following can be moved to the top since this is duplicated for every section in this time interval; but need to evaluate. */

				/* we can weight sections using different methods.  option defines the method;
				SelSecOpts() returns the maxscore for the option and the score for this SID */
				option=WEOPT; 
				/* if option is negative, we are randomly selecting option every year - derive compliment */
				if(option<0)option=option*(-1);
				/* ********************************************************************************************************************** */


				SelSecOpts(option,sid,auproj,&maxscore,&total);

				/* if we are weighting a section heavier 'cause it is in a project area, then do the following */
				secscore=0;


				if(total<=0)total=0;
				if(total>0) {
					if( (DEAD) ){total+=10;if(total>maxscore)maxscore=total;}  /* to weight sids with usable dead pads */
					/* if( (DEAD) ){total=total*2;if(total>maxscore)maxscore=total;} */  /* to weight sids with usable dead pads */
					cnt++;
					if(maxscore>savemaxscore)savemaxscore=maxscore;
					if(cnt>MAXWEI){printf("ERROR, cnt in selectsection too large \n");exit(-1);}
					WEIptr[1].num=cnt;WEIptr[1].remain=cnt;
					WEIptr[cnt].sid=sid;
					WEIptr[cnt].weight=total;
					WEIptr[cnt].used=0;
					WEIptr[cnt].proj=AUSptr->SLIST[auproj].proj[j];
					grand+=total;
					/* fprintf(diag,"%d %f %d\n",sid,area,time); */
				}
				} /* if prob > 0.01 */
			} /* if MinHole */
		}/* if area */
	} /* for j */
	/* if cnt <=0, then we have no available sections, so return.  Since WEIptr[1].remain ==0,
	Establish() will do nothing */
	 if(TIGHT==111) {
		fprintf(checkp,"cnt %d %f %d %d\n",cnt,grand,option,WEIptr[1].remain);
	} 
	if(cnt<=0)return((char)0);

	/* create cumulative freq */
	WEIptr[1].cumu=WEIptr[1].weight/grand;
	WEIptr[1].relmax=WEIptr[1].weight/savemaxscore;if(WEIptr[1].relmax<0)WEIptr[1].relmax=0;if(WEIptr[1].relmax>1)WEIptr[1].relmax=1.0;

	for(j=2;j<=cnt;j++) {
		WEIptr[j].cumu=(WEIptr[j].weight/grand)+WEIptr[j-1].cumu;
		WEIptr[j].relmax=WEIptr[j].weight/savemaxscore;if(WEIptr[j].relmax<0)WEIptr[j].relmax=0;if(WEIptr[j].relmax>1)WEIptr[j].relmax=1.0;
	}

	/* Let's order the sections according to their weights */
     OrderWgts(cnt,grand); 

	return((char)1);
}

/* SelectSections calls this to determine if we have at least 1 available dead pad for this AU.
Establish() calls this to pickp up the THEPID of an available dead pad. */
char	CheckDeadPads(int auproj, int sid, int *thepid,PADINFO **theptr)
{
	int			i,j,pid,au;
	PADINFO		*ptr;
	char		use;

	au=AUSptr->aulist[auproj];  /* pick up the au of this auxproj */
	use=0;

	/* pick up the pad IDS in this sid */
	for(i=1;i<=SECPptr[sid].counter;i++) {
		pid=SECPptr[sid].padid[i];  /* i is accession; pid is patch id */
		/* find this pid in pad struct */
		ptr=PADIptr;
		for(j=1;j<=pid-1;j++) {  /* think we store by accession, so this should work */
			ptr=ptr->next_ptr;
		}
		if(ptr->id !=pid) {printf("ERROR in CheckDeadPads %d %d\n",pid,ptr->id);exit(-1);}
		/* we now are pointing at the correct pid.  Determine if the pad is dead */
		if(ptr->activedyn==0 && ptr->begin!=0) { /* if a dead pad and if start time is not zero (which means not sure this is an OG pad) */
			use=1;
			if(ptr->au==au){
				use=0; /* if the pad is currently labled as this au, then don't use */
			}else {	
				/* over time the pad can switch from e.g. au=1 to au=2 then back to au=1.  Check the au of the wells to
				determine if any of them have been of this au (the one coming in).  If so, then don't use. */
				for(j=1;j<=ptr->nwells;j++) {
					if(ptr->Nwellptr[j].au==au)use=0;
				}
			} /* else */
		} /* if activedyn */
		if(use==1){
			*thepid=pid; *theptr=ptr; return(use);  /* at least 1 available dead pad */
		}
	} /* for i */
	return((char)0); /* no available dead */
}


/* we assume that au even for combos is aulist[i] entry.  For
combos, proj is the original project instead of the NEW project because new
projects may be a combo of different original projects.  */

/* this gets the BHA for the section, au x proj combo */
void GetSecInfo(int sid, int i, int proj, float *area)
{
	int		combo,j;

	combo=SECptr[sid].combos;  /* no. of combos this section is in */
	for(j=1;j<=combo;j++) {
		if(SECptr[sid].SEptr[j].au==AUSptr->aulist[i] && SECptr[sid].SEptr[j].proj==proj) {  /* use proj instead of projlist to accommodate 
																							 combined FILptr specs */
			*area=SECptr[sid].SEptr[j].BHA;return;
		}
	}
	printf("ERROR, could not find au and proj in SECptr\n");
	printf("%d %d %d %d\n",i,sid,AUSptr->aulist[i],proj);
	printf("%d %d\n",SECptr[sid].SEptr[1].au,SECptr[sid].SEptr[1].proj);  /* doesn't make sense since SEptr[] is not really set */
	exit(-1);
	*area=0;
}


/* Use relmax to figure out quantiles (of the values), and re-order so that higher prob sections are first, followed
by next highest, etc.. */
void	OrderWgts(int inum,double grand)
{
	int		low,medium,high,i,j;
	int		*lowl,*mediuml,*highl;  /* lists to store index into WEIptr[] */
	WEIGHT	*ptr;	
	int		cnter,id;
	int		l,m,h;
	char	tr;
	float	Ran23(float *);
	int		spin,*save;

	spin=0;
	lowl=(int *) calloc(inum+1,sizeof(int));
	mediuml=(int *) calloc(inum+1,sizeof(int));
	highl=(int *) calloc(inum+1,sizeof(int));
	low=0;medium=0;high=0;
	l=0;m=0;h=0;

	/* determine who is high, medium, and low */
	for(i=1;i<=inum;i++) {
		if(WEIptr[i].relmax>=.66) {
			h++;high++;highl[high]=i;
		}else if(WEIptr[i].relmax>=.33) {
			m++;medium++;mediuml[medium]=i;
		}else {
			l++;low++;lowl[low]=i;
		}
	}

	ptr=(WEIGHT *) calloc(inum+1,sizeof(WEIGHT));
	save=(int *) calloc(inum+1,sizeof(int));

	cnter=0;

	/* randomly assigned an order */
	if(high>0) {
		while(h>0) {
			tr=1;
			if( (float)h/(float)high <.1 || h<=2) {  /* find the next available highl[] */
				for(j=1;j<=high;j++) {
					if(highl[j]!=0)id=j;
				}
				tr=0;
			}
			while(tr) {
				if(spin>50000)printf("Spinning in OrderWgts %d %d %d\n",h,high,cnter);
				tr=0;
				id=Ran23(&kdum) * (high+1) ;  /* id is entry into highl */
				spin++;
				if(id<0 || id > high) tr=1;
			}
			if(highl[id]!=0) {
				cnter++;
				/* save everything */
				save[cnter]=highl[id];  /* for now, this stores the index into highl. This
										   index is the index into WEIptr set above */
				highl[id]=0;
				h--;
			}
		}
	}
	if(medium>0) {
		while(m>0) {
			tr=1;
			if((float)m/(float)medium <.1 || m<=2) {  /* find the next available mediuml[] */
				for(j=1;j<=medium;j++) {
					if(mediuml[j]!=0)id=j;
				}
				tr=0;
			}
			while(tr) {
				if(spin>50000)printf("Spinning in OrderWgts %d %d %d\n",m,medium,cnter);
				tr=0;
				id=Ran23(&kdum) * (medium+1) ;  /* id is entry into highl */
				spin++;
				if(id<0 || id > medium) tr=1;
			}
     		if(mediuml[id]!=0) {
				cnter++;
				/* save everything */
				save[cnter]=mediuml[id];  /* for now, this stores the index into mediuml. This
										   index is the index into WEIptr set above */
				mediuml[id]=0;
				m--;
			}
		}
	}

	if(low>0) {
		while(l>0) {
			tr=1;
			if((float)l/(float)low <.1 || l<=2) {  /* find the next available lowl[] */
				for(j=1;j<=low;j++) {
					if(lowl[j]!=0)id=j;
				}
				tr=0;
			}
			while(tr) {
				if(spin>50000)printf("Spinning in OrderWgts %d %d %d\n",l,low,cnter);
				tr=0;
				id=Ran23(&kdum) * (low+1) ;  /* id is entry into lowl */
				spin++;
				if(id<0 || id > low) tr=1;
			}
			if(lowl[id]!=0) {
				cnter++;
				/* save everything */
				save[cnter]=lowl[id];  /* for now, this stores the index into lowl. This
										   index is the index into WEIptr set above */
				lowl[id]=0;
				l--;
			}
		}
	}
	if(inum!=cnter) {printf("ERROR cnter & inum != %d %d\n",cnter,inum);exit(-1);}

	/* shuffle the order */
	ptr[1].num=WEIptr[1].num; ptr[1].remain=WEIptr[1].remain;  /* save the master info */
	for(i=1;i<=inum;i++) {
		id=save[i];
		if(id==0){printf("ERROR with id ==0 %d\n");exit(-1);}
		ptr[i].sid=WEIptr[id].sid;
		ptr[i].weight=WEIptr[id].weight;
		ptr[i].relmax=WEIptr[id].relmax;
		ptr[i].used=WEIptr[id].used;
		ptr[i].proj=WEIptr[id].proj;
		ptr[i].cumu=WEIptr[id].cumu;
	}
	/* just to be sure */
	WEIptr[1].num=ptr[1].num;WEIptr[1].remain=ptr[1].remain;

	/* Store the shuffled order and re-calculate cumu */
	for(i=1;i<=inum;i++) {
		WEIptr[i].sid=ptr[i].sid;
		WEIptr[i].weight=ptr[i].weight;
		WEIptr[i].relmax=ptr[i].relmax;
		WEIptr[i].used=ptr[i].used;
		WEIptr[i].proj=ptr[i].proj;
		if(i==1) {
			WEIptr[i].cumu=WEIptr[i].weight/grand;
		}else {
			WEIptr[i].cumu=(WEIptr[i].weight/grand)+WEIptr[i-1].cumu;
		}		
		/* printf(" weights %d %f %f\n",i,WEIptr[i].cumu,WEIptr[i].weight); */

		/* fprintf(crapf,"%d %f %f %f %d\n",j,WEIptr[j].cumu,WEIptr[j].weight,SECGptr[WEIptr[j].sid].prob,WEIptr[j].sid);  */
	}
	free(lowl);free(mediuml);free(highl);free(ptr);free(save);
}