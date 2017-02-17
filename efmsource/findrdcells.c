/* FindRdCells() - Derives the approximate centroid of a section, then uses a search window for road cells
that do not overlap a nontraverse area (i.e., non-developable area).  Stores these locations for use in Locate().  This is performed
for a selected section the first time the section is accessed for an AUxPROJ combo and time step, and after
the establishment of a pad and road; e.g., called in Establish() before Locate() is called, then after code=2 return from Locate(). 
*/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include "efm.h"
#include "geo.h"
#include "plss.h"
#include "roads.h"

extern	int		col;

void	FindRDCells(int auproj, int sid)
{

	char		ConvToRC2(float,float, int *, int *),piss;
	void		ConvToPts(int,int,float *, float *);
	long long	indexl;
	char		Valid(int,int);
	int			i,j,displ,r,c,k,ii,jj;
	float		n,e,tn,te;
	int			br,er,bc,ec;
	

	/* pick up ca. the middle of this section and use it as the focal pt of a search window */
	i=SECPTSptr[sid].cnt/2;
	if(i<=0)i=SECPTSptr[sid].cnt;  /* ###.cnt==1, then the above math gives us i=0 which results
								   in n and e being zero in the following line. Really shouldn't be using
								   this sid if ###.cnt==0 [given how SelectSections filters sids before storing them in
								   WEIptr].  Using masterspace.render may give us 1 pt in a section.. */

	n=SECPTSptr[sid].n[i];e=SECPTSptr[sid].e[i];
	piss=ConvToRC2(n,e,&r,&c);
	if(!piss) {
		printf("in FindRdCells() %f %f %d %d %d %d\n",n,e,r,c,sid,SECPTSptr[sid].cnt);exit(-1);
	}

	/* displ=5500m/grain; */
	displ=4500/30;

	j=STORERDptr[1].no;
	for(i=1;i<=j;i++) {  /* clean up the list from last time.  We don't reinit ###.no, so we should be
						 able to Monte Carlo without any problems */
		if(STORERDptr[i].list!=NULL)free(STORERDptr[i].list);
	}
	STORERDptr[1].no=0;  /* Initialize before setting */
	STORERDptr[1].sid=sid;  /* Initialize before setting; used in UpdateRdStore() */

	br=er=r;bc=ec=c; 

	/* need to save results & determine nearest after evaluating all orientations */
	while(displ>0) {
		displ--;
		br--;er++;bc--;ec++;
		ii=br;
		for(jj=bc;jj<=ec;jj++) { /* across the top */
			if(Valid(br,jj)) {
				indexl=(long long)br * (long long)col;indexl+=(long long)jj;
				if(rds[indexl]>0 || rds[indexl]==-10) {  /* this assumes that all new roads are type=10.  This allows roads not
														 stored in rdmaster, rdindex, and corresponding structs to be accessed.
														 See NOTE in roads.h regarding TEMPRptr */
					if(nontraverse[indexl]<=0) {
						if(STORERDptr[1].no<50000){
							STORERDptr[1].no++;
							STORERDptr[STORERDptr[1].no].r=ii;STORERDptr[STORERDptr[1].no].c=jj;
							ConvToPts(ii,jj,&tn,&te); /* coords of a road */
							STORERDptr[STORERDptr[1].no].n=tn;STORERDptr[STORERDptr[1].no].e=te;
							k=STORERDptr[1].no;
							STORERDptr[k].list= (char *) calloc(SECPTSptr[sid].cnt+1,sizeof(char)); 
						}else {
							displ=0;
						}
					}

				}
			} /* end of if Valid */
		} /* across the top */
		jj=bc;
		for(ii=br;ii<=er;ii++) { /* across LHS */
			if(Valid(ii,bc)) {
				indexl=(long long)ii * (long long)col;indexl+=(long long)bc;
				if(rds[indexl]>0 || rds[indexl]==-10) {
					if(nontraverse[indexl]<=0) {
						if(STORERDptr[1].no<50000){
							STORERDptr[1].no++;
							STORERDptr[STORERDptr[1].no].r=ii;STORERDptr[STORERDptr[1].no].c=jj;
							ConvToPts(ii,jj,&tn,&te); /* coords of a road */
							STORERDptr[STORERDptr[1].no].n=tn;STORERDptr[STORERDptr[1].no].e=te;
							k=STORERDptr[1].no;
							STORERDptr[k].list= (char *) calloc(SECPTSptr[sid].cnt+1,sizeof(char)); 
						}else {
							displ=0;
						}
					}
				}
			} /* end of if Valid */
		} /* across the LHS */

		jj=ec;
		for(ii=br;ii<=er;ii++) { /* across RHS */
			if(Valid(ii,ec)) {
				indexl=(long long)ii * (long long)col;indexl+=(long long)ec;
				if(rds[indexl]>0 || rds[indexl]==-10) {
					if(nontraverse[indexl]<=0) {
						if(STORERDptr[1].no<50000){
							STORERDptr[1].no++;
							STORERDptr[STORERDptr[1].no].r=ii;STORERDptr[STORERDptr[1].no].c=jj;
							ConvToPts(ii,jj,&tn,&te); /* coords of a road */
							STORERDptr[STORERDptr[1].no].n=tn;STORERDptr[STORERDptr[1].no].e=te;
							k=STORERDptr[1].no;
							STORERDptr[k].list= (char *) calloc(SECPTSptr[sid].cnt+1,sizeof(char)); 
						}else {
							displ=0;
						}
					}
				}
			} /* end of if Valid */
		} /* across the RHS */

		ii=er;
		for(jj=bc;jj<=ec;jj++) { /* across the bottom */
			if(Valid(er,jj)) {
				indexl=(long long)er * (long long)col;indexl+=(long long)jj;
				if(rds[indexl]>0 || rds[indexl]==-10) {
					if(nontraverse[indexl]<=0) {
						if(STORERDptr[1].no<50000){
							STORERDptr[1].no++;
							STORERDptr[STORERDptr[1].no].r=ii;STORERDptr[STORERDptr[1].no].c=jj;
							ConvToPts(ii,jj,&tn,&te); /* coords of a road */
							STORERDptr[STORERDptr[1].no].n=tn;STORERDptr[STORERDptr[1].no].e=te;
							k=STORERDptr[1].no;
							STORERDptr[k].list= (char *) calloc(SECPTSptr[sid].cnt+1,sizeof(char)); 
						}else {
							displ=0;
						}
					}
				}
			} /* end of if Valid */
		} /* across the bottom */
	} /* end of while */
}