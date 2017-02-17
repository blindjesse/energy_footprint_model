/* ReadPatterns() - Read/store the 30-m grid version of the digitized pads.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include "padpat.h"

extern	FILE	*themap;
 

void	ReadPatterns()
{
	int		r,c,i,j,k,id;
	int		icnt,nlines;

	/* 	Input pattern file format:
		No. of sequences
			end pattern ID for sequence 1
			"	"		"	"	"		2
			etc... for No. of sequences
THEN:
		num_of_pat
		ID r c nlines
		r c 
		r c
		r c

	After the sequence info, the first line is no. of patterns in the file, follwed by repeating pattern info such as ID
	max r, max c, & no. of lines to follow.  max r & max c are used to create a rectangle.  Then the lines that
	follow indicate the r & c to turn off.
*/



	/* No. of sequences */
	fscanf(themap,"%d\n",&numpat);   /* temp use of numpat */
	SEQptr= (SEQPAT *) calloc(numpat+1,sizeof(SEQPAT));
	for(i=1;i<=numpat;i++) {
		fscanf(themap,"%d\n",&icnt);  /* read the END ID */
		SEQptr[i].end=icnt;
		if(i==1) {
			SEQptr[i].beg=1;SEQptr[i].delta=SEQptr[i].end;
		}else {
			SEQptr[i].end=icnt;SEQptr[i].beg=SEQptr[i-1].end+1;
			SEQptr[i].delta=SEQptr[i].end-SEQptr[i].beg+1;
		}
	}

	/* Now read the pattern info */

	/* no. of patterns */
	fscanf(themap,"%d\n",&numpat);
	PADPptr=(PADPAT *) calloc(numpat+1,sizeof(PADPAT));
	if(PADPptr==NULL) {printf("PADPptr==NULL\n");exit(-1);}
	

	/* In this version we use the pattern PAD ID to index into PADPptr.  We check to make sure numpat and the ID
	are commensurate.  This ID is stored in the master pad struct (patID) and can be used along with centroid
	and boundary vertices of the patterns to generate polygonal outputs.  */
	for(icnt=1;icnt<=numpat;icnt++) {
		fscanf(themap,"%d %d %d %d\n",&id,&r,&c,&nlines);
		if(id<0 || id>numpat) {printf("ERROR, Pad Pattern ID is not valid %d\n",id);exit(-1);}
		PADPptr[id].r=r;PADPptr[id].c=c;PADPptr[id].aptr=(char *) calloc(r*c,sizeof(char));
		for(i=0;i<r*c;i++)PADPptr[id].aptr[i]=1;		/* create a rectangle */
		for(k=1;k<=nlines;k++) {
			fscanf(themap,"%d %d\n",&i,&j);	/* cells to turn off */
			PADPptr[id].aptr[i*c+j]=0;
		}
	}
	fclose(themap);
}