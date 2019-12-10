#include "CA.h"

extern Cnst *Pust,*Nul,*Odin; extern char Inf[];
extern List *L;

Form FormTras(Form F){
 Form FT,FF,FU,FV,FS,A,B;
 switch(F.C->Atr) {
  case PUST: case CNST: case IPLL: case SPLN: break;
  case VARY:
   if(!(F.V->Znach==Pust)&&(F.V->Znach.C->Atr!=CNST))
    F.V->Znach=FormTras(F.V->Znach);
   if(F.V->Znach.C->Atr==CNST) F.C=F.V->Znach.C;
   break;
  case UNOP: FU=FormTras(F.U->Arg);
   if(FU!=F.U->Arg) switch(F.U->Func) {
    case  1: F=-FU; break;
    case  2: F=sqrt(FU); break;
    case  3: F=sin(FU); break;
    case  4: F=cos(FU); break;
    case  5: F=tg(FU); break;
    case  6: F=arcsin(FU); break;
    case  7: F=arctg(FU); break;
    case  8: F=exp(FU); break;
    case  9: F=ln(FU); break;
    case 10: F=sh(FU); break;
    case 11: F=ch(FU); break;
    case 12: F=arsh(FU); break;
    case 13: F=abs(FU); break;
    case 14: F=rand(FU); break;
    case 15: F=ceil(FU); break;
    case 16: F=floor(FU); break;
    case 17: F=arccos(FU); break;
   } break;
  case IFOP:
   FT=FormTras(F.L->FT); FF=FormTras(F.L->FF);
   if(FT==FF) F=FT;
   else {
    FU=FormTras(F.L->U); FV=FormTras(F.L->V);
    if((FT!=F.L->FT)||(FF!=F.L->FF)||(FU!=F.L->U)||(FV!=F.L->V))
     F.L=TakeIfop(F.L->Oper,FU,FV,FT,FF);
   } break;
  case BNOP:
   FU=FormTras(F.B->L); FV=FormTras(F.B->R);
   if((FU!=F.B->L)||(FV!=F.B->R)) switch(F.B->Oper) {
    case 1: F=FU+FV; break;
    case 2: F=FU-FV; break;
    case 3: F=FU*FV; break;
    case 4: F=FU/FV; break;
    case 5: F=FU^FV; break;
   } break;
  case SUMM:
   if((FS=FormTras(F.S->F))==Nul) F.C=Nul;
   else {
    A=FormTras(F.S->a); B=FormTras(F.S->b);
    F.S=TakeSumm(FS,F.S->i,A,B);
   } break;
  case INTG:
   if((FS=FormTras(F.G->f))==Nul) F.C=Nul;
   else if(F.G->Type==DEFINE){
    A=FormTras(F.G->a); B=FormTras(F.G->b);
    F.G=TakeIntg(FS,F.G->x,A,B);
   }
   else
    F.G=TakeIntg(FS,F.G->x);
   break;
 } return F;
}
//---------------------------------------------------------------------------
Form FormOpenDiff(Form F){
/* процедура проводит замену формулы F формулой, у которой  */
/* все переменные, имя которых содержит знак "'" заменяются */
/* их формульными значениями                                */
 Form FT,FF,FU,FV,FS,A,B;
 switch(F.C->Atr) {
  case PUST: case CNST: case IPLL: case SPLN: break;
  case VARY:
   if(F.V->Znach.C==Pust) break;
   if(strchr(F.V->Name,'\''))
    F=FormOpenDiff(F.V->Znach);
   break;
  case UNOP: FU=FormOpenDiff(F.U->Arg);
   if(FU!=F.U->Arg) switch(F.U->Func) {
    case  1: F=-FU;        break;
    case  2: F=sqrt(FU);   break;
    case  3: F=sin(FU);    break;
    case  4: F=cos(FU);    break;
    case  5: F=tg(FU);     break;
    case  6: F=arcsin(FU); break;
    case  7: F=arctg(FU);  break;
    case  8: F=exp(FU);    break;
    case  9: F=ln(FU);     break;
    case 10: F=sh(FU);     break;
    case 11: F=ch(FU);     break;
    case 12: F=arsh(FU);   break;
    case 13: F=abs(FU);    break;
    case 14: F=rand(FU);   break;
    case 15: F=ceil(FU);   break;
    case 16: F=floor(FU);  break;
    case 17: F=arccos(FU); break;
   } break;
  case IFOP:
   FT=FormOpenDiff(F.L->FT); FF=FormOpenDiff(F.L->FF);
   if(FT==FF) F=FT;
   else {
    FU=FormOpenDiff(F.L->U); FV=FormOpenDiff(F.L->V);
    if((FT!=F.L->FT)||(FF!=F.L->FF)||(FU!=F.L->U)||(FV!=F.L->V))
     F.L=TakeIfop(F.L->Oper,FU,FV,FT,FF);
   } break;
  case BNOP:
   FU=FormOpenDiff(F.B->L); FV=FormOpenDiff(F.B->R);
   if((FU!=F.B->L)||(FV!=F.B->R)) switch(F.B->Oper) {
    case 1: F=FU+FV; break;
    case 2: F=FU-FV; break;
    case 3: F=FU*FV; break;
    case 4: F=FU/FV; break;
    case 5: F=FU^FV; break;
   } break;
  case SUMM:
   if((FS=FormOpenDiff(F.S->F))==Nul) F.C=Nul;
   else {
    A=FormOpenDiff(F.S->a); B=FormOpenDiff(F.S->b);
    F.S=TakeSumm(FS,F.S->i,A,B);
   }
   break;
  case INTG:
   if((FS=FormOpenDiff(F.G->f))==Nul) F.C=Nul;
   else if(F.G->Type==DEFINE){
    A=FormOpenDiff(F.G->a); B=FormOpenDiff(F.G->b);
    F.G=TakeIntg(FS,F.G->x,A,B);
   }
   else
    F.G=TakeIntg(FS,F.G->x);
   break;
 }
 return F;
}
//---------------------------------------------------------------------------
Form ObNul(Form F){
 Form FT,FF,FU,FV,FS,A,B; Vary *V;
 switch(F.C->Atr) {
  case PUST: case CNST: case IPLL: case SPLN: break;
  case VARY:
   if(F.V->Znach.C==Nul) F.C=Nul;
//   else F=ObNul(F.V->Znach);
   else{
//    F.V->Znach=ObNul(F.V->Znach);
    FV=ObNul(F.V->Znach);
    if(FV!=F.V->Znach){
     sprintf(Inf,"_%s",F.V->Name); V=TakeVary(Inf);
     V->Znach=FV; F.V=V;
    }
    if(F.V->Znach.C==Nul) F.C=Nul;
    else if(F.V->Znach.C->Atr==CNST) F.C=F.V->Znach.C;
   }
   break;
  case UNOP: FU=ObNul(F.U->Arg);
   if(FU!=F.U->Arg) switch(F.U->Func) {
    case  1: F=-FU; break;
    case  2: F=sqrt(FU); break;
    case  3: F=sin(FU); break;
    case  4: F=cos(FU); break;
    case  5: F=tg(FU); break;
    case  6: F=arcsin(FU); break;
    case  7: F=arctg(FU); break;
    case  8: F=exp(FU); break;
    case  9: F=ln(FU); break;
    case 10: F=sh(FU); break;
    case 11: F=ch(FU); break;
    case 12: F=arsh(FU); break;
    case 13: F=abs(FU); break;
    case 14: F=rand(FU); break;
    case 15: F=ceil(FU); break;
    case 16: F=floor(FU); break;
    case 17: F=arccos(FU); break;
   }
   break;
  case IFOP:
   FT=ObNul(F.L->FT); FF=ObNul(F.L->FF);
   if(FT==FF) F=FT;
   else {
    FU=ObNul(F.L->U); FV=ObNul(F.L->V);
    if((FT!=F.L->FT)||(FF!=F.L->FF)||(FU!=F.L->U)||(FV!=F.L->V))
     F.L=TakeIfop(F.L->Oper,FU,FV,FT,FF);
   }
   break;
  case BNOP:
   FU=ObNul(F.B->L); FV=ObNul(F.B->R);
   if((FU!=F.B->L)||(FV!=F.B->R)) switch(F.B->Oper) {
    case 1: F=FU+FV; break;
    case 2: F=FU-FV; break;
    case 3: F=FU*FV; break;
    case 4: F=FU/FV; break;
    case 5: F=FU^FV; break;
   }
   break;
  case SUMM:
   if((FS=ObNul(F.S->F))==Nul) F.C=Nul;
   else {
    A=ObNul(F.S->a); B=ObNul(F.S->b);
    F.S=TakeSumm(FS,F.S->i,A,B);
   }
   break;
  case INTG:
   if((FS=ObNul(F.G->f))==Nul) F.C=Nul;
   else if(F.G->Type==DEFINE){
    A=ObNul(F.G->a); B=ObNul(F.G->b);
    F.G=TakeIntg(FS,F.G->x,A,B);
   }
   else
    F.G=TakeIntg(FS,F.G->x);
   break;
 }
 return F;
}
//---------------------------------------------------------------------------
bool Coord(Vary *V){
//возвращает истину, если переменная есть обобщенная координата
 for(Cord *K=L->K;K;K=K->Sled) if(V==K->V) return true;
 return false;
}
//---------------------------------------------------------------------------
Form ObNulCord(Form F){
 Form FT,FF,FU,FV,FS,A,B; Vary *V;
 switch(F.C->Atr) {
  case PUST: case CNST: case IPLL: case SPLN: break;
  case VARY:
   if(Coord(F.V)) F.C=Nul;
   else{
//    F.V->Znach=ObNul(F.V->Znach);
    FV=ObNulCord(F.V->Znach);
//    if(FV!=F.V->Znach){
//     sprintf(Inf,"_%s",F.V->Name); V=TakeVary(Inf);
//     V->Znach=FV; F.V=V;
//    }
    if(F.V->Znach.C==Nul) F.C=Nul;
    else if(F.V->Znach.C->Atr==CNST) F.C=F.V->Znach.C;
   }
   break;
  case UNOP: FU=ObNulCord(F.U->Arg);
   if(FU!=F.U->Arg) switch(F.U->Func) {
    case  1: F=-FU; break;
    case  2: F=sqrt(FU); break;
    case  3: F=sin(FU); break;
    case  4: F=cos(FU); break;
    case  5: F=tg(FU); break;
    case  6: F=arcsin(FU); break;
    case  7: F=arctg(FU); break;
    case  8: F=exp(FU); break;
    case  9: F=ln(FU); break;
    case 10: F=sh(FU); break;
    case 11: F=ch(FU); break;
    case 12: F=arsh(FU); break;
    case 13: F=abs(FU); break;
    case 14: F=rand(FU); break;
    case 15: F=ceil(FU); break;
    case 16: F=floor(FU); break;
    case 17: F=arccos(FU); break;
   }
   break;
  case IFOP:
   FT=ObNulCord(F.L->FT); FF=ObNulCord(F.L->FF);
   if(FT==FF) F=FT;
   else {
    FU=ObNulCord(F.L->U); FV=ObNulCord(F.L->V);
    if((FT!=F.L->FT)||(FF!=F.L->FF)||(FU!=F.L->U)||(FV!=F.L->V))
     F.L=TakeIfop(F.L->Oper,FU,FV,FT,FF);
   }
   break;
  case BNOP:
   FU=ObNulCord(F.B->L); FV=ObNulCord(F.B->R);
   if((FU!=F.B->L)||(FV!=F.B->R)) switch(F.B->Oper) {
    case 1: F=FU+FV; break;
    case 2: F=FU-FV; break;
    case 3: F=FU*FV; break;
    case 4: F=FU/FV; break;
    case 5: F=FU^FV; break;
   }
   break;
  case SUMM:
   if((FS=ObNulCord(F.S->F))==Nul) F.C=Nul;
   else {
    A=ObNulCord(F.S->a); B=ObNulCord(F.S->b);
    F.S=TakeSumm(FS,F.S->i,A,B);
   }
   break;
  case INTG:
   if((FS=ObNulCord(F.G->f))==Nul) F.C=Nul;
   else if(F.G->Type==DEFINE){
    A=ObNulCord(F.G->a); B=ObNulCord(F.G->b);
    F.G=TakeIntg(FS,F.G->x,A,B);
   }
   else
    F.G=TakeIntg(FS,F.G->x);
   break;
 }
 return F;
}
//---------------------------------------------------------------------------
Form FormToOpen(Form F){
 Form FT,FF,FU,FV,FS,A,B;
 switch(F.C->Atr) {
  case PUST: case CNST: case IPLL: case SPLN: break;
  case VARY:
   if((F.V->Znach==Pust)||(F.V->Znach.C->Atr==CNST)) break;
   F=F.V->Znach;
   break;
  case UNOP: FU=FormToOpen(F.U->Arg);
   if(FU!=F.U->Arg) switch(F.U->Func) {
    case  1: F=-FU; break;
    case  2: F=sqrt(FU); break;
    case  3: F=sin(FU); break;
    case  4: F=cos(FU); break;
    case  5: F=tg(FU); break;
    case  6: F=arcsin(FU); break;
    case  7: F=arctg(FU); break;
    case  8: F=exp(FU); break;
    case  9: F=ln(FU); break;
    case 10: F=sh(FU); break;
    case 11: F=ch(FU); break;
    case 12: F=arsh(FU); break;
    case 13: F=abs(FU); break;
    case 14: F=rand(FU); break;
    case 15: F=ceil(FU); break;
    case 16: F=floor(FU); break;
    case 17: F=arccos(FU); break;
   } break;
  case IFOP:
   FT=FormToOpen(F.L->FT); FF=FormToOpen(F.L->FF);
   if(FT==FF) F=FT;
   else {
    FU=FormToOpen(F.L->U); FV=FormToOpen(F.L->V);
    if((FT!=F.L->FT)||(FF!=F.L->FF)||(FU!=F.L->U)||(FV!=F.L->V))
     F.L=TakeIfop(F.L->Oper,FU,FV,FT,FF);
   } break;
  case BNOP:
   FU=FormToOpen(F.B->L); FV=FormToOpen(F.B->R);
   if((FU!=F.B->L)||(FV!=F.B->R)) switch(F.B->Oper) {
    case 1: F=FU+FV; break;
    case 2: F=FU-FV; break;
    case 3: F=FU*FV; break;
    case 4: F=FU/FV; break;
    case 5: F=FU^FV; break;
   } break;
  case SUMM:
   if((FS=FormToOpen(F.S->F))==Nul) F.C=Nul;
   else {
    A=FormToOpen(F.S->a); B=FormToOpen(F.S->b);
    F.S=TakeSumm(FS,F.S->i,A,B);
   } break;
  case INTG:
   if((FS=FormToOpen(F.G->f))==Nul) F.C=Nul;
   else if(F.G->Type==DEFINE){
    A=FormToOpen(F.G->a); B=FormToOpen(F.G->b);
    F.G=TakeIntg(FS,F.G->x,A,B);
   }
   else
    F.G=TakeIntg(FS,F.G->x);
   break;
 } return F;
}
//---------------------------------------------------------------------------

