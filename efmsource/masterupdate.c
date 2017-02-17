/* MasterUpdate() - Updates global section structures with new well/pad information, dynamic versions of the build-out designs
(decrements the no. of wells left to establish), and other structures used to track well/pad establishment each year.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include "efm.h"
#include "geo.h"
#include "plss.h"
#include "TBHA.h"


void	MasterUpdate(int auproj,int sid,int tlistid, float area,int proj)
{

	int		j,au;
	char	tr;
	void	StoreSpec(int,int,int,int);

	/* Update priority scheme */
	if(SECGptr[sid].priority==4)SECGptr[sid].priority=3;

	/* store spec use */
	au=AUSptr->aulist[auproj];
	StoreSpec(au,proj,(int)TList[tlistid].acre,TList[tlistid].wells);



	/* update grand totals in FILptr */
		FILptr[auproj].maxwells-=TList[tlistid].wells;
		/* if(FILptr[auproj].maxwells<0)FILptr[auproj].maxwells=0;  */
		FILptr[auproj].BHOLE-=(TList[tlistid].acre * (float)TList[tlistid].wells);  /* decrement the total amount of Bhole area to consume for this au x proj combo */


		/* new track version */
		TrackBptr[auproj].thetotal-=(TList[tlistid].acre * (float)TList[tlistid].wells);
		TrackBptr[auproj].thisyr -=(TList[tlistid].acre * (float)TList[tlistid].wells);





	/* update secstr3.in info  - assume everything is active.  Also assuming that all pads are NEW */
	SECptr[sid].tpads++;SECptr[sid].apads++;SECptr[sid].twells+=TList[tlistid].wells;
	SECptr[sid].awells+=TList[tlistid].wells;
	/* Need to deal with pad area and surface area - 	
	float	padarea;  total area of pads 
	float	surface;   beginning surface area - pad area; i.e., developable area without pads */

	/* update the auxproj tallies in secstr3.in for this section */
	tr=1;
	for(j=1;j<=SECptr[sid].combos;j++) {
		if(SECptr[sid].SEptr[j].au==AUSptr->aulist[auproj] &&
			SECptr[sid].SEptr[j].proj==proj ) {   /* use proj instead of projlist  - to accommodate project combos within an AU */
				tr=0;
				SECptr[sid].SEptr[j].tpads++;SECptr[sid].SEptr[j].apads++;
				SECptr[sid].SEptr[j].twells+=TList[tlistid].wells;
				SECptr[sid].SEptr[j].awells+=TList[tlistid].wells;
				/* pad area ? */
				SECptr[sid].SEptr[j].BHA=area;  /* remaining BHarea */
		}
	}
	if(tr) {printf("ERROR, couldn't find auproj in masterupdate\n");exit(-1);}

}

