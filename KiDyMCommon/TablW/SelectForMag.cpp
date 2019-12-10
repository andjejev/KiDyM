//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "SelectForMag.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormMag *FormMag;
int MaxMag=11;
//---------------------------------------------------------------------------
__fastcall TFormMag::TFormMag(TComponent* Owner):TForm(Owner){
}
//---------------------------------------------------------------------------
void __fastcall TFormMag::FormClose(TObject *Sender, TCloseAction &Action){
 if(!Select) ComboBoxMag->ItemIndex=-1;
}
//---------------------------------------------------------------------------
void __fastcall TFormMag::ComboBoxMagDblClick(TObject *Sender){
 Select=true; Close();
}
//---------------------------------------------------------------------------
void __fastcall TFormMag::ComboBoxMagKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift){
 switch(Key){
  case VK_ESCAPE: Select=false; break;
  case VK_RETURN:
   if(!ComboBoxMag->Text.IsEmpty())
    ComboBoxMag->Items->Add(ComboBoxMag->Text);
   Select=true;  break;
  default: return;
 }
 Close();
}
//---------------------------------------------------------------------------
void __fastcall TFormMag::FormShow(TObject *Sender){
 int H;
 Select=false;
 if(ComboBoxMag->ItemIndex>ComboBoxMag->Items->Count) ComboBoxMag->ItemIndex=0;
/*
 if(ComboBoxMag->Items->Count<MaxMag)
//  ComboBoxMag->Height=
//   6*(ComboBoxMag->Items->Count+2)*ComboBoxMag->Canvas->TextHeight(L"M")/5;
//  H=6*(ComboBoxMag->Items->Count+2)*ComboBoxMag->Canvas->TextHeight(L"M")/5;
  H=(ComboBoxMag->Items->Count+2)*ComboBoxMag->Canvas->TextHeight(L"M");
 else
  H=12*ComboBoxMag->Canvas->TextHeight(L"M");
//  ComboBoxMag->Height=12*ComboBoxMag->Canvas->TextHeight(L"M");
// if(ComboBoxMag->Items->Count<11)
//  H=6*(ComboBoxMag->Items->Count+2)*ComboBoxMag->Canvas->TextHeight(L"M")/5;
// else H=12*ComboBoxMag->Canvas->TextHeight(L"M");
 Height=H+40;
*/
// ComboBoxMag->Height=H;
 ComboBoxMag->Height=this->PanelMag->ClientHeight;
}
//---------------------------------------------------------------------------

