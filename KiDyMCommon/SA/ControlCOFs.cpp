//---------------------------------------------------------------------------
#pragma hdrstop

#include "ControlCOFs.h"
#include <math.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)

extern Cnst *Nul,*Odin,*Pust; extern List *L;
static Form NumX,NumY,Denum,F; extern int Wg,KP[],Kmg,dS[],*SzFont;
extern double Tn,Tk,Tend,MSt[],MG[],MstT[],Sg;
extern wchar_t Inf[]; extern bool PRINTVARS,PRNTOPRIGHT;

static Form NumXDep,NumXIndep,NumYDep,NumYIndep,DenumDep,DenumIndep,U1,U2,U,dU1,dU2;
wchar_t NameCoord1[32],NameCoord2[32],NameVCord1[32],NameVCord2[32],NameWCord1[32],
 NameWCord2[32],NameXcof[32],NameX_cof[32],NameY_cof[32],NameCoord[32],
 NameVCord[32],NameWCord[32];
Vary *Vc1,*Vc2,*Vv1,*Vv2,*Vw1,*Vw2,*Vc,*Vv,*Vw,*Vx,*Vx_; Form FVx,FVx_;
//---------------------------------------------------------------------------
bool Depedente(Form F){
//зависит ли формула от управл€ющих углов, их скоростей и ускорений
 return VarInForm(true,F,Vc1)||VarInForm(true,F,Vc2)||
        VarInForm(true,F,Vv1)||VarInForm(true,F,Vv2)||
        VarInForm(true,F,Vw1)||VarInForm(true,F,Vw2);
}
//---------------------------------------------------------------------------
double ValOfParm(Parm *P){ double D; Cnst *C; Vary *V;
 if(P->Nam){
  if(FindCnst(P->Nam,&C)) D=C->Val;
  else{ V=TakeVary(P->Nam); D=Val(V->Znach); }
 }
 else if(P->F.C!=Pust) D=Val(P->F);
 else D=P->Val;
 return D;
}
//---------------------------------------------------------------------------
double C1[3],C2[3],V1[3],V2[3],W1[3],W2[3],dt,C[3],V[3],W[3];
//---------------------------------------------------------------------------
void VW1(int i,double Val){
 Vv1->Val=V1[0]=(Val-C1[1])/dt;
 if(i>1)
  Vw1->Val=W1[0]=(V1[0]-V1[1])/dt;
}
//---------------------------------------------------------------------------
void VW2(int i,double Val){
 Vv2->Val=V2[0]=(C2[0]-C2[1])/dt;
 if(i==1)
  Vw2->Val=W2[0]=(V2[0]-V2[1])/dt;
}
//---------------------------------------------------------------------------
void ControlCOF(Root *I,TStatusBar *StatusBar,TTabControl *TC){
 double Drezx,Drezy,d1,d2,l=0.000001,e=1e-5,
  g1,g2,D1,D2,D;
 Root *Ins;  Parm *P; wchar_t *s,NameB[256],Head[256]; bool MOMIN=false; int i;
 Vary *Vfx,*Vfy,*Vfz,*Vix,*Viy,*Viz,*Vp,*Vsx1,*Vsx2,*Vsx3,*Vsy1,*Vsy2,*Vsy3,*Vx,
  *Vy,*Vzn,*Vnmx,*Vnmy,*Vd,*Vx_,*Vy_,*Xc,*Yc;
 Form Ffx,Ffy,Ffz,Fix,Fiy,Fiz,Fp,Fsx1,Fsx2,Fsx3,Fsy1,Fsy2,Fsy3,F,Fg,Fd,FVx_,FVy_;
//найдем времена - начальное и конечное
 if((Ins=FindInstLast(L"¬–≈ћя"))||(Ins=FindInstLast(L"¬ј–№»–ќ¬ј“№"))){
  if(KolElem(Ins->First->R)==2){//границы в скобках
   Tn=!Ins->First->R->Nam ? Ins->First->R->Val:
	  Val(StrToForm(Ins->First->R->Nam));
   Tk=Tend=!Ins->First->R->Sled->Nam ? Ins->First->R->Sled->Val:
	  Val(StrToForm(Ins->First->R->Sled->Nam));
  }
  else{//границы подр€д в списке
   Tn=!Ins->First->Sled->Nam ? Ins->First->Sled->Val :
	   Val(StrToForm(Ins->First->Sled->Nam));
   if(Ins->First->Sled->Sled)
	Tk=Tend=!Ins->First->Sled->Sled->Nam ? Ins->First->Sled->Sled->Val :
	   Val(StrToForm(Ins->First->Sled->Sled->Nam));
   else{
	swprintf(Inf,L"\n¬ инструкции \"%s\" не указана верхн€€ граница времени",
	 Ins->Name);
	Error(0,0,true);
 }}}
//–азберем инструкцию L"”ѕ–ј¬Ћ≈Ќ»≈ COF"
/*”правление COF:=координаты COF(X_cof,Y_cof),управл€ющие координаты(tau(0,0),omega(0,0));*/
 if((Ins=FindInstLast(L"”ѕ–ј¬Ћ≈Ќ»≈ COF"))){
  if(FindParm(Ins->First,L" ќќ–ƒ»Ќј“џ COF",&P)){
   wcscpy(NameX_cof,P->R->Nam); wcscpy(NameY_cof,P->R->Sled->Nam);
  }
  if(FindParm(Ins->First,L"”ѕ–ј¬Ћяёў»≈  ќќ–ƒ»Ќј“џ",&P)){
//сформируем имена и сами переменные дл€ управл€ющих координат, их скоростей и ускорений
   wcscpy(NameCoord1,P->R->Nam);                         Vc1=TakeVary(NameCoord1);
   wcscpy(NameCoord2,P->R->Sled->Nam);                   Vc2=TakeVary(NameCoord2);
   swprintf(NameVCord1,L"%s'%s",NameCoord1,L->Time->Name); Vv1=TakeVary(NameVCord1);
   swprintf(NameVCord2,L"%s'%s",NameCoord2,L->Time->Name); Vv2=TakeVary(NameVCord2);
   swprintf(NameWCord1,L"%s'%s",NameVCord1,L->Time->Name); Vw1=TakeVary(NameWCord1);
   swprintf(NameWCord2,L"%s'%s",NameVCord2,L->Time->Name); Vw2=TakeVary(NameWCord2);
//получим начальные значени€ управлющих координат
   Vc1->P=Vc2->P=Vv1->P=Vv2->P=Vw1->P=Vw2->P=0;
   L->Time->Val=0; ClearVar();
   Vc1->Val=ValOfParm(P->R->R);
   Vv1->Val=ValOfParm(P->R->R->Sled);
   Vc2->Val=ValOfParm(P->R->Sled->R);
   Vv2->Val=ValOfParm(P->R->Sled->R->Sled);
 }}
//накопим формулы дл€ зависимых и независимых от управл€ющих координат,
//их скоростей и ускорений
 NumXDep.C=Nul; NumYDep.C=Nul; DenumDep.C=Nul; NumXIndep.C=Nul; NumYIndep.C=Nul;
 DenumIndep.C=Nul; Fg.V=L->V;
 for(Body *b=L->B;b;b=b->Sled){
  wcscpy(NameB,b->Name); if(s=wcschr(NameB,'~')) *s='\0';
  swprintf(Inf,L"Fx@%s",NameB);     Vfx =TakeVary(Inf); Ffx.V =Vfx;
  swprintf(Inf,L"Fy@%s",NameB);     Vfy =TakeVary(Inf); Ffy.V =Vfy;
  swprintf(Inf,L"Fz@%s",NameB);     Vfz =TakeVary(Inf); Ffz.V =Vfz;
  swprintf(Inf,L"P@%s",NameB);      Vp  =TakeVary(Inf); Fp.V  =Vp;
  swprintf(Inf,L"Sx1@%s",NameB);    Vsx1=TakeVary(Inf); Fsx1.V=Vsx1;
  swprintf(Inf,L"Sx2@%s",NameB);    Vsx2=TakeVary(Inf); Fsx2.V=Vsx2;
  swprintf(Inf,L"Sy1@%s",NameB);    Vsy1=TakeVary(Inf); Fsy1.V=Vsy1;
  swprintf(Inf,L"Sy2@%s",NameB);    Vsy2=TakeVary(Inf); Fsy2.V=Vsy2;
  Vfx->Znach=b->m*b->ACk[0];      Vfy->Znach=b->m*b->ACk[1];
  Vfz->Znach=b->m*b->ACk[2];      Vp->Znach=b->m*Fg;
  Vsx1->Znach=(Ffz+Fp)*b->rCk[0]; Vsx2->Znach=Ffx*b->rCk[2];
  Vsy1->Znach=(Ffz+Fp)*b->rCk[1]; Vsy2->Znach=Ffy*b->rCk[2];
  if(Depedente(Vsx1->Znach)) NumXDep=NumXDep+Fsx1;
  else NumXIndep=NumXIndep+Fsx1;
  if(Depedente(Vsx2->Znach)) NumXDep=NumXDep-Fsx2;
  else NumXIndep=NumXIndep-Fsx2;
  if(Depedente(Vsy1->Znach)) NumYDep=NumYDep+Fsy1;
  else NumYIndep=NumYIndep+Fsy1;
  if(Depedente(Vsy2->Znach)) NumYDep=NumYDep-Fsy2;
  else NumYIndep=NumYIndep-Fsy2;
  if(Depedente(Vfz->Znach))  DenumDep=DenumDep+Ffz+Fp;
  else DenumIndep=DenumIndep+Ffz+Fp;
  if(MOMIN){//здесь надо дописать
   swprintf(Inf,L"Ix@%s",NameB);  Vix =TakeVary(Inf); Fix.V =Vix;
   swprintf(Inf,L"Iy@%s",NameB);  Viy =TakeVary(Inf); Fiy.V =Viy;
   swprintf(Inf,L"Iz@%s",NameB);  Viz =TakeVary(Inf); Fiz.V =Viz;
   swprintf(Inf,L"Sx3@%s",NameB); Vsx3=TakeVary(Inf); Fsx3.V=Vsx3;
   swprintf(Inf,L"Sy3@%s",NameB); Vsy3=TakeVary(Inf); Fsy3.V=Vsy3;
   Vix->Znach=b->Jx*b->ekk[0]+(b->Jz-b->Jy)*b->wkk[1]*b->wkk[2];
   Viy->Znach=b->Jy*b->ekk[1]+(b->Jx-b->Jz)*b->wkk[0]*b->wkk[2];
   Viz->Znach=b->Jx*b->ekk[2]+(b->Jy-b->Jx)*b->wkk[0]*b->wkk[1];
   Vsx3->Znach=b->Sk0[1][0]*Vix->Znach+b->Sk0[1][1]*Viy->Znach+
			   b->Sk0[1][2]*Viz->Znach;
   Vsy3->Znach=b->Sk0[0][0]*Vix->Znach+b->Sk0[0][1]*Viy->Znach+
			   b->Sk0[0][2]*Viz->Znach;
   if(Depedente(Vsx3->Znach)) NumYDep=NumYDep-Fsx3;
   else NumYIndep=NumYIndep-Fsx3;
   if(Depedente(Vsy3->Znach)) NumYDep=NumYDep-Fsy3;
   else NumYIndep=NumYIndep+Fsy3;
 }}
 NumX=NumXDep+NumXIndep;   NumY=NumYDep+NumYIndep; Denum=DenumDep+DenumIndep;
 Vzn =TakeVary(L"Zn");      Vzn->Znach=Denum;
 Vnmx=TakeVary(L"ChX");     Vnmx->Znach=NumX;
 Vnmy=TakeVary(L"ChY");     Vnmy->Znach=NumY;
 Vx  =TakeVary(L"Xcof");    Vy =TakeVary(L"Ycof");
 Vx->Znach=NumX/Denum;     Vy->Znach=NumY/Denum;
 Vx_ =TakeVary(NameX_cof); Vy_=TakeVary(NameY_cof); FVx_.V=Vx_; FVy_.V=Vy_;
// U1=NumX-FVx_*Denum; U2=NumY-FVy_*Denum;
 U1=NumX/Denum-FVx_; U2=NumY/Denum-FVy_;
 D1=Val(U1);
 D2=Val(U2);
 dU1=2.0*(U1*DifBy(U1,Vc1)+U2*DifBy(U2,Vc1));
 dU2=2.0*(U1*DifBy(U1,Vc2)+U2*DifBy(U2,Vc2));
 D1=Val(dU1);
 D2=Val(dU2);
 C1[0]=C1[1]=C1[2]=Vc1->Val; C2[0]=C2[1]=C2[2]=Vc2->Val;
 V1[0]=V1[1]=V1[2]=Vv1->Val; V2[0]=V2[1]=V2[2]=Vv2->Val;
 W1[0]=W1[1]=W1[2]=Vw1->Val; W2[0]=W2[1]=W2[2]=Vw2->Val;
 L->Time->P=0; if(Wg) dt=(Tk-Tn)/(Wg-1); else dt=0.001;
PRINTVARS=true; _wunlink(L"PrintVars.txt");
 for(double t=Tn+dt,i=1;t<=Tk;t+=dt,i++){
  double dC=0.001/*,Dx,Dy,Dx_,Dy_*/;
  L->Time->Val=t;
  V1[2]=V1[1]; V2[2]=V2[1]; C1[2]=C1[1]; C2[2]=C2[1];
  V1[1]=V1[0]; V2[1]=V2[0]; C1[1]=C1[0]; C2[1]=C2[0];
  ClearVar();
//if(t>0.5){
// Dx=Val(Vx->Znach);
// Dy=Val(Vy->Znach);
// Dx_=Val(Vx_->Znach);
// Dy_=Val(Vy_->Znach);
//}
  D1=Val(U1);
  D2=Val(U2);
  D=pow(D1,2.0)+pow(D2,2.0); l=0.0001;
  while(D>e){
//   ClearVar();
//   g1=Val(dU1); g2=Val(dU2);
   Vc1->Val-=dC;
   VW1(i,Vc1->Val);
   ClearVar();
   D1=Val(U1); D2=Val(U2); D=pow(D1,2.0)+pow(D2,2.0);
   Vc1->Val+=2*dC;
   VW1(i,Vc1->Val);
   ClearVar();
   D1=Val(U1); D2=Val(U2);
   g1=(pow(D1,2.0)+pow(D2,2.0)-D)/2/dC;
   Vc1->Val-=dC;
   Vc2->Val-=dC;
   VW2(i,Vc2->Val);
   ClearVar();
   D1=Val(U1); D2=Val(U2); D=pow(D1,2.0)+pow(D2,2.0);
   Vc2->Val+=2*dC;
   VW2(i,Vc2->Val);
   ClearVar();
   D1=Val(U1); D2=Val(U2);
   g2=(pow(D1,2.0)+pow(D2,2.0)-D)/2/dC;
   Vc2->Val-=dC;
   ClearVar();
   C1[0]=Vc1->Val-l*g1; Vc1->Val=C1[0];
   C2[0]=Vc2->Val-l*g2; Vc2->Val=C2[0];
   D1=Val(U1);
   D2=Val(U2);
   D=pow(D1,2.0)+pow(D2,2.0);
  }
  PRINTVARS=false;
}}
//---------------------------------------------------------------------------
void VW(int i,double Val){
 Vv->Val=(Val-C[1])/dt;
// if(V[1]==V[2]) Vw->Val=W[0]=0;
// else
 V[0]=Vv->Val;
  Vw->Val=W[0]=(V[0]-V[1])/dt;
}
//---------------------------------------------------------------------------
void Metrica(TCanvas *Canvas){
 TRect Rect=Canvas->ClipRect;
 int W=Rect.Width(),i,j,Ps,LPx,Wgmm/*,HorzSize*/,Wo,Wu;
// HorzSize=GetDeviceCaps(Canvas->Handle,HORZSIZE); //mm
 LPx=GetDeviceCaps(Canvas->Handle,LOGPIXELSX);
 Canvas->Font->Size=*SzFont;
 Wo=Canvas->TextWidth(L"000.000E+00"); Wu=PRNTOPRIGHT?Wo:0;
 Wg=W-Wo-Wu; Wgmm=Wg*25.4/LPx;
 KP[0]=1+Wgmm/Sg;
 dS[0]=Round((double)(Sg*LPx)/25.4);
}
//---------------------------------------------------------------------------
bool ContrCOF(Parm *Par,TRichEdit *R,TCanvas *Canvas){
 int i,j,K=100; double l=0.1,e=1e-6,g,D,**Q,Ps,dCmax=0.05,dC,dT=0.01,t;
 Root *Ins; Parm *P; wchar_t *s,NameB[256],Head[256]; bool Pech=true; FILE *F;
//найдем времена - начальное и конечное
 if((Ins=FindInstLast(L"¬–≈ћя"))||(Ins=FindInstLast(L"¬ј–№»–ќ¬ј“№"))){
  if(KolElem(Ins->First->R)==2){//границы в скобках
   Tn=!Ins->First->R->Nam ? Ins->First->R->Val:
	  Val(StrToForm(Ins->First->R->Nam));
   Tk=Tend=!Ins->First->R->Sled->Nam ? Ins->First->R->Sled->Val:
	  Val(StrToForm(Ins->First->R->Sled->Nam));
  }
  else{//границы подр€д в списке
   Tn=!Ins->First->Sled->Nam ? Ins->First->Sled->Val :
	   Val(StrToForm(Ins->First->Sled->Nam));
   if(Ins->First->Sled->Sled)
	Tk=Tend=!Ins->First->Sled->Sled->Nam ? Ins->First->Sled->Sled->Val :
	   Val(StrToForm(Ins->First->Sled->Sled->Nam));
   else{
	swprintf(Inf,L"\n¬ инструкции \"%s\" не указана верхн€€ граница времени",
	 Ins->Name);
	Error(0,0,true);
 }}}
 TakeDifTime(NULL,NULL,false);
//–азберем инструкцию L"”ѕ–ј¬Ћ≈Ќ»≈ COF"
/*”правление COF : абсциссы COF(X_cof,Xcof),управл€юща€ координата(tau(0));*/
 if(FindParmUpp(Par,L"јЅ—÷»——џ COF",&P)){
  wcscpy(NameXcof,P->R->Nam); wcscpy(NameX_cof,P->R->Sled->Nam);
 }
 if(FindParmUpp(Par,L"”ѕ–ј¬Ћяёўјя  ќќ–ƒ»Ќј“ј",&P)){
//сформируем имена и сами переменные дл€ управл€ющих координат, их скоростей и ускорений
  wcscpy(NameCoord,P->R->Nam);                        Vc=TakeVary(NameCoord);
  swprintf(NameVCord,L"%s'%s",NameCoord,L->Time->Name); Vv=TakeVary(NameVCord);
  swprintf(NameWCord,L"%s'%s",NameVCord,L->Time->Name); Vw=TakeVary(NameWCord);
//получим начальное значение управл€ющей координаты
  Vc->P=Vv->P=Vw->P=0;
  L->Time->Val=0; ClearVar();
  Vc->Val=ValOfParm(P->R->R);
  Vv->Val=ValOfParm(P->R->R->Sled);
 }
//накопим формулы дл€ зависимых и независимых от управл€ющих координат,
//их скоростей и ускорений
 Vx=TakeVary(NameXcof); Vx_=TakeVary(NameX_cof); FVx.V=Vx; FVx_.V=Vx_;
 C[0]=C[1]=C[2]=Vc->Val; V[0]=V[1]=V[2]=Vv->Val;  W[0]=W[1]=W[2]=Vw->Val;
 L->Time->P=0;
 Metrica(Canvas);
 MSt[0]=(Tk-Tn)/(KP[0]-1);
 for(Ps=0;MSt[0]>10.0;Ps++) MSt[0]/=10.0;
 for(;MSt[0]<1.0;Ps--) MSt[0]*=10.0;
 for(j=1;j<Kmg;j++) if(MSt[0]<=MG[j]){ MSt[0]=MG[j]; break; }
 for(j=abs(Ps);j--;) if(Ps>0) MSt[0]*=10; else MSt[0]/=10;
 MstT[0]=MSt[0]/Sg; MSt[0]/=dS[0];
 Wg=1+Round((Tk-Tn)/MSt[0]);
 dt=(Tk-Tn)/(Wg-1); t=Tn;
 dC=0.0001; //dC=dCmax*dt/(K*dT);
 Q=(double **)calloc(3,SzD);
 for(i=0;i<3;i++) Q[i]=(double *)calloc(Wg,SzD);
 Q[0][0]=Vc->Val; Q[1][0]=Vv->Val; Q[2][0]=Vw->Val;
 int jo=-200;
 for(i=1;i<Wg;i++){ double Dx,Dx_;
  L->Time->Val=t=Tn+i*dt;
  V[2]=V[1]; C[2]=C[1]; V[1]=V[0]; C[1]=C[0];
  ClearVar();
//PRINTVARS=true;
Dx_=Val(FVx_);
//if(jo==-200){
// PRINTVARS=true; _wunlink(L"PrintVars.txt");
//}
//else
// PRINTVARS=false;
  Dx=Val(FVx); D=pow(Dx-Dx_,2); j=0;
  if(D>e){
   D=1.7e308;
   for(j=-K;j<=K;j++){
	Vc->Val=C[1]+j*dC;
	VW(i,Vc->Val);
	ClearVar();
//if(j==-K||j==0||j==K){
// PRINTVARS=true; _wunlink(L"PrintVars.txt");
//}
	Dx=Val(FVx);
//PRINTVARS=false;
	if(pow(Dx-Dx_,2.0)<D){
	 C[0]=Vc->Val; V[0]=Vv->Val; W[0]=Vw->Val; jo=j;
	 D=pow(Dx-Dx_,2);
	}
   }
   Vc->Val=C[0]; Vv->Val=V[0]; Vw->Val=W[0];
   VW(i,Vc->Val);
   ClearVar();
  }
/*  while(D>e){ j++;
   Vc->Val-=dC;
   VW(i,Vc->Val);
   ClearVar();
   Dx=Val(FVx); D=pow(Dx-Dx_,2);
   Vc->Val+=2*dC;
   VW(i,Vc->Val);
   ClearVar();
   Dx=Val(FVx);
   g=(pow(Dx-Dx_,2.0)-D)/2/dC;
   Vc->Val-=dC;
   VW(i,Vc->Val);
   ClearVar();
   C[0]=Vc->Val-l*g;
   while(C[0]>2*M_PI) C[0]-=2*M_PI;
   while(C[0]<-2*M_PI) C[0]+=2*M_PI;
   Vc->Val=C[0];
   VW(i,Vc->Val);
   Dx=Val(FVx); D=pow(Dx-Dx_,2);
  }*/
  if(C[0]-C[1]>dCmax) C[0]=C[1]+dCmax;
  if(C[0]-C[1]<-dCmax) C[0]=C[1]-dCmax;
  Vc->Val=C[0];
  VW(i,Vc->Val);
  Q[0][i]=Vc->Val; Q[1][i]=Vv->Val; Q[2][i]=Vw->Val;
//  PRINTVARS=false;
 }
 for(i=0;i<3;i++){
  Form Ft; wchar_t NameSpln[80],NameArr[80]; Ft.V=L->Time;
  swprintf(NameSpln,L"%s",i==0?NameCoord:i==1?NameVCord:NameWCord);
  swprintf(NameArr,L"%s_arr",NameSpln);
  Arra *Arr=TakeArra(NameArr); Arr->N=2*Wg;
  Arr->A=(double *)calloc(Arr->N,SzD);
  for(j=0;j<Wg;j++){
   Arr->A[2*j]=dt*j;
   Arr->A[2*j+1]=Q[i][j];
  }
  (i==0?Vc:i==1?Vv:Vw)->Znach.P=TakeSpln(1,Arr,Ft);
 }
 Vc->P=Vv->P=Vw->P=1;
 return true;
}

