/* Init()  - Initializes global variables and structures.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include "efm.h"
#include "padpat.h"
#include "geo.h"
#include "padalloc.h"
#include "roads.h"
#include "plss.h"
#include "rdend.h"

extern	float	idum,jdum;

void	Init()
{
	int		r,c,i,j;
	char	nam[255],number[255];

	zz=0;  /* silly counter - for use anywhere */
	idum=-1;jdum=-1;
	rads=3.141592654/180.0;
	padnum=0;			/* master pad accession number */
	padactive=0;		/* no. of active pads */
	padinactive=0;		/* no. of inactive pads */
	dynamactive=0;dynaminactive=0;
	padreveg=0;			/* pads that have been revegetated */
	numpat=0;			/* no. of pad templates */
	newline=0;			/* accession of new roads (lines) - see DrawSLine() */


	
	/* INIT TFILL */
	TFILL[0]=TFILL[1]=TFILL[2]=TFILL[3]=TFILL[4]=0;


	/* static struct for storing road targets that traverse a nontraverse area.
	Set in CheckRd and used in FindNearestCheck().  Hardcoded to avoid free() issues.
	The max possible is checked in CheckRd. */
	OLDRDptr=(OLDROAD *) calloc(6001,sizeof(OLDROAD));
	STORERDptr=(OLDROAD *) calloc(50001,sizeof(OLDROAD));  /* experimental method for storing rds around a section */


	rdwidth = (float *) calloc(12,sizeof(float)); /* width(m) by road type */
	rdwidth[1]=40;		/* primary route - 201 */
	rdwidth[2]=40;		/* " divided lanes - 203 */
	rdwidth[3]=15;		/* secondary route - 205 */
	rdwidth[4]=10;		/* road class 3 -209 */
	rdwidth[5]=10;		/* road class 4 - 210.  THESE tend to be pad roads */
	rdwidth[6]=5;		/* trail - other than 4-wheel drive 211 - the 211s */
	rdwidth[7]=40;		/* ramp in interchange - 402 */
	rdwidth[8]=5;		/* the 211TT stuff */
	rdwidth[9]=10;		/* added, missing roads */
	rdwidth[10]=10;		/* roads created during run time */
	rdwidth[11]=10;		/* spur roads */

 

	coredisturbproportion=0.05;  /* 0.05 */



	MAXWEI=20000;MAXTL=500;MAXSECPAD=200;
	TList=(TLIST *) calloc(MAXTL+1,sizeof(TLIST));
	WEIptr=(WEIGHT *) calloc(MAXWEI+1,sizeof(WEIGHT));
	try=(int *) calloc(MAXTL+1,sizeof(int));
	
	/* max RD segs */
	/* maxrdend=94731; vers4 */  /* need to change this if the number of rds change */

/*	maxrdend=94641;*/  /* vers5.  This is incremented in a sim */
/*	MAXRDEND=94641; */ /* vers5.  This is held constant and used as a displacement constant throughout */

	/* the latest vers 5n */
	maxrdend=94491;  /* vers5n.  This is incremented in a sim */
	MAXRDEND=94491;  /* vers5n.  This is held constant and used as a displacement constant throughout */


	MAXPADWELLS=80;  /* max number of wells on a pad */


checkp=fopen("checkp","w");  /* to check processing */
checkrd=fopen("checkrd","w");  /* to check road processing */

filetfill=fopen("tfillall","a");		/* records tfill frequency */
}