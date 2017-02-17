/* PrintCoreArea() - Output amount of surface disturbance and the proportion for each sage-grouse core area.  
Used for diagnostics.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include "efm.h"
#include "geo.h"



/* prints out the amount (ha) and proportion of surface distance in each core area */

void	PrintCoreArea()
{
	int		i;

	return;   /* diagnostic of sorts, unless of specific interest this is turned off */

	printf("Sage Grouse Core Area Stats \n");
	for(i=1;i<=MaxCore;i++) {
		printf("%d %f %f\n",i,CAptr[i].ha,CAptr[i].prop);  /* core area ID, surface disturbance (ha), proportion of area */
	}
}
	