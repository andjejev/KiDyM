#ifndef TablWH_
#define TablWH_

#define SzC sizeof(wchar_t)
#define SzI sizeof(int)
#define SzL sizeof(long)
#define SzD sizeof(double)
#define SzF sizeof(float)
#define SzV sizeof(void *)
//---------------------------------------------------------------------------
void *Sled(void *p);
//---------------------------------------------------------------------------
int KolElem(void *P);
//---------------------------------------------------------------------------
struct Magazine { Magazine *Sled; wchar_t *S; };
//---------------------------------------------------------------------------
int FindMagazine(Magazine *Rout,wchar_t *S,Magazine **M);
//---------------------------------------------------------------------------
Magazine *TakeMagazine(Magazine **Rout,wchar_t *S);
//---------------------------------------------------------------------------
#endif
