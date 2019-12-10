#include "CA.h"
#include <string.h>
#include <stdio.h>
#include <io.h>
#include <stdlib.h>
#include <alloc.h>
#include <math.h>

extern List *L,*LBeg; extern Vary *V,*Time,*Freq;
extern int Line,IsError,NzK,NPV,LpodvD,Lpodv,NYear; extern Cnst *Pust,*C,*Nul;
extern char Inf[],OutFile[],DgsFile[],HtmFile[],WORKDIR[],Autor[];
extern FILE *OUTFILE,*DGSFILE,*HTMFILE;  extern double h_RK;
//---------------------------------------------------------------------------
bool FindParm(Parm *R,char *S,Parm **P){
 for(*P=R;*P;*P=(*P)->Sled){
  if((*P)->Nam&&!strcmp((*P)->Nam,S)) return true;
 }
 return false;
}
//---------------------------------------------------------------------------
bool FindParmUpp(Parm *R,char *S,Parm **P){
 char UppS[256],UppName[256];
 strcpy(UppS,S); LowToUpp(UppS);
 for(*P=R;*P;*P=(*P)->Sled){
  if((*P)->Nam){
   strcpy(UppName,(*P)->Nam); LowToUpp(UppName);
   if(!strcmp(UppName,UppS)) return true;
 }}
 return false;
}
//---------------------------------------------------------------------------
bool FindParmUppStr(Parm *R,char *S,Parm **P){
 char UppS[256],UppName[256];
 strcpy(UppS,S); LowToUpp(UppS);
 for(*P=R;*P;*P=(*P)->Sled){
  if((*P)->Nam){
   strcpy(UppName,(*P)->Nam); LowToUpp(UppName);
   if(strstr(UppName,UppS)==UppName) return true;
 }}
 return false;
}
//---------------------------------------------------------------------------
bool FindCord(char *S,Cord **K){
 if(!(*K=L->K)) return false;
 for(;;*K=(*K)->Sled){
  if(!strcmp((*K)->V->Name,S)) return true;
  if(!((*K)->Sled)) return false;
}}
//---------------------------------------------------------------------------
Cord *FindCord(Cord *K,char *S){
 for(;K;K=K->Sled) if(!strcmp(K->V->Name,S)) return K; return NULL;
}
//---------------------------------------------------------------------------
Cord *TakeCord(Vary *V){
 Cord *K,*C=(Cord *)calloc(1,sizeof(Cord));
 if(!C){ IsError=true; return NULL; }
 C->V=V; K=L->K;
 if(!K) L->K=C;
 else { for(;K->Sled;K=K->Sled); K->Sled=C; }
 return C;
}
//---------------------------------------------------------------------------
Cord *TakeDelayK(Vary *V){
 Cord *K=L->Dq,*C=(Cord *)calloc(1,sizeof(Cord));
 if(!C){ IsError=true; return NULL; }
 C->V=V;
 if(!K) L->Dq=C;
 else { for(;K->Sled;K=K->Sled); K->Sled=C; }
 return C;
}
//---------------------------------------------------------------------------
Cord *TakeDelayV(Vary *V){
 Cord *K=L->Dv,*C=(Cord *)calloc(1,sizeof(Cord));
 if(!C){ IsError=true; return NULL; }
 C->V=V;
 if(!K) L->Dv=C;
 else { for(;K->Sled;K=K->Sled); K->Sled=C; }
 return C;
}
//---------------------------------------------------------------------------
bool FindZavC(char *S,Cord **Z){
 if(!(*Z=L->Z)) return false;
 for(;;*Z=(*Z)->Sled){
  if(!strcmp((*Z)->V->Name,S)) return true;
  if(!((*Z)->Sled)) return false;
}}
//---------------------------------------------------------------------------
Cord *TakeZavC(Vary *V){
 Cord *Z,*C=(Cord *)calloc(1,sizeof(Cord));
 if(!C){ IsError=true; return NULL; }
 C->V=V; Z=L->Z;
 if(!Z) L->Z=C;
 else { for(;Z->Sled;Z=Z->Sled); Z->Sled=C; }
 return C;
}
//---------------------------------------------------------------------------
bool FindPsevdo(char *S,Cord **W){
 if(!(*W=L->W)) return false;
 for(;;*W=(*W)->Sled){
  if(!strcmp((*W)->V->Name,S)) return true;
  if(!((*W)->Sled)) return false;
}}
//---------------------------------------------------------------------------
Cord *TakePsevdo(Vary *V){
 Cord *W,*C=(Cord *)calloc(1,sizeof(Cord));
 if(!C){ IsError=true; return NULL; }
 C->V=V; W=L->W;
 if(!W) L->W=C;
 else { for(;W->Sled;W=W->Sled); W->Sled=C; }
 return C;
}
//---------------------------------------------------------------------------
bool FindBody(char *S,Body **B){
 if(!(*B=L->B)) return false;
 for(;;*B=(*B)->Sled){
  if(!strcmp((*B)->Name,S)) return true;
  if(!((*B)->Sled)) return false;
}}
//---------------------------------------------------------------------------
Body *TakeBody(char *S,char *s){
 Body *B,*b; char Buf[256];
 if(s) *s++='\0'; CleanStr(S,Buf);
 if(FindBody(Buf,&B)) return B;
 if(b=(Body *)calloc(1,sizeof(Body))){
  b->Line=Line; b->Kx.C=Nul; b->Ky.C=Nul; b->Kz.C=Nul;
  b->Jxz.C=Nul; b->Jyz.C=Nul; b->Jxz.C=Nul;
  if(b->Name=(char *)malloc(strlen(Buf)+1)){
   strcpy(b->Name,Buf);
   if(s){
    CleanStr(s,Buf);
    b->BaseName=(char *)malloc(strlen(Buf)+1); strcpy(b->BaseName,Buf);
  }}
  return (B?B->Sled:L->B)=b;
 }
 IsError=true; return NULL;
}
//---------------------------------------------------------------------------
bool FindStringer(char *S,Stringer **B){
 if(!(*B=L->T)) return false;
 for(;;*B=(*B)->Sled){
  if(!strcmp((*B)->Name,S)) return true;
  if(!((*B)->Sled)) return false;
}}
//---------------------------------------------------------------------------
Stringer *TakeStringer(char *S){
 Stringer *T,*t; char Buf[256];
 CleanStr(S,Buf);
 if(FindStringer(Buf,&T)) return T;
 if(t=(Stringer *)calloc(1,sizeof(Stringer))){
  t->Line=Line; t->Cix.C=t->Ciy.C=t->Ck.C=t->Cx.C=t->Cy.C=t->Cz.C=Nul;
  t->FE.C=t->FG.C=t->FL.C=t->FF.C=t->FJx.C=t->FJy.C=t->FJz.C=Pust;
  t->dA.C=t->dB.C=t->dG.C=t->dX.C=t->dY.C=t->dZ.C=Pust;
  t->X1.C=t->X2.C=t->Y1.C=t->Y2.C=t->Z1.C=Pust=t->Z2.C=Pust;
  t->Name=strcpy((char *)calloc(strlen(Buf)+1,SzC),Buf);
  return (T?T->Sled:L->T)=t;
 }
 IsError=true; return NULL;
}
//---------------------------------------------------------------------------
bool FindUnkn(char *S){ Cord *C;
 if(!(C=L->P)) return false;
 for(;;C=C->Sled){
  if(!strcmp(C->V->Name,S)) return true; if(!(C->Sled)) return false;
}}
//---------------------------------------------------------------------------
Cord *TakeUnkn(Vary *V){
 Cord *P,*I=(Cord *)calloc(1,sizeof(Cord));
 if(!I){ IsError=true; return NULL; }
 I->V=V;
 if(!(P=L->P)) L->P=I; else { for(;P->Sled;P=P->Sled); P->Sled=I; }
 return I;
}
//---------------------------------------------------------------------------
Cord *TakeRecur(Vary *V,Vary *v){
 Cord *P,*I=(Cord *)calloc(1,sizeof(Cord)),*i=(Cord *)calloc(1,sizeof(Cord));
 if(!I||!i){ IsError=true; return NULL; }
 I->V=V; i->V=v;
 if(!(P=L->R)) L->R=I; else { for(;P->Sled;P=P->Sled); P->Sled=I; }
 if(!(P=L->N)) L->N=i; else { for(;P->Sled;P=P->Sled); P->Sled=i; }
 return I;
}
//---------------------------------------------------------------------------
bool FindVary(char *S,Vary **V){
 Compress(S);
 if(!S||!*S){ *V=NULL; return false; }
 if(L->Time&&!strcmp(L->Time->Name,S)){ *V=L->Time; return true; }
 if(L->Freq&&!strcmp(L->Freq->Name,S)){ *V=L->Freq; return true; }
 if(!(*V=L->V)) return false;
 for(;;*V=(*V)->Sled){
  if(!strcmp((*V)->Name,S)) return true;
  if(!((*V)->Sled)) return false;
}}
//---------------------------------------------------------------------------
Vary *TakeVary(char *S){ Vary *V;
 Compress(S);
 if(FindVary(S,&V)) return V;
 Vary *W=(Vary *)calloc(1,sizeof(Vary));
 if(!W){ IsError=true; return NULL; }
 W->Name=(char *)malloc(strlen(S)+1);
 if(!(W->Name)){ IsError=true; return NULL; }
 strcpy(W->Name,S); W->Atr=VARY; W->P=W->Q=1; W->Znach.C=Pust;
 return (V?V->Sled:L->V)=W;
}
//---------------------------------------------------------------------------
bool FindVary(String S,Vary **V){ return FindVary(S.c_str(),V); }
//---------------------------------------------------------------------------
Vary *TakeVary(String S){ return TakeVary(S.c_str()); }
//---------------------------------------------------------------------------
bool FindCloseElem(char *S,Elem **E){
 if(!(*E=L->E)) return false;
 for(;;*E=(*E)->Sled){
  if(((*E)->Name[0]=='@')&&!strcmp((*E)->Name+1,S)) return true;
  if(!((*E)->Sled)) return false;
}}
//---------------------------------------------------------------------------
bool FindElem(char *S,Elem **E){
 if(!(*E=L->E)) return false;
 for(;;*E=(*E)->Sled){
  if(!strcmp((*E)->Name,S)) return true;
  if(!((*E)->Sled)) return false;
}}
//---------------------------------------------------------------------------
Elem *TakeElem(char *S){ Elem *E;
 if(FindElem(S,&E)) return E;
 Elem *e=(Elem *)calloc(1,sizeof(Elem));
 if(!e){ IsError=true; return NULL; }
 e->Name=(char *)malloc(strlen(S)+1);
 if(!(e->Name)){ IsError=true; return NULL; }
 strcpy(e->Name,S); e->Line=Line; e->Znach.C=Pust;
 return (E?E->Sled:L->E)=e;
}
//---------------------------------------------------------------------------
bool FindArra(char *S,Arra **A){
 if(!(*A=L->A)) return false;
 for(;;*A=(*A)->Sled){
  if(!strcmp((*A)->Name,S)) return true;
  if(!((*A)->Sled)) return false;
}}
//---------------------------------------------------------------------------
Arra *TakeArra(char *S){ Arra *A;
 if(FindArra(S,&A)) return A;
 Arra *a=(Arra *)calloc(1,sizeof(Arra));
 if(!a){ IsError=true; return NULL; }
 a->Name=(char *)malloc(strlen(S)+1); strcpy(a->Name,S);
 if(!(a->Name)){ IsError=true; return NULL; }
 return (A?A->Sled:L->A)=a;
}
//---------------------------------------------------------------------------
Root *FindInstLast(char *S){ Root *Rez=NULL;
 for(Root *I=L->Inst;I;I=I->Sled){
  LowToUpp(I->Name); if(!strcmp(I->Name,S)) Rez=I;
 }
 return Rez;
}
//---------------------------------------------------------------------------
Root *FindOperLast(char *S){ Root *Rez=NULL;
 for(Root *O=L->Oper;O;O=O->Sled){
  LowToUpp(O->Name); if(!strcmp(O->Name,S)) Rez=O;
 }
 return Rez;
}
//---------------------------------------------------------------------------
bool FindInst(char *S,Root **I){
 if(!(*I=L->Inst)) return false;
 for(;;*I=(*I)->Sled){
  LowToUpp((*I)->Name);
  if(!strcmp((*I)->Name,S)) return true;
  if(!((*I)->Sled)) return false;
}}
//---------------------------------------------------------------------------
Root *TakeInst(char *S){ Root *I;
 if(FindInst(S,&I)) return I;
 Root *i=(Root *)calloc(1,sizeof(Root));
 if(!i){ IsError=true; return NULL; }
 i->Name=(char *)malloc(strlen(S)+1);
 if(!(i->Name)){ IsError=true; return NULL; }
 strcpy(i->Name,S);
 return (I?I->Sled:L->Inst)=i;
}
//---------------------------------------------------------------------------
bool FindOper(char *S,Root **O){
 if(!(*O=L->Oper)) return false;
 for(;;*O=(*O)->Sled){
  LowToUpp((*O)->Name);
  if(!strcmp((*O)->Name,S)) return true;
  if(!((*O)->Sled)) return false;
}}
//---------------------------------------------------------------------------
Root *TakeOper(char *S){ Root *O;
 Root *o=(Root *)calloc(1,sizeof(Root));
 if(!o){ IsError=true; return NULL; }
 o->Name=(char *)malloc(strlen(S)+1);
 if(!(o->Name)){ IsError=true; return NULL; }
 strcpy(o->Name,S); O=L->Oper;
 if(O){ while(O->Sled) O=O->Sled; return O->Sled=o; }
 else return L->Oper=o;
}
//---------------------------------------------------------------------------
bool FindCnst(char *S,Cnst **C){
 Compress(S);
 for(*C=Pust;;*C=(*C)->Sled){
  if(strcmp((*C)->Name,S)==0) return true; if(!((*C)->Sled)) return false;
}}
//---------------------------------------------------------------------------
Cnst *TakeCnst(char *S){ Cnst *C;
 Compress(S);
 if(FindCnst(S,&C)) return C;
 Cnst *c=(Cnst *)calloc(1,sizeof(Cnst));
 if(!c){ IsError=true; return NULL; }
 c->Name=(char *)malloc(strlen(S)+1);
 if(!(c->Name)){ IsError=true; return NULL; }
 strcpy(c->Name,S); c->Val=atof(S); c->Atr=CNST; return C->Sled=c;
}
//---------------------------------------------------------------------------
bool FindCnst(double D,Cnst **C){
 for(*C=Pust;;*C=(*C)->Sled){
  if((*C)->Atr&&((*C)->Val==D)) return true;
  if(!((*C)->Sled)) return false;
}}
//---------------------------------------------------------------------------
Cnst *TakeCnst(double D){ char S[80]; Cnst *C;
 if(FindCnst(D,&C)) return C;
 Cnst *c=(Cnst *)calloc(1,sizeof(Cnst));
 if(!c){ IsError=true; return NULL; }
 sprintf(S,"%g",D); c->Name=(char *)malloc(strlen(S)+1);
 if(!(c->Name)){ IsError=true; return NULL; }
 strcpy(c->Name,S);
 c->Val=D; c->Atr=CNST; return C->Sled=c;
}
//---------------------------------------------------------------------------
bool FindList(char *S,List **L){
 if(!(*L=LBeg)) return false;
 for(;;*L=(*L)->Sled){
  if(strcmp((*L)->Name,S)==0) return true; if(!((*L)->Sled)) return false;
}}
//---------------------------------------------------------------------------
List *TakeList(char *S){ List *l;
 if(FindList(S,&L)) return L;
 l=(List *)calloc(1,sizeof(List));
 if(!l){ IsError=true; return NULL; }
 l->Name=(char *)malloc(strlen(S)+1);
 if(!(l->Name)){ IsError=true; return NULL; }
 (L?L->Sled:LBeg)=l; L=LBeg;
 strcpy(l->Name,S); l->U=NULL;
 l->Time=TakeVary("t"); l->Freq=TakeVary("w");
 if(!strcmp(S,"ОСНОВНОЙ")){
  l->V=TakeVary("g"); l->V->P=0; l->V->Val=9.8062;
  l->V->Znach.C=TakeCnst(9.8062);
 }
 else l->V=NULL;
 return l;
}
//---------------------------------------------------------------------------
bool FindUnop(int Oper,Form F,Unop **U){
 if(!(*U=L->U)) return false;
 for(;;*U=(*U)->Sled){
  if((Oper==(*U)->Func)&&(F==(*U)->Arg)) return true;
  if(!((*U)->Sled)) return false;
}}
//---------------------------------------------------------------------------
Unop *TakeUnop(int Oper,Form F){ Unop *U,*g;
 if(FindUnop(Oper,F,&U)) return U;
 g=(Unop *)calloc(1,sizeof(Unop));
 if(!g){ IsError=true; return NULL; }
 g->Atr=UNOP; g->Func=Oper; g->Arg=F; g->O=0;
 return (U?U->Sled:L->U)=g;
}
//---------------------------------------------------------------------------
Bnop *TakeBnop( int Oper,Form L,Form R){
 Bnop *g=(Bnop *)calloc(1,sizeof(Bnop));
 if(!g){ IsError=true; return NULL; }
 g->Atr=BNOP; g->Oper=Oper; g->L=L; g->R=R; return g;
}
//---------------------------------------------------------------------------
Ifop *TakeIfop( int Oper,Form U,Form V,Form FT,Form FF){
 Ifop *g=(Ifop *)calloc(1,sizeof(Ifop));
 if(!g){ IsError=true; return NULL; }
 g->Atr=IFOP; g->Oper=Oper; g->U=U; g->V=V; g->FT=FT; g->FF=FF; return g;
}
//---------------------------------------------------------------------------
bool FindIpll(int Type,Arra *A,Form F,Ipll **I){
 if(!(*I=L->L)) return false;
 for(;;*I=(*I)->Sled){
  if((Type==(*I)->Type)&&(A==(*I)->A)&&(F==(*I)->F)) return true;
  if(!((*I)->Sled)) return false;
}}
//---------------------------------------------------------------------------
Ipll *TakeIpll(int Type,Arra *A,Form F){ Ipll *I,*i;
 if(FindIpll(Type,A,F,&I)) return I;
 i=(Ipll *)calloc(1,sizeof(Ipll));
 if(!i){ IsError=true; return NULL; }
 i->Atr=IPLL; i->Type=Type; i->A=A; i->F=F;
 return (I?I->Sled:L->L)=i;
}
//---------------------------------------------------------------------------
bool FindSpln(int Typ,Arra *A,Form Arg,Spln **P){
 if(!(*P=L->S)) return false;
 for(;;*P=(*P)->Sled){
  if(((*P)->A==A)&&(Typ==(*P)->Type)&&(Arg==(*P)->F)) return true;
  if(!((*P)->Sled)) return false;
}}
//---------------------------------------------------------------------------
void TakeSplns(int Typ,Arra *A){
 if(A->N){
  for(Spln *P=L->S;P;P=P->Sled)
   if((P->A==A)&&(P->Type==Typ)&&!P->a&&!P->b&&!P->c&&!P->d)
    P->CalcSpln();
}}
//---------------------------------------------------------------------------
bool FindSpln(Spln *R,char *S,Spln **P){
 if(!(*P=R)) return 0;
 if(strcmp(S,R->A->Name)<0){ *P=NULL; return 0; }
 for(;;*P=(*P)->Sled){
  if(!strcmp((*P)->A->Name,S)) return 1;
  if(!((*P)->Sled)) return 0;
  if(strcmp((*P)->Sled->A->Name,S)>0) return 0;
}}
//---------------------------------------------------------------------------
Spln *TakeSpln(Spln **R,char *S,Arra *A){
 Spln *P,*p; if(!S) return NULL;
 if(FindSpln(*R,S,&P)) p=P;
 else{
  p=(Spln *)calloc(1,sizeof(Spln));
  if(!P){ p->Sled=*R;           *R=p; }
  else  { p->Sled=P->Sled; P->Sled=p; }
 }
 p->A=A; p->A->Name=strcpy((char *)calloc(strlen(S)+1,SzC),S);
 return p;
}
//---------------------------------------------------------------------------
int GausCrout(double **A,double *B,int n,double *Y,int Repeat);
//---------------------------------------------------------------------------
int SolTred(double *a,double *b,double *d,int n,double *x){
// a - нижняя (верхняя) диагональ, b - главная диагональ,
// d - вектор правой части, x - вектор решения, n - порядок системы.
 double *B=(double *)calloc(n,SzD),*D=(double *)calloc(n,SzD); int i;
//найдем рекурсивные коэффициенты
 D[0]=d[0]; B[0]=b[0];
 for(i=1;i<n;i++){
  if(B[i-1]==0.0) return 0;
  else{
   D[i]=d[i]-a[i-1]*D[i-1]/B[i-1];
   B[i]=b[i]-a[i-1]*a[i-1]/B[i-1];
 }}
 x[n-1]=D[n-1]/B[n-1];
 if(n>2) for(i=n-2;i;i--) x[i]=(D[i]-a[i-1]*x[i+1])/B[i];
 if(n>1) x[0]=(D[0]-a[0]*x[1])/B[0];
 free(B); free(D);
 return true;
}
//---------------------------------------------------------------------------
void Spln::CalcSpln(void){
 int i,N=A->N/2-1,Rez=1; //число интервалов
 double *X=(double *)calloc(4*N,sizeof(double)),//массив неизвестных a,b,c,d .
  *B=(double *)calloc(N,sizeof(double)),//главная диагональ трехдиагональной матрицы
  *C=(double *)calloc(N,sizeof(double)),//нижняя поддиагональ трехдиагональной матрицы
  *D=(double *)calloc(N,sizeof(double)),hi;//массив правых частей
 if(!B||!C||!D||!X){
  sprintf(Inf,"Недостаточно памяти для размещения сплайна \"%s\".",A->Name);
  if(B) free(B); if(X) free(X); if(C) free(C); if(D) free(D); Error(0,0,true); return;
 }
 a=X; b=a+N; c=b+N; d=c+N;
 for(int i=0;i<N-1;i++){
  hi=A->A[2*(i+1)]-A->A[2*i];
  C[i]=1.0; B[i]=4.0;
  D[i]=3.0*(A->A[2*i+1]-2*A->A[2*i+3]+A->A[2*i+5])/hi/hi;
 }
 Rez=SolTred(C,B,D,N-1,c+1);
 if(!Rez){
  sprintf(Inf,"Система уравнений несовместна для сплайна \"%s\"!",A->Name);
  Error(0,0,true);
 }
 for(i=0;i<N-1;i++){
  hi=A->A[2*(i+1)]-A->A[2*i]; a[i]=A->A[2*i+1];
  b[i]=(A->A[2*i+3]-A->A[2*i+1])/hi-(c[i+1]+2.0*c[i])*hi/3.0;
  d[i]=(c[i+1]-c[i])/3.0/hi;
 }
 hi=A->A[2*(i+1)]-A->A[2*i]; a[i]=A->A[2*i+1];
 b[i]=(A->A[2*i+3]-A->A[2*i+1])/hi-2.0*c[i]*hi/3.0;
 d[i]=-c[i]/3.0/hi;
 free(B); free(C); free(D);
}
//---------------------------------------------------------------------------
Spln *TakeSpln(int Typ,Arra *A,Form Arg){ Spln *P;
 if(FindSpln(Typ,A,Arg,&P)){ P->A=A; if(A->N) P->CalcSpln(); return P; }
 Spln *p=(Spln *)calloc(1,sizeof(Spln));
 if(!p){ IsError=true; return NULL; }
 p->Atr=SPLN; p->Type=Typ; p->A=A; p->F=Arg;
 if(A->N) p->CalcSpln();
 return (P?P->Sled:L->S)=p;
}
//---------------------------------------------------------------------------
Spln *TakeSpln(int Type,Arra *A,double *a,double *b,double *c,double *d,Form F){
 Spln *g=(Spln *)calloc(1,sizeof(Spln));
 if(!g){ IsError=true; return NULL; }
 g->Atr=SPLN; g->Type=Type; g->A=A; g->F=F; g->a=a; g->b=b; g->c=c; g->d=d;
 if(L->S){ Spln *s; for(s=L->S;s->Sled;s=s->Sled); s->Sled=g; }
 else L->S=g;
 return g;
}
//---------------------------------------------------------------------------
Summ *TakeSumm(Form F,Vary *i,Form a,Form b){
 Summ *g=(Summ *)calloc(1,sizeof(Summ));
 if(!g){ IsError=true; return NULL; }
 g->Atr=SUMM; g->Type=0; g->i=i; g->a=a; g->b=b; g->F=F;
 if(L->M){ Summ *s; for(s=L->M;s->Sled;s=s->Sled); s->Sled=g; }
 else L->M=g;
 return g;
}
//---------------------------------------------------------------------------
Intg *FindIntg(Form f,Vary *x,Form a,Form b,int Type){ Intg *I;
 for(I=L->I;I;I=I->Sled)
  if((I->f==f)&&(I->x==x)&&(I->a==a)&&(I->b==b)&&(I->Type==Type)) break;
 return I;
}
//---------------------------------------------------------------------------
Intg *TakeIntg(Form f,Vary *x,Form a,Form b){
 Intg *g;
 g=(Intg *)calloc(1,sizeof(Intg));
 if(!g){ IsError=true; return NULL; }
 g->Atr=INTG; g->Type=1; g->P=1; g->x=x;
 g->f=f; g->a=a; g->b=b; g->F.C=g->Rez.C=Pust;
 return g;
}
//---------------------------------------------------------------------------
Intg *TakeIntg(Form f,Vary *x){
 Form a,b; Intg *g;
 a.C=b.C=Pust;
 g=(Intg *)calloc(1,sizeof(Intg));
 if(!g){ IsError=true; return NULL; }
 g->Atr=INTG; g->Type=0; g->x=x;
 g->a.C=g->b.C=g->F.C=g->Rez.C=Pust;
 g->f=f;
 return g;
}
//---------------------------------------------------------------------------
Solu *FindSol(Form f,Vary *x,Form a,Form b){ Solu *s;
 for(s=L->C;s;s=s->Sled) if((s->f==f)&&(s->x==x)&&(s->a==a)&&(s->b==b)) break;
 return s;
}
//---------------------------------------------------------------------------
Solu *TakeSolu(Form f,Vary *x,Form a,Form b){
 Solu *s=(Solu *)calloc(1,sizeof(Solu));
 if(!s){ IsError=true; return NULL; }
 s->Atr=RUUT; s->x=x; s->f=f; s->a=a; s->b=b;
 if(L->C){ Solu *r; for(r=L->C;r->Sled;r=r->Sled); r->Sled=s; }
 else L->C=s;
 return s;
}
//---------------------------------------------------------------------------
Rest *FindRest(Form a,Form b){ Rest *r;
 for(r=L->O;r;r=r->Sled) if((r->a==a)&&(r->b==b)) break;
 return r;
}
//---------------------------------------------------------------------------
Rest *TakeRest(Form a,Form b){
 Rest *r=(Rest *)calloc(1,sizeof(Rest));
 if(!r){ IsError=true; return NULL; }
 r->Atr=REST; r->a=a; r->b=b;
 if(L->O){ Rest *o; for(o=L->O;o->Sled;o=o->Sled); o->Sled=r; }
 else L->O=r;
 return r;
}
//---------------------------------------------------------------------------
Form TakeApForm(Form F){
 switch(F.C->Atr){
  case BNOP: TakeApForm(F.B->L);   TakeApForm(F.B->R);
	     free(F.B); break;
  case IFOP: TakeApForm(F.L->U);   TakeApForm(F.L->V);
	     TakeApForm(F.L->FT);  TakeApForm(F.L->FF);
	     free(F.L); break;
  case SUMM: TakeApForm(F.S->a);   TakeApForm(F.S->b);
	     TakeApForm(F.S->F);   free(F.S); break;
  case IPLL: TakeApForm(F.I->F);   free(F.I); break;
  case SPLN: TakeApForm(F.I->F);   free(F.I); break;
 }
 return F.C=Pust,F;
}
//---------------------------------------------------------------------------
void TakeApParm(Parm *P){
 Parm *p; while(p=P){ P=p->Sled; if(p->Nam) free(p->Nam); free(p); }
}
//---------------------------------------------------------------------------
void TakeApOper(Root *O){
 TakeApParm(O->First); if(O->Name) free(O->Name); free(O);
}
//---------------------------------------------------------------------------
bool NeIzv(Vary *V){ Root *I;
 if((I=FindInstLast("НЕИЗВЕСТНЫЕ"))||(I=FindInstLast("НЕИЗВЕСТНОЕ"))||
    (I=FindInstLast("НЕИЗВЕСТНАЯ"))){
  for(Parm *p=I->First;p;p=p->Sled) if(!strcmp(p->Nam,V->Name)) return true;
 }
 return false;
}
//---------------------------------------------------------------------------
Vary *Recur(Vary *V){ Root *I; Vary *v;//возвращаем аргумент
 if(I=FindInstLast("НОВЫЕ ЗНАЧЕНИЯ")){
  for(Parm *p=I->First;p;p=p->Sled)
   if(!strcmp(p->Nam,V->Name)&&FindVary(p->R->Nam,&v)) return v;
 }
 return NULL;
}
//---------------------------------------------------------------------------
bool NoDefine(Vary *V){ char *s,*s1;
 if(V->Znach!=Pust) return false;
 if(s=strchr(V->Name,'\'')){
  if(!strcmp(++s,L->Time->Name)) return false;
  if(s1=strchr(s,'\'')){ *s1='\0';
   if((!strcmp(s,L->Time->Name))&&(!strcmp(s1+1,L->Time->Name))){
    *s1='\''; return false;
 }}}
 return true;
}
//---------------------------------------------------------------------------
void DifTime1(Vary *V){
 Form F,Fv; Vary *K; char NameV[256],*s,qst[256];
 strcpy(NameV,V->Name); s=strchr(NameV,'\'');  *s='\0';
 K=TakeVary(NameV);
 F=DifBy(K->Znach,L->Time);
 for(Cord *q=L->K;q;q=q->Sled){
  sprintf(qst,"%s'%s",q->V->Name,L->Time->Name); Fv.V=TakeVary(qst);
  F=F+DifBy(K->Znach,q->V)*Fv;
 }
 V->Znach=F;
}
//---------------------------------------------------------------------------
void DifTime2(Vary *V){
 Form F,Fv,Fw; Vary *K; char *s=strrchr(V->Name,'\''),qs2t[80],qst[80];
 *s='\0'; K=TakeVary(V->Name); *s='\'';
 if(K->Znach==Pust) DifTime1(K);
 F=DifBy(K->Znach,L->Time);
 for(Cord *q=L->K;q;q=q->Sled){
  sprintf( qst,"%s'%s",q->V->Name,L->Time->Name); Fv.V=TakeVary( qst);
  sprintf(qs2t,"%s'%s",Fv.V->Name,L->Time->Name); Fw.V=TakeVary(qs2t);
  F=F+DifBy(K->Znach,q->V)*Fv+DifBy(K->Znach,Fv.V)*Fw;
 }
 V->Znach=F;
}
//---------------------------------------------------------------------------
bool Coord(char *NameV){
 for(Cord *K=L->K;K;K=K->Sled)
  if(!strcmp(K->V->Name,NameV)) return true;
 return false;
}
//---------------------------------------------------------------------------
void TakeDifTime(){
 char *s,*s1,Time1[256],Time2[256],Name[256]; Root *I; Vary *V;
// Сделаем производные по времени
 sprintf(Time2,"'%s'%s",L->Time->Name,L->Time->Name);
 sprintf(Time1,"'%s",L->Time->Name);
 for(V=L->V;V;V=V->Sled){
  if(V->Znach!=Pust) continue;
  if(!(s=strchr(V->Name,'\''))) continue;
  strcpy(Name,V->Name);
  if(s=strstr(Name,Time2)){
   *s='\0'; if(Coord(Name)) continue; *s='\''; DifTime2(V);
  }
  else if(s=strstr(Name,Time1)){
   *s='\0'; if(Coord(Name)) continue; *s='\''; DifTime1(V);
 }}
 if((I=FindInstLast("ПОКАЗАТЬ"))||(I=FindInstLast("ПЕЧАТАТЬ"))){
  for(Parm *P=I->First;P;P=P->Sled){
   if(!P->Nam||!(s=strchr(P->Nam,'\''))) continue;
   V=TakeVary(P->Nam); if(V->Znach!=Pust) continue;
   strcpy(Name,V->Name);
   if(s=strstr(Name,Time2)){
    *s='\0';
    if(Coord(Name)) continue;
    *s='\'';
    DifTime2(V);
   }
   else if(s=strstr(Name,Time1)){
    *s='\0';
    if(Coord(Name)) continue;
    *s='\'';
    DifTime1(V);
}}}}
//---------------------------------------------------------------------------
bool NoVariation(Vary *V){
 Root *I; Vary *v;
 char *Instruct[]={"ВАРЬИРОВАТЬ","ДИАПАЗОН ГАРМОНИК","ГАРМОНИКИ","ГАРМОНИКА",
  "ДИАПАЗОН ЧАСТОТ","ЧАСТОТЫ","ЧАСТОТА","ДИАПАЗОН ОБОРОТОВ","ОБОРОТЫ","ОБОРОТ",
  "ДИАПАЗОНЫ","ДИАПАЗОН"};
 for(int i=0;i<12;i++){
  if(I=FindInstLast(Instruct[i])){
   for(Parm *P=I->First;P;P=P->Sled){
    if(P->Nam&&!strcmp(V->Name,P->Nam)) return false;
 }}}
 return true;
}
//---------------------------------------------------------------------------
bool PsevdoVelosity(Vary *V){
 Root *I; Vary *v;
 if(!(I=FindInstLast("ПСЕВДОСКОРОСТИ"))&&!(I=FindInstLast("КВАЗИСКОРОСТИ")))
  return false;
 for(Parm *P=I->First;P;P=P->Sled){
  if(P->Nam&&!strcmp(V->Name,P->Nam)) return true;
 }
 return false;
}
//---------------------------------------------------------------------------
bool NoVelosity(Vary *V){//проверяем, не задана ли скорость переменной
 sprintf(Inf,"%s'%s",V->Name,L->Time->Name);
 if(FindVary(Inf,&V)&&(V->Znach.C!=Pust)) return false; return true;
}
//---------------------------------------------------------------------------
bool Delay(Vary *V){//не яляется ли переменная сдвинутой по времени другой переменной
 char *s=strchr(V->Name,'~'),*ss;
 if(s){
  *s='\0';
  for(Vary *v=L->V;v;v=v->Sled){
   if(!strcmp(v->Name,s+1)){//
    if(ss=strchr(V->Name,'\'')){
     if(!strcmp(ss+1,L->Time->Name)){ *s='~'; TakeDelayV(V); }
    }
    else{ *s='~'; TakeDelayK(V); }
    return true;
  }}
  *s='~';
 }
 return false;
}
//---------------------------------------------------------------------------
bool GeneralVelosity(Vary *V){//проверяем, что имя переменной есть обобщенная скорость
 char *s=strchr(V->Name,'\'');
 if(s){
  *s='\0';
  if(Coord(V->Name)&&!strchr(s+1,'\'')){ *s='\''; return true; }
  *s='\''; return false;
 }
 return false;
}
//---------------------------------------------------------------------------
int Coord(List *L){
 int Nk=0; Vary *v; Cord *K,*P,*Z,*W,*R,*N,*Dq,*Dv; Root *I; NzK=NPV=0;
 while(K=L->K){ L->K=K->Sled; free(K); }
 while(P=L->P){ L->P=P->Sled; free(P); }
 while(Z=L->Z){ L->Z=Z->Sled; free(Z); }
 while(W=L->W){ L->W=W->Sled; free(W); }
 while(R=L->R){ L->R=R->Sled; free(R); }
 while(N=L->N){ L->N=N->Sled; free(N); }
 while(Dq=L->Dq){ L->Dq=Dq->Sled; free(Dq); }
 while(Dv=L->Dv){ L->Dv=Dv->Sled; free(Dv); }
 for(Vary *V=L->V;V;V=V->Sled){
  if(Delay(V)) continue;
 //  if(strchr(V->Name,'\'')) continue;
  if(NoDefine(V)&&strcmp(V->Name,"_T")&&strcmp(V->Name,"_П")&&
     strcmp(V->Name,"_dTpodt")&&NoVariation(V)){
   if(NeIzv(V)) TakeUnkn(V);
   else if(v=Recur(V)) TakeRecur(V,v);
   else if(PsevdoVelosity(V)){ NPV++; TakePsevdo(V); }
   else if(NoVelosity(V)){ Nk++; TakeCord(V); }
   else { NzK++; TakeZavC(V); }
 }}
 for(K=L->Dq;K;K=K->Sled){
  char *s=strchr(K->V->Name,'~'); Vary *V;
  *s='\0';//если до '~' записана не обобщенная координата генерируем новую о.к.
  if(!Coord(K->V->Name)){ V=TakeVary(K->V->Name); Nk++; TakeCord(V); }
  *s='~';
 }
 for(K=L->Dv;K;K=K->Sled){
  char *s=strchr(K->V->Name,'~'); Vary *V;
  *s='\0';
//если до '~' записана не обобщ. скорость и не псевдоскорость генерируем новую о.к.
  if(!GeneralVelosity(K->V)&&!PsevdoVelosity(K->V)){
   V=TakeVary(K->V->Name); Nk++; TakeCord(V);
  }
  *s='~';
 }
 TakeDifTime();
 return Nk;
}
//---------------------------------------------------------------------------
int CoordWithoutDT(List *L){
//Создание списка ОК без генерации об. скоростей и ускорений
 int Nk=0; Vary *v; Cord *K,*P,*Z,*W,*R,*N,*Dq,*Dv; Root *I; NzK=NPV=0;
 while(K=L->K){ L->K=K->Sled; free(K); }
 while(P=L->P){ L->P=P->Sled; free(P); }
 while(Z=L->Z){ L->Z=Z->Sled; free(Z); }
 while(W=L->W){ L->W=W->Sled; free(W); }
 while(R=L->R){ L->R=R->Sled; free(R); }
 while(N=L->N){ L->N=N->Sled; free(N); }
 while(Dq=L->Dq){ L->Dq=Dq->Sled; free(Dq); }
 while(Dv=L->Dv){ L->Dv=Dv->Sled; free(Dv); }
 for(Vary *V=L->V;V;V=V->Sled){
//  if(strchr(V->Name,'\'')) continue;
  if(NoDefine(V)&&strcmp(V->Name,"_T")&&strcmp(V->Name,"_П")&&
     strcmp(V->Name,"_dTpodt")&&NoVariation(V)){
   if(NeIzv(V)) TakeUnkn(V);
   else if(v=Recur(V)) TakeRecur(V,v);
   else if(PsevdoVelosity(V)){ NPV++; TakePsevdo(V); }
   else if(NoVelosity(V)){ Nk++; TakeCord(V); }
   else { NzK++; TakeZavC(V); }
 }}
 for(K=L->Dq;K;K=K->Sled){
  char *s=strchr(K->V->Name,'~'); Vary *V;
  *s='\0';//если до '~' записана не обобщенная координата генерируем новую о.к.
  if(!Coord(K->V->Name)){ V=TakeVary(K->V->Name); Nk++; TakeCord(V); }
  *s='~';
 }
 for(K=L->Dv;K;K=K->Sled){
  char *s=strchr(K->V->Name,'~'); Vary *V;
  *s='\0';
//если до '~' записана не обобщ. скорость и не псевдоскорость генерируем новую о.к.
  if(!GeneralVelosity(K->V)&&!PsevdoVelosity(K->V)){
   V=TakeVary(K->V->Name); Nk++; TakeCord(V);
  }
  *s='~';
 }
 return Nk;
}
//---------------------------------------------------------------------------
bool VassalOfKoord(List *L,Form F){
 for(Cord *K=L->K;K;K=K->Sled){
  if(VarInForm(true,F,K->V)) return true;
 }
 return false;
}
//---------------------------------------------------------------------------
bool VassalOfPsevdo(List *L,Form F){
 char S[256]; Vary *V;
 for(Cord *K=L->W;K;K=K->Sled){
  if(VarInForm(true,F,K->V)) return true;
  sprintf(S,"%s'%s",K->V->Name,L->Time->Name); V=TakeVary(S);
  if(VarInForm(true,F,V)) return true;
 }
 return false;
}
//---------------------------------------------------------------------------
bool VassalOfDepedent(List *L,Form F){
 for(Cord *K=L->Z;K;K=K->Sled){
  if(VarInForm(true,F,K->V)) return true;
 }
 return false;
}
//---------------------------------------------------------------------------
bool VassalOfObscure(List *L,Form F){
 for(Cord *K=L->P;K;K=K->Sled){
  if(VarInForm(true,F,K->V)) return true;
 }
 return false;
}
//---------------------------------------------------------------------------
bool VassalOfVelosity(List *L,Form F){
 char S[256]; Vary *V;
 for(Cord *K=L->K;K;K=K->Sled){
  sprintf(S,"%s'%s",K->V->Name,L->Time->Name); V=TakeVary(S);
  if(VarInForm(true,F,V)) return true;
 }
 return false;
}
//---------------------------------------------------------------------------
bool VassalOfAccelerat(List *L,Form F){
 char S[256]; Vary *V;
 for(Cord *K=L->K;K;K=K->Sled){
  sprintf(S,"%s'%s'%s",K->V->Name,L->Time->Name,L->Time->Name); V=TakeVary(S);
  if(VarInForm(true,F,V)) return true;
 }
 return false;
}
//---------------------------------------------------------------------------
bool VassalOfKoord(List *L,Vary *V){
 for(Cord *K=L->K;K;K=K->Sled){
  if((V==K->V)||VarInForm(true,V->Znach,K->V)) return true;
 }
 return false;
}
//---------------------------------------------------------------------------
bool VassalOfKoord(List *L,Elem *E){
 return VassalOfKoord(L,E->K);
}
//---------------------------------------------------------------------------
bool IsVaried(List *L,Form F){
 Root *I=FindInstLast(TextFromFile(WORKDIR,"EditKiDyM",173)); Vary *V;
 if(!I) I=FindInstLast(TextFromFile(WORKDIR,"EditKiDyM",175));
 if(!I) I=FindInstLast(TextFromFile(WORKDIR,"EditKiDyM",176));
 if(I){
  for(Parm *P=I->First;P;P=P->Sled){
   if(P->Nam&&FindVary(P->Nam,&V)&&VarInForm(true,F,V)) return true;
   if(!P->R){ P=P->Sled->Sled; }
 }}
 return false;
}
//---------------------------------------------------------------------------
bool EqvConst(Form F){
 if(F.C->Atr==3||F.C->Atr==0) return false;
 if(VarInForm(true,F,L->Time))
  return false; //зависит от времени
 if(VarInForm(true,F,L->Freq))
  return false; //зависит от частоты
 if(L->K){
  if(VassalOfKoord(L,F))
   return false;//зависит от обобщенных координат
  if(VassalOfVelosity(L,F))
   return false;//зависит от обобщенных скоростей
  if(VassalOfAccelerat(L,F))
   return false;//зависит от обобщенных ускорений
 }
 if(L->W&&VassalOfPsevdo(L,F))
  return false;//зависит от псевдокоростей
 if(L->Z&&VassalOfDepedent(L,F))
  return false;//зависит от координат с зависимыми скоростями
 if(L->P&&VassalOfObscure(L,F))
  return false;//зависит от неизвестных
 if(IsVaried(L,F))
  return false;//зависит от варьируемых переменных
 return true;
}
//---------------------------------------------------------------------------
int KolInst(char *Name){ int Rez=0; char NameInst[256];
 for(Root *r=L->Inst;r;r=r->Sled){
  strcpy(NameInst,r->Name); LowToUpp(NameInst);
  if(!strcmp(NameInst,Name)) Rez++;
 }
 return Rez;
}
//---------------------------------------------------------------------------
int VaryToIntg(int k,Form F,Cnst *Znach){
//сборка интегралов в список листа из формулы F и присвоение переменным
//интегрирования значений Znach
 switch(F.C->Atr){
  case CNST :
  case VARY : break;
  case UNOP : k=VaryToIntg(k,F.U->Arg,Znach);  break;
  case BNOP : k=VaryToIntg(k,F.B->L,Znach); k=VaryToIntg(k,F.B->R,Znach); break;
  case IFOP : k=VaryToIntg(k,F.L->U,Znach); k=VaryToIntg(k,F.L->V,Znach);
   k=VaryToIntg(k,F.L->FF,Znach); k=VaryToIntg(k,F.L->FT,Znach);  break;
  case SUMM: k=VaryToIntg(k,F.S->F,Znach);  break;
  case INTG:
   if(!FindIntg(F.G->f,F.G->x,F.G->a,F.G->b,1)){
    if(!L->I){ L->I=F.G; L->I->Sled=NULL; }
    else{
     Intg *s; for(s=L->I;s->Sled;s=s->Sled); s->Sled=F.G; s->Sled->Sled=NULL;
    }
    k++; F.G->x->Znach.C=Znach;
   }
   k=VaryToIntg(k,F.G->a,Znach); k=VaryToIntg(k,F.G->b,Znach);
   k=VaryToIntg(k,F.G->f,Znach);
   break;
  case IPLL: k=VaryToIntg(k,F.I->F,Znach);  break;
  case SPLN: k=VaryToIntg(k,F.P->F,Znach);
 }
 return k;
}
//---------------------------------------------------------------------------
int VaryToSol(int k,Form F,Cnst *Znach){
//сборка корней в список листа из формулы F и присвоение переменным
//корней значений Znach
 switch(F.C->Atr){
  case CNST :
  case VARY : break;
  case UNOP : k=VaryToSol(k,F.U->Arg,Znach);  break;
  case BNOP : k=VaryToSol(k,F.B->L,Znach); k=VaryToSol(k,F.B->R,Znach); break;
  case IFOP : k=VaryToSol(k,F.L->U,Znach); k=VaryToSol(k,F.L->V,Znach);
   k=VaryToSol(k,F.L->FF,Znach); k=VaryToSol(k,F.L->FT,Znach);  break;
  case SUMM: k=VaryToSol(k,F.S->F,Znach);  break;
  case RUUT:
   if(!FindSol(F.R->f,F.R->x,F.R->a,F.R->b)){
    if(!L->C){ L->C=F.R; L->C->Sled=NULL; }
    else{
     Solu *s; for(s=L->C;s->Sled;s=s->Sled); s->Sled=F.R; s->Sled->Sled=NULL;
    }
    k++; F.R->x->Znach.C=Znach;
   }
   k=VaryToSol(k,F.R->a,Znach); k=VaryToSol(k,F.R->b,Znach);
   k=VaryToSol(k,F.R->f,Znach);
   break;
  case IPLL: k=VaryToSol(k,F.I->F,Znach);  break;
  case SPLN: k=VaryToSol(k,F.P->F,Znach);
 }
 return k;
}
//---------------------------------------------------------------------------
bool __fastcall ReplaceVarsToConst(TStatusBar *StatusBar,TCGauge *CGauge,
  bool Second){
 int K=0,Ki=0,Kr=0; Form F; bool Uge=false; FILE *FILE,*FF; bool Rez=false;
 AnsiString Inf=StatusBar->SimpleText.c_str();
 StatusBar->SimpleText=
  "Замена переменных их постоянными значениями, взятие интегралов и поиск корней";
 if(CGauge){ CGauge->MaxValue=K; CGauge->Progress=0; }
 if(DGSFILE=fopen(DgsFile,"r+")) FILE=DGSFILE;
 else FILE=HTMFILE;
 if(L->I&&Second){
  FF=fopen("timedgs.htm","w");
  if(FF){ char S[1024];
   while(fgets(S,1023,DGSFILE)){
	if(!strstr(S,"Переменные, формулы которых не зависят от времени,"))
	 fputs(S,FF);
	else break;
   }
   fclose(FILE); unlink(FILE==DGSFILE?DgsFile:HtmFile);
   fclose(FF); rename("timedgs.htm",FILE==DGSFILE?DgsFile:HtmFile);
   FILE=fopen(FILE==DGSFILE?DgsFile:HtmFile,"r+");
   if(FILE==DGSFILE) fseek(DGSFILE,0,2);
  }
  Rez=true;
 }
 else
  if(FILE==DGSFILE) fseek(DGSFILE,-LpodvD,2);
 for(Vary *V=L->V;V;V=V->Sled){
  if(CGauge) CGauge->Progress++;
  if(EqvConst(V->Znach)){
   if(!Uge){//один раз пишем
	fprintf(FILE,"<br><h4 align=\"left\"><font color=\"#000099\"><b>"
	 "Переменные, формулы которых не зависят от времени,\n"
	 "поэтому в расчетах используются их постоянные значения:</b></font></h4>\n");
	fputs("<p align=\"left\">",FILE);
	Uge=true;
   }
   h_RK=-1.0; F.V=V; V->Val=Value(F); V->P=0;
   PrintForm(false,FILE,V->Name,V->Znach);
   fprintf(FILE,"=%g;<br>",V->Val);
 }}
 fputs("</p>\n",FILE);
 if(L->I){ Intg *I;
  StatusBar->SimpleText="Аналитическое вычисление интегралов";
  for(I=L->I;I;I=I->Sled) Ki++;
  if(CGauge){ CGauge->Progress=0; CGauge->MaxValue=Ki; } Uge=false;
  for(I=L->I;I;I=I->Sled){
   if(CGauge) CGauge->Progress++;
   if((I->F!=Pust)||IsTaken(I)){ F.G=I;
	if(!Uge){//один раз пишем
	 fprintf(FILE,"<h4 align=\"left\"><font color=\"#000099\"><b>"
	  "Интегралы, которые берутся аналитически,\n"
	  "поэтому в расчетах используются их аналитические значения:</b></font></h4>\n");
	 fputs("<p align=\"left\">",FILE);
	 Uge=true;
	}
	PrintForm(false,FILE,NULL,F);   //Распечатаем интеграл
	fprintf(FILE,"<br>");
  }}
  for(I=L->I;I;I=I->Sled){
   if(I->x==L->Time&&I->b.V==L->Time){
	I->B=Value(I->a); I->Val=0.0;
  }}
  if(CGauge) CGauge->Progress=0;
  if(Uge) fputs("</p>\n",FILE);
 }
 if(FILE==DGSFILE){
  fprintf(DGSFILE,"<hr><font face=\"Georgia, Times New Roman, Times, serif\""
   " color=\"#326464\">&copy; %s, %d<br></font>\n",Autor,NYear+1900);
  fprintf(DGSFILE,"</body>\n</html>\n");
 }
 else{
  fprintf(HTMFILE,
   "<hr><font face=\"Georgia, Times New Roman, Times, serif\""
   " color=\"#326464\">&copy; %s, %d<br></font><br>\n",Autor,NYear+1900)+1;
  fprintf(HTMFILE,"</body>\n</html>\n")+2;
 }
 fclose(FILE);
 if(CGauge) CGauge->Progress=0; StatusBar->SimpleText=Inf;
 return Rez;
}
//---------------------------------------------------------------------------

