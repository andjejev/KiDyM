#include "CA.h"
#include <alloc.h>
#include <math.h>
#include <string.h>

extern char Inf[];

int GausCrout(double **A,double *B,int n,double *Y,int Repeat){
 double t,S; int i,k,imax=0,p; static int *pivot;
 if(Repeat) goto lab1;
 else{
  if(pivot) free(pivot); pivot=(int *)calloc(n,sizeof(int));
 }
 for(k=0;k<n;k++){ t=0;
  for(i=k;i<n;i++){
   S=0; for(p=0;p<k;p++) S+=A[i][p]*A[p][k]; A[i][k]-=S;
   if(fabs(A[i][k])>t) { t=fabs(A[i][k]); imax=i; }
  }
  pivot[k]=imax;//A[imax][k] -наибольший элемент в к-ом столбце
  // переставим строки k и imax
  if(imax!=k) {
   for(i=0;i<n;i++){
    t=A[k][i]; A[k][i]=A[imax][i]; A[imax][i]=t;
   }
   t=B[k]; B[k]=B[imax]; B[imax]=t;
  }//перестановки завершены переходим к исключению
  if(A[k][k]==0.0)  {
   strcpy(Inf,"Матрица вырожденная!"); Error(0,0,true);
   free(pivot); return 0;
  }
  else t=1/A[k][k];
  for(i=k+1;i<n;i++) { S=t*A[i][k]; A[i][k]=S; }
  for(i=k+1;i<n;i++) {
   S=0; for(p=0;p<k;p++) S+=A[k][p]*A[p][i]; A[k][i]-=S;
  }
  S=0; for(p=0;p<k;p++) S+=A[k][p]*B[p]; B[k]-=S;
 }
 goto lab2;
//триангуляция закончена, переходим к обратной подстановке.
//нижеследующий оперетор выполняется при repeat=1;
lab1: for(k=0;k<n;k++){
       t=B[pivot[k]]; B[pivot[k]]=B[k]; B[k]=t;
       S=0; for(p=0;p<k;p++) S+=A[k][p]*B[p]; B[k]-=S;
      }
lab2: for(k=n-1;k!=-1;k--){
       S=0; for(p=k+1;p<n;p++) S+=(A[k][p]/A[k][k])*Y[p];
       if(B[k]==0.0&&S==0.0) Y[k]=0.0;
       else Y[k]=B[k]/A[k][k]-S;
      }
// free(pivot);
 return 1;
}

int SLAU (int N,double **A,double *B,double *x) {
 int Rez=GausCrout(A,B,N,x,0);
 if(!Rez) { strcpy(Inf,"Система уравнений несовместна!"); Error(0,0,true); }
 return Rez;
}

