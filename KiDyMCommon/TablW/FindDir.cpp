#include <vcl.h>
#pragma hdrstop

#include "FindDir.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormFindDir *FormFindDir;
//---------------------------------------------------------------------------
__fastcall TFormFindDir::TFormFindDir(TComponent* Owner):TForm(Owner){
}
//---------------------------------------------------------------------------
void __fastcall TFormFindDir::ButtonCancelClick(TObject *Sender){
 Select=false;
 Close();
}
//---------------------------------------------------------------------------
void __fastcall TFormFindDir::ButtonOKClick(TObject *Sender){
 Select=true;
 Close();
}
//---------------------------------------------------------------------------

