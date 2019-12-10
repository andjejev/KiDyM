//---------------------------------------------------------------------------
#pragma hdrstop

#include "math.h"
#include "stdio.h"
#include <CA.h>

//---------------------------------------------------------------------------
#pragma package(smart_init)
extern List *L;
//---------------------------------------------------------------------------
void Furie(double **FF,int n,int Kt,int Kg,double **Ac,double **As,double *U){
//FF - массив [0:n-1][0:Kt-1] (Kt дискретных значений n переменных),
//Kg - число гармоник разложения ряда фурье,
//Ac,As - массивы [0:n-1][0:Kg] (0,1,...,Kg - е амплитуды гармоник для каждой
//из n переменных)
//U[0:Kt-1] - рабочий массив размера Kt
 double Max=FF[0][0],Min=Max;
 for(int j=1;j<Kt;j++){
  if(FF[0][j]>Max) Max=FF[0][j]; if(FF[0][j]<Min) Min=FF[0][j];
 }
 Max-=Min;
 if((fabs(FF[0][0]-FF[0][Kt-1])<1e-5*Max)&&
  fabs(FF[0][Kt-1]-FF[0][0])<1e-5*fabs(FF[0][1]-FF[0][Kt-2]))
  Kt--;//периодическая функция!
 for(int i=0;i<n;i++){
  double *ac=Ac[i],*as=As[i],*F=FF[i]; U[1]=F[Kt-1]; U[0]=0.0;
  for(int k=0;k<=Kg;k++,ac++,as++){
   double w=2*cos(M_PI*2*k/Kt);
   for(int j=2;j<Kt;j++) U[j]=w*U[j-1]-U[j-2]+F[Kt-j];
   *ac=(U[Kt-1]*w/2-U[Kt-2]+(*F))*2/Kt;
   if(k) *as=(U[Kt-1]*sin(M_PI*2*k/Kt))*2/Kt;
   else{ *ac/=2; *as=0; }
}}}
//---------------------------------------------------------------------------
double Round (double D,double R) {
 return R*(D>=0.0?floor(D/R+0.5):ceil(D/R-0.5));
}
//---------------------------------------------------------------------------
double Round (double D) { return Round(D,1.0); }
//---------------------------------------------------------------------------
bool COFtoCOM_Furie(double Xc0,double Yc0,double Zc,double dt,int Kg,
  int Kp,double *TcofP,double *Xcof,double *Ycof,double *Xcom,double *Ycom,
  double *Vxcom,double *Vycom,double *Axcom,double *Aycom,
  double **Fs,double **Ac,double **As,double *U){
 bool Rez=true; //возвращаемое значение = 1 - все в порядке, = 0 - решения нет
//Xc0,Yc0 - начальные значения координат центра масс,
//Zc - аппликата ЦМ - константа!, dt - шаг по времени,
//K - число точек, Кg - число гармоник разложения Фурье (достаточно 20),
//Kp - число точек перегиба, считая и начальный момент
//TcofP[0:Kp-1], - массив моментов времени перегибов, считая и начальный момент
//Xcof[0:K-1],Ycof[0:K-1] - массивы координат COF,
//Xcom[0:K-1],Ycom[0:K-1] - возвращаемые массивы абсцисс и ординат COM
//Vxcom[0:K-1],Vycom[0:K-1] - возвращаемые массивы абсцисс и ординат скоростей COM
//Axcom[0:K-1],Aycom[0:K-1] - возвращаемые массивы абсцисс и ординат ускорений COM
//Fs[0:1][0:kmax-1] - массивы для абсцисс и ординат COF для разложения Фурье
//kmax - максимальное число точек в интервалах между моментами перегибов
//Ac[0:1][0:Kg], As[0:1][0:Kg], U[0:kmax-1] - рабочие массивы,
//первые два используются для косинусных и синусных амплитуд разложения сначала
//для Xcof и Ycof, затем содержат косинусные и синусные амплитуды разложения Xcom и Ycom
 int kkk=0;//индекс начальной точки текущего интервала между точками пергиба
//FILE *Fo=_wfopen(L"Hyper.txt",L"w");
 for(int i=1;i<Kp;i++){//Цикл по участкам, заканчивающихся моментами перегибов
  double tn=TcofP[i-1],tk=TcofP[i],T=tk-tn,omega=2*M_PI/T,omegat,komegat,ko,
   zgko1,g=L->V->Val,OMEGA=sqrt(g/Zc),sinOT=sinh(OMEGA*T),cosOT=cosh(OMEGA*T),
   kk=(1-cosOT)/sinOT,RezTime,ax,bx,ay,by,tau,
   Xc0_Xcof0=Xc0-Xcof[0],Yc0_Ycof0=Yc0-Ycof[0],// - разности начальных значений
//  координат COM и COF
   Ax,Bx,Ay,By;//амплитуды косинусной и синусной гиперболических составляющих;
  int k=Round(T/dt)+1; bool Old=true;//число точек на данном участке
//Уберем тренд
  bx=Xcof[kkk];            by=Ycof[kkk];
  ax=(Xcof[kkk+k-1]-bx)/T; ay=(Ycof[kkk+k-1]-by)/T;
  for(int j=0;j<k;j++){  int jk=j+kkk;
   tau=j*dt;
   Fs[0][j]=Xcof[jk]-ax*tau-bx;
   Fs[1][j]=Ycof[jk]-ay*tau-by;
  }
  Furie(Fs,2,k,Kg,Ac,As,U);
  Ax=Xc0_Xcof0-Ac[0][0];
  Ay=Yc0_Ycof0-Ac[1][0];
  for(int k=1;k<=Kg;k++){
   ko=k*omega; zgko1=1+Zc*ko*ko/g;
   Ac[0][k]/=zgko1; As[0][k]/=zgko1;
   Ac[1][k]/=zgko1; As[1][k]/=zgko1;
   Ax-=Ac[0][k];    Ay-=Ac[1][k];
  }
  Bx=Ax*kk-Xc0_Xcof0/sinOT;
  By=Ay*kk-Yc0_Ycof0/sinOT;
//fwprintf(Fo,L"tau\tAx_coshOt\tBx_sinhOt\tAy_coshOt\tBy_sinhOt\tSum_poX\tSum_poY\n");
  for(int j=0;j<k;j++){
   int jk=j+kkk; double coshOt,sinhOt,OMEGAt,OMEGA2=OMEGA*OMEGA;
//Ax_coshOt,Bx_sinhOt,Ay_coshOt,By_sinhOt;
   tau=j*dt; omegat=omega*tau;
   OMEGAt=OMEGA*tau; coshOt=cosh(OMEGAt); sinhOt=sinh(OMEGAt);
//Ax_coshOt=Ax*coshOt; Bx_sinhOt=+Bx*sinhOt; Ay_coshOt=Ay*coshOt; By_sinhOt=By*sinhOt;
//fwprintf(Fo,L"%g\t%g\t%g\t%g\t%g\t%g\t%g\n",
//tau,Ax_coshOt,Bx_sinhOt,Ay_coshOt,By_sinhOt,Ax_coshOt+Bx_sinhOt,Ay_coshOt+By_sinhOt);
/*   if(Old){
    Xcom[jk]=Ac[0][0]+Ax*coshOt+Bx*sinhOt;
    Ycom[jk]=Ac[1][0]+Ay*coshOt+By*sinhOt;
    Vxcom[jk]=OMEGA*(Ax*sinhOt+Bx*coshOt);
    Vycom[jk]=OMEGA*(Ay*sinhOt+By*coshOt);
    Axcom[jk]=OMEGA2*(Ax*coshOt+Bx*sinhOt);
    Aycom[jk]=OMEGA2*(Ay*coshOt+By*sinhOt);
   }
   else{*/
    Xcom[jk]=Ac[0][0]+0.5*((Ax+Bx)*exp(OMEGAt)+(Ax-Bx)*exp(-OMEGAt));
    Ycom[jk]=Ac[1][0]+0.5*((Ay+By)*exp(OMEGAt)+(Ay-By)*exp(-OMEGAt));
    Vxcom[jk]=OMEGA*0.5*((Ax+Bx)*exp(OMEGAt)-(Ax-Bx)*exp(-OMEGAt));
    Vycom[jk]=OMEGA*0.5*((Ay+By)*exp(OMEGAt)-(Ay-By)*exp(-OMEGAt));
    Axcom[jk]=OMEGA2*0.5*((Ax+Bx)*exp(OMEGAt)+(Ax-Bx)*exp(-OMEGAt));
    Aycom[jk]=OMEGA2*0.5*((Ay+By)*exp(OMEGAt)+(Ay-By)*exp(-OMEGAt));
//   }
   for(int k=1;k<=Kg;k++){
    komegat=k*omegat;
    double komega=k*omega, komega2=komega*komega,
     cos_ko=cos(komegat), sin_ko=sin(komegat);
    Xcom[jk]+=Ac[0][k]*cos_ko+As[0][k]*sin_ko;
    Ycom[jk]+=Ac[1][k]*cos_ko+As[1][k]*sin_ko;
    Vxcom[jk]+=komega*(As[0][k]*cos_ko-Ac[0][k]*sin_ko);
    Vycom[jk]+=komega*(As[1][k]*cos_ko-Ac[1][k]*sin_ko);
    Axcom[jk]-=komega2*(As[0][k]*sin_ko+Ac[0][k]*cos_ko);
    Aycom[jk]-=komega2*(As[1][k]*sin_ko+Ac[1][k]*cos_ko);
  }}
//fclose(Fo);
  for(int j=0;j<k;j++){ int jk=j+kkk;
   tau=j*dt;
   Xcom[jk]+=ax*tau+bx;
   Ycom[jk]+=ay*tau+by;
   Vxcom[jk]+=ax;
   Vycom[jk]+=ay;
  }
  kkk+=k-1;
 }
 return Rez;
}
//---------------------------------------------------------------------------
void RefreshCOFCOM(double Xcf,double Ycf,//программа пересчета COM
  double Xc,double Yc,double Zc,double *ArrXcof,double *ArrYcof,
  double dt,int K,int j,int Kend,int Kg,int Kp,double *TcofP,bool Exp,
  double **Ac,double **As,double *U,double **Fs,double *Xcom,double *Ycom,
  double *Vxcom,double *Vycom,double *Axcom,double *Aycom){
//time - текущий момент времени (здесь в коде не используется);
//Xcf,Ycf,Xc,Yc,Zc - текущие возмущенные координаты COF и COM (Zc=const?);
//ArrXcof[0:K-1] и ArrYcof[0:K-1] - массивы координат COF из проекта походки при
//первом обращении к процедуре и результат прокладки траектории COF с предыдущего
//вызова процедуры, начиная с time и
// до конца текущего шага ходьбы,   если time<0.5*Ts или
// до конца следующего шага ходьбы, если time>0.5*Ts, где Ts - длительность шага;
//в результате работы процедуры массивы будут содержать новые координаты COF;
//dt - шаг по времени - дискретность массивов ArrXcof и ArrYcof;
//K - число точек в этих массивах; j - индекс текущих значений координат COF и СОМ;
//Kend - число точек от текущей до конца корректируемого участка координат;
//Kg - число гармоник разложения Фурье;
//Kp - число точек перегиба, считая и начальный момент (где может не быть его)
//TcofP[0:Kp-1], - массив моментов времени перегибов, считая и начальный момент
//Exp - экспоненциальный или равномерный закон уменьшения невязок координат;
//Ac[0:1][0:Kg], As[0:1][0:Kg], U[0:K-1] - рабочие массивы - первые два
// используются для косинусных и синусных амплитуд разложения сначала
// для Xcof и Ycof, затем перед выходом из процедуры содержат косинусные и
// синусные амплитуды разложения Xcom и Ycom;
//Fs[0:1][0:K-1]={Xcof,Ycof} - массивы абсцисс и ординат COF для разложения Фурье;
//Xcom[0:K-1],Ycom[0:K-1] - возвращаемые массивы абсцисс и ординат COM.
 int Nend=Kend-1,N=K-1;//число интервалов и число гармоник разложений Фурье COF и COM
 double ax,bx,tau,t,ay,by,dx,dy,T=Nend*dt;
//Ошибки распределяем линейно или экспоненциально по массивам ArrXcof и ArrYcof,
//уменьшая их к концу
 dx=Xcf-ArrXcof[j]; dy=Ycf-ArrYcof[j];//текущие ошибки по координатам COF
 ArrXcof[j]+=dx; ArrYcof[j]+=dy;
 if(Exp){//для экспоненциального закона распределения ошибок
  double k,alfa=10;
  for(int i=1;i<Nend;i++){
   k=exp(-alfa*dt); dx*=k; dy*=k;
   ArrXcof[i+j]+=dx; ArrYcof[i+j]+=dy;
 }}
 else{//для равномерного закона распределения ошибок
  double k;
  for(int i=1;i<Nend;i++){
   k=(double)(Nend-i)/(Nend);
   ArrXcof[i+j]+=dx*k; ArrYcof[i+j]+=dy*k;
 }}
//Вычислим и уберем тренды at+b
// bx=ArrXcof[j]; by=ArrYcof[j]; ax=(ArrXcof[Nend+j]-bx)/T; ay=(ArrYcof[Nend+j]-by)/T;
// Fs[0][0]=ArrXcof[j]-bx; Fs[1][0]=ArrYcof[j]-by; Kg=20;
//Убираем тренд
// for(int i=1;i<Kend;i++,tau+=dt){
//  tau=i*dt;
//  Fs[0][i]=ArrXcof[i+j]-ax*tau-bx; Fs[1][i]=ArrYcof[i+j]-ay*tau-by;
// }
//Решим тележечные уравнения
// COFtoCOM_Furie(9.81,Xc-Xcf,Yc-Ycf,Zc,dt,Kend,Kg,Fs,true,Xcom+j,Ycom+j,Ac,As,U);
 COFtoCOM_Furie(Xc,Yc,Zc,dt,Kg,Kp,TcofP,ArrXcof+j,ArrYcof+j,Xcom+j,Ycom+j,
  Vxcom+j,Vycom+j,Axcom+j,Aycom+j,Fs,Ac,As,U);
//Вернем тренд
// Xcom[j]+=bx; Ycom[j]+=by;
// for(int i=1;i<Kend;i++){
//  tau=i*dt; Xcom[i+j]+=ax*tau+bx; Ycom[i+j]+=ay*tau+by;
// }
//Выведем в файл результаты
FILE *F=_wfopen(L"RefreshCOFCOM.txt",L"a");
fwprintf(F,L"\nArrXcof_arr[%d]=L",2*K);
for(int i=0;i<K;i++){ tau=i*dt;
 if(!(i%5)) fputwc('\n',F);
 fwprintf(F,L"%6.1f, %8.5f",tau,ArrXcof[i]);
 if(i==N) fputws(L";\n",F); else fputws(L", ",F);
}
fwprintf(F,L"\nArrYcof_arr[%d]=L",2*K);
for(int i=0;i<K;i++){ tau=i*dt;
 if(!(i%5)) fputwc('\n',F);
 fwprintf(F,L"%6.1f, %8.5f",tau,ArrYcof[i]);
 if(i==N) fputws(L";\n",F); else fputws(L", ",F);
}
fwprintf(F,L"\nXcom_arr[%d]=L",2*K);
for(int i=0;i<K;i++){ tau=i*dt;
 if(!(i%5)) fputwc('\n',F);
 fwprintf(F,L"%6.1f, %8.5f",tau,Xcom[i]);
 if(i==N) fputws(L";\n",F); else fputws(L", ",F);
}
fwprintf(F,L"\nYcom_arr[%d]=L",2*K);
for(int i=0;i<K;i++){ tau=i*dt;
 if(!(i%5)) fputwc('\n',F);
 fwprintf(F,L"%6.1f, %8.5f",tau,Ycom[i]);
 if(i==N) fputws(L";\n",F); else fputws(L", ",F);
}
fclose(F);
}
