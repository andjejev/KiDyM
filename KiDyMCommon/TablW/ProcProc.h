//---------------------------------------------------------------------------
#ifndef ProcProcH
#define ProcProcH

#include <vcl.h>
#include "ConvDW.h"

//---------------------------------------------------------------------------
class TProcProc{
  TImage *I; bool VisibleProc; int Min,Max,Step,Val,Proc; TColor Phon,Band;
  char Text[16];
 public:
  TProcProc();
  void Init(TImage *Image,bool ShowProc,int MinVal,int MaxVal,int St,TColor CP,TColor CB);
  void Clear();
  void Clear(int Mx);
  void Clear(int Mx,TColor B);
  void Add();
  void SetMax(int M){ Max=M; }
  void Value(int V);
};
//---------------------------------------------------------------------------
#endif
