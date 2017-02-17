/* Crinput() - Converts an ASCII grid file to a customized binary file that
contains a header with row, col, grian, and upperleft UTM coordinates,
and a data segment that only contains the row, col, and value of non-zero grid elements.
*/



#include <stdio.h>
#include <math.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
 
typedef struct STORE{
	int			row;
	int			col;
	int		value;
	struct STORE	*nextptr;
}STORE;
STORE		*Sptr,*ESptr;


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

FILE			*thelog,*header; /* a log of processing results incl. errors, headerinfo for us
										when using the output files */


FILE	*themap,*outmap;
int		row,col;
float	grain;
float	utme,utmn;
int		rowm,colm;
float	grainm;
float	utmem,utmnm;




main(int argc, char *argv[])
{
	void			ReadLayer1(int);
	long long		i;
	long long		num;
	double			numd,amount;
	
	if(strcmp(argv[1],"?")==0) {
		printf("crinput <input- ascii grid> <output- binary file> \n");exit(-1);
	}

	thelog=fopen("THELOG","a");
	header=fopen("HEADER","a");

	fprintf(thelog,"%s %s %s\n",argv[1],argv[2],argv[3]);
	fprintf(header,"%s %s %s\n",argv[1],argv[2],argv[3]);

	counter=0;counterd=0.0;
	themap=fopen(argv[1],"r");
	ReadLayer1((int)0);

	printf("Finished reading/processing input \n");

	SCptr=(STOREC *) calloc(counter+1,sizeof(STOREC));
	if(SCptr==NULL) {
		printf("ERROR, SCptr==NULL,; insufficient memory\n");exit(-1);
	}

	printf("Creating binary output\n");
	fprintf(thelog,"Creating binary output\n");


	ESptr=Sptr;counterd=0;
	for(i=1;i<=counter;i++) {
		counterd=i;
		SCptr[i].row=ESptr->row;SCptr[i].col=ESptr->col;SCptr[i].value=ESptr->value;
		ESptr=ESptr->nextptr;
		if(ESptr==NULL && i<counter) {
			printf("ERROR, ran out of ESptr struct before reaching end of counter %f\n",counterd);
			fprintf(thelog,"ERROR, ran out of ESptr struct before reaching end of counter %f\n",counterd);
			exit(-1);
		}
	}
	printf("Writing output \n");
	outmap=fopen(argv[2],"wb");
	/* write header */
	fwrite(HEADptr,sizeof(HEADER),1,outmap);

	/* write the data */
	num=fwrite(SCptr,sizeof(STOREC),counter+1,outmap);
	fclose(outmap);
	numd=(double)num;
	printf("Number of records written = %f\n",numd);
	fprintf(thelog,"Number of records written = %f\n",numd);
	fprintf(header,"binary %f\n",numd);
	fprintf(thelog,"\n");
}


void	ReadLayer1(int code)
{
	int		i,j,value;
	char	valuec[255],st[255];
	float	valuef;

	printf("Reading map layer\n");
	/* read map file */
	fscanf(themap,"%s %d",st,&col);		/* no. of column */
		if(code==1)fprintf(outmap,"%s %d\n",st,col);
	fscanf(themap,"%s %d",st,&row);		/* no. of rows */
		if(code==1)fprintf(outmap,"%s %d\n",st,row);
	fscanf(themap,"%s %s",st,valuec);		/* easting */
		if(code==1)fprintf(outmap,"%s %s\n",st,valuec);
		utme=atof(valuec);
	fscanf(themap,"%s %s",st,valuec);		/* northing */
		if(code==1)fprintf(outmap,"%s %s\n",st,valuec);
		utmn=atof(valuec);
	fscanf(themap,"%s %s",st,valuec);		/* grain size */
		if(code==1)fprintf(outmap,"%s %s\n",st,valuec);
		grain=atof(valuec);
	fscanf(themap,"%s %d",st,&value);		/* -9999 */
		if(code==1)fprintf(outmap,"%s %d\n",st,value);

	/* derive upper left UTMN */
	utmn+= (grain * (float)row);
	fprintf(thelog,"rows, cols, grain, upper left UTMs = %d %d %f %f %f\n",row,col,grain,utme,utmn);





	for(i=0;i<row;i++) {
		for(j=0;j<col;j++) {
			fscanf(themap,"%s",valuec);	
			value=atoi(valuec);

			if(value>0) {
				if(Sptr==NULL) {
					Sptr=(STORE *) calloc(1,sizeof(STORE));
					ESptr=Sptr;
					Sptr->row=i;Sptr->col=j;Sptr->value=value;counter++;counterd++;
				}else {
					ESptr->nextptr=(STORE *) calloc(1,sizeof(STORE));
					ESptr=ESptr->nextptr;
					if(ESptr==NULL) {printf("ERROR, ESptr== NULL\n");exit(-1);}
					ESptr->row=i;ESptr->col=j;ESptr->value=value;counter++;counterd++;
				}
			}
		}
	}	
	fclose(themap);

	printf("No. of valid cells = %f\n",counterd);
	fprintf(thelog,"No. of valid cells = %f\n",counterd);
	fprintf(header,"%f\n",counterd);

	fprintf(header,"%d %d %f %f %f\n",row,col,grain,utme,utmn);	

	HEADptr=(HEADER *) calloc(1,sizeof(HEADER));
	HEADptr->counter=counter;
	HEADptr->row=row;HEADptr->col=col;
	HEADptr->grain=grain;HEADptr->ULE=utme;HEADptr->ULN=utmn;
}
