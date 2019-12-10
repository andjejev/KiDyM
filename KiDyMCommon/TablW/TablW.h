#ifndef TablWH
#define TablWH
#include <stdio.h>
#include <vcl.h>
#include "ConvDW.h"#
//---------------------------------------------------------------------------
#define SzC sizeof(wchar_t)
#define SzI sizeof(int)
#define SzL sizeof(long)
#define SzD sizeof(double)
#define SzF sizeof(float)
#define SzV sizeof(void *)
#define IsD 0
#define IsS 1

struct Magazine { Magazine *Sled; wchar_t *S; };

struct DouSt{
 int Atr; union { double D; wchar_t *S; } U;
 DouSt& operator = (DouSt DS);
 DouSt& operator = (wchar_t *S);
 DouSt& operator = (double D);
 DouSt& operator = (long L)  { *this=(double)L; return *this; };
 DouSt& operator = (int I)   { *this=(double)I; return *this; };
};

//---------------------------------------------------------------------------
void *Sled(void *p);

int KolElem(void *P);

int KSimbStr(wchar_t *S,wchar_t C);

void DelStruct(void **Rout,void *S);

void DelAllList(void **Rout);

//Error
bool ErrorChar(wchar_t B,int Out,wchar_t *S);
bool ErrorString(wchar_t *S,int Out,wchar_t *Mess);

//Magazine
bool FindAllMagazine(Magazine *R,wchar_t *S);
Magazine *FindMagazineAll(Magazine *R,wchar_t *S);
int FindMagazine(Magazine *Rout,wchar_t *S,Magazine **M);
Magazine *TakeMagazine(Magazine **Rout,wchar_t *S);
Magazine *MakeMagazine(Magazine **Rout,wchar_t *S);
Magazine *AddMagazine(Magazine **Rout,wchar_t *S);
void DelAllMagazine(Magazine **R);
void DelMagazineCmp(Magazine **Rout,wchar_t *S);
void DelMagazine(Magazine **Rout,Magazine *mm);
Magazine *FileToMagazin(wchar_t *NameFile);
int MagazineToFile(wchar_t *NameFile,Magazine *R);
Magazine *FileToMagazinUnSort(wchar_t *NameFile,bool DOS);
int MagazinToFile(Magazine *M,wchar_t *NameFile);
wchar_t *StrN(Magazine *M,int N);
Magazine *FileToMagazinBezDW(wchar_t *NameFile);
int MagazineToFileBezDW(wchar_t *NameFile,Magazine *R);

//работа со строками
void StringTabl(wchar_t *S,wchar_t *MasD,FILE *F);
void ReplaceChar(wchar_t *S,wchar_t Bz,wchar_t Bt);

long DtoK(double D);
//wchar_t IntToChar(int i);
//int CharToInt(wchar_t C);
double Round(double D,double R);
double Round(double D);
wchar_t *LtoStr(int Pol, unsigned long Val, wchar_t *S);
wchar_t *LtoStrU(int Pol, unsigned long Val, wchar_t *S);

class MasLinF {
  long Pos; int N;
 public:
  long *ReadML (int K,long *L);
  long  ReadLi (int i);
  void  WriteML(int K,long *L);
  void  WriteLi(int i,long L);
  void  AddLi  (int i,long D);
  void  Free   ();
  void  Calloc (int K);
  void  ObNul  () { Pos=0L; N=0; }
};

int InitMasLinF();

class MasDinF {
  long Pos; int N;
 public:
  double *ReadMD (int K,double *L);
  double  ReadDi (int i);
  void    WriteMD(int K,double *L);
  void    WriteDi(int i,double L);
  void    AddDi  (int i,double D);
  void    Free   ();
  void    Calloc (int K);
};

int InitMasDinF();

//преобразование строкового поля структуры в кодировке ANSI в
//кодировку ASCII и записи результата преобразования в буфер Buf
void PoleWINtoDOS(wchar_t *Pole,wchar_t *Buf);
//преобразование цепного списка в массив и обратно
void **LtoM(void *Rout);
void MtoL(void **M,int N);
//определение указателя на положение в строке S N-го символа B
wchar_t *FindKSimb(wchar_t *S,wchar_t B,int N);
wchar_t *StrN(wchar_t *S,wchar_t B,int N,wchar_t *Buf);
int NStrMagCmp(Magazine *M,wchar_t *S);
wchar_t *StrN(Magazine *M,int N);
int NItemInMas(int *Iv,int N,int i);
void InverList(void **P);
bool Pusta(AnsiString S);
int ReadStrOpt(wchar_t *Str,FILE *F);
void PutCentrS(wchar_t *S,FILE *F,int Size);
int CmpFioUkr(wchar_t *LFio1,wchar_t *LFio2);

struct Balanc { Balanc *Sled; wchar_t *E1,*E2; double K; };
void DelAllBalanc(Balanc **Rout);
int NStrMag(Magazine *M,wchar_t *S);
Magazine *FindMag(Magazine *M,wchar_t *S);
Magazine *ShortMag (Magazine *M);

void UnPack(wchar_t *LHA,wchar_t *Arhiv,wchar_t *File);
void Pack(wchar_t *LHA,wchar_t *Arhiv,wchar_t *File);
wchar_t *Pole(wchar_t *s);
void ViewMagazine(Magazine *M,wchar_t *Zag,wchar_t *Shapka,wchar_t *Help);
Magazine *FileToMagazinHead (wchar_t *NameFile,wchar_t *Head);
#endif
