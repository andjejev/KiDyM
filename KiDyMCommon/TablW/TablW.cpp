#include <vcl.h>
#include "TablW.h"
#include "ConvDW.h"
#include "EditList.h"
#include <math.h>
#include <sys\stat.h>
#include <stdio.h>
#include <io.h>
#include <stdlib.h>
#include <string.h>
#include <process.h>

wchar_t FILEMAS[256];
extern wchar_t Inf[];
extern TEditList *EditList;
//---------------------------------------------------------------------------
int KSimbStr(wchar_t *S,wchar_t C){ int K=0; while(*S) if(*S++==C) K++; return K; }
//---------------------------------------------------------------------------
void *Sled(void *p){ return (void *)*(unsigned long *)p; }
//---------------------------------------------------------------------------
int KolElem(void *P){ int k=0; for(;P;P=Sled(P)) k++; return k; }
//---------------------------------------------------------------------------
void DelStruct(void **Rout,void *S){
 void *s=*Rout; if(!S) return;
 if(S==*Rout){ *Rout=Sled(s); free(S); return; }
 for(;s;s=Sled(s))
  if(Sled(s)==S){
   *(void **)*(unsigned long *)(s)=Sled(S); free(S); return;
}}
//---------------------------------------------------------------------------
void DelAllList(void **Rout){
 for(void *P;P=*Rout;*Rout=Sled(P),free(P)); *Rout=NULL;
}
//---------------------------------------------------------------------------
bool FindAllMagazine(Magazine *R,wchar_t *S){
//поиск сплошняком в неотсортированном магазине
 for(Magazine *m=R;m;m=m->Sled) if(!wcscmp(m->S,S)) return true; return false;
}
//---------------------------------------------------------------------------
Magazine *FindMagazineAll(Magazine *R,wchar_t *S){
//поиск сплошняком в неотсортированном магазине
 for(Magazine *m=R;m;m=m->Sled) if(!wcscmp(m->S,S)) return m; return NULL;
}
//---------------------------------------------------------------------------
/* процедуры поиска элементов по имени в соответствующих списках.
   возвращают:
    0,если элемент надо ставить в начало списка
    1,если такой элемент найден - его адрес находится в последнем
       формальном параметре: */
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
 m->S=wcscpy((wchar_t *)malloc(SzC*(wcslen(S)+1)),S);
 if(!M){ m->Sled=*Rout;   return   *Rout=m; }
 else  { m->Sled=M->Sled; return M->Sled=m; }
}
//---------------------------------------------------------------------------
Magazine *MakeMagazine(Magazine **Rout,wchar_t *S){
 Magazine *M,*m; if(!S) return NULL; if(FindMagazine(*Rout,S,&M)) return M;
 m=(Magazine *)calloc(1,sizeof(Magazine)); m->S=S;
 if(!M){ m->Sled=*Rout;   return   *Rout=m; }
 else   { m->Sled=M->Sled; return M->Sled=m; }
}
//---------------------------------------------------------------------------
Magazine *AddMagazine(Magazine **Rout,wchar_t *S){
 Magazine *M,*m;
 if(!S) return NULL;
 M=(Magazine *)calloc(1,sizeof(Magazine));
 M->S=wcscpy((wchar_t *)malloc(SzC*(wcslen(S)+1)),S);
 if(!(*Rout)) *Rout=M;
 else{ for(m=*Rout;m->Sled;m=m->Sled); m->Sled=M; }
 return M;
}
//---------------------------------------------------------------------------
Magazine *FileToMagazin(wchar_t *NameFile){//Зачитывание с сортировкой и преобраз из ДОС в ВИН
 FILE *F; Magazine *Rout=NULL; wchar_t B,S[256],*s=S;
 if(!(F=_wfopen(NameFile,L"r"))){ return NULL; }
 while(!feof(F)) switch(B=fgetwc(F)){
  case '\n': *s='\0'; StrDOStoWIN(S); TakeMagazine(&Rout,S); s=S; break;
  default: *s++=B;
 }
 fclose(F); return Rout;
}
//---------------------------------------------------------------------------
Magazine *FileToMagazinBezDW(wchar_t *NameFile){//Зачитывание с сортировкой и без преобраз
 FILE *F; Magazine *Rout=NULL; wchar_t B,S[256],*s=S;
 if(!(F=_wfopen(NameFile,L"r"))){ return NULL; }
 while(!feof(F)) switch(B=fgetwc(F)){
  case '\n': *s='\0'; /*StrDOStoWIN(S); */TakeMagazine(&Rout,S); s=S; break;
  default: *s++=B;
 }
 fclose(F); return Rout;
}
//---------------------------------------------------------------------------
int MagazineToFile(wchar_t *NameFile,Magazine *R){ Magazine *m; FILE *F;
 wchar_t S[256];    //с преобраз WINtoDOS
 if(!(F=_wfopen(NameFile,L"w"))) return 0;
 for(m=R;m;m=m->Sled){
  swprintf(S,L"%s",m->S);
  fwprintf(F,L"%s\n",StrWINtoDOS(S));
 }
 fclose(F); return 1;
}
//---------------------------------------------------------------------------
int MagazineToFileBezDW(wchar_t *NameFile,Magazine *R){ Magazine *m; FILE *F;
 wchar_t S[256];    //без преобраз WINtoDOS
 if(!(F=_wfopen(NameFile,L"w"))) return 0;
 for(m=R;m;m=m->Sled){
  swprintf(S,L"%s",m->S);
  fwprintf(F,L"%s\n",/*StrWINtoDOS(*/S);
 }
 fclose(F); return 1;
}
//---------------------------------------------------------------------------
void DelAllMagazine(Magazine **R){
 for(Magazine *M;M=*R;){ *R=M->Sled; if(M->S) free(M->S); free(M); }
}
//---------------------------------------------------------------------------
void DelMagazineCmp(Magazine **Rout,wchar_t *S){
 if(!*Rout) return;
 Magazine *M,*m=M=*Rout;
 if(!wcscmp(M->S,S)) *Rout=M->Sled;
 for(;M=m->Sled;m=m->Sled) if(!wcscmp(M->S,S)){ m->Sled=M->Sled; break; }
 if(M) free(M);
}
//---------------------------------------------------------------------------
void DelMagazine(Magazine **Rout,Magazine *mm){
 Magazine *M,*m=M=*Rout;
 if(!*Rout) return;
 if(mm==M) *Rout=M->Sled;
 for(;M=m->Sled;m=m->Sled) if(mm==M){ m->Sled=M->Sled; break; }
 if(M) free(M);
}
//---------------------------------------------------------------------------
bool ErrorChar(wchar_t B,int Out,wchar_t *S){
 wchar_t Inf[256],Esc='\xb3';
 //изменить кодировку
 if((B==' ')||(B=='.')||(B==',')||(B=='/')||(B=='\\')||(B=='-')||(B=='"')||
    (B=='+')||(B=='*')||(B=='!')||(B=='?')||(B=='@')||(B=='#')||(B=='.')||
    (B=='$')||(B=='%')||(B=='(')||(B==')')||(B=='{')||(B=='}')||(B==':')||
    (B=='[')||(B==']')||(B==Esc)||(B=='|')||(B=='\n')||(B=='=')||
    (B=='&')||(B=='>')||(B=='<')||isdigit(B)||isalpha(B)||isciril(B))
     return false;
 if(Out)
  swprintf(Inf,L"Недопустимый символ '%c' в %s.\n"
   L"Запоминание не рекомендуется.",B,S);
 else
  swprintf(Inf,L"Недопустимый символ '%c' в файле \"%s\".\n"
   L"Чтение информации прекращено.",B,S);
  Application->MessageBox(Inf,L"Обнаружен недопустимый символ!",
     MB_OK|MB_ICONINFORMATION);
  return true;
}
//---------------------------------------------------------------------------
bool ErrorString(wchar_t *S,int Out,wchar_t *Mess){
 for(;S&&*S;*S++){
  if(ErrorChar(*S,Out,Mess)){
   swprintf(Inf,L"Недопустимый символ '%c' в строке %s в \"%s\".\n",*S,S,Mess);
   Application->MessageBox(Inf,L"Обнаружен недопустимый символ!",
    MB_OK|MB_ICONINFORMATION);
   return true;
 }}
 return false;
}
//---------------------------------------------------------------------------
Magazine *FileToMagazinUnSort(wchar_t *NameFile,bool DOS){//без соротировки
 FILE *F; Magazine *Root=NULL,*m,**Pm=&Root; wchar_t B,S[256],*s=S;
 long L=0,Lf; struct _stat St;
 if(!(F=_wfopen(NameFile,L"r"))||feof(F)) return NULL;
 _wstat(NameFile,&St); Lf=St.st_size;
 while(!feof(F)) switch(B=fgetwc(F)){
  case '\n': *s++='\0'; L+=2;
   if(DOS) StrDOStoWIN(S);
   *Pm=m=(Magazine *)calloc(1,sizeof(Magazine)); Pm=(Magazine **)(*Pm);
   m->S=wcscpy((wchar_t *)malloc(((int)(s-S))*SzC),S); s=S;  break;
  default: *s++=B; L++;
 } fclose(F);
 if((L-1)!=Lf){
  swprintf(S,L"Длина прочитанной информации из магазина %s"
   L"не совпадает с длиной файла.",NameFile);
  Application->MessageBox(S,L"Файле не прочитался!",MB_OK|MB_ICONINFORMATION);
 }
 return Root;
}
//---------------------------------------------------------------------------
int MagazinToFile(Magazine *M,wchar_t *NameFile){
/* Вывод L"магазина" строк M в файл с именем NameFile. Строки в файле
   отделяются символом '\n'. Возвращает 1 при успехе и 0,если файл
   нельзя открыть. */
 unsigned long L=0L,Lf,Tf,T=time(NULL); struct _stat St;
 wchar_t NewMag[]=L"newmag.",*Tchk,Bak[256],Mess[256]; FILE *F;
 if(!M||!(F=_wfopen(NewMag,L"w"))) return 0;
 if(Tchk=wcschr(NameFile,'.')) *Tchk='\0'; swprintf(Bak,L"%s.bak",NameFile);
 if(Tchk) *Tchk='.';
 for(;M;M=M->Sled){
  wcscpy(Inf,M->S); StrWINtoDOS(Inf);
  if(!ErrorString(M->S,1,NameFile)) L+=fwprintf(F,L"%s\n",Inf)+1;
 }
 fclose(F);
 if(!_waccess(NewMag,0)){
  _wstat(NewMag,&St); Tf=St.st_mtime+1; Lf=St.st_size;
  if(Tf>=T&&Lf==L){
   _wunlink(Bak); _wrename(NameFile,Bak); _wrename(NewMag,NameFile);
  }
  else {
   if(Lf!=L){
    swprintf(Mess,L"Запомнить магазин в файле \"%s\" не удалось.\n"
     L"Длина. Старый сохранен.\n"
     L"Tf=%lu, T=%lu, Lf=%lu, L=%lu",NameFile,Tf,T,Lf,L);
	Application->MessageBox(Mess,L"Ошибка!",MB_OK|MB_ICONERROR);
	_wunlink(NewMag);
   }
   else {
	swprintf(Mess,L"Запомнить магазин в файле \"%s\" не удалось.\n"
	 L"Время. Старый сохранен.\n"
	 L"Tf=%lu, T=%lu, Lf=%lu, L=%lu",NameFile,Tf,T,Lf,L);
	Application->MessageBox(Mess,L"Ошибка!",MB_OK|MB_ICONERROR);
    _wunlink(NewMag);
   }
 }} return 1;
}
//---------------------------------------------------------------------------
/* Вывод в файл F N символов B: */
void PutNc(wchar_t B,FILE *F,int N){ while(N--) fputwc(B,F); }
//---------------------------------------------------------------------------
/* Вывод в поле размера Size в файл F строку S с центровкой в поле: */
void PutCentrS(wchar_t *S,FILE *F,int Size){
 int  Ls=wcslen(S);
 if(Ls>Size) PutNc('*',F,Size);
 else {
  PutNc(' ',F,(Size-Ls)/2); fputws(S,F); PutNc(' ',F,Size-((Size-Ls)/2)-Ls);
}}
//---------------------------------------------------------------------------
void StringTabl(wchar_t *S,wchar_t *MasD,FILE *F){
/* Вывод в файл F строки таблицы S,размеры графок которой задаются
   строкой MasD вида: L"#<целое>#<целое>...". Строка S должна иметь вид:
   <символ(ы)><разделитель><символ(ы)><разделитель>...<символ(ы)>.
   В качестве разделителя могут использоваться символы:
    '¦','¦','¦','¦','+','+','¦','¦','+','+','+','+','+','+','-',
     '-','+','+','¦','¦','+','+','-','-','¦','+','-','-','-','-',
    '+','+','+','+','+','+','+','+'.
   В качестве отдельного символа может быть ' ','-','='. В этом слу-
   чае они выводятся в файл в количестве,задаваемом MasD.
   Символы между разделителями будут выведены в файл F  и отцентрованы
   в поле размером,заданным в MasD.                                  */
 wchar_t B,*s=S,*m=MasD+1;
 for(;;){ B=*s++;
  if(B>178&&B<196||B>196&&B<205||B>205&&B<219||!B){
   *(s-1)='\0';
   switch(*S){
    case ' ':case '\xc4':case '\xcd':
	 if(!(S[1])){ PutNc(*S,F,_wtoi(m)); break; }
	default: PutCentrS(S,F,_wtoi(m));
   }
   if(B){
    *(s-1)=B;
    fputwc(B,F);
    S=s; while(*m++!='#');
   }
   else {
    fputwc('\n',F); return;
}}}}
//---------------------------------------------------------------------------
void ReplaceChar(wchar_t *S,wchar_t Bz,wchar_t Bt){
//замена одного символа на другой
 while(*S){ if(*S==Bz) *S=Bt; S++; }
}
//---------------------------------------------------------------------------
static FILE *Fmas; static int KStrMas; static wchar_t FileMas[13];
//---------------------------------------------------------------------------
int InitMasLinF(){
 wcscpy(FileMas,FILEMAS); KStrMas=0;
 if(Fmas=_wfopen(FileMas,L"wb")) fclose(Fmas); else return 0;
 if(Fmas=_wfopen(FileMas,L"r+b")) return 1; return 0;
}
//---------------------------------------------------------------------------
long *MasLinF::ReadML(int K,long *L){
 if(!Pos) for(int i=0;i<K;i++) L[i]=0L;
 else{
  fseek(Fmas,Pos-1,0); fread(L,SzL,K,Fmas);
 }
 return L;
}
//---------------------------------------------------------------------------
long MasLinF::ReadLi(int i){ long L;
 if(!Pos) return 0L;
 fseek(Fmas,Pos-1+i*SzL,0); fread(&L,SzL,1,Fmas); return L;
}
//---------------------------------------------------------------------------
void MasLinF::WriteML(int K,long *L){
 if(!KStrMas){ if(!InitMasLinF()) return; }
 if(!Pos||(N<K)) this->Calloc(K);
 if(Pos&&(N>=K)){ fseek(Fmas,Pos-1,0); fwrite(L,SzL,K,Fmas); }
}
//---------------------------------------------------------------------------
void MasLinF::WriteLi(int i,long L){
 fseek(Fmas,Pos-1+i*SzL,0); fwrite(&L,SzL,1,Fmas);
}
//---------------------------------------------------------------------------
void MasLinF::AddLi(int i,long D){ long L;
 fseek(Fmas,Pos-1+i*SzL,0); fread(&L,SzL,1,Fmas);
 L+=D;
 fseek(Fmas,-(long)SzL,1); fwrite(&L,SzL,1,Fmas);
}
//---------------------------------------------------------------------------
void MasLinF::Free(){
 if(Pos==0L||KStrMas==0) return;
 if(KStrMas>0) KStrMas--;
 if(KStrMas==0){ fclose(Fmas); _wunlink(FileMas); }
}
//---------------------------------------------------------------------------
void MasLinF::Calloc(int K){ long L=0L;
 if(!KStrMas&&!Fmas&&!InitMasLinF()){ Pos=0L; return; }
 if(K>N){
  KStrMas++; fseek(Fmas,0,2); Pos=ftell(Fmas)+1; N=K;
 }
 else fseek(Fmas,Pos-1,0);
 for(int i=0;i<K;i++) fwrite(&L,SzL,1,Fmas);
}
//---------------------------------------------------------------------------
static FILE *FmasD; static int KStrMasD; static wchar_t FileMasD[13];
//---------------------------------------------------------------------------
int InitMasDinF(){
 wcscpy(FileMasD,FILEMAS); KStrMasD=0;
 if(FmasD=_wfopen(FileMasD,L"wb")) fclose(FmasD); else return 0;
 if(FmasD=_wfopen(FileMasD,L"w+b")) return 1; return 0;
}
//---------------------------------------------------------------------------
double *MasDinF::ReadMD(int K,double *D){
 if(!Pos) for(int i=0;i<K;i++) D[i]=0.0;
 else{
  fseek(FmasD,Pos-1,0); fread(D,SzD,K,FmasD);
 }
 return D;
}
//---------------------------------------------------------------------------
double MasDinF::ReadDi(int i){
 double D;
 if(!Pos) return 0.0;
 fseek(FmasD,Pos-1+i*SzD,0); fread(&D,SzD,1,FmasD); return D;
}
//---------------------------------------------------------------------------
void MasDinF::WriteMD(int K,double *D){
 if(!KStrMasD){ InitMasDinF(); Pos=1; }
 else { fseek(FmasD,0,2); Pos=ftell(FmasD)+1; }
 fwrite(D,SzD,K,FmasD); KStrMasD++;
}
//---------------------------------------------------------------------------
void MasDinF::WriteDi(int i,double D){
 fseek(FmasD,Pos-1+i*SzD,0); fwrite(&D,SzD,1,FmasD);
}
//---------------------------------------------------------------------------
void MasDinF::AddDi(int i,double d){ double D;
 fseek(FmasD,Pos-1+i*SzD,0); fread(&D,SzD,1,FmasD); D+=d;
 fseek(FmasD,Pos-1+i*SzD,0); fwrite(&D,SzD,1,FmasD);
}
//---------------------------------------------------------------------------
void MasDinF::Free(){ //освобождение одной строки
 if(Pos==0L||KStrMasD==0) return;
 if(KStrMasD>0) KStrMasD--;
 if(KStrMasD==0){ fclose(FmasD); _wunlink(FileMasD); }
}
//---------------------------------------------------------------------------
void MasDinF::Calloc(int K){ double D=0.0; //захват одной строки
 if(!KStrMasD&&!FmasD&&!InitMasDinF()){ Pos=0L; return; }
 if(K>N){
  KStrMasD++; fseek(FmasD,0,2); Pos=ftell(FmasD)+1; N=K;
 }
 else fseek(FmasD,Pos-1,0);
 for(int i=0;i<K;i++) fwrite(&D,SzD,1,FmasD);
}
//---------------------------------------------------------------------------
long DtoK(double D){ D*=100.0; return (long)(D>=0.0?floor(D+0.5):ceil(D-0.5)); }
//---------------------------------------------------------------------------
void PoleWINtoDOS(wchar_t *Pole,wchar_t *Buf){
//преобразование строкового поля структуры в кодировке ANSI в
//кодировку ASCII и записи результата преобразования в буфер Buf
 if(Pole){ wcscpy(Buf,Pole); StrWINtoDOS(Buf); } else Buf[0]='\0';
}
//---------------------------------------------------------------------------
void **LtoM(void *Rout){
 int i,k=KolElem(Rout); void **M=(void **)malloc(k*SzV),**m=M;
 for(i=0;i<k;i++,Rout=Sled(Rout)) *m++=Rout; return M;
}
//---------------------------------------------------------------------------
void MtoL(void **M,int N){ void *P; int i=1;
 for(;i<N;i++){
  P=*M++; *(void **)*(unsigned long *)P=*M;
 }
 *(void **)*(unsigned long *)(*M)=NULL;
}
//---------------------------------------------------------------------------
wchar_t *FindKSimb(wchar_t *S,wchar_t B,int N){
 for(int i=0;*S;S++)
  if(*S==B){ i++; if(i==N) return S; }
 return NULL;
}
//---------------------------------------------------------------------------
wchar_t *StrN(wchar_t *S,wchar_t B,int N,wchar_t *Buf){
//процедура выдачи подстроки с номером N из строки S, в которой
//подстроки разделены символом B. Результат записывается в строку
//Buf, адрес которой возвращается
 wchar_t *s=S,*s0,*b=Buf; int K=KSimbStr(S,B);
 if(N<0||N>K) return NULL;
 for(int i=0;i<N;i++){ s=wcschr(s,B); s++; }
 s0=wcschr(s,B); if(!s0) s0=S+wcslen(S);
 while(s<s0) *b++=*s++; *b='\0';
 return Buf;
}
//---------------------------------------------------------------------------
int NStrMag (Magazine *M,wchar_t *S){ int k=0;
 if(!S) return -1; for(;M;M=M->Sled,k++) if(M->S==S) return k; return -1;
}
//---------------------------------------------------------------------------
int NStrMagCmp(Magazine *M,wchar_t *S){ int k=0;
 if(!S) return -1;
 for(;M;M=M->Sled,k++) if(!wcscmp(M->S,S)) return k; return -1;
}
//---------------------------------------------------------------------------
wchar_t *StrN(Magazine *M,int N){
 int k=0; if(N<0||N>KolElem(M)) return NULL;
 for(;M&&(k<N);M=M->Sled,k++); return M->S;
}
//---------------------------------------------------------------------------
int NItemInMas(int *Iv,int N,int i){
 for(int k=0;k<N;k++) if(Iv[k]==i) return k; return -1;
}
//---------------------------------------------------------------------------
/* переворачиваем список: */
void InverList(void **P){ void *p,*pp,*Sp;
 for(p=*P,*P=NULL;p;){ Sp=Sled(p); pp=Sp; /*Sp=*P;*/ *P=p; p=pp; }
}
//---------------------------------------------------------------------------
bool Pusta(String S){
 wchar_t Sc[2048];
 wcscpy(Sc,S.w_str());
 for(wchar_t *s=Sc;*s;s++) if(*s!=' ') return false;
 return true;
}
//---------------------------------------------------------------------------
int ReadStrOpt(wchar_t *Str,FILE *F){ wchar_t S[256],*s;
 if(feof(F)||!Fgets(S,255,F)) return 0;
 if((s=wcschr(S,'#'))||(s=wcschr(S,'\n'))) s--; else s=S+wcslen(S)-1;
 while(*s==' ') s--; *(++s)='\0'; wcscpy(Str,S); return 1;
}
//---------------------------------------------------------------------------
int NChar(wchar_t B){ int N=-10;
 wchar_t *r,*Alf=L"АБВГДЕЁЄЖЗИІIЇЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯабвгдеёєжзиіiїйклмнопрстуфхцчшщъыьэюя";
 if(!B) return -11;
 for(r=Alf;*r;r++,N++)
  if(*r==B)return N;
 return int(B);
};
//---------------------------------------------------------------------------
int CmpFioUkr(wchar_t *LFio1,wchar_t *LFio2){
 wchar_t *b1,*b2;
 if(!LFio1&&!LFio2) return 0;
 if(!LFio1) return -1; if(!LFio2) return 1;
 for(b1=LFio1,b2=LFio2;*b1&&*b2;b1++,b2++)
  if(*b1!=*b2) break;
 return NChar(*b1)-NChar(*b2);
};
//---------------------------------------------------------------------------
void DelAllBalanc (Balanc **Rout){ Balanc *B=*Rout;
 for(;B;B=*Rout ){*Rout=B->Sled; free(B);} *Rout=NULL;
}
//---------------------------------------------------------------------------
Magazine *FindMag (Magazine *M,wchar_t *S){
 for(;M;M=M->Sled) if(wcsstr(M->S,S)==M->S) return M; return NULL;
}
//---------------------------------------------------------------------------
Magazine *ShortMag (Magazine *M){
 Magazine *Mn=NULL,*mn,*m,**Pm=&Mn; wchar_t *BEG,*BL;
 for(m=M;m;m=m->Sled){
  for(BEG=m->S;*BEG==' ';BEG++);
  if(BL=wcschr(BEG,' ')) *BL='\0';
  *Pm=mn=(Magazine *)calloc(1,sizeof(Magazine)); Pm=(Magazine **)mn;
  mn->S=wcscpy((wchar_t *)calloc(wcslen(BEG)+1,SzC),BEG);
  if(BL) *BL=' ';
 }
 return Mn;
}
//---------------------------------------------------------------------------
void UnPack(wchar_t *LHA,wchar_t *Arhiv,wchar_t *File){
 _wspawnl(P_WAIT,LHA,LHA,L"x",Arhiv,File,NULL);
// swprintf(Inf,L"%s x %s %s",LHA,Arhiv,File);
// WinExec(Inf,SW_HIDE);
}
//---------------------------------------------------------------------------
void Pack(wchar_t *LHA,wchar_t *Arhiv,wchar_t *File){
// CopyToArhiv(Arhiv,File);
 _wspawnl(P_WAIT,LHA,LHA,L"u",Arhiv,File,NULL);
// swprintf(Inf,L"%s u %s %s",LHA,Arhiv,File);
// WinExec(Inf,SW_HIDE);
}
//---------------------------------------------------------------------------
wchar_t *Pole(wchar_t *s){
 for(;*s&&(*s!=L'\xb3')&&(*s!=L'\n');s++); *s++=L'\0'; return s;
}
//---------------------------------------------------------------------------
/*void ViewMagazine(Magazine *M,wchar_t *Zag,wchar_t *Shapka,wchar_t *Help){
//Просмотр магазина
 wchar_t *FileSh=L"shviewmag",*Format=L"%-s",*Fprn=L"viewmag.prn",
  **Head=(wchar_t **)calloc(1,SzV);
 int i,*MNN=(int *)calloc(2,SzI); FILE *F; MNN[0]=1;
 if(F=_wfopen(L"shviewmag",L"w")){ Fputs(Shapka,F); fputwc('\n',F); fclose(F); }
 Head[0]=(wchar_t *)calloc(256,SzV); wcscpy(Head[0],L"Просмотр");
 EditList=new TEditList((void **)&M,MNN,false,NULL,false,NULL,Format,FileSh,
  Head,Fprn,NULL,NULL,NULL,0,-1,-1,-1,-1,Zag,NULL,1,Help,NULL,NULL);
 if(EditList->Diagnos()){
  Application->CreateForm(__classid(TFormEditList),&FormEditList);
  FormEditList->ShowModal();
 }
 if(Head){ if(Head[0]) free(Head[0]); free(Head); }
 if(MNN) free(MNN); if(Help) free(Help); delete EditList;
}*/
//---------------------------------------------------------------------------
DouSt& DouSt::operator = (DouSt DS){
 if((Atr==IsS)&&U.S){ free(U.S); U.S=NULL; }
 switch(DS.Atr){
  case IsS:
   switch(Atr){
    case IsS:
     if(!DS.U.S) U.S=NULL;
     else{ unsigned int L=wcslen(DS.U.S);
      if(!U.S) U.S=(wchar_t *)calloc(SzC*(L+1),SzC);
      else if(wcslen(U.S)<L+1) U.S=(wchar_t *)realloc(U.S,(L+1)*SzC);
      wcscpy(U.S,DS.U.S);
     }
     break;
    case IsD:
     if(!DS.U.S) U.S=NULL;
     else{
      unsigned int L=wcslen(DS.U.S);
      U.S=(wchar_t *)calloc(L+1,SzC);
      wcscpy(U.S,DS.U.S);
     }
     break;
   }
   break;
  case IsD:
   U.D=DS.U.D;
   break;
 }
 Atr=DS.Atr;
 return *this;
}
//---------------------------------------------------------------------------
DouSt& DouSt::operator = (wchar_t *S){
 if(Atr==IsS){
  if(U.S){ free(U.S); U.S=NULL; }
  if(!S){ U.S=NULL; return *this; }
 }
 else if(!S){ U.S=NULL; return *this; }
 U.S=wcscpy((wchar_t *)calloc(SzC*(wcslen(S)+1),SzC),S);
 Atr=IsS;
 return *this;
}
//---------------------------------------------------------------------------
DouSt& DouSt::operator = (double D){
 if(Atr==IsS){ Atr=IsD; if(U.S){ free(U.S); U.S=NULL; }}
 U.D=D;
 return *this;
}
//---------------------------------------------------------------------------

