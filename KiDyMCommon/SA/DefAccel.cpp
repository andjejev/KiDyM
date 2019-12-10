//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include <math.h>
#include "DefAccel.h"
//---------------------------------------------------------------------------

#pragma package(smart_init)
extern Cnst *Nul;
extern char Inf[],OprFile[],WORKDIR[]; extern List *L; extern bool IsError;
extern bool OTLADKA; extern Form *DsetaT,*DsetaTT;
extern FILE *OPRFILE;
//---------------------------------------------------------------------------
// Оператор создает список инерционных элементов из списка твердых тел,
//---------------------------------------------------------------------------
bool DefAccel (Parm *Par,TRichEdit *R){
 return true;
}
//---------------------------------------------------------------------------

