//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include <tchar.h>
#include <io.h>
#include <stdio.h>
//---------------------------------------------------------------------------
USEFORM("..\..\..\..\Весенний семестр 2019\Проекты\Башевский Д.И-24б\ForEditKDMUniCod\TablW\ParamPrint.cpp", FormParamPrint);
USEFORM("..\..\..\..\Весенний семестр 2019\Проекты\Башевский Д.И-24б\ForEditKDMUniCod\TablW\SelectForMag.cpp", FormMag);
USEFORM("Kinema.cpp", FormKinema);
USEFORM("..\..\..\..\Весенний семестр 2019\Проекты\Башевский Д.И-24б\ForEditKDMUniCod\TablW\ToPrn.cpp", FormToPrn);
USEFORM("..\..\..\..\Весенний семестр 2019\Проекты\Башевский Д.И-24б\ForEditKDMUniCod\SA\Colors.cpp", FormColors);
USEFORM("..\..\..\..\Весенний семестр 2019\Проекты\Башевский Д.И-24б\ForEditKDMUniCod\Diagnostika\FormPrRE.cpp", FormPrintRichEdit);
USEFORM("..\..\..\..\Весенний семестр 2019\Проекты\Башевский Д.И-24б\ForEditKDMUniCod\SA\OtchetForm.cpp", FormOtchet);
//---------------------------------------------------------------------------
USEFORM("..\..\..\..\Весенний семестр 2019\Проекты\Башевский Д.И-24б\\"
 "ForEditKDMUniCod\Diagnostika\FormPrRE.cpp", FormPrintRichEdit);
//---------------------------------------------------------------------------
wchar_t InpFile[256],ErrFile[256],OutFile[256],OprFile[256],
 DgsFile[256],Inf[4096],TmpFile[256],WORKDIR[256],HtmFile[256],
 CurrDir[256],BMPFile[256],JPGFile[256];
extern bool IsError; extern wchar_t LANGUAGELITERA;
bool OWN;
//---------------------------------------------------------------------------
WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPSTR ARGS, int){
 char *s,SS[256]; int L=strlen(ARGS); String S;
 if(!L){
  Application->MessageBox(
   L"При вызове программы следует передать ей имя файла с полным"
   L" путем с исходными данными",L"Ошибка",MB_OK|MB_ICONERROR);
  return 0;
 }
 if(ARGS[0]=='\"'){ L--; strcpy(SS,ARGS+1); } else strcpy(SS,ARGS);
 if((s=strstr(SS," Own"))||
	(s=strstr(SS," OWN"))||(s=strstr(SS," Own"))){
  *s='\0'; L-=4; OWN=true;
 }
 if(SS[L-1]=='\"') SS[L-1]='\0';
 S=SS;
 wcscpy(InpFile,S.w_str());
 if(_waccess(InpFile,0)){
  swprintf(Inf,L"Файл \"%s\" не существует!",InpFile);
  Application->MessageBox(Inf,L"Ошибка",MB_OK|MB_ICONERROR);
  return 0;
 }
 try{
  Application->Initialize();
  Application->MainFormOnTaskBar = true;
  Application->CreateForm(__classid(TFormKinema), &FormKinema);
		Application->Run();
 }
 catch (Exception &exception){
  Application->ShowException(&exception);
 }
 catch(...){
  try{
   throw Exception("");
  }
  catch (Exception &exception){
   Application->ShowException(&exception);
 }}
 return 0;
}
//---------------------------------------------------------------------------
