/* Global structures used to store information on initial roads, interim structures to store road paths, and
structures to store final, new road locations and vertices.
*/

typedef struct RDSTORE{ /* indexed by sequential number */
	double		utmn;
	double		utme;
}RDSTORE;

typedef struct ROADANCPTR { /* effectively indexed by yr, but with index=1 being the input anchor pts */
	struct  RDSTORE		*aptr;
	int					entries;		/* no. of anchor pts in this struct */	
}ROADANCPTR;
ROADANCPTR		*ROADANCptr;


typedef struct ROADSTOREN{  /* sequential index - temp struct used in UpdateAnchorPts() */
	float		utmn;
	float		utme;
	int			code;
	long long			id;			/* row *col index */
} ROADSTOREN;
ROADSTOREN		*Newptr;


typedef struct	RDS{  /* used in drawline routines - temp storage - cleared at the end of DrawLinemp() */
	int			r;
	int			c;
	char		hit;		
	char		quad;		 
	struct RDS			*ptr;
	struct	RDS	*nextptr;
}RDS;
RDS			*RDptr,*ENDRptr,*NEWrd; 


typedef struct OLDROAD{ /* used in checkrd() and findnearestcheck().  Stores previously selected rd pts
						that can not be used.  FindNearestCheck() skips over these pts during its search.
						Hard coded max = 6000.  Use OLDRDptr[1].no to set the number of entries each time this struct is
						used */
	int		no;
	int		sid;		/* sid is set only in [1]; required for UpdateRdStore() to function properly */ 
	int		r;
	int		c;
	float	n;			/* n & e; used in STORERDptr; set in FindRdCells() */
	float	e;
	char	*list;   /* stores 1 for pts that cross nontraverse areas.  indexed by ptk which is dynamic for each section.
					 ptk is the seqeuntial order of GRTS pts in a section. */
}OLDROAD;
OLDROAD		*OLDRDptr,*STORERDptr;  /* STORERDptr is used in New method to store roads around a section  - set to 10001 in init(). */
char		FORCEDRDflag;  /*=1 if we had to hunt down an acceptable road target.  Means use the following 2 vars
						   for the r & c of a road target */
int			FORCEDRDr;
int			FORCEDRDc;


typedef struct TEMPRD{  /* row * col index - see NOTE below */
	float		utmn;
	float		utme;
} TEMPRD;
TEMPRD			*TEMPRptr;

typedef struct RDVERTS{ /* used as interim storage of vertices for a new line - created to faciliate smoothing.
						Allocated in AssignRoads().  Deallocated after global storage of anchor pts. Set in
						Draw routines.  For now, we allcoate 5000 and see if it works */
	int			type;   /* the road type - set to -1*type in the interim for newbies */
	float		n;		/* new n and e after processrds() */
	float		e;
	int			r;		/* new row and col */
	int			c;
	long long	newline; /* newline # for this application. starts at 1 and icrements for the life of the sim */
	int			time;	/* calendar time */
	int			code;	/* 0 if an anchor pt that can be modified, 1 anchor pt that can not be modified */
	int			action;	/* 2 is stored as an anchor, 1 - do not store */
	long long	index;     /* this is the original row,col index */
	long long	index2;		/* this is the new row,col index  - set in ProcessRds() */
	int			oldr;		/* original row and col */
	int			oldc;
	float		oldn;	/* original n and e */
	float		olde;
	int			anchor;  /* =0 if do not save as anchor, 1 - save.  Based on NEW coordinates and distance to 
						 new cell centroid */
}RDVERTS;
RDVERTS		*RDVERTSptr;
int			NOVERTS;		/* no. of vertices in RDVERTS.  If >=5000, the program stops.. */

/* NOTE - rdmaster and rdindex track anchors of the initial roads and the roads added during a time step.  However,
within a time step, these two pieces of info are not updated (happens after all pads have been established for the time step).  Thus, new 
pads are dynamically burned into the landscape within a time step but roads are not.  The TEMPRD struct is a way to
store road anchors established within a time step.  All anchor pt update and storage logic remains.  TEMPRD is allocated at the 
beginning and eliminated at the end of a time step.  It is used in AssignRoads().  FindNearest was updated to allow
roads = -10 to be recognized (roads = -10 are roads established within a time step and correspond to TANC entries, which are
accessed and stored via rdmaster and rdindex in UpdateAnchorPts).  Thus, TEMPRD entries only pertain to roads=  - 10. */

int				*rdmaster;		/* indexed by row*col, and contains the
								sequential order of the ROADANCPTR ptr.  The original
								road anchor pts are stored in the first ROADANCPTR[].aptr.  
								Every year, a new .aptr is created to store that yrs anchor pts.
								The content of rdmaster indicates the ROADANCPTR pts to use
								for a cell. */
int				*rdindex;		/* indexed by row*col, and contains the sequential number entry into
								one of the RDSTORE allocations for that cell.  rdmaster indicates the
								ROADANCPTR to pick up, then rdindex indicates the sequential entry in that
								ptr where the anchor pt of a cell is located.  Values in rdindex are repeated
								but are unique within the same rdmaster value.  E.g., the original anchors
								are stored in the first RDANCptr; rdindex values could range from 1 to 558,000.
								In the first yr of a sim, the second RDANCptr is used, and corresponding values
								in rdindex may range from 1 to say 500. */
long long				newanchors;		/* no. of new road anchors within a yr.  Init in main at the beginning of a time step. */

FILE			*anchor;		/* temp storage of new anchor pts & is reused every yr */



/* ************ These are used in FindNearest() */
int			nearr,nearc,savetype;
double		roaddistance;