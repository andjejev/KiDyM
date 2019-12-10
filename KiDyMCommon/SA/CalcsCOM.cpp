//---------------------------------------------------------------------------
#pragma hdrstop

#include "CA.h"
#include "ConvDW.h"
#include "time.h"

extern List *L; extern wchar_t Inf[]; static int Wg;
extern Cnst *Nul,*Pust;
bool TOFILE;
//---------------------------------------------------------------------------

#pragma package(smart_init)

void Furie(double **FF,int n,int Kt,int Kg,double **Ac,double **As,double *U);
//------------------------------------------------------------------------
bool COFtoCOM_Furie(double Xc0,double Yc0,double Zc,double dt,/*int K,*/int Kg,
  int Kp,double *TcofP,double *Xcof,double *Ycof,double *Xcom,double *Ycom,
  double *Vxcom, double *Vycom, double *Axcom, double *Aycom,
  double **Fs,double **Ac,double **As,double *U);
//------------------------------------------------------------------------
bool CalcsCOMpoFurie(Parm *Par,TRichEdit *R,TCanvas *Canvas){
 Vary *Xcof,*Ycof,*ZC,*Xc,*Yc,*Vxc,*Vyc,*Axc,*Ayc; int j,N,K,Kg,Kp;
 clock_t timer,DeltaTime; FILE *F;
 double *TcofP,**Ac,**As,*U,Xc0,Yc0,*Xcom,*Ycom,*Xcf,*Ycf,**Fs,Zc,*Vxcom,*Vycom,
  *Axcom,*Aycom/*,g=L->V->Val*/,dt,Tn,Tk,tau,t/*,RezTime*/;
 wchar_t NameArr[256],NameSpln[256]; Arra *ArrX,*ArrY,*ArrVx,*ArrVy,*ArrAx,*ArrAy;
 Root *Ins; Form Ft; Ft.V=L->Time;
// COF(Xcof,Ycof), Перегибы(t1,t1+T,t1+2*T,t1+3*T,t1+4*T), Высота ЦМ(ZC),
// Дискретность(dt), COM(Xc,Yc);
//найдем времена - начальное и конечное
 if((Ins=FindInstLast(L"ВРЕМЯ"))||(Ins=FindInstLast(L"ВАРЬИРОВАТЬ"))){
  if(KolElem(Ins->First->R)==2){//границы в скобках
   Tn=!Ins->First->R->Nam ? Ins->First->R->Val:
      Val(StrToForm(Ins->First->R->Nam));
   Tk=!Ins->First->R->Sled->Nam ? Ins->First->R->Sled->Val:
      Val(StrToForm(Ins->First->R->Sled->Nam));
  }
  else{//границы подряд в списке
   Tn=!Ins->First->Sled->Nam ? Ins->First->Sled->Val :
       Val(StrToForm(Ins->First->Sled->Nam));
   if(Ins->First->Sled->Sled){
    Tk=!Ins->First->Sled->Sled->Nam ? Ins->First->Sled->Sled->Val :
       Val(StrToForm(Ins->First->Sled->Sled->Nam));
   }
   else{
    swprintf(Inf,L"\nВ инструкции \"%s\" не указана верхняя граница времени",
     Ins->Name);
    Error(0,0,true);
 }}}
 for(;Par;Par=Par->Sled){
  LowToUpp(Par->Nam);
  if(!wcscmp(Par->Nam,L"COF")){ Parm *P=Par->R;
   if((KolElem(P)!=2)||!P->Nam||!P->Sled->Nam){
    swprintf(Inf,L"\nВ инструкции \"COF\" оператора\n"
     L"\"РАССЧИТАТЬ COM по Фурье\"\n"
     L"надо указать имена 2-х переменных,\n"
     L"задающих абсциссу и ординату траектории COF");
    Error(0,0,true);
    return false;
   }
   for(int i=0;P;P=P->Sled,i++){
    switch(i){
     case 0: Xcof=TakeVary(P->Nam); break;
     case 1: Ycof=TakeVary(P->Nam); break;
   }}
   continue;
  }
  if(!wcscmp(Par->Nam,L"ДИСКРЕТНОСТЬ")){
   if(Par->R->Nam) dt=Val(TakeVary(Par->R->Nam)->Znach);
   else if(Par->R->F.C&&(Par->R->F.C!=Pust)) dt=Val(Par->R->F);
   else dt=Par->R->Val;
   if(dt==0.0){
    swprintf(Inf,L"\nВ инструкции \"ДИСКРЕТНОСТЬ\" оператора\n"
     L"\"РАССЧИТАТЬ COM по Фурье\"\n"
     L"надо указать ненулевое значение шага по времени");
    Error(0,0,true);
    return false;
   }
   continue;
  }
  if(!wcscmp(Par->Nam,L"ПЕРЕГИБЫ")){
   Parm *P=Par->R; int kmax=0;
   Kp=KolElem(P); L->Time->P=0;
   TcofP=(double *)calloc(Kp,SzD);
   for(int i=0,k;P;P=P->Sled,i++){
    if(P->Nam) t=Value(TakeVary(P->Nam)->Znach);
    else if(P->F.C&&(P->F.C!=Pust)) t=Value(P->F);
    else t=P->Val;
    if(i){
     k=Round((t-TcofP[i-1])/dt);
     if(k>kmax) kmax=k;
    }
    TcofP[i]=t;
   }
   if(t>Tk) Tk=t;
   kmax++;
   Ac=(double **)calloc(2,SzV); As=(double **)calloc(2,SzV);
   Fs=(double **)calloc(2,SzV);
   N=Round((Tk-Tn)/dt); K=N+1;
   Kg=40; //floor(0.5*N); //число гармоник
   for(int i=0;i<2;i++){
    Ac[i]=(double *)calloc(Kg+1,SzD); As[i]=(double *)calloc(Kg+1,SzD);
    Fs[i]=(double *)calloc(kmax,SzD);
   }
   U=(double *)calloc(kmax,SzD);//рабочий массив
   Xcf=(double *)calloc(K,SzD);
   Ycf=(double *)calloc(K,SzD);//массивы координат COF.
   Xcom=(double *)calloc(K,SzD);
   Ycom=(double *)calloc(K,SzD);//массивы результатов - координат COM.
   Vxcom=(double *)calloc(K,SzD);
   Vycom=(double *)calloc(K,SzD);//массивы результатов - проекций скоростей COM.
   Axcom=(double *)calloc(K,SzD);
   Aycom=(double *)calloc(K,SzD);//массивы результатов - проекций ускорений COM.
   L->Time->P=0;
   for(int i=0;i<K;i++){
    t=Tn+i*dt; L->Time->Val=t; ClearVar();
    Xcf[i]=Val(Xcof->Znach);
    Ycf[i]=Val(Ycof->Znach);
   }
   continue;
  }
  if(!wcscmp(Par->Nam,L"ВЫСОТА ЦМ")){
   ZC=TakeVary(Par->R->Nam); Zc=Val(ZC->Znach);
   if(Zc<=0.0){
    swprintf(Inf,L"\nВ инструкции \"ВЫСОТА ЦМ\" оператора\n"
     L"\"РАССЧИТАТЬ COM по Фурье\"\n"
     L"надо указать имя переменной с положительным значением высоты");
    Error(0,0,true);
    return false;
   }
   continue;
  }
  if(!wcscmp(Par->Nam,L"COM")){
   Parm *P=Par->R;
   if((KolElem(P)!=8)||
     !P->Nam||
     !P->Sled->Nam||
     !P->Sled->Sled->Nam||
     !P->Sled->Sled->Sled->Nam||
     !P->Sled->Sled->Sled->Sled->Nam||
     !P->Sled->Sled->Sled->Sled->Sled->Nam||
     !P->Sled->Sled->Sled->Sled->Sled->Sled->Nam||
     !P->Sled->Sled->Sled->Sled->Sled->Sled->Sled->Nam){
    swprintf(Inf,L"\nВ инструкции \"COM\" оператора\n"
     L"\"РАССЧИТАТЬ COM по Фурье\"\n"
     L"надо указать 8 имен переменных, содержащих\n"
     L"начальные значения координат, значения координат во времени,\n"
     L"проекций скоростей и ускорений центра масс");
    Error(0,0,true);
    return false;
   }
   Xc0=Value(TakeVary(P->Nam)->Znach);
   P=P->Sled;
   Yc0=Value(TakeVary(P->Nam)->Znach);
   P=P->Sled;
   Xc=TakeVary(P->Nam);
   swprintf(NameSpln,L"%s",Xc->Name);
   swprintf(NameArr,L"%s_arr",Xc->Name);
   ArrX=TakeArra(NameArr);
   ArrX->N=2*K;
   ArrX->A=(double *)calloc(ArrX->N,SzD);
   P=P->Sled;
   Yc=TakeVary(P->Nam);
   swprintf(NameSpln,L"%s",Yc->Name);
   swprintf(NameArr,L"%s_arr",Yc->Name);
   ArrY=TakeArra(NameArr);
   ArrY->N=2*K;
   ArrY->A=(double *)calloc(ArrY->N,SzD);
   P=P->Sled;
   Vxc=TakeVary(P->Nam);
   swprintf(NameSpln,L"%s",Vxc->Name);
   swprintf(NameArr,L"%s_arr",Vxc->Name);
   ArrVx=TakeArra(NameArr);
   ArrVx->N=2*K;
   ArrVx->A=(double *)calloc(ArrVx->N,SzD);
   P=P->Sled;
   Vyc=TakeVary(P->Nam);
   swprintf(NameSpln,L"%s",Vyc->Name);
   swprintf(NameArr,L"%s_arr",Vyc->Name);
   ArrVy=TakeArra(NameArr);
   ArrVy->N=2*K;
   ArrVy->A=(double *)calloc(ArrVy->N,SzD);
   P=P->Sled;
   Axc=TakeVary(P->Nam);
   swprintf(NameSpln,L"%s",Axc->Name);
   swprintf(NameArr,L"%s_arr",Axc->Name);
   ArrAx=TakeArra(NameArr);
   ArrAx->N=2*K;
   ArrAx->A=(double *)calloc(ArrAx->N,SzD);
   P=P->Sled;
   Ayc=TakeVary(P->Nam);
   swprintf(NameSpln,L"%s",Ayc->Name);
   swprintf(NameArr,L"%s_arr",Ayc->Name);
   ArrAy=TakeArra(NameArr);
   ArrAy->N=2*K;
   ArrAy->A=(double *)calloc(ArrVy->N,SzD);
  }
  if(!wcscmp(Par->Nam,L"РЕШЕНИЕ В ФАЙЛ"))
   TOFILE=true;
 }
 timer=clock();
 COFtoCOM_Furie(Xc0,Yc0,Zc,dt,Kg,Kp,TcofP,Xcf,Ycf,Xcom,Ycom,Vxcom,Vycom,
  Axcom,Aycom,Fs,Ac,As,U);
 DeltaTime=(clock()-timer);
// RezTime=(double)DeltaTime/CLK_TCK;
 for(j=0;j<K;j++){
  tau=j*dt; t=Tn+tau;
  ArrX->A[(j)*2]=t; ArrX->A[(j)*2+1]=Xcom[j];
  ArrY->A[(j)*2]=t; ArrY->A[(j)*2+1]=Ycom[j];
  ArrVx->A[(j)*2]=t; ArrVx->A[(j)*2+1]=Vxcom[j];
  ArrVy->A[(j)*2]=t; ArrVy->A[(j)*2+1]=Vycom[j];
  ArrAx->A[(j)*2]=t; ArrAx->A[(j)*2+1]=Axcom[j];
  ArrAy->A[(j)*2]=t; ArrAy->A[(j)*2+1]=Aycom[j];
 }
 Xc->Znach.P=TakeSpln(1,ArrX,Ft);
 Yc->Znach.P=TakeSpln(1,ArrY,Ft);
 Vxc->Znach.P=TakeSpln(1,ArrVx,Ft);
 Vyc->Znach.P=TakeSpln(1,ArrVy,Ft);
 Axc->Znach.P=TakeSpln(1,ArrAx,Ft);
 Ayc->Znach.P=TakeSpln(1,ArrAy,Ft);
 if(TOFILE){
  if(F=_wfopen(L"Xc.arr",L"wb")){
   int K=ArrX->N;
   fwrite(&K,SzI,1,F);//сначала пишем, размер массива, затем его самого
   fwrite(ArrX->A,SzD,K,F);
   fclose(F);
  }
  if(F=_wfopen(L"Yc.arr",L"wb")){
   int K=ArrY->N;
   fwrite(&K,SzI,1,F);//сначала пишем, размер массива, затем его самого
   fwrite(ArrY->A,SzD,K,F);
   fclose(F);
  }
  if(F=_wfopen(L"Vxc.arr",L"wb")){
   int K=ArrVx->N;
   fwrite(&K,SzI,1,F);//сначала пишем, размер массива, затем его самого
   fwrite(ArrVx->A,SzD,K,F);
   fclose(F);
  }
  if(F=_wfopen(L"Vyc.arr",L"wb")){
   int K=ArrVy->N;
   fwrite(&K,SzI,1,F);//сначала пишем, размер массива, затем его самого
   fwrite(ArrVy->A,SzD,K,F);
   fclose(F);
  }
  if(F=_wfopen(L"Axc.arr",L"wb")){
   int K=ArrAx->N;
   fwrite(&K,SzI,1,F);//сначала пишем, размер массива, затем его самого
   fwrite(ArrAx->A,SzD,K,F);
   fclose(F);
  }
  if(F=_wfopen(L"Ayc.arr",L"wb")){
   int K=ArrAy->N;
   fwrite(&K,SzI,1,F);//сначала пишем, размер массива, затем его самого
   fwrite(ArrAy->A,SzD,K,F);
   fclose(F);
 }}
 free(Xcf); free(Ycf); free(Xcom); free(Ycom); free(Vxcom); free(Vycom);
 free(Axcom); free(Aycom); free(TcofP); free(U);
 for(int i=0;i<2;i++){ free(Ac[i]); free(As[i]); free(Fs[i]); }
 free(Ac); free(As); free(Fs);
 return true;
}
//---------------------------------------------------------------------------
bool CalcsCOM (Parm *Par,TRichEdit *R,TCanvas *Canvas){
 Vary *Xcof,*Ycof,*ZC,*Xc,*Yc; int j,N; FILE *F,*Fx;
 double *a,*b,*c,*d,Zc,Kc,Kd,tau,g=L->V->Val,dt,t=0;
 wchar_t NameArr[256],NameSpln[256]; Form Ft; Ft.V=L->Time; Arra *Arr;
//Дано(Xcof,Ycof,ZC), Найти(Xc,Yc)
 for(;Par;Par=Par->Sled){
  if(!wcscmp(LowToUpp(Par->Nam),L"ДАНО")){ Parm *P=Par->R;
   for(int i=0;P;P=P->Sled,i++){
    switch(i){
     case 0: Xcof=TakeVary(P->Nam); break;
     case 1: Ycof=TakeVary(P->Nam); break;
     case 2:
      ZC=TakeVary(P->Nam); Zc=Val(ZC->Znach); Kc=6.0*Zc/g; Kd=Kc/3.0;
      break;
  }}}
  else if(!wcscmp(LowToUpp(Par->Nam),L"НАЙТИ")){ Parm *P=Par->R;
   for(int i=0;P;P=P->Sled,i++){
    switch(i){
     case 0:
      Xc=TakeVary(P->Nam);
      N=Xcof->Znach.P->A->N/2-1; //число интервалов
      a=(double *)calloc(N,sizeof(double));
      b=(double *)calloc(N,sizeof(double));
      c=(double *)calloc(N,sizeof(double));
      d=(double *)calloc(N,sizeof(double));//массивы коэффициентов a,b,c,d .
      dt=Xcof->Znach.P->A->A[2];
      for(j=0;j<N;j++){
       d[j]=Xcof->Znach.P->d[j];
       c[j]=Xcof->Znach.P->c[j];
       b[j]=Xcof->Znach.P->b[j]+Kc*d[j];
       a[j]=Xcof->Znach.P->a[j]+Kd*c[j];
       t+=dt;
      }
      swprintf(NameSpln,L"%s",Xc->Name);
      swprintf(NameArr,L"%s_arr",Xc->Name);
      Arr=TakeArra(NameArr); Arr->N=Xcof->Znach.P->A->N;
      Arr->A=(double *)calloc(Arr->N,SzD);
      for(j=0,t=0;j<Arr->N/2-1;j++){
       Arr->A[j*2]=Xcof->Znach.P->A->A[j*2];
       Arr->A[j*2+1]=a[j];
       t+=dt;
      }
      Arr->A[j*2]=Xcof->Znach.P->A->A[j*2];
      Arr->A[j*2+1]=d[j-1]*dt*dt*dt+c[j-1]*dt*dt+b[j-1]*dt+a[j-1];
      Xc->Znach.P=TakeSpln(1,Arr,Ft);
      break;
     case 1:
      Yc=TakeVary(P->Nam);
      N=Ycof->Znach.P->A->N/2-1; //число интервалов
      a=(double *)calloc(N,sizeof(double));
      b=(double *)calloc(N,sizeof(double));
      c=(double *)calloc(N,sizeof(double));
      d=(double *)calloc(N,sizeof(double));//массивы коэффициентов a,b,c,d .
      dt=Ycof->Znach.P->A->A[2];
      for(j=0,t=0;j<N;j++){
       d[j]=Ycof->Znach.P->d[j];
       c[j]=Ycof->Znach.P->c[j];
       b[j]=Ycof->Znach.P->b[j]+Kc*d[j];
       a[j]=Ycof->Znach.P->a[j]+Kd*c[j];
       t+=dt;
      }
      swprintf(NameSpln,L"%s",Yc->Name);
      swprintf(NameArr,L"%s_arr",Yc->Name);
      Arr=TakeArra(NameArr); Arr->N=Ycof->Znach.P->A->N;
      Arr->A=(double *)calloc(Arr->N,SzD);
      for(j=0,t=0;j<Arr->N/2-1;j++){
       Arr->A[j*2]=Ycof->Znach.P->A->A[j*2];
       Arr->A[j*2+1]=a[j];
       t+=dt;
      }
      Arr->A[j*2]=Ycof->Znach.P->A->A[j*2];
      Arr->A[j*2+1]=d[j-1]*dt*dt*dt+c[j-1]*dt*dt+b[j-1]*dt+a[j-1];
      Yc->Znach.P=TakeSpln(1,Arr,Ft);
      break;
 }}}}
 return true;
}
//---------------------------------------------------------------------------
double ValFromParm(Parm *P){
 if(P->F.C&&P->F!=Pust) return Val(P->F);
 if(P->Nam){ Vary *V=TakeVary(P->Nam); return Val(V->Znach); }
 return P->Val;
}
//---------------------------------------------------------------------------
void RefreshCOFCOM(double Xcf,double Ycf,//программа пересчета COM
  double Xc,double Yc,double Zc,double *ArrXcof,double *ArrYcof,
  double dt,int K,int j,int Kend,int Kg,int Kp,double *TcofP,bool Exp,
  double **Ac,double **As,double *U,double **Fs,double *Xcom,double *Ycom,
  double *Vxcom,double *Vycom,double *Axcom,double *Aycom);
//---------------------------------------------------------------------------
void PrintArr(wchar_t *NameArr,int K,double t,double dt,double *Arr,FILE *F){
 fwprintf(F,L"%s[%d]=L",NameArr,2*K);
 for(int j=0;j<K;j++,t+=dt){
  if(!(j%5)) fputwc('\n',F);
  fwprintf(F,L"%6.1f, %8.5f",t,Arr[j]);
  if(j==(K-1)) fputws(L";\n",F); else fputws(L", ",F);
}}
//---------------------------------------------------------------------------
bool Perturbation(Parm *Par,TRichEdit *R,TCanvas *Canvas){
 Vary *VXcof,*VYcof,*ZC; int K,N,Kg,Kend,K4,Kp; FILE *F; Root *Ins;
 double t,tau,dt,Zc,/*g=L->V->Val,*/t1[3],t2[3],**Ac,**As,*U,**Fs,
  *Xcom,*Ycom,*Vxcom,*Vycom,*Axcom,*Aycom,*Xcof,*Ycof,ax=0.0,
  bx=0.0,ay=0.0,by=0.0,Tn,Tk,T,df,*TcofP;
 ClearVar();
//найдем времена - начальное и конечное
 if((Ins=FindInstLast(L"ВРЕМЯ"))||(Ins=FindInstLast(L"ВАРЬИРОВАТЬ"))){
  if(KolElem(Ins->First->R)==2){//границы в скобках
   Tn=!Ins->First->R->Nam ? Ins->First->R->Val:
      Val(StrToForm(Ins->First->R->Nam));
   Tk=!Ins->First->R->Sled->Nam ? Ins->First->R->Sled->Val:
      Val(StrToForm(Ins->First->R->Sled->Nam));
  }
  else{//границы подряд в списке
   Tn=!Ins->First->Sled->Nam ? Ins->First->Sled->Val :
       Val(StrToForm(Ins->First->Sled->Nam));
   if(Ins->First->Sled->Sled)
    Tk=!Ins->First->Sled->Sled->Nam ? Ins->First->Sled->Sled->Val :
       Val(StrToForm(Ins->First->Sled->Sled->Nam));
   else{
    swprintf(Inf,L"\nВ инструкции \"%s\" не указана верхняя граница времени",
     Ins->Name);
    Error(0,0,true);
 }}}
 for(;Par;Par=Par->Sled){ Parm *P=Par->R;
  LowToUpp(Par->Nam);
  if(!wcscmp(Par->Nam,L"ДИСКРЕТ ВРЕМЕНИ")){
   dt=ValFromParm(Par->R);
   continue;
  }
  if(!wcscmp(Par->Nam,L"КООРДИНАТЫ COF")){
   for(int i=0;P;P=P->Sled,i++){
    switch(i){
     case 0: VXcof=TakeVary(P->Nam); break;
     case 1: VYcof=TakeVary(P->Nam); break;
   }}
   continue;
  }
  if(!wcscmp(Par->Nam,L"ВЫСОТА ЦМ")){
   ZC=TakeVary(P->Nam); Zc=Val(ZC->Znach);
   continue;
  }
  if(!wcscmp(Par->Nam,L"ШАГ 1")){//Начало, середина и конец (середина 2-хоп. фазы) шага
   for(int i=0;P;P=P->Sled,i++)
    t1[i]=ValFromParm(P);
   continue;
  }
  if(!wcscmp(Par->Nam,L"ШАГ 2")){//Начало, середина и конец (середина 2-хоп. фазы) шага
   for(int i=0;P;P=P->Sled,i++)
    t2[i]=ValFromParm(P);
   continue;
 }}
// Tn=t1[0]; Tk=t2[2];
 T=Tk-Tn;
 N=Round(T/dt); K=N+1; Kg=floor(0.5*N);
//Сначала рассчитаем идеальные траектории COF и COM
 Ac=(double **)calloc(2,SzV); As=(double **)calloc(2,SzV);
 Fs=(double **)calloc(2,SzV);
 for(int i=0;i<2;i++){
  Ac[i]=(double *)calloc(Kg+1,SzD); As[i]=(double *)calloc(Kg+1,SzD);
  Fs[i]=(double *)calloc(K,SzD);
 }
 U=(double *)calloc(K,SzD);//рабочий массив
 Xcof=(double *)calloc(K,SzD);
 Ycof=(double *)calloc(K,SzD);//массивы координат COF.
 Xcom=(double *)calloc(K,SzD);
 Ycom=(double *)calloc(K,SzD);//массивы результатов - координат COM.
 Vxcom=(double *)calloc(K,SzD);
 Vycom=(double *)calloc(K,SzD);//массивы результатов - проекций скоростей COM.
 Axcom=(double *)calloc(K,SzD);
 Aycom=(double *)calloc(K,SzD);//массивы результатов - проекций ускорений COM.
 L->Time->P=0;
 for(int j=0;j<K;j++){
  tau=j*dt; t=Tn+tau; L->Time->Val=t; ClearVar();
  Xcof[j]=Val(VXcof->Znach);
  Ycof[j]=Val(VYcof->Znach);
 }
F=_wfopen(L"RefreshCOFCOM.txt",L"w");
PrintArr(L"Xcofp_arr",K,Tn,dt,Xcof,F);
PrintArr(L"Ycofp_arr",K,Tn,dt,Ycof,F);
 Kp=4; TcofP=(double *)calloc(Kp,SzD);
 TcofP[0]=0; TcofP[1]=t1[2]; TcofP[2]=t2[2]; TcofP[3]=Tk;
 COFtoCOM_Furie(Xcof[0],Ycof[0],Zc,dt,20,Kp,TcofP,Xcof,Ycof,Xcom,Ycom,
 Vxcom,Vycom,Axcom,Aycom,Fs,Ac,As,U);
PrintArr(L"Xcomp_arr",K,Tn,dt,Xcom,F);
PrintArr(L"Ycomp_arr",K,Tn,dt,Ycom,F);
fclose(F);
 K4=Round(t2[1]/dt)+1;
FILE *Fxcf=_wfopen(L"dxcf.txt",L"w"),*Fycf=_wfopen(L"dycf.txt",L"w"),
*Fxcm=_wfopen(L"dxcm.txt",L"w"),*Fycm=_wfopen(L"dycm.txt",L"w");
fwprintf(Fxcf,L"dxcf[%d]=\n",2*K4); fwprintf(Fycf,L"dycf[%d]=\n",2*K4);
fwprintf(Fxcm,L"dxcm[%d]=\n",2*K4); fwprintf(Fycm,L"dycm[%d]=\n",2*K4);
 for(int j=0;j<K4;j++){
  double Xcf,Ycf,dCOF=5e-2,Xcm,Ycm,dx,dy; int IndCOF,K56=Round(t2[2]/dt)+1;
  tau=j*dt; t=Tn+tau;
  if(tau<=t1[1]){
   Kend=Round(t1[2]/dt)+1-j; Kp=2; TcofP[1]=t1[2]-j;
  }
  else{
   Kend=Round(t2[2]/dt)+1-j; Kp=3; TcofP[2]=t2[2]-j;
  }
  dx=(random(101)-50)*dCOF/100.0; fwprintf(Fxcf,L"%g, %g%c",t,dx,j<K4-1?',':';');
  Xcf=Xcof[j]+dx;
  dy=(random(101)-50)*dCOF/100.0; fwprintf(Fycf,L"%g, %g%c",t,dy,j<K4-1?',':';');
  Ycf=Ycof[j]+dy;
  dx=(random(101)-50)*dCOF/100.0; fwprintf(Fxcm,L"%g, %g%c",t,dx,j<K4-1?',':';');
  Xcm=Xcom[j]+dx;
  dy=(random(101)-50)*dCOF/100.0; fwprintf(Fycm,L"%g, %g%c",t,dy,j<K4-1?',':';');
  Ycm=Ycom[j]+dy;
  Kg=floor(0.5*(Kend-1));
  RefreshCOFCOM(Xcf,Ycf,Xcm,Ycm,Zc,Xcof,Ycof,dt,K56,j,Kend,Kg,Kp,TcofP,true,
   Ac,As,U,Fs,Xcom,Ycom,Vxcom,Vycom,Axcom,Aycom);
 }
fclose(Fxcf); fclose(Fycf); fclose(Fxcm); fclose(Fycm);
 free(TcofP);
 for(int i=0;i<2;i++){ free(Ac[i]); free(As[i]); free(Fs[i]); }
 free(Ac); free(As); free(Fs);
 free(U); free(Xcof); free(Ycof); free(Xcom); free(Ycom);
 free(Vxcom); free(Vycom); free(Axcom); free(Aycom);
 return true;
}
//---------------------------------------------------------------------------
