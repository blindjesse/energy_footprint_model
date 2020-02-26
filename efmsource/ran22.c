/* ran22.c */
#include <stdio.h>
#include <math.h>
#include <stdint.h>


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

extern float	jdum;

/* returns a u.r.d. between 0 & 1 */

float Ran22(float *jdum) /* idum is initialized with a a number <0 */
{
 int            j;
 long           k;
 static int32_t jdum2=123456789;
 static int32_t iy=0;
 static int32_t iv[NTAB];
 float          temp;

 if(*jdum<=0){
   if(-(*jdum)<1)*jdum=1;
   else *jdum=-(*jdum);
   jdum2=(*jdum);
   for(j=NTAB+7;j>=0;j--){
     k=(*jdum)/IQ1;
     *jdum=IA1*(*jdum-k*IQ1)-k*IR1;
     if(*jdum<0) *jdum+=IM1;
     if(j<NTAB) iv[j]=*jdum;
   } 
   iy=iv[0];
 }
 k=(*jdum)/IQ1;
 *jdum=IA1*(*jdum-k*IQ1)-k*IR1;
 if(*jdum<0) *jdum+=IM1;
 k=jdum2/IQ2;
 jdum2=IA2*(jdum2-k*IQ2)-k*IR2;
 if(jdum2<0)jdum2+=IM2;
 j=iy/NDIV;
 iy=iv[j]-jdum2;
 iv[j]= *jdum;
 if(iy<1)iy +=IMM1;
 if( (temp=AM*iy)>RNMX) return (RNMX);
 else return (temp);
}


