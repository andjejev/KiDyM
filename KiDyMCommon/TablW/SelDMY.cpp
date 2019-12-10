//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "SelDMY.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormSelDMY *FormSelDMY; bool OK;
//---------------------------------------------------------------------------
__fastcall TFormSelDMY::TFormSelDMY(TComponent* Owner):TForm(Owner){
 NY=NM=ND=0;
}
//---------------------------------------------------------------------------
void __fastcall TFormSelDMY::ButtonOKDateClick(TObject *Sender){
 unsigned short ny,nm,nd;
 DateTimePicker->Date.DecodeDate(&ny,&nm,&nd);
 NY=ny; NM=nm; ND=nd; OK=true;
 Close();
}
//---------------------------------------------------------------------------
void __fastcall TFormSelDMY::ButtonCancelDateClick(TObject *Sender){
 NY=NM=ND=0;
 Close();
}
//---------------------------------------------------------------------------
void __fastcall TFormSelDMY::FormShow(TObject *Sender){
 if(NY&&NM&&ND)
  DateTimePicker->Date=EncodeDate((Word)(NY),(Word)NM,(Word)ND);
 FormSelDMY->ClientWidth=DateTimePicker->Width;
 FormSelDMY->ClientHeight=DateTimePicker->Height+ButtonOKDate->Height+6;
}
//---------------------------------------------------------------------------
void __fastcall TFormSelDMY::FormClose(TObject *Sender,TCloseAction &Action){
 if(!OK) NY=NM=ND=0;
}
//---------------------------------------------------------------------------


