/* CreatePads() - Uses padpattern# output (UTM Centroids) in the simulation and the relativized boundary coordinates of the pad patterns
to create pads with the proper UTM coordinates.  Output from this module is used to create the pad shapefile. 
*/

#include <stdio.h>
#include <math.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include "createpads.h"


void	CreatePads(char nam[255], char cmd[255],int beg, int end,char nam2[255])
{

	FILE	*fnam,*fout2,*tempc;
	char	value1[255],value2[255],wells[255];
	int		id,ret,count,i,time1,time2;
	int		padid,temporal;
	double	n,e;
	PT		*ptr;


	/* printf("createpad <patbnd.cor - template vertices> <padpattern - wlci pad centroids> < >=time stamp> < <=time stamp> <output>\n");exit(-1); */

	tempc=fopen("tempc","w");
	time1=beg;			/* process info for only this time stamp */
	time2=end;
	fout2=fopen(nam2,"w");
	fprintf(fout2,"ACCESS,YR,N,E,NN,EE\n");  /* header */


	/* Process the template vertices */
	fnam=fopen(nam,"r");
	fscanf(fnam,"%d\n",&maxid);


	Pptr=(PAT *) calloc(maxid+1,sizeof(PAT));
	if(Pptr==NULL) {printf("ERROR, Pptr==NULL\n");exit(-1);}


	/* format of padbnd.cor  - id, count, n and east displacement to add to padpattern centroid */
	ret=255;
	while(ret!=EOF) {
		ret=fscanf(fnam,"%d %d %s %s\n",&id,&count,value1,value2);
		if(ret!=EOF) {
			Pptr[id].vertices=count;
			Pptr[id].displn=atof(value1);Pptr[id].disple=atof(value2);
			Pptr[id].aptr=(PT *) calloc(count+1,sizeof(PT));
			ptr=Pptr[id].aptr;
			if(ptr==NULL) {printf("ERROR, ptr==NULL\n");exit(-1);}
			for(i=1;i<=count;i++) {
				fscanf(fnam,"%s %s\n",value1,value2);
				ptr[i].utmn=atof(value1);ptr[i].utme=atof(value2);
			}/* end of for i */
		} /* if ret */
	}/* while */
	fclose(fnam);


	
	/* read and process the pad centroids.
	PADID, pattern ID, time stamp, utmn, utme of centroid */
	fnam=fopen(cmd,"r");
	ret=255;
	while(ret!=EOF) {
		ret=fscanf(fnam,"%d %d %d %s %s %s\n",&padid,&id,&temporal,value1,value2,wells);  /* wells is for the new version of - wlciv13a  */
		if(ret!=EOF) {
			if(id<0 || id>maxid){printf("ERROR, id of centroid file is invalid %d %d\n",maxid,id);exit(-1);}
			ptr=Pptr[id].aptr;
			if(ptr==NULL) {printf("ERROR in picking up ptr\n");exit(-1);}
			if(temporal >=time1 && temporal <=time2) {
				n=atof(value1);e=atof(value2);

				/* add displacement */
				n=n+Pptr[id].displn;e=e+Pptr[id].disple;
				if(Pptr[id].displn>50 || Pptr[id].disple>50){printf("Warning - displ >50 %d\n",id);}

				count=Pptr[id].vertices;
				for(i=1;i<=count;i++) {
					fprintf(fout2,"%d, %d, %f, %f,%f,%f\n",padid,temporal,n-ptr[i].utmn, e-ptr[i].utme,n,e);
					fprintf(tempc,"%d %d %f %f %f %f\n",padid,temporal,n-ptr[i].utmn, e-ptr[i].utme,n,e);
				}/* end of for i */
				fprintf(fout2,"%d, %d, %f, %f,%f,%f\n",padid,temporal,n-ptr[1].utmn, e-ptr[1].utme,n,e);  /* This ensures that we loop back to the start */
				fprintf(tempc,"%d %d %f %f %f %f\n",padid,temporal,n-ptr[i].utmn, e-ptr[i].utme,n,e);
			}/* if temporal */
		} /* if ret */
	}/* while */

	free(Pptr);fclose(fout2);fclose(tempc);
}