//---------------------------------------------------------------------------
#include <vcl.h>
#include <io.h>
#pragma hdrstop

#include "ConvDW.h"
#include "TablW.h"
#include "EditMag.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "CSPIN"
#pragma resource "*.dfm"
TFormEditMag *FormEditMag;
extern char Inf[];
//---------------------------------------------------------------------------
__fastcall TFormEditMag::TFormEditMag(TComponent* Owner):TForm(Owner){}
//---------------------------------------------------------------------------
void __fastcall TFormEditMag::FormShow(TObject *Sender){
 Memo->Clear();
 if(DOS){
  Magazine *M=FileToMagazin(File);
  for(Magazine *m=M;m;m=m->Sled) Memo->Lines->Add(m->S);
  DelAllMagazine(&M);
 }
 else
  Memo->Lines->LoadFromFile(File);
 Memo->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TFormEditMag::FormClose(TObject *Sender,TCloseAction &Action){
 if(DOS){
  for(int i=0;i<Memo->Lines->Count;i++){
   strcpy(Inf,Memo->Lines->Strings[i].c_str());
   StrWINtoDOS(Inf);
   Memo->Lines->Strings[i]=Inf;
 }}
 if(Memo->Lines->Count) Memo->Lines->SaveToFile(File);
}
//---------------------------------------------------------------------------
void __fastcall TFormEditMag::FormCreate(TObject *Sender){
 ComboBoxFont->Items->Add("Arial");
 ComboBoxFont->Items->Add("Courier New"); ComboBoxFont->Text="Courier New";
 ComboBoxFont->Items->Add("MS Sans Serif");
 ComboBoxFont->Items->Add("Times New Roman");
 Memo->Font->Size=CSpinEditSize->Value;
 Memo->Font->Name=ComboBoxFont->Text;
}
//---------------------------------------------------------------------------
void __fastcall TFormEditMag::ComboBoxFontSelect(TObject *Sender){
 Memo->Font->Name=ComboBoxFont->Text;
}
//---------------------------------------------------------------------------
void __fastcall TFormEditMag::CSpinEditSizeChange(TObject *Sender){
 Memo->Font->Size=CSpinEditSize->Value;
}
//---------------------------------------------------------------------------

