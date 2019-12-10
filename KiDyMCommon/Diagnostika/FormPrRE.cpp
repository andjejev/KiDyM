//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormPrRE.h"
#include "ConvDW.h"
#include "ParamPrint.h"
#include "ToPrn.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormPrintRichEdit *FormPrintRichEdit;
extern TFormParamPrint *FormParamPrint;
extern TFormToPrn *FormToPrn;
extern TRichEdit *RichEdit;
extern wchar_t WORKDIR[];
extern TStringList *SLF;
//---------------------------------------------------------------------------
void __fastcall TFormPrintRichEdit::PrepareTabControl(){
 String S; double x;
 int Lmax,L,i,imax,SizeOld,dH,dW;
 LPx=GetDeviceCaps(Prntr->Handle,LOGPIXELSX),
 LPy=GetDeviceCaps(Prntr->Handle,LOGPIXELSY);
 Image->Align=alClient;
 Height=Screen->Height-60; Width=Screen->Width;
 TabControlPrint->Tabs->Clear();
 S.sprintf(TextFromFile(WORKDIR,L"FormPrRE",&SLF,1),1);
 TabControlPrint->Tabs->Add(S);
 Prntr->Orientation=FormParamPrint->OrientKnig?poPortrait:poLandscape;
 MarginTop=Round((double)FormParamPrint->MarginTopmm*LPy/25.4);
 MarginLeft=Round((double)FormParamPrint->MarginLeftmm*LPx/25.4);
 MarginRight=Round((double)FormParamPrint->MarginRightmm*LPx/25.4);
 MarginBottom=Round((double)FormParamPrint->MarginBotmm*LPy/25.4);
 dH=Height-Image->Height-2; dW=Width-Image->Width-2;
 LPix=GetDeviceCaps(Canvas->Handle,LOGPIXELSX);
 LPiy=GetDeviceCaps(Canvas->Handle,LOGPIXELSY);
 Wimm=254*Image->Width/LPix/10;
 Wpmm=GetDeviceCaps(Prntr->Handle,HORZSIZE);
 Himm=254*Image->Height/LPiy/10;
 Hpmm=GetDeviceCaps(Prntr->Handle,VERTSIZE);
 if(Himm*Wpmm>Hpmm*Wimm){
  Height=dH+Image->Height*Hpmm*Wimm/Wpmm/Himm+2;
  Himm=254*Image->Height/LPiy/10;
 }
 else{
  Width=dW+Image->Width*Himm*Wpmm/Wimm/Hpmm+2;
  Wimm=254*Image->Width/LPix/10;
 }
 Prntr->Canvas->Font->Assign(RichEdit->Font);
 Image->Canvas->Font->Assign(RichEdit->Font);
 wcscpy(FormParamPrint->FontName,Prntr->Canvas->Font->Name.c_str());
 FormParamPrint->FontSize=Prntr->Canvas->Font->Size;
 Image->Canvas->Font->Size=Prntr->Canvas->Font->Size*Wimm/Wpmm;
// Image->Canvas->Font->Size=Prntr->Canvas->Font->Size*Himm/Hpmm;
 Himm=Hpmm*Image->Canvas->Font->Size/Prntr->Canvas->Font->Size;
 Height=dH+Round(10*LPiy*Himm/254)+2;
 Top=(Screen->Height-60-Height)/2;
 Wimm=Wpmm*Image->Canvas->Font->Size/Prntr->Canvas->Font->Size;
 Width=dW+Round(10*LPix*Wimm/254)+2;
 Left=(Screen->Width-Width)/2;
 Hs=Round(9*Prntr->Canvas->Font->Size*LPy/72/8);
 Ks=(Prntr->PageHeight-MarginTop-MarginBottom)/Hs;
 Kp=RichEdit->Lines->Count/Ks;
 if(RichEdit->Lines->Count%Ks) Kp++;
 for(i=1;i<Kp;i++){
  S.sprintf(TextFromFile(WORKDIR,L"FormPrRE",&SLF,1),i+1);
  TabControlPrint->Tabs->Add(S);
 }
 imax=Ks<RichEdit->Lines->Count?Ks:RichEdit->Lines->Count;
 Image->Canvas->Rectangle(Image->ClientRect);
// X=Round(FormParamPrint->MarginLeftmm*Wimm*10*LPix/Wpmm/254)+1;
 X=FormParamPrint->MarginLeftmm*Wimm*10*LPix/Wpmm/254;
 Hsi=Round(9*Image->Canvas->Font->Size*LPix/72/8);
// Y=Y0=Round(FormParamPrint->MarginTopmm*Himm*10*LPiy/Hpmm/254)+1;
 Y=Y0=FormParamPrint->MarginTopmm*Himm*10*LPiy/Hpmm/254;
 for(i=0;i<imax;i++){
  Image->Canvas->TextOut(X,Y,RichEdit->Lines->Strings[i]); Y+=Hsi;
 }
 Image->Canvas->MoveTo(Image->ClientWidth-1,0);
 Image->Canvas->LineTo(Image->ClientWidth-1,Image->ClientHeight);
 TColor Color=Image->Canvas->Pen->Color;
 Image->Canvas->Pen->Color=Image->Canvas->Brush->Color;
 x=Image->ClientWidth-
    FormParamPrint->MarginRightmm*Wimm*10*LPix/Wpmm/254;
 Image->Canvas->Rectangle(x,1,Image->ClientWidth-1,Image->ClientHeight-1);
 Image->Canvas->Pen->Color=Color;
}
//---------------------------------------------------------------------------
static wchar_t Titul[256];
//---------------------------------------------------------------------------
void __fastcall TFormPrintRichEdit::FormShow(TObject *Sender){
 wchar_t *s; wcscpy(Titul,FormParamPrint->FilePrn);
 if(s=wcsrchr(Titul,'.')) wcscpy(s+1,L"prn"); else wcscat(Titul,L".prn");
 Prntr=Printer();
 Top=0; Height=Screen->Height-60; Left=0; Width=Screen->Width;
 Image->Canvas->Brush->Color=clWindow;
 Prntr->Canvas->Font->Name=FormParamPrint->FontName;
 Prntr->Canvas->Font->Size=FormParamPrint->FontSize;
 Prntr->Canvas->Font->Color=FormParamPrint->FontColor;
 Prntr->Orientation=FormParamPrint->OrientKnig?poPortrait:poLandscape;
 Prntr->Title=Titul;
 LPx=GetDeviceCaps(Prntr->Handle,LOGPIXELSX),
 LPy=GetDeviceCaps(Prntr->Handle,LOGPIXELSY);
 MarginTop=Round((double)FormParamPrint->MarginTopmm*LPy/25.4);
 MarginLeft=Round((double)FormParamPrint->MarginLeftmm*LPx/25.4);
 MarginRight=Round((double)FormParamPrint->MarginRightmm*LPx/25.4);
 MarginBottom=Round((double)FormParamPrint->MarginBotmm*LPy/25.4);
 PrepareTabControl();
}
//---------------------------------------------------------------------------
__fastcall TFormPrintRichEdit::TFormPrintRichEdit(TComponent* Owner)
        : TForm(Owner){
 Caption=TextFromFile(WORKDIR,L"FormPrRE",&SLF,2);
 ToolButtonPrint->Caption=TextFromFile(WORKDIR,L"FormPrRE",&SLF,3);
 ToolButtonSetup->Caption=TextFromFile(WORKDIR,L"FormPrRE",&SLF,4);
 ToolButtonExit->Caption=TextFromFile(WORKDIR,L"FormPrRE",&SLF,5);
}
//---------------------------------------------------------------------------
void __fastcall TFormPrintRichEdit::ButtonSetupClick(TObject *Sender){
 FormParamPrint->TakeParam();
 FormParamPrint->ShowModal();
 if(FormParamPrint->Change){
  Prntr->Canvas->Font->Name=FormParamPrint->FontName;
  Prntr->Canvas->Font->Size=FormParamPrint->FontSize;
  Prntr->Canvas->Font->Color=FormParamPrint->FontColor;
  Prntr->Orientation=FormParamPrint->OrientKnig?poPortrait:poLandscape;
  LPx=GetDeviceCaps(Prntr->Handle,LOGPIXELSX),
  LPy=GetDeviceCaps(Prntr->Handle,LOGPIXELSY);
  MarginTop=Round((double)FormParamPrint->MarginTopmm*LPy/25.4);
  MarginLeft=Round((double)FormParamPrint->MarginLeftmm*LPx/25.4);
  MarginRight=Round((double)FormParamPrint->MarginRightmm*LPx/25.4);
  MarginBottom=Round((double)FormParamPrint->MarginBotmm*LPy/25.4);
  PrepareTabControl();
 }
 else TabControlPrint->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TFormPrintRichEdit::TabControlPrintChange(TObject *Sender){
 AnsiString S; double x; TColor Color=Image->Canvas->Pen->Color;
 int Ind=TabControlPrint->TabIndex,i,
  imax=Ks<(RichEdit->Lines->Count-Ind*Ks)?Ks:(RichEdit->Lines->Count-Ind*Ks);
 Image->Canvas->Rectangle(Image->ClientRect);
 Y=Y0;
 for(i=0;i<imax;i++){
  Image->Canvas->TextOut(X,Y,RichEdit->Lines->Strings[Ind*Ks+i]);
  Y+=Hsi;
 }
 Image->Canvas->MoveTo(Image->ClientWidth-1,0);
 Image->Canvas->LineTo(Image->ClientWidth-1,Image->ClientHeight);
 Image->Canvas->Pen->Color=Image->Canvas->Brush->Color;
 x=Image->ClientWidth-
    /*Round(*/FormParamPrint->MarginRightmm*Wimm*10*LPix/Wpmm/254/*)*/;
 Image->Canvas->Rectangle(x,1,Image->ClientWidth-1,Image->ClientHeight-1);
 Image->Canvas->Pen->Color=Color;
}
//---------------------------------------------------------------------------
void __fastcall TFormPrintRichEdit::ToolButtonExitClick(TObject *Sender){
 Close();
}
//---------------------------------------------------------------------------
void __fastcall REToPrinter(){
 FormPrintRichEdit->ToPrntr();
}
//---------------------------------------------------------------------------
void __fastcall TFormPrintRichEdit::ToolButtonPrintClick(TObject *Sender){
 Application->CreateForm(__classid(TFormToPrn),&FormToPrn);
 FormToPrn->MaxPage=TabControlPrint->Tabs->Count;
 FormToPrn->FromPage=1;
 FormToPrn->ToPage=TabControlPrint->Tabs->Count;
 FormToPrn->ToPrinter=REToPrinter;
 swprintf(FormToPrn->FileHelp,L"%s\\toprn.htm",WORKDIR);
 FormToPrn->ShowModal();
 TabControlPrint->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TFormPrintRichEdit::ToPrntr(){
 int i,j,k,kk,W,H,x; TColor Color;
// if(!Prntr){
  Prntr=Printer();
  Top=0; Height=Screen->Height-60; Left=0; Width=Screen->Width;
  Image->Canvas->Brush->Color=clWindow;
  Prntr->Canvas->Font->Name=FormParamPrint->FontName;
  Prntr->Canvas->Font->Size=FormParamPrint->FontSize;
  Prntr->Canvas->Font->Color=FormParamPrint->FontColor;
  Prntr->Orientation=FormParamPrint->OrientKnig?poPortrait:poLandscape;
  Prntr->Title=Titul;
  LPx=GetDeviceCaps(Prntr->Handle,LOGPIXELSX),
  LPy=GetDeviceCaps(Prntr->Handle,LOGPIXELSY);
  MarginTop=Round((double)FormParamPrint->MarginTopmm*LPy/25.4);
  MarginLeft=Round((double)FormParamPrint->MarginLeftmm*LPx/25.4);
  MarginRight=Round((double)FormParamPrint->MarginRightmm*LPx/25.4);
  MarginBottom=Round((double)FormParamPrint->MarginBotmm*LPy/25.4);
  PrepareTabControl();
// }
 W=GetDeviceCaps(Prntr->Handle,HORZRES); Color=Prntr->Canvas->Pen->Color;
 H=GetDeviceCaps(Prntr->Handle,VERTRES); x=W-MarginRight;
 Prntr->BeginDoc();
 kk=RichEdit->Lines->Count%Ks; kk=(FormToPrn->ToPage!=Kp||!kk)?Ks:kk;
 for(i=0;i<FormToPrn->Copies;i++){
  for(j=FormToPrn->FromPage;j<FormToPrn->ToPage;j++) {
   for(k=0;k<Ks;k++)
    Prntr->Canvas->TextOut(MarginLeft,MarginTop+k*Hs,
     RichEdit->Lines->Strings[k+(j-1)*Ks]);
   Prntr->Canvas->Pen->Color=Prntr->Canvas->Brush->Color;
   Prntr->Canvas->Rectangle(x,MarginTop,W-1,H-1);
   Prntr->Canvas->Pen->Color=Color;
   Prntr->NewPage();
  }
  for(k=0;k<kk;k++)
   Prntr->Canvas->TextOut(MarginLeft,MarginTop+k*Hs,
    RichEdit->Lines->Strings[k+(FormToPrn->ToPage-1)*Ks]);
  Prntr->Canvas->Pen->Color=Prntr->Canvas->Brush->Color;
  Prntr->Canvas->Rectangle(x,MarginTop,W-1,H-1);
  Prntr->Canvas->Pen->Color=Color;
 }
 Prntr->EndDoc();
}
//---------------------------------------------------------------------------


