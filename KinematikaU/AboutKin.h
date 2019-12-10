//----------------------------------------------------------------------------
#ifndef AboutH
#define AboutH
//----------------------------------------------------------------------------
#include <System.hpp>
#include <Windows.hpp>
#include <SysUtils.hpp>
#include <Classes.hpp>
#include <Graphics.hpp>
#include <Forms.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
//----------------------------------------------------------------------------
class TAboutFormKin : public TForm
{
__published:
		TTimer *Timer;
		TImage *ProgramIcon;
		TImage *BufferIcon;
		TLabel *Version;
		TLabel *Comments;
		TLabel *Label1;
		TLabel *Label2;
		TLabel *HyperLink;
		TLabel *Label4;
		TLabel *Label5;
		TImage *ImageCopy;
		void __fastcall FormShow(TObject *Sender);
		void __fastcall TimerTimer(TObject *Sender);
		void __fastcall HyperLinkClick(TObject *Sender);
		void __fastcall HyperLinkMouseMove(TObject *Sender,
		  TShiftState Shift, int X, int Y);
		void __fastcall FormMouseMove(TObject *Sender, TShiftState Shift,
		  int X, int Y);
private:
public:
 TColor ColorShar,ColorLine;
 double x1,x2,x3,x4,y1,y2,y3,y4,z1,z2,z3,z4,h,l,r,a,b,Fi,Teta,Psi,dt,t;
 int Xm[12],Ym[12],Xc,Yc,W,H;

 virtual __fastcall TAboutFormKin(TComponent* AOwner);
 void __fastcall DrawTetraedr(double Fi,double Psi,double Teta);
 void __fastcall DrawShar(TCanvas *Canvas,int X,int Y);
};
//----------------------------------------------------------------------------
extern PACKAGE TAboutFormKin *AboutFormKin;
//----------------------------------------------------------------------------
#endif
