//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "InvRecord.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormInvRec *FormInvRec;

int Tab[7][8]={
 {4, 1, 1, 1, 1, 1, 1, 5},
 {2, 2, 2, 2, 1, 1, 1, 2},
 {2, 2, 2, 2, 1, 1, 1, 2},
 {2, 2, 2, 2, 2, 2, 1, 2},
 {2, 2, 2, 2, 2, 2, 1, 2},
 {5, 1, 1, 1, 1, 1, 1, 3}
};
//---------------------------------------------------------------------------
int Index(Vagon *v){ char Buf[16];
 switch(v->ToStr(Buf)[0]){
  case '+': return 1;
  case '-': return 2;
  case '^': return 3;
  case '*': return 4;
  case '/': return 5;
  case '(': return 6;
  default: /*')'*/ return 7;
}}
//---------------------------------------------------------------------------
int Tabl(Vagon *N,Vagon *T){
 int i=0,j;
 if((N->Atr()==CNST)||(N->Atr()==VARY)) return 0;
 j=Index(N);
 if(T) i=Index(T);
 return Tab[i][j];
}
//---------------------------------------------------------------------------
__fastcall TFormInvRec::TFormInvRec(TComponent* Owner) : TForm(Owner){
}
//---------------------------------------------------------------------------
void __fastcall TFormInvRec::Button1Click(TObject *Sender){
 char *s,Text[1024],Buf[256],*b=Buf; Cnst *C; Vary *V; Znac *B;
 s=strcpy(Text,EditFormuls->Text.c_str());
 if(*s=='\0'){
  Application->MessageBoxA("Пустая строка","Ошибка",MB_OK);
  return;
 }
 if(*s=='-'){
  Application->MessageBoxA("Унарный минус не обрабатывается!","Ошибка",MB_OK);
  return;
 }
 while(*s){
  switch(*s){
   case ' ': s++; break;
   case '(':
    B=new Znac(*s++); NewYork.push(B);
    break;
   case ')':
   case '+': case '-': case '*': case '/': case '^':
    *b='\0'; b=Buf;
    if(Buf[0]){
     if(isdigit(Buf[0])){//цифра
      C=new Cnst(atof(Buf)); NewYork.push(C);
     }
    }
  }
 }
}
//---------------------------------------------------------------------------
