/* Global structures used to store PLSS section information.
*/

typedef struct SECTION{		 /* indexed by sequential coded sections; stores the GRTS pts  for each section */
	int		cnt;			/* total number of pts in this section combo */
	int		counter;        /* dynamic counter for storing info into n & e.  Decremented
							as coords are used.  RESET to cnt in REINIT */
	int		spacelimit;		/* # of pts not usable due to spacing.  RESET to 0 in REINIT */

	float	*n;				/* pairwise record of the coords for the section combo */
	float	*e;
	float	*nb;			/* stores n before shuffling */
	float	*eb;			/* " */
	char	*avail;			/* =0 if available, else 1=used .  RESET to 0  in REINIT for the MONTE */
	char	*space;			/* =1 if not available due to spacing.  RESET to 0 in REINIT */
}SECTION;
SECTION		*SECPTSptr;



typedef struct SECGINFO{  /* indexed by section code - general info about the section.  */
	int		*neigh;		/* the 8 neighbor SID2 codes */
	int		lease;  /* integer percent of section with leases */
	int		fed0;  /* integer percent of section with no mineral rights */
	int		fed1;  /* " with federal mineral rights */
	int		fed2;	/* " with other mineral rights */
	float	prob; /* maxent probability */
	float	ha1; /* total size of section in ha */
	float	ha3; /* size of developable surface area in ha */
	float	nlha; /* ha of nolease */
	float	propnl; /* proportion of total area with nl */
	char	priority;   /* used to spread out development.  See SelectSections();  reinit resets to 0  */
}SECGINFO;
SECGINFO	*SECGptr;


typedef struct SECPAD{ /* indexed by SID2 code */
	int		num;		/* original number of pads */
	int		counter;    /* incremented as more pads are added - reset to num in REINIT */
	int		*padid;     /* allocated to a max of 200 in readsecpad.c - counter determines valid padid[1 to counter] entries */
}SECPAD;
SECPAD		*SECPptr;
int			MAXSECPAD;  /* = 200 */


typedef struct SECAP{ /* indexed by combos */
	int		au;		/* the au of this combo */
	int		proj;	/* the project of this combo */
	int		tpads;  /* total no. of pads */
	int		apads;  /* no. active pads */
	int		ipads;  /* no. of inactive pads */
	int		twells;  /* total no. of wells */
	int		awells;  /* active wells */
	int		iwells;  /* inactive wells */
	float	padarea;  /* total area of pads */
	float	BHC;  /* bottom hole area consumed in acres */
	float	BHA;  /* bottom hole area remaining in acres */
	char	avail; /* 1 if avail, else 0; set in dopads3 and REINIT to 0; NOT IMPLEMENTED ???? */
}SECAP;
typedef struct SEC{ /* indexed by coded section number.  stores info about each section - problematic in that
					section info is not divided into zones (e.g., where a zone dissects
					a section??) */
	int		combos;  /* number of au x proj combos that this sid belongs to*/
	int		tpads;  /* total no. of pads */
	int		apads;  /* no. active pads */
	int		ipads;  /* no. of inactive pads */
	int		twells;  /* total no. of wells */
	int		awells;  /* active wells */
	int		iwells;  /* inactive wells */
	float	padarea;  /* total area of pads */
	float	surface;  /* beginning surface area - pad area; i.e., developable area without pads */
	struct SECAP  *SEptr; /* allocated by no. of combos */
}SEC;
SEC		*SECptr,*SECBASEptr;  /* SECptr is the dynamic record - SECBASEptr stores original info for MONTE -
							  SECptr info is set to SECBASEptr in REINIT */

int		maxsec;  /* max access no. sections */



typedef struct SECTLIST{ /* indexed by auxproj accession */
	int		*sid2; /* allocated by nsection[j], where j is auproj accession */
	int		*proj;	/* original project - needed to track sections in au combos */
}SECTLIST;
typedef struct AUSECTLIST{  /* stores au x project sections.  there is only 1 list */
	int		aus;  /* total no. of AUs */
	int		projects;  /* total no. of projects */
	int		auproj;  /* number of au x project combos */
	int		*aulist;  /* allocated by auproj; stores the au, the project, and nsections for each auproj by accession */
	int		*projlist;	/* " */
	int		*nsection;	/* " */
	struct  SECTLIST *SLIST;  /* allocated by auxproj */
}AUSECTLIST;
AUSECTLIST		*AUSptr;


/* TList is set for each au x proj each time step to store info about specs.
WEIGHT is the same, but for available sections in a au x proj each time step */

typedef struct TLIST{  /* used to evaluate well establishment given bottom hole spacing availability and need.  
					   Allocated in INIT; Set in Dopads3()  */
	int			auproj;  /* the au x proj combo code - only set in TLIST[1] */
	int			validn;  /* number of TLIST elements used for this spec; only set in TLIST[1] */
	int			twells;  /* annual allotment based on all specs - decremented and set only in TLIST[1] */
	float		BHa;     /* annual allotment of BH area to consume - decremented and set only in TLIST[1] */
	int			num;  /* the ith entry in FILLR */
	int			wells; /* no. wells per pad */
	float		acre;	/* acre spacing of a well */
	int			numwells;   /* annual no. of wells to establish for this spec - decrement as they are established.  NOT USED IN THIS VERSION */
	int			padpat;  /* pad pattern */
	char		horizontal;  /* 1 if horizontal - special processing, else 0 */
	int			proj;	 /* original project no. used especially for combos */
	int			padn;  /* no. of pads given the no. wells per pad  - decrement as they are established. NOT USED IN THIS VERSION*/
 }TLIST;
TLIST			*TList;   /* TList is set to 30 elements in Init, and is recycled in DoPadsOpt ?? */

int				MAXTL;  /* max allocation of TList */
int				*try;   /* holds entry into valid TList specs for the section at hand  - init in INIT to MAXTL */


typedef struct WEIGHT{ /* allocated in INIT, set in SELECTSECTIONS.  weights each available section.
					   hardcoded max of n=MAXWEI.  After INIT(), however, there is a check to determine if
					   MAXWEI needs to be larger. */
	int		sid;
	int		num;  /* number of entries; only set in WEIptr[1] */
	int		remain;  /* no. available - decremented as they are used up */
	int		proj;    /* the original projects of this section  - used esp. for combos */
	float	weight;  /* sum of weights */
	float	relmax;  /* the weight as a proportion of the possible max value */
	float	cumu;
	char	used; /* =1 if used, else 0 */
}WEIGHT;
WEIGHT		*WEIptr;

int			MAXWEI;   /* max allocation of WEIptr - set in init and used when dealing with WEIptr */



/* A Global struct to hold tallies of no. of wells per au x proj each time step - INIT it readaulist, 
initialized in Perform() which is called to output to sumrates */
typedef struct GWELLS{  /* indexed for different storage requirements  */
	int		wells;
	int		pads;
	float	area;   /* used in GPStore to dynamically store BHA */
}GWELLS;
GWELLS		*GStore;  /* indexed by no. of au x project combos */
GWELLS		*GPStore; /* indexed by no. of actual project areas */

char		clusteron;  /* =1 if we use priority code for weighting; else 0.  set,reset in Dopads3() */
int			WEOPT;   /* global option code to indicate section weighting option -SelSecOpt() */
char		*PRIOR,SET;  /* indexed by sid2; =1 if a priority section; SET =1 when a priority section (in expand.in) is
						 included in the list of sections & you want to use the Ordered wgts; else SET=0. */