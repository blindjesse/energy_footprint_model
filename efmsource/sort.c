/* Sort() - Sorting algorithm. Used primarily in Curve().
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include "curve.h"
 



void	Sort(float amat[],int bmat[],int dmat[], int array_size)
{
	int		i,j,min,tempi,tempj;
	float	temp;

	for(i=0;i<array_size -1; ++i) {
		min=i;
		for(j=i+1;j<array_size;++j){
			if(amat[j]<amat[min])
				min=j;
		}
		temp=amat[i];tempi=bmat[i];tempj=dmat[i];
		amat[i]=amat[min];bmat[i]=bmat[min];dmat[i]=dmat[min];
		amat[min]=temp;bmat[min]=tempi;dmat[min]=tempj;
	}
 
}	