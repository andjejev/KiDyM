#ifndef SzFormatH
#define SzFormatH

#include <vcl.h>
#pragma hdrstop
#include <stdio.h>
#include <grids.hpp>
#include <buttons.hpp>
#include <Printers.hpp>

//---------------------------------------------------------------------------
class TKnopka{
 public:
  TKnopka *Sled;
 private:
  int Nn,Nk,Xn,Xk,Yn,Yk,WidthPrn;
 //Nn,Nk - нач. и конеч. номер столбцов под кнопкой (от 0);
 //Xn,Xk,Yn,Yk - координаты в символах левого верхнего и нижнего правого углов
 //WidthPrn - ширина кнопки при печати в пикселях
 TSpeedButton *B;
 public:
  TSpeedButton *GetB(void) {return B;};
  void SetXn(int N){Xn=N;};
  void SetYn(int N){Yn=N;};
  void SetXk(int K){Xk=K;};
  void SetYk(int K){Yk=K;};
  int  GetXn(void){return Xn;};
  int  GetXk(void){return Xk;};
  int  GetYk(void){return Yk;};
  void SetNn(int K){ Nn=K; B->GroupIndex=K+1; B->AllowAllUp=true; };
  void SetNk(int K){Nk=K;};
  int  GetNn(void){return Nn;};
  int  GetNk(void){return Nk;};
  int  GetYn(void){return Yn;};
  int  GetWidthPrn(void){ return WidthPrn; }
  void SetWidthPrn(int L){ WidthPrn=L; }
  void SetB(TTabControl *TabControl,wchar_t *S,int N);
  void SetColorB(TColor ColorPhonFix,TColor ColorText);
  int  GetLeft(){ return B->Left; };
  void SetLeft(int Left){ B->Left=Left; };
  void SetWidth(int Width){ B->Width=Width; };
  int  SetCoordHeight(void);
  int  SetCoordTop(int Top);
  void PrintKnopka(TPrinter *Prn,int L,int H,
       int X0,int Y0,int *Li,int xpn,int xpk);
  void DrawKnopka(TCanvas *Canvas,int L,int H,int X0,int Y0,
   int *Li,int xpn,int xpk,double Kx,double Ky,Magazine *Ms);
 ~TKnopka(){delete B;};
};
//---------------------------------------------------------------------------
class TShapka{
  TKnopka *R; wchar_t *FileSh;
 public:
  TShapka(wchar_t *File){
   R=NULL; wcscpy(FileSh=(wchar_t *)calloc(wcslen(File)+1,SzC),File);
  };
 ~TShapka();
  TKnopka *GetR(void){return R;};
  void DefWidthButtonPrn(TCanvas *CanvasPrn,double Ktw,Magazine *Ms);
  void DefWidthButtonPrn(int LB,Magazine *Ms);
  void DefWidthButtonColsPrn(int *Li,Magazine *Ms);
  void Fill(TTabControl *TabControl);
  void Color(TColor ColorPhonFix,TColor ColorText);
  void Ext(TStringGrid *StringGrid);
  void SetNnNk(void);
  int  SetHeightShapka(void);
  int  SetHeightShapkaPrn(int FontHeight);
  int  SetTopShapka(int Top);
  void SetLeft(TStringGrid *SG);
  void SetLeftExt(int Old,TStringGrid *SG);
  void ExtFromCol(int Col,TStringGrid *SG);
  void PrintShapka(TPrinter *Prn,int L,int H,int X0,int &Y0,int *Li,
        int xpn,int xpk);
  void DrawShapka(TCanvas *Canvas,int L,int H,int X0,int &Y0,int *Li,
        int NN,int NK,double Kx,double Ky,Magazine *Ms);
};
//---------------------------------------------------------------------------
int SizePole(wchar_t B);
//---------------------------------------------------------------------------
bool SizeFormatFild(void *Rout,wchar_t *Format,int *NSumm,
 FILE *FSh,int *MasF0,int **MasF,int **MasD0,int **MasD,
 int **MasL0,int **MasL,int **Size,wchar_t **SForm,wchar_t ***MasForm,
 int *KForm,int *HSh,int *LTab,TStringGrid *SG,int FixedCols,
 bool Show0);
//---------------------------------------------------------------------------
void Shapka(FILE *FSh,int KForm,int *MasD0,int *MasD,wchar_t **StrSh);
//---------------------------------------------------------------------------
void ShowShapka(TImage *Image,wchar_t **StrSh,int HSh,int LTab);
//---------------------------------------------------------------------------
void OutPutHead(FILE *FOut,wchar_t *Head,int LTab,int *KStr);
//---------------------------------------------------------------------------
#endif
