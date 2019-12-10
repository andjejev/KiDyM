//---------------------------------------------------------------------------

#ifndef EmblemaKernH
#define EmblemaKernH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TFormKidyMEmblema:public TForm{
__published:	// IDE-managed Components
 TImage *ProgramIcon;
        TImage *BufferIcon;
 TTimer *Timer;
        void __fastcall TimerTimer(TObject *Sender);
        void __fastcall FormShow(TObject *Sender);
        void __fastcall FormKeyPress(TObject *Sender, char &Key);
private:	// User declarations
public:		// User declarations
 TColor ColorShar,ColorLine;
 double x1,x2,x3,x4,y1,y2,y3,y4,z1,z2,z3,z4,h,l,r,a,b,Fi,Teta,Psi,dt,t;
 int Xm[12],Ym[12],Xc,Yc,W,H;

      __fastcall TFormKidyMEmblema(TComponent* Owner);
 void __fastcall DrawTetraedr(double Fi,double Psi,double Teta);
 void __fastcall DrawShar(TCanvas *Canvas,int X,int Y);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormKidyMEmblema *FormKidyMEmblema;
//---------------------------------------------------------------------------
#endif
