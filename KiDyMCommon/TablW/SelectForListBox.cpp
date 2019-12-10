//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "SelectForListBox.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormMagListBox *FormMagListBox;
//---------------------------------------------------------------------------
__fastcall TFormMagListBox::TFormMagListBox(TComponent* Owner):TForm(Owner){
}
//---------------------------------------------------------------------------
void __fastcall TFormMagListBox::FormClose(TObject *Sender,TCloseAction &Action){
 if(!Select) ListBox->ItemIndex=-1;
}
//---------------------------------------------------------------------------
void __fastcall TFormMagListBox::FormShow(TObject *Sender){
 int w,W=0,h=ListBox->Canvas->TextHeight("M"),I=0;
 Caption="Выбирайте...";
 if(ListBox->ItemIndex>0) I=ListBox->ItemIndex;
 Select=false;
 for(int i=0;i<ListBox->Count;i++){
  w=ListBox->Canvas->TextWidth(FormMagListBox->ListBox->Items->Strings[i]);
  if(w>W) W=w;
 }
 ListBox->ItemIndex=I;
 Width=W+16;
 Height=24+
  (ListBox->Items->Count<11?6*ListBox->Items->Count*h/5+16:Height=12*h);
}
//---------------------------------------------------------------------------
void __fastcall TFormMagListBox::ListBoxDblClick(TObject *Sender){
 Select=true; Close();
}
//---------------------------------------------------------------------------
void __fastcall TFormMagListBox::ListBoxKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift){
 switch(Key){
  case VK_ESCAPE: Select=false; break;
  case VK_RETURN: Select=true;  break;
  default: return;
 }
 Close();
}
//---------------------------------------------------------------------------
