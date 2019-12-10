//---------------------------------------------------------------------------

#pragma hdrstop

#include <types.hpp>
#include <comctrls.hpp>
#include <vcl.h>
#include "ConvDW.h"
#include "Termometr.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

extern char Inf[];

TTermometr::TTermometr(TColor Clr,TColor ForeClr,TStatusBar *StBar){
 Min=0; Max=100; Pos=Min; Stp=1; Color=Clr; ForeColor=ForeClr;
 StatusBar=StBar; Image=NULL;
// Image=new TImage(StatusBar);
// Image->Parent=StatusBar; Image->Visible=false;
// Image->Canvas->Font->Assign(StatusBar->Canvas->Font);
// Image->Canvas->Font->Color=clBlack;
// Image->Canvas->Pen->Color=clBlack;
// Image->Canvas->Brush->Color=clWhite;
// Image->Width=Image->Canvas->TextWidth("99%");
// Image->Height=StatusBar->ClientHeight-2;
};
//---------------------------------------------------------------------------
void TTermometr::StepIn(){
 char Proc[16]; AnsiString TextN;
 if(Pos==Min){
  StatusBar->Canvas->Brush->Style=bsSolid;
  StatusBar->Canvas->Brush->Color=Color;
  StatusBar->Canvas->Pen->Color=Color;
  R.Top=3; R.Bottom=StatusBar->ClientHeight-1; W=200;
  R.Left=StatusBar->ClientWidth-W-1;
//  Rt.Top=R.Top; Rt.Bottom=R.Bottom;
//  Rt.Left=(StatusBar->ClientWidth-Image->Width)/2;
//  Rt.Right=Rt.Left+Image->Width;
  Color=StatusBar->Canvas->Brush->Color;
  StatusBar->Canvas->Brush->Color=ForeColor;
  StatusBar->Canvas->Pen->Color=ForeColor;
//  Image->Canvas->TextOut(0,0,"0%");
//  Image->Canvas->CopyMode=cmSrcCopy;
//  StatusBar->Canvas->CopyRect(Rt,StatusBar->Canvas,Image->ClientRect);
  Prc=PrcOld=0;
  Text=StatusBar->SimpleText; //TextNew="";
  StatusBar->SimpleText="";
//  TextNew=Text+", 100%";
  strcpy(Proc,", 100%");
  TextN=Text+Proc;
//  l=StatusBar->ClientWidth-StatusBar->Canvas->TextWidth(TextNew)-W-8;
  l=StatusBar->ClientWidth-StatusBar->Canvas->TextWidth(TextN)-W-8;
  StatusBar->Canvas->Brush->Color=Color;
//  StatusBar->Canvas->TextOut(l,3,TextNew);
  StatusBar->Canvas->TextOut(l,3,TextN);
 }
 Pos+=Stp;
 if(Pos>Min){
  if(Pos>Max){
//   StatusBar->Canvas->Brush->Color=Color;
//   StatusBar->Canvas->Pen->Color=Color;
//   R.Right=R.Left+W;
//   StatusBar->Canvas->Rectangle(R);
   StatusBar->SimpleText="";
  }
  else{
   Prc=Round((Pos-Min)*100/(Max-Min));
   if(Prc>PrcOld){
//    int d=(W-StatusBar->Canvas->TextWidth(Proc))/2;
    R.Right=R.Left+Round((double)(Pos-Min)*W/(Max-Min));
    StatusBar->Canvas->Brush->Color=ForeColor;
    StatusBar->Canvas->Rectangle(R);
    StatusBar->Canvas->Brush->Color=Color;
//    Image->Canvas->CopyMode=cmSrcInvert;
//    StatusBar->Canvas->CopyRect(Rt,StatusBar->Canvas,Image->ClientRect);
    sprintf(Proc,"%d%%",Prc); PrcOld=Prc;
//    Image->Canvas->TextOut(0,0,Proc);
//    StatusBar->Canvas->CopyRect(Rt,StatusBar->Canvas,Image->ClientRect);
//    TextNew=Text+(Prc>99?", ":Prc>9?",   ":",     ")+Proc;
    TextN=Text+(Prc>99?", ":Prc>9?",   ":",     ")+Proc;
//    StatusBar->Canvas->TextOut(l,3,TextNew);
    StatusBar->Canvas->TextOut(l,3,TextN);
//   StatusBar->Canvas->Font->Color=clBlack;
//   StatusBar->Canvas->TextOut(R.Left+d,R.Top,Proc);
//   StatusBar->Canvas->Pen->Mode=pmCopy;
//   StatusBar->Canvas->Pen->Color=ForeColor;
}}}}
//---------------------------------------------------------------------------

