/* ReadGInfo() - Read/store PLSS section neighbor info, and other info (proportion with leases,
proportion by ownership, total surface area, developed surface area, proportion with no
leases) about each section. 
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include "plss.h"


void ReadGInfo(char nam[255])
{

	FILE	*fnam;
	int		ret,id,i,j;
	char	value1[255];



	fnam=fopen(nam,"r");
	fscanf(fnam,"%d\n",&i);  /* read master section number */
	if(i!=maxsec) {printf("ERROR, master sections delta in readginfo.c\n");exit(-1);}
	SECGptr=(SECGINFO *) calloc(maxsec+1,sizeof(SECGINFO));
	for(i=1;i<=maxsec;i++) {
		SECGptr[i].neigh=(int *) calloc(9,sizeof(int));  /* SID2 code of the 8 neighbors */

	}
	ret=255;
	while(ret!=EOF) {
		ret=fscanf(fnam,"%d ",&id);
		if(ret!=EOF) {
			for(j=1;j<=8;j++) fscanf(fnam,"%d ",&SECGptr[id].neigh[j]);
			fscanf(fnam,"%d %d %d %d",&SECGptr[id].lease,&SECGptr[id].fed0,&SECGptr[id].fed1,&SECGptr[id].fed2);
			fscanf(fnam,"%s ",value1);SECGptr[id].prob=atof(value1);
			fscanf(fnam,"%s ",value1);SECGptr[id].ha1=atof(value1);
			fscanf(fnam,"%s ",value1);SECGptr[id].ha3=atof(value1);
			fscanf(fnam,"%s ",value1);SECGptr[id].nlha=atof(value1);
			fscanf(fnam,"%s ",value1);SECGptr[id].propnl=atof(value1);
		}
	}
	fclose(fnam);

}
