//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
//---------------------------------------------------------------------------
USEFORM("EmblemaKern.cpp", FormKidyMEmblema);
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE,HINSTANCE,LPSTR,int){
 try{
  Application->Initialize();
  Application->CreateForm(__classid(TFormKidyMEmblema), &FormKidyMEmblema);
                 Application->Run();
 }
 catch(Exception &exception){
  Application->ShowException(&exception);
 }
 catch(...){
  try{
   throw Exception("");
  }
  catch(Exception &exception){
   Application->ShowException(&exception);
 }}
 return 0;
}
//---------------------------------------------------------------------------
