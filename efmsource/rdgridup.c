/* RdGridUp() - Updates the 10-m road grid & nodes.  Called from ProcessRds()for every new road.
This is a modification of rdgrid.c. 
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include "efm.h"
#include "geo.h"
#include "roads.h"
#include "rdend.h"
#include "rdgrid.h"


void	RdGridUp()
{

	int			rdid;
	char		value1[255],value2[255];
	int			oldid,r,c,cnt;
	double		n,e,saven,savee,deltan,deltae,stepsize;
	char		ConvToRC10(double,double,int *, int *);
	void		pause();
	int			i,j,step,k,l,id,jkl;
	long long   indexl;
	char		tr,Valid10(int,int);


	rdid=maxrdend;  /* this newline is rdid=maxrdend */
	oldid=0;saven=0;
	for(jkl=1;jkl<=NOVERTS;jkl++) {
			n=RDVERTSptr[jkl].n;e=RDVERTSptr[jkl].e;
			if(oldid==0)oldid=rdid;
			if(oldid==rdid) {
				if(saven==0) {
					saven=n;savee=e;
				}else {
					/* create 10-m grid between saven/e and n/e */
					deltan=n-saven;deltae=e-savee;

					step=10;stepsize=10.0;
					deltan=deltan/(double)10.0;deltae=deltae/(double)10.0;
					tr=0;
					if(fabs(deltan)>=10 || fabs(deltae)>=10)tr=1;
					while(tr) {
						tr=0;
						step+=10;stepsize+=10.0;
						deltan=n-saven;deltae=e-savee;
						deltan=deltan/stepsize;deltae=deltae/stepsize;
						if(fabs(deltan)>=10 || fabs(deltae)>=10)tr=1;
					}


					if(ConvToRC10(saven,savee,&r,&c)) {
						indexl=(long long) r * (long long)col10;indexl+=(long long)c;
						if(grid10[indexl]==-9999) {
							grid10[indexl]=rdid;
						}else if(grid10[indexl]>0 && grid10[indexl]!=rdid) {
							grid10[indexl]=-1;
						}
					}else {
						printf("%d %f %f %f %f %f %f\n",rdid,saven,savee,n,e,deltan,deltae); 
						/* pause(); */
					}
					saven=saven+deltan;
					savee=savee+deltae;


				
					for(i=1;i<=step;i++){
						if(ConvToRC10(saven,savee,&r,&c)) {
							indexl=(long long) r * (long long)col10;indexl+=(long long)c;
							if(grid10[indexl]==-9999) {
								grid10[indexl]=rdid;
							}else if(grid10[indexl]>0 && grid10[indexl]!=rdid) {
								grid10[indexl]=-1;
							}
						}else {
							printf("%d %f %f %f %f %f %f\n",rdid,saven,savee,n,e,deltan,deltae);
							/* pause(); */
						}
							saven=saven+deltan;
							savee=savee+deltae;
						}
					saven=n;savee=e;
				}
			}else {
				oldid=rdid;
				saven=n;savee=e;
			}
		}
}


void	RdNodeUP()
{
	int			i,j,cnt,*hit,k,l,id;
	long long	indexl;
	char		Valid10(int,int);


	/* determine no. of nodes for each road segment */
	RDNptr=(RDNODE *) calloc(maxrdend+1,sizeof(RDNODE));
	for(i=0;i<row10;i++) {
		for(j=0;j<col10;j++) {
			indexl=(long long) i * (long long)col10;indexl+=(long long)j;
			if(grid10[indexl]==-1) {  /* if a node */
				hit=(int *) calloc(10,sizeof(int));
				/* check around this node and determine rdid of connecting roads */
				cnt=0;

				for(k=i-1;k<=i+1;k++) {
					for(l=j-1;l<=j+1;l++) {
						if(Valid10(k,l)) {
							if(grid10[k*col10+l]>0) {
								cnt++;id=grid10[k*col10+l];
								hit[cnt]=id;  /* save the id cause we're go back thru and adjust */
							}
						}
					}
				}

				/* clear dups */
				for(k=1;k<cnt;k++) {
					for(l=k+1;l<=cnt;l++) {
						if(hit[k]==hit[l])hit[l]=0;
					}
				}
				for(k=1;k<=cnt;k++) {
					if(hit[k]>0) {
						RDNptr[hit[k]].count++;
					}
				}
				free(hit);

			}  /* if a node */
		} /* for j */
	} /* for i */
}