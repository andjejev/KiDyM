//---------------------------------------------------------------------------
#include <vcl.h>
#include <stdio.h>
#include "ToPrn.h"
#include "ParamPrint.h"
#include <io.h>
#pragma hdrstop
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

TFormToPrn *FormToPrn;
extern TFormParamPrint *FormParamPrint;
extern wchar_t Inf[],WORKDIR[];
wchar_t FileHelp[256];
//---------------------------------------------------------------------------
__fastcall TFormToPrn::TFormToPrn(TComponent* Owner):TForm(Owner){
 MaxPage=FromPage=ToPage=1; Copies=1; Collate=false;
}
//---------------------------------------------------------------------------
void __fastcall TFormToPrn::DrawCollate(){
 int h=ImageCollate->Height/3,d=ImageCollate->Width*2/13;
 ImageCollate->Canvas->Brush->Color=clBtnFace;
 ImageCollate->Canvas->Pen->Width=0;
 ImageCollate->Canvas->Rectangle(0,0,
  ImageCollate->ClientWidth-1,ImageCollate->ClientHeight-1);
 ImageCollate->Canvas->Pen->Width=1;
 ImageCollate->Canvas->Brush->Color=clWhite;
 if(Collate){
  ImageCollate->Canvas->Rectangle(2*d,h/2,3*d,3*h/2);
  ImageCollate->Canvas->TextOut(5*d/2-1,h/2+1,L"3");
  ImageCollate->Canvas->Rectangle(3*d/2,h,5*d/2,2*h);
  ImageCollate->Canvas->TextOut(2*d-1,h+1,L"2");
  ImageCollate->Canvas->Rectangle(d,3*h/2,2*d,5*h/2);
  ImageCollate->Canvas->TextOut(3*d/2-1,3*h/2+1,L"1");
  ImageCollate->Canvas->Rectangle(5*d,h/2,6*d,3*h/2);
  ImageCollate->Canvas->TextOut(11*d/2-1,h/2+1,L"3");
  ImageCollate->Canvas->Rectangle(9*d/2,h,11*d/2,2*h);
  ImageCollate->Canvas->TextOut(5*d-1,h+1,L"2");
  ImageCollate->Canvas->Rectangle(4*d,3*h/2,5*d,5*h/2);
  ImageCollate->Canvas->TextOut(9*d/2-1,3*h/2+1,L"1");
 }
 else {
  ImageCollate->Canvas->Rectangle(d,3*h/4,2*d,7*h/4);
  ImageCollate->Canvas->TextOut(3*d/2-1,3*h/4+1,L"1");
  ImageCollate->Canvas->Rectangle(d/2,5*h/4,3*d/2,9*h/4);
  ImageCollate->Canvas->TextOut(d-1,5*h/4+1,L"1");
  ImageCollate->Canvas->Rectangle(3*d,3*h/4,4*d,7*h/4);
  ImageCollate->Canvas->TextOut(7*d/2-1,3*h/4+1,L"2");
  ImageCollate->Canvas->Rectangle(5*d/2,5*h/4,7*d/2,9*h/4);
  ImageCollate->Canvas->TextOut(3*d-1,5*h/4+1,L"2");
  ImageCollate->Canvas->Rectangle(5*d,3*h/4,6*d,7*h/4);
  ImageCollate->Canvas->TextOut(11*d/2-1,3*h/4+1,L"3");
  ImageCollate->Canvas->Rectangle(9*d/2,5*h/4,11*d/2,9*h/4);
  ImageCollate->Canvas->TextOut(5*d-1,5*h/4+1,L"3");
 }
}
//---------------------------------------------------------------------------
void __fastcall TFormToPrn::FormShow(TObject *Sender){
 FormParamPrint->Defines();
 if(MaxPage==1) {
  EditBegin->Enabled=false; UpDownBegin->Enabled=false;
  EditEnd->Enabled=false;   UpDownEnd->Enabled=false;
 }
 else {
  EditBegin->Enabled=true; UpDownBegin->Enabled=true;
  EditEnd->Enabled=true;   UpDownEnd->Enabled=true;
  UpDownBegin->Max=ToPage;
  UpDownEnd->Max=ToPage;
  UpDownBegin->Position=FromPage;
  UpDownEnd->Position=ToPage;
  swprintf(Inf,L"Все страницы (с %d-й по %d-ю)",FromPage,ToPage);
 }
 UpDownCount->Position=1;
 CheckBoxCollate->Checked=Collate;
 DrawCollate();
}
//---------------------------------------------------------------------------
void __fastcall TFormToPrn::ButtonCancelClick(TObject *Sender){
 Close();
}
//---------------------------------------------------------------------------
void __fastcall TFormToPrn::CheckBoxCollateClick(TObject *Sender){
 Collate=CheckBoxCollate->Checked;
 DrawCollate();
}
//---------------------------------------------------------------------------
void __fastcall TFormToPrn::ButtonHelpClick(TObject *Sender){
 wchar_t *s; AnsiString AS; wcscpy(FileHelp,WORKDIR);
 if(s=wcsrchr(FileHelp,'\\')) wcscpy(s+1,L"Help\\ToPrn.htm");
 else wcscat(FileHelp,L"\\Help\\ToPrn.htm");
 if(!_waccess(FileHelp,0)){
  AS=FileHelp; ShellExecute(NULL,"open",AS.c_str(),NULL,NULL,SW_SHOW); }
 else{
  swprintf(Inf,L"Не найден файл помощи:\n\"%s!\"",FileHelp);
  Application->MessageBox(Inf,L"Внимание!",MB_OK|MB_ICONWARNING);
}}
//---------------------------------------------------------------------------
void __fastcall TFormToPrn::ButtonOkClick(TObject *Sender){
 FromPage=UpDownBegin->Position;
 ToPage=UpDownEnd->Position;
 Copies=UpDownCount->Position;
 ToPrinter();
 ButtonCancelClick(Sender);
}
//---------------------------------------------------------------------------


