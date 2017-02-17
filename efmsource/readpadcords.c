/* ReadPadCords() - Read/store the relative coordinates of the digitized oil/gas pads.
Pad patterns are stored in palettes, where each palette corresonds to a different 
no. of wells/pad. By default, palette number 1 has the smallest pads [1-2 wells/pad], 
no. 2 has the next larger pads [3-5 wells/pad], etc...  */

#include <stdio.h>
#include <math.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include "efm.h"
#include "padpat.h"

FILE	*themap;

void	ReadPadCords()
{

	char	value1[255],value2[255];
	int		id,ret,count,i,max;
	PT		*ptr;


	/* Process the template vertices */
	fscanf(themap,"%d\n",&max);

	if(max!=numpat) {printf("ERROR, numpat and maxid != %d %d\n",numpat,max);exit(-1);}


	PATCptr=(PATCORDS *) calloc(numpat+1,sizeof(PATCORDS));
	if(PATCptr==NULL) {printf("ERROR, PATCptr==NULL\n");exit(-1);}


	/* format of padbnd.cor  - id, count, n and east displacement to add to padpattern centroid */
	ret=255;
	while(ret!=EOF) {
		ret=fscanf(themap,"%d %d %s %s\n",&id,&count,value1,value2);
		if(ret!=EOF) {
			PATCptr[id].vertices=count;
			PATCptr[id].displn=atof(value1);PATCptr[id].disple=atof(value2);
			PATCptr[id].aptr=(PT *) calloc(count+1,sizeof(PT));
			ptr=PATCptr[id].aptr;
			if(ptr==NULL) {printf("ERROR, ptr==NULL\n");exit(-1);}
			for(i=1;i<=count;i++) {
				fscanf(themap,"%s %s\n",value1,value2);
				ptr[i].utmn=atof(value1);ptr[i].utme=atof(value2);
			}/* end of for i */
		} /* if ret */
	}/* while */
	fclose(themap);
}