//---------------------------------------------------------------------------

#include <vcl.h>
#include <io.h>
#include <stdio.h>
#include <dir.h>
#include "tablw.h"

#pragma hdrstop

#include "ParamPrint.h"
#include "ConvDW.h"
#include "OtchetForm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormParamPrint *FormParamPrint;
extern TFormOtchet *FormOtchet;
extern wchar_t Inf[],WORKDIR[];
extern TStringList *SLP;
//---------------------------------------------------------------------------
bool Ver2000(){
_OSVERSIONINFOA OSVer;
 OSVer.dwOSVersionInfoSize = sizeof(_OSVERSIONINFO);
 GetVersionEx(&OSVer);
 if((OSVer.dwPlatformId==VER_PLATFORM_WIN32_NT)&&
    (OSVer.dwMajorVersion==5)&&
    (OSVer.dwMinorVersion==0)) return true;
 return false;
}
//---------------------------------------------------------------------------
void __fastcall TFormParamPrint::TakeParam(void){
 FontSize=10;     FontColor=clWindowText;
 MarginLeftmm=20; MarginRightmm=20;
 MarginTopmm=20;  MarginBotmm=20;
 wcscpy(FontName,L"Courier New");
 OrientKnig=true;//книжная
 ReadOptPrn();
}
//---------------------------------------------------------------------------
__fastcall TFormParamPrint::TFormParamPrint(TComponent* Owner):TForm(Owner){
 Interval=1.0;
 ToolButtonHelp->Hint=TextFromFile(WORKDIR,L"ParamPrint",&SLP,17);
 ToolButtonExit->Hint=TextFromFile(WORKDIR,L"ParamPrint",&SLP,18);
 Caption=TextFromFile(WORKDIR,L"ParamPrint",&SLP,19);
 LabelInt->Caption=TextFromFile(WORKDIR,L"ParamPrint",&SLP,20);
 ButtonCancel->Caption=TextFromFile(WORKDIR,L"ParamPrint",&SLP,21);
 GroupBoxMargin->Caption=TextFromFile(WORKDIR,L"ParamPrint",&SLP,22);
 ButtonPrint->Caption=TextFromFile(WORKDIR,L"ParamPrint",&SLP,23);
 ButtonFont->Caption=TextFromFile(WORKDIR,L"ParamPrint",&SLP,24);
 LabelLeft->Caption=TextFromFile(WORKDIR,L"ParamPrint",&SLP,25);
 LabelRight->Caption=TextFromFile(WORKDIR,L"ParamPrint",&SLP,26);
 LabelTop->Caption=TextFromFile(WORKDIR,L"ParamPrint",&SLP,27);
 LabelBottom->Caption=TextFromFile(WORKDIR,L"ParamPrint",&SLP,28);
// Defines();
}
//---------------------------------------------------------------------------
bool __fastcall TFormParamPrint::IsSetOrient(TPrinter *P){
 DEVMODE *p=NULL;
 if(P->Printers->Count){
  wchar_t ADevice[256],ADriver[256],APort[256];
  unsigned int ADeviceMode;
  P->GetPrinter(ADevice,ADriver,APort,ADeviceMode);
  p=(DEVMODE *)GlobalLock((void *)ADeviceMode);
 }
 return (!IsWin2000||p)?true:false;
}
//---------------------------------------------------------------------------
void __fastcall TFormParamPrint::ButtonCancelClick(TObject *Sender){
 if(Prn&&Prn->Printers->Count){
  Prn->Canvas->Font->Name=FontName;
  Prn->Canvas->Font->Size=FontSize;
  Prn->Canvas->Font->Color=FontColor;
  if(IsSetOrient(Prn))
   Prn->Orientation=OrientKnig?poPortrait:poLandscape;
  Prn->Title=FilePrn;
  LPx=GetDeviceCaps(Prn->Handle,LOGPIXELSX);
  LPy=GetDeviceCaps(Prn->Handle,LOGPIXELSY);
  PhW=GetDeviceCaps(Prn->Handle,PHYSICALWIDTH);
  PhH=GetDeviceCaps(Prn->Handle,PHYSICALHEIGHT);
  Wmm=GetDeviceCaps(Prn->Handle,HORZSIZE);
  Hmm=GetDeviceCaps(Prn->Handle,VERTSIZE);
  PhL=GetDeviceCaps(Prn->Handle,PHYSICALOFFSETX);
  PhT=GetDeviceCaps(Prn->Handle,PHYSICALOFFSETY);
  PhWmm=Round(PhW*25.4/LPx);
  PhHmm=Round(PhH*25.4/LPy);
  PhTmm=Round(PhT*25.4/LPy);
  PhBmm=PhHmm-PhTmm-Hmm;
  PhLmm=Round(PhL*25.4/LPx);
  PhRmm=PhWmm-PhLmm-Wmm;
 }
 UpDownTop->Position=MarginTopmm;
 UpDownLeft->Position=MarginLeftmm;
 UpDownRight->Position=MarginRightmm;
 UpDownBottom->Position=MarginBotmm;
 Change=false; Close();
}
//---------------------------------------------------------------------------
void __fastcall TFormParamPrint::ButtonPrintClick(TObject *Sender){
 if(!Prn||!Prn->Printers->Count){
  Application->MessageBox(L"В системе нет принтеров!\n"
   L"Поэтому настройки печати невозможны!",L"ОШИБКА!",MB_OK|MB_ICONERROR);
  return;
 }
 if(IsSetOrient(Prn)){
  wchar_t PrinterOld[256];
  wcscpy(PrinterOld,Prn->Printers->Strings[Prn->PrinterIndex].c_str());
  PrinterSetupDialog->Execute();
  if(wcscmp(PrinterOld,Prn->Printers->Strings[Prn->PrinterIndex].c_str()))
   CheckBoxPrinters->Enabled=true;
 }
 else{
  Application->MessageBox(L"Изменять настройки этого принтера\n"
   L"можно только на внешнем уровне?",
   L"С этим принтером в системе WINDOWS-2000 печать проблематична!",
   MB_OK|MB_ICONINFORMATION);
}}
//---------------------------------------------------------------------------
void __fastcall TFormParamPrint::ButtonFontClick(TObject *Sender){
 if(!Prn||!Prn->Printers->Count){
  Application->MessageBox(L"В системе нет принтеров!\n"
   L"Поэтому настройки печати невозможны!",L"ОШИБКА!",MB_OK|MB_ICONERROR);
  return;
 }
 if(FontDialog->Execute())
  FontDialog->Font->Charset=RUSSIAN_CHARSET;
}
//---------------------------------------------------------------------------
void __fastcall TFormParamPrint::ButtonOKClick(TObject *Sender){
 if(UpDownTop->Position<PhTmm||UpDownLeft->Position<PhLmm||
    UpDownRight->Position<PhRmm||UpDownBottom->Position<PhBmm){
  swprintf(Inf,TextFromFile(WORKDIR,L"ParamPrint",&SLP,1));
  if(Application->MessageBox(Inf,TextFromFile(WORKDIR,L"ParamPrint",&SLP,2),
     MB_YESNO|MB_ICONQUESTION)==ID_YES){
   if(UpDownTop->Position<PhTmm) UpDownTop->Position=PhTmm;
   if(UpDownLeft->Position<PhLmm) UpDownLeft->Position=PhLmm;
   if(UpDownRight->Position<PhRmm) UpDownRight->Position=PhRmm;
   if(UpDownBottom->Position<PhBmm) UpDownBottom->Position=PhBmm;
   return;
 }}
 Interval=0.0;//_wtof(ditInt->Text.w_str());  ?????10.03.2019
 if(Interval<0.01){
  swprintf(Inf,TextFromFile(WORKDIR,L"ParamPrint",&SLP,3));
  if(Application->MessageBox(Inf,TextFromFile(WORKDIR,L"ParamPrint",&SLP,2),
     MB_YESNO|MB_ICONQUESTION)==ID_YES){
   Interval=1.0;
   swprintf(Inf,L"%0.2f",Interval); EditInt->Text=Inf;
   return;
 }}
 wcscpy(FontName,FontDialog->Font->Name.c_str());
 FontSize=FontDialog->Font->Size;
 FontColor=FontDialog->Font->Color;
 Podch=FontDialog->Font->Style.Contains(fsUnderline);
 MarginTopmm=UpDownTop->Position;
 MarginLeftmm=UpDownLeft->Position;
 MarginRightmm=UpDownRight->Position;
 MarginBotmm=UpDownBottom->Position;
 if(Prn->Printers->Count){
  Magazine *Mf=NULL,*m; struct _wffblk FB; FILE *Fi,*Fo; wchar_t Maska[256];
  if(IsSetOrient(Prn))
   OrientKnig=Prn->Orientation==poPortrait;
  else
   OrientKnig=poPortrait;
  wcscpy(PrintName,Prn->Printers->Strings[Prn->PrinterIndex].c_str());
  if(CheckBoxPrinters->Enabled&&(CheckBoxPrinters->Checked==true)){
   for(int i=_wfindfirst(L"*.popt",&FB,0);!i;i=_wfindnext(&FB))
    TakeMagazine(&Mf,FB.ff_name);
   for(m=Mf;m;m=m->Sled){
    if(Fo=_wfopen(L"temp.popt",L"w")){
     if(Fi=_wfopen(m->S,L"r")){
      for(int i=0;i<9;i++){ fgetws(Inf,255,Fi); fputws(Inf,Fo); }
      fgetws(Inf,255,Fi);
      swprintf(Inf,TextFromFile(WORKDIR,L"ParamPrint",&SLP,14),PrintName);
      Fputs(Inf,Fo); fputwc('\n',Fo);
      while(fgetws(Inf,255,Fi)) fputws(Inf,Fo);
      fclose(Fi);
     }
	 fclose(Fo);
	 if(Fi){ _wunlink(m->S); _wrename(L"temp.popt",m->S); }
   }}
   DelAllMagazine(&Mf);
   CheckBoxPrinters->Enabled=false;
   CheckBoxPrinters->Checked=false;
  }
 /*
  if(Prn->GetDevMode()){
   switch(Prn->GetDevMode()->dmPaperSize){
	case DMPAPER_LETTER:
	 wcscpy(PaperName,L"Letter, 8 1/2- by 11-inches");              break;
	case DMPAPER_LEGAL:
	 wcscpy(PaperName,L"Legal, 8 1/2- by 14-inches");               break;
	case DMPAPER_A4:
	 wcscpy(PaperName,L"A4 Sheet, 210- by 297-millimeters");        break;
	case DMPAPER_CSHEET:
	 wcscpy(PaperName,L"C Sheet, 17- by 22-inches");                break;
	case DMPAPER_DSHEET:
	 wcscpy(PaperName,L"D Sheet, 22- by 34-inches");                break;
	case DMPAPER_ESHEET:
	 wcscpy(PaperName,L"E Sheet, 34- by 44-inches");                break;
	case DMPAPER_LETTERSMALL:
	 wcscpy(PaperName,L"Letter Small, 8 1/2- by 11-inches");        break;
	case DMPAPER_TABLOID:
	 wcscpy(PaperName,L"Tabloid, 11- by 17-inches");                break;
	case DMPAPER_LEDGER:
	 wcscpy(PaperName,L"Ledger, 17- by 11-inches");                 break;
	case DMPAPER_STATEMENT:
	 wcscpy(PaperName,L"Statement, 5 1/2- by 8 1/2-inches");        break;
	case DMPAPER_EXECUTIVE:
	 wcscpy(PaperName,L"Executive, 7 1/4- by 10 1/2-inches");       break;
	case DMPAPER_A3:
	 wcscpy(PaperName,L"A3 sheet, 297- by 420-millimeters");        break;
	case DMPAPER_A4SMALL:
	 wcscpy(PaperName,L"A4 small sheet, 210- by 297-millimeters");  break;
	case DMPAPER_A5:
	 wcscpy(PaperName,L"A5 sheet, 148- by 210-millimeters");        break;
	case DMPAPER_B4:
	 wcscpy(PaperName,L"B4 sheet, 250- by 354-millimeters");        break;
	case DMPAPER_B5:
	 wcscpy(PaperName,L"B5 sheet, 182- by 257-millimeter paper");   break;
	case DMPAPER_FOLIO:
	 wcscpy(PaperName,L"Folio, 8 1/2- by 13-inch paper");           break;
	case DMPAPER_QUARTO:
	 wcscpy(PaperName,L"Quarto, 215- by 275-millimeter paper");     break;
	case DMPAPER_10X14:
	 wcscpy(PaperName,L"10- by 14-inch sheet");                     break;
	case DMPAPER_11X17:
	 wcscpy(PaperName,L"11- by 17-inch sheet");                     break;
	case DMPAPER_NOTE:
	 wcscpy(PaperName,L"Note, 8 1/2- by 11-inches");                break;
	case DMPAPER_ENV_9:
	 wcscpy(PaperName,L"#9 Envelope, 3 7/8- by 8 7/8-inches");      break;
	case DMPAPER_ENV_10:
	 wcscpy(PaperName,L"#10 Envelope, 4 1/8- by 9 1/2-inches");     break;
	case DMPAPER_ENV_11:
	 wcscpy(PaperName,L"#11 Envelope, 4 1/2- by 10 3/8-inches");    break;
	case DMPAPER_ENV_12:
	 wcscpy(PaperName,L"#12 Envelope, 4 3/4- by 11-inches");        break;
	case DMPAPER_ENV_14:
	 wcscpy(PaperName,L"#14 Envelope, 5- by 11 1/2-inches");        break;
	case DMPAPER_ENV_DL:
	 wcscpy(PaperName,L"DL Envelope, 110- by 220-millimeters");     break;
	case DMPAPER_ENV_C5:
	 wcscpy(PaperName,L"C5 Envelope, 162- by 229-millimeters");     break;
	case DMPAPER_ENV_C3:
	 wcscpy(PaperName,L"C3 Envelope,  324- by 458-millimeters");    break;
	case DMPAPER_ENV_C4:
	 wcscpy(PaperName,L"C4 Envelope,  229- by 324-millimeters");    break;
	case DMPAPER_ENV_C6:
	 wcscpy(PaperName,L"C6 Envelope,  114- by 162-millimeters");    break;
	case DMPAPER_ENV_C65:
	 wcscpy(PaperName,L"C65 Envelope, 114- by 229-millimeters");    break;
	case DMPAPER_ENV_B4:
	 wcscpy(PaperName,L"B4 Envelope,  250- by 353-millimeters");    break;
	case DMPAPER_ENV_B5:
	 wcscpy(PaperName,L"B5 Envelope,  176- by 250-millimeters");    break;
	case DMPAPER_ENV_B6:
	 wcscpy(PaperName,L"B6 Envelope,  176- by 125-millimeters");    break;
	case DMPAPER_ENV_ITALY:
	 wcscpy(PaperName,L"Italy Envelope, 110- by 230-millimeters");  break;
	case DMPAPER_ENV_MONARCH:
	 wcscpy(PaperName,L"Monarch Envelope, 3 7/8- by 7 1/2-inches"); break;
	case DMPAPER_ENV_PERSONAL:
	 wcscpy(PaperName,L"6 3/4 Envelope, 3 5/8- by 6 1/2-inches");   break;
	case DMPAPER_FANFOLD_US:
	 wcscpy(PaperName,L"US Std Fanfold, 14 7/8- by 11-inches");     break;
	case DMPAPER_FANFOLD_STD_GERMAN:
	 wcscpy(PaperName,L"German Std Fanfold, 8 1/2- by 12-inches");  break;
	case DMPAPER_FANFOLD_LGL_GERMAN:
	 wcscpy(PaperName,L"German Legal Fanfold, 8 1/2- by 13-inches");break;
 }}
 */
 }
 SaveOptPrn();
 Change=true; Close();
}
//---------------------------------------------------------------------------
void __fastcall TFormParamPrint::ToolButtonHelpClick(TObject *Sender){
 wchar_t *s; /*wcscpy(FileHelp,WORKDIR);
 if(s=wcsrchr(FileHelp,'\\')) wcscpy(s+1,L"Help\\ParamPrint.htm");
 else wcscat(FileHelp,L"\\Help\\ParamPrint.htm");*/
//***********************************************
 // В имени FileHelp уже есть нужный каталог Hlp-ов и надо только заменить
//соответствующее имя *.htm !!!!!!!! Так как было у тебя написано
//получается, что из WORKDIR удаляется рабочая папка (!!!)
//и получается путь: c:\Help\ParamPrint.htm!!!!
//Я думаю, что и у тебя такого пути НЕТ!!!
//***********************************************
  if(s=wcsrchr(FileHelp,'\\')) wcscpy(s+1,L"ParamPrint.htm");
  else wcscat(FileHelp,L"\\ParamPrint.htm");

 if(_waccess(FileHelp,0)){
  swprintf(Inf,TextFromFile(WORKDIR,L"ParamPrint",&SLP,4),FileHelp);
  Application->MessageBox(Inf,TextFromFile(WORKDIR,L"ParamPrint",&SLP,2),
   MB_OK|MB_ICONWARNING);
  return;
 }
 if(!FormOtchet)
  Application->CreateForm(__classid(TFormOtchet),&FormOtchet);
 FormOtchet->cbURL->Text=FileHelp;
 FormOtchet->ShowModal();
}
//---------------------------------------------------------------------------
void __fastcall TFormParamPrint::ToolButtonExitClick(TObject *Sender){
 Close();
}
//---------------------------------------------------------------------------
void __fastcall TFormParamPrint::EditTopKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift){
 if(Key==VK_RETURN) EditTopExit(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TFormParamPrint::EditLeftKeyDown(TObject *Sender,
      WORD &Key, TShiftState Shift){
 if(Key==VK_RETURN) EditLeftExit(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TFormParamPrint::EditBottomKeyDown(TObject *Sender,
      WORD &Key, TShiftState Shift){
 if(Key==VK_RETURN) EditBottomExit(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TFormParamPrint::EditRightKeyDown(TObject *Sender,
      WORD &Key, TShiftState Shift){
 if(Key==VK_RETURN) EditRightExit(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TFormParamPrint::EditRightExit(TObject *Sender){
 short OldPos=UpDownRight->Position;
 if(!EditRight->Text.IsEmpty()){
  short NewPos=_wtoi(EditRight->Text.c_str());
  if(NewPos<UpDownRight->Min){
   EditRight->Text=OldPos; return;
  }
  UpDownRight->Position=NewPos;
 }
 else{
  EditRight->Text=OldPos; EditRight->SetFocus();
}}
//---------------------------------------------------------------------------
void __fastcall TFormParamPrint::EditTopExit(TObject *Sender){
 short OldPos=UpDownTop->Position;
 if(!EditTop->Text.IsEmpty()){
  short NewPos=_wtoi(EditTop->Text.c_str());
  if(NewPos<UpDownTop->Min){
   EditTop->Text=OldPos; return;
  }
  UpDownTop->Position=NewPos;
 }
 else{
  EditTop->Text=OldPos; EditTop->SetFocus();
}}
//---------------------------------------------------------------------------
void __fastcall TFormParamPrint::EditLeftExit(TObject *Sender){
 short OldPos=UpDownLeft->Position;
 if(!EditLeft->Text.IsEmpty()){
  short NewPos=_wtoi(EditLeft->Text.c_str());
  if(NewPos<UpDownLeft->Min){
   EditLeft->Text=OldPos; return;
  }
  UpDownLeft->Position=NewPos;
 }
 else{
  EditLeft->Text=OldPos; EditLeft->SetFocus();
}}
//---------------------------------------------------------------------------
void __fastcall TFormParamPrint::EditBottomExit(TObject *Sender){
 short OldPos=UpDownBottom->Position;
 if(!EditBottom->Text.IsEmpty()){
  short NewPos=_wtoi(EditBottom->Text.c_str());
  if(NewPos<UpDownBottom->Min){
   EditBottom->Text=OldPos; return;
  }
  UpDownBottom->Position=NewPos;
 }
 else{
  EditBottom->Text=OldPos; EditBottom->SetFocus();
}}
//---------------------------------------------------------------------------
void __fastcall TFormParamPrint::EditBottomKeyPress(TObject *Sender,
      wchar_t &Key){
 if(isdigit(Key)){
  AnsiString Text=EditBottom->Text; Text+=Key;
  if(Text.ToInt()>UpDownBottom->Max) Key='\0';
  return;
 }
 if((Key!=VK_BACK)&&(Key!=VK_ESCAPE)&&(Key!=VK_RETURN))
  Key='\0';
}
//---------------------------------------------------------------------------
void __fastcall TFormParamPrint::EditRightKeyPress(TObject *Sender,
      wchar_t &Key){
 if(isdigit(Key)){
  AnsiString Text=EditRight->Text; Text+=Key;
  if(Text.ToInt()>UpDownRight->Max) Key='\0';
  return;
 }
 if((Key!=VK_BACK)&&(Key!=VK_ESCAPE)&&(Key!=VK_RETURN))
  Key='\0';
}
//---------------------------------------------------------------------------
void __fastcall TFormParamPrint::EditTopKeyPress(TObject *Sender,
 wchar_t &Key){
 if(isdigit(Key)){
  AnsiString Text=EditTop->Text; Text+=Key;
  if(Text.ToInt()>UpDownTop->Max) Key='\0';
  return;
 }
 if((Key!=VK_BACK)&&(Key!=VK_ESCAPE)&&(Key!=VK_RETURN))
  Key='\0';
}
//---------------------------------------------------------------------------
void __fastcall TFormParamPrint::EditLeftKeyPress(TObject *Sender,
      wchar_t &Key){
 if(isdigit(Key)){
  AnsiString Text=EditLeft->Text; Text+=Key;
  if(Text.ToInt()>UpDownLeft->Max) Key='\0';
  return;
 }
 if((Key!=VK_BACK)&&(Key!=VK_ESCAPE)&&(Key!=VK_RETURN))
  Key='\0';
}
//---------------------------------------------------------------------------
/*wchar_t *__fastcall TFormParamPrint::WhatDefPrn(wchar_t *S){
 PRINTDLG *pd=(PRINTDLG *)calloc(1,sizeof(PRINTDLG));
 BOOL Rez; DWORD ERR;
 DEVMODE DVM; DEVNAMES DVN;
// memset(&pd,0,sizeof(PRINTDLG));
 memset(&DVM,0,sizeof(DEVMODE));
 memset(&DVN,0,sizeof(DEVNAMES));
 pd->lStructSize=(unsigned long)(1000*sizeof(PRINTDLG));
 pd->hwndOwner=NULL; pd->Flags=PD_RETURNDEFAULT|PD_RETURNIC;
 pd->hwndOwner=Handle;
// pd.hDevMode=&DVM; pd.hDevNames=&DVN;
 Rez=PrintDlg(pd);
 ERR=CommDlgExtendedError();
 free(pd);
} */

//---------------------------------------------------------------------------
void __fastcall TFormParamPrint::ReadOptPrn(void) {
 FILE *F; wchar_t S[256],*s,FileOpt[256]; int i;
 Prn=Printer();
 _devicemodeW *DevMode;
 IsWin2000=Ver2000();
// WhatDefPrn(S);
 if(FilePrn){
  wcscpy(FileOpt,FilePrn);
  if(s=wcsrchr(FileOpt,'.')) wcscpy(s+1,L"popt");
  else wcscat(FileOpt,L".popt");
  if(_waccess(FileOpt,0)) wcscpy(FileOpt,L"EditPrn.popt");
  if(F=_wfopen(FileOpt,L"r")) {
   if(fgetws(s=S,255,F)) {
    if(s=wcschr(S,'#')) {
     for(s--;s>S&&*s==' ';s--); *(++s)='\0'; wcscpy(FontName,S);
   }}
   if(fgetws(S,255,F)) FontSize=_wtoi(S);
   if(fgetws(S,255,F)) FontColor=(TColor)_wtoi(S);
   if(fgetws(S,255,F)) MarginLeftmm=_wtoi(S);
   if(fgetws(S,255,F)) MarginRightmm=_wtoi(S);
   if(fgetws(S,255,F)) MarginTopmm=_wtoi(S);
   if(fgetws(S,255,F)) MarginBotmm=_wtoi(S);
   if(fgetws(S,255,F)) OrientKnig=_wtoi(S);
   if(fgetws(S,255,F)) Podch=_wtoi(S);
   if(fgetws(s=S,255,F)) {
    if(s=wcschr(S,'#')) {
     for(s--;s>S&&*s==' ';s--); *(++s)='\0'; wcscpy(PrintName,S);
   }}
   if(fgetws(s=S,255,F)) {
    if(s=wcschr(S,'#')) {
     for(s--;s>S&&*s==' ';s--); *(++s)='\0'; wcscpy(PaperName,S);
   }}
   if(fgetws(S,255,F)) Interval=_wtof(S);
   fclose(F);
 }}
 if(Interval<0.01) Interval=1.0;
 if(Prn&&PrintName){
  for(i=0;i<Prn->Printers->Count;i++){
   if(!wcscmp(Prn->Printers->Strings[i].c_str(),PrintName)){
    Prn->PrinterIndex=i; break;
 }}}
 DevMode=/*Prn?Prn->GetDevMode():*/NULL;
 if(DevMode){
  if(!wcscmp(PaperName,L"Letter, 8 1/2- by 11-inches")){
   DevMode->dmPaperSize=DMPAPER_LETTER; return;
  }
  if(!wcscmp(PaperName,L"Legal, 8 1/2- by 14-inches")){
   DevMode->dmPaperSize=DMPAPER_LEGAL; return;
  }
  if(!wcscmp(PaperName,L"A4 Sheet, 210- by 297-millimeters")){
   DevMode->dmPaperSize=DMPAPER_A4; return;
  }
  if(!wcscmp(PaperName,L"C Sheet, 17- by 22-inches")){
   DevMode->dmPaperSize=DMPAPER_CSHEET; return;
  }
  if(!wcscmp(PaperName,L"D Sheet, 22- by 34-inches")){
   DevMode->dmPaperSize=DMPAPER_DSHEET; return;
  }
  if(!wcscmp(PaperName,L"E Sheet, 34- by 44-inches")){
   DevMode->dmPaperSize=DMPAPER_ESHEET; return;
  }
  if(!wcscmp(PaperName,L"Letter Small, 8 1/2- by 11-inches")){
   DevMode->dmPaperSize=DMPAPER_LETTERSMALL; return;
  }
  if(!wcscmp(PaperName,L"Tabloid, 11- by 17-inches")){
   DevMode->dmPaperSize=DMPAPER_TABLOID; return;
  }
  if(!wcscmp(PaperName,L"Ledger, 17- by 11-inches")){
   DevMode->dmPaperSize=DMPAPER_LEDGER; return;
  }
  if(!wcscmp(PaperName,L"Statement, 5 1/2- by 8 1/2-inches")){
   DevMode->dmPaperSize=DMPAPER_STATEMENT; return;
  }
  if(!wcscmp(PaperName,L"Executive, 7 1/4- by 10 1/2-inches")){
   DevMode->dmPaperSize=DMPAPER_EXECUTIVE; return;
  }
  if(!wcscmp(PaperName,L"A3 sheet, 297- by 420-millimeters")){
   DevMode->dmPaperSize=DMPAPER_A3; return;
  }
  if(!wcscmp(PaperName,L"A4 small sheet, 210- by 297-millimeters")){
   DevMode->dmPaperSize=DMPAPER_A4SMALL; return;
  }
  if(!wcscmp(PaperName,L"A5 sheet, 148- by 210-millimeters")){
   DevMode->dmPaperSize=DMPAPER_A5; return;
  }
  if(!wcscmp(PaperName,L"B4 sheet, 250- by 354-millimeters")){
   DevMode->dmPaperSize=DMPAPER_B4; return;
  }
  if(!wcscmp(PaperName,L"B5 sheet, 182- by 257-millimeter paper")){
   DevMode->dmPaperSize=DMPAPER_B5; return;
  }
  if(!wcscmp(PaperName,L"Folio, 8 1/2- by 13-inch paper")){
   DevMode->dmPaperSize=DMPAPER_FOLIO; return;
  }
  if(!wcscmp(PaperName,L"Quarto, 215- by 275-millimeter paper")){
   DevMode->dmPaperSize=DMPAPER_QUARTO; return;
  }
  if(!wcscmp(PaperName,L"10- by 14-inch sheet")){
   DevMode->dmPaperSize=DMPAPER_10X14; return;
  }
  if(!wcscmp(PaperName,L"11- by 17-inch sheet")){
   DevMode->dmPaperSize=DMPAPER_11X17; return;
  }
  if(!wcscmp(PaperName,L"Note, 8 1/2- by 11-inches")){
   DevMode->dmPaperSize=DMPAPER_NOTE; return;
  }
  if(!wcscmp(PaperName,L"#9 Envelope, 3 7/8- by 8 7/8-inches")){
   DevMode->dmPaperSize=DMPAPER_ENV_9; return;
  }
  if(!wcscmp(PaperName,L"#10 Envelope, 4 1/8- by 9 1/2-inches")){
   DevMode->dmPaperSize=DMPAPER_ENV_10; return;
  }
  if(!wcscmp(PaperName,L"#11 Envelope, 4 1/2- by 10 3/8-inches")){
   DevMode->dmPaperSize=DMPAPER_ENV_11; return;
  }
  if(!wcscmp(PaperName,L"#12 Envelope, 4 3/4- by 11-inches")){
   DevMode->dmPaperSize=DMPAPER_ENV_12; return;
  }
  if(!wcscmp(PaperName,L"#14 Envelope, 5- by 11 1/2-inches")){
   DevMode->dmPaperSize=DMPAPER_ENV_14; return;
  }
  if(!wcscmp(PaperName,L"DL Envelope, 110- by 220-millimeters")){
   DevMode->dmPaperSize=DMPAPER_ENV_DL; return;
  }
  if(!wcscmp(PaperName,L"C5 Envelope, 162- by 229-millimeters")){
   DevMode->dmPaperSize=DMPAPER_ENV_C5; return;
  }
  if(!wcscmp(PaperName,L"C3 Envelope,  324- by 458-millimeters")){
   DevMode->dmPaperSize=DMPAPER_ENV_C3; return;
  }
  if(!wcscmp(PaperName,L"C4 Envelope,  229- by 324-millimeters")){
   DevMode->dmPaperSize=DMPAPER_ENV_C4; return;
  }
  if(!wcscmp(PaperName,L"C6 Envelope,  114- by 162-millimeters")){
   DevMode->dmPaperSize=DMPAPER_ENV_C6; return;
  }
  if(!wcscmp(PaperName,L"C65 Envelope, 114- by 229-millimeters")){
   DevMode->dmPaperSize=DMPAPER_ENV_C65; return;
  }
  if(!wcscmp(PaperName,L"B4 Envelope,  250- by 353-millimeters")){
   DevMode->dmPaperSize=DMPAPER_ENV_B4; return;
  }
  if(!wcscmp(PaperName,L"B5 Envelope,  176- by 250-millimeters")){
   DevMode->dmPaperSize=DMPAPER_ENV_B5; return;
  }
  if(!wcscmp(PaperName,L"B6 Envelope,  176- by 125-millimeters")){
   DevMode->dmPaperSize=DMPAPER_ENV_B6; return;
  }
  if(!wcscmp(PaperName,L"Italy Envelope, 110- by 230-millimeters")){
   DevMode->dmPaperSize=DMPAPER_ENV_ITALY; return;
  }
  if(!wcscmp(PaperName,L"Monarch Envelope, 3 7/8- by 7 1/2-inches")){
   DevMode->dmPaperSize=DMPAPER_ENV_MONARCH; return;
  }
  if(!wcscmp(PaperName,L"6 3/4 Envelope, 3 5/8- by 6 1/2-inches")){
   DevMode->dmPaperSize=DMPAPER_ENV_PERSONAL; return;
  }
  if(!wcscmp(PaperName,L"US Std Fanfold, 14 7/8- by 11-inches")){
   DevMode->dmPaperSize=DMPAPER_FANFOLD_US; return;
  }
  if(!wcscmp(PaperName,L"German Std Fanfold, 8 1/2- by 12-inches")){
   DevMode->dmPaperSize=DMPAPER_FANFOLD_STD_GERMAN; return;
  }
  if(!wcscmp(PaperName,L"German Legal Fanfold, 8 1/2- by 13-inches")){
   DevMode->dmPaperSize=DMPAPER_FANFOLD_LGL_GERMAN; return;
}}}
//---------------------------------------------------------------------------
void __fastcall TFormParamPrint::SaveOptPrn(void){
 wchar_t FileOpt[256],*s; FILE *F;
 wcscpy(FileOpt,FilePrn);
 if(s=wcsrchr(FileOpt,'.')) wcscpy(s+1,L"popt");
 else wcscat(FileOpt,L".popt");
 if(F=_wfopen(FileOpt,L"w")){
  swprintf(Inf,TextFromFile(WORKDIR,L"ParamPrint",&SLP,5),FontName); Fputs(Inf,F); fputwc('\n',F);
  swprintf(Inf,TextFromFile(WORKDIR,L"ParamPrint",&SLP,6),FontSize); Fputs(Inf,F); fputwc('\n',F);
  swprintf(Inf,TextFromFile(WORKDIR,L"ParamPrint",&SLP,7),FontColor); Fputs(Inf,F); fputwc('\n',F);
  swprintf(Inf,TextFromFile(WORKDIR,L"ParamPrint",&SLP,8),MarginLeftmm); Fputs(Inf,F); fputwc('\n',F);
  swprintf(Inf,TextFromFile(WORKDIR,L"ParamPrint",&SLP,9),MarginRightmm); Fputs(Inf,F); fputwc('\n',F);
  swprintf(Inf,TextFromFile(WORKDIR,L"ParamPrint",&SLP,10),MarginTopmm); Fputs(Inf,F); fputwc('\n',F);
  swprintf(Inf,TextFromFile(WORKDIR,L"ParamPrint",&SLP,11),MarginBotmm); Fputs(Inf,F); fputwc('\n',F);
  swprintf(Inf,TextFromFile(WORKDIR,L"ParamPrint",&SLP,12),OrientKnig); Fputs(Inf,F); fputwc('\n',F);
  swprintf(Inf,TextFromFile(WORKDIR,L"ParamPrint",&SLP,13),Podch); Fputs(Inf,F); fputwc('\n',F);
  swprintf(Inf,TextFromFile(WORKDIR,L"ParamPrint",&SLP,14),
   IsSetOrient(Prn)?PrintName:L" "); Fputs(Inf,F); fputc('\n',F);
  swprintf(Inf,TextFromFile(WORKDIR,L"ParamPrint",&SLP,15),PaperName); Fputs(Inf,F); fputwc('\n',F);
  swprintf(Inf,TextFromFile(WORKDIR,L"ParamPrint",&SLP,16),Interval); Fputs(Inf,F); fputwc('\n',F);
  fclose(F);
}}
//---------------------------------------------------------------------------
void __fastcall TFormParamPrint::Defines(){
 Prn=Printer();
 TakeParam();
 EditTop->Text=MarginTopmm;
 EditLeft->Text=MarginLeftmm;
 EditRight->Text=MarginRightmm;
 EditBottom->Text=MarginBotmm;
 FontDialog->Font->Charset=RUSSIAN_CHARSET;
 FontDialog->Font->Name=FontName;
 FontDialog->Font->Size=FontSize;
 FontDialog->Font->Color=FontColor;
 if(Podch){
  if(!FontDialog->Font->Style.Contains(fsUnderline))
   FontDialog->Font->Style=TFontStyles()<<fsUnderline;
 }
 else
  if(FontDialog->Font->Style.Contains(fsUnderline))
   FontDialog->Font->Style=TFontStyles()>>fsUnderline;
 swprintf(Inf,L"%0.2f",Interval); EditInt->Text=Inf;
 if(Prn->Printers->Count){
  Prn->Canvas->Font->Name=FontName;
  Prn->Canvas->Font->Size=FontSize;
  Prn->Canvas->Font->Color=FontColor;
  if(IsSetOrient(Prn))
   Prn->Orientation=OrientKnig?poPortrait:poLandscape;
  Prn->Title=FilePrn;
// if(OrientKnig) Prn->Orientation=poPortrait;
// else Prn->Orientation=poLandscape;
  LPx=GetDeviceCaps(Prn->Handle,LOGPIXELSX);
  LPy=GetDeviceCaps(Prn->Handle,LOGPIXELSY);
  PhW=GetDeviceCaps(Prn->Handle,PHYSICALWIDTH);
  PhH=GetDeviceCaps(Prn->Handle,PHYSICALHEIGHT);
  Wmm=GetDeviceCaps(Prn->Handle,HORZSIZE);
  Hmm=GetDeviceCaps(Prn->Handle,VERTSIZE);
  PhL=GetDeviceCaps(Prn->Handle,PHYSICALOFFSETX);
  PhT=GetDeviceCaps(Prn->Handle,PHYSICALOFFSETY);
  PhWmm=Round(PhW*25.4/LPx);
  PhHmm=Round(PhH*25.4/LPy);
  PhTmm=Round(PhT*25.4/LPy);
  PhBmm=PhHmm-PhTmm-Hmm;
  PhLmm=Round(PhL*25.4/LPx);
  PhRmm=PhWmm-PhLmm-Wmm;
 }
 UpDownTop->Position=MarginTopmm;
 UpDownLeft->Position=MarginLeftmm;
 UpDownRight->Position=MarginRightmm;
 UpDownBottom->Position=MarginBotmm;
}
//---------------------------------------------------------------------------
void __fastcall TFormParamPrint::FormShow(TObject *Sender){
 Prn=NULL;
 Height=GroupBoxMargin->Top+GroupBoxMargin->Height+ToolBar->Height+32;
 Width=CheckBoxPrinters->Left+CheckBoxPrinters->Width+24;
 Defines();
}
//---------------------------------------------------------------------------
void __fastcall TFormParamPrint::Button1Click(TObject *Sender){
 Magazine *Mf=NULL,*m; wchar_t *s,Maska[256],File[256]; _wffblk FB; FILE *Fi,*Fo;
 PrinterSetupDialog->Execute();
 swprintf(Maska,L"*.popt");
 for(int i=_wfindfirst(Maska,&FB,0);!i;i=_wfindnext(&FB))
  TakeMagazine(&Mf,FB.ff_name);
 for(m=Mf;m;m=m->Sled){
  wcscpy(File,m->S);
  if(s=wcsrchr(File,'.')) wcscpy(s,L".nopt"); else wcscat(File,L".nopt");
  if((Fi=_wfopen(m->S,L"r"))&&(Fo=_wfopen(File,L"w"))){
   for(int i=0;i<9;i++){ fgetws(Maska,255,Fi); fputws(Maska,Fo); }
   fgetws(Maska,255,Fi);
   swprintf(Inf,L"%-40s # Наименование принтера\n",
    Prn->Printers->Strings[FormParamPrint->Prn->PrinterIndex].c_str());
   Fputs(Inf,Fo);
   while(fgetws(Maska,255,Fi)) fputws(Maska,Fo);
   if(Fi) fclose(Fi); if(Fo) fclose(Fo);
   _wunlink(m->S);
   _wrename(File,m->S);
 }}
 DelAllMagazine(&Mf);
 swprintf(Inf,L"Принтер - %s сохранен в настройках печати",
  Prn->Printers->Strings[FormParamPrint->Prn->PrinterIndex].c_str());
 Application->MessageBox(Inf,L"Поздравляем!",MB_OK);
}
//---------------------------------------------------------------------------


