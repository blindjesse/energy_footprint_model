/* binascii() - converts a binary files created with crinput.exe to an ascii grid */

#include <stdio.h>
#include <math.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
 
typedef struct STOREC{
	int			row;
	int			col;
	int		value;
}STOREC;
STOREC		*SCptr;


typedef struct HEADER{
	long long	counter;
	float		grain;
	int			row;
	int			col;
	float		ULE;  /* upper left UTME */
	float		ULN;  /* upper left UTMN */
}HEADER;
HEADER		*HEADptr;

 
long long		counter;		/* No. of valid cells */
double			counterd;		


FILE		*fnam,*foutmap;
long long	index,id,counter,maxcnt;
int			row,col,*thelayer;


main(int argc, char *argv[])
{


	int			i,j;


	if(strcmp(argv[1],"?")==0) {
		printf("binascii <input- binary grid from crinput> <output- ascii grid>\n"); 
		exit(-1);
	}

	foutmap=fopen(argv[2],"w");
	fnam=fopen(argv[1],"rb");

	HEADptr=(HEADER *) calloc(1,sizeof(HEADER));
	fread(HEADptr,sizeof(HEADER),1,fnam);
	

	index=HEADptr->counter;
	SCptr=(STOREC *) calloc(index+1,sizeof(STOREC));
	fread(SCptr,sizeof(STOREC),index+1,fnam);
	fclose(fnam);

	row=HEADptr->row;col=HEADptr->col;
	thelayer=(int *) calloc(row*col,sizeof(int));
	for(i=0;i<row*col;i++)thelayer[i]=-9999;
	for(id=1;id<=index;id++) {
		counter=SCptr[id].row*(long long)col;
		counter+=SCptr[id].col;
		thelayer[counter]=SCptr[id].value;
	}

	fprintf(foutmap,"ncols %d\n",HEADptr->col);
	fprintf(foutmap,"nrows %d\n",HEADptr->row);
	fprintf(foutmap,"xllcorner %f\n",HEADptr->ULE);
	fprintf(foutmap,"yllcorner %f\n",HEADptr->ULN - ((float)row*HEADptr->grain));
	fprintf(foutmap,"cellsize %f\n",HEADptr->grain);
	fprintf(foutmap,"NODATA_VALUE -9999\n");

	for(i=0;i<row;i++) {
		for(j=0;j<col;j++) {
			fprintf(foutmap,"%d ",(int)thelayer[i*col+j]);
		}
		fprintf(foutmap,"\n");
	}
}