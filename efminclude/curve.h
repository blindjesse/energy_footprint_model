/* Variable and structures used in the Curve() function.
*/

int		*patchid;  /* holds patch id of unique area patches - for CURVE */
int		 maxpid;  /* max patchid - set in ReadAreaPatches() */

/* temp storage of distances */
typedef struct CHECKG{
	float	val[2];
}CHECKG;
CHECKG		*checkgrid;

int		MINR,MAXR,MINC,MAXC,NEWROW,NEWCOL;

/* used in sort */
float	*amat;
int		*bmat,*dmat;
