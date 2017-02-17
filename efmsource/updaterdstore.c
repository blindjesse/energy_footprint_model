/* UpdateRdStore() - Usurps the need to call FindRDCells() after establishing a road in a section (a short-cut that is
possible for only certain conditions).  Called from ProcessRds() after saving the road-anchor points 
(and the setting of rds) of a new road segment.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include "geo.h"
#include "plss.h"
#include "roads.h"

extern	int	row,col;

/* index is the row*col value- need to translate to 30-m row and col */
void	UpdateRdStore(long long index, float n, float e)
{
	char		Valid(int, int);
	int			sid,k;
	int			ii,jj;

	sid=STORERDptr[1].sid;
	if(sid<=0)return;

	if(STORERDptr[1].no<50000){
		ii=index/(long long)col;  /* the row */
		jj=index-((long long)ii*(long long)col);  /* the column */
		if(Valid(ii,jj)) {
			STORERDptr[1].no++;
			STORERDptr[STORERDptr[1].no].r=ii;STORERDptr[STORERDptr[1].no].c=jj;
			STORERDptr[STORERDptr[1].no].n=n;STORERDptr[STORERDptr[1].no].e=e;
			k=STORERDptr[1].no;
			STORERDptr[k].list= (char *) calloc(SECPTSptr[sid].cnt+1,sizeof(char));
			if(rds[ii*col+jj]!=-10)printf("warning rds in updaterdstore !=-10 %d\n",rds[ii*col+jj]);
		}
	}else { /* all full */
		return;
	}

}