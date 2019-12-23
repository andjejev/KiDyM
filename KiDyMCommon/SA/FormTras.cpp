#include "CA.h"

extern Cnst *Pust,*Nul,*Odin; extern wchar_t Inf[],*NameFunc[];
extern List *L; extern bool REPVARNACNST;
/*
Form FindRazm(Form F){
 F.C=Pust; return F;
 Form Razm,Razm_1,Razm_2; double v; wchar_t *s; Vary *V1,*V2;
 switch(F.C->Atr) {
  case PUST: case CNST: Razm.C=Pust; break;
  case UNOP:
   switch(F.U->Func){
    case 1: case 13: case 14: case 16: case 15: case 18:  //13 14 15 16 18
     Razm=FindRazm(F.U->Arg);
    break;
    case 2://корень квадратный
     Razm_1=FindRazm(F.U->Arg);
     Razm=sqrt(Razm_1);
     break;
   }
   break;
  case IPLL: Razm=FindRazm(F.I->F); break;
  case SPLN: Razm=FindRazm(F.P->F); break;
  case VARY:
   if(s=wcschr(F.V->Name,'\'')){
    *s='\0'; Razm.C=Pust;
    if(FindVary(F.V->Name,&V1)&&FindVary(s+1,&V2)){
     Form F1,F2;
     F1.V=V1; F2.V=V2;
     Razm_1=FindRazm(F1); Razm_2=FindRazm(F2);
     if((Razm_1!=Pust)&&(Razm_2!=Pust))
      Razm=Razm_1/Razm_2;
    }
    *s='\'';
   }
   else if(F.V->Razm.C!=Pust)
    Razm=F.V->Razm;
   else
    Razm=FindRazm(F.V->Znach);
   break;
  case IFOP:
   Razm_1=FindRazm(F.L->FT); Razm_2=FindRazm(F.L->FF);
   Razm.C=Pust;
   if(Razm_1==Razm_2) Razm=Razm_1;
   else Razm.C=Pust;
   break;
  case BNOP:
   Razm_1=FindRazm(F.B->L); Razm_2=FindRazm(F.B->R);
   switch(F.B->Oper) {
    case 1: Razm=Razm_1; break;
    case 2: Razm=Razm_1; break;
    case 3:
     if(Razm_1==Pust) Razm=Razm_2;
     else if(Razm_2==Pust) Razm=Razm_1;
     else Razm=Razm_1*Razm_2;
     break;
    case 4: case 5:
     if(Razm_1==Pust) Razm=1/Razm_2;
     else if(Razm_2==Pust) Razm=Razm_1;
     else Razm=Razm_1/Razm_2;
     break;
    case 6:
     if((Razm_2==Pust)&&(F.B->Oper==6)&&
        (F.B->R.C->Atr==CNST)&&(v=F.B->R.C->Val,(v==ceil(v))))
      Razm=Razm_1^F.B->R;
     else Razm.C=Pust;
     break;
   }
   break;
 }
 return Razm;
}*/
//---------------------------------------------------------------------------
Factor::Factor(Form f){ F=f; }
//---------------------------------------------------------------------------
int Factor::Cmp(Factor *f){//this < f, возвращает -1, если =, то 0, иначе -1
 swprintf(Inf,L"Для типа = %d не определено соотношение с типом",f->F.C->Atr,F.C->Atr);
 switch(F.C->Atr){
  case CNST:
   switch(f->F.C->Atr){
    case CNST:
     if(F.C->Name&&f->F.C->Name) return wcscmp(F.C->Name,f->F.C->Name);
     else if(F.C->Val<f->F.C->Val) return -1;
     else if(F.C->Val>f->F.C->Val) return  1;
     return 0;
    case VARY:
     if(F.C->Name) return wcscmp(F.C->Name,f->F.V->Name);
    case UNOP:
    case BNOP: return -1;
   }
   break;
  case VARY:
   switch(f->F.C->Atr){
    case CNST:
     if(f->F.C->Name) return wcscmp(f->F.C->Name,F.V->Name);
     return 1;
    case VARY: return wcscmp(F.V->Name,f->F.V->Name);
    case UNOP: return -1;
    case BNOP: return -1;
   }
   break;
  case UNOP:
   switch(f->F.C->Atr){
    case CNST: return 1;
    case VARY: return 1;
    case UNOP:
     if(F.U->Func<f->F.U->Func) return -1;
     else if(F.U->Func>f->F.U->Func) return 1;
     else{
      Factor F1(F.U->Arg),F2(f->F.U->Arg);
      return F1.Cmp(&F2);
     }
    case BNOP: return -1;
   }
   break;
  case BNOP:
   switch(f->F.C->Atr){
    case CNST:
    case VARY:
    case UNOP: return 1;
    case BNOP:
     if(F.B->Oper<f->F.B->Oper) return -1;
     else if(F.B->Oper>f->F.B->Oper) return 1;
     else{
      Factor L1(F.B->L),L2(f->F.B->L); int Rez;
      if(!(Rez=L1.Cmp(&L2))){
       Factor R1(F.B->R),R2(f->F.B->R);
       return R1.Cmp(&R2);
      }
      return Rez;
     }
   }
   break;
 }
 Application->MessageBox(Inf,L"Factor::Cmp",MB_OK);
 return 0;
}
//---------------------------------------------------------------------------
/* процедура поиска сомножителя в списке.
   возвращает:
    *F=NULL и 0, если новый сомножитель надо ставить в начало списка
    *F\=NULL и 0, если новый сомножитель надо ставить после *F
    *F\=NULL и 1, если такой сомножитель найден - указатель на него находится
                  в последнем формальном параметре: */
int Summand::Find(Factor *f,Factor **F){
 if(!(*F=R)) return 0;
 if(R->Cmp(f)>0){ *F=NULL; return 0; }
 for(;;*F=(*F)->Sled){
  if(!(*F)->Cmp(f)) return 1;
  if(!((*F)->Sled)) return 0;
  if((*F)->Sled->Cmp(f)>0) return 0;
}}
//---------------------------------------------------------------------------
Factor *Summand::Add(Factor *f){
 Factor *pF,*pf; if(!f) return NULL;
// if(Find(f,&pF)) return pF;
 Find(f,&pF); pf=new Factor; *pf=*f;
 if(!pF){ pf->Sled=R;        return        R=pf; }
 else   { pf->Sled=pF->Sled; return pF->Sled=pf; }
}
//---------------------------------------------------------------------------
Summand::~Summand(){
// Factor *r;
// while(r=R){
//  R=r->Sled;
//  delete r;
}//}
//---------------------------------------------------------------------------
Train::Train(Form F){
 R=NULL;
 TrainFromForm(F);
}
//---------------------------------------------------------------------------
void Train::Add(Train *t){
 Summand *s; while(s=t->R){ t->R=s->Sled; s->Sled=NULL; Add(s); }
}
//---------------------------------------------------------------------------
Summand *operator * (Summand A,Summand B){
 Summand *R=new Summand; Factor *f;
 for(f=A.R;f;f=f->Sled) R->Add(f);
 for(f=B.R;f;f=f->Sled) R->Add(f);
 return R;
}
//---------------------------------------------------------------------------
void Train::TrainFromForm(Form F){
// if((F.C->Atr>BNOP)||(F.C->Atr==BNOP)&&(F.B->Oper>5)){
 if(F.C->Atr>BNOP){
  swprintf(Inf,L"Состав может быть построен из констант,\n"
   L"переменных, унарных и бинарных ('+', '-', '*', '^') операций");
  Application->MessageBox(Inf,L"Train::TrainFromForm",MB_OK);
 }
 else{
  Summand *s; Factor *f; Train *T,*Lf,*Rf; Form FF;
  switch(F.C->Atr){
   case CNST://константу - в сомножитель, а его в слагаемое нового состава
   case VARY://переменную - в сомножитель, а его в слагаемое нового состава
    f=new Factor;
    if(F.C->Atr==CNST) f->F.C=F.C; else f->F.V=F.V;
    s=new Summand;
    s->Add(f);
    Add(s);
    delete s;
    delete f;
    break;
   case UNOP:
    //аргумент превратим в новый нормированный состав:
    T=new Train;
    T->TrainFromForm(F.U->Arg);
T->ToFile(L"T.prn",true);
    //этот состав снова превратим в формулу:
    //создадим новый сомножитель с новой унарной операцией с этим новым аргументом:
FF=T->ToForm();
    delete T;
    f=new Factor;
    f->F.U=TakeUnop(F.U->Func,FF);
    //из сомножителя сделаем слагаемое в создаваемом новом составе
    s=new Summand;
    s->Add(f);
    Add(s);
    delete s;
    delete f;
    break;
   case BNOP:
    //сделаем состав из левой формулы-операнда:
    Lf=new Train; Rf=new Train;
    Lf->TrainFromForm(F.B->L);
Lf->ToFile(L"L.prn",true);
    //сделаем состав из правой формулы-операнда:
    Rf->TrainFromForm(F.B->R);
Rf->ToFile(L"R.prn",true);
    switch(F.B->Oper){
     case 1: //сложение
      //внедрим второй состав в первый - получим новый нормированный состав:
      Lf->Add(Rf);
      Add(Lf);
      break;
     case 2: //вычитание
      //поменяем знаки в слагаемых второго состава
      Rf->Minus();
      //внедрим второй состав в первый - получим новый нормированный состав:
      Lf->Add(Rf);
      Add(Lf);
      break;
     case 3: //умножение
      //перемножим каждое слагаемое одного состава с каждым слагаемым другого и
      //сделаем новый состав из этих слагаемых-произведений
      for(Summand *s1=Lf->R;s1;s1=s1->Sled){
       for(Summand *s2=Rf->R;s2;s2=s2->Sled){
        Summand *s12=new Summand;
        for(Factor *f=s1->R;f;f=f->Sled) s12->Add(f);
        for(Factor *f=s2->R;f;f=f->Sled) s12->Add(f);
        s12->Znak = s1->Znak!=s2->Znak ? true : false;
        Add(s12);
        delete s12;
      }}
      break;
     case 4: //деление
      //создадим новый сомножитель с новой бинарной операцией с этими формулами:
      f=new Factor;
      f->F.B=TakeBnop(F.B->Oper,Lf->ToForm(),Rf->ToForm());
      //из сомножителя сделаем слагаемое в создаваемом новом составе
      s=new Summand;
      s->Add(f); Add(s);
      delete s;
      delete f;
      break;
     case 6: //возведение в степень
      //создадим новый сомножитель с новой бинарной операцией с этими формулами:
      f=new Factor;
      f->F.B=TakeBnop(F.B->Oper,Lf->ToForm(),Rf->ToForm());
      //из сомножителя сделаем слагаемое в создаваемом новом составе
      s=new Summand;
      s->Add(f); Add(s);
      delete s;
      delete f;
      break;
    }
    delete Lf; delete Rf;
}}}
//---------------------------------------------------------------------------
int Summand::Cmp(Summand *s){//this < s, возвращает -1, если =, то 0, иначе 1
 if(R&&s->R){
  Factor *f1,*f2; int Rez;
  for(f1=R,f2=s->R;f1&&f2;f1=f1->Sled,f2=f2->Sled){
   if(f1&&f2&&(Rez=f1->Cmp(f2))) return Rez;
   if(!f2) return -1;
   if(!f1) return 1;
 }}
 if(R) return -1;
 return 1;
}
//---------------------------------------------------------------------------
/* процедура поиска слагаемого в списке.
   возвращает:
    *S=NULL и 0, если новое слагаемое надо ставить в начало списка
    *S\=NULL и 0, если новое слагаемое надо ставить после *S
    *S\=NULL и 1, если такое слагаемое найдено - указатель на него находится
                  в последнем формальном параметре: */
int Train::Find(Summand *s,Summand **S){
 if(!(*S=R)) return 0;
 if(R->Cmp(s)>0){ *S=NULL; return 0; }
 for(;;*S=(*S)->Sled){
  if(!(*S)->Cmp(s)) return 1;
  if(!((*S)->Sled)) return 0;
  if((*S)->Sled->Cmp(s)>0) return 0;
}}
//---------------------------------------------------------------------------
void Train::Add(Summand *s){
 Summand *pS,*ps; if(!s) return;
 Find(s,&pS); ps=new Summand; *ps=*s; s->R=NULL;
 if(!pS){ ps->Sled=R;        R=ps; }
 else   { ps->Sled=pS->Sled; pS->Sled=ps; }
}
//---------------------------------------------------------------------------
void Train::ToStr(AnsiString &S){
 String sf;
 for(Summand *s=R;s;s=s->Sled){
  if(s->Znak) S+='-'; else if(s!=R) S+='+';
  for(Factor *f=s->R;f;f=f->Sled){
   FormToStr(f->F,&sf, false);
   S+=sf; if(f->Sled) S+='*';
}}}
//---------------------------------------------------------------------------
void Train::ToFile(wchar_t *NameFile,bool New){
 FILE *F=_wfopen(NameFile,New?L"w":L"a"); AnsiString S;
 ToStr(S);
 fwprintf(F,L"%s\n",S.c_str());
 fclose(F);
}
//---------------------------------------------------------------------------
Form Factor::ListToForm(){
 if(!Sled) return F;
 else{
  Form L=Sled->ListToForm();
  L.B=TakeBnop(3,F,L);
  return L;
}}
//---------------------------------------------------------------------------
bool Factor::Eqv(Factor *f){ //Эквивалентны ли факторы (сомножители) ?
 Train *ArgL,*ArgR;
 if(F.C->Atr!=f->F.C->Atr) return false;
 switch(F.C->Atr){
  case CNST:
   if((F.C->Name!=f->F.C->Name)||(F.C->Val!=f->F.C->Val)) return false;
   break;
  case VARY:
   if(F.V->Name!=f->F.V->Name) return false;
   break;
  case UNOP:
   if(F.U->Func!=f->F.U->Func) return false;
   ArgL=new Train; ArgR=new Train;
   ArgL->TrainFromForm(F.U->Arg);
   ArgR->TrainFromForm(f->F.U->Arg);
   if(!ArgL->Eqv(ArgR)){
    delete ArgL; delete ArgR;
    return false;
   }
   delete ArgL; delete ArgR;
   break;
  case BNOP:
   if(F.B->Oper!=f->F.B->Oper) return false;
   ArgL=new Train; ArgR=new Train;
   ArgL->TrainFromForm(F.B->L);
   ArgR->TrainFromForm(f->F.B->L);
   if(!ArgL->Eqv(ArgR)) return false;
   ArgL->TrainFromForm(F.B->R);
   ArgR->TrainFromForm(f->F.B->R);
   if(!ArgL->Eqv(ArgR)){
    delete ArgL; delete ArgR;
    return false;
   }
   delete ArgL; delete ArgR;
   break;
 }
 return true;
}
//---------------------------------------------------------------------------
Form Summand::ToForm(){
 Form F,FF;
 if(!R){ F.C=Pust; return F; }
 FF=R->F;
 if(!R->Sled) return FF;
 F=R->Sled->ListToForm();
 F.B=TakeBnop(3,FF,F);
 return F;
}
//---------------------------------------------------------------------------
Form Summand::ListToForm(){
 Form F=ToForm(),L;
 if(!Sled) return F;
 L=Sled->ListToForm();
 if(IsMins(F)&&IsMins(L)) L.B=TakeBnop(3,F.U->Arg,L.U->Arg);
 if(IsMins(F)){ F.B=TakeBnop(3,F.U->Arg,L); L.U=TakeUnop(1,F); }
 if(IsMins(L)){ F.B=TakeBnop(3,F,L.U->Arg); L.U=TakeUnop(1,F); }
 L.B=TakeBnop(3,F,L);
 return L;
}
//---------------------------------------------------------------------------
bool Summand::Eqv(Summand *S){
 if(Znak!=S->Znak) return false;
 if(KolElem(R)!=KolElem(S->R)) return false;
 for(Factor *f1=R,*f2=S->R;f1&&f2;f1=f1->Sled,f2=f2->Sled)
  if(!f1->Eqv(f2)) return false;
 return true;
}
//---------------------------------------------------------------------------
Form Train::ToForm(){ Form F,FF,Rez;
 if(!R){ F.C=Pust; return F; }
 FF=R->ToForm();
 if(!R->Sled) return FF;
 F=R->Sled->ListToForm();
 Rez.B=IsMins(F)?
       TakeBnop(2,FF,F):
       TakeBnop(1,FF,F);
 return Rez;
}
//---------------------------------------------------------------------------
bool Train::Eqv(Train *T){
 if(KolElem(R)!=KolElem(T->R)) return false;
 for(Summand *s1=R,*s2=T->R;s1&&s2;s1=s1->Sled,s2=s2->Sled)
  if(!s1->Eqv(s2)) return false;
 return true;
}
//---------------------------------------------------------------------------
Train::~Train(){
// Summand *r;
// while(r=R){
//  R=r->Sled;
//  delete(r);
// }
}
//---------------------------------------------------------------------------
/*
Terms *Norm(Terms *){
 return NULL;
}
Form TermsToForm(Terms *){
 Form F;
 F.C=Pust;
 return F;
}
void TermsToFile(Terms *T,wchar_t *NameFile, bool New){
 FILE *F=_wfopen(NameFile,New?L"w":L"a");
 if(F){ fputwc('\n',F);
  for(Factor *f=T->F;f;f=f->Sled){
   if(f->Znak) fputwc('-',F);
   else if(f!=T->F) fputwc('+',F);
   switch (f->C->Atr){
    case CNST:
     if(f->C->Name) fputws(f->C->Name,F);
     else fwprintf(F,L"%g",f->C->Val);
    break;
    case VARY: fputws(f->V->Name,F);
    break;
    case UNOP:
     fwprintf(F,L"%s(L",NameFunc[f->U->Func]);
     PrintForm(false,F,NULL,f->U->Arg); fputwc(')',F);
   }
  }
  fclose(F);
 }
}

Terms *FormToTerms(Form F){
 Terms *Rez,*FS,*FT; Form FU,FV;//,FT,FF,FU,FV,A,B;
 switch(F.C->Atr){ Unop *U;
  case PUST: case CNST:
   Rez=(Terms*)calloc(1,sizeof(Terms));
   Rez->F=(Factor*)calloc(1,sizeof(Factor));
   Rez->F->C=F.C;
   break;
  case IPLL: case SPLN:
   Rez=(Terms*)calloc(1,sizeof(Terms));
   Rez->F=(Factor*)calloc(1,sizeof(Factor));
   FS=FormToTerms(F.I->F);
  // FS=Norm(FS);
   if(F.C->Atr==IPLL){
    Ipll *I=(Ipll*)calloc(1,sizeof(Ipll));
    I->Atr=IPLL; I->Type=F.I->Type; I->A=F.I->A;
   // I->F=TermsToForm(FS);
    Rez->F->I=I;
   }
   else{
    Spln *I=(Spln*)calloc(1,sizeof(Spln));
    I->Atr=SPLN; I->Type=F.I->Type; I->A=F.I->A;
    I->F=TermsToForm(FS);
    Rez->F->P=I;
   }
   break;
  case VARY:
   Rez=(Terms*)calloc(1,sizeof(Terms));
   Rez->F=(Factor*)calloc(1,sizeof(Factor));
   Rez->F->V=F.V;
   break;
  case UNOP:
   Rez=(Terms*)calloc(1,sizeof(Terms));
   Rez->F=(Factor*)calloc(1,sizeof(Factor));
   FS=FormToTerms(F.U->Arg);
   FS=Norm(FS);
   U=(Unop*)calloc(1,sizeof(Unop));
   U->Atr=UNOP; U->Func=F.U->Func;
   U->Arg=TermsToForm(FS);
   Rez->F->U=U;
   break;
  case BNOP:
   FS=FormToTerms(F.B->L);
   FT=FormToTerms(F.B->R);
   switch(F.B->Oper) {
    case 1:
     if(FT&&FT->F){
      for(Factor *f=FT->F,*Sled=f->Sled;f;f=Sled) FS->Add(f);
     }
     Rez=FS;
     break;
    case 2:
     if(FT&&FT->F){
      for(Factor *f=FT->F,*Sled=f->Sled;f;f=Sled){
       f->Znak=!f->Znak;
       FS->Add(f);
     }
     Rez=FS;
     if(FS){
      Terms *r; for(r=FS;r&&r->Sled;r=r->Sled);
      FT->F->Znak=!FT->F->Znak; r->Sled=FT;
     }
     break;
    case 3:
     FU=F.B->L; FV=F.B->R;
     Form LL,LR,RL,RR; Terms *TLL,*TLR,*TRL,*TRR,*r;
     if((FU.C->Atr==BNOP)&&(FU.B->Oper<3)&&(FV.C->Atr==BNOP)&&(FV.B->Oper<3)){
      //Form LL,LR,RL,RR; Terms *TLL,*TLR,*TRL,*TRR,*r;
      switch(FU.B->Oper){
       case 1:
        switch(FV.B->Oper){
         case 1:
          LL=FU.B->L*FV.B->L;  TLL=FormToTerms(LL);
          LR=FU.B->L*FV.B->L;  TLR=FormToTerms(LR);
          RL=FU.B->L*FV.B->L;  TRL=FormToTerms(RL);
          RR=FU.B->L*FV.B->L;  TRR=FormToTerms(RR);
          break;
         case 2:
          LL=FU.B->L*FV.B->L;  TLL=FormToTerms(LL);
          LR=-FU.B->L*FV.B->L; TLR=FormToTerms(LR);
          RL=FU.B->L*FV.B->L;  TRL=FormToTerms(RL);
          RR=-FU.B->L*FV.B->L; TRR=FormToTerms(RR);
          break;
        }
        break;
       case 2:
        switch(FV.B->Oper){
         case 1:
          LL=FU.B->L*FV.B->L;  TLL=FormToTerms(LL);
          LR=FU.B->L*FV.B->L;  TLR=FormToTerms(LR);
          RL=-FU.B->L*FV.B->L; TRL=FormToTerms(RL);
          RR=-FU.B->L*FV.B->L; TRR=FormToTerms(RR);
          break;
         case 2:
          LL=FU.B->L*FV.B->L;  TLL=FormToTerms(LL);
          LR=-FU.B->L*FV.B->L; TLR=FormToTerms(LR);
          RL=-FU.B->L*FV.B->L; TRL=FormToTerms(RL);
          RR=FU.B->L*FV.B->L;  TRR=FormToTerms(RR);
          break;
        }
        break;
      }
      for(r=TLL;r&&r->Sled;r=r->Sled);
      if(TLR->F->Znak) TLR->F->Znak=!TLR->F->Znak;
      r->Sled=TLR;
      for(;r&&r->Sled;r=r->Sled);
      if(TRL->F->Znak) TRL->F->Znak=!TRL->F->Znak;
      r->Sled=TRL;
      for(;r&&r->Sled;r=r->Sled);
      if(TRR->F->Znak) TRR->F->Znak=!TRR->F->Znak;
      r->Sled=TRR;
      Rez=TLL;
     }
     else if((FV.C->Atr==BNOP)&&(FV.B->Oper<3)){
     // Form LL,LR,RL,RR; Terms *TLL,*TLR,*TRL,*TRR,*r;
      switch(FU.B->Oper){
       case 1:
        switch(FV.B->Oper){
         case 1:
          LL=FU.B->L*FV.B->L;  TLL=FormToTerms(LL);
          LR=FU.B->L*FV.B->L;  TLR=FormToTerms(LR);
          break;
         case 2:
          LL=FU.B->L*FV.B->L;  TLL=FormToTerms(LL);
          LR=-FU.B->L*FV.B->L; TLR=FormToTerms(LR);
          break;
        }
        break;
      }
      for(r=TLL;r&&r->Sled;r=r->Sled);
      if(TLR->F->Znak) TLR->F->Znak=!TLR->F->Znak;
      r->Sled=TLR;
      Rez=TLL;
     }
     else if((FU.C->Atr==BNOP)&&(FU.B->Oper<3)){
     // Form LL,LR,RL,RR; Terms *TLL,*TLR,*TRL,*TRR,*r;
      switch(FU.B->Oper){
       case 1:
        switch(FV.B->Oper){
         case 1:
          LL=FU.B->L*FV.B->L;  TLL=FormToTerms(LL);
          RL=FU.B->L*FV.B->L;  TLR=FormToTerms(LR);
          break;
         case 2:
          LL=FU.B->L*FV.B->L;  TLL=FormToTerms(LL);
          RL=-FU.B->L*FV.B->L; TLR=FormToTerms(LR);
          break;
        }
        break;
      }
      for(r=TLL;r&&r->Sled;r=r->Sled);
      if(TLR->F->Znak) TLR->F->Znak=!TLR->F->Znak;
      r->Sled=TLR;
      Rez=TLL;
     }
     else{
      LL=FU*FV;  Rez=FormToTerms(LL);
     }
//     F=FU*FV;
     break;
//    case 4: F=FU/FV; break;
//    case 5: F=FU%FV; break;
//    case 6: F=FU^FV; break;
   } break;
//  case SUMM:
//   if((FS=FormTras(F.S->F))==Nul) F.C=Nul;
//   else {
//    A=FormTras(F.S->a); B=FormTras(F.S->b);
//    F.S=TakeSumm(FS,F.S->i,A,B);
//   } break;
//  case INTG:
//   if((FS=FormTras(F.G->f))==Nul) F.C=Nul;
//   else if(F.G->Type==DEFINE){
//    A=FormTras(F.G->a); B=FormTras(F.G->b);
//    F.G=TakeIntg(FS,F.G->x,A,B);
//   }
//   else
//    F.G=TakeIntg(FS,F.G->x);
//   break;
 } return Rez;
} */
//---------------------------------------------------------------------------
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
    case 5: F=FU%FV; break;
    case 6: F=FU^FV; break;
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
/* все переменные, имя которых содержит знак L"'" заменяются */
/* их формульными значениями                                */
 Form FT,FF,FU,FV,FS,A,B;
 switch(F.C->Atr) {
  case PUST: case CNST: case IPLL: case SPLN: break;
  case VARY:
   if(F.V->Znach.C==Pust) break;
   if(wcschr(F.V->Name,'\''))
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
    case 5: F=FU%FV; break;
    case 6: F=FU^FV; break;
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
     swprintf(Inf,L"_%s",F.V->Name); V=TakeVary(Inf);
     V->Znach=FV; F.V=V;
    }
    if(F.V->Znach.C==Nul) F.C=Nul;
    else if(REPVARNACNST&&(F.V->Znach.C->Atr==CNST)) F.C=F.V->Znach.C;
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
    case 5: F=FU%FV; break;
    case 6: F=FU^FV; break;
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
Form RemoveItem(Form F,Vary *V){
//убираем слагаемое - переменную V из формулы F, предполагая, что такое слагаемое есть
 if(F.C->Atr==BNOP){
  if(F.B->Oper==1){
   if(F.B->L.V==V)
    F=F.B->R;
   else if(F.B->R.V==V)
    F=F.B->L;
   else{
    if(VarInForm(false,F.B->L,V))
     F.B->L=RemoveItem(F.B->L,V);
    else if(VarInForm(false,F.B->R,V))
     F.B->R=RemoveItem(F.B->R,V);
  }}
  else if(F.B->Oper==2){
   if(F.B->L.V==V)
    F=-F.B->R;
   else if(F.B->R.V==V)
    F=F.B->L;
   else{
    if(VarInForm(false,F.B->L,V))
     F.B->L=RemoveItem(F.B->L,V);
    else if(VarInForm(false,F.B->R,V))
     F.B->R=RemoveItem(F.B->R,V);
 }}}
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
//     swprintf(Inf,L"_%s",F.V->Name); V=TakeVary(Inf);
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
    case 5: F=FU%FV; break;
    case 6: F=FU^FV; break;
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
    case 5: F=FU%FV; break;
    case 6: F=FU^FV; break;
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

