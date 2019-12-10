//---------------------------------------------------------------------------
#pragma hdrstop

#include <stdio.h>
#include <vcl.h>
#include "TablW_.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

/* процедуры поиска элементов по имени в соответствующих списках.
   возвращают:
	0,если элемент надо ставить в начало списка
	1,если такой элемент найден - его адрес находится в последнем
	   формальном параметре: */
//---------------------------------------------------------------------------
void *Sled(void *p){ return (void *)*(unsigned long *)p; }
//---------------------------------------------------------------------------
int KolElem(void *P){ int k=0; for(;P;P=Sled(P)) k++; return k; }
//---------------------------------------------------------------------------
int FindMagazine(Magazine *Rout,wchar_t *S,Magazine **M){
 if(!(*M=Rout)) return 0;
 if(wcscmp(S,Rout->S)<0){ *M=NULL; return 0; }
 for(;;*M=(*M)->Sled){
  if(!wcscmp((*M)->S,S)) return 1;
  if(!((*M)->Sled)) return 0;
  if(wcscmp((*M)->Sled->S,S)>0) return 0;
}}
//---------------------------------------------------------------------------
Magazine *TakeMagazine(Magazine **Rout,wchar_t *S){
 Magazine *M,*m; if(!S) return NULL;
 if(FindMagazine(*Rout,S,&M)) return M;
 m=(Magazine *)calloc(1,sizeof(Magazine));
 m->S=wcscpy((wchar_t *)malloc((wcslen(S)+1)*sizeof(wchar_t)),S);
 if(!M){ m->Sled=*Rout;   return   *Rout=m; }
 else  { m->Sled=M->Sled; return M->Sled=m; }
}
//---------------------------------------------------------------------------
Magazine *AddMagazine(Magazine **Rout,wchar_t *S){
 Magazine *M,*m;
 if(!S) return NULL;
 M=(Magazine *)calloc(1,sizeof(Magazine));
 M->S=wcscpy((wchar_t *)malloc((wcslen(S)+1)*sizeof(wchar_t)),S);
 if(!(*Rout)) *Rout=M;
 else{ for(m=*Rout;m->Sled;m=m->Sled); m->Sled=M; }
 return M;
}
//---------------------------------------------------------------------------
void DelAllMagazine(Magazine **R){
 for(Magazine *M;M=*R;){ *R=M->Sled; if(M->S) free(M->S); free(M); }
}
//---------------------------------------------------------------------------
