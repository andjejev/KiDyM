#include "CA.h"
#include "EditList.h"
#include <io.h>

#define SYNHRON  false
#define INTERJER true
/*
    Оператор позволяет размножить стержни, твердые тела, дискретные элементы,
 переменные и члены списков инструкций механической модели,
 в обозначениях которых имеется комбинация
           $<индекс-символ>, или $(<формула из индексов>)
 где
  $ - признак индексируемости переменной или элемента, а
      <индекс-символ> - буква, обозначающая сам индекс,
      (<формула от индексов>) - формула, содержащая <индексы-символы>,
      константы и переменные, равные постоянным величинам, например,
  Тело$k~Тело$j|Rx(X),Ry(Y),R(Z),Sx(alfa),Sx(beta),Sx(gamma)|Jx,Jy,Jz;       (1)
  Массаi|Rx(X$i),Ry(Y$i),R(Z$i),Q(p0$i,p1$i,p2$i,p3$i)|Jx,Jy,Jz,Jxy,Jyz,Jxz; (2)
  a$k=b$k+c$j; ("a k-тое равно b k-тое плюс c j-тое")                        (3)
  P$k.fi$k=m$k*g*sin(alfa$j);                                                (4)
  D$(2*i+1)=2*D$(2*i); ("D два i плюс первое равно удвоенному D два i-тое")  (5)
 Синтаксис самого оператора следующий
      РАЗМНОЖИТЬ : <индекс-символ1>(<ОТ1>,<ДО1>[,<ШАГ1>]),
                   <индекс-символ2>(<ОТ2>,<ДО2>[,<ШАГ2>]),...;,
 например,
      РАЗМНОЖИТЬ : i(1,5),k(2,6,2),j(1,5,2);                                 (6)
    По умолчанию шаг индекса равен 1, поэтому при таком значении он может
 не указываться.
    Оператор просматривает списки твердых тел, элементов, переменных и формулы,
 а также списки инструкций НАЧАЛЬНЫЕ(ОЕ) (КОНЕЧНЫЕ(ОЕ)) УСЛОВИЯ(Е),НОВЫЕ(ОЕ)
 ЗНАЧЕНИЯ(Е), НАЧАЛЬНЫЕ(ОЕ) ЗНАЧЕНИЯ(Е), АНИМАЦИЯ, ПЕЧАТАТЬ, ПОКАЗАТЬ,
 СРАВНИТЬ, СОВМЕСТИТЬ и генерирует твердые тела, элементы и переменные
 со своими формулами, а также формирует новые  инструкции с новыми списками,
 создавая в памяти ПК соответствующие объекты с именами, которые
 получаются из исходных последовательной заменой комбинаций символов
 $<индекс-символ> числами ОТ,ОТ+ШАГ,ОТ+ШАГ*2,...,ДО или индексных формул их
 значениями при подстановке в них вместо индексов таких же чисел в
 соотвествии с границами и шагом, заданными для <индекс-символа> в операторе.
    При этом все индексы в каком-либо выражении растут синхронно и
 одновременно от своего нижнего значения до своего верхнего значения с
 указанным шагом. Если в имя переменной и (или) в ее формулу, в имя элемента,
 в формулу его координаты или его формульное значение, а также в имя твердого
 тела или в имя тела, относительно которого заданы сдвиги и повороты свзанной
 с ним системы координат, в обозначения этих сдвигов и поворотов, а также в
 обзначения их моментов инерции, входит несколько выражений $<индекс-символ>
 с разными <индекс-символами>, то соответствующие выражения будут созданы
 только в том случае, если все (!) индексы приняли значения из своих диапа-
 зонов. Если значение хотя бы одного индекса выходит за его собственный
 диапазон, генерация соответствующих объектов прекращается.
    Например, приведенный выше оператор (6) вместо выражений (1)-(5) создаст

 Тело2~Тело1|Rx(x21),Ry(y21),R(z21),Sx(alfa21),Sx(beta21),Sx(gamma21)|Jx,Jy,Jz;
 Тело4~Тело3|Rx(x43),Ry(y43),R(z43),Sx(alfa43),Sx(beta43),Sx(gamma43)|Jx,Jy,Jz;
 Тело6~Тело5|Rx(x65),Ry(y65),R(z65),Sx(alfa65),Sx(beta65),Sx(gamma65)|Jx,Jy,Jz;
 Тчк1|Rx(X1),Ry(Y1),R(Z1),Q(p01),p11,p21,p31)|Jx,Jy,Jz,Jxy,Jyz,Jxz;
 Тчк2|Rx(X2),Ry(Y2),R(Z2),Q(p02),p12,p22,p32)|Jx,Jy,Jz,Jxy,Jyz,Jxz;
 Тчк3|Rx(X3),Ry(Y3),R(Z3),Q(p03),p13,p23,p33)|Jx,Jy,Jz,Jxy,Jyz,Jxz;
 Тчк4|Rx(X4),Ry(Y4),R(Z4),Q(p04),p14,p24,p34)|Jx,Jy,Jz,Jxy,Jyz,Jxz;
 Тчк5|Rx(X5),Ry(Y5),R(Z5),Q(p05),p15,p25,p35)|Jx,Jy,Jz,Jxy,Jyz,Jxz;
 a2=b2+c1; a4=b4+c3; a6=b6+c5;
 P2.fi2=m2*g*sin(alfa1); P4.fi4=m4*g*sin(alfa3); P6.fi6=m6*g*sin(alfa5);
 D3=2*D2;  D5=2*D4; D7=2*D6; D9=2*D8; D11=2*D10;
*/

extern List *L; extern char SimbInd;
extern char ErrFile[],OprFile[],Inf[],WORKDIR[],FILESHAP[];
extern Cnst *Pust; extern FILE *OPRFILE;
static char FullHead[256];
extern TEditList *EditList;
Magazine *IndexBrak;

class StringerBodyVarElem{
 public:
  StringerBodyVarElem *Sled;
 private:
  char *Prot,*Form; StringerBodyVarElem *R;
 public:
  void FreeProt(){ if(Prot) free(Prot); };
  void FreeForm(){ if(Form) free(Form); };
  void SetProt(Stringer *T);
  void SetProt(Body *B);
  void SetProt(Vary *V);
  void SetProt(Elem *E);
  void SetForm(Stringer *T);
  void SetForm(Body *B);
  void SetForm(Vary *V);
  void SetForm(Elem *E);
  void SetElem(StringerBodyVarElem *r){ R=r; };
  char *GetProt(){ return Prot; }
  char *GetForm(){ return Form; }
};

void StringerBodyVarElem::SetProt(Vary *V){
 AnsiString S1=V->Name,S2;
 S1+="="; FormToStr(V->Znach,&S2); S1+=S2;
 Prot=(char *)calloc(S1.Length()+1,SzC); strcpy(Prot,S1.c_str());
}

void StringerBodyVarElem::SetForm(Vary *V){
 AnsiString S1=V->Name,S2;
 S1+="="; FormToStr(V->Znach,&S2); S1+=S2;
 Form=(char *)calloc(S1.Length()+1,SzC); strcpy(Form,S1.c_str());
}

void StringerBodyVarElem::SetProt(Elem *E){
 AnsiString S1=E->Name,S2;
 S1+="="; FormToStr(E->Znach,&S2); S1+=S2;
 Prot=(char *)calloc(S1.Length()+1,SzC); strcpy(Prot,S1.c_str());
}

void StringerBodyVarElem::SetForm(Elem *E){
 AnsiString S1=E->Name,S2;
 S1+="="; FormToStr(E->Znach,&S2); S1+=S2;
 Form=(char *)calloc(S1.Length()+1,SzC); strcpy(Form,S1.c_str());
}

void StringerBodyVarElem::SetForm(Body *B){
 AnsiString S1=B->Name,S2;
 if(B->BaseName){ S1+='~'; S1+=B->BaseName; }
 S1+=" | ";
 for(Parm *P=B->KoordSys;P;P=P->Sled){
  if(P->Nam){
   S1+=P->Nam;
   if(P->R){
    S1+='(';
    for(Parm *p=P->R;p;p=p->Sled){
     if(p->Nam) S1+=p->Nam; else S1+=p->Val; if(p->Sled) S1+=",";
    }
    S1+=')';
   }
   if(P->Sled) S1+=", ";
 }}
 S1+=" | ";
 for(Parm *P=B->Inert;P;P=P->Sled){
  if(P->Nam){
   S1+=P->Nam;
   if(P->R){ S1+='='; if(P->R->Nam) S1+=P->R->Nam; else S1+=P->R->Val; }
  }
  if(P->Sled) S1+=", ";
 }
 Form=(char *)calloc(S1.Length()+1,SzC); strcpy(Form,S1.c_str());
}

void StringerBodyVarElem::SetProt(Body *B){
 AnsiString S1=B->Name,S2;
 if(B->BaseName){ S1+='~'; S1+=B->BaseName; }
 S1+=" | ";
 for(Parm *P=B->KoordSys;P;P=P->Sled){
  if(P->Nam){
   S1+=P->Nam;
   if(P->R){
    S1+='(';
    for(Parm *p=P->R;p;p=p->Sled){
     if(p->Nam) S1+=p->Nam; else S1+=p->Val; if(p->Sled) S1+=",";
    }
    S1+=')';
   }
   if(P->Sled) S1+=", ";
 }}
 S1+=" | ";
 for(Parm *P=B->Inert;P;P=P->Sled){
  if(P->Nam){
   S1+=P->Nam;
   if(P->R){ S1+='='; if(P->R->Nam) S1+=P->R->Nam; else S1+=P->R->Val; }
  }
  if(P->Sled) S1+=", ";
 }
 Prot=(char *)calloc(S1.Length()+1,SzC); strcpy(Prot,S1.c_str());
}

void StringerBodyVarElem::SetForm(Stringer *T){
 AnsiString S1=T->Name,S2;
 S1+=" \\ ";
 for(Parm *P=T->Parms;P;P=P->Sled){
  if(P->Nam){
   S1+=P->Nam;
   if(P->R){
    S1+='(';
    for(Parm *p=P->R;p;p=p->Sled){
     if(p->Nam) S1+=p->Nam; else S1+=p->Val; if(p->Sled) S1+=",";
    }
    S1+=')';
   }
   if(P->Sled) S1+=", ";
 }}
 Form=(char *)calloc(S1.Length()+1,SzC); strcpy(Form,S1.c_str());
}

void StringerBodyVarElem::SetProt(Stringer *T){
 AnsiString S1=T->Name,S2;
 S1+=" \\ ";
 for(Parm *P=T->Parms;P;P=P->Sled){
  if(P->Nam){
   S1+=P->Nam;
   if(P->R){
    S1+='(';
    for(Parm *p=P->R;p;p=p->Sled){
     if(p->Nam) S1+=p->Nam; else S1+=p->Val; if(p->Sled) S1+=",";
    }
    S1+=')';
   }
   if(P->Sled) S1+=", ";
 }}
 Prot=(char *)calloc(S1.Length()+1,SzC); strcpy(Prot,S1.c_str());
}

class TListSBVE{
 private:
  StringerBodyVarElem *T,*B,*V,*E,**Pt,**Pb,**Pv,**Pe;
 public:
  TListSBVE(){ V=NULL; E=NULL; B=NULL; T=NULL; Pv=&V; Pe=&E; Pb=&B; Pt=&T; };
  void Add(Vary *v);
  void Add(Elem *e);
  void Add(Body *b);
  void Add(Stringer *b);
  void AddProt(Vary *v);
  void AddProt(Elem *e);
  void AddProt(Body *b);
  void AddProt(Stringer *b);
  void View();
  void VtoFile();
  void EtoFile();
  void BtoFile();
  void TtoFile();
 ~TListSBVE();
};

void TListSBVE::VtoFile(){
 Form F; char Formula[1024],*f,*s,*Name; List *Lo=L;
 L=(List *)calloc(1,sizeof(List));
 for(StringerBodyVarElem *v=V;v;v=v->Sled){
  fputs("<tr>\n <td>",OPRFILE);
  if(v->GetProt()){
//   StrToHTM(v->GetProt(),OPRFILE);
   for(Name=s=v->GetProt();*s&&(*s!='=');s++); *s='\0';
   strcpy(Inf,Name); Grek(Inf);
   strcpy(Formula,s+1);
   for(f=Formula;*f;f++)
    switch(*f){
     case '[': *f='('; break;
     case ']': *f=')'; break;
     case '{': *f='('; break;
     case '}': *f=')'; break;
    }
   F=StrToForm(Formula); PrintForm(false,OPRFILE,Name,F); *s='=';
  }
  fputs("</td>\n<td>",OPRFILE);
  if(v->GetForm()){
   for(Name=s=v->GetForm();*s&&(*s!='=');s++); *s='\0';
   strcpy(Formula,s+1);
   for(f=Formula;*f;f++)
    switch(*f){
     case '[': *f='('; break;
     case ']': *f=')'; break;
     case '{': *f='('; break;
     case '}': *f=')'; break;
    }
   F=StrToForm(Formula);
   PrintForm(false,OPRFILE,Name,F); *s='=';
 }}
 free(L);
 L=Lo;
}

void TListSBVE::EtoFile(){
 Form F; char Formula[1024],*f,*s,*Name;
 for(StringerBodyVarElem *e=E;e;e=e->Sled){
  fputs("<tr>\n <td>",OPRFILE);
  if(e->GetProt()) StrToHTM(e->GetProt(),OPRFILE);
  fputs("</td>\n<td>",OPRFILE);
  if(e->GetForm()){
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

void TListSBVE::BtoFile(){
 Form F; char Formula[1024],*f,*s,*Name;
 for(StringerBodyVarElem *b=B;b;b=b->Sled){
  fputs("<tr>\n <td>",OPRFILE);
  if(b->GetProt())
   StrToHTM(b->GetProt(),OPRFILE);
  fputs("</td>\n<td>",OPRFILE);
  if(b->GetForm())
   StrToHTM(b->GetForm(),OPRFILE);
}}

void TListSBVE::TtoFile(){
 Form F; char Formula[1024],*f,*s,*Name;
 for(StringerBodyVarElem *t=T;t;t=t->Sled){
  fputs("<tr>\n <td>",OPRFILE);
  if(t->GetProt())
   StrToHTM(t->GetProt(),OPRFILE);
  fputs("</td>\n<td>",OPRFILE);
  if(t->GetForm())
   StrToHTM(t->GetForm(),OPRFILE);
}}

TListSBVE::~TListSBVE(){
 for(StringerBodyVarElem *t;t=T;){ T->FreeProt(); t->FreeForm(); T=t->Sled; free(t); }
 for(StringerBodyVarElem *b;b=B;){ b->FreeProt(); b->FreeForm(); B=b->Sled; free(b); }
 for(StringerBodyVarElem *v;v=V;){ v->FreeProt(); v->FreeForm(); V=v->Sled; free(v); }
 for(StringerBodyVarElem *e;e=E;){ e->FreeProt(); e->FreeForm(); E=e->Sled; free(e); }
 Pt=&T; Pb=&B; Pv=&V; Pe=&E;
}

void TListSBVE::AddProt(Stringer *T){
 StringerBodyVarElem *t;
 *Pt=t=(StringerBodyVarElem *)calloc(1,sizeof(StringerBodyVarElem));
 Pt=(StringerBodyVarElem **)t; t->SetProt(T);
}

void TListSBVE::AddProt(Body *B){
 StringerBodyVarElem *b;
 *Pb=b=(StringerBodyVarElem *)calloc(1,sizeof(StringerBodyVarElem));
 Pb=(StringerBodyVarElem **)b; b->SetProt(B);
}

void TListSBVE::AddProt(Vary *V){  StringerBodyVarElem *v;
 *Pv=v=(StringerBodyVarElem *)calloc(1,sizeof(StringerBodyVarElem));
 Pv=(StringerBodyVarElem **)v; v->SetProt(V);
}

void TListSBVE::AddProt(Elem *E){ StringerBodyVarElem *e;
 *Pe=e=(StringerBodyVarElem *)calloc(1,sizeof(StringerBodyVarElem));
 Pe=(StringerBodyVarElem **)e; e->SetProt(E);
}

void TListSBVE::Add(Stringer *T){
 StringerBodyVarElem *t;
 *Pt=t=(StringerBodyVarElem *)calloc(1,sizeof(StringerBodyVarElem));
 Pt=(StringerBodyVarElem **)t; t->SetForm(T);
}

void TListSBVE::Add(Body *B){
 StringerBodyVarElem *b;
 *Pb=b=(StringerBodyVarElem *)calloc(1,sizeof(StringerBodyVarElem));
 Pb=(StringerBodyVarElem **)b; b->SetForm(B);
}

void TListSBVE::Add(Vary *V){
 StringerBodyVarElem *v;
 *Pv=v=(StringerBodyVarElem *)calloc(1,sizeof(StringerBodyVarElem));
 Pv=(StringerBodyVarElem **)v; v->SetForm(V);
}

void TListSBVE::Add(Elem *E){
 StringerBodyVarElem *e;
 *Pe=e=(StringerBodyVarElem *)calloc(1,sizeof(StringerBodyVarElem));
 Pe=(StringerBodyVarElem **)e; e->SetForm(E);
}

void ViewStringer(void **R){
 TEditList *EditListOld=EditList; FILE *F;
 int i,*MNN=(int *)calloc(3,SzI),H=Screen->Height-100;
 char *FileSh="shapkat",*Format="%-s%-s",**Head=(char **)calloc(1,SzV),
  *Fprn="index.prn",*Help=(char *)calloc(256,SzV),Dir[256],*s,
  *FH=(char *)calloc(256,SzC),Predki[32],Deti[16];
 if(F=fopen(FileSh,"w")){
  strcpy(Predki,"Исходные выражения"); StrWINtoDOS(Predki);
  strcpy(Deti,"Новые записи \"стержни\""); StrWINtoDOS(Deti);
  sprintf(Inf,"%s\xb3%s\n",Predki,Deti); fputs(Inf,F); fclose(F);
 }
 sprintf(FH,"Оператор \"РАЗМНОЖИТЬ\" сгенерировал\n"
  "\"стержни\" из индексированных выражений");
 Head[0]=(char *)calloc(256,SzC); strcpy(Head[0],"Новые записи \"стержень\"");
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

void ViewBody(void **R){
 TEditList *EditListOld=EditList; FILE *F;
 int i,*MNN=(int *)calloc(3,SzI),H=Screen->Height-100;
 char *FileSh="shapkab",*Format="%-s%-s",**Head=(char **)calloc(1,SzV),
  *Fprn="index.prn",*Help=(char *)calloc(256,SzV),Dir[256],*s,
  *FH=(char *)calloc(256,SzC),Predki[32],Deti[16];
 if(F=fopen(FileSh,"w")){
  strcpy(Predki,"Исходные выражения"); StrWINtoDOS(Predki);
  strcpy(Deti,"Новые записи \"твердое тело\""); StrWINtoDOS(Deti);
  sprintf(Inf,"%s\xb3%s\n",Predki,Deti); fputs(Inf,F); fclose(F);
 }
 sprintf(FH,"Оператор \"РАЗМНОЖИТЬ\" сгенерировал\n"
  "\"тердые тела\" из индексированных выражений");
 Head[0]=(char *)calloc(256,SzC); strcpy(Head[0],"Новые записи \"твердое тело\"");
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

void ViewElem(void **R){
 TEditList *EditListOld=EditList; FILE *F;
 int i,*MNN=(int *)calloc(3,SzI),H=Screen->Height-100;
 char *FileSh="shapkae",*Format="%-s%-s",**Head=(char **)calloc(1,SzV),
  *Fprn="index.prn",*Help=(char *)calloc(256,SzV),Dir[256],*s,
  *FH=(char *)calloc(256,SzC),Predki[32],Deti[16];
 if(F=fopen(FileSh,"w")){
  strcpy(Predki,"Исходные выражения"); StrWINtoDOS(Predki);
  strcpy(Deti,"Элементы"); StrWINtoDOS(Deti);
  sprintf(Inf,"%s\xb3%s\n",Predki,Deti); fputs(Inf,F); fclose(F);
 }
 sprintf(FH,"Оператор \"РАЗМНОЖИТЬ\" сгенерировал\n"
  "элементы из индексированных выражений");
 Head[0]=(char *)calloc(256,SzC); strcpy(Head[0],"Элементы");
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

void TListSBVE::View(){
 FILE *F; int i,*MNN=(int *)calloc(3,SzI),H=Screen->Height-60;
 char *FileSh="shapkav",*Format=B&&E?"%-s%-s%p%p":E||B?"%-s%-s%p":"%-s%-s",
  **Head=(char **)calloc(3,SzV),*s,*Fprn="index.prn",Predki[32],Deti[16],
  Dir[256],*Help=(char *)calloc(256,SzV);
 void (**Proc)(void **)=(void (**)(void **))calloc(2,SzV);
 if(F=fopen(FileSh,"w")){
  strcpy(Predki,"Исходные выражения"); StrWINtoDOS(Predki);
  strcpy(Deti,"Переменные"); StrWINtoDOS(Deti);
  sprintf(Inf,"%s\xb3%s\n",Predki,Deti); fputs(Inf,F); fclose(F);
 }
 if(B&&E){ Proc[0]=ViewElem; Proc[1]=ViewBody; }
 else if(E) Proc[0]=ViewElem;
 else if(B) Proc[0]=ViewBody;
 sprintf(FullHead,"Оператор \"РАЗМНОЖИТЬ\" сгенерировал "
  "переменные из индексированных выражений");
 Head[0]=(char *)calloc(256,SzC); Head[1]=(char *)calloc(256,SzC);
 for(StringerBodyVarElem *v=V;v;v=v->Sled) v->SetElem(E);
 for(i=0;i<2;i++) MNN[i]=i+1;
 strcpy(Head[0],"Переменные"); strcpy(Head[1],"Элементы");
 strcpy(Head[2],"Новые записи \"твердое тело\"");
 strcpy(Dir,WORKDIR); if(s=strrchr(Dir,'\\')) strcpy(s,"\\Help");
 sprintf(Help,"%s\\ViewIndex.htm",Dir);
 EditList=new TEditList((void **)&V,MNN,false,NULL,false,NULL,Format,FileSh,Head,
  Fprn,NULL,NULL,B||E?Proc:NULL,1,-1,0,-1,H,FullHead,NULL,0,Help,NULL,NULL);
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
  TInd *Sled; char *SI,**Vals; int GS[3],Steps;
};

class TInds{
 public:
  TInd *R;
  TInds(){ R=NULL; }
  TInd *FindInd(char *I);
  bool AddInd(Parm *p);
  bool AddInd(char *s);
  bool IndLimit(char *Name,int &N);
  bool IndLimit(Stringer *B,int &N);
  bool IndLimit(Body *B,int &N);
  bool IndLimit(Vary *V,int &N);
  bool IndLimit(Elem *E,int &N);
  bool IndLimit(Form F,int &N);
  bool Index(Stringer *B,TListSBVE &ListSBVE);
  bool Index(Body *B,TListSBVE &ListSBVE);
  bool Index(Vary *V,TListSBVE &ListSBVE);
  bool Index(Elem *E,TListSBVE &ListSBVE);
//  bool IndexForm(Body *V,TListSBVE &ListSBVE);
  bool IndexForm(Vary *V,TListSBVE &ListSBVE);
  bool IndexForm(Elem *E,TListSBVE &ListSBVE);
  bool NewName(char *Name,int i,char *New);
  bool NewForm(Form Fv,int i,Form &F);
 ~TInds();
} *Inds,*IndExp;

TInds::~TInds(){
 for(TInd *r;r=R;){
  R=r->Sled;
  if(r->SI) free(r->SI);
  if(r->Vals){
   for(int i=0;i<r->Steps+1;i++) if(r->Vals[i]) free(r->Vals[i]);
   free(r->Vals);
  }
  free(r);
}}

TInd *TInds::FindInd(char *I){
 for(TInd *r=R;r;r=r->Sled)
  if(!strcmp(r->SI,I)) return r; return NULL;
}

bool ValParm(Parm *p,char *NameInd,char *S,int &Rez){
 Vary *V; Form Fp; FILE *F; double D;
 if(p->Nam){
  if(FindVary(p->Nam,&V)) Rez=(int)Round(Val(V->Znach));
  else{
   Fp=StrToForm(p->Nam);
   if(Fp.C->Atr==4){//неизвестная переменная
    sprintf(Inf,"Имя \"%s\", указанное в операторе \"РАЗМНОЖИТЬ\"\n"
     "как %s индекса %s,\nне найдено среди переменных задачи",p->Nam,S,NameInd);
    Error(0,0,true);
    return false;
   }
   else{
    if(!Valc(Fp,D)){
     sprintf(Inf,"Формула \"%s\", указанное в операторе \"РАЗМНОЖИТЬ\"\n"
      "как %s индекса %s,\nне может быть вычислена",p->Nam,S,NameInd);
     Error(0,0,true);
     return false;
    }
    else Rez=(int)Round(D);
 }}}
 else Rez=(int)Round(p->Val);
 return true;
}

bool TInds::AddInd(Parm *p){
 Vary *V; FILE *F; char *Head,NameInd[256]; Form Fr; double Z; int GS[3];
 if(!p||!p->Nam){
  sprintf(Inf,"В операторе \"РАЗМНОЖИТЬ\"\nне указано наименование индекса");
  Error(0,0,true);
  return false;
 }
 strcpy(NameInd,p->Nam);
 p=p->R;
 if(!ValParm(p,NameInd,"начальное значение",GS[0])) return false;
 p=p->Sled;
 if(!ValParm(p,NameInd,"конечное значение",GS[1])) return false;
 if(p&&p->Sled){
  p=p->Sled;
  if(!ValParm(p,NameInd,"шаг значений",GS[2])) return false;
 }
 else GS[2]=1;
 if((GS[0]<0)||(GS[1]<0)||(GS[2]<0)||(GS[1]<GS[0])){
  if(GS[0]<0)
   sprintf(Inf,
    "Для индекса \"%s\" в операторе \"РАЗМНОЖИТЬ\" нижняя граница отрицательна",
    NameInd);
  else if(GS[1]<0)
   sprintf(Inf,
    "Для индекса \"%s\" в операторе \"РАЗМНОЖИТЬ\" верхняя граница отрицательна",
    NameInd);
  else if(GS[2]<0)
   sprintf(Inf,
    "Для индекса \"%s\" в операторе \"РАЗМНОЖИТЬ\" шаг задан отрицательным",
    NameInd);
  else if(GS[1]<GS[0])
   sprintf(Inf,
    "Для индекса \"%s\" в операторе \"РАЗМНОЖИТЬ\" верхняя граница меньше нижней",
    NameInd);
  Atten(0,0);
  TakeMagazine(&IndexBrak,NameInd);
  return false;
 }
 V=TakeVary(NameInd); V->P=0; V->Znach.C=TakeCnst(GS[0]);
 TInd *Ind=(TInd *)calloc(1,sizeof(TInd)); Ind->Sled=R; R=Ind;
 Ind->SI=strcpy((char *)calloc(strlen(NameInd)+1,SzC),NameInd);
 Ind->GS[0]=GS[0]; Ind->GS[1]=GS[1]; Ind->GS[2]=GS[2];
 Ind->Steps=(int)Round((GS[1]-GS[0])/GS[2]);
 return true;
}

void VaryMagFromForm(Form F,Magazine **M){ Form FF;
 switch(F.C->Atr){
  case CNST: case PUST: break;
  case VARY: TakeMagazine(M,F.V->Name); break;
  case UNOP: VaryMagFromForm(F.U->Arg,M); break;
  case BNOP: VaryMagFromForm(F.B->L,M); VaryMagFromForm(F.B->R,M); break;
  case IFOP:
   VaryMagFromForm(F.L->U,M);  VaryMagFromForm(F.L->V,M);
   VaryMagFromForm(F.L->FT,M); VaryMagFromForm(F.L->FF,M);
   break;
  case SUMM: break;
  case INTG:
   FF.V=F.G->x;
   VaryMagFromForm(FF,M);     VaryMagFromForm(F.G->a,M);
   VaryMagFromForm(F.G->b,M); VaryMagFromForm(F.G->f,M);
   break;
  case IPLL: VaryMagFromForm(F.I->F,M); break;
  case SPLN: VaryMagFromForm(F.P->F,M); break;
}}

bool TInds::AddInd(char *S){//s - индексное выражение, начинающееся с '$'!
 if(FindInd(S)) return true;
 Vary *V; FILE *F; char *Head,*s0,Buf[256],B; TInd *Ind,*I; int Ks=0,Steps=-1,St;
 Magazine *Mv=NULL,*m;
/* if(FindVary(S+1,&V)&&(V->Znach.C->Atr==3)){
  TInd *Ind=(TInd *)calloc(1,sizeof(TInd)); Ind->Sled=R; R=Ind;
  Ind->SI=strcpy((char *)calloc(strlen(S+1)+1,SzC),S+1);
  Ind->GS[0]=Ind->GS[1]=V->Znach.C->Val; Ind->GS[2]=1;
  Ind->Steps=0;
 }*/
//сначала определим число шагов как минимум из составляющих выражений
 for(char *s=S;*s;s++){//движемся по индексному выражению
//   if(*s=='\'') break;
  if(*s==SimbInd){ //встретили начало подвыражения
   if(s[1]!='('){//простой случай - одна переменая-индекс, заканчивается '\0'!
    B=s[2]; s[2]='\0';
    if(!(I=Inds->FindInd(s+1))){
     if(!FindMagazine(IndexBrak,s+1,&m)){
      if(s!=S)
       sprintf(Inf,"Индекс \"%s\", входящий в выражение \"%s\""
        "не указан в операторе \"РАЗМНОЖИТЬ\"",s+1,S);
      else
       sprintf(Inf,"Индекс \"%s\", не указан в операторе \"РАЗМНОЖИТЬ\"",s+1);
      Error(0,0,true);
      s[2]=B; return false;
    }}
    else{//найден индекс с границами
     s[2]=B;
     St=Round((I->GS[1]-I->GS[0])/I->GS[2]);
     if((Steps<0)||(St<Steps))
      Steps=St;
   }}
   else{ //скобочное выражение
    for(s0=s+1;*s0;s0++){
     if(*s0=='(') Ks++;
     else if(*s0==SimbInd){ FILE *F;
//началось новое индексное выражение, а осталась незакрытая скобка
      if(s!=S)
       sprintf(Inf,"В индексном выражении \"%s\",входящем в выражение \"%s\""
        "не хватает закрыт%s",s,S,Ks>1?"ых скобок":"ой скобки");
      else
       sprintf(Inf,"В индексном выражении \"%s\","
        "не хватает закрыт%s",s,Ks>1?"ых скобок":"ой скобки");
      Error(0,0,true);
      return false;
     }
     else if(*s0==')'){
      Ks--;
      if(!Ks){ Form F; //закрываем подвыражение
       B=*(s0+1); *(s0+1)='\0';
       if(I=FindInd(s)){//s начинается с $! - такое подвыражение уже встречалось
        St=I->Steps; *(s0+1)=B;
       }
       else{ int z; St=0; //делаем новое подвыражение
        F=StrToForm(s+1); DelAllMagazine(&Mv);
        VaryMagFromForm(F,&Mv);//магазин переменных из формулы
        for(Magazine *m=Mv;m;m=m->Sled){
         if(!(FindVary(m->S,&V))){ FILE *F;
//для индекса нет переменной
          sprintf(Inf,"Для индекса \"%s\","
           "входящего в выражение \"%s\""
           "отсутствует переменная с его значениями",m->S,s);
          Error(0,0,true);
          DelAllMagazine(&Mv); return false;
         }
         else{
          if(I=Inds->FindInd(m->S)){
           if(!St||(St<Round((I->GS[1]-I->GS[0])/I->GS[2])))
           St=Round((I->GS[1]-I->GS[0])/I->GS[2]);
        }}}
//породим индексное выражение для подвыражения
        Ind=(TInd *)calloc(1,sizeof(TInd)); Ind->Sled=R; R=Ind;
        Ind->Vals=(char **)calloc(St+1,SzV); Ind->Steps=St;
        Ind->SI=strcpy((char *)calloc(strlen(s)+1,SzC),s);
//массив строк
        for(int i=0;i<St+1;i++){
         for(Magazine *m=Mv;m;m=m->Sled){
          if(I=Inds->FindInd(m->S)){
           FindVary(m->S,&V);
           V->Val=I->GS[0]+i*I->GS[2];
         }}
         sprintf(Buf,"%d",(int)Round(Value(F)));//вычислили значение индексного подвыражения
         Ind->Vals[i]=strcpy((char *)calloc(strlen(Buf)+1,SzC),Buf);
       }}
       *(s0+1)=B; s=s0;
       if((Steps<0)||(St<Steps))
        Steps=St;
       break;
    }}}
    if(Ks){ FILE *F;
//осталась незакрытая скобка
     if(s!=S)
      sprintf(Inf,"В индексном выражении \"%s\"входящего в выражение \"%s\""
       "нарушен баланс скобок",S,s);
     else
      sprintf(Inf,"В индексном выражении \"%s\"нарушен баланс скобок",S);
     Error(0,0,true);
     return false;
 }}}}
 if(!FindInd(S)){
//породим индексное выражение для всего выражения
//массив строк
  char **Vals=(char **)calloc(Steps+1,SzV);
  for(int i=0;i<Steps+1;i++){ char *b=Buf;
   for(char *s=S;*s;s++){//движемся по индексному выражению
    if(*s==SimbInd){ //встретили начало подвыражения
     if(s[1]!='('){//простой случай - одна переменая-индекс, заканчивается '\0'!
      B=s[2]; s[2]='\0';
      I=Inds->FindInd(s+1); s[2]=B;
      b+=sprintf(b,"%d%s",I->GS[0]+i*I->GS[2],s+2); s++;
     }
     else{ Ks=0; //скобочное выражение
      for(s0=s+1;*s0;s0++){
       if(*s0=='(') Ks++;
       else if(*s0==')'){
        Ks--;
        if(!Ks){ Form F; //закрываем подвыражение
         B=*(s0+1); *(s0+1)='\0';
         I=FindInd(s); s0[1]=B;
         b+=sprintf(b,"%s",I->Vals[i]);
         *(s0+1)=B; s=s0; break;
    }}}}}
    else *b++=*s;
   }
   *b='\0';
   Vals[i]=strcpy((char *)calloc(strlen(Buf)+1,SzC),Buf);
  }
  Ind=(TInd *)calloc(1,sizeof(TInd)); Ind->Sled=R; R=Ind;
  Ind->Vals=Vals; Ind->Steps=Steps;
  Ind->SI=strcpy((char *)calloc(strlen(S)+1,SzC),S);
 }
 return true;
}

bool TInds::IndLimit(Form F,int &N){
 TInd *I; char B; Form FF;
 switch(F.C->Atr){
  case CNST: case PUST: return true; //break;
  case VARY:
   for(char *v=F.V->Name;*v;v++){
    if(*v==SimbInd){ v++;
     if(*v=='('){ int Ks=1;
      for(char *v0=++v;*v0;v0++){
       if(*v0=='(') Ks++;
       else if(*v0==')'){
        Ks--;
        if(!Ks){ Form F;
         *v0='\0'; F=StrToForm(v); *v0=')';
         if(!IndLimit(F,N)) return false;
     }}}}
     else{
      B=v[1]; v[1]='\0';
      if(I=FindInd(v)){
       v[1]=B;
       if(((I->GS[1]-I->GS[0])/I->GS[2]+1)<N)
        N=(I->GS[1]-I->GS[0])/I->GS[2]+1;
      }
      else{ N=0; v[1]=B; return false; }
   }}}
   if(!IndLimit(F.V,N)) return false;
   break;
  case UNOP: IndLimit(F.U->Arg,N); break;
  case BNOP: IndLimit(F.B->L,N); IndLimit(F.B->R,N); break;
  case IFOP:
   IndLimit(F.L->U,N);  IndLimit(F.L->V,N);
   IndLimit(F.L->FT,N); IndLimit(F.L->FF,N);
   break;
  case SUMM: break;
  case INTG:
   FF.V=F.G->x;
   IndLimit(FF,N);     IndLimit(F.G->a,N);
   IndLimit(F.G->b,N); IndLimit(F.G->f,N);
   break;
  case IPLL: IndLimit(F.I->F,N); break;
  case SPLN: IndLimit(F.P->F,N); break;
 }
 return N>0;
}

bool TInds::IndLimit(char *Name,int &N){
 TInd *I; FILE *F; char B;
 for(char *s=Name;*s;s++){
  if(*s==SimbInd){ s++;
   if(*s=='('){ int Ks=1;
    for(char *s0=++s;*s0;s0++){
     if(*s0=='(') Ks++;
     else if(*s0==')'){
      Ks--;
      if(!Ks){ Form F;
       *s0='\0'; F=StrToForm(s); *s0=')';
       if(!IndLimit(F,N)) return false;
   }}}}
   else{
    B=s[1]; s[1]='\0';
    if(I=FindInd(s)){ s[1]=B;
     if(((I->GS[1]-I->GS[0])/I->GS[2]+1)<N)
      N=(I->GS[1]-I->GS[0])/I->GS[2]+1;
    }
    else{
     sprintf(Inf,"Имя \"%s\"\nсодержит в качестве индекса переменную,"
      " которая не указана в операторе \"РАЗМНОЖИТЬ\"",Name);
     Atten(0,0);
     N=0; s[1]=B; return true;
 }}}}
 return N>0;
}

bool TInds::IndLimit(Vary *V,int &N){
 TInd *I; FILE *F; char B;
 for(char *v=V->Name;*v;v++){
  if(*v==SimbInd){ v++;
   if(*v=='('){ int Ks=1;
    for(char *v0=++v;*v0;v0++){
     if(*v0=='(') Ks++;
     else if(*v0==')'){
      Ks--;
      if(!Ks){ Form F;
       *v0='\0'; F=StrToForm(v); *v0=')';
       if(!IndLimit(F,N)) return false;
   }}}}
   else{
    B=v[1]; v[1]='\0';
    if(I=FindInd(v)){ v[1]=B;
     if(((I->GS[1]-I->GS[0])/I->GS[2]+1)<N)
      N=(I->GS[1]-I->GS[0])/I->GS[2]+1;
    }
    else{
     sprintf(Inf,
      "Имя переменной \"%s\" содержит в качестве индекса переменную %s,\n"
      "которая не указана в операторе \"РАЗМНОЖИТЬ\"",V->Name,v);
     Atten(V->Line,0);
     N=0; v[1]=B; return true;
 }}}}
 if(!IndLimit(V->Znach,N)){
  sprintf(Inf,"Формула для переменной \"%s\"содержит переменные с индексами,\n"
   "которые не указаны в операторе \"РАЗМНОЖИТЬ\"",V->Name);
  Atten(0,0);
  N=0; return true;
 }
 return N>0;
}

bool TInds::IndLimit(Stringer *T,int &N){ TInd *I; FILE *F; char b;
 IndLimit(T->Name,N);
 for(Parm *P=T->Parms;P;P=P->Sled){
  if(P->Nam){
   IndLimit(P->Nam,N);
   for(Parm *p=P->R;p;p=p->Sled){
    if(p->Nam) IndLimit(p->Nam,N);
 }}}
 return N>0;
}

bool TInds::IndLimit(Body *B,int &N){ TInd *I; FILE *F; char b;
 IndLimit(B->Name,N);
 if(B->BaseName) IndLimit(B->BaseName,N);
 for(Parm *P=B->KoordSys;P;P=P->Sled){
  if(P->Nam){
   IndLimit(P->Nam,N);
   for(Parm *p=P->R;p;p=p->Sled){
    if(p->Nam) IndLimit(p->Nam,N);
 }}}
 for(Parm *P=B->Inert;P;P=P->Sled){
  if(P->Nam){
   IndLimit(P->Nam,N);
   if(P->R&&P->R->Nam) IndLimit(P->R->Nam,N);
 }}
 return N>0;
}

bool TInds::IndLimit(Elem *E,int &N){ TInd *I; FILE *F; char B;
 for(char *e=E->Name;*e;e++){
  if(*e==SimbInd){ e++;
   if(*e=='('){ int Ks=1;
    for(char *e0=++e;*e0;e0++){
     if(*e0=='(') Ks++;
     else if(*e0==')'){
      Ks--;
      if(!Ks){ Form F;
       *e0='\0'; F=StrToForm(e); *e0=')';
       IndLimit(F,N);
   }}}}
   else{ B=e[1]; e[1]='\0';
    if(I=FindInd(e)){ e[1]=B;
     if(((I->GS[1]-I->GS[0])/I->GS[2]+1)<N)
      N=(I->GS[1]-I->GS[0])/I->GS[2]+1;
    }
    else{ N=0; e[1]=B; return false; }
 }}}
 IndLimit(E->K,N);
 if(!IndLimit(E->Znach,N)){
  sprintf(Inf,"Формула для элемента \"%s\"\n"
   "содержит переменные с индексами, которые не указаны<br>\n"
   "в операторе \"РАЗМНОЖИТЬ\"",E->Name);
  Atten(0,0);
  N=0; return true;
 }
 return N>0;
}

bool TInds::NewName(char *Name,int i,char *New){
 TInd *I; char *s=Name,*n=New,B; Vary *V;
 for(s=Name,n=New;*s;s++){
  if(*s==SimbInd){ s++;
   if(*s=='('){ int Ks=1;
    for(char *s0=++s;*s0;s0++){
     if(*s0=='(') Ks++;
     else if(*s0==')'){
      Ks--;
      if(!Ks){ Form F; Magazine *Mv; //магазин переменных из формулы
       *s0='\0'; F=StrToForm(s); *s0=')';
       ClearVar();
       VaryMagFromForm(F,&Mv);
       for(Magazine *m=Mv;m;m=m->Sled){
        if(!(I=FindInd(m->S))){ FILE *F;
//индекса нет в операторе РАЗМНОЖИТЬ
         sprintf(Inf,"Индекс \"%s\", входящий в выражение \"%s\""
          "отсутствует в операторе \"РАЗМНОЖИТЬ\"",m->S,s);
         Error(0,0,true);
         DelAllMagazine(&Mv); return false;
        }
        if((i*I->GS[2]+I->GS[0])>I->GS[1]){
         DelAllMagazine(&Mv); return false;
        }
        if(!(FindVary(m->S,&V))){ FILE *F;
//для индекса нет переменной
         sprintf(Inf,"Для индекса \"%s\",входящего в выражение \"%s\"\n"
          "не сгенерирована переменная с его значениями",m->S,s);
         Error(0,0,true);
         DelAllMagazine(&Mv); return false;
        }
//порядок!
        V->Val=i*I->GS[2]+I->GS[0];
       }
       n+=sprintf(n,"%d",Val(F));
   }}}}
   else{
    B=s[1]; s[1]='\0';
    if((I=FindInd(s))&&(i*I->GS[2]+I->GS[0]<=I->GS[1])){
     n+=sprintf(n,"%d",i*I->GS[2]+I->GS[0]); s[1]=B;
    }
    else{ s[1]=B; return false; }
  }}
  else *n++=*s;
 }
 *n='\0'; return true;
}

bool TInds::NewForm(Form Fv,int i,Form &Fn){
 switch(Fv.C->Atr){ char Name[256],*s; Form F,L,U,V,FT,FF; Arra *A; TInd *I;
  case PUST: case CNST: Fn=Fv; return true;
  case VARY:
   if(I=Inds->FindInd(Fv.V->Name)){//переменная есть индекс!
    Fn.C=TakeCnst(i*I->GS[2]+I->GS[0]); Fv.V->Znach=Fn;
   }
   else{
    if(s=strchr(Fv.V->Name,SimbInd)){//имя переменной содержит индекс
     if(s[1]=='('){
      if(I=IndExp->FindInd(s)){
       *s='\0'; sprintf(Name,"%s%s",Fv.V->Name,I->Vals[i]); *s=SimbInd;
     }}
     else
      if(!NewName(Fv.V->Name,i,Name)) return false;
     Fn.V=TakeVary(Name);
//Породим первообразные
     for(char *s=Name;s=strchr(s+1,'\'');*s='\''){ *s='\0'; TakeVary(Name); }
    }
    else Fn=Fv;
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
    case 14: Fn=rand(F);   break;
    case 15: Fn=ceil(F);   break;
    case 16: Fn=floor(F);  break;
    case 17: Fn=arccos(F);
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
  case SUMM: break;
  case INTG:
   F.V=Fv.G->x;
   if(!NewForm(F,i,FF)) return false;
   else if(!NewForm(Fv.G->a,i,U)) return false;
   else if(!NewForm(Fv.G->b,i,V)) return false;
   else if(!NewForm(Fv.G->f,i,FT)) return false;
   Fn.G=TakeIntg(FT,FF.V,U,V); break;
   break;
  case RUUT:
   F.V=Fv.R->x;
   if(!NewForm(F,i,FF)) return false;
   else if(!NewForm(Fv.R->a,i,U)) return false;
   else if(!NewForm(Fv.R->b,i,V)) return false;
   else if(!NewForm(Fv.R->f,i,FT)) return false;
   Fn.R=TakeSolu(FT,FF.V,U,V); break;
   break;
 }
 return true;
}

int Nstr,Kstr=70;

bool TInds::Index(Vary *V,TListSBVE &ListSBVE){
 int N=MaxInt; char Name[256]; Form F; Vary *v;
 if(!IndLimit(V,N)) return false;
 for(int i=0;i<N;i++){
  if(!NewName(V->Name,i,Name)) return false;
  if(!NewForm(V->Znach,i,F)) return false;
  v=TakeVary(Name); v->Znach=F; ListSBVE.Add(v);
//Породим первообразные
  for(char *s=Name;s=strchr(s+1,'\'');*s='\''){ *s='\0'; TakeVary(Name); }
 }
 return true;
}

bool TInds::IndexForm(Vary *V,TListSBVE &ListSBVE){
 char Name[256],*s,*t=strchr(V->Name,SimbInd); TInd *Ind=FindInd(t); Form F; Vary *v;
 *t='\0';
 for(int i=0;i<Ind->Steps+1;i++){
  sprintf(Name,"%s%s",V->Name,Ind->Vals[i]);
  if(!NewForm(V->Znach,i,F)) return false;
  v=TakeVary(Name); v->Znach=F; ListSBVE.Add(v);
//Породим первообразные
  for(s=Name;s=strchr(s+1,'\'');*s='\''){ *s='\0'; TakeVary(Name); }
 }
 *t=SimbInd;
 return true;
}

bool TInds::IndexForm(Elem *E,TListSBVE &ListSBVE){
 char Name[256],*s,*v; TInd *Ind; Form F; Elem *e;
 s=strchr(E->Name,SimbInd); Ind=FindInd(s);
 for(int i=0;i<1+Ind->Steps;i++){
  if(!NewForm(E->Znach,i,F)) return false;
  *s='\0'; sprintf(Name,"%s%s",E->Name,Ind->Vals[i]); *s=SimbInd;
  e=TakeElem(Name); e->Znach=F;
  v=strchr(Name,'.');
  e->K=TakeVary(v+1);
  ListSBVE.Add(e);
 }
 return true;
}

bool TInds::Index(Elem *E,TListSBVE &ListSBVE){
 int N=MaxInt; char Name[256],*s; Form F; Elem *e; Vary *v;
 if(!IndLimit(E,N)) return false;
 for(int i=0;i<N;i++){
  if(!NewName(E->Name,i,Name)) return false;
  if(s=strchr(Name,'.')){
   v=TakeVary(s+1);
//Породим первообразные
   for(char *s=Name;s=strchr(s+1,'\'');*s='\''){ *s='\0'; TakeVary(Name); }
  }
  if(!NewForm(E->Znach,i,F)) return false;
  e=TakeElem(Name); e->Znach=F; e->K=v; ListSBVE.Add(e);
 }
 return true;
}

bool TInds::Index(Body *B,TListSBVE &ListSBVE){
 int N=MaxInt,j; char Name[256],BaseName[256],*s; Body *b; Parm *pn;
 if(!IndLimit(B,N)) return false;
 for(int i=0;i<N;i++){
  if(!NewName(B->Name,i,Name)) return false;
  if(B->BaseName) if(!NewName(B->BaseName,i,BaseName)) return false;
  b=TakeBody(Name,B->BaseName?BaseName:NULL); b->Line=B->Line;
  if(B->KoordSys){
   b->KoordSys=(Parm *)calloc(KolElem(B->KoordSys),sizeof(Parm));
   for(j=0;j<KolElem(B->KoordSys)-1;j++) b->KoordSys[j].Sled=b->KoordSys+j+1;
   if(B->KoordSys->Nam){
    if(!NewName(B->KoordSys->Nam,i,Name)) return false;
    b->KoordSys->Nam=strcpy((char *)malloc((strlen(Name)+1)*SzC),Name);
   }
   for(Parm *P1=B->KoordSys,*P2=b->KoordSys;P1;P1=P1->Sled,P2=P2->Sled){
    if(P1->Nam){
     if(!NewName(P1->Nam,i,Name)) return false;
     P2->Nam=strcpy((char *)malloc((strlen(Name)+1)*SzC),Name);
     if(P1->R){
      P2->R=(Parm *)calloc(KolElem(P1->R),sizeof(Parm));
      for(j=0;j<KolElem(P1->R)-1;j++) P2->R[j].Sled=P2->R+j+1;
      for(Parm *p1=P1->R,*p2=P2->R;p1;p1=p1->Sled,p2=p2->Sled){
       if(p1->Nam){
        if(!NewName(p1->Nam,i,Name)) return false;
        p2->Nam=strcpy((char *)malloc((strlen(Name)+1)*SzC),Name);
       }
       else p2->Val=p1->Val;
  }}}}}
  if(B->Inert){
   b->Inert=(Parm *)calloc(KolElem(B->Inert),sizeof(Parm));
   for(j=0;j<KolElem(B->Inert)-1;j++) b->Inert[j].Sled=b->Inert+j+1;
   if(B->Inert->Nam){
    if(!NewName(B->Inert->Nam,i,Name)) return false;
    b->Inert->Nam=strcpy((char *)malloc((strlen(Name)+1)*SzC),Name);
   }
   for(Parm *P1=B->Inert,*P2=b->Inert;P1;P1=P1->Sled,P2=P2->Sled){
    if(P1->Nam){
     if(!NewName(P1->Nam,i,Name)) return false;
     P2->Nam=strcpy((char *)malloc((strlen(Name)+1)*SzC),Name);
     if(P1->R){
      P2->R=(Parm *)calloc(1,sizeof(Parm));
      if(P1->R->Nam){
       if(!NewName(P1->R->Nam,i,Name)) return false;
       P2->R->Nam=strcpy((char *)malloc((strlen(Name)+1)*SzC),Name);
      }
      else P2->R->Val=P1->R->Val;
  }}}}
  ListSBVE.Add(b);
 }
 return true;
}

bool TInds::Index(Stringer *T,TListSBVE &ListSBVE){
 int N=MaxInt,j; char Name[256],*s; Stringer *t; Parm *pn;
 if(!IndLimit(T,N)) return false;
 for(int i=0;i<N;i++){
  if(!NewName(T->Name,i,Name)) return false;
  t=TakeStringer(Name); t->Line=T->Line;
  if(T->Parms){
   t->Parms=(Parm *)calloc(KolElem(T->Parms),sizeof(Parm));
   for(j=0;j<KolElem(T->Parms)-1;j++) t->Parms[j].Sled=t->Parms+j+1;
   if(T->Parms->Nam){
    if(!NewName(T->Parms->Nam,i,Name)) return false;
    t->Parms->Nam=strcpy((char *)calloc((strlen(Name)+1),SzC),Name);
   }
   for(Parm *P1=T->Parms,*P2=t->Parms;P1;P1=P1->Sled,P2=P2->Sled){
    if(P1->Nam){
     if(!NewName(P1->Nam,i,Name)) return false;
     P2->Nam=(char *)calloc(strlen(Name)+1,SzC);
     strcpy(P2->Nam,Name);
     if(P1->R){
      P2->R=(Parm *)calloc(KolElem(P1->R),sizeof(Parm));
      for(j=0;j<KolElem(P1->R)-1;j++) P2->R[j].Sled=P2->R+j+1;
      for(Parm *p1=P1->R,*p2=P2->R;p1;p1=p1->Sled,p2=p2->Sled){
       if(p1->Nam){
        if(!NewName(p1->Nam,i,Name)) return false;
        p2->Nam=(char *)calloc(strlen(Name)+1,SzC);
        strcpy(p2->Nam,Name);
       }
       else p2->Val=p1->Val;
  }}}}}
  ListSBVE.Add(t);
 }
 return true;
}

bool IsSimbInd(Root *I){
 for(Parm *P=I->First;P;P=P->Sled){
  if(P->Nam&&strchr(P->Nam,SimbInd)) return true;
  for(Parm *p=P->R;p;p=p->Sled){
   if(p->Nam&&strchr(p->Nam,SimbInd)) return true;
 }}
 return false;
}

Parm *IsSimbInd(Parm *R){
 for(Parm *p=R;p;p=p->Sled) if(p->Nam&&strchr(p->Nam,SimbInd)) return p;
 return NULL;
}

char *TakeExp(char *Beg){//возвращаем конец индексного выражения, куда ставим \0
 int Ks=1; char *s0=Beg;
 for(;*s0;s0++){
  if(*s0=='(') Ks++;
  else if(*s0==')'){ Ks--; if(!Ks){ *s0='\0'; break; }
 }}
 return s0;
}

int DefSteps(Parm *P,int Steps,bool Type){
//Определяем число шагов для параметра и его списка
//если параметр и его список имеет индексы, то число шагов определяется
//как наименьшее из всех, отсутствие индекса не влияет на результат
 char *s,B,*b; TInd *Ind; Parm *p;
// if(!P->Nam) return Steps==-1?0:Steps;
 if(!P->Nam) return Steps;
 if(P->Nam&&(s=strchr(P->Nam,SimbInd))){//позиция символа индекса
  if(s[1]=='('){//действуем по индексному выражению
   if(Ind=IndExp->FindInd(s)){//нашли индексное выражение
    if(Type==SYNHRON)
     if((Steps<0)||(Ind->Steps<Steps)) Steps=Ind->Steps;
    else
     if((Steps<0)||(Ind->Steps>Steps)) Steps=Ind->Steps;
    for(p=P->R;p;p=p->Sled) Steps=DefSteps(p,Steps,Type);
  }}
  else{
   B=s[2]; s[2]='\0';
   if(Ind=Inds->FindInd(s+1)){//нашли индексное выражение
    s[2]=B;
    if(Type==SYNHRON){
     if((Steps<0)||(Ind->Steps<Steps)) Steps=Ind->Steps;
    }
    else
     if((Steps<0)||(Ind->Steps>Steps)) Steps=Ind->Steps;
    for(p=P->R;p;p=p->Sled) Steps=DefSteps(p,Steps,Type);
    for(s+=2;*s;s++){
     if(*s==SimbInd){
      if(s[1]=='('){//действуем по индексному выражению
       if(Ind=IndExp->FindInd(s)){//нашли индексное выражение
        if(Type==SYNHRON)
         if((Steps<0)||(Ind->Steps<Steps)) Steps=Ind->Steps;
        else
         if((Steps<0)||(Ind->Steps>Steps)) Steps=Ind->Steps;
        for(p=P->R;p;p=p->Sled) Steps=DefSteps(p,Steps,Type);
      }}
      else{
       B=s[2]; s[2]='\0';
       if(Ind=Inds->FindInd(s+1)){//нашли индексное выражение
        s[2]=B;
        if(Type==SYNHRON){
         if((Steps<0)||(Ind->Steps<Steps)) Steps=Ind->Steps;
        }
        else
         if((Steps<0)||(Ind->Steps>Steps)) Steps=Ind->Steps;
       }
       for(p=P->R;p;p=p->Sled) Steps=DefSteps(p,Steps,Type);
   }}}}
   else return -1;
 }}
 else{
  for(p=P->R;p;p=p->Sled)
   Steps=DefSteps(p,Steps,Type);
 }
 return Steps==-1?0:Steps;
}

Parm *List_i(Parm *P,int i){
 Parm *R=NULL,**Pf=&R,*p; char *s,Buf[256],B,*b; TInd *Ind;
 for(;P;P=P->Sled){
  *Pf=p=(Parm *)calloc(1,sizeof(Parm)); Pf=(Parm **)p; p->Line=P->Line;
  if(P->Nam){
   if(s=strchr(P->Nam,SimbInd)){//позиция символа индекса
    if(s[1]=='('){//действуем по индексному выражению
     if(Ind=IndExp->FindInd(s)){//нашли индексное выражение
      *s='\0'; sprintf(Buf,"%s%s",P->Nam,Ind->Vals[i]); *s=SimbInd;
      p->Nam=strcpy((char *)calloc(strlen(Buf)+1,SzC),Buf);
    }}
    else{
     B=s[2]; s[2]='\0';
     if(Ind=Inds->FindInd(s+1)){//нашли индексное выражение
      s[2]=B; *s='\0';
      b+=sprintf(b=Buf,"%s%d",P->Nam,Ind->GS[0]+i*Ind->GS[2]); *s=SimbInd;
      for(s+=2;*s;s++){
       if(*s==SimbInd){
        if(s[1]=='('){//действуем по индексному выражению
         if(Ind=IndExp->FindInd(s)){//нашли индексное выражение
          b+=sprintf(b,"%s",Ind->Vals[i]);
        }}
        else{
         B=s[2]; s[2]='\0';
         if(Ind=Inds->FindInd(s+1)){//нашли индексное выражение
          s[2]=B; b+=sprintf(b=Buf,"%d",Ind->GS[0]+i*Ind->GS[2]); s++;
       }}}
       else *b++=*s;
      }
      *b='\0'; p->Nam=strcpy((char *)calloc(strlen(Buf)+1,SzC),Buf);
   }}}
   else
    p->Nam=strcpy((char *)calloc(strlen(P->Nam)+1,SzC),P->Nam);
  }
  else p->Val=P->Val;
 }
 return R;
}

Parm *List(Parm *P){
 Parm *R=NULL,*Sled,**Pf=&R,*p; char *s,Buf[256],B,*b; TInd *Ind; int i,Steps;
 for(;P;P=P->Sled){
  Sled=P->Sled; P->Sled=NULL;
  Steps=DefSteps(P,-1,SYNHRON);
  for(int i=0;i<Steps+1;i++){
   *Pf=p=(Parm *)calloc(1,sizeof(Parm)); Pf=(Parm **)p; p->Line=P->Line;
   if(P->Nam){
    if(s=strchr(P->Nam,SimbInd)){//позиция символа индекса
     if(s[1]=='('){//действуем по индексному выражению
      if(Ind=IndExp->FindInd(s)){//нашли индексное выражение
       *s='\0'; sprintf(Buf,"%s%s",P->Nam,Ind->Vals[i]); *s=SimbInd;
       p->Nam=strcpy((char *)calloc(strlen(Buf)+1,SzC),Buf);
     }}
     else{
      B=s[2]; s[2]='\0';
      if(Ind=Inds->FindInd(s+1)){//нашли индексное выражение
       s[2]=B; *s='\0';
       b+=sprintf(b=Buf,"%s%d",P->Nam,Ind->GS[0]+i*Ind->GS[2]); *s=SimbInd;
       for(s+=2;*s;s++){
        if(*s==SimbInd){
         if(s[1]=='('){//действуем по индексному выражению
          if(Ind=IndExp->FindInd(s)){//нашли индексное выражение
           b+=sprintf(b,"%s",Ind->Vals[i]);
         }}
         else{
          B=s[2]; s[2]='\0';
          if(Ind=Inds->FindInd(s+1)){//нашли индексное выражение
           s[2]=B; b+=sprintf(b=Buf,"%d",Ind->GS[0]+i*Ind->GS[2]); s++;
        }}}
        else *b++=*s;
       }
       *b='\0'; p->Nam=strcpy((char *)calloc(strlen(Buf)+1,SzC),Buf);
    }}}
    else
     p->Nam=strcpy((char *)calloc(strlen(P->Nam)+1,SzC),P->Nam);
   }
   else p->Val=P->Val;
  }
  P->Sled=Sled;
 }
 return R;
}

Parm *NewParms(Parm *P,bool Type){
 Parm *R=NULL,**Pf=&R,*p; char *s; TInd *Ind;
 if(Type==SYNHRON){
  for(;P;P=P->Sled){
   if(P->Nam){
    if(s=strchr(P->Nam,SimbInd)){//позиция символа индекса
     if(s[1]=='('){//действуем по индексному выражению
      if(Ind=IndExp->FindInd(s)){//нашли индексное выражение
       for(int i=0;i<Ind->Steps;i++){
        *Pf=p=(Parm *)calloc(1,sizeof(Parm)); Pf=(Parm **)p; p->Line=P->Line;
        p->Nam=strcpy((char *)calloc(strlen(Ind->Vals[i])+1,SzC),Ind->Vals[i]);
        p->R=List_i(P->R,i);
     }}}
     else if(Ind=Inds->FindInd(s+1)){//нашли индексное выражение
      for(int i=0;i<Ind->Steps;i++){
       *Pf=p=(Parm *)calloc(1,sizeof(Parm)); Pf=(Parm **)p; p->Line=P->Line;
       sprintf(Inf,"%d",Ind->GS[0]+i*Ind->GS[2]);
       p->Nam=strcpy((char *)calloc(strlen(Inf)+1,SzC),Inf);
       p->R=List_i(P->R,i);
    }}}
    else{
     *Pf=p=(Parm *)calloc(1,sizeof(Parm)); Pf=(Parm **)p; p->Line=P->Line;
     p->Nam=strcpy((char *)calloc(strlen(P->Nam)+1,SzC),P->Nam);
     p->R=List_i(P->R,0);
   }}
   else{
    *Pf=p=(Parm *)calloc(1,sizeof(Parm)); Pf=(Parm **)p; p->Line=P->Line;
    p->Val=P->Val;
    p->R=List_i(P->R,0);
 }}}
 else{//INTERJER
  for(;P;P=P->Sled){
   if(P->Nam){
    if(s=strchr(P->Nam,SimbInd)){//позиция символа индекса
     if(s[1]=='('){//действуем по индексному выражению
      if(Ind=IndExp->FindInd(s)){//нашли индексное выражение
       for(int i=0;i<Ind->Steps;i++){
        *Pf=p=(Parm *)calloc(1,sizeof(Parm)); Pf=(Parm **)p; p->Line=P->Line;
        p->Nam=strcpy((char *)calloc(strlen(Ind->Vals[i])+1,SzC),Ind->Vals[i]);
        p->R=NewParms(P->R,Type);
     }}}
     else if(Ind=Inds->FindInd(s+1)){//нашли индексное выражение
      for(int i=0;i<Ind->Steps;i++){
       *Pf=p=(Parm *)calloc(1,sizeof(Parm)); Pf=(Parm **)p; p->Line=P->Line;
       sprintf(Inf,"%d",Ind->GS[0]+i*Ind->GS[2]);
       p->Nam=strcpy((char *)calloc(strlen(Inf)+1,SzC),Inf);
       p->R=NewParms(P->R,Type);
    }}}
    else{
     *Pf=p=(Parm *)calloc(1,sizeof(Parm)); Pf=(Parm **)p; p->Line=P->Line;
     p->Nam=strcpy((char *)calloc(strlen(P->Nam)+1,SzC),P->Nam);
     p->R=NewParms(P->R,Type);
   }}
   else{
    *Pf=p=(Parm *)calloc(1,sizeof(Parm)); Pf=(Parm **)p; p->Line=P->Line;
    p->Val=P->Val;
    p->R=NewParms(P->R,Type);
 }}}
 return R;
}

void IndexInstr(Root *I,bool Type){
 Root *N; Parm **Pf,*P,*p; char *s,Buf[256],B,*b; TInd *Ind; int Steps=-1;
 LowToUpp(I->Name);
 N=(Root *)calloc(1,sizeof(Root)); N->Sled=I->Sled; I->Sled=N;//новая инструкция
 Pf=&(N->First);
 I->Name=(char *)realloc(I->Name,(strlen(I->Name)+2)*SzC);
 strcpy(Inf,I->Name); sprintf(I->Name,"@%s",Inf);
 N->Name=strcpy((char *)calloc(strlen(Inf)+1,SzC),Inf); N->Line=I->Line;
 Pf=&(N->First);
 if(Type==SYNHRON){
  for(P=I->First;P;P=P->Sled){
   Steps=DefSteps(P,-1,Type);
   if(Steps>=0){
    for(int i=0;i<Steps+1;i++){
     *Pf=p=(Parm *)calloc(1,sizeof(Parm)); Pf=(Parm **)p; p->Line=P->Line;
     if(P->Nam){
      if(s=strchr(P->Nam,SimbInd)){//позиция символа индекса
       if(s[1]=='('){//действуем по индексному выражению
        if(Ind=IndExp->FindInd(s)){//нашли индексное выражение
         *s='\0'; sprintf(Buf,"%s%s",P->Nam,Ind->Vals[i]); *s=SimbInd;
         p->Nam=strcpy((char *)calloc(strlen(Buf)+1,SzC),Buf);
       }}
       else{
        *s='\0'; b+=sprintf(b=Buf,"%s",P->Nam); *s=SimbInd;
        B=s[2]; s[2]='\0';
        if(Ind=Inds->FindInd(s+1)){//нашли индексное выражение
         s[2]=B; b+=sprintf(b,"%d",Ind->GS[0]+i*Ind->GS[2]);
         for(s+=2;*s;s++){
          if(*s==SimbInd){
           if(s[1]=='('){//действуем по индексному выражению
            if(Ind=IndExp->FindInd(s)){//нашли индексное выражение
             b+=sprintf(b,"%s",Ind->Vals[i]); break;
           }}
           else{
            B=s[2]; s[2]='\0';
            if(Ind=Inds->FindInd(s+1)){//нашли индексное выражение
             s[2]=B; b+=sprintf(b,"%d",Ind->GS[0]+i*Ind->GS[2]); s++;
          }}}
          else *b++=*s;
         }
         *b='\0'; p->Nam=strcpy((char *)calloc(strlen(Buf)+2,SzC),Buf);
      }}}
      else
       p->Nam=strcpy((char *)calloc(strlen(P->Nam)+1,SzC),P->Nam);
      p->R=List_i(P->R,i);
     }
     else
      p->Val=P->Val;
   }}
 }}
 else{
  for(P=I->First;P;P=P->Sled){
   *Pf=p=(Parm *)calloc(1,sizeof(Parm)); Pf=(Parm **)p; p->Line=P->Line;
   p->Nam=strcpy((char *)calloc(strlen(P->Nam)+1,SzC),P->Nam);
   p->R=List(P->R);
}}}

bool MultiPly(Parm *Par,TRichEdit *R){
 char *inf=Inf,*s,NameInstr[256],NameOper[256]; TListSBVE ListSBVE;
 FILE *F; Root *I; bool Mode; Vary *V,*W=NULL,**Pv=&W; Elem *e,*E=NULL,**Pe=&E;
 Magazine *Mi=NULL,*Mo=NULL,*m; int Line;
 Body *B=NULL,**Pb=&B,*b; Stringer *T=NULL,**Pt=&T,*t;
 for(Root *o=L->Oper;o;o=o->Sled) if(o->First==Par){ Line=o->Line; break; }
//Диагностика
 if(OPRFILE=fopen(OprFile,"w")){
  fprintf(OPRFILE,
   "<html>\n<head>\n<title>ПРОТОКОЛ ОПЕРАТОРОВ</title>\n"
   "<meta http-equiv=\"Content-Type\" content=\"text/html;"
   "charset=windows-1251\">\n</head>\n"
   "<body bgcolor=\"#FFFFFF\" text=\"#800000\""
   " background=\"%s/fon2.jpg\""
   " font face=\"Times New Roman, Times, serif\">\n",
    WORKDIR);
 }
 Inds=new TInds;
 for(Parm *p=Par;p;p=p->Sled){
  if(!p->Nam||!p->R){ Inds->~TInds(); goto Ex; }
  Inds->AddInd(p);
 }
 IndExp=new TInds;
//Соберем индексированные выражения из стержней
 for(Stringer *T=L->T;T;T=T->Sled)
  if((s=strchr(T->Name,SimbInd))&&(s[1]=='('))
   IndExp->AddInd(s);
//Соберем индексированные выражения из твердых тел
 for(Body *B=L->B;B;B=B->Sled){
  if((s=strchr(B->Name,SimbInd))&&(s[1]=='('))
   IndExp->AddInd(s);
  if(B->BaseName&&(s=strchr(B->BaseName,SimbInd))&&(s[1]=='('))
   IndExp->AddInd(s);
 }
//Соберем индексированные выражения из переменных
 for(Vary *V=L->V;V;V=V->Sled)
  if((s=strchr(V->Name,SimbInd))&&(s[1]=='('))
   IndExp->AddInd(s);
//Соберем индексированные выражения из элементов
 for(Elem *E=L->E;E;E=E->Sled){
  if((s=strchr(E->Name,SimbInd))&&(s[1]=='('))
   IndExp->AddInd(s);
 }
//Соберем индексированные выражения из операторов
 for(Root *O=L->Oper->Sled;O;O=O->Sled){
  strcpy(NameOper,O->Name); LowToUpp(NameOper);
  if(IsSimbInd(O))
   TakeMagazine(&Mo,NameOper);
 }
 for(m=Mo;m;m=m->Sled){
  Root *O=FindOperLast(m->S);
  for(Parm *P=O->First;P;P=P->Sled){
   if(P->Nam&&(s=strchr(P->Nam,SimbInd))&&s[1]) IndExp->AddInd(s);
   for(Parm *p=P->R;p;p=p->Sled){
    if(p->Nam&&(s=strchr(p->Nam,SimbInd))&&s[1]) IndExp->AddInd(s);
    for(Parm *pp=p->R;pp;pp=pp->Sled)
     if(pp->Nam&&(s=strchr(pp->Nam,SimbInd))&&s[1]) IndExp->AddInd(s);
 }}}
//Сделаем индексированные операторы
 for(Root *O=L->Oper->Sled;O;O=O->Sled){
  if(*O->Name=='@') continue;
  strcpy(Inf,O->Name); LowToUpp(Inf);
  if(FindMagazine(Mo,Inf,&m)){
   IndexInstr(O,SYNHRON);
   O=O->Sled;
 }}
//Соберем индексированные выражения из инструкций
 for(I=L->Inst;I;I=I->Sled){
  strcpy(NameInstr,I->Name); LowToUpp(NameInstr);
  if(IsSimbInd(I))
   TakeMagazine(&Mi,NameInstr);
 }
 for(m=Mi;m;m=m->Sled){
  I=FindInstLast(m->S);
  for(Parm *P=I->First;P;P=P->Sled){
   if(P->Nam&&(s=strchr(P->Nam,SimbInd))&&s[1]) IndExp->AddInd(s);
   for(Parm *p=P->R;p;p=p->Sled)
    if(p->Nam&&(s=strchr(p->Nam,SimbInd))&&s[1]) IndExp->AddInd(s);
 }}
//Сделаем индексированные инструкции
// for(m=Mi;m;m=m->Sled){
//  I=FindInstLast(m->S);
 for(I=L->Inst;I;){
  strcpy(Inf,I->Name); LowToUpp(Inf);
  if(FindMagazine(Mi,Inf,&m)){
   Mode=!strcmp(Inf,"СРАВНИТЬ")||!strcmp(Inf,"СОВМЕСТИТЬ")?INTERJER:SYNHRON;
   IndexInstr(I,Mode);
   I=I->Sled;
  }
  I=I->Sled;
 }
//Вынем индексированные переменные
 if(V=L->V){ Vary *Sled; char c;
  while(Sled=V->Sled){
   if((s=strchr(Sled->Name,SimbInd))&&(c=s[2],s[2]='\0',Inds->FindInd(s+1))){
    V->Sled=Sled->Sled; s[2]=c;
    *Pv=Sled; Sled->Sled=NULL; Pv=(Vary **)((char *)Sled+SzL);
   }
   else V=V->Sled;
 }}
//Вынем индексированные элементы
 while((e=L->E)&&(s=strchr(e->Name,SimbInd))&&(Inds->FindInd(s+1)||Inds->FindInd(s+1))){
  L->E=e->Sled;
  *Pe=e; e->Sled=NULL; Pe=(Elem **)e;
 }
 if(e=L->E){ Elem *Sled;
  while(Sled=e->Sled){
   if((s=strchr(Sled->Name,SimbInd))&&(Inds->FindInd(s+1)||Inds->FindInd(s+1))){
    e->Sled=Sled->Sled;
    *Pe=Sled; Sled->Sled=NULL; Pe=(Elem **)Sled;
   }
   else e=e->Sled;
 }}
//Вынем индексированные твёрдые тела
 while((b=L->B)&&(s=strchr(b->Name,SimbInd))&&(Inds->FindInd(s+1)||Inds->FindInd(s+1))){
  L->B=b->Sled; *Pb=b; b->Sled=NULL; Pb=(Body **)b;
 }
 if(b=L->B){ Body *Sled;
  while(Sled=b->Sled){
   if((s=strchr(Sled->Name,SimbInd))&&(Inds->FindInd(s+1)||Inds->FindInd(s+1))){
    b->Sled=Sled->Sled;
    *Pb=Sled; Sled->Sled=NULL; Pb=(Body **)Sled;
   }
   else b=b->Sled;
 }}
//Вынем индексированные стержни
 while((t=L->T)&&(s=strchr(t->Name,SimbInd))&&(Inds->FindInd(s+1)||Inds->FindInd(s+1))){
  L->T=t->Sled; *Pt=t; t->Sled=NULL; Pt=(Stringer **)t;
 }
 if(t=L->T){ Stringer *Sled;
  while(Sled=t->Sled){
   if((s=strchr(Sled->Name,SimbInd))&&(Inds->FindInd(s+1)||Inds->FindInd(s+1))){
    t->Sled=Sled->Sled;
    *Pt=Sled; Sled->Sled=NULL; Pt=(Stringer **)Sled;
   }
   else t=t->Sled;
 }}
 if(W||E||B||T){
  fprintf(OPRFILE,
   "<h3 align=\"center\"><font color=\"#800000\">"
   "Оператор \"РАЗМНОЖИТЬ\" строки %d сгенерировал "
   "из индексированных выражений</font></h3>\n",Line);
  if(W){
   fprintf(OPRFILE,"<b>Переменные:</b><br>\n");
   fputs("<table width=\"100%\" border=\"1\">\n",OPRFILE);
   fputs("<tr>",OPRFILE);
   fputs("<td><div align=\"center\"><b>Исходные выражения</b></div></td>\n"
    "<td><div align=\"center\"><b>Новые переменные</b></div></td>\n</tr>\n",OPRFILE);
   for(V=W;V;V=V->Sled){
    if(V->Znach.C!=Pust){
     ListSBVE.AddProt(V);
     s=strchr(V->Name,SimbInd);
     if(s[1]=='('){
      if(!IndExp->IndexForm(V,ListSBVE)){
       fclose(OPRFILE); goto Ex3;
     }}
     else{
      if(!Inds->Index(V,ListSBVE)){
       fclose(OPRFILE); goto Ex3;
   }}}}
   ListSBVE.VtoFile();
   fputs("</table>\n",OPRFILE);
  }
  if(E){
   fprintf(OPRFILE,"<b>Элементы:</b><br>\n");
   fputs("<table width=\"100%\" border=\"1\">\n",OPRFILE);
   fputs("<tr>",OPRFILE);
   fputs("<td><div align=\"center\"><b>Исходные выражения</b></div></td>\n"
    "<td><div align=\"center\"><b>Новые элементы</b></div></td>\n</tr>",OPRFILE);
   for(e=E;e;e=e->Sled){
    ListSBVE.AddProt(e);
    s=strchr(E->Name,SimbInd);
    if(s[1]=='('){
     if(!IndExp->IndexForm(e,ListSBVE)){
      fclose(OPRFILE); goto Ex3;
    }}
    else{
     if(!Inds->Index(e,ListSBVE)){ fclose(OPRFILE); goto Ex3; }
   }}
   ListSBVE.EtoFile();
   fputs("</table\n",OPRFILE);
  }
  if(B){
   fprintf(OPRFILE,"<b>Твердые тела:</b><br>\n");
   fputs("<table width=\"100%\" border=\"1\">\n",OPRFILE);
   fputs("<tr>",OPRFILE);
   fputs("<td><div align=\"center\"><b>Исходные выражения</b></div></td>\n"
    "<td><div align=\"center\"><b>Новые твердые тела</b></div></td>\n</tr>",OPRFILE);
   for(b=B;b;b=b->Sled){
    ListSBVE.AddProt(b);
    if(!Inds->Index(b,ListSBVE)){ fclose(OPRFILE); goto Ex3; }
   }
   ListSBVE.BtoFile();
   fputs("</table\n",OPRFILE);
  }
  if(T){
   fprintf(OPRFILE,"<b>Стержни:</b><br>\n");
   fputs("<table width=\"100%\" border=\"1\">\n",OPRFILE);
   fputs("<tr>",OPRFILE);
   fputs("<td><div align=\"center\"><b>Исходные выражения</b></div></td>\n"
    "<td><div align=\"center\"><b>Новые стержни</b></div></td>\n</tr>",OPRFILE);
   for(t=T;t;t=t->Sled){
    ListSBVE.AddProt(t);
    if(!Inds->Index(t,ListSBVE)){ fclose(OPRFILE); goto Ex3; }
   }
   ListSBVE.TtoFile();
   fputs("</table\n",OPRFILE);
  }
  fputs("\n<br><hr></font></body>\n</html>\n",OPRFILE);
 }
 fclose(OPRFILE);
 delete Inds; delete IndExp;
 return true;
Ex:sprintf(inf,
 "Оператор \"РАЗМНОЖИТЬ\" должен быть записан в виде:<br>\n"
 "РАЗМНОЖИТЬ : <индекс-символ1>(<ОТ1>,<ДО1>[,<ШАГ1>]),<br>\n"
 "             <индекс-символ2>(<ОТ2>,<ДО2>[,<ШАГ2>]),...;,<br>\n"
 "например,    РАЗМНОЖИТЬ : i(1,5),k(2,5,1);<br>\n");
 if(F=fopen(OprFile,"a")){ Fputs(Inf,F); fclose(F); }
 Application->MessageBox(Inf,"Ошибка в операторе \*РАЗМНОЖИТЬ\*:",MB_OK);
 return false;
Ex3: delete Inds; delete IndExp;
 return false;
}
