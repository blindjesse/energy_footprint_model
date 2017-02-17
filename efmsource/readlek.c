/* ReadLek() - Read/store sage-grouse lek points(UTM) and create a gridded map of these pts.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include "efm.h"
#include "geo.h"

extern	FILE	*themap;
extern	int		row,col;
extern	float	utmn,utme,grain;

void	ReadLek()
{

	int		ret;
	char	value1[255],value2[255];
	float	n,e;
	int		r,c;
	char	ConvToRC2(float,float, int *, int*),tr;
	long long	indexl;



		/* data are e and n */


		lek_n=0;
		ret=255;

		while(ret!=EOF) {
			ret=fscanf(themap,"%s %s\n",value1,value2);
			if(ret!=EOF) {
				lek_n++;
			}
		}
		rewind(themap);



		LEKptr=(LEK *) calloc(lek_n+1,sizeof(LEK));
		LEKSptr=(LEKSTORE *) calloc(lek_n+1,sizeof(LEKSTORE));

		lek_n=0;ret=255;
		while(ret!=EOF) {
			ret=fscanf(themap,"%s %s\n",value1,value2);
			if(ret!=EOF) {
				lek_n++;
				if(lek_n>1250) {
					printf("Warning, LEK # >1250\n");exit(-1);
				}
				n=atof(value2);e=atof(value1);
				LEKptr[lek_n].n=n;LEKptr[lek_n].e=e;
				n=n - (.5*grain);e=e + (.5*grain);	/* places coords in center of a grid cell */
				tr=ConvToRC2(n,e,&r,&c);
				if(!tr) {
					printf("in readlek %f %f %d %d\n",n,e,r,c);exit(-1);
				}
				indexl=(long long)r * (long long)col;indexl+=(long long)c;
				lek[indexl]=lek_n;		/* indicate lek center using lek number*/
				LEKptr[lek_n].r=r;LEKptr[lek_n].c=c;
			}
		}

		fclose(themap);

}
