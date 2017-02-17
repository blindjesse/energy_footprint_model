/* Global structures used to store road end-point information.
*/
typedef struct STORE1{   /* indexed by RD ID  - stores pad info to output to storeit*/
	int		PID;
	int		centroidr;
	int		centroidc;
}STORE1;
STORE1		*store1;


int			maxrdend;  /* set in init and reinit; incoming no. of max rd segments.  This is incremented in a sim */
int			MAXRDEND;  /* a constant and used as a displacement throughout */

int			irecord,*record;  /* used in checkrdactive.x */