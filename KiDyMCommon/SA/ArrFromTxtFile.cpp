#include "CA.h"

extern wchar_t CurrDir[],Inf[]; extern List *L;

double ReadChislo(FILE *F){
 wchar_t C,S[256],*s=S;
 while(C=fgetwc(F),(C!='\t')&&(C!='\n')) *s++=C; *s='\0';
 return _wtof(S);
}
//---------------------------------------------------------------
bool ReadArrFromFile(Parm *Par,TRichEdit *R,TCanvas *Canvas){
//формат текстового файла:
//первая строка заголовочная - пропускается
//в каждой следующей - значения времени, переменных, разделенных символом табуляции
 FILE *F; int Ks=0,Kv,i,j; wchar_t S[256]; Arra **A; double t; Form Ft;
 if(!Par->Nam) return false;
 swprintf(S,L"%s\\%s",CurrDir,Par->Nam);
 if(!(F=_wfopen(S,L"r"))){
  swprintf(Inf,L"Не найден файл: %s",S);
  Application->MessageBox(Inf,L"Оператор: Прочитать массивы из файла",MB_OK);
  return false;
 }
 while(fgetws(S,256,F)) Ks++; Ks--;
 Kv=KolElem(Par->Sled);
 A=(Arra **)calloc(Kv,SzV);
 for(Par=Par->Sled,i=0;Par;Par=Par->Sled,i++){
  if(Par->Nam){
   A[i]=TakeArra(Par->Nam);
   A[i]->N=Ks*2;
   A[i]->A=(double *)calloc(A[i]->N,SzD);
 }}
 rewind(F);
 fgetws(S,255,F);
 for(i=0;i<Ks;i++){
  t=ReadChislo(F);
  for(j=0;j<Kv;j++){
   A[j]->A[i*2+1]=ReadChislo(F);
   A[j]->A[i*2]=t;
 }}
 fclose(F);
 Ft.V=L->Time;
 for(j=0;j<Kv;j++){
  TakeSpln(1,A[j],Ft);
 }
 return true;
}

