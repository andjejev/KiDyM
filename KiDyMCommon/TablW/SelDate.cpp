//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "SelDate.h"
#include <locale>
#include <stdio.h>

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormDate *FormDate;
extern char Inf[];
//---------------------------------------------------------------------------
__fastcall TFormDate::TFormDate(TComponent* Owner):TForm(Owner){
 Period=false;
}
//---------------------------------------------------------------------------
void __fastcall TFormDate::ButtonCancelDateClick(TObject *Sender){
 if(Period) NYmin=NYmax=NMmin=NMmax=NDmin=NDmax=0;
 else NY=NM=ND=0;
 Close();
}
//---------------------------------------------------------------------------
void __fastcall TFormDate::FormShow(TObject *Sender){
 if(Period){
  GroupBoxDate->Caption=" Выбирайте период ";
  LabelDate->Visible=true; LabelDateEnd->Visible=true;
  DateTimePicker->MinDate=EncodeDate((Word)(NYmin+1900),(Word)NMmin,
   (Word)NDmin);
  DateTimePicker->MaxDate=EncodeDate((Word)(NYmax+1900),(Word)NMmax,
   (Word)NDmax);
  DateTimePicker->Date=EncodeDate((Word)(NYmin+1900),(Word)NMmin,
   (Word)NDmin);
  DateTimePickerEnd->MinDate=EncodeDate((Word)(NYmin+1900),(Word)NMmin,
   (Word)NDmin);
  DateTimePickerEnd->MaxDate=EncodeDate((Word)(NYmax+1900),(Word)NMmax,
   (Word)NDmax);
  DateTimePickerEnd->Date=EncodeDate((Word)(NYmax+1900),(Word)NMmax,
   (Word)NDmax);
  DateTimePickerEnd->Visible=true;
 }
 else{
  GroupBoxDate->Caption=" Выбирайте дату ";
  LabelDate->Visible=false; LabelDateEnd->Visible=false;
  DateTimePicker->Date=EncodeDate((Word)(NY+1900),(Word)NM,
   (Word)ND);
  DateTimePicker->MinDate=EncodeDate((Word)(NYmin+1900),(Word)NMmin,
   (Word)NDmin);
  DateTimePicker->MaxDate=EncodeDate((Word)(NYmax+1900),(Word)NMmax,
   (Word)NDmax);
  DateTimePickerEnd->Visible=false;
}}
//---------------------------------------------------------------------------
void __fastcall TFormDate::ButtonOKDateClick(TObject *Sender){
 unsigned short ny,nm,nd;
 DateTimePicker->Date.DecodeDate(&ny,&nm,&nd);
 if(Period){
  NYmin=ny; NMmin=nm; NDmin=nd;
  DateTimePickerEnd->Date.DecodeDate(&ny,&nm,&nd);
  NYmax=ny; NMmax=nm; NDmax=nd;
  if(NDmax<NDmin){
   sprintf(Inf,"День конца периода не может быть\nменьше дня начала периода!");
   Application->MessageBox(Inf,"Ошибка",MB_OK|MB_ICONERROR);
   return;
  }
 }
 else{
  NY=ny; NM=nm; ND=nd;
 }
 Close();
}
//---------------------------------------------------------------------------

