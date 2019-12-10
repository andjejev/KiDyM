#include "CA.h"
#include "EditList.h"
#include <io.h>

#define SYNHRON  false
#define INTERJER true
// ќператор позвол€ет размножить элементы, переменные и члены списков
// инструкций и операторов, в обозначени€х которых имеетс€ комбинаци€
//           $<индекс-символ>, или $(<формула от индексов>)
// где $ - признак индексируемости переменной или элемента, а
// <индекс-символ> - буква, обозначающа€ сам индекс,
// (<формула от индексов>) - формула, содержаща€ <индексы-символы>,
// константы и переменные, раные посто€нным величинам, например,
//  a$i=b$i+c$k; ("a i-тое равно b i-тое плюс c k-тое")                      (1)
//  P$k.fi$k=m$k*g*sin(alfa$i);                                              (2)
//  D$(2*i+1)=2*D$(2*i); ("D два i плюс первое рано удвоенному D два i-тое") (3)
// —интаксис самого оператора следующий
//      –ј«ћЌќ∆»“№ : <индекс-символ1>(<ќ“1>,<ƒќ1>[,<Ўј√1>]),
//                   <индекс-символ2>(<ќ“2>,<ƒќ2>[,<Ўј√2>]),...;,
// например,
//      –ј«ћЌќ∆»“№ : i(1,5),k(2,5,1);                       (4)
// ѕо умолчанию шаг индекса равен 1, поэтому при таком значении он может
// не указыватьс€.
// ќператор просматривает списки элементов, переменных и их формулы,
// а также списки инструкций Ќј„јЋ№Ќџ≈ ( ќЌ≈„Ќ≤џ≈) ”—Ћќ¬»я, јЌ»ћј÷»я,
// ѕќ ј«ј“№, —–ј¬Ќ»“№, —ќ¬ћ≈—“»“№ и генерирует элементы и переменные
// со своими формулами, а также формирует новые списки инструкций,
// создава€ в пам€ти ѕ  соответствующие объекты с именами, которые
// получаютс€ из исходных последовательной заменой комбинаций символов
// $<индекс-символ> числами ќ“,ќ“+Ўј√,ќ“+Ўј√*2,... или
// индексных формул их значени€ми при подстановке в них вместо индексов таких
// же чисел в соотвествии с границами и шагом, заданными дл€ <индекс-символа>
// в операторе.
// ѕри этом все индексы растут синхронно и одновременно от своего нижнего
// значени€ с указанным шагом до своего верхнего значени€. ≈сли в переменную
// и (или) в ее формулу или в элемент, в формулу его координаты или его
// формульное значение или в индексную формулу входит несколько выражений
// $<индекс-символ> с разными <индекс-символами>, то соответствующа€
// переменна€ и элемент будут созданы только в том случае, если все (!)
// индексы прин€ли значени€ из своих диапазонов. ≈сли значение хот€ бы
// одного индекса выходит за его диапазон, генераци€ соответствующих объектов
// прекращаетс€. Ќапример, приведенный выше оператор (4) вместо выражений
// (1) и (2) создаст
// a1=b1+c2; a2=b2+c3; a3=b3+c4; a4=b4+c5;
// P2.fi2=m2*g*sin(alfa1); P3.fi3=m3*g*sin(alfa2);
// P4.fi4=m4*g*sin(alfa3); P5.fi5=m5*g*sin(alfa4);
// D3=2*D2;  D5=2*D4; D7=2*D6; D9=2*D8; D11=2*D10;
extern List *L; extern char SimbInd;
extern char ErrFile[],OprFile[],Inf[],WORKDIR[],FILESHAP[];
extern Cnst *Pust; extern FILE *OPRFILE;
static char FullHead[256];
extern TEditList *EditList;

class VarElem{
 public:
  VarElem *Sled;
 private:
  char *Prot,*Form; VarElem *E;
 public:
  void FreeProt(){ if(Prot) free(Prot); };
  void FreeForm(){ if(Form) free(Form); };
  void SetProt(Vary *V);
  void SetProt(Elem *E);
  void SetForm(Vary *V);
  void SetForm(Elem *E);
  void SetElem(VarElem *e){ E=e; };
  char *GetProt(){ return Prot; }
  char *GetForm(){ return Form; }
};

void VarElem::SetProt(Vary *V){
 AnsiString S1,S2;
 S1=V->Name; S1+="="; FormToStr(V->Znach,&S2); S1+=S2;
 Prot=(char *)calloc(S1.Length()+1,SzC); strcpy(Prot,S1.c_str());
}

void VarElem::SetForm(Vary *V){
 AnsiString S1,S2;
 S1=V->Name; S1+="="; FormToStr(V->Znach,&S2); S1+=S2;
 Form=(char *)calloc(S1.Length()+1,SzC); strcpy(Form,S1.c_str());
}

void VarElem::SetProt(Elem *E){
 AnsiString S1,S2;
 S1=E->Name; S1+="="; FormToStr(E->Znach,&S2); S1+=S2;
 Prot=(char *)calloc(S1.Length()+1,SzC); strcpy(Prot,S1.c_str());
}

void VarElem::SetForm(Elem *E){
 AnsiString S1,S2;
 S1=E->Name; S1+="="; FormToStr(E->Znach,&S2); S1+=S2;
 Form=(char *)calloc(S1.Length()+1,SzC); strcpy(Form,S1.c_str());
}

class TListVE{
 private:
  VarElem *V,*E,**Pv,**Pe;
 public:
  TListVE(){ V=NULL; E=NULL; Pv=&V; Pe=&E; };
  void Add(Vary *v);
  void Add(Elem *e);
  void AddProt(Vary *v);
  void AddProt(Elem *e);
  void View();
  void VtoFile();
  void EtoFile();
 ~TListVE();
};

void TListVE::VtoFile(){
 Form F; char Formula[1024],*f,*s,*Name;
 for(VarElem *v=V;v;v=v->Sled){
  fputs("<tr>\n <td>",OPRFILE);
  if(v->GetProt()) StrToHTM(v->GetProt(),OPRFILE);
  fputs("</td>\n<td>",OPRFILE);
  if(v->GetForm()){
//   StrToHTM(v->GetForm(),OPRFILE);
   for(Name=s=v->GetForm();*s&&(*s!='=');s++); *s++='\0';
   strcpy(Formula,s);
   for(f=Formula;*f;f++)
    switch(*f){
     case '[': *f='('; break;
     case ']': *f=')'; break;
     case '{': *f='('; break;
     case '}': *f=')'; break;
    }
   F=StrToForm(Formula); PrintForm(false,OPRFILE,Name,F); *s='=';
}}}

void TListVE::EtoFile(){
 Form F; char Formula[1024],*f,*s,*Name;
 for(VarElem *e=E;e;e=e->Sled){
  fputs("<tr>\n <td>",OPRFILE);
  if(e->GetProt()) StrToHTM(e->GetProt(),OPRFILE);
  fputs("</td>\n<td>",OPRFILE);
  if(e->GetForm()){
//   StrToHTM(e->GetForm(),OPRFILE);
   for(Name=s=e->GetForm();*s&&(*s!='=');s++); *s++='\0';
   strcpy(Formula,s);
   for(f=Formula;*f;f++)
    switch(*f){
     case '[': *f='('; break;
     case ']': *f=')'; break;
     case '{': *f='('; break;
     case '}': *f=')'; break;
    }
   F=StrToForm(Formula);
   PrintForm(false,OPRFILE,Name,F); *s='=';
}}}

TListVE::~TListVE(){
 for(VarElem *v;v=V;){ v->FreeProt(); v->FreeForm(); V=v->Sled; free(v); }
 for(VarElem *e;e=E;){ e->FreeProt(); e->FreeForm(); E=e->Sled; free(e); }
 Pv=&V; Pe=&E;
}

void TListVE::AddProt(Vary *V){ VarElem *v;
 *Pv=v=(VarElem *)calloc(1,sizeof(VarElem)); Pv=(VarElem **)v;
 v->SetProt(V);
}

void TListVE::AddProt(Elem *E){ VarElem *e;
 *Pe=e=(VarElem *)calloc(1,sizeof(VarElem)); Pe=(VarElem **)e;
 e->SetProt(E);
}

void TListVE::Add(Vary *V){ VarElem *v;
 *Pv=v=(VarElem *)calloc(1,sizeof(VarElem)); Pv=(VarElem **)v;
 v->SetForm(V);
}

void TListVE::Add(Elem *E){ VarElem *e;
 *Pe=e=(VarElem *)calloc(1,sizeof(VarElem)); Pe=(VarElem **)e;
 e->SetForm(E);
}

void ViewElem(void **R){
 TEditList *EditListOld=EditList; FILE *F;
 int i,*MNN=(int *)calloc(3,SzI),H=Screen->Height-100;
 char *FileSh="shapkae",*Format="%-s%-s",**Head=(char **)calloc(1,SzV),
  *Fprn="index.prn",*Help=(char *)calloc(256,SzV),Dir[256],*s,
  *FH=(char *)calloc(256,SzC),Predki[32],Deti[16];
 if(F=fopen(FileSh,"w")){
  strcpy(Predki,"»сходные выражени€"); StrWINtoDOS(Predki);
  strcpy(Deti,"Ёлементы"); StrWINtoDOS(Deti);
  sprintf(Inf,"%s\xb3%s\n",Predki,Deti); fputs(Inf,F); fclose(F);
 }
 sprintf(FH,"ќператор \"–ј«ћЌќ∆»“№\" сгенерировал\n"
  "элементы из индексированных выражений");
 Head[0]=(char *)calloc(256,SzC); strcpy(Head[0],"Ёлементы");
 for(i=0;i<2;i++) MNN[i]=i+1;
 strcpy(Dir,WORKDIR); if(s=strrchr(Dir,'\\')) strcpy(s,"\\Help");
 sprintf(Help,"%s\\ViewIndex.htm",Dir);
 EditList=new TEditList(R,MNN,false,NULL,false,NULL,Format,FileSh,Head,
  Fprn,NULL,NULL,NULL,1,-1,40,-1,H,FH,NULL,0,Help,NULL,NULL);
 if(EditList->Diagnos()){
  Application->CreateForm(__classid(TFormEditList),&FormEditList);
  FormEditList->ShowModal();
 }
 if(MNN) free(MNN); if(Help) free(Help);
 if(Head){ if(Head[0]) free(Head[0]); free(Head); }
 delete EditList; EditList=EditListOld;
}

void TListVE::View(){
 FILE *F; int i,*MNN=(int *)calloc(3,SzI),H=Screen->Height-60;
 char *FileSh="shapkav",*Format="%-s%-s%p",**Head=(char **)calloc(2,SzV),*s,
  *Fprn="index.prn",*Help=(char *)calloc(256,SzV),Predki[32],Deti[16],Dir[256];
 void (**Proc)(void **)=(void (**)(void **))calloc(1,SzV);
 if(F=fopen(FileSh,"w")){
  strcpy(Predki,"»сходные выражени€"); StrWINtoDOS(Predki);
  strcpy(Deti,"ѕеременные"); StrWINtoDOS(Deti);
  sprintf(Inf,"%s\xb3%s\n",Predki,Deti); fputs(Inf,F); fclose(F);
 }
 Proc[0]=ViewElem;
 sprintf(FullHead,"ќператор \"–ј«ћЌќ∆»“№\" сгенерировал "
  "переменные из индексированных выражений");
 Head[0]=(char *)calloc(256,SzC); Head[1]=(char *)calloc(256,SzC);
 for(VarElem *v=V;v;v=v->Sled) v->SetElem(E);
 for(i=0;i<2;i++) MNN[i]=i+1;
 strcpy(Head[0],"ѕеременные"); strcpy(Head[1],"Ёлементы");
 strcpy(Dir,WORKDIR); if(s=strrchr(Dir,'\\')) strcpy(s,"\\Help");
 sprintf(Help,"%s\\ViewIndex.htm",Dir);
 EditList=new TEditList((void **)&V,MNN,false,NULL,false,NULL,Format,FileSh,Head,
  Fprn,NULL,NULL,Proc,1,-1,0,-1,H,FullHead,NULL,0,Help,NULL,NULL);
 if(EditList->Diagnos()){
  Application->CreateForm(__classid(TFormEditList),&FormEditList);
  FormEditList->ShowModal();
 }
 if(MNN) free(MNN); if(Help) free(Help);
 if(Head){ for(i=0;i<2;i++) if(Head[i]) free(Head[i]); free(Head); }
 delete EditList;
}

class TInd{
 public:
  TInd *Sled; char I; int GS[3];
};

class TInds{
 public:
  TInd *R;
  TInds(){ R=NULL; }
  TInd *FindInd(char I);
  bool AddInd(Parm *p);
  bool IndLimit(char *Name,int &N);
  bool IndLimit(Vary *V,int &N);
  bool IndLimit(Elem *E,int &N);
  bool IndLimit(Form F,int &N);
  bool Index(Vary *V,TListVE &ListVE);
  bool Index(Elem *E,TListVE &ListVE);
  bool NewName(char *Name,int i,char *New);
  bool NewForm(Form Fv,int i,Form &F);
 ~TInds();
};

TInds::~TInds(){ for(TInd *r;r=R;){ R=r->Sled; free(r); }}

TInd *TInds::FindInd(char I){
 for(TInd *r=R;r;r=r->Sled) if(r->I==I) return r; return NULL;
}

bool TInds::AddInd(Parm *p){
 Vary *V; FILE *F; Parm *P=p; char *Head;
 TInd *Ind=(TInd *)calloc(1,sizeof(TInd)); Ind->Sled=R; R=Ind;
 if(p&&p->Nam) Ind->I=*p->Nam;
 else{
  if(OPRFILE=fopen(OprFile,access(OprFile,0)?"w":"a")){
   sprintf(Inf,"¬ операторе &quot;–ј«ћЌќ∆»“№&quot;<br>\n"
    "не указано наименование индекса");
   Fputs(Inf,F); fclose(F);
  }
  return false;
 }
 p=p->R;
 if(p->Nam){
  if(!FindVary(p->Nam,&V)){
   if(F=fopen(OprFile,"a")){
    sprintf(Inf,"»м€ &quot;%s&quot;, указанное в операторе &quot;–ј«ћЌќ∆»“№&quot;<br>\n"
     "как начальное значение индекса %s,<br>\n"
     "не найдено среди переменных задачи",p->Nam,Ind->I);
    Fputs(Inf,F); fclose(F);
   }
   return false;
  }
  else Ind->GS[0]=(int)Round(Val(V->Znach));
 }
 else Ind->GS[0]=(int)Round(p->Val);
 p=p->Sled;
 if(p){
  if(p->Nam){
   if(!FindVary(p->Nam,&V)){
    if(F=fopen(OprFile,"a")){
     sprintf(Inf,"»м€ &quot;%s&quot;, указанное в операторе &quot;–ј«ћЌќ∆»“№&quot;<br>\n"
      "как конечное значение индекса %s,<br>\n"
      "не найдено среди переменных задачи",p->Nam,Ind->I);
     Fputs(Inf,F); fclose(F);
    }
    return false;
   }
   else Ind->GS[1]=(int)Round(Val(V->Znach));
  }
  else Ind->GS[1]=(int)Round(p->Val);
 }
 if(p&&p->Sled){
  p=p->Sled;
  if(p){
   if(p->Nam){
    if(!FindVary(p->Nam,&V)){
     if(F=fopen(OprFile,"a")){
      sprintf(Inf,"»м€ &quot;%s&quot;, указанное в операторе &quot;–ј«ћЌќ∆»“№&quot;<br>\n"
       "как шаг значений индекса %s,<br>\n"
       "не найдено среди переменных задачи",p->Nam,Ind->I);
      Fputs(Inf,F); fclose(F);
     }
     return false;
    }
    else Ind->GS[2]=(int)Round(Val(V->Znach));
   }
   else Ind->GS[2]=(int)Round(p->Val);
  }
  else Ind->GS[2]=1;
 }
 else Ind->GS[2]=1;
 if((Ind->GS[0]<0)||(Ind->GS[1]<0)||(Ind->GS[2]<0)||(Ind->GS[1]<=Ind->GS[0])){
  if(F=fopen(OprFile,"a")){
   sprintf(Inf,"«начени€ границ индексов должны быть положительными,<br>\n"
    "нижн€€ граница должна быть меньше верхней, шаг должен быть >0.<br>\n"
    "ƒл€ индекса &quot;%s&quot;, в операторе &quot;–ј«ћЌќ∆»“№&quot; это не так",
    P->Nam);
   Fputs(Inf,F); fclose(F);
  }
  return false;
 }
 return true;
}

bool TInds::IndLimit(Form F,int &N){ TInd *I;
 switch(F.C->Atr){
  case CNST: case PUST: break;
  case VARY:
   for(char *v=F.V->Name;*v;v++){
    if(*v==SimbInd){ v++;
     if(I=FindInd(*v)){
      if(((I->GS[1]-I->GS[0])/I->GS[2]+1)<N)
       N=(I->GS[1]-I->GS[0])/I->GS[2]+1;
     }
     else N=0;
   }}
   IndLimit(F.V,N);
   break;
  case UNOP: IndLimit(F.U->Arg,N); break;
  case BNOP: IndLimit(F.B->L,N); IndLimit(F.B->R,N); break;
  case IFOP:
   IndLimit(F.L->U,N);  IndLimit(F.L->V,N);
   IndLimit(F.L->FT,N); IndLimit(F.L->FF,N);
   break;
  case SUMM: break;
  case IPLL: IndLimit(F.I->F,N); break;
  case SPLN: IndLimit(F.P->F,N); break;
 }
 return N;
}

bool TInds::IndLimit(char *Name,int &N){ TInd *I; FILE *F; 
 for(char *s=Name;*s;s++){
  if(*s==SimbInd){ s++;
   if(I=FindInd(*s)){
    if(((I->GS[1]-I->GS[0])/I->GS[2]+1)<N)
     N=(I->GS[1]-I->GS[0])/I->GS[2]+1;
   }
   else{
    if(F=fopen(ErrFile,"a")){
     sprintf(Inf,"»м€ \"%s\"\n"
      "содержит в качестве индекса переменную,\n"
      "котора€ не указана в операторе \"–ј«ћЌќ∆»“№\"",Name);
     Application->MessageBox(Inf,"ќшибка в индексе:",MB_OK);
     strcat(Inf,"\n"); Fputs(Inf,F); fclose(F);
    }
    N=0;
 }}}
 return N;
}

bool TInds::IndLimit(Vary *V,int &N){ TInd *I; FILE *F; 
 for(char *v=V->Name;*v;v++){
  if(*v==SimbInd){ v++;
   if(I=FindInd(*v)){
    if(((I->GS[1]-I->GS[0])/I->GS[2]+1)<N)
     N=(I->GS[1]-I->GS[0])/I->GS[2]+1;
   }
   else{
    if(F=fopen(ErrFile,"a")){
     sprintf(Inf,"»м€ переменной \"%s\"\n"
      "содержит в качестве индекса переменную,\n"
      "котора€ не указана в операторе \"–ј«ћЌќ∆»“№\"",V->Name);
     Application->MessageBox(Inf,"ќшибка в индексе:",MB_OK);
     strcat(Inf,"\n"); Fputs(Inf,F); fclose(F);
    }
    return 0;
 }}}
 if(!IndLimit(V->Znach,N)){
  if(F=fopen(ErrFile,"a")){
   sprintf(Inf,"‘ормула дл€ переменной \"%s\"\n"
    "содержит переменные с индексами, которые не указаны\n"
    "в операторе \"–ј«ћЌќ∆»“№\"",V->Name);
   Application->MessageBox(Inf,"ќшибка в индексе:",MB_OK);
   strcat(Inf,"\n"); Fputs(Inf,F); fclose(F);
 }}
 return N;
}

bool TInds::IndLimit(Elem *E,int &N){ TInd *I; FILE *F;
 for(char *e=E->Name;*e;e++){
  if(*e==SimbInd){ e++;
   if(I=FindInd(*e)){
    if(((I->GS[1]-I->GS[0])/I->GS[2]+1)<N)
     N=(I->GS[1]-I->GS[0])/I->GS[2]+1;
   }
   else return 0;
 }}
 IndLimit(E->K,N);
 if(!IndLimit(E->Znach,N)){
  if(F=fopen(OprFile,"a")){
   sprintf(Inf,"‘ормула дл€ элемента &quot;%s&quot;<br>\n"
    "содержит переменные с индексами, которые не указаны<br>\n"
    "в операторе &quot;–ј«ћЌќ∆»“№&quot;",E->Name);
   strcat(Inf,"\n"); Fputs(Inf,F); fclose(F);
 }}
 return N;
}

bool TInds::NewName(char *Name,int i,char *New){
 TInd *I; char *s=Name,*n=New;
 for(s=Name,n=New;*s;s++){
  if(*s==SimbInd){ s++;
   if((I=FindInd(*s))&&((i*I->GS[2]+I->GS[0])<=I->GS[1]))
    n+=sprintf(n,"%d",i*I->GS[2]+I->GS[0]);
   else return false;
  }
  else *n++=*s;
 }
 *n='\0'; return true;
}

bool TInds::NewForm(Form Fv,int i,Form &Fn){
 switch(Fv.C->Atr){ char Name[256]; Form F,L,U,V,FT,FF; Arra *A; TInd *I;
  case PUST: case CNST: Fn=Fv; return true;
  case VARY:
   if((!Fv.V->Name[1])&&(I=FindInd(Fv.V->Name[0]))){
    Fn.C=TakeCnst(i*I->GS[2]+I->GS[0]); Fv.V->Znach=Fn;
   }
   else{
    if(!NewName(Fv.V->Name,i,Name)) return false;
    Fn.V=TakeVary(Name);
//ѕородим первообразные
    for(char *s=Name;s=strchr(s+1,'\'');*s='\''){ *s='\0'; TakeVary(Name); }
   }
   break;
  case UNOP:
   if(!NewForm(Fv.U->Arg,i,F)) return false;
   switch(Fv.U->Func){
    case  1: Fn=-F;        break;
    case  2: Fn=sqrt(F);   break;
    case  3: Fn=sin(F);    break;
    case  4: Fn=cos(F);    break;
    case  5: Fn=tg(F);     break;
    case  6: Fn=arcsin(F); break;
    case  7: Fn=arctg(F);  break;
    case  8: Fn=exp(F);    break;
    case  9: Fn=ln(F);     break;
    case 10: Fn=sh(F);     break;
    case 11: Fn=ch(F);     break;
    case 12: Fn=arsh(F);   break;
    case 13: Fn=abs(F);    break;
    case 14: Fn=rand(F);
   }
   break;
  case BNOP:
   if(!NewForm(Fv.B->L,i,L)) return false;
   else if(!NewForm(Fv.B->R,i,F)) return false;
   switch(Fv.B->Oper){
    case  1: Fn=L+F; break;
    case  2: Fn=L-F; break;
    case  3: Fn=L*F; break;
    case  4: Fn=L/F; break;
    case  5: Fn=L^F;
   }
   break;
  case IFOP:
   if(!NewForm(Fv.L->U,i,U)) return false;
   else if(!NewForm(Fv.L->V,i,V)) return false;
   else if(!NewForm(Fv.L->FT,i,FT)) return false;
   else if(!NewForm(Fv.L->FF,i,FF)) return false;
   Fn.L=TakeIfop(Fv.L->Oper,U,V,FT,FF); break;
  case SUMM: break;
  case IPLL:
   if(!NewName(Fv.I->A->Name,i,Name)) return false;
   if(!FindArra(Name,&A)) A=Fv.I->A;
   if(!NewForm(Fv.I->F,i,F)) return false;
   Fn.I=TakeIpll(Fv.I->Type,A,F);
   break;
  case SPLN:
   if(!NewName(Fv.P->A->Name,i,Name)) return false;
   if(!FindArra(Name,&A)) A=Fv.P->A;
   if(!NewForm(Fv.P->F,i,F)) return false;
   Fn.P=TakeSpln(Fv.P->Type,A,F);
   break;
 }
 return true;
}

int Nstr,Kstr=70;

bool TInds::Index(Vary *V,TListVE &ListVE){
 int N=MaxInt; char Name[256]; Form F; Vary *v;
 if(!IndLimit(V,N)) return false;
 for(int i=0;i<N;i++){
  if(!NewName(V->Name,i,Name)) return false;
  if(!NewForm(V->Znach,i,F)) return false;
  v=TakeVary(Name); v->Znach=F; ListVE.Add(v);
//ѕородим первообразные
  for(char *s=Name;s=strchr(s+1,'\'');*s='\''){ *s='\0'; TakeVary(Name); }
 }
 return true;
}

bool TInds::Index(Elem *E,TListVE &ListVE){
 int N=MaxInt; char Name[256],*s; Form F; Elem *e; Vary *v;
 if(!IndLimit(E,N)) return false;
 for(int i=0;i<N;i++){
  if(!NewName(E->Name,i,Name)) return false;
  if(s=strchr(Name,'.')){
   v=TakeVary(s+1);
//ѕородим первообразные
   for(char *s=Name;s=strchr(s+1,'\'');*s='\''){ *s='\0'; TakeVary(Name); }
  }
  if(!NewForm(E->Znach,i,F)) return false;
  e=TakeElem(Name); e->Znach=F; e->K=v; ListVE.Add(e);
 }
 return true;
}

bool IsSimbInd(Root *I){
 for(Parm *P=I->First;P;P=P->Sled){
  if(strchr(P->Nam,SimbInd)) return true;
  for(Parm *p=P->R;p;p=p->Sled){
   if(p->Nam&&strchr(p->Nam,SimbInd)) return true;
 }}
 return false;
}

Parm *IsSimbInd(Parm *R){
 for(Parm *p=R;p;p=p->Sled) if(p->Nam&&strchr(p->Nam,SimbInd)) return p;
 return NULL;
}

bool IndexInstr(char *InstrName,TInds &Inds,bool Type){
 Root *I,*N; Vary *V; FILE *F;
 for(I=L->Inst;I;I=I->Sled){
  LowToUpp(I->Name);
  if(!strcmp(I->Name,InstrName)){
   Parm *P,*p,**Pf,*f; char *h,*n,*b,Buf[256]; TInd *Indi,*Indj;
   if(IsSimbInd(I)){//в инструкции есть индексы
    N=(Root *)calloc(1,sizeof(Root)); N->Sled=I->Sled; I->Sled=N;
    Pf=&(N->First); I->Name[0]='@'; //N->Line=I->Line;
    N->Name=strcpy((char *)calloc(80,SzC),InstrName);
    for(P=I->First;P;P=P->Sled){ int Nmax=MaxInt;
     if(h=strchr(P->Nam,SimbInd)){//позици€ символа
      if(Indi=Inds.FindInd(h[1])){//границы индексов
       if(!Inds.IndLimit(P->Nam,Nmax)) return false;
       for(int ii=0;ii<Nmax;ii++){
        int i=Indi->GS[0]+ii*Indi->GS[2];
//       for(int i=Indi->GS[0];i<=Indi->GS[1];i+=Indi->GS[2]){
        *Pf=f=(Parm *)calloc(1,sizeof(Parm)); Pf=(Parm **)f; f->Line=P->Line;
        if(!Inds.NewName(P->Nam,ii,Buf)) return false;
/*        for(n=P->Nam,b=Buf;*n;n++){
         if(*n==SimbInd){ n++; b+=sprintf(b,"%d",i); } else *b++=*n;
        }
        *b='\0'; */
        f->Nam=strcpy((char *)calloc(strlen(Buf)+1,SzC),Buf);
        if(P->R){ Parm *ff,**PfR=&f->R; char *hh; int j;
         for(p=P->R;p;p=p->Sled){
          if(p->Nam){ int Nm=MaxInt;
           if(hh=strchr(p->Nam,SimbInd)){
            if(Indj=Inds.FindInd(hh[1])){
             if(Type==SYNHRON){//синхронные индексы
              if(!Inds.IndLimit(p->Nam,Nm)) return false;
              j=Indj->GS[0]+(int)Round((i-Indi->GS[0])/Indi->GS[2])*Indj->GS[2];
              if(j>Indj->GS[1]){
               if(F=fopen(OprFile,"a")){
                sprintf(Inf,
                 "»ндекс %c элемента %s имеет число значений меньшее,<br>\n"
                 "чем индекс %c элемента %s его внутреннего списка",
                 h[1],P->Nam,hh[1],p->Nam);
                Fputs(Inf,F); fclose(F);
               }
               return false;
              }
              *PfR=ff=(Parm *)calloc(1,sizeof(Parm)); PfR=(Parm **)ff;
              ff->Line=p->Line;
              if(!Inds.NewName(p->Nam,ii,Buf)) return false;
/*              for(n=p->Nam,b=Buf;*n;n++){
              if(*n==SimbInd){ n++; b+=sprintf(b,"%d",j); } else *b++=*n;
             }
             *b='\0';*/
              ff->Nam=strcpy((char *)calloc(strlen(Buf)+1,SzC),Buf);
             }
             else{//асинхронные индексы
              for(int j=Indj->GS[0];j<=Indj->GS[1];j+=Indj->GS[2]){
               *PfR=ff=(Parm *)calloc(1,sizeof(Parm)); PfR=(Parm **)ff;
               ff->Line=p->Line;
               for(n=p->Nam,b=Buf;*n;n++){
                if(*n==SimbInd){ n++; b+=sprintf(b,"%d",j); } else *b++=*n;
               }
               *b='\0'; ff->Nam=strcpy((char *)calloc(strlen(Buf)+1,SzC),Buf);
            }}}
            else{
             if(F=fopen(OprFile,"a")){
              sprintf(Inf,
               "»ндекс %c элемента %s не определен в операторе –ј«ћЌќ∆»“№",
               h[1],P->Nam);
              Fputs(Inf,F); fclose(F);
             }
             return false;
           }}
           else{//нет индекса в имени
            *PfR=ff=(Parm *)calloc(1,sizeof(Parm)); PfR=(Parm **)ff;
            ff->Line=p->Line;
            ff->Nam=strcpy((char *)calloc(strlen(P->Nam)+1,SzC),p->Nam);
          }}
          else{//числовое значение параметра
           *PfR=ff=(Parm *)calloc(1,sizeof(Parm)); PfR=(Parm **)ff;
           ff->Line=p->Line; ff->Val=p->Val;
     }}}}}}
     else{//нет индекса
      if(P->R){
       if(p=IsSimbInd(P->R)){
        Parm *ff,**PfR; char *hh,*Nam=p->Nam;
        if(Type==SYNHRON){
         if(h=strchr(p->Nam,SimbInd)){
          if(Indi=Inds.FindInd(h[1])){
           for(int i=Indi->GS[0];i<=Indi->GS[1];i+=Indi->GS[2]){
            *Pf=f=(Parm *)calloc(1,sizeof(Parm)); Pf=(Parm **)f; f->Line=p->Line;
            f->Nam=strcpy((char *)calloc(strlen(P->Nam)+1,SzC),P->Nam); PfR=&f->R;
            for(Parm *p=P->R;p;p=p->Sled){
             *PfR=ff=(Parm *)calloc(1,sizeof(Parm));
             PfR=(Parm **)ff;
             ff->Line=p->Line;
             if(p->Nam){
              if(hh=strchr(p->Nam,SimbInd)){
               if(Indj=Inds.FindInd(hh[1])){
                int j=Indj->GS[0]+(int)Round((i-Indi->GS[0])/Indi->GS[2])*Indj->GS[2];
                if(j>Indj->GS[1]){
                 if(F=fopen(OprFile,"a")){
                  sprintf(Inf,
                   "»ндекс %c элемента %s имеет число значений меньшее,<br>\n"
                   "чем индекс %c элемента %s из того же списка",
                   h[1],Nam,hh[1],p->Nam);
                  Fputs(Inf,F); fclose(F);
                 }
                 return false;
                }
                for(n=p->Nam,b=Buf;*n;n++){
                 if(*n==SimbInd){ n++; b+=sprintf(b,"%d",j); } else *b++=*n;
                }
                *b='\0'; ff->Nam=strcpy((char *)calloc(strlen(Buf)+1,SzC),Buf);
              }}
              else//нет индекса
               ff->Nam=strcpy((char *)calloc(strlen(p->Nam)+1,SzC),p->Nam);
             }
             else//числовое  значение
              ff->Val=p->Val;
        }}}}}
        else{//несинхронные индексы
         *Pf=f=(Parm *)calloc(1,sizeof(Parm)); Pf=(Parm **)f; f->Line=p->Line;
         f->Nam=strcpy((char *)calloc(strlen(P->Nam)+1,SzC),P->Nam); PfR=&f->R;
         for(Parm *p=P->R;p;p=p->Sled){
          if(p->Nam){
           if(hh=strchr(p->Nam,SimbInd)){
            if(Indi=Inds.FindInd(hh[1])){
             for(int i=Indi->GS[0];i<=Indi->GS[1];i+=Indi->GS[2]){
              *PfR=ff=(Parm *)calloc(1,sizeof(Parm)); PfR=(Parm **)ff;
              ff->Line=p->Line;
              for(n=p->Nam,b=Buf;*n;n++){
               if(*n==SimbInd){ n++; b+=sprintf(b,"%d",i); } else *b++=*n;
              }
              *b='\0'; ff->Nam=strcpy((char *)calloc(strlen(Buf)+1,SzC),Buf);
           }}}
           else{
            *PfR=ff=(Parm *)calloc(1,sizeof(Parm)); PfR=(Parm **)ff;
            ff->Line=p->Line;
            ff->Nam=strcpy((char *)calloc(strlen(p->Nam)+1,SzC),p->Nam);
          }}
          else{
           *PfR=ff=(Parm *)calloc(1,sizeof(Parm)); PfR=(Parm **)ff;
           ff->Line=p->Line; ff->Val=p->Val;
       }}}}
       else{//в списке нет переменных с индексами
        *Pf=f=(Parm *)calloc(1,sizeof(Parm)); Pf=(Parm **)f; f->Line=P->Line;
        Parm *ff,**PfR=&f->R;
        f->Nam=strcpy((char *)calloc(strlen(P->Nam)+1,SzC),P->Nam);
        for(Parm *p=P->R;p;p=p->Sled){
         *PfR=ff=(Parm *)calloc(1,sizeof(Parm)); PfR=(Parm **)ff;
         ff->Line=p->Line;
         if(p->Nam)
          ff->Nam=strcpy((char *)calloc(strlen(p->Nam)+1,SzC),p->Nam);
         else ff->Val=p->Val;
      }}}
      else{
       *Pf=f=(Parm *)calloc(1,sizeof(Parm)); Pf=(Parm **)f; f->Line=P->Line;
       f->Nam=strcpy((char *)calloc(strlen(P->Nam)+1,SzC),P->Nam);
 }}}}}}
 return true;
}

bool MultiPly(Parm *Par,TRichEdit *R){
 char *inf=Inf,*s; TInds Inds; Vary *V,*W=NULL; Elem *e,*E=NULL;
 TListVE ListVE; FILE *F;
//ƒиагностика
 if(OPRFILE=fopen(OprFile,/*access(OprFile,0)?*/"w"/*:"a"*/))
  fprintf(OPRFILE,
   "<html>\n<head>\n<title>ѕ–ќ“ќ ќЋ ќѕ≈–ј“ќ–ј \"–ј«ћЌќ∆»“№\"</title>\n"
   "<meta http-equiv=\"Content-Type\" content=\"text/html;"
   "charset=windows-1251\">\n</head>\n"
   "<body bgcolor=\"#FFFFFF\" text=\"#800000\""
   " background=\"%s/fon2.jpg\""
   " font face=\"Times New Roman, Times, serif\">\n",
    WORKDIR);
 for(Parm *p=Par;p;p=p->Sled){
  if(!p->Nam||!p->R){
   inf+=sprintf(inf,"ѕервый элемент оператора "
    "\"–ј«ћЌќ∆»“№\" должен быть наименованием индекса!\n");
   goto Ex;
  }
  Inds.AddInd(p);
 }
//—делаем список индексированных инструкуций
 if(!IndexInstr("Ќј„јЋ№Ќџ≈ ”—Ћќ¬»я",Inds,SYNHRON))  return false;
 if(!IndexInstr("Ќј„јЋ№Ќќ≈ ”—Ћќ¬»≈",Inds,SYNHRON))  return false;
 if(!IndexInstr(" ќЌ≈„Ќџ≈ ”—Ћќ¬»я",Inds,SYNHRON))   return false;
 if(!IndexInstr(" ќЌ≈„Ќќ≈ ”—Ћќ¬»≈",Inds,SYNHRON))   return false;
 if(!IndexInstr("Ќј„јЋ№Ќџ≈ «Ќј„≈Ќ»я",Inds,SYNHRON)) return false;
 if(!IndexInstr("Ќј„јЋ№Ќќ≈ «Ќј„≈Ќ»≈",Inds,SYNHRON)) return false;
 if(!IndexInstr("Ќќ¬џ≈ «Ќј„≈Ќ»я",Inds,SYNHRON))     return false;
 if(!IndexInstr("Ќќ¬ќ≈ «Ќј„≈Ќ»≈",Inds,SYNHRON))     return false;
 if(!IndexInstr("јЌ»ћј÷»я",Inds,SYNHRON))           return false;
 if(!IndexInstr("ѕќ ј«ј“№",Inds,SYNHRON))           return false;
 if(!IndexInstr("ѕ≈„ј“ј“№",Inds,SYNHRON))           return false;
 if(!IndexInstr("—–ј¬Ќ»“№",Inds,INTERJER))          return false;
 if(!IndexInstr("—ќ¬ћ≈—“»“№",Inds,INTERJER))        return false;
 if(V=L->V){ Vary *Sled;
  while(Sled=V->Sled){
   if(strchr(Sled->Name,SimbInd)){ V->Sled=Sled->Sled; Sled->Sled=W; W=Sled; }
   else V=V->Sled;
 }}
//—делаем список индексированных элементов
 while((e=L->E)&&strchr(e->Name,SimbInd)){ L->E=e->Sled; e->Sled=E; E=e; }
 if(e=L->E){ Elem *Sled;
  while(Sled=e->Sled){
   if(strchr(Sled->Name,SimbInd)){ e->Sled=Sled->Sled; Sled->Sled=E; E=Sled; }
   else e=e->Sled;
 }}
 if(W||E){
  fprintf(OPRFILE,
   "ќператор \"–ј«ћЌќ∆»“№\" сгенерировал из индексированных выражений<hr>\n");
  if(W){
   fprintf(OPRFILE,"<b>ѕеременные:</b><br>\n");
   fputs("<table width=\"100%\" border=\"1\">\n",OPRFILE);
   fputs("<tr>",OPRFILE);
   fputs("<td><div align=\"center\"><b>»сходные выражени€</b></div></td>\n"
    "<td><div align=\"center\"><b>ѕеременные</b></div></td>\n</tr>\n",OPRFILE);
   for(V=W;V;V=V->Sled){
    if(V->Znach.C!=Pust){
     ListVE.AddProt(V);
     if(!Inds.Index(V,ListVE)){
      fclose(OPRFILE); return false;
   }}}
   ListVE.VtoFile();
   fputs("</table>\n",OPRFILE);
  }
  if(E){
   fprintf(OPRFILE,"<b>Ёлементы:</b><br>\n");
   fputs("<table width=\"100%\" border=\"1\">\n",OPRFILE);
   fputs("<tr>",OPRFILE);
   fputs("<td><div align=\"center\"><b>»сходные выражени€</b></div></td>\n"
    "<td><div align=\"center\"><b>Ёлементы</b></div></td>\n</tr>",OPRFILE);
   for(e=E;e;e=e->Sled){
    ListVE.AddProt(e);
    if(!Inds.Index(e,ListVE)){ fclose(OPRFILE); return false; }
   }
   ListVE.EtoFile();
   fputs("</table\n",OPRFILE);
  }
  fputs("</font></body>\n</html>\n",OPRFILE);
 }
 fclose(OPRFILE);
 return true;
Ex:sprintf(inf,
 "ќператор \"–ј«ћЌќ∆»“№\" должен быть записан в виде:<br>\n"
 "–ј«ћЌќ∆»“№ : <индекс-символ1>(<ќ“1>,<ƒќ1>[,<Ўј√1>]),<br>\n"
 "             <индекс-символ2>(<ќ“2>,<ƒќ2>[,<Ўј√2>]),...;,<br>\n"
 "например,    –ј«ћЌќ∆»“№ : i(1,5),k(2,5,1);<br>\n");
Ex2:
 if(F=fopen(OprFile,"a")){ Fputs(Inf,F); fclose(F); }
 Application->MessageBox(Inf,"ќшибка в операторе \*–ј«ћЌќ∆»“№\*:",MB_OK);
 return false;
}
