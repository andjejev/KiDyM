#include "CA.h"
#include "tablw.h"
#include <string.h>
#include <stdio.h>
#include <io.h>
#include <stdlib.h>
#include <alloc.h>
#include <math.h>

extern List *L,*LBeg; extern Vary *Time,*Freq;
extern int Line,IsError,NzK,NPV,Kpodv,NYear,KColumn;
extern Cnst *Pust,*C,*Nul,*Odin; extern String SF;
extern wchar_t Inf[],DgsFile[],WORKDIR[],Autor[],AtrNelin,
 Sdgs[],*sdgs;
extern FILE *DGSFILE,*HTMFILE; extern double h_RK;
extern bool PRINTVARS,OTLADKA; extern Magazine *PrintVars,*MUnit;
extern TStringList *SLD,*SLP,*SLK;

Cord *Rm,*Rl,*Rt,*Ra;//Корни осн.размерностей
//---------------------------------------------------------------------------
void DelAllList(void **Rout){
 for(void *P;P=*Rout;*Rout=Sled(P),free(P)); *Rout=NULL;
}
//---------------------------------------------------------------------------
bool FindParmParm(Parm *R,wchar_t *S,Parm **P,Parm **p){
 for(*P=R;*P;*P=(*P)->Sled)
  for(*p=(*P)->R;*p;*p=(*p)->Sled)
   if((*p)->Nam&&!wcscmp((*p)->Nam,S)) return true;
 return false;
}
//---------------------------------------------------------------------------
bool FindParm(Parm *R,wchar_t *S,Parm **P){
 for(*P=R;*P;*P=(*P)->Sled){
  if((*P)->Nam&&!wcscmp((*P)->Nam,S)) return true;
 }
 return false;
}
//---------------------------------------------------------------------------
bool FindParmUpp(Parm *R,wchar_t *S,Parm **P){
 wchar_t UppS[256],UppName[256];
 wcscpy(UppS,S); LowToUpp(UppS);
 for(*P=R;*P;*P=(*P)->Sled){
  if((*P)->Nam){
   wcscpy(UppName,(*P)->Nam); LowToUpp(UppName);
   if(!wcscmp(UppName,UppS)) return true;
 }}
 return false;
}
//---------------------------------------------------------------------------
bool FindParmUppStr(Parm *R,wchar_t *S,Parm **P){
 wchar_t UppS[256],UppName[256];
 wcscpy(UppS,S); LowToUpp(UppS);
 for(*P=R;*P;*P=(*P)->Sled){
  if((*P)->Nam){
   wcscpy(UppName,(*P)->Nam); LowToUpp(UppName);
   if(wcsstr(UppName,UppS)==UppName) return true;
 }}
 return false;
}
//---------------------------------------------------------------------------
bool FindCord(wchar_t *S,Cord **K){
 if(!(*K=L->K)) return false;
 for(;;*K=(*K)->Sled){
  if(!wcscmp((*K)->V->Name,S)) return true;
  if(!((*K)->Sled)) return false;
}}
//---------------------------------------------------------------------------
Cord *FindCord(Cord *K,wchar_t *S){
 for(;K;K=K->Sled) if(!wcscmp(K->V->Name,S)) return K; return NULL;
}
//---------------------------------------------------------------------------
Cord *FindCord(Vary *V,Cord *K){
 for(;K;K=K->Sled) if(K->V==V) return K; return NULL;
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
void DelCord(Cord **R,Cord *q){
 Cord *k;
 if(!(k=*R)) return;
 if(!wcscmp(k->V->Name,q->V->Name)){ *R=(*R)->Sled; free(k); return; }
 for(;k->Sled;k=k->Sled){
  if(!wcscmp(k->Sled->V->Name,q->V->Name)){
   q=k->Sled; k->Sled=q->Sled; free(q); return;
}}}
//---------------------------------------------------------------------------
Cord *TakeVeloCord(Vary *V){
 Cord *S,*C=(Cord *)calloc(1,sizeof(Cord));
 if(!C){ IsError=true; return NULL; }
 C->V=V; S=L->VK;
 if(!S) L->VK=C;
 else { for(;S->Sled;S=S->Sled); S->Sled=C; }
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
int IndexZ(wchar_t *NameV){ Cord *z; int i=0;
 if(!(z=L->Z)) return -1;
 for(;;z=z->Sled,i++){
  if(!wcscmp(z->V->Name,NameV)) return i;
}}
//---------------------------------------------------------------------------
bool FindZavC(wchar_t *S,Cord **Z){
 if(!(*Z=L->Z)) return false;
 for(;;*Z=(*Z)->Sled){
  if(!wcscmp((*Z)->V->Name,S)) return true;
  if(!((*Z)->Sled)) return false;
}}
//---------------------------------------------------------------------------
Cord *TakeZavC(Vary *V){
 Cord *Z,*C=(Cord *)calloc(1,sizeof(Cord));
 if(!C){ IsError=true; return NULL; }
 C->V=V;
 if(!L->Z) L->Z=C;
 else { for(Z=L->Z;Z->Sled;Z=Z->Sled); Z->Sled=C; }
 return C;
}
//---------------------------------------------------------------------------
int IndexW(wchar_t *NameV){ Cord *w; int i=0;
 if(!(w=L->W)) return -1;
 for(;;w=w->Sled,i++){
  if(!wcscmp(w->V->Name,NameV)) return i;
}}
//---------------------------------------------------------------------------
bool FindPsevdo(wchar_t *S,Cord **W){
 if(!(*W=L->W)) return false;
 for(;;*W=(*W)->Sled){
  if(!wcscmp((*W)->V->Name,S)) return true;
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
bool FindBody(wchar_t *S,Body **B){
 if(!(*B=L->B)) return false;
 for(;;*B=(*B)->Sled){
  if(!wcscmp((*B)->Name,S)) return true;
  if(!((*B)->Sled)) return false;
}}
//---------------------------------------------------------------------------
Body *TakeBody(wchar_t *S,wchar_t *s){
 Body *B,*b; wchar_t Buf[256];
 if(s&&(*s=='~')) *s++='\0';
 CleanStr(S,Buf);
 if(FindBody(Buf,&B)){
  if(!B->Line) B->Line=Line;
  if(B->BaseName){
   free(B->BaseName); B->BaseName=NULL;
  }
  if(B->KoordSys){
   for(Parm *P=B->KoordSys->R;P;P=P->Sled){
	if(P->Nam){ free(P->Nam); P->Nam=NULL; }
	if(P->R){ DelAllList((void **)(P+SzV)); P->R=NULL; }
   }
   if(B->KoordSys->R){
	DelAllList((void **)(&(B->KoordSys->R)));
	B->KoordSys->R=NULL;
   }
   DelAllList((void **)&(B->KoordSys)); B->KoordSys=NULL;
  }
  if(B->Inert){
   for(Parm *P=B->Inert->R;P;P=P->Sled){
	if(P->Nam){ free(P->Nam); P->Nam=NULL; }
	if(P->R){ DelAllList((void **)(P+SzV)); P->R=NULL; }
   }
   if(B->Inert->R){
	DelAllList((void **)&(B->Inert->R));
	B->Inert->R=NULL;
   }
   DelAllList((void **)&(B->Inert)); B->Inert=NULL;
  }
  if(s){
   CleanStr(s,Buf);
   B->BaseName=(wchar_t *)malloc(SzC*(wcslen(Buf)+1));
   wcscpy(B->BaseName,Buf);
  }
  return B;
 }
 if(b=(Body *)calloc(1,sizeof(Body))){
  b->Line=Line; b->Kx.C=b->Ky.C=b->Kz.C=Nul;
  b->m.C=b->Jx.C=b->Jy.C=b->Jz.C=b->Jxy.C=b->Jyz.C=b->Jxz.C=Nul;
  for(int i=0;i<3;i++){
   b->rOk[i].C=b->VOk[i].C=b->AOk[i].C=b->vOk[i].C=b->aOk[i].C=
   b->rCk[i].C=b->VCk[i].C=b->ACk[i].C=b->vCk[i].C=b->aCk[i].C=
   b->wkk[i].C=b->ekk[i].C=Nul;
   for(int j=0;j<3;j++)
	b->Sk0[i][j].C=i==j?Odin:Nul;
  }
  if(b->Name=(wchar_t *)malloc(SzC*(wcslen(Buf)+1))){
   wcscpy(b->Name,Buf);
   if(s){
	CleanStr(s,Buf);
	b->BaseName=(wchar_t *)malloc(SzC*(wcslen(Buf)+1));
	wcscpy(b->BaseName,Buf);
  }}
  return (B?B->Sled:L->B)=b;
 }
 IsError=true; return NULL;
}
//---------------------------------------------------------------------------
bool FindStringer(wchar_t *S,Stringer **B){
 if(!(*B=L->T)) return false;
 for(;;*B=(*B)->Sled){
  if(!wcscmp((*B)->Name,S)) return true;
  if(!((*B)->Sled)) return false;
}}
//---------------------------------------------------------------------------
Stringer *TakeStringer(wchar_t *S){
 Stringer *T,*t; wchar_t Buf[256];
 CleanStr(S,Buf);
 if(FindStringer(Buf,&T)){ if(!T->Line) T->Line=Line; return T; }
 if(t=(Stringer *)calloc(1,sizeof(Stringer))){
  t->Line=Line; t->Cix.C=t->Ciy.C=t->Ck.C=t->Cx.C=t->Cy.C=t->Cz.C=Nul;
  t->FE.C=t->FG.C=t->FL.C=t->FF.C=t->FJx.C=t->FJy.C=t->FJz.C=Pust;
  t->dA.C=t->dB.C=t->dG.C=t->dX.C=t->dY.C=t->dZ.C=Pust;
  t->X1.C=t->X2.C=t->Y1.C=t->Y2.C=t->Z1.C=Pust=t->Z2.C=Pust;
  t->Name=wcscpy((wchar_t *)calloc(wcslen(Buf)+1,SzC),Buf);
  return (T?T->Sled:L->T)=t;
 }
 IsError=true; return NULL;
}
//---------------------------------------------------------------------------
bool FindUnkn(wchar_t *S){ Cord *C;
 if(!(C=L->P)) return false;
 for(;;C=C->Sled){
  if(!wcscmp(C->V->Name,S)) return true; if(!(C->Sled)) return false;
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
 Cord *P,*I=(Cord *)calloc(1,sizeof(Cord)),
  *i=(Cord *)calloc(1,sizeof(Cord));
 if(!I||!i){ IsError=true; return NULL; }
 I->V=V; i->V=v;
 if(!(P=L->R)) L->R=I; else { for(;P->Sled;P=P->Sled); P->Sled=I; }
 if(!(P=L->N)) L->N=i; else { for(;P->Sled;P=P->Sled); P->Sled=i; }
 return I;
}
//---------------------------------------------------------------------------
void DelVary(wchar_t *S){
 for(Vary *V=L->V;V&&V->Sled;V=V->Sled){
  if(!wcscmp(V->Sled->Name,S)){
   Vary *v=V->Sled; V->Sled=v->Sled; free(v->Name); free(v);
}}}
//---------------------------------------------------------------------------
/*
bool ArrI::Cmp(Arra *a,Form Ind){
 if(A!=a) return true;
 if(FInd!=Ind) return true;
 if(FInd.C->Atr!=CNST||Ind.C->Atr!=CNST||FInd.C->Val!=Ind.C->Val) return true;
 return false;
}
//---------------------------------------------------------------------------
bool FindArrI(Arra *A,Form F,ArrI **a){
 if(!(*a=L->a)) return false;
 for(;;*a=(*a)->Sled){
  if(!(*a)->Cmp(A,F)) return true;
  if(!((*a)->Sled)) return false;
}}
//---------------------------------------------------------------------------
ArrI *TakeArrI(Arra *A,Form F){ ArrI *a,*b;
 if(FindArrI(A,F,&a)) return a;
 b=(ArrI *)calloc(1,sizeof(ArrI));
 if(!b){ IsError=true; return NULL; }
 b->A=A; b->Atr=ARRI; b->FInd=F;
 return (a?a->Sled:L->a)=b;
}*/
//---------------------------------------------------------------------------
bool FindVary(wchar_t *S,Vary **V){
 Compress(S);
 if(!S||!*S){ *V=NULL; return false; }
 if(L->Time&&!wcscmp(L->Time->Name,S)){ *V=L->Time; return true; }
 if(L->Freq&&!wcscmp(L->Freq->Name,S)){ *V=L->Freq; return true; }
 if(!(*V=L->V)) return false;
 for(;;*V=(*V)->Sled){
  if(!wcscmp((*V)->Name,S)) return true;
  if(!(*V)->Sled) return false;
}}
//---------------------------------------------------------------------------
Vary *TakeVary(wchar_t *S){ Vary *V;
 if(!S||!*S) return NULL;
//if(!wcscmp(S,L"E"))
//V=NULL;
 Compress(S);
 if(FindVary(S,&V)) return V;
 Vary *W=(Vary *)calloc(1,sizeof(Vary));
 if(!W){ IsError=true; return NULL; }
 W->Name=(wchar_t *)malloc(SzC*(wcslen(S)+1));
 W->Razm.C=Pust;  W->Krazm=1.0;
 if(!(W->Name)){ IsError=true; return NULL; }
 wcscpy(W->Name,S); W->Atr=VARY; W->P=W->Q=1; W->U=0; W->Znach.C=Pust;

 return (V?V->Sled:L->V)=W;
}
//---------------------------------------------------------------------------
bool FindVary(String S,Vary **V){ return FindVary(S.c_str(),V); }
//---------------------------------------------------------------------------
Vary *TakeVary(String S){ return TakeVary(S.c_str()); }
//---------------------------------------------------------------------------
bool FindCloseElem(wchar_t *S,Elem **E){
 if(!(*E=L->E)) return false;
 for(;;*E=(*E)->Sled){
  if(((*E)->Name[0]=='@')&&!wcscmp((*E)->Name+1,S)) return true;
  if(!((*E)->Sled)) return false;
}}
//---------------------------------------------------------------------------
bool FindElem(wchar_t *S,Elem **E){
 if(!(*E=L->E)) return false;
 for(;;*E=(*E)->Sled){
  if(!wcscmp((*E)->Name,S)) return true;
  if(!((*E)->Sled)) return false;
}}
//---------------------------------------------------------------------------
bool FindKoordElem(wchar_t *Koord,Elem **E){
 if(!(*E=L->E)) return false;
 for(;;*E=(*E)->Sled){
  if(wcschr((*E)->Name,AtrNelin)&&!wcscmp((*E)->K->Name,Koord)) return true;
  if(!((*E)->Sled)) return false;
}}
//---------------------------------------------------------------------------
Elem *TakeElem(wchar_t *S){ Elem *E;
 if(FindElem(S,&E)){ if(!E->Line) E->Line=Line; return E; }
 Elem *e=(Elem *)calloc(1,sizeof(Elem));
 if(!e){ IsError=true; return NULL; }
 e->Name=(wchar_t *)malloc(SzC*(wcslen(S)+1));
 if(!(e->Name)){ IsError=true; return NULL; }
 wcscpy(e->Name,S); e->Line=Line; e->Znach.C=Pust;
 return (E?E->Sled:L->E)=e;
}
//---------------------------------------------------------------------------
bool FindArra(wchar_t *S,Arra **A){
 if(!(*A=L->A)) return false;
 for(;;*A=(*A)->Sled){
  if(!wcscmp((*A)->Name,S)) return true;
  if(!((*A)->Sled)) return false;
}}
//---------------------------------------------------------------------------
Arra *TakeArra(wchar_t *S){ Arra *A;
 if(FindArra(S,&A)){ if(!A->Line) A->Line=Line; return A; }
 Arra *a=(Arra *)calloc(1,sizeof(Arra)); a->Line=Line;
 if(!a){ IsError=true; return NULL; }
 a->Name=(wchar_t *)malloc(SzC*(wcslen(S)+1)); wcscpy(a->Name,S);
 a->Znach.C=Pust; a->Atr=ARRA;
 if(!(a->Name)){ IsError=true; return NULL; }
 return (A?A->Sled:L->A)=a;
}
//---------------------------------------------------------------------------
bool FindArrI(Arra *A,Form Find,ArrI **a){
 if(!(*a=L->a)) return false;
 for(;;*a=(*a)->Sled){
  if(((*a)->A==A)&&((*a)->FInd==Find)) return true;
  if(!((*a)->Sled)) return false;
}}
//---------------------------------------------------------------------------
ArrI *TakeArrI(Arra *A,Form Find){ ArrI *a;
 if(FindArrI(A,Find,&a)){ if(!a->Line) a->Line=Line; return a; }
 ArrI *ai=(ArrI *)calloc(1,sizeof(ArrI)); ai->Line=Line;
 if(!ai){ IsError=true; return NULL; }
 ai->Atr=ARRI; ai->A=A; ai->FInd=Find;
 return (a?a->Sled:L->a)=ai;
}
//---------------------------------------------------------------------------
Arra *FindArraLast(wchar_t *S){ Arra *Rez=NULL;
 for(Arra *A=L->A;A;A=A->Sled){
  LowToUpp(A->Name); if(!wcscmp(A->Name,S)) Rez=A;
 }
 return Rez;
}
//---------------------------------------------------------------------------
Root *FindInstLast(wchar_t *S){ Root *Rez=NULL;
 for(Root *I=L->Inst;I;I=I->Sled){
  LowToUpp(I->Name); if(!wcscmp(I->Name,S)) Rez=I;
 }
 return Rez;
}
//---------------------------------------------------------------------------
Root *FindOperLast(wchar_t *S){ Root *Rez=NULL;
 for(Root *O=L->Oper;O;O=O->Sled){
  LowToUpp(O->Name); if(!wcscmp(O->Name,S)) Rez=O;
 }
 return Rez;
}
//---------------------------------------------------------------------------
bool FindInst(wchar_t *S,Root **I){
 if(!(*I=L->Inst)) return false;
 for(;;*I=(*I)->Sled){
  LowToUpp((*I)->Name);
  if(!wcscmp((*I)->Name,S)) return true;
  if(!((*I)->Sled)) return false;
}}
//---------------------------------------------------------------------------
Root *TakeInst(wchar_t *S){ Root *I;
 if(FindInst(S,&I)) return I;
 Root *i=(Root *)calloc(1,sizeof(Root));
 if(!i){ IsError=true; return NULL; }
 i->Name=(wchar_t *)malloc(SzC*(wcslen(S)+1));
 if(!(i->Name)){ IsError=true; return NULL; }
 wcscpy(i->Name,S);
 return (I?I->Sled:L->Inst)=i;
}
//---------------------------------------------------------------------------
bool FindOper(wchar_t *S,Root **O){
 if(!(*O=L->Oper)) return false;
 for(;;*O=(*O)->Sled){
  LowToUpp((*O)->Name);
  if(!wcscmp((*O)->Name,S)) return true;
  if(!((*O)->Sled)) return false;
}}
//---------------------------------------------------------------------------
Root *TakeOper(wchar_t *S){ Root *O;
 Root *o=(Root *)calloc(1,sizeof(Root));
 if(!o){ IsError=true; return NULL; }
 o->Name=(wchar_t *)malloc(SzC*(wcslen(S)+1));
 if(!(o->Name)){ IsError=true; return NULL; }
 wcscpy(o->Name,S); O=L->Oper;
 if(O){ while(O->Sled) O=O->Sled; return O->Sled=o; }
 else return L->Oper=o;
}
//---------------------------------------------------------------------------
/*bool FindCnst(wchar_t *S,Cnst **C){
 Compress(S);
 for(*C=Pust;;*C=(*C)->Sled){
  if((*C)->Name&&!wcscmp((*C)->Name,S)) return true;
  if(!((*C)->Sled)) return false;
}}
//---------------------------------------------------------------------------
Cnst *TakeCnst(wchar_t *S){ Cnst *C,*c;
 Compress(S);
 if(isalpha(S[0])||isciril(S[0])){
  if(FindCnst(S,&C))
   return C;
  c=(Cnst *)calloc(1,sizeof(Cnst));
  c->Name=wcscpy((wchar_t *)malloc(SzC*(wcslen(S)+1)),S);
 }
 else{
  double D=S?_wtof(S):0.0;
  if(FindCnst(D,&C)) return C;
  c=(Cnst *)calloc(1,sizeof(Cnst));
  c->Val=_wtof(S);
 }
 c->Atr=CNST; return C->Sled=c;
}
//---------------------------------------------------------------------------
bool FindCnst(double D,Cnst **C){
 for(*C=Pust->Sled;;*C=(*C)->Sled){
  if((*C)->Val==D) return true;
  if(!((*C)->Sled)) return false;
}}
//---------------------------------------------------------------------------
Cnst *TakeCnst(double D){ wchar_t S[80]; Cnst *C;
 if(FindCnst(D,&C)) return C;
 Cnst *c=(Cnst *)calloc(1,sizeof(Cnst));
 if(!c){ IsError=true; return NULL; }
 c->Val=D; c->Atr=CNST; return C->Sled=c;
}*/
//---------------------------------------------------------------------------
int FindCnst(wchar_t *S,Cnst **C){
//if(!wcscmp(S,L"pi"))
//wcscpy(S,L"pi");
 if(isalpha(*S)||isciril(*S)){
  for(*C=Pust->Sled;*C;*C=(*C)->Sled)
   if((*C)->Name&&!wcscmp((*C)->Name,S)) return 1;
  return 0;
 }
 else{
  double D=_wtof(S);
  return FindCnst(D,C);
}}
//---------------------------------------------------------------------------
int FindCnst(double D,Cnst **C){
 for(*C=Pust->Sled;;*C=(*C)->Sled){
  if((*C)->Val==D) return 1;
  if(!((*C)->Sled)) return 0;
  if((*C)->Sled->Val>D) return 0;
}}
//---------------------------------------------------------------------------
Cnst *TakeCnst(double D){
 Cnst *C,*c;
 if(FindCnst(D,&C)) return C;
 c=(Cnst *)calloc(1,sizeof(Cnst)); c->Atr=CNST; c->Val=D; c->Sled=C->Sled;
 return C->Sled=c;
}
//---------------------------------------------------------------------------
Cnst *TakeCnst(wchar_t *S){
 return TakeCnst(_wtof(S));
}
//---------------------------------------------------------------------------
Cnst *TakeCnst(wchar_t *S,double D){
 Cnst *C=TakeCnst(D);
 C->Name=wcscpy((wchar_t *)calloc(wcslen(S)+1,sizeof(wchar_t)),S);
 return C;
}
//---------------------------------------------------------------------------
bool FindList(wchar_t *S,List **L){
 if(!(*L=LBeg)) return false;
 for(;;*L=(*L)->Sled){
  if(wcscmp((*L)->Name,S)==0) return true; if(!((*L)->Sled)) return false;
}}
//---------------------------------------------------------------------------
List *TakeList(wchar_t *S){
 List *l,*lo; Cord *r;
 if(FindList(S,&L)) return L;
 l=(List *)calloc(1,sizeof(List));
 if(!l){ IsError=true; return NULL; }
 l->Name=(wchar_t *)malloc(SzC*(wcslen(S)+1));
 if(!(l->Name)){ IsError=true; return NULL; }
 (L?L->Sled:LBeg)=l; L=LBeg;
 wcscpy(l->Name,S);
 if(!wcscmp(S,L"ОСНОВНОЙ")){
  l->Time=TakeVary(L"t"); l->Freq=TakeVary(L"w");
  l->V=TakeVary(L"g"); l->V->P=0; l->V->Val=9.8062;
  l->V->Znach.C=TakeCnst(9.8062);
 }
 else{
  if(!wcscmp(S,L"РАЗМЕРНОСТИ")){ double D=M_PI/180.0;
   Vary *rad,*degree,*c,*min,*hour,*day,*mounth,*year,
	*m,*cm,*km,*mm,*mile,*dm,*duim,
	*kg,*gram,*ton,
	*dj,*n;
   Form Fc,Fc2,Fm,Frad,Fdegree,Fkg,Fcm,Fgram,Fmin,Fhour,Fday,
	Fmounth,Fyear,Fkm,Fmm,Fmile,Fdm,Fduim,Fton,FDj,FN;
   L=l;
   c=TakeVary(L"с"); min=TakeVary(L"мин"); hour=TakeVary(L"час");
   day=TakeVary(L"день"); mounth=TakeVary(L"месяц");
   year=TakeVary(L"год");
   m=TakeVary(L"м"); cm=TakeVary(L"см"); km=TakeVary(L"км"); mm=TakeVary(L"мм");
   mile=TakeVary(L"миля"); dm=TakeVary(L"дм"); duim=TakeVary(L"дюйм");
   rad=TakeVary(L"рад"); degree=TakeVary(L"°");
   kg=TakeVary(L"кг"); gram=TakeVary(L"г"); ton=TakeVary(L"т");
   dj=TakeVary(L"Дж"); n=TakeVary(L"Н");
   Fc.V=c; Fmin.V=min; Fhour.V=hour; Fday.V=day; Fmounth.V=mounth; Fyear.V=year;
   Fm.V=m; Fcm.V=cm; Fkm.V=km; Fmile.V=mile; Fmm.V=mm; Fdm.V=dm; Fduim.V=duim;
   Fgram.V=gram; Fton.V=ton; Fkg.V=kg;
   Fc2=Fc*Fc; Frad.V=rad; Fdegree.V=degree;
   FN=Fkg*Fm/(Fc*Fc); FDj=FN*Fm;
   kg->Znach.C=Odin; m->Znach.C=Odin; c->Znach.C=Odin;
   rad->Znach.C=Odin;
   min->Znach=60*Fc; hour->Znach=360*Fc; day->Znach=86400*Fc;
   mounth->Znach=2592000*Fc; year->Znach=31104000*Fc;
   ton->Znach=1000*Fkg; gram->Znach=0.001*Fkg;
   km->Znach=1000*Fm; mm->Znach=0.001*Fm; mile->Znach=1852*Fm;
   duim->Znach=0.0254*Fm; cm->Znach=0.01*Fm; dm->Znach=0.1*Fm;
   dj->Znach=FDj; n->Znach=FN; degree->Znach=D*Frad;
   if(FindList(L"ОСНОВНОЙ",&L)){
	L->V->Razm=Fm/Fc2;
	L->Time->Razm=Fc;
	L->Freq->Razm=Frad/Fc;
   }
   Rm=(Cord *)calloc(3,sizeof(Cord));
   Rl=(Cord *)calloc(7,sizeof(Cord));
   Rt=(Cord *)calloc(6,sizeof(Cord));
   Ra=(Cord *)calloc(2,sizeof(Cord));

   r=Rm; for(int i=0;i<2;i++,r=r->Sled) r->Sled=r+1;
   Rm[0].V=kg; Rm[1].V=gram; Rm[2].V=ton;

   r=Rl; for(int i=0;i<6;i++,r=r->Sled) r->Sled=r+1;
   Rl[0].V=m;    Rl[1].V=cm; Rl[2].V=km;   Rl[3].V=mm;
   Rl[4].V=mile; Rl[5].V=dm; Rl[6].V=duim;

   r=Rt; for(int i=0;i<5;i++,r=r->Sled) r->Sled=r+1;
   Rt[0].V=c;   Rt[1].V=min;    Rt[2].V=hour;
   Rt[3].V=day; Rt[4].V=mounth; Rt[5].V=year;

   r=Ra; r->Sled=r+1;  Ra[0].V=rad; Ra[1].V=degree;
 }}
 return l;
}
//---------------------------------------------------------------------------
int TypOfRazm(Vary *V){
 for(Cord *r=Rm;r;r=r->Sled)
  if(!wcscmp(V->Name,r->V->Name)) return MASSA;
 for(Cord *r=Rl;r;r=r->Sled)
  if(!wcscmp(V->Name,r->V->Name)) return LENGTH;
 for(Cord *r=Rt;r;r=r->Sled)
  if(!wcscmp(V->Name,r->V->Name)) return TIME;
 return 0;
}
//---------------------------------------------------------------------------
void CalcKRazm(Vary *V){//Вычисляем коэффициент размерности
//для переменной из исходных данных
 if(V->Razm.C!=Pust){
  Cord *Nm=NULL,*Dn=NULL; double K=1.0;
  List *Lo=L; Vary *v; wchar_t *S;
  L=TakeList(L"РАЗМЕРНОСТИ");
  FormTo2List(V->Razm,&Nm,&Dn);
  for(Cord *q=Nm;q;q=q->Sled){
   switch(TypOfRazm(q->V)){
	case MASSA:
	 S=MUnit->S;
	 break;
	case LENGTH:
	 S=MUnit->Sled->S;
	 break;
	case TIME:
	 S=MUnit->Sled->Sled->S;
	 break;
   }
   v=TakeVary(S);
   K*=Val(q->V->Znach)/Val(v->Znach);
  }
  for(Cord *q=Dn;q;q=q->Sled){
   switch(TypOfRazm(q->V)){
	case MASSA:
	 S=MUnit->S;
	 break;
	case LENGTH:
	 S=MUnit->Sled->S;
	 break;
	case TIME:
	 S=MUnit->Sled->Sled->S;
	 break;
   }
   v=TakeVary(S);
   K*=Val(v->Znach)/Val(q->V->Znach);
  }
  L=Lo;
  V->Krazm=K;
}}
//---------------------------------------------------------------------------
void ConvOldRazm(Vary *V){
 if(V->Razm.C!=Pust){
  Cord *Nm=NULL,*Dn=NULL;
  Form NumRazm,DenRazm,F; NumRazm.C=DenRazm.C=Odin;
  List *Lo=L; Vary *v;
  wchar_t *S;
  L=TakeList(L"РАЗМЕРНОСТИ");
  FormTo2List(V->Razm,&Nm,&Dn);
  for(Cord *q=Nm;q;q=q->Sled){
   switch(TypOfRazm(q->V)){
	case MASSA:
	 S=MUnit->S;
	 break;
	case LENGTH:
	 S=MUnit->Sled->S;
	 break;
	case TIME:
	 S=MUnit->Sled->Sled->S;
	 break;
   }
   v=TakeVary(S); F.V=v;
   NumRazm=NumRazm*F;
  }
  for(Cord *q=Dn;q;q=q->Sled){
   switch(TypOfRazm(q->V)){
	case MASSA:
	 S=MUnit->S;
	 break;
	case LENGTH:
	 S=MUnit->Sled->S;
	 break;
	case TIME:
	 S=MUnit->Sled->Sled->S;
	 break;
   }
   v=TakeVary(S); F.V=v;
   DenRazm=DenRazm*F;
  }
  L=Lo;
  V->Razm=NumRazm/DenRazm;
}}
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
 g->Atr=IFOP; g->Oper=Oper; g->U=U; g->V=V; g->FT=FT; g->FF=FF; g->UT.C=Pust;
 if((U.C->Atr!=VARY)&&(Oper>6)){ Cord *K;
  swprintf(Inf,
   L"При использовании логических операций типа \'<<\' (или \'>>\')\n"
   L"в данной версии ССКА КиДиМ допускается использовать перед этим\n"
   L"знаком только переменную",Line);
  Application->MessageBox(Inf,L"* ОШИБКА! *",MB_OK);
  IsError=true; return NULL;
 }
 return g;
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
   if((P->A==A)&&(P->Type==Typ)/*&&!P->a&&!P->b&&!P->c&&!P->d*/)
    P->CalcSpln();
}}
//---------------------------------------------------------------------------
bool FindSpln(Spln *R,wchar_t *S,Spln **P){
 if(!(*P=R)) return 0;
 if(wcscmp(S,R->A->Name)<0){ *P=NULL; return 0; }
 for(;;*P=(*P)->Sled){
  if(!wcscmp((*P)->A->Name,S)) return 1;
  if(!((*P)->Sled)) return 0;
  if(wcscmp((*P)->Sled->A->Name,S)>0) return 0;
}}
//---------------------------------------------------------------------------
Spln *TakeSpln(Spln **R,wchar_t *S,Arra *A){
 Spln *P,*p; if(!S) return NULL;
 if(FindSpln(*R,S,&P)) p=P;
 else{
  p=(Spln *)calloc(1,sizeof(Spln));
  if(!P){ p->Sled=*R;           *R=p; }
  else  { p->Sled=P->Sled; P->Sled=p; }
 }
 p->A=A; p->A->Name=wcscpy((wchar_t *)calloc(wcslen(S)+1,SzC),S);
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
 double *X=(double *)calloc(4*N,SzD),//массив неизвестных a,b,c,d .
  *B=(double *)calloc(N,SzD),//главная диагональ трехдиагональной матрицы
  *C=(double *)calloc(N,SzD),//нижняя поддиагональ трехдиагональной матрицы
  *D=(double *)calloc(N,SzD),hi;//массив правых частей
 if(!B||!C||!D||!X){
  swprintf(Inf,L"Недостаточно памяти для размещения сплайна \"%s\".",A->Name);
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
  swprintf(Inf,L"Система уравнений несовместна для сплайна \"%s\"!",A->Name);
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
 if(FindSpln(Typ,A,Arg,&P)){
  P->A=A;
  if(A->N)
   P->CalcSpln();
   return P;
 }
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
Summ *FindSumm(Form F,Vary *i,Form a,Form b,int Type){ Summ *M;
 for(M=L->M;M;M=M->Sled)
  if((M->F==F)&&(M->i==i)&&(M->a==a)&&(M->b==b)&&(M->Type==Type)) break;
 return M;
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
 if((I=FindInstLast(L"НЕИЗВЕСТНЫЕ"))||(I=FindInstLast(L"НЕИЗВЕСТНОЕ"))||
    (I=FindInstLast(L"НЕИЗВЕСТНАЯ"))){
  for(Parm *p=I->First;p;p=p->Sled) if(!wcscmp(p->Nam,V->Name)) return true;
 }
 return false;
}
//---------------------------------------------------------------------------
Vary *Recur(Vary *V){ Root *I; Vary *v;//возвращаем аргумент
 if(I=FindInstLast(L"НОВЫЕ ЗНАЧЕНИЯ")){
  for(Parm *p=I->First;p;p=p->Sled)
   if(!wcscmp(p->Nam,V->Name)&&FindVary(p->R->Nam,&v)) return v;
 }
 return NULL;
}
//---------------------------------------------------------------------------
bool NoDefine(Vary *V){ wchar_t *s,*s1;
 if(V->Znach!=Pust) return false;
 if(s=wcschr(V->Name,'\'')){
  if(!wcscmp(++s,L->Time->Name)) return false;
  if(s1=wcschr(s,'\'')){ *s1='\0';
   if((!wcscmp(s,L->Time->Name))&&(!wcscmp(s1+1,L->Time->Name))){
    *s1='\''; return false;
 }}}
 return true;
}
//---------------------------------------------------------------------------
void DifTime1(Vary *V){
 Form F,Fv,Fw; Vary *K; wchar_t NameV[256],*s,qs2t[80],qst[256];
 wcscpy(NameV,V->Name); s=wcschr(NameV,'\''); *s='\0';
 K=TakeVary(NameV);
 F=DifBy(K->Znach,L->Time);
 for(Cord *q=L->K;q;q=q->Sled){
  swprintf(qst,L"%s'%s",q->V->Name,L->Time->Name); Fv.V=TakeVary(qst);
  swprintf(qs2t,L"%s'%s",Fv.V->Name,L->Time->Name); Fw.V=TakeVary(qs2t);
  F=F+DifBy(K->Znach,q->V)*Fv+DifBy(K->Znach,Fv.V)*Fw;
 }
 V->Znach=F;
 if((V->Razm.C==Pust)&&(K->Razm.C!=Pust)){
  List *Lo=L;  Form Razm; Vary *Vc;
  L=TakeList(L"РАЗМЕРНОСТИ");
  Vc=TakeVary(L"с"); Razm.V=Vc;
  V->Razm=K->Razm/Razm;
  L=Lo;
}}
//---------------------------------------------------------------------------
void DifTime2(Vary *V){
 Form F,Fv,Fw; Vary *K; wchar_t *s=wcsrchr(V->Name,'\''),qs2t[80],qst[80];
 *s='\0'; K=TakeVary(V->Name); *s='\'';
 if(K->Znach==Pust) DifTime1(K);
 F=DifBy(K->Znach,L->Time);
 for(Cord *q=L->K;q;q=q->Sled){
  swprintf( qst,L"%s'%s",q->V->Name,L->Time->Name); Fv.V=TakeVary( qst);
  swprintf(qs2t,L"%s'%s",Fv.V->Name,L->Time->Name); Fw.V=TakeVary(qs2t);
  F=F+DifBy(K->Znach,q->V)*Fv+DifBy(K->Znach,Fv.V)*Fw;
 }
 V->Znach=F;
 if((V->Razm.C==Pust)&&(K->Razm.C!=Pust)){
  List *Lo=L;  Form Razm; Vary *Vc;
  L=TakeList(L"РАЗМЕРНОСТИ");
  Vc=TakeVary(L"с"); Razm.V=Vc;
  V->Razm=K->Razm/(Razm*Razm);
  L=Lo;
}}
//---------------------------------------------------------------------------
bool Coord(wchar_t *NameV){
 for(Cord *K=L->K;K;K=K->Sled)
  if(!wcscmp(K->V->Name,NameV)) return true;
 return false;
}
//---------------------------------------------------------------------------
void TakeDifVary(TCGauge *CGauge,TStatusBar *StatusBar){
// Сделаем производные не по времени
 wchar_t *s,*ss,NameV[256],Time1[256],Time2[256],SBold[256]; Vary *v,*vv;
 bool IsV,Isv; int NoTime1,NoTime2; FILE *F;
 swprintf(Time2,L"%s'%s",L->Time->Name,L->Time->Name);
 swprintf(Time1,L"%s",L->Time->Name);  int i=0;
 if(StatusBar){
  wcscpy(SBold,StatusBar->SimpleText.c_str());
  StatusBar->SimpleText=L"Создание производных не по времени";
 }
 if(CGauge){
  CGauge->Progress=0;
  for(Vary *V=L->V;V;V=V->Sled) i++;
  CGauge->MaxValue=i; i=0;
 }
 for(Vary *V=L->V;V;i++,V=V->Sled){
  if(CGauge) CGauge->Progress++;
  if(V->Znach.C->Atr==DIFF){
   V->Znach=DifBy(V->Znach.D->Y->Znach,V->Znach.D->X);
   continue;
  }
  wcscpy(NameV,V->Name); s=wcschr(NameV,'\'');
  while(s){
   *s='\0';
   NoTime1=wcscmp(s+1,Time1); NoTime2=wcscmp(s+1,Time2);
   if(NoTime1&&NoTime2){
    IsV=FindVary(NameV,&v); Isv=FindVary(s+1,&vv);
    if(IsV&&Isv&&(V->Znach==Pust)){
     V->Znach=DifBy(v->Znach,vv); s=NULL;
    }
    else{
     *s='\'';
     s=wcschr(s+1,'\'');
   }}
   else s=NULL;
 }}
 StatusBar->SimpleText=SBold;
}
//---------------------------------------------------------------------------
void PrintVar(Vary *V){
 FILE *Fo=_wfopen(L"OtladkaV.txt",L"a");
 PrintForm(true,Fo,V->Name,V->Znach);
 fputwc('\n',Fo);
 fclose(Fo);
}
//---------------------------------------------------------------------------
void TakeDifTime(TCGauge *CGauge,TStatusBar *StatusBar,bool IsDifCoord){
 wchar_t *s,Time1[256],Time2[256],SBold[256]; Root *I; int i=0; Vary *V;
// Сделаем производные по времени
//IsDifCoord для кинетостатики должно быть равно true
 swprintf(Time2,L"'%s'%s",L->Time->Name,L->Time->Name);
 swprintf(Time1,L"'%s",L->Time->Name);
 if(StatusBar){
  wcscpy(SBold,StatusBar->SimpleText.c_str());
  StatusBar->SimpleText=L"Создание производных по времени";
 }
 if(CGauge){
  CGauge->Progress=0;
  CGauge->MaxValue=KolElem(L->V); 
 }
 for(V=L->V;V;i++,V=V->Sled){
  if(CGauge) CGauge->Progress++;
  if(!(s=wcschr(V->Name,'\''))) continue;
  if(V->Znach.C!=Pust) continue;
  if(!wcscmp(s,Time2)){
   *s='\0';
   if(!IsDifCoord&&L->K&&Coord(V->Name)){ *s='\''; continue; }
   *s='\'';
   DifTime2(V);
  }
  else if(!wcscmp(s,Time1)){
   if(L->K){
    *s='\0';
    if(!IsDifCoord&&Coord(V->Name)){ *s='\''; continue; }
    *s='\'';
    DifTime1(V);
   }
   else{
    Vary *VV;
    *s='\0'; VV=TakeVary(V->Name); *s='\'';
    DifByT(VV);
 }}}
 if(StatusBar) StatusBar->SimpleText=SBold;
 if(CGauge) CGauge->Progress=0;
 if((I=FindInstLast(L"ПОКАЗАТЬ"))||(I=FindInstLast(L"ПЕЧАТАТЬ"))){
  for(Parm *P=I->First;P;P=P->Sled){
   if(!P->Nam||!(s=wcschr(P->Nam,'\''))) continue;
   if(FindVary(P->Nam,&V)&&(V->Znach!=Pust)) continue;
   if(s){ *s='\0'; if(!FindVary(P->Nam,&V)){ *s='\''; continue; } *s='\''; }
   if(!wcscmp(s,Time2)){
    *s='\0';
    if(!IsDifCoord&&L->K&&Coord(P->Nam)){ *s='\''; continue; }
    *s='\'';
    V=TakeVary(P->Nam);
    DifTime2(V);
   }
   else if(!wcscmp(s,Time1)){
    *s='\0';
    if(!IsDifCoord&&L->K&&Coord(P->Nam)){ *s='\''; continue; }
    *s='\'';
    V=TakeVary(P->Nam);
    DifTime1(V);
 }}}
 if(I=FindInstLast(L"СРАВНИТЬ")){
  for(Parm *P=I->First;P;P=P->Sled){
   if(!P->Nam||!(s=wcschr(P->Nam,'\''))||!FindVary(P->Nam,&V)||(V->Znach!=Pust)) continue;
   if(!wcscmp(s,Time2)){
    *s='\0';
    if(!IsDifCoord&&L->K&&Coord(P->Nam)){ *s='\''; continue; }
    *s='\'';
    DifTime2(V);
   }
   else if(!wcscmp(s,Time1)){
    *s='\0';
    if(!IsDifCoord&&L->K&&Coord(P->Nam)){ *s='\''; continue; }
    *s='\'';
    DifTime1(V);
 }}}
 if(I=FindInstLast(L"СОВМЕСТИТЬ")){
  for(Parm *P=I->First;P;P=P->Sled){
   if(!P->Nam||!(s=wcschr(P->Nam,'\''))||!FindVary(P->Nam,&V)||(V->Znach!=Pust)) continue;
   if(!wcscmp(s,Time2)){
    *s='\0';
    if(!IsDifCoord&&L->K&&Coord(P->Nam)){ *s='\''; continue; }
    *s='\'';
    DifTime2(V);
   }
   else if(!wcscmp(s,Time1)){
    *s='\0';
    if(!IsDifCoord&&L->K&&Coord(P->Nam)){ *s='\''; continue; }
    *s='\'';
    DifTime1(V);
 }}}
}
//---------------------------------------------------------------------------
bool NoVariation(Vary *V){
 Root *I; Vary *v;
 wchar_t *Instruct[]={L"ВАРЬИРОВАТЬ",L"ДИАПАЗОН ГАРМОНИК",L"ГАРМОНИКИ",L"ГАРМОНИКА",
  L"ДИАПАЗОН ЧАСТОТ",L"ЧАСТОТЫ",L"ЧАСТОТА",L"ДИАПАЗОН ОБОРОТОВ",L"ОБОРОТЫ",L"ОБОРОТ",
  L"ДИАПАЗОНЫ",L"ДИАПАЗОН"};
 for(int i=0;i<12;i++){
  if(I=FindInstLast(Instruct[i])){
   for(Parm *P=I->First;P;P=P->Sled){
    if(P->Nam&&!wcscmp(V->Name,P->Nam)) return false;
 }}}
 return true;
}
//---------------------------------------------------------------------------
bool PsevdoVelosity(Vary *V){
 Root *I; Vary *v;
 if(!(I=FindInstLast(L"ПСЕВДОСКОРОСТИ"))&&!(I=FindInstLast(L"КВАЗИСКОРОСТИ")))
  return false;
 for(Parm *P=I->First;P;P=P->Sled){
  if(P->Nam&&!wcscmp(V->Name,P->Nam)) return true;
 }
 return false;
}
//---------------------------------------------------------------------------
bool IsInInst(Vary *V){
 Root *I; Vary *v;
 if(!(I=FindInstLast(L"ОБОБЩЕННЫЕ КООРДИНАТЫ")))
  return false;
 for(Parm *P=I->First;P;P=P->Sled){
  if(P->Nam&&!wcscmp(V->Name,P->Nam)) return true;
 }
 return false;
}
//---------------------------------------------------------------------------
bool NoVelosity(Vary *V){//проверяем, не задана ли скорость переменной
 swprintf(Inf,L"%s'%s",V->Name,L->Time->Name);
 if(FindVary(Inf,&V)&&(V->Znach.C!=Pust))
  return false;
 return true;
}
//---------------------------------------------------------------------------
bool Delay(Vary *V){
//не яляется ли переменная сдвинутой по времени другой переменной
 wchar_t *s=wcschr(V->Name,'~'),*ss;
 if(s){
  *s='\0';
  for(Vary *v=L->V;v;v=v->Sled){
   if(!wcscmp(v->Name,s+1)){//
    if(ss=wcschr(V->Name,'\'')){
     if(!wcscmp(ss+1,L->Time->Name)){ *s='~'; TakeDelayV(V); }
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
 wchar_t *s=wcschr(V->Name,'\'');
 if(s){
  *s='\0';
  if(Coord(V->Name)&&!wcschr(s+1,'\'')){ *s='\''; return true; }
  *s='\''; return false;
 }
 return false;
}
//---------------------------------------------------------------------------
bool IsSumIndex(Vary *V){
 for(Summ *S=L->M;S;S=S->Sled) if(V==S->i) return true; return false;
}
bool IsIntgIndex(Vary *V){
 for(Intg *I=L->I;I;I=I->Sled) if(V==I->x) return true; return false;
}
//---------------------------------------------------------------------------
int Coord(List *L,TCGauge *CGauge,TStatusBar *StatusBar){
 int Nk=0; Vary *v,*V; Cord *K,*P,*Z,*W,*R,*N,*Dq,*Dv; Root *I;
 Magazine *Mu=NULL,*Mp=NULL,*Mc=NULL,*Mz=NULL,*m;
 NzK=NPV=0;
 while(K=L->K){ L->K=K->Sled; free(K); }
 while(P=L->P){ L->P=P->Sled; free(P); }
 while(Z=L->Z){ L->Z=Z->Sled; free(Z); }
 while(W=L->W){ L->W=W->Sled; free(W); }
 while(R=L->R){ L->R=R->Sled; free(R); }
 while(N=L->N){ L->N=N->Sled; free(N); }
 while(Dq=L->Dq){ L->Dq=Dq->Sled; free(Dq); }
 while(Dv=L->Dv){ L->Dv=Dv->Sled; free(Dv); }
 for(V=L->V;V;V=V->Sled){
  if(Delay(V)) continue;
  if(NoDefine(V)){
   if(IsSumIndex(V)) continue;
   if(IsIntgIndex(V)) continue;
   if(wcscmp(V->Name,L"_T"))
	if(wcscmp(V->Name,L"_П"))
	 if(wcscmp(V->Name,L"_dTpodt"))
	  if(NoVariation(V)){
	   if(NeIzv(V)) TakeMagazine(&Mu,V->Name);
	   else if(v=Recur(V)) TakeRecur(V,v);
			else if(PsevdoVelosity(V)){ NPV++; TakeMagazine(&Mp,V->Name); }
				 else if(NoVelosity(V)){ Nk++; TakeMagazine(&Mc,V->Name); }
					  else{ NzK++; TakeMagazine(&Mz,V->Name); }
	  }
 }}
 for(m=Mu;m;m=m->Sled){ FindVary(m->S,&V); TakeUnkn(V);   } DelAllMagazine(&Mu);
 for(m=Mp;m;m=m->Sled){ FindVary(m->S,&V); TakePsevdo(V); } DelAllMagazine(&Mp);
 for(m=Mc;m;m=m->Sled){ FindVary(m->S,&V); TakeCord(V);   } DelAllMagazine(&Mc);
 for(m=Mz;m;m=m->Sled){ FindVary(m->S,&V); TakeZavC(V);   } DelAllMagazine(&Mz);
 for(K=L->Dq;K;K=K->Sled){
  wchar_t *s=wcschr(K->V->Name,'~'); Vary *V;
  *s='\0';//если до '~' записана не обобщенная координата генерируем новую о.к.
  if(!Coord(K->V->Name)){ V=TakeVary(K->V->Name); Nk++; TakeCord(V); }
  *s='~';
 }
 for(K=L->Dv;K;K=K->Sled){
  wchar_t *s=wcschr(K->V->Name,'~'); Vary *V;
  *s='\0';
//если до '~' записана не обобщ. скорость и не псевдоскорость генерируем новую о.к.
  if(!GeneralVelosity(K->V)&&!PsevdoVelosity(K->V)){
   V=TakeVary(K->V->Name); Nk++; TakeCord(V);
  }
  *s='~';
 }
 TakeDifTime(CGauge,StatusBar,false);
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
  if(NoDefine(V)&&wcscmp(V->Name,L"_T")&&wcscmp(V->Name,L"_П")&&
     wcscmp(V->Name,L"_dTpodt")&&NoVariation(V)){
   if(NeIzv(V)) TakeUnkn(V);
   else if(v=Recur(V)) TakeRecur(V,v);
   else if(PsevdoVelosity(V)){ NPV++; TakePsevdo(V); }
   else if(NoVelosity(V)){ Nk++; TakeCord(V); }
   else{
    NzK++;
    TakeZavC(V);
 }}}
 for(K=L->Dq;K;K=K->Sled){
  wchar_t *s=wcschr(K->V->Name,'~'); Vary *V;
  *s='\0';//если до '~' записана не обобщенная координата генерируем новую о.к.
  if(!Coord(K->V->Name)){ V=TakeVary(K->V->Name); Nk++; TakeCord(V); }
  *s='~';
 }
 for(K=L->Dv;K;K=K->Sled){
  wchar_t *s=wcschr(K->V->Name,'~'); Vary *V;
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
int IndexOK(wchar_t *NameV){ int i=0;
 for(Cord *K=L->K;K;i++,K=K->Sled){
  if(!wcscmp(NameV,K->V->Name)) return i;
 }
 return -1;
}
//---------------------------------------------------------------------------
int IndexOV(wchar_t *NameV){ int i=0;
 for(Cord *K=L->K;K;i++,K=K->Sled){
  swprintf(Inf,L"%s'%s",K->V->Name,L->Time->Name);
  if(!wcscmp(NameV,Inf)) return i;
 }
 return -1;
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
 wchar_t S[256]; Vary *V;
 for(Cord *K=L->W;K;K=K->Sled){
  if(VarInForm(true,F,K->V)) return true;
  swprintf(S,L"%s'%s",K->V->Name,L->Time->Name); V=TakeVary(S);
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
bool VassalOfSumIndex(List *L,Form F){
 for(Summ *S=L->M;S;S=S->Sled){
  if(VarInForm(true,F,S->i)) return true;
 }
 return false;
}
//---------------------------------------------------------------------------
bool VassalOfIntgVary(List *L,Form F){
 for(Intg *I=L->I;I;I=I->Sled){
  if(VarInForm(true,F,I->x)) return true;
 }
 return false;
}
//---------------------------------------------------------------------------
bool VassalOfVelosity(List *L,Form F){
 wchar_t S[256]; Vary *V;
 for(Cord *K=L->K;K;K=K->Sled){
  swprintf(S,L"%s'%s",K->V->Name,L->Time->Name); V=TakeVary(S);
  if(VarInForm(true,F,V)) return true;
 }
 return false;
}
//---------------------------------------------------------------------------
bool VassalOfAccelerat(List *L,Form F){
 wchar_t S[256]; Vary *V;
 for(Cord *K=L->K;K;K=K->Sled){
  swprintf(S,L"%s'%s'%s",K->V->Name,L->Time->Name,L->Time->Name); V=TakeVary(S);
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
 Root  *I=FindInstLast(TextFromFile(WORKDIR,L"EditKiDyM",&SLK,173)); Vary *V;
 if(I){
  for(Parm *P=I->First;P;P=P->Sled){
   if(P->Nam&&FindVary(P->Nam,&V)&&VarInForm(true,F,V)) return true;
//   if(!P->R){ P=P->Sled->Sled; }
 }}
 I=FindInstLast(TextFromFile(WORKDIR,L"EditKiDyM",&SLK,175));
 if(I){
  for(Parm *P=I->First;P;P=P->Sled){
   if(P->Nam&&FindVary(P->Nam,&V)&&VarInForm(true,F,V)) return true;
   if(!P->R){ P=P->Sled->Sled; }
 }}
 I=FindInstLast(TextFromFile(WORKDIR,L"EditKiDyM",&SLK,176));
 if(I){
  for(Parm *P=I->First;P;P=P->Sled){
   if(P->Nam&&FindVary(P->Nam,&V)&&VarInForm(true,F,V)) return true;
   if(!P->R){ P=P->Sled->Sled; }
 }}
 return false;
}
//---------------------------------------------------------------------------
bool VassalOfRand(Form F){
//зависит ли формула от случайной функции rand
 switch(F.C->Atr){
  case CNST : return false;
  case VARY : return VassalOfRand(F.V->Znach);
  case UNOP :
   return F.U->Func==RAND ? true : VassalOfRand(F.U->Arg);
  case BNOP : return VassalOfRand(F.B->L)||VassalOfRand(F.B->R);
  case IFOP :
   return
    VassalOfRand(F.L->U)||VassalOfRand(F.L->V)||
    VassalOfRand(F.L->FF)||VassalOfRand(F.L->FT);
  case SUMM:
   return
    VassalOfRand(F.S->a)||VassalOfRand(F.S->b)||VassalOfRand(F.S->F);
  case INTG:
   return
    VassalOfRand(F.G->a)||VassalOfRand(F.G->b)||VassalOfRand(F.G->f);
  case IPLL:
   return VassalOfRand(F.I->F);
  case SPLN:
   return VassalOfRand(F.P->F);
 }
 return true;
}
//---------------------------------------------------------------------------
bool EqvConst(Form F){
 if(F.C->Atr==3||F.C->Atr==0) return false;
 if(VarInForm(true,F,L->Time))
  return false; //зависит от времени
 if(VarInForm(true,F,L->Freq))
  return false; //зависит от частоты
 if(VassalOfRand(F))
  return false; //зависит от rand
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
 if(L->M&&VassalOfSumIndex(L,F))
  return false;//зависит от индексов сумм
 if(L->I&&VassalOfIntgVary(L,F))
  return false;//зависит от переменных интегрирования
 return true;
}
//---------------------------------------------------------------------------
int KolInst(wchar_t *Name){ int Rez=0; wchar_t NameInst[256];
 for(Root *r=L->Inst;r;r=r->Sled){
  wcscpy(NameInst,r->Name); LowToUpp(NameInst);
  if(!wcscmp(NameInst,Name)) Rez++;
 }
 return Rez;
}
//---------------------------------------------------------------------------
int VaryToSumm(int k,Form F,Cnst *Znach){
//сборка сумматоров в список листа из формулы F и присвоение переменным
//суммирования значений Znach
 switch(F.C->Atr){
  case CNST :
  case VARY : break;
  case UNOP : k=VaryToSumm(k,F.U->Arg,Znach);  break;
  case BNOP : k=VaryToSumm(k,F.B->L,Znach); k=VaryToSumm(k,F.B->R,Znach); break;
  case IFOP : k=VaryToSumm(k,F.L->U,Znach); k=VaryToSumm(k,F.L->V,Znach);
   k=VaryToSumm(k,F.L->FF,Znach); k=VaryToSumm(k,F.L->FT,Znach);  break;
  case SUMM:
   if(!FindSumm(F.S->F,F.S->i,F.S->a,F.S->b,0)){
    if(!L->M){ L->M=F.S; L->M->Sled=NULL; }
    else{
     Summ *s; for(s=L->M;s->Sled;s=s->Sled); s->Sled=F.S; s->Sled->Sled=NULL;
    }
    k++; F.S->i->Znach.C=Znach;
   }
   k=VaryToSumm(k,F.S->a,Znach); k=VaryToSumm(k,F.S->b,Znach);
   k=VaryToSumm(k,F.S->F,Znach);  break;
  case INTG:
   k=VaryToSumm(k,F.G->a,Znach); k=VaryToSumm(k,F.G->b,Znach);
   k=VaryToSumm(k,F.G->f,Znach);
   break;
  case IPLL: k=VaryToSumm(k,F.I->F,Znach);  break;
  case SPLN: k=VaryToSumm(k,F.P->F,Znach);
 }
 return k;
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
 int K=0,Ki=0,i; Form F; bool Uge=false; FILE *FF; bool Rez=false; Vary *V;
 AnsiString Inf=StatusBar->SimpleText.c_str();
 StatusBar->SimpleText=
  L"Замена переменных их постоянными значениями, взятие интегралов и поиск корней";
// if(CGauge){ CGauge->MaxValue=K; CGauge->Progress=0; }
// if(DGSFILE=_wfopen(DgsFile,L"r+")) FILE=DGSFILE;
// else FILE=HTMFILE;
 if(!SLD) SLD=new TStringList;
 else for(int i=0;i<Kpodv;i++) SLD->Delete(SLD->Count-1);
/* if(L->I&&Second){
  FF=_wfopen(L"timedgs.htm",L"w");
  if(FF){ wchar_t S[1024];
   while(fgetws(S,1023,DGSFILE)){
	if(!wcsstr(S,L"Переменные, формулы которых не зависят от времени,L"))
	 fputws(S,FF);
	else break;
   }
   fclose(FILE); _wunlink(FILE==DGSFILE?DgsFile:HtmFile);
   fclose(FF); _wrename(L"timedgs.htm",FILE==DGSFILE?DgsFile:HtmFile);
   FILE=_wfopen(FILE==DGSFILE?DgsFile:HtmFile,L"r+");
   if(FILE==DGSFILE) fseek(DGSFILE,0,2);
  }
  Rez=true;
 }
 else
  if(FILE==DGSFILE) fseek(DGSFILE,-LpodvD,2);*/
 if(L->I){ Intg *I;
  StatusBar->SimpleText=L"Аналитическое вычисление интегралов";
  for(I=L->I;I;I=I->Sled) Ki++;
  if(CGauge){ CGauge->Progress=0; CGauge->MaxValue=Ki; } Uge=false;
  for(I=L->I;I;I=I->Sled){
   if(CGauge) CGauge->Progress++;
   if((I->F!=Pust)||IsTaken(I)){ F.G=I;
	if(!Uge){//один раз пишем
//	 fwprintf(FILE,L"<h4 align=\"left\"><font color=\"#000099\"><b>"
//	  L"Интегралы, которые берутся аналитически,\n"
//	  L"поэтому в расчетах используются их аналитические значения:</b></font></h4>\n");
//	 fputws(L"<p align=\"left\">",FILE);
	 SLD->Add(
	  L"<h4 align=\"left\"><font color=\"#000099\"><b>"
	  L"Интегралы, которые берутся аналитически,");
	 SLD->Add(L"поэтому в расчетах используются их аналитические"
	  L" значения:</b></font></h4>");
	 SLD->Add(L"<p align=\"left\">");
	 Uge=true;
	}
	//Распечатаем интеграл
//	PrintForm(false,FILE,NULL,F);
	sdgs=FormToStringList(F,NULL,Sdgs,sdgs,SLD,false,&SF);
	sdgs+=swprintf(sdgs,L"<br>"); SLD->Add(Sdgs);
	*Sdgs='\0'; sdgs=Sdgs;
  }}
  for(I=L->I;I;I=I->Sled){
   if(I->x==L->Time&&I->b.V==L->Time){
	I->B=Value(I->a); I->Val=0.0;
  }}
  if(CGauge) CGauge->Progress=0;
//  if(Uge) fputws(L"</p>\n",FILE);
 }
 Uge=false;
 for(i=1,V=L->V;V;V=V->Sled){
  if(EqvConst(V->Znach)){
   if(!Uge){//один раз пишем
//	fwprintf(FILE,
//	 L"<br><h4 align=\"left\"><font color=\"#000099\"><b>"
//	 L"Переменные, заменяемые своими постоянными значениями:"
//	 L"</b></font></h4>\n");
//	fputws(L"<p align=\"left\">",FILE);
	SLD->Add(
	 L"<br><h4 align=\"left\"><font color=\"#000099\"><b>"
	 L"Переменные, заменяемые своими постоянными значениями:"
	 L"</b></font></h4>");
	SLD->Add(L"<p align=\"left\">");
	Uge=true;
//	fwprintf(FILE,L"<table width=\"100%\" border=\"0\">");
	SLD->Add(L"<table width=\"100%\" border=\"0\">");
   }
   if(i>KColumn){
	i=1;
//	fwprintf(FILE,L"</tr>\n");
	SLD->Add(L"</tr>");
   }
   if(i==1)
//	fwprintf(FILE,L"<tr>\n");
	SLD->Add(L"<tr>");
//   fwprintf(FILE,L"<td width=\"%0.0f%%\">",Round(100/KColumn));
   swprintf(Sdgs,L"<td width=\"%0.0f%%\">",Round(100/KColumn));
   SLD->Add(Sdgs);
   h_RK=-1.0; V->Val=Val(V->Znach); V->P=0;
//   PrintForm(false,FILE,V->Name,V->Znach);
   sdgs=FormToStringList(V->Znach,V->Name,Sdgs,sdgs,SLD,false,&SF);
//   fwprintf(FILE,L"=%g;<br>",V->Val);
//   fputws(L"</td>\n",FILE); i++;
   sdgs+=swprintf(sdgs,L"=%g;<br>",V->Val);
   sdgs+=swprintf(sdgs,L"</td>"); SLD->Add(Sdgs); sdgs=Sdgs;
   i++;
 }}
// fwprintf(FILE,L"</tr></table>\n");
 SLD->Add(L"</tr></table>");
 for(int i=0;i<Kpodv;i++) SLD->Add(SLP->Strings[i]);
 SLD->SaveToFile(DgsFile);
// if(FILE==DGSFILE){
//  fwprintf(DGSFILE,L"<hr><font face=\"Georgia, Times New Roman, Times, serif\""
//   L" color=\"#326464\">&copy; %s, %d<br></font>\n",Autor,NYear+1900);
//  fwprintf(DGSFILE,L"</body>\n</html>\n");
// }
// else{
//  fwprintf(HTMFILE,
//   L"<hr><font face=\"Georgia, Times New Roman, Times, serif\""
//   L" color=\"#326464\">&copy; %s, %d<br></font><br>\n",Autor,NYear+1900)+1;
//  fwprintf(HTMFILE,L"</body>\n</html>\n")+2;
// }
// fclose(FILE);
 if(CGauge) CGauge->Progress=0; StatusBar->SimpleText=Inf;
 return Rez;
}
//---------------------------------------------------------------------------
bool VassalFromVaryDiap(Form F){ Root *I; Vary *V;
 if((I=FindInstLast(L"ДИАПАЗОН"))||(I=FindInstLast(L"ДИАПАЗОНЫ")))
  for(Parm *P=I->First;P;P=P->Sled)
   if(FindVary(P->Nam,&V)&&VarInForm(true,F,V)) return true;
 return false;
}
//---------------------------------------------------------------------------
bool VassalFromVaryParam(Form F){ Root *I; Vary *V;
 if(I=FindInstLast(L"ВАРЬИРОВАТЬ"))
  for(Parm *P=I->First;P;P=P->Sled)
   if(FindVary(P->Nam,&V)&&VarInForm(true,F,V)) return true;
 return false;
}
//---------------------------------------------------------------------------

