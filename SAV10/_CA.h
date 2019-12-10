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

		 /*  ���������� ��������:  */
struct Cnst; struct Vary; struct Unop; struct Bnop; struct Ifop; struct Summ;
struct Intg; struct Ipll; struct Spln; struct Body; struct Rest; struct Solu;

union  Form{ //�������
 Cnst *C; Vary *V; Unop *U; Bnop *B; Ifop *L; Summ *S; Intg *G; Ipll *I;
 Spln *P; Solu *R; Rest *O;
};
struct Cnst{//����������
 int Atr; Cnst *Sled; wchar_t *Name; double Val;
};
struct Vary{//����������
 int Atr; Vary *Sled; wchar_t *Name; Form Znach; double Val; int P,Q,O,Line;
};
struct Unop{//������� ��������
 int Atr; Unop *Sled; int Func; Form Arg; double Val; int P,O;
};
struct Bnop{//�������� ��������
 int Atr; int Oper; Form L,R;
};
struct Ifop{//�������� �����������
 int Atr; int Oper; Form U,V,FT,FF;
};
struct Summ{//�����
 int Atr; Summ *Sled; int Type; Vary *i; Form a,b,F;
};
struct Intg{//��������
 int Atr; //=INTG=19
 Intg *Sled;
 int Type,//=1, ������������, =0, �������������� ��������
  P;      //>=0, ���� ������� ��������
 Vary *x; //���������� ��������������
 Form a,b,//������� ���������������, f,F
  f,F,    //��������������� ������� � �������������
  Rez;    //������������� ��������� �������������� � ������ ����������� ��������
 double Val,B; //��������� ��������� ��������� ��� �������� ������� = B
};
struct Solu{//������ ������� ����� ����������
 int Atr;//=RUUT=20
 Solu *Sled;
 Vary *x; //����������-��������
 Form f,//�������, ������ ������� ���� �����
  a,b;//������� ��� ������ �����
 double Val; //��������� ��������� ����������� �����
};
struct Rest{//��������� ���������� ������� �� �������
 int Atr;//=REST=21
 Rest *Sled;
 Form a,b;//��������� � �����������
 double Val; //��������� ��������� ���������� ������� �� �������
};
struct Arra{//������
 Arra *Sled; int N; wchar_t *Name; double *A; Form *F; int Line;
};
struct Ipll{//�������� ������������
 int Atr; Ipll *Sled; int Type; Arra *A; Form F;
};
struct Spln{//���������� ������
 int Atr; Spln *Sled; int Type; Arra *A; Form F; double *a,*b,*c,*d;
 void CalcSpln(void);
};
struct Term{//������� ����������
 Term *Sled; Form F; int Powr;
};
struct Elem{//�������
 Elem *Sled; wchar_t *Name; Vary *K; Form Znach; int Line;
};
struct Cord{//����������
 Cord *Sled; Vary *V;
};
struct Parm{//��������  ����������
 Parm *Sled,*R; wchar_t *Nam; double Val; int Line;
};
struct Root{//���������� ��� ��������
 Root *Sled; wchar_t *Name; Parm *First; int Line;
};

struct Body{//������� ����
 Body *Sled; wchar_t *Name,*BaseName; Parm *KoordSys,*Inert; int Line;
 Form rCk[4],vCk[4],aCk[4],wkk[4],Tk0[4][4],Kx,Ky,Kz,Jxy,Jyz,Jxz; bool Accum;
};

struct Stringer{//��������
 Stringer *Sled; wchar_t *Name; Parm *Parms; int Line;
 Form FE,FG,FL,FF,FJx,FJy,FJz,Cix,Ciy,Ck,Cx,Cy,Cz,dA,dB,dG,dX,dY,dZ,
  X1,Y1,Z1,X2,Y2,Z2;
 bool Fill(wchar_t *Name);
 bool Elems();
 bool TakeRGCoords(Parm *P,wchar_t *Name);
 bool TakeAnglsCoords(Parm *P);
 void TakeDekartCoords(Parm *P);
};

struct List{//����
 List *Sled; wchar_t *Name; Vary *V,*Time,*Freq; Unop *U; Elem *E; Ipll *L;
 Body *B; Stringer *T; Arra *A; Summ *M; Intg *I; Solu *C; Rest *O;
//����. ����������, �����������,
//���������� � ���������� ���������� (����������), ��������������
 Cord *K,*P,*Z,*W,*R,*N,*Dq,*Dv;
 Root *Inst,*Oper; Term *Sklad; Spln *S;
};

	  // �������� � ���������:
//�������:
 Form operator  - (Form F);
//��������:
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
		/*  ���������� �������:  */
 bool IsPust (Form F); //������� - �����?
 bool IsMins (Form F); //������� - ������� �����?
 bool IsCnst (Form F); //������� - ���������?
 bool IsVary (Form F); //������� - ����������?
 bool IsSumm (Form F); //������� - �����?
 bool IsDiff (Form F); //������� - ��������?
 bool IsMult (Form F); //������� - ������������?
 bool IsDivd (Form F); //������� - �����?
 bool IsPowr (Form F); //������� - ��������� �������?
 bool IsSin  (Form F); //������� - �����?
 bool IsCos  (Form F); //������� - �������?
 bool VassalOfKoord(List *L,Form F);//������� ������� �� ���������� ���������?
 bool VassalOfKoord(List *L,Elem *E);//���������� �������� ������� �� ���������� ���������?
 bool VassalOfKoord(List *L,Vary *V);//���������� ������� �� ���������� ���������
 bool VassalOfPsevdo(List *L,Form F);//������� ������� �� ��������������?
 bool VassalOfDepedent(List *L,Form F);//������� ������� �� ��������� � ���������� ����������?
 bool VassalOfVelosity(List *L,Form F);//������� ������� �� ���������� ���������?
 bool VassalOfAccelerat(List *L,Form F);//������� ������� �� ��������� ���������?
 bool OwnRelation(Vary *V);//������� �� ���������� V ���� �� ����
 Form SubLimits(Intg *I);//��������� ������� ��������������
 bool IsTaken(Intg *I);
//������� �� ������������ ��������, ���� ��, �� � ���� Eqv ��������� Intg �����
//�������� ������� ��������������, � � ���� Rez - ������� � ������ �����������
//�������� �������������� � ������������� ����
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
/* ��� ��������� ���� ��������� ������������ ����� ����-    */
/* ����������� �������� � ������ S � ������ �������� �����. */
/* ������������ 1 ���� ����� ������ ������� � 0 - ���� ���. */
/* � ������ ������ ������ ���������� �������� �������� ��-  */
/* ��� ������ �������, �� ������  ������ - NULL, ���� ���-  */
/* ��� ����, ��� ����� ���������� ������� ������ - � ����   */
/* ����� ��������� ����� ������.                            */
 bool IsVaried(List *L,Vary *V);//����������� �� ���������� V
 Cnst *TakeCnst (wchar_t *S);     Cnst *TakeCnst (double D);
 Vary *TakeVary (wchar_t *S);     Elem *TakeElem (wchar_t *S);
 Arra *TakeArra (wchar_t *S);     Root *TakeInst (wchar_t *S);
 Root *TakeOper (wchar_t *S);     List *TakeList (wchar_t *S);
 Cord *TakeCord (Vary *V);     Cord *FindCord(Cord *K,wchar_t *S);
 Vary *TakeVary (String S);    Body *TakeBody (wchar_t *S,wchar_t *s);
 Stringer *TakeStringer (wchar_t *S);
/* ��� ��������� ���� ��������� ������������ ����� ����-    */
/* ����������� �������� � ������ S � ������ �������� �����. */
/* ���� ������ ������� �� �����������, �� ������������ ��-  */
/* ��� � ��������� � ����� ������. �������� ����� ������-   */
/* ���� ��� ���������� �������                              */
/* ��� ��������� ���� ��������� ������������ ����� ����-    */
/* ����������� �������� � ������ S � ������ �������� �����. */
/* ���� ������ ������� �� �����������, �� ������������ ��-  */
/* ��� � ��������� � ����� ������. �������� ����� ������-   */
/* ���� ��� ���������� �������                              */

int Coord(List *L);
/* ��������� ����������� ������� ���������� ���������       */
int CoordWithoutDT(List *L);
/* ��������� ����������� ������� ���������� ���������
   ��� ��������� ������ ��� ��������� � ���������       */
void TakeDifTime();
/* ��������� ��������� ������ ��� ��������� � ���������       */

int PrintForm(bool DOS,FILE *OUTFILE,wchar_t *Name, Form F);
// ��������� ��������������� ������ ������� F � ������ S;
// �������� �� � ���� <Name> = <�������>;, ���� Name&&*Name!=0,
// ���� Name!=NULL � *Name='\0', �� � ���� \n<�������>,
// ����� � ���� <�������>.                  */

Form TakeApForm (Form F); void TakeApOper(Root *O);
void TakeApParm(Parm *p);
/* ��������� ��������� ������������ �������� ������� (����- */
/* ������ ������, ���������� ��� ������� � �������� �����-  */
/* ���), ���������, ������ ���������� ���������� ���������- */
/* �����.                                                   */

void Take_t_w (void);
/* ��������� ���������� ���������� '�����' (t) � ���������� */
/* '�������' (w) ��� �������� ����� L.                      */

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
/* ��������� ���� ��������� ���������� �������, ��������,   */
/* ��������, �������� � ������ ������������ � �����������   */
/* � ������������� �������� � ������� Oper (����� Type) ��� */
/* ���������������� ���������.                              */

   Form FormTras(Form F);
/* ��������� �������� ����������� ������� F, �� ���� ����-  */
/* ���� ��� ����������, �� ���������� ����������� ��������- */
/* ���� (�� ���� Znach!=Pust), �� ����������, � ��������    */
/* � ����������� �������� ���������� ���� ��������.         */

   Form FormOpenDiff(Form F);
/* ��������� �������� ������ ������� F ��������, � �������  */
/* ��� ����������, ��� ������� �������� ���� "'" ���������� */
/* �� ����������� ����������.                               */

   Form FormToOpen(Form F);
/* ��������� �������� ����������� ���������� � �������,     */
/* ���� �������� ���������� �� ���� ���������. ���� �����-  */
/* ���� ���������� �������� ���������, ���������� �� ���-   */
/* ������.                                                  */
   Form ObNul(Form F);
/* ��������� �������� �������, ���������� �� ��� �������-
   ������ ������� ����������, ������ ����.                  */
Form ObNulCord(Form F);
/* ��������� �������� �������, ���������� ������ ����������
   ��������� ����.                                          */

void OutPut(TRichEdit *TR,int Otstup, wchar_t *WORD, wchar_t *S);
/* ��������� ������� �� ����� ���������� S, �������� ee ��   */
/* ���������. ��� Otstup=0 ����� ����� �� ��������� ������   */
/*2 ��� ��������. ��� Otstup>0 ����� ��������� ������ ������, */
/* ��������� ������ �������� � ����� WORD, � ��� ��������-   */
/* ��� �������� � �������, ������� ����� WORD.               */

void Error(int Line,int Pos,bool Mess);
// ��������� ������� � ���� ERRFILE ���������� �� ������ Inf (�����-
// ����� ��������), ��������� �� ������� ������ ����� �������� ������,
// ������� ������� � ���, ��� ������������ ������ � ������ *  ������  *.
// ����� ����, ����������� ��������� IsError ���������� �������� 2,
// � KolErr++.

void Atten(int Line,int Pos);
// ��������� ������� � ���� ERRFILE ���������� �� ������ Inf (�����-
// ����� ��������), ��������� �� ������� ������ ����� �������� ������,
// ������� ������� � ���, ��� ������������ ������ � ������ * �������� *.
// ����� ����, ������� �������������� ���������� �� 1 ( KolMes++ ).

void ErrorSynt(TRichEdit *RE,wchar_t *S,int Cod=2);

   void InitCA   (void); void SbrosDif (void);
   void CodElem(wchar_t *Cod);
   bool VvodJob(TRichEdit *Ri,TRichEdit *Ro,TCGauge *CGauge,TStatusBar *StatusBar);
//   bool KolElem (List *L, wchar_t *SE);
   Form DifBy(Form F,Vary *V);//������ ������� ����������� F �� V
   Form DifByT(Vary *X);//������ ����������� �� ������� �� ���������� �

   bool Valc(Form F,double &Znach);
   void ClearVar (void); double Val(Form F); double Value(Form E);
   bool VarInForm (bool Everywhere,Form F,Vary *X);
		 /*   ������������� �������:   */
   Form   sqrt(Form F); Form    sin(Form F); Form    cos(Form F);
   Form     tg(Form F); Form arcsin(Form F); Form arccos(Form F);
   Form  arctg(Form F); Form    exp(Form F); Form     ln(Form F);
   Form     sh(Form F); Form     ch(Form F); Form   arsh(Form F);
   Form   abs (Form F); Form   rand(Form F); Form   ceil(Form F);
   Form  floor(Form F);
	      /*   ���������� ����������:   */
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
//��������� ���������� �������� VarsInForm ������� ����������, ��������
//� ������� F � � ���������� � ��������, �������� � ��� �������
  int VaryToIntg(int k,Form F,Cnst *Znach);
//��������� ���������� ������ ���������� �� ������ ����������
  int VaryToSol(int k,Form F,Cnst *Znach);
//��������� ���������� ������ ������ �� ������ ����������
  bool __fastcall ReplaceVarsToConst(TStatusBar *StatusBar,TCGauge *CGauge,
   bool Second);
//��������� ������ ������ ���������� ���������� ����������
//---------------------------------------------------------------------------

#endif

