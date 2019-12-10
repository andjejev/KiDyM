#include "CA.h"
#include <math.h>

extern wchar_t Inf[];
extern Cnst *Pust;
//---------------------------------------------------------------------------
//Расчет коэффициентов полиномов по статье Morisawa и др.
bool DefKoefMorisawa(int m,double *T,double ZC,
  double qC0,double vC0,double qC1,double vC1,double *qCOF,double *vCOF,int *Nj,
  double **A,double **a,double **D,double *B,double *X,bool CalcD,bool Usk){
//m - число участков;
//T[0:m+1] - массив моментов времени для границ интервалов,
//ZC - постоянная аппликата центра масс,
//qC0,vC0,qC1,vC1 - значения и скорость слева и справа для CОМ
//qCOF[0:m+1],vCOF[0:m+1] - массивы значений и скоростей реперных точек для COF
//Nj[0:m-1] - массив степеней полиномов по участкам
//A[0:Сумма_по_участкам(Nj+1)+2*m], a[0:Сумма_по_участкам(Nj+1)] - массивы для
//      коффициентов полиномов для СОМ и COF, в которых индекс равен показателю
//      степени, для COM еще 2 константы интегрирования по каждому участку
//D[0:2*m+Сумма(Nj+1)-1][0:2*m+Сумма(Nj+1)-1] - матрица СЛАУ,
//B[0:2*m+Сумма(Nj+1)-1] - вектор правой части,
//X[0:2*m+Сумма(Nj+1)-1] - вектор решения,
//CalcD - если = true, то считать D, иначе не считать (она уже посчитана)
//Элементы вектора решения X располагаем:
// A[0,0],A[0,1],...,A[0,Nj[0]],V[0],W[0], - первый участок
// A[1,0],A[1,1],...,A[1,Nj[1]],V[1],W[1], - второй участок
// ...
// A[m-1,0],A[m-1,1],...,A[m-1,Nj[m-1]],V[m-1],W[m-1] - последний участок
//Коэффициенты полиномов для COF располагаются в массиве
// a[0,0],a[0,1],...,a[0,Nj[0]] - первый участок
// a[1,0],a[1,1],...,a[1,Nj[1]] - второй участок
// ...
// a[m-1,0],a[m-1,1],...,a[m-1,Nj[m-1]] - последний участок
 double o2=9.81/ZC,o=sqrt(o2),K1,dT,*x,r,**G,*C; wchar_t Inf[256]; int i,j,di;
FILE *F=_wfopen(L"DB.txt",L"w");int N=0;
 if(CalcD){
//1.
  D[0][0]=1.0;
  D[0][Nj[0]+1]=1.0;
  B[0]=qC0; //начальное условие по COM
  D[1][1]=1.0;
  D[1][Nj[0]+2]=1.0;
  B[1]=vC0; //начальное условие по скорости COM
if(F){
 for(i=0;i<m;i++,N+=3) N+=Nj[i];
G=(double **)calloc(N,SzV); C=(double *)calloc(N,SzD);
for(int i=0;i<N;i++) G[i]=(double *)calloc(N,SzD);
 fwprintf(F,L"1. D[0][0]=%12.4f; D[0][%d]=%12.4f; B[0]=%12.4f\n",
  D[0][0],Nj[0]+1,D[0][Nj[0]+1],B[0]);
 fwprintf(F,L"   D[1][1]=%12.4f; D[1][%d]=%12.4f; B[1]=%12.4f\n",
  D[1][1],Nj[0]+2,D[1][Nj[0]+2],B[1]);
}
//2.
fwprintf(F,L"2.\n");
  for(j=1,di=0;j<m;j++){
   K1=1.0; dT=T[j]-T[j-1];
   for(i=0;i<=Nj[j-1];){
    D[2*j][i+di]=K1;
fwprintf(F,L"D[%d][%d]=%8.4f\n",2*j,i+di,D[2*j][i+di]);
    D[2*j+1][i+di]=i*K1/dT;
fwprintf(F,L"D[%d][%d]=%8.4f\n",2*j+1,i+di,D[2*j+1][i+di]);
    K1*=dT;
    i++;
   }
   D[2*j][i+di]=cosh(o*dT);
fwprintf(F,L"D[%d][%d]=%8.4f\n",2*j,i+di,D[2*j][i+di]);
   D[2*j+1][i+di]=o*sinh(o*dT);
fwprintf(F,L"D[%d][%d]=%8.4f\n",2*j+1,i+di,D[2*j+1][i+di]);
   i++;
   D[2*j][i+di]=sinh(o*dT);
fwprintf(F,L"D[%d][%d]=%8.4f\n",2*j,i+di,D[2*j][i+di]);
   D[2*j+1][i+di]=o*cosh(o*dT);
fwprintf(F,L"D[%d][%d]=%8.4f\n",2*j+1,i+di,D[2*j+1][i+di]);
   i++;
   D[2*j][i+di]=-1.0;
fwprintf(F,L"D[%d][%d]=%8.4f\n",2*j,i+di,D[2*j][i+di]);
   D[2*j][i+di+Nj[j]+1]=-1.0;
fwprintf(F,L"D[%d][%d]=%8.4f\n",2*j,i+di+Nj[j]+1,D[2*j][i+di+Nj[j]+1]);
   i++;
   D[2*j+1][i+di]=-1.0;
fwprintf(F,L"D[%d][%d]=%8.4f\n",2*j+1,i+di,D[2*j+1][i+di]);
   D[2*j+1][i+di+Nj[j]+1]=-o;
fwprintf(F,L"D[%d][%d]=%8.4f\n",2*j+1,i+di+Nj[j]+1,D[2*j+1][i+di+Nj[j]+1]);
   di+=Nj[j-1]+3;
  }
//3.
fwprintf(F,L"3.\n");
  K1=1.0; dT=T[j]-T[j-1];
  for(i=0;i<=Nj[j-1];i++){
   D[2*j][i+di]=K1;
fwprintf(F,L"D[%d][%d]=%8.4f\n",2*j,i+di,D[2*j][i+di]);
   D[2*j+1][i+di]=i*K1/dT;
fwprintf(F,L"D[%d][%d]=%8.4f\n",2*j+1,i+di,D[2*j+1][i+di]);
   K1*=dT;
  }
  B[2*j]=qC1;
  B[2*j+1]=vC1;
fwprintf(F,L"B[%d]=%8.4f; B[%d]=%8.4f\n",2*j,B[2*j],2*j+1,B[2*j+1]);
  D[2*j][i+di]=cosh(o*dT);
fwprintf(F,L"D[%d][%d]=%8.4f\n",2*j,i+di,D[2*j][i+di]);
  D[2*j+1][i+di]=o*sinh(o*dT);
fwprintf(F,L"D[%d][%d]=%8.4f\n",2*j+1,i+di,D[2*j+1][i+di]);
  i++;
  D[2*j][i+di]=sinh(o*dT);
fwprintf(F,L"D[%d][%d]=%8.4f\n",2*j,i+di,D[2*j][i+di]);
  D[2*j+1][i+di]=o*cosh(o*dT);
fwprintf(F,L"D[%d][%d]=%8.4f\n",2*j+1,i+di,D[2*j+1][i+di]);
  i++;
//4.
fwprintf(F,L"4.\n");
  for(j=0,di=0;j<m;j++){
   D[2*(m+j+1)][di]    =1.0;
fwprintf(F,L"D[%d][%d]=%8.4f\n",2*(m+j+1),di,D[2*(m+j+1)][di]);
   D[2*(m+j+1)][2+di]  =-2.0/o2;
fwprintf(F,L"D[%d][%d]=%8.4f\n",2*(m+j+1),2+di,D[2*(m+j+1)][2+di]);
   B[2*(m+j+1)]=qCOF[j];
   D[2*(m+j+1)+1][1+di]=1.0;
fwprintf(F,L"D[%d][%d]=%8.4f\n",2*(m+j+1)+1,1+di,D[2*(m+j+1)+1][1+di]);
   D[2*(m+j+1)+1][3+di]=-6.0/o2;
fwprintf(F,L"D[%d][%d]=%8.4f\n",2*(m+j+1)+1,3+di,D[2*(m+j+1)+1][3+di]);
   B[2*(m+j+1)+1]=vCOF[j];
fwprintf(F,L"B[%d]=%8.4f; B[%d]=%8.4f\n",2*(m+j+1),B[2*(m+j+1)],2*(m+j+1)+1,B[2*(m+j+1)+1]);
   di+=Nj[j]+3;
  }
//5.
fwprintf(F,L"5.\n");
  for(j=0,di=0;j<m;j++){
   dT=T[j+1]-T[j];
   for(i=0;i<=Nj[j];i++){
    D[2*(2*m+j+1)][i+di]=pow(dT,i)-i*(i-1)*pow(dT,i-2)/o2;
fwprintf(F,L"D[%d][%d]=%8.4f\n",2*(2*m+j+1),i+di,D[2*(2*m+j+1)][i+di]);
    D[2*(2*m+j+1)+1][i+di]=i*(pow(dT,i-1)-(i-1)*(i-2)*pow(dT,i-3)/o2);
fwprintf(F,L"D[%d][%d]=%8.4f\n",2*(2*m+j+1)+1,i+di,D[2*(2*m+j+1)+1][i+di]);
   }
   B[2*(2*m+j+1)]=qCOF[j+1];
   B[2*(2*m+j+1)+1]=vCOF[j+1];
fwprintf(F,L"B[%d]=%8.4f; B[%d]=%8.4f\n",2*(2*m+j+1),B[2*(2*m+j+1)],
 2*(2*m+j+1)+1,B[2*(2*m+j+1)+1]);
   di+=Nj[j]+3;
 }}
fwprintf(F,L"\nD=L");
 for(i=0;i<N;i++){
  fwprintf(F,L"\n");
  for(j=0;j<N;j++)
   fwprintf(F,L"%12.4f",D[i][j]);
 }
 fwprintf(F,L"\nB=L");
 for(j=0;j<N;j++)
  fwprintf(F,L"%12.4f",B[j]);
 fwprintf(F,L"\n");
// if(!SLAU(32,D,B,X,false)){
 for(i=0;i<N;i++){
  C[i]=B[i];
  for(j=0;j<N;j++)
   G[i][j]=D[i][j];
 }
 if(!Crout(32,D,X,B,1e-9)){
  return false;
 }
 fwprintf(F,L"Проверка:\n");
 for(i=0;i<N;i++){
  r=0.0;
  for(j=0;j<N;j++)
   r+=G[i][j]*X[j];
  r-=C[i];
  fwprintf(F,L"G[%d]*X-C[%d]=%g; ",i,i,r);
 }
 fputwc('\n',F);
 x=X;
 fwprintf(F,L"Решение:\n");
 for(int j=0;j<m;j++){
  for(i=0;i<=Nj[j];i++){
   fwprintf(F,L"A[%d][%d]=%8.4f, ",j,i,*x);
   A[j][i]=*x++;
  }
  fwprintf(F,L"V[%d]=%8.4f, ",j,*x);
  A[j][i++]=*x++;
  fwprintf(F,L"W[%d]=%8.4f\n",j,*x);
  A[j][i++]=*x++;
  for(i=0;i<Nj[j]-1;i++){
   a[j][i]=A[j][i]-(i+1)*(i+2)*A[j][i+2]/o2;
   fwprintf(F,L"a[%d][%d]=%8.4f, ",j,i,a[j][i]);
  }
  a[j][i]=A[j][i];
  fwprintf(F,L"a[%d][%d]=%8.4f, ",j,i,a[j][i]);
  i++;
  a[j][i]=A[j][i];
  fwprintf(F,L"a[%d][%d]=%8.4f\n",j,i,a[j][i]);
 }
 fclose(F);
 return true;
}

//Расчет коэффициентов полиномов 7-й степени
bool DefKoef(double dt,double ZC,
  double dq,double *a,double *alfa,
  double **D,double *B,double *X,bool CalcD){
//  double qCOF0,double vCOF0,double qCOF1,
//  double qC0,  double vC0,  double aC0,
//  double qC1,  double vC1,  double aC1,double *a,double *alfa,
//  double **D,double *B,double *X){
//dt - временной интервал полиномиального представления,
//ZC - постоянная аппликата центра масс,
//qCOF0,vCOF0,qCOF1 - значение и скорость слева и значение справа для COF
//qC0,vC0,aC0,qC1,vC1,aC1 - значения, скорость, ускорение слева и справа для CОМ
//a,alfa - массивы размера 8 для результатов - коффициентов полиномов для СОМ и
//         COF, в которых индекс равен показателю степени
//D,B,X - рабочие массивы размерностью [0:3][0:3], [0:3], [0:3]
//CalcD - если = true, то считать D, иначе не считать
 double k2=ZC/9.81; wchar_t Inf[256];
 if(CalcD){
  D[0][0]=pow(dt,5)*(dt*dt-42*k2); D[0][1]=pow(dt,4)*(dt*dt-30*k2);
  D[0][2]=pow(dt,3)*(dt*dt-20*k2); D[0][3]=dt*dt*(dt*dt-12*k2);
  D[1][0]=pow(dt,7);               D[1][1]=pow(dt,6);
  D[1][2]=pow(dt,5);               D[1][3]=pow(dt,4);
  D[2][0]=7*pow(dt,6);             D[2][1]=6*pow(dt,5);
  D[2][2]=5*pow(dt,4);             D[2][3]=4*dt*dt*dt;
  D[3][0]=42*pow(dt,5);            D[3][1]=30*pow(dt,4);
  D[3][2]=20*dt*dt*dt;             D[3][3]=12*dt*dt;
 }
 B[3]=B[2]=0; B[1]=B[0]=dq;
// if(!Crout(4,D,X,B,1e-9)){
 if(!SLAU(4,D,X,B,false)){
  swprintf(Inf,L"Cистема уравнений не имеет решения!");
  return false;
 }
 for(int i=0;i<4;i++){ alfa[7-i]=X[i]; }
 alfa[0]=0.0;  alfa[1]=0.0;  alfa[3]=(alfa[1]-a[1])/(6*k2); alfa[2]=0.0;
 a[7]=alfa[7]; a[6]=alfa[6]; a[5]=alfa[5]-42*k2*alfa[7];    a[4]=alfa[4]-30*k2*alfa[6];
 a[3]=alfa[3]-20*k2*alfa[5]; a[2]=alfa[2]-12*k2*alfa[4];    a[1]=0.0; a[0]=0.0;
 return true;
}

//Расчет значений полинома 7-й степени
double CalcValue(double t,double  *a){
//t - относительное время, осчитываемое от левого конца интервала разложения
//a - массив коэффициентов полинома, в котором индекс равен показателю степени
 double Rez=a[0]+a[1]*t+a[2]*t*t;
 for(int i=3;i<8;i++) Rez+=a[i]*pow(t,i);
 return Rez;
}

double Rest(double Ch, double Zn){
 return Ch-Zn*floor(Ch/Zn);
}

bool PrepCalcCOFCOM(double L,double V,double Kbeg,double Kdsp,double Kdel,
  double ZC,double F,double A,double E,double H,double **a,double **alfa,
  double &Tstp,double &Tbeg,double &Tdsp,double &Tdel){ bool Rez=true;
//a и alfa - массивы коэффициентов полинома:
//           [0] и [1] - для абсцисс, [2],[3],[4] - для ординат
 double Dr[16],B[4],X[4],T12=Tstp-Tdsp,T23=Tdsp,Tds2=0.5*Tdsp,T01=Tds2,
  **D=(double **)calloc(4,SzD);
 Tstp=L/V; Tbeg=Kbeg*Tstp; Tdsp=Kdsp*Tstp; Tdel=Kdel*Tstp; double YR6=-0.5*A;
 D[0]=Dr; D[1]=Dr+4; D[2]=Dr+8; D[3]=Dr+12;
 if(!DefKoef(T12,ZC,  E-H,a[0],alfa[0],D,B,X,true)||
    !DefKoef(T01,ZC,  YR6,a[2],alfa[2],D,B,X,true)||
    !DefKoef(T23,ZC,L-E+H,a[1],alfa[1],D,B,X,true)||
    !DefKoef(T23,ZC,    A,a[3],alfa[3],D,B,X,false)) Rez=false;
 free(D);
 return Rez;
}

void CalcCOFCOM(double t,double &Tstp,double &Tbeg,double &Tdsp,
  double L,double F,double A,double E,double H,
  double &Xcof,double &Ycof,double &Xcom,double &Ycom,double **a,double **alfa){
 double Tds2=0.5*Tdsp,Tst2=2.0*Tstp,L2=2.0*L,Ch=t-Tbeg-Tds2,tr2=Rest(Ch,Tst2),
  x0=L2*floor(Ch/Tst2),   x1=E-H+L2*floor(Ch/Tst2),
  x2=L+L2*floor(Ch/Tst2), x3=x1+L,
  tau01x=tr2, tau12x=tr2-Tstp+Tdsp, tau23x=tr2-Tstp, tau34x=tr2-Tst2+Tdsp,
  y0=0, y1=-0.5*A, y2=y1, y3=0.5*A, y4=y3,
  tau01y=t-Tbeg, tau23y=tr2-Tstp+Tdsp, tau45y=tr2-Tst2+Tdsp;

 Xcof=t<=Tbeg+Tds2   ? x0 :
      tr2<=Tstp-Tdsp ? CalcValue(tau01x,a[0])+x0 :
      tr2<=Tstp      ? CalcValue(tau12x,a[1])+x1 :
      tr2<=Tst2-Tdsp ? CalcValue(tau23x,a[0])+x2 :
                       CalcValue(tau34x,a[1])+x3;
 Xcom=t<=Tbeg+Tds2   ? x0 :
      tr2<=Tstp-Tdsp ? CalcValue(tau01x,alfa[0])+x0 :
      tr2<=Tstp      ? CalcValue(tau12x,alfa[1])+x1 :
      tr2<=Tst2-Tdsp ? CalcValue(tau23x,alfa[0])+x2 :
                       CalcValue(tau34x,alfa[1])+x3;
 Ycof=t<=Tbeg        ? y0 :
      t<=Tbeg+Tds2   ? CalcValue(tau01y,a[2])+y0 :
      tr2<=Tstp-Tdsp ? y2 :
      tr2<=Tstp      ? CalcValue(tau23y,a[3])+y2 :
      tr2<=Tst2-Tdsp ? y3 :
                      -CalcValue(tau45y,a[3])+y4;
 Ycom=t<=Tbeg        ? y0 :
      t<=Tbeg+Tds2   ? CalcValue(tau01y,alfa[2])+y0 :
      tr2<=Tstp-Tdsp ? y2 :
      tr2<=Tstp      ? CalcValue(tau23y,alfa[3])+y1 :
      tr2<=Tst2-Tdsp ? y3 :
                      -CalcValue(tau45y,alfa[3])+y4;
}
//---------------------------------------------------------------------------
bool Kraut (Parm *Par,TRichEdit *R,TCanvas *Canvas){
 int N; double **A,*P,*X; Arra *D,*B,*V;
 for(;Par;Par=Par->Sled){
  LowToUpp(Par->Nam);
  if(!wcscmp(Par->Nam,L"МАТРИЦА")){
   FindArra(Par->R->Nam,&D);
  }
  else if(!wcscmp(Par->Nam,L"ПРАВАЯ ЧАСТЬ")){
   FindArra(Par->R->Nam,&B);
  }
  else if(!wcscmp(Par->Nam,L"РЕШЕНИЕ")){
   FindArra(Par->R->Nam,&V);
  }
  else{
   swprintf(Inf,L"Оператор решения системы линейных уравнений\n"
    L"записывается в следующем виде:\n"
    L"Краут : Матрица(<Имя_массива_матрицы_левой_части>),\n"
    L"  Правая часть(<Имя_массива_вектора_правой_части>),\n"
    L"  Решение(<Имя_массива_вектора_решения>);,\n"
    L"где <Массив_матрицы_левой_части> должен быть\n"
    L" заполнен формулами или значениями,\n"
    L" <Массив_вектора_правой_части> должен содержать формулы или значения,\n"
    L" <Массив_вектора_решения> должен быть заполнен именами переменных -\n"
    L" получателей значений вектора решения.");
   Application->MessageBoxA(Inf,L" ОШИБКА! ",MB_OK);
   return false;
 }}
 if(B->N==V->N&&(B->N*B->N==D->N))
  N=B->N;
 else{
  swprintf(Inf,L"В операторе КРАУТ не согласованы массивы");
  Application->MessageBoxA(Inf,L" ОШИБКА! ",MB_OK);
  return false;
 }
 A=(double **)calloc(N,SzV);
 P=(double *)calloc(N,SzD); X=(double *)calloc(N,SzD);
 for(int i=0;i<N;i++)
  A[i]=(double *)calloc(N,SzD);
 ClearVar();
 for(int i,j,k=0;k<D->N;k++){
  i=k/N; j=k%N;
  if(D->F&&(D->F[k].C!=Pust))
   A[i][j]=Val(D->F[k]);
 }
 for(int i=0;i<N;i++){
  if(B->F&&(B->F[i].C!=Pust))
   P[i]=Val(B->F[i]);
 }
 if(!SLAU(N,A,P,X,false)){
  swprintf(Inf,L"В операторе КРАУТ система уравнений не имеет решения!");
  Application->MessageBoxA(Inf,L" ОШИБКА! ",MB_OK);
  for(int i=0;i<N;i++)
   free(A[i]);
  free(A); free(P); free(X);
  return false;
 }
 for(int i=0;i<V->N;i++){
  if(V->F[i].V) { V->F[i].V->P=0; V->F[i].V->Val=X[i]; }
 }
// for(int i=0;i<N;i++)
//  free(A[i]);
 free(A);
 free(P);
 free(X);
 return true;
}
//---------------------------------------------------------------------------
bool Polinom (Parm *Par,TRichEdit *R,TCanvas *Canvas){
 int N,NL,NR; double T,**A,*PL,*PR,*X; Arra *a; Vary *V;
 ClearVar();
 for(;Par;Par=Par->Sled){
  LowToUpp(Par->Nam);
  if(!wcscmp(Par->Nam,L"СТЕПЕНЬ")){
   if(Par->R->Nam){
    if(FindVary(Par->R->Nam,&V))
     N=Round(Val(V->Znach));
   }
   else if(Par->R->F.C&&(Par->R->F.C!=Pust))
    N=Round(Val(Par->R->F));
   else N=Round(Par->R->Val);
  }
  else if(!wcscmp(Par->Nam,L"ВРЕМЕННОЙ ИНТЕРВАЛ")||!wcscmp(Par->Nam,L"ИНТЕРВАЛ")){
   if(Par->R->Nam){
    if(FindVary(Par->R->Nam,&V))
     T=Val(V->Znach);
    else if(Par->R->F.C&&(Par->R->F.C!=Pust))
     T=Val(Par->R->F);
    else T=Par->R->Val;
  }}
  else if(!wcscmp(Par->Nam,L"СЛЕВА")){ int i=0;
   NL=KolElem(Par->R);
   PL=(double *)calloc(NL,SzD);
   for(Parm *P=Par->R;P;i++,P=P->Sled){
    if(P->Nam){
     if(FindVary(P->Nam,&V))
      PL[i]=Val(V->Znach);
    }
    else if(P->F.C&&(P->F.C!=Pust))
     PL[i]=Val(P->F);
    else PL[i]=P->Val;
  }}
  else if(!wcscmp(Par->Nam,L"СПРАВА")){ int i=0;
   NR=KolElem(Par->R);
   PR=(double *)calloc(NR,SzD);
   for(Parm *P=Par->R;P;i++,P=P->Sled){
    if(P->Nam){
     if(FindVary(P->Nam,&V))
      PR[i]=Val(V->Znach);
    }
    else if(P->F.C&&(P->F.C!=Pust))
     PR[i]=Val(P->F);
    else PR[i]=P->Val;
   }
   X=(double *)calloc(NR,SzD);
   A=(double **)calloc(NR,SzV);
   for(i=0;i<NR;i++) A[i]=(double *)calloc(NR,SzD);
  }
  else if(!wcscmp(Par->Nam,L"КОЭФФИЦИЕНТЫ")){
   FindArra(Par->R->Nam,&a);
  }
  else{
   swprintf(Inf,L"Оператор построения полинома\n"
    L"записывается в следующем виде:\n"
    L"Полином : Степень(<Число>|<Имя_переменной>|<Формула>),\n"
    L"  [Временной ]интервал(<Число>|<Имя_переменной>|<Формула>),\n"
    L"  Слева(<Список_из_чисел|имен_переменных|формул>),\n"
    L"  Справа(<Список_из_чисел|имен_переменных|формул>),\n"
    L"  Коэффициенты(<Имя_массива_коэффициентов>);,\n"
    L"где Степень содержит степень полинома\n"
    L"    [Временной ]интервал - область определения аргумента,\n"
    L"    Слева - массив из значения функции и ее производных на левом конце,\n"
    L"    Справа - массив из значения функции и ее производных на правом конце,\n"
    L"    Коэффициенты - массив для коэффициентов полинома, индекс\n"
    L"     в этом массиве равен степени переменной полинома.");
   Application->MessageBoxA(Inf,L" ОШИБКА! ",MB_OK);
   return false;
 }}
 a->A[0]=PL[0]; a->A[1]=PL[1];
 for(int i=2;i<NL;i++){
  a->A[i]=PL[i];
  for(int j=i;j<NL;j++)
   a->A[i]/=j;
 }
 for(int i=0;i<NR;i++){
  for(int j=0;j<NR;j++){
   A[i][j]=pow(T,N-i-NR+1+j);
   for(int k=0;k<i;k++)
    A[i][j]*=N-NR+j+1-k;
 }}
 if(!SLAU(NR,A,PR,X,false)){
  swprintf(Inf,L"Cистема уравнений не имеет решения!");
  return false;
 }
 for(int i=0;i<NR;i++){
  a->A[i+NL]=X[i];
  free(A[i]);
 }
 free(A); free(PL); free(PR); free(X);
 return true;
}
//---------------------------------------------------------------------------
bool PolinomsCOFCOM(Parm *Par,TRichEdit *R,TCanvas *Canvas){
 int N,NLCOF,NRCOF,NLCOM,NRCOM;
 double T,*PLCOF,*PRCOF,*PLCOM,*PRCOM,*aF,*aM,ZC,**D,*B,*X;
 Arra *aCOF,*aCOM; Vary *V;
 aF=(double *)calloc(8,SzD); aM=(double *)calloc(8,SzD);
 B=(double *)calloc(4,SzD);   X=(double *)calloc(4,SzD);
 D=(double **)calloc(4,SzV);
 for(int i=0;i<4;i++) D[i]=(double *)calloc(4,SzD);
 ClearVar();
//Полиномы COF и COM : Степень(7),Временной интервал(dtx1),СлеваCOF(0,0),
//СправаCOF(x1),СлеваCOM(0,0,0),СправаCOM(x1,0,0),
// Коэффициенты(ax1,ax1cm);
 for(;Par;Par=Par->Sled){
  LowToUpp(Par->Nam);
  if(!wcscmp(Par->Nam,L"СТЕПЕНЬ")){
   if(Par->R->Nam){
    if(FindVary(Par->R->Nam,&V))
     N=Round(Val(V->Znach));
   }
   else if(Par->R->F.C&&(Par->R->F.C!=Pust))
    N=Round(Val(Par->R->F));
   else N=Round(Par->R->Val);
  }
  else if(!wcscmp(Par->Nam,L"ВРЕМЕННОЙ ИНТЕРВАЛ")||!wcscmp(Par->Nam,L"ИНТЕРВАЛ")){
   if(Par->R->Nam){
    if(FindVary(Par->R->Nam,&V))
     T=Val(V->Znach);
    else if(Par->R->F.C&&(Par->R->F.C!=Pust))
     T=Val(Par->R->F);
    else T=Par->R->Val;
  }}
  else if(!wcscmp(Par->Nam,L"ВЫСОТА COM")){
   if(Par->R->Nam){
    if(FindVary(Par->R->Nam,&V))
     ZC=Val(V->Znach);
    else if(Par->R->F.C&&(Par->R->F.C!=Pust))
     ZC=Val(Par->R->F);
    else ZC=Par->R->Val;
  }}
  else if(!wcscmp(Par->Nam,L"СЛЕВАCOF")){ int i=0;
   NLCOF=KolElem(Par->R);
   PLCOF=(double *)calloc(NLCOF,SzD);
   for(Parm *P=Par->R;P;i++,P=P->Sled){
    if(P->Nam){
     if(FindVary(P->Nam,&V))
      PLCOF[i]=Val(V->Znach);
    }
    else if(P->F.C&&(P->F.C!=Pust))
     PLCOF[i]=Val(P->F);
    else PLCOF[i]=P->Val;
  }}
  else if(!wcscmp(Par->Nam,L"СПРАВАCOF")){ int i=0;
   NRCOF=KolElem(Par->R);
   PRCOF=(double *)calloc(NRCOF,SzD);
   for(Parm *P=Par->R;P;i++,P=P->Sled){
    if(P->Nam){
     if(FindVary(P->Nam,&V))
      PRCOF[i]=Val(V->Znach);
    }
    else if(P->F.C&&(P->F.C!=Pust))
     PRCOF[i]=Val(P->F);
    else PRCOF[i]=P->Val;
  }}
  else if(!wcscmp(Par->Nam,L"СЛЕВАCOM")){ int i=0;
   NLCOM=KolElem(Par->R);
   PLCOM=(double *)calloc(NLCOM,SzD);
   for(Parm *P=Par->R;P;i++,P=P->Sled){
    if(P->Nam){
     if(FindVary(P->Nam,&V))
      PLCOM[i]=Val(V->Znach);
    }
    else if(P->F.C&&(P->F.C!=Pust))
     PLCOM[i]=Val(P->F);
    else PLCOM[i]=P->Val;
  }}
  else if(!wcscmp(Par->Nam,L"СПРАВАCOM")){ int i=0;
   NRCOM=KolElem(Par->R);
   PRCOM=(double *)calloc(NRCOM,SzD);
   for(Parm *P=Par->R;P;i++,P=P->Sled){
    if(P->Nam){
     if(FindVary(P->Nam,&V))
      PRCOM[i]=Val(V->Znach);
    }
    else if(P->F.C&&(P->F.C!=Pust))
     PRCOM[i]=Val(P->F);
    else PRCOM[i]=P->Val;
  }}
  else if(!wcscmp(Par->Nam,L"КОЭФФИЦИЕНТЫ")){
   FindArra(Par->R->Nam,&aCOF);
   FindArra(Par->R->Sled->Nam,&aCOM);
  }
  else{
   swprintf(Inf,L"Оператор построения полиномов для COF и COM\n"
    L"записывается в следующем виде:\n"
    L"Полином : Степень(<Число>|<Имя_переменной>|<Формула>),\n"
    L"  [Временной ]интервал(<Число>|<Имя_переменной>|<Формула>),\n"
    L"  СлеваCOF(<Список_из_чисел|имен_переменных|формул>),\n"
    L"  СправаCOF(<Список_из_чисел|имен_переменных|формул>),\n"
    L"  СлеваCOM(<Список_из_чисел|имен_переменных|формул>),\n"
    L"  СправаCOM(<Список_из_чисел|имен_переменных|формул>),\n"
    L"  Коэффициенты(<Имя_массива_коэффициентов_для_COF>,L"
    L"    <Имя_массива_коэффициентов_для_COM>);,\n"
    L"где Степень содержит степень полинома - 7\n"
    L"    [Временной ]интервал - область определения аргумента,\n"
    L"    СлеваCOF - массив из значения функции COF и ее производных на левом конце,\n"
    L"    СправаCOF - массив из значения функции COF и ее производных на правом конце,\n"
    L"    СлеваCOM - массив из значения функции COM и ее производных на левом конце,\n"
    L"    СправаCOM - массив из значения функции COM и ее производных на правом конце,\n"
    L"    Коэффициенты - массив для коэффициентов полинома для COF и\n"
    L"     массив для коэффициентов полинома для COM,\n"
    L"    При этом индекс в массивах равен степени переменной полинома плюс 1.");
   Application->MessageBoxA(Inf,L" ОШИБКА! ",MB_OK);
   return false;
 }}
 if(DefKoef(T,ZC,PRCOF[0],aF,aM,D,B,X,true)){
// DefKoef(T,ZC,PLCOF[0],PLCOF[1],PRCOF[0],PLCOM[0],PLCOM[1],PLCOM[2],
//   PRCOM[0],PRCOM[1],PRCOM[2],aF,aM,D,B,X)){
  for(int i=0;i<8;i++){
   aCOF->A[i]=aF[i]; aCOM->A[i]=aM[i];
 }}
// free(PLCOF); free(PRCOF); free(PLCOM); free(PRCOM); free(aF); free(aM);
 return true;
}
//---------------------------------------------------------------------------
bool MorisawaCOFCOM(Parm *Par,TRichEdit *R,TCanvas *Canvas){
// Реперные точки(времена(T1,T2,T3,T4,T5),абсциссы COF(x1,x2,x3,x4,x5),
// скорости COF(0,v1,v1,v1,v1),cтепени(Nj[1],Nj[2],Nj[3],Nj[4])),Высота COM(ZC),
// СлеваCOM(x1,0),СправаCOM(x5,0),Коэффициенты COF(a1,a2,a3,a4),
// Коэффициенты COM(A1,A2,A3,A4);
 int *Nj,K,i,KxCOF,KvCOF,m,Kkf,Kkm,Sols,Urs; wchar_t ParName[16]; bool Usk;
 double *xCOF,*vCOF,xCOM0,vCOM0,xCOMk,vCOMk,**a,**A,**D,*B,*X,ZC,*T,d,dT,
  d4,d5,d6[4],d7[4],d8,d9,d10[5],d11[5],d12[5],d13[5],o2,o;
 Arra **aCOF,**aCOM,*Arr; Vary *V;
 ClearVar();
 for(;Par;Par=Par->Sled){
  wcscpy(ParName,Par->Nam);
  LowToUpp(Par->Nam);
  if(!wcscmp(Par->Nam,L"РЕПЕРНЫЕ ТОЧКИ")){
   for(Parm *P=Par->R;P;P=P->Sled){
    if(P->Nam){
     LowToUpp(P->Nam);
     if(!wcscmp(P->Nam,L"ВРЕМЕНА")){
      Parm *p;
      K=KolElem(P->R);
      T=(double *)calloc(K,SzD);
      for(p=P->R,i=0;p;p=p->Sled,i++){
       if(FindVary(p->Nam,&V)) T[i]=Val(V->Znach);
       else if(p->F.C&&(p->F.C!=Pust)) T[i]=Val(p->F);
       else T[i]=p->Val;
      }
     }
     else if(!wcscmp(P->Nam,L"АБСЦИССЫ COF")||!wcscmp(P->Nam,L"ОРДИНАТЫ COF")){
      Parm *p;
      KxCOF=KolElem(P->R);
      if(KxCOF!=K){
       swprintf(Inf,L"В операторе \"Полиномы Morisawa для COF и COM\"\n"
        L"размер массива заданных абсцисс или ординат COF\n"
        L"должен быть равным размеру массива времен!");
       Application->MessageBoxA(Inf,L" ОШИБКА! ",MB_OK);
       goto Ex;
      }
      xCOF=(double *)calloc(KxCOF,SzD);
      for(p=P->R,i=0;p;p=p->Sled,i++){
       if(FindVary(p->Nam,&V)) xCOF[i]=Val(V->Znach);
       else if(p->F.C&&(p->F.C!=Pust)) xCOF[i]=Val(p->F);
       else xCOF[i]=p->Val;
     }}
     else if(!wcscmp(P->Nam,L"СКОРОСТИ COF")){
      Parm *p;
      KvCOF=KolElem(P->R);
      if(KvCOF!=K){
       swprintf(Inf,L"В операторе \"Полиномы Morisawa для COF и COM\"\n"
        L"размер массива заданных скоростей COF должен быть\n"
        L"равным размеру массивов времен и абсцисс (ординат)!");
       Application->MessageBoxA(Inf,L" ОШИБКА! ",MB_OK);
       goto Ex;
      }
      vCOF=(double *)calloc(KvCOF,SzD);
      for(p=P->R,i=0;p;p=p->Sled,i++){
       if(FindVary(p->Nam,&V)) vCOF[i]=Val(V->Znach);
       else if(p->F.C&&(p->F.C!=Pust)) vCOF[i]=Val(p->F);
       else vCOF[i]=p->Val;
     }}
     else if(!wcscmp(P->Nam,L"СТЕПЕНИ")){
      Parm *p; int N=0; Arra *Ar=NULL;
      if(P->R->Nam&&FindArra(P->R->Nam,&Ar))
       m=Ar->N;
      else
       m=KolElem(P->R);
      if(m!=K-1){
       swprintf(Inf,L"В операторе \"Полиномы Morisawa для COF и COM\"\n"
        L"размер массива степеней полиномов участков должен быть на 1\n"
        L"меньше размеров массивов времен, абсцисс (ординат) и скоростей COF!");
       Application->MessageBoxA(Inf,L" ОШИБКА! ",MB_OK);
       goto Ex;
      }
      Nj=(int *)calloc(m,SzI);
      if(Ar){
       for(i=0;i<m;i++)
        Nj[i]=Ar->A?Ar->A[i]:Val(Ar->F[i]);
      }
      else{
       for(p=P->R,i=0;p;p=p->Sled,i++){
        if(FindVary(p->Nam,&V))
         Nj[i]=Round(Val(V->Znach));
        else if(p->F.C&&(p->F.C!=Pust))
         Nj[i]=Round(Val(p->F));
        else Nj[i]=Round(p->Val);
      }}
      for(int j=0;j<m;j++) N+=3+Nj[j];
      B=(double *)calloc(N,SzD); X=(double *)calloc(N,SzD);
      D=(double **)calloc(N,SzV);
	  A=(double **)calloc(m,SzV); a=(double **)calloc(m,SzV);
      for(int j=0;j<N;j++)
       D[j]=(double *)calloc(N,SzD);
      for(int j=0;j<m;j++){
       A[j]=(double *)calloc(Nj[j]+3,SzD);
       a[j]=(double *)calloc(Nj[j]+1,SzD);
  }}}}}
  else if(!wcscmp(Par->Nam,L"ВЫСОТА COM")){
   if(FindVary(Par->R->Nam,&V)) ZC=Val(V->Znach);
   else if(Par->R->F.C&&(Par->R->F.C!=Pust)) ZC=Val(Par->R->F);
   else ZC=Par->R->Val;
   o2=9.81/ZC; o=sqrt(o2);
  }
  else if(!wcscmp(Par->Nam,L"СЛЕВАCOM")){
   if(KolElem(Par->R)!=2){
    swprintf(Inf,L"В операторе \"Полиномы Morisawa для COF и COM\"\n"
     L"инструкция \"%s\" должна содержать 2 значения!",ParName);
    Application->MessageBoxA(Inf,L" ОШИБКА! ",MB_OK);
    goto Ex;
   }
   if(FindVary(Par->R->Nam,&V)) xCOM0=Val(V->Znach);
   else if(Par->R->F.C&&(Par->R->F.C!=Pust)) xCOM0=Val(Par->R->F);
   else xCOM0=Par->R->Val;
   if(FindVary(Par->R->Sled->Nam,&V)) vCOM0=Val(V->Znach);
   else if(Par->R->Sled->F.C&&(Par->R->Sled->F.C!=Pust))
    vCOM0=Val(Par->R->Sled->F);
   else vCOM0=Par->R->Sled->Val;
  }
  else if(!wcscmp(Par->Nam,L"СПРАВАCOM")){
   if(KolElem(Par->R)!=2){
    swprintf(Inf,L"В операторе \"Полиномы Morisawa для COF и COM\"\n"
     L"инструкция \"%s\" должна содержать 2 значения!",ParName);
    Application->MessageBoxA(Inf,L" ОШИБКА! ",MB_OK);
    goto Ex;
   }
   if(FindVary(Par->R->Nam,&V)) xCOMk=Val(V->Znach);
   else if(Par->R->F.C&&(Par->R->F.C!=Pust)) xCOMk=Val(Par->R->F);
   else xCOMk=Par->R->Val;
   if(FindVary(Par->R->Sled->Nam,&V)) vCOMk=Val(V->Znach);
   else if(Par->R->Sled->F.C&&(Par->R->Sled->F.C!=Pust))
    vCOMk=Val(Par->R->Sled->F);
   else vCOMk=Par->R->Sled->Val;
  }
  else if(!wcscmp(Par->Nam,L"КОЭФФИЦИЕНТЫ COF")){
   Parm *p; int j;
   Kkf=KolElem(Par->R);
   if(Kkf!=m){
    swprintf(Inf,L"В операторе \"Полиномы Morisawa для COF и COM\"\n"
     L"размер массива коэффициентов COM должен быть\n"
     L"равным размеру массива степеней (числу участков)!");
    Application->MessageBoxA(Inf,L" ОШИБКА! ",MB_OK);
    goto Ex;
   }
   aCOF=(Arra **)calloc(Kkf,SzV);
   for(p=Par->R,j=0;p;p=p->Sled,j++){
    if(FindArra(p->Nam,&Arr)){
     aCOF[j]=Arr;
     if(aCOF[j]->N<(Nj[j]+1)){
      if(aCOF[j]->A)
       aCOF[j]->A=(double *)realloc(aCOF[j]->A,(Nj[j]+1)*SzD);
      else
       aCOF[j]->A=(double *)calloc(Nj[j]+1,SzD);
    }}
    else{
     aCOF[j]=TakeArra(p->Nam);
     aCOF[j]->A=(double *)calloc(Nj[j]+1,SzD);
    }
    aCOF[j]->N=Nj[j]+1;
  }}
  else if(!wcscmp(Par->Nam,L"КОЭФФИЦИЕНТЫ COM")){
   Parm *p; int j;
   Kkm=KolElem(Par->R);
   if(Kkm!=m){
    swprintf(Inf,L"В операторе \"Полиномы Morisawa для COF и COM\"\n"
     L"размер массива коэффициентов COM должен быть\n"
     L"равным размеру массива степеней (числу участков)!");
    Application->MessageBoxA(Inf,L" ОШИБКА! ",MB_OK);
    goto Ex;
   }
   aCOM=(Arra **)calloc(Kkm,SzV);
   for(p=Par->R,j=0;p;p=p->Sled,j++){
    if(FindArra(p->Nam,&Arr)){
     aCOM[j]=Arr;
     if(aCOM[j]->N<(Nj[j]+3))
      if(aCOM[j]->A)
       aCOM[j]->A=(double *)realloc(aCOM[j]->A,(Nj[j]+3)*SzD);
      else
       aCOM[j]->A=(double *)calloc(Nj[j]+3,SzD);
    }
    else{
     aCOM[j]=TakeArra(p->Nam);
     aCOM[j]->A=(double *)calloc(Nj[j]+3,SzD);
    }
    aCOM[j]->N=Nj[j]+3;
  }}
  else{
Ex:swprintf(Inf,L"Оператор построения полиномов для COF и COM\n"
    L"по статье Морисавы записывается в следующем виде:\n"
    L"Полиномы Morisawa для COF и COM :\n"
    L" Реперные точки(времена(<Список_из_чисел|имен_переменных|формул>),\n"
    L"  абсциссы COF(<Список_из_чисел|имен_переменных|формул>),\n"
    L"  скорости COF(<Список_из_чисел|имен_переменных|формул>),\n"
    L"  cтепени(<Имя_массива_или_Список_из_чисел|имен_переменных|формул>)),\n"
    L" Высота COM(<Высота центра масс>),\n"
    L" СлеваCOM(<Координата и скорость центра масс на левом конце>),\n"
    L" СправаCOM(<Координата и скорость центра масс на правом конце>),\n"
    L" Коэффициенты COF(<Имена массивов для коэффициентов полиномов COF>),\n"
    L" Коэффициенты COM(<Имена массивов для коэффициентов полиномов COM));\n"
    L"    При этом индекс в массивах равен степени переменной полинома плюс 1.\n"
    L"    Для центра масс здесь еще в конце 2 константы интегрирования");
   Application->MessageBoxA(Inf,L" ОШИБКА! ",MB_OK);
   return false;
 }}
 Sols=6*m+2; Urs=2*m;
 for(int j=0;j<m;j++) Urs+=Nj[j]+1;
 if(Sols==Urs) Usk=false;
 else if(Sols+2*m==Urs) Usk=true;
 else{
  swprintf(Inf,L"Оператор построения полиномов для COF и COM\n"
              L"по статье Морисавы, обнаружил несовпадение\n"
              L"числа уравнений числу искомых коэффициентов.\n");
  Application->MessageBoxA(Inf,L" ОШИБКА! ",MB_OK);
  return false;
 }
 if(!DefKoefMorisawa(K-1,T,ZC,xCOM0,vCOM0,xCOMk,vCOMk,xCOF,vCOF,Nj,A,a,D,B,X,true,Usk)){
  swprintf(Inf,L"Матрица системы уравнений вырожденная!");
  Application->MessageBoxA(Inf,L"Оператор \"Полиномы Morisawa для COF и COM\"",MB_OK);
  return false;
 }
//Проверка уравнений
//1.(4) и (5)
 d4=A[0][0]+A[0][5]-xCOM0;
 d5=A[0][1]+A[0][6]-vCOM0;
//2.(6) и (7)
 for(int j=0;j<m-1;){
  dT=T[j+1]-T[j];
  d=A[j][0];
  d6[j]=d;
  for(d7[j]=0.0,i=1;i<=Nj[j];i++){
   d=A[j][i]*pow(dT,i);
   d6[j]+=d;
   d=i*A[j][i]*pow(dT,i-1);
   d7[j]+=d;
  }
  d=A[j][i]*cosh(o*dT);
  d6[j]+=d;
  d=A[j][i]*o*sinh(o*dT);
  d7[j]+=d;
  i++;
  d=A[j][i]*sinh(o*dT);
  d6[j]+=d;
  d=A[j][i]*o*cosh(o*dT);
  d7[j]+=d;
  d6[j]-=A[j+1][0];
  d6[j]-=A[j+1][Nj[j+1]+1];
  d7[j]-=A[j+1][1];
  d7[j]-=o*A[j+1][Nj[j+1]+2];
  j++;
 }
//3.(8) и (9)
 d8=-xCOMk; d9=-vCOMk;
 dT=T[m]-T[m-1];
 d=A[m-1][0];
 d8+=d;
 for(i=1;i<=Nj[m-1];i++){
  d=A[m-1][i]*pow(dT,i);
  d8+=d;
  d=i*A[m-1][i]*pow(dT,i-1);
  d9+=d;
 }
 d=A[m-1][i]*cosh(o*dT);
 d8+=d;
 d=A[m-1][i]*o*sinh(o*dT);
 d9+=d;
 i++;
 d=A[m-1][i]*sinh(o*dT);
 d8+=d;
 d=A[m-1][i]*o*cosh(o*dT);
 d9+=d;
//4.(10) и (11)
//5.(12) и (13)
 for(int j=0;j<m;){
  dT=T[j+1]-T[j];
  d=A[j][0];
  d10[j]=d;
  d=2.0*A[j][2]/o2;
  d10[j]-=d;
  d=xCOF[j];
  d10[j]-=d;
  d=A[j][1];
  d11[j]=d;
  d=6.0*A[j][3]/o2;
  d11[j]-=d;
  d=vCOF[j];
  d11[j]-=d;
  d=A[j][0];
  d12[j]=d;
  d=2.0*A[j][2]/o2;
  d12[j]-=d;
  for(d13[j]=0.0,i=1;i<=Nj[j];i++){
   d=A[j][i]*pow(dT,i);
   d12[j]+=d;
   d=i*A[j][i]*pow(dT,i-1);
   d13[j]+=d;
   if(i<Nj[j]-1){
    d=(i+1)*(i+2)*A[j][i+2]*pow(dT,i)/o2;
    d12[j]-=d;
    d=i*(i+1)*(i+2)*A[j][i+2]*pow(dT,i-1)/o2;
    d13[j]-=d;
  }}
  d=xCOF[j+1];
  d12[j]-=d;
  d=vCOF[j+1];
  d13[j]-=d;
  j++;
 }
 for(int j=0;j<m;j++){ int i=0;
  for(;i<=Nj[j];i++){
   aCOF[j]->A[i]=a[j][i];
   aCOM[j]->A[i]=A[j][i];
  }
  aCOM[j]->A[i]=A[j][i];
  i++;
  aCOM[j]->A[i]=A[j][i];
 }
/*
  if(OPRFILE=_wfopen(OprFile,_waccess(OprFile,0)?L"w":L"a")){
   fwprintf(OPRFILE,
    L"<html>\n<head>\n<title>ПРОТОКОЛ ОПЕРАТОРА \"СТРУКТУРЫ\" строки %d</title>\n"
    L"<meta http-equiv=\"Content-Type\" content=\"text/html;"
    L"wchar_tset=windows-1251\">\n</head>\n"
    L"<body bgcolor=\"#FFFFFF\" text=\"#800000\""
    L" background=\"%s/fon2.jpg\""
    L" font face=\"Times New Roman, Times, serif\">\n"
    L"ПРОТОКОЛ ОПЕРАТОРА \"СТРУКТУРЫ\" строки %d<br>\n<br>\n",
     Line,WORKDIR,Line);
   swprintf(Inf,L"ПРОТОКОЛ ОПЕРАТОРА \"СТРУКТУРЫ\" строки %d\n",Line);
 //  RichEdit->Lines->Add(Inf);
   if(KolElem(R)>1){
    fwprintf(OPRFILE,
     L"Оператором созданы формулы координат точек:<br>\n");
 //   RichEdit->Lines->Add(L"Оператором созданы формулы координат точек:\n");
   }
   else{
    fwprintf(OPRFILE,
     L"Оператором созданы формулы координат точки:<br>\n");
 //   RichEdit->Lines->Add(L"Оператором созданы формулы координат точки:\n");
   }
   fputws(L"<table width=\"100%\" border=\"1\">\n",OPRFILE);
   fputws(L"<tr>",OPRFILE);
   fputws(L"<td><div align=\"center\"><b>Точка</b></div></td>\n"
    L"<td><div align=\"center\"><b>Координаты</b></div></td>\n",OPRFILE);
   fputws(L"</tr>",OPRFILE);
   for(Tochka *t=R;t;t=t->Sled){
    fwprintf(OPRFILE,L"<tr>\n <td>%s</td>\n<td>&nbsp;</td>\n",t->Name);
    fputws(L"<tr>\n",OPRFILE);
    fwprintf(OPRFILE,L" <td>&nbsp;</td>\n<td>");
    PrintForm(false,OPRFILE,t->V[0]->Name,t->V[0]->Znach);
    fputws(L"</td>\n</tr>\n",OPRFILE);
    fputws(L"<tr>\n",OPRFILE);
    fwprintf(OPRFILE,L" <td>&nbsp;</td>\n<td>");
    PrintForm(false,OPRFILE,t->V[1]->Name,t->V[1]->Znach);
    fputws(L"</td>\n</tr>\n",OPRFILE);
    if(_3D){
     fputws(L"<tr>\n",OPRFILE);
     fwprintf(OPRFILE,L" <td>&nbsp;</td>\n<td>");
     PrintForm(false,OPRFILE,t->V[2]->Name,t->V[2]->Znach);
     fputws(L"</td>\n</tr>\n",OPRFILE);
  }}}
  fputws(L"</table></font>",OPRFILE);
  fwprintf(OPRFILE,L"</body>\n</html>\n");
  fclose(OPRFILE);
*/
  return true;
}
//---------------------------------------------------------------------------

