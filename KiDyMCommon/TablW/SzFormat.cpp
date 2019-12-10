#include "EditList.h"
#include "SzFormat.h"
#include "ConvDW.h"
#include <stdlib.h>
#include "TablW.h"

extern TFormEditList *FormEditList; extern int FontSize;
extern wchar_t Inf[];
static FILE *FmasD;
static int KStrMasD; static wchar_t FileMasD[13];
//---------------------------------------------------------------------------
int SizePole (wchar_t B) { int L;
 switch(B) {
  case 'c':                     L=2*SzC; break;
  case 'd':                     L=SzI; break;
  case 'l': case 'k':           L=SzL; break;
  case 'e': case 'f': case 'g': L=SzD; break;
  case 'E': case 'F': case 'G': L=SzF; break;
  case 'A': case 'a': case 'm': case 'ф':
  case 'n': case 'p': case 's': L=SzV; break;
  case 'К':         L=sizeof(MasLinF); break;
  case 'U':                     L=SzV; break;
   default: L=0;
 }
 return L;
}
//---------------------------------------------------------------------------
bool SizeFormatFild(void *Rout,wchar_t *Format,int *NSumm,FILE *FSh,
 int *MasF0,int **MasF,int **MasD0,int **MasD,int **MasL0,int **MasL,
 int **Size,wchar_t **SForm,wchar_t ***MasForm,int *KForm,int *HSh,int *LTab,
 TStringGrid *SG,int FixedCols,bool Show0) {
/* Процедура определяет размеры и форматы вывода полей шапки */
 int LSh=0,KF=0,KFSh=1,i,j,k,L=0,N=0,ND=0,*size,*masl,*masf,*n,Lt=0,d,KSumm=0,*nsumm;
 wchar_t *f=Format,B1,b,M,*sform,**masform,S[1024],*s,FormS[80],
  Buf[1024],*Tchk,**m,Zn,B; double D; MasLinF *Li; DouSt *DS;
 void *p; double *a,e,*Summa=NULL,*summa,*dk; long l,*K,*lk; float *A,E;
 if(NSumm&&*NSumm) {
  for(nsumm=NSumm;*nsumm++;KSumm++);
  Summa=(double *)calloc(KSumm,SzD);
 }
/* Сколько динамических частей: */
 for(*KForm=0;B1=*f;f++)
  switch(B1=*f) {
   case 'a': case 'A': case'n':case 'K': case 'ф':
   case 'К':/*русское К*/
   case 'U':/*массив DouSt*/
    ND++; *KForm+=N-1; break;
   case '%': KF++; (*KForm)++; N=abs(_wtoi(f+1));
 }
//m - массив строк (wchar_t **);
//n - целочисленный массив; a - вешественный массив; U - массив DouSt
//K - массив денег; А - вешественный массив из файла;
//--------*----------*--------------*-----------------
/* Вектор размеров динамических массивов: */
 *Size=size=(int *)malloc(ND*sizeof(int));
 for(f=Format;B1=*f;f++) switch(B1) {
  case 'a': case 'A': case'n':case 'K':case 'ф':
  case 'К':/*русское К*/
  case 'U':/*массив DouSt*/
   *size++=N; break;
  case '%': N=abs(_wtoi(f+1));
 }
//--------*----------*------------*------
/* Высота,длина шапки,число полей в ней: */
 if(FSh){
  rewind(FSh);
  *HSh=0;//высота шапки в строках
  N=1; KFSh=0;//текущее и общее число столбцов в шапке
//L и LSh - текущая и общая ширина шапки в символах
  while(!feof(FSh)){
   B=fgetwc(FSh);
   switch(ConvChrDOStoWIN(B),B){
    case '\n': case EOF:
     if(L) (*HSh)++; if(L>=LSh) LSh=L; L=0;
     if(N>KFSh) KFSh=N; N=1;
     break;
    case  179: case 186: N++; L++;
     break;
    case  '\t': L+=7;
     default: L++;
 }}}
 if(KFSh!=*KForm) {
  swprintf(Inf,L"Число полей в шапке (%d) и в формате (%d) не одинаково!",
   KFSh,*KForm);
  Application->MessageBox(Inf,L"Ошибка!",MB_OK|MB_ICONWARNING);
  if(*Size) { free(*Size); *Size=NULL; }
  if(Summa) { free(Summa); Summa=NULL; }
  return false;
 }
 *SForm=(wchar_t *)calloc(KF+1,SzC);
 *MasF =(int *)calloc(KF,SzI);    /* массив смещений полей в ОЗУ    */
 *MasForm=(wchar_t **)malloc((*KForm)*SzV); /* массив форматов полей в таблице */
 *MasL =(int *)calloc(*KForm,SzI); /* массив длин полей в таблице*/
 *MasD =(int *)calloc(*KForm,SzI); /* массив смещений полей в таблице*/
 *MasL0=(int *)calloc(*KForm,SzI); /* массив длин полей в шапке*/
 *MasD0=(int *)calloc(*KForm,SzI); /* массив смещений полей в шапке*/
 /* Размеры полей в шапке: */
 if(FSh) {
  rewind(FSh); KFSh=L=0;
  for(i=0;i<*HSh-1;i++) fgetws(Buf,1023,FSh);
  while(!feof(FSh))
   switch(B=getc(FSh)) {
    case '\n':case EOF:
     if(LSh-Lt>L) L=LSh-Lt; (*MasL0)[KFSh]=L; (Lt)+=L; if(B=='\n') getc(FSh);
     break;
    case  179: (*MasL0)[KFSh++]=L; (Lt)+=L+1; L=0; break;
    case '\t': L+=7;
     default: L++;
   }
//  for(i=0;i<*KForm;i++) (*MasL0)[i]=(*MasL0)[i]*14/10;
  for(i=0;i<*KForm;i++) (*MasL0)[i]+=1;
  for(**MasD0=**MasL0,i=1;i<*KForm;i++) (*MasD0)[i]+=(*MasD0)[i-1]+(*MasL0)[i]+1;
 }
/* Делаем форматы: */
/* определяем длины полей по формату: */
 for(f=Format,masl=*MasL,size=*Size,sform=*SForm;B1=*f;f++) switch(B1) {
  case '%': *masl=abs(_wtoi(f+1)); break;
  case 'a': case 'A': case 'n':case 'K':case 'ф':
  case 'К':/*русское К*/
  case 'U':/*массив DouSt*/
   *masl=0; masl+=*size++; *sform++=B1; break;
  case 'd':case 'l':case 'f':case 'g':case 'e':case 'F':case 'G':
  case 'E':case 'c':case 's':case 'm':case 'k': masl++; *sform++=B1;
 }
/* определяем массив смещений полей в ОЗУ от начала структуры: */
 for(i=0;i<KF;i++) (*MasF)[i]=MasF0[i];
 for(i=1;i<KF;i++) (*MasF)[i]+=(*MasF)[i-1];
/* определяем длины полей по данным */
 for(p=Rout;p;p=Sled(p))
  for(masl=*MasL,size=*Size,masf=*MasF,sform=*SForm,nsumm=NSumm,summa=Summa,
   j=0,f=Format+1; B1=*sform++;j++){
   Zn=*f; while(*f&&*f!='.'&&*f!='%') f++;
   switch(B1) {
    case 'd': d=*(int *)((wchar_t *)p+*masf++);
     if(NSumm&&*NSumm&&j==*nsumm-1) { (*summa++)+=d; nsumm++; }
     switch(Zn) {
      case '-': L=swprintf(S,L"%-d",d); break;
      case '+': L=swprintf(S,L"%+d",d); break;
      default:  L=swprintf(S,L"%d",d);
     }
     if(L>=*masl) *masl=L; masl++; break;
    case 'l': l=*(long *)((wchar_t *)p+*masf++);
     if(NSumm&&*NSumm&&j==*nsumm-1) { (*summa++)+=l; nsumm++; }
     switch(Zn) {
      case '-': L=swprintf(S,L"%-ld",l); break;
      case '+': L=swprintf(S,L"%+ld",l); break;
      default:  L=swprintf(S,L"%ld",l);
     }
     if(L>=*masl) *masl=L; masl++; break;
    case 'k': l=*(long *)((wchar_t *)p+*masf++); e=(double)l/100.0;
     if(NSumm&&*NSumm&&j==*nsumm-1) { (*summa++)+=e; nsumm++; }
     switch(Zn) {
      case '-': L=swprintf(S,L"%-0.2f",e); break;
      case '+': L=swprintf(S,L"%+0.2f",e); break;
      default:  L=swprintf(S,L"%0.2f",e);
     }
     if(L>=*masl) *masl=L; masl++; break;
    case 'c':
     if(!(*masl))
      *masl=1;
     masl++;
     masf++;
     break;
    case 'f': case 'e': case 'g':
     e=*(double *)((wchar_t *)p+*masf++);
     if(NSumm&&*NSumm&&j==*nsumm-1) { (*summa++)+=e; nsumm++; }
     if(*f=='.') {
      N=_wtoi(f+1);
      switch(Zn) {
       case '-': swprintf(FormS,L"%%-0.%d%c",N,B1); break;
       case '+': swprintf(FormS,L"%%+0.%d%c",N,B1); break;
       default:  swprintf(FormS,L"%%0.%d%c",N,B1);
     }}
     else switch(Zn) {
       case '-': wcscpy(FormS,L"%-g"); break;
       case '+': wcscpy(FormS,L"%+g"); break;
       default:  wcscpy(FormS,L"%g"); break;
     }
     L=swprintf(S,FormS,e);
     if(*f=='.') { for(s=S; *s==' '; s++,L--); }
     if(L>=*masl) *masl=L; masl++; break;
    case 'F': case 'E': case 'G':
     E=*( float *)((wchar_t *)p+*masf++);
     if(NSumm&&*NSumm&&j==*nsumm-1) { (*summa++)+=E; nsumm++; }
     if(*f=='.') {
      N=_wtoi(f+1);
      switch(Zn) {
       case '-': swprintf(FormS,L"%%-0.%d%c",N,B1=='F'?'f':B1=='E'?'e':'g'); break;
       case '+': swprintf(FormS,L"%%+0.%d%c",N,B1=='F'?'f':B1=='E'?'e':'g'); break;
       default:  swprintf(FormS,L"%%0.%d%c",N,B1=='F'?'f':B1=='E'?'e':'g');
     }}
     else switch(Zn) {
       case '-': wcscpy(FormS,L"%-g"); break;
       case '+': wcscpy(FormS,L"%+g"); break;
       default:  wcscpy(FormS,L"%g"); break;
     }
     L=swprintf(S,FormS,E);
     if(*f=='.') { for(s=S; *s==' '; s++,L--); }
     if(L>=*masl) *masl=L; masl++; break;
    case 's': case 'm':
     s=(wchar_t *)*(unsigned long *)((wchar_t *)p+*masf++);
     if(s) while(*s&&*s==' ') s++;
     if(NSumm&&*NSumm&&j==*nsumm-1){ (*summa++)+=s?_wtof(s):0.0; nsumm++; }
     L=s?wcslen(s):0; if(L>*masl) *masl=L; masl++; break;
    case 'n':
     n=(int *)*(unsigned long *)((wchar_t *)p+*masf++);
     if(n)
      for(k=*size++,j+=k;k--;n++) {
       if(NSumm&&*NSumm&&j-k==*nsumm) { (*summa++)+=*n; nsumm++; }
       L=swprintf(S,L"%d",*n); if(L>=*masl) *masl=L; masl++;
      }
     else { j+=*size-1; masl+=*size++; } break;
    case 'K':
    case 'К': //русское К
     if(B1=='К') {//русское К
      K=(long *)calloc(*size,SzL);
      MasLinF *L=p?(MasLinF *)((wchar_t *)p+*masf++):NULL;
      if(L&&K) lk=L->ReadML(*size,K);
     }
     else K=lk=(long *)*(unsigned long *)((wchar_t *)p+*masf++);
     if(K) {
      if((*f=='.')&&((N=_wtoi(f+1))>=0))
       swprintf(FormS,L"%%0.%df",N);
      else
       wcscpy(FormS,L"%g");
      for(k=*size++,j+=k;k--;) {
       if(NSumm&&*NSumm&&j-k==*nsumm) {
      	(*summa++)+=(double)(*lk)/100.0; nsumm++;
       }
       L=swprintf(S,FormS,(double)(*lk)/100.0);
       if(*f=='.') for(s=S;*s==' ';s++,L--);
       if(L>=*masl) *masl=L; masl++; lk++;
      }
      j--; if(B1=='К') free(K); //русское К
     }
     else { j+=*size-1; masl+=*size++; } break;
    case 'a': case 'ф':
     if(B1=='ф') {
      a=(double *)calloc(*size,SzD);
      MasDinF *D=p?(MasDinF *)((wchar_t *)p+*masf++):NULL;
      if(D&&a) dk=D->ReadMD(*size,a);
     }
     else a=dk=(double *)*(unsigned long *)((wchar_t *)p+*masf++);
     if(a) {
      if(*f=='.') { N=_wtoi(f+1); swprintf(FormS,L"%%0.%df",N); }
      else wcscpy(FormS,L"%g");
      for(k=*size++,j+=k;k--;) {
       if(NSumm&&*NSumm&&j-k==*nsumm) {	(*summa++)+=*dk; nsumm++; }
       L=swprintf(S,FormS,*dk);
       if(*f=='.') for(s=S;*s==' ';s++,L--);
       if(L>=*masl) *masl=L; masl++; dk++;
      }
      j--; if(B1=='ф') free(a);
     }
     else { j+=*size-1; masl+=*size++; } break;
    case 'A':
     if(B=='A') A=( float *)*(unsigned long *)((wchar_t *)p+*masf++);
     if(A) {
      if(*f=='.') { N=_wtoi(f+1); swprintf(FormS,L"%%0.%df",N); }
      else wcscpy(FormS,L"%g");
      for(k=*size++,j+=k;k--;) {
       if(NSumm&&*NSumm&&j-k==*nsumm) { (*summa++)+=*A; nsumm++; }
       L=swprintf(S,FormS,*A);
       if(*f=='.') { for(s=S; *s==' '; s++,L--); /*L-=(int)(s-S);*/ }
       if(L>=*masl) *masl=L; masl++; A++;
      }
      j--;
     }
     else { j+=*size-1; masl+=*size++; } break;
    case 'U':
     if(DS=(DouSt *)*(unsigned long *)((wchar_t *)p+*masf++)){
      for(k=*size++,j+=k;k--;DS++){
       if(DS->Atr==IsD){
        if(NSumm&&*NSumm&&(j-k==*nsumm)){
         (*summa++)+=DS->U.D; nsumm++;
        }
        if((*f=='.')&&((N=_wtoi(f+1))>=0))
         swprintf(FormS,L"%%0.%df",N);
        else
         wcscpy(FormS,L"%g");
        L=swprintf(S,FormS,DS->U.D);
       }
       else
        L=swprintf(S,L"%s",DS->U.S);
       if(*f=='.') for(s=S; *s==' '; s++,L--);
       if(L>=*masl) *masl=L; masl++;
      }
      j--;
     }
     else { j+=*size-1; masl+=*size++; } break;
    default: masl++;
   }
   if(*f=='.') while(*f&&*f!='%') f++; f++;
 }
 if(NSumm&&*NSumm) {
  p=Rout; masl=*MasL; size=*Size; masf=*MasF; nsumm=NSumm; summa=Summa;
  for(j=0,f=Format+1,sform=*SForm; B1=*sform++; j++,masf++) {
   while(*f&&*f!='.'&&*f!='%') f++;
   switch(B1) {
    case 'd':
     if(j==*nsumm-1) {
      L=swprintf(S,L"%d",*summa++); nsumm++; if(L>=*masl) *masl=L;
     } masl++; break;
    case 'l':
     if(j==*nsumm-1) {
      L=swprintf(S,L"%ld",*summa++); nsumm++; if(L>=*masl) *masl=L;
     } masl++; break;
    case 'k':
     if(j==*nsumm-1) {
      L=swprintf(S,L"%0.2f",*summa++); nsumm++; if(L>=*masl) *masl=L;
     } masl++; break;
    case 's':
     if(j==*nsumm-1) {
      L=swprintf(S,L"%0.2f",*summa++); nsumm++; if(L>=*masl) *masl=L;
     } masl++; break;
    case 'f': case 'e': case 'g':
     if(j==*nsumm-1) {
      if(*f=='.') { N=_wtoi(f+1); swprintf(FormS,L"%%0.%d%c",N,B1); }
      else wcscpy(FormS,L"%g");
      L=swprintf(S,FormS,*summa++); nsumm++;
      if(*f=='.') { for(s=S; *s==' '; s++,L--);  }
      if(L>=*masl) *masl=L;
     } masl++; break;
    case 'F': case 'E': case 'G':
     if(j==*nsumm-1) {
      if(*f=='.') {
       N=_wtoi(f+1); swprintf(FormS,L"%%0.%d%c",N,B1=='F'?'f':B1=='E'?'e':'g');
      }
      else wcscpy(FormS,L"%g");
      L=swprintf(S,FormS,*summa++); nsumm++;
      if(*f=='.') { for(s=S; *s==' '; s++,L--); }
      if(L>=*masl) *masl=L;
     } masl++; break;
    case 'n':
     for(k=*size++,j+=k;k--;masl++) {
      if(j-k==*nsumm) {
       L=swprintf(S,L"%d",*summa++); nsumm++;  if(L>=*masl) *masl=L; masl++;
     }} break;
    case 'a':case 'A':case 'K':case 'ф':case 'К'://русское К
     if((*f=='.')&&((N=_wtoi(f+1))>=0)) swprintf(FormS,L"%%0.%df",N);
     else wcscpy(FormS,L"%g");
     for(k=*size++,j+=k;k--;masl++) {
      if(j-k==*nsumm) {
       L=swprintf(S,FormS,*summa++); nsumm++;
       if(*f=='.') for(s=S;*s==' ';s++,L--);
       if(L>=*masl) *masl=L;
     }} j--; break;
    case 'U'://массив DouSt
     DS=(DouSt *)*(unsigned long *)((wchar_t *)p+*masf);
     for(k=0; k<*size; k++,masl++) {
      if((j+k)==(*nsumm-1)) {
       wcscpy(FormS,L"%s");
       if(DS&&(DS[k].Atr==IsD)){
        if((*f=='.')&&((N=_wtoi(f+1))>=0)) swprintf(FormS,L"%%0.%df",N);
        else wcscpy(FormS,L"%g");
       }
       L=swprintf(S,FormS,*summa++); nsumm++;
       if(*f=='.') for(s=S;*s==' ';s++,L--);
       if(L>=*masl) *masl=L;
     }}
     j--; size++;
     break;
    default: masl++;
   }
   if(*f=='.') while(*f&&*f!='%') f++; f++;
  }
 }
 if(NSumm&&*NSumm&&Summa) free(Summa);
//увеличим ширину каждого столбца на 4
// for(i=0;i<*KForm;i++) (*MasL)[i]+=4;
 for(i=*KForm;i--;)
  if((*MasL0)[i]>(*MasL)[i])(*MasL)[i]=(*MasL0)[i];
 for(i=1,Lt=**MasL;i<*KForm;i++) Lt+=(*MasL)[i]+1;
 if(*LTab-Lt>0) {
  L=(*LTab-Lt)/(*KForm); Lt=(*LTab-Lt)%(*KForm);
  if(L) for(i=0;i<*KForm;i++) (*MasL)[i]+=L;
  for(i=0;i<Lt;i++) (*MasL)[i]+=1;
 }
/* формирование массива форматов вывода полей структуры */
 masl=*MasL; masf=*MasF; masform=*MasForm; size=*Size; Tchk=NULL; M='\0'; p=Rout;
 for(f=Format+1,N=1; N; f++){
  switch(B1=*f){
   case '.': Tchk=f; if(f[1]=='-'){ Tchk=NULL; f++;} break;
   case '-': M=B1; break;
   case '+': M=B1; break;
   case'\0': N=0;
   case '%':
    switch(b=*(f-1)) {
     case 'n':
      if(!Tchk) Tchk=f-1; *f='\0';
      for(k=*size++;k--;) {
	   L=swprintf(S,M=='-'?L"%%-%d%s":M=='+'?L"%%+%d%s":L"%%%d%s",*masl++,Tchk);
	   *masform++=wcscpy((wchar_t *)malloc((L+1)*SzC),S);
	  }
	  *(f-1)='d'; break;
	 case 'a': case 'A':case 'K':case 'ф':case 'К':/*русское К*/
	  if(!Tchk) Tchk=f-1; *f='\0';
	  *(f-1)=Tchk ? 'f' : 'g';
	  for(k=*size++;k--;) {
	   L=swprintf(S,M=='-'?L"%%-%d%s":M=='+'?L"%%+%d%s":L"%%%d%s",*masl++,Tchk);
       *masform++=wcscpy((wchar_t *)malloc((L+1)*SzC),S);
      }
      break;
     case 'U'://массив DouSt
      DS=(DouSt *)*(unsigned long *)((wchar_t *)p+*masf);
      for(k=0;k<*size;k++){
       if(DS){
        if(DS[k].Atr==IsD){
         if(Tchk){
          if(_wtoi(Tchk+1)>0)
//           L=swprintf(S,M=='-'?"%%-%d.%df":M=='+'?"%%+%d.%df":"%%%d.%df",
           L=swprintf(S,L"%%%d.%df",
            *masl++,_wtoi(Tchk+1));
          else
//           L=swprintf(S,M=='-'?"%%-%dg":M=='+'?"%%+%dg":"%%%dg",*masl++);
           L=swprintf(S,L"%%%dg",*masl++);
         }
         else
//          L=swprintf(S,M=='-'?"%%-%dg":M=='+'?"%%+%dg":"%%%dg",*masl++);
          L=swprintf(S,L"%%%dg",*masl++);
        }
        else
//         L=swprintf(S,M=='-'?"%%-%ds":M=='+'?"%%+%ds":"%%%ds",*masl++);
         L=swprintf(S,L"%%-%ds",*masl++);
       }
       else
		L=swprintf(S,M=='-'?L"%%-%d%s":M=='+'?L"%%+%d%s":L"%%%d%s",*masl++,Tchk);
	   *masform++=wcscpy((wchar_t *)malloc((L+1)*SzC),S);
	  }
	  size++;
	  break;
	 default:
	  if(!Tchk) Tchk=f-1; *f='\0';
      L=swprintf(S,M=='-'?L"%%-%d%s":M=='+'?L"%%+%d%s":L"%%%d%s",*masl++,Tchk);
      *masform++=wcscpy((wchar_t *)malloc((L+1)*SzC),S);
    }
    masf++;
    Tchk=NULL; *(f-1)=b; *f=B1; M='\0';
 }}
 for(**MasD=**MasL,i=1;i<*KForm;i++) (*MasD)[i]+=(*MasD)[i-1]+(*MasL)[i]+1;
 *LTab=(*MasD)[*KForm-1];
/* заполняем StringGrid: */
 if(!SG) goto Ex;
//29.042.013 SG->RowCount=0; SG->ColCount=0; //очищаем предыдущее
 SG->RowCount=KolElem(Rout); SG->ColCount=*KForm;
 SG->FixedCols=FixedCols;
 for(p=Rout,i=0;p;p=Sled(p),i++){
  for(masl=*MasL,size=*Size,masf=*MasF,sform=*SForm,j=0,f=Format+1;
       B1=*sform++;j++){
   Zn=*f; while(*f&&*f!='.'&&*f!='%') f++;
   switch(B1) {
    case 'd': d=*(int *)((wchar_t *)p+*masf++);
     switch(Zn) {
      case '-': swprintf(FormS,L"%%-d"); break;
      case '+': swprintf(FormS,L"%%+%dd",*masl); break;
      default:  swprintf(FormS,L"%%%dd",*masl); break;
     }
     if(d||Show0){ swprintf(S,FormS,d); SG->Cells[j][i]=S; }
     else SG->Cells[j][i]=L"";
     masl++; break;
    case 'l': l=*(long *)((wchar_t *)p+*masf++);
     switch(Zn) {
      case '-': swprintf(FormS,L"%%-ld"); break;
      case '+': swprintf(FormS,L"%%+%dld",*masl); break;
      default:  swprintf(FormS,L"%%%dld",*masl); break;
     }
     if(l||Show0){ swprintf(S,FormS,l); SG->Cells[j][i]=S; }
     else SG->Cells[j][i]=L"";
     masl++; break;
    case 'k': l=*(long *)((wchar_t *)p+*masf++); e=(double)l/100.0;
     switch(Zn) {
      case '-': swprintf(S,L"%-0.2f",e); break;
      case '+': swprintf(S,L"%+0.2f",e); break;
      default:  swprintf(S,L"%0.2f",e);
     }
     if(l||Show0) SG->Cells[j][i]=S;
     else SG->Cells[j][i]=L"";
     masl++; break;
    case 'c':
     swprintf(FormS,L"%%%dc",*masl/2+1);
     swprintf(S,FormS,*((wchar_t *)p+*masf++));
     if(S[0]||Show0) SG->Cells[j][i]=S;
     else SG->Cells[j][i]=L"";
     masl++;
     break;
    case 'f': case 'e': case 'g':
     e=*(double *)((wchar_t *)p+*masf++);
     if(*f=='.') {
      N=_wtoi(f+1);
      switch(Zn) {
       case '-': swprintf(FormS,L"%%-0.%d%c",N,B1); break;
       case '+': swprintf(FormS,L"%%+%d.%d%c",*masl,N,B1); break;
       default:  swprintf(FormS,L"%%%d.%d%c",*masl,N,B1);
     }}
     else
      switch(Zn) {
       case '-': swprintf(FormS,L"%%-g"); break;
       case '+': swprintf(FormS,L"%%+%dg",*masl); break;
       default:  swprintf(FormS,L"%%%dg",*masl); break;
     }
     if((e!=0.0)||Show0){ swprintf(S,FormS,e); SG->Cells[j][i]=S; }
     else SG->Cells[j][i]=L"";
     masl++; break;
    case 'F': case 'E': case 'G':
     E=*( float *)((wchar_t *)p+*masf++);
     if(*f=='.') {
      N=_wtoi(f+1);
      switch(Zn) {
       case '-': swprintf(FormS,L"%%-0.%d%c",N,B1=='F'?'f':B1=='E'?'e':'g'); break;
       case '+': swprintf(FormS,L"%%+0.%d%c",N,B1=='F'?'f':B1=='E'?'e':'g'); break;
       default:  swprintf(FormS,L"%%0.%d%c",N,B1=='F'?'f':B1=='E'?'e':'g');
     }}
     else switch(Zn) {
       case '-': wcscpy(FormS,L"%-g"); break;
       case '+': wcscpy(FormS,L"%+g"); break;
       default:  wcscpy(FormS,L"%g"); break;
     }
     if((E!=0.0)||Show0){swprintf(S,FormS,E); SG->Cells[j][i]=S;}
     else SG->Cells[j][i]=L"";
     masl++; break;
    case 's': case 'm':
     s=(wchar_t *)*(unsigned long *)((wchar_t *)p+*masf++);
     switch(Zn) {
      case '-': swprintf(FormS,L"%%-s"); break;
      case '+': swprintf(FormS,L"%%+%ds",*masl); break;
      default:  swprintf(FormS,L"%%%ds",*masl); break;
     }
	 swprintf(S,FormS,s?s:L"\0"); SG->Cells[j][i]=S;
     masl++; break;
    case 'n':
     n=(int *)*(unsigned long *)((wchar_t *)p+*masf++);
     if(n){
      for(k=*size++;k--;n++) {
       if(*n||Show0){swprintf(S,L"%d",*n); SG->Cells[j][i]=S;}
       j++; masl++;
      }
      j--;
     }
     else { j+=*size-1; masl+=*size++; } break;
    case 'K':
     if(K=(long *)*(unsigned long *)((wchar_t *)p+*masf++)){
      for(k=*size++;k--;) { double D=(double)(*K)/100.0;
       if(*K||Show0){ swprintf(S,L"%0.2f",D); SG->Cells[j][i]=S; }
       else SG->Cells[j][i]=L"";
       K++; j++; masl++;
      }
      j--;
     }
     else { j+=*size-1; masl+=*size++; } break;
    case 'U':
     if(DS=(DouSt *)*(unsigned long *)((wchar_t *)p+*masf++)){
      for(k=*size++;k--;){
       if(DS->Atr==IsD){
        double D=DS->U.D;
        if((D!=0.0)||Show0){
//         if((*f=='.')&&((N=_wtoi(f+1))>=0)) swprintf(FormS,L"%%%d.%df",*masl,N);
//         else swprintf(FormS,L"%%%dg",*masl);
//         swprintf(S,FormS,D); SG->Cells[j][i]=S;
         swprintf(S,(*MasForm)[j],DS->U.D); SG->Cells[j][i]=S;
        }
        else SG->Cells[j][i]=L"";
       }
       else{
        swprintf(S,(*MasForm)[j],DS->U.S); SG->Cells[j][i]=S;
//        SG->Cells[j][i]=DS->U.S?DS->U.S:"";
       }
       DS++; j++; masl++;
      }
      j--;
     }
     else { j+=*size-1; masl+=*size++; } break;
    case 'К': //русское К
     K=(long *)calloc(*size,SzL);
     Li=p?(MasLinF *)((wchar_t *)p+*masf++):NULL;
     if(Li&&K) lk=Li->ReadML(*size,K);
     for(k=*size++;k--;) {
      if(*f=='.') {
       N=_wtoi(f+1);
       switch(Zn) {
        case '-': swprintf(FormS,L"%%-0.%df",N); break;
        case '+': swprintf(FormS,L"%%+%d.%df",*masl,N); break;
        default:
         if(f[1]=='-') swprintf(FormS,L"%%%dg",*masl);
         else swprintf(FormS,L"%%%d.%df",*masl,N);
      }}
      else
       switch(Zn) {
        case '-': swprintf(FormS,L"%%-g"); break;
        case '+': swprintf(FormS,L"%%+%dg",*masl); break;
        default:  swprintf(FormS,L"%%%dg",*masl); break;
      }
      if(*lk||Show0){ swprintf(S,FormS,(double)(*lk)/100.0); SG->Cells[j][i]=S; }
      else SG->Cells[j][i]=L"";
      lk++; j++; masl++;
     }
     j--; if(K) free(K); break;
    case 'a': case 'ф':
     if(B1=='ф') {
      a=(double *)calloc(*size,SzD);
      MasDinF *D=p?(MasDinF *)((wchar_t *)p+*masf++):NULL;
      if(D&&a) dk=D->ReadMD(*size,a);
     }
     else a=dk=(double *)*(unsigned long *)((wchar_t *)p+*masf++);
     if(a) {
      for(k=*size++;k--;) {
       if(*f=='.') { N=_wtoi(f+1); swprintf(FormS,L"%%%d.%df",*masl,N); }
       else swprintf(FormS,L"%%%dg",*masl);
       if(*dk||Show0){ swprintf(S,FormS,*dk); SG->Cells[j][i]=S; }
       else SG->Cells[j][i]=L"";
       j++; dk++; masl++;
      }
      j--; if(B1=='ф') free(a);
     }
     else { j+=*size-1; masl+=*size++; } break;
    case 'A':
     if(B1=='A') A=( float *)*(unsigned long *)((wchar_t *)p+*masf++);
     if(A) {
      if(*f=='.') { N=_wtoi(f+1); swprintf(FormS,L"%%0.%df",N); }
      else wcscpy(FormS,L"%g");
      for(k=*size++,j+=k;k--;) {
       if(*A||Show0){ swprintf(S,FormS,*A); SG->Cells[j][i]=S; }
       else SG->Cells[j][i]=L"";
       A++; masl++;
      }
      j--;
     }
     else { j+=*size-1; masl+=*size++; } break;
   }
   if(*f=='.') while(*f&&*f!='%') f++; f++;
 }}
Ex: return true;
}
//---------------------------------------------------------------------------
void Shapka (FILE *FSh,int KForm,int *MasD0,int *MasD,wchar_t **StrSh) {
/* Запоминание шапки в массив строк StrSh */
 int i,j,k,d1,d2,L,NStr=1;
 wchar_t S[1024],B,*s0=*StrSh,*s,*sh;
 if(FSh) { rewind(FSh);
  for(sh=StrSh[1],s=S,j=0,k=0,d1=-1;!feof(FSh);){
   if((B=getc(FSh))=='\n'||B<196&&B>178||B>196&&B<205||B>205&&B<220) {
    L=(int)(s-S); *s='\0'; s=S; d2=MasD[k];
    for(i=0; i<(d2-d1-L-1)/2; i++) *sh++=(*S==196 ? 196 : ' ');
    sh+=swprintf(sh,L"%s",S);
    for(i=0; i<d2-d1-L-1-(d2-d1-L-1)/2; i++) *sh++=(*S==196 ? 196 : ' ');
    if(B!='\n') { *sh++=B; j++; k++; d1=d2; }
    else { k=j=0; d1=-1; *sh='\0'; sh=StrSh[++NStr]; }
   }
   else {
    if(B=='\t') { for(i=8;i--;) *s++=' '; j+=8; } else { *s++=B; j++; }
    if(j>MasD0[k]&&k<KForm-1) k++;
  }}
  for(sh=StrSh[1];*sh;sh++,s0++)//строка с индексом 0
   if(*sh==179) *s0=194; else *s0=196;
  *s0='\0';
  for(sh=StrSh[NStr-1],s=StrSh[NStr];*sh;sh++,s++)
   if(*sh==179) *s=193; else *s=196;
  *s='\0';
}}
//---------------------------------------------------------------------------
void ShowShapka(TImage *Image,wchar_t **StrSh,int HSh,int LTab) {
 int i,j,H=Image->Canvas->TextHeight(StrSh[1]),x0,y0,xh0,yh0,xh1,
  xv0,yv0,yv1,L=Image->Canvas->TextWidth(L" ");
 wchar_t *s,*s0; wchar_t S[1024];
 Image->Height=H*HSh;
 Image->Canvas->Rectangle(L/4,H/2,L*(LTab+1)+3*L/4,H*(HSh-1)+H/2);
 x0=L;
 //по средним горизонтальным линиям шапки
 for(i=1;i<HSh-1;i++){
  y0=H*i; s0=S;
  for(s=StrSh[i];*s;s++){
   switch (*s){
    case 195: *s0++=' '; *s0++=' '; break;
    case 194: *s0++=' '; *s0++=' '; break;
    case 179: *s0++=' '; *s0++=' '; break;
    case 196: *s0++=' '; *s0++=' '; break;
    case 197: *s0++=' '; *s0++=' '; break;
    case 180: *s0++=' '; *s0++=' '; break;
    case 193: *s0++=' '; *s0++=' '; break;
    default: *s0++=ConvChrDOStoWIN(*s);
   }
  }
  *s0++=' '; *s0='\0';//рисуем текст
  Image->Canvas->TextOut(x0,y0,/*StrDOStoWIN(*/S/*)*/);
  S[0]='\0';
 }
//Рисуем внутренние вертикальные и горизонтальные линии
 for(i=0;i<HSh-1;i++){
  s=StrSh[i];
  if(*s==196&&i) { xh0=0; yh0=H*i+H/2; s++; }
  for(j=wcslen(s)+1;j--;s++){
   switch (*s){
    case 194: //рисуем вертикальные линии
     *s='\0'; xv0=L*wcslen(StrSh[i])+L; *s=194;
     yv0=H*i+H/2; yv1=H*(HSh-1)+H/2;
     Image->Canvas->MoveTo(xv0,yv0); Image->Canvas->LineTo(xv0,yv1);
     break;
    case 195: //горизонтальные  начало
     *s='\0'; xh0=L*(wcslen(StrSh[i])+2); *s=195;
     yh0=H*i+H/2; break;
    case 180: //горизонтальные  конец и рисование
     *s='\0'; xh1=L*(wcslen(StrSh[i])+2); *s=180;
     Image->Canvas->MoveTo(xh0,yh0); Image->Canvas->LineTo(xh1,yh0);
     xh0=-1; break;
    case 0:
     if(xh0>-1){
      xh1=L*(wcslen(StrSh[i])+2)+L/2;
      Image->Canvas->MoveTo(xh0,yh0); Image->Canvas->LineTo(xh1,yh0);
     }
     break;
}}}}
//---------------------------------------------------------------------------
void OutPutHead (FILE *FOut,wchar_t *Head,int LTab,int *KStr) {
/* Вывод заголовка таблицы в файл с его центровкой и подсчетом числа строк */
 wchar_t *h=Head,*s; int i,L;
 //*KStr=0;
/* if(OTSTUPTITL>0.0) {
  *KStr=OTSTUPTITL*6/25.2/INTERVAL;
  for(i=0;i<OTSTUPTITL/30;i++) fwprintf(FOut,L"\x1B\x4A%c",(wchar_t)255);
  fwprintf(FOut,L"\x1B\x4A%c",(wchar_t)(216/25.2*(OTSTUPTITL%30)));
 }*/
 for(;s=wcschr(h,'\n');) {
  *s='\0';
  L=(int)(s-h);
  (*KStr)++;
  for(i=0;i<(LTab-L)/2;i++) fputwc(' ',FOut); fwprintf(FOut,L"%s",StrWINtoDOS(h)); *s++='\n';
  for(i=0;i<LTab-L-(LTab-L)/2;i++) fputwc(' ',FOut); fputwc('\n',FOut); h=s;
 }
 L=wcslen(h);
 for(i=0;i<(LTab-L)/2;i++) fputwc(' ',FOut); fwprintf(FOut,L"%s",StrWINtoDOS(h));
 for(i=0;i<LTab-L-(LTab-L)/2;i++) fputwc(' ',FOut); fputwc('\n',FOut);
}
//---------------------------------------------------------------------------
void TKnopka::SetB(TTabControl *TabControl,wchar_t *S,int N){
 long BtnStyle;
 B=new TSpeedButton(TabControl);
 B->Parent=TabControl;
// BtnStyle=GetWindowLong(B->Handle,GWL_STYLE);
// BtnStyle|=BS_MULTILINE;
// SetWindowLong(B->Handle,GWL_STYLE,BtnStyle);
 B->Font->Name=L"Courier New"; B->Font->Size=FontSize;
 B->Caption=S; /*B->TabStop=false;*/ B->HelpContext=N;
// B->OnClick=FormEditList->ClickSort;
 B->OnMouseDown=FormEditList->MouseDown;
// if(Nn==Nk) B->OnContextPopup=FormEditList->ContextPopupClipping;
// B->Font->Color=255; B->Brush->Color=14869191;
}
//---------------------------------------------------------------------------
void TKnopka::SetColorB(TColor ColorPhonFix,TColor ColorText){
// B->Brush->Color=ColorPhonFix;
 B->Font->Color=(TColor)ColorText;
}
//---------------------------------------------------------------------------
TShapka::~TShapka(){
 if(FileSh) free(FileSh);
 for(TKnopka *r;r=R;){
  R=r->Sled;
  r->~TKnopka();
  free(r);
}}
//---------------------------------------------------------------------------
void TShapka::Color(TColor ColorPhonFix,TColor ColorText){
 TKnopka *r;
 for(r=R;r;r=r->Sled)
  r->SetColorB(ColorPhonFix,ColorText);
}
//---------------------------------------------------------------------------
void TShapka::Fill(TTabControl *TabControl){
 FILE *F=_wfopen(FileSh,L"r");
 int N=2,L=0,l,i,j; //в файле не содержатся
       //ограничивающие линии: верхняя и нижняя
 wchar_t **StrSh,*s,*s1; TKnopka **Pr=&R,*r,*rj;
 if(!F){
  swprintf(Inf,L"Не открылся файл шапки - <%s>",FileSh);
  Application->MessageBox(Inf,L"Невозможно показать таблицу!",
   MB_OK|MB_ICONWARNING);
  return;
 }
//Определяем число строк в шапке
 while(fgetws(Inf,1023,F)){N++; l=wcslen(Inf); if(l>L) L=l;}
 StrSh=(wchar_t **)calloc(N,SzV);
 rewind(F);
 for(i=1;i<N-1;i++) {
  StrSh[i]=(wchar_t *)calloc(L+1,SzC);
  fgetws(StrSh[i],1023,F);
  l=wcslen(StrSh[i]);
  s=StrSh[i]+l-1;
  if(l<L) { //если строка короче max, дописываем пробелы справа
   for(j=0;j<(L-l);j++) *s++=' '; *s=(wchar_t)179;
  }
  else if(*(s-1)==(wchar_t)196) *s=(wchar_t)180; else *s=(wchar_t)179;
 }
//формируем нулевую и последнюю строчки
 StrSh[0]=(wchar_t *)calloc(L+1,SzC);
 StrSh[N-1]=(wchar_t *)calloc(L+1,SzC);
 for(s1=StrSh[1],s=StrSh[0];*s1;s1++,s++)//строка с индексом 0
  if(*s1==(wchar_t)179) *s=(wchar_t)194; else *s=(wchar_t)196;
 *(s-1)=(wchar_t)191; *s='\0';
 for(s1=StrSh[N-2],s=StrSh[N-1];*s1;s1++,s++)
  if(*s1==(wchar_t)179) *s=(wchar_t)193; else *s=(wchar_t)196;
 *(s-1)=(wchar_t)217; *s='\0';
 fclose(F);
//F=_wfopen(L"fff",L"w");for(i=0;i<N;i++){fputws(StrSh[i],F); fputwc('\n',F);}fclose(F);
//Формируем список кнопок шапки
 for(i=0;i<N;i++)
  for(j=0;j<L;j++)
   switch((wchar_t)StrSh[i][j]) {
    case 194:
     r->SetXk(j);
     *Pr=r=(TKnopka *)calloc(1,sizeof(TKnopka)); Pr=(TKnopka **)r;
     r->SetXn(j); r->SetYn(i); break;
    case 180:
     r->SetXk(j); break;
    case 191:
     r->SetXk(j); break;
    case 196: if(j!=0) break;
    case 195:
     for(rj=R;rj;rj=rj->Sled)
      if((rj->GetXn()==j)&&!rj->GetYk()){rj->SetYk(i); break;}
     if(i<N-1){
      *Pr=r=(TKnopka *)calloc(1,sizeof(TKnopka)); Pr=(TKnopka **)r;
      r->SetXn(j); r->SetYn(i); break;
     } break;
    case 197:
     r->SetXk(j);
     for(rj=R;rj;rj=rj->Sled)
      if((rj->GetXn()==j)&&!rj->GetYk()){rj->SetYk(i); break;}
     *Pr=r=(TKnopka *)calloc(1,sizeof(TKnopka)); Pr=(TKnopka **)r;
     r->SetXn(j); r->SetYn(i); break;
    case 193:
     for(rj=R;rj;rj=rj->Sled)
      if((rj->GetXn()==j)&&!rj->GetYk()){rj->SetYk(i); break;}
     break;
   }
//Формируем список Button
 for(rj=R;rj;rj=rj->Sled) {
  wchar_t S[1024],*s=S;
  for(i=rj->GetYn()+1;i<rj->GetYk();i++) {
   int j0=rj->GetXn(),jk=rj->GetXk(); if(j0) j0++;
   for(j=j0;j<jk;j++)
    *s++=StrSh[i][j];
   *s++='\r'; *s++='\n';
  }
  *(s-2)='\0';
  StrDOStoWIN(S);
  rj->SetB(TabControl,S,rj->GetNn());
 }
 for(i=0;i<N;i++) free(StrSh[i]); free(StrSh);
}
//---------------------------------------------------------------------------
void TShapka::Ext(TStringGrid *SG){
 TKnopka *tk;
//Установим пиксельные координаты кнопкам
 for(tk=R;tk;tk=tk->Sled){ int L=4,W=0,i;
  for(i=0;i<tk->GetNn();i++)
   L+=SG->ColWidths[i]+(i?1:3);
  for(i=tk->GetNn();i<tk->GetNk()+1;i++)
   W+=SG->ColWidths[i]+(i?1:3);
  tk->SetLeft(L); tk->SetWidth(W);
}}
//---------------------------------------------------------------------------
void TShapka::SetLeft(TStringGrid *SG){ int L0,i,L; TKnopka *tk;
//Установим пиксельные координаты кнопкам
//посчитаем ширину неподвижных столбцов
//восстановим ширины кнопок
 for(tk=R;tk;tk=tk->Sled){ int W=0,i;
  for(i=tk->GetNn();i<tk->GetNk()+1;i++)
   W+=SG->ColWidths[i]+1;//(i?1:5);
  tk->SetWidth(W);
 }
 L0=5;
 for(i=0;i<SG->FixedCols;i++) L0+=SG->ColWidths[i]+1;
 for(tk=R;tk;tk=tk->Sled){
//если правый столбец кнопки находится до первого видимого
  i=tk->GetNk();
  if(i<(SG->FixedCols)) continue;//фиксированные столбцы на то и фиксированные,
//чтобы с ними НИЧЕГО НЕ ДЕЛАТЬ
  if(i<SG->LeftCol) tk->SetWidth(0);
//  else tk->SetWidth(SG->ColWidths[i]+(i?1:5));
//если левый столбец кнопки находится до первого видимого
  if(tk->GetNn()<SG->LeftCol) tk->SetLeft(L0);
  else{
   for(i=SG->LeftCol,L=L0;i<tk->GetNn();i++)
    L+=SG->ColWidths[i]+1;//(i?1:5);
   tk->SetLeft(L);
}}}
//---------------------------------------------------------------------------
void TShapka::SetLeftExt(int Old,TStringGrid *SG){
 int New=SG->LeftCol,L=0,i,L0=4; TKnopka *tk;
 //определим величину прокрутки
 for(i=0;i<SG->FixedCols;i++) L0+=SG->ColWidths[i]+1;
 if(New>Old) for(i=Old;i<New;i++) L+=SG->ColWidths[i]+1;
 else for(i=New;i<Old;i++) L-=SG->ColWidths[i]+1;
 for(tk=R;tk;tk=tk->Sled)
  if(tk->GetNn()>SG->FixedCols){//если это не фиксированная кнопка
   if(SG->FixedCols&&((tk->GetLeft()-L)<L0))
    tk->GetB()->Visible=false;
   else
    tk->GetB()->Visible=true;
   tk->SetLeft(tk->GetLeft()-L);
  }
 if(SG->FixedCols){
  for(tk=R;tk;tk=tk->Sled)
   if(!tk->GetB()->Visible) tk->GetB()->Visible=true;
  for(tk=R;tk;tk=tk->Sled){
   if(tk->GetNn()<SG->FixedCols) continue;
   if(tk->GetLeft()<L0) tk->GetB()->Visible=false;
}}}
//---------------------------------------------------------------------------
void TShapka::ExtFromCol(int Col,TStringGrid *SG){
 int W,i,L,L0=4; TKnopka *tk;
 for(tk=R;tk;tk=tk->Sled){
  if((Col>=tk->GetNn())&&(Col<=tk->GetNk())){
   for(i=tk->GetNn(),W=0;i<tk->GetNk()+1;i++)
    W+=SG->ColWidths[i]+(i?1:5);
   tk->SetWidth(W);
 }}
 for(i=0;i<SG->FixedCols;i++) L0+=SG->ColWidths[i]+1;
 for(tk=R;tk;tk=tk->Sled){
  if(tk->GetNk()<SG->FixedCols) continue;
  for(i=SG->LeftCol,L=L0;i<tk->GetNn();){
   if(tk->GetNn()>SG->LeftCol){
    L+=SG->ColWidths[i]+(i?1:5);
    tk->GetB()->Visible=true;
   }
   i++;
  }
  tk->SetLeft(L);
 }
}
//---------------------------------------------------------------------------
int CmpInt(const void *B1,const void *B2) {
 return (*(int *)B1)-(*(int *)B2);
}
//---------------------------------------------------------------------------
void TShapka::SetNnNk(void){ //массив абсолютных смещений границ столбцов
 int KolStlb=0,n,N=0,*Mks,i; TKnopka *tk,*tn;
//Найдем число строк N
 for(tk=R;tk;tk=tk->Sled) if(tk->GetYk()>N) N=tk->GetYk();
//Для кнопок, у которых Yk=N, заполним Nn и Nk и посчитаем колич.столбцов
 for(tk=R;tk;tk=tk->Sled) if(tk->GetYk()==N) KolStlb++;
 Mks=(int *)calloc(KolStlb,SzI);
 for(tk=R,i=0;tk;tk=tk->Sled) if(tk->GetYk()==N) Mks[i++]=tk->GetXk();
 qsort(Mks,KolStlb,SzI,CmpInt);
 for(tk=R;tk;tk=tk->Sled)
  if(tk->GetYk()==N) {
   n=NItemInMas(Mks,KolStlb,tk->GetXk()); tk->SetNn(n); tk->SetNk(n);
   tk->GetB()->HelpContext=n;
  }
 free(Mks);
//Заполним Nn и Nk для остальных
 for(tk=R;tk;tk=tk->Sled)
  if(tk->GetYk()!=N)
   for(tn=R;tn;tn=tn->Sled)
    if(tn->GetYk()==N){
     if(tn->GetXn()==tk->GetXn()) tk->SetNn(tn->GetNn());
     if(tn->GetXk()==tk->GetXk()) tk->SetNk(tn->GetNk());
    }
}
//---------------------------------------------------------------------------
int TKnopka::SetCoordHeight(void){
 B->Height=12*(Yk-Yn)*abs(B->Font->Height)/10; return B->Height;
};
//---------------------------------------------------------------------------
int TKnopka::SetCoordTop(int Top){
 B->Top=12*Yn*abs(B->Font->Height)/10+Top;
 return B->Top;
};
//---------------------------------------------------------------------------
int TShapka::SetHeightShapka(void){
 TKnopka *tk; int Ymax=0,Y;
 for(tk=R;tk;tk=tk->Sled){
  tk->SetCoordHeight(); Y=tk->GetYk(); if(Y>Ymax) Ymax=Y;
 }
 return 12*Ymax*abs(R->GetB()->Font->Height)/10;
}
//---------------------------------------------------------------------------
int TShapka::SetHeightShapkaPrn(int H/*FontHeight*/){
 TKnopka *tk; int Ymax=0,Y;
 for(tk=R;tk;tk=tk->Sled){
  Y=tk->GetYk(); if(Y>Ymax) Ymax=Y;
 }
// return Round(12*Ymax*abs(FontHeight)/10);
 return Ymax*H;
}
//---------------------------------------------------------------------------
int TShapka::SetTopShapka(int Top){
 TKnopka *tk;
 for(tk=R;tk;tk=tk->Sled)
  tk->SetCoordTop(Top);
 return R->SetCoordTop(Top);
}
// ---------------------------------------------------------------------------
void TKnopka::PrintKnopka(TPrinter *Prn,int L,int H,int X0,int Y0,
   int *Li,int xpn,int xpk){
//Считаем координаты углов кнопки в пикселях (X0 и Y0 в пикселях)
 int xn,xk,yn,yk,i,Y,X,Kn=KSimbStr(B->Caption.c_str(),'\n'),l,lp;
 wchar_t Caption[256],*h,*n;
 h=wcscpy(Caption,B->Caption.c_str());
 for(xn=0,i=0;i<Nn;i++) xn+=Li[i]+L;
 for(xk=0,i=0;i<=Nk;i++) xk+=Li[i]+L;
 if(xk<=xpn||xn>=xpk) return;
 yn=Y0+Yn*H; yk=Y0+Yk*H;
//Рисуем рамку кнопки
 if(xn>=xpn){
  Prn->Canvas->MoveTo(xn+X0,yn); Prn->Canvas->LineTo(xn+X0,yk);
 }
 if(xk<=xpk){
  Prn->Canvas->MoveTo(xk+X0,yn); Prn->Canvas->LineTo(xk+X0,yk);
 }
//верхняя горизонтальная
 Prn->Canvas->MoveTo((xn>=xpn?xn:xpn)+X0,yn);
 Prn->Canvas->LineTo((xk<=xpk?xk:xpk)+X0,yn);
//нижняя горизонтальная
 Prn->Canvas->MoveTo((xn>=xpn?xn:xpn)+X0,yk);
 Prn->Canvas->LineTo((xk<=xpk?xk:xpk)+X0,yk);
//пишем надпись кнопки
 Y=yn+H/2+H*(Yk-Yn-Kn-2)/2;
 lp=(xk<=xpk?xk:xpk)-(xn>=xpn?xn:xpn);
 Prn->Canvas->Font->Style=TFontStyles()<<fsBold;
 for(i=0;i<Kn;i++) {
  n=wcsstr(h,L"\r\n"); *n='\0';
  l=Prn->Canvas->TextWidth(h);
  X=xn+(lp-l)/2;
  if(X<xn) X=xn;
  Prn->Canvas->TextOut(X+X0,Y,h);
  Y+=H; *n++='\r'; *n++='\n'; h=n;
 }
 l=Prn->Canvas->TextWidth(h);
 X=xn+(lp-l)/2;
 if(X<xn) X=xn;
 Prn->Canvas->TextOut(X+X0,Y,h);
 Prn->Canvas->Font->Style=TFontStyles()>>fsBold;
}
//---------------------------------------------------------------------------
void TShapka::PrintShapka(TPrinter *Prn,int L,int H,int X0,int &Y0,
  int *Li,int NN,int NK){
 int xpn,xpk,i;
//печать шапки
 Prn->Canvas->Font->Style=TFontStyles()<<fsBold;
 for(xpn=0,i=0;i<NN;i++) xpn+=Li[i]+L;//начало области печати шапки
 for(xpk=0,i=0;i<=NK;i++) xpk+=Li[i]+L;//конец области печати шапки
 for(TKnopka *t=R;t;t=t->Sled) t->PrintKnopka(Prn,L,H,X0,Y0,Li,xpn,xpk);
 Y0+=SetHeightShapkaPrn(H/*Prn->Canvas->Font->Height*/);
 Prn->Canvas->Font->Style=TFontStyles()>>fsBold;
}
//---------------------------------------------------------------------------
/*void TKnopka::DrawKnopka(TCanvas *Canvas,int L,int H,int X0,int Y0,
   int *Li,int xpn,int xpk,double Kx,double Ky){
//Считаем координаты углов кнопки в пикселях (X0 и Y0 в пикселях)
 int xn,xk,yn,yk,i,Y,X,Kn=KSimbStr(B->Caption.c_str(),'\n'),l,lp;
 wchar_t Caption[256],*h,*n;
 h=wcscpy(Caption,B->Caption.c_str());
 for(xn=0,i=0;i<Nn;i++) xn+=Li[i]+L;
 for(xk=0,i=0;i<=Nk;i++) xk+=Li[i]+L;
 if(xk<=xpn||xn>=xpk) return;
 yn=Y0+Yn*H; yk=Y0+Yk*H;
//Рисуем рамку кнопки
 Canvas->Font->Style=TFontStyles()<<fsBold;
 if(xn>=xpn){
  Canvas->MoveTo(Round((xn+X0)*Kx),Round(yn*Ky));
  Canvas->LineTo(Round((xn+X0)*Kx),Round(yk*Ky));
 }
 if(xk<=xpk){
  Canvas->MoveTo(Round((xk+X0)*Kx),Round(yn*Ky));
  Canvas->LineTo(Round((xk+X0)*Kx),Round(yk*Ky));
 }
//верхняя горизонтальная
 Canvas->MoveTo(Round(((xn>=xpn?xn:xpn)+X0)*Kx),Round(yn*Ky));
 Canvas->LineTo(Round(((xk<=xpk?xk:xpk)+X0)*Kx),Round(yn*Ky));
//нижняя горизонтальная
 Canvas->MoveTo(Round(((xn>=xpn?xn:xpn)+X0)*Kx),Round(yk*Ky));
 Canvas->LineTo(Round(((xk<=xpk?xk:xpk)+X0)*Kx),Round(yk*Ky));
//пишем надпись кнопки
 Y=yn+H/2+H*(Yk-Yn-Kn-2)/2;
 lp=(xk<=xpk?xk:xpk)-(xn>=xpn?xn:xpn);
 for(i=0;i<Kn;i++) {
  if(n=wcsstr(h,L"\r\n")) *n='\0';
  l=Canvas->TextWidth(h)/Kx;
  X=xn+(lp-l)/2+1; if(X<xn) X=xn;
  Canvas->TextOut(Round((X+X0)*Kx),Round(Y*Ky),h);
  Y+=H; *n++='\r'; *n++='\n'; h=n;
 }
 l=Canvas->TextWidth(h)/Kx;
 X=xn+(lp-l)/2+1; //if(X<xn) X=xn;
 Canvas->TextOut(Round((X+X0)*Kx),Round(Y*Ky),h);
 Canvas->Font->Style=TFontStyles()>>fsBold;
} */
//---------------------------------------------------------------------------
void TKnopka::DrawKnopka(TCanvas *Canvas,int L,int H,int X0,int Y0,
   int *Li,int xpn,int xpk,double Kx,double Ky,Magazine *Ms){
//Считаем координаты углов кнопки в пикселях (X0 и Y0 в пикселях)
 int x,y,xn,xk,yn,yk,i,Y,X,Kn=KSimbStr(B->Caption.c_str(),'\n'),l/*,lp*/;
 wchar_t Caption[256],*h,*n; TRect Rect; Magazine *m;
 h=wcscpy(Caption,B->Caption.c_str());
 for(xn=0,i=0;i<Nn;i++){
  swprintf(Inf,L"%d",i); if(FindMagazine(Ms,Inf,&m)) xn+=Li[i]+L;
 }
 for(xk=0,i=0;i<=Nk;i++){
  swprintf(Inf,L"%d",i); if(FindMagazine(Ms,Inf,&m)) xk+=Li[i]+L;
 }
 if(xk<=xpn||xn>=xpk) return;
 yn=Y0+Yn*H; yk=Y0+Yk*H;
//Рисуем рамку кнопки
 Canvas->Font->Style=TFontStyles()<<fsBold;
 if(xn>=xpn){
  Canvas->MoveTo(Round((xn+X0)*Kx),Round(yn*Ky));
  Canvas->LineTo(Round((xn+X0)*Kx),Round((yk+H/5)*Ky));
 }
 if(xk<=xpk){
  Canvas->MoveTo(Round((xk+X0)*Kx),Round(yn*Ky));
  Canvas->LineTo(Round((xk+X0)*Kx),Round((yk+H/5)*Ky));
 }
//верхняя горизонтальная
 Canvas->MoveTo(Round(((xn>=xpn?xn:xpn)+X0)*Kx),Round(yn*Ky));
 Canvas->LineTo(Round(((xk<=xpk?xk:xpk)+X0)*Kx),Round(yn*Ky));
//нижняя горизонтальная
 Canvas->MoveTo(Round(((xn>=xpn?xn:xpn)+X0)*Kx),Round(yk*Ky));
 Canvas->LineTo(Round(((xk<=xpk?xk:xpk)+X0)*Kx),Round(yk*Ky));
//пишем надпись кнопки
 Y=yn+H/2+H*(Yk-Yn-Kn-2)/2;
// lp=(xk<=xpk?xk:xpk)-(xn>=xpn?xn:xpn);
 Rect.Left=(max(xn,xpn)+X0+L/2)*Kx; Rect.Right=(min(xk,xpk)+X0-L/2)*Kx;
 for(i=0;i<Kn;i++) {
  if(n=wcsstr(h,L"\r\n")) *n='\0';
  l=Canvas->TextWidth(h)/Kx;
  X=xn+(xk-xn-l)/2; //if(X<xn) X=xn;
  x=(int)Round((X+X0)*Kx)+1; y=(int)Round(Y*Ky);
  Rect.Top=Y*Ky; Rect.Bottom=(Y+H)*Ky;
  Canvas->TextRect(Rect,x,y,h);
  Y+=H; *n++='\r'; *n++='\n'; h=n;
 }
 l=Canvas->TextWidth(h)/Kx;
 X=xn+(xk-xn-l)/2; //if(X<xn) X=xn;
 x=(int)Round((X+X0)*Kx)+1; y=(int)Round(Y*Ky);
 Rect.Top=Y*Ky; Rect.Bottom=(Y+H)*Ky;
 Canvas->TextRect(Rect,x,y,h);
 Canvas->Font->Style=TFontStyles()>>fsBold;
}
//---------------------------------------------------------------------------
void TShapka::DrawShapka(TCanvas *Canvas,int L,int H,int X0,int &Y0,int *Li,
  int NN,int NK,double Kx,double Ky,Magazine *Ms){
 int xpn,xpk,i; Magazine *m;
   //рисование шапки на CANVAS
 for(xpn=0,i=0;i<NN;i++){
  swprintf(Inf,L"%d",i); if(!FindMagazine(Ms,Inf,&m)) continue;
  xpn+=Li[i]+L;//начало области рисования шапки
 }
 for(xpk=xpn;i<=NK;i++){
  swprintf(Inf,L"%d",i); if(!FindMagazine(Ms,Inf,&m)) continue;
  xpk+=Li[i]+L;//конец области рисования шапки
 }
 for(TKnopka *t=R;t;t=t->Sled){
  swprintf(Inf,L"%d",t->GetNn());
  if(FindMagazine(Ms,Inf,&m))
   t->DrawKnopka(Canvas,L,H,X0,Y0,Li,xpn,xpk,Kx,Ky,Ms);
 }  
 Y0+=SetHeightShapkaPrn(H);
}
//---------------------------------------------------------------------------
void TShapka::DefWidthButtonPrn(TCanvas *CanvasPrn,double Ktw,Magazine *Ms){
 wchar_t *s,*sr,Caption[256]; int L,l,Ks,i; Magazine *m;
 CanvasPrn->Font->Style=TFontStyles()<<fsBold;
 for(TKnopka *r=R;r;r=r->Sled){
  swprintf(Inf,L"%d",r->GetNn()); if(!FindMagazine(Ms,Inf,&m)) continue;
  wcscpy(Caption,r->GetB()->Caption.c_str());
  Ks=KSimbStr(Caption,'\r');
  for(i=0,s=Caption,L=0;i<Ks;i++){
   sr=wcschr(s,'\r'); *sr++='\0';
   l=CanvasPrn->TextWidth(s); if(l>L) L=l;
   s=sr+1;
  }
  l=CanvasPrn->TextWidth(s); if(l>L) L=l;
  r->SetWidthPrn(Round(Ktw*L));
 }
 CanvasPrn->Font->Style=TFontStyles()>>fsBold;
}
//---------------------------------------------------------------------------
void TShapka::DefWidthButtonPrn(int LChar,Magazine *Ms){
 wchar_t *s,*sr,Caption[256]; int L,l,Ks,i; Magazine *m;
 for(TKnopka *r=R;r;r=r->Sled){
  swprintf(Inf,L"%d",r->GetNn()); if(!FindMagazine(Ms,Inf,&m)) continue;
  wcscpy(Caption,r->GetB()->Caption.c_str());
  Ks=KSimbStr(Caption,'\r');
  for(i=0,s=Caption,L=0;i<Ks;i++){
   sr=wcschr(s,'\r'); *sr++='\0';
   l=wcslen(s)*LChar; if(l>L) L=l;
   s=sr+1;
  }
  l=wcslen(s)*LChar; if(l>L) L=l;
  r->SetWidthPrn(L);
}}
//---------------------------------------------------------------------------
void TShapka::DefWidthButtonColsPrn(int *Li,Magazine *Ms){ Magazine *m;
 for(TKnopka *r=R;r;r=r->Sled){
  swprintf(Inf,L"%d",r->GetNn()); if(!FindMagazine(Ms,Inf,&m)) continue;
  if((r->GetNn()==r->GetNk())&&(r->GetWidthPrn()>Li[r->GetNn()]))
   Li[r->GetNn()]=r->GetWidthPrn();
}}
//---------------------------------------------------------------------------

