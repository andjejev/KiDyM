#include "CA.h"
#include <alloc.h>
#include <math.h>
#include <string.h>

extern wchar_t Inf[];

bool GausCrout(double **A,double *B,int n,double *Y){
 double t,S; int i,k,imax=0,p;
 static int *pivot=(int *)calloc(n,SzI),N=n;
 if(n>N) pivot=(int *)realloc(pivot,n*SzI);
 for(k=0;k<n;k++){ t=0;
  for(i=k;i<n;i++){
   S=0; for(p=0;p<k;p++) S+=A[i][p]*A[p][k]; A[i][k]-=S;
   if(fabs(A[i][k])>t) { t=fabs(A[i][k]); imax=i; }
  }
  pivot[k]=imax;//A[imax][k] - наибольший элемент в к-ом столбце
// переставим строки k и imax
  if(imax!=k) {
   for(i=0;i<n;i++){
    t=A[k][i]; A[k][i]=A[imax][i]; A[imax][i]=t;
   }
   t=B[k]; B[k]=B[imax]; B[imax]=t;
  }
//перестановки завершены переходим к исключению
  if(A[k][k]==0.0)  {
//    wcscpy(Inf,L"Матрица вырожденная!"); Error(0,0,true);
   return false;
  }
  else t=1/A[k][k];
  for(i=k+1;i<n;i++) { S=t*A[i][k]; A[i][k]=S; }
  for(i=k+1;i<n;i++) {
   S=0; for(p=0;p<k;p++) S+=A[k][p]*A[p][i]; A[k][i]-=S;
  }
  S=0; for(p=0;p<k;p++) S+=A[k][p]*B[p]; B[k]-=S;
 }
//триангуляция закончена, переходим к обратной подстановке.
 for(k=n-1;k!=-1;k--){
  S=0; for(p=k+1;p<n;p++) S+=(A[k][p]/A[k][k])*Y[p];
  if(B[k]==0.0&&S==0.0) Y[k]=0.0;
  else Y[k]=B[k]/A[k][k]-S;
 }
 return true;
}

int GausCrout(double **A,double *B,int n,double *Y,bool Repeat){
 double t,S; int i,k,imax=0,p;
 static int *pivot=(int *)calloc(n,SzI),N=n;
 if(n>N) pivot=(int *)realloc(pivot,n*SzI);
 if(Repeat){
//выполняется при repeat=1;
  for(k=0;k<n;k++){
   t=B[pivot[k]]; B[pivot[k]]=B[k]; B[k]=t;
   S=0; for(p=0;p<k;p++) S+=A[k][p]*B[p]; B[k]-=S;
 }}
 else{
  for(k=0;k<n;k++){ t=0;
   for(i=k;i<n;i++){
    S=0; for(p=0;p<k;p++) S+=A[i][p]*A[p][k]; A[i][k]-=S;
    if(fabs(A[i][k])>t) { t=fabs(A[i][k]); imax=i; }
   }
   pivot[k]=imax;//A[imax][k] - наибольший элемент в к-ом столбце
// переставим строки k и imax
   if(imax!=k) {
    for(i=0;i<n;i++){
     t=A[k][i]; A[k][i]=A[imax][i]; A[imax][i]=t;
    }
    t=B[k]; B[k]=B[imax]; B[imax]=t;
   }
//перестановки завершены переходим к исключению
   if(A[k][k]==0.0)  {
//    wcscpy(Inf,L"Матрица вырожденная!"); Error(0,0,true);
    return 0;
   }
   else t=1/A[k][k];
   for(i=k+1;i<n;i++) { S=t*A[i][k]; A[i][k]=S; }
   for(i=k+1;i<n;i++) {
    S=0; for(p=0;p<k;p++) S+=A[k][p]*A[p][i]; A[k][i]-=S;
   }
   S=0; for(p=0;p<k;p++) S+=A[k][p]*B[p]; B[k]-=S;
 }}
//триангуляция закончена, переходим к обратной подстановке.
 for(k=n-1;k!=-1;k--){
  S=0; for(p=k+1;p<n;p++) S+=(A[k][p]/A[k][k])*Y[p];
  if(B[k]==0.0&&S==0.0) Y[k]=0.0;
  else Y[k]=B[k]/A[k][k]-S;
 }
 return 1;
}

int SLAU(int N,double **A,double *p,double *x,bool Repeat) {
 return GausCrout(A,p,N,x,Repeat);
}

int Crout(int n, double **A, double *Y, double *B, double Eps){
 double t,S; int i,k,imax=0,p;
 for(k=0;k<n;k++){ t=0;
  for(i=k;i<n;i++){
   S=0; for(p=0;p<k;p++) S+=A[i][p]*A[p][k]; A[i][k]-=S;
   if(fabs(A[i][k])>t) { t=fabs(A[i][k]); imax=i; }
  }
//переставим строки k и imax
  if(imax!=k) {
   for(i=0;i<n;i++){
    t=A[k][i]; A[k][i]=A[imax][i]; A[imax][i]=t;
   }
   t=B[k]; B[k]=B[imax]; B[imax]=t;
  }//перестановки завершены переходим к исключению
  if(fabs(A[k][k])<Eps)//"Матрица вырожденная!"
   return 0;
  t=1/A[k][k];
  for(i=k+1;i<n;i++) { S=t*A[i][k]; A[i][k]=S; }
  for(i=k+1;i<n;i++) {
   S=0; for(p=0;p<k;p++) S+=A[k][p]*A[p][i]; A[k][i]-=S;
  }
  S=0; for(p=0;p<k;p++) S+=A[k][p]*B[p]; B[k]-=S;
 }
//триангуляция закончена, переходим к обратной подстановке.
 for(k=n-1;k!=-1;k--){
  S=0; for(p=k+1;p<n;p++) S+=(A[k][p]/A[k][k])*Y[p];
  if(fabs(B[k])<Eps&&fabs(S)<Eps) Y[k]=0.0;
  else Y[k]=B[k]/A[k][k]-S;
 }
 return 1;
}

