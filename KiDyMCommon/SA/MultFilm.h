//---------------------------------------------------------------------------
#ifndef MultFilmH
#define MultFilmH

#define LimitMultFilms 10

#include <vcl.h>
#include <CGAUGES.h>
#include "TablW.h"
#include "ConvDW.h"
#include "CA.h"
#include "SHDocVw_OCX.h"
#include <OleCtrls.hpp>
#include "Termometr.h"
//---------------------------------------------------------------------------
struct Centroid{
 public:
  Centroid *Sled;
  wchar_t *Name;             //имя
  Form FVxA,FVyA,FWfi;    //формулы для проекций скорости полюса и угловой тела
  int Kt;                 //число точек
  double *XP,*YP,*xp,*yp; //координаты МЦС абсолютные и относительные
};

int FindCentroid(Centroid *Rout,wchar_t *S,Centroid **M);
Centroid *TakeCentroid(Centroid **Rout,wchar_t *S,int K);

struct Trajectory{
 public:
  Trajectory *Sled;
  wchar_t *Name;   //имя
  int Kt,*X,*Y; //число точек и координаты точек на экране
  double *x,*y; //координаты точек в экранной СК
  TColor *C;
};

int FindTrajectory(Centroid *Rout,wchar_t *S,Trajectory **M);
Trajectory *TakeTrajectory(Trajectory **Rout,wchar_t *S,int K);

class TMultFilms{
 public:
  TMenuItem *MMFs,*mMFi[LimitMultFilms];
  Magazine *HeadMult,*NamesVarys; Spln *Rsp;
  double *Vals,Min[2],Max[2],Slv[2],Spr[2],MSt[2],MstT[2],t,dt,
   *q0,*Vq0,*z0,*w0,Tn,Tk,xk,yk,MG[10];
  int *WidthPen,Kt,PenWidth,FontSize,
   Xk,Yk,//к-ты левого нижнего угла рамки графика в пикселах
   Sg,//шаг сетки в мм
   KP[2],// число пунктирных линий по X и Y
   dS[2],// число пикселей в шаге сетки
   Hh,Hy,Hs,//заголовка, названия оси Y, высота строки
   Wg,Hg,Hc,Hx,//ширина, высота графика, чисел по оси X, названия оси X,
   Wo,Wu;//ширина отступа слева, отступа справа
  TImage *Image,*PhonImage; Graphics::TBitmap *BitMap;
  TPaintBox *AnimaPaintBox,*PhonPaintBox;
  TCGauge *CGauge; TTermometr *Termometr;
  TColor *ColorGraph,*ColorHead,*ColorPhon,*ColorGrid,*ColorCifrGrid,
   *ColorNameOs,*ColorRamka,FontColor,BrushColor,PenColor;
  TStatusBar *StatusBar; Form *DsetaT;
  bool *NoCalc,*IsNoCalc,*SHLEIF,*TOBMP,*TOJPEG;
  TTimer *TimerMultFilm;
  TCppWebBrowser *CppWebBrowser;
  TToolButton *TB_Table,*TB_ToBMP,*TB_Print,*ToolButtonInclude,*tbFormula,
   *ToolButtonRead;
  void *CalcQ;
  String Head; wchar_t NameY[256],NameX[256];
  TMultFilms(TMenuItem *MMF,TMenuItem *mMF0,TMenuItem *mMF1,
   TMenuItem *mMF2,TMenuItem *mMF3,TMenuItem *mMF4,TMenuItem *mMF5,
   TMenuItem *mMF6,TMenuItem *mMF7,TMenuItem *mMF8,TMenuItem *mMF9,
   int *WP,long *Tm,int *Kk,
   TImage *Im,
//   TPaintBox *Im,
   TImage *PIm,
   TCGauge *CG,
   TColor *clG,TColor *clH,TColor *clP,TColor *clS,TColor *clC,TColor *clN,
   TColor *clR,TStatusBar *SB,
   double tn,double tend,double *qo,double *Vqo,double *zo,double *wo,double *mg,
   Form *DT,
   bool *NC,bool *IsNC,bool *Shlf,TTimer *TimerMF,TCppWebBrowser *CppWB,
   TToolButton *Table,TToolButton *ToBMP,TToolButton *Print,
   TToolButton *TBI,TToolButton *TBF,TToolButton *TBR,TToolButton *TBG,
   bool *ToJpg,bool *ToBmp);
  void  __fastcall DefMSTMultFilm(TCanvas *Canvas);
  void  __fastcall DrawKadr(TCanvas *Canvas);
  void  __fastcall DrawPhon(TCanvas *Canvas);
  void  __fastcall PrepMultFilm(TImage *Image);
  void  __fastcall TimerMultFilmTimer(TObject *Sender);
  void  __fastcall mMFClick(TObject *Sender);
  void  __fastcall Metrica(TCanvas *Canvas);
  void  __fastcall DrawHead(TCanvas *Canvas);
  void  __fastcall DrawSetka(TCanvas *Canvas);

  void  __fastcall TextTest(Root *Tv,Parm *p,Magazine *m);
  void  __fastcall PointTest(Root *Tv,Parm *p,Magazine *m);
  void  __fastcall ScrewRackTest(Root *Tv,Parm *p,Magazine *m);
  void  __fastcall LineTest(Root *Tv,Parm *p,Magazine *m);
  void  __fastcall FingerTest(Root *Tv,Parm *p,Magazine *m);
  void  __fastcall KinParTest(Root *Tv,Parm *p,Magazine *m,int That);
  void  __fastcall SpeedTest(Root *Tv,Parm *p,Magazine *m);
  void  __fastcall AtangTest(Root *Tv,Parm *p,Magazine *m);
  void  __fastcall AnormTest(Root *Tv,Parm *p,Magazine *m);
  void  __fastcall CircleTest(Root *Tv,Parm *p,Magazine *m);
  void  __fastcall Circle3dTest(Root *Tv,Parm *p,Magazine *m);
  void  __fastcall EllipsTest(Root *Tv,Parm *p,Magazine *m);
  void  __fastcall QuadrantTest(Root *Tv,Parm *p,Magazine *m);
  void  __fastcall StringTest(Root *Tv,Parm *p,Magazine *m);
  void  __fastcall BarTest(Root *Tv,Parm *p,Magazine *m);
  void  __fastcall TriAnglTest(Root *Tv,Parm *p,Magazine *m);
  void  __fastcall ArcTest(Root *Tv,Parm *p,Magazine *m);
  void  __fastcall ArcEllipsTest(Root *Tv,Parm *p,Magazine *m);
  void  __fastcall SpringTest(Root *Tv,Parm *p,Magazine *m);
  void  __fastcall HingeTest(Root *Tv,Parm *p,Magazine *m);
  void  __fastcall MultiLinkTest(Root *Tv,Parm *p,Magazine *m);
  void  __fastcall Polygon3dTest(Root *Tv,Parm *p,Magazine *m);
  void  __fastcall CentroidsTest(Root *Tv,Parm *p,Magazine *m);
  void  __fastcall SplineTest(Root *Tv,Parm *p,Magazine *m);
  void  __fastcall TraectorTest(Root *Tv,Parm *p,Magazine *m);
  void  __fastcall Traector3dTest(Root *Tv,Parm *p,Magazine *m);
  void  __fastcall KolesoTest(Root *Tv,Parm *p,Magazine *m);
  void  __fastcall VorotTest(Root *Tv,Parm *p,Magazine *m);
  void  __fastcall GateWithLoadTest(Root *Tv,Parm *p,Magazine *m);
  void  __fastcall GearTest(Root *Tv,Parm *p,Magazine *m);
  void  __fastcall Manipulator3dTest(Root *Tv,Parm *p,Magazine *m);
  
  Parm * __fastcall GabaritPoint(Parm *P);
  Parm * __fastcall GabaritRejka(Parm *P);
  Parm * __fastcall GabaritLine(Parm *P);
  Parm * __fastcall GabaritVector(Parm *P);
  Parm * __fastcall GabaritFinger(Parm *P);
  Parm * __fastcall GabaritSpeed(Parm *P);
  Parm * __fastcall GabaritAtang(Parm *P);
  Parm * __fastcall GabaritAnorm(Parm *P);
  Parm * __fastcall GabaritCircle(Parm *P);
  Parm * __fastcall GabaritCircle3d(Parm *P);
  Parm * __fastcall GabaritEllips(Parm *P);
  Parm * __fastcall GabaritSector(Parm *P);
  Parm * __fastcall GabaritBar(Parm *P);
  Parm * __fastcall GabaritTriangle(Parm *P);
  Parm * __fastcall GabaritArc(Parm *P);
  Parm * __fastcall GabaritArcEllips(Parm *P);
  Parm * __fastcall GabaritString(Parm *P);
  Parm * __fastcall GabaritSpring(Parm *P);
  Parm * __fastcall GabaritHinge(Parm *P,bool Living);
  Parm * __fastcall GabaritMultiLink(Parm *P);
  Parm * __fastcall GabaritPolygon3d(Parm *P);
  Parm * __fastcall GabaritCentroids(Parm *P);
  Parm * __fastcall GabaritSpline(Parm *P);
  Parm * __fastcall GabaritTrajectory(Parm *P);
  Parm * __fastcall GabaritTrajectory3d(Parm *P);
  Parm * __fastcall GabaritKoleso(Parm *P);
  Parm * __fastcall GabaritVorot(Parm *P);
  Parm * __fastcall GabaritGateWithLoad(Parm *P);
  Parm * __fastcall GabaritGear(Parm *P);
  Parm *__fastcall GabaritManipulator3d(Parm *P);

  Parm * __fastcall DrawText(TCanvas *Canvas,Parm *P);
  Parm * __fastcall DrawPoint(TCanvas *Canvas,Parm *P);
  Parm * __fastcall DrawRejka(TCanvas *Canvas,Parm *P);
  Parm * __fastcall DrawLine(TCanvas *Canvas,Parm *P);
  Parm * __fastcall DrawVector(TCanvas *Canvas,Parm *P);
  Parm * __fastcall DrawFinger(TCanvas *Canvas,Parm *P);
  Parm * __fastcall DrawSpeed(TCanvas *Canvas,Parm *P);
  Parm * __fastcall DrawAtang(TCanvas *Canvas,Parm *P);
  Parm * __fastcall DrawAnorm(TCanvas *Canvas,Parm *P);
  Parm * __fastcall DrawCircle(TCanvas *Canvas,Parm *P);
  Parm * __fastcall DrawEllips(TCanvas *Canvas,Parm *P);
  Parm * __fastcall DrawBar(TCanvas *Canvas,Parm *P);
  Parm * __fastcall DrawTriangle(TCanvas *Canvas,Parm *P);
  Parm * __fastcall DrawSector(TCanvas *Canvas,Parm *P);
  Parm * __fastcall DrawArc(TCanvas *Canvas,Parm *P);
  Parm * __fastcall DrawArcEllips(TCanvas *Canvas,Parm *P);
  Parm * __fastcall DrawSpring(TCanvas *Canvas,Parm *P);
  Parm * __fastcall DrawString(TCanvas *Canvas,Parm *P);
  Parm * __fastcall DrawHinge(TCanvas *Canvas,Parm *P,bool Living);
  Parm * __fastcall DrawMultiLink(TCanvas *Canvas,Parm *P);
  Parm * __fastcall DrawCircle3d(TCanvas *Canvas,Parm *P);
  Parm * __fastcall DrawPolygon3d(TCanvas *Canvas,Parm *P);
  Parm * __fastcall DrawFixedCentroids(TCanvas *Canvas,Parm *P);
  Parm * __fastcall DrawMobileCentroids(TCanvas *Canvas,Parm *P);
  Parm * __fastcall DrawSpline(TCanvas *Canvas,Parm *P);
  Parm * __fastcall DrawTrajectory(TCanvas *Canvas,Parm *P);
  Parm * __fastcall DrawTrajectory3d(TCanvas *Canvas,Parm *P);
  Parm * __fastcall DrawKoleso(TCanvas *Canvas,Parm *P);
  Parm * __fastcall DrawVorot(TCanvas *Canvas,Parm *P);
  Parm * __fastcall DrawGateWithLoad(TCanvas *Canvas,Parm *P);
  Parm * __fastcall DrawGear(TCanvas *Canvas,Parm *P);
  Parm *__fastcall DrawManipulator3d(TCanvas *Canvas,Parm *P);
  
  Parm * __fastcall ClearText(TCanvas *Canvas,Parm *P);
  Parm * __fastcall ClearPoint(TCanvas *Canvas,Parm *P);
  Parm * __fastcall ClearRejka(TCanvas *Canvas,Parm *P);
  Parm * __fastcall ClearLine(TCanvas *Canvas,Parm *P);
  Parm * __fastcall ClearVector(TCanvas *Canvas,Parm *P);
  Parm * __fastcall ClearFinger(TCanvas *Canvas,Parm *P);
  Parm * __fastcall ClearCircle(TCanvas *Canvas,Parm *P);
  Parm * __fastcall ClearBar(TCanvas *Canvas,Parm *P);
  Parm * __fastcall ClearTriangle(TCanvas *Canvas,Parm *P);
  Parm * __fastcall ClearSector(TCanvas *Canvas,Parm *P);
  Parm * __fastcall ClearArc(TCanvas *Canvas,Parm *P);
  Parm * __fastcall ClearSpring(TCanvas *Canvas,Parm *P);
  Parm * __fastcall ClearFixHinge(TCanvas *Canvas,Parm *P);
  Parm * __fastcall ClearKoleso(TCanvas *Canvas,Parm *P);
  Parm * __fastcall ClearVorot(TCanvas *Canvas,Parm *P);
  Parm * __fastcall ClearGear(TCanvas *Canvas,Parm *P);

  Parm * __fastcall ParmText(Parm *P,int &X,int &Y,int &Size,TColor &Color,wchar_t **T);
  Parm * __fastcall ParmPoint(Parm *P,int &X,int &Y,TColor &Color);
  Parm * __fastcall ParmPoint4(Parm *P,int &X,int &Y,int &W,TColor &Color);
  Parm *__fastcall TMultFilms::ParmPoint5(Parm *P,wchar_t *Name,
   int &X,int &Y,int &W,TColor &Color);
  Parm * __fastcall ParmRejka(Parm *P,double &xA,double &yA,double &a1,
   double &xB,double &yB,double &a2,int &Width,TColor &Color);
  Parm * __fastcall ParmLine(Parm *P,int &XA,int &YA,int &XB,int &YB,
   int &Width,TColor &Color);
  Parm * __fastcall ParmVector(Parm *P,int &XA,int &YA,int &XB,int &YB,
   int &Width,TColor &Color);
  Parm * __fastcall ParmFinger(Parm *P,int &XA,int &YA,int &XB,int &YB,
   int &Width,TColor &Color);
  Parm * __fastcall ParmSpeed(Parm *P,int &XA,int &YA,int &XB,int &YB,
   int &Width,TColor &Color);
  Parm * __fastcall ParmAtang(Parm *P,int &XA,int &YA,int &XB,int &YB,
   int &Width,TColor &Color);
  Parm * __fastcall ParmAnorm(Parm *P,int &XA,int &YA,int &XB,int &YB,
   int &Width,TColor &Color);
  Parm * __fastcall ParmCircle(Parm *P,int &X,int &Y,int &R,int &Width,
   TColor &ColorK,TColor &ColorF);
  Parm * __fastcall ParmEllips(Parm *P,int &X,int &Y,int &a,int &b,int &Width,
   TColor &ColorK,TColor &ColorF);
  Parm * __fastcall ParmBar(Parm *P,TPoint *tP,int &Width,TColor &ColorL,
   TColor &ColorF);
  TPoint *__fastcall ParmCircle3d(Parm *P,int &Width,
   TColor &ColorK,TColor &ColorF,int K);
  TPoint *__fastcall ParmPolygon3d(Parm *P,int &Width,
   TColor &ColorL,TColor &ColorF,int &K);
  Parm * __fastcall ParmTriangle(Parm *P,TPoint *tP,int &Width,TColor &ColorL,
   TColor &ColorF);
  Parm * __fastcall ParmSector(Parm *P,int &X1,int &Y1,int &X2,int &Y2,int &X3,
   int &Y3,int &X4,int &Y4,int &Width,TColor &cL,TColor &cF);
  Parm * __fastcall ParmArc(Parm *P,int &X1,int &Y1,int &X2,int &Y2,int &X3,
   int &Y3,int &X4,int &Y4,int &Width,TColor &cL);
  Parm * __fastcall ParmArcEllips(Parm *P,int &X1,int &Y1,int &X2,int &Y2,int &X3,
   int &Y3,int &X4,int &Y4,int &Width,TColor &cL);
  Parm * __fastcall ParmSpring(Parm *P,double &xA,double &yA,double &xB,
   double &yB,double &d,int &Width,TColor &Color,int &N);
  Parm * __fastcall ParmString(Parm *P,int &XA,int &YA,int &XB,int &YB,
   int &R,int &Width,TColor &Color,TColor &ColorKrug);
  Parm * __fastcall ParmHinge(Parm *P,int &XO,int &YO,
   int &XA,int &YA,int &XB,int &YB,int &XC,int &YC,int &XD,int &YD,
   int &XE,int &YE,int &XF,int &YF,int &XG,int &YG,int &XH,int &YH,
   int *X,int *Y,int &R,int &H,int &SH,int &Width,TColor &Color,
   TColor &ColorKrug,bool Living);
  Parm * __fastcall ParmKoleso(Parm *P,int &X,int &Y,int &R,double &Fi,int &Width,
   TColor &ColorK,TColor &ColorF);
  Parm * __fastcall ParmVorot(Parm *P,int &X,int &Y,int &R,double &L,
   double &Alfa,double &Fi,int &Width,TColor &ColorK,TColor &ColorF);
  Parm *__fastcall ParmGateWithLoad(Parm *P,int &X,int &Y,int &R,double &Fi,
   double &Al,double &l0,double &h,double &l,int &W,TColor &K,TColor &F,TColor &B);
  Parm * __fastcall ParmGear(Parm *P,int &X1,int &Y1,int &R1,double &Fi1,int &X2,
   int &Y2,int &R2,double &Fi2,int &F,int &Width,TColor &ColorK,TColor &ColorF);
  void   __fastcall SortZ(void);
  double __fastcall ZPolygon3d(Parm *R);
  double __fastcall ZCircle3d(Parm *P);
  void   __fastcall ToBMP(TTabControl *TabControl,bool Message);
  void   __fastcall Include(TTabControl *TabControl);
};

void CalcQSplns();
void TakeDLL();
#endif
