//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "OtchetForm.h"
#include <stdio.h>
#include <io.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormOtchet *FormOtchet;
extern wchar_t WORKDIR[],Inf[]; 
//---------------------------------------------------------------------------
__fastcall TFormOtchet::TFormOtchet(TComponent* Owner):TForm(Owner){}
//---------------------------------------------------------------------------
void __fastcall TFormOtchet::FormShow(TObject *Sender){
 Load();
 CppWebBrowser->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TFormOtchet::Load(){
//�������� URL ��� �����, ��������� � �������� cbURL->Text
// CppWebBrowser->Navigate(WideString(cbURL->Text));
 CppWebBrowser->Navigate(cbURL->Text.w_str());
}
//---------------------------------------------------------------------------
void __fastcall TFormOtchet::cbURLClick(TObject *Sender){
 Load();
 CppWebBrowser->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TFormOtchet::cbURLKeyDown(TObject *Sender, WORD &Key,
 TShiftState Shift){
//�������� ��� ������� Enter �� cbURL
 if(Key==VK_RETURN) Load();
}
//---------------------------------------------------------------------------
void __fastcall TFormOtchet::CppWebBrowserBeforeNavigate2(TObject *Sender,
  LPDISPATCH pDisp, Variant *URL, Variant *Flags,Variant *TargetFrameName,
  Variant *PostData, Variant *Headers,VARIANT_BOOL *Cancel){
//��������� ������� ����� ���������
//���������� ������� cbURL
// int Index=cbURL->Items->IndexOf(AnsiString(*URL));
 int Index=cbURL->Items->IndexOf(*URL);
 if(Index==-1){
//  cbURL->Items->Insert(0,AnsiString(*URL));
  cbURL->Items->Insert(0,*URL);
  cbURL->ItemIndex=0;
 }
 else cbURL->ItemIndex=Index;
 tbForward->Enabled=(cbURL->ItemIndex>0);
 tbBack->Enabled=(cbURL->ItemIndex<cbURL->Items->Count-1);
}
//---------------------------------------------------------------------------
void __fastcall TFormOtchet::CppWebBrowserProgressChange(TObject *Sender,
      long Progress, long ProgressMax){
//��������� ������ � ������ ���������
 if(Progress>0)
  StatusBar->SimpleText=Format(L"�������� %s: ��������� %d ����� �� %d",
   ARRAYOFCONST((AnsiString(CppWebBrowser->LocationName),
   (int)(Progress/1024),(int)(ProgressMax/1024))));
}
//---------------------------------------------------------------------------
void __fastcall TFormOtchet::tbBackClick(TObject *Sender){
//������� �� ���������� ��������
 cbURL->Text=cbURL->Items->Strings[cbURL->ItemIndex+1];
 Load();
 CppWebBrowser->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TFormOtchet::tbForwardClick(TObject *Sender){
//������� �� ��������� ��������
 cbURL->Text=cbURL->Items->Strings[cbURL->ItemIndex-1];
 Load();
 CppWebBrowser->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TFormOtchet::tbOpenClick(TObject *Sender){
//����� ������� �������� �����
 if(OpenDialog->Execute()){
  cbURL->Text=OpenDialog->FileName;
  Load();
  CppWebBrowser->SetFocus();
}}
//---------------------------------------------------------------------------
void __fastcall TFormOtchet::tbExitClick(TObject *Sender){
 Close();
}
//---------------------------------------------------------------------------
void __fastcall TFormOtchet::tbHelpClick(TObject *Sender){
 wchar_t Help[256],*s; wcscpy(Help,WORKDIR);
/* if(s=wcsrchr(Help,'\\')) wcscpy(s+1,L"Help\\BrouserHelp.htm");
 else*/ wcscat(Help,L"\\Help\\BrouserHelp.htm");
 if(_waccess(Help,0)){
  swprintf(Inf,L"�� ������ ���� ������\n\"%s\"",Help);
  Application->MessageBox(Inf,L"��������� ������!",MB_OK);
  return;
 }
 cbURL->Text=Help; Load();
}
//---------------------------------------------------------------------------
void __fastcall TFormOtchet::FormClose(TObject *Sender,TCloseAction &Action){
 cbURL->Items->Clear();
}
//---------------------------------------------------------------------------

