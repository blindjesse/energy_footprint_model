/* DataRecord() - Determines if a road is not longer active via a call to CheckRdActive(),
then resets the file croads.  croads is obsolete but remains active; shold be removed in subsequent code clean up. 
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include "efm.h"
#include "geo.h"
#include "plss.h"


void	DataRecord()
{

	int			i,j;
	PADINFO		*ptr;
	float		bh;
	FILE		*fnam;
	char		cmd[255],nam[255],number[255];
	void		CheckRdActive();




	CheckRdActive();
	fclose(croads);

	croads=fopen("croads","w");
	fprintf(croads,"N,E,ID,Year,Active\n");
}


