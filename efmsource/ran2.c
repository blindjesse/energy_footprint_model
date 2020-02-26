/* ran2.c */
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

extern float	idum;

/* returns a u.r.d. between 0 & 1 */

float Ran2(float *idum) /* idum is initialized with a a number <0 */
{
 int            j;
 long           k;
 static int32_t idum2=123456789;
 static int32_t iy=0;
 static int32_t iv[NTAB];
 float          temp;

 if(*idum<=0){
   if(-(*idum)<1)*idum=1;
   else *idum=-(*idum);
   idum2=(*idum);
   for(j=NTAB+7;j>=0;j--){
     k=(*idum)/IQ1;
     *idum=IA1*(*idum-k*IQ1)-k*IR1;
     if(*idum<0) *idum+=IM1;
     if(j<NTAB) iv[j]=*idum;
   } 
   iy=iv[0];
 }
 k=(*idum)/IQ1;
 *idum=IA1*(*idum-k*IQ1)-k*IR1;
 if(*idum<0) *idum+=IM1;
 k=idum2/IQ2;
 idum2=IA2*(idum2-k*IQ2)-k*IR2;
 if(idum2<0)idum2+=IM2;
 j=iy/NDIV;
 iy=iv[j]-idum2;
 iv[j]= *idum;
 if(iy<1)iy +=IMM1;
 if( (temp=AM*iy)>RNMX) return (RNMX);
 else return (temp);
}



/* gasdev (float *idum) -> returns a normally distributed deviate with zero mean
 and unit variance, using Ran2(idum) as the source of uniform deviates.
*/

float GasDev()
{
	float	GasD();
	float	value;


	value=-3.0;
	while( fabs(value)>2.5) {
		value=GasD();
	}
	return(value);
}



float GasD()
{
 static int    iset=0;
 static float  gset;
 float         fac,rsq,v1,v2;
 float         Ran2(float *);

 if(iset==0) {
   do {
    v1=2.0*Ran2(&idum)-1.0;
    v2=2.0*Ran2(&idum)-1.0; 
    rsq=v1*v1+v2*v2;
  } while (rsq >=1.0 || rsq==0);
 fac=sqrt(-2.0*log(rsq)/rsq);
 gset=v1*fac;
 iset=1;
 return (v2*fac);
 } else {
   iset=0;
   return (gset);
 }
}
