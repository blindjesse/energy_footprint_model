/* DrawLinemP()- Draws a road between points, guided by least-resistance pathway based on DEM.

Very complex looking, but rather simple.

1)  determine the Quad (1-4) based on i,j to ii,jj.
2)  determine angle and distance - the sin, cos trig was worked out to function properly.
3)  the coords from i, j out to 4 cells (both sides) that are parallel to the vector between i,j & ii,jj are determined.
4)  the run & rise is determined in 1-cell intervals from i,j, to ii, jj (called displ).  This is added
to the origin and to the parallel cell locations.  
5) first, the flow path value adjacent to the origin is evaluated to determine the 'next' cell of the road.  The 'next' cell
is stored and used in the next displ step.  The lowest flow value that is adjacent to a previously selected road
cell is selected as the 'next' cell.  This continues for a distance = displ.  There are instances where the road
doesn't stop adjacent to ii,jj.  Thus, a call to DrawSLinePMOD()[draw straight line] is made to ensure that the road ends 
at ii,jj.  The trig was worked out for deriving the parallel displacements.  The way the rise & run is added to the 
displacements seems to be correct, but could use some additional scrutiny.

If it is a short line (<=3 cells worth) DrawSLineP() is called to generate vertices


DrawSLinePMOD(n,e,dn,de); is called to draw a straight line from the last
cell to the target cell, if necessary.  There are no calls to other draw routines inside of this module.
It updates RDSptr.  RDSptr is used below in DeriveRD() to 
generate vertices - so no need to store anything in RDVERTSptr inside of this module.


if(numcells<=3) DrawSLineP(sn,se,dn,de,(int)10) is called instead of DeriveRD().  DrawSLineP generates
vertices, so calls are made to Record() to update RDVERTSptr[]..  No calls to other Draw routines
are within DrawSLineP.

*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include "efm.h"
#include "geo.h"
#include "roads.h"
#include "rdend.h"


extern	int		row,col;
extern	float	grain;


/* i,j are grid r,c of source; sn, se are utm of source
ii,jj are grid r,c of target; dn, de are utm of target */
void	DrawLinemP(int padid,int sr, int sc, int tar, int tac,float ssn,float sse, float ddn,float dde)
{
	int		dist;							/* distance in no. of cells */
	double		deltar,deltac;
	float	angle;
	int		displ,r,c,quad,k,l;
	float	a,b;
	int		rbotr,rbotc,lbotr,lbotc;
	float	botr,botc;
	int		lastr,lastc,lastf,storer,storec,lastff;
	char	Valid(int,int);
	void	ClearRDS(),ClearRDS2();
	void	DeriveRd(int,float,float,float,float);
	int		numcells;
	void	DrawSLineP(float,float,float,float,int);	/* straight line based on points */
	int		FindNearest(int,int,int, int *, int *),type;
	int		i,j,ii,jj;
	float	sn,se,dn,de,n,e;
	void	ConvToPtsF(int,int,float *, float *);
	void	DrawSLinePMOD(float sn, float se, float tn, float te);
	int		seq,index;
	RDSTORE	*Rptr;
	void	pause();
	void	ProcessRds();
	long long indexl;
	int		ventus;  /* the search window when evaluating the path of a road - no. of cells */
	int		tolerance;  /* added to flow values when checking for least resistance */


	if(sr==tar && sc==tac) return;  /* don't go done that path  - this routine goes boungers when cords are the same. */


	tolerance = 7;
	ventus=2;  /* was set to 4 */

	i=sr;j=sc;ii=tar;jj=tac;sn=ssn;se=sse;dn=ddn;de=dde;   /* translate here to accommodate how we terminate a road */

	a=i-ii;b=j-jj;
	dist = sqrt( (a*a) + (b*b) );
	deltar = fabs((float)i-(float)ii);deltac=fabs((float)j-(float)jj);
	numcells=0;							/* no. of new roads cells  - if <=3, draw a straight line */



	RDptr=(RDS *) calloc(1,sizeof(RDS));
	if(RDptr==NULL){printf("ERROR, RDptr==NULL\n");exit(-1);}
	/* Force first one to be i,j */
	RDptr->r=i;RDptr->c=j;RDptr->hit=1;
	RDptr->nextptr=(RDS *) calloc(1,sizeof(RDS));
	ENDRptr=RDptr->nextptr;


	/* quad 1 */
	if(ii<i && jj>=j) {
		if(deltar==0 ||deltac==0) {
			angle=0;
		} else {
			angle=(float)deltac/(float)deltar;  /* delta east /delta north */
		}
		angle=atan(angle);	

 

		/* displace to the left and to the right a distance of 4 from the origin */
		indexl=(long long)i * (long long)col;indexl+=(long long)j;
		lastr=i;lastc=j;lastf=flow[indexl];lastff=lastf;
		for(displ=1;displ<=dist;displ++) {
			r=displ*cos(angle);r=r*(-1.0);c=displ*sin(angle); /* this is the run & rise along the vector from i,j to ii,jj at 
															  distance = displ */
			storer=storec=-1;
			if(r!=0 || c!=0 ) {
				k=i+r;l=j+c;		/* add the run & rise to the origin - this is the center line or the straight line 
									from i,j to ii,jj */

				/* if the new k & l are adjacent to the previous cell, then if the flow value of this cell
				is less than the previous cell, store the coords of this cell and the flow value.  Otherwise, if
				this is the first cell that is adjacent and its flow value is => than the previous cell, just
				store the coords. */
				if(k!=lastr || l!=lastc) {	/*  If not the same r,c as last time */

					if(Valid(k,l)) {
						if(k>= lastr-1 && k<=lastr+1 && l>=lastc-1 && l<=lastc+1) {
							indexl=(long long)k * (long long)col;indexl+=(long long)l;
							if(flow[indexl]+tolerance <=lastf) {
								storer=k;storec=l;lastf=flow[indexl];
							}else {
								if(storer==-1) {
									storer=k;storec=l;lastff=flow[indexl];
								}else {
									if(flow[indexl]+tolerance <lastff) {
										storer=k;storec=l;lastff=flow[indexl];
									}
								} /* if else */
							}/* if else */
						} /* k */
					}/* valid */

					/* evaluate 4 cells to the top and to the bottom */
					k=i+r-ventus;l=j+c;
					for(k=i+r-ventus;k<=i+r+ventus;k++) {
						if(Valid(k,l)) {
							if(k>= lastr-1 && k<=lastr+1 && l>=lastc-1 && l<=lastc+1) {
								indexl=(long long)k * (long long)col;indexl+=(long long)l;
								if(flow[indexl]+tolerance <=lastf) {
									storer=k;storec=l;lastf=flow[indexl];
								}else {
									if(storer==-1) {
										storer=k;storec=l;lastff=flow[indexl];
									}else {
										if(flow[indexl]+tolerance <lastff) {
											storer=k;storec=l;lastff=flow[indexl];
										}
									} /* if else */
								}/* if else */
							} /* k */
						}/* valid */
					} /* for k */
						


				} /* if k != */
			} /* if r!=0 || */
			if(storer!=-1) {
				indexl=(long long)storer * (long long)col;indexl+=(long long)storec;
				lastr=storer;lastc=storec;lastf=flow[indexl];
				ENDRptr->r=lastr;ENDRptr->c=lastc;ENDRptr->hit=1;ENDRptr->quad=1;numcells++;
				ENDRptr->nextptr=(RDS *) calloc(1,sizeof(RDS));
				ENDRptr=ENDRptr->nextptr;
				if(ENDRptr==NULL){printf("ERROR, ENDRptr==NULL\n");exit(-1);}
			}
		} /* end of for displ */

	} else if(ii>=i && jj>j) {		/* quad 2*/
		if(deltar==0 ||deltac==0) {
			angle=0;
		} else {
			angle=(float)deltar/(float)deltac;  /* delta north /delta east */
		}
		angle=atan(angle);
		indexl=(long long)i * (long long)col;indexl+=(long long)j;
		lastr=i;lastc=j;lastf=(int)flow[indexl];lastff=lastf;

		for(displ=1;displ<=dist;displ++) {
			r=displ*sin(angle);c=displ*cos(angle);
			storer=storec=-1;
			if(r!=0 || c!=0 ) {
				k=i+r;l=j+c;		/* add the run & rise to the origin - this is the center line or the straight line 
									from i,j to ii,jj */

				/* if the new k & l are adjacent to the previous cell, then if the flow value of this cell
				is less than the previous cell, store the coords of this cell and the flow value.  Otherwise, if
				this is the first cell that is adjacent and its flow value is => than the previous cell, just
				store the coords. */
				if(k!=lastr || l!=lastc) {	/*  If not the same r,c as last time */

					if(Valid(k,l)) {
						if(k>= lastr-1 && k<=lastr+1 && l>=lastc-1 && l<=lastc+1) {
							indexl=(long long)k * (long long)col;indexl+=(long long)l;
							if(flow[indexl]+tolerance <=lastf) {
								storer=k;storec=l;lastf=flow[indexl];
							}else {
								if(storer==-1) {
									storer=k;storec=l;lastff=flow[indexl];
								}else {
									if(flow[indexl]+tolerance<lastff) {
										storer=k;storec=l;lastff=flow[indexl];
									}
								} /* if else */
							}/* if else */
						} /* k */
					}/* valid */

					/* evaluate 4 cells to the top and to the bottom */
					k=i+r-ventus;l=j+c;
					for(k=i+r-ventus;k<=i+r+ventus;k++) {
						if(Valid(k,l)) {
							if(k>= lastr-1 && k<=lastr+1 && l>=lastc-1 && l<=lastc+1) {
								indexl=(long long)k * (long long)col;indexl+=(long long)l;
								if(flow[indexl]+tolerance <=lastf) {
									storer=k;storec=l;lastf=flow[indexl];
								}else {
									if(storer==-1) {
										storer=k;storec=l;lastff=flow[indexl];
									}else {
										if(flow[indexl]+tolerance<lastff) {
											storer=k;storec=l;lastff=flow[indexl];
										}
									} /* if else */
								}/* if else */
							} /* k */
						}/* valid */
					} /* for k */
				} /* if k != */
			}

			if(storer!=-1) {
				indexl=(long long)storer * (long long)col;indexl+=(long long)storec;
				lastr=storer;lastc=storec;lastf=flow[indexl];
				ENDRptr->r=lastr;ENDRptr->c=lastc;ENDRptr->hit=1;ENDRptr->quad=2;numcells++;
				ENDRptr->nextptr=(RDS *) calloc(1,sizeof(RDS));
				ENDRptr=ENDRptr->nextptr;
				if(ENDRptr==NULL){printf("ERROR, ENDRptr==NULL\n");exit(-1);}
			}
		} /* end of for displ */

	} else if(ii>i && jj<=j) {	/* quad 3 */
		if(deltar==0 ||deltac==0) {
			angle=0;
		} else {
			angle=(float)deltac/(float)deltar;  /* delta east /delta north */
		}
		angle=atan(angle);

		indexl=(long long)i * (long long)col;indexl+=(long long)j;
		lastr=i;lastc=j;lastf=(int)flow[indexl];lastff=lastf;
		for(displ=1;displ<=dist;displ++) {
			r=displ*cos(angle);c=displ*sin(angle);c=c*(-1.0);
			storer=storec=-1;
			if(r!=0 || c!=0 ) {
				k=i+r;l=j+c;		/* add the run & rise to the origin - this is the center line or the straight line 
									from i,j to ii,jj */

				/* if the new k & l are adjacent to the previous cell, then if the flow value of this cell
				is less than the previous cell, store the coords of this cell and the flow value.  Otherwise, if
				this is the first cell that is adjacent and its flow value is => than the previous cell, just
				store the coords. */
				if(k!=lastr || l!=lastc) {	/*  If not the same r,c as last time */

					if(Valid(k,l)) {
						if(k>= lastr-1 && k<=lastr+1 && l>=lastc-1 && l<=lastc+1) {
							indexl=(long long)k * (long long)col;indexl+=(long long)l;
							if(flow[indexl]+tolerance <=lastf) {
								storer=k;storec=l;lastf=flow[indexl];
							}else {
								if(storer==-1) {
									storer=k;storec=l;lastff=flow[indexl];
								}else {
									if(flow[indexl]+tolerance <lastff) {
										storer=k;storec=l;lastff=flow[indexl];
									}
								} /* if else */
							}/* if else */
						} /* k */
					}/* valid */

					/* evaluate 4 cells to the top and to the bottom */
					k=i+r-ventus;l=j+c;
					for(k=i+r-ventus;k<=i+r+ventus;k++) {
						if(Valid(k,l)) {
							if(k>= lastr-1 && k<=lastr+1 && l>=lastc-1 && l<=lastc+1) {
								indexl=(long long)k * (long long)col;indexl+=(long long)l;
								if(flow[indexl]+tolerance <=lastf) {
									storer=k;storec=l;lastf=flow[indexl];
								}else {
									if(storer==-1) {
										storer=k;storec=l;lastff=flow[indexl];
									}else {
										if(flow[indexl]+tolerance<lastff) {
											storer=k;storec=l;lastff=flow[indexl];
										}
									} /* if else */
								}/* if else */
							} /* k */
						}/* valid */
					} /* for k */
				} /* if k != */
			}
			if(storer!=-1) {
				indexl=(long long)storer * (long long)col;indexl+=(long long)storec;
				lastr=storer;lastc=storec;lastf=flow[indexl];
				ENDRptr->r=lastr;ENDRptr->c=lastc;ENDRptr->hit=1;ENDRptr->quad=3;numcells++;
				ENDRptr->nextptr=(RDS *) calloc(1,sizeof(RDS));
				ENDRptr=ENDRptr->nextptr;
				if(ENDRptr==NULL){printf("ERROR, ENDRptr==NULL\n");exit(-1);}
			}
		} /* end of for displ */

	} else if(ii<=i && jj<j) {		/* quad 4 */
		if(deltar==0 ||deltac==0) {
			angle=0;
		} else {
			angle=(float)deltar/(float)deltac;  /* delta north /delta east */
		}
		angle=atan(angle);
		if(padid==-134){printf("angle %f\n",angle);pause(); }

		indexl=(long long)i * (long long)col;indexl+=(long long)j;
		lastr=i;lastc=j;lastf=(int)flow[indexl];lastff=lastf;
		for(displ=1;displ<=dist;displ++) {
			r=displ*sin(angle);r=r*(-1.0);c=displ*cos(angle);c=c*(-1.0);
			storer=storec=-1;
			if(r!=0 || c!=0 ) {
				k=i+r;l=j+c;		/* add the run & rise to the origin - this is the center line or the straight line 
									from i,j to ii,jj */

				/* if the new k & l are adjacent to the previous cell, then if the flow value of this cell
				is less than the previous cell, store the coords of this cell and the flow value.  Otherwise, if
				this is the first cell that is adjacent and its flow value is => than the previous cell, just
				store the coords. */
				if(k!=lastr || l!=lastc) {	/*  If not the same r,c as last time */

					if(Valid(k,l)) {
						if(k>= lastr-1 && k<=lastr+1 && l>=lastc-1 && l<=lastc+1) {
							indexl=(long long)k * (long long)col;indexl+=(long long)l;
							if(flow[indexl]+tolerance <=lastf) {
								storer=k;storec=l;lastf=flow[indexl];	if(padid==-134){printf("1 k l %d %d %d\n",k,l,lastf);pause(); }
							}else {
								if(storer==-1) {
									storer=k;storec=l;lastff=flow[indexl];if(padid==-134){printf("2 k l %d %d %d\n",k,l,lastff);pause(); }
								}else {
									if(flow[indexl]+tolerance<lastff) {
										storer=k;storec=l;lastff=flow[indexl];if(padid==-134){printf("3 k l %d %d %d\n",k,l,lastff);pause(); }
									}
								} /* if else */
							}/* if else */
						} /* k */
					}/* valid */

					/* evaluate 4 cells to the top and to the bottom */
					k=i+r-ventus;l=j+c;
					for(k=i+r-ventus;k<=i+r+ventus;k++) {
						if(Valid(k,l)) {
							if(k>= lastr-1 && k<=lastr+1 && l>=lastc-1 && l<=lastc+1) {
								indexl=(long long)k * (long long)col;indexl+=(long long)l;
								if(flow[indexl]+tolerance <=lastf) {
									storer=k;storec=l;lastf=flow[indexl];if(padid==-134){printf("4 k l %d %d %d\n",k,l,lastf);pause(); }
								}else {
									if(storer==-1) {
										storer=k;storec=l;lastff=flow[indexl];if(padid==-134){printf("5 k l %d %d %d\n",k,l,lastff);pause(); }
									}else {
										if(flow[indexl]+tolerance <lastff) {
											storer=k;storec=l;lastff=flow[indexl];if(padid==-134){printf("6 k l %d %d %d\n",k,l,lastff);pause(); }
										}
									} /* if else */
								}/* if else */
							} /* k */
						}/* valid */
					} /* for k */
				} /* if k != */
			}
			if(storer!=-1) {
				indexl=(long long)storer * (long long)col;indexl+=(long long)storec;
				lastr=storer;lastc=storec;lastf=flow[indexl];
				ENDRptr->r=lastr;ENDRptr->c=lastc;ENDRptr->hit=1;ENDRptr->quad=4;numcells++;
				ENDRptr->nextptr=(RDS *) calloc(1,sizeof(RDS));
				ENDRptr=ENDRptr->nextptr;
				if(ENDRptr==NULL){printf("ERROR, ENDRptr==NULL\n");exit(-1);}

			}
		} /* end of for displ */
	} /* end of quad selection */

		/* check to make sure everything ended AOK */

		if(lastr!=ii || lastc!=jj) {
			ConvToPtsF(lastr,lastc,&n,&e);
			/* printf("padid %d\n",padid);
			printf("%d %d\n",lastr,lastc);
			printf("%d %d \n",ii,jj);
			printf("%d %d %d %d %f %f %f %f %f %f\n",sr,sc, tar, tac,ssn, sse,  ddn, dde,n,e); */

			DrawSLinePMOD(n,e,dn,de);  /* this draws a straight line based on pts and updates RDptr.  RDptr
									   is used below in DeriveRD() to generate vertices - so no need to
									   store anything in RDVERTSptr inside of this module */
		}


	/* Now determine the vertices of this road ********************************* */
	if(numcells<=3) {
		DrawSLineP(sn,se,dn,de,(int)10);
	}else {
		DeriveRd(numcells,sn,se,dn,de);
	}
	ProcessRds();  /* Smooth, output to RDlines#.csv, then globally store anchor pts */
	ClearRDS(); NEWrd=NULL;RDptr=NULL;ENDRptr=NULL; 
	free(RDVERTSptr);NOVERTS=0;
}

/* Derives a line given first cut at road cells.
* new road vertices are stored in rdlines
* Anchor pts are set - SetRdCells().

Fancy logic:
1) A square is labeled 1- 9, with 1 in the upper left, 2 in the upper middle, 3 in the upper right... This
continues thru the middle of the square (4, 5, 6), and then at the bottom of the square (7, 8, 9).  These
indicate where the line passes from 1 cell to another. Starting with the first cell, the utm pt locations are determined based on the angle between
the first and the second cell.  prev is recorded based on angle, then the next cell is evaluated, based on the
subsequent cell (the 3rd cell).  Pts are moved to the left, right, or middle depending on the angle with the
next cell.  A set number of angles and pt positions are invisioned, such that this should work for all
quads. */

void	DeriveRd(int numcells,float sn,float se,float dn,float de)
{

	int		sr,sc,dr,dc;
	int		GetAngle(int,int,int,int);
	int		angle;
	int		one,zero,rdcode;
	int		prev;
	float	n,e,no,ea;
	void	ConvToPtsL(int, int, float *, float *);  /* this returns the upper left coords */
	void	ConvToPtsF(int, int, float *, float *);  /* this returns the middle of the cell coords */
	void	Record(float , float,int , int ,int);
	void	GetNE(int,float,float,float *,float *);
	char	tr;
	int		cnt;
	void	DrawSLineP(float,float,float,float,int);	/* straight line based on points */
	RDS		*ptrs,*ptrd,*Nptr,*Nptrd;


	rdcode=10;
	one=1;zero=0;

	cnt=0;
	/* Eliminate duplicates from the RDS list */


	ptrs=RDptr;
	while(ptrs!=NULL) {
		sr=ptrs->r;sc=ptrs->c;
		ptrd=ptrs->nextptr;
		while(ptrd!=NULL) {
			dr=ptrd->r;dc=ptrd->c;
			if(sr==dr && sc==dc) {
				ptrd->hit=0;cnt++;
			}
			ptrd=ptrd->nextptr;
		}
		ptrs=ptrs->nextptr;
	}

	ptrs=RDptr;
	while(ptrs!=NULL) {
		if(ptrs->hit >0) {
			if(NEWrd==NULL) {
				NEWrd = (RDS *) calloc(1,sizeof(RDS));
				Nptr=NEWrd;
				Nptr->r=ptrs->r;Nptr->c=ptrs->c;
				Nptr->hit=1;
				/* if(loop==17)printf("NEW %d %d %d %d\n",Nptr->r,Nptr->c,Nptr,ptrs); */
			}else {
				Nptr->nextptr= (RDS *) calloc(1,sizeof(RDS));
				Nptr=Nptr->nextptr;
				Nptr->r=ptrs->r;Nptr->c=ptrs->c;
			/*	if(loop==17)printf("NEW %d %d %d %d\n",Nptr->r,Nptr->c,Nptr,ptrs); */
				Nptr->hit=1;
			}
		}
		ptrs=ptrs->nextptr;
	}


	/* if after eliminating duplicate cells we only have 3 or less remaining, do DrawLineSP */
	if(numcells-cnt <=3) {
		DrawSLineP(sn,se,dn,de,rdcode);return;
	}



    /* increment line counter here - if DrawSLineP is called, it also increments line numbers */   
	newline++;

/*	if(newline==162) {
		Nptr=NEWrd;
		while(Nptr!=NULL) {
			printf("%d %d %d\n",Nptr->r,Nptr->c,Nptr->hit);
			Nptr=Nptr->nextptr;
		}
	} */


	/* Else start with the first 2 cells */
	Nptr=NEWrd;	sr=Nptr->r;sc=Nptr->c;
	Nptrd=Nptr->nextptr;	dr=Nptrd->r;dc=Nptrd->c;
	angle=GetAngle(sr,sc,dr,dc);


/* Schema:

Locations in a square are labeled as follows:

	1	11	2	33	3
	4	44	5	66	6
	7	77	8	99	9

	prev refers to the direction of entry into the source cell from the previous source cell.  prev
	is used to bend the road thru the current source cell towards the destination cell.
*/

	if(angle==0) {
		prev=2;	Record(sn,se,one,rdcode,(int)2);	
	}else if(angle==90) {
		prev=6;Record(sn,se,one,rdcode,(int)2);
	}else if(angle==45) {
		prev=3;Record(sn,se,one,rdcode,(int)2);
		ConvToPtsL(sr,sc,&n,&e);
		e=e+grain;Record(n,e,zero,rdcode,(int)1);
	}else if(angle ==180) {
		prev=8;Record(sn,se,one,rdcode,(int)2);
	}else if(angle==270) {
		prev=4;Record(sn,se,one,rdcode,(int)2);
	}else if(angle==135){
		prev=9;Record(sn,se,one,rdcode,(int)2);
		ConvToPtsL(sr,sc,&n,&e);
		e=e+grain;n=n-grain;Record(n,e,zero,rdcode,(int)1);
	}else if(angle==225) {
		prev=7;Record(sn,se,one,rdcode,(int)2);
		ConvToPtsL(sr,sc,&n,&e);
		n=n-grain;Record(n,e,zero,rdcode,(int)1);
	}else if(angle==315) {
		prev=1;Record(sn,se,one,rdcode,(int)2);
		ConvToPtsL(sr,sc,&n,&e);
		Record(n,e,zero,rdcode,(int)1);
	}else {
		printf("ERROR, invalid angle - first one %d\n",angle);exit(-1);
	}
	
	
	/* Do the remaining cells */
	Nptr=Nptr->nextptr;
	while(Nptr!=NULL) {
		if(Nptr->hit >0) {
			sr=Nptr->r;sc=Nptr->c;
			if(Nptr->nextptr!=NULL) {
				Nptrd=Nptr->nextptr;
				if(Nptrd->hit >0) {
					dr=Nptrd->r;dc=Nptrd->c;
						angle=GetAngle(sr,sc,dr,dc);
						ConvToPtsL(sr,sc,&n,&e);	/* get coords of the upper left of the cell */
						/* if(newline==162) {printf("%d %d %d\n",newline,angle,prev);} */
						if(angle==0) {
							if(prev==1) {
								GetNE((int)5,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)2);
								prev=2;
							}else if(prev==2) {
								GetNE((int)5,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)2);
								prev=2;
							}else if(prev==3) {
								GetNE( (int)5,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)2);
								prev=2;
							}else if(prev==4) {
								GetNE( (int)33,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)2);
								prev=2;
							}else if(prev==6) {
								GetNE( (int)11,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)2);
								prev=2;
							}else if(prev==7) {
								GetNE( (int)33,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)2);
								prev=2;
							}else if(prev==8) {
								GetNE( (int)5,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)2);
								prev=2;
							}else if(prev==9) {
								GetNE( (int)11,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)2);
								prev=2;
							}
						}else if(angle==90) {
							if(prev==1) {
								GetNE((int)99,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)2);
								prev=6;
							}else if(prev==2) {
								GetNE((int)99,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)2);
								prev=6;
							}else if(prev==3) {
								GetNE( (int)5,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)2);
								prev=6;
							}else if(prev==4) {
								GetNE( (int)5,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)2);
								prev=6;
							}else if(prev==6) {
								GetNE( (int)5,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)2);
								prev=6;
							}else if(prev==7) {
								GetNE( (int)66,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)2);
								prev=6;
							}else if(prev==8) {
								GetNE( (int)33,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)2);
								prev=6;
							}else if(prev==9) {
								GetNE( (int)33,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)2);
								prev=6;
							}
						}else if(angle==180) {
							if(prev==1) {
								GetNE((int)99,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)2);
								prev=8;
							}else if(prev==2) {
								GetNE((int)5,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)2);
								prev=8;
							}else if(prev==3) {
								GetNE( (int)77,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)2);
								prev=8;
							}else if(prev==4) {
								GetNE( (int)99,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)2);
								prev=8;
							}else if(prev==6) {
								GetNE( (int)77,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)2);
								prev=8;
							}else if(prev==7) {
								GetNE( (int)66,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)2);
								prev=8;
							}else if(prev==8) {
								GetNE( (int)5,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)2);
								prev=8;
							}else if(prev==9) {
								GetNE( (int)77,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)2);
								prev=8;
							}
						}else if(angle==270) {
							if(prev==1) {
								GetNE((int)44,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)2);
								prev=4;
							}else if(prev==2) {
								GetNE((int)77,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)2);
								prev=4;
							}else if(prev==3) {
								GetNE( (int)77,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)2);
								prev=4;
							}else if(prev==4) {
								GetNE( (int)5,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)2);
								prev=4;
							}else if(prev==6) {
								GetNE( (int)5,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)2);
								prev=4;
							}else if(prev==7) {
								GetNE( (int)11,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)2);
								prev=4;
							}else if(prev==8) {
								GetNE( (int)11,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)2);
								prev=4;
							}else if(prev==9) {
								GetNE( (int)11,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)2);
								prev=4;
							}
						}else if(angle==45) {
							if(prev==1) {
								GetNE((int)66,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)2); /* when doing diagonals, store first as anchor, but not corner pt */
								GetNE((int)3,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)1);
								prev=3;
							}else if(prev==2) {
								GetNE((int)5,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)2);
								GetNE((int)3,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)1);
								prev=3;
							}else if(prev==3) {
								GetNE( (int)5,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)2);
								GetNE((int)3,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)1);
								prev=3;
							}else if(prev==4) {
								GetNE( (int)66,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)2);
								GetNE((int)3,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)1);
								prev=3;
							}else if(prev==6) {
								GetNE( (int)5,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)2);
								GetNE((int)3,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)1);
								prev=3;
							}else if(prev==7) {
								GetNE( (int)5,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)2);
								GetNE((int)3,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)1);
								prev=3;
							}else if(prev==8) {
								GetNE( (int)33,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)2);
								GetNE((int)3,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)1);
								prev=3;
							}else if(prev==9) {
								GetNE( (int)11,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)2);
								GetNE((int)3,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)1);
								prev=3;
							}
						}else if(angle==315) { 
							if(prev==1) {
								GetNE((int)5,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)2); /* when doing diagonals, store first as anchor, but not corner pt */
								GetNE((int)1,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)1);
								prev=1;
							}else if(prev==2) {
								GetNE((int)44,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)2);
								GetNE((int)1,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)1);
								prev=1;
							}else if(prev==3) {
								GetNE( (int)77,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)2);
								GetNE((int)1,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)1);
								prev=1;
							}else if(prev==4) {
								GetNE( (int)5,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)2);
								GetNE((int)1,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)1);
								prev=1;
							}else if(prev==6) {
								GetNE( (int)44,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)2);
								GetNE((int)1,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)1);
								prev=1;
							}else if(prev==7) {
								GetNE( (int)33,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)2);
								GetNE((int)1,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)1);
								prev=1;
							}else if(prev==8) {
								GetNE( (int)11,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)2);
								GetNE((int)1,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)1);
								prev=1;
							}else if(prev==9) {
								GetNE( (int)11,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)2);
								GetNE((int)1,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)1);
								prev=1;
							}
						}else if(angle==135) { 
							if(prev==1) {
								GetNE((int)5,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)2); /* when doing diagonals, store first as anchor, but not corner pt */
								GetNE((int)9,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)1);
								prev=9;
							}else if(prev==2) {
								GetNE((int)99,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)2);
								GetNE((int)9,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)1);
								prev=9;
							}else if(prev==3) {
								GetNE( (int)77,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)2);
								GetNE((int)9,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)1);
								prev=9;
							}else if(prev==4) {
								GetNE( (int)99,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)2);
								GetNE((int)9,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)1);
								prev=9;
							}else if(prev==6) {
								GetNE( (int)5,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)2);
								GetNE((int)9,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)1);
								prev=9;
							}else if(prev==7) {
								GetNE( (int)33,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)2);
								GetNE((int)9,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)1);
								prev=9;
							}else if(prev==8) {
								GetNE( (int)99,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)2);
								GetNE((int)9,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)1);
								prev=9;
							}else if(prev==9) {
								GetNE( (int)5,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)2);
								GetNE((int)9,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)1);
								prev=9;
							}
						}else if(angle==225) { 
							if(prev==1) {
								GetNE((int)99,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)2); /* when doing diagonals, store first as anchor, but not corner pt */
								GetNE((int)7,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)1);
								prev=7;
							}else if(prev==2) {
								GetNE((int)77,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)2);
								GetNE((int)7,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)1);
								prev=7;
							}else if(prev==3) {
								GetNE( (int)5,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)2);
								GetNE((int)7,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)1);
								prev=7;
							}else if(prev==4) {
								GetNE( (int)5,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)2);
								GetNE((int)7,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)1);
								prev=7;
							}else if(prev==6) {
								GetNE( (int)77,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)2);
								GetNE((int)7,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)1);
								prev=7;
							}else if(prev==7) {
								GetNE( (int)5,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)2);
								GetNE((int)7,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)1);
								prev=7;
							}else if(prev==8) {
								GetNE( (int)44,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)2);
								GetNE((int)7,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)1);
								prev=7;
							}else if(prev==9) {
								GetNE( (int)44,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)2);
								GetNE((int)7,n,e,&no,&ea);
								Record(no,ea,zero,rdcode,(int)1);
								prev=7;
							}
						}else {
							printf("ERROR, invalid angle %d\n",angle);exit(-1);
						}
				} /* if hit */
			} /* != NULL */
		} /* if hit */
		Nptr=Nptr->nextptr;
	} /* while */

	/* set the target utm */
	Record(dn,de,one,rdcode,(int)1);	/* already has an anchor, hence (int)1) */
}

/* derives road vertices for specific regions of a cell */
void	GetNE(int code,float n, float e, float *no, float *ea)
{

	float		r,c;

	switch(code){
		case(1):		/* NW corner */
			*no=n;*ea = e;
		break;
		case(3):		/* NE corner */
			*no=n;*ea=e+grain;
		break;
		case(5):		/* center */
			*no=n - (0.5*grain); *ea = e +(.5*grain);	/* centroid */
		break;
		case(7):		/* SW corner */
			*no=n-grain;*ea=e;
		break;
		case(9):		/* SE corner */
			*no=n-grain;*ea=e+grain;
		break;
		case(11):		/* LHS upper */
			*no=n-(.25*grain);*ea=e+(.25*grain);
		break;
		case(33):		/* RHS upper */
			*no=n-(.25*grain);*ea=e+(.75*grain);
		break;
		case(77):		/* LHS lower */
			*no=n-(.75*grain);*ea=e+(.25*grain);
		break;
		case(99):		/* RHS lower */
			*no=n-(.75*grain);*ea=e+(.75*grain);
		break;
		case(44):		/* LHS center */
			*no=n-(.5*grain);*ea=e+(.25*grain);
		break;
		case(66):		/* RHS center */
			*no=n-(.5*grain);*ea=e+(.75*grain);
		break;
	}
}

/* get angle between source and destination */
int	GetAngle(sr,sc,dr,dc)
{
	int			deltar,deltac;
	int			ans;

	if(sr==dr){
		if(sc-dc <0) return ((int)90);
		if(sc-dc >0) return( (int) 270);
	}
	if(sc==dc){
		if(sr-dr >0)return( (int)0);
		if(sr-dr <0)return( (int)180);
	}
	
	deltar=sr-dr;deltac=sc-dc;

	if(deltar >0) {
		if(deltac>0) return( (int)315);
		if(deltac<0)return( (int) 45);
	}
	if(deltar<0) {
		if(deltac>0) return( (int) 225);
		if(deltac<0) return( (int) 135);
	}
	printf("ERROR, problem in GetAngle %d %d %d %d %d %d\n",deltar,deltac,sr,sc,dr,dc);exit(-1);
}

/* rdcode is road type code?? keep it for now but consider deleting */
void	Record(float n, float e, int code, int rdcode, int action)
{
	char	ConvToRC2(float,float,int *, int *),tr;
	int		r,c;
	long long	indexl;


	tr=ConvToRC2(n,e,&r,&c);
	if(!tr) {
		printf("in Record %f %f %d %d\n",n,e,r,c);exit(-1);
	}
	indexl=(long long)r * (long long)col;indexl+=(long long)c;

	NOVERTS++;
	if(NOVERTS>=5000) {printf("ERROR, NOVERTS too large \n");exit(-1);}

	RDVERTSptr[NOVERTS].n=n; RDVERTSptr[NOVERTS].e=e;
	RDVERTSptr[NOVERTS].oldn=n;	RDVERTSptr[NOVERTS].olde=e;
	RDVERTSptr[NOVERTS].oldr=r;	RDVERTSptr[NOVERTS].oldc=c;

	RDVERTSptr[NOVERTS].code=code;RDVERTSptr[NOVERTS].action=action;
	RDVERTSptr[NOVERTS].newline=newline;
	RDVERTSptr[NOVERTS].time=time;
	RDVERTSptr[NOVERTS].index=indexl;
	RDVERTSptr[NOVERTS].type=rdcode;
}



/* record vertices of road and set road anchor */
void	Recordold(float n, float e,int code, int rdcode,int action)
{
	void	SetRdCells(float,float,int,int); 

	fprintf(linesf,"%f, %f, %d, %d, 5\n",n,e,newline,time);
	fprintf(croads,"%f, %f, %d, %d, 1\n",n,e,newline+MAXRDEND,time);

	if(action==2) SetRdCells(n,e,code,rdcode);
}

void	ClearRDS2()
{

	int		i;

	RDS		*ptr;
	RDS		*SHit;
	int		cnt;
	SHit=(RDS *) calloc(5000,sizeof(RDS));


	ptr=RDptr;
	cnt=0;
	while(ptr!=NULL) {
	/*	printf("R %d %d %d\n",ptr->r,ptr->c,ptr); */
		if(cnt<4998) {
			cnt++;
			SHit[cnt].ptr=ptr;
			ptr=ptr->nextptr;
		}

	}
	ptr=NEWrd;
	while(ptr!=NULL) {
		/* printf("N %d %d %d\n",ptr->r,ptr->c,ptr); */
		if(cnt<4998) {
			cnt++;
			SHit[cnt].ptr=ptr;
			ptr=ptr->nextptr;
		}
	}

	for(i=1;i<=cnt;i++) {
		free(SHit[i].ptr);
	}
	free(SHit);
}

/* clear memory */
void	ClearRDS()
{
	RDS		*ptr,*old;

/*	if(loop==17) {
		ptr=RDptr;
		while(ptr!=NULL) {
			printf("R %d %d %d\n",ptr->r,ptr->c,ptr);
			ptr=ptr->nextptr;
		}

		ptr=NEWrd;
		while(ptr!=NULL) {
			printf("N %d %d %d\n",ptr->r,ptr->c,ptr);
			ptr=ptr->nextptr;
		}
	} */

	old=RDptr;
	while(old!=NULL) {
		/* if(loop==17) printf("old_n %d\n",old->nextptr); */
		ptr=old->nextptr;  /* if(loop==17)printf("r next %d\n",ptr); */
		/* if(loop==17)printf("r %d %d\n",old,ptr); */
		free(old);
		old=ptr; /* if(loop==17)printf("rold %d\n",old); */
	}

	old=NEWrd;
	while(old!=NULL) {
		ptr=old->nextptr;
		/* if(loop==17)printf("n %d %d\n",old,ptr); */
		free(old);
		old=ptr;
	}
}