/* AutoRdc() - automatically creates road shapefiles from rdlines#.csv.
Process:
baserdinfo.in contains the attributes of the baseline road layer.  This info
and the same info from rdlines#.csv (for the specified rep and year) is written to tempr.  Also, the
relevant n,e,rdid info from rdlines#.csv is written to rdl_rep_year.csv.

The updated activation/deactivation info in roadfo# is read and stored in store[].  roadfo#
only contains the info for a specified year (or years since the last roadfo was dumpped out).  Thus,
you must read all the previous roadfo files up to the year of interest to update the active/inative
status of roads.  Gotta make sure all the previous files are being read - the 2-parms inputs should
allow you to access all the previous files?.  

Tempr is converted to tempr.csv which contains the attributes for baseline and new roads.  In this conversion,
the active status from store[] is added to the data, including time when a road became inactive.  Currently using a -1 to indicate
inactive roads.

Note that store is hard coded to 500000 (max rdid of <500000).
  
*/

#include <stdio.h>
#include <math.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include "rdshapefile.h"
#include "file_operations.h"
 

 


void	RdShapefile(int repnumber, int duration, int datarecord)
{
 
	int		i,beg,end;
	char	cmd[255],number[255],nam[255];
	char	prefix[255];
	FILE	*fnam,*fin;
	int		time;
	void	Extract(char *, char *,int);
	void	CreateRDJ();
	void	ReadRdfo(int,int);


	/*	printf("autord <rdlines#.csv   beg#> <end# > <time begin> <time end> <time step> <2-parms: begin and step for roadfo>\n"); */


	MAXRDENDSH=94491;  /* has to match the version of the road layer used in a sim */
/* baserdinfovn5.in - version 5n - hardcoded in extract.c */

	time=2012+duration;
	roadbSH=2012+datarecord;roadsSH=datarecord;


      i=repnumber;
			strcpy(cmd,"rdlines");sprintf(number,"%d",i);strcat(cmd,number);strcat(cmd,".csv");  /* input file */
			strcpy(nam,"rdl");sprintf(number,"%d",i);strcat(nam,number);sprintf(number,"%d",time);strcat(nam,number);
			strcat(nam,".csv");  /* output file */
			 Extract(cmd,nam,time);
			 ReadRdfo(i,time);  /* process all the roadfos up to this time,; using tstep */
			 CreateRDJ();  /* creates the info to join to the merged road file */

			/* rename rdlines#.csv to rdlines.csv */
      sprintf(cmd,REMOVE_IF_EXISTS,"rdlines.csv");system(cmd);

			/* copy npads#.csv to npads.csv */
			strcpy(nam,"rdl");sprintf(number,"%d",i);strcat(nam,number);
			sprintf(number,"%d",time);strcat(nam,number);
			strcat(nam,".csv");
			strcpy(cmd,COPY);strcat(cmd,nam);strcat(cmd," rdlines.csv");
			printf("%s\n",cmd);
			system(cmd);

			/* execute dordslb.py */
			strcpy(cmd,"dordslb.py");
			system(cmd);


			strcpy(prefix,"rdrep");sprintf(number,"%d",i);strcat(prefix,number);
			printf("Deleting files with prefix %s\n",prefix);
			strcat(prefix,"*");
			strcpy(cmd,REMOVE);strcat(cmd,prefix);system(cmd);
			/* rename therds ** */
			strcpy(prefix,"rdrep");sprintf(number,"%d",i);strcat(prefix,number);sprintf(number,"%d",time);strcat(prefix,number);

			strcpy(cmd,RENAME);strcat(cmd,"therds.dbf ");strcpy(nam,prefix);strcat(nam,".dbf");strcat(cmd,nam);
			system(cmd);

			strcpy(cmd,RENAME);strcat(cmd,"therds.sbn ");strcpy(nam,prefix);strcat(nam,".sbn");strcat(cmd,nam);
			system(cmd);

			strcpy(cmd,RENAME);strcat(cmd,"therds.prj ");strcpy(nam,prefix);strcat(nam,".prj");strcat(cmd,nam);
			system(cmd);

			strcpy(cmd,RENAME);strcat(cmd,"therds.sbx ");strcpy(nam,prefix);strcat(nam,".sbx");strcat(cmd,nam);
			system(cmd);

			strcpy(cmd,RENAME);strcat(cmd,"therds.shp ");strcpy(nam,prefix);strcat(nam,".shp");strcat(cmd,nam);
			system(cmd);

			strcpy(cmd,RENAME);strcat(cmd,"therds.shp.xml ");strcpy(nam,prefix);strcat(nam,".shp.xml");strcat(cmd,nam);
			system(cmd);
		
			strcpy(cmd,RENAME);strcat(cmd,"therds.shx ");strcpy(nam,prefix);strcat(nam,".shx");strcat(cmd,nam);
			system(cmd);



			strcpy(prefix,"rdbufrep");sprintf(number,"%d",i);strcat(prefix,number);
			printf("Deleting files with prefix %s\n",prefix);
			strcat(prefix,"*");
			strcpy(cmd,REMOVE);strcat(cmd,prefix);system(cmd);
			/********** rename therdsbuf */
			strcpy(prefix,"rdbufrep");sprintf(number,"%d",i);strcat(prefix,number);sprintf(number,"%d",time);strcat(prefix,number);

			strcpy(cmd,RENAME);strcat(cmd,"therdsbuf.dbf ");strcpy(nam,prefix);strcat(nam,".dbf");strcat(cmd,nam);
			system(cmd);

			strcpy(cmd,RENAME);strcat(cmd,"therdsbuf.sbn ");strcpy(nam,prefix);strcat(nam,".sbn");strcat(cmd,nam);
			system(cmd);

			strcpy(cmd,RENAME);strcat(cmd,"therdsbuf.prj ");strcpy(nam,prefix);strcat(nam,".prj");strcat(cmd,nam);
			system(cmd);

			strcpy(cmd,RENAME);strcat(cmd,"therdsbuf.sbx ");strcpy(nam,prefix);strcat(nam,".sbx");strcat(cmd,nam);
			system(cmd);

			strcpy(cmd,RENAME);strcat(cmd,"therdsbuf.shp ");strcpy(nam,prefix);strcat(nam,".shp");strcat(cmd,nam);
			system(cmd);

			strcpy(cmd,RENAME);strcat(cmd,"therdsbuf.shp.xml ");strcpy(nam,prefix);strcat(nam,".shp.xml");strcat(cmd,nam);
			system(cmd);
		
			strcpy(cmd,RENAME);strcat(cmd,"therdsbuf.shx ");strcpy(nam,prefix);strcat(nam,".shx");strcat(cmd,nam);
			system(cmd);

}	



/* Extract() - creates tempr and extracts time-stamped data form rdlines#.csv */

void	Extract(char namin[255],char namout[255],int theyr)
{
	
	FILE	*fnam,*in,*out;
	char	value1[255],value2[255];
	char	buf[255];
	char	*tok;
	int		i,ret;
	int		rdid,type,yr,active;
	int		oldid;
	float	n,e,buffer;

	/* first read baserdinfo.in and write to tempr */
	fnam=fopen("tempr","w");
	in=fopen("baserdinfovn5.in","r");
	/* read header */
	for(i=1;i<=6;i++)fscanf(in,"%s ",value1);
	ret=255;
	while(ret!=EOF) {
		ret=fscanf(in,"%d %d %s %s %d %d\n",&rdid,&type,value1,value2,&yr,&active);
		if(ret!=EOF) {
			fprintf(fnam,"%d %d %s %s %d %d\n",rdid,type,value1,value2,yr,active);
		}
	}
	fclose(in);

	oldid=0;
	in=fopen(namin,"r");
	out=fopen(namout,"w");
	fprintf(out,"N,E,rdid\n");

	/* read, write header */
	fgets(buf, sizeof(buf), in);
	tok = strtok(buf, ",");  
	tok = strtok(NULL, ",");  
	tok = strtok(NULL, ",");  
	tok = strtok(NULL, ",");  
	tok = strtok(NULL, ",");  
	tok = strtok(NULL, ","); 

	while(fgets(buf, sizeof(buf), in)!=NULL) {
		tok = strtok(buf, ",");n=atof(tok);
		tok = strtok(NULL, ",");e=atof(tok);
		tok = strtok(NULL, ",");rdid=atoi(tok);rdid=rdid+MAXRDENDSH;
		tok = strtok(NULL, ",");yr=atoi(tok);
		tok = strtok(NULL, ",");buffer=atof(tok);
		tok = strtok(NULL, ",");active=atoi(tok);
		type=5;
		if(yr<=theyr) {
			if(oldid==0) {
				oldid=rdid;
				fprintf(fnam,"%d %d %f %f %d %d\n",rdid,type,buffer*2,buffer,yr,active);
			}else {
				if(oldid==rdid) {
				}else {
					oldid=rdid;
					fprintf(fnam,"%d %d %f %f %d %d\n",rdid,type,buffer*2,buffer,yr,active);
				}
			}
			fprintf(out,"%f, %f, %d\n",n,e,rdid);
		}
	}
	fclose(in);fclose(out);fclose(fnam);
}
	


/* ReadRdfo() - process all roadfos up to the specified time period */
void	ReadRdfo(int i,int time)
{  
	int		j;
	char	cmd[255],number[255];
	FILE	*in;
	int		id,active,pad,ret,thetime;
	
	storerSH=(STORESH *) calloc(500000,sizeof(STORESH));  /* hard coded */

	/* read all the roadfos up to time = time */

	for(j=roadbSH;j<=time;j=j+roadsSH){
		strcpy(cmd,"roadfo");sprintf(number,"%d",i);strcat(cmd,number);
		sprintf(number,"%d",j);strcat(cmd,number);
		in=fopen(cmd,"r");

		ret=255;
		while(ret!=EOF) {
			ret=fscanf(in,"%d %d %d %d\n",&id,&active,&pad,&thetime);
			if(ret!=EOF) {
				if(id>=500000){printf("ERROR, id too large in readrdfo\n");exit(-1);}
				if(active ==0)active=-1;
				storerSH[id].active=(char)active;
				storerSH[id].time=thetime;
				if(active==1)storerSH[id].time=0;  /* just to be sure */
			}
		}
		fclose(in);
	}
}


/* Createrdj() - creates the info to join to the road layer */
void	CreateRDJ()
{
	
	FILE	*in,*out;
	char	value1[255],value2[255];
	char	buf[255];
	char	*tok;
	int		i,ret;
	int		rdid,type,yr,active;
	int		time;
	float	n,e,buffer;

	/* first read baserdinfo.in and write to tempr */
	in=fopen("tempr","r");
	out=fopen("tempr.csv","w");  /* this can be hard coded cause right after this we use this *.csv file, then recycle */
	fprintf(out,"rdid,type,width,buffer,yr,active,yrinact\n");
	ret=255;
	while(ret!=EOF) {
		ret=fscanf(in,"%d %d %s %s %d %d\n",&rdid,&type,value1,value2,&yr,&active);
		if(ret!=EOF) {
			if(rdid>=500000){printf("ERROR, may have too many rds for storerSH\n");exit(-1);}
			if(storerSH[rdid].active==1)active=1;
			if(storerSH[rdid].active==-1)active=-1;
			time=0;if(active==-1)time=storerSH[rdid].time;
			fprintf(out,"%d, %d, %s, %s, %d, %d, %d\n",rdid,type,value1,value2,yr,active,time);
		}
	}
	fclose(in);fclose(out);
	free(storerSH);
}
