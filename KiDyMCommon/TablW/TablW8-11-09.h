#ifndef TablWH
#define TablWH
#include <stdio.h>
#include <vcl.h>
//---------------------------------------------------------------------------
#define SzC sizeof(char)
#define SzI sizeof(int)
#define SzL sizeof(long)
#define SzD sizeof(double)
#define SzF sizeof(float)
#define SzV sizeof(void *)

extern bool ERRORDATA;
struct Magazine { Magazine *Sled; char *S; };

//---------------------------------------------------------------------------
void *Sled(void *p);

int KolElem(void *P);

int KSimbStr(char *S,char C);

void DelStruct(void **Rout,void *S);

void DelAllList(void **Rout);

//Error
bool ErrorChar(unsigned char B,int Out,char *S);
bool ErrorString(char *S,int Out,char *Mess);

//Magazine
bool FindAllMagazine(Magazine *R,char *S);
Magazine *FindMagazineAll(Magazine *R,char *S);
int FindMagazine(Magazine *Rout,char *S,Magazine **M);
Magazine *TakeMagazine(Magazine **Rout,char *S);
Magazine *MakeMagazine(Magazine **Rout,char *S);
Magazine *AddMagazine(Magazine **Rout,char *S);
void DelAllMagazine(Magazine **R);
void DelMagazineCmp(Magazine **Rout,char *S);
void DelMagazine(Magazine **Rout,Magazine *mm);
Magazine *FileToMagazin(char *NameFile);
int MagazineToFile(char *NameFile,Magazine *R);
Magazine *FileToMagazinUnSort(char *NameFile,bool DOS);
int MagazinToFile(Magazine *M,char *NameFile);
//работа со строками
void StringTabl(char *S,char *MasD,FILE *F);
void ReplaceChar(char *S,char Bz,char Bt);

long DtoK(double D);
char IntToChar(int i);
int CharToInt(char C);
double Round(double D,double R);
double Round(double D);
char *LtoStr(int Pol, unsigned long Val, char *S);
char *LtoStrU(int Pol, unsigned long Val, char *S);

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
void PoleWINtoDOS(char *Pole,char *Buf);
//преобразование цепного списка в массив и обратно
void **LtoM(void *Rout);
void MtoL(void **M,int N);
//определение указателя на положение в строке S N-го символа B
char *FindKSimb(char *S,char B,int N);
char *StrN(char *S,char B,int N,char *Buf);
int NStrMagCmp(Magazine *M,char *S);
char *StrN(Magazine *M,int N);
int NItemInMas(int *Iv,int N,int i);
void InverList(void **P);
bool Pusta(AnsiString S);
int ReadStrOpt(char *Str,FILE *F);
void PutCentrS(char *S,FILE *F,int Size);
int CmpFioUkr(char *LFio1,char *LFio2);

struct Balanc { Balanc *Sled; char *E1,*E2; double K; };
void DelAllBalanc(Balanc **Rout);
int NStrMag(Magazine *M,char *S);
Magazine *FindMag(Magazine *M,char *S);
Magazine *ShortMag (Magazine *M);

void UnPack(char *LHA,char *Arhiv,char *File);
void Pack(char *LHA,char *Arhiv,char *File);
char *Pole(char *s);
void ViewMagazine(Magazine *M,char *Zag,char *Shapka,char *Help);

#endif
