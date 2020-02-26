/* PadShapefile() - Creates the pad shapefile at the end of a simulated replicate.
Calls Createpads() to transform pad centroids in padpattern# to a pad boundary in UTM coordinates.
Uses information in dumpp#### to derive pad attributes.
Calls Dopads.py to create the pad shapefile and to merge it with the baseline pad shapefile.
The end result is the generation of padrep# shapefile where # is the replication number.
*/

#include <stdio.h>
#include <math.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include "file_operations.h"

 
typedef struct STOREPS{ /* indexed by ID */
	float	utmn;
	float	utme;
}STOREPS;
STOREPS		*STptrps;
int			maxidps;



void PadShapefile(int repnumber, int duration, int datarecord, char nam[255])
{
 
	int		i,j,ret;
	char	cmd[255],number[255],nam2[255];
	char	prefix[255];
	void	CreatePads(char *, char *,int,int, char *);
	FILE	*fnam;
	int		id,yr;
	char	value1[255],value2[255],value3[255],value4[255];
	void	Dumpp(char *, char *);
	int		time;

	printf("\n");printf("CREATING SHAPEFILES\n");

	/*	printf("autopad <padpattern#  beg#> <end# > < >= beg timestamp> < <= end timestamp >\n");exit(-1); */

	time=duration+2012;   /* This is the correct year */ 

	strcpy(nam2,"npads");sprintf(number,"%d",repnumber);strcat(nam2,number); strcat(nam2,".csv");
	strcpy(cmd,"padpattern");sprintf(number,"%d",repnumber);strcat(cmd,number);
	CreatePads(nam,cmd,time,time,nam2);

	i=repnumber;

			/* create centroid file here  ************************************************** */
			/* non csv file is called tempc */
			fnam=fopen("tempc","r");ret=255;maxidps=0;
			while(ret!=EOF) {
				ret=fscanf(fnam,"%d %d %s %s %s %s\n",&id,&yr,value1,value2,value3,value4);
				if(ret!=EOF) {
					if(id>maxidps)maxidps=id;
				}
			}
			fclose(fnam);
			STptrps=(STOREPS *) calloc(maxidps+1,sizeof(STOREPS));


			fnam=fopen("tempc","r");ret=255;
			while(ret!=EOF) {
				ret=fscanf(fnam,"%d %d %s %s %s %s\n",&id,&yr,value1,value2,value3,value4);
				if(ret!=EOF) {
					STptrps[id].utmn=atof(value3);STptrps[id].utme=atof(value4);
				}
			}
			fclose(fnam);
			strcpy(nam,"centr");sprintf(number,"%d",i);strcat(nam,number);
			sprintf(number,"%d",time);strcat(nam,number);
			strcat(nam,".csv");

			fnam=fopen(nam,"w");
			fprintf(fnam,"ACCESS,N,E\n");
			for(j=1;j<=maxidps;j++) {
				if(STptrps[j].utmn>0)fprintf(fnam,"%d, %f, %f\n",j,STptrps[j].utmn,STptrps[j].utme);
			}
			fclose(fnam);
			free(STptrps);
	/* ************************************************************************************* */
	/* ************************************************************************************* */

			/* Process dumpp# file */
			strcpy(cmd,"dumpp");sprintf(number,"%d",i);strcat(cmd,number);
			sprintf(number,"%d",time);strcat(cmd,number);

			strcpy(nam2,"dodo");sprintf(number,"%d",i);strcat(nam2,number);
			sprintf(number,"%d",time);strcat(nam2,number);
			strcat(nam2,".csv");
			Dumpp(cmd,nam2);



	    /* rename npads#.csv to npads.csv */
    sprintf(cmd, REMOVE_IF_EXISTS, "npads.csv"); 
		/* copy npads#.csv to npads.csv */
		strcpy(nam2,"npads");sprintf(number,"%d",i);strcat(nam2,number);strcat(nam2,".csv");
		strcpy(cmd,COPY);strcat(cmd,nam2);strcat(cmd," npads.csv");
		printf("%s\n",cmd);
		system(cmd);

			/* rename centr#.csv to centr.csv */
      sprintf(cmd, REMOVE_IF_EXISTS, "centr.csv");system(cmd);

			/* copy npads#.csv to npads.csv */
			strcpy(nam2,"centr");sprintf(number,"%d",i);strcat(nam2,number);
			sprintf(number,"%d",time);strcat(nam2,number);
			strcat(nam2,".csv");
			strcpy(cmd,COPY);strcat(cmd,nam2);strcat(cmd," centr.csv");
			printf("%s\n",cmd);
			system(cmd);


			/* rename dodo#.csv to centr.csv */
      sprintf(cmd, REMOVE_IF_EXISTS, "dodo.csv");system(cmd);

			/* copy npads#.csv to npads.csv */
			strcpy(nam2,"dodo");sprintf(number,"%d",i);strcat(nam2,number);
			sprintf(number,"%d",time);strcat(nam2,number);
			strcat(nam2,".csv");
			strcpy(cmd,COPY);strcat(cmd,nam2);strcat(cmd," dodo.csv");
			printf("%s\n",cmd);
			system(cmd);



		/* execute dopads.py */
		strcpy(cmd,"dopads.py");
		printf("%s\n",cmd);
		system(cmd);


		strcpy(prefix,"padrep");sprintf(number,"%d",i);		strcat(prefix,number);
		printf("Deleting files with prefix %s\n",prefix);
		strcat(prefix,"*");
		strcpy(cmd,REMOVE);strcat(cmd,prefix);system(cmd);
		/* rename thepads.** */
		strcpy(prefix,"padrep");sprintf(number,"%d",i);strcat(prefix,number);sprintf(number,"%d",time);strcat(prefix,number);

		strcpy(cmd,RENAME);strcat(cmd,"thepads.dbf ");strcpy(nam,prefix);strcat(nam,".dbf");strcat(cmd,nam);
		system(cmd);

		strcpy(cmd,RENAME);strcat(cmd,"thepads.sbn ");strcpy(nam,prefix);strcat(nam,".sbn");strcat(cmd,nam);
		system(cmd);

		strcpy(cmd,RENAME);strcat(cmd,"thepads.prj ");strcpy(nam,prefix);strcat(nam,".prj");strcat(cmd,nam);
		system(cmd);

		strcpy(cmd,RENAME);strcat(cmd,"thepads.sbx ");strcpy(nam,prefix);strcat(nam,".sbx");strcat(cmd,nam);
		system(cmd);

		strcpy(cmd,RENAME);strcat(cmd,"thepads.shp ");strcpy(nam,prefix);strcat(nam,".shp");strcat(cmd,nam);
		system(cmd);

		strcpy(cmd,RENAME);strcat(cmd,"thepads.shp.xml ");strcpy(nam,prefix);strcat(nam,".shp.xml");strcat(cmd,nam);
		system(cmd);
		
		strcpy(cmd,RENAME);strcat(cmd,"thepads.shx ");strcpy(nam,prefix);strcat(nam,".shx");strcat(cmd,nam);
		system(cmd);
}	


void	Dumpp(char namin[255],char namout[255])
{

	int			j,ret;
	FILE		*in,*out;
	char		value1[255];
	int			id,au,proj,SID2,begin,end,activedyn,nwells,activewells,lease,fed1;
	int			yearbeg,yearend;

	in=fopen(namin,"r");
	out=fopen(namout,"w");
	fprintf(out,"ACCESS,AU,PROJ,SID,START,STOP,ACTIVE,NWELLS,ACWELLS\n");

	ret=255;
	while(ret!=EOF) {
		ret=fscanf(in,"%d %d %d %d %d %d %d %d %d %d %d %s\n",&id,&au,&proj,&SID2,&begin,&end,&activedyn,&nwells,&activewells,&lease,
			&fed1,value1);
		if(ret!=EOF) {
			fprintf(out,"%d, %d, %d, %d, %d, %d, %d, %d, %d\n",id,au,proj,SID2,begin,end,activedyn,nwells,activewells);
			for(j=1;j<=activewells;j++) {
				fscanf(in,"%d %d %s\n",&yearbeg,&yearend,value1);
			}
		}
	}
	fclose(in);fclose(out);
}
