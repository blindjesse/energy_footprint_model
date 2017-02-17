/* SetWells() - Initiates establishing pads & wells.
Sets up the evaluation of sections and the random selection of a section for development.
Determines if a well and pad fits the section.  If so, returns
the section information to SimDevelop(). 
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include "efm.h"
#include "geo.h"
#include "padpat.h"
#include "plss.h"

extern	float	idum;


char SetWells(int auproj,int *thesid,int *entry, float *barea)
{

	int		SelectSectR();  /* does the actual selection of sections */
	int		SelectSectO();
	int		sid,j;
	void	GetSecInfo(int,int,int,float *);
	float	area;
	char	MinHole(float,int);
	float	Ran2(float *);



 	j=SelectSectR();  /* j is entry into WEIptr[] - cumu method for selecting sections */


	if(j!=0) {
		sid=WEIptr[j].sid;
		/* get BHole remaining in this section */
		GetSecInfo(sid,auproj,WEIptr[j].proj,&area);  /* get BH info for this section, for this au x proj combo */
		/* figure out if we can stick something in here given the current specs stored in TList */
		if(MinHole(area,WEIptr[j].proj)) {
			*thesid=sid;*entry=j;*barea=area;  /* auproj combo, the section, j entry into WEIptr, BH area */
			return((char)1);
		}else {
			WEIptr[j].used=1;  /* no room for even the min spec, don't make this section available at this point in time */
			WEIptr[1].remain--;
			return((char)0);
		}

	}
	return((char)0);  /* didn't get one this time */
}

/* given the BHA, is there at least 1 of the specs that could fit */
char MinHole(float area,int proj)
{
	int		i;
	float	bh;

	for(i=1;i<=TList[1].validn;i++) {
		if(TList[1].twells>0) {
			if(TList[i].proj==proj) {
				bh=(float)TList[i].wells*TList[i].acre;  /* this is BH area required for 1 pad of this spec */
				if(area>=bh)return((char)1);
				if(TIGHT==1) {if(area+10 >= bh)return((char)1); } /* round up */
			}
		}
	}
	return((char)0);
}

/* randomly select a section */
int	SelectSectR()
{
	float	Ran2(float *);
	double	urv;
	int		i;

	urv=Ran2(&idum);

	for(i=1;i<=WEIptr[1].num;i++) {
		if(urv <=WEIptr[i].cumu && WEIptr[i].used==0) return(i);
	}
	return((int)0);
}

/* If the order of WEIptr was already shuffled in SelectSections (OrderWgts.c) then use this function */
int	SelectSectO()
{
	int		i;

	for(i=1;i<=WEIptr[1].num;i++) {
		if(WEIptr[i].used==0) return(i);
	}
	return((int)0);
}