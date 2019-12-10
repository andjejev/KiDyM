//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include <tchar.h>
#include <io.h>
#include <stdio.h>
#include <dir.h>
//---------------------------------------------------------------------------
USEFORM("Kinema.cpp", FormKinema);
USEFORM("..\KiDyMCommon\Diagnostika\FormPrRE.cpp", FormPrintRichEdit);
USEFORM("..\KiDyMCommon\SA\Colors.cpp", FormColors);
USEFORM("..\KiDyMCommon\SA\OtchetForm.cpp", FormOtchet);
USEFORM("..\KiDyMCommon\TablW\ToPrn.cpp", FormToPrn);
USEFORM("..\KiDyMCommon\TablW\EditList.cpp", FormEditList);
USEFORM("..\KiDyMCommon\TablW\EditListPrn.cpp", FormEditListPrn);
USEFORM("..\KiDyMCommon\TablW\EditTextButton.cpp", FormEditTextButton);
USEFORM("..\KiDyMCommon\TablW\ParamPrint.cpp", FormParamPrint);
USEFORM("..\KiDyMCommon\TablW\SelectForMag.cpp", FormMag);
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
 String SS; char *s; int L=strlen(ARGS);
 if(!L){
  Application->MessageBox(
   L"При вызове программы следует передать ей имя файла с полным"
   L" путем с исходными данными",L"Ошибка",MB_OK|MB_ICONERROR);
  return 0;
 }
 if((s=strstr(ARGS,"Own"))||(s=strstr(ARGS,"OWN"))){
  *--s='\0'; OWN=true;
 }
 L=strlen(ARGS); //_wgetcwd(CurrDir,255);
 if(ARGS[L-1]=='\"') ARGS[L-1]='\0';
 if(ARGS[0]=='\"') SS=ARGS+1;
 else SS=ARGS;
 wcscpy(InpFile,SS.w_str());
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
