
/* Curve() - Derives long, convoluted roads.  Called whenever a relatively straight line between a pad and existing road
is not possible due to crossing nontraverse areas.

1) r,c (row, column) is the pad location; rr and cc is a road cell.

2) Donut() uses concentric squares to find the closest road (Euclidean).  There is some padding in this function
   to extend the search.  The Euclidean closest may not be the actual closest due to obstructions.  Searches occur only within the
   area patch id (patchid[]) of the pad location. 

3) If a road is located, then in Check we determine the largest bounding box for generating distance matrices.  All the
MINR, MAXR stuff pertains to this bounding box.  We use max(delta row, delta col) + 4 * 40 to generate the largest box.
checkgrid is allocated to hold 2 distance matrices in local units; that is, the map coords are translated to the coords of the
largest box - saves on memory allocation.  All the TransS() TransL() calls translate map to local (S) or local to map (L) coords.

4) First generate a distance matrix from the road to the pad - index=0.  Start with a square around the road cell which has dist=1 and
generate the closest euclidean distance from the road cell for each cell.  Increase the surrounding square 1 cell at a time 
and cycle thru all cells to ensure all that can be set are and with the minimum distance.  This continues until the search box (defined by
passed argument to DoIt()) is fully exapnded.

5) A distance value of the pad cell is checked.  If =0, then we expand the search box by 40 cells and repeat #4.  If we still don't have
a distance for the pad cell, we free checkgrid and return 0 to indicate that we can't do a road - life goes on. Else we do #4-5 again from
the pad to the road to generate the second distance matrix - index=1.

6) If we're still afloat, then we double check to see if the pad and road cell have the same Integer distance ((int)largest1 == (int)largest2).
If not, abort.  If so, within the bounding box (checkgrid[]) we determine if both distance matrices have a value >0.  If so, we
determine if the sum of the values is +- 10 of largest1 (could have used largest 2).  If so, the distance and r,c of matrix 2 (index=1) 
are stored in amt, bmat, and dmat.  We do an ascending of amt[] which is the distance.

7) Using the sorted arrays, we first save b[0] and d[0] which are the pad cell coords.  We store the grid cells in structure x which is used
later to generate road coords.  These first 2 coords are noted, then we go looking for the next road cell.  Starting with these coords, we go thru
the sorted arrays from [0] to the end and look for a cell that is an adjacent or diagonal neighbor.  We note the coords of a neighbor as we find one, 
but we overwrite this internal recording as we find another neighbor.  After we exhaust the search, the final neighbor is the one that is the farthest
from the [0] cell.  We store the coords of this neighbor.  Say this neighbor was at entry [5].  Next, we start at [6] and look for neighbors of [5], and again
when we exhaust the search we end up with a neighbor that is the farthest from entry [5], say it was entry [10].  Next, we start at [11] and
again look for neighbors, etc...   We continue this process until we end up picking up the road cell.  However, along the way if we happen to hit
a valid road cell or are next to a valid road cell then we use that road cell as the closest road and stop processing.  Cause Donut() doesn't
really pick up the closest accessible road, we'll have situations where we run across closer roads before getting to the targeted road cell - o well.

8) Subsequent processing uses existing modules to generate the center-line vertices. 

*/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include "geo.h"
#include "roads.h"
#include "curve.h"

extern	int		row,col;

/* n, e, r, and c are coords of the pad location, numcells = number of verts, dn de are for the final road cell */
char	Curve(int r, int c, int *numcells, float *dn, float *de)
{

	int		thepid,rr,cc,fr,fc,cells;
	char	Donut(int,int,int,int *,int *);
	char	Check(int,int,int,int,int,int *, int *,int *);
	void	ConvToPtsF(int,int, float *, float *);
	float	tn,te;
	

	printf("Curve\n");

	thepid=patchid[r*col+c];


	/*  find a road */
	/* printf("rc %d %d %d\n",r,c,thepid); */
	if(Donut(r,c,thepid,&rr,&cc)) {
		/* r & c are the pad origin, rr and cc is the road anchor;
		Find a path between these coords */
		/* printf("rr cc %d %d %d\n",rr,cc,thepid); */

		/* Don't think we need this.   
		ConvToPtsF(r,c,&sn,&se);
		ConvToPtsF(rr,cc,&tn,&te);
		Trig(sn,se,tn,te,&thequad,&theangle,&thecompass,&dist);
		printf("%d %f %f %f\n",thequad,theangle,thecompass,dist);  */

		if(Check(r,c,rr,cc,thepid, &fr,&fc,&cells)) { /* fr,fc is the FINAL road cell - can be different from rr,cc */
			*numcells=cells;ConvToPtsF(fr,fc,&tn,&te);
			*dn=tn;*de=te;
			/* printf("FInal %d %d %d %f %f\n",cells,fr,fc,tn,te); */
			return((char)2);  /* got one! */
		}else {
			return((char)0); /* don't got one */
		}
	} else {
		return((char)0);  /* no road, nothing happened */
	}

}


char	Check(int r, int c,int rr,int cc,int thepid,int *fr,int *fc,int *cells)
{

	char	Doit(int,int,int,int,int,int,int);
	float	largest1,largest2;
	int		i,j,ret,k,l;
	char	value[255];
	int		savei,savej,savecode;
	float	sum,sum1;
	char	DoCheck(int,int,int *,int *,int *);
	int		max,ffr,ffc,cellnum;
	char	number[255];
	int		cnt,incr;
	int		x1,x2;
	char	TransS(int,int,int *,int *);
	char	TransL(int,int,int *,int *);
	void	Sort(float *, int *, int *,int);
	void	ERRORP(int);

	/* 40 is arbitrary - could be 2x? */
	max=abs(rr-r)+40; if(abs(cc-c)+40>max)max=abs(cc-c)+40;

	/* find domain of bounding box */
	x1 = rr - max - 160; x2=r-max-160;
	MINR = x1; if(x2<MINR)MINR=x2;
	x1 = rr + max + 160; x2=r +max+160;
	MAXR = x1;if(x2>MAXR)MAXR=x2;
	x1 = cc - max - 160; x2=c-max-160;
	MINC = x1; if(x2<MINC)MINC=x2;
	x1 = cc + max + 160; x2=c +max+160;
	MAXC = x1;if(x2>MAXC)MAXC=x2;

	NEWROW=MAXR-MINR+1;
	NEWCOL=MAXC-MINC+1;

	/*  printf("new cords %d %d %d %d %d %d\n",MINR,MAXR,MINC,MAXC,NEWROW,NEWCOL); 
	 printf("other %d %d %d %d %d\n",r,c,rr,cc,max);  */


	checkgrid=(CHECKG *) calloc(NEWROW*NEWCOL,sizeof(CHECKG));
	if(checkgrid==NULL){ERRORP((int)1);return((char)0);}
	if(!TransS(rr,cc,&i,&j)) {ERRORP((int)1);return((char)0);}

	checkgrid[i*NEWCOL+j].val[0]=1;  /* set the target to 1 */

	if(!Doit(rr,cc,r,c,thepid,max,(int)0)){ERRORP((int)1);return((char)0);}
	if(!TransS(r,c,&i,&j)) {ERRORP((int)1);return((char)0);}
	largest1=checkgrid[i*NEWCOL+j].val[0];
	/* printf("1 checkgrid of end with max %f %d\n",largest1,max); */

	cnt=4;incr=0;
	if(largest1>0)cnt=0;
	while(cnt>0) {
		cnt--;incr++;
		if(!Doit(rr,cc,r,c,thepid,max+(40*incr),(int)0 )){ERRORP((int)1);return((char)0);}
		largest1=checkgrid[i*NEWCOL+j].val[0];
		/* printf("1 rep/checkgrid of end with max %f %d %d %d\n",largest1,max,cnt,incr); */
		if(largest1>0)cnt=0;
	}

	if(largest1==0){printf("distance matrix1 failed\n");ERRORP((int)1);return((char)0);}


	checkgrid[i*NEWCOL+j].val[1]=1;  /* set the target to 1 */


	if(!Doit(r,c,rr,cc,thepid,max,(int)1)){ERRORP((int)1);return((char)0);}
	if(!TransS(rr,cc,&i,&j)) {ERRORP((int)1);return((char)0);}
	largest2=checkgrid[i*NEWCOL+j].val[1];
	/* printf("1a checkgrid of end %f\n",largest2); */
	cnt=4;incr=0;
	if(largest2>0)cnt=0;
	while(cnt>0) {
		cnt--;incr++;
		if(!Doit(r,c,rr,cc,thepid,max+(40*incr),(int)1 )){ERRORP((int)1);return((char)0);}
		largest2=checkgrid[i*NEWCOL+j].val[1];
		/* printf("1a rep/checkgrid of end with max %f %d %d %d\n",largest2,max,cnt,incr); */
		if(largest2>0)cnt=0;
	}

	if(largest2==0){printf("distance matrix2 failed\n");ERRORP((int)1);return((char)0);}
	if((int)largest1!=(int)largest2){printf("distances differ %f %f\n",largest1,largest2);ERRORP((int)1);return((char)0);}

	/* printf("rr,cc %d %d\n",i,j);
	printf("values %f %f\n",checkgrid[i*NEWCOL+j].val[0],checkgrid[i*NEWCOL+j].val[1]); */

	cnt=0;
	for(i=0;i<NEWROW*NEWCOL;i++) {
		if(checkgrid[i].val[0]>0 && checkgrid[i].val[1]>0) {
			if(checkgrid[i].val[0]<=largest1 && checkgrid[i].val[1]<=largest2) {
				if(checkgrid[i].val[0]+checkgrid[i].val[1] >largest1 - 10 && checkgrid[i].val[0]+checkgrid[i].val[1] <=largest1 + 10)cnt++;
			}
		}
	}
	amat=(float *) calloc(cnt,sizeof(float));	bmat=(int *) calloc(cnt,sizeof(int)); dmat=(int *) calloc(cnt,sizeof(int));

	cnt=-1;
	for(i=0;i<NEWROW;i++) {
		for(j=0;j<NEWCOL;j++) {
			if(checkgrid[i*NEWCOL+j].val[0]>0 && checkgrid[i*NEWCOL+j].val[1]>0) {
				if(checkgrid[i*NEWCOL+j].val[0]<=largest1 && checkgrid[i*NEWCOL+j].val[1]<=largest2) {
					if(checkgrid[i*NEWCOL+j].val[0]+checkgrid[i*NEWCOL+j].val[1] >largest1 - 10 && checkgrid[i*NEWCOL+j].val[0]+checkgrid[i*NEWCOL+j].val[1] <=largest1 + 10){
						cnt++;
						if(!TransL(i,j,&k,&l)) {printf("ERROR L\n");ERRORP((int)2);return((char)0);}
						amat[cnt]=checkgrid[i*NEWCOL+j].val[1];
						bmat[cnt]=k;
						dmat[cnt]=l;
					}
				}
			}
		}
	}

	if(cnt==-1) {ERRORP((int)2);return((char)0);}
	/* printf("Sort\n"); */
	Sort(amat,bmat,dmat,cnt+1);

	/* printf("DoCheck %d %f %d %d\n",cnt+1,amat[cnt],bmat[cnt],dmat[cnt]); */
	if(DoCheck(cnt+1,thepid,&ffr,&ffc,&cellnum)) {
		*fr=ffr;*fc=ffc;*cells=cellnum;
		ERRORP((int)2);return((char)1);
	}else {
		ERRORP((int)2);return((char)0);
	}
}

/* loops from be to er and bc to ec until all distances do not change, then we move onto the next most outer set of cells.  
The size of the bounding box is set by max.  Above, max will increase 4 times (4x 40) to see if we can get there from here. */
char	Doit(int r, int c,int rr, int cc,int theid,int max,int index)
{
	int		i,j,icnt,k,l;
	char	tr;
	char	Valid(int,int);
	float	code;
	char	Neighbor(int,int,int,float *);
	char	TransS(int,int,int *,int *);


	for(icnt=1;icnt<=max;icnt++) {
		tr=1;
		while(tr) {
			tr=0;
			for(i=r-icnt;i<=r+icnt;i++) {
				for(j=c-icnt;j<=c+icnt;j++) {
					if(Valid(i,j)) {
						if(patchid[i*col+j]==theid) {
							if(Neighbor(i,j,index,&code)) {
								if(!TransS(i,j,&k,&l)) {/* printf("ERROR\n");*/ return((char)0);}
								if(checkgrid[k*NEWCOL+l].val[index]==0) {
									checkgrid[k*NEWCOL+l].val[index]=code;tr=1;
								}else if(code <checkgrid[k*NEWCOL+l].val[index]){
									checkgrid[k*NEWCOL+l].val[index]=code;tr=1;
								}
								if(code==0){printf("ERROR, code==0 \n");return((char)0);}
							}
						}
					}
				}
			}
		}
	}
	return((char)1);
}


char	Neighbor(int i, int j, int index, float *code)
{
	char	Valid(int,int);
	double	dist,dists;
	void	ConvToPtsF(int, int, float *, float*);
	float	sn,se,tn,te;
	int		r,c,k,l;
	char	hit;
	int		saver,savec;
	char	TransS(int,int,int *,int *);

	hit=0;
	dists=-1;

	for(r=i-1;r<=i+1;r++) {
		for(c=j-1;c<=j+1;c++) {
			if(Valid(r,c)) {
				if(r==i && c==j) {  /* do nothing with the focal cell */
				}else {
					if(!TransS(r,c,&k,&l)) {/* printf("ERROR\n"); */ return((char)0);}
					if(checkgrid[k*NEWCOL+l].val[index]>0) {
						ConvToPtsF(r,c,&sn,&se);
						ConvToPtsF(i,j,&tn,&te);
						dist=(sn-tn)*(sn-tn);
						dist+=(se-te)*(se-te);
						dist=sqrt(dist);
						if(dists==-1) {
							hit=1;saver=r;savec=c;dists=dist+checkgrid[k*NEWCOL+l].val[index];
						}else if(dist+checkgrid[k*NEWCOL+l].val[index]<dists) {
							hit=1;saver=r;savec=c;dists=dist+checkgrid[k*NEWCOL+l].val[index];
						}
					}
				}
			}
		}
	}
	if(hit==0)return((char)0);
	*code=dists;return((char)1);
}

/*Ascending sort on distance */
char	DoCheck(int inum,int thepid,int *ther, int *thec, int *numcells)
{
	int		i,j,savei,savej,cnt,jkl,start,cells;
	int		savestart,keepi,keepj;
	char	tr;
	int		br,bc;
	char	Valid(int,int);
	char	ValidR(int);
	void	SetPTR();

	cells=0;

	RDptr=(RDS *) calloc(1,sizeof(RDS));
	if(RDptr==NULL){printf("ERROR, RDptr==NULL\n");exit(-1);}
	/* Force first one to be i,j */
	RDptr->r=bmat[0];RDptr->c=dmat[0];RDptr->hit=1;
	RDptr->nextptr=(RDS *) calloc(1,sizeof(RDS));
	ENDRptr=RDptr->nextptr;

	/* first should be distance=1 - the pad cell */
	savei=bmat[0];savej=dmat[0];

	start=1;tr=1;
	while(tr) {
	/*	printf("start %d\n",start); */
		for(jkl=start;jkl<inum;jkl++) {
			i=bmat[jkl];j=dmat[jkl];
			if(abs(savei-i)<=1) {
				if(abs(savej-j)<=1) {
					savestart=jkl;keepi=i;keepj=j;
				}
			}
		}
		start=savestart+1;
		savei=keepi;savej=keepj;
		ENDRptr->r=savei;ENDRptr->c=savej;ENDRptr->hit=1;cells++;
		SetPTR();
		if(start>inum)tr=0;
		if(ValidR(rds[savei*col+savej])) {
			tr=0;  /* if we hit a road before the end, then we have a closer road so stop */
		    /* need to save savei and savej as the NEW or FINAL rr,cc */
			*ther=savei;*thec=savej;
		}

		/* check for a road around this cell */
		if(tr) {
			for(br=savei-1;br<=savei+1;br++) {
				for(bc=savej-1;bc<=savej+1;bc++) {
					if(Valid(br,bc)) {
						if(br!=savei || bc!=savej) {
							if(ValidR(rds[br*col+bc]) && patchid[br*col+bc]==thepid) {
								ENDRptr->r=br;ENDRptr->c=bc;ENDRptr->hit=1;cells++;
								SetPTR();
								*ther=savei;*thec=savej;*numcells=cells;
								return((char)1); /* use this as the rd point */
							}
						}
					}
				}
			}
		} /* if tr */
	} /* while */
	*ther=savei;*thec=savej;*numcells=cells;
	return((char)1);
}


void	SetPTR()
{
	ENDRptr->nextptr=(RDS *) calloc(1,sizeof(RDS));
	ENDRptr=ENDRptr->nextptr;
	if(ENDRptr==NULL){printf("ERROR, ENDRptr==NULL\n");exit(-1);}
}


/* translate from large grid to small grid coords */
char	TransS(int r, int c, int *i, int *j)
{
	*i=r-MINR;
	*j=c-MINC;

	if(*i<0 || *j<0) {
		/* printf("ERROR in TransS\n"); */
		return((char)0);
	}
	if(*i>=NEWROW || *j>=NEWCOL) {
		/* printf("ERROR in TransS\n"); */
		return((char)0);
	}
	return((char)1);
}

/* translate from small grid to large grid coords */
char	TransL(int r, int c, int *i, int *j)
{
	char	Valid(int,int);
	int		k,l;
	k=r+MINR;
	l=c+MINC;
	if(Valid(k,l)) {
		*i=k;*j=l;
		return((char)1);
	}else {
		return((char)0);
	}
}


void	ERRORP(int code)
{
	switch(code) {
		case(1):
			free(checkgrid);
		break;
		case(2):
			free(checkgrid);free(amat);free(bmat);free(dmat);
		break;
	}
}