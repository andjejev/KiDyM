//---------------------------------------------------------------------------

#ifndef KinemaH
#define KinemaH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include "CA.h"
#include "TablW.h"
#include <ExtCtrls.hpp>
#include <Menus.hpp>
#include <Dialogs.hpp>
#include <Grids.hpp>
#include <ImgList.hpp>
#include <ToolWin.hpp>
#include <Mask.hpp>
#include <printers.hpp>
#include "SHDocVw_OCX.h"
#include <OleCtrls.hpp>
#include "CGAUGES.h"
#include <System.ImageList.hpp>
//---------------------------------------------------------------------------
class TFormKinema : public TForm{
__published:	// IDE-managed Components
  TMainMenu *MainMenu;
  TMenuItem *MenuOpt;
  TColorDialog *ColorDialog;
  TMenuItem *mColors;
  TMenuItem *mCountTabl;
  TMenuItem *mFont;
  TGroupBox *GroupBox;
  TButton *ButtonOKCountTab;
  TFontDialog *FontDialog;
  TImageList *ImageList;
  TToolButton *ToolButtonToBMP;
  TToolButton *ToolButtonPrint;
  TToolButton *ToolButtonTable;
  TToolButton *ToolButton1;
  TToolButton *ToolButtonHelp;
  TToolButton *ToolButtonExit;
  TToolButton *ToolButton2;
  TToolButton *ToolButtonAbout;
  TUpDown *UpDownCountTab;
  TEdit *EditCountTab;
  TToolButton *ToolButtonCalculator;
  TToolButton *ToolButtonRead;
  TToolButton *ToolButtonGame;
  TTimer *TimerGames;
  TButton *ButtonCancel;
  TToolButton *ToolButton3;
  TToolButton *ToolButtonInclude;
  TPopupMenu *PopupMenu;
  TMenuItem *pmColors;
  TMenuItem *pmDiskret;
  TMenuItem *pmFont;
  TUpDown *UpDownKTime;
  TMenuItem *pmOtchet;
  TMenuItem *pmToOtch;
  TMenuItem *pmToBMP;
  TMenuItem *pmToTabl;
  TMenuItem *pmGame;
  TMenuItem *pmValue;
  TMenuItem *pmPrint;
  TMenuItem *pmCalc;
  TMenuItem *pmHelp;
  TMenuItem *pmAbout;
  TMenuItem *pmExit;
  TTimer *Timer3D;
  TCppWebBrowser *CppWebBrowser;
  TMenuItem *MReport;
  TMenuItem *MDiagnos;
  TPanel *Panel;
  TStatusBar *StatusBar;
  TCGauge *CGauge;
  TMenuItem *mWidthPen;
  TMenuItem *mMultTime;
  TMenuItem *mKadrProSec;
  TMenuItem *mStroboSkop;
  TMenuItem *MMultFilms;
  TMenuItem *mMultFilm0;
  TMenuItem *mMultFilm1;
  TMenuItem *mMultFilm2;
  TMenuItem *mMultFilm3;
  TMenuItem *mMultFilm4;
  TMenuItem *mMultFilm5;
  TMenuItem *mMultFilm6;
  TMenuItem *mMultFilm7;
  TMenuItem *mMultFilm8;
  TMenuItem *mMultFilm9;
  TTimer *TimerMultFilm;
  TMenuItem *MOperators;
  TToolButton *tbSpline;
  TToolBar *ToolBar;
  TToolButton *tbFurie;
  TMenuItem *pmSpline;
  TMenuItem *pmFurie;
  TMenuItem *mPrintTabl;
  TButton *ButtonMax;
  TMenuItem *mKBMP;
  TMenuItem *mToJPEG;
  TMenuItem *mToBMP;
  TRadioGroup *RadioGroupSec;
  TMenuItem *mMarksInRow;
  TTimer *TimerModel;
  TMenuItem *mPrintOp;
  TGroupBox *GroupBoxEps;
  TEdit *EditEps;
  TButton *ButtonEpsOK;
  TButton *ButtonEpsCancel;
  TMenuItem *mEps;
  TMenuItem *mEquaScal;
  TUpDown *UpDownEps;
  TMenuItem *mPrintTopRight;
  TMenuItem *mPrintHead;
  TMenuItem *mKColumn;
  TToolButton *ToolButtonRot;
  TTimer *TimerArea;
  TImage *ImageGraph;
  TImage *PhonImage;
  TMenuItem *mRenovCalc;
  TMenuItem *mToProc;
  TTabControl *TabControl;
  TRichEdit *RichEditInp;
        TMenuItem *mToProcKoordPoint;
        TMenuItem *mCreateProcIdentSizes;
        TMenuItem *mCreateProcIdentNewtons;
  void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
  void __fastcall mColorsClick(TObject *Sender);
  void __fastcall mCountTablClick(TObject *Sender);
  void __fastcall ButtonOKCountTabClick(TObject *Sender);
  void __fastcall mFontClick(TObject *Sender);
  void __fastcall TabControlChange(TObject *Sender);
  void __fastcall FontDialogApply(TObject *Sender, HWND Wnd);
  void __fastcall ToolButtonTableClick(TObject *Sender);
  void __fastcall ToolButtonPrintClick(TObject *Sender);
  void __fastcall ToolButtonToBMPClick(TObject *Sender);
  void __fastcall ToolButtonExitClick(TObject *Sender);
  void __fastcall ToolButtonHelpClick(TObject *Sender);
  void __fastcall FormKeyUp(TObject *Sender, WORD &Key,TShiftState Shift);
  void __fastcall ToolButtonAboutClick(TObject *Sender);
  void __fastcall EditCountTabExit(TObject *Sender);
  void __fastcall EditCountTabKeyDown(TObject *Sender,WORD &Key,TShiftState Shift);
  void __fastcall EditCountTabKeyPress(TObject *Sender, wchar_t &Key);
  void __fastcall FormPaint(TObject *Sender);
  void __fastcall ToolButtonGameClick(TObject *Sender);
  void __fastcall ToolButtonCalculatorClick(TObject *Sender);
  void __fastcall ToolButtonReadClick(TObject *Sender);
  void __fastcall TimerGamesTimer(TObject *Sender);
  void __fastcall ButtonCancelClick(TObject *Sender);
  void __fastcall OtchetClick(TObject *Sender);
  void __fastcall ToolButtonIncludeClick(TObject *Sender);
  void __fastcall UpDownKTimeClick(TObject *Sender,TUDBtnType Button);
  void __fastcall PopupMenuPopup(TObject *Sender);
  void __fastcall FormCreate(TObject *Sender);
  void __fastcall Timer3DTimer(TObject *Sender);
  void __fastcall MReportClick(TObject *Sender);
  void __fastcall MDiagnosClick(TObject *Sender);
  void __fastcall mWidthPenClick(TObject *Sender);
  void __fastcall mMultTimeClick(TObject *Sender);
  void __fastcall mKadrProSecClick(TObject *Sender);
  void __fastcall mStroboSkopClick(TObject *Sender);
  void __fastcall mMultFilm0Click(TObject *Sender);
  void __fastcall tbSplineClick(TObject *Sender);
  void __fastcall tbFurieClick(TObject *Sender);
  void __fastcall mPrintTablClick(TObject *Sender);
  void __fastcall ButtonMaxClick(TObject *Sender);
  void __fastcall mKBMPClick(TObject *Sender);
  void __fastcall mToJPEGClick(TObject *Sender);
  void __fastcall mToBMPClick(TObject *Sender);
  void __fastcall TabControlDrawTab(TCustomTabControl *Control,
          int TabIndex, const TRect &Rect, bool Active);
  void __fastcall mMarksInRowClick(TObject *Sender);
  void __fastcall TimerModelTimer(TObject *Sender);
  void __fastcall FormResize(TObject *Sender);
  void __fastcall mPrintOpClick(TObject *Sender);
  void __fastcall MOperatorsClick(TObject *Sender);
  void __fastcall ButtonEpsOKClick(TObject *Sender);
  void __fastcall ButtonEpsCancelClick(TObject *Sender);
  void __fastcall mEpsClick(TObject *Sender);
  void __fastcall mEquaScalClick(TObject *Sender);
  void __fastcall mPrintTopRightClick(TObject *Sender);
  void __fastcall mPrintHeadClick(TObject *Sender);
  void __fastcall MouseDown(TObject *Sender,
   TMouseButton Button, TShiftState Shift, int X, int Y);
  void __fastcall MouseMove(TObject *Sender,TShiftState Shift, int X, int Y);
  void __fastcall MouseUp(TObject *Sender,TMouseButton Button,
   TShiftState Shift,int X,int Y);
  void __fastcall TimerAreaTimer(TObject *Sender);
  void __fastcall mKColumnClick(TObject *Sender);
  bool __fastcall ReadRezFile(wchar_t *File);
  void __fastcall SaveRezFile(wchar_t *File);
  bool __fastcall LoadRezFile(wchar_t *File);
  void __fastcall ImageGraphMouseDown(TObject *Sender,
   TMouseButton Button, TShiftState Shift, int X, int Y);
  void __fastcall mRenovCalcClick(TObject *Sender);
  void __fastcall mToProcClick(TObject *Sender);
  void __fastcall FormShow(TObject *Sender);
		void __fastcall mToProcKoordPointClick(TObject *Sender);
		void __fastcall mCreateProcIdentSizesClick(TObject *Sender);
		void __fastcall mCreateProcIdentNewtonsClick(TObject *Sender);
private:	// User declarations
 TImageKDM *ImageKDM;
 TPaintBox *DataKDMPaintBox; TScrollBox *ScrollBox; int H,W;
 bool GodoGraf; int xt,yt,zt;//координаты и для проигрыша
 double xdt,ydt,zdt,tt,dtt;//координаты и время для проигрыша
 bool ChangeOpt,Gamer,Reader,ColorPrint,ColorBMP,Perspect;
 String NameFontEd,NameFont,Razm[2];
 int SizeFontEd,ExFontEd,ItFontEd,UnFontEd,SizeFont,ExFont,ItFont,UnFont,
  N,KG,NORM,
  Lo[2], //длина оси
  KS[2],KSx,KSy,Kt,// число пикселей по X и Y в кадре, годографе
  Xk,Yk, //к-ты левого нижнего угла рамки графика в пикселах
  MaxOs[2],n, //максимальные числа пикселей по горизонтали и вертикали экрана
  PoleLeft,PoleRight,PoleTop,PoleBottom,//поля устройства
  Ht,Hh,Hy,h,//высота таблицы, названия, названия оси Y,строки,
  Hg,Hc,Hx,//высота графика, чисел по оси X, названия оси X,
  Wo,Wu;//ширина отступа слева, графика, отступа справа
 double *F,*Y,*y,*X,*x,Xo,Yo,dx,*f,XK,X0,Diag,
  xk,yk[2],cFi,sFi,cPsi,sPsi,cTeta,sTeta,
  Fi,Psi,Teta,c11,c12,c13,c21,c22,c23,c31,c32,c33;
//границы диапазона изменения времени, масштаб,
//мини-, максимум, значения функции на левой и на правой
//границах сетки графика функции,
//координаты начала кадра в абсолютной системе координат
 TPrinter *Prntr;
public:		// User declarations
 String Head,Name,NameZ,NameY,NameX,RazmX,RazmY;
 TColor ColorHead,ColorNameOs,ColorTabl,ColorGrid,ColorCifrGrid,
  ColorRamka,ColorGraph,ColorPhonEd,ColorFontEd,ColorPhon,ColorFont,
  ColorGraph2,ColorGraph3,ColorGraph4,ColorGraph5;
 bool flag;
 int x_start, x_end, y_start, y_end,  checkX, checkY;
 double teta, fi;
 int CountTabl;
   double Eps1,Eps2,p0,p1,p2,p3,XC,YC,ZC,xA,xB,yA,yB,XA,YA,XB,YB;
		__fastcall TFormKinema(TComponent* Owner);
   bool __fastcall Kinemat (void);
   void __fastcall OptToFile();
   void __fastcall FileToOpt();
   bool __fastcall DrawGraf(TCanvas *Canvas);
   void __fastcall SravnGraf(TCanvas *Canvas,wchar_t *Name);
//   void __fastcall Metrica(TCanvas *Canvas);
   void __fastcall Metrica(TCanvas *Canvas,Magazine *Names);
   void __fastcall DrawHead(TCanvas *Canvas,String Head);
   void __fastcall DrawTabl(TCanvas *Canvas,Magazine *Names);
   void __fastcall DrawSetka(TCanvas *Canvas,Magazine *Names);
   bool __fastcall DefMSTGodo();
   bool __fastcall DefMSTGodo3D();
   bool __fastcall DefMSTGraf();
   bool __fastcall DefMSTGraf(Magazine *Names);
   void __fastcall DrawGrafik(TCanvas *Canvas,Magazine *Names);
   void __fastcall DrawGodograf(TCanvas *Canvas);
   void __fastcall GrafFunc(TCanvas *Canvas);
   void __fastcall Godograph(TCanvas *Canvas);
   void __fastcall Godograph3D(TCanvas *Canvas);
   void __fastcall PrepDraw3D(TCanvas *Canvas);
   void __fastcall DrawCurve3D(TCanvas *Canvas,TColor Color);
 double __fastcall ToScreen(double x,double y,double z,int &xe,int &ye);
   void __fastcall DrawBar3D(TCanvas *Canvas,bool Color);
   void __fastcall DrawArea(TCanvas *Canvas,bool Color);
   void __fastcall DrawVectors(TCanvas *Canvas,bool Color);
   void __fastcall FuncXYZ(double t,double &x,double &y,double &z);
 double __fastcall FuncXYZ(double x,double y);
   void __fastcall SravnFunc(TCanvas *Canvas,Magazine *Names);
   void __fastcall FindPoint(int x,int y,int &i,int &j);
   void __fastcall DrawCross(int i,int j);
   void __fastcall SelectSizeFont(TCanvas *Canvas);
   void __fastcall ColorGraphik(TCanvas *Canvas,int x0,int y0,
    int x,int y,int LPx,int LPy,double &l,int i);
   void __fastcall VarsReplaceConst();
   void __fastcall PaintBoxPaint(TObject *Sender);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormKinema *FormKinema;
//---------------------------------------------------------------------------
class Square {
 public:
  Square *Sled;
  int Count;
  TPoint V[5]; //вершины с экранными абсциссой и ординатой
// Zce экранная аппликата центральной точки для сортировки,
// Z - абсолютная аппликата центральной точки для цвета
  double Zce,Z;
  void Fill(double Xl,double Yn,double Xp,double Yv);
};
#endif
