/* Donut()  - Generates a distance matrix between 2 points on the landscape - an existing road cell
and a pad (centroid).  Called by Curve() and effectively searches around obstructions for the shortest pathway for 
a road.  During development of this function, the resulting distance matrix often had a 'hole' in/near the center; hence the name Donut().
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include "geo.h"
#include "curve.h"

extern	int	row,col;

char	Donut(int r, int c,int theid, int *rr, int *cc)
{
	int		i,j,br,er,bc,ec,icnt;
	int		savei,savej;
	char	tr;
	char	Valid(int,int);
	int		nopid,more;
	char	first;
	double	savedist,dist;
	char	ValidR(int);


	first=1;more=0;
	tr=1;icnt=0;
	while(tr) {
		icnt++;
		if(more>0) {if (icnt> more+20) {*rr=savei;*cc=savej;return((char)1);}}
		nopid=0;
		br=r-icnt;er=br;bc=c-icnt;ec=c+icnt;	/* across the top */
		for(i=br;i<=er;i++) {
			for(j=bc;j<=ec;j++) {
				if(Valid(i,j)) {
					if(patchid[i*col+j]==theid) {
						nopid=1;
						if(ValidR(rds[i*col+j])){
							if(first){
								first=0;more=icnt;
								savei=i;savej=j;savedist=( (r-i)*(r-i) + (c-j)*(c-j));savedist=sqrt(savedist);
							}else {
								dist=( (r-i)*(r-i) + (c-j)*(c-j));dist=sqrt(dist);
								if(dist<savedist) {savedist=dist;savei=i;savej=j;}
							}
						}
					}
				}
			}
		}
		br=r+icnt;er=br;bc=c-icnt;ec=c+icnt;	/* across the bottom */
		for(i=br;i<=er;i++) {
			for(j=bc;j<=ec;j++) {
				if(Valid(i,j)) {
					if(patchid[i*col+j]==theid) {
						nopid=1;
						if(ValidR(rds[i*col+j])){
							if(first){
								first=0;more=icnt;
								savei=i;savej=j;savedist=( (r-i)*(r-i) + (c-j)*(c-j));savedist=sqrt(savedist);
							}else {
								dist=( (r-i)*(r-i) + (c-j)*(c-j));dist=sqrt(dist);
								if(dist<savedist) {savedist=dist;savei=i;savej=j;}
							}
						}
					}
				}
			}
		}
		br=r-icnt;er=r+icnt;bc=c-icnt;ec=bc;	/* down LHS */
		for(i=br;i<=er;i++) {
			for(j=bc;j<=ec;j++) {
				if(Valid(i,j)) {
					if(patchid[i*col+j]==theid) {
						nopid=1;
						if(ValidR(rds[i*col+j])){
							if(first){
								first=0;more=icnt;
								savei=i;savej=j;savedist=( (r-i)*(r-i) + (c-j)*(c-j));savedist=sqrt(savedist);
							}else {
								dist=( (r-i)*(r-i) + (c-j)*(c-j));dist=sqrt(dist);
								if(dist<savedist) {savedist=dist;savei=i;savej=j;}
							}
						}
					}
				}
			}
		}
		br=r-icnt;er=r+icnt;bc=c+icnt;ec=bc;	/* down RHS */
		for(i=br;i<=er;i++) {
			for(j=bc;j<=ec;j++) {
				if(Valid(i,j)) {
					if(patchid[i*col+j]==theid) {
						nopid=1;
						if(ValidR(rds[i*col+j])){
							if(first){
								first=0;more=icnt;
								savei=i;savej=j;savedist=( (r-i)*(r-i) + (c-j)*(c-j));savedist=sqrt(savedist);
							}else {
								dist=( (r-i)*(r-i) + (c-j)*(c-j));dist=sqrt(dist);
								if(dist<savedist) {savedist=dist;savei=i;savej=j;}
							}
						}
					}
				}
			}
		}
		if(nopid==0) {  /* this terminates if we run outof patch area BUT we may have found a road before exceeding the extent of the patch */
			if(!first) {*rr=savei;*cc=savej;return((char)1);}
			return((char)0);
		}
	} /* end of while */
	return((char)0);
}	

char	ValidR(int id)
{
	if(id>0) {
		if(id==1 || id==2 || id==7) return((char)0);
		return((char)1);
	}else {
		return((char)0);
	}
}

