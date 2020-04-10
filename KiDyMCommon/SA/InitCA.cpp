#include "CA.h"
#include "ConvDW.h"
#include <alloc.h>
#include <string.h>
#include <math.h>
#include <values.h>

Magazine *MUnit;
extern wchar_t *ZNAC,SymbMC,SymbMQ,SymbMV,SymbMM;

bool IsError,IsSyntError,ISRAND;
int N,MAXFUNC=20,MAXZNAC=11,NzK,NPV;
wchar_t AtrInerc,AtrDisip,AtrElast,AtrForse,AtrNelin,AtrLiner,
 COD[16],BASEUNIT[16],
 *NameFunc[] = {
   L"_",L"-",L"sqrt",L"sin",L"cos",L"tg",L"arcsin",L"arctg",
   L"exp",L"ln",L"sh",L"ch",L"arsh",L"abs",L"rand",L"ceil",
   L"floor",L"arccos",L"!",L"~",
  },
// *NameCnst[] = {
//   L"_inf",L"pi",L"2PI",L"3PI",L"4PI",L"5PI",L"6PI",L"7PI",L"PI",
//   L"PIna2",L"PIna3",L"PIna4",L"PIna6",L"PIna8",L"PIna10",
//   L"PIna12",L"exp",L"3",L"2",L"1",L"0.5",L"0",L"sqrt(2)",
//   L"sqrt(3)",L"sqrt(5)",L"\0",
//  };
 *NameCnst[] = {
   L"_inf",L"inf",L"pi",L"exp",L"3",L"2",L"1",L"0.5",L"0",L"\0",
  };
List *LBeg,*L;
//Cnst
// *_infinity,*infinity,*PI,*DvaPI,*TriPI,*FourPI,*FivePI,*SixPI,
// *SevenPI,*PIna2,*PIna3,*PIna4,*PIna6,*PIna8,*PIna10,*PIna12,
// *Exp,*Tri,*Dva,*Odin,*Halb,*pi,*Nul,*Sqrt2,*Sqrt3,*Sqrt5,*Pust;
Cnst
 *_infinity,*infinity,*PI,*Exp,*Tri,*Dva,*Odin,*Halb,*pi,*Nul,*Pust;
static Vary *G;
//------------------------------------------------------------------------
Cnst *InitCnst (int Atr,Cnst *Sled,wchar_t *Name,double D) {
 Cnst *C=(Cnst *)calloc(1,sizeof(Cnst));
 C->Atr=Atr; C->Sled=Sled; C->Name=Name; C->Val=D; return C;
}
//------------------------------------------------------------------------
void CodElem(wchar_t *Cod) {
 wcscpy(COD,Cod);
 AtrInerc=Cod[0]; AtrDisip=Cod[1]; AtrElast=Cod[2];
 AtrForse=Cod[3]; AtrNelin=Cod[4];
 if(wcslen(Cod)>5) AtrLiner=Cod[5];
}
//------------------------------------------------------------------------
void BaseUnit(wchar_t *Units){ wchar_t *s,*s0;
 wcscpy(BASEUNIT,Units);
 for(s=s0=Units;*s0;s0++){
  if(*s0==' '){ *s0='\0';
  TakeMagazine(&MUnit,s);
  *s0=' '; s=s0+1;
 }}
 TakeMagazine(&MUnit,s);
}
//------------------------------------------------------------------------
void InitCA(void){ //Vary *V;
 ZNAC=(wchar_t *)calloc(MAXZNAC+1,sizeof(wchar_t));
 wcscpy(ZNAC,L" +-*/%^");
 ZNAC[7]=SymbMQ; ZNAC[8]=SymbMM; ZNAC[9]=SymbMV; ZNAC[10]=SymbMC;
 if(!Pust){
  Pust=(Cnst *)calloc(1,sizeof(Cnst));
  Pust->Atr=PUST; Pust->Name=(wchar_t *)calloc(1,sizeof(wchar_t));
  Pust->Val=0.0;
  Pust->Sled=_infinity=(Cnst *)calloc(1,sizeof(Cnst));
  _infinity->Atr=CNST;
  _infinity->Name=(wchar_t *)calloc(5,sizeof(wchar_t));
  wcscpy(_infinity->Name,NameCnst[0]);
  _infinity->Val=-1.797e308;//MAXDOUBLE;
  infinity=TakeCnst(NameCnst[1],1.797e308/*MAXDOUBLE*/);
  pi    = TakeCnst(NameCnst[2],M_PI  );
  ::Exp = TakeCnst(NameCnst[3],M_E   );
  Tri   = TakeCnst(NameCnst[4],3.0);
  Dva   = TakeCnst(NameCnst[5],2.0);
  Odin  = TakeCnst(NameCnst[6],1.0);
  Halb  = TakeCnst(NameCnst[7],0.5);
  Nul   = TakeCnst(NameCnst[8],0.0);
 }
 CodElem(L"JDCPNL");
 BaseUnit(L"кг м с");
 if(!L) LBeg=L=TakeList(L"ќ—Ќќ¬Ќќ…");
}
//------------------------------------------------------------------------
bool SpecSimb(wchar_t C){
 if(C=='\x1B'||C=='\t') return true; return false;
}
//------------------------------------------------------------------------
wchar_t *RichAttr(TRichEdit *R,wchar_t *S){ wchar_t Buf[512];
 if(*S=='\t'){//красна€ строка
  if(R->Font->Pitch!=TFontPitch::fpFixed)
   swprintf(Buf,L"      %s",S+1);
  else  swprintf(Buf,L"   %s",S+1);
  wcscpy(S,Buf);
 }
 else if(*S=='\x1B'){//Esc-последовательность
  S++;
  switch(*S++){
   case 'E':
    if(!R->SelAttributes->Style.Contains(fsBold))
     R->SelAttributes->Style=R->SelAttributes->Style<<fsBold;
    break;
   case 'F':
    if(R->SelAttributes->Style.Contains(fsBold))
     R->SelAttributes->Style=R->SelAttributes->Style>>fsBold;
    break;
 }}
 return S;
}
//------------------------------------------------------------------------
void RichAdd(TRichEdit *R,wchar_t *S){
 while(SpecSimb(*S))
  S=RichAttr(R,S);
 StrDOStoWIN(S); S[wcslen(S)-1]='\0';
 R->Lines->Add(S);
}
//------------------------------------------------------------------------

