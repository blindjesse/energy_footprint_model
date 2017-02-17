/* Global structures related to requests to reduce well development intensity. Largely
obsolete but retained until decision is made to enhance or eliminate.
*/

/* struct for holding well reduction requests */
typedef struct REDUCE{  /* for each year of the simulation */
	int		time;	/* the year */
	float	nwells;	 /* total number of wells for this time period */
	float	BHA;     /* another way to adjust annual allotment - this is total BHA for the year.  Derived using only well reduction to estimate BHA adjustments. */
}REDUCE;
REDUCE		*REDptr;

char		reducetrue;  /* 1 if using reduction, else 0 */
char		reducetrueBHA;  /* 1 if using BHA reduction, else 0 */
char		*notenforced;  /* indexed by auproj code max and contains a 1 for the codes that do not have reduction */
float		rfactor;  /* this is the reduction factor  - set in DeriveReduce() */
float		BHAfactor; /* reduction factor based on BHA targets - set in DeriveReduce() */
