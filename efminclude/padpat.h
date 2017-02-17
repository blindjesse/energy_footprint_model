/* Global structures used to store the digitized pads.
*/
typedef struct PT{
	double		utmn;
	double		utme;
}PT;


typedef struct PATCORDS{	/* boundary vertices of pattern pads */
	int			vertices;	/* number of vertices in STRUCT PT */
	struct		PT	*aptr;	
	double		displn;		/* centroid displacement.  To adjust, centroid + displacement. */
	double		disple;

}PATCORDS;
PATCORDS		*PATCptr;



/* Pad Patterns - the ID is relative to the set of template pads used to generate the patterns.  ID is NOT
necessarily related to the incoming pads or the master pad file */
typedef struct PADPAT{ /* index by PAD ID which carrys over from the template processing */
	int		r;	/* no. of rows */
	int		c;  /* no. of cols */
	char	*aptr;	/* rectangle containing the pattern */
}PADPAT;
PADPAT		*PADPptr;


typedef struct SEQPAT{  /* stores info about the pattern sequences.  Can have
						multiple sequences - e.g., 1 for average sized pads,
						2 for large pads.  Index by no. of sequences (at least 1) */
int		beg;			/* this the smallest pattern code for the sequence */
int		end;			/* this is the largest pattern code for the sequence */
int		delta;			/* end - beg + 1 */
}SEQPAT;
SEQPAT		*SEQptr;

int			numpat;		/* max ID of pad patterns in the pallate (can be the same as no. of patterns or NOT, but should be? */