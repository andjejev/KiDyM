//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "PrinTabs.h"
#include "ConvDW.h"
#include "ParamPrint.h"
#include "ToPrn.h"
#include "viewzarp.h"
#include "OtchetForm.h"
#include "Zarp.h"
#include <process.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormPrintTabs *FormPrintTabs;
extern TFormOtchet *FormOtchet;
extern TFormParamPrint *FormParamPrint;
extern TFormZarp *FormZarp;
extern TFormToPrn *FormToPrn;
extern char WORKDIR[],FIRMA[],*NameMont[],BUHGALTER_DOLG[],BUHGALTER_NAME[];
extern int NY,NM;
int *PosPage;

//---------------------------------------------------------------------------
int __fastcall TFormPrintTabs::KPage(int Kstr){
 int Kp=1,H=0,L=0,i,j,k,l,m,Nstr=0,Rez,Lmax;
 char Format[15],FileSh[]="tablspr",S[4096],*s0,*s,STab[1024],
   FilePrn[]="Itogi.prn";
 FILE *FSh,*FPrn;
 if(FPrn=fopen(FilePrn,"w")) fclose(FPrn);
 for(i=0;i<FormZarp->CheckListBoxTabItogi->Items->Count;i++){
  if(FormZarp->CheckListBoxTabItogi->State[i]==cbChecked){
   if(BookForSpr(FileSh,Rs[i],Format,NULL,&H,&L)){
    if(Nstr+H+5>Kstr){ Kp++; Nstr=0; } Nstr+=H+5;
 }}}
 PosPage=(int *)realloc(PosPage,Kp*SzI); Nstr=0; j=1;
 for(i=0;i<FormZarp->CheckListBoxTabItogi->Items->Count;i++){
  if(FormZarp->CheckListBoxTabItogi->State[i]==cbChecked){
   PosPage[0]=i;
   if(BookForSpr(FileSh,Rs[i],Format,NULL,&H,&L)) Nstr+=H+5;
   break;
 }}
 for(;i<FormZarp->CheckListBoxTabItogi->Items->Count;i++){
  if(FormZarp->CheckListBoxTabItogi->State[i]==cbChecked){
   if(BookForSpr(FileSh,Rs[i],Format,NULL,&H,&L)){
    if(Nstr+H+5>Kstr){ PosPage[j++]=i; Nstr=0; } Nstr+=H+5;
 }}}
 return Kp;
}
//---------------------------------------------------------------------------
void __fastcall TFormPrintTabs::ToCanvas(TCanvas *Canvas,int Np,int Kstr,int Hs){
 int H=0,L=0,i,j,k,l,b,*Mdl=NULL,Nstr=0,Lmax;
 FILE *FSh; Book *RNew;
 char Format[16],FileSh[]="tablspr",S[4096],*s0,*s;
 for(i=PosPage[Np];i<FormZarp->CheckListBoxTabItogi->Items->Count;i++){
  if(FormZarp->CheckListBoxTabItogi->State[i]==cbChecked){
   if(RNew=BookForSpr(FileSh,Rs[i],Format,NULL,&H,&L)){
    Mdl=(int *)realloc(Mdl,L*SzI);
    b=Canvas->TextWidth(" ");
    for(j=0;j<L;j++){
     sprintf(Inf,"%0.2f",RNew->NU[j].U.D); Mdl[j]=Canvas->TextWidth(Inf);
    }
    if(FSh=fopen(FileSh,"r")){ Lmax=0;
     for(k=0;k<H;k++) {
      fgets(S,4095,FSh);
      for(j=0,s=S;j<L-1;j++){
       s0=strchr(s,'\xb3'); *s0='\0';
       l=Canvas->TextWidth(StrDOStoWIN(s)); s=++s0;
       if(l>Mdl[j]) Mdl[j]=l;
      }
      s0=strchr(s,'\n'); *s0='\0';
      l=Canvas->TextWidth(StrDOStoWIN(s));
      if(l>Mdl[j]) Mdl[j]=l;
      for(j=0,l=1;j<L;j++) l+=Mdl[j]+b/2+b/2+1;
      if(l>Lmax) Lmax=l;
     }
     fclose(FSh);
    }
    else continue;
    if((Nstr+H+5)>Kstr) break;
    Nstr+=H+5;
    if(FSh=fopen(FileSh,"r")){
     l=Canvas->TextWidth(FormZarp->CheckListBoxTabItogi->Items->Strings[i]);
     while(Lmax<l){ b+=2; Lmax+=2*L; }
     Canvas->TextOut(X0+(Lmax-l)/2,Y,
      FormZarp->CheckListBoxTabItogi->Items->Strings[i]);
     Y+=Hs; Canvas->MoveTo(X0,Y+Hs/2); Canvas->LineTo(X0+Lmax,Y+Hs/2);
     X=X0; Canvas->MoveTo(X,Y+Hs/2); Canvas->LineTo(X,Y+(H+3)*Hs+Hs/2);
     for(j=0;j<L;j++){
      X+=Mdl[j]+b/2+b/2+1;
      Canvas->MoveTo(X,Y+Hs/2); Canvas->LineTo(X,Y+(H+3)*Hs+Hs/2);
     }
     Y+=Hs;
     for(k=0;k<H;k++) {
      fgets(S,4095,FSh); X=X0+1+b/2;
      for(j=0,s=S;j<L-1;j++){
       s0=strchr(s,'\xb3'); *s0++='\0';
       l=(Mdl[j]-Canvas->TextWidth(StrDOStoWIN(s)))/2;
       Canvas->TextOut(X+l,Y,s); X+=b/2+Mdl[j]+1+b/2;
       s=s0;
      }
      s0=strchr(s,'\n'); *s0='\0';
      l=(Mdl[j]-Canvas->TextWidth(StrDOStoWIN(s)))/2;
      Canvas->TextOut(X+l,Y,s); Y+=Hs;
      s=s0;
     }
     fclose(FSh); X=X0+1+b/2;
     Canvas->MoveTo(X0,Y+Hs/2); Canvas->LineTo(X0+Lmax,Y+Hs/2); Y+=Hs;
     for(j=0;j<L;j++){
//      sprintf(Inf,"%0.2f",RNew->NU[j].U.D);
      l=Canvas->TextWidth(Inf);
      Canvas->TextOut(X+Mdl[j]-l,Y,Inf); X+=b/2+Mdl[j]+1+b/2;
     }
     Y+=Hs; Canvas->MoveTo(X0,Y+Hs/2); Canvas->LineTo(X0+Lmax,Y+Hs/2); Y+=Hs;
 }}}}
 if(Mdl) free(Mdl);
}
//---------------------------------------------------------------------------
void __fastcall TFormPrintTabs::PrepareTabControl(){
 AnsiString S; double x; int Lmax,L,i,imax,SizeOld,dH,dW;
 Image->Align=alClient;
 Height=Screen->Height-60; Width=Screen->Width;
 TabControlPrint->Tabs->Clear();
 TabControlPrint->Tabs->Add("стр. 1");
 Prntr->Orientation=FormParamPrint->OrientKnig?poPortrait:poLandscape;
 LPx=GetDeviceCaps(Prntr->Handle,LOGPIXELSX),
 LPy=GetDeviceCaps(Prntr->Handle,LOGPIXELSY);
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
 Prntr->Canvas->Font->Name=FormParamPrint->FontName;
 Prntr->Canvas->Font->Size=FormParamPrint->FontSize;
 Prntr->Canvas->Font->Color=FormParamPrint->FontColor;
 Image->Canvas->Font->Assign(Prntr->Canvas->Font);
 Image->Canvas->Font->Size=Prntr->Canvas->Font->Size*Wimm/Wpmm;
 Himm=Hpmm*Image->Canvas->Font->Size/Prntr->Canvas->Font->Size;
 Height=dH+Round(10*LPiy*Himm/254)+2;
 Top=(Screen->Height-60-Height)/2;
 Wimm=Wpmm*Image->Canvas->Font->Size/Prntr->Canvas->Font->Size;
 Width=dW+Round(10*LPix*Wimm/254)+2;
 Left=(Screen->Width-Width)/2;
 Hs=Round(9*Prntr->Canvas->Font->Size*LPy/72/8);
 Ks=(Prntr->PageHeight-MarginTop-MarginBottom)/Hs;
 Kp=KPage(Ks);
 for(i=1;i<Kp;i++){
  S.sprintf("стр. %d",i+1); TabControlPrint->Tabs->Add(S);
 }
 Image->Canvas->Rectangle(Image->ClientRect);
 X0=FormParamPrint->MarginLeftmm*Wimm*10*LPix/Wpmm/254;
 Hsi=Round(9*Image->Canvas->Font->Size*LPix/72/8);
 Y0=Y=FormParamPrint->MarginTopmm*Himm*10*LPiy/Hpmm/254;
 ToCanvas(Image->Canvas,0,Ks,Hsi);
 if(Kp==1){
  sprintf(Inf,"%s ____________________ %s",BUHGALTER_DOLG,BUHGALTER_NAME);
  Image->Canvas->TextOut(X0,Y+Hsi,Inf);
 }
 Image->Canvas->MoveTo(Image->ClientWidth-1,0);
 Image->Canvas->LineTo(Image->ClientWidth-1,Image->ClientHeight);
 TColor Color=Image->Canvas->Pen->Color;
 Image->Canvas->Pen->Color=Image->Canvas->Brush->Color;
 x=Image->ClientWidth-FormParamPrint->MarginRightmm*Wimm*10*LPix/Wpmm/254;
 Image->Canvas->Rectangle(x,1,Image->ClientWidth-1,Image->ClientHeight-1);
 Image->Canvas->Pen->Color=Color;
}
//---------------------------------------------------------------------------
static char Titul[256];
//---------------------------------------------------------------------------
void __fastcall TFormPrintTabs::FormShow(TObject *Sender){
 char *s; strcpy(Titul,FormParamPrint->FilePrn);
 if(s=strrchr(Titul,'.')) strcpy(s+1,"prn"); else strcat(Titul,".prn");
 Prntr=Printer();
 Top=0; Height=Screen->Height-60; Left=0; Width=Screen->Width;
 Image->Canvas->Brush->Color=clWindow;
 Prntr->Title=Titul;
 PrepareTabControl();
}
//---------------------------------------------------------------------------
__fastcall TFormPrintTabs::TFormPrintTabs(TComponent* Owner):TForm(Owner){
 sprintf(Inf,"Просмотр и печать таблиц итоговых данных. %s %d. %s",
  NameMont[NM-1],NY+1900,FIRMA);
 Caption=Inf; 
 PosPage=NULL;
}
//---------------------------------------------------------------------------
void __fastcall TFormPrintTabs::ButtonSetupClick(TObject *Sender){
// FormParamPrint->TakeParam();
 FormParamPrint->ShowModal();
 if(FormParamPrint->Change){
  Prntr->Canvas->Font->Name=FormParamPrint->FontName;
  Prntr->Canvas->Font->Size=FormParamPrint->FontSize;
  Prntr->Canvas->Font->Color=FormParamPrint->FontColor;
  PrepareTabControl();
 }
 else TabControlPrint->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TFormPrintTabs::TabControlPrintChange(TObject *Sender){
 double x; TColor Color=Image->Canvas->Pen->Color;
 Image->Canvas->Rectangle(Image->ClientRect);
 X0=FormParamPrint->MarginLeftmm*Wimm*10*LPix/Wpmm/254;
 Y0=Y=FormParamPrint->MarginTopmm*Himm*10*LPiy/Hpmm/254;
 ToCanvas(Image->Canvas,TabControlPrint->TabIndex,Ks,Hsi);
 if(TabControlPrint->TabIndex==(Kp-1)){
  sprintf(Inf,"%s ____________________ %s",BUHGALTER_DOLG,BUHGALTER_NAME);
  Image->Canvas->TextOut(X0,Y+Hsi,Inf);
 }
 Image->Canvas->MoveTo(Image->ClientWidth-1,0);
 Image->Canvas->LineTo(Image->ClientWidth-1,Image->ClientHeight);
 Image->Canvas->Pen->Color=Image->Canvas->Brush->Color;
 x=Image->ClientWidth-FormParamPrint->MarginRightmm*Wimm*10*LPix/Wpmm/254;
 Image->Canvas->Rectangle(x,1,Image->ClientWidth-1,Image->ClientHeight-1);
 Image->Canvas->Pen->Color=Color;
}
//---------------------------------------------------------------------------
void __fastcall TFormPrintTabs::ToolButtonExitClick(TObject *Sender){
 Close(); free(PosPage); PosPage=NULL;
}
//---------------------------------------------------------------------------
void __fastcall ToPrint(){
 FormPrintTabs->ToPrn();
}
//---------------------------------------------------------------------------
void __fastcall TFormPrintTabs::ToolButtonPrintClick(TObject *Sender){
 Application->CreateForm(__classid(TFormToPrn),&FormToPrn);
 FormToPrn->MaxPage=TabControlPrint->Tabs->Count;
 FormToPrn->FromPage=1;
 FormToPrn->ToPage=TabControlPrint->Tabs->Count;
 FormToPrn->ToPrinter=ToPrint;
 sprintf(FormToPrn->FileHelp,"%s\\toprn.htm",WORKDIR);
 FormToPrn->ShowModal();
 TabControlPrint->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TFormPrintTabs::ToPrn(){
 int i,j,W=GetDeviceCaps(Prntr->Handle,HORZRES),
  H=GetDeviceCaps(Prntr->Handle,VERTRES),x=W-MarginRight;
  TColor Color=Prntr->Canvas->Pen->Color;
 Prntr->BeginDoc();
 for(i=0;i<FormToPrn->Copies;i++){
  for(j=FormToPrn->FromPage;j<=FormToPrn->ToPage;j++){
   Y=Y0=MarginTop; X=MarginLeft;
   ToCanvas(Prntr->Canvas,j-1,Ks,Hs);
   if(j==Kp){
    sprintf(Inf,"%s ____________________ %s",BUHGALTER_DOLG,BUHGALTER_NAME);
    Prntr->Canvas->TextOut(X0,Y+Hs,Inf);
   }
   Prntr->Canvas->Pen->Color=Prntr->Canvas->Brush->Color;
   Prntr->Canvas->Rectangle(x,MarginTop,W-1,H-1);
   Prntr->Canvas->Pen->Color=Color;
   Prntr->NewPage();
 }}
 Prntr->EndDoc();
}
//---------------------------------------------------------------------------
void __fastcall TFormPrintTabs::ToolButtonDOSPrnClick(TObject *Sender){
 char PrintDosExe[256];
 if(FormZarp->PrnTablItogi()){
  if(*WORKDIR) sprintf(PrintDosExe,"%s\\PrintDos.exe",WORKDIR);
  else sprintf(PrintDosExe,"PrintDos.exe");
  spawnl(P_NOWAIT,PrintDosExe,PrintDosExe,"Itogi.prn",NULL);
}}
//---------------------------------------------------------------------------
void __fastcall TFormPrintTabs::ToolButtonHelpClick(TObject *Sender){
 char Help[256],*s; strcpy(Help,WORKDIR);
 if(s=strrchr(Help,'\\')) strcpy(s+1,"Help\\PrintTabs.htm");
 else strcat(Help,"\\Help\\PrintTabs.htm");
 if(access(Help,0)){
  sprintf(Inf,"Не найден файл помощи\n\"%s\"",Help);
  Application->MessageBox(Inf,"Системная ошибка!",MB_OK);
  return;
 }
 if(!FormOtchet)
  Application->CreateForm(__classid(TFormOtchet),&FormOtchet);
 FormOtchet->cbURL->Text=Help;
 FormOtchet->ShowModal();
}
//---------------------------------------------------------------------------

