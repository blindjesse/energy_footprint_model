/* Global structures used to create the virtual 10-m road grid.
*/
typedef struct RDNODE{  /* indexed by seg no. */
	int		count;  /* no. of nodes */
	int		padid;  /* this is a primary overlapping pad.  If overlap with >1 pads, then many is set */
	char	many;  /* if overlap with >1 pad, them many =1 */
}RDNODE;
RDNODE		*RDNptr;


typedef struct PADRD{  /* indexed by padid */
	int		rdid;
	char	many;  /* if overlap with >1 road, then many=1 */
}PADRD;
PADRD			*PADRptr;


int			row10,col10;
float		grain10;
float		utmnup10,utmnlo10,utme10;
int			*grid10;
char		RDVERTSin[255];  /* global name of rdverts.in */


/* deactivation logic:

Step1: When deactivating a pad, if many==0 then rdid is the only road connected to the pad.  Goto step2.

Step2: If RDNODE[rdid] only is connected to one other road, then count=1; if there
is only 1 pad connected by rdid, then many=0; else if count>1 and many==1 DONOT deactivate.
There is a problem if there is another pad overlapping the node of a road that otherwise only
has 1 pad at the other end of the road.  In such a case, the road is likely not deactivated. */
