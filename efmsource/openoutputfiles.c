/* OpenOutputFiles() - 0pens all output files, and uses replication number as a suffix in most cases.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include "efm.h"
#include "geo.h"
#include "padalloc.h"
#include "file_operations.h"

 

void	OpenOutputFiles()
{
	int		r,c,i,j;
	char	nam[255],number[255];


	strcpy(nam,"rdlines");sprintf(number,"%d",repnumber);strcat(nam,number);strcat(nam,".csv");
	linesf=fopen(nam,"w");		/* DrawSLine & DrawSLineP*/ /* coordinates of new road lines */
	fprintf(linesf,"N,E,RDID,YR,BUFFER,CCODE\n");	/* add header for ease of Arcmap processing.
														CCODE is just 1 and used to quickly rasterize all rds
														without regard to type. */
	croads=fopen("croads","w");  /* a time-interval copy of rdlines */
	fprintf(croads,"N,E,ID,Year,Active\n");

	strcpy(nam,"padpattern");sprintf(number,"%d",repnumber);strcat(nam,number);
	padpatID=fopen(nam,"w");	/* Fill/PadSize - records pattern ID and centroid utms of newly created pads */

	strcpy(nam,"sumrates");sprintf(number,"%d",repnumber);strcat(nam,number);
	sumrates=fopen(nam,"w");		/* records no. of wells/pads by au x proj combos */

	strcpy(nam,"sumratesp");sprintf(number,"%d",repnumber);strcat(nam,number);
	sumratesp=fopen(nam,"w");		/* records no. of wells/pads by project */


	strcpy(nam,"mrecord");sprintf(number,"%d",repnumber);strcat(nam,number);
	mrecord=fopen(nam,"w");		/* records active pad/wells before/after deactivation - used in DeActivateWells */


	strcpy(nam,"rates");sprintf(number,"%d",repnumber);strcat(nam,number);
	ratef=fopen(nam,"w");		/* records active pad/wells before/after deactivation - used in DeActivateWells */


	strcpy(nam,"specs");sprintf(number,"%d",repnumber);strcat(nam,number);
	outspecs=fopen(nam,"w");		/* records active pad/wells before/after deactivation - used in DeActivateWells */


	strcpy(nam,"BHarea");sprintf(number,"%d",repnumber);strcat(nam,number);
	fileBH=fopen(nam,"w");		/* records BHA estimates for each auproj */



	/* deal with rdend pts.in which is stored as RDEND */
  sprintf(nam,REMOVE_IF_EXISTS,"endpts.dat");system(nam);
	strcpy(nam,COPY);strcat(nam,RDENDnam);strcat(nam," endpts.dat");system(nam);
	endpts=fopen("endpts.dat","a");


	/* deal with storeit - stores previous deactivated roads */
  sprintf(nam, REMOVE_IF_EXISTS,"storeit");system(nam);
}
