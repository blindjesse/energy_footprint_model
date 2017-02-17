/* ReadSecPad() - Read/store the section-pad list 
(the ID of initial pads in each section). 
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include "plss.h"


void ReadSecPad(char nam[255])
{

	FILE	*fnam;
	int		ret,id,i,j,inum,pid;


	fnam=fopen(nam,"r");
	fscanf(fnam,"%d\n",&i);  /* read master section number */
	if(i!=maxsec) {printf("ERROR, master sections delta in readsecpad.c\n");exit(-1);}
	SECPptr=(SECPAD *) calloc(maxsec+1,sizeof(SECPAD));
	for(i=1;i<=maxsec;i++) {
		SECPptr[i].padid=(int *) calloc(MAXSECPAD+1,sizeof(int));  /* SID2 code of up to 200 pads */
	}
	ret=255;
	while(ret!=EOF) {
		ret=fscanf(fnam,"%d %d\n",&id,&inum);
		if(ret!=EOF) {
			SECPptr[id].num=inum;
			SECPptr[id].counter=inum;
			for(j=1;j<=inum;j++) {
				fscanf(fnam,"%d \n",&pid);
				if(j<=200)SECPptr[id].padid[j]=pid;
			}
			if(inum>200) {SECPptr[id].num=200;SECPptr[id].counter=200;} /* set to max allowable */
		}
	}
	fclose(fnam);

}
