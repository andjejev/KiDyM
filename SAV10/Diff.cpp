#include "CA.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <io.h>
#include <values.h>

extern List *L,*LBeg;
extern Cnst *Nul,*Odin,*Dva,*Tri,*Pust;
extern wchar_t Inf[],ErrFile[],Serr[],SimbInd,*serr; extern FILE *ERRFILE;
extern TStringList *SLD,*SLH,*SLO,*SLE; extern String SF;
extern double h_RK,Eps,Eta;
extern bool NoCalc,OTLADKA,VISIBLEDIFFOP,PRINTVARS;
bool RUNGE_ORIGINAL(double x,double y,double (*f)(double x),double eps,
  double eta,double xfin,double &yfin,wchar_t *V);
extern Form FIntg; extern Vary *VArg; extern FILE *FFF;
extern Magazine *MVars;

double Ipl(int N,double *Tab,double X){ int i;
 for(i=N-4;i&&(X<Tab[i]);i-=2);
 return(Tab[i+3]-Tab[i+1])/(Tab[i+2]-Tab[i])*(X-Tab[i])+Tab[i+1];
}

double Ipl(Ipll *I,double X){ int i;
 int N=I->A->N;
 if(I->A->A){
  double *Tab=I->A->A;
  for(i=N-4;i&&(X<Tab[i]);i-=2);
  return(Tab[i+3]-Tab[i+1])/(Tab[i+2]-Tab[i])*(X-Tab[i])+Tab[i+1];
 }
 else if(I->A->F){
  for(i=N-4;i&&(X<Val(I->A->F[i]));i-=2);
  return(Val(I->A->F[i+3])-Val(I->A->F[i+1]))/
        (Val(I->A->F[i+2])-Val(I->A->F[i]))*(X-Val(I->A->F[i]))+Val(I->A->F[i+1]);
 }
 else return 0.0;
}

double Spl(Spln *S,double X){ int i,Int; double Dx;
 if(!S->a&&!S->b&&!S->c&&!S->d) S->CalcSpln();
 for(i=S->A->N-4;i&&(X<S->A->A[i]);i-=2); Int=i/2; Dx=X-S->A->A[i];
 return S->a[Int]+S->b[Int]*Dx+S->c[Int]*Dx*Dx+S->d[Int]*Dx*Dx*Dx;
}

void SbrosDif(void){ List *LIST;
 for(LIST=LBeg;LIST;LIST=LIST->Sled)
  for(Vary *V=LIST->V;V;V=V->Sled) V->Q=abs(V->Q);
}

Form DifBy(Form F,Vary *V);//полная частная производная F по V
Form DifByT(Vary *X);//полная производная по времени от переменной Х

Form DifByT(Form F){
 Form u,v,us,vs; AnsiString S; //Vary *t=L->Time;
 switch(F.C->Atr){
  case PUST:
  case CNST: return(F.C=Nul),F;
  case VARY:
   F=DifByT(F.V);
   return F;
  case UNOP:
   u=F.U->Arg;
   v=DifByT(u);
   switch(F.U->Func){
    case  1: return -v;
    case  2: return(0.5*v)/F;
    case  3: return v*cos(u);
    case  4: return(-v)*sin(u);
    case  5: return v*(1 +(F^2));
    case  6: return v/sqrt(1-(u^2));
    case  7: return v/(1+(u^2));
    case  8: return v*F;
    case  9: return v/u;
    case 10: return v*ch(u);
    case 11: return v*sh(u);
    case 12: return v/sqrt(1+(u^2));
    case 13:
     Form Nl; Nl.C=Nul;
     F.L=TakeIfop(4,F.U->Arg,Nl,v,-v);
     return F;
    case 14:
     wcscpy(Inf,L"Попытка дифференцировать случайную функцию."); ErrorSynt(0,0);
     return (F.C=Pust),F;
    case 15: //ceil
    case 16: //floor
     return (F.C=Nul),F;
    case 17: return (-v)/sqrt(1-(u^2));
    case 18: //!
     return (F.C=Nul),F;
   }
   return (F.C=Nul),F;
  case BNOP:
   u=F.B->L; v=F.B->R; us=DifByT(u); vs=DifByT(v);
   switch(F.B->Oper){
    case  1:
     return us + vs;
    case  2:
     return us - vs;
    case  3:
     return((u*vs)+(us*v));
    case  4:
     return((us*v)-(u*vs))/(v^2);
    case  5:
     F=us;
//     F.C=Nul;
     return F;
    case  6:
     return F*((vs*ln(u))+((us*v)/u));
   }
   return (F.C=Nul),F;
  case IFOP:
   u=DifByT(F.L->FT); v=DifByT(F.L->FF);
   return
    F.L=TakeIfop(F.L->Oper,F.L->U,F.L->V,u,v),F;
  case SUMM:
   F.S=TakeSumm(DifByT(F.S->F),F.S->i,F.S->a,F.S->b);
   return F;
  case INTG:
   if(F.G->Type==1)
    F.G=TakeIntg(DifByT(F.G->f),F.G->x,DifByT(F.G->a),DifByT(F.G->b));
   else
    F.G=TakeIntg(DifByT(F.G->f),F.G->x);
   return F;
 }
 return (F.C=Nul),F;
}

Form Dif(Form F,Vary *X){
 Form u,v,us,vs; AnsiString S;
 switch(F.C->Atr){
  case PUST:
  case CNST: return(F.C=Nul),F;
  case VARY:
   if(F.V==X) return(F.C=Odin),F;
   else{
    Vary *VA; wchar_t Buf[256];
    swprintf(Buf,L"%s'%s",F.V->Name,X->Name); VA=TakeVary(Buf);
    if(VA->Q>0){
     VA->Q=-1;
     VA->Znach=Dif(F.V->Znach,X);
    }
    switch(VA->Znach.C->Atr){
     case PUST: F.C=Nul; break;
     case CNST: F.C=VA->Znach.C; break;
     default: F.V=VA;
   }}
   return F;
  case UNOP:
   u=F.U->Arg;
//if(VISIBLEDIFFOP)
//FormToStr(u,&S);
//if(!S.IsEmpty())
// Application->MessageBoxA(S.c_str(),L"Diff: UNOP: u=L",MB_OK);
   if(!VarInForm(false,u,X)){
    F.C=Nul; return F;
   }
   v=Dif(u,X);
   switch(F.U->Func){
    case  1: return -v;
    case  2: return(0.5*v)/F;
    case  3: return v*cos(u);
    case  4: return(-v)*sin(u);
    case  5: return v*(1 +(F^2));
    case  6: return v/sqrt(1-(u^2));
    case  7: return v/(1+(u^2));
    case  8: return v*F;
    case  9: return v/u;
    case 10: return v*ch(u);
    case 11: return v*sh(u);
    case 12: return v/sqrt(1+(u^2));
    case 13:
     Form Nl; Nl.C=Nul;
     F.L=TakeIfop(4,F.U->Arg,Nl,v,-v);
     return F;
    case 14:
     wcscpy(Inf,L"Попытка дифференцировать случайную функцию."); ErrorSynt(0,0);
     return (F.C=Pust),F;
    case 15: //ceil
    case 16: //floor
     return (F.C=Nul),F;
    case 17: return (-v)/sqrt(1-(u^2));
    case 18: //!
     return (F.C=Nul),F;
   }
  case BNOP:
   u=F.B->L; v=F.B->R;
//if(VISIBLEDIFFOP){
//FormToStr(u,&S);
//if(!S.IsEmpty())
// Application->MessageBoxA(S.c_str(),L"Diff: BNOP: u=L",MB_OK);
//FormToStr(v,&S);
//if(!S.IsEmpty())
// Application->MessageBoxA(S.c_str(),L"Diff: BNOP: v=L",MB_OK);
//}
   if(!VarInForm(false,u,X)&&!VarInForm(false,v,X)){
    F.C=Nul; return F;
   }
   if(VarInForm(false,u,X))
    us=Dif(u,X);
   else
    us.C=Nul;
   if(VarInForm(false,v,X))
    vs=Dif(v,X);
   else
    vs.C=Nul;
   switch(F.B->Oper){
    case  1:
     if(!VarInForm(false,u,X))
      return vs;
     if(!VarInForm(false,v,X))
      return us;
     return us + vs;
    case  2:
     if(!VarInForm(false,u,X))
      return -vs;
     if(!VarInForm(false,v,X))
      return  us;
     return us - vs;
    case  3:
     if(!VarInForm(false,u,X))
      return u*vs;
     if(!VarInForm(false,v,X))
      return v*us;
     return((u*vs)+(us*v));
    case  4:
     if(!VarInForm(false,u,X)) return -(u*vs)/(v^2);
     if(!VarInForm(false,v,X)) return (1/v)*us; return((us*v)-(u*vs))/(v^2);
    case  5:
     F=us;
     return F;
    case  6:
     if(!VarInForm(false,u,X))
      return (ln(u)*vs)*F;
     if(!VarInForm(false,v,X))
      return v*(u^(v-1))*us;
     return F*((vs*ln(u))+((us*v)/u));
   }
  case IFOP:
   u=Dif(F.L->FT,X); if(F.L->FF.C!=Pust) v=Dif(F.L->FF,X); else v.C=Pust;
   return
    F.L=TakeIfop(F.L->Oper,F.L->U,F.L->V,u,v),F;
  case SUMM:
   F.S=TakeSumm(Dif(F.S->F,X),F.S->i,F.S->a,F.S->b);
   return F;
  case INTG:
   if(F.G->Type==1)
    F.G=TakeIntg(Dif(F.G->f,X),F.G->x,
         VarInForm(true,F.G->a,X)?Dif(F.G->a,X):F.G->a,
         VarInForm(true,F.G->b,X)?Dif(F.G->b,X):F.G->b);
   else
    F.G=TakeIntg(Dif(F.G->f,X),F.G->x);
   return F;
  case IPLL:
   switch(F.I->Type){
    case 1: Form g;
     if(g=Dif(F.I->F,X),g.C==Nul) return F.C=Nul,F;
     else{
      swprintf(Inf,L"%s'",F.I->A->Name); Arra *Arr=TakeArra(Inf);
      double *Tabl=F.I->A->A; double Xa,Xb,Ya,Yb; int n=F.I->A->N;
      double *TablS=(double *)calloc(n-2,SzD);
      Xa=Tabl[0]; Ya=Tabl[1];
      for(int i=2;i<n;i+=2){
       Xb=Tabl[i]; Yb=Tabl[i+1];
       TablS[i-2]=(Xa+Xb)/2; TablS[i-1]=(Yb-Ya)/(Xb-Xa);
       Xa=Xb; Ya=Yb;
      }
      Arr->A=TablS; Arr->N=n-2; F.I=TakeIpll(1,Arr,F.I->F); return F*g;
     }
    case 2: F.C=Nul;
   }
   return F;
  case SPLN:
   switch(F.P->Type){
    case 1: Form g;
     if(g=Dif(F.P->F,X),g.C==Nul) return F.C=Nul,F;
     else{
      Arra *Arr; int n=F.P->A->N/2-1,i;
	  double t,*as,*a,*bs=(double *)calloc(n,SzD),*b=bs,*cc,*dd,
	   *cs=(double *)calloc(n,SzD),*c=cs,
       *ds=(double *)calloc(n,SzD);
      swprintf(Inf,L"%s'%s",F.P->A->Name,L->Time->Name);
      Arr=TakeArra(Inf); Arr->N=F.P->A->N;
      a=as=F.P->b; cc=F.P->c; dd=F.P->d;
      for(int i=0;i<n;i++){ *b++=2*(*cc++); *c++=3*(*dd++); }
      if(Arr->A) free(Arr->A); Arr->A=(double *)calloc(Arr->N,SzD);
      for(i=0;i<n;i++){
       Arr->A[2*i]=F.P->A->A[2*i];
       Arr->A[2*i+1]=*a++;
      }
      Arr->A[2*i]=F.P->A->A[2*i]; t=F.P->A->A[2*i]-F.P->A->A[2*i-2];
      Arr->A[2*i+1]=a[-1]+b[-1]*t+c[-1]*t*t;
      F.P=TakeSpln(1,Arr,as,bs,cs,ds,F.P->F);
      F=F*g;
     }
     break;
    default: F.C=Nul;
   }
   return F;
  case RUUT:
   u=Dif(F.R->f,X);
   if(u!=Nul) F.R=TakeSolu(u,F.R->x,F.R->a,F.R->b); else F=u;
   return F;
  case REST: return Dif(F.O->a,X);
  case ARRI:
   if(F.a->A->F){
    int i=Val(F.a->FInd);
    if(!i) F.C=Nul;
    else F=Dif(F.a->A->F[i-1],X);
   }
   else F.C=Nul;
   return F;
  default: return(F.C=Nul),F;
}}

Form DifBy(Form F,Vary *X){
 if(VarInForm(false,F,X))
  F=Dif(F,X);
 else
  F.C=Nul;
 return F;
}

bool VarInForm1(Form F,Vary *X){//зависит ли F непосредственно от X
 switch(F.C->Atr){ int i;
  case PUST: case CNST: return false;
  case VARY:
   if(F.V==X) return true;
   if(F.V->Znach.C==Pust) return true;
   return VarInForm1(F.V->Znach,X);
  case UNOP: return VarInForm1(F.U->Arg,X);
  case BNOP:
   return VarInForm1(F.B->L,X)||VarInForm1(F.B->R,X);
  case IFOP:
   return
    VarInForm1(F.L->FT,X)||VarInForm1(F.L->FF,X)||
    VarInForm1(F.L->U,X)||VarInForm1(F.L->V,X);
  case SUMM:
   return
    VarInForm1(F.S->F,X)||VarInForm1(F.S->a,X)||
    VarInForm1(F.S->b,X)||VarInForm1(F.S->i->Znach,X);
  case INTG:
   return
    VarInForm1(F.G->f,X)||VarInForm1(F.G->a,X)||
    VarInForm1(F.G->b,X)||VarInForm1(F.G->x->Znach,X);
  case IPLL:
   return VarInForm1(F.I->F,X);
  case SPLN:
   return VarInForm1(F.P->F,X);
  case RUUT:
   return (X==F.R->x)||VarInForm1(F.R->x->Znach,X);
  case REST:
   return VarInForm1(F.O->a,X)||VarInForm1(F.O->b,X);
  case ARRI:
   if(F.a->A->F){
    i=Val(F.a->FInd);
    return VarInForm1(F.a->FInd,X)||(i>0&&VarInForm1(F.a->A->F[i-1],X));
   }
   else
    return VarInForm1(F.a->FInd,X);
  default: return false;
}}

bool VarFromT(Vary *X){ Vary *V; Cord *K;
//переменная X зависит явно или неявно (через обобщенные координаты,
//обобщенные и псевдоскорости) от времени t?
 if(X==L->Time) return true;
 if((X->Znach==Pust)||VarInForm1(X->Znach,L->Time)) return true;
 for(K=L->K;K;K=K->Sled){
  if(X==K->V) return true;
  if(VarInForm1(X->Znach,K->V)) return true;
  swprintf(Inf,L"%s'%s",K->V->Name,L->Time->Name);
  if(FindVary(Inf,&V)&&(V->Znach.C==Pust)&&
   (X==V||VarInForm1(X->Znach,V))) return true;
 }
 for(K=L->Z;K;K=K->Sled){
  if(X==K->V) return true;
  if(VarInForm1(X->Znach,K->V)) return true;
  swprintf(Inf,L"%s'%s",K->V->Name,L->Time->Name);
  if(FindVary(Inf,&V)&&(V->Znach.C==Pust)&&
   (X==V||VarInForm1(X->Znach,V))) return true;
 }
 for(K=L->W;K;K=K->Sled){
  if(X==K->V) return true;
  if(VarInForm1(X->Znach,K->V)) return true;
 }
 return false;
}

Form DifT1(Form F){//частная производная по явно входящему времени
 Form u,v,us,vs; wchar_t Buf[256];
 switch(F.C->Atr){
  case PUST: case CNST: F.C=Nul; return F;
  case VARY: F.C=(F.V==L->Time)?Odin:Nul; return F;
  case BNOP:
   u=F.B->L; v=F.B->R; us=DifT1(u); vs=DifT1(v);
   switch(F.B->Oper){
    case  1: return us + vs;
    case  2: return us - vs;
    case  3: return((u*vs)+(us*v));
    case  4:
     if(!VarInForm1(u,L->Time)) return -(u*vs)/(v^2);
     if(!VarInForm1(v,L->Time)) return us/v;
     return((us*v)-(u*vs))/(v^2);
    case  5:
     F=us;
//     F.C=Nul;
     return F;
    case  6:
     if(!VarInForm1(u,L->Time)) return(ln(u)*vs)*F;
     if(!VarInForm1(v,L->Time)) return v*(u^(v-1))*us;
     return F*((vs*ln(u))+((us*v)/u));
   }
   break;
  case UNOP:
   u=F.U->Arg; v=DifT1(u);
   switch(F.U->Func){
    case  1: return -v;               //минус
    case  2: return(0.5*v)/F;         //квадратный корень
    case  3: return v*cos(u);         //sin
    case  4: return(-v)*sin(u);       //cos
    case  5: return v*(1+(F^2));     //tg
    case  6: return v/sqrt(1-(u^2)); //arcsin
    case  7: return v/(1+(u^2));     //arctg
    case  8: return v*F;              //exp
    case  9: return v/u;              //ln
    case 10: return v*ch(u);          //sh
    case 11: return v*sh(u);          //ch
    case 12: return v/sqrt(1+(u^2));  //arsh
    case 13:                          //abs
     Form Nl; Nl.C=Nul;
     F.L=TakeIfop(4,F.U->Arg,Nl,v,-v);
     return F;
    case 14:
     wcscpy(Inf,L"Попытка дифференцировать случайную функцию."); ErrorSynt(0,0);
     return (F.C=Pust),F;
    case 15: //ceil
    case 16: //floor
     return(F.C=Nul),F;
    case 17: return (-v)/sqrt(1-(u^2)); //arccos
    case 18: //!
     return(F.C=Nul),F;
   }
  case IFOP:
   u=DifT1(F.L->FT); if(F.L->FF.C!=Pust) v=DifT1(F.L->FF); else v.C=Pust;
   return
    F.L=TakeIfop(F.L->Oper,F.L->U,F.L->V,u,v),F;
  case SUMM:
   F.S=TakeSumm(DifT1(F.S->F),F.S->i,F.S->a,F.S->b);
   return F;
  case INTG:
   if(F.G->Type==DEFINE)
    F.G=TakeIntg(DifT1(F.G->f),F.G->x,F.G->a,F.G->b);
   else
    F.G=TakeIntg(DifT1(F.G->f),F.G->x);
   return F;
  case IPLL:
   switch(F.I->Type){
    case 1: Form g;
     if(g=DifT1(F.I->F),g.C==Nul) return F.C=Nul,F;
     else{
      swprintf(Inf,L"%s'",F.I->A->Name); Arra *Arr=TakeArra(Inf);
      double *Tabl=F.I->A->A; double Xa,Xb,Ya,Yb; int n=F.I->A->N;
      double *TablS=(double *)calloc(n-2,SzD);
      Xa=Tabl[0]; Ya=Tabl[1];
      for(int i=2;i<n;i+=2){
       Xb=Tabl[i]; Yb=Tabl[i+1];
       TablS[i-2]=(Xa+Xb)/2; TablS[i-1]=(Yb-Ya)/(Xb-Xa);
       Xa=Xb; Ya=Yb;
      }
      Arr->A=TablS; Arr->N=n-2; F.I=TakeIpll(1,Arr,F.I->F); return F*g;
     }
    case 2: return F.C=Nul,F;
   }
  case SPLN:
   if(F.P->Type==1){
    Form g=DifT1(F.P->F);
    if(g==Nul) F.C=Nul;
    else{
     Arra *Arr; int n=F.P->A->N/2-1,i;
	 double t,*as,*a,*bs=(double *)calloc(n,SzD),*b=bs,*cc,*dd,
	  *cs=(double *)calloc(n,SzD),*c=cs,
	  *ds=(double *)calloc(n,SzD);
     swprintf(Inf,L"%s'%s",F.P->A->Name,L->Time->Name);
     Arr=TakeArra(Inf); Arr->N=F.P->A->N;
     a=as=F.P->b; cc=F.P->c; dd=F.P->d;
     for(int i=0;i<n;i++){ *b++=2*(*cc++); *c++=3*(*dd++); }
     if(Arr->A) free(Arr->A); Arr->A=(double *)calloc(Arr->N,SzD);
     for(i=0;i<n;i++){
      Arr->A[2*i]=F.P->A->A[2*i];
      Arr->A[2*i+1]=*a++;
     }
     Arr->A[2*i]=F.P->A->A[2*i]; t=F.P->A->A[2*i]-F.P->A->A[2*i-2];
     Arr->A[2*i+1]=a[-1]+b[-1]*t+c[-1]*t*t;
     F.P=TakeSpln(1,Arr,as,bs,cs,ds,F.P->F);
     F=F*g;
    }
    return F;
   }
   else return F.C=Nul,F;
  case RUUT:
   return F.C=Nul,F;
  case REST:
   return DifT1(F.O->a);
  case ARRI:
   if(F.a->A->F){
    int i=Val(F.a->FInd);
    if(!i){ F.C=Nul; return F; }
    return DifT1(F.a->A->F[i-1]);
   }
   else{ F.C=Nul; return F; }
 }
 F.C=Nul; return F;
}

Form Dif1(Form F,Vary *X){
//частная производная F по явно входящей переменной X
 int i; wchar_t Buf[80]; Form u,v,us,vs;
 switch(F.C->Atr){
  case PUST: case CNST: F.C=Nul; return F;
  case VARY: F.C=(F.V==X)?Odin:Nul; return F;
  case BNOP:
   u=F.B->L; v=F.B->R; us=Dif1(u,X); vs=Dif1(v,X);
   switch(F.B->Oper){
    case  1: return us + vs;
    case  2: return us - vs;
    case  3: return (u*vs)+(us*v);
    case  4:
     if(!VarInForm1(u,X)) return -(u*vs)/(v^2);
     if(!VarInForm1(v,X)) return(1/v)*us; return((us*v)-(u*vs))/(v^2);
    case  5:
     F=us;
//     F.C=Nul;
     return F;
    case  6:
     if(!VarInForm1(u,X)) return(ln(u)*vs)*F;
     if(!VarInForm1(v,X)) return v*(u^(v-1))*us;
     return F*((vs*ln(u))+((us*v)/u));
   }
  case UNOP:
   u=F.U->Arg; if(!VarInForm1(u,X)){ F.C=Nul; return F; } v=Dif1(u,X);
   switch(F.U->Func){
    case  1: return -v;
    case  2: return(0.5*v)/F;
    case  3: return v*cos(u);
    case  4: return -(v*sin(u));
    case  5: return v*(1 +(F^2));
    case  6: return v/sqrt(1 -(u^2));
    case  7: return v/(1 +(u^2));
    case  8: return v*F;
    case  9: return v/u;
    case 10: return v*ch(u);
    case 11: return v*sh(u);
    case 12: return v/sqrt(1+(u^2));
    case 13:
     Form Nl; Nl.C=Nul;
     F.L=TakeIfop(4,F.U->Arg,Nl,v,-v);
     return F;
    case 14:
     wcscpy(Inf,L"Попытка дифференцировать случайную функцию."); ErrorSynt(0,0);
     return (F.C=Pust),F;
    case 15: //ceil
    case 16: //floor
     return(F.C=Nul),F;
    case 17: return (-v)/sqrt(1-(u^2)); //arccos
    case 18: //!
     return(F.C=Nul),F;
   }
  case IFOP:
   u=Dif1(F.L->FT,X); if(F.L->FF.C!=Pust) v=Dif1(F.L->FF,X); else v.C=Pust;
   return
    F.L=TakeIfop(F.L->Oper,F.L->U,F.L->V,u,v),F;
  case SUMM:
   F.S=TakeSumm(Dif1(F.S->F,X),F.S->i,F.S->a,F.S->b);
   return F;
  case INTG:
   if(F.G->Type==DEFINE)
    F.G=TakeIntg(Dif1(F.G->f,X),F.G->x,F.G->a,F.G->b);
   else
    F.G=TakeIntg(Dif1(F.G->f,X),F.G->x);
   return F;
  case IPLL:
   switch(F.I->Type){
    case 1: Form g;
     if(g=Dif1(F.I->F,X),g.C==Nul) return F.C=Nul,F;
     else{
      swprintf(Inf,L"%s'",F.I->A->Name); Arra *Arr=TakeArra(Inf);
      double *Tabl=F.I->A->A; double Xa,Xb,Ya,Yb; int n=F.I->A->N;
      double *TablS=(double *)calloc(n-2,SzD);
      Xa=Tabl[0]; Ya=Tabl[1];
      for(int i=2;i<n;i+=2){
       Xb=Tabl[i]; Yb=Tabl[i+1];
       TablS[i-2]=(Xa+Xb)/2; TablS[i-1]=(Yb-Ya)/(Xb-Xa);
       Xa=Xb; Ya=Yb;
      }
      Arr->A=TablS; Arr->N=n-2; F.I=TakeIpll(1,Arr,F.I->F); return F*g;
     }
    case 2: return F.C=Nul,F;
   }
  case SPLN:
   switch(F.P->Type){
    case 1: Form g;
     if(g=Dif1(F.P->F,X),g.C==Nul) return F.C=Nul,F;
     else{
      Arra *Arr; int n=F.P->A->N/2-1,i;
      double t,*as,*a,*bs=(double *)calloc(n,SzD),*b=bs,*cc,*dd,
       *cs=(double *)calloc(n,SzD),*c=cs,
       *ds=(double *)calloc(n,SzD);
      swprintf(Inf,L"%s'%s",F.P->A->Name,L->Time->Name);
      Arr=TakeArra(Inf); Arr->N=F.P->A->N;
      a=as=F.P->b; cc=F.P->c; dd=F.P->d;
      for(int i=0;i<n;i++){ *b++=2*(*cc++); *c++=3*(*dd++); }
      if(Arr->A) free(Arr->A); Arr->A=(double *)calloc(Arr->N,SzD);
      for(i=0;i<n;i++){
       Arr->A[2*i]=F.P->A->A[2*i];
       Arr->A[2*i+1]=*a++;
      }
      Arr->A[2*i]=F.P->A->A[2*i]; t=F.P->A->A[2*i]-F.P->A->A[2*i-2];
      Arr->A[2*i+1]=a[-1]+b[-1]*t+c[-1]*t*t;
      F.P=TakeSpln(1,Arr,as,bs,cs,ds,F.P->F);
      return F*g;
     }
    default: return F.C=Nul,F;
   }
  case RUUT:
   return F.C=Nul,F;
  case REST:
   return Dif1(F.O->a,X);
  case ARRI:
   if(F.a->A->F){
    i=Val(F.a->FInd);
    if(!i){ F.C=Nul; return F; }
    return Dif1(F.a->A->F[i-1],X);
   }
   else{ F.C=Nul; return F; }
 }
 F.C=Nul; return F;
}

Form DifBy1(Form F,Vary *X){
 if(VarInForm1(F,X))
  F=Dif1(F,X);
 else F.C=Nul;
 return F;
}

void ListVary(Form F,Magazine **M){
//составляется список переменных, от которых непосредственно зависит F
 switch(F.C->Atr){
  case PUST: case CNST: return;
  case VARY:
   if(VarFromT(F.V)) TakeMagazine(M,F.V->Name);
   return;
  case BNOP:
   ListVary(F.B->L,M);
   ListVary(F.B->R,M);
   return;
  case UNOP:
   ListVary(F.U->Arg,M); return;
  case IFOP:
   ListVary(F.L->U,M);
   ListVary(F.L->V,M);
   ListVary(F.L->FT,M);
   ListVary(F.L->FF,M); return;
  case SUMM:
   ListVary(F.S->F,M);
   ListVary(F.S->a,M);
   ListVary(F.S->b,M);
   ListVary(F.S->i->Znach,M); return;
  case INTG:
   ListVary(F.G->f,M);
   ListVary(F.G->a,M);
   ListVary(F.G->b,M);
   ListVary(F.G->x->Znach,M); return;
  case IPLL:
   ListVary(F.I->F,M); return;
  case SPLN:
   ListVary(F.P->F,M); return;
  case ARRI:
   ListVary(F.P->F,M);
   if(F.a->A->F){
    int i=Val(F.a->FInd);
    if(i>0) ListVary(F.a->A->F[i-1],M);
   }
   break;
  default: return;
}}

Form DifT(Vary *X){//полная производная по времени от переменной X,
//получаемая как сумма частной производной X по явно входящему t и
//произведений частных производных X по явно входящим в ее выражение
//переменным, зависящим явно и неявно от времени, на полные
//их производные по времени, свернутые в промежуточные переменные
 Form F,f,df; F.C=Nul; Vary *V,*v; Magazine *ListV=NULL; wchar_t Buf[256];
//if(FindVary(L"XO",&V))
//F.C=Nul;
 ListVary(X->Znach,&ListV);//получили общий список переменных, входящих в Х
 if(!ListV) F.C=Pust;
 for(Magazine *m=ListV;m;m=m->Sled){
  if(!wcscmp(m->S,L->Time->Name)){
//если это время, то генерируем частную производную по времени
   if(KolElem(ListV)>1){
    swprintf(Buf,L"%s`%s",X->Name,L->Time->Name);
    V=TakeVary(Buf);
    if((V->Znach.C==Pust)&&(X->Znach.C!=Pust)) V->Znach=DifT1(X->Znach);
    f.V=V; F=F+f;
   }
   else F=DifT1(X->Znach);
  }
  else{
//иначе умножаем частную производнуюю Х по зависимой переменной на
//сгенерованную новую переменную - полную производную по времени от
//зависимой переменной
   swprintf(Buf,L"%s'%s",m->S,L->Time->Name);
   V=TakeVary(Buf); v=TakeVary(m->S);
   if((V->Razm.C==Pust)&&(v->Razm.C!=Pust)){
    List *Lo=L;  Form Razm; Vary *Vc;
    L=TakeList(L"РАЗМЕРНОСТИ");
    Vc=TakeVary(L"с"); Razm.V=Vc;
    V->Razm=v->Razm/Razm;
    L=Lo;
   }
   if((V->Znach.C==Pust)&&(v->Znach.C!=Pust)) V->Znach=DifT(v); f.V=V;
   swprintf(Buf,L"__%s'%s",X->Name,m->S);
   V=TakeVary(Buf);
   if(V->Znach.C==Pust) V->Znach=DifBy1(X->Znach,v);
   if(V->Znach.C==Odin) F=F+f;
   else{ df.V=V; F=F+df*f; }
//   F=F+DifBy1(X->Znach,v)*f;
   if((V->Razm.C==Pust)&&(X->Razm.C!=Pust)&&(v->Razm.C!=Pust)){
    V->Razm=X->Razm/v->Razm;
    if(V->Razm.C==Odin) V->Razm.C=Pust;
 }}}
 DelAllMagazine(&ListV);
 return F;
}

Form DifByT(Vary *X){ Form F;//полная производная по времени от переменной Х
 if(VarFromT(X)){ Vary *V;
  swprintf(Inf,L"%s'%s",X->Name,L->Time->Name); V=TakeVary(Inf);
  if(V->Znach.C==Pust)
   V->Znach=DifT(X);
  F.V=V;
 }
 else F.C=Nul;
 return F;
}

//typedef struct Spisok Spisok; Spisok *Sp,*Spk;
//struct Spisok{ Spisok *Sled; Vary *V; };

bool DetVar(bool Everywhere,Form F,Vary *X){ FILE *FF;
 switch(F.C->Atr){
  case PUST: //if(X==L->Time) return true;
  case CNST: return false;
  case VARY:
   if(F.V==X) return true;
   return DetVar(Everywhere,F.V->Znach,X);
  case UNOP: return DetVar(Everywhere,F.U->Arg,X);
  case BNOP:
   return DetVar(Everywhere,F.B->L,X) || DetVar(Everywhere,F.B->R,X);
  case IFOP:
   if(DetVar(Everywhere,F.L->FT,X) || DetVar(Everywhere,F.L->FF,X))
    return true;
   if(Everywhere)
    if(DetVar(Everywhere,F.L->U,X) || DetVar(Everywhere,F.L->V,X))
     return true;
   return false;
  case SUMM:
   if(DetVar(Everywhere,F.S->F,X)) return true;
   if(Everywhere){ Form FI; FI.V=F.S->i;
    if(DetVar(Everywhere,F.S->a,X) || DetVar(Everywhere,F.S->b,X) ||
       DetVar(Everywhere,FI,X)) return true;
   }
   return false;
  case INTG:
   if(DetVar(Everywhere,F.G->f,X)) return true;
   if(Everywhere){ Form FI; FI.V=F.G->x;
    if(DetVar(Everywhere,FI,X)||
       (F.G->a!=PUST)&&DetVar(Everywhere,F.G->a,X) ||
       (F.G->b!=PUST)&&DetVar(Everywhere,F.G->b,X)) return true;
   }
   return false;
  case IPLL: return DetVar(Everywhere,F.I->F,X);
  case SPLN: return DetVar(Everywhere,F.P->F,X);
  case RUUT: return (X==F.R->x)||DetVar(Everywhere,F.R->x->Znach,X);
  case REST: return DetVar(Everywhere,F.O->a,X)||DetVar(Everywhere,F.O->b,X);
  case ARRI:
   if(F.a->A->F){
    int i=Val(F.a->FInd);
    return DetVar(Everywhere,F.a->FInd,X)||i>0&&DetVar(Everywhere,F.a->A->F[i-1],X);
   }
   else return DetVar(Everywhere,F.a->FInd,X);
  default  : return false;
}}

bool VarInForm(bool Everywhere,Form F,Vary *X){
 return DetVar(Everywhere,F,X);
}

bool DetVarForIntg(bool Everywhere,Form F,Vary *X){
 switch(F.C->Atr){
  case PUST: if(X==L->Time) return true;
  case CNST: return false;
  case VARY:
   if(F.V==X) return true;
   return DetVarForIntg(Everywhere,F.V->Znach,X);
  case UNOP: return DetVarForIntg(Everywhere,F.U->Arg,X);
  case BNOP:
   return
    DetVarForIntg(Everywhere,F.B->L,X) || DetVarForIntg(Everywhere,F.B->R,X);
  case IFOP:
   if(DetVarForIntg(Everywhere,F.L->FT,X)||DetVarForIntg(Everywhere,F.L->FF,X))
    return true;
   if(Everywhere)
    if(DetVarForIntg(Everywhere,F.L->U,X)||DetVarForIntg(Everywhere,F.L->V,X))
     return true;
   return false;
  case SUMM:
   if(DetVarForIntg(Everywhere,F.S->F,X)) return true;
   if(Everywhere){ Form FI; FI.V=F.S->i;
    if(DetVarForIntg(Everywhere,F.S->a,X)||DetVarForIntg(Everywhere,F.S->b,X)||
       DetVarForIntg(Everywhere,FI,X)) return true;
   }
   return false;
  case INTG:
   if(DetVarForIntg(Everywhere,F.G->f,X)) return true;
   if(Everywhere){ Form FI; FI.V=F.G->x;
    if(DetVarForIntg(Everywhere,FI,X)||
       (F.G->a!=PUST)&&DetVarForIntg(Everywhere,F.G->a,X) ||
       (F.G->b!=PUST)&&DetVarForIntg(Everywhere,F.G->b,X)) return true;
   }
   return false;
  case IPLL: return DetVarForIntg(Everywhere,F.I->F,X);
  case SPLN: return DetVarForIntg(Everywhere,F.P->F,X);
  case RUUT: return (X==F.R->x)||DetVarForIntg(Everywhere,F.R->x->Znach,X);
  case REST:
   return DetVarForIntg(Everywhere,F.O->a,X)||DetVarForIntg(Everywhere,F.O->b,X);
  case ARRI:
   if(F.a->A->F){
    int i=Val(F.a->FInd);
    return
     DetVarForIntg(Everywhere,F.a->FInd,X)||
     i>0&&DetVarForIntg(Everywhere,F.a->A->F[i-1],X);
   }
   else return DetVarForIntg(Everywhere,F.a->FInd,X);
  default  : return false;
}}

bool VarInFormForIntg(bool Everywhere,Form F,Vary *X){
 return DetVarForIntg(Everywhere,F,X);
}

void ClearVar(void){ //int N=0;
 L->Time->P=abs(L->Time->P); L->Freq->P=abs(L->Freq->P);
 for(Vary *V=L->V;V;V=V->Sled){
//  N++;
  V->P=abs(V->P);
 }
 for(Unop *U=L->U;U;U=U->Sled) U->P=0;//abs(U->P);
 for(Intg *I=L->I;I;I=I->Sled) I->P=abs(I->P);
}

double FuncIntg(double x);

bool Valc(Form F,double &Znach){
 bool a; double R,u,v,r,ut; int i,n,pp; Vary *VArgp; Form FIntgp;
 switch(F.C->Atr){
  case PUST : Znach=0.0; return false;
  case CNST : Znach=F.C->Val; return true;
  case VARY :
   if(F.V->P<=0){ Znach=F.V->Val; return true; }
   if(Valc(F.V->Znach,Znach)){ F.V->P=-1; F.V->Val=Znach; return true; }
   return false;
  case UNOP:
   if(F.U->P){ Znach=F.U->Val; return true; }
   if(Valc(F.U->Arg,r)){
    F.U->P=1;
    switch(F.U->Func){
     case  1: Znach=F.U->Val=-r; return true;
     case  2:
      if(r<0.0){ Znach=0.0; return false; }
      Znach=F.U->Val=sqrt(r); return true;
     case  3: Znach=F.U->Val=sin(r); return true;
     case  4: Znach=F.U->Val=cos(r); return true;
     case  5:
      if((fabs(r)==M_PI/2)||(fabs(r)==3*M_PI/2)){ Znach=0.0; return false; }
      Znach=F.U->Val=tan(r); return true;
     case  6:
      if(fabs(r)>1.0){ Znach=0.0; return false; }
      Znach=F.U->Val=asin(r); return true;
     case  7: Znach=F.U->Val=atan(r); return true;
     case  8: Znach=F.U->Val=exp(r); return true;
     case  9:
      if(r>0.0){ Znach=F.U->Val=log(r); return true; }
      Znach=0.0; return false;
     case 10: Znach=F.U->Val=sinh(r); return true;
     case 11: Znach=F.U->Val=cosh(r); return true;
     case 12: Znach=F.U->Val=log(r+sqrt(1.0+r*r)); return true;
     case 13: Znach=F.U->Val=fabs(r); return true;
     case 14: Znach=F.U->Val=r*rand()/RAND_MAX; return true;
     case 15: Znach=F.U->Val=ceil(r); return true;
     case 16: Znach=F.U->Val=floor(r); return true;
     case 17:
      if(fabs(r)>1.0){ Znach=0.0; return false; }
      Znach=F.U->Val=acos(r); return true;
     case 18: //!
      n=(int)Round(r);
      if(!n){ Znach=1; return true; }
      else if(n>0){ Znach=1; for(int i=2;i<=n;i++) Znach*=i; return true; }
      return false;
   }}
   else{ Znach=0.0; return false; }
  case BNOP:
   if(Valc(F.B->L,u))
    if(Valc(F.B->R,v)){
    switch(F.B->Oper){
     case  1: Znach=u+v; return true;
     case  2: Znach=u-v; return true;
     case  3: Znach=u*v; return true;
     case  4:
      if(v==0.0){ Znach=0.0; return false; }
      Znach=u/v; return true;
     case  5:
      if(v==0.0){ Znach=0.0; return false; }
      Znach=(u-v*floor(u/v)); return true;
     case  6:
      if((F.B->R.C->Atr==CNST)&&(v=F.B->R.C->Val,(v==ceil(v)))){
       int p=(int)(v<0?u=1/u,-v:v);
       v=1; for(int i=0;i<p;i++) v*=u; Znach=v; return true;
      }
      if(u>0.0){ Znach=exp(v*log(u)); return true; }
      if(u==0.0){ Znach=0.0; return true; }
      Znach=0.0; return false;
   }}
   else{ Znach=0.0; return false; }
  case IFOP :
   if(!Valc(F.L->U,u)||!Valc(F.L->V,v)){ Znach=0.0; return false; }
   else{
    a=false;
    if(F.L->Oper>6){
     if(!Valc(F.L->UT,ut)){ Znach=0.0; return false; }
    }
    switch(F.L->Oper){
     case  1: if(u< v) a=true; break;
     case  2: if(u> v) a=true; break;
     case  3: if(u<=v) a=true; break;
     case  4: if(u>=v) a=true; break;
     case  5: if(u!=v) a=true; break;
     case  6: if(u==v) a=true; break;
     case  7:
      if(u<v&&ut<0&&(F.L->fu==false)&&(F.L->fv==false)){
       a=true; F.L->fu=true; F.L->fv=true; break;
      }
      break;
     case  8:
      if(u>v&&ut>0&&(F.L->fu==false)&&(F.L->fv==false)){
       a=true; F.L->fu=true; F.L->fv=true; break;
      }
      break;
    }
    if(a){
     if(Valc(F.L->FT,Znach)) return true; else{ Znach=0.0; return false; }
    }
    else if(F.L->FF.C!=Pust) {
     if(Valc(F.L->FF,Znach)) return true; else{ Znach=0.0; return false; }
    }
    else return true;
   }
  case SUMM:
   if(Valc(F.S->b,u)){
    n=(int)u; pp=F.S->i->P; Znach=0.0; F.S->i->P=0;
    for(int i=0; i<n; i++){
     F.S->i->Val=i; if(Valc(F.S->F,v)) Znach+=v; else{ Znach=0.0; return false; }
    }
    F.S->i->P=pp;
    return true;
   }
   else{ Znach=0.0; return false; }
  case INTG:
   if(Valc(F.G->a,u)&&Valc(F.G->b,v)){
    double x=u,y=0;
    pp=F.G->x->P; F.G->x->P=0; VArgp=VArg; FIntgp=FIntg;
    VArg=F.G->x; FIntg=F.G->f;
    if((x<v)&&!RUNGE_ORIGINAL(x,y,FuncIntg,Eps,Eta,v,Znach,VArg->Name)){
     Znach=0.0; F.S->i->P=pp; VArg=VArgp; FIntg=FIntgp;
     return false;
    }
    F.G->x->P=pp; VArg=VArgp; FIntg=FIntgp;
    return true;
   }
   else{ Znach=0.0; return false; }
  case IPLL:
   if(Valc(F.I->F,R)){
    switch(F.I->Type){
     case  1: Znach=Ipl(F.I,R); return true;
     case  2: Znach=(F.I->A->A)[(int)R]; return true;
   }}
   else{ Znach=0.0; return false; }
  case SPLN:
   if(Valc(F.P->F,R)){
    switch(F.P->Type){
     case  1: Znach=Spl(F.P,R); return true;
     default: Znach=0.0; return true;
   }}
   else{ Znach=0.0; return false; }
  case RUUT:
   pp=F.R->x->P; F.R->x->P=0;
   if((F.R->a!=Pust)&&(F.R->b!=Pust)){
    double a,b,fa,fb,fc;
    if(Valc(F.R->a,a)&&Valc(F.R->b,b)){
     F.R->x->Val=b; ClearVar();
     if(Valc(F.R->f,fb)){
      F.R->x->Val=a; ClearVar();
      if(Valc(F.R->f,fa)){
	   if(fa*fb>0){ String S;
        FormToStr(F.R->f,&S,false);
        swprintf(Inf,L"Неправильно указан интервал [%g,%g] переменной \"%s\"\n"
         L"поиска корня функции \"%s\".\n"
         L"На концах интервала функция должна принимать значения разных знаков!\n"
         L"Здесь же значения = [%g, %g]",a,b,F.R->x->Name,S.c_str(),fa,fb);
        Application->MessageBox(Inf,L"Ошибка!",MB_OK);
        F.R->Val=-0.5*(a+b); Znach=F.R->Val; return false;
       }
       for(int i=0;(fabs(fb-fa)>Eps)&&(i<1000);i++){
        double c=0.5*(b+a);
        F.R->x->Val=c; ClearVar();
        if(Valc(F.R->f,fc)){
         if(fa*fc<0){ b=c; fb=fc; }
         else{ a=c; fa=fc; }
        }
        else{ Znach=0; return false; }
      }}
      else{ Znach=0; return false; }
     }
     else{ Znach=0; return false; }
    }
    else{ Znach=0; return false; }
    Znach=0.5*(a+b);
   }
   F.R->x->P=pp;
   return true;
  case REST:
   if((F.O->a!=Pust)&&(F.O->b!=Pust)){ double a,b;
    ClearVar();
    if(Valc(F.O->a,a)&&Valc(F.O->b,b)){
     while(a>b){ a-=b; }
     Znach=a;
     return true;
    }
    Znach=0; return false;
   }
   else{ Znach=0; return false; }
  case ARRI:
   i=Val(F.a->FInd);
   if(!i) return true;
   if(F.a->A->F){
    return Valc(F.a->A->F[i-1],Znach);
   }
  default: Znach=0; return false;
 }
// return false;
}

double ValSumm(Form F,Vary *I){
 double D=0,u,v,x,y,a,b,c,fb,fa,fc; wchar_t *s,NameV[256],*inf; Vary *V; bool Rez;
 int i,i0,in,pp,Ind,n; Vary *VArgp; Form FIntgp; bool Mess;
 switch(F.C->Atr){
  case CNST : return F.C->Val;
  case VARY :
   if(F.V==I){
    if(F.V->P<=0) D=F.V->Val;
    else{
     F.V->P=-1; D=F.V->Val=ValSumm(F.V->Znach,I);
     if(NoCalc&&!Mess){
      Mess=true;
      swprintf(Inf,L"Нельзя вычислить значение переменной \"%s\"\n"
       L"См. закладку \"Ошибки\" в редакторе КиДиМ",F.V->Name);
      Application->MessageBox(Inf,L"Ошибка!",MB_OK);
      inf+=swprintf(inf=Inf,L"Нельзя вычислить значение переменной: \"%s\"",F.V->Name);
      Error(0,0,false);
      if(SLE){ Magazine *MagV=NULL,*m; Vary *V;
//     PrintForm(true,ERRFILE,F.V->Name,F.V->Znach);
       serr=FormToStringList(F.V->Znach,F.V->Name,Serr,serr,SLE,false,&SF);
//       swprintf(Inf,L"\nЗначения входящих в нее переменных:\n"); Fputs(Inf,ERRFILE);
       SLE->Add(Serr); SLE->Add(L"Значения входящих в нее переменных:");
//     fwprintf(ERRFILE,L"%s=%g;\n",L->Time->Name,L->Time->Val);
       swprintf(Serr,L"%s=%g;",L->Time->Name,L->Time->Val); SLE->Add(Serr);
       UtilVars(F.V->Znach,&MagV,true);
       for(m=MagV;m;m=m->Sled)
//     fwprintf(ERRFILE,L"%s;\n",m->S);
       SLE->Add(m->S);
//     fclose(ERRFILE);
       SLE->SaveToFile(ErrFile);
    }}}
    return D;
   }
   if((s=wcschr(F.V->Name,SimbInd))&&(wcsstr(s+1,I->Name)==s+1)){
    *s='\0';
    if(FindVary(s+1,&V)){
     if(V->P<=0) Ind=Round(V->Val);
     else{
      Ind=Val(V->Znach);
      if(NoCalc&&!Mess){
       Mess=true;
       swprintf(Inf,L"Нельзя вычислить значение переменной \"%s\"\n"
        L"См. закладку \"Ошибки\" в редакторе КиДиМ",V->Name);
       Application->MessageBox(Inf,L"Ошибка!",MB_OK);
       inf+=swprintf(inf=Inf,L"Нельзя вычислить значение переменной: \"%s\"",V->Name);
       Error(0,0,false);
       if(SLE){ Magazine *MagV=NULL,*m; Vary *V;
//      PrintForm(true,ERRFILE,V->Name,F.V->Znach);
	serr=FormToStringList(F.V->Znach,V->Name,Serr,serr,SLE,false,&SF);
//      swprintf(Inf,L"\nЗначения входящих в нее переменных:\n"); Fputs(Inf,ERRFILE);
//      fwprintf(ERRFILE,L"%s=%g;\n",L->Time->Name,L->Time->Val);
	swprintf(Serr,L"%s=%g;",L->Time->Name,L->Time->Val);
	SLE->Add(Serr);	Serr[0]='\0'; serr=Serr;
	UtilVars(V->Znach,&MagV,true);
        for(m=MagV;m;m=m->Sled)
//      fwprintf(ERRFILE,L"%s;\n",m->S);
        SLE->Add(m->S);
//      fclose(ERRFILE);
	SLE->SaveToFile(ErrFile);
       }
       return 0;
     }}
     swprintf(NameV,L"%s%d",F.V->Name,Ind); *s=SimbInd;
     if(FindVary(NameV,&V)){
      if(V->P<=0) D=V->Val;
      else{
       V->P=-1; D=V->Val=ValSumm(V->Znach,I);
       if(NoCalc&&!Mess){
        Mess=true;
        swprintf(Inf,L"Нельзя вычислить значение переменной \"%s\"\n"
         L"См. закладку \"Ошибки\" в редакторе КиДиМ",V->Name);
        Application->MessageBox(Inf,L"Ошибка!",MB_OK);
        inf+=swprintf(inf=Inf,L"Нельзя вычислить значение переменной: \"%s\"",V->Name);
        Error(0,0,false);
		if(SLE){ Magazine *MagV=NULL,*m; Vary *V;
//       PrintForm(true,ERRFILE,V->Name,V->Znach);
	 serr=FormToStringList(V->Znach,V->Name,Serr,serr,SLE,false,&SF);
//	 swprintf(Inf,L"\nЗначения входящих в нее переменных:\n"); Fputs(Inf,ERRFILE);
         SLE->Add(Serr); SLE->Add(L"Значения входящих в нее переменных:"); 
//         wcscpy(Inf,L->Time->Name); Fputs(Inf,ERRFILE);
//       fwprintf(ERRFILE,L"%s=%g;\n",L->Time->Name,L->Time->Val);
	 swprintf(Serr,L"%s=%g;",L->Time->Name,L->Time->Val); SLE->Add(Serr);
	 UtilVars(V->Znach,&MagV,true);
         for(m=MagV;m;m=m->Sled)
//       fwprintf(ERRFILE,L"%s;\n",m->S);
         SLE->Add(m->S);
//       fclose(ERRFILE);
	 SLE->SaveToFile(ErrFile);
      }}}
      return D;
     }
     else return 0;
    }
    else{ *s=SimbInd; return 0; }
   }
   if(F.V->P<=0) D=F.V->Val;
   else{
    F.V->P=-1; D=F.V->Val=ValSumm(F.V->Znach,I);
    if(NoCalc&&!Mess){
     Mess=true;
     swprintf(Inf,L"Нельзя вычислить значение переменной \"%s\"\n"
      L"См. закладку \"Ошибки\" в редакторе КиДиМ",F.V->Name);
     Application->MessageBox(Inf,L"Ошибка!",MB_OK);
     swprintf(Inf,L"Нельзя вычислить значение переменной: \"%s\"",F.V->Name);
     Error(0,0,false);
     if(SLE){ Magazine *MagV=NULL,*m; Vary *V;
//    PrintForm(true,ERRFILE,F.V->Name,F.V->Znach);
      serr=FormToStringList(V->Znach,F.V->Name,Serr,serr,SLE,false,&SF); SLD->Add(Serr);
//    swprintf(Inf,L"\nЗначения входящих в нее переменных:\n"); Fputs(Inf,ERRFILE);
      SLD->Add(L"Значения входящих в нее переменных:"); 
//    fwprintf(ERRFILE,L"%s=%g;\n",L->Time->Name,L->Time->Val);
      swprintf(Serr,L"%s=%g;",L->Time->Name,L->Time->Val); SLE->Add(Serr);
      UtilVars(F.V->Znach,&MagV,true);
      for(m=MagV;m;m=m->Sled)
//    fwprintf(ERRFILE,L"%s;\n",m->S);
      SLE->Add(m->S);
//    fclose(ERRFILE);
      SLE->SaveToFile(ErrFile);
   }}}
   return D;
  case UNOP:
   D=ValSumm(F.U->Arg,I);
   switch(F.U->Func){
    case  1: D=-D;        break;
    case  2: D=sqrt(D);   break;
    case  3: D=sin(D);    break;
    case  4: D=cos(D);    break;
    case  5: D=tan(D);    break;
    case  6: D=asin(D);   break;
    case  7: D=atan(D);   break;
    case  8: D=exp(D);    break;
    case  9: D=log(D);    break;
    case 10: D=sinh(D);   break;
    case 11: D=cosh(D);   break;
    case 12:
     if(D+sqrt(1.0+D*D)<=0.0){ D=0.0; NoCalc=true; }
     else D=log(D+sqrt(1.0+D*D));
     break;
    case 13: D=fabs(D);   break;
    case 14: D=D*rand()/RAND_MAX; break;
    case 15: D=ceil(D);   break;
    case 16: D=floor(D);  break;
    case 17:
     if(fabs(D)>1.0){ D=0.0; NoCalc=true; }
     else D=acos(D);
     break;
    case 18: //!
     n=(int)Round(D);
     if(!n) D=1.0;
     else if(n>0){ D=1.0; for(int i=2;i<=n;i++) D*=i; }
     else{ D=0.0; NoCalc=true; }
   }
   return D;
  case BNOP:
   u=ValSumm(F.B->L,I);
   v=ValSumm(F.B->R,I);
   switch(F.B->Oper){
    case  1: D = u + v; break;
    case  2: D = u - v; break;
    case  3: D = u * v; break;
    case  4:
     if(v==0.0){ D=0.0; NoCalc=true; }
     else D = u/v;
     break;
    case  5:
     if(v==0.0){ D=0.0; NoCalc=true; }
     else D = (u-v*floor(u/v));
     break;
    case  6:
     if(v==ceil(v)){
      int p=(int)(v < 0 ? u=1/u,-v : v);
      v=1; for(int i=0;i<p;i++) v*=u; D=v;
     }
     else if(u>0.0)  D=exp(v*log(u));
     else if(u==0.0) D=0.0;
     else{ D=0.0; NoCalc=true; }
   }
   return D;
  case IFOP :
   u=ValSumm(F.L->U,I); v=ValSumm(F.L->V,I); Rez=false;
   switch(F.L->Oper){
    case  1: if(u< v) Rez=true; break;
    case  2: if(u> v) Rez=true; break;
    case  3: if(u<=v) Rez=true; break;
    case  4: if(u>=v) Rez=true; break;
    case  5: if(u!=v) Rez=true;
   }
   D = Rez ? ValSumm(F.L->FT,I) : ValSumm(F.L->FF,I);
   return D;
  case IPLL: D=ValSumm(F.I->F,I);
   switch(F.I->Type){
    case  1: D=Ipl(F.I,D); break;
    case  2: D=(F.I->A->A)[(int)Round(D)]; break;
   }
   return D;
  case SPLN: D=ValSumm(F.P->F,I);
   switch(F.P->Type){
    case  1: D=Spl(F.P,D); break;
    default: D=0.0;
   }
   return D;
  case SUMM:
   i0=(int)ValSumm(F.S->a,I); in=(int)ValSumm(F.S->b,I);
   pp=F.S->i->P; D=0.0; F.S->i->P=0;
   for(int i=i0; i<=in; i++){
    F.S->i->Val=i; ClearVar();
    D+=ValSumm(F.S->F,F.S->i);
   }
   F.S->i->P=pp;
   return D;
  case INTG:
   pp=F.G->x->P; F.G->x->P=0;
   if((F.G->a!=Pust)&&(F.G->b!=Pust)&&(F.G->Rez!=Pust)||(F.G->F!=Pust)||
      IsTaken(F.G))
    D=ValSumm(F.G->Rez!=Pust?F.G->Rez:F.G->F,I);//интеграл взялся аналитически!
   else if(F.G->P<1)//интеграл уже посчитан
    D=F.G->Val;
   else{
    y=0; D=0.0; a=Val(F.G->a); b=Val(F.G->b);
    if(a<b){
//     if(h_RK==-1.0||h_RK>(b-a))
      h_RK=b-a;
     VArgp=VArg; FIntgp=FIntg;
     VArg=F.G->x; FIntg=F.G->f;
     RUNGE_ORIGINAL(/*x=*/a,y,FuncIntg,Eps,Eta,b,D,VArg->Name);
     VArg=VArgp; FIntg=FIntgp;
   }}
   F.G->x->P=pp;
   return D;
  case RUUT:
   pp=F.R->x->P; F.R->x->P=0;
   if((F.R->a!=Pust)&&(F.R->b!=Pust)){
    y=0; D=0.0; ClearVar();
    a=Val(F.R->a); b=Val(F.R->b);
    F.R->x->Val=b; ClearVar(); fb=Val(F.R->f);
    F.R->x->Val=a; ClearVar(); fa=Val(F.R->f);
	if(fa*fb>0){ String S;
	 FormToStr(F.R->f,&S,false);
     swprintf(Inf,L"Неправильно указан интервал [%g, %g] переменной \"%s\"\n"
      L"поиска корня функции \"%s\".\n"
      L"На концах интервала функция должна принимать значения разных знаков!\n"
      L"Здесь же значения = [%g, %g]",a,b,F.R->x->Name,S.c_str(),fa,fb);
     Application->MessageBox(Inf,L"Ошибка!",MB_OK);
     D=F.R->Val=-0.5*(a+b); return D;
    }
    for(int i=0;(fabs(fb-fa)>Eps)&&(i<1000);i++){
     c=0.5*(b+a); F.R->x->Val=c;
     ClearVar(); fc=Val(F.R->f);
     if(fa*fc<0){ b=c; fb=fc; }
     else{ a=c; fa=fc; }
   }}
   F.R->x->P=pp; D=0.5*(a+b);
   return D;
  case REST:
   if((F.O->a!=Pust)&&(F.O->b!=Pust)){
    ClearVar(); a=Val(F.O->a); b=Val(F.O->b);
    while(a>b){ a-=b; } D=a;
   }
   return D;
  case ARRI:
   ClearVar();
   i=ValSumm(F.a->FInd,I);
   if(!i) return F.a->A->N/2;
   if(F.a->A->F){
    return ValSumm(F.a->A->F[i-1],I);
   }
   else if(F.a->A->A)
    return F.a->A->A[i-1];
  default: return 0;
}}

Magazine *PrintVars;
#define PRINTD //FF=_wfopen(L"file",L"a"); fwprintf(FF,L"\n%g",D); fclose(FF)
double Val(Form F){
 bool Rez; double R,r,D,u,v,ut,x,y,a,b,fa,fb,c,fc; int n,pp,i,i0,in; wchar_t *inf;
 /*static bool Mess=false,Beg=false*/; FILE *FPV; //static int Kb=0;
 Vary *VArgp; Form FIntgp;
//FILE *FF=_wfopen(L"file",L"a"); PrintForm(true,FF,L"вычисляем F",F); fputwc('\n',FF); fclose(FF);
 switch(F.C->Atr){
  case CNST : return F.C->Val;
  case VARY :
   if(F.V->P<=0) D=F.V->Val;
   else{
    D=F.V->Val=Val(F.V->Znach);
    F.V->P=-1;
   }
if(PRINTVARS){
 swprintf(Inf,L"%s=%g;",F.V->Name,D);
 TakeMagazine(&PrintVars,Inf);
}
   return D;
  case UNOP :
   if(F.U->P){ D=F.U->Val; PRINTD; }
   else{
    r=Val(F.U->Arg); F.U->P=1;
    switch(F.U->Func){
     case  1: D=r==0.0?0.0:-r; break;
     case  2:
      if(r<0){ D=0.0; NoCalc=true; } else D=sqrt(r); break;
     case  3: D=sin(r); break;
     case  4: D=cos(r); break;
     case  5: D=(r==M_PI_2)||(r==-M_PI_2)?MAXDOUBLE:tan(r); break;
     case  6: if(fabs(r)>1.0){ D=0.0; NoCalc=true; } else D=asin(r); break;
     case  7: D=atan(r); break;
     case  8: D=exp(r);  break;
     case  9: if(r<=0.0){ D=0.0; NoCalc=true; } else D=log(r); break;
     case 10: D=sinh(r); break;
     case 11: D=cosh(r); break;
     case 12:
      if(r+sqrt(1.0+r*r)<=0.0){ D=0.0; NoCalc=true; }
      else D=log(r+sqrt(1.0+r*r));
       break;
     case 13: D=fabs(r); break;
     case 14: D=r*rand()/RAND_MAX; break;
     case 15: D=ceil(r); break;
     case 16: D=floor(r); break;
     case 17: if(fabs(r)>1.0){ D=0.0; NoCalc=true; } else D=acos(r); break;
     case 18: //!
      n=(int)Round(r);
      if(!n) D=1.0;
      else if(n>0){ D=1.0; for(int i=2;i<=n;i++) D*=i; }
      else{ D=0.0; NoCalc=true; }
      break;
   }}
   PRINTD; return F.U->Val=D;
  case BNOP:
   u=Val(F.B->L);
   v=Val(F.B->R);
   switch(F.B->Oper){
    case  1: D = u + v; break;
    case  2: D = u - v; break;
    case  3: D = u * v; break;
    case  4:
     if(v==0.0){ D=0.0; NoCalc=true; } else D = u/v; break;
    case  5:
     if(v==0.0){ D=0.0; NoCalc=true; } else D = (u-v*floor(u/v)); break;
    case  6:
     if(v==ceil(v)){
      int p=(int)(v < 0 ? u=1/u,-v : v);
      v=1; for(int i=0;i<p;i++) v*=u; D=v;
     }
     else if(u>0.0){ D=exp(v*log(u)); }
     else if(u==0.0){ D=0.0; }
     else{ D=0.0; NoCalc=true; }
     break;
   }
   PRINTD; return D;
  case IFOP:
   u=Val(F.L->U); v=Val(F.L->V);
   if(F.L->Oper>6) // << или >>
    ut=Val(F.L->UT); //посчитаем значение скорости левого операнда
   Rez=false;
   switch(F.L->Oper){
    case  1: if(u< v) Rez=true; break;
    case  2: if(u> v) Rez=true; break;
    case  3: if(u<=v) Rez=true; break;
    case  4: if(u>=v) Rez=true; break;
    case  5: if(u!=v) Rez=true; break;
    case  6: if(u==v) Rez=true; break;
    case  7:
     if(u<v&&ut<0){
      if((F.L->fu==false)&&(F.L->fv==false)&&(L->Time->Val>F.L->tf)){
//1-й раз выполнено условие
       Rez=F.L->fu=F.L->fv=true; F.L->tt=L->Time->Val;
     }}
     else if(L->Time->Val>F.L->tt){//не выполнено условие
      F.L->fu=F.L->fv=false; F.L->tf=L->Time->Val;
     }
     break;
    case  8:
     if(u>v&&ut>0){
      if((F.L->fu==false)&&(F.L->fv==false)&&(L->Time->Val>F.L->tf)){
//1-й раз выполнено условие
       Rez=F.L->fu=F.L->fv=true; F.L->tt=L->Time->Val;
     }}
     else if(L->Time->Val>F.L->tt){//не выполнено условие
      F.L->fu=F.L->fv=false; F.L->tf=L->Time->Val;
     }
     break;
   }
   D = Rez ? F.L->Old=Val(F.L->FT) :
             F.L->FF.C!=Pust ?
                               Val(F.L->FF) :
                               F.L->Old;
   PRINTD; return D;
  case IPLL: R=Val(F.I->F);
   switch(F.I->Type){
    case  1: D=Ipl(F.I,R); break;
    case  2: D=(F.I->A->A)[(int)R];
   }
   PRINTD; return D;
  case SPLN:
   R=Val(F.P->F);
   switch(F.P->Type){
    case  1:
     D=Spl(F.P,R);
     break;
    default: D=0.0;
   }
   PRINTD; return D;
  case SUMM:
   i0=(int)Val(F.S->a); in=(int)Val(F.S->b); n=in-i0+1;
   pp=F.S->i->P; D=0.0; F.S->i->P=0;
   for(int i=i0; i<=in; i++){ double d;
    F.S->i->Val=i; //ClearVar();
//    d=Value(F.S->F);
//    D+=d;
    d=ValSumm(F.S->F,F.S->i);
    D+=d;
   }
   F.S->i->P=pp;
   PRINTD; return D;
  case INTG:
   pp=F.G->x->P; F.G->x->P=0;
   if(F.G->Rez!=Pust)//интеграл взялся аналитически!
    D=Value(F.G->Rez);
   else if(F.G->P<1)//интеграл уже посчитан
    D=F.G->Val;
    else{ double h_old=h_RK;//интеграл надо считать
     y=0; ClearVar(); b=Val(F.G->b); D=0.0;
     if(F.G->x==L->Time&&F.G->b.V==L->Time) a=F.G->B;
     else a=Val(F.G->a);
     if(a<b){
//      if(h_RK==-1.0||h_RK>(b-a))
       h_RK=b-a;
      VArgp=VArg; FIntgp=FIntg;
      VArg=F.G->x; FIntg=F.G->f;
      RUNGE_ORIGINAL(/*x=*/a,y,FuncIntg,Eps,Eta,b,D,VArg->Name);
      VArg=VArgp; FIntg=FIntgp; F.G->B=b; F.G->Val=D; F.G->P=-1;
      h_RK=h_old;
    }}
   F.G->x->P=pp;
   PRINTD; return D;
  case RUUT:
   pp=F.R->x->P; F.R->x->P=0;
   if((F.R->a!=Pust)&&(F.R->b!=Pust)){
    D=0.0; ClearVar();
    a=Val(F.R->a); b=Val(F.R->b);
    F.R->x->Val=b; ClearVar(); fb=Val(F.R->f);
    F.R->x->Val=a; ClearVar(); fa=Val(F.R->f);
	if(fa*fb>0){ String S;
	 FormToStr(F.R->f,&S,false);
     swprintf(Inf,L"Неправильно указан интервал [%g,%g] переменной \"%s\"\n"
      L"поиска корня функции \"%s\".\n"
      L"На концах интервала функция должна принимать значения разных знаков!\n"
      L"Здесь же значения = [%g, %g]",a,b,F.R->x->Name,S.c_str(),fa,fb);
     Application->MessageBox(Inf,L"Ошибка!",MB_OK);
     F.R->Val=-0.5*(a+b); return F.R->Val;
    }
    for(i=0;(fabs(fb-fa)>Eps)&&(i<1000);i++){
     c=0.5*(b+a); F.R->x->Val=c;
     ClearVar(); fc=Val(F.R->f);
     if(fa*fc<0){ b=c; fb=fc; }
     else{ a=c; fa=fc; }
   }}
   F.R->x->P=pp; D=0.5*(a+b);
   PRINTD; return D;
  case REST:
   if((F.O->a!=Pust)&&(F.O->b!=Pust)){
    a=Val(F.O->a); b=Val(F.O->b);
    while(a>b){ a-=b; } D=a;
   }
   return D;
  case ARRI:
   i=Val(F.a->FInd);
   if(!i)
    return F.a->A->N/2;
   if(F.a->A->F){
    return Val(F.a->A->F[i-1]);
   }
   else return F.a->A->A[i-1];
 }
 return 0.0;
}

double Value(Form F){ double Rez=0.0;
 ClearVar();
 switch(F.C->Atr){
  case CNST :               return F.C->Val;
  case VARY : if(F.V->P<=0) return F.V->Val; F=F.V->Znach;
  default   : Rez=Val(F);
   if(NoCalc){
	wchar_t *inf,Buf[1024]; String SF; //FILE *FF;
//    if(FF=_wfopen(L"temp.txt",L"w")){
	 inf+=swprintf(inf=Inf,L"Нельзя вычислить значение формулы\n");
	 FormToStr(F,&SF,false);
	 inf+=swprintf(inf,L"%s",SF.w_str());
//     PrintForm(true,FF,NULL,F);
//     fclose(FF);
//     if(FF=_wfopen(L"temp.txt",L"r")){
//      while(Fgets(Buf,1023,FF)) inf+=swprintf(inf,Buf);
//      fclose(FF);
//    }}
	inf+=swprintf(inf,L"См. закладку \"Ошибки\" в редакторе КиДиМ");
	Application->MessageBox(Inf,L"Ошибка!",MB_OK);
	inf+=swprintf(inf=Inf,L"Нельзя вычислить значение переменной: \"%s\"",F.V->Name);
	Error(0,0,false);
	if(SLE){ Magazine *MagV=NULL,*m;
//       PrintForm(true,ERRFILE,NULL,F.V->Znach);может было закомментировано - если да то строку ниже убрать
	 serr=FormToStringList(F.V->Znach,NULL,Serr,serr,SLE,false,&SF); SLE->Add(Serr);
//	 swprintf(Inf,L"\nЗначения входящих в нее переменных:\n"); Fputs(Inf,ERRFILE);
         SLE->Add("Значения входящих в нее переменных:");
//fwprintf(ERRFILE,L"%s=%g;\n",L->Time->Name,L->Time->Val);
	 swprintf(Serr,L"%s=%g;",L->Time->Name,L->Time->Val); SLE->Add(Serr);
	 UtilVars(F,&MagV,true);
	 for(m=MagV;m;m=m->Sled)
//fwprintf(ERRFILE,L"%s;\n",m->S);
	  SLE->Add(m->S);
//       fclose(ERRFILE);
	 SLE->SaveToFile(ErrFile);
	 DelAllMagazine(&MagV);
   }}
  return Rez;
}}

bool Calcul(Form F,double &Znach){
 ClearVar(); return Valc(F,Znach);
}

struct TestV{ TestV *Sled; Vary *v; } *Rtv;

bool FindTestV(Vary *V){
 for(TestV *tv=Rtv;tv;tv=tv->Sled)
  if(tv->v==V) return true;
 return false;
}

bool IsCykl(Magazine *Mv,Form F,Magazine **Mrez){
 Magazine *m; Form FF; int i;
 switch (F.C->Atr) {
  case PUST:case CNST: return false;
  case VARY:
   if(FindMagazine(Mv,F.V->Name,&m)) return true;
   else{
    Magazine *Mmv=NULL,**PMmv=&Mmv,*mm; bool Rez;
    for(m=Mv;m;m=m->Sled){
     *PMmv=mm=(Magazine *)calloc(1,sizeof(Magazine)); PMmv=(Magazine **)mm;
     mm->S=wcscpy((wchar_t *)calloc(wcslen(m->S)+1,SzC),m->S);
    }
//    TakeMagazine(&Mmv,F.V->Name);
    if(Rez=IsCykl(Mmv,F.V->Znach,Mrez)){
     mm=(Magazine *)calloc(1,sizeof(Magazine));
     mm->Sled=*Mrez; *Mrez=mm;
//     swprintf(Inf,L"%s, строка %d",F.V->Name,F.V->Line);
//     mm->S=wcscpy((wchar_t *)calloc(wcslen(Inf)+1,SzC),Inf);
     mm->S=wcscpy((wchar_t *)calloc(wcslen(F.V->Name)+1,SzC),F.V->Name);
    }
    DelAllMagazine(&Mmv);
    return Rez;
   }
  case UNOP:
   return IsCykl(Mv,F.U->Arg,Mrez);
  case BNOP :
   return IsCykl(Mv,F.B->L,Mrez)||IsCykl(Mv,F.B->R,Mrez);
  case IFOP :
   return IsCykl(Mv,F.L->U,Mrez)||IsCykl(Mv,F.L->V,Mrez)||
    IsCykl(Mv,F.L->FT,Mrez)||IsCykl(Mv,F.L->FF,Mrez);
  case SUMM :
   FF.V=F.S->i;
   return IsCykl(Mv,FF,Mrez)||IsCykl(Mv,F.S->a,Mrez)||
    IsCykl(Mv,F.S->b,Mrez)||IsCykl(Mv,F.S->F,Mrez);
  case INTG :
   FF.V=F.G->x;
   return IsCykl(Mv,FF,Mrez)||IsCykl(Mv,F.G->a,Mrez)||
    IsCykl(Mv,F.G->b,Mrez)||IsCykl(Mv,F.G->f,Mrez);
  case IPLL :
   return IsCykl(Mv,F.I->F,Mrez);
  case SPLN:
   return IsCykl(Mv,F.P->F,Mrez);
  case RUUT:
   return IsCykl(Mv,F.R->x->Znach,Mrez)||IsCykl(Mv,F.R->f,Mrez)||
    IsCykl(Mv,F.R->a,Mrez)||IsCykl(Mv,F.R->b,Mrez);
  case REST:
   return IsCykl(Mv,F.O->a,Mrez)||IsCykl(Mv,F.O->b,Mrez);
  case ARRI:
   if(!IsCykl(Mv,F.a->FInd,Mrez)){
    i=Val(F.a->FInd);
    if(!i) return false;
    if(F.a->A->F)
     return IsCykl(Mv,F.a->A->F[i-1],Mrez);
    else return false;
   }
   return true;
 }
 return false;
}

Form SubLimit(Form f,Vary *x,Form L){//подставим предел интегрирования
 Form F,u,v,FF,FT;
 switch(f.C->Atr){
  case CNST: F.C=f.C; break;
  case VARY:
   if(f.V==x) F=L;
   else F=f;
   break;
  case UNOP:
   u=SubLimit(f.U->Arg,x,L);
   switch(f.U->Func){
    case  1: F=-u;        break;
    case  2: F=sqrt(u);   break;
    case  3: F=sin(u);    break;
    case  4: F=cos(u);    break;
    case  5: F=tg(u);     break;
    case  6: F=arcsin(u); break;
    case  7: F=arctg(u);  break;
    case  8: F=exp(u);    break;
    case  9: F=ln(u);     break;
    case 10: F=sh(u);     break;
    case 11: F=ch(u);     break;
    case 12: F=arsh(u);   break;
    case 13: F=abs(u);    break;
    case 14: F=rand(u);   break;
    case 15: F=ceil(u);   break;
    case 16: F=floor(u);  break;
    case 17: F=arccos(u); break;
    default: F.U=TakeUnop(f.U->Func,u);
   }
   break;
  case BNOP:
   u=SubLimit(f.B->L,x,L);
   v=SubLimit(f.B->R,x,L);
   switch(f.B->Oper){
    case 1: F=u+v; break;
    case 2: F=u-v; break;
    case 3: F=u*v; break;
    case 4: F=u/v; break;
    case 5: F=u-v*floor(u/v); break;
    case 6: F=u^v; break;
   }
   break;
  case IFOP:
   u=SubLimit(f.L->U,x,L);
   v=SubLimit(f.L->V,x,L);
   FT=SubLimit(f.L->FT,x,L);
   FF=SubLimit(f.L->FF,x,L);
   F.L=TakeIfop(f.L->Oper,u,v,FT,FF);
   break;
  case SUMM:
   F=f;
   break;
  case INTG:
   F=f;
   break;
  case IPLL:
   break;
  case SPLN:
   break;
 }
 return F;
}

int IsInForm(Form F,Vary *V){
 if(IsVary(F)&&(F.V==V)) return 1; //сомножитель - такая же переменная
 else if(IsMult(F)){
  if(IsVary(F.B->L)&&(F.B->L.V==V))//такая же переменная - в произведении слева
   return 2;
  if(IsVary(F.B->R)&&(F.B->R.V==V))//такая же переменная - в произведении справа
   return 3;
 }
 else if(IsPowr(F)){
  if(IsVary(F.B->L)&&(F.B->L.V==V))//такая же переменная - в основании степени
   return 4;
 }
 return 0;
}

Form OpenMult(Form L,Form R){
 Form F,a,b,c,d,A,B,FDva;
//if(OTLADKA){
// FILE *F=_wfopen(L"OTLADKA",L"a");
// PrintForm(true,F,L"L",L); fputwc('\n',F);
// PrintForm(true,F,L"R",R); fputwc('\n',F);
// fclose(F);
//}
 if((IsSumm(L)||IsDiff(L))&&!IsSumm(R)&&!IsDiff(R)){//слева сумма, а справа - нет
  a=L.B->L; b=IsSumm(L)?L.B->R:-L.B->R; c=R; d.C=Nul;
  a=OpenMult(a,c); b=OpenMult(b,c);
  F.B=TakeBnop(IsMins(b)?2:1,a,IsMins(b)?b.U->Arg:b);
 }
 else if((IsSumm(R)||IsDiff(R))&&!IsSumm(L)&&!IsDiff(L)){//справа сумма, а слева - нет
  a=L; b.C=Nul; c=R.B->L; d=IsSumm(R)?R.B->R:-R.B->R;
  a=OpenMult(a,c); b=OpenMult(a,d);
  F.B=TakeBnop(IsMins(b)?2:1,a,IsMins(b)?b.U->Arg:b);
 }
 else if((IsSumm(L)||IsDiff(L))&&IsSumm(R)||IsDiff(R)){
  a=L.B->L; b=IsSumm(L)?L.B->R:-L.B->R; c=R.B->L; d=IsSumm(R)?R.B->R:-R.B->R;
  A=OpenMult(a,c); B=OpenMult(a,d);
  A.B=TakeBnop(IsMins(B)?2:1,A,IsMins(B)?B.U->Arg:B);
  B=OpenMult(b,c);
  A.B=TakeBnop(IsMins(B)?2:1,A,IsMins(B)?B.U->Arg:B);
  B=OpenMult(b,d);
  F.B=TakeBnop(IsMins(B)?2:1,A,IsMins(B)?B.U->Arg:B);
 }
 else{ bool Minus=false; FDva.C=Dva;
  if(IsMins(L)){ Minus=!Minus; L=L.U->Arg; }
  if(IsMins(R)){ Minus=!Minus; R=R.U->Arg; }
  if(L==Nul||R==Nul) F.C=Nul;
  else{
   if(L==Odin) F=R;
   else if(R==Odin) F=L;
   else{
    if(IsVary(L)){
     switch(IsInForm(R,L.V)){
      case 0: F.B=TakeBnop(3,L,R); break;
      case 1: F.B=TakeBnop(6,L,FDva); break;
      case 2: F.B=TakeBnop(6,L,FDva); F.B=TakeBnop(3,F,R.B->R); break;
      case 3: F.B=TakeBnop(6,L,FDva); F.B=TakeBnop(3,F,R.B->L); break;
      case 4: F.B=TakeBnop(6,L,R.B->R+1);
    }}
    if(IsMult(L)){
     if(IsVary(L.B->L)){
      switch(IsInForm(R,L.B->L.V)){
       case 0:
        if(IsVary(L.B->R)){
         switch(IsInForm(R,L.B->R.V)){
          case 0: F.B=TakeBnop(3,L,R); break;
          case 1: F.B=TakeBnop(6,L.B->R,FDva); F.B=TakeBnop(3,L.B->L,F); break;
          case 2:
           F.B=TakeBnop(6,L.B->R,FDva); F.B=TakeBnop(3,L.B->L,F);
           F.B=TakeBnop(3,F,R.B->R);
           break;
          case 3:
           F.B=TakeBnop(6,L.B->R,FDva); F.B=TakeBnop(3,L.B->L,F);
           F.B=TakeBnop(3,F,R.B->L);
           break;
          case 4:
           F.B=TakeBnop(6,L.B->R,R.B->R+1); F.B=TakeBnop(3,L.B->L,F); break;
        }}
        break;
       case 1: F.B=TakeBnop(6,L,FDva); F.B=TakeBnop(3,F,L.B->R); break;
       case 2:
        F.B=TakeBnop(6,L.B->L,FDva); F.B=TakeBnop(3,F,L.B->R);
        F.B=TakeBnop(3,F,R.B->R);
        break;
       case 3:
        F.B=TakeBnop(6,L.B->L,FDva); F.B=TakeBnop(3,F,L.B->R);
        F.B=TakeBnop(3,F,R.B->L);
        break;
       case 4: F.B=TakeBnop(6,L.B->L,R.B->R+1); F.B=TakeBnop(3,F,L.B->R); break;
    }}}
    if(IsPowr(L)){
     if(IsVary(L.B->L)){
      switch(IsInForm(R,L.B->L.V)){
       case 0: F.B=TakeBnop(3,L,R); break;
       case 1: F.B=TakeBnop(6,L.B->L,L.B->R+1); break;
       case 2:
        F.B=TakeBnop(6,L.B->L,L.B->R+1); F.B=TakeBnop(3,F,R.B->R); break;
       case 3:
        F.B=TakeBnop(6,L.B->L,L.B->R+1); F.B=TakeBnop(3,F,R.B->L); break;
       case 4: F.B=TakeBnop(6,L.B->L,L.B->R+R.B->R); break;
  }}}}}
  if(Minus) F=-F;
 }
/*if(OTLADKA){
 FILE *FF=_wfopen(L"OTLADKA",L"a");
 PrintForm(true,FF,L"F",F); fputwc('\n',FF);
 fclose(FF);
}*/
 return F;
}

Form OpenPow(Form L,double D){
 int p=(int)D; Form F;
 if(IsSumm(L)||IsDiff(L)){
  Form FF,a=L.B->L,b=L.B->R,c,d; wchar_t S[16];
/*if(OTLADKA){
 FILE *F=_wfopen(L"OTLADKA",L"a");
 PrintForm(true,F,L"\nL",L); fputwc('\n',F);
 fclose(F);
} */
  switch(p){
   case 0: F.C=Odin; return F;
   case 1: F=L; return F;
   case 2:
    F.B=TakeBnop(1,OpenPow(a,2),OpenPow(b,2)); c.C=Dva;
    FF=OpenMult(c,OpenMult(a,b)); wcscpy(S,L"F^2");
    break;
   case 3:
    c.C=Tri;
    F.B=TakeBnop(1,OpenPow(a,3),OpenMult(c,OpenMult(a,OpenPow(b,2))));
    FF.B=TakeBnop(1,OpenMult(c,OpenMult(OpenPow(a,2),b)),OpenPow(b,3));
    wcscpy(S,L"F^3");
    break;
   case 4:
    c.C=TakeCnst(6.0);
    F=OpenPow(a,4)+OpenMult(c,OpenMult(OpenPow(a,2),OpenPow(b,2)))+OpenPow(b,4);
    c.C=TakeCnst(4.0);
    FF=OpenMult(c,OpenMult(OpenPow(a,3),b))+OpenMult(c,OpenMult(a,OpenPow(b,3)));
     wcscpy(S,L"F^4");
    break;
   case 5:
    c.C=TakeCnst(10.0); d.C=TakeCnst(5.0);
    F=OpenPow(a,5)+OpenMult(c,OpenMult(OpenPow(a,3),OpenPow(b,2)))+
      OpenMult(d,OpenMult(a,OpenPow(b,4)));
    FF=OpenMult(d,OpenMult(OpenPow(a,4),b))+
       OpenMult(c,OpenMult(OpenPow(a,2),OpenPow(b,3)))+OpenPow(b,5);
     wcscpy(S,L"F^5");
    break;
   default:
    if(L.B->Oper==1) F=OpenMult(a+b,OpenPow(a+b,p-1));
    else             F=OpenMult(a-b,OpenPow(a-b,p-1));
    swprintf(S,L"F^%d",p);
/*if(OTLADKA){
 FILE *FF=_wfopen(L"OTLADKA",L"a");
 PrintForm(true,FF,S,F); fputwc('\n',FF);
 fclose(FF);
} */
   return F;
  }
  if(IsMins(L)) F.B=TakeBnop(2,F,FF);
  else          F.B=TakeBnop(1,F,FF);
/*if(OTLADKA){
 FILE *FF=_wfopen(L"OTLADKA",L"a");
 PrintForm(true,FF,S,F); fputwc('\n',FF);
 fclose(FF);
}*/
 }
 else if(IsMult(L)){
  F.B=TakeBnop(3,(L.B->L^p),(L.B->R^p));
 }
 else if(IsDivd(L)){
  F.B=TakeBnop(4,(L.B->L^p),(L.B->R^p));
 }
 else{
  Form c; c.C=TakeCnst(p);
  if(L==Nul||L==Odin) F=L;
  else F.B=TakeBnop(6,L,c);
 }
 return F;
}

Form OpenBrack(Form F){
 Form u,v,FF,FT,L,R;
 switch(F.C->Atr){
  case CNST:
  case VARY: break;
  case UNOP:
   u=OpenBrack(F.U->Arg);
   switch(F.U->Func){
    case  1: F=-u;        break;
    case  2: F=sqrt(u);   break;
    case  3: F=sin(u);    break;
    case  4: F=cos(u);    break;
    case  5: F=tg(u);     break;
    case  6: F=arcsin(u); break;
    case  7: F=arctg(u);  break;
    case  8: F=exp(u);    break;
    case  9: F=ln(u);     break;
    case 10: F=sh(u);     break;
    case 11: F=ch(u);     break;
    case 12: F=arsh(u);   break;
    case 13: F=abs(u);    break;
    case 14: F=rand(u);   break;
    case 15: F=ceil(u);   break;
    case 16: F=floor(u);  break;
    case 17: F=arccos(u); break;
    default: F.U=TakeUnop(F.U->Func,u);
   }
   break;
  case BNOP:
   switch(F.B->Oper){
    case 1:
   u=OpenBrack(F.B->L);
/*if(OTLADKA){
 FILE *F=_wfopen(L"OTLADKA",L"a");
 PrintForm(true,F,L"u",u); fputwc('\n',F);
 fclose(F);
} */
   v=OpenBrack(F.B->R);
/*if(OTLADKA){
 FILE *F=_wfopen(L"OTLADKA",L"a");
 PrintForm(true,F,L"v",v); fputwc('\n',F);
 fclose(F);
} */
     F=u+v;
/*if(OTLADKA){
 FILE *FF=_wfopen(L"OTLADKA",L"a");
 PrintForm(true,FF,L"u+v",F); fputwc('\n',FF);
 fclose(FF);
} */
     break;
    case 2:
   u=OpenBrack(F.B->L);
/*if(OTLADKA){
 FILE *F=_wfopen(L"OTLADKA",L"a");
 PrintForm(true,F,L"u",u); fputwc('\n',F);
 fclose(F);
} */
   v=OpenBrack(F.B->R);
/*if(OTLADKA){
 FILE *F=_wfopen(L"OTLADKA",L"a");
 PrintForm(true,F,L"v",v); fputwc('\n',F);
 fclose(F);
} */
     F=u-v;
/*if(OTLADKA){
 FILE *FF=_wfopen(L"OTLADKA",L"a");
 PrintForm(true,FF,L"u-v",F); fputwc('\n',FF);
 fclose(FF);
} */
     break;
    case 3:
     L=F.B->L; R=F.B->R;
     if(IsSumm(L)||IsDiff(L)||IsSumm(R)||IsDiff(R)){
      F=OpenMult(L,R);
      F=OpenBrack(F);
     }
     else{
      u=OpenBrack(F.B->L);
/*if(OTLADKA){
 FILE *F=_wfopen(L"OTLADKA",L"a");
 PrintForm(true,F,L"u",u); fputwc('\n',F);
 fclose(F);
} */
      v=OpenBrack(F.B->R);
/*if(OTLADKA){
 FILE *F=_wfopen(L"OTLADKA",L"a");
 PrintForm(true,F,L"v",v); fputwc('\n',F);
 fclose(F);
} */
      F=u*v;
     }
     break;
    case 4:
   u=OpenBrack(F.B->L);
/*if(OTLADKA){
 FILE *F=_wfopen(L"OTLADKA",L"a");
 PrintForm(true,F,L"u",u); fputwc('\n',F);
 fclose(F);
} */
   v=OpenBrack(F.B->R);
     F=u/v;
/*if(OTLADKA){
 FILE *FF=_wfopen(L"OTLADKA",L"a");
 PrintForm(true,FF,L"u/v",F); fputwc('\n',FF);
 fclose(FF);
} */
     break;
    case 5: break;
    case 6:
     L=F.B->L; R=F.B->R;
     if((IsSumm(L)||IsDiff(L))&&(IsCnst(R)&&(R.C->Val==ceil(R.C->Val)))){
      F=OpenPow(L,R.C->Val);
/*if(OTLADKA){
 FILE *FF=_wfopen(L"OTLADKA",L"a");
 PrintForm(true,FF,L"L^p",F); fputwc('\n',FF);
 fclose(FF);
} */
      F=OpenBrack(F);
/*if(OTLADKA){
 FILE *FF=_wfopen(L"OTLADKA",L"a");
 PrintForm(true,FF,L"OpenBrack(F)",F); fputwc('\n',FF);
 fclose(FF);
} */
     }
     break;
   }
   break;
  case IFOP:
   u=OpenBrack(F.L->U);
   v=OpenBrack(F.L->V);
   FT=OpenBrack(F.L->FT);
   FF=OpenBrack(F.L->FF);
   F.L=TakeIfop(F.L->Oper,u,v,FT,FF);
   break;
  case SUMM:
   break;
  case INTG:
   break;
  case IPLL:
   break;
  case SPLN:
   break;
 }
 return F;
}

Form SubLimits(Intg *I){ Form Rez;//подставим пределы интегрирования
 if((I->a==Pust)&&(I->b==Pust)) Rez.C=Pust;
 else{
  Form Fa=SubLimit(I->F,I->x,I->a),
   Fb=SubLimit(I->F,I->x,I->b);
//  Fa=OpenBrack(Fa);
//  Fb=OpenBrack(Fb);
  Rez=Fb-Fa;
/*if(OTLADKA){
 FILE *F=_wfopen(L"OTLADKA",L"a");
 PrintForm(true,F,L"Rez",Rez); fputwc('\n',F);
 fclose(F);
} */
 }
 return Rez;
}

Form IsTaken(Form f,Vary *x){//берется ли аналитически интеграл от f,
//если да, то возвратится результат интегрирования (первообразная), иначе - Pust
 Form Rez,u,v,F; Rez.C=Pust;
 if(!VarInFormForIntg(true,f,x)){
//функция не зависит от переменной интегрирования - она выносится
  Rez.V=x; return f*Rez;
 }
 switch(f.C->Atr){
  case VARY:
   if(f.V==x)//интегрируется переменная интегрирования
    return Rez=0.5*(f^2);
   else//интегрируется переменная, зависящая от переменой интегрирования
    return Rez=IsTaken(f.V->Znach,x);
  case UNOP:
   if(f.U->Func==1){
    u=IsTaken(f.U->Arg,x);
    if(u==Pust) return u;
    return -u;
   }
   if(IsVary(f.U->Arg)&&(f.U->Arg.V==x)){
    switch(f.U->Func){
     case  2: Rez=(2/3)*(f.U->Arg^1.5);    break;
     case  3: Rez=-cos(f.U->Arg);          break;
     case  4: Rez= sin(f.U->Arg);          break;
     case  5: Rez=-ln(abs(cos(f.U->Arg))); break;
     case  8: Rez=f;                       break;
     case 10: Rez=ch(f.U->Arg);            break;
     case 11: Rez=sh(f.U->Arg);            break;
    }
    return Rez;
   }
   else{
    if(IsMult(f.U->Arg)&&
       (!VarInFormForIntg(true,f.U->Arg.B->L,x)&&(f.U->Arg.B->R==x)||
        !VarInFormForIntg(true,f.U->Arg.B->R,x)&&(f.U->Arg.B->L==x))||
       IsDivd(f.U->Arg)&&
        !VarInFormForIntg(true,f.U->Arg.B->R,x)&&(f.U->Arg.B->L==x)){
     Form L=(f.U->Arg.B->R==x)?f.U->Arg.B->L:f.U->Arg.B->R,
          R=(f.U->Arg.B->R==x)?f.U->Arg.B->R:f.U->Arg.B->L;
     if(IsDivd(f.U->Arg)) L=1/L;
     switch(f.U->Func){
      case  2: Rez= (2*sqrt(L)/3)*(R^1.5);        break;
      case  3: Rez=-(1/L)*cos(f.U->Arg);          break;
      case  4: Rez= (1/L)*sin(f.U->Arg);          break;
      case  5: Rez=-(1/L)*ln(abs(cos(f.U->Arg))); break;
      case  8: Rez= f/L;                          break;
      case 10: Rez= (1/L)*ch(f.U->Arg);           break;
      case 11: Rez= (1/L)*sh(f.U->Arg);           break;
     }
     return Rez;
   }}
   break;
  case BNOP:
   switch(f.B->Oper){
    case 1:
    case 2:
     u=IsTaken(f.B->L,x);
     v=IsTaken(f.B->R,x);
     if(u==Pust) return u;
     if(v==Pust) return v;
     return Rez=f.B->Oper==1?u+v:u-v;
    case 3:
     if((f.B->L!=Pust)&&!VarInFormForIntg(true,f.B->L,x)){//левый сомножитель константа
      v=IsTaken(f.B->R,x); //интегрируем правый
      Rez= v==Pust ? v : f.B->L*v;
     }
     else if((f.B->R!=Pust)&&!VarInFormForIntg(true,f.B->R,x)){//правый сомножитель константа
      u=IsTaken(f.B->L,x); //интегрируем левый
      Rez= u==Pust ? u : u*f.B->R;
     }
     return Rez;
    case 4:
     if(!VarInFormForIntg(true,f.B->L,x)){//числитель константа
      if(f.B->R.V==x)//знаменатель - переменная итегрирования
       Rez=f.B->L*ln(f.B->R);
      else{
       if(IsPowr(f.B->R)&&(IsSin(f.B->R.B->L)||IsCos(f.B->R.B->L))&&
          (f.B->R.B->R==Dva)&&
           (IsMult(f.B->R.B->L.U->Arg)&&
			!VarInFormForIntg(true,f.B->R.B->L.U->Arg.B->L,x)&&
			IsVary(f.B->R.B->L.U->Arg.B->R)&&(f.B->R.B->L.U->Arg.B->R.V==x))||
		   IsVary(f.B->R.B->L.U->Arg)&&(f.B->R.B->L.U->Arg.V==x)){
  //знаменатель - квадрат синуса a*x
		Form L,R;
		if(IsMult(f.B->R.B->L.U->Arg)){
		 L=f.B->R.B->L.U->Arg.B->L; R=f.B->R.B->L.U->Arg.B->R;
		}
		else{ L.C=Odin; R=f.B->R.B->L.U->Arg; }
		Rez=tg(f.B->R.B->L.U->Arg);
		Rez=IsSin(f.B->R.B->L)?-f.B->L/(L*Rez):(f.B->L/L)*Rez;
	 }}}
     else if(!VarInFormForIntg(true,f.B->R,x)){//знаменатель константа
      u=IsTaken(f.B->L,x); //интегрируем левый
      if(u==Pust) Rez=u;
      else Rez=u/f.B->R;
     }
     return Rez;
    case 5: F.C=Nul; return F;
    case 6:
     if(!VarInFormForIntg(true,f.B->R,x)){//показатель степени константа
      if(f.B->L.V==x){//основание - переменная итегрирования
       F=f.B->R+1;
       Rez=(1/F)*(f.B->L^F);
     }}
     else if(!VarInFormForIntg(true,f.B->L,x)){//основание - константа
      if(f.B->R.V==x)//показатель степени - переменная итегрирования
       Rez=f/ln(f.B->L);
     }
     return Rez;
   }
  case IFOP: return Rez;
  case SUMM: return Rez;
  case INTG:
   if(IsTaken(f.G)&&((F=IsTaken(f.G->Rez!=Pust?f.G->Rez:f.G->F,f.G->x))!=Pust)){
//    Rez=f.G->F; f=f.G->Rez;
    Rez=F; f=SubLimits(f.G);
   }
  case IPLL:
  case SPLN: return Rez;
 }
 return Rez;
}

bool IsTaken(Intg *I){//берется ли аналитически интеграл, если да, то в поле Eqv
                      //структуры Intg будет записана формула интегрирования, а
                      //в поле Rez - формула с учетом подстановки пределов
 if(I->F==Pust){
  Form F;
  if(!VarInFormForIntg(true,I->f,I->x)){
//подынтегральная функция не зависит от переменной интегрирования - она выносится
   F.V=I->x;
   I->F=I->f*F;
   I->Rez=SubLimits(I);
   return true;
  }
  switch(I->f.C->Atr){
   case VARY:
    if(I->f.V==I->x){//под интегралом стоит переменная интегрирования
     I->F=0.5*(I->f^2); I->Rez=SubLimits(I);
     return true;
    }
    else{//под интегралом стоит переменная, зависящая от переменой интегрирования
     F=IsTaken(I->f.V->Znach,I->x);
     if(F!=Pust){ I->F=F; I->Rez=SubLimits(I); }
     return (I->F!=Pust);
    }
   case INTG://под интегралом стоит интеграл
    if(IsTaken(I->f.G)&&((F=IsTaken(I->f.G->Rez!=Pust?I->f.G->Rez:I->f.G->F,I->x))!=Pust)){
     I->F=F; I->Rez=SubLimits(I);
     return (I->F!=Pust);
    }
    return false;
   default://пытаемся проинтегрировать подыинтегральную функцию
    F=IsTaken(I->f,I->x);
    if(F!=Pust){
     I->F=F; I->Rez=SubLimits(I);
    }
    return (I->F!=Pust);
  }
//  return false;
 }
 return true;
}

Form ReplaceLitlVary(Form F,Parm *P){
 Form Fu,U,W,X,Y;
 switch(F.C->Atr){
  case PUST:
  case CNST:
  case VARY: return F;
  case UNOP:
   Fu=ReplaceLitlVary(F.U->Arg,P);
   switch(F.U->Func){
    case  1: return -Fu;
    case  2: return sqrt(Fu);
    case  3: case 5: case 6: case 7: case 10: case 12:
     if(F.U->Arg.V->Atr==VARY){
      for(;P;P=P->Sled)
       if(!wcscmp(F.U->Arg.V->Name,P->Nam)) return F.U->Arg;
     }
     else return F;
    case  4: case 8: case 11:
     if(F.U->Arg.V->Atr==VARY){
      for(;P;P=P->Sled)
       if(!wcscmp(F.U->Arg.V->Name,P->Nam)) return F.C=Odin,F;
     }
     else return F;
    case  9://ln
     if(F.U->Arg.V->Atr==VARY){
      for(;P;P=P->Sled)
       if(!wcscmp(F.U->Arg.V->Name,P->Nam)) return F.C=TakeCnst(-MINDOUBLE),F;
     }
     else return F;
    case  17://arccos
     return F;
    default: return F;
   }
  case BNOP:
   U=ReplaceLitlVary(F.B->L,P); W=ReplaceLitlVary(F.B->R,P);
   if(U!=F.B->L||W!=F.B->R){ bool IsL=false,IsR=false;
    switch(F.B->Oper){
     case 1: return U+W;
     case 2: return U-W;
     case 3:
      if((U.V->Atr==VARY)&&(W.V->Atr==VARY)){
       for(;P;P=P->Sled){
        if(!wcscmp(U.V->Name,P->Nam)){ IsL=true; if(IsR) break; }
        if(!wcscmp(W.V->Name,P->Nam)){ IsR=true; if(IsL) break; }
      }}
      if(IsL&&IsR) return F.C=Nul,F;
      else return U*W;
     case 4: return U/W;
     case 5: return U-W*floor(U/W);
     case 6: return U^W;
   }}
   else return F;
  case IFOP:
   U=ReplaceLitlVary(F.L->U,P);  W=ReplaceLitlVary(F.L->V,P);
   X=ReplaceLitlVary(F.L->FT,P); Y=ReplaceLitlVary(F.L->FF,P);
   if(U!=F.L->U||W!=F.L->V||X!=F.L->FT||Y!=F.L->FF){
    return F.L=TakeIfop(F.L->Oper,U,W,X,Y),F;
   }
  default: return F;
}}

void PrepareIncompleteIf(Form F){
 switch(F.C->Atr){
  case CNST: return;
  case VARY: PrepareIncompleteIf(F.V->Znach); return;
  case UNOP: PrepareIncompleteIf(F.U->Arg);   return;
  case BNOP: PrepareIncompleteIf(F.B->L); PrepareIncompleteIf(F.B->R); return;
  case IFOP:
   PrepareIncompleteIf(F.L->U);  PrepareIncompleteIf(F.L->V);
   PrepareIncompleteIf(F.L->FT);
   if(F.L->FF.C!=Pust) PrepareIncompleteIf(F.L->FF);
   if(F.L->Oper>6){
    if(F.L->UT.C==Pust){ //нет формулы для скорости левого операнда
     if(F.L->U.C->Atr==VARY){ Cord *K; //если левый операнд - переменная
      if(FindCord(F.L->U.V->Name,&K)){ //найдем ее в обобщенных координатах
       swprintf(Inf,L"%s'%s",F.L->U.V->Name,L->Time->Name);
       F.L->UT.V=TakeVary(Inf); //и создадим переменную - скорость ее
      }
      else
       F.L->UT=DifByT(F.L->U.V); //построим формулу скорости левого операнда
   }}}
   return;
  case SUMM:
   PrepareIncompleteIf(F.S->a); PrepareIncompleteIf(F.S->b);
   PrepareIncompleteIf(F.S->F); return;
  case INTG:
   PrepareIncompleteIf(F.G->a); PrepareIncompleteIf(F.G->b);
   PrepareIncompleteIf(F.G->f); PrepareIncompleteIf(F.G->F);
   PrepareIncompleteIf(F.G->Rez); return;
  case SPLN: PrepareIncompleteIf(F.P->F); return;
  case RUUT:
   PrepareIncompleteIf(F.R->a); PrepareIncompleteIf(F.R->b);
   PrepareIncompleteIf(F.R->f); return;
  case REST:
   PrepareIncompleteIf(F.O->a); PrepareIncompleteIf(F.O->b); return;
}}

void PrepareIncompleteIf(void){
 for(Vary *V=L->V;V;V=V->Sled){
  PrepareIncompleteIf(V->Znach);
}}
//-----------------------------------------------------------------------------
double ToBaseUnit(Form Razm){
 List *LRazm;
 if(FindList(L"РАЗМЕРНОСТИ",&LRazm)){
  for(Vary *V=LRazm->V; V; V=V->Sled ){
   if (V->Znach.C==Pust){ V->P=0; V->Val=1.0; }
  }
  return Val(Razm);
 }
 return 1.0;
}
//------------------------------------------------------------------------------
double ValKRazm(Form F){   //Расчет переводного коэффициента размерности
 bool Rez; double D,u,v; Vary *V;
//FILE *FF=fopen(L"file","a"); PrintForm(true,FF,L"вычисляем F",F); fputc('\n',FF); fclose(FF);
 switch(F.C->Atr){
  case CNST : D=1.0; break;
  case VARY :
   if(FindVary(F.V->Name,&V))
    D=V->Znach.B->L.C->Val;
   else D=1.0;
   break;
  case BNOP:
   u=ValKRazm(F.B->L);
   v=ValKRazm(F.B->R);
   switch(F.B->Oper){
    case  3: D = u * v; break;
    case  4: D = u/v; break;
    case  6:
     D=pow(u,F.B->R.C->Val);
     break;
 }}
 return D;
}
//-----------------------------------------------------------------------------
void CalcKRazm(Vary*V){
 List *LOld=L;
 L=TakeList(L"РАЗМЕРНОСТИ");
 V->Krazm=ValKRazm(V->Razm);
 L=LOld;
}
//-----------------------------------------------------------------------------


