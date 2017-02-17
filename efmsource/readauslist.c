/* ReadAUSecList() - Read/store ausectlist.in  - list of AUs (USGS Oil/Gas Assessment Units) and sections within AUs.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include "plss.h"


void ReadAUSecList(char nam[255],char nam1[255])
{

	FILE	*fnam,*combos;
	int		ret,id,i,j,k,n,kk,cnt,id2,l;
	int		storeaup,au,proj,num;
	int		GetAUPcode(int , int );
	void	pause();
	int		yrbeg,yrend,nwells,bhole,twells;
	char	tr;


	AUSptr=(AUSECTLIST *) calloc(1,sizeof(AUSECTLIST));
		
	fnam=fopen(nam,"r");
	fscanf(fnam,"%d\n",&AUSptr->aus);  /* read no. of aus */
	fscanf(fnam,"%d\n",&AUSptr->projects);  /* read no. of projects */
	fscanf(fnam,"%d\n",&AUSptr->auproj);  /* read no. of auxproject combos */
	storeaup=AUSptr->auproj;



	/* to handle the combos */
	/* determine the actual max no. of au x projects given the desired number of combos */
	ret=255;
	if(strcmp(nam1,"none")!=0) { /* if none, there no combos; else nam1 = the name of the master combos.## */
		combos=fopen(nam1,"r");
		while(ret!=EOF) {
			ret=fscanf(combos,"%d %d %d %d %d %d %d %d\n",&au,&proj,&n,&twells,&nwells,&bhole,&yrbeg,&yrend); /* au, proj, number of auxprojects to combine */
			if(ret!=EOF) {
				AUSptr->projects=proj;AUSptr->auproj++;  /* assume proj in above read increments successively & every au proj combo
														 in combos files represents another unique au x project combo */
				for(j=1;j<=n;j++) {
					fscanf(combos,"%d %d %d %d\n",&au,&proj,&yrbeg,&yrend);
				}
			}
		}
		fclose(combos);
	}




	/* allocate by auproj */
	AUSptr->aulist= (int *) calloc(AUSptr->auproj+1,sizeof(int));
	AUSptr->projlist= (int *) calloc(AUSptr->auproj+1,sizeof(int));
	AUSptr->nsection= (int *) calloc(AUSptr->auproj+1,sizeof(int));
	AUSptr->SLIST= (SECTLIST *) calloc(AUSptr->auproj+1,sizeof(SECTLIST));

	for(i=1;i<=storeaup;i++) {  /* storeaup is no. to read from ausectlist.in */
		fscanf(fnam,"%d %d %d\n",&id,&j,&n); 
		AUSptr->aulist[i]=id; /* au */
		AUSptr->projlist[i]=j; /* project */
		AUSptr->nsection[i]=n; /* no. sections */
		if(AUSptr->nsection[i]>MAXWEI){printf("ERROR, too many sections\n");exit(-1);}
	}

	ret=255;cnt=0;
	while(ret!=EOF) {
		ret=fscanf(fnam,"%d %d %d\n",&id,&k,&n);
		if(ret!=EOF) {
			cnt++;
			/* double check to make sure info is lined up */
			if(AUSptr->aulist[cnt]!=id || AUSptr->projlist[cnt]!=k || AUSptr->nsection[cnt]!=n) {
				printf("ERROR, AUSptr codes not in sequence\n");exit(-1);
			}
			AUSptr->SLIST[cnt].sid2=(int *) calloc(n+1,sizeof(int));
			AUSptr->SLIST[cnt].proj=(int *) calloc(n+1,sizeof(int));
			for(j=1;j<=n;j++) {
				fscanf(fnam,"%d\n",&AUSptr->SLIST[cnt].sid2[j]);
				AUSptr->SLIST[cnt].proj[j]=k;  /* record original project  - to record original auxproj use cnt */
			}

			/* check with SECptr to make sure section au x proj codes are also stored the same way */
			for(j=1;j<=n;j++) {  /* go thru each SID associated with this AU x PROJ */
				id=AUSptr->SLIST[cnt].sid2[j];
				/* AUSptr->aulist[cnt] & AUSptr->projlist[cnt] These are the master values for accession=cnt */
				/* There should be an entry in SECptr[id].SEptr[].au and in ....proj that matches what we just stored
				   in AUSptr... */

				tr=0;
				for(l=1;l<=SECptr[id].combos;l++) {
					if(SECptr[id].SEptr[l].au == AUSptr->aulist[cnt] &&
					   SECptr[id].SEptr[l].proj == AUSptr->projlist[cnt]) tr=1;
				}
				if(!tr) {
					printf("Ausectionlist contains a auxproj for a SID that doesn't match SECptr\n");
					printf("%d %d %d %d %d %d\n",id,cnt,SECptr[id].SEptr[l].au,AUSptr->aulist[cnt],SECptr[id].SEptr[l].proj,AUSptr->projlist[cnt]); 
					exit(-1);
				}
			}
		}
	}




	/* Here is where we reopen combos.## and create the combos in ausectlist struct */
	ret=255;
	if(strcmp(nam1,"none")!=0) {
		combos=fopen(nam1,"r");
		while(ret!=EOF) {
			ret=fscanf(combos,"%d %d %d %d %d %d %d %d\n",&au,&proj,&n,&twells,&nwells,&bhole,&yrbeg,&yrend); /* au, proj, number of auxprojects to combine */
			if(ret!=EOF) {
				cnt++;
				AUSptr->aulist[cnt]=au;AUSptr->projlist[cnt]=proj;
				for(j=1;j<=n;j++) {
					fscanf(combos,"%d %d %d %d\n",&au,&proj,&yrbeg,&yrend);id=GetAUPcode(au,proj); /* id = auproj code */
					AUSptr->nsection[cnt]+=AUSptr->nsection[id];  /* sum no. of sections for this combo */
				}

				if(AUSptr->nsection[cnt]>MAXWEI){printf("ERROR, too many sections in combo\n");exit(-1);}
				AUSptr->SLIST[cnt].sid2=(int *) calloc(AUSptr->nsection[cnt]+1,sizeof(int));
				AUSptr->SLIST[cnt].proj=(int *) calloc(AUSptr->nsection[cnt]+1,sizeof(int));
				/* need to go thru the original lists and merge into this au x project combo */
			}
		}
		fclose(combos);
		combos=fopen(nam1,"r");ret=255;
		while(ret!=EOF) {
			ret=fscanf(combos,"%d %d %d %d %d %d %d %d\n",&au,&proj,&n,&twells,&nwells,&bhole,&yrbeg,&yrend); /* au, proj, number of auxprojects to combine */
			if(ret!=EOF) {
				kk=0;
				id=GetAUPcode(au,proj); /* the combo auxproj code */

				for(j=1;j<=n;j++) {
					fscanf(combos,"%d %d %d %d\n",&au,&proj,&yrbeg,&yrend);id2=GetAUPcode(au,proj); /* id = auproj code */
					for(k=1;k<=AUSptr->nsection[id2];k++) {
						kk++;
						AUSptr->SLIST[id].proj[kk]=AUSptr->SLIST[id2].proj[k];
						AUSptr->SLIST[id].sid2[kk]=AUSptr->SLIST[id2].sid2[k];
					}
					/* printf("%d %d\n",kk,id);printf("%d %d\n",AUSptr->SLIST[id].proj[kk],AUSptr->SLIST[id].sid2[kk]); */
				}
			}
		}
		fclose(combos);
	}




	if(cnt!=AUSptr->auproj){printf("ERROR, cnt and auproj != %d %d\n",cnt,AUSptr->auproj);exit(-1);}
	fclose(fnam);


	/* Initialize GStore - global storage of wells and pads per au x proj each time step */
	GStore = (GWELLS *) calloc(AUSptr->auproj+1,sizeof(GWELLS));
	GPStore = (GWELLS *) calloc(AUSptr->projects+1,sizeof(GWELLS));
}


