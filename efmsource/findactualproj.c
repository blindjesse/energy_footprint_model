/* FindActualProj() - Returns the actual project code give the section SID (
section ID).  A synthetic AU x PROJ combo is assigned the next available project number (accession number) even though
the combo is comprised of 1 or more projects (which are numerically coded).  E.g., a combo may be comprised of
AU 1, Project areas 9 & 10, but 51 is the assigned project number for this combo.  A section, however, will overlap one of 
the underlying project areas.  This project area code (number) is determined and returned
in this function.
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include "efm.h"
#include "plss.h"


int		FindActualProj(int auproj,int sid)
{

	int		i,proj;

	for(i=1;i<=AUSptr->nsection[auproj];i++){
		if(AUSptr->SLIST[auproj].sid2[i] == sid) {
			proj=AUSptr->SLIST[auproj].proj[i];
			if(proj<0 || proj> AUSptr->projects) {printf("ERROR, proj value is not valid\n");exit(-1);}
			return(proj);
		}
	}
	printf("ERROR, could not find proj in FindActualProj() \n");exit(-1);
	return((int)0);

}


