#include "CA.h"
#include "ConvDW.h"
#include <alloc.h>
#include <string.h>
#include <math.h>

bool IsError,IsSyntError,ISRAND; int N,MAXFUNC=18,MAXZNAC=5,NzK,NPV;
char AtrInerc,AtrDisip,AtrElast,AtrForse,AtrNelin,/*AtrLiner,*/COD[7],
 *NameFunc[] = {
   "_","-","sqrt","sin","cos","tg","arcsin","arctg","exp","ln","sh","ch",
   "arsh","abs","rand","ceil","floor","arccos"
  },
 *NameCnst[] = {
   "pi","2PI","3PI","4PI","5PI","6PI","7PI","PI","PIna2","PIna3","PIna4","PIna6",
   "exp","3","2","1","0.5","0","sqrt(2)","sqrt(3)","sqrt(5)","\0",
  };
List *LBeg,*L;
Cnst
 *PI,*DvaPI,*TriPI,*FourPI,*FivePI,*SixPI,*SevenPI,*PIna2,*PIna3,*PIna4,*PIna6,
 *Exp,*Tri,*Dva,*Odin,*Halb,*pi,*Nul,*Sqrt2,*Sqrt3,*Sqrt5,*Pust;
Vary *G;
//------------------------------------------------------------------------
Cnst *InitCnst (int Atr,Cnst *Sled,char *Name,double D) {
 Cnst *C=(Cnst *)calloc(1,sizeof(Cnst));
 C->Atr=Atr; C->Sled=Sled; C->Name=Name; C->Val=D; return C;
}
//------------------------------------------------------------------------
void CodElem(char *Cod) {
 strcpy(COD,Cod);
 AtrInerc=Cod[0]; AtrDisip=Cod[1]; AtrElast=Cod[2];
 AtrForse=Cod[3]; AtrNelin=Cod[4]; //AtrLiner=Cod[5];
}
//------------------------------------------------------------------------
void InitCA(void){ //Vary *V;
 if(!Pust){
  pi        = InitCnst ( CNST,NULL   ,NameCnst[ 0],M_PI  );
  DvaPI     = InitCnst ( CNST,pi     ,NameCnst[ 1],M_PI*2);
  TriPI     = InitCnst ( CNST,DvaPI  ,NameCnst[ 2],M_PI*3);
  FourPI    = InitCnst ( CNST,TriPI  ,NameCnst[ 3],M_PI*4);
  FivePI    = InitCnst ( CNST,FourPI ,NameCnst[ 4],M_PI*5);
  SixPI     = InitCnst ( CNST,FivePI ,NameCnst[ 5],M_PI*6);
  SevenPI   = InitCnst ( CNST,SixPI  ,NameCnst[ 6],M_PI*7);
  PI        = InitCnst ( CNST,SevenPI,NameCnst[ 7],M_PI  );
  PIna2     = InitCnst ( CNST,PI     ,NameCnst[ 8],M_PI/2);
  PIna3     = InitCnst ( CNST,PIna2  ,NameCnst[ 9],M_PI/3);
  PIna4     = InitCnst ( CNST,PIna3  ,NameCnst[10],M_PI/4);
  PIna6     = InitCnst ( CNST,PIna4  ,NameCnst[11],M_PI/6);
  Exp       = InitCnst ( CNST,PIna6  ,NameCnst[12],M_E   );
  Tri       = InitCnst ( CNST,Exp    ,NameCnst[13],3     );
  Dva       = InitCnst ( CNST,Tri    ,NameCnst[14],2     );
  Odin      = InitCnst ( CNST,Dva    ,NameCnst[15],1     );
  Halb      = InitCnst ( CNST,Odin   ,NameCnst[16],0.5   );
  Nul       = InitCnst ( CNST,Halb   ,NameCnst[17],0     );
  Sqrt2     = InitCnst ( CNST,Nul    ,NameCnst[18],sqrt(2));
  Sqrt3     = InitCnst ( CNST,Sqrt2  ,NameCnst[19],sqrt(3));
  Sqrt5     = InitCnst ( CNST,Sqrt3  ,NameCnst[20],sqrt(5));
  Pust      = InitCnst ( PUST,Sqrt5  ,NameCnst[21],0      );
 }
 CodElem("JDCPNL");
 if(!L) LBeg=L=TakeList("ОСНОВНОЙ");
}
//------------------------------------------------------------------------
bool SpecSimb(char C){
 if(C=='\x1B'||C=='\t') return true; return false;
}
//------------------------------------------------------------------------
char *RichAttr(TRichEdit *R,char *S){ char Buf[512];
 if(*S=='\t'){//красная строка
  if(R->Font->Pitch!=fpFixed) sprintf(Buf,"      %s",S+1);
  else  sprintf(Buf,"   %s",S+1);
  strcpy(S,Buf);
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
void RichAdd(TRichEdit *R,char *S){
 while(SpecSimb(*S))
  S=RichAttr(R,S);
 StrDOStoWIN(S); S[strlen(S)-1]='\0';
 R->Lines->Add(S);
}
//------------------------------------------------------------------------

