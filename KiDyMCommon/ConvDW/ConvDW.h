#ifndef CONVDW
#define CONVDW

#include <stdio.h>
#include <vcl.h>

char *ChStrDOStoWIN(char *S);
wchar_t ConvChrDOStoWIN(wchar_t B);
wchar_t ConvChrWINtoDOS(wchar_t B);
wchar_t ConvChrWINtoPRN(wchar_t B);
wchar_t *StrDOStoWIN (wchar_t *S);
wchar_t *StrWINtoDOS (wchar_t *S);
wchar_t *StrWINtoPRN (wchar_t *S);
double Round (double D,double R);
double Round (double D);
wchar_t *Fgets(wchar_t *S,int n,FILE *F);
int  Fputs (wchar_t *S,FILE *F);
int  Fgetc(FILE *F);
int  Fputc (wchar_t B,FILE *F);
int  Fputs(wchar_t *s,FILE *stream);
int  KSimbStr (wchar_t *S,wchar_t C);
wchar_t IntToChar (int i);
int  CharToInt (wchar_t C);
bool isciril(wchar_t c);
bool isLargeLatin(wchar_t c);
bool isLargeCiril(wchar_t c);
bool isLarge(wchar_t c);
bool IsLiters(wchar_t *S);//в строке S есть буквы
wchar_t *TextFromFile(wchar_t *Dir,wchar_t *NameFile,
 TStringList **SL,int N);
wchar_t *ToLat(wchar_t *S);
wchar_t *LowToUpp(wchar_t *S);
wchar_t *UppToLow(wchar_t *S);
int  max(int a,int b);
int  min(int a,int b);
wchar_t *NameToFile(wchar_t *S);
wchar_t *NameToFileRus(wchar_t *S);
void Unlink(wchar_t *File);
bool GrandToLitlSimbKiril(wchar_t &B);
bool LitlToGrandSimbKiril(wchar_t &B);
void CopyFromArhiv(wchar_t *Arhiv,wchar_t *File);
void CopyToArhiv(wchar_t *Arhiv,wchar_t *File);
bool CreateDir(wchar_t *Dir);
void Pack(wchar_t *Kod,wchar_t *Path,wchar_t *Arc,wchar_t *File);
int  Grek(wchar_t *S,int &Kod);//возвращает код и длину греческой буквы
void DrawGrekText(TCanvas *Canvas,int Left,int Top,int Width,
 wchar_t *S,wchar_t SimbInd);
void DrawGrekText(TCanvas *Canvas,const TRect &Rect,wchar_t *S,
 wchar_t SimbInd);
int  LengthGrek(TCanvas *Canvas,wchar_t *S,wchar_t SimbInd);
int  StrCmp(wchar_t *s1,wchar_t *s2);
bool CopyFiles(wchar_t *Fi,wchar_t *Fo);
bool CopyFiles2(wchar_t *Fi,wchar_t *Fo);
wchar_t *Pole(wchar_t *S);
bool AllCifry(wchar_t *S);
wchar_t *CleanStr(wchar_t *SIn,wchar_t *SOut);
void CopyFileToFile(wchar_t *FileOld,wchar_t *FileNew);
bool Number(wchar_t *S);
bool IsCifra(wchar_t B);
void FormSizes(int &Left, int &Top, int &Width, int &Height);
int  KodGrek(wchar_t *S,int &Kod);
void CopyZam(FILE *F,wchar_t *Target);
#endif
