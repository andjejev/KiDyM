#include "CA.h"
#include "EditList.h"
#include <io.h>

#define SYNHRON  0
#define INTERJER 1
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
  a$k=b$k+c$j; (L"a k-тое равно b k-тое плюс c j-тое")                        (3)
  P$k.fi$k=m$k*g*sin(alfa$j);                                                (4)
  D$(2*i+1)=2*D$(2*i); (L"D два i плюс первое равно удвоенному D два i-тое")  (5)
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

extern List *L; extern wchar_t SimbInd;
extern wchar_t OprFile[],Inf[],WORKDIR[],FILESHAP[],Sopr[],*sopr;
extern Cnst *Pust; extern FILE *OPRFILE;
extern TStringList *SLO; extern String SF;
static wchar_t FullHead[256];
extern TEditList *EditList;
Magazine *IndexBrak,*Mmes1,*Mmes2;

class StringerBodyVarElemArra{
 public:
  StringerBodyVarElemArra *Sled;
 private:
  wchar_t *Prot,*Form; StringerBodyVarElemArra *R;
 public:
  void FreeProt(){ if(Prot) free(Prot); };
  void FreeForm(){ if(Form) free(Form); };
  void SetProt(Stringer *T);
  void SetProt(Body *B);
  void SetProt(Vary *V);
  void SetProt(Elem *E);
  void SetProt(Arra *a);
  void SetForm(Stringer *T);
  void SetForm(Body *B);
  void SetForm(Vary *V);
  void SetForm(Arra *V);
  void SetForm(Elem *E);
  void SetElem(StringerBodyVarElemArra *r){ R=r; };
  wchar_t *GetProt(){ return Prot; }
  wchar_t *GetForm(){ return Form; }
};

void StringerBodyVarElemArra::SetProt(Vary *V){
 String S1=V->Name,S2;
 S1+=L"=L"; FormToStr(V->Znach,&S2,false); S1+=S2;
 Prot=(wchar_t *)calloc(S1.Length()+1,SzC); wcscpy(Prot,S1.c_str());
}

void StringerBodyVarElemArra::SetForm(Arra *A){
 String S1,S2;
 S1.sprintf(L"%s[%d]=L",A->Name,A->N);
 for(int i=0;i<A->N;i++){
  if(A->F) FormToStr(A->F[i],&S2,false);
  else if(A->A) S2.sprintf(L"%g",A->A[i]);
  if(i<A->N-1) S2+=L", L";
  S1+=S2;
 }
 Form=(wchar_t *)calloc(S1.Length()+1,SzC); wcscpy(Form,S1.c_str());
}

void StringerBodyVarElemArra::SetProt(Arra *a){
 String S1,S2;
 S1.sprintf(L"%s[%d]=L",a->Name,a->N);
 for(int i=0;i<a->N;i++){
  if(a->F) FormToStr(a->F[i],&S2,false);
  else if(a->A) S2.sprintf(L"%g",a->A[i]);
  if(i<a->N-1) S2+=L", L";
  S1+=S2;
 }
 Prot=(wchar_t *)calloc(S1.Length()+1,SzC); wcscpy(Prot,S1.c_str());
}

void StringerBodyVarElemArra::SetForm(Vary *V){
 String S1=V->Name,S2;
 S1+=L"=L"; FormToStr(V->Znach,&S2,false); S1+=S2;
 Form=(wchar_t *)calloc(S1.Length()+1,SzC); wcscpy(Form,S1.c_str());
}

void StringerBodyVarElemArra::SetProt(Elem *E){
 String S1=E->Name,S2;
 S1+=L"=L"; FormToStr(E->Znach,&S2,false); S1+=S2;
 Prot=(wchar_t *)calloc(S1.Length()+1,SzC); wcscpy(Prot,S1.c_str());
}

void StringerBodyVarElemArra::SetForm(Elem *E){
 String S1=E->Name,S2;
 S1+=L"=L"; FormToStr(E->Znach,&S2,false); S1+=S2;
 Form=(wchar_t *)calloc(S1.Length()+1,SzC); wcscpy(Form,S1.c_str());
}

void StringerBodyVarElemArra::SetForm(Body *B){
 String S1=B->Name,S2;
 if(B->BaseName){ S1+='~'; S1+=B->BaseName; }
 S1+=L" | L";
 for(Parm *P=B->KoordSys;P;P=P->Sled){
  if(P->Nam){
   S1+=P->Nam;
   if(P->R){
	S1+='(';
	for(Parm *p=P->R;p;p=p->Sled){
	 if(p->Nam) S1+=p->Nam; else S1+=p->Val; if(p->Sled) S1+=L",L";
	}
	S1+=')';
   }
   if(P->Sled) S1+=L", L";
 }}
 S1+=L" | L";
 for(Parm *P=B->Inert;P;P=P->Sled){
  if(P->Nam){
   S1+=P->Nam;
   if(P->R){ S1+='='; if(P->R->Nam) S1+=P->R->Nam; else S1+=P->R->Val; }
  }
  if(P->Sled) S1+=L", L";
 }
 Form=(wchar_t *)calloc(S1.Length()+1,SzC); wcscpy(Form,S1.c_str());
}

void StringerBodyVarElemArra::SetProt(Body *B){
 String S1=B->Name,S2;
 if(B->BaseName){ S1+='~'; S1+=B->BaseName; }
 S1+=L" | L";
 for(Parm *P=B->KoordSys;P;P=P->Sled){
  if(P->Nam){
   S1+=P->Nam;
   if(P->R){
	S1+='(';
	for(Parm *p=P->R;p;p=p->Sled){
	 if(p->Nam) S1+=p->Nam; else S1+=p->Val; if(p->Sled) S1+=L",L";
	}
	S1+=')';
   }
   if(P->Sled) S1+=L", L";
 }}
 S1+=L" | L";
 for(Parm *P=B->Inert;P;P=P->Sled){
  if(P->Nam){
   S1+=P->Nam;
   if(P->R){ S1+='='; if(P->R->Nam) S1+=P->R->Nam; else S1+=P->R->Val; }
  }
  if(P->Sled) S1+=L", L";
 }
 Prot=(wchar_t *)calloc(S1.Length()+1,SzC); wcscpy(Prot,S1.c_str());
}

void StringerBodyVarElemArra::SetForm(Stringer *T){
 String S1=T->Name,S2;
 S1+=L" \\ L";
 for(Parm *P=T->Parms;P;P=P->Sled){
  if(P->Nam){
   S1+=P->Nam;
   if(P->R){
    S1+='(';
    for(Parm *p=P->R;p;p=p->Sled){
     if(p->Nam) S1+=p->Nam; else S1+=p->Val; if(p->Sled) S1+=L",L";
    }
    S1+=')';
   }
   if(P->Sled) S1+=L", L";
 }}
 Form=(wchar_t *)calloc(S1.Length()+1,SzC); wcscpy(Form,S1.c_str());
}

void StringerBodyVarElemArra::SetProt(Stringer *T){
 String S1=T->Name,S2;
 S1+=L" \\ L";
 for(Parm *P=T->Parms;P;P=P->Sled){
  if(P->Nam){
   S1+=P->Nam;
   if(P->R){
    S1+='(';
    for(Parm *p=P->R;p;p=p->Sled){
     if(p->Nam) S1+=p->Nam; else S1+=p->Val; if(p->Sled) S1+=L",L";
    }
    S1+=')';
   }
   if(P->Sled) S1+=L", L";
 }}
 Prot=(wchar_t *)calloc(S1.Length()+1,SzC); wcscpy(Prot,S1.c_str());
}

class TListSBVEA{
 private:
  StringerBodyVarElemArra *T,*B,*V,*E,*A,**Pt,**Pb,**Pv,**Pe,**Pa;
 public:
  TListSBVEA(){
   V=NULL; E=NULL; B=NULL; T=NULL; A=NULL; Pv=&V; Pe=&E; Pb=&B; Pt=&T; Pa=&A;
  };
  void Add(Vary *v);
  void Add(Arra *v);
  void Add(Elem *e);
  void Add(Body *b);
  void Add(Stringer *b);
  void AddProt(Vary *v);
  void AddProt(Arra *a);
  void AddProt(Elem *e);
  void AddProt(Body *b);
  void AddProt(Stringer *b);
  void View();
  void VtoFile();
  void EtoFile();
  void BtoFile();
  void TtoFile();
  void AtoFile();
 ~TListSBVEA();
};

void TListSBVEA::VtoFile(){
 Form F; wchar_t Formula[1024],*f,*s,*Name; List *Lo=L;
 L=(List *)calloc(1,sizeof(List));
 L->Time=TakeVary(Lo->Time->Name);
 for(StringerBodyVarElemArra *v=V;v;v=v->Sled){
//  fputws(L"<tr>\n <td>",OPRFILE);
  SLO->Add(L"<tr>"); SLO->Add(L"<td>");
  if(v->GetProt()){
   StrToHTM(v->GetProt(),SLO);
   for(Name=s=v->GetProt();*s&&(*s!='=');s++); *s='\0';
   wcscpy(Inf,Name); LiterGrekToHtm(Inf); //Grek(Inf);
   wcscpy(Formula,s+1);
   for(f=Formula;*f;f++)
    switch(*f){
     case '[': *f='('; break;
     case ']': *f=')'; break;
     case '{': *f='('; break;
     case '}': *f=')'; break;
    }
   F=StrToForm(Formula);
   //PrintForm(false,OPRFILE,Name,F);
   sopr=FormToStringList(F,Name,Sopr,sopr,SLO,false,&SF);
   *s='=';
  }
//  fputws(L"</td>\n<td>",OPRFILE);
  SLO->Add(L"</td>"); sopr+=swprintf(sopr,L"<td>");
  if(v->GetForm()){
   for(Name=s=v->GetForm();*s&&(*s!='=');s++); *s='\0';
   wcscpy(Formula,s+1);
   for(f=Formula;*f;f++)
	switch(*f){
	 case '[': *f='('; break;
	 case ']': *f=')'; break;
	 case '{': *f='('; break;
	 case '}': *f=')'; break;
	}
   F=StrToForm(Formula);
   //PrintForm(false,OPRFILE,Name,F);
   sopr=FormToStringList(F,Name,Sopr,sopr,SLO,false,&SF);
   *s='=';
 }}
 free(L);
 L=Lo;
}

void TListSBVEA::AtoFile(){
 Form F; wchar_t Formula[1024],*f,*f0,*s,*Name; List *Lo=L;
 L=(List *)calloc(1,sizeof(List));
 L->Time=TakeVary(Lo->Time->Name);
 for(StringerBodyVarElemArra *a=A;a;a=a->Sled){
//  fputws(L"<tr>\n <td>",OPRFILE);
  SLO->Add(L"<tr>"); sopr+=swprintf(sopr,L"<td>");
  if(Name=s=a->GetProt()){
   for(;*s&&(*s!='=');s++); *s='\0';
   wcscpy(Inf,Name); LiterGrekToHtm(Inf); //Grek(Inf);
   wcscpy(Formula,s+1);
//   fwprintf(OPRFILE,L"%s=L",Inf);
   sopr+=swprintf(sopr,L"%s=L",Inf);
   for(f=f0=Formula;*f0;f0++)
	switch(*f0){
	 case '[': *f0='('; break;
	 case ']': *f0=')'; break;
	 case '{': *f0='('; break;
	 case '}': *f0=')'; break;
	 case ',':
	  *f0++='\0'; F=StrToForm(f);
//    PrintForm(false,OPRFILE,NULL,F);
	  sopr=FormToStringList(F,NULL,Sopr,sopr,SLO,false,&SF);
	  /*fputws(L", ",OPRFILE);*/ sopr+=swprintf(sopr,L", ");
	  while(*f0==' ') f0++; f=f0;
	}
   F=StrToForm(f);
// PrintForm(false,OPRFILE,NULL,F);
   sopr=FormToStringList(F,NULL,Sopr,sopr,SLO,false,&SF);
  }
//  fputws(L"</td>\n<td>",OPRFILE);
  sopr+=swprintf(sopr,L"</td>");
  SLO->Add(Sopr); *Sopr='\0'; sopr=Sopr;
  sopr+=swprintf(sopr,L"<td>");
  if(Name=s=a->GetForm()){
   for(;*s&&(*s!='=');s++); *s='\0';
   wcscpy(Inf,Name); LiterGrekToHtm(Inf); //Grek(Inf);
   wcscpy(Formula,s+1);
   //fwprintf(OPRFILE,L"%s=L",Inf);
   sopr+=swprintf(sopr,L"%s=L",Inf);
   for(f=f0=Formula;*f0;f0++)
	switch(*f0){
	 case '[': *f0='('; break;
	 case ']': *f0=')'; break;
	 case '{': *f0='('; break;
	 case '}': *f0=')'; break;
	 case ',':
	  *f0++='\0'; F=StrToForm(f);
//    PrintForm(false,OPRFILE,NULL,F);
	  sopr=FormToStringList(F,NULL,Sopr,sopr,SLO,false,&SF);
	  /*fputws(L", ",OPRFILE);*/ sopr+=swprintf(sopr,L", ");
	  while(*f0==' ') f0++; f=f0;
	}
   F=StrToForm(f); //PrintForm(false,OPRFILE,NULL,F);
   sopr=FormToStringList(F,NULL,Sopr,sopr,SLO,false,&SF);
 }}
 free(L);
 L=Lo;
}

void TListSBVEA::EtoFile(){
 Form F; wchar_t Formula[1024],*f,*s,*Name;
 for(StringerBodyVarElemArra *e=E;e;e=e->Sled){
//  fputws(L"<tr>\n <td>",OPRFILE);
  SLO->Add(L"<tr>"); sopr+=swprintf(sopr,L" <td>");
  if(e->GetProt()) StrToHTM(e->GetProt(),SLO);
//  fputws(L"</td>\n<td>",OPRFILE);
  SLO->Add(L"</td>"); sopr+=swprintf(sopr,L"<td>");
  if(e->GetForm()){
   for(Name=s=e->GetForm();*s&&(*s!='=');s++); *s++='\0';
   wcscpy(Formula,s);
   for(f=Formula;*f;f++)
	switch(*f){
	 case '[': *f='('; break;
	 case ']': *f=')'; break;
	 case '{': *f='('; break;
	 case '}': *f=')'; break;
	}
   F=StrToForm(Formula);
   //PrintForm(false,OPRFILE,Name,F);
   sopr=FormToStringList(F,Name,Sopr,sopr,SLO,false,&SF);
   *s='=';
}}}

void TListSBVEA::BtoFile(){
 Form F; wchar_t Formula[1024],*f,*s,*Name;
 for(StringerBodyVarElemArra *b=B;b;b=b->Sled){
//  fputws(L"<tr>\n <td>",OPRFILE);
  SLO->Add(L"<tr>"); sopr+=swprintf(sopr,L" <td>");
  if(b->GetProt())
   StrToHTM(b->GetProt(),SLO);
//  fputws(L"</td>\n<td>",OPRFILE);
  SLO->Add(L"</td>"); sopr+=swprintf(sopr,L" <td>");
  if(b->GetForm())
   StrToHTM(b->GetForm(),SLO);
}}

void TListSBVEA::TtoFile(){
 Form F; wchar_t Formula[1024],*f,*s,*Name;
 for(StringerBodyVarElemArra *t=T;t;t=t->Sled){
//  fputws(L"<tr>\n <td>",OPRFILE);
  SLO->Add(L"<tr>"); sopr+=swprintf(sopr,L" <td>");
  if(t->GetProt())
   StrToHTM(t->GetProt(),SLO);
//  fputws(L"</td>\n<td>",OPRFILE);
  SLO->Add(L"</tds>"); sopr+=swprintf(sopr,L" <td>");
  if(t->GetForm())
   StrToHTM(t->GetForm(),SLO);
}}

TListSBVEA::~TListSBVEA(){
 for(StringerBodyVarElemArra *t;t=T;){ T->FreeProt(); t->FreeForm(); T=t->Sled; free(t); }
 for(StringerBodyVarElemArra *b;b=B;){ b->FreeProt(); b->FreeForm(); B=b->Sled; free(b); }
 for(StringerBodyVarElemArra *v;v=V;){ v->FreeProt(); v->FreeForm(); V=v->Sled; free(v); }
 for(StringerBodyVarElemArra *e;e=E;){ e->FreeProt(); e->FreeForm(); E=e->Sled; free(e); }
 Pt=&T; Pb=&B; Pv=&V; Pe=&E;
}

void TListSBVEA::AddProt(Stringer *T){
 StringerBodyVarElemArra *t;
 *Pt=t=(StringerBodyVarElemArra *)calloc(1,sizeof(StringerBodyVarElemArra));
 Pt=(StringerBodyVarElemArra **)t; t->SetProt(T);
}

void TListSBVEA::AddProt(Body *B){
 StringerBodyVarElemArra *b;
 *Pb=b=(StringerBodyVarElemArra *)calloc(1,sizeof(StringerBodyVarElemArra));
 Pb=(StringerBodyVarElemArra **)b; b->SetProt(B);
}

void TListSBVEA::AddProt(Vary *V){  StringerBodyVarElemArra *v;
 *Pv=v=(StringerBodyVarElemArra *)calloc(1,sizeof(StringerBodyVarElemArra));
 Pv=(StringerBodyVarElemArra **)v; v->SetProt(V);
}

void TListSBVEA::AddProt(Arra *a){  StringerBodyVarElemArra *aa;
 *Pa=aa=(StringerBodyVarElemArra *)calloc(1,sizeof(StringerBodyVarElemArra));
 Pa=(StringerBodyVarElemArra **)aa; aa->SetProt(a);
}

void TListSBVEA::AddProt(Elem *E){ StringerBodyVarElemArra *e;
 *Pe=e=(StringerBodyVarElemArra *)calloc(1,sizeof(StringerBodyVarElemArra));
 Pe=(StringerBodyVarElemArra **)e; e->SetProt(E);
}

void TListSBVEA::Add(Stringer *T){
 StringerBodyVarElemArra *t;
 *Pt=t=(StringerBodyVarElemArra *)calloc(1,sizeof(StringerBodyVarElemArra));
 Pt=(StringerBodyVarElemArra **)t; t->SetForm(T);
}

void TListSBVEA::Add(Body *B){
 StringerBodyVarElemArra *b;
 *Pb=b=(StringerBodyVarElemArra *)calloc(1,sizeof(StringerBodyVarElemArra));
 Pb=(StringerBodyVarElemArra **)b; b->SetForm(B);
}

void TListSBVEA::Add(Vary *V){
 StringerBodyVarElemArra *v;
 *Pv=v=(StringerBodyVarElemArra *)calloc(1,sizeof(StringerBodyVarElemArra));
 Pv=(StringerBodyVarElemArra **)v; v->SetForm(V);
}

void TListSBVEA::Add(Arra *A){
 StringerBodyVarElemArra *a;
 *Pa=a=(StringerBodyVarElemArra *)calloc(1,sizeof(StringerBodyVarElemArra));
 Pa=(StringerBodyVarElemArra **)a; a->SetForm(A);
}

void TListSBVEA::Add(Elem *E){
 StringerBodyVarElemArra *e;
 *Pe=e=(StringerBodyVarElemArra *)calloc(1,sizeof(StringerBodyVarElemArra));
 Pe=(StringerBodyVarElemArra **)e; e->SetForm(E);
}

void ViewStringer(void **R){
 TEditList *EditListOld=EditList; FILE *F;
 int i,*MNN=(int *)calloc(3,SzI),H=Screen->Height-100;
 wchar_t *FileSh=L"shapkat",*Format=L"%-s%-s",**Head=(wchar_t **)calloc(1,SzV),
  *Fprn=L"index.prn",*Help=(wchar_t *)calloc(256,SzV),Dir[256],*s,
  *FH=(wchar_t *)calloc(256,SzC),Predki[32],Deti[16];
 if(F=_wfopen(FileSh,L"w")){
  wcscpy(Predki,L"Исходные выражения"); StrWINtoDOS(Predki);
  wcscpy(Deti,L"Новые записи \"стержни\""); StrWINtoDOS(Deti);
  swprintf(Inf,L"%s\xb3%s\n",Predki,Deti); fputws(Inf,F); fclose(F);
 }
 swprintf(FH,L"Оператор \"РАЗМНОЖИТЬ\" сгенерировал\n"
  L"\"стержни\" из индексированных выражений");
 Head[0]=(wchar_t *)calloc(256,SzC); wcscpy(Head[0],L"Новые записи \"стержень\"");
 for(i=0;i<2;i++) MNN[i]=i+1;
 wcscpy(Dir,WORKDIR); if(s=wcsrchr(Dir,'\\')) wcscpy(s,L"\\Help");
 swprintf(Help,L"%s\\ViewIndex.htm",Dir);
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
 wchar_t *FileSh=L"shapkab",*Format=L"%-s%-s",**Head=(wchar_t **)calloc(1,SzV),
  *Fprn=L"index.prn",*Help=(wchar_t *)calloc(256,SzV),Dir[256],*s,
  *FH=(wchar_t *)calloc(256,SzC),Predki[32],Deti[16];
 if(F=_wfopen(FileSh,L"w")){
  wcscpy(Predki,L"Исходные выражения"); StrWINtoDOS(Predki);
  wcscpy(Deti,L"Новые записи \"твердое тело\""); StrWINtoDOS(Deti);
  swprintf(Inf,L"%s\xb3%s\n",Predki,Deti); fputws(Inf,F); fclose(F);
 }
 swprintf(FH,L"Оператор \"РАЗМНОЖИТЬ\" сгенерировал\n"
  L"\"тердые тела\" из индексированных выражений");
 Head[0]=(wchar_t *)calloc(256,SzC); wcscpy(Head[0],L"Новые записи \"твердое тело\"");
 for(i=0;i<2;i++) MNN[i]=i+1;
 wcscpy(Dir,WORKDIR); if(s=wcsrchr(Dir,'\\')) wcscpy(s,L"\\Help");
 swprintf(Help,L"%s\\ViewIndex.htm",Dir);
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
 wchar_t *FileSh=L"shapkae",*Format=L"%-s%-s",**Head=(wchar_t **)calloc(1,SzV),
  *Fprn=L"index.prn",*Help=(wchar_t *)calloc(256,SzV),Dir[256],*s,
  *FH=(wchar_t *)calloc(256,SzC),Predki[32],Deti[16];
 if(F=_wfopen(FileSh,L"w")){
  wcscpy(Predki,L"Исходные выражения"); StrWINtoDOS(Predki);
  wcscpy(Deti,L"Элементы"); StrWINtoDOS(Deti);
  swprintf(Inf,L"%s\xb3%s\n",Predki,Deti); fputws(Inf,F); fclose(F);
 }
 swprintf(FH,L"Оператор \"РАЗМНОЖИТЬ\" сгенерировал\n"
  L"элементы из индексированных выражений");
 Head[0]=(wchar_t *)calloc(256,SzC); wcscpy(Head[0],L"Элементы");
 for(i=0;i<2;i++) MNN[i]=i+1;
 wcscpy(Dir,WORKDIR); if(s=wcsrchr(Dir,'\\')) wcscpy(s,L"\\Help");
 swprintf(Help,L"%s\\ViewIndex.htm",Dir);
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

void TListSBVEA::View(){
 FILE *F; int i,*MNN=(int *)calloc(3,SzI),H=Screen->Height-60;
 wchar_t *FileSh=L"shapkav",
  *Format=B&&E?L"%-s%-s%p%p":E||B?L"%-s%-s%p":L"%-s%-s",
  **Head=(wchar_t **)calloc(3,SzV),*s,*Fprn=L"index.prn",Predki[32],Deti[16],
  Dir[256],*Help=(wchar_t *)calloc(256,SzV);
 void (**Proc)(void **)=(void (**)(void **))calloc(2,SzV);
 if(F=_wfopen(FileSh,L"w")){
  wcscpy(Predki,L"Исходные выражения"); StrWINtoDOS(Predki);
  wcscpy(Deti,L"Переменные"); StrWINtoDOS(Deti);
  swprintf(Inf,L"%s\xb3%s\n",Predki,Deti); fputws(Inf,F); fclose(F);
 }
 if(B&&E){ Proc[0]=ViewElem; Proc[1]=ViewBody; }
 else if(E) Proc[0]=ViewElem;
 else if(B) Proc[0]=ViewBody;
 swprintf(FullHead,L"Оператор \"РАЗМНОЖИТЬ\" сгенерировал L"
  L"переменные из индексированных выражений");
 Head[0]=(wchar_t *)calloc(256,SzC); Head[1]=(wchar_t *)calloc(256,SzC);
 for(StringerBodyVarElemArra *v=V;v;v=v->Sled) v->SetElem(E);
 for(i=0;i<2;i++) MNN[i]=i+1;
 wcscpy(Head[0],L"Переменные"); wcscpy(Head[1],L"Элементы");
 wcscpy(Head[2],L"Новые записи \"твердое тело\"");
 wcscpy(Dir,WORKDIR); if(s=wcsrchr(Dir,'\\')) wcscpy(s,L"\\Help");
 swprintf(Help,L"%s\\ViewIndex.htm",Dir);
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
  TInd *Sled; wchar_t *SI,**Vals; int GS[3],Steps;
};

class TInds{
 public:
  TInd *R;
  TInds(){ R=NULL; }
  TInd *FindInd(wchar_t *I);
  bool AddInd(Parm *p);
  bool AddInd(wchar_t *s);
  bool IndLimit(wchar_t *Name,int &N);
  bool IndLimit(Stringer *B,int &N);
  bool IndLimit(Body *B,int &N);
  bool IndLimit(Vary *V,int &N);
  bool IndLimit(Arra *a,int &N);
  bool IndLimit(Elem *E,int &N);
  bool IndLimit(Form F,int &N);
  bool Index(Stringer *B,TListSBVEA &ListSBVEA);
  bool Index(Body *B,TListSBVEA &ListSBVEA);
  bool Index(Vary *V,TListSBVEA &ListSBVEA);
  bool Index(Arra *a,TListSBVEA &ListSBVEA);
  bool Index(Elem *E,TListSBVEA &ListSBVEA);
//  bool IndexForm(Body *V,TListSBVEA &ListSBVEA);
  bool IndexForm(Vary *V,TListSBVEA &ListSBVEA);
  bool IndexForm(Arra *a,TListSBVEA &ListSBVEA);
  bool IndexForm(Elem *E,TListSBVEA &ListSBVEA);
  bool NewName(wchar_t *Name,int i,wchar_t *New);
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

TInd *TInds::FindInd(wchar_t *I){
 if(*I==SimbInd) I++;
 for(TInd *r=R;r;r=r->Sled)
  if(!wcscmp(r->SI,I)) return r;
 return NULL;
}

bool ValParm(Parm *p,wchar_t *NameInd,wchar_t *S,int &Rez){
 Vary *V; Form Fp; FILE *F; double D;
 if(p->Nam){
  if(FindVary(p->Nam,&V)) Rez=(int)Round(Val(V->Znach));
  else{
   Fp=StrToForm(p->Nam);
   if(Fp.C->Atr==4){//неизвестная переменная
    swprintf(Inf,L"Имя \"%s\", указанное в операторе \"РАЗМНОЖИТЬ\"\n"
     L"как %s индекса %s,\nне найдено среди переменных задачи",p->Nam,S,NameInd);
    Error(0,0,true);
    return false;
   }
   else{
    if(!Valc(Fp,D)){
     swprintf(Inf,L"Формула \"%s\", указанное в операторе \"РАЗМНОЖИТЬ\"\n"
      L"как %s индекса %s,\nне может быть вычислена",p->Nam,S,NameInd);
     Error(0,0,true);
     return false;
    }
    else Rez=(int)Round(D);
 }}}
 else
  if(p->F.C!=Pust) Rez=Val(p->F);
 else Rez=(int)Round(p->Val);
 return true;
}

bool TInds::AddInd(Parm *p){
 Vary *V; FILE *F; wchar_t *Head,NameInd[256]; Form Fr; double Z; int GS[3];
 if(!p||!p->Nam){
  swprintf(Inf,L"В операторе \"РАЗМНОЖИТЬ\"\nне указано наименование индекса");
  Error(0,0,true);
  return false;
 }
 wcscpy(NameInd,p->Nam);
 p=p->R;
 if(!ValParm(p,NameInd,L"начальное значение",GS[0])) return false;
 p=p->Sled;
 if(!ValParm(p,NameInd,L"конечное значение",GS[1])) return false;
 if(p&&p->Sled){
  p=p->Sled;
  if(!ValParm(p,NameInd,L"шаг значений",GS[2])) return false;
 }
 else GS[2]=1;
 if((GS[0]<0)||(GS[1]<0)||(GS[2]<0)||(GS[1]<GS[0])){
  if(GS[0]<0)
   swprintf(Inf,
    L"Для индекса \"%s\" в операторе \"РАЗМНОЖИТЬ\" нижняя граница отрицательна",
    NameInd);
  else if(GS[1]<0)
   swprintf(Inf,
    L"Для индекса \"%s\" в операторе \"РАЗМНОЖИТЬ\" верхняя граница отрицательна",
    NameInd);
  else if(GS[2]<0)
   swprintf(Inf,
    L"Для индекса \"%s\" в операторе \"РАЗМНОЖИТЬ\" шаг задан отрицательным",
    NameInd);
  else if(GS[1]<GS[0])
   swprintf(Inf,
    L"Для индекса \"%s\" в операторе \"РАЗМНОЖИТЬ\" верхняя граница меньше нижней",
    NameInd);
  Atten(0,0);
  TakeMagazine(&IndexBrak,NameInd);
  return false;
 }
 V=TakeVary(NameInd); V->P=0; V->Znach.C=TakeCnst(GS[0]);
 TInd *Ind=(TInd *)calloc(1,sizeof(TInd)); Ind->Sled=R; R=Ind;
 Ind->SI=wcscpy((wchar_t *)calloc(wcslen(NameInd)+1,SzC),NameInd);
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

bool TInds::AddInd(wchar_t *S){//s - индексное выражение, начинающееся с '$'!
 Vary *V; FILE *F; wchar_t *Head,*s0,Buf[256],B; TInd *Ind,*I; int Ks=0,Steps=-1,St;
 Magazine *Mv=NULL,*m;
 if(s0=wcschr(S+1,'\''))
  *s0='\0';
 if(FindInd(S+1)){ if(s0) *s0='\''; return true; }
 if(s0) *s0='\'';
/* if(FindVary(S+1,&V)&&(V->Znach.C->Atr==3)){
  TInd *Ind=(TInd *)calloc(1,sizeof(TInd)); Ind->Sled=R; R=Ind;
  Ind->SI=wcscpy((wchar_t *)calloc(wcslen(S+1)+1,SzC),S+1);
  Ind->GS[0]=Ind->GS[1]=V->Znach.C->Val; Ind->GS[2]=1;
  Ind->Steps=0;
 }*/
//сначала определим число шагов как минимум из составляющих выражений
 for(wchar_t *s=S;*s;s++){//движемся по индексному выражению
//   if(*s=='\'') break;
  if(*s==SimbInd){ //встретили начало подвыражения
   if(s[1]!='('){//простой случай - одна переменая-индекс, заканчивается '\0'!
    B=s[2]; s[2]='\0';
    if(!(I=Inds->FindInd(s+1))){
     if(!FindMagazine(IndexBrak,s+1,&m)){
      if(s!=S){
       swprintf(Inf,L"Индекс \"%s\", входящий в выражение \"%s\""
        L"не указан в операторе \"РАЗМНОЖИТЬ\"",s+1,S);
       Error(0,0,true);
       s[2]=B; return false;
      }
      else{
//       swprintf(Inf,L"Индекс \"%s\", не указан в операторе \"РАЗМНОЖИТЬ\"",s+1);
//       Atten(0,0);
       TakeMagazine(&Mmes2,s+1);
       s[2]=B; return true;
    }}}
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
       swprintf(Inf,L"В индексном выражении \"%s\",входящем в выражение \"%s\""
        L"не хватает закрыт%s",s,S,Ks>1?"ых скобок":"ой скобки");
      else
       swprintf(Inf,L"В индексном выражении \"%s\",L"
        L"не хватает закрыт%s",s,Ks>1?"ых скобок":"ой скобки");
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
          swprintf(Inf,L"Для индекса \"%s\",L"
           L"входящего в выражение \"%s\""
           L"отсутствует переменная с его значениями",m->S,s);
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
        Ind->Vals=(wchar_t **)calloc(St+1,SzV); Ind->Steps=St;
        Ind->SI=wcscpy((wchar_t *)calloc(wcslen(s)+1,SzC),s);
//массив строк
        for(int i=0;i<St+1;i++){
         for(Magazine *m=Mv;m;m=m->Sled){
          if(I=Inds->FindInd(m->S)){
           FindVary(m->S,&V);
           V->Val=I->GS[0]+i*I->GS[2];
         }}
         swprintf(Buf,L"%d",(int)Round(Value(F)));//вычислили значение индексного подвыражения
         Ind->Vals[i]=wcscpy((wchar_t *)calloc(wcslen(Buf)+1,SzC),Buf);
       }}
       *(s0+1)=B; s=s0;
       if((Steps<0)||(St<Steps))
        Steps=St;
       break;
    }}}
    if(Ks){ FILE *F;
//осталась незакрытая скобка
     if(s!=S)
      swprintf(Inf,L"В индексном выражении \"%s\"входящего в выражение \"%s\""
       L"нарушен баланс скобок",S,s);
     else
      swprintf(Inf,L"В индексном выражении \"%s\"нарушен баланс скобок",S);
     Error(0,0,true);
     return false;
 }}}}
 if(!FindInd(S+1)){
//породим индексное выражение для всего выражения
//массив строк
  wchar_t **Vals=(wchar_t **)calloc(Steps+1,SzV);
  for(int i=0;i<Steps+1;i++){ wchar_t *b=Buf;
   for(wchar_t *s=S;*s;s++){//движемся по индексному выражению
    if(*s==SimbInd){ //встретили начало подвыражения
     if(s[1]!='('){//простой случай - одна переменая-индекс, заканчивается '\0'!
      B=s[2]; s[2]='\0';
      I=Inds->FindInd(s+1); s[2]=B;
      b+=swprintf(b,L"%d%s",I->GS[0]+i*I->GS[2],s+2); s++;
     }
     else{ Ks=0; //скобочное выражение
      for(s0=s+1;*s0;s0++){
       if(*s0=='(') Ks++;
       else if(*s0==')'){
        Ks--;
        if(!Ks){ Form F; //закрываем подвыражение
         B=*(s0+1); *(s0+1)='\0';
         I=FindInd(s); s0[1]=B;
         b+=swprintf(b,L"%s",I->Vals[i]);
         *(s0+1)=B; s=s0; break;
    }}}}}
    else *b++=*s;
   }
   *b='\0';
   Vals[i]=wcscpy((wchar_t *)calloc(wcslen(Buf)+1,SzC),Buf);
  }
  Ind=(TInd *)calloc(1,sizeof(TInd)); Ind->Sled=R; R=Ind;
  Ind->Vals=Vals; Ind->Steps=Steps;
  Ind->SI=wcscpy((wchar_t *)calloc(wcslen(S),SzC),S+1);
 }
 return true;
}

bool TInds::IndLimit(Form F,int &N){
 TInd *I; wchar_t B; Form FF;
 switch(F.C->Atr){
  case CNST: case PUST: return true; //break;
  case VARY:
   for(wchar_t *v=F.V->Name;*v;v++){
    if(*v==SimbInd){ v++;
     if(*v=='('){ int Ks=1;
      for(wchar_t *v0=++v;*v0;v0++){
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

bool TInds::IndLimit(wchar_t *Name,int &N){
 TInd *I; FILE *F; wchar_t B;
 for(wchar_t *s=Name;*s;s++){
  if(*s==SimbInd){ s++;
   if(*s=='('){ int Ks=1;
    for(wchar_t *s0=++s;*s0;s0++){
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
     swprintf(Inf,L"Имя \"%s\"\nсодержит в качестве индекса переменную,L"
      L" которая не указана в операторе \"РАЗМНОЖИТЬ\"",Name);
     Atten(0,0);
     N=0; s[1]=B; return true;
 }}}}
 return N>0;
}

bool TInds::IndLimit(Vary *V,int &N){
 TInd *I; FILE *F; wchar_t B;
 for(wchar_t *v=V->Name;*v;v++){
  if(*v==SimbInd){ v++;
   if(*v=='('){ int Ks=1;
    for(wchar_t *v0=++v;*v0;v0++){
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
     swprintf(Inf,
      L"Имя переменной \"%s\" содержит в качестве индекса переменную %s,\n"
      L"которая не указана в операторе \"РАЗМНОЖИТЬ\"",V->Name,v);
     Atten(V->Line,0);
     N=0; v[1]=B; return true;
 }}}}
 if(!IndLimit(V->Znach,N)){
  swprintf(Inf,L"Формула для переменной \"%s\"содержит переменные с индексами,\n"
   L"которые не указаны в операторе \"РАЗМНОЖИТЬ\"",V->Name);
  Atten(0,0);
  N=0; return true;
 }
 return N>0;
}

bool TInds::IndLimit(Arra *A,int &N){
 TInd *I; FILE *F; wchar_t B;
 for(wchar_t *a=A->Name;*a;a++){
  if(*a==SimbInd){ a++;
   if(*a=='('){ int Ks=1;
    for(wchar_t *a0=++a;*a0;a0++){
     if(*a0=='(') Ks++;
     else if(*a0==')'){
      Ks--;
      if(!Ks){ Form F;
       *a0='\0'; F=StrToForm(a); *a0=')';
       if(!IndLimit(F,N)) return false;
   }}}}
   else{
    B=a[1]; a[1]='\0';
    if(I=FindInd(a)){ a[1]=B;
     if(((I->GS[1]-I->GS[0])/I->GS[2]+1)<N)
      N=(I->GS[1]-I->GS[0])/I->GS[2]+1;
    }
    else{
     swprintf(Inf,
      L"Имя массива \"%s\" содержит в качестве индекса переменную %s,\n"
      L"которая не указана в операторе \"РАЗМНОЖИТЬ\"",A->Name,a);
     Atten(A->Line,0);
     N=0; a[1]=B; return true;
 }}}}
 if(A->F)
  for(int i=0;i<A->N;i++)
   if(!IndLimit(A->F[i],N)){
    swprintf(Inf,
    L"Формула с индексом [%d] для массива \"%s\"содержит переменные\n"
    L"с индексами, которые не указаны в операторе \"РАЗМНОЖИТЬ\"",i,A->Name);
  Atten(0,0);
  N=0; return true;
 }
 return N>0;
}

bool TInds::IndLimit(Stringer *T,int &N){ TInd *I; FILE *F; wchar_t b;
 IndLimit(T->Name,N);
 for(Parm *P=T->Parms;P;P=P->Sled){
  if(P->Nam){
   IndLimit(P->Nam,N);
   for(Parm *p=P->R;p;p=p->Sled){
    if(p->Nam) IndLimit(p->Nam,N);
 }}}
 return N>0;
}

bool TInds::IndLimit(Body *B,int &N){ TInd *I; FILE *F; wchar_t b;
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

bool TInds::IndLimit(Elem *E,int &N){ TInd *I; FILE *F; wchar_t B;
 for(wchar_t *e=E->Name;*e;e++){
  if(*e==SimbInd){ e++;
   if(*e=='('){ int Ks=1;
    for(wchar_t *e0=++e;*e0;e0++){
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
  swprintf(Inf,L"Формула для элемента \"%s\"\n"
   L"содержит переменные с индексами, которые не указаны<br>\n"
   L"в операторе \"РАЗМНОЖИТЬ\"",E->Name);
  Atten(0,0);
  N=0; return true;
 }
 return N>0;
}

bool TInds::NewName(wchar_t *Name,int i,wchar_t *New){
 TInd *I; wchar_t *s=Name,*n=New,B; Vary *V;
 for(s=Name,n=New;*s;s++){
  if(*s==SimbInd){ s++;
   if(*s=='('){ int Ks=1;
    for(wchar_t *s0=++s;*s0;s0++){
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
         swprintf(Inf,L"Индекс \"%s\", входящий в выражение \"%s\""
          L"отсутствует в операторе \"РАЗМНОЖИТЬ\"",m->S,s);
         Error(0,0,true);
         DelAllMagazine(&Mv); return false;
        }
        if((i*I->GS[2]+I->GS[0])>I->GS[1]){
         DelAllMagazine(&Mv); return false;
        }
        if(!(FindVary(m->S,&V))){ FILE *F;
//для индекса нет переменной
         swprintf(Inf,L"Для индекса \"%s\",входящего в выражение \"%s\"\n"
          L"не сгенерирована переменная с его значениями",m->S,s);
         Error(0,0,true);
         DelAllMagazine(&Mv); return false;
        }
//порядок!
        V->Val=i*I->GS[2]+I->GS[0];
       }
       n+=swprintf(n,L"%d",Val(F));
   }}}}
   else{
    B=s[1]; s[1]='\0';
    if((I=FindInd(s))&&(i*I->GS[2]+I->GS[0]<=I->GS[1])){
     n+=swprintf(n,L"%d",i*I->GS[2]+I->GS[0]); s[1]=B;
    }
    else{ s[1]=B; return false; }
  }}
  else *n++=*s;
 }
 *n='\0'; return true;
}

bool TInds::NewForm(Form Fv,int i,Form &Fn){
 switch(Fv.C->Atr){ wchar_t Name[256],*s; Form F,L,U,V,FT,FF; Arra *A; TInd *I;
  case PUST: case CNST: Fn=Fv; return true;
  case VARY:
   if(I=Inds->FindInd(Fv.V->Name)){//переменная есть индекс!
    Fn.C=TakeCnst(i*I->GS[2]+I->GS[0]); Fv.V->Znach=Fn;
   }
   else{
    if(s=wcschr(Fv.V->Name,SimbInd)){//имя переменной содержит индекс
     if(s[1]=='('){
      if(I=IndExp->FindInd(s)){
       *s='\0'; swprintf(Name,L"%s%s",Fv.V->Name,I->Vals[i]); *s=SimbInd;
     }}
     else
      if(!NewName(Fv.V->Name,i,Name)) return false;
     Fn.V=TakeVary(Name);
//Породим первообразные
     for(wchar_t *s=Name;s=wcschr(s+1,'\'');*s='\''){ *s='\0'; TakeVary(Name); }
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
    case  5: Fn=L%F; break;
    case  6: Fn=L^F;
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
   Fn.G=TakeIntg(FT,FF.V,U,V);
   break;
  case RUUT:
   F.V=Fv.R->x;
   if(!NewForm(F,i,FF)) return false;
   else if(!NewForm(Fv.R->a,i,U)) return false;
   else if(!NewForm(Fv.R->b,i,V)) return false;
   else if(!NewForm(Fv.R->f,i,FT)) return false;
   Fn.R=TakeSolu(FT,FF.V,U,V);
   break;
 }
 return true;
}

int Nstr,Kstr=70;

bool TInds::Index(Vary *V,TListSBVEA &ListSBVEA){
 int N=MaxInt; wchar_t Name[256]; Form F; Vary *v;
 if(!IndLimit(V,N)) return false;
 for(int i=0;i<N;i++){
  if(!NewName(V->Name,i,Name)) return false;
  if(!NewForm(V->Znach,i,F)) return false;
  if(FindVary(Name,&v)&&(v->Znach!=Pust)){
   swprintf(Inf,
    L"При обработке оператора \"РАЗМНОЖИТЬ\" в строке %d\n"
    L"обнаружена попытка повторной генерации переменной:\n%s",V->Line,Name);
   Application->MessageBoxA(Inf,L"Внимание!",MB_OK);
  }
  else{
   v=TakeVary(Name);
   v->Line=V->Line;
  }
  v->Znach=F; ListSBVEA.Add(v);
//Породим первообразные
  for(wchar_t *s=Name;s=wcschr(s+1,'\'');*s='\''){ *s='\0'; TakeVary(Name); }
 }
 return true;
}

bool TInds::IndexForm(Vary *V,TListSBVEA &ListSBVEA){
 wchar_t Name[256],*s,*t=wcschr(V->Name,SimbInd);
 TInd *Ind=FindInd(t); Form F; Vary *v;
 *t='\0';
 for(int i=0;i<Ind->Steps+1;i++){
  swprintf(Name,L"%s%s",V->Name,Ind->Vals[i]);
  if(!NewForm(V->Znach,i,F)) return false;
  v=TakeVary(Name); v->Znach=F; ListSBVEA.Add(v);
//Породим первообразные
  for(s=Name;s=wcschr(s+1,'\'');*s='\''){ *s='\0'; TakeVary(Name); }
 }
 *t=SimbInd;
 return true;
}

bool TInds::Index(Arra *a,TListSBVEA &ListSBVEA){
 int N=MaxInt; wchar_t Name[256]; Form *F; Arra *A;
 if(!IndLimit(a,N)) return false;
 for(int i=0;i<N;i++){
  if(!NewName(a->Name,i,Name)) return false;
  if(a->F)
   F=(Form *)calloc(a->N,sizeof(Form));
   for(int j=0;j<a->N;j++){
    if(!NewForm(a->F[j],i,F[j])) return false;
  }
  A=TakeArra(Name); A->F=F; A->A=a->A; A->N=a->N; A->Line=a->Line;
  ListSBVEA.Add(A);
 }
 return true;
}

bool TInds::IndexForm(Arra *a,TListSBVEA &ListSBVEA){
 wchar_t Name[256],*s,*t=wcschr(a->Name,SimbInd); TInd *Ind=FindInd(t);
 Form *F; Arra *A;
 *t='\0'; if(a->F) F=(Form *)calloc(a->N,sizeof(Form));
 for(int i=0;i<Ind->Steps+1;i++){
  swprintf(Name,L"%s%s",a->Name,Ind->Vals[i]);
  if(a->F)
   for(int i=0;i<a->N;i++){
    if(!NewForm(a->F[i],i,F[i])) return false;
  }
  A=TakeArra(Name); A->F=F; A->A=a->A; A->N=a->N; A->Line=a->Line;
  ListSBVEA.Add(A);
 }
 *t=SimbInd;
 return true;
}

bool TInds::IndexForm(Elem *E,TListSBVEA &ListSBVEA){
 wchar_t Name[256],*s,*v; TInd *Ind; Form F; Elem *e;
 s=wcschr(E->Name,SimbInd); Ind=FindInd(s);
 for(int i=0;i<1+Ind->Steps;i++){
  if(!NewForm(E->Znach,i,F)) return false;
  *s='\0'; swprintf(Name,L"%s%s",E->Name,Ind->Vals[i]); *s=SimbInd;
  e=TakeElem(Name); e->Znach=F;
  v=wcschr(Name,'.');
  e->K=TakeVary(v+1);
  ListSBVEA.Add(e);
 }
 return true;
}

bool TInds::Index(Elem *E,TListSBVEA &ListSBVEA){
 int N=MaxInt; wchar_t Name[256],*s; Form F; Elem *e; Vary *v;
 if(!IndLimit(E,N)) return false;
 for(int i=0;i<N;i++){
  if(!NewName(E->Name,i,Name)) return false;
  if(s=wcschr(Name,'.')){
   v=TakeVary(s+1);
//Породим первообразные
   for(wchar_t *s=Name;s=wcschr(s+1,'\'');*s='\''){ *s='\0'; TakeVary(Name); }
  }
  if(!NewForm(E->Znach,i,F)) return false;
  e=TakeElem(Name); e->Znach=F; e->K=v; ListSBVEA.Add(e);
 }
 return true;
}

bool TInds::Index(Body *B,TListSBVEA &ListSBVEA){
 int N=MaxInt,j; wchar_t Name[256],BaseName[256],*s; Body *b; Parm *pn;
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
    b->KoordSys->Nam=wcscpy((wchar_t *)malloc((wcslen(Name)+1)*SzC),Name);
   }
   for(Parm *P1=B->KoordSys,*P2=b->KoordSys;P1;P1=P1->Sled,P2=P2->Sled){
    if(P1->Nam){
     if(!NewName(P1->Nam,i,Name)) return false;
     P2->Nam=wcscpy((wchar_t *)malloc((wcslen(Name)+1)*SzC),Name);
     if(P1->R){
      P2->R=(Parm *)calloc(KolElem(P1->R),sizeof(Parm));
      for(j=0;j<KolElem(P1->R)-1;j++) P2->R[j].Sled=P2->R+j+1;
      for(Parm *p1=P1->R,*p2=P2->R;p1;p1=p1->Sled,p2=p2->Sled){
       if(p1->Nam){
        if(!NewName(p1->Nam,i,Name)) return false;
        p2->Nam=wcscpy((wchar_t *)malloc((wcslen(Name)+1)*SzC),Name);
       }
       else p2->Val=p1->Val;
  }}}}}
  if(B->Inert){
   b->Inert=(Parm *)calloc(KolElem(B->Inert),sizeof(Parm));
   for(j=0;j<KolElem(B->Inert)-1;j++) b->Inert[j].Sled=b->Inert+j+1;
   if(B->Inert->Nam){
    if(!NewName(B->Inert->Nam,i,Name)) return false;
    b->Inert->Nam=wcscpy((wchar_t *)malloc((wcslen(Name)+1)*SzC),Name);
   }
   for(Parm *P1=B->Inert,*P2=b->Inert;P1;P1=P1->Sled,P2=P2->Sled){
    if(P1->Nam){
     if(!NewName(P1->Nam,i,Name)) return false;
     P2->Nam=wcscpy((wchar_t *)malloc((wcslen(Name)+1)*SzC),Name);
     if(P1->R){
      P2->R=(Parm *)calloc(1,sizeof(Parm));
      if(P1->R->Nam){
       if(!NewName(P1->R->Nam,i,Name)) return false;
       P2->R->Nam=wcscpy((wchar_t *)malloc((wcslen(Name)+1)*SzC),Name);
      }
      else P2->R->Val=P1->R->Val;
  }}}}
  ListSBVEA.Add(b);
 }
 return true;
}

bool TInds::Index(Stringer *T,TListSBVEA &ListSBVEA){
 int N=MaxInt,j; wchar_t Name[256],*s; Stringer *t; Parm *pn;
 if(!IndLimit(T,N)) return false;
 for(int i=0;i<N;i++){
  if(!NewName(T->Name,i,Name)) return false;
  t=TakeStringer(Name); t->Line=T->Line;
  if(T->Parms){
   t->Parms=(Parm *)calloc(KolElem(T->Parms),sizeof(Parm));
   for(j=0;j<KolElem(T->Parms)-1;j++) t->Parms[j].Sled=t->Parms+j+1;
   if(T->Parms->Nam){
    if(!NewName(T->Parms->Nam,i,Name)) return false;
    t->Parms->Nam=wcscpy((wchar_t *)calloc((wcslen(Name)+1),SzC),Name);
   }
   for(Parm *P1=T->Parms,*P2=t->Parms;P1;P1=P1->Sled,P2=P2->Sled){
    if(P1->Nam){
     if(!NewName(P1->Nam,i,Name)) return false;
     P2->Nam=(wchar_t *)calloc(wcslen(Name)+1,SzC);
     wcscpy(P2->Nam,Name);
     if(P1->R){
      P2->R=(Parm *)calloc(KolElem(P1->R),sizeof(Parm));
      for(j=0;j<KolElem(P1->R)-1;j++) P2->R[j].Sled=P2->R+j+1;
      for(Parm *p1=P1->R,*p2=P2->R;p1;p1=p1->Sled,p2=p2->Sled){
       if(p1->Nam){
        if(!NewName(p1->Nam,i,Name)) return false;
        p2->Nam=(wchar_t *)calloc(wcslen(Name)+1,SzC);
        wcscpy(p2->Nam,Name);
       }
       else p2->Val=p1->Val;
  }}}}}
  ListSBVEA.Add(t);
 }
 return true;
}

bool IsSimbInd(Root *I){
 for(Parm *P=I->First;P;P=P->Sled){
  if(P->Nam&&wcschr(P->Nam,SimbInd)) return true;
  for(Parm *p=P->R;p;p=p->Sled){
   if(p->Nam&&wcschr(p->Nam,SimbInd)) return true;
 }}
 return false;
}

Parm *IsSimbInd(Parm *R){
 for(Parm *p=R;p;p=p->Sled) if(p->Nam&&wcschr(p->Nam,SimbInd)) return p;
 return NULL;
}

wchar_t *TakeExp(wchar_t *Beg){//возвращаем конец индексного выражения, куда ставим \0
 int Ks=1; wchar_t *s0=Beg;
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
 wchar_t *s,B,*b; TInd *Ind; Parm *p;
// if(!P->Nam) return Steps==-1?0:Steps;
 if(!P->Nam||!(s=wcschr(P->Nam,SimbInd))&&!P->R) return Steps;
 if(P->Nam&&(s=wcschr(P->Nam,SimbInd))){//позиция символа индекса
  if(s[1]=='('){//действуем по индексному выражению
   if(Ind=IndExp->FindInd(s)){//нашли индексное выражение
    if(Type==SYNHRON){
     if((Steps<0)||(Ind->Steps<Steps)) Steps=Ind->Steps;
    }
    else if(Type==INTERJER)
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
    else if(Type==INTERJER)
     if((Steps<0)||(Ind->Steps>Steps)) Steps=Ind->Steps;
    for(p=P->R;p;p=p->Sled) Steps=DefSteps(p,Steps,Type);
    for(s+=2;*s;s++){
     if(*s==SimbInd){
      if(s[1]=='('){//действуем по индексному выражению
       if(Ind=IndExp->FindInd(s)){//нашли индексное выражение
        if(Type==SYNHRON){
         if((Steps<0)||(Ind->Steps<Steps)) Steps=Ind->Steps;
        }
        else if(Type==INTERJER)
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
        else if(Type==INTERJER)
         if((Steps<0)||(Ind->Steps>Steps)) Steps=Ind->Steps;
       }
       for(p=P->R;p;p=p->Sled) Steps=DefSteps(p,Steps,Type);
      }
     }
    }
   }
   else return -1;
  }
 }
 else{
  for(p=P->R;p;p=p->Sled)
   Steps=DefSteps(p,Steps,Type);
 }
 return Steps==-1?0:Steps;
}

Parm *List_i(Parm *P,int i){
 Parm *R=NULL,**Pf=&R,*p; wchar_t *s,Buf[256],B,*b; TInd *Ind;
 for(;P;P=P->Sled){
  *Pf=p=(Parm *)calloc(1,sizeof(Parm)); Pf=(Parm **)p; p->Line=P->Line;
  if(P->Nam){
   if(s=wcschr(P->Nam,SimbInd)){//позиция символа индекса
    if(s[1]=='('){//действуем по индексному выражению
     if(Ind=IndExp->FindInd(s)){//нашли индексное выражение
      *s='\0'; swprintf(Buf,L"%s%s",P->Nam,Ind->Vals[i]); *s=SimbInd;
      p->Nam=wcscpy((wchar_t *)calloc(wcslen(Buf)+1,SzC),Buf);
    }}
    else{
     B=s[2]; s[2]='\0';
     if(Ind=Inds->FindInd(s+1)){//нашли индексное выражение
      s[2]=B; *s='\0';
      b+=swprintf(b=Buf,L"%s%d",P->Nam,Ind->GS[0]+i*Ind->GS[2]); *s=SimbInd;
      for(s+=2;*s;s++){
       if(*s==SimbInd){
        if(s[1]=='('){//действуем по индексному выражению
         if(Ind=IndExp->FindInd(s)){//нашли индексное выражение
          b+=swprintf(b,L"%s",Ind->Vals[i]);
        }}
        else{
         B=s[2]; s[2]='\0';
         if(Ind=Inds->FindInd(s+1)){//нашли индексное выражение
          s[2]=B; b+=swprintf(b=Buf,L"%d",Ind->GS[0]+i*Ind->GS[2]); s++;
       }}}
       else *b++=*s;
      }
      *b='\0'; p->Nam=wcscpy((wchar_t *)calloc(wcslen(Buf)+1,SzC),Buf);
   }}}
   else
    p->Nam=wcscpy((wchar_t *)calloc(wcslen(P->Nam)+1,SzC),P->Nam);
  }
  else p->Val=P->Val;
 }
 return R;
}

Parm *List(Parm *P){
 Parm *R=NULL,*Sled,**Pf=&R,*p; wchar_t *s,Buf[256],B,*b; TInd *Ind; int i,Steps;
 for(;P;P=P->Sled){
  Sled=P->Sled; P->Sled=NULL;
  Steps=DefSteps(P,-1,SYNHRON);
  if(Steps==-1){ *Pf=p=P; Pf=(Parm **)p; }
  for(int i=0;i<Steps+1;i++){
   *Pf=p=(Parm *)calloc(1,sizeof(Parm)); Pf=(Parm **)p; p->Line=P->Line;
   if(P->Nam){
    if(s=wcschr(P->Nam,SimbInd)){//позиция символа индекса
     if(s[1]=='('){//действуем по индексному выражению
      if(Ind=IndExp->FindInd(s)){//нашли индексное выражение
       *s='\0'; swprintf(Buf,L"%s%s",P->Nam,Ind->Vals[i]); *s=SimbInd;
       p->Nam=wcscpy((wchar_t *)calloc(wcslen(Buf)+1,SzC),Buf);
     }}
     else{
      B=s[2]; s[2]='\0';
      if(Ind=Inds->FindInd(s+1)){//нашли индексное выражение
       s[2]=B; *s='\0';
       b+=swprintf(b=Buf,L"%s%d",P->Nam,Ind->GS[0]+i*Ind->GS[2]); *s=SimbInd;
       for(s+=2;*s;s++){
        if(*s==SimbInd){
         if(s[1]=='('){//действуем по индексному выражению
          if(Ind=IndExp->FindInd(s)){//нашли индексное выражение
           b+=swprintf(b,L"%s",Ind->Vals[i]);
         }}
         else{
          B=s[2]; s[2]='\0';
          if(Ind=Inds->FindInd(s+1)){//нашли индексное выражение
           s[2]=B; b+=swprintf(b=Buf,L"%d",Ind->GS[0]+i*Ind->GS[2]); s++;
        }}}
        else *b++=*s;
       }
       *b='\0'; p->Nam=wcscpy((wchar_t *)calloc(wcslen(Buf)+1,SzC),Buf);
       if(P->R){ Parm **ppR=&(p->R),*pr; wchar_t *bi;
        for(Parm *pR=P->R;pR;pR=pR->Sled){
         *ppR=pr=(Parm *)calloc(1,sizeof(Parm)); ppR=(Parm **)pr;
         if(pR->Nam){
          if((bi=wcschr(pR->Nam,SimbInd))&&(!wcscmp(bi+1,Ind->SI))){
           *bi='\0';
           swprintf(Buf,L"%s%d",pR->Nam,Ind->GS[0]+i*Ind->GS[2]);
           *bi=SimbInd;
           pr->Nam=wcscpy((wchar_t *)calloc(wcslen(Buf)+1,SzC),Buf);
          }
          else pr->Nam=pR->Nam;
         }
         else pr->Val=pR->Val;
    }}}}}
    else
     p->Nam=wcscpy((wchar_t *)calloc(wcslen(P->Nam)+1,SzC),P->Nam);
   }
   else p->Val=P->Val;
  }
  P->Sled=Sled;
 }
 return R;
}
/*
Parm *NewParms(Parm *P,bool Type){
 Parm *R=NULL,**Pf=&R,*p; wchar_t *s; TInd *Ind;
 if(Type==SYNHRON){
  for(;P;P=P->Sled){
   if(P->Nam){
    if(s=wcschr(P->Nam,SimbInd)){//позиция символа индекса
     if(s[1]=='('){//действуем по индексному выражению
      if(Ind=IndExp->FindInd(s)){//нашли индексное выражение
       for(int i=0;i<Ind->Steps;i++){
        *Pf=p=(Parm *)calloc(1,sizeof(Parm)); Pf=(Parm **)p; p->Line=P->Line;
        p->Nam=wcscpy((wchar_t *)calloc(wcslen(Ind->Vals[i])+1,SzC),Ind->Vals[i]);
        p->R=List_i(P->R,i);
     }}}
     else if(Ind=Inds->FindInd(s+1)){//нашли индексное выражение
      for(int i=0;i<Ind->Steps;i++){
       *Pf=p=(Parm *)calloc(1,sizeof(Parm)); Pf=(Parm **)p; p->Line=P->Line;
       swprintf(Inf,L"%d",Ind->GS[0]+i*Ind->GS[2]);
       p->Nam=wcscpy((wchar_t *)calloc(wcslen(Inf)+1,SzC),Inf);
       p->R=List_i(P->R,i);
    }}}
    else{
     *Pf=p=(Parm *)calloc(1,sizeof(Parm)); Pf=(Parm **)p; p->Line=P->Line;
     p->Nam=wcscpy((wchar_t *)calloc(wcslen(P->Nam)+1,SzC),P->Nam);
     p->R=List_i(P->R,0);
   }}
   else{
    *Pf=p=(Parm *)calloc(1,sizeof(Parm)); Pf=(Parm **)p; p->Line=P->Line;
    p->Val=P->Val;
    p->R=List_i(P->R,0);
 }}}
 else if(Type==INTERJER){
  for(;P;P=P->Sled){
   if(P->Nam){
    if(s=wcschr(P->Nam,SimbInd)){//позиция символа индекса
     if(s[1]=='('){//действуем по индексному выражению
      if(Ind=IndExp->FindInd(s)){//нашли индексное выражение
       for(int i=0;i<Ind->Steps;i++){
        *Pf=p=(Parm *)calloc(1,sizeof(Parm)); Pf=(Parm **)p; p->Line=P->Line;
        p->Nam=wcscpy((wchar_t *)calloc(wcslen(Ind->Vals[i])+1,SzC),Ind->Vals[i]);
        p->R=NewParms(P->R,Type);
     }}}
     else if(Ind=Inds->FindInd(s+1)){//нашли индексное выражение
      for(int i=0;i<Ind->Steps;i++){
       *Pf=p=(Parm *)calloc(1,sizeof(Parm)); Pf=(Parm **)p; p->Line=P->Line;
       swprintf(Inf,L"%d",Ind->GS[0]+i*Ind->GS[2]);
       p->Nam=wcscpy((wchar_t *)calloc(wcslen(Inf)+1,SzC),Inf);
       p->R=NewParms(P->R,Type);
    }}}
    else{
     *Pf=p=(Parm *)calloc(1,sizeof(Parm)); Pf=(Parm **)p; p->Line=P->Line;
     p->Nam=wcscpy((wchar_t *)calloc(wcslen(P->Nam)+1,SzC),P->Nam);
     p->R=NewParms(P->R,Type);
   }}
   else{
    *Pf=p=(Parm *)calloc(1,sizeof(Parm)); Pf=(Parm **)p; p->Line=P->Line;
    p->Val=P->Val;
    p->R=NewParms(P->R,Type);
 }}}
 return R;
}
*/
void IndexInstr(Root *I,bool Type){
 Root *N; Parm **Pf,*P,*p; wchar_t *s,Buf[256],B,*b; TInd *Ind; int Steps=-1;
 bool FindInd=false;
 LowToUpp(I->Name);
 N=(Root *)calloc(1,sizeof(Root)); N->Sled=I->Sled; I->Sled=N;//новая инструкция
 Pf=&(N->First);
 I->Name=(wchar_t *)realloc(I->Name,(wcslen(I->Name)+2)*SzC);
 wcscpy(Inf,I->Name); swprintf(I->Name,L"@%s",Inf);
 N->Name=wcscpy((wchar_t *)calloc(wcslen(Inf)+1,SzC),Inf); N->Line=I->Line;
 if(Type==SYNHRON){
  for(P=I->First;P;P=P->Sled){
   Steps=DefSteps(P,-1,Type);
   if(Steps<0){ Steps=0; FindInd=false; }
   else FindInd=true;
   if(Steps>=0){
    for(int i=0;i<Steps+1;i++){
     *Pf=p=(Parm *)calloc(1,sizeof(Parm)); Pf=(Parm **)p; p->Line=P->Line;
     if(P->Nam){
      if((s=wcschr(P->Nam,SimbInd))&&FindInd){//позиция символа индекса
       if(s[1]=='('){//действуем по индексному выражению
        if(Ind=IndExp->FindInd(s)){//нашли индексное выражение
         *s='\0'; swprintf(Buf,L"%s%s",P->Nam,Ind->Vals[i]); *s=SimbInd;
         p->Nam=wcscpy((wchar_t *)calloc(wcslen(Buf)+1,SzC),Buf);
       }}
       else{
        *s='\0'; b+=swprintf(b=Buf,L"%s",P->Nam); *s=SimbInd;
        B=s[2]; s[2]='\0';
        if(Ind=Inds->FindInd(s+1)){//нашли индексное выражение
         s[2]=B; b+=swprintf(b,L"%d",Ind->GS[0]+i*Ind->GS[2]);
         for(s+=2;*s;s++){
          if(*s==SimbInd){
           if(s[1]=='('){//действуем по индексному выражению
            if(Ind=IndExp->FindInd(s)){//нашли индексное выражение
             b+=swprintf(b,L"%s",Ind->Vals[i]); break;
           }}
           else{
            B=s[2]; s[2]='\0';
            if(Ind=Inds->FindInd(s+1)){//нашли индексное выражение
             s[2]=B; b+=swprintf(b,L"%d",Ind->GS[0]+i*Ind->GS[2]); s++;
          }}}
          else *b++=*s;
         }
         *b='\0'; p->Nam=wcscpy((wchar_t *)calloc(wcslen(Buf)+2,SzC),Buf);
      }}}
      else
       p->Nam=wcscpy((wchar_t *)calloc(wcslen(P->Nam)+1,SzC),P->Nam);
      p->R=List_i(P->R,i);
     }
     else
      p->Val=P->Val;
 }}}}
 else if(Type==INTERJER){
  for(P=I->First;P;P=P->Sled){
   *Pf=p=(Parm *)calloc(1,sizeof(Parm)); Pf=(Parm **)p; p->Line=P->Line;
   p->Nam=wcscpy((wchar_t *)calloc(wcslen(P->Nam)+1,SzC),P->Nam);
   p->R=List(P->R);
}}}

bool MultiPly(Parm *Par,TRichEdit *R,TCanvas *Canvas){
 wchar_t *inf=Inf,*s,NameInstr[256],NameOper[256],bb; TListSBVEA ListSBVEA;
 FILE *F; Root *I; Vary *V,*W=NULL,**Pv=&W; Elem *e,*E=NULL,**Pe=&E;
 Magazine *Mi=NULL,*Mo=NULL,*m; int Line,Mode,i; Arra *A=NULL,*a,**Pa=&A;
 Body *B=NULL,**Pb=&B,*b; Stringer *T=NULL,**Pt=&T,*t; TInd *ind;
 DelAllMagazine(&Mmes1); DelAllMagazine(&Mmes2);
 for(Root *o=L->Oper;o;o=o->Sled) if(o->First==Par){ Line=o->Line; break; }
//Диагностика
// if(OPRFILE=_wfopen(OprFile,!_waccess(OprFile,0)?L"a":L"w")){
 if(!SLO) SLO=new TStringList;
 if(SLO){
/*  fwprintf(OPRFILE,
   L"<html>\n<head>\n<title>ПРОТОКОЛ ОПЕРАТОРОВ</title>\n"
   L"<meta http-equiv=\"Content-Type\" content=\"text/html;"
   L"wchar_tset=windows-1251\">\n</head>\n"
   L"<body bgcolor=\"#FFFFFF\" text=\"#800000\""
   L" background=\"%s/fon2.jpg\""
   L" font face=\"Times New Roman, Times, serif\">\n",
	WORKDIR);*/
  SLO->Add(L"<html>");
  SLO->Add(L"<head>");
  SLO->Add(L"<title>ПРОТОКОЛ ОПЕРАТОРОВ</title>");
  SLO->Add(L"<meta http-equiv=\"Content-Type\" "
   L"content=\"text/html; charset=windows-1251\"></head>");
  swprintf(Sopr,L"<body bgcolor=\"#FFFFFF\" text=\"#800000\""
   L" background=\"%s/fon2.jpg\""
   L" font face=\"Times New Roman, Times, serif\">",WORKDIR);
  SLO->Add(Sopr);
 }
 Inds=new TInds;
 for(Parm *p=Par;p;p=p->Sled){
  if(!p->Nam||!p->R){ Inds->~TInds(); goto Ex; }
  Inds->AddInd(p);
 }
/* fwprintf(OPRFILE,
  L"<h3 align=\"center\"><font color=\"#800000\">"
  L"Оператор \"РАЗМНОЖИТЬ\" строки %d обрабатывает индексы"
  L" в пределах с шагом</font></h3>\n",Line);*/
 swprintf(Sopr,
  L"<h3 align=\"center\"><font color=\"#800000\">"
  L"Оператор \"РАЗМНОЖИТЬ\" строки %d обрабатывает индексы"
  L" в пределах с шагом</font></h3>",Line);
 SLO->Add(Sopr);
// fputws(L"<table width=\"100%\" border=\"1\">\n",OPRFILE);
 SLO->Add(L"<table width=\"100%\" border=\"1\">");
 switch(KolElem(Inds->R)){
  case 1:
   SLO->Add(L"<tr><td rowspan=2><div align=\"center\">"
	L"<b>Имя</b></div></td>");
   SLO->Add(L"<td colspan=2><div align=\"center\">"
	L"<b>Границы</b></div></td>");
   SLO->Add(L"<td rowspan=2><div align=\"center\"><b>Шаг</b>"
	L"</div></td></tr>");
   SLO->Add(L"<tr><td><div align=\"center\"><b>нижняя</b>"
	L"</div></td>");
   SLO->Add(L"<td><div align=\"center\"><b>верхняя</b>"
	L"</div></td></tr>");
   swprintf(Sopr,L"%s</tr>",Inds->R->SI); SLO->Add(Sopr);
   ind=Inds->R;
   swprintf(Sopr,L"<td><div align=\"right\"><b>%s</b>"
	L"</div></td>",ind->SI); SLO->Add(Sopr);
   swprintf(Sopr,L"<td><div align=\"right\"><b>%d</b>"
	L"</div></td>",ind->GS[0]); SLO->Add(Sopr);
   swprintf(Sopr,L"<td><div align=\"right\"><b>%d</b>"
	L"</div></td>",ind->GS[1]); SLO->Add(Sopr);
   swprintf(Sopr,L"<td><div align=\"right\"><b>%d</b>"
	L"</div></td>",ind->GS[2]); SLO->Add(Sopr);
   break;
  case 2:
   SLO->Add(L"<tr>");
   for(i=0;i<2;i++){
//	fputws(L"<td rowspan=2><div align=\"center\"><b>Имя</b></div></td>\n"
//	 L"<td colspan=2><div align=\"center\"><b>Границы</b></div></td>\n"
//	 L"<td rowspan=2><div align=\"center\"><b>Шаг</b></div></td>\n",
//	 OPRFILE);
	SLO->Add(L"<td rowspan=2><div align=\"center\">"
	 L"<b>Имя</b></div></td>");
	SLO->Add(L"<td colspan=2><div align=\"center\">"
	 L"<b>Границы</b></div></td>");
	SLO->Add(L"<td rowspan=2><div align=\"center\"><b>Шаг</b>"
	 L"</div></td></tr>");
   }
   SLO->Add(L"</tr>"); SLO->Add(L"<tr>");
   for(i=0;i<2;i++){
	SLO->Add(L"<td><div align=\"center\"><b>нижняя</b>"
	 L"</div></td>");
	SLO->Add(L"<td><div align=\"center\"><b>верхняя</b>"
	 L"</div></td></tr>");
   }
   SLO->Add(L"</tr>"); SLO->Add(L"<tr>");
   for(i=0,ind=Inds->R;i<2;i++,ind=ind->Sled){
//	fwprintf(OPRFILE,
//   L"<td><div align=\"right\"><b>%s</b></div></td>\n"
//	 L"<td><div align=\"right\"><b>%d</b></div></td>\n"
//	 L"<td><div align=\"right\"><b>%d</b></div></td>\n"
//	 L"<td><div align=\"right\"><b>%d</b></div></td>\n",
//	 ind->SI,ind->GS[0],ind->GS[1],ind->GS[2]);
	swprintf(Sopr,L"<td><div align=\"right\"><b>%s</b>"
	 L"</div></td>",ind->SI); SLO->Add(Sopr);
	swprintf(Sopr,L"<td><div align=\"right\"><b>%d</b>"
	 L"</div></td>",ind->GS[0]); SLO->Add(Sopr);
	swprintf(Sopr,L"<td><div align=\"right\"><b>%d</b>"
	 L"</div></td>",ind->GS[1]); SLO->Add(Sopr);
	swprintf(Sopr,L"<td><div align=\"right\"><b>%d</b>"
	 L"</div></td>",ind->GS[2]); SLO->Add(Sopr);
   }
   break;
  default:
//   fputws(L"<tr>\n",OPRFILE);
   SLO->Add(L"<tr>");
   for(i=0;i<3;i++){
//	fputws(L"<td rowspan=2><div align=\"center\"><b>Имя</b></div></td>\n"
//	 L"<td colspan=2><div align=\"center\"><b>Границы</b></div></td>\n"
//	 L"<td rowspan=2><div align=\"center\"><b>Шаг</b></div></td>\n",
//	 OPRFILE);
	SLO->Add(L"<td rowspan=2><div align=\"center\">"
	 L"<b>Имя</b></div></td>");
	SLO->Add(L"<td colspan=2><div align=\"center\">"
	 L"<b>Границы</b></div></td>");
	SLO->Add(L"<td rowspan=2><div align=\"center\"><b>Шаг</b>"
	 L"</div></td></tr>");
   }
//   fputws(L"</tr>\n",OPRFILE);
//   fputws(L"<tr>\n",OPRFILE);
   SLO->Add(L"</tr>"); SLO->Add(L"<tr>");
   for(i=0;i<3;i++){
//	fputws(L"<td><div align=\"center\"><b>нижняя</b></div></td>\n"
//	 L"<td><div align=\"center\"><b>верхняя</b></div></td>\n",
//	 OPRFILE);
	SLO->Add(L"<td><div align=\"center\"><b>нижняя</b>"
	 L"</div></td>");
	SLO->Add(L"<td><div align=\"center\"><b>верхняя</b>"
	 L"</div></td></tr>");
   }
//   fputws(L"</tr>\n",OPRFILE);
//   fputws(L"<tr>\n",OPRFILE);
   SLO->Add(L"</tr>"); SLO->Add(L"<tr>");
   for(i=0,ind=Inds->R;i<KolElem(Inds->R);i++,ind=ind->Sled){
	if(i&&!(i%3))
//	 fputws(L"</tr><tr>\n",OPRFILE);
	 SLO->Add(L"</tr><tr>");
//	fwprintf(OPRFILE,L"<td><div align=\"right\"><b>%s</b></div></td>\n"
//	 L"<td><div align=\"right\"><b>%d</b></div></td>\n"
//	 L"<td><div align=\"right\"><b>%d</b></div></td>\n"
//	 L"<td><div align=\"right\"><b>%d</b></div></td>\n",
//	 ind->SI,ind->GS[0],ind->GS[1],ind->GS[2]);
	swprintf(Sopr,L"<td><div align=\"right\"><b>%s</b>"
	 L"</div></td>",ind->SI); SLO->Add(Sopr);
	swprintf(Sopr,L"<td><div align=\"right\"><b>%d</b>"
	 L"</div></td>",ind->GS[0]); SLO->Add(Sopr);
	swprintf(Sopr,L"<td><div align=\"right\"><b>%d</b>"
	 L"</div></td>",ind->GS[1]); SLO->Add(Sopr);
	swprintf(Sopr,L"<td><div align=\"right\"><b>%d</b>"
	 L"</div></td>",ind->GS[2]); SLO->Add(Sopr);
   }
   break;
 }
// fputws(L"</tr>\n</tabl>\n",OPRFILE);
// fputws(L"</body>\n</html>",OPRFILE);
 SLO->Add(L"</tr>");   SLO->Add(L"</tabl>");
 SLO->Add(L"</body>"); SLO->Add(L"</tabl>");
 IndExp=new TInds;
//Соберем индексированные выражения из стержней
 for(Stringer *T=L->T;T;T=T->Sled)
  if((s=wcschr(T->Name,SimbInd))&&(s[1]=='('))
   IndExp->AddInd(s);
//Соберем индексированные выражения из твердых тел
 for(Body *B=L->B;B;B=B->Sled){
  if((s=wcschr(B->Name,SimbInd))&&(s[1]=='('))
   IndExp->AddInd(s);
  if(B->BaseName&&(s=wcschr(B->BaseName,SimbInd))&&(s[1]=='('))
   IndExp->AddInd(s);
 }
//Соберем индексированные выражения из переменных
 for(Vary *V=L->V;V;V=V->Sled)
  if((s=wcschr(V->Name,SimbInd))&&(s[1]=='('))
   IndExp->AddInd(s);
//Соберем индексированные выражения из элементов
 for(Elem *E=L->E;E;E=E->Sled){
  if((s=wcschr(E->Name,SimbInd))&&(s[1]=='('))
   IndExp->AddInd(s);
 }
//Соберем индексированные выражения из операторов
 for(Root *O=L->Oper->Sled;O;O=O->Sled){
  wcscpy(NameOper,O->Name); LowToUpp(NameOper);
  if(IsSimbInd(O))
   TakeMagazine(&Mo,NameOper);
 }
 for(m=Mo;m;m=m->Sled){
  Root *O=FindOperLast(m->S);
  for(Parm *P=O->First;P;P=P->Sled){
   if(P->Nam&&(s=wcschr(P->Nam,SimbInd))&&s[1]) IndExp->AddInd(s);
   for(Parm *p=P->R;p;p=p->Sled){
	if(p->Nam&&(s=wcschr(p->Nam,SimbInd))&&s[1]) IndExp->AddInd(s);
	for(Parm *pp=p->R;pp;pp=pp->Sled)
	 if(pp->Nam&&(s=wcschr(pp->Nam,SimbInd))&&s[1]) IndExp->AddInd(s);
 }}}
//Сделаем индексированные операторы
 if(Mo){
  for(Root *O=L->Oper->Sled;O;O=O->Sled){
   if(*O->Name=='@') continue;
   wcscpy(Inf,O->Name); LowToUpp(Inf);
   if(FindMagazine(Mo,Inf,&m)){
	IndexInstr(O,SYNHRON);
	O=O->Sled;
 }}}
//Соберем индексированные выражения из инструкций
 for(I=L->Inst;I;I=I->Sled){
  wcscpy(NameInstr,I->Name); LowToUpp(NameInstr);
  if(IsSimbInd(I))
   TakeMagazine(&Mi,NameInstr);
 }
 for(m=Mi;m;m=m->Sled){
  I=FindInstLast(m->S);
  for(Parm *P=I->First;P;P=P->Sled){
   if(P->Nam&&(s=wcschr(P->Nam,SimbInd))&&s[1])
	IndExp->AddInd(s);
   for(Parm *p=P->R;p;p=p->Sled)
	if(p->Nam&&(s=wcschr(p->Nam,SimbInd))&&s[1])
	 IndExp->AddInd(s);
 }}
//Сделаем индексированные инструкции
 for(I=L->Inst;I;I=I->Sled){
  wcscpy(Inf,I->Name); LowToUpp(Inf);
  if(FindMagazine(Mi,Inf,&m)){
   bool Mode=!wcscmp(Inf,L"СРАВНИТЬ")||!wcscmp(Inf,L"СОВМЕСТИТЬ")?INTERJER:SYNHRON;
   IndexInstr(I,Mode);
   I=I->Sled;
 }}
//Вынем индексированные массивы
 if(a=L->A){ Arra *Sled;
  while(Sled=a->Sled){
   if(s=wcschr(Sled->Name,SimbInd)){ int Kod;
	if(KodGrek(s+1,Kod)) a=a->Sled;
	else{ wchar_t c=s[2]; s[2]='\0';
	 if(Inds->FindInd(s+1)){
	  a->Sled=Sled->Sled;
	  *Pa=Sled; Sled->Sled=NULL; Pa=(Arra **)((wchar_t *)Sled);
	 }
	 else a=a->Sled;
	 s[2]=c;
   }}
   else a=a->Sled;
 }}
//Вынем индексированные переменные
 if(V=L->V){ Vary *Sled;
  while(Sled=V->Sled){
   if(s=wcschr(Sled->Name,SimbInd)){ int Kod;
	if(KodGrek(s+1,Kod)) V=V->Sled;
	else{ wchar_t c=s[2]; s[2]='\0';
	 if(Inds->FindInd(s+1)){
	  V->Sled=Sled->Sled;
	  *Pv=Sled; Sled->Sled=NULL; Pv=(Vary **)((wchar_t *)Sled/*+SzL*/);
	 }
	 else V=V->Sled;
	 s[2]=c;
   }}
   else V=V->Sled;
 }}
//Вынем индексированные элементы
 while((e=L->E)&&(s=wcschr(e->Name,SimbInd))&&(bb=s[2],s[2]='\0',Inds->FindInd(s+1))){
  L->E=e->Sled; s[2]=bb;
  *Pe=e; e->Sled=NULL; Pe=(Elem **)e;
 }
 if(e=L->E){ Elem *Sled;
  while(Sled=e->Sled){
   if((s=wcschr(Sled->Name,SimbInd))&&(bb=s[2],s[2]='\0',Inds->FindInd(s+1))){
	e->Sled=Sled->Sled; s[2]=bb;
	*Pe=Sled; Sled->Sled=NULL; Pe=(Elem **)Sled;
   }
   else e=e->Sled;
 }}
//Вынем индексированные твёрдые тела
 while((b=L->B)&&(s=wcschr(b->Name,SimbInd))&&(bb=s[2],s[2]='\0',Inds->FindInd(s+1))){
  L->B=b->Sled; *Pb=b; b->Sled=NULL; Pb=(Body **)b; s[2]=bb;
 }
 if(b=L->B){ Body *Sled;
  while(Sled=b->Sled){
   if((s=wcschr(Sled->Name,SimbInd))&&(bb=s[2],s[2]='\0',Inds->FindInd(s+1))){
	b->Sled=Sled->Sled; s[2]=bb;
	*Pb=Sled; Sled->Sled=NULL; Pb=(Body **)Sled;
   }
   else b=b->Sled;
 }}
//Вынем индексированные стержни
 while((t=L->T)&&(s=wcschr(t->Name,SimbInd))&&(bb=s[2],s[2]='\0',Inds->FindInd(s+1))){
  L->T=t->Sled; *Pt=t; t->Sled=NULL; Pt=(Stringer **)t; s[2]=bb;
 }
 if(t=L->T){ Stringer *Sled;
  while(Sled=t->Sled){
   if((s=wcschr(Sled->Name,SimbInd))&&(bb=s[2],s[2]='\0',Inds->FindInd(s+1))){
	t->Sled=Sled->Sled; s[2]=bb;
	*Pt=Sled; Sled->Sled=NULL; Pt=(Stringer **)Sled;
   }
   else t=t->Sled;
 }}
 if(W||E||B||T||A){
//  fwprintf(OPRFILE,
//   L"<h3 align=\"center\"><font color=\"#800000\">"
//   L"Оператор \"РАЗМНОЖИТЬ\" строки %d сгенерировал "
//   L"из индексированных выражений</font></h3>\n",Line);
  swprintf(Sopr,
   L"<h3 align=\"center\"><font color=\"#800000\">"
   L"Оператор \"РАЗМНОЖИТЬ\" строки %d сгенерировал "
   L"из индексированных выражений</font></h3>",Line);
  SLO->Add(Sopr);
  if(W){
//   fwprintf(OPRFILE,L"<b>Переменные:</b><br>\n");
//   fputws(L"<table width=\"100%\" border=\"1\">\n",OPRFILE);
//   fputws(L"<tr><td><div align=\"center\">"
//	L"<b>Исходные выражения</b></div></td>\n");
//   fputws(L"<td><div align=\"center\"><b>Новые переменные</b>"
//	L"</div></td>\n</tr>\n",OPRFILE);
   SLO->Add(L"<b>Переменные:</b><br>");
   SLO->Add(L"<table width=\"100%\" border=\"1\">");
   SLO->Add(L"<tr><td><div align=\"center\">"
	L"<b>Исходные выражения</b></div></td>");
   SLO->Add(L"<td><div align=\"center\">"
	L"<b>Новые переменные</b></div></td>");
   sopr+=swprintf(sopr,L"</tr>");
   for(V=W;V;V=V->Sled){
	if(V->Znach.C!=Pust){ Vary *vv;
	 ListSBVEA.AddProt(V);
	 s=wcschr(V->Name,SimbInd);
	 if(s[1]=='('){
	  if(!IndExp->IndexForm(V,ListSBVEA)){
//	   fclose(OPRFILE);
	   SLO->Add(Sopr); SLO->SaveToFile(OprFile);
	   goto Ex3;
	 }}
	 else{
	  if(!Inds->Index(V,ListSBVEA)){
	   SLO->Add(Sopr); SLO->SaveToFile(OprFile);
//	   fclose(OPRFILE);
	   goto Ex3;
   }}}}
   ListSBVEA.VtoFile();
//   fputws(L"</table>\n",OPRFILE);
   sopr+=swprintf(sopr,L"</table>");
  }
  if(E){
//   fwprintf(OPRFILE,L"<b>Элементы:</b><br>\n");
//   fputws(L"<table width=\"100%\" border=\"1\">\n",OPRFILE);
//   fputws(L"<tr>",OPRFILE);
//   fputws(L"<td><div align=\"center\"><b>Исходные выражения</b></div></td>\n"
//	L"<td><div align=\"center\"><b>Новые элементы</b></div></td>\n</tr>",OPRFILE);
   SLO->Add(L"<b>Элементы:</b><br>");
   SLO->Add(L"<table width=\"100%\" border=\"1\">");
   SLO->Add(L"<tr><td><div align=\"center\">"
	L"<b>Исходные выражения</b></div></td>");
   SLO->Add(L"<td><div align=\"center\">"
	L"<b>Новые элементы</b></div></td>");
   sopr+=swprintf(sopr,L"</tr>");
   for(e=E;e;e=e->Sled){
	ListSBVEA.AddProt(e);
	s=wcschr(E->Name,SimbInd);
	if(s[1]=='('){
	 if(!IndExp->IndexForm(e,ListSBVEA)){
	  SLO->Add(Sopr); SLO->SaveToFile(OprFile);
//	  fclose(OPRFILE);
	  goto Ex3;
	}}
	else{
	 if(!Inds->Index(e,ListSBVEA)){
//	  fclose(OPRFILE);
	  SLO->Add(Sopr); SLO->SaveToFile(OprFile);
	  goto Ex3;
   }}}
   ListSBVEA.EtoFile();
//   fputws(L"</table\n",OPRFILE);
   sopr+=swprintf(sopr,L"</table>");
  }
  if(B){
//   fwprintf(OPRFILE,L"<b>Твердые тела:</b><br>\n");
//   fputws(L"<table width=\"100%\" border=\"1\">\n",OPRFILE);
//   fputws(L"<tr>",OPRFILE);
//   fputws(L"<td><div align=\"center\"><b>Исходные выражения</b></div></td>\n"
//	L"<td><div align=\"center\"><b>Новые твердые тела</b></div></td>\n</tr>",OPRFILE);
   SLO->Add(L"<b>Твердые тела:</b><br>");
   SLO->Add(L"<table width=\"100%\" border=\"1\">");
   SLO->Add(L"<tr><td><div align=\"center\">"
	L"<b>Исходные выражения</b></div></td>");
   SLO->Add(L"<td><div align=\"center\">"
	L"<b>Новые твердые тела</b></div></td>");
   sopr+=swprintf(sopr,L"</tr>");
   for(b=B;b;b=b->Sled){
	ListSBVEA.AddProt(b);
	if(!Inds->Index(b,ListSBVEA)){
//	 fclose(OPRFILE);
	 SLO->Add(Sopr); SLO->SaveToFile(OprFile);
	 goto Ex3;
   }}
   ListSBVEA.BtoFile();
//   fputws(L"</table\n",OPRFILE);
   sopr+=swprintf(sopr,L"</table>");
  }
  if(T){
//   fwprintf(OPRFILE,L"<b>Стержни:</b><br>\n");
//   fputws(L"<table width=\"100%\" border=\"1\">\n",OPRFILE);
//   fputws(L"<tr>",OPRFILE);
//   fputws(L"<td><div align=\"center\"><b>Исходные выражения</b></div></td>\n"
//	L"<td><div align=\"center\"><b>Новые стержни</b></div></td>\n</tr>",OPRFILE);
   SLO->Add(L"<b>Стержни:</b><br>");
   SLO->Add(L"<table width=\"100%\" border=\"1\">");
   SLO->Add(L"<tr><td><div align=\"center\">"
	L"<b>Исходные выражения</b></div></td>");
   SLO->Add(L"<td><div align=\"center\">"
	L"<b>Новые стержни</b></div></td>");
   sopr+=swprintf(sopr,L"</tr>");
   for(t=T;t;t=t->Sled){
	ListSBVEA.AddProt(t);
	if(!Inds->Index(t,ListSBVEA)){
//	 fclose(OPRFILE);
	 SLO->Add(Sopr); SLO->SaveToFile(OprFile);
	 goto Ex3;
   }}
   ListSBVEA.TtoFile();
//   fputws(L"</table\n",OPRFILE);
   sopr+=swprintf(sopr,L"</table>");
  }
  if(A){
//   fwprintf(OPRFILE,L"<b>Массивы:</b><br>\n");
//   fputws(L"<table width=\"100%\" border=\"1\">\n",OPRFILE);
//   fputws(L"<tr>",OPRFILE);
//   fputws(L"<td><div align=\"center\"><b>Исходные выражения</b></div></td>\n"
//	L"<td><div align=\"center\"><b>Новые массивы</b></div></td>\n</tr>\n",OPRFILE);
   SLO->Add(L"<b>Массивы:</b><br>");
   SLO->Add(L"<table width=\"100%\" border=\"1\">");
   SLO->Add(L"<tr><td><div align=\"center\">"
	L"<b>Исходные выражения</b></div></td>");
   SLO->Add(L"<td><div align=\"center\">"
	L"<b>Новые масивы</b></div></td>");
   sopr+=swprintf(sopr,L"</tr>");
   for(a=A;a;a=a->Sled){
	if(a->A||a->F){
	 ListSBVEA.AddProt(a);
	 s=wcschr(a->Name,SimbInd);
	 if(s[1]=='('){
	  if(!IndExp->IndexForm(a,ListSBVEA)){
//	   fclose(OPRFILE);
	   SLO->Add(Sopr); SLO->SaveToFile(OprFile);
	   goto Ex3;
	 }}
	 else if(!Inds->Index(a,ListSBVEA)){
//	  fclose(OPRFILE);
	  SLO->Add(Sopr); SLO->SaveToFile(OprFile);
	  goto Ex3;
   }}}
   ListSBVEA.AtoFile();
//   fputws(L"</table>\n",OPRFILE);
   sopr+=swprintf(sopr,L"</table>");
  }
//  fputws(L"\n<br><hr></font></body>\n</html>\n",OPRFILE);
  SLO->Add(Sopr);
  SLO->Add(L"<br><hr></font></body>");
  SLO->Add(L"</html>");
 }
// fclose(OPRFILE);
 SLO->SaveToFile(OprFile);
 delete Inds; delete IndExp;
 if(Mmes2){ wchar_t *inf;
  if(Mmes2->Sled){
   inf+=swprintf(inf=Inf,L"Следующие индексы не указаны в операторе \"РАЗМНОЖИТЬ\":\n\'%s\'",
	Mmes2->S);
   for(Magazine *m=Mmes2->Sled;m;m=m->Sled)
	inf+=swprintf(inf,L", \'%s\'",m->S);
  }
  else
   inf+=swprintf(inf=Inf,L"Индекс %s не указан в операторе \"РАЗМНОЖИТЬ\"",Mmes2->S);
  Application->MessageBox(Inf,L"Внимание!",MB_OK);
 }
 return true;
Ex:swprintf(inf,
 L"Оператор \"РАЗМНОЖИТЬ\" должен быть записан в виде:<br>\n"
 L"РАЗМНОЖИТЬ : <индекс-символ1>(<ОТ1>,<ДО1>[,<ШАГ1>]),<br>\n"
 L"             <индекс-символ2>(<ОТ2>,<ДО2>[,<ШАГ2>]),...;,<br>\n"
 L"например,    РАЗМНОЖИТЬ : i(1,5),k(2,5,3);<br>\n");
 if(F=_wfopen(OprFile,L"a")){ Fputs(Inf,F); fclose(F); }
 Application->MessageBox(Inf,L"Ошибка в операторе \*РАЗМНОЖИТЬ\*:",MB_OK);
 return false;
Ex3: delete Inds; delete IndExp;
 return false;
}
