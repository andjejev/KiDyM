//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include <math.h>
#include "EmblemaKern.h"
#include "ConvDW.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormKidyMEmblema *FormKidyMEmblema;
//---------------------------------------------------------------------------
__fastcall TFormKidyMEmblema::TFormKidyMEmblema(TComponent* Owner):TForm(Owner){
 ColorShar=(TColor)(unsigned long)(0x4080FF);
 ColorLine=clBlue;
}
//---------------------------------------------------------------------------
void __fastcall TFormKidyMEmblema::DrawShar(TCanvas *Canvas,int X,int Y){
 Canvas->Pen->Color=ColorShar/2;
 Canvas->Ellipse(X-r,Y-r,X+r,Y+r);
 Canvas->Pen->Color=ColorShar;
 Canvas->Ellipse(X-14*r/16,Y-14*r/16,X+14*r/16,Y+14*r/16);
 Canvas->Pen->Color=(TColor)(unsigned long)(0x80C8FF);
 Canvas->Ellipse(X+3,Y-7,X+7,Y-3);
 Canvas->Pen->Color=(TColor)(unsigned long)(0xC0E3FF);
 Canvas->Ellipse(X+2,Y-8,X+8,Y-2);
}
//---------------------------------------------------------------------
void __fastcall TFormKidyMEmblema::DrawTetraedr(double Fi,double Psi,double Teta){
 double X1,Y1,Z1,X2,Y2,Z2,X3,Y3,Z3,X4,Y4,Z4,X,Y,Z,
  cos11,cos12,cos13,cos21,cos22,cos23,cos31,cos32,cos33,
  cosFi=cos(Fi),sinFi=sin(Fi),cosPsi=cos(Psi),sinPsi=sin(Psi),
  cosTeta=cos(Teta),sinTeta=sin(Teta);
 int X1c,X2c,X3c,X4c,Y1c,Y2c,Y3c,Y4c,
  X12,X21,Y12,Y21,X13,X31,Y13,Y31,X14,X41,Y14,Y41,
  X23,X32,Y23,Y32,X24,X42,Y24,Y42,X34,X43,Y34,Y43;
 cos11=cosPsi*cosTeta*cosFi-sinPsi*sinFi;
 cos12=-cosPsi*cosTeta*sinFi-sinPsi*cosFi;
 cos13=cosPsi*sinTeta;
 cos21=sinPsi*cosTeta*cosFi+cosPsi*sinFi;
 cos22=-sinPsi*cosTeta*sinFi+cosPsi*cosFi;
 cos23=sinPsi*sinTeta;
 cos31=-sinTeta*cosFi;
 cos32=sinTeta*sinFi;
 cos33=cosTeta;
 X1=x1*cos11+y1*cos12+z1*cos13;
 Y1=x1*cos21+y1*cos22+z1*cos23;
 Z1=x1*cos31+y1*cos32+z1*cos33;
 X2=x2*cos11+y2*cos12+z2*cos13;
 Y2=x2*cos21+y2*cos22+z2*cos23;
 Z2=x2*cos31+y2*cos32+z2*cos33;
 if(Z2>Z1){ X=X1; Y=Y1; Z=Z1; X1=X2; Y1=Y2; Z1=Z2; X2=X; Y2=Y; Z2=Z; }
 X3=x3*cos11+y3*cos12+z3*cos13;
 Y3=x3*cos21+y3*cos22+z3*cos23;
 Z3=x3*cos31+y3*cos32+z3*cos33;
 if(Z3>Z2){ X=X2; Y=Y2; Z=Z2; X2=X3; Y2=Y3; Z2=Z3; X3=X; Y3=Y; Z3=Z; }
 if(Z2>Z1){ X=X1; Y=Y1; Z=Z1; X1=X2; Y1=Y2; Z1=Z2; X2=X; Y2=Y; Z2=Z; }
 X4=x4*cos11+y4*cos12+z4*cos13;
 Y4=x4*cos21+y4*cos22+z4*cos23;
 Z4=x4*cos31+y4*cos32+z4*cos33;
 if(Z4>Z3){ X=X3; Y=Y3; Z=Z3; X3=X4; Y3=Y4; Z3=Z4; X4=X; Y4=Y; Z4=Z; }
 if(Z3>Z2){ X=X2; Y=Y2; Z=Z2; X2=X3; Y2=Y3; Z2=Z3; X3=X; Y3=Y; Z3=Z; }
 if(Z2>Z1){ X=X1; Y=Y1; Z=Z1; X1=X2; Y1=Y2; Z1=Z2; X2=X; Y2=Y; Z2=Z; }
 X1c=Xc+Round(X1); Y1c=Yc+Round(Y1); X2c=Xc+Round(X2);
 Y2c=Yc+Round(Y2); X3c=Xc+Round(X3); Y3c=Yc+Round(Y3);
 X4c=Xc+Round(X4); Y4c=Yc+Round(Y4);
 X12=Xc+Round(a*X2+b*X1); X21=Xc+Round(a*X1+b*X2);
 Y12=Yc+Round(a*Y2+b*Y1); Y21=Yc+Round(a*Y1+b*Y2);
 X13=Xc+Round(a*X3+b*X1); X31=Xc+Round(a*X1+b*X3);
 Y13=Yc+Round(a*Y3+b*Y1); Y31=Yc+Round(a*Y1+b*Y3);
 X14=Xc+Round(a*X4+b*X1); X41=Xc+Round(a*X1+b*X4);
 Y14=Yc+Round(a*Y4+b*Y1); Y41=Yc+Round(a*Y1+b*Y4);
 X23=Xc+Round(a*X3+b*X2); X32=Xc+Round(a*X2+b*X3);
 Y23=Yc+Round(a*Y3+b*Y2); Y32=Yc+Round(a*Y2+b*Y3);
 X24=Xc+Round(a*X4+b*X2); X42=Xc+Round(a*X2+b*X4);
 Y24=Yc+Round(a*Y4+b*Y2); Y42=Yc+Round(a*Y2+b*Y4);
 X34=Xc+Round(a*X4+b*X3); X43=Xc+Round(a*X3+b*X4);
 Y34=Yc+Round(a*Y4+b*Y3); Y43=Yc+Round(a*Y3+b*Y4);
 ProgramIcon->Canvas->CopyRect(ProgramIcon->ClientRect,
  BufferIcon->Canvas,BufferIcon->ClientRect);
 ProgramIcon->Canvas->Pen->Color=ColorShar;
 DrawShar(ProgramIcon->Canvas,X1c,Y1c);
 ProgramIcon->Canvas->Pen->Color=ColorLine;
 ProgramIcon->Canvas->Pen->Width=3;
 ProgramIcon->Canvas->MoveTo(X12,Y12);
 ProgramIcon->Canvas->LineTo(X21,Y21);
 ProgramIcon->Canvas->MoveTo(X13,Y13);
 ProgramIcon->Canvas->LineTo(X31,Y31);
 ProgramIcon->Canvas->MoveTo(X14,Y14);
 ProgramIcon->Canvas->LineTo(X41,Y41);
 ProgramIcon->Canvas->Pen->Color=ColorShar;
 DrawShar(ProgramIcon->Canvas,X2c,Y2c);
 ProgramIcon->Canvas->Pen->Color=ColorLine;
 ProgramIcon->Canvas->MoveTo(X23,Y23);
 ProgramIcon->Canvas->LineTo(X32,Y32);
 ProgramIcon->Canvas->MoveTo(X24,Y24);
 ProgramIcon->Canvas->LineTo(X42,Y42);
 ProgramIcon->Canvas->Pen->Color=ColorShar;
 DrawShar(ProgramIcon->Canvas,X3c,Y3c);
 ProgramIcon->Canvas->Pen->Color=ColorLine;
 ProgramIcon->Canvas->MoveTo(X34,Y34);
 ProgramIcon->Canvas->LineTo(X43,Y43);
 ProgramIcon->Canvas->Pen->Color=ColorShar;
 DrawShar(ProgramIcon->Canvas,X4c,Y4c);
 ProgramIcon->Canvas->Pen->Width=1;
}
//---------------------------------------------------------------------
void __fastcall TFormKidyMEmblema::FormShow(TObject *Sender){ char *s;
 t=0; dt=Timer->Interval; l=140; r=16; a=r/l; b=1-a;
 Width=420; Left=(Screen->Width-Width)/2; Top=455; Height=250;
 W=ProgramIcon->Width; H=ProgramIcon->Height;
 Xc=ProgramIcon->Width/2; Yc=ProgramIcon->Height/2;
 Xm[ 0]=W/5;     Ym[0]=15*H/16; Xm[ 1]=W/3;     Ym[1]=3*H/4;
 Xm[ 2]=W/2;     Ym[2]=2*H/3;   Xm[ 3]=5*W/8;   Ym[3]=2*H/3;
 Xm[ 4]=15*W/20; Ym[4]=42*H/50; Xm[ 5]=0.98*W;  Ym[5]=2*H/3;
 Xm[ 6]=15*W/16; Ym[6]=7*H/16;  Xm[ 7]=6*W/32;  Ym[7]=5*H/16;
 Xm[ 8]=12*W/40; Ym[8]=H/2;     Xm[ 9]=5*W/12;  Ym[9]=3*H/8;
 Xm[10]=W/3;     Ym[10]=3*H/16; Xm[11]=3*W/8;   Ym[11]=H/16;
 h=4.0*l*sqrt(3.0)/9.0;
 x1=x2=l*sqrt(3.0)/6.0; x3=-2.0*x1; x4=0.0;
 y1=-l/2.0; y2=-y1; y3=y4=0.0; z1=z2=z3=-h/4.0; z4=-3*z1;
 BufferIcon->Left=ProgramIcon->Left;
 BufferIcon->Top=ProgramIcon->Top;
 BufferIcon->Width=ProgramIcon->Width;
 BufferIcon->Height=ProgramIcon->Height;
 BufferIcon->Canvas->Brush->Color=clBlack;
 BufferIcon->Canvas->Pen->Color=clBlack;
 BufferIcon->Canvas->Rectangle(0,0,BufferIcon->Width,BufferIcon->Height);
 BufferIcon->Canvas->Brush->Color=clWhite;
 BufferIcon->Canvas->Pen->Color=clWhite;
 for(int i=0;i<12;i++){
  BufferIcon->Canvas->Pixels[Xm[i]-1][Ym[i]]=clWhite;
  BufferIcon->Canvas->Pixels[Xm[i]][Ym[i]]=clWhite;
  BufferIcon->Canvas->Pixels[Xm[i]+1][Ym[i]]=clWhite;
  BufferIcon->Canvas->Pixels[Xm[i]][Ym[i]-1]=clWhite;
  BufferIcon->Canvas->Pixels[Xm[i]][Ym[i]+1]=clWhite;
 }
 ProgramIcon->Canvas->Brush->Color=ColorShar;
 Timer->Enabled=true;
}
//---------------------------------------------------------------------------
void __fastcall TFormKidyMEmblema::TimerTimer(TObject *Sender){
 t+=0.001*dt;
 Fi  =2*M_PI*t/15.0; if(Fi  >2*M_PI) Fi  -=2*M_PI;
 Teta=2*M_PI*t/25.0; if(Teta>2*M_PI) Teta-=2*M_PI;
 Psi =2*M_PI*t/5.0;  if(Psi >2*M_PI) Psi -=2*M_PI;
 DrawTetraedr(Fi,Psi,Teta);
}
//---------------------------------------------------------------------------
void __fastcall TFormKidyMEmblema::FormKeyPress(TObject *Sender, char &Key){
 if(Key==VK_RETURN) Close();
}
//---------------------------------------------------------------------------

