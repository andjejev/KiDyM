//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "OperMess.h"
#include "ParamPrint.h"
#include "FormPrRE.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormOper *FormOper;
extern TFormParamPrint *FormParamPrint;
extern TRichEdit *RichEdit;
extern TFormPrintRichEdit *FormPrintRichEdit;

//---------------------------------------------------------------------------
__fastcall TFormOper::TFormOper(TComponent* Owner):TForm(Owner){}
//---------------------------------------------------------------------------
void __fastcall TFormOper::tbExitClick(TObject *Sender){ Close(); }
//---------------------------------------------------------------------------
void __fastcall TFormOper::tbPrintClick(TObject *Sender){
 Application->CreateForm(__classid(TFormParamPrint),&FormParamPrint);
 FormParamPrint->FilePrn=FilePrn;
 FormParamPrint->TakeParam();
 Application->CreateForm(__classid(TFormPrintRichEdit),&FormPrintRichEdit);
 FormPrintRichEdit->FileKDM=FilePrn;
 RichEdit=RichEditOper;
 FormPrintRichEdit->ShowModal();
}
//---------------------------------------------------------------------------
void __fastcall TFormOper::FormShow(TObject *Sender){
 RichEditOper->SelStart=0; RichEditOper->SelLength=0;
 Height=ToolBar->Height+6*RichEditOper->Lines->Count*Canvas->TextHeight("M")/5+64;
}
//---------------------------------------------------------------------------

void __fastcall TFormOper::FormKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift){
 if((Key==VK_RETURN)||(Key==VK_ESCAPE)) Close();
}
//---------------------------------------------------------------------------

