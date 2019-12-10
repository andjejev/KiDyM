//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "RTF.h"
#include <stdio.h>
#include "ConvDW.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormRTF *FormRTF;
//---------------------------------------------------------------------------
__fastcall TFormRTF::TFormRTF(TComponent* Owner):TForm(Owner){}
//---------------------------------------------------------------------------
void __fastcall TFormRTF::FormShow(TObject *Sender){
 FILE *F; char S[256],*s,Buf[256],B; double Kf=1.0; int PosB=-1; Buf[1]='\0';
 Caption=File; RichEdit->Text="\0";
 RichEdit->Clear();
 if(F=fopen(File,"r")){
  while(fgets(S,255,F)){
   for(s=S;*s;s++){
    if(*s!='\x1B'){
     B=(char)ConvChrDOStoWIN(*s);
     RichEdit->Text=RichEdit->Text+B;
     RichEdit->SelStart=RichEdit->Text.Length();
    }
    else{
     if(s[1]) s++;
     switch(*s){
      case 'E': PosB=RichEdit->Text.Length();
       RichEdit->SelAttributes->Style=
        RichEdit->SelAttributes->Style << fsBold;   break;
      case 'F':
       if(PosB>-1){
        RichEdit->SelStart=PosB;
        RichEdit->SelLength=RichEdit->Text.Length()-PosB;
        RichEdit->SelAttributes->Style=
         RichEdit->SelAttributes->Style << fsBold;
        PosB=-1;
       }
       break;
      case '4':
       RichEdit->SelAttributes->Style=
        RichEdit->SelAttributes->Style << fsItalic;
       break;
      case '5':
       RichEdit->SelAttributes->Style=
        RichEdit->SelAttributes->Style >> fsItalic;
       break;
      case 'g':
       RichEdit->SelAttributes->Size=8;
       break;
      case 'P':
       RichEdit->SelAttributes->Size=12;
       break;
      case 'w':
       if(s[1]){ s++; RichEdit->SelAttributes->Size=*s=='1'?24:12; }
       break;
      case 'W':
       if(s[1]){ s++; RichEdit->SelAttributes->Size=*s=='1'?24:12; }
       break;
      case '@':
       RichEdit->SelAttributes->Name="Times New Roman";
       RichEdit->SelAttributes->Size=12;
       break;
  }}}}
  fclose(F);
}}
//---------------------------------------------------------------------------
void __fastcall TFormRTF::tbExitClick(TObject *Sender){
 Close();
}
//---------------------------------------------------------------------------

