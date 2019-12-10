#include <vcl.h>
#pragma hdrstop

#include "MultiSelect.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
#define max(a, b)  (((a) > (b)) ? (a) : (b))
TFormMultiSelect *FormMultiSelect;
//---------------------------------------------------------------------------
__fastcall TFormMultiSelect::TFormMultiSelect(TComponent* Owner):TForm(Owner){
}
//---------------------------------------------------------------------------
void __fastcall TFormMultiSelect::ButtonCancelClick(TObject *Sender){
 for(int i=0;i<CheckListBox->Items->Count;i++)
  CheckListBox->State[i]=cbUnchecked;
 FOkCancel=false;
 Close();
}
//---------------------------------------------------------------------------
void __fastcall TFormMultiSelect::ButtonAllClick(TObject *Sender){
//  Выбрать все
 for(int i=0;i<CheckListBox->Items->Count;i++)
  CheckListBox->State[i]=cbChecked;
}
//---------------------------------------------------------------------------
void __fastcall TFormMultiSelect::CheckListBoxKeyDown(TObject *Sender,
      WORD &Key, TShiftState Shift){
 if(Key=='A'&&Shift.Contains(ssCtrl)) ButtonAllClick(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TFormMultiSelect::FormClose(TObject *Sender,
      TCloseAction &Action){
 ButtonCancelClick(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TFormMultiSelect::ButtonOKClick(TObject *Sender){
 FOkCancel=true; Close();
}
//---------------------------------------------------------------------------
void __fastcall TFormMultiSelect::FormShow(TObject *Sender){
 int w,W=0,h=Canvas->TextHeight("M"),H=h*CheckListBox->Items->Count+h/2;
 for(int i=0;i<CheckListBox->Items->Count;i++){
  w=Canvas->TextWidth(CheckListBox->Items->Strings[i]); if(w>W) W=w;
 }
 w=Canvas->TextWidth(ButtonCancel->Caption);
 h=Canvas->TextHeight(ButtonCancel->Caption);
 FOkCancel=false;
 CheckListBox->Left=h/2; CheckListBox->Width=15*W/10;
 CheckListBox->Height=H;
 ButtonCancel->Left=CheckListBox->Left+CheckListBox->Width+h;
 ButtonOK->Left =ButtonCancel->Left;
 ButtonAll->Left=ButtonCancel->Left;
 CheckListBox->Top=h/2;
 ButtonOK->Top=CheckListBox->Top;
 ButtonOK->Width=15*w/10; ButtonOK->Height=15*h/10;
 ButtonCancel->Width=ButtonOK->Width;
 ButtonCancel->Height=ButtonOK->Height;
 ButtonAll->Width=ButtonOK->Width;
 ButtonAll->Height=ButtonOK->Height;
 ButtonCancel->Top=ButtonOK->Top+ButtonOK->Height+h/2;
 ButtonAll->Top=ButtonCancel->Top+ButtonCancel->Height+h/2;
 ClientHeight=h/2+
  max(ButtonAll->Top+ButtonAll->Height,CheckListBox->Top+CheckListBox->Height);
 if(Height>(Screen->Height-60)) Height=Screen->Height-60;
 if(CheckListBox->Top+CheckListBox->Height>ClientHeight-h/2)
  CheckListBox->Height=ClientHeight-h/2-CheckListBox->Top;
 ClientWidth=CheckListBox->Width+ButtonCancel->Width+2*h;
}
//---------------------------------------------------------------------------

