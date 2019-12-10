//---------------------------------------------------------------------------

#include <vcl.h>
#include <io.h>
#pragma hdrstop

#include "ConvDW.h"
#include "OtchetForm.h"
#include "EditTextButton.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormEditTextButton *FormEditTextButton;
TEditTextButton *EditTextButton;
extern TFormOtchet *FormOtchet;
extern wchar_t Inf[],WORKDIR[];
//---------------------------------------------------------------------------
__fastcall TFormEditTextButton::TFormEditTextButton(TComponent* Owner):
  TForm(Owner){
}
//---------------------------------------------------------------------------
void __fastcall TFormEditTextButton::FormShow(TObject *Sender){
 if(!EditTextButton->Text.IsEmpty())//если текст не пустой
  RichEditText->Text=EditTextButton->Text;
 else
  if(EditTextButton->NameFile[0]){
   EditTextButton->LoadDosFile();
}}
//---------------------------------------------------------------------------
void TEditTextButton::LoadDosFile(){
 FILE *F; wchar_t S[2048],*s;
 FormEditTextButton->RichEditText->Clear();
 if(!_waccess(NameFile,0)&&(F=_wfopen(NameFile,L"r"))){
  while(Fgets(S,2047,F)){
   if(s=wcsrchr(S,'\n')) *s='\0';
   FormEditTextButton->RichEditText->Lines->Add(S);
  }
  fclose(F);
 }
 else {
  swprintf(Inf,L"Не открылся файл \"%s\" для редактирования",NameFile);
  Application->MessageBox(
    L"Если не будете его формировать вручную,\nзакройте форму",
   Inf,MB_OK|MB_ICONQUESTION);
}}
//---------------------------------------------------------------------------
void __fastcall TFormEditTextButton::FormClose(TObject *Sender,
      TCloseAction &Action){ //
 if(RichEditText->Modified) EditTextButton->SaveDosFile();
}
//---------------------------------------------------------------------------
void __fastcall TFormEditTextButton::ToolButtonHelpClick(TObject *Sender){
 wchar_t Help[256],*s; wcscpy(Help,WORKDIR);
/* if(s=wcsrchr(Help,'\\')) wcscpy(s+1,L"Help\\EditTextButton.htm");
 else*/ wcscat(Help,L"\\Help\\EditTextButton.htm");
 if(_waccess(Help,0)){
  swprintf(Inf,L"Не найден файл помощи\n\"%s\"",Help);
  Application->MessageBox(Inf,L"Системная ошибка!",MB_OK);
  return;
 }
 if(!FormOtchet)
  Application->CreateForm(__classid(TFormOtchet),&FormOtchet);
 FormOtchet->cbURL->Text=Help;
 FormOtchet->ShowModal();
}
//---------------------------------------------------------------------------
void __fastcall TFormEditTextButton::ToolButtonExitClick(TObject *Sender){
 Close();
}
//---------------------------------------------------------------------------

