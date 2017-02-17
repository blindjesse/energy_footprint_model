/* FillMod() - Handles the use of multiple build-out design versions within the same run.
Updates global structures that store the multiple designs and that regulate the frequency
of implementation (user provided).
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include "efm.h"
#include "geo.h"
#include  "plss.h"
#include "padalloc.h"



void	FillMod(char nam[255])
{

	char		wellsyr[255];
	FILE		*fnam,*fnam1;
	char		value1[255],value2[255],nam1[255];
	void		ProcessFill(char *,int,char *);
	int			ret,ret1;
	int			au,proj,twells,tbhole,bhole,perpad,size,horiz,start,stop;
	char		prob[255]; /* probability of annual use of a build-out spec */
	int			vert,dir,hor,t;

	nfillers=0;
	if(strcmp(nam,"none")==0) return;


	fnam=fopen(nam,"r");
	ret=255;nfillers=0;
	while(ret!=EOF) {  /* name of fillertime; probability, combos name */
		if(nfillers==0){
			ret=fscanf(fnam,"%s %s %s %d %d %d\n",value1,value2,nam1,&vert,&dir,&hor);  /* handles fixed FILLmax[] vaues - for 3 build-outs */
			FILLmax[1]=vert;FILLmax[2]=dir;FILLmax[3]=hor;
		}else {
			ret=fscanf(fnam,"%s %s %s\n",value1,value2,nam1);
		}
		if(ret!=EOF) {
			nfillers++;
		}
	}
	fclose(fnam);

	FILSTptr=(FILSTORE *) calloc(nfillers+1,sizeof(FILSTORE));
	FILSTptr[1].n=nfillers;  /* no. of filler specs */

	fnam=fopen(nam,"r");
	ret=255;nfillers=0;
	while(ret!=EOF) {
		if(nfillers==0){
			ret=fscanf(fnam,"%s %s %s %d %d %d\n",value1,value2,nam1,&vert,&dir,&hor);  /* handles fixed FILLmax[] vaues - for 3 build-outs */
		}else {
			ret=fscanf(fnam,"%s %s %s\n",value1,value2,nam1);
		}
		if(ret!=EOF) {
			nfillers++;
			FILSTptr[nfillers].prob=atof(value2);  /* set the probability of annual use OF THIS ENTIRE BUILD_OUT DESIGN */
			FILSTptr[nfillers].prob+=FILSTptr[nfillers-1].prob;  /* derive cumulative */

		}
	}
	fclose(fnam);

	/*  ASSUME that the first spec is the main spec already procssed by readfillr().  Start with the second set of specs */
	fnam=fopen(nam,"r");
	ret=255;nfillers=0;
	while(ret!=EOF) {
		if(nfillers==0){
			ret=fscanf(fnam,"%s %s %s %d %d %d\n",value1,value2,nam1,&vert,&dir,&hor);  /* handles fixed FILLmax[] vaues - for 3 build-outs */
		}else {
			ret=fscanf(fnam,"%s %s %s\n",value1,value2,nam1);
		}
		if(ret!=EOF) {
			nfillers++;
			if(nfillers>1) {
				ProcessFill(value1,nfillers,nam1);
			}
		}
	}

/* Here is where we reset MAXSPECS using all the possible specs. */
	free(SPECRECORDptr);  /* Already set in readfillr */
	MAXSPECS=0;
	/* count the number of specs */
	fnam=fopen(nam,"r");
	ret=255;t=0;
	while(ret!=EOF) {
		if(t==0){ /* stupid trick */
			ret=fscanf(fnam,"%s %s %s %d %d %d\n",value1,value2,nam1,&vert,&dir,&hor);  /* handles fixed FILLmax[] vaues - for 3 build-outs */
			t=1;
		}else {
			ret=fscanf(fnam,"%s %s %s\n",value1,value2,nam1);
		}
		if(ret!=EOF) {
			ret1=255;fnam1=fopen(value1,"r");
			while(ret1!=EOF) {
				ret1=fscanf(fnam1,"%d %d %d %d %s %d %d %d %d %s %d %d\n",&au,&proj,&twells,&tbhole,wellsyr,&bhole,&perpad,&size,&horiz,prob,&start,&stop);
				if(ret1!=EOF) {
					MAXSPECS++;
				}
			}
			fclose(fnam1);
		}
	}
	fclose(fnam);
	SPECRECORDptr= (SPECRECORD *) calloc(MAXSPECS+1,sizeof(SPECRECORD));

	MAXSPECS=0;
	/* count the number of specs */
	fnam=fopen(nam,"r");
	ret=255;t=0;
	while(ret!=EOF) {
		if(t==0){
			ret=fscanf(fnam,"%s %s %s %d %d %d\n",value1,value2,nam1,&vert,&dir,&hor);  /* handles fixed FILLmax[] vaues - for 3 build-outs */
			t=1;
		}else {
			ret=fscanf(fnam,"%s %s %s\n",value1,value2,nam1);
		}
		if(ret!=EOF) {
			ret1=255;fnam1=fopen(value1,"r");
			while(ret1!=EOF) {
				ret1=fscanf(fnam1,"%d %d %d %d %s %d %d %d %d %s %d %d\n",&au,&proj,&twells,&tbhole,wellsyr,&bhole,&perpad,&size,&horiz,prob,&start,&stop);
				if(ret1!=EOF) {
					MAXSPECS++;
					SPECRECORDptr[MAXSPECS].au=au;SPECRECORDptr[MAXSPECS].proj=proj;SPECRECORDptr[MAXSPECS].size=size;
					SPECRECORDptr[MAXSPECS].BHA=bhole;SPECRECORDptr[MAXSPECS].perpad=perpad;						
				}
			}
			fclose(fnam1);
		}
	}
	fclose(fnam);

}

/* nam[] is the name of the fillr.# file.  nam1 is the name of the associated combos file, which can be = "none". */
void	ProcessFill(char nam[255],int nfillers,char nam1[255])
{

	int		max,i,ret,iyr,cnt,*hit;
	FILE	*fnam;
	int		GetAUPcode(int,int),code;
	int		au,proj,twells,bhole,perpad,size,start,stop,tbhole;
	int		horiz;
	char	wellsyr[255];
	FILLRATE	*ptr;
	float		value;
	void		ReviewMOD(int);
	void		DoCombo(char *,FILLRATE *); /* name of combos.## file and FILptr ptr */
	char		prob[255];

	/* there is a max number of au x proj combos  - first determine the number of entries per combo */
	max=AUSptr->auproj;
	hit=(int *) calloc(max+1,sizeof(int));
	fnam=fopen(nam,"r");

	ret=255;i=0;
	while(ret!=EOF) {
		ret=fscanf(fnam,"%d %d %d %d %s %d %d %d %d %s %d %d\n",&au,&proj,&twells,&tbhole,wellsyr,&bhole,&perpad,&size,&horiz,prob,&start,&stop);
		if(ret!=EOF) {
			code=GetAUPcode(au,proj);
			if(code<=0){printf("ERROR, au,proj code not found in fillmod() %d %d\n",au,proj);exit(-1);}
			hit[code]++;
		}
	}
	fclose(fnam);



	FILSTptr[nfillers].PTR =(FILLRATE *) calloc(max+1,sizeof(FILLRATE));  /* need to set to max auxproj combos */
	ptr=FILSTptr[nfillers].PTR;
	for(i=1;i<=max;i++) {
		ptr[i].num=hit[i];   /* no. of specs for this au x proj combo */
		ptr[i].next_ptr= (FILLRATE *) calloc(ptr[i].num+1,sizeof(FILLRATE));
		hit[i]=0;  /* reset and reuse to set struct */
	}

	fnam=fopen(nam,"r");
	ret=255;
	while(ret!=EOF) {
		/* au, proj, spacing, max number, padpattern, nwells */
		ret=fscanf(fnam,"%d %d %d %d %s %d %d %d %d %s %d %d\n",&au,&proj,&twells,&tbhole,wellsyr,&bhole,&perpad,&size,&horiz,prob,&start,&stop);
		if(ret!=EOF) {
			code=GetAUPcode(au,proj);  /* au x proj accession code */
			hit[code]++;
			if(hit[code]==1) {
				ptr[code].maxwells=twells;ptr[code].setmaxwells=twells;ptr[code].acre=bhole;ptr[code].setmaxwells2=twells;
				ptr[code].padpat=size;ptr[code].start=start;ptr[code].stop=stop;
				ptr[code].horizontal=(char)horiz;ptr[code].puse=(char)(atof(prob)*10.0);
				ptr[code].wells=perpad;ptr[code].BHOLE=tbhole;ptr[code].BHOLEsave=tbhole;ptr[code].proj=proj;ptr[code].BHOLEsave2=tbhole;
				value=atof(wellsyr);
				if(value>0 && value <1) {
					ptr[code].prob=value;
				}else {
					ptr[code].numwells=(int)value;
				}
			} else {
				/* don't think the following 2 lines are necessary since these are set in FLIptr[1] above */
				ptr[code].next_ptr[hit[code]-1].maxwells=twells;
				ptr[code].next_ptr[hit[code]-1].setmaxwells=twells;ptr[code].next_ptr[hit[code]-1].setmaxwells2=twells;

				ptr[code].next_ptr[hit[code]-1].acre=bhole;
				ptr[code].next_ptr[hit[code]-1].padpat=size;
				ptr[code].next_ptr[hit[code]-1].horizontal=(char)horiz;
				ptr[code].next_ptr[hit[code]-1].puse=(char)(atof(prob)*10.0);
				ptr[code].next_ptr[hit[code]-1].start=start;
				ptr[code].next_ptr[hit[code]-1].stop=stop;
				ptr[code].next_ptr[hit[code]-1].wells=perpad;ptr[code].next_ptr[hit[code]-1].proj=proj;
				value=atof(wellsyr);
				if(value>0 && value <1) {
					ptr[code].next_ptr[hit[code]-1].prob=value;
				}else {
					ptr[code].next_ptr[hit[code]-1].numwells=(int)value;
				}
			}
		}
	}
	fclose(fnam);
	free(hit);

	ptr=FILSTptr[nfillers].PTR;

	/* DoCombos() is in readfillr.c */
	DoCombo(nam1,ptr);  /* name of combos file and the ptr to the FILptr struct;  nam1 may be = "none" */

	ReviewMOD(nfillers); 
}



void	ReviewMOD(int entry)
{
	int			i,j;
	FILLRATE	*ptr;
	FILE		*review;

	ptr=FILSTptr[entry].PTR;

	review=fopen("reviewfill","a");  /* originally populated in readfillr () */
	fprintf(review,"Specs for nfiller= %d\n",entry);

	for(i=1;i<=AUSptr->auproj;i++) {
		fprintf(review,"%d %d %d\n",ptr[i].num,AUSptr->aulist[i],AUSptr->projlist[i]);
		fprintf(review,"%d %d %d %f %f %f %d %d %d %d %d %d %d\n",ptr[i].maxwells,ptr[i].setmaxwells,ptr[i].numwells,ptr[i].prob,ptr[i].acre,ptr[i].BHOLEsave,ptr[i].wells,
			ptr[i].padpat,ptr[i].start,ptr[i].stop,ptr[i].proj,(int)ptr[i].horizontal,(int)ptr[i].puse);
		for(j=2;j<=ptr[i].num;j++) {
			fprintf(review,"%d %d %d %f %f %f %d %d %d %d %d %d %d\n",ptr[i].next_ptr[j-1].maxwells,ptr[i].next_ptr[j-1].setmaxwells,ptr[i].next_ptr[j-1].numwells,ptr[i].next_ptr[j-1].prob,ptr[i].next_ptr[j-1].acre,ptr[i].next_ptr[j-1].BHOLEsave,ptr[i].next_ptr[j-1].wells,
			ptr[i].next_ptr[j-1].padpat,ptr[i].next_ptr[j-1].start,ptr[i].next_ptr[j-1].stop,ptr[i].next_ptr[j-1].proj,(int)ptr[i].next_ptr[j-1].horizontal,(int)ptr[i].next_ptr[j-1].puse);
		}
	}
	fclose(review);
}

