/* StoreSpec()  - Stores design specifications that are used in a simulation.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include "efm.h"


void	StoreSpec(int au, int proj, int BHA, int perpad)
{
	int		i;

	for(i=1;i<=MAXSPECS;i++) {
		if(SPECRECORDptr[i].au==au) {
			if(SPECRECORDptr[i].proj==proj) {
				if(SPECRECORDptr[i].BHA==BHA) {
					if(SPECRECORDptr[i].perpad==perpad) {
						SPECRECORDptr[i].cnt++;
						return;
					}
				}
			}
		}
	}
	printf("Could not find spec in storage\n");
}

void	OutputSpec()
{
	int		i;

	for(i=1;i<=MAXSPECS;i++) {
		if(SPECRECORDptr[i].cnt>0) {
			fprintf(outspecs,"%d %d %d %d %d\n",SPECRECORDptr[i].au,SPECRECORDptr[i].proj,SPECRECORDptr[i].BHA,
				SPECRECORDptr[i].perpad,SPECRECORDptr[i].cnt);
		}
	}
}