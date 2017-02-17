/* ReadMSpace() - Read/store the GRTS points (UTM).  These are used
to randomly locate pads in each PLSS section.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include "padalloc.h"
#include "plss.h"

 extern		int	col;

void	ReadMSpace(char nam[255])
{
 
	FILE		*fin;
	int			ret,sec,inum,count,index;
	char		value1[255],value2[255];
	int			i,j,r,c;
	char		ConvToRC2(float,float ,int *,int *),tr;
	int			sub;
	void		pause();
	int			select;






	fin=fopen(nam,"r");
	fscanf(fin,"%d\n",&maxsec);  /* read master section number  */

	SECPTSptr=(SECTION *) calloc(maxsec+1,sizeof(SECTION));


	ret=255;

		while(ret !=EOF){
		ret=fscanf(fin,"%d %d\n",&sec,&inum);  /* section, no. of pts that follow */
		if(ret!=EOF) {		/* */
			SECPTSptr[sec].cnt=inum;
			SECPTSptr[sec].e=(float *) calloc(inum+1,sizeof(float));
			SECPTSptr[sec].n=(float *) calloc(inum+1,sizeof(float));
			SECPTSptr[sec].eb=(float *) calloc(inum+1,sizeof(float));
			SECPTSptr[sec].nb=(float *) calloc(inum+1,sizeof(float));
			SECPTSptr[sec].avail=(char *) calloc(inum+1,sizeof(char));
			SECPTSptr[sec].space=(char *) calloc(inum+1,sizeof(char));
			for(j=1;j<=inum;j++) {
				fscanf(fin,"%s %s\n",value1,value2);
				SECPTSptr[sec].counter++;index=SECPTSptr[sec].counter;
				if(index>SECPTSptr[sec].cnt) {printf("ERROR, index and cnt error %d %d\n",index,SECPTSptr[sec].cnt);exit(-1);}
				SECPTSptr[sec].n[index]=atof(value1);SECPTSptr[sec].e[index]=atof(value2);
			}
			if(SECPTSptr[sec].cnt!=SECPTSptr[sec].counter) {printf("ERROR, cnt and counter != \n");exit(-1);}
		}
	} /* while */
	fclose(fin);
}


