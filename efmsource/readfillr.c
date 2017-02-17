/* ReadFillR()  - Read/store information that delineates the proportion of time different build-out
designs are used to mediate oil/gas development.  Old terminology was 'infill or fill rates';
however, designs aren't necessary required to impose infilling.  
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



void	ReadFillR(char nam[255],char nam1[255])
{
	int			i,max,ret,iyr,cnt,*hit;
	int			au,proj,twells,bhole,perpad,size,start,stop,tbhole;
	int			horiz;  /* if a horizontal well */
	char		wellsyr[255];
	FILE		*fnam;
	int			GetAUPcode(int,int),code;
	float		value;
	void		Review();
	void		DoCombo(char *,FILLRATE *),DoG();
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
			if(code<=0){printf("ERROR, au,proj code not found %d %d\n",au,proj);exit(-1);}
			hit[code]++;
		}
	}
	fclose(fnam);



	FILptr=(FILLRATE *) calloc(max+1,sizeof(FILLRATE));  /* need to set to max auxproj combos */
	for(i=1;i<=max;i++) {
		FILptr[i].num=hit[i];   /* no. of specs for this au x proj combo */
		FILptr[i].next_ptr= (FILLRATE *) calloc(FILptr[i].num+1,sizeof(FILLRATE));
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
				FILptr[code].maxwells=twells;FILptr[code].setmaxwells=twells;FILptr[code].acre=bhole;FILptr[code].setmaxwells2=twells;
				FILptr[code].padpat=size;FILptr[code].start=start;FILptr[code].stop=stop;
				FILptr[code].wells=perpad;FILptr[code].BHOLE=tbhole;FILptr[code].BHOLEsave=tbhole;FILptr[code].proj=proj;FILptr[code].BHOLEsave2=tbhole;
				FILptr[code].horizontal=(char)horiz;FILptr[code].puse=(char)(atof(prob)*10.0); /* char prob of use */
				value=atof(wellsyr);
				if(value>0 && value <1) {
					FILptr[code].prob=value;
				}else {
					FILptr[code].numwells=(int)value;
				}
			} else {
				FILptr[code].next_ptr[hit[code]-1].maxwells=twells;
				FILptr[code].next_ptr[hit[code]-1].setmaxwells=twells;
				FILptr[code].next_ptr[hit[code]-1].acre=bhole;
				FILptr[code].next_ptr[hit[code]-1].padpat=size;
				FILptr[code].next_ptr[hit[code]-1].horizontal=(char)horiz;
				FILptr[code].next_ptr[hit[code]-1].puse=(char)(atof(prob)*10.0);
				FILptr[code].next_ptr[hit[code]-1].start=start;
				FILptr[code].next_ptr[hit[code]-1].stop=stop;
				FILptr[code].next_ptr[hit[code]-1].wells=perpad;FILptr[code].next_ptr[hit[code]-1].proj=proj;
				value=atof(wellsyr);
				if(value>0 && value <1) {
					FILptr[code].next_ptr[hit[code]-1].prob=value;
				}else {
					FILptr[code].next_ptr[hit[code]-1].numwells=(int)value;
				}
			}
		}
	}
	fclose(fnam);
	free(hit);

	/* create FILptr for combos if necessary */
	DoCombo(nam1,FILptr);

	/* init GWELLS to output info to sumrates; & GPStore */
	DoG();


	Review();   /* review fillr parameters - see if exit() is active at end of this routine */



/* 5/2015 add on.  Determine no. of specs then set up storage to record the number of times each spec is implemented.
Storage is by AU x actual proj.  NOTE: when using fillmod.* this storage is erased then reset with the full
complement of specs -> in FillMod().*/
	fnam=fopen(nam,"r");
	ret=255;MAXSPECS=0;
	while(ret!=EOF) {
		/* au, proj, spacing, max number, padpattern, nwells */
		ret=fscanf(fnam,"%d %d %d %d %s %d %d %d %d %s %d %d\n",&au,&proj,&twells,&tbhole,wellsyr,&bhole,&perpad,&size,&horiz,prob,&start,&stop);
		if(ret!=EOF) {
			MAXSPECS++;
		}
	}
	fclose(fnam);
	SPECRECORDptr=(SPECRECORD *) calloc(MAXSPECS+1,sizeof(SPECRECORD));

	fnam=fopen(nam,"r");
	ret=255;MAXSPECS=0;
	while(ret!=EOF) {
		/* au, proj, spacing, max number, padpattern, nwells */
		ret=fscanf(fnam,"%d %d %d %d %s %d %d %d %d %s %d %d\n",&au,&proj,&twells,&tbhole,wellsyr,&bhole,&perpad,&size,&horiz,prob,&start,&stop);
		if(ret!=EOF) {
			MAXSPECS++;
			SPECRECORDptr[MAXSPECS].au=au;SPECRECORDptr[MAXSPECS].proj=proj;SPECRECORDptr[MAXSPECS].size=size;
			SPECRECORDptr[MAXSPECS].BHA=bhole;SPECRECORDptr[MAXSPECS].perpad=perpad;		
		}
	}
	fclose(fnam);
}

int		GetAUPcode(int au, int proj)
{
	int		i;

	for(i=1;i<=AUSptr->auproj;i++) {
		if(AUSptr->aulist[i]==au && AUSptr->projlist[i]==proj) return(i);
	}
	return((int)0);
}



void	DoCombo(char nam1[255],FILLRATE *ptr)
{
	FILE	*combos;
	int		ret,au,proj,n,yrbeg,yrend,id,id2,twells,nwells,bhole;
	int		GetAUPcode(int,int),i,j;
	int		hit;


	if(strcmp(nam1,"none")==0)return;  /* combo option not selected; else name of the combos.## file  */
		combos=fopen(nam1,"r");ret=255;
		while(ret!=EOF) {
			ret=fscanf(combos,"%d %d %d %d %d %d %d %d\n",&au,&proj,&n,&twells,&nwells,&bhole,&yrbeg,&yrend); /* au, proj, number of auxprojects to combine */
			if(ret!=EOF) {
				id2=GetAUPcode(au,proj);  /* this is a new combo id */
				hit=0;
				ptr[id2].start=yrbeg;ptr[id2].stop=yrend;
				ptr[id2].maxwells=twells;ptr[id2].setmaxwells=twells;ptr[id2].setmaxwells2=twells;
				ptr[id2].BHOLE=bhole;ptr[id2].BHOLEsave=bhole;ptr[id2].BHOLEsave2=bhole;
				ptr[id2].numwells=nwells;ptr[id2].combofirst=1;
				/* allocate the following to store info */
				ptr[id2].list=(int *) calloc(AUSptr->auproj+1,sizeof(int));  /* saves a list of the auproj codes comprising this combo */
				for(j=1;j<=n;j++) {
					fscanf(combos,"%d %d %d %d\n",&au,&proj,&yrbeg,&yrend);id=GetAUPcode(au,proj); /* id = auproj code */
					ptr[id2].list[id]=1;  /* store the combined auxproj codes - here we assume that all should be included even if the yr codes don't match.
											 This is because we use this list to adjust total requested wells and BH the first time this NEW Combo FILptr is used in dopads */
					if(ptr[id].start==yrbeg && ptr[id].stop==yrend)hit++;
					for(i=2;i<=ptr[id].num;i++){
						if(ptr[id].next_ptr[i-1].start==yrbeg && ptr[id].next_ptr[i-1].stop==yrend)hit++;
					}
				}
				ptr[id2].num=hit;ptr[id2].next_ptr=(FILLRATE *) calloc(ptr[id2].num+1,sizeof(FILLRATE));
			}
		}
		fclose(combos);
		/* now that we've set FILptr[].num and next_ptr, populate the fields */
		combos=fopen(nam1,"r");ret=255;
		while(ret!=EOF) {
			ret=fscanf(combos,"%d %d %d %d %d %d %d %d\n",&au,&proj,&n,&twells,&nwells,&bhole,&yrbeg,&yrend); /* au, proj, number of auxprojects to combine */
			if(ret!=EOF) {
				id2=GetAUPcode(au,proj);  /* this is a new combo id */
				hit=0;
				for(j=1;j<=n;j++) {
					fscanf(combos,"%d %d %d %d\n",&au,&proj,&yrbeg,&yrend);id=GetAUPcode(au,proj); /* id = auproj code of the FILLR to combine */
					if(ptr[id].start==yrbeg && ptr[id].stop==yrend) {
						hit++;
						if(hit==1) {
							ptr[id2].acre=ptr[id].acre;ptr[id2].wells=ptr[id].wells;  /* wells per pad, acres per well */
							ptr[id2].proj=proj;ptr[id2].padpat=ptr[id].padpat;ptr[id2].prob=ptr[id].prob;
							ptr[id2].horizontal=ptr[id].horizontal;ptr[id2].puse=ptr[id].puse;
						}else {
							ptr[id2].next_ptr[hit-1].acre=ptr[id].acre;ptr[id2].next_ptr[hit-1].wells=ptr[id].wells;
							ptr[id2].next_ptr[hit-1].horizontal=ptr[id].horizontal;
							ptr[id2].next_ptr[hit-1].puse=ptr[id].puse;
							ptr[id2].next_ptr[hit-1].padpat=ptr[id].padpat;ptr[id2].next_ptr[hit-1].prob=ptr[id].prob;
							ptr[id2].next_ptr[hit-1].proj=proj;
							ptr[id2].next_ptr[hit-1].start=ptr[id2].start;ptr[id2].next_ptr[hit-1].stop=ptr[id2].stop;
						}
					}
					for(i=2;i<=ptr[id].num;i++){
						if(ptr[id].next_ptr[i-1].start==yrbeg && ptr[id].next_ptr[i-1].stop==yrend){
							hit++;
							if(hit==1) {
								ptr[id2].acre=ptr[id].next_ptr[i-1].acre;ptr[id2].wells=ptr[id].next_ptr[i-1].wells;  /* wells per pad, acres per well */
								ptr[id2].horizontal=ptr[id].next_ptr[i-1].horizontal;ptr[id2].puse=ptr[id].next_ptr[i-1].puse;
								ptr[id2].proj=proj;ptr[id2].padpat=ptr[id].next_ptr[i-1].padpat;ptr[id2].prob=ptr[id].next_ptr[i-1].prob;
							}else {
								ptr[id2].next_ptr[hit-1].acre=ptr[id].next_ptr[i-1].acre;ptr[id2].next_ptr[hit-1].wells=ptr[id].next_ptr[i-1].wells;  /* wells per pad, acres per well */
								ptr[id2].next_ptr[hit-1].proj=proj;ptr[id2].next_ptr[hit-1].padpat=ptr[id].next_ptr[i-1].padpat;ptr[id2].next_ptr[hit-1].prob=ptr[id].next_ptr[i-1].prob;
								ptr[id2].next_ptr[hit-1].start=ptr[id2].start;ptr[id2].next_ptr[hit-1].stop=ptr[id2].stop;
								ptr[id2].next_ptr[hit-1].horizontal=ptr[id].next_ptr[i-1].horizontal;
								ptr[id2].next_ptr[hit-1].puse=ptr[id].next_ptr[i-1].puse;
							}
						}
					}
				}
			}
		}
		fclose(combos);
}




void	Review()
{
	int		i,j;
	FILE	*review;

	review=fopen("reviewfill","w");

	for(i=1;i<=AUSptr->auproj;i++) {
		fprintf(review,"%d %d %d\n",FILptr[i].num,AUSptr->aulist[i],AUSptr->projlist[i]);
		fprintf(review,"%d %d %d %f %f %f %d %d %d %d %d %d %d\n",FILptr[i].maxwells,FILptr[i].setmaxwells,FILptr[i].numwells,FILptr[i].prob,FILptr[i].acre,FILptr[i].BHOLEsave,FILptr[i].wells,
			FILptr[i].padpat,FILptr[i].start,FILptr[i].stop,FILptr[i].proj,(int)FILptr[i].horizontal,(int)FILptr[i].puse);
		for(j=2;j<=FILptr[i].num;j++) {
			fprintf(review,"%d %d %d %f %f %f %d %d %d %d %d %d %d\n",FILptr[i].next_ptr[j-1].maxwells,FILptr[i].next_ptr[j-1].setmaxwells,FILptr[i].next_ptr[j-1].numwells,FILptr[i].next_ptr[j-1].prob,FILptr[i].next_ptr[j-1].acre,FILptr[i].next_ptr[j-1].BHOLEsave,FILptr[i].next_ptr[j-1].wells,
			FILptr[i].next_ptr[j-1].padpat,FILptr[i].next_ptr[j-1].start,FILptr[i].next_ptr[j-1].stop,FILptr[i].next_ptr[j-1].proj,(int)FILptr[i].next_ptr[j-1].horizontal,
			(int)FILptr[i].next_ptr[j-1].puse);
		}
	}
	fclose(review); /* exit(-1); */
}

/* set GWELLS with initial info */
void	DoG()
{
	int		i;
	for(i=1;i<=AUSptr->auproj;i++) {
		GStore[i].wells=FILptr[i].maxwells;GStore[i].pads=0;
	}
	for(i=1;i<=AUSptr->projects;i++) {
		GPStore[i].area=0;GPStore[i].pads=0;GPStore[i].wells=0;
	}

}










/* the functional version - save until the new version is totally working */
void	DoComboOLD(char nam1[255])
{
	FILE	*combos;
	int		ret,au,proj,n,yrbeg,yrend,id,id2,twells,nwells,bhole;
	int		GetAUPcode(int,int),i,j;
	int		hit;


	if(strcmp(nam1,"none")==0)return;  /* combo option not selected */
		combos=fopen(nam1,"r");ret=255;
		while(ret!=EOF) {
			ret=fscanf(combos,"%d %d %d %d %d %d %d %d\n",&au,&proj,&n,&twells,&nwells,&bhole,&yrbeg,&yrend); /* au, proj, number of auxprojects to combine */
			if(ret!=EOF) {
				id2=GetAUPcode(au,proj);  /* this is a new combo id */
				hit=0;
				FILptr[id2].start=yrbeg;FILptr[id2].stop=yrend;
				FILptr[id2].maxwells=twells;FILptr[id2].setmaxwells=twells;FILptr[id2].setmaxwells2=twells;
				FILptr[id2].BHOLE=bhole;FILptr[id2].BHOLEsave=bhole;FILptr[id2].BHOLEsave2=bhole;
				FILptr[id2].numwells=nwells;FILptr[id2].combofirst=1;
				/* allocate the following to store info */
				FILptr[id2].list=(int *) calloc(AUSptr->auproj+1,sizeof(int));  /* saves a list of the auproj codes comprising this combo */
				for(j=1;j<=n;j++) {
					fscanf(combos,"%d %d %d %d\n",&au,&proj,&yrbeg,&yrend);id=GetAUPcode(au,proj); /* id = auproj code */
					FILptr[id2].list[id]=1;  /* store the combined auxproj codes - here we assume that all should be included even if the yr codes don't match.
											 This is because we use this list to adjust total requested wells and BH the first time this NEW Combo FILptr is used in dopads */
					if(FILptr[id].start==yrbeg && FILptr[id].stop==yrend)hit++;
					for(i=2;i<=FILptr[id].num;i++){
						if(FILptr[id].next_ptr[i-1].start==yrbeg && FILptr[id].next_ptr[i-1].stop==yrend)hit++;
					}
				}
				FILptr[id2].num=hit;FILptr[id2].next_ptr=(FILLRATE *) calloc(FILptr[id2].num+1,sizeof(FILLRATE));
			}
		}
		fclose(combos);
		/* now that we've set FILptr[].num and next_ptr, populate the fields */
		combos=fopen(nam1,"r");ret=255;
		while(ret!=EOF) {
			ret=fscanf(combos,"%d %d %d %d %d %d %d %d\n",&au,&proj,&n,&twells,&nwells,&bhole,&yrbeg,&yrend); /* au, proj, number of auxprojects to combine */
			if(ret!=EOF) {
				id2=GetAUPcode(au,proj);  /* this is a new combo id */
				hit=0;
				for(j=1;j<=n;j++) {
					fscanf(combos,"%d %d %d %d\n",&au,&proj,&yrbeg,&yrend);id=GetAUPcode(au,proj); /* id = auproj code of the FILLR to combine */
					if(FILptr[id].start==yrbeg && FILptr[id].stop==yrend) {
						hit++;
						if(hit==1) {
							FILptr[id2].acre=FILptr[id].acre;FILptr[id2].wells=FILptr[id].wells;  /* wells per pad, acres per well */
							FILptr[id2].proj=proj;FILptr[id2].padpat=FILptr[id].padpat;FILptr[id2].prob=FILptr[id].prob;
							FILptr[id2].horizontal=FILptr[id].horizontal;
						}else {
							FILptr[id2].next_ptr[hit-1].acre=FILptr[id].acre;FILptr[id2].next_ptr[hit-1].wells=FILptr[id].wells;
							FILptr[id2].next_ptr[hit-1].horizontal=FILptr[id].horizontal;
							FILptr[id2].next_ptr[hit-1].padpat=FILptr[id].padpat;FILptr[id2].next_ptr[hit-1].prob=FILptr[id].prob;
							FILptr[id2].next_ptr[hit-1].proj=proj;
							FILptr[id2].next_ptr[hit-1].start=FILptr[id2].start;FILptr[id2].next_ptr[hit-1].stop=FILptr[id2].stop;
						}
					}
					for(i=2;i<=FILptr[id].num;i++){
						if(FILptr[id].next_ptr[i-1].start==yrbeg && FILptr[id].next_ptr[i-1].stop==yrend){
							hit++;
							if(hit==1) {
								FILptr[id2].acre=FILptr[id].next_ptr[i-1].acre;FILptr[id2].wells=FILptr[id].next_ptr[i-1].wells;  /* wells per pad, acres per well */
								FILptr[id2].horizontal=FILptr[id].next_ptr[i-1].horizontal;
								FILptr[id2].proj=proj;FILptr[id2].padpat=FILptr[id].next_ptr[i-1].padpat;FILptr[id2].prob=FILptr[id].next_ptr[i-1].prob;
							}else {
								FILptr[id2].next_ptr[hit-1].acre=FILptr[id].next_ptr[i-1].acre;FILptr[id2].next_ptr[hit-1].wells=FILptr[id].next_ptr[i-1].wells;  /* wells per pad, acres per well */
								FILptr[id2].next_ptr[hit-1].proj=proj;FILptr[id2].next_ptr[hit-1].padpat=FILptr[id].next_ptr[i-1].padpat;FILptr[id2].next_ptr[hit-1].prob=FILptr[id].next_ptr[i-1].prob;
								FILptr[id2].next_ptr[hit-1].start=FILptr[id2].start;FILptr[id2].next_ptr[hit-1].stop=FILptr[id2].stop;
								FILptr[id2].next_ptr[hit-1].horizontal=FILptr[id].next_ptr[i-1].horizontal;
							}
						}
					}
				}
			}
		}
		fclose(combos);
}
