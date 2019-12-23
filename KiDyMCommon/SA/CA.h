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
#define ARRA     14
#define ELEM     15
#define SPLN     16
#define BODY     17
#define STRINGER 18
#define INTG     19
#define RUUT     20
#define REST     21
#define TEXTCOMM 22
#define ARRI     23
#define NUMB     24
#define FORMULA  25
#define NAMELIST 26
#define DIFF     27
#define MULTVECT 28
#define DEFINE   1
#define NODEF    0
#define BLANK    ' '
#define RAND     14
#define IPOL     0
#define MATR     1
#define CLMN     2
#define VECT     3
#define QUAT     4
#define MINUS    1
#define SIN      3
#define COS      4
#define TG       5
#define MASSA    1
#define LENGTH   2
#define TIME     3

		 /*  ���������� ��������:  */
struct Cnst; struct Vary; struct Unop; struct Bnop; struct Ifop;
struct Summ; struct Intg; struct Ipll; struct Spln; struct Body;
struct Rest; struct Solu; struct ArrI; struct Diff; struct Arra;

union  Form{ //�������
 Cnst *C; Vary *V; Unop *U; Bnop *B; Ifop *L; Summ *S; Intg *G;
 Ipll *I; Spln *P; Solu *R; Rest *O; ArrI *a; Diff *D; Arra *A;
};
struct Cnst{//����������
 Cnst *Sled; int Atr; wchar_t *Name; double Val;
};
struct Vary{//����������
 Vary *Sled; int Atr; wchar_t *Name; Form Znach,Razm;
 double Val,Krazm; int P,Q,O,U,Line;
};
struct Unop{//������� ��������
 Unop *Sled; int Atr,Func; Form Arg; double Val; int P,O;
};
struct Bnop{//�������� ��������
 int Oper; int Atr; Form L,R;
};
struct Ifop{//�������� �����������
 int Oper; int Atr; Form U,V,FT,FF,UT;//UT - ������� �������� ������ ��������
 bool fu,fv; //����� �������������� �������� ���������� � �� �������� ��� << (>>)
 double tt,tf, //����� ������� ���������� � ������������ ������� �
  Old;         //������������ �������� ��� ���������� ���������� ��������
};
struct Summ{//�����
 Summ *Sled; int Atr; int Type; Vary *i; Form a,b,F;
};
struct Intg{//��������
 Intg *Sled;
 int Atr, //=INTG=19
  Type,//=1, ������������, =0, �������������� ��������
  P;      //>=0, ���� ������� ��������
 Vary *x; //���������� ��������������
 Form a,b,//������� ���������������, f,F
  f,F,    //��������������� ������� � �������������
  Rez;    //������������� ��������� �������������� � ������ ����������� ��������
 double Val,B; //��������� ��������� ��������� ��� �������� ������� = B
};
struct Solu{//������ ������� ����� ����������
 Solu *Sled;
 int Atr;//=RUUT=20
 Vary *x; //����������-��������
 Form f,//�������, ������ ������� ���� �����
  a,b;//������� ��� ������ �����
 double Val; //��������� ��������� ����������� �����
};
struct Rest{//��������� ���������� ������� �� �������
 Rest *Sled;
 int Atr;//=REST=21
 Form a,b;//��������� � �����������
 double Val; //��������� ��������� ���������� ������� �� �������
};
struct Arra{//������
 Arra *Sled; int Atr,N,Type; wchar_t *Name; double *A;
 Form *F,Znach; int Line;
};
struct ArrI{//������� �������
 ArrI *Sled; int Atr; Arra *A; int I,Line; double a; Form FInd;
 bool Cmp(Arra *A,Form Find);
};
struct Ipll{//�������� ������������
 Ipll *Sled; int Atr; int Type; Arra *A; Form F;
};
struct Spln{//���������� ������
 Spln *Sled; int Atr; int Type; Arra *A; Form F; double *a,*b,*c,*d;
 void CalcSpln(void);
};
struct Diff{//�������� ��������
 Vary *Y; int Atr; Vary *X;
};
struct Elem{//�������
 Elem *Sled; wchar_t *Name; Vary *K; Form Znach; int Line;
};
struct Cord{//���������� - ������ ����������
 Cord *Sled; Vary *V;
};
struct Parm{//��������  ����������
 Parm *Sled,*R; wchar_t *Nam; double Val; Form F; int Line;
};
struct Root{//���������� ��� ��������
 Root *Sled; wchar_t *Name; Parm *First; int Line;
};

struct Body{//������� ����
 Body *Sled; wchar_t *Name,*BaseName; Parm *KoordSys,*Inert; int Line; bool Accum;
 Form rOk[3],VOk[3],AOk[3],vOk[3],aOk[3],rCk[3],VCk[3],ACk[3],vCk[3],aCk[3],
  wkk[3],ekk[3],Sk0[3][3],roOkk_1[3],vOkk[3],aOkk[3],roCkk[3],vCkk[3],aCkk[3],
  m,Jx,Jy,Jz,Jxy,Jyz,Jxz,Kx,Ky,Kz;
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

bool IsUnop (Form F){ return (F.C->Atr==UNOP); }
bool IsMins (Form F){ return ((F.C->Atr==UNOP)&&(F.U->Func==1)); }
bool IsSqrt (Form F){ return ((F.C->Atr==UNOP)&&(F.U->Func==2)); }
bool IsSin  (Form F){ return ((F.C->Atr==UNOP)&&(F.U->Func==3)); }
bool IsCos  (Form F){ return ((F.C->Atr==UNOP)&&(F.U->Func==4)); }
bool IsPust (Form F){ return (F.C->Atr==PUST); }
bool IsCnst (Form F){ return ((F.C->Atr==CNST)&&!IsPust(F)); }
bool IsVary (Form F){ return (F.C->Atr==VARY); }
bool IsSumm (Form F){ return ((F.C->Atr==BNOP)&&(F.B->Oper==1)); }
bool IsDiff (Form F){ return ((F.C->Atr==BNOP)&&(F.B->Oper==2)); }
bool IsMult (Form F){ return ((F.C->Atr==BNOP)&&(F.B->Oper==3)); }
bool IsDivd (Form F){ return ((F.C->Atr==BNOP)&&(F.B->Oper==4)); }
bool IsRest (Form F){ return ((F.C->Atr==BNOP)&&(F.B->Oper==5)); }
bool IsPowr (Form F){ return ((F.C->Atr==BNOP)&&(F.B->Oper==6)); }

class Factor{//�����������
 public:
  Factor *Sled;  //������ �� ��������� ������������
//  union{ Cnst *C; Vary *V; Unop *U; Bnop *B; Ipll *I; Spln *P; };
  Form F;
  Factor(){ Sled=NULL; }
  Factor(Form F);
  int Cmp(Factor *f);
  Form ListToForm();
  bool Eqv(Factor *);
};

class Summand{ //��������� - ������ �� ������������
 public:
  Summand *Sled;  //������ �� ��������� ���������
  bool Znak;//true - ��� �����, false - ��� ����
  Factor *R;   //������ ������ ������������
  Summand(){ R=NULL; Sled=NULL; Znak=false; }
  int Find(Factor *f,Factor **pF);
  Factor *Add(Factor *f);
  int Cmp(Summand *s);
  Form ListToForm();
  Form ToForm();
  bool Eqv(Summand *);
 ~Summand();
};

class Train{  //������ �� ���������
  Summand *R; //������ ������ ���������
 public:
  Train(){ R=NULL; }
  Train(Form F);
  void TrainFromForm(Form F);
  void Minus(){ for(Summand *s=R;s;s=s->Sled) s->Znak=!s->Znak; }
  int  Find(Summand *s,Summand **pS);
  void Add(Train *t);
  void Add(Summand *s);
  void ToStr(AnsiString &S);
  void ToFile(wchar_t *NameFile, bool New);
  Form ToForm();
  bool Eqv(Train *);
  bool Nul(){ return R==NULL; }
 ~Train();
};

struct List{//����
 List *Sled; wchar_t *Name; Vary *V,*Time,*Freq; Unop *U; Elem *E; Ipll *L;
 Body *B; Stringer *T; Arra *A; Summ *M; Intg *I; Solu *C; Rest *O; ArrI *a;
 Parm *Units; // Units - ������ ������������ ����� - � ����� ����������� ���
 // ������� ������ (m(��), l(�), t(�)), � � ����� �������� - ���������������� 
//����. ����������, �����������,
//���������� � ���������� ���������� (����������), ��������������
 Cord *K,*P,*Z,*W,*R,*N,*Dq,*Dv,*VK;
 Root *Inst,*Oper; Spln *S;
};
//---------------------------------------------------------------------------
class TextComm{
 public:
  TextComm *Sled; int Atr,Line; String S;
};
//---------------------------------------------------------------------------
class TSizesLines{
 public:
  int ind, h, y, x, l;
};
//---------------------------------------------------------------------------
class TImageUnit{
 public:
  TImageUnit *Sled; int Atr,Line,Pos, H, L; bool Comm;
  union {Vary *V; Elem *E; Root *R; Arra *A; Body *B; Stringer *S; TextComm *T; }
   UnitKDM;
  int Height(TCanvas *Canvas,TSizesLines *SL);
  int Width(TCanvas *Canvas,TSizesLines *SL);
  int Draw(TCanvas *Canvas,int X, int Ya, TSizesLines *SL,int MarginLeft);
//  void ToHTM(FILE *F);
};
//---------------------------------------------------------------------------
class TImageKDM{
 public:
  TImageUnit *R,**p,*r; TSizesLines *SL; int MarginLeft;
  TImageKDM(){ R=NULL; p=&R; MarginLeft=8; }
  void Add(Vary *V){
   *p=r=(TImageUnit *)calloc(1,sizeof(TImageUnit));
   r->UnitKDM.V=V; r->Atr=VARY; r->Line=V->Line; p=(TImageUnit **)r;
  };
  void Add(Elem *E){
   *p=r=(TImageUnit *)calloc(1,sizeof(TImageUnit));
   r->UnitKDM.E=E; r->Atr=ELEM; r->Line=E->Line; p=(TImageUnit **)r;
  };
  void Add(Root *R){
   *p=r=(TImageUnit *)calloc(1,sizeof(TImageUnit));
   r->UnitKDM.R=R; r->Atr=ROUT; r->Line=R->Line; p=(TImageUnit **)r;
  };
  void Add(Arra *A){
   *p=r=(TImageUnit *)calloc(1,sizeof(TImageUnit));
   r->UnitKDM.A=A; r->Atr=ARRA; r->Line=A->Line; p=(TImageUnit **)r;
  };
  void Add(Body *B){
   *p=r=(TImageUnit *)calloc(1,sizeof(TImageUnit));
   r->UnitKDM.B=B; r->Atr=BODY; r->Line=B->Line; p=(TImageUnit **)r;
  };
  void Add(Stringer *S){
   *p=r=(TImageUnit *)calloc(1,sizeof(TImageUnit));
   r->UnitKDM.S=S; r->Atr=STRINGER; r->Line=S->Line; p=(TImageUnit **)r;
  };
  void Add(TextComm *T){
   *p=r=(TImageUnit *)calloc(1,sizeof(TImageUnit));
   r->UnitKDM.T=T; r->Atr=TEXTCOMM; r->Line=T->Line; p=(TImageUnit **)r;
  };
//  void ToHTM(FILE *F);
  Root *FindLastInst(wchar_t *Name);
  void ToImage(TImage *Im,TPaintBox *Img);
//  void __fastcall Sizes(TImage *Img, int y);
  void __fastcall AreaSizes(TCanvas *Canvas, int &maxH, int &maxL);
  void Draws(TPaintBox *PaintBox);
  void DelInstCalc(Root *I_Sled);
  void DrawShow(TPaintBox *pbReport,int &y);
  void VaryHeight(TCanvas *Canvas, int &H);
 ~TImageKDM(){ while(r=R){ R=r->Sled; free(r); }}
};
//---------------------------------------------------------------------------
	  // �������� � ���������:
//�������:
 Form operator  - (Form F);
//��������:
 bool operator == (Form A, Form  B);  bool operator != (Form A, Form  B);
 bool operator == (Form F, Cnst *C);  bool operator != (Form F, Cnst *C);
 bool operator == (Form F, Vary *V);  Form operator += (Form A, Form  B);
 Form operator  + (Form A, Form   B); Form operator  - (Form A, Form   B);
 Form operator  * (Form A, Form   B); Form operator  / (Form A, Form   B);
 Form operator  % (Form A, Form   B); Form operator  ^ (Form A, Form   B);
 Form operator  + (Form A, double D); Form operator  - (Form A, double D);
 Form operator  * (Form A, double D); Form operator  / (Form A, double D);
 Form operator  % (Form A, double D); Form operator  ^ (Form A, double D);
 Form operator  + (double D, Form A); Form operator  - (double D, Form A);
 Form operator  * (double D, Form A); Form operator  / (double D, Form A);
 Form operator  % (double D, Form A); Form operator  ^ (double D, Form A);
 Summand *operator * (Summand A,Summand B);
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
 bool IsRest (Form F); //������� - ������� �� �������?
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
 Form IsTaken(Form f,Vary *x);
//������� �� ������������ �������� �� f, ���� ��, ��
//����������� ��������� �������������� (�������������), ����� - Pust
 bool IsTaken(Intg *I);
//������� �� ������������ ��������, ���� ��, �� � ���� Eqv ��������� Intg �����
//�������� ������� ��������������, � � ���� Rez - ������� � ������ �����������
//�������� �������������� � ������������� ����
// bool FindCnst  (wchar_t *S,Cnst **C); bool FindCnst (double D,Cnst **C);
 int FindCnst   (wchar_t *S,Cnst **C); int  FindCnst (double D,Cnst **C);
 bool FindVary  (wchar_t *S,Vary **V); bool FindVary(String S,Vary **V);
 bool FindElem  (wchar_t *S,Elem **E); bool FindCloseElem(wchar_t *S,Elem **E);
 bool FindArra  (wchar_t *S,Arra **A); bool FindList(wchar_t *S,List **L);
 bool FindInst  (wchar_t *S,Root **I); bool FindOper(wchar_t *S,Root **O);
 bool FindCord  (wchar_t *S,Cord **K); bool FindZavC(wchar_t *S,Cord **K);
 bool FindPsevdo(wchar_t *S,Cord **K); bool FindUnkn(wchar_t *S);
 bool FindBody (wchar_t *S,Body **B);  bool FindKoordElem(wchar_t *Koord,Elem **E);
 Root *FindInstLast(wchar_t *S);       Root *FindOperLast(wchar_t *S);
 Arra *FindArraLast(wchar_t *S);        
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
 Cnst *TakeCnst (wchar_t *S,double D);
 Vary *TakeVary (wchar_t *S);     void DelVary(wchar_t *S);
 Elem *TakeElem (wchar_t *S);     Diff *TakeDiff(Vary *y,Vary *x);
 Arra *TakeArra (wchar_t *S);     ArrI *TakeArrI(Arra *A,Form F);
 Root *TakeInst (wchar_t *S);     Root *TakeOper (wchar_t *S);
 List *TakeList (wchar_t *S);     Cord *FindCord(Vary *V,Cord *K);
 Cord *TakeCord (Vary *V);     Cord *FindCord(Cord *K,wchar_t *S);
 Cord *TakeVeloCord(Vary *V);  void DelCord(Cord **R,Cord *q);
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

int Coord(List *L,TCGauge *CGauge,TStatusBar *StatusBar);
/* ��������� ����������� ������� ���������� ���������       */
bool Coord(wchar_t *NameV);
/* ���������, �� �������� �� ���������� � ������ NameV ���������� ����������� */
int CoordWithoutDT(List *L);
/* ��������� ����������� ������� ���������� ���������
   ��� ��������� ������ ��� ��������� � ���������       */
void TakeDifTime(TCGauge *CGauge,TStatusBar *StatusBar,bool IsDifCoord);
/* ��������� ��������� ������ ��� ��������� � ���������       */
void TakeDifVary(TCGauge *CGauge,TStatusBar *StatusBar);
/* ��������� ��������� ���������� �� ���������� (�� �� �������0 */
int PrintForm(bool DOS,FILE *OUTFILE,wchar_t *Name, Form F);
// ��������� ��������������� ������ ������� F � ������ S;
// �������� �� � ���� <Name> = <�������>;, ���� Name&&*Name!=0,
// ���� Name!=NULL � *Name='\0', �� � ���� \n<�������>,
// ����� � ���� <�������>.                  */
int PrintFormToVars(FILE *OUTFILE,wchar_t *NameF,Form F,Magazine *ToV);
// ��������� ����� � ���� OUTFILE ��������� ����� ������� F;
// �������� �� � ���� <Name> = <�������>;, ���� Name&&*Name!=0,
// ���� Name!=NULL � *Name='\0', �� � ���� \n<�������>,
// ����� � ���� <�������>.
// ����� ��������� ������� ��� ���� ����������, �������� � ������� F
// � �� ������� � �������� ����� ���������� �� ������ Tov                  */
//---------------------------------------------------------------------------
void PrintMatr(wchar_t *File,wchar_t *Comm,wchar_t *NameMatr,double **M,int N);
//���������� ������� � �������� �������� ����������� N � ���� � ������ File
//---------------------------------------------------------------------------
void PrintVect(wchar_t *File,wchar_t *Comm,wchar_t *NameVect,double *V,int N);
//���������� ������� V �������� �������� ����������� N � ���� � ������ File
//---------------------------------------------------------------------------
void PrintVectF(wchar_t *File,wchar_t *Comm,wchar_t *NameVect,Form *V,int N);
//���������� ������� V ���������� �������� ����������� N � ���� � ������ File
//---------------------------------------------------------------------------
void PrintMatrF(wchar_t *File,wchar_t *Comm,wchar_t *NameMatr,Form **M,int N);
//���������� ������� � ���������� �������� ����������� N � ���� � ������ File
//---------------------------------------------------------------------------
void FullPrntForm (FILE *OUTFILE, Form F, Magazine **M);
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
   Summ *FindSumm(Form F,Vary *i,Form a,Form b,int Type);
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

Form FindRazm(Form F);
/* ��������� �������� ����������� ����������  */
/*  */
/*  */
/*  */

   Form FormOpenDiff(Form F);
/* ��������� �������� ������ ������� F ��������, � �������  */
/* ��� ����������, ��� ������� �������� ���� L"'" ���������� */
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
   bool VvodJob(TStringList *SLM,TRichEdit *Ri,TCGauge *CGauge,
				TStatusBar *StatusBar,TImageKDM *ImageKDM,
				TTabControl *TabControl,TCanvas *Canvas);
   Form DifBy(Form F,Vary *V);//������ ������� ����������� F �� V
   Form DifByT(Vary *X);//������ ����������� �� ������� �� ���������� �
   Form DifByT(Form F);//������ ����������� �� ������� �� ������� F

   bool Valc(Form F,double &Znach);
   void ClearVar (void); double Val(Form F); double Value(Form E);
   bool VarInForm (bool Everywhere,Form F,Vary *X);
   bool CnstInForm(Form F,Cnst *X);
		 /*   ������������� �������:   */
   Form   sqrt(Form F); Form    sin(Form F); Form    cos(Form F);
   Form     tg(Form F); Form arcsin(Form F); Form arccos(Form F);
   Form  arctg(Form F); Form    exp(Form F); Form     ln(Form F);
   Form     sh(Form F); Form     ch(Form F); Form   arsh(Form F);
   Form   abs (Form F); Form   rand(Form F); Form   ceil(Form F);
   Form  floor(Form F);
	      /*   ���������� ����������:   */
  bool Treug          (Parm *Par,TRichEdit *R,TCanvas *Canvas);
  bool MidlElem       (Parm *Par,TRichEdit *R,TCanvas *Canvas);
  bool LeadFors       (Parm *Par,TRichEdit *R,TCanvas *Canvas);
  bool Inertion       (Parm *Par,TRichEdit *R,TCanvas *Canvas);
  bool Manipulator    (Parm *Par,TRichEdit *R,TCanvas *Canvas);
  bool Structurs      (Parm *Par,TRichEdit *R,TCanvas *Canvas);
  bool MultiPly       (Parm *Par,TRichEdit *R,TCanvas *Canvas);
  bool ElastBar       (Parm *Par,TRichEdit *R,TCanvas *Canvas);
  bool ToOpen         (Parm *Par,TRichEdit *R,TCanvas *Canvas);
  bool ToClose        (Parm *Par,TRichEdit *R,TCanvas *Canvas);
  bool Solids         (Parm *Par,TRichEdit *R,TCanvas *Canvas);
  bool ElasticRods    (Parm *Par,TRichEdit *R,TCanvas *Canvas);
  bool Quaternion     (Parm *Par,TRichEdit *R,TCanvas *Canvas);
  bool LitlCount      (Parm *Par,TRichEdit *R,TCanvas *Canvas);
  bool ContrCOF       (Parm *Par,TRichEdit *R,TCanvas *Canvas);
  bool CalcsCOM       (Parm *Par,TRichEdit *R,TCanvas *Canvas);
  bool CalcsCOMpoFurie(Parm *Par,TRichEdit *R,TCanvas *Canvas);
  bool Perturbation   (Parm *Par,TRichEdit *R,TCanvas *Canvas);
  bool Kraut          (Parm *Par,TRichEdit *R,TCanvas *Canvas);
  bool Polinom        (Parm *Par,TRichEdit *R,TCanvas *Canvas);
  bool PolinomsCOFCOM (Parm *Par,TRichEdit *R,TCanvas *Canvas);
  bool MorisawaCOFCOM (Parm *Par,TRichEdit *R,TCanvas *Canvas);
  bool ReadArrFromFile(Parm *Par,TRichEdit *R,TCanvas *Canvas);
  bool Bodys          (Body *B,TStatusBar *StatusBar,TTabControl *TC,TCGauge *CGauge);
  bool Stringers      (Stringer *B);

  int SLAU (int N,double **A,double *B,double *x,bool Repeat);
  int Crout(int n, double **A, double *Y, double *B, double Eps);

  bool SpecSimb(wchar_t C);
  wchar_t *RichAttr(TRichEdit *R,wchar_t *S);
  void RichAdd(TRichEdit *R,wchar_t *S);
  void ReadAllRight(Parm *P,FILE *F,bool ToGrek,bool Eqv);
  wchar_t *ReadAllRight(Parm *P,wchar_t *Ln,wchar_t *l,
   TStringList *SL,String *SF,bool ToGrek,bool Eqv);
//  void StrToHTM(wchar_t *S,FILE *HTMFILE);
  void StrToHTM(wchar_t *S,TStringList *SL);
  void StrToHTM(wchar_t *S);
  void TablToHTM(wchar_t *File,int CountTabl,double Tn,double Tk,
  bool Godograf,wchar_t *Format1,wchar_t *Format2,wchar_t *Format3,double *X,
   double *Y,int Podv,wchar_t *Autor);
  void ToTable(wchar_t *Name,int CountTabl,double Tn,double Tk,
   void(*Funcs)(double,Magazine *,double *),wchar_t *File);
  void ClearOper(void);
  void PrintOper(bool DOS,FILE *OUTFILE,Form F,int *KolOp);
  void PrintOper(TStringList *SL,Form F,int *KolOp);
  bool IsCykl(Magazine *Mv,Form F,Magazine **Mrez);
  void TablToReport(wchar_t *Name,wchar_t *Arg,int CountTabl,double Tn,double Tk,
   bool Godograf,void (*Funcs)(double,Magazine *,double *),
   Magazine **NamesInclude,Magazine **NamesTAB,Magazine **NamesSpline,
   Magazine **NamesFurie);
  void SplToReport(wchar_t *Name,int CountTabl,double Tn,double Tk,bool Godograf,
   void (*Funcs)(double,Magazine *,double *),Magazine **NamesInclude,
   Magazine **NamesTAB,Magazine **NamesSpline,Magazine **NamesFurie,
   TCGauge *CGauge);
  void SplGdrfToReport(wchar_t *Name,int CountTabl,double Tn,double Tk,
    void (*Funcs)(double,Magazine *,double *),Magazine **NamesInclude,
    Magazine **NamesTAB,Magazine **NamesSpline,Magazine **NamesFurie,
    TCGauge *CGauge);
  void FurieToReport(wchar_t *Name,int CountTabl,double Tn,double Tk,bool Godograf,
   void (*Funcs)(double,Magazine *,double *),Magazine **NamesInclude,
   Magazine **NamesTAB,Magazine **NamesSpline,Magazine **NamesFurie,
   TCGauge *CGauge);
  String *FormToStr(Form F,String *S,bool HTML);
  int Grek(wchar_t *S);
  wchar_t *LiterGrekToHtm(wchar_t *S);
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
  void VarsFromForm(Form F,Magazine **VarsInForm);
//��������� ���������� �������� VarsInForm ������� ����������, ��������
//� ������� F � � ���������� � ��������, �������� � ��� �������
  void VarsFromFormDefVars(Form F,Cord **VarsInForm,Cord *Exclus);
//��������� ���������� ������ ���������� VarsInForm �����������, ���������
//� ������� F � � ���������� � ��������, �������� � ��� �������
  void MagVars(Form F,Magazine **VarsInForm);
//��������� ���������� �������� VarsInForm ������� ����������,
//��������������� �������� � ������� F ��� ������� ����������,
//�������� � ��� �������
  int VaryToSumm(int k,Form F,Cnst *Znach);
//��������� ���������� ������ ���������� �� ������ ����������
  int VaryToIntg(int k,Form F,Cnst *Znach);
//��������� ���������� ������ ���������� �� ������ ����������
  int VaryToSol(int k,Form F,Cnst *Znach);
//��������� ���������� ������ ������ �� ������ ����������
  bool __fastcall ReplaceVarsToConst(TStatusBar *StatusBar,TCGauge *CGauge,
   bool Second);
//��������� ������ � �������� ������������������ ������ ������� ������� ����������
  Form ReplaceLitlVary(Form F,Parm *P);
  void PrepareIncompleteIf(void);//���������� ������������� ��������� ���������
//������ �� � ������� ���������� �� ���������� �������� ��� ���������
  bool VassalFromVaryDiap(Form F);
//������ �� � ������� ���������� �� ���������� �����������
  bool VassalFromVaryParam(Form F);
  void TakeCordDefVars(Cord **R,Vary *V);
//---------------------------------------------------------------------------
  int IndexW(wchar_t *NameV); //����� ���������� � ������ ���������������
  int IndexZ(wchar_t *NameV); //����� ���������� � ������ �� � ���������� ����������
  int IndexOK(wchar_t *NameV); //����� ���������� � ������ �� � ������������ ����������
  int IndexOV(wchar_t *NameV); //����� ���������� � ������ ����������� ���������
  int FormToFileWithRep(FILE *Fo,wchar_t *NameF,Form F);
  wchar_t *ZamStrich(wchar_t *NameV);
  void ListVary(Form F,Magazine **M);
//������������ ������ ����������, �� ������� ��������������� ������� F
  Form SetRazmByForm(Form Znach);//����������� ����������� �� ������� Znach
  Form SetRazmByName(wchar_t *Name);//����������� ����������� �� ����� ���������� �� '\''
  void SetRazm(void);//��������� ������������ ���� ���������� ����� L"��������"

  void UnOpToVary(Form F,Cord **UnOps);
//�������� ������ ����������, ������ ������� ���������
  Form UnOpRepVary(Form F,Cord *UnOps);
//������ � F ������� �������� �� ����������, ������ ���� ���������
//---------------------------------------------------------------------------
double ToBaseUnit(Form Razm);//������ ���������� ������������ � �������� ������������
void FormTo2List(Form F,Cord **Ch,Cord **Zn);
void CalcKRazm(Vary*V);// ���������� ������������ �����������
void ConvOldRazm(Vary *V);//�������������� � ����������� ������������
int TypOfRazm(Vary *V);//����������� ��� ����������� - �����, ����� ��� �����
bool FindParmParm(Parm *R,wchar_t *S,Parm **P,Parm **p);
Form RemoveItem(Form F,Vary *V);//������� ��������� - ���������� V �� ������� F
bool CreateProcIdentSizes();
bool CreateProcIdentNewtons();
void PrintCord(wchar_t *NameCord,Cord *Q);
wchar_t *FormToStringList(Form F,wchar_t *NameVary,wchar_t *Ln,
  wchar_t *l,TStringList *SL,bool EndLn,String *SF);
#endif

