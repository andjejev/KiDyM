#include <vcl.h>
#include <alloc.h>
#include <math.h>
#include <values.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ComCtrls.hpp>
#include "ConvDW.h"

extern double *RK_z,*RK_y1,*RK_y2,*RK_y3,*RK_w,h_RK;
extern int N,NPV,NzK,ss_RK; extern bool NoCalc; extern FILE *FRK;
extern int NDIG; extern wchar_t Inf[];
static int Por,PorA,PorB,PorE,sign,ss; static wchar_t RezA[256],RezB[256];

void rk1step(int n,double x,double *y,double h,double &xh,double *yh,
  void (*f)(double x)){
 double a[]={0.5*h,0.5*h,h,h,0.5*h}; xh=x;
 for(int k=0;k<n;k++)
  yh[k]=RK_w[k]=y[k];
 for(int j=0;j<4;j++){
  f(xh);
  xh=x+a[j];
  if(NoCalc)
   return;
  for(int k=0;k<n;k++){ yh[k]+=a[j+1]*RK_z[k]/3.0; RK_w[k]=y[k]+a[j]*RK_z[k]; }
}}

/*
double comp(double a,double b,double e){
 if(a==b) return 0.0;
 else{
  int Por=0; double AbsA=fabs(a),AbsB=fabs(b),AbsE=fabs(e),Max;
  if(AbsA>AbsB) Max=AbsA>AbsE?AbsA:AbsE; else Max=AbsB>AbsE?AbsB:AbsE;
  while(Max>1){ Max*=0.1; Por++; }
  while(Max<1){ Max*=10; Por--; }
  if(Por<0){ Por=-Por; for(int i=0;i<Por;i++){ AbsA*=10; AbsB*=10; }}
  else for(int i=0;i<Por;i++){ AbsA*=0.1; AbsB*=0.1; }
  return AbsA>AbsB?AbsA-AbsB:AbsB-AbsA;
}}
*/
double comp(double a,double b,double e){
 if(a==b) return 0.0;
 else{
  swprintf(RezA,L"0.%s",ecvt(a,NDIG,&PorA,&sign));
  swprintf(RezB,L"0.%s",ecvt(b,NDIG,&PorB,&sign));
  if(PorA>PorB) Por=PorA>(1-NDIG)?PorA:(1-NDIG);
  else Por=PorB>(1-NDIG)?PorB:(1-NDIG);
  swprintf(RezA+NDIG+2,L"E%d",PorA-Por);
  swprintf(RezB+NDIG+2,L"E%d",PorB-Por);
  return a=fabs(_wtof(RezA)-_wtof(RezB));
}}

bool RUNGE_ORIG(double x,double *y,int n,void (*f)(double x),double eps,
  double eta,double xfin,double *yfin,bool prim,TStatusBar *SB,wchar_t *Head){
 int k; bool out; double x1,x2,x3,h; wchar_t *inf=Inf;
  inf+=swprintf(inf,Head);
start: if(prim){ h=xfin-x; ss_RK=0; } else h=h_RK; out=false;
 lab1: if(((x+2.01*h-xfin)*h)>0) {
        h_RK=h; out=true; h=(xfin-x)/2;
        swprintf(inf,L": h=%g",h); if(SB) SB->SimpleText=Inf;
       }
       rk1step(n,x,y,2*h,x1,RK_y1,f);
 lab2: rk1step(n,x,y,h,x2,RK_y2,f);
       rk1step(n,x2,RK_y2,h,x3,RK_y3,f);
       for(k=0;k<n;k++)
        if(comp(RK_y1[k],RK_y3[k],eta)>eps) goto lab3;
//        if(comp1(RK_y1[k],RK_y3[k],eta)>eps) goto lab3;
//        if(fabs(RK_y1[k]-RK_y3[k])>eps) goto lab3;
       x=x3; //i=0;
       if(out) goto fin;
       for(k=0;k<n;k++) y[k]=RK_y3[k];
       if(ss_RK==5){
        ss_RK=0; h*=2;
        swprintf(inf,L": h=%g",h); SB->SimpleText=Inf;
       }
       ss_RK+=1; goto lab1;
 lab3: h*=0.5;
       swprintf(inf,L": h=%g",h); SB->SimpleText=Inf;
       out=false; x1=x2; //i++; if(i==10) return false;
       for(k=0;k<n;k++) RK_y1[k]=y[k]; goto lab2;
 fin:  for(k=0;k<n;k++) yfin[k]=RK_y3[k]; return true;
}

void rk1stepForIntg(double x,double y,double h,double &xh,double &yh,
  double (*f)(double x)){
 double w,z,a[]={0.5*h,0.5*h,h,h,0.5*h};
 xh=x; yh=y;
 for(int j=0;j<4;j++){
  z=f(xh); xh=x+a[j];
  if(NoCalc) return;
  yh+=a[j+1]*z/3.0;
}}

bool RUNGE_ORIGINAL(double x,double y,double (*f)(double x),double eps,
  double eta,double xfin,double &yfin,wchar_t *V){
 bool out=false; double x1,x2,x3,h,y1,y2,y3;
start: h=h_RK; xfin=x+h_RK; ss=0;
 lab1: if((x+2.01*h-xfin)>0){
        out=true; h=(xfin-x)/2;
       }
       rk1stepForIntg(x,y,2*h,x1,y1,f);
 lab2: rk1stepForIntg(x,y,h,x2,y2,f);
       rk1stepForIntg(x2,y2,h,x3,y3,f);
       if(comp(y1,y3,eta)>eps)
        goto lab3;
       x=x3;
       if(out)
        goto fin;
       y=y3;
       if(ss==5){
        ss=0; h*=2;
       }
       ss+=1; goto lab1;
 lab3: h*=0.5; out=false; x1=x2;
       y1=y; goto lab2;
 fin:  yfin=y3; h_RK=h; return true;
}

bool RUNGE(double x,double *y,int n,void (*f)(double x),double eps,double eta,
  double xfin,double *yfin){
 int k,ss,i=0; bool out; double x1,x2,x3,h;
 start:h=xfin-x; ss=0; out=false;
 lab1: if(((x+2.01*h-xfin)*h)>0) { out=true; h=(xfin-x)/2; }
       rk1step(n,x,y,2*h,x1,RK_y1,f);
       if(NoCalc){ if(i>=10) return false; NoCalc=false; h*=0.5; i++; ss=0; goto lab1; }
 lab2: rk1step(n,x,y,h,x2,RK_y2,f);
       if(NoCalc){ if(i>=10) return false; NoCalc=false; h*=0.5; i++; ss=0; goto lab1; }
       rk1step(n,x2,RK_y2,h,x3,RK_y3,f);
       if(NoCalc){ if(i>=10) return false; NoCalc=false; i++; goto lab3; }
       for(k=0;k<n;k++)
        if(comp(RK_y1[k],RK_y3[k],eta)>eps) goto lab3;
//	if(fabs(RK_y1[k]-RK_y3[k])>eps) goto lab3;
       x=x3; i=0;
       if(out) goto fin;
       for(k=0;k<n;k++) y[k]=RK_y3[k];
       if(ss==5) { ss=0; h*=2; }
       ss+=1; goto lab1;
 lab3: h*=0.5; out=false; x1=x2; //i++; if(i==10) return false;
       for(k=0;k<n;k++) RK_y1[k]=y[k]; goto lab2;
 fin:  for(k=0;k<n;k++) yfin[k]=RK_y3[k]; return true;
}

bool RUNGE_Ex(double x,double *y,int n,void (*f)(double x),double eps,double eta,
  double xfin,double *yfin){
 int k,ss,i=0; bool out; double x1,x2,x3;
 start:
  if(h_RK<0)
   h_RK=xfin-x;
  ss=0; out=false;
 lab1: if(((x+2.01*h_RK-xfin)*h_RK)>0){
        out=true;
        h_RK=(xfin-x)/2;
       }
       rk1step(n,x,y,2*h_RK,x1,RK_y1,f);
       if(NoCalc){
        if(i>=10) return false;
        NoCalc=false; h_RK*=0.5; i++; ss=0; goto lab1;
       }
 lab2: rk1step(n,x,y,h_RK,x2,RK_y2,f);
       if(NoCalc){
        if(i>=10) return false;
        NoCalc=false; h_RK*=0.5; i++; ss=0; goto lab1;
       }
       rk1step(n,x2,RK_y2,h_RK,x3,RK_y3,f);
       if(NoCalc){ if(i>=10) return false; NoCalc=false; i++; goto lab3; }
       for(k=0;k<n;k++)
//        if(comp(RK_y1[k],RK_y3[k],eta)>eps) goto lab3;
	if(fabs(RK_y1[k]-RK_y3[k])>eps) goto lab3;
       x=x3; i=0;
       if(out) goto fin;
       for(k=0;k<n;k++) y[k]=RK_y3[k];
       if(ss==5){
        ss=0; h_RK*=2;
       }
       ss+=1; goto lab1;
 lab3: h_RK*=0.5; out=false; x1=x2; //i++; if(i==10) return false;
       for(k=0;k<n;k++) RK_y1[k]=y[k]; goto lab2;
 fin:  for(k=0;k<n;k++) yfin[k]=RK_y3[k]; return true;
}

bool Adams(int &Nom,double x,double *y,void (*f)(double x),double Eps,
  double xfin,double *yfin){
// Nom - номер вызова (от 0), x - начальное значение аргумента,
// y - вектор начального состояния:
//     [N независимых обобщенных скоростей,
//      NPV псевдоскоростей,
//      N обобщенных координат с независимыми скоростями,
//      NzK обобщенных координат с зависимыми скоростями],
// xfin - конечное значение аргумента,
// yfin - вычисленный вектор конечного состояния = у(хfin),
// f - функция, вычисляющая по аргументу х вектор RK_z, содержащий:
//     [N-мерный вектор обобщенных ускорений координат с независимыми скоростями,
//      NPV-мерный вектор псевдоускорений,
//      N-мерный неиспользуемый вектор,
//      NzK-мерный вектор зависимых скоростей].
// Используем RK_y1=RK_z(-h), RK_y2=RK_z(-2h), RK_y3=RK_z(-3h).
 int n=Round(1.0/Eps),i,j,k=2*N+NPV+NzK,NC=0;
 double h;
M:h=(xfin-x)/n;
 for(i=0;i<k;i++) RK_w[i]=y[i];
 for(int I=0;I<n;I++){
  f(x);
  if(NoCalc){ if(NC==10) return false; NoCalc=false; n*=2; NC++; goto M; }
  x+=h; Nom=I+1; if(I){ for(i=0;i<k;i++) y[i]=RK_w[i]; }
  switch(Nom){
   case 1:
    for(i=0;i<N;i++){
     j=N+NPV+i;
     RK_w[i]=y[i]+RK_z[i]*h; RK_w[j]=y[j]+y[i]*h+RK_z[i]*h*h/2;
    }
    for(i=0;i<NPV;i++){ j=N+i; RK_w[j]=y[j]+RK_z[j]*h; }
    for(i=0;i<NzK;i++){ j=2*N+NPV+i; RK_w[j]=y[j]+RK_z[j]*h; }
    for(i=0;i<k;i++) RK_y1[i]=RK_z[i];
    break;
   case 2:
    for(i=0;i<N;i++){
     j=N+NPV+i;
     RK_w[i]=y[i]+0.5*h*(3*RK_z[i]-RK_y1[i]);
     RK_w[j]=y[j]+y[i]*h+(h*h/6)*(4*RK_z[i]-RK_y1[i]);
    }
    for(i=0;i<NPV;i++){ j=N+i; RK_w[j]=y[j]+0.5*h*(3*RK_z[j]-RK_y1[j]); }
    for(i=0;i<NzK;i++){ j=2*N+NPV+i; RK_w[j]=y[j]+0.5*h*(3*RK_z[j]-RK_y1[j]); }
    for(i=0;i<k;i++){ RK_y2[i]=RK_y1[i]; RK_y1[i]=RK_z[i]; }
    break;
   case 3:
    for(i=0;i<N;i++){
     j=N+NPV+i;
     RK_w[i]=y[i]+(h/12.0)*(23.0*RK_z[i]-16.0*RK_y1[i]+5.0*RK_y2[i]);
     RK_w[j]=y[j]+y[i]*h+(h*h/24.0)*(19.0*RK_z[i]-10.0*RK_y1[i]+3.0*RK_y2[i]);
    }
    for(i=0;i<NPV;i++){
     j=N+i; RK_w[j]=y[j]+(h/12.0)*(23.0*RK_z[j]-16.0*RK_y1[j]+5.0*RK_y2[j]);
    }
    for(i=0;i<NzK;i++){
     j=2*N+NPV+i; RK_w[j]=y[j]+(h/12.0)*(23.0*RK_z[j]-16.0*RK_y1[j]+5.0*RK_y2[j]);
    }
    for(i=0;i<k;i++){ RK_y3[i]=RK_y2[i]; RK_y2[i]=RK_y1[i]; RK_y1[i]=RK_z[i]; }
    break;
   default:
    if(Nom>3){
     for(i=0;i<N;i++){
      j=N+NPV+i;
      RK_w[i]=y[i]+(h/24.0)*(55.0*RK_z[i]-59.0*RK_y1[i]+37.0*RK_y2[i]-9.0*RK_y3[i]);
      RK_w[j]=y[j]+y[i]*h+
       (h*h/360.0)*(323.0*RK_z[i]-264.0*RK_y1[i]+159.0*RK_y2[i]-38.0*RK_y3[i]);
     }
     for(i=0;i<NPV;i++){
      j=N+i;
      RK_w[j]=y[j]+(h/24.0)*(55.0*RK_z[j]-59.0*RK_y1[j]+37.0*RK_y2[j]-9.0*RK_y3[j]);
     }
     for(i=0;i<NzK;i++){
      j=2*N+NPV+i;
      RK_w[j]=y[j]+(h/24.0)*(55.0*RK_z[j]-59.0*RK_y1[j]+37.0*RK_y2[j]-9.0*RK_y3[j]);
     }
     for(i=0;i<k;i++){ RK_y3[i]=RK_y2[i]; RK_y2[i]=RK_y1[i]; RK_y1[i]=RK_z[i]; }
 }} }
 for(i=0;i<k;i++) yfin[i]=RK_w[i];
 return true;
}

bool Eiler(double x,double *y,void (*f)(double x),double Eps,double xfin,double *yfin){
// y - вектор начального состояния:
//     [N независимых обобщенных скоростей,
//      N обобщенных координат с независимыми скоростями],
// xfin - конечное значение аргумента,
// yfin - вычисленный вектор конечного состояния = у(хfin),
// f - функция, вычисляющая по аргументу х вектор RK_z, содержащий:
//     [N-мерный вектор обобщенных ускорений координат с независимыми скоростями,
//      N-мерный неиспользуемый вектор].
 int n=Round(1.0/Eps),i,j,k=2*N; double h;
M:h=(xfin-x)/n;
 for(i=0;i<k;i++) RK_w[i]=y[i];
 for(int I=0;I<n;I++){
  f(x);
  for(i=0;i<k;i++) y[i]=RK_w[i]+h*RK_z[i];
  for(i=0;i<k;i++) RK_w[i]=y[i];
 }
 for(i=0;i<k;i++) yfin[i]=y[i]; return true;
}

