//---------------------------------------------------------------------------

#ifndef TermometrH
#define TermometrH

#include <vcl.h>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TTermometr{
 public:
  int Min,Max,Pos,Stp,Prc,PrcOld,l,W; TRect R,Rt; TColor Color,ForeColor;
  TStatusBar *StatusBar; TImage *Image; AnsiString Text,TextNew;
  TTermometr(TColor Clr,TColor ForeClr,TStatusBar *StBar);
  void StepIn();
 ~TTermometr(){ if(Image) delete Image; }
};
//---------------------------------------------------------------------------
#endif
