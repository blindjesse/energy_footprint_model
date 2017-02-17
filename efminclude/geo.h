/* Pointers and structures used to store most of the ingested geospatial data layers.
*/


int					padnum;			/* master number of pads - used to add unique ID to new pads */
long long			padactive;		/* no. of active pads */
long long			INITIALACTIVE;  /* saves padactive value for the initial landscape - used in ReInit */
long long			INITIALINACTIVE; /* initial padinactive value - " */
long long			padinactive;    /* no. of inactive pads */
long long			dynamactive,dynaminactive;   /* dynamic count of active and inactive pads */
long long			padreveg;		/* no. pads that have been revegetated */


int					*flow;			/* DEM */


int					*nontraverse;		/* special layer indicating areas that can not be traversed by new roads */
int					*rds,*pads;
int					*rdsid;			    /* ID codes of roads */
char				*sage;				/* sagebrush, bareground % cover */
short				*SGcore;			/* sage-grouse core area ID */
char				*fedmin,*surf;		/* Federal mineral rights - 0 - off limits, 1 - ALL, 2 - None;
											surf = surface ownership; 1- BLM, 2= Private, 3 = State */
int					*lek;				/* indicates occurrence of a lek based on pt data */
int					*lekperim;			/* the new lek perimeter grid info */
int					lek_n;				/* no. of lek pts */
typedef struct	LEK{ /* stores lek info  - indexed by number of leks */
	int			r;		/* input coords translated to r & c */
	int			c;
	float		n;		/* coords of lek center */
	float		e;
}LEK;
LEK		*LEKptr;


typedef struct LEKSTORE{ /* allocate lek_n+1 at initiation */ /* holds the lek accession number relevant to a SG core area pad - used to evaluate the 5% disturbance rule */
	int		no;		/* no of sequential entries to process  - set in LEKSptr[1] only..  */
	int		lpt;    /* the accession number of the lek pt */
}LEKSTORE;
LEKSTORE		*LEKSptr;

 long long			maxcnt;	 /* row * col */

 double				coredisturbproportion;  /* proportion of disturbance allowed in SG core area */




