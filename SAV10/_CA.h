#ifndef __CA_H
#define __CA_H

#pragma hdrstop

#include <vcl.h>
#include <comctrls.hpp>
#include <cgauges.h>
#include <stdio.h>
#include "ConvDW.h"
#include "TablW.h"

#define PUST      0
#define Double    1
#define Long      2
#define CNST      3
#define VARY      4
#define UNOP      5
#define BNOP      6
#define IFOP      7
#define IPLL      8
#define SUMM      9
#define ROUT     10
#define INST     11
#define OPER     12
#define NAME     13
#define ARR      14
#define ELEM     15
#define SPLN     16
#define BODY     17
#define STRINGER 18
#define INTG     19
#define RUUT     20
#define REST     21
#define DEFINE   1
#define NODEF    0
#define BLANK   ' '

		 /*  Объявления структур:  */
struct Cnst; struct Vary; struct Unop; struct Bnop; struct Ifop; struct Summ;
struct Intg; struct Ipll; struct Spln; struct Body; struct Rest; struct Solu;

union  Form{ //формула
 Cnst *C; Vary *V; Unop *U; Bnop *B; Ifop *L; Summ *S; Intg *G; Ipll *I;
 Spln *P; Solu *R; Rest *O;
};
struct Cnst{//постоянная
 int Atr; Cnst *Sled; wchar_t *Name; double Val;
};
struct Vary{//переменная
 int Atr; Vary *Sled; wchar_t *Name; Form Znach; double Val; int P,Q,O,Line;
};
struct Unop{//унарная операция
 int Atr; Unop *Sled; int Func; Form Arg; double Val; int P,O;
};
struct Bnop{//бинарная операция
 int Atr; int Oper; Form L,R;
};
struct Ifop{//условное предложение
 int Atr; int Oper; Form U,V,FT,FF;
};
struct Summ{//сумма
 int Atr; Summ *Sled; int Type; Vary *i; Form a,b,F;
};
struct Intg{//интеграл
 int Atr; //=INTG=19
 Intg *Sled;
 int Type,//=1, определенный, =0, неопределенный интеграл
  P;      //>=0, если итеграл посчитан
 Vary *x; //переменная интегрирования
 Form a,b,//пределы интегрировапния, f,F
  f,F,    //подынтегральная функция и первообразная
  Rez;    //аналитический результат интегрирования с учетом подстановки пределов
 double Val,B; //численный результат интеграла для верхнего предела = B
};
struct Solu{//корень функции одной переменной
 int Atr;//=RUUT=20
 Solu *Sled;
 Vary *x; //переменная-аргумент
 Form f,//функция, корень которой надо найти
  a,b;//участок для поиска корня
 double Val; //численный результат определения корня
};
struct Rest{//результат вычисления остатка от деления
 int Atr;//=REST=21
 Rest *Sled;
 Form a,b;//числитель и знаменатель
 double Val; //численный результат вычисления остатка от деления
};
struct Arra{//массив
 Arra *Sled; int N; wchar_t *Name; double *A; Form *F; int Line;
};
struct Ipll{//линейная интерполяция
 int Atr; Ipll *Sled; int Type; Arra *A; Form F;
};
struct Spln{//кубический сплайн
 int Atr; Spln *Sled; int Type; Arra *A; Form F; double *a,*b,*c,*d;
 void CalcSpln(void);
};
struct Term{//элемент многочлена
 Term *Sled; Form F; int Powr;
};
struct Elem{//элемент
 Elem *Sled; wchar_t *Name; Vary *K; Form Znach; int Line;
};
struct Cord{//координата
 Cord *Sled; Vary *V;
};
struct Parm{//параметр  инструкции
 Parm *Sled,*R; wchar_t *Nam; double Val; int Line;
};
struct Root{//инструкция или оператор
 Root *Sled; wchar_t *Name; Parm *First; int Line;
};

struct Body{//твердое тело
 Body *Sled; wchar_t *Name,*BaseName; Parm *KoordSys,*Inert; int Line;
 Form rCk[4],vCk[4],aCk[4],wkk[4],Tk0[4][4],Kx,Ky,Kz,Jxy,Jyz,Jxz; bool Accum;
};

struct Stringer{//стержень
 Stringer *Sled; wchar_t *Name; Parm *Parms; int Line;
 Form FE,FG,FL,FF,FJx,FJy,FJz,Cix,Ciy,Ck,Cx,Cy,Cz,dA,dB,dG,dX,dY,dZ,
  X1,Y1,Z1,X2,Y2,Z2;
 bool Fill(wchar_t *Name);
 bool Elems();
 bool TakeRGCoords(Parm *P,wchar_t *Name);
 bool TakeAnglsCoords(Parm *P);
 void TakeDekartCoords(Parm *P);
};

struct List{//лист
 List *Sled; wchar_t *Name; Vary *V,*Time,*Freq; Unop *U; Elem *E; Ipll *L;
 Body *B; Stringer *T; Arra *A; Summ *M; Intg *I; Solu *C; Rest *O;
//обоб. координаты, неизвестные,
//координаты с зависимыми вариациями (скоростями), псевдоскорости
 Cord *K,*P,*Z,*W,*R,*N,*Dq,*Dv;
 Root *Inst,*Oper; Term *Sklad; Spln *S;
};

	  // Операции с формулами:
//унарные:
 Form operator  - (Form F);
//бинарные:
 bool operator == (Form A, Form  B);  bool operator != (Form A, Form  B);
 bool operator == (Form F, Cnst *C);  bool operator != (Form F, Cnst *C);
 bool operator == (Form F, Vary *V);  Form operator += (Form A, Form  B);
 Form operator  + (Form A, Form   B); Form operator  - (Form A, Form   B);
 Form operator  * (Form A, Form   B); Form operator  / (Form A, Form   B);
 Form operator  ^ (Form A, Form   B); Form operator  + (Form A, double D);
 Form operator  - (Form A, double D); Form operator  * (Form A, double D);
 Form operator  / (Form A, double D); Form operator  ^ (Form A, double D);
 Form operator  + (double D, Form A); Form operator  - (double D, Form A);
 Form operator  * (double D, Form A); Form operator  / (double D, Form A);
 Form operator  ^ (double D, Form A);
		/*  Объявления функций:  */
 bool IsPust (Form F); //Формула - пуста?
 bool IsMins (Form F); //Формула - унарный минус?
 bool IsCnst (Form F); //Формула - константа?
 bool IsVary (Form F); //Формула - переменная?
 bool IsSumm (Form F); //Формула - сумма?
 bool IsDiff (Form F); //Формула - разность?
 bool IsMult (Form F); //Формула - произведение?
 bool IsDivd (Form F); //Формула - дробь?
 bool IsPowr (Form F); //Формула - степенная функция?
 bool IsSin  (Form F); //Формула - синус?
 bool IsCos  (Form F); //Формула - косинус?
 bool VassalOfKoord(List *L,Form F);//формула зависит от обобщенных координат?
 bool VassalOfKoord(List *L,Elem *E);//координата элемента зависит от обобщенных координат?
 bool VassalOfKoord(List *L,Vary *V);//переменная зависит от обобщенных координат
 bool VassalOfPsevdo(List *L,Form F);//формула зависит от квазискоростей?
 bool VassalOfDepedent(List *L,Form F);//формула зависит от координат с зависимыми скоростями?
 bool VassalOfVelosity(List *L,Form F);//формула зависит от обобщенных скоростей?
 bool VassalOfAccelerat(List *L,Form F);//формула зависит от обощенных ускорений?
 bool OwnRelation(Vary *V);//зависит ли переменная V сама от себя
 Form SubLimits(Intg *I);//подставим пределы интегрирования
 bool IsTaken(Intg *I);
//берется ли аналитически интеграл, если да, то в поле Eqv структуры Intg будет
//записана формула интегрирования, а в поле Rez - формула с учетом подстановки
//пределов интегрирования в аналитическом виде
 bool FindCnst  (wchar_t *S,Cnst **C);  bool FindCnst(double D, Cnst **C);
 bool FindVary  (wchar_t *S,Vary **V);  bool FindVary(String S,Vary **V);
 bool FindElem  (wchar_t *S, Elem **E); bool FindCloseElem(wchar_t *S,Elem **E);
 bool FindArra  (wchar_t *S,Arra **A);  bool FindList(wchar_t *S, List **L);
 bool FindInst  (wchar_t *S,Root **I);  bool FindOper(wchar_t *S, Root **O);
 bool FindCord  (wchar_t *S,Cord **K);  bool FindZavC(wchar_t *S, Cord **K);
 bool FindPsevdo(wchar_t *S,Cord **K);  bool FindUnkn(wchar_t *S);
 bool FindBody (wchar_t *S,Body **B);
 Root *FindInstLast(wchar_t *S);        Root *FindOperLast(wchar_t *S);
 bool FindParm (Parm *R,wchar_t *S,Parm **P);
 bool FindParmUpp (Parm *R,wchar_t *S,Parm **P);
 bool FindParmUppStr (Parm *R,wchar_t *S,Parm **P);
 bool FindSpln(int Typ,Arra *A,Spln **P);
 bool FindSpln(Spln *R,wchar_t *S,Spln **P);
/* все указанные выше процедуры осуществляют поиск соот-    */
/* ветствующих объектов с именем S в списке текущего листа. */
/* Вырабатывают 1 если такой объект имеется и 0 - если нет. */
/* В первом случае второй формальный параметр содержит ад-  */
/* рес такого объекта, во втором  случае - NULL, если спи-  */
/* сок пуст, или адрес последнего объекта списка - к нему   */
/* можно прицепить новый объект.                            */
 bool IsVaried(List *L,Vary *V);//Варьируется ли переменная V
 Cnst *TakeCnst (wchar_t *S);     Cnst *TakeCnst (double D);
 Vary *TakeVary (wchar_t *S);     Elem *TakeElem (wchar_t *S);
 Arra *TakeArra (wchar_t *S);     Root *TakeInst (wchar_t *S);
 Root *TakeOper (wchar_t *S);     List *TakeList (wchar_t *S);
 Cord *TakeCord (Vary *V);     Cord *FindCord(Cord *K,wchar_t *S);
 Vary *TakeVary (String S);    Body *TakeBody (wchar_t *S,wchar_t *s);
 Stringer *TakeStringer (wchar_t *S);
/* все указанные выше процедуры осуществляют поиск соот-    */
/* ветствующих объектов с именем S в списке текущего листа. */
/* Если такого объекта не оказывается, то генерируется но-  */
/* вый и цепляется в конец списка. Выдается адрес найден-   */
/* ного или созданного объекта                              */
/* все указанные выше процедуры осуществляют поиск соот-    */
/* ветствующих объектов с именем S в списке текущего листа. */
/* Если такого объекта не оказывается, то генерируется но-  */
/* вый и цепляется в конец списка. Выдается адрес найден-   */
/* ного или созданного объекта                              */

int Coord(List *L);
/* процедура составления списков обобщенных координат       */
int CoordWithoutDT(List *L);
/* процедура составления списков обобщенных координат
   без генерации формул для скоростей и ускорений       */
void TakeDifTime();
/* процедура генерации формул для скоростей и ускорений       */

int PrintForm(bool DOS,FILE *OUTFILE,wchar_t *Name, Form F);
// процедура преобразовывает дерево формулы F в строку S;
// печатает ее в виде <Name> = <формула>;, если Name&&*Name!=0,
// если Name!=NULL и *Name='\0', то в виде \n<формула>,
// иначе в виде <формула>.                  */

Form TakeApForm (Form F); void TakeApOper(Root *O);
void TakeApParm(Parm *p);
/* указанные процедуры осуществляют разборку формулы (осво- */
/* бождая память, отведенную под унарные и бинарные опера-  */
/* ции), оператора, списка параметров инструкции соответст- */
/* венно.                                                   */

void Take_t_w (void);
/* процедура генерирует переменную 'время' (t) и переменную */
/* 'частота' (w) для текущего листа L.                      */

   Unop *TakeUnop ( int Oper, Form F );
   Bnop *TakeBnop ( int Oper, Form L, Form R );
   Ifop *TakeIfop ( int Oper, Form U, Form V, Form FT, Form FF );
   Ipll *TakeIpll ( int Type, Arra *A, Form F );
   Spln *TakeSpln ( int Type, Arra *A, Form F );
   void  TakeSplns( int Type, Arra *A );
   Spln *TakeSpln ( int Type, Arra *A, double *a,
                    double *b, double *c, double *d, Form F);
   Spln *TakeSpln ( Spln **R, wchar_t *S, Arra *A );
   Summ *TakeSumm(Form F,Vary *I,Form A,Form B);
   Intg *FindIntg(Form f,Vary *x,Form a,Form b,int Type);
   Intg *TakeIntg(Form F,Vary *I,Form A,Form B);
   Intg *TakeIntg(Form F,Vary *I);
   Solu *FindSolu(Form f,Vary *x,Form a,Form b);
   Solu *TakeSolu(Form F,Vary *I,Form a,Form b);
   Rest *FindRest(Form a,Form b);
   Rest *TakeRest(Form a,Form b);
/* указанные выше процедуры генерируют унарную, бинарную,   */
/* условную, линейную и сплайн интерполяцию и суммирующую   */
/* и интегрирующую операции с номером Oper (типом Type) над */
/* соответствующими формулами.                              */

   Form FormTras(Form F);
/* процедура проводит трассировку формулы F, то есть заме-  */
/* няет все переменные, не являющиеся обобщенными координа- */
/* тами (их поле Znach!=Pust), их значениями, а операции    */
/* с константами заменяет значениями этих операций.         */

   Form FormOpenDiff(Form F);
/* процедура проводит замену формулы F формулой, у которой  */
/* все переменные, имя которых содержит знак "'" заменяются */
/* их формульными значениями.                               */

   Form FormToOpen(Form F);
/* процедура проводит подстановку переменных в формуле,     */
/* если значение переменной не есть константа. Если значе-  */
/* нием переменной является константа, переменная не тро-   */
/* гается.                                                  */
   Form ObNul(Form F);
/* процедура упрощает формулу, подставляя во все промежу-
   точные формулы переменные, равные нулю.                  */
Form ObNulCord(Form F);
/* процедура упрощает формулу, подставляя вместо обобщенных
   координат нули.                                          */

void OutPut(TRichEdit *TR,int Otstup, wchar_t *WORD, wchar_t *S);
/* процедура выводит на экран информацию S, разбивая ee на   */
/* подстроки. При Otstup=0 вывод будет со следующей строки   */
/*2 без отступов. При Otstup>0 будет пропущена пустая строка, */
/* следующая строка начнется с фразы WORD, а все последую-   */
/* щие начнутся с отступа, равного длине WORD.               */

void Error(int Line,int Pos,bool Mess);
// Процедура выводит в файл ERRFILE информацию из строки Inf (глоба-
// льный параметр), предваряя ее номером строки файла исходных данных,
// номером позиции в ней, где локализована ошибка и словом *  ОШИБКА  *.
// Кроме того, глобальному параметру IsError присвоится значение 2,
// а KolErr++.

void Atten(int Line,int Pos);
// Процедура выводит в файл ERRFILE информацию из строки Inf (глоба-
// льный параметр), предваряя ее номером строки файла исходных данных,
// номером позиции в ней, где локализована ошибка и словом * ВНИМАНИЕ *.
// Кроме того, счетчик предупреждений увеличится на 1 ( KolMes++ ).

void ErrorSynt(TRichEdit *RE,wchar_t *S,int Cod=2);

   void InitCA   (void); void SbrosDif (void);
   void CodElem(wchar_t *Cod);
   bool VvodJob(TRichEdit *Ri,TRichEdit *Ro,TCGauge *CGauge,TStatusBar *StatusBar);
//   bool KolElem (List *L, wchar_t *SE);
   Form DifBy(Form F,Vary *V);//полная частная производная F по V
   Form DifByT(Vary *X);//полная производная по времени от переменной Х

   bool Valc(Form F,double &Znach);
   void ClearVar (void); double Val(Form F); double Value(Form E);
   bool VarInForm (bool Everywhere,Form F,Vary *X);
		 /*   Перегружаемые функции:   */
   Form   sqrt(Form F); Form    sin(Form F); Form    cos(Form F);
   Form     tg(Form F); Form arcsin(Form F); Form arccos(Form F);
   Form  arctg(Form F); Form    exp(Form F); Form     ln(Form F);
   Form     sh(Form F); Form     ch(Form F); Form   arsh(Form F);
   Form   abs (Form F); Form   rand(Form F); Form   ceil(Form F);
   Form  floor(Form F);
	      /*   Объявления операторов:   */
  bool Treug      (Parm *Par,TRichEdit *R);
  bool MidlElem   (Parm *Par,TRichEdit *R);
  bool LeadFors   (Parm *Par,TRichEdit *R);
  bool Inertion   (Parm *Par,TRichEdit *R);
  bool Manipulator(Parm *Par,TRichEdit *R);
  bool Structurs  (Parm *Par,TRichEdit *R);
  bool MultiPly   (Parm *Par,TRichEdit *R);
  bool ElastBar   (Parm *Par,TRichEdit *R);
  bool ToOpen     (Parm *Par,TRichEdit *R);
  bool ToClose    (Parm *Par,TRichEdit *R);
  bool Solids     (Parm *Par,TRichEdit *R);
  bool ElasticRods(Parm *Par,TRichEdit *R);
  bool Quaternion (Parm *Par,TRichEdit *R);
  bool Bodys      (Body *B);
  bool Stringers  (Stringer *B);

  int SLAU (int N,double **A,double *B,double *x);

  bool SpecSimb(wchar_t C);
  wchar_t *RichAttr(TRichEdit *R,wchar_t *S);
  void RichAdd(TRichEdit *R,wchar_t *S);
  void ReadAllRight(Parm *P,FILE *F,bool ToGrek,bool Eqv);
  void StrToHTM(wchar_t *S,FILE *HTMFILE);
  void StrToHTM(wchar_t *S);
  void TablToHTM(wchar_t *File,int CountTabl,double Tn,double Tk,
  bool Godograf,wchar_t *Format1,wchar_t *Format2,wchar_t *Format3,double *X,
   double *Y,int Podv,wchar_t *Autor);
  void ToTable(wchar_t *Name,int CountTabl,double Tn,double Tk,int Lpodv,
  wchar_t *Autor,void(*Funcs)(double,Magazine *,double *),wchar_t *File);
  void ClearOper(void);
  void PrintOper(bool DOS,FILE *OUTFILE,Form F,int *KolOp);
  bool IsCykl(Magazine *Mv,Form F);
  void TablToReport(wchar_t *Name,wchar_t *Arg,int CountTabl,double Tn,double Tk,
   bool Godograf,void (*Funcs)(double,Magazine *,double *),
   Magazine **NamesInclude,Magazine **NamesTAB,Magazine **NamesSpline,
   Magazine **NamesFurie,int Lpodv,wchar_t *Autor);
  void SplToReport(wchar_t *Name,int CountTabl,double Tn,double Tk,bool Godograf,
   void (*Funcs)(double,Magazine *,double *),Magazine **NamesInclude,
   Magazine **NamesTAB,Magazine **NamesSpline,Magazine **NamesFurie,
   int Lpodv,wchar_t *Autor,TCGauge *CGauge);
  void SplGdrfToReport(wchar_t *Name,int CountTabl,double Tn,double Tk,
    void (*Funcs)(double,Magazine *,double *),Magazine **NamesInclude,
    Magazine **NamesTAB,Magazine **NamesSpline,Magazine **NamesFurie,
    int Lpodv,wchar_t *Autor,TCGauge *CGauge);
  void FurieToReport(wchar_t *Name,int CountTabl,double Tn,double Tk,bool Godograf,
   void (*Funcs)(double,Magazine *,double *),Magazine **NamesInclude,
   Magazine **NamesTAB,Magazine **NamesSpline,Magazine **NamesFurie,
   int Lpodv,wchar_t *Autor,TCGauge *CGauge);
  AnsiString *FormToStr(Form F,AnsiString *S);
  int Grek(wchar_t *S);
  Form StrToForm(wchar_t *Sp);
  wchar_t *Compress(wchar_t *S);
  bool Calcul(Form F,double &Znach);
  bool EqvConst(Form F);
  double Ipl(Ipll *I,double X);
  double Spl(Spln *S,double X);
  int KolInst(wchar_t *Name);
  void UtilVars(Form F,Magazine **VarsInForm,bool ViewVal);
//Процедура заполнения магазина VarsInForm именами переменных, входящих
//в формулу F и в переменные и операции, входящие в эту формулу
  int VaryToIntg(int k,Form F,Cnst *Znach);
//Процедура заполнения списка интегралов из формул переменных
  int VaryToSol(int k,Form F,Cnst *Znach);
//Процедура заполнения списка корней из формул переменных
  bool __fastcall ReplaceVarsToConst(TStatusBar *StatusBar,TCGauge *CGauge,
   bool Second);
//Процедура замены формул постоянных переменных костантами
//---------------------------------------------------------------------------

#endif

