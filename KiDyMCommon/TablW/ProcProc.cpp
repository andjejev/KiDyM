//---------------------------------------------------------------------------


#pragma hdrstop

#include "ProcProc.h"

#pragma package(smart_init)

//---------------------------------------------------------------------------
TProcProc::TProcProc(){
 I=NULL; VisibleProc=false; Min=Val=Proc=0; Max=100; Step=1;
 Phon=clBtnFace; Band=clBlue;
}
//---------------------------------------------------------------------------
void TProcProc::Init(TImage *Image,bool ShowProc,int MinVal,int MaxVal,int St,
  TColor CP,TColor CB){
 I=Image; VisibleProc=ShowProc; Min=Val=MinVal; Max=MaxVal; Step=St;
 Phon=CP; Band=CB;
}
//---------------------------------------------------------------------------
void TProcProc::Clear(){
 Min=0; Val=0; Step=1; 
 if(I){ I->Canvas->Brush->Color=Phon; I->Canvas->Rectangle(I->ClientRect); }
}
//---------------------------------------------------------------------------
void TProcProc::Clear(int Mx){
 Min=0; Val=0; Step=1; Max=Mx;
 if(I){ I->Canvas->Brush->Color=Phon; I->Canvas->Rectangle(I->ClientRect); }
}
//---------------------------------------------------------------------------
void TProcProc::Clear(int Mx,TColor B){
 Min=0; Val=0; Step=1; Max=Mx; Band=B;
 if(I){ I->Canvas->Brush->Color=Phon; I->Canvas->Rectangle(I->ClientRect); }
}
//---------------------------------------------------------------------------
void TProcProc::Add(){ int L,H;
 if(Val<=Min) Val=Min+Step; else if(Val<Max) Val+=Step; else return;
 L=Round((Val-Min)*I->ClientWidth/(Max-Min));
 I->Canvas->Rectangle(0,0,L,I->ClientHeight-1);
 if(VisibleProc){
  Proc=Round((Val-Min)*100/(Max-Min)); sprintf(Text,"%d %%",Proc);
  L=I->Canvas->TextWidth(Text); H=I->Canvas->TextHeight(Text);
  I->Canvas->TextOutA((I->ClientWidth-L)/2,(I->ClientHeight-H)/2,Text);
}}
//---------------------------------------------------------------------------
void TProcProc::Value(int V){ int L,H;
 if(Val<=Min) Val=Min; else if(Val>Max) Val=Max;
 L=Round((Val-Min)*I->ClientWidth/(Max-Min));
 I->Canvas->Rectangle(0,0,L,I->ClientHeight-1);
 if(VisibleProc){
  Proc=Round((Val-Min)*100/(Max-Min)); sprintf(Text,"%d %%",Proc);
  L=I->Canvas->TextWidth(Text); H=I->Canvas->TextHeight(Text);
  I->Canvas->TextOutA((I->ClientWidth-L)/2,(I->ClientHeight-H)/2,Text);
}}
//---------------------------------------------------------------------------

