/*SelSecOpts() - Based on the user-selected method, derives weights for a section.  
Weights are summed in SelectSections() and used to randomly select sections for development.

*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include "efm.h"
#include "geo.h"
#include "padalloc.h"
#include "binary.h"
#include "padpat.h"
#include "plss.h"



void	SelSecOpts(int option, int sid,int auproj,float *themaxscore, double *thetotal)
{

	float	maxscore;
	double	total,temp;
	int		k,id;
	int		FindActualProj(int,int );
	char	tr;



	if(option<=0 || option >5){printf("ERROR in SelSecOpts %d\n",option);exit(-1);}

	switch(option) {



	  case(1):  /* oil/gas potential */
		  maxscore=5.535;
			total=0;
			total+= exp(SECGptr[sid].prob*1.7110); /* geophysical prob = 5.535 when prob=1.0 */
			*themaxscore=maxscore;*thetotal=total;
		break;

		case(2):  /* Neighbors only */
			maxscore=8.0; 

			/* check out neighbors */
			temp=0.0;total=0;
			for(k=1;k<=8;k++) {
				id=SECGptr[sid].neigh[k];
				if(SECptr[id].tpads>0) { /* if neighbor has >0 pads */
					temp++;
				}
			} /* for k */
			total=temp/8.0;
			*themaxscore=maxscore;*thetotal=total;
		break;

		case(3):  /* lease and mineral rights */

		  maxscore=2.0;
            total=0;
		    total+=(float)SECGptr[sid].lease/100.0;   /* percentage (integer) of section with leases */
			total+=(float)SECGptr[sid].fed1/100.0; /* percentage (integer) of mineral rights that is federal */
			total+=((float)SECGptr[sid].fed2/100.0);   /* percentage (integer) of mineral rights that is other  */
			*themaxscore=maxscore;*thetotal=total;
		break;

	   case(4):  /* Composite of all criteria */
		  maxscore=15.535;
		  	/* check out the neighbors */
			temp=0.0;total=0;
			for(k=1;k<=8;k++) {
				id=SECGptr[sid].neigh[k];
				if(SECptr[id].tpads>0) { /* if neighbor has >0 pads */
					temp++;
				}
			} /* for k */
			total=temp;

			total+=(float)SECGptr[sid].lease/100.0;   /* percentage of section with leases */
			total+=(float)SECGptr[sid].fed1/100.0; /* percentage of mineral rights that is federal */
			total+=((float)SECGptr[sid].fed2/100.0);   /* percentage of mineral rights that is other  */
			total+= exp(SECGptr[sid].prob*1.7110); /* geophysical prob = 5.535 when prob=1.0 */
			*themaxscore=maxscore;*thetotal=total;
		break;

		case(5):  /* All sections have equal prob. of selection */
			maxscore=1;total=1;			
			*themaxscore=maxscore;*thetotal=total;
		break;

	} /* end of switch */
}