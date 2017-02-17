/* Global structure used to track well/pad establishment for reporting purposes.
*/

typedef struct TRACK{ /* by auproj comnbo */
	char	active; /*=1 if actively used */
	float	total;  /* targeted sim duration total BHA */
	float	peryr; /* targeted BHA per yr */
	float	thisyr; /* BHA this yr to established; can increment if last yr's amount was less than peryr */
	float	thetotal; /* set to total in DeriveBHA, then decremented as wells are established - running total */
}TRACK;
TRACK		*TrackBptr;

