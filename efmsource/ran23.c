/* ran23.c */
#include <stdio.h>
#include <math.h>


#define   IM1 2147483563
#define   IM2 21474833399
#define   AM (1.0/IM1)
#define   IMM1  (IM1-1)
#define   IA1 40014
#define   IA2 40692
#define   IQ1 53668
#define   IQ2 52774 
#define   IR1 12211 
#define   IR2 3791
#define   NTAB 32
#define   NDIV (1+(IMM1-1)/NTAB)
#define   EPS 1.2e-7
#define   RNMX (1.0-EPS)

extern float	kdum;

/* returns a u.r.d. between 0 & 1 */

float Ran23(float *kdum) /* idum is initialized with a a number <0 */
{
 int           j;
 long          k;
 static  long  kdum2=123456789;
 static  long  iy=0;
 static long   iv[NTAB];
 float         temp;

 if(*kdum<=0){
   if(-(*kdum)<1)*kdum=1;
   else *kdum=-(*kdum);
   kdum2=(*kdum);
   for(j=NTAB+7;j>=0;j--){
     k=(*kdum)/IQ1;
     *kdum=IA1*(*kdum-k*IQ1)-k*IR1;
     if(*kdum<0) *kdum+=IM1;
     if(j<NTAB) iv[j]=*kdum;
   } 
   iy=iv[0];
 }
 k=(*kdum)/IQ1;
 *kdum=IA1*(*kdum-k*IQ1)-k*IR1;
 if(*kdum<0) *kdum+=IM1;
 k=kdum2/IQ2;
 kdum2=IA2*(kdum2-k*IQ2)-k*IR2;
 if(kdum2<0)kdum2+=IM2;
 j=iy/NDIV;
 iy=iv[j]-kdum2;
 iv[j]= *kdum;
 if(iy<1)iy +=IMM1;
 if( (temp=AM*iy)>RNMX) return (RNMX);
 else return (temp);
}


