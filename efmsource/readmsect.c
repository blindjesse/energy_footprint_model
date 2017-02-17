/* ReadMSect() - Read/store information about pads, wells, and the bottom hole area already developed and
that is available to future development for each PLSS section.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include "plss.h"

 

void	ReadMSect(char nam[255])
{
 
	FILE		*fin;
	int			ret,sid,combos,tpads,apads,ipads,twells,awells,iwells;
	int			au,proj,j;
	char		value1[255],value2[255],value3[255];



	fin=fopen(nam,"r");

	/* first up is maxsec value */
	fscanf(fin,"%d\n",&j);
	if(j!=maxsec){printf("ERROR, maxsec error in readmsect()\n");exit(-1);} 


	SECptr=(SEC *) calloc(maxsec+1,sizeof(SEC));
	SECBASEptr=(SEC *) calloc(maxsec+1,sizeof(SEC));

	ret=255;
	while(ret !=EOF){
		ret=fscanf(fin,"%d %d\n",&sid,&combos);  /* section, no. of au x proj combos */
		if(ret!=EOF) {		
			if(sid>maxsec){printf("ERROR, sid >maxsect\n");exit(-1);}
			SECptr[sid].combos=combos;SECBASEptr[sid].combos=combos;
			/* read totals for the section */
			fscanf(fin,"%d %d %d %d %d %d %s %s\n",&tpads,&apads,&ipads,&twells,&awells,&iwells,value1,value2);
			SECptr[sid].tpads=tpads;SECptr[sid].apads=apads;SECptr[sid].ipads=ipads;
			SECptr[sid].twells=twells;SECptr[sid].awells=awells;SECptr[sid].iwells=iwells;
			SECptr[sid].padarea=atof(value1);SECptr[sid].surface=atof(value2);

			SECBASEptr[sid].tpads=tpads;SECBASEptr[sid].apads=apads;SECBASEptr[sid].ipads=ipads;
			SECBASEptr[sid].twells=twells;SECBASEptr[sid].awells=awells;SECBASEptr[sid].iwells=iwells;
			SECBASEptr[sid].padarea=atof(value1);SECBASEptr[sid].surface=atof(value2);



			SECptr[sid].SEptr=(SECAP *) calloc(combos+1,sizeof(SECAP));
			SECBASEptr[sid].SEptr=(SECAP *) calloc(combos+1,sizeof(SECAP));
			for(j=1;j<=combos;j++) {
				fscanf(fin,"%d %d\n",&au,&proj);
				fscanf(fin,"%d %d %d %d %d %d %s %s %s\n",&tpads,&apads,&ipads,&twells,&awells,&iwells,value1,value2,value3);  /* read au-proj specific info */
				SECptr[sid].SEptr[j].au=au;SECptr[sid].SEptr[j].proj=proj;
				SECptr[sid].SEptr[j].tpads=tpads;SECptr[sid].SEptr[j].apads=apads;SECptr[sid].SEptr[j].ipads=ipads;
				SECptr[sid].SEptr[j].twells=twells;SECptr[sid].SEptr[j].awells=awells;SECptr[sid].SEptr[j].iwells=iwells;
				SECptr[sid].SEptr[j].padarea=atof(value1);SECptr[sid].SEptr[j].BHC=atof(value2);
				SECptr[sid].SEptr[j].BHA=atof(value3);  /* this is where we can boost the BHA */
				if(SECptr[sid].SEptr[j].BHA >=629 && SECptr[sid].SEptr[j].BHA <640) SECptr[sid].SEptr[j].BHA=641; /* Boost so
																												  we can use 320 and 640
																												  acres for horizontal wells */


				SECBASEptr[sid].SEptr[j].au=au;SECBASEptr[sid].SEptr[j].proj=proj;
				SECBASEptr[sid].SEptr[j].tpads=tpads;SECBASEptr[sid].SEptr[j].apads=apads;SECBASEptr[sid].SEptr[j].ipads=ipads;
				SECBASEptr[sid].SEptr[j].twells=twells;SECBASEptr[sid].SEptr[j].awells=awells;SECBASEptr[sid].SEptr[j].iwells=iwells;
				SECBASEptr[sid].SEptr[j].padarea=atof(value1);SECBASEptr[sid].SEptr[j].BHC=atof(value2);
				SECBASEptr[sid].SEptr[j].BHA=atof(value3); /* do the same here - this is where we can boost the BHA */
				if(SECBASEptr[sid].SEptr[j].BHA >=629 && SECBASEptr[sid].SEptr[j].BHA <640) SECBASEptr[sid].SEptr[j].BHA=641;
			}
		}
	} /* while */
	fclose(fin);
}