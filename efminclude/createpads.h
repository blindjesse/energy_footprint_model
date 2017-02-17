int		maxid;		/* maxid and should be total number of pad patterns */



typedef struct PT{
	double		utmn;
	double		utme;
}PT;


typedef struct PAT{
	int			vertices;	/* number of vertices in STRUCT PT */
	struct		PT	*aptr;	
	double		displn;		/* centroid displacement.  to adjust, centroid + displacement. */
	double		disple;

}PAT;
PAT		*Pptr;
