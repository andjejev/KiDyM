//---------------------------------------------------------------------------
#ifndef EditListH
#define EditListH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ImgList.hpp>
#include <ToolWin.hpp>
#include <ExtCtrls.hpp>
#include <Grids.hpp>
#include "TablW.h"
#include <Menus.hpp>
#include <Printers.hpp>
#include "cdiroutl.h"
#include <Outline.hpp>
#include <Dialogs.hpp>
#include "SzFormat.h"
//---------------------------------------------------------------------------
class TEditList{
 public:
  int Row,Col,VERT,Row0,/*,Left,Top,Width,Height*///��������� � ������� �����
//VERT - ����������� �� ������� �������������� �����������(1)
//��� �������������(0)
   Kp,//����� ���������� �� ��������� ������� �����
   LeftF,TopF,WidthF,HeightF, //���������� ������ �������� ���� �����, �� ������ � ������
   *MasNN,//������ ������� ������������ ����� ��������� (�� 1 � � ����� 0)
   SizeR;//������ ���������
//�������� ������, ����� ������, ����� ������, ��������, ������
  wchar_t FileHelp[256],*FilePrn,FontName[256];
  String Value;
  double Interval;
  void *Kukla;
  void(*Save)(void);//��������� ����������� �� ����� ������
                    //(�������� ������ ��� ������� ������)
  void(*CallCell)(int R,int C);//���������-���������� ��.������ �� ������
                    //(�������� ������ ��� ������� ������)
  bool(*TestField)(TStringGrid *SG,int Row,int NF,wchar_t *Old);
//�������� ��������� �������������� ������� SG, Row - ����� ������,
//Old - �������� (� ���� ������) ���� �� ���������,
//NF=-1 - ������� �������� � �������� ��� �������� ������
//NF=0  - ������� �������� � �������� ��� ���������� ����� ������
//NF=1,2,...,k - ������� �������� � �������� ��� ��������� NF-�� ����
//(k - ��������� �������)
//NF=k+1 - ���� ��������� ��� ������
//NF=k+2 - ���� ��������� ��� �������
  void(**ProcList)(void **R);//������ �������� �������������� ������� ������� �����
   //��� ViewTabl
  wchar_t *FullHead,//������� ��������
   **Head,//������ �������� �������� ��� TabControl�, ������ �������
          //����� - �������� �������� �������� �������, ��������� -
          //��� ����� �������� �������� ������ ������� ������
   *FileSh,//�������� ����� �����
   *Podval,//������
   *FullFormat;//������ ������ ����� (%p - ��������� �� ������)
  TShapka *Shapk;
  bool Podch,Show0,SelectMad;//������������ �� ������ � ���������� �� ����
 private:
  void **Root;
  int
   *NSumm,//������ ������� (�� 1) ����������� ��������, ���������� 0
   KolFixStlb, //- ���������� ������������� �������� � �������
   EKForm;//����� �������� �������
  Magazine ***MM;//������ ���� ������ ���������
  bool *AddMM,Select;//������ ���������� �� ���������� ���������
  wchar_t
   **EMasForm;//������ �������� ����� �������, � �� ���������
 public:
  TEditList(void **R,int *MNN,bool Pch,int *NS,bool Sh0,Magazine ***M,
    wchar_t *F,wchar_t *FSh,wchar_t **H,wchar_t *Fprn,void (*SaveR)(void),
    bool(*Test)(TStringGrid *,int,int,wchar_t *),void(**Proc)(void **),
    int KFStlb,int Lf,int Tf,int Wf,int Hf,wchar_t *FH,wchar_t *Pd,int Vert,
    wchar_t *FileH,bool *AddM,void(*CalCel)(int,int));
  bool GetSelect(){ return Select; };
  void SetSelect(bool S){ Select=S; };
  int GetKolFixStlb(void) {return KolFixStlb;};
  int GetNSumm(int i){
   if(NSumm&&*NSumm) return NSumm[i];
   else return 0;
  };
  int *GetNSumm(){ return NSumm; };
  void *GetR() {return *Root;};
  void SetR(void **R) { Root=R; };
  wchar_t **GetEMasForm() {return EMasForm;};
  int GetEKForm() {return EKForm;};
  void SetEMasFormJ(int i,wchar_t *F){
   EMasForm[i]=(wchar_t *)realloc(EMasForm[i],(wcslen(F)+1)*SzC);
   wcscpy(EMasForm[i],F);
  }
  void __fastcall SetTabs(TTabControl *T);
//�����, ������� � ������� � �����
  void __fastcall MakeTab(TTabControl *TabControl,TStringGrid *StringGrid,
   THeaderControl *HeaderControl);
  bool __fastcall Diagnos();
  Magazine **Mag(int i) { if(MM) return MM[i]; else return NULL; };
  int WhatLarge(int J,wchar_t *Format,wchar_t *SPole);
  void __fastcall ChangeWidthCol(int *HeaderSumm,int Col,
   THeaderControl *ActivHeaderControl,TStringGrid *ActivSG);
  int MasN(int i){ return MasNN[i]; };
  void __fastcall StrSGToRout(TStringGrid *SG,int i,wchar_t *r);
  void __fastcall StringGridToRout(TStringGrid *SG);
  wchar_t *FormatCol(int Col){
   return EMasForm[Col];
  };
  void __fastcall DeleteStr(int N);
  void *__fastcall InsStr(int N,TStringGrid *SG);
  void __fastcall PrintFullHead(int Knfh,TPrinter *Prn,
        int lp,int H,int MarginLeft,int &Y);
  void __fastcall PrintHead(int Knh,TPrinter *Prn,
        int lp,int H,int MarginLeft,int &Y);
  void __fastcall PrintPodval(int Knp,TPrinter *Prn,
        int H,int MarginLeft,int MarginTop,int Hp,int hp,int &Y);
  void  __fastcall DrawPodval(int Knp,TCanvas *Canvas,int H,
     int MarginLeft,int MarginTop,int Hp,int hp,int &Y,double Kx,double Ky);
  void __fastcall PrintOnePage(int Knfh,int Knh,TPrinter *Prn,
         int lp,int L,int H,int MarginLeft,int MarginTop,int Hp,
         int hp,int &Y,int *Li,int Knp,double Ktw);
  void __fastcall PrintManyPage(int Knfh,int Knh,TPrinter *Prn,
         int lp,int L,int H,int MarginLeft,int MarginTop,int Hp,
         int hp,int &Y,int *Li,int Knp,int Ks,int Hfh,int Hh,
         int HSh,double Ktw);
  int  __fastcall PrintBroadTabl(int Knfh,int Knh,TPrinter *Prn,int lp,
         int L,int H,int MarginLeft,int MarginTop,int Hp,int hp,
         int &Y,int *Li,int Knp,int Ks,int Hfh,int Hh,int HSh,int Kfs,
         int Lfs,int Ht,int Nsn,int Nsk,int HIt,int NPage,double Ktw);
  void __fastcall PrintHugeTabl(int Knfh,int Knh,TPrinter *Prn,int lp,
         int L,int H,int MarginLeft,int MarginTop,int Hp,int hp,
         int &Y,int *Li,int Knp,int Ks,int Hfh,int Hh,int HSh,int Kfs,
         int Lfs,int Ht,double Ktw);
  void __fastcall DefL(TPrinter *Prn,int L,int &Lfs,int *Li,
   int *LiDo,int *LiZa,int &Lt,double Ktw,Magazine *M);
  void __fastcall DefWidthSGCols(TPrinter *Prn,int *Li,int *LiDo,
   int *LiPo,double Ktw,Magazine *M);
  void __fastcall DefWidthSGCols(TPrinter *Prn,int *Li,int *LiDo,
   int *LiPo,int LChar,Magazine *M);
  bool show0(){ return Show0; }
  void __fastcall SetKukla();
  wchar_t *FH(){ return FullHead; }
  wchar_t *FileHlp(){ return FileHelp; }
  wchar_t *H0(){ return Head[0]; }
  wchar_t *Podv(){ return Podval; }
 ~TEditList();
};
FILE *OpenShap(wchar_t *AllShap,wchar_t *FileSh);
//---------------------------------------------------------------------------
class TFormEditList : public TForm{
__published:	// IDE-managed Components
        TStatusBar  *StatusBarEditList;
        TToolBar    *ToolBarEditList;
        TToolButton *ToolButtonSave;
        TToolButton *ToolButton2;
        TToolButton *ToolButtonMove;
        TToolButton *ToolButtonCopy;
        TImageList  *ImageList;
        TToolButton *ToolButtonInsert;
        TToolButton *ToolButton8;
        TToolButton *ToolButtonPrint;
        TToolButton *ToolButtonFinde;
        TToolButton *ToolButtonRep;
        TToolButton *ToolButton14;
        TToolButton *ToolButtonHelp;
        TTabControl *TabControlEditList;
        TToolButton *ToolButton1;
        TToolButton *ToolButtonQuit;
        TToolButton *ToolButtonDirection;
        TPopupMenu  *PopupMenuEditList;
        TMenuItem   *pmInsStr;
        TMenuItem   *pmAddStr;
        TMenuItem   *pmDelStr;
        TMenuItem   *pmInsCopyStr;
        TMenuItem   *pmAddCopyStr;
        TMenuItem   *pmSave;
        TToolButton *ToolButtonImagePrn;
        TButton     *ButtonFullHead;
        TButton     *ButtonPodval;
        TToolButton *ToolButton3;
        TToolButton *ToolButtonCalcul;
        TFindDialog *FindDialogEditList;
        TReplaceDialog *ReplaceDialogEditList;
        TMenuItem   *pmFind;
        TMenuItem   *pmReplace;
        TToolButton *ToolButton4;
        TToolButton *ToolButtonDOSPrn;
        TMenuItem   *pmFromMag;
        TMenuItem   *pmLine;
        TMenuItem   *pmList1;
        TMenuItem   *pmList2;
        TMenuItem   *pmList3;
        TMenuItem   *pmList4;
        TToolButton *ToolButtonPrnOpt;
        TMenuItem   *pmFind1;
        TMenuItem   *pmFind2;
        TMenuItem   *pmFind3;
        TMenuItem   *pmFind4;
        TMenuItem   *pmFind5;
        TMenuItem   *pmFind6;
        TMenuItem   *pmFind7;
        TMenuItem   *pmFind8;
        TMenuItem   *pmFind9;
        TMenuItem   *pmFind10;
        TMenuItem   *pmFind11;
        TMenuItem   *pmFind12;
        TMenuItem   *pmColorPhon;
        TMenuItem   *pmColorText;
        TMenuItem   *pmColorFixPhon;
        TMenuItem   *pmVvodEnter;
        TColorDialog *ColorDialog;
        TMenuItem   *N1;
        TMenuItem   *N2;
        TMenuItem   *N3;
        TMenuItem   *N4;
        TMenuItem   *N5;
        TToolButton *tbEnterTabl;
        TToolButton *ToolButton5;
        TToolButton *ToolButtonSelect;
        TMenuItem   *N6;
        TMenuItem   *pmCut;
        TMenuItem   *pmCopy;
        TMenuItem   *pmPaste;
        TToolButton *ToolButtonExel;
        TToolButton *ToolButton7;
        TToolButton *ToolButton6;
        TUpDown *UpDown;
        TToolButton *ToolButton9;
        TMenuItem *pmList5;
        TStringGrid *StringGridEditList;
        THeaderControl *HeaderControlEditList;
        void __fastcall TabControlEditListChange(TObject *Sender);
        void __fastcall StringGridEditListKeyDown(TObject *Sender,
          WORD &Key, TShiftState Shift);
        void __fastcall ListBoxMagKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall ToolButtonQuitClick(TObject *Sender);
        void __fastcall StringGridEditListTopLeftChanged(TObject *Sender);
        void __fastcall ToolButtonDirectionClick(TObject *Sender);
        void __fastcall pmDelStrClick(TObject *Sender);
        void __fastcall pmInsStrClick(TObject *Sender);
        void __fastcall pmInsCopyStrClick(TObject *Sender);
        void __fastcall pmAddCopyStrClick(TObject *Sender);
        void __fastcall pmAddStrClick(TObject *Sender);
        void __fastcall pmSaveClick(TObject *Sender);
        void __fastcall FormCreate(TObject *Sender);
        void __fastcall StringGridEditListSelectCell(TObject *Sender,
          int ACol,int ARow,bool &CanSelect);
        void __fastcall StringGridEditListEnter(TObject *Sender);
        void __fastcall ToolButtonPrintClick(TObject *Sender);
        void __fastcall ButtonFullHeadClick(TObject *Sender);
        void __fastcall ToolButtonSaveClick(TObject *Sender);
        void __fastcall ButtonPodvalClick(TObject *Sender);
        void __fastcall ToolButtonHelpClick(TObject *Sender);
        void __fastcall ToolButtonCalculClick(TObject *Sender);
        void __fastcall ToolButtonFindeClick(TObject *Sender);
        void __fastcall FindDialogEditListFind(TObject *Sender);
        void __fastcall ToolButtonRepClick(TObject *Sender);
        void __fastcall ReplaceDialogEditListFind(TObject *Sender);
        void __fastcall ReplaceDialogEditListReplace(TObject *Sender);
        void __fastcall pmFindClick(TObject *Sender);
        void __fastcall pmReplaceClick(TObject *Sender);
        void __fastcall ToolButtonImagePrnClick(TObject *Sender);
        void __fastcall ListBoxMagDblClick(TObject *Sender);
        void __fastcall ToolButtonDOSPrnClick(TObject *Sender);
        void __fastcall pmFromMagClick(TObject *Sender);
        void __fastcall pmList1Click(TObject *Sender);
        void __fastcall pmList2Click(TObject *Sender);
        void __fastcall pmList3Click(TObject *Sender);
        void __fastcall pmList4Click(TObject *Sender);
        void __fastcall ToolButtonPrnOptClick(TObject *Sender);
        void __fastcall pmFind1Click(TObject *Sender);
        void __fastcall pmFind2Click(TObject *Sender);
        void __fastcall pmFind3Click(TObject *Sender);
        void __fastcall pmFind4Click(TObject *Sender);
        void __fastcall pmFind5Click(TObject *Sender);
        void __fastcall pmFind6Click(TObject *Sender);
        void __fastcall pmFind7Click(TObject *Sender);
        void __fastcall pmFind8Click(TObject *Sender);
        void __fastcall pmFind9Click(TObject *Sender);
        void __fastcall pmFind10Click(TObject *Sender);
        void __fastcall pmFind11Click(TObject *Sender);
        void __fastcall pmFind12Click(TObject *Sender);
        void __fastcall pmColorPhonClick(TObject *Sender);
        void __fastcall pmColorFixPhonClick(TObject *Sender);
        void __fastcall pmColorTextClick(TObject *Sender);
        void __fastcall pmVvodEnterClick(TObject *Sender);
        void __fastcall MouseDown(TObject *Sender,TMouseButton Button,
         TShiftState Shift,int X,int Y);
        void __fastcall ContextPopupClipping(TObject *Sender,
         const TPoint &MousePos, bool &Handled);
        void __fastcall StringGridEditListDblClick(TObject *Sender);
        void __fastcall tbEnterTablClick(TObject *Sender);
        void __fastcall ToolButtonSelectClick(TObject *Sender);
        void __fastcall ToolButtonInsertClick(TObject *Sender);
        void __fastcall pmCutClick(TObject *Sender);
        void __fastcall pmCopyClick(TObject *Sender);
        void __fastcall pmPasteClick(TObject *Sender);
        void __fastcall ToolButtonExelClick(TObject *Sender);
        void __fastcall UpDownClick(TObject *Sender, TUDBtnType Button);
        void __fastcall pmList5Click(TObject *Sender);
        void __fastcall FormShow(TObject *Sender);
//        void __fastcall FormCanResize(TObject *Sender, int &NewWidth,
//          int &NewHeight, bool &Resize);
        void __fastcall FormResize(TObject *Sender);
        void __fastcall StringGridEditListDrawCell(TObject *Sender,
          int ACol, int ARow, TRect &Rect, TGridDrawState State);
private:	// User declarations
 int LeftColOld,minWidth,//�����. ������ ���� �� �������� ������ � ��������
  Hi, //������ � �������� ������ ������������
  Hfh,//������ � �������� FullHead
  Hht,//������ � �������� ������� TabControl
  HSh,//������ � �������� ������ �����
  HSg,//������ � �������� StringGrid
  Hhc,//������ � �������� HeaderControl
  Hsb,//������ � �������� StatusBar
  Hp; //������ � �������� Podval
 WORD KEY;
 TColor ColorPhonEdit,ColorPhonView,ColorPhonFixEdit,ColorPhonFixView,
  ColorTextEdit,ColorTextView;
public:		// User declarations
 int VvodEnter,KRep;
 String WhatFind,RepText;
      __fastcall TFormEditList(TComponent* Owner);
 void __fastcall CalcNewSums(bool ReCalc);
 void __fastcall PrintStringGrid(TPrinter *Prn,int L,int H,
         int MarginLeft,int &Y,int *Li,int NN,int NK,int SN,
         int SK,double Ktw);
 void __fastcall DrawStringGrid(TCanvas *Canvas,int L,int H,
         int MarginLeft,int &Y,int *Li,int NN,int NK,int SN,int SK,
         double Kx,double Ky,Magazine *Ms);
 void __fastcall PrintHeaderControl(TPrinter *Prn,int L,int H,
         int MarginLeft,int &Y,int *Li,int NN,int NK,double Ktw);
 void __fastcall DrawHeaderControl(TCanvas *Canvas,int L,int H,int MarginLeft,
  int &Y,int *Li,int NN,int NK,double Kx,double Ky,Magazine *Ms);
 void __fastcall ReadColorOpt(void);
 bool __fastcall SaveColorOpt(void);
 TColor  GetColorPhonFixView() {return ColorPhonFixView;};
 TColor  GetColorPhonFixEdit() {return ColorPhonFixEdit;};
 TColor  GetColorTextEdit() {return ColorTextEdit;};
 TColor  GetColorTextView() {return ColorTextView;};
 bool __fastcall NoClose();
 void __fastcall BackSort();
};
//---------------------------------------------------------------------------
extern PACKAGE TFormEditList *FormEditList;
void NewSumma(double OldD,double NewD,int Col);
void NewCol(double OldD,double NewD,int Col,int Row);
//---------------------------------------------------------------------------
struct TTable{ void *r; int N; String *S; };
//---------------------------------------------------------------------------
#endif
