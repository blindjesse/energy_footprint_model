/* Global structures for storing pad and well information and build-out designs, FILEs used throughout the model, and 
many of the misc. gobal variables.
*/

typedef struct NWELLS{  /* allocated when PADIptr is allocated - fixed size of MAXPADWELLS which is set in INIT() */
	int		yearbeg;	/* first used */
  	int		yearend;	/* last used */
	int		setend;     /* set to original year - yearend is set to setend in REINIT - is set in readpadinfo(& 2) but not used.... */
	int		au;			/* AU code */
	int		proj;
	int		status;		/* 1=active, 0=inactive */
	float	bottom;		/* bottom hole acreage used (acres) */
}NWELLS;
typedef struct	PADINFO{  /* index by project accession number - global storage of PAD INFO */
	char	active;		/* 1= yes, 2 OR 0 =no  - original - used to initialize activedyn in Monte reps.  The 2 value doesn't seem to be used... */
	char	activedyn;  /* " " , but this is the dynamic field - can change during a sim */
	char	reveg;		/* revegetated; 0=no, 1= yes */
	char	type[255];  /* pad type */
	float	ha;			/* size */
	int		begin;		/* creation yr */
	int		end;		/* last used yr */
	int		mid;		/* master pad ID; if zero then newly created pad */
	int		id;			/* this project's accession id */
	int		duration;	/* yrs in use */
	int		yrssince;	/* yrs since last used */
	int		fmin;		/* majority federal mineral status of pad */
	int		owner;		/*  " owner */
	int		au;			/*  AU  code */
	int		proj;		/* project code */
	int		SID2;
	int		centroidr;	/* centroid row of newly created pad */
	int		centroidc;	/* centroid col of newly created pad - set in PadSize() & used to draw-in polygonal pads */
	int		patID;		/* pattern ID of this pad if created during a simulation - set in PadSize() */
	int		nwells;		/* no. of wells - imported for existing pads; created and stored for new pads in Fill() */
	int		activewells;		/* no. of active wells */
	int		inactivewells;	/* no. of inactive wells */
	float	utme;		/* coords of centroid - imported for existing pads; created & stored for new pads in Fill() */
	float	utmn;
	char	change;  /* set to 1 if info changed */
	struct	NWELLS	*Nwellptr;	
	struct	PADINFO	*next_ptr;
}PADINFO;
PADINFO		*PADIptr,*ENDPADIptr,*THEENDPTR;
PADINFO		*PADIptrBASE,*ENDPADIptrBASE;
int			MAXPADWELLS;  /* max no. of wells on a pad */
int			REUSEPADS;  /*=1 to reuse pads, else 0 */
float		reusepadsprob;  /* program argument - probability of using dead pads */



/* info about core areas */
typedef struct	COREAREA{ /* index by core-area accession no..  Core map should be coded by consecutive accession codes. */
	double		ha;			/* size */
	double		disturb;	/* running tally of surface disturbance (ha) */
	double		prop;		/* proportion of current disturbance */
	double		num;		/* number of cells in the core area */
}COREAREA;
COREAREA		*CAptr;
int				MaxCore;	/* global no. of core areas */



typedef struct FILLRATE{   /* indexed by au x proj accession code.  nextptr is used to store multiple
						   specs for this au x proj accession code. NOTE First entry doesn't use next_ptr,
						   and therafter, use next_ptr[j-1] to access additional specs */
	/* With wlciv152, maxwells and BHOLE pertain to the entire complement of specs for this au x proj combo.  These
	values are only set and decremented in FILptr[1] */
	float		BHOLE;			/* this is bottom hole area to consume in this au x proj - decremented as wells are established  */
	float		BHOLEsave;		/* original BHOLE - used to reset BHOLE in REINIT */
	float		BHOLEsave2;     /* dups BOLEsave, but is dynamic for combos - used to set annual allotment of BHarea */
	int			num;			/* number of different specifications.  Only set in FILptr[1].... */
	int			maxwells;		/* max number of wells for this spec - decremented as wells are established */
	int			setmaxwells;	/* original no. of maxwells - use this to reset maxwells in REINIT */
	int			setmaxwells2;   /* see BHOLEsave2 */
	int			numwells;		/* no. of wells per year for this spec */
	float		prob;		/* used when no. of new pads per year is <1 */
	float		acre;		/* This is bottom hole spacing (acres) for this well/pad combo, e.g., 10, 5, 40, 320  */
	int			wells;		/* no. of wells on a pad of this type */
	int			padpat;		/* padpattern sequence to use for the zone.  E.g., 1 is the first 
							list in the padpatterns, 2 = the second.  Gotta make
							sure padpatterns and this info match - padpattern needs to contain
							the number of sequences specified in fillr.v# */
	char		horizontal;  /* indicates a horizontal well - special processing */
	char		puse;		/* this is the annual probability of using this build-out spec; stored as char - (char)(probability *10); 0 means not used, 1 all the time;
							else puse is evaluated each time step to determine if the spec is used that time step. */
	int			start;		/* calendar year interval (incl.) for these specs to be active */
	int			stop;
	int			proj;		/* specifies the original project - needed when dealing with combos */
	char		combofirst;	/* a generated combo and set to 1 to indicate never has been used; to 2 to indicate it has been used; reset to 1 in REINIT  */
	int			*list;		/* for combos; used in FILptr[1] - indexed by AUSptr->auproj+1; if entry==1, then included in this combo */
	struct	FILLRATE	*next_ptr;   /* For each au x proj, you can have multiple pad types,spacing, patterns etc... */
}FILLRATE;
FILLRATE		*FILptr;		

typedef struct FILSTORE{ /* accession storage of the list of fillr specs */
	float		prob;  /* annual probability of use */
	int			n;     /* number of FILSTORE allocations - only set in FILSTptr[1] */
	FILLRATE	*PTR;
}FILSTORE;
FILSTORE		*FILSTptr;
int				nfillers;   /* no. of filler specs - set to zero if only 1 set of specs */


typedef struct SPECRECORD{ /* accession of specs; used to produce specs# output.  Used to record no. of times a spec (in an au x proj) is used */
	int		au;
	int		proj;  /* actual proj */
	int		BHA;   /* bottom-hole area in acres */
	int		perpad;  /* no. per pad */
	int		size;  /* size code */
	int		cnt;  /* the number of times this spec is used */
}SPECRECORD;
SPECRECORD		*SPECRECORDptr;
int				MAXSPECS;  /* max no. of specs */


char		TIGHT;      /* =1 if we evaluate new pad spacing in CheckNear() [dec 2015 mods] based only on similar AU x Proj combo; else 0 evaluates
						spacing based on any and all pads */
char		CURVEACTIVE; /* =1 if we use the Curve proc for roads; else 0 */
float		*rdwidth;	/* init and set in init - width of road types in meters */
int			time;		/* year of sim, e.g., 2010; also used to set begin yr in PADINFO = 2009 + yr of sim */
float		rads,ha;
int			duration;	/* sim duration */
int			loop;		/* records the time interval of the loop in duration units - i.e., loop=1, loop=2.  Used to set ROADANCptr entries */
long long	newline;	/* accession counter for tagging pts of new roads (lines) - used in DrawSLine() & other road drawing functions */
double		aexp,bexp;	/* coefficients of +exponential equation used to determine reveg  - set in Init() */

double		totalsagearea;  /* ha of sagebrush steppe in the project */
int			monte,repnumber; /* monte is the no. of reps - 23rd argument; repnumber is used inside to est file names */

double		wellscumu,padscumu;  /* used in Perform to record cumulative nos. of NEW pads and new wells */

int			datarecord;  /* the time interval for recording pads and rds  - to produce time-step maps of infrastructure */
float		SECTselect;  /* prob (x) of using cumu method in selecting sections.  1-x = prob of using ranked order  method */

int			POUT;  /* No. of auxproj combos output to sumrates- POUT is set in Perform() */

char		SECTIONRD;  /* Used in Establish and Locate/NextStep.  Is set to 1 in Establish for each section being evaluated.  If there are no valid roads
						to pts in a section, then the first call to Locate (which evaluates all pts) will result in SECTIONRD =1 which will
						terminate evaluation of the remaining specs for the focal section and for the current time period.  
						This prevents evaluation of the suite of specs when in fact roads can not be created.  If core area restrictions or
						pad size prevents establishment of a pad in the first call to Locate, SECTIONRD is set to 0 to allow
						evaluation of all remaining specs. */
int			STARTREP;   /* argument specifying the first value of repnumber - typically set to 1 but can be used to supplement 
						an existing set of runs.  Need to use different random number seed to ensure unique reps. */
int			TFILL[5];  /* tracks frequency of fillr */
int			FILLmax[5]; /* used in CheckFIll */


FILE		*linesf;	/* records pts of new roads */
FILE		*padpatID;		/* records pad pattern ID and centroid UTMs of newly created pads */
FILE		*dumppads;		/* used in dumppads.c */
FILE		*mrecord;   /* records active pads and wells before and after deactivation */
FILE		*ratef;     /* stores info about rate of pad and well establishement  - used in Perform */
FILE		*croads;   /* " " for rdlines#.csv */
FILE		*endpts;   /* local and dynamic copy of rdendpts.in */
FILE		*outspecs;  /* no. of times each spec is used */
FILE		*fileBH;    /* new 2016 file to store BHA estimates for each auproj */
char		RDENDnam[255];  /* stores name of incoming rdendpts.in. Used in openoutputfiles */


int			zz;		 /* a general var */
char		flagit;  /* a general flag */

FILE		*checkp;  /* to check processing */
FILE		*checkrd;  /* to check CheckRd processing */
FILE		*filetfill;
