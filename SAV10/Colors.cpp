//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Colors.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormColors *FormColors;
static int X,Y,X1,Y1,X2,Y2;

//---------------------------------------------------------------------------
__fastcall TFormColors::TFormColors(TComponent* Owner):TForm(Owner){
 Left=15; 
}
//---------------------------------------------------------------------------
void __fastcall TFormColors::ButtonHeadClick(TObject *Sender){
 ColorDialog->Color=ColorHead;
 if(ColorDialog->Execute()){
  ColorHead=ColorDialog->Color;
  ShowColorHead();
}}
//---------------------------------------------------------------------------
void __fastcall TFormColors::ButtonOsClick(TObject *Sender){
 ColorDialog->Color=ColorOs;
 if(ColorDialog->Execute()){
  ColorOs=ColorDialog->Color;
  ShowColorOs();
}}
//---------------------------------------------------------------------------
void __fastcall TFormColors::ButtonDigitClick(TObject *Sender){
 ColorDialog->Color=ColorDigit;
 if(ColorDialog->Execute()){
  ColorDigit=ColorDialog->Color;
  ShowColorDigit();
}}
//---------------------------------------------------------------------------
void __fastcall TFormColors::ButtonTablClick(TObject *Sender){
 int X1,Y1,X2,Y2;
 ColorDialog->Color=ColorTabl;
 if(ColorDialog->Execute()){
  ColorTabl=ColorDialog->Color;
  ShowColorTabl();
}}
//---------------------------------------------------------------------------
void __fastcall TFormColors::ButtonSetkaClick(TObject *Sender){
 int X1,Y1,X2,Y2;
 ColorDialog->Color=ColorSetka;
 if(ColorDialog->Execute()){
  ColorSetka=ColorDialog->Color;
  ShowColorSetka();
}}
//---------------------------------------------------------------------------
void __fastcall TFormColors::ButtonRamkaClick(TObject *Sender){
 int X1,Y1,X2,Y2;
 ColorDialog->Color=ColorRamka;
 if(ColorDialog->Execute()){
  ColorRamka=ColorDialog->Color;
  ShowColorRamka();
}}
//---------------------------------------------------------------------------
void __fastcall TFormColors::ButtonGrClick(TObject *Sender){
 int X1,Y1,X2,Y2;
 ColorDialog->Color=ColorGr;
 if(ColorDialog->Execute()){
  ColorGr=ColorDialog->Color;
  ShowColorGr(ImageGr,ColorGr);
}}
//---------------------------------------------------------------------------
void __fastcall TFormColors::ButtonGr2Click(TObject *Sender){
 int X1,Y1,X2,Y2;
 ColorDialog->Color=ColorGr2;
 if(ColorDialog->Execute()){
  ColorGr2=ColorDialog->Color;
  ShowColorGr(ImageGr2,ColorGr2);
}}
//---------------------------------------------------------------------------
void __fastcall TFormColors::ButtonGr3Click(TObject *Sender){
 int X1,Y1,X2,Y2;
 ColorDialog->Color=ColorGr3;
 if(ColorDialog->Execute()){
  ColorGr3=ColorDialog->Color;
  ShowColorGr(ImageGr3,ColorGr3);
}}
//---------------------------------------------------------------------------
void __fastcall TFormColors::ButtonGr4Click(TObject *Sender){
 int X1,Y1,X2,Y2;
 ColorDialog->Color=ColorGr4;
 if(ColorDialog->Execute()){
  ColorGr4=ColorDialog->Color;
  ShowColorGr(ImageGr4,ColorGr4);
}}
//---------------------------------------------------------------------------
void __fastcall TFormColors::ButtonGr5Click(TObject *Sender){
 int X1,Y1,X2,Y2;
 ColorDialog->Color=ColorGr5;
 if(ColorDialog->Execute()){
  ColorGr5=ColorDialog->Color;
  ShowColorGr(ImageGr5,ColorGr5);
}}
//---------------------------------------------------------------------------
void __fastcall TFormColors::ButtonPhonClick(TObject *Sender){
 ColorDialog->Color=ColorPhon;
 if(ColorDialog->Execute()){
  ColorPhon=ColorDialog->Color;
  ShowColorPhon();
}}
//---------------------------------------------------------------------------
void __fastcall TFormColors::ButtonCancelClick(TObject *Sender){
 Close(); ModalResult=-1;
}
//---------------------------------------------------------------------------
void __fastcall TFormColors::ButtonOKClick(TObject *Sender){
 SetColors(); Close(); ModalResult=1;
}
//---------------------------------------------------------------------------
void __fastcall TFormColors::ShowColorHead(){
 ImageHead->Canvas->Brush->Color=clWhite;
 ImageHead->Canvas->Pen->Color=clBlack;
 ImageHead->Canvas->Rectangle(ImageHead->ClientRect);
 ImageHead->Canvas->Font->Assign(Font);
 ImageHead->Canvas->Font->Color=ColorHead;
 X=(ImageHead->ClientWidth-ImageHead->Canvas->TextWidth("ÇÀÃÎËÎÂÎÊ"))/2;
 Y=(ImageHead->ClientHeight-ImageHead->Canvas->TextHeight("Ç"))/2;
 ImageHead->Canvas->TextOut(X,Y,"ÇÀÃÎËÎÂÎÊ");
}
//---------------------------------------------------------------------------
void __fastcall TFormColors::ShowColorOs(){
 ImageOs->Canvas->Brush->Color=clWhite;
 ImageOs->Canvas->Pen->Color=clBlack;
 ImageOs->Canvas->Rectangle(ImageOs->ClientRect);
 ImageOs->Canvas->Font->Assign(Font);
 ImageOs->Canvas->Font->Color=ColorOs;
 X=(ImageOs->ClientWidth-ImageOs->Canvas->TextWidth("Íàçâàíèÿ îñåé"))/2;
 Y=(ImageOs->ClientHeight-ImageOs->Canvas->TextHeight("Í"))/2;
 ImageOs->Canvas->TextOut(X,Y,"Íàçâàíèÿ îñåé");
}
//---------------------------------------------------------------------------
void __fastcall TFormColors::ShowColorTabl(){
 ImageTabl->Canvas->Brush->Color=clWhite;
 ImageTabl->Canvas->Pen->Color=clBlack;
 ImageTabl->Canvas->Pen->Width=1;
 ImageTabl->Canvas->Rectangle(ImageTabl->ClientRect);
 ImageTabl->Canvas->Pen->Color=ColorTabl;
 X1=0; Y1=Y2=ImageTabl->ClientHeight/2; X2=ImageTabl->ClientWidth;
 ImageTabl->Canvas->MoveTo(X1,Y1); ImageTabl->Canvas->LineTo(X2,Y2);
 X1=ImageTabl->ClientWidth/3; Y1=ImageTabl->ClientHeight-1;
 for(int i=0;i<3;i++){
  ImageTabl->Canvas->MoveTo(i*X1,1); ImageTabl->Canvas->LineTo(i*X1,Y1);
}}
//---------------------------------------------------------------------------
void __fastcall TFormColors::ShowColorSetka(){
 ImageSetka->Canvas->Brush->Color=ColorPhon;
 ImageSetka->Canvas->Pen->Color=clBlack;
 ImageSetka->Canvas->Pen->Width=1;
 ImageSetka->Canvas->Rectangle(ImageSetka->ClientRect);
 ImageSetka->Canvas->Pen->Color=ColorSetka;
 X1=0; Y1=Y2=ImageSetka->ClientHeight/2; X2=ImageSetka->ClientWidth;
 ImageSetka->Canvas->MoveTo(X1,Y1); ImageSetka->Canvas->LineTo(X2,Y2);
 for(int i=0;i<7;i++){
  ImageSetka->Canvas->MoveTo((i+1)*Y1,1);
  ImageSetka->Canvas->LineTo((i+1)*Y1,ImageSetka->ClientHeight-1);
}}
//---------------------------------------------------------------------------
void __fastcall TFormColors::ShowColorDigit(){
 ImageDigit->Canvas->Brush->Color=clWhite;
 ImageDigit->Canvas->Pen->Color=clBlack;
 ImageDigit->Canvas->Rectangle(ImageDigit->ClientRect);
 ImageDigit->Canvas->Font->Assign(Font);
 ImageDigit->Canvas->Font->Color=ColorDigit;
 X=(ImageDigit->ClientWidth-ImageDigit->Canvas->TextWidth("1234567890"))/2;
 Y=(ImageDigit->ClientHeight-ImageDigit->Canvas->TextHeight("1"))/2;
 ImageDigit->Canvas->TextOut(X,Y,"1234567890");
}
//---------------------------------------------------------------------------
void __fastcall TFormColors::ShowColorRamka(){
 ImageRamka->Canvas->Brush->Color=ColorPhon;
 ImageRamka->Canvas->Pen->Color=ColorRamka;
 ImageRamka->Canvas->Pen->Width=4;
 ImageRamka->Canvas->Rectangle(ImageRamka->ClientRect);
}
//---------------------------------------------------------------------------
void __fastcall TFormColors::ShowColorPhon(){
 ImagePhon->Canvas->Pen->Color=clBlack;
 ImagePhon->Canvas->Brush->Color=ColorPhon;
 ImagePhon->Canvas->Rectangle(ImagePhon->ClientRect);
 ShowColorSetka(); ShowColorRamka();
 ShowColorGr(ImageGr,ColorGr);   ShowColorGr(ImageGr2,ColorGr2);
 ShowColorGr(ImageGr3,ColorGr3); ShowColorGr(ImageGr4,ColorGr4);
 ShowColorGr(ImageGr5,ColorGr5);
}
//---------------------------------------------------------------------------
void __fastcall TFormColors::ShowColorGr(TImage *Image,TColor Color){
 Image->Canvas->Brush->Color=ColorPhon;
 Image->Canvas->Pen->Color=clBlack;
 Image->Canvas->Pen->Width=1;
 Image->Canvas->Rectangle(Image->ClientRect);
 Image->Canvas->Pen->Color=Color;
 Image->Canvas->Pen->Width=3;
 X1=Y1=Y2=Image->ClientHeight/2; X2=Image->ClientWidth-X1;
 Image->Canvas->MoveTo(X1,Y1); Image->Canvas->LineTo(X2,Y2);
}
//---------------------------------------------------------------------------
void __fastcall TFormColors::FormShow(TObject *Sender){
 ShowColorHead(); ShowColorOs(); ShowColorTabl(); ShowColorSetka();
 ShowColorDigit(); ShowColorRamka(); ShowColorPhon();
 ShowColorGr(ImageGr,ColorGr);   ShowColorGr(ImageGr2,ColorGr2);
 ShowColorGr(ImageGr3,ColorGr3); ShowColorGr(ImageGr4,ColorGr4);
 ShowColorGr(ImageGr5,ColorGr5);
}
//---------------------------------------------------------------------------


