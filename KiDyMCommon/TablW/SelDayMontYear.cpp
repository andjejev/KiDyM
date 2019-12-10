//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "SelDayMontYear.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormSelDMY *FormSelDMY;
//---------------------------------------------------------------------------
__fastcall TFormSelDMY::TFormSelDMY(TComponent* Owner):TForm(Owner){
 NY=NM=ND=0;
}
//---------------------------------------------------------------------------
void __fastcall TFormSelDMY::FormCreate(TObject *Sender){
 if(NY&&NM&&ND)
  DateTimePicker->MaxDate=EncodeDate((Word)(NY+1900),(Word)NM,(Word)ND);
}
//---------------------------------------------------------------------------
void __fastcall TFormDate::ButtonOKDateClick(TObject *Sender){
 unsigned short ny,nm,nd;
 DateTimePicker->Date.DecodeDate(&ny,&nm,&nd);
 NY=ny; NM=nm; ND=nd;
 Close();
}
//---------------------------------------------------------------------------
void __fastcall TFormSelDMY::ButtonCancelDateClick(TObject *Sender){
 NY=NM=ND=0;
 Close();
}
//---------------------------------------------------------------------------
