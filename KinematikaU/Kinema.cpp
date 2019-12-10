//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "Kinema.h"
#include "FormPrRE.h"
#include "AboutKin.h"
#include <string.h>
#include <io.h>
#include <dir.h>
#include <errno.h>
#include <process.h>
#include <values.h>
#include <extctrls.hpp>
//#include "ConvDW.h"
#include <printers.hpp>
#include <jpeg.hpp>
#include <sys/stat.h>
#include <Registry.hpp>
#include "time.h"
#include "math.h"
#include "Colors.h"
#include "ParamPrint.h"
#include "OtchetForm.h"
#include "OperMess.h"
#include "EditList.h"
#include "MultFilm.h"
//#include "CalcCordKontrPoint.h"
//#include "IdentSizes.h"
//---------------------------------------------------------------------------
#define COUNTTABL 0
#define WIDTHPEN  1
#define TMULT     2
#define KKADR     3
#define KBMP      4
#define KEQVSCAL  5
#define EPS       6
#define KCOLUMN   7

#pragma package(smart_init)
#pragma link "SHDocVw_OCX"
#pragma link "CGAUGES"
#pragma resource "*.dfm"
using namespace std;

extern TFormOper *FormOper;
extern TFormColors *FormColors;
extern TFormParamPrint *FormParamPrint;
//extern TFormOtchet *FormOtchet;
extern PACKAGE TFormOtchet *FormOtchet;
extern Cnst *Nul,*Pust;
extern List *LBeg;
extern bool IsError,IsSyntError,OWN,SMALLFONT;
extern long PozFile;
extern List *L;
extern wchar_t InpFile[],ErrFile[],Inf[],DgsFile[],HtmFile[],OutFile[],OprFile[],
 WORKDIR[],HeadWork[],HeadVersion[],Autor[],CurrDir[],BMPFile[],JPGFile[],
 HeadCalc[],LANGUAGELITERA,BMPKadr[256],BMPKadrs[256],JPGKadrs[256],Dir[256],
 AtrInerc,AtrElast,AtrForse,AtrNelin,AtrDisip;
extern int N,PUNKT,NzK,NPV,StrLong,Kinst,Nkadr;
extern TJPEGImage *JPEG;
extern Centroid *Rcentr; //корень списка центроид
extern Trajectory *Rtraek; //корень списка траекторий
extern FILE *PRNT_V;
//---------------------------------------------------------------------------
TImage *Image; TEditList *EditList;
TFormKinema *FormKinema; TMultFilms *MultFilms; Form TLarin;
TRichEdit *RichEdit; Root *InsTime; TImage *ImageBMP;
wchar_t OptFile[256],BrowserName[80],SimbInd='$',NameCalc[256],
 FILESHAP[256]=L"",File_tz[256];//,ANSIFile[256];
long TMult=5000,DateInp;
int Nt,I,J,Interval=50,DiscretTyp,WidthPen,lmax=96,Kkadr=50,
 NDIG=15,**Xs,**Ys,sign,KColumn=3,NYear,Kpodv,MAXITER=100,KP[3],
 Kmg,dS[2],Wg,ss_RK,*SzFont,*PL,*PR,timer,DeltaTime,VELOKIN,
 KIter,USEDLL;
FILE *OPRFILE,*INFILE,*ERRFILE,*HTMFILE,*DGSFILE,*TMPFILE;
TStringList *SLI,*SLH,*SLD,*SLO,*SLE,*SLT,*SLP,*SLK,*SLF,*SLM;
String SF;
wchar_t Shtm[1024],Sdgs[1024],Sopr[1024],Serr[1024],Stmp[1024],
 *shtm,*sdgs,*sopr,*serr,*stmp;
Vary *Vp,*VgX,*VgY,*VgZ,*VArg,*V_d_YC_at_BdR_po_d_alfa_R_po_t;
Form VgXt,VgYt,VgZt,VgXtt,VgYtt,VgZtt,FVp,FIntg;
bool NoCalc,IsNoCalc,GRAPHICS,SHLEIF,Otladka,PRINTABL=true,
 PRINTOP,TOJPEG,TOBMP,RENOVCALC,ALLOW,PERMUT,RESIZE=false,
 ArgTime=true,VISIBLETIMEINTEGR,TABLTOFILE,OKTOFILE,
 PRNTOPRIGHT=true,PRINTHEAD=true,HTMLREPORT=true,
 VISIBLEKINETICENERGY,OTLADKA,VISIBLEDISSIPFUNCTION,
 VISIBLEPOTENENERGY,VISIBLEAPPELFUNCTION,ISINCOMPLETEIF,
 WriteRezFile,INVERSE,LAGRANGE,*BORDER,REPVARNACNST=true,CPP,
 PRINTVARS,ISRAZM,ALL_ANGLES,UNICOD;
double Kbmp=5.0/4.0,KTime=1.0,**Qxyz,**Qmf,*YK,*YKmin,*YKmax,
 *YKans,*dYK,*Y,**Q,**S,Eta,Eps,X_n,X_k,Y_n,Y_k,Z_n,Z_k,Sg,
 KEqvScal=2.0,*Qtz,*q,Tend,MSt[3],Min[3],Max[3],Slv[3],Spr[3],
 Low[3],Hig[3],Ner[3],Far[3],Tn,Tk,MG[10],MstT[3],
 **MA,*VB,*q0,*RK_z,*RK_y1,*RK_y2,*RK_y3,*RK_w,h_RK;
Spln **SQ,**SS; Form *Uravn,*Dseta,**Af,*Bf;
Magazine *NamesKadr,*NamesBMP,*NamesInclude,*NamesTAB,
 *NamesSpline,*NamesFurie,*MTabAdd,*Sravn,*Sovm,*Names,*Mangles;
//---------------------------------------------------------------------------
double FuncIntg(double x){
 VArg->Val=x;
 x=Value(FIntg);
 return x;
}
//---------------------------------------------------------------------------
void PrintVect(wchar_t *File,wchar_t *Comm,wchar_t *NameVect,int N,double *V){
 FILE *F=_wfopen(File,L"a"); wchar_t Name[256];
 swprintf(Name,L"\n%s",Comm); fputws(Name,F);
 for(int i=0;i<N;i++)
  fwprintf(F,L"%s[%d]=%0.6f\t",NameVect,i,V[i]);
 fclose(F);
}
//---------------------------------------------------------------------------
void PrintMatr(wchar_t *File,wchar_t *Comm,wchar_t *NameMatr,int N,double **M){
 FILE *F=_wfopen(File,L"a"); wchar_t Name[256];
 swprintf(Name,L"\n%s",Comm); fputws(Name,F);
 for(int i=0;i<N;i++){
  for(int j=0;j<N;j++)
   fwprintf(F,L"%s[%d][%d]=%0.6f\t",NameMatr,i,j,M[i][j]);
  fputwc('\n',F);
 }
 fclose(F);
}
//---------------------------------------------------------------------------
void CalcUravnCOM(double *qk,double *uv,double *rC,double *r6R,double *r6L,
 double *Ss,double *Cm,double *Ms,double *Ur);
void CalcUqCOM(double *qk,double *Ss,double *Cm,double *Ms,double **Uq);
//---------------------------------------------------------------------------
int NewtonOK(double t){
 static Rez=1; int i,j; Form *afi; Cord *q; Root *I; bool Ex,ForPO=false,OUT_=false;
 double *y,*dy,*ai,*bi,D;
// double qkb[]={-0.00836, 0.578968, 0.883845, 0.304876, 0.0,  0.00836,
//                0.00836, 0.583037, 0.891762, 0.304665, 0.0, -0.00836};
 double qkb[]={ 0.0, 0.56, 1, 0.485, 0.0,  0.013,
                0.0, 0.56, 1, 0.485, 0.0, -0.013};
// for(int i=0;i<12;i++) YK[i]=q0[i];//начальное приближение
 if(!Rez) return -1;
 NoCalc=false;
 L->Time->Val=t;
// for(q=L->K,y=YK;q;q=q->Sled) q->V->Val=*y++;
 for(int k=0;k<MAXITER;k++){ //swprintf(Inf,L"%d-я итерация\n",k);
  ClearVar(); //PRINTVARS=true;
  for(q=L->K,y=YK;q;q=q->Sled)
   q->V->Val=*y++;
  if(ForPO){
   Vary *V; double rC[3],r6R[3],r6L[3];
   if(FindVary(L"XC",&V)) rC[0]=Val(V->Znach);
   if(FindVary(L"YC",&V)) rC[1]=Val(V->Znach);
   if(FindVary(L"ZC",&V)) rC[2]=Val(V->Znach);
   if(FindVary(L"XR6",&V)) r6R[0]=Val(V->Znach);
   if(FindVary(L"YR6",&V)) r6R[1]=Val(V->Znach);
   if(FindVary(L"ZR6",&V)) r6R[2]=Val(V->Znach);
   if(FindVary(L"XL6",&V)) r6L[0]=Val(V->Znach);
   if(FindVary(L"YL6",&V)) r6L[1]=Val(V->Znach);
   if(FindVary(L"ZL6",&V)) r6L[2]=Val(V->Znach);
//   CalcUravnCOM(YK,NULL,rC,r6R,r6L,Ss,Cm,Ms,VB);
//   CalcUqCOM(YK,Ss,Cm,Ms,MA);
  }
  else{
   for(i=0,bi=VB;i<N;i++){
//PRNT_V=_wfopen(L"ZnachVars.txt",L"w");
    *bi++=Val(Uravn[i]);
//fclose(PRNT_V);
   }
   for(i=0;i<N;i++)
    for(j=0,ai=MA[i],afi=Af[i];j<N;j++)
     *ai++=Val(*afi++);
  }
  for(i=0,bi=VB,Ex=true;i<N;i++)
   if(fabs(*bi++)>Eps){ Ex=false; break; }
  if(Ex) goto M;
  Rez=Crout(N,MA,dYK,VB,1e-6);
  if(!Rez){
   if(!SLE) SLE=new TStringList;
   if(SLE){
//   if(ERRFILE=_wfopen(ErrFile,L"a")){
	wcscpy(Serr,L"A="); SLE->Add(Serr); Serr[0]='\0'; serr=Serr;
//	fputws(L"A=\n",ERRFILE);
	for(int i=0;i<N;i++){
//	 for(int j=0;j<N;j++) fwprintf(ERRFILE,L"%g\t",MA[i][j]);
	 for(int j=0;j<N;j++) serr+=swprintf(serr,L"%g\t",MA[i][j]);
//	 fputwc('\n',ERRFILE);
	 SLE->Add(Serr); Serr[0]='\0'; serr=Serr;
	}
	SLE->SaveToFile(ErrFile); Serr[0]='\0'; serr=Serr;
//    fclose(ERRFILE);
   }
   Application->MessageBox(L"Система уравнений несовместна!\n"
	L"См. закладку ОШИБКИ",L"Ошибка!",MB_OK);
   NoCalc=true;
   return -1;
  }
  for(i=0,y=YK,dy=dYK;i<N;i++,y++)
   *y-=*dy++;
  for(i=0,y=YK,dy=dYK;i<N;i++,y++,dy++){
   if(fabs(*dy)>Eps){
    if(fabs(*y)<Eps) break;
    else
     if(fabs(*dy)>Eps*fabs(*y)) break;
  }}
M:if(i==N){
   for(j=0,q=L->K;j<N;j++,q=q->Sled){
    if(!BORDER||(YK[j]>=YKmin[j])&&(YK[j]<=YKmax[j])) YKans[j]=YK[j];
    else{
     if(YK[j]<YKmin[j])
      YKans[j]=YKmin[j];
     else if(YK[j]>YKmax[j])
      YKans[j]=YKmax[j];
     BORDER[j]=true; OUT_=true;
   }}
   if(ForPO&&(t==Tn)&&OUT_) break;
   return k+1;
 }}
 if(ForPO&&(t==Tn)&&OUT_){
  for(int i=0;i<12;i++) YK[i]=qkb[i];//начальное приближение
  Rez=NewtonOK(t);
  if(Rez>0) return Rez;
 }
 swprintf(Inf,L"Решение за %d итераций не найдено!",MAXITER);
 Application->MessageBox(Inf,L"Ошибка!",MB_OK);
 NoCalc=true; Rez=0;
 return -MAXITER;
}
//---------------------------------------------------------------------------
double NormAngle(double d){
 double P2=2.0*M_PI,k=d>=P2?floor(d/P2):d<=-P2?ceil(d/P2):0.0;
 return d-=k*P2;
}
//---------------------------------------------------------------------------
bool ReadRezFile(int Kt){ //надо ли загружаться из файла
 wchar_t FileRez[256],*s; FILE *F; struct _stat St; long DateRez;
 int Ktf,Kinst=KolInst(TextFromFile(WORKDIR,L"EditKiDyM",&SLK,111))+
  KolInst(TextFromFile(WORKDIR,L"EditKiDyM",&SLK,159));
 DateRez=0L; WriteRezFile=true;
 wcscpy(FileRez,InpFile);
 if(Kinst){
  if(!(s=wcsrchr(FileRez,'.'))) s=FileRez+wcslen(FileRez);
  swprintf(s,L"№%d.rez",Kinst+1);
 }
 else{
  if(s=wcsrchr(FileRez,'.')) wcscpy(s+1,L"rez"); else wcscat(FileRez,L".rez");
 }
 if(!_waccess(FileRez,0)){
  if(F=_wfopen(FileRez,L"rb")){
   fread(&DateRez,SzL,1,F); fread(&Ktf,SzI,1,F);
   fclose(F);
   if(Ktf==Kt){
    if(DateInp>DateRez){
     if(RENOVCALC){
      swprintf(Inf,L"Исходные данные изменены.\n"
       L"Если изменения внесены в модель, следует сделать новый расчет.\n"
       L"Если изменена часть данных, отвечающая за отображение результатов,\n"
       L"можно воспользоваться результатами уже полученными ранее.\n"
       L"Использовать старые результаты?");
      if(Application->MessageBox(Inf,L"Внимание!",MB_YESNO|MB_ICONQUESTION)==ID_YES)
       WriteRezFile=false;
    }}
    else WriteRezFile=false;
 }}}
 return !WriteRezFile;
}
//---------------------------------------------------------------------------
void SaveRezFile(int Kt){
 wchar_t FileRez[256],*s; FILE *F; struct _stat St; long DateInpFile;
 int K,i,Kinst=KolInst(TextFromFile(WORKDIR,L"EditKiDyM",&SLK,111))+
  KolInst(TextFromFile(WORKDIR,L"EditKiDyM",&SLK,159));
 _wstat(InpFile,&St); DateInpFile=St.st_mtime;
 wcscpy(FileRez,InpFile);
 if(Kinst){
  if(!(s=wcsrchr(FileRez,'.'))) s=FileRez+wcslen(FileRez);
  swprintf(s,L"№%d.rez",Kinst+1);
 }
 else{
  if(s=wcsrchr(FileRez,'.')) wcscpy(s+1,L"rez"); else wcscat(FileRez,L".rez");
 }
 if(F=_wfopen(FileRez,L"wb")){
  fwrite(&DateInpFile,SzL,1,F); fwrite(&Kt,SzI,1,F);
  for(K=0;K<=Kt;K++){
   for(i=0;i<N;i++) YK[i]=Q[i][K];
   fwrite(YK,SzD,N,F);
  }
  fclose(F);
}}
//---------------------------------------------------------------------------
void LoadRezFile(int Kt){
//Формат файла: long DateInp; int Kt; double(Kt*(Kq+Kz+Kw));
 wchar_t FileRez[256],*s; FILE *F; long DateInpFile;
 int K,i,Kinst=KolInst(TextFromFile(WORKDIR,L"EditKiDyM",&SLK,111))+
  KolInst(TextFromFile(WORKDIR,L"EditKiDyM",&SLK,159));
 wcscpy(FileRez,InpFile);
 if(Kinst){
  if(!(s=wcsrchr(FileRez,'.'))) s=FileRez+wcslen(FileRez);
  swprintf(s,L"№%d.rez",Kinst+1);
 }
 else{
  if(s=wcsrchr(FileRez,'.')) wcscpy(s+1,L"rez"); else wcscat(FileRez,L".rez");
 }
 if(F=_wfopen(FileRez,L"rb")){
  fread(&DateInpFile,SzL,1,F); fread(&Kt,SzI,1,F);
  if(!Q){
   Q=(double **)calloc(N,SzV);
   for(i=0;i<N;i++)
    Q[i]=(double *)calloc(Kt+1,SzD);
  }
  for(K=0;K<=Kt;K++){
   Magazine *M; Cord *q; int i;
   fread(YK,SzD,N,F);
   for(q=L->K,i=0;i<N;q=q->Sled,i++)
    Q[i][K]=ALL_ANGLES||Mangles&&FindMagazine(Mangles,q->V->Name,&M)?
            NormAngle(YK[i]):YK[i];
  }
  fclose(F);
}}
//---------------------------------------------------------------------------
void CalcR(double t){
 static Rez=1; int i,j; Form *afi; Cord *q;
 double *y,*ai,*bi;
 if(!Rez) return;
 NoCalc=false;
 L->Time->Val=t;
 ClearVar();
 for(q=L->K,y=YK;q;q=q->Sled) q->V->Val=*y++;
 for(i=0,bi=VB;i<N;i++){
  *bi++=-Val(Bf[i]);
  for(j=0,ai=MA[i],afi=Af[i];j<N;j++)
   *ai++=Val(*afi++);
 }
 Rez=SLAU(N,MA,VB,RK_z,false);
 if(!Rez){
  if(!SLE) SLE=new TStringList;
  if(SLE){
//   if(ERRFILE=_wfopen(ErrFile,L"a")){
   wcscpy(Serr,L"A="); SLE->Add(Serr); Serr[0]='\0'; serr=Serr;
//	fputws(L"A=\n",ERRFILE);
   for(int i=0;i<N;i++){
//	 for(int j=0;j<N;j++) fwprintf(ERRFILE,L"%g\t",MA[i][j]);
	for(int j=0;j<N;j++) serr+=swprintf(serr,L"%g\t",MA[i][j]);
//	 fputwc('\n',ERRFILE);
	SLE->Add(Serr); Serr[0]='\0'; serr=Serr;
   }
   SLE->SaveToFile(ErrFile); Serr[0]='\0'; serr=Serr;
//    fclose(ERRFILE);
  }
  Application->MessageBox(L"Система уравнений несовместна!\n"
   L"См. закладку ОШИБКИ",L"Ошибка!",MB_OK);
  NoCalc=true;
 }
 NoCalc=false;
}
//---------------------------------------------------------------------------
bool RUNGE_Ex(double x,double *y,int n,void (*f)(double x),double eps,double eta,
  double xfin,double *yfin);
bool Adams(int &Nom,double x,double *y,void (*f)(double x),double Eps,
  double xfin,double *yfin);
//---------------------------------------------------------------------------
Vary *vCX,*vCY,*vCZ;
//---------------------------------------------------------------------------
void CalcVelos(double t){
 L->Time->Val=t; ClearVar();
 RK_z[0]=Val(vCX->Znach);
 RK_z[1]=Val(vCY->Znach);
 RK_z[3]=Val(vCZ->Znach);
}
//---------------------------------------------------------------------------
bool InverseKinByVelos(int Wg,int &Kmin,int &Kmax,double dt){
 double t=Tn,*yk,rC[3],rCK[3],RezTime; int i; Cord *q;
 Vary *XC,*YC,*ZC; Root *I; Parm *P; clock_t timer,DeltaTime;
//Исходим из заданной скорости общего центра масс, заданной своими проекциями,
//интегрируем их, далее решаем обычную обратную кинематику по СОМ
 if(!(I=FindInstLast(L"КООРДИНАТЫ ЦЕНТРА МАСС"))) return false;
 L->Time->P=0; L->Time->Val=t; ClearVar();
 P=I->First; if(P->Nam){ XC=TakeVary(P->Nam); rCK[0]=Val(XC->Znach); }
 P=P->Sled;  if(P->Nam){ YC=TakeVary(P->Nam); rCK[1]=Val(YC->Znach); }
 P=P->Sled;  if(P->Nam){ ZC=TakeVary(P->Nam); rCK[2]=Val(ZC->Znach); }
 if(!(I=FindInstLast(L"ПРОЕКЦИИ СКОРОСТИ ЦМ"))) return false;
 P=I->First; if(P->Nam) vCX=TakeVary(P->Nam);
 P=P->Sled;  if(P->Nam) vCY=TakeVary(P->Nam);
 P=P->Sled;  if(P->Nam) vCZ=TakeVary(P->Nam);
 RK_z=(double *)calloc(3,SzD);  RK_y1=(double *)calloc(3,SzD);
 RK_y2=(double *)calloc(3,SzD); RK_y3=(double *)calloc(3,SzD);
 RK_w=(double *)calloc(3,SzD);  Eps=1e-4; h_RK=-Eps;
 timer=clock();
 for(int K=1;K<Wg;K++){
  for(i=0;i<3;i++) rC[i]=rCK[i];
//  Adams(K,t,rC,CalcVelos,dt,t+dt,rCK);
  RUNGE_Ex(t,rC,3,CalcVelos,Eps,Eps,t+dt,rCK);
  for(i=0;i<3;i++) rC[i]=rCK[i];
  t+=dt; if(t>Tk) t=Tk;
  for(i=0;i<N;i++) Y[i]=YK[i];
  i=NewtonOK(t);
  if(i>Kmax) Kmax=i; if(i<Kmin) Kmin=i;
  FormKinema->CGauge->Progress++;
  if(NoCalc){ double *y=Y;//при неудаче возращаемся к старым значениям
   IsNoCalc=true;
   t-=dt; L->Time->Val=t;
   for(i=0;i<N;i++) YK[i]=Y[i];
   for(q=L->K,i=0;q;q=q->Sled,i++)
    q->V->Val=*y++;
   break;
  }
  else{//при удаче сохраняем новые значения
   for(i=0;i<N;i++){
    Q[i][K]=YKans[i];
    double D=(Q[i][K]-Q[i][K-1])/dt;
    if(!BORDER||(D>=YKmin[i+N])&&(D<=YKmax[i+N])) S[i][K]=D;
    else{
     if(D<YKmin[i+N])
      S[i][K]=YKmin[i+N];
     else if(D>YKmax[i+N])
      S[i][K]=YKmax[i+N];
     BORDER[i+N]=true;
   }}
   L->Time->Val=t;
   for(yk=YK,q=L->K,i=0;q;q=q->Sled,i++)
    q->V->Val=*yk++;
 }}
 DeltaTime=(clock()-timer);
 RezTime=(double)DeltaTime/CLK_TCK;
 free(RK_z); free(RK_y1); free(RK_y2); free(RK_y3); free(RK_w);
 return true;
}
//---------------------------------------------------------------------------
bool InverseKinByVelos(int Wg,double dt){
 double t=Tn,*yk; int i; Cord *q;
 RK_z=(double *)calloc(N,SzD);  RK_y1=(double *)calloc(N,SzD);
 RK_y2=(double *)calloc(N,SzD); RK_y3=(double *)calloc(N,SzD);
 RK_w=(double *)calloc(N,SzD);  h_RK=-Eps;
 for(int K=1;K<Wg;K++){
  for(i=0;i<N;i++) Y[i]=YK[i];
  RUNGE_Ex(t,Y,N,CalcR,Eps,Eps,t+dt,YK);
  t+=dt; if(t>Tk) t=Tk;
  FormKinema->CGauge->Progress++;
  if(NoCalc){ double *y=Y;//при неудаче возращаемся к старым значениям
   IsNoCalc=true;
   t-=dt; L->Time->Val=t;
   for(i=0;i<N;i++) YK[i]=Y[i];
   for(q=L->K,i=0;q;q=q->Sled,i++)
    q->V->Val=*y++;
   break;
  }
  else{//при удаче сохраняем новые значения
   for(i=0,q=L->K;i<N;i++,q=q->Sled){
    if(!BORDER||(YK[i]>=YKmin[i])&&(YK[i]<=YKmax[i])) YKans[i]=YK[i];
    else{
     if(YK[i]<YKmin[i])
      YKans[i]=YKmin[i];
     else if(YK[i]>YKmax[i])
      YKans[i]=YKmax[i];
     BORDER[i]=true;
   }}
   for(i=0;i<N;i++){
    Q[i][K]=YKans[i];
    double D=(Q[i][K]-Q[i][K-1])/dt;
    if(!BORDER||(D>=YKmin[i+N])&&(D<=YKmax[i+N])) S[i][K]=D;
    else{
     if(D<YKmin[i+N])
      S[i][K]=YKmin[i+N];
     else if(D>YKmax[i+N])
      S[i][K]=YKmax[i+N];
     BORDER[i+N]=true;
   }}
   L->Time->Val=t;
   for(yk=YK,q=L->K,i=0;q;q=q->Sled,i++)
    q->V->Val=*yk++;
 }}
 free(RK_z); free(RK_y1); free(RK_y2); free(RK_y3); free(RK_w);
 return true;
}
//---------------------------------------------------------------------------
bool InverseKinOfCoord(int &Wg,int &Kmin,int Kmax,double dt){
 double t=Tn,*yk,RezTime; int i; Cord *q; clock_t timer,DeltaTime;
 timer=clock();
 for(int K=1;K<Wg;K++){
  t+=dt; if(t>Tk) t=Tk;
  for(i=0;i<N;i++) Y[i]=YK[i];
  i=NewtonOK(t); KIter+=i;
  if(i>Kmax) Kmax=i; if(i<Kmin) Kmin=i;
  FormKinema->CGauge->Progress++;
  if(NoCalc){ double *y=Y;//при неудаче возращаемся к старым значениям
   IsNoCalc=true;
   t-=dt; L->Time->Val=t;
   for(i=0;i<N;i++) YK[i]=Y[i];
   for(q=L->K,i=0;q;q=q->Sled,i++)
    q->V->Val=*y++;
   break;
  }
  else{ Magazine *M;//при удаче сохраняем новые значения
   for(q=L->K,i=0;i<N;q=q->Sled,i++){
    Q[i][K]=ALL_ANGLES||Mangles&&FindMagazine(Mangles,q->V->Name,&M)?
            NormAngle(YKans[i]):YKans[i];
//     if(FindInstLast(L"ПРОИЗВОДНЫЕ ОБОБЩЕННЫХ КООРДИНАТ")){
     double D=(Q[i][K]-Q[i][K-1])/dt;
     if(!BORDER||(D>=YKmin[i+N])&&(D<=YKmax[i+N])) S[i][K]=D;
     else{
      if(D<YKmin[i+N])
       S[i][K]=YKmin[i+N];
      else if(D>YKmax[i+N])
       S[i][K]=YKmax[i+N];
      BORDER[i+N]=true;
     }
//     }
   }
   L->Time->Val=t;
   for(yk=YK,q=L->K,i=0;q;q=q->Sled,i++)
    q->V->Val=*yk++;
 }}
 DeltaTime=(clock()-timer);
 RezTime=(double)DeltaTime/CLK_TCK;
 return true;
}
//---------------------------------------------------------------------------
void CalcQSplns(){
 double dt,t,v,*yk=YK,y; int i,j,K,Kmin=100,Kmax=0,Ps; Cord *q;
 wchar_t SBold[256]; Magazine *M;
 if(!INVERSE||LAGRANGE) return;
 IsNoCalc=false; Min[0]=Slv[0]=Tn; timer = clock();
 FormKinema->CGauge->ForeColor=FormKinema->ColorGrid;
 FormKinema->CGauge->Progress=0;
 MSt[0]=(Tk-Tn)/(KP[0]-1);
 for(Ps=0;MSt[0]>10.0;Ps++) MSt[0]/=10.0;
 for(;MSt[0]<1.0;Ps--) MSt[0]*=10.0;
 for(int j=1;j<Kmg;j++) if(MSt[0]<=MG[j]){ MSt[0]=MG[j]; break; }
 for(int j=abs(Ps);j--;) if(Ps>0) MSt[0]*=10; else MSt[0]/=10;
 MstT[0]=MSt[0]/Sg; MSt[0]/=dS[0];
 Wg=1+Round((Tk-Tn)/MSt[0]);
 FormKinema->CGauge->MaxValue=Wg; FormKinema->CGauge->Visible=true;
 dt=(Tk-Tn)/(Wg-1); t=Tn;
 wcscpy(SBold,FormKinema->StatusBar->SimpleText.c_str());
 if(YK&&::ReadRezFile(Wg)){
  FormKinema->StatusBar->SimpleText=L"Загрузка результатов";
  ::LoadRezFile(Wg); //FirstCalcGrf=false;
 }
 else{ double T;
  FormKinema->StatusBar->SimpleText=L"Расчет обратной задачи кинематики";
//начальное приближение
  for(q=L->K,i=0;q;q=q->Sled,i++)
   *yk++=q->V->Val=q0[i];
//выделим память под массивы Q, Y и S
  Q=(double **)calloc(N,SzV); Y=(double *)calloc(N,SzD);
  for(i=0;i<N;i++) Q[i]=(double *)calloc(Wg,SzD);
//  if(FindInstLast(L"ПРОИЗВОДНЫЕ ОБОБЩЕННЫХ КООРДИНАТ")){
   S=(double **)calloc(N,SzV);
   for(i=0;i<N;i++) S[i]=(double *)calloc(Wg,SzD);
//  }
//  if(!VELOKIN){
//Уточним начальные условия
   Kmax=Kmin=KIter=NewtonOK(t);
   for(q=L->K,i=0;i<N;q=q->Sled,i++)
    Q[i][0]=ALL_ANGLES||Mangles&&FindMagazine(Mangles,q->V->Name,&M)?
            NormAngle(YK[i]):YK[i];
   if(NoCalc) return;
//  }
  FormKinema->CGauge->Progress++;
  T=clock();
  switch(VELOKIN){
   case 0:
    InverseKinOfCoord(Wg,Kmin,Kmax,dt);
    break;
   case 1:
    InverseKinByVelos(Wg,dt);
    break;
   case 2:
    InverseKinByVelos(Wg,Kmin,Kmax,dt);
  }
  T=(clock()-T)/CLK_TCK;
//  if(FindInstLast(L"ПРОИЗВОДНЫЕ ОБОБЩЕННЫХ КООРДИНАТ"))
   for(i=0;i<N;i++) S[i][0]=S[i][1];
  if(BORDER){
   for(q=L->K,i=0;i<N;q=q->Sled,i++){
    if(BORDER[i]){ wchar_t *inf=Inf;
     inf+=swprintf(inf,
      L"для обобщенных координат:\n%s",q->V->Name);
     if(q->Sled){
      for(q=q->Sled,j=i+1;q;q=q->Sled,j++)
       if(BORDER[j]) inf+=swprintf(inf,L", %s",q->V->Name);
     }
     Application->MessageBoxA(Inf,L"Нарушены границы",MB_OK);
     break;
   }}
/*
   for(q=L->VK,i=0;q;q=q->Sled,i++){
    if(BORDER[i+N]){ wchar_t *inf=Inf;
     inf+=swprintf(inf,
      L"для обобщенных скоростей:\n%s",q->V->Name);
     if(q->Sled){
	  for(q=q->Sled,j=i+1;q;q=q->Sled,j++)
	   if(BORDER[j+N]) inf+=swprintf(inf,L", %s",q->V->Name);
	 }
	 Application->MessageBoxA(Inf,L"Нарушены границы",MB_OK);
	 break;
   }}
*/
  }
  DeltaTime=(clock()-timer)/CLK_TCK;
//  if(HTMFILE=_wfopen(HtmFile,L"r+")){
  if(SLH){
   int EndInd=SLH->Count-1; String SEnd=SLH->Strings[EndInd];
//   if(!fseek(HTMFILE,-Lpodv,2)){
   if(DeltaTime>3600)
	shtm+=swprintf(shtm,L"<h4 align=\"left\"><font color=\"#009900\">"
"%d. Время расчета обратной задачи кинематики: %d час. %d мин. %d.%d",
	 PUNKT++,DeltaTime/3600,(DeltaTime%3600)/60,((DeltaTime%3600)%60),
	 (int)Round(1000*(T-floor(T))));
//	 fwprintf(HTMFILE,L"<h4 align=\"left\"><font color=\"#009900\">"
//"%d. Время расчета обратной задачи кинематики: %d час. %d мин. %d.%d",
//	  PUNKT++,DeltaTime/3600,(DeltaTime%3600)/60,((DeltaTime%3600)%60),
//	  (int)Round(1000*(T-floor(T))));
   else if(DeltaTime>60)
	shtm+=swprintf(shtm,L"<h4 align=\"left\"><font color=\"#009900\">"
	 L"%d. Время расчета обратной задачи кинематики: %d мин. %d.%d",
	 PUNKT++,DeltaTime/60,DeltaTime%60,(int)Round(1000*(T-floor(T))));
//	 fwprintf(HTMFILE,L"<h4 align=\"left\"><font color=\"#009900\">"
//	  L"%d. Время расчета обратной задачи кинематики: %d мин. %d.%d",
//	  PUNKT++,DeltaTime/60,DeltaTime%60,(int)Round(1000*(T-floor(T))));
   else{ double D=T-floor(T); int d=Round(1000*(T-floor(T)));
	shtm+=swprintf(shtm,L"<h4 align=\"left\"><font color=\"#009900\">"
	 L"%d. Время расчета обратной задачи кинематики: %d.%d",
	 PUNKT++,DeltaTime,(int)Round(1000*(T-floor(T))));
//	 fwprintf(HTMFILE,L"<h4 align=\"left\"><font color=\"#009900\">"
//	  L"%d. Время расчета обратной задачи кинематики: %d.%d",
//	  PUNKT++,DeltaTime,(int)Round(1000*(T-floor(T))));
   }
   wcscpy(shtm,L" сек.<br>");
   SLH->Add(Shtm); shtm=Shtm;
   shtm+=swprintf(shtm,L"&nbsp&nbsp&nbsp&nbsp"
	L"Минимальное число итераций составило:&nbsp&nbsp%d, "
	L"максимальное:&nbsp&nbsp%d, общее:&nbsp&nbsp%d</font></h4>",
	Kmin,Kmax,KIter);
   SLH->Add(Shtm); shtm=Shtm;
   SLH->Add(SEnd);
   SLH->SaveToFile(HtmFile);
//	fwprintf(HTMFILE,
//	 L" сек.\n<br>&nbsp&nbsp&nbsp&nbsp"
//	 L"Минимальное число итераций составило:&nbsp&nbsp%d, L"
//			  L"максимальное:&nbsp&nbsp%d, общее:&nbsp&nbsp%d</font></h4>",
//	 Kmin,Kmax,KIter);
//	fwprintf(HTMFILE,
//	 L"<hr><font face=\"Georgia, Times New Roman, Times, serif\""
//	 L" color=\"#326464\">&copy; %s, %d<br></font><br>\n",Autor,NYear+1900)+1;
//	fclose(HTMFILE);
 }}//}
 FormKinema->StatusBar->SimpleText=SBold;
 wchar_t NameArr[256],NameSpln[256]; Form Ft;
 FormKinema->CGauge->Progress=0; FormKinema->CGauge->Visible=false;
 SQ=(Spln **)calloc(N,SzV);
 Ft.V=L->Time;
 for(i=0;i<N;i++){
  swprintf(NameSpln,L"%s",Dseta[i].V->Name);
  swprintf(NameArr,L"%s_arr",Dseta[i].V->Name);
  Arra *Arr=TakeArra(NameArr); Arr->N=2*Wg;
  Arr->A=(double *)calloc(Arr->N,SzD); t=Tn;
  for(K=0;K<Wg;K++){
   Arr->A[2*K]=t; t+=dt; Arr->A[2*K+1]=Q[i][K];
  }
  SQ[i]=TakeSpln(1,Arr,Ft);
 }
// if(FindInstLast(L"ПРОИЗВОДНЫЕ ОБОБЩЕННЫХ КООРДИНАТ")){
  SS=(Spln **)calloc(N,SzV);
  for(i=0;i<N;i++){ Vary *V;
   swprintf(NameSpln,L"%s'%s",Dseta[i].V->Name,L->Time->Name);
   swprintf(NameArr,L"%s_t_arr",Dseta[i].V->Name);
   Arra *Arr=TakeArra(NameArr); Arr->N=2*Wg;
   Arr->A=(double *)calloc(Arr->N,SzD); t=Tn+dt;
   for(K=1;K<Wg;K++){
    double D=(Q[i][K]-Q[i][K-1])/dt;
    Arr->A[2*K]=t;
    t+=dt;
    if(BORDER){
     if(D<YKmin[i+N])
      D=YKmin[i+N];
     else if(D>YKmax[i+N])
      D=YKmax[i+N];
    }
    Arr->A[2*K+1]=D;
   }
   Arr->A[0]=Tn; Arr->A[1]=Arr->A[3];
   SS[i]=TakeSpln(1,Arr,Ft);
   V=TakeVary(NameSpln);
   V->Znach.P=SS[i];
  }
// }
 if(YK&&::WriteRezFile) ::SaveRezFile(Wg);
}
//---------------------------------------------------------------------------
bool Diagnos(TCGauge *CGauge,TStatusBar *StatusBar,TTabControl *TC);
//---------------------------------------------------------------------------
bool __fastcall EndInt(){ return false; }
//---------------------------------------------------------------------------
double Func(double X){ Form F; Cord *q; int i;
 NoCalc=false; L->Time->Val=X;
 F.V=Vp;
 if(INVERSE&&!LAGRANGE){
  for(q=L->K,i=0;q;q=q->Sled,i++)
   q->V->Val=Spl(SQ[i],X);
  for(q=L->VK,i=0;q;q=q->Sled,i++)
   q->V->Val=Spl(SS[i],X);
 }
 X=Val(F);
 if(NoCalc&&!IsNoCalc) IsNoCalc=true;
 return X;
}
//---------------------------------------------------------------------------
void Funcs(double t,Magazine *Names,double *Y){
 Cord *q; int i; Magazine *m;
 NoCalc=false;
 (ArgTime ? L->Time : VgX)->Val=t;
// if(ArgTime) L->Time->Val=t; else VgX->Val=t; L->Time->Val=t;
 Vary *Vp; Form FVp;
 ClearVar();
 if(INVERSE&&!LAGRANGE){
  for(q=L->K,i=0;q;q=q->Sled,i++)
   q->V->Val=Spl(SQ[i],t);
  for(q=L->VK,i=0;q;q=q->Sled,i++)
   q->V->Val=Spl(SS[i],t);
 }
 for(m=Names,i=0;m;m=m->Sled,i++){
  FindVary(m->S,&Vp); FVp.V=Vp;
  *Y=Val(FVp);
  Y++;
 }
 if(NoCalc&&!IsNoCalc) IsNoCalc=true;
}
//---------------------------------------------------------------------------
void FuncsQ(double t,Magazine *Names,double *Y,bool First){
 NoCalc=false; L->Time->Val=t;
 Vary *Vp; Form FVp; int i; static int K,N,Kt,No=0; Magazine *m; Cord *q;
 ClearVar();
 if(INVERSE&&!LAGRANGE){
  First=true;
  for(q=L->K,i=0;q;q=q->Sled,i++)
   q->V->Val=Spl(SQ[i],t);
  for(q=L->VK,i=0;q;q=q->Sled,i++)
   q->V->Val=Spl(SS[i],t);
 }
 if(t==Tn){
  K=0;
  if(First){
   if(No) No=N;
   Kt=TMult*Kkadr/1000; N=KolElem(Names);
   if(Qmf){
    if(N>No){
     Qmf=(double **)realloc(Qmf,N*SzV);
	 for(i=0;i<No;i++) Qmf[i]=(double *)realloc(Qmf[i],(Kt+1)*SzD);
	 for(;i<N;i++) Qmf[i]=(double *)calloc(Kt+1,SzD);
   }}
   else{
    Qmf=(double **)calloc(N,SzV);
    for(i=0;i<N;i++) Qmf[i]=(double *)calloc(Kt+1,SzD);
   }
   No=N;
 }}
 if(First){
  for(m=Names,i=0;m;m=m->Sled,i++){
   FindVary(m->S,&Vp); FVp.V=Vp; *Y++=Qmf[i][K]=Val(FVp);
  }
  if(NoCalc&&!IsNoCalc) IsNoCalc=true;
 }
 else{
  for(m=Names,i=0;m;m=m->Sled,i++,Y++){
   *Y=Qmf[i][K]; NoCalc=IsNoCalc=false;
   if(FindVary(m->S,&Vp)){ Vp->Val=*Y; Vp->P=-1; }
 }}
 K++;
}
//---------------------------------------------------------------------------
double FuncX(int P,double T){
 NoCalc=false;
 T=Val(P?VgXt:VgX->Znach);
 if(NoCalc&&!IsNoCalc) IsNoCalc=true;
 return T;
}
//---------------------------------------------------------------------------
double FuncY(int P,double T){
 NoCalc=false;
 T=Val(P?VgYt:VgY->Znach);
 if(NoCalc&&!IsNoCalc) IsNoCalc=true;
 return T;
}
//---------------------------------------------------------------------------
void __fastcall TFormKinema::FuncXYZ(double t,double &x,double &y,double &z){
 static bool First; int Kt=Hg>Wg?Hg:Wg;
 Nt=Round((t-Tn)*(Kt-1)/(Tk-Tn));
 if(t==Tn){
  if(!Qxyz){
   Qxyz=(double **)calloc(Kt,SzD);
   for(int i=0;i<Kt;i++) Qxyz[i]=(double *)calloc(3,SzD);
   First=true;
  }
  else First=false;
 }
 if(First){
  NoCalc=false; ClearVar(); L->Time->Val=t;
  x=Val(VgX->Znach); y=Val(VgY->Znach); z=Val(VgZ->Znach);
  Qxyz[Nt][0]=x; Qxyz[Nt][1]=y; Qxyz[Nt][2]=z;
  if(NoCalc&&!IsNoCalc) IsNoCalc=true;
 }
 else{
  x=Qxyz[Nt][0]; y=Qxyz[Nt][1]; z=Qxyz[Nt][2];
}}
//---------------------------------------------------------------------------
double __fastcall TFormKinema::FuncXYZ(double x,double y){
 NoCalc=false; ClearVar();
 VgX->Val=x; if(VgX->P==1) VgX->P=-1;
 VgY->Val=y; if(VgY->P==1) VgY->P=-1;
 return Val(VgZ->Znach);
}
//---------------------------------------------------------------------------
Form ReadForm(int Prio,TRichEdit *RE);
Root *Tv,*OK;
int Initialize(void);
//---------------------------------------------------------------------------
bool DiagnosTabl(Root *I){
//Таблица:=BaseData20.06.txt,t,N,
// epsilon°$R,nu°$R,beta°$R,gamma°$R,teta°$R,alfa°$R,epsilon°$L,nu°$L,beta°$L,gamma°$L,teta°$L,alfa°$L,
// X@t6R,Y@t6R,Z@t6R,X@t6L,Y@t6L,Z@t6L,X@AR,Y@AR,Z@AR,X@CR,Y@CR,Z@CR,X@AL,Y@AL,Z@AL,X@CL,Y@CL,Z@CL;
 wchar_t File[256],*inf; int N,i; Parm *P; Vary *V; Magazine *M=NULL; double t;
 if(I->First->Nam){
  swprintf(File,L"%s\\%s",CurrDir,I->First->Nam);
  P=I->First->Sled;
 }
 else{
  swprintf(Inf,L"Первым параметром в инструкции\n\"%s\"\nдолжно быть имя файла",I->Name);
  Application->MessageBoxA(Inf,I->Name,MB_OK);
  return TABLTOFILE=false;
 }
 if(!P->Nam) t=P->Val; else if(P->Nam&&FindVary(P->Nam,&V)) t=Value(V->Znach);
 if(t<Tn||t>Tk){
  swprintf(Inf,L"Вторым параметром в инструкции\n\"%s\"\nдолжно быть\n"
   L"значение момента времени, для которого надо сформировать таблицу значений",
   I->Name);
  Application->MessageBoxA(Inf,I->Name,MB_OK);
  return TABLTOFILE=false;
 }
 P=P->Sled;
 if(!P->Nam||!FindVary(P->Nam,&V)){
  swprintf(Inf,L"Третьим параметром в инструкции\n\"%s\"\nдолжен быть\n"
   L"номер эксперимента в файле",I->Name);
  Application->MessageBoxA(Inf,I->Name,MB_OK);
  return TABLTOFILE=false;
 }
 N=Round(Value(V->Znach));
 for(P=P->Sled,i=3;P;P=P->Sled,i++){
  if(!P->Nam||!FindVary(P->Nam,&V)){
   swprintf(Inf,L"%d",i);
   TakeMagazine(&M,Inf);
 }}
 if(M){
  inf+=swprintf(inf=Inf,L"Параметрами, начиная с четвертого,\nв инструкции\n\"%s\"\n"
   L"должны быть имена переменных,\nчто нарушается для номер%s\n",
   I->Name,M->Sled?"ов":"а");
  for(Magazine *m=M;m;m=m->Sled)
   inf+=m->Sled?swprintf(inf,L"%s, ",m->S):swprintf(inf,L"%s;");
  Application->MessageBoxA(Inf,I->Name,MB_OK);
  return TABLTOFILE=false;
 }
 return TABLTOFILE=true;
}
//---------------------------------------------------------------------------
bool DiagnosOK(Root *I){
//ОБОБЩЕННЫЕ КООРДИНАТЫ В ФАЙЛ := OK_01.08.txt,tn,N;
 wchar_t File[256],*inf; int N,i; Parm *P; Vary *V; Magazine *M=NULL; double t;
 if(I->First->Nam){
  swprintf(File,L"%s\\%s",CurrDir,I->First->Nam);
  P=I->First->Sled;
 }
 else{
  swprintf(Inf,L"Первым параметром в инструкции\n\"%s\"\nдолжно быть имя файла",I->Name);
  Application->MessageBoxA(Inf,I->Name,MB_OK);
  return OKTOFILE=false;
 }
 if(!P->Nam) t=P->Val; else if(P->Nam&&FindVary(P->Nam,&V)) t=Value(V->Znach);
 if(t<Tn||t>Tk){
  swprintf(Inf,L"Вторым параметром в инструкции\n\"%s\"\n"
   L"должно бытьзначение момента времени,\n"
   L"для которого надо сохранить значения обобщенных кооринат",I->Name);
  Application->MessageBoxA(Inf,I->Name,MB_OK);
  return OKTOFILE=false;
 }
 P=P->Sled;
 if(!P->Nam||!FindVary(P->Nam,&V)){
  swprintf(Inf,L"Третьим параметром в инструкции\n\"%s\"\n"
   L"должен быть номер расчета\n- номер строки с данными в файле",I->Name);
  Application->MessageBoxA(Inf,I->Name,MB_OK);
  return OKTOFILE=false;
 }
 N=Round(Value(V->Znach));
/* if(P->Sled){
  P=P->Sled;
  if(P->Nam){
   if(!wcscmp(LowToUpp(P->Nam),L"ВСЕ УГЛЫ"))
    ALL_ANGLES=true;
   else if(!wcscmp(LowToUpp(P->Nam),L"УГЛЫ")){
    for(P=P->R;P;P=P->Sled)
     if(P->Nam) TakeMagazine(&Mangles,P->Nam);
  }}
  else{
   swprintf(Inf,L"Четвертым параметром в инструкции\n\"%s\"\n"
    L"должно быть указание, все ли ОК - углы,\n"
    L"или дано перечисление углов среди ОК\n"
    L"предложением \"ВСЕ УГЛЫ\" или предложением\n"
    L"УГЛЫ(<список_имен_ОК_углов>)",I->Name);
   Application->MessageBoxA(Inf,I->Name,MB_OK);
   return OKTOFILE=false;
 }}*/
 return OKTOFILE=true;
}
//---------------------------------------------------------------------------
bool __fastcall TFormKinema::Kinemat(void){
 double Pr=Round(100/KColumn);
 int i,j,l,Lmax; Vary *V; Root *I; Parm *p;
 wchar_t *inf,SBold[256],*st,Lst[80],Time2[256],Time3[256];
 swprintf(Inf,L"%s: %s",HeadVersion[0]?HeadVersion:HeadWork,Autor);
 StatusBar->SimpleText=Inf;
 wcscpy(Lst,L"В исходных данных");
 if(L->I&&(Eps1<1e-13)){
  swprintf(Inf,
   L"Задана очень высокая точность для интегрирования (%g).\n"
   L"Это может привести к неоправданно большому времени интегрирования\n"
   L"Если настаиваете на этом значении - нажмите \"НЕТ\".\n"
   L"Если нажмете \"ДА\", то сейчас точность примет значение 1е-6.\n"
   L"Точность Вы можете изменить в НАСТРОЙКЕ.\n",Eps1);
  if(Application->MessageBox(Inf,L"Внимание!",MB_YESNO)==ID_YES){
   Eps=Eps1=Eta=Eps2=1e-6; ChangeOpt=true; OptToFile();
 }}
//дадим переменным интегрирования и аргументам функций поиска корней значения Nul
 for(Intg *I=L->I;I;I=I->Sled) I->x->Znach.C=Nul;
 for(Solu *c=L->C;c;c=c->Sled) c->x->Znach.C=Nul;
 if(L->K){ N=0;
  for(Cord *q=L->K;q;q=q->Sled) if(q->V->Znach.C==Pust) N++;
 }
 else N=Coord(L,CGauge,StatusBar);
 if(INVERSE){
  if(LAGRANGE){
//   Cord *K=NULL;
  }
  else{ Cord *K; Form *F; Parm *P;
   mToProc->Visible=true;
   for(K=L->K;K;K=K->Sled)
	TabControl->Tabs->Add(K->V->Name);
   Uravn=(Form *)calloc(N,SzV); Dseta=(Form *)calloc(N,SzV);
   MA=(double **)calloc(N,SzV); VB=(double *)calloc(N,SzD);
   Af=(Form **)calloc(N,SzV);   Bf=(Form *)calloc(N,SzV);
   YK=(double *)calloc(N,SzD);  YKans=(double *)calloc(N,SzD);
   q0=(double *)calloc(N,SzD);  dYK=(double *)calloc(N,SzD);
   if(!Uravn||!Dseta||!MA||!VB||!Af||!Bf||!YK||!q0||!dYK){
	if(q0) free(q0); if(Uravn) free(Uravn); if(Dseta) free(Dseta);
	if(MA) free(MA); if(VB) free(VB); if(Af) free(Af);
	if(Bf) free(Bf); if(YK) free(YK); if(dYK) free(dYK);
	swprintf(Inf,L"Нельзя выделить память для рабочих массивов!");
	Application->MessageBox(Inf,L"Ошибка!",MB_OK|MB_ICONERROR);
	Error(0,0,true); return false;
   }
   for(int i=0;i<N;i++) {
	MA[i]=(double *)calloc(N,SzD);
	Af[i]=(Form *)calloc(N,SzV);
   }
   for(K=L->K,F=Dseta;K;K=K->Sled) (F++)->V=K->V;
//   if(I=FindInstLast(L"ПРОИЗВОДНЫЕ ОБОБЩЕННЫХ КООРДИНАТ")){//создаем об.скорости
	for(K=L->K;K;K=K->Sled){
	 swprintf(Inf,L"%s'%s",K->V->Name,L->Time->Name);
	 TakeVeloCord(TakeVary(Inf));
	}
//   }
   if(I=FindInstLast(L"ОГРАНИЧЕНИЯ")){
	Cord *q; int i; Vary *V;
	YKmin=(double *)calloc(2*N,SzD);
	YKmax=(double *)calloc(2*N,SzD);
	BORDER=(bool *)calloc(2*N,sizeof(bool));
	for(q=L->K,i=0;q;q=q->Sled,i++){
	 if(FindParm(I->First,q->V->Name,&P)){
	  if(!P->R){
	   swprintf(Inf,L"В инструкции %s для обобщенной координаты %s\n"
		L"не указаны границы",I->Name,q->V->Name);
	   Application->MessageBox(Inf,L"Ошибка!",MB_OK|MB_ICONERROR);
	   Error(0,0,true); return false;
	  }
	  if(KolElem(P->R)!=2){
	   swprintf(Inf,L"В инструкции %s для обобщенной координаты %s\n"
		L"указана только одна граница",I->Name,q->V->Name);
	   Application->MessageBox(Inf,L"Ошибка!",MB_OK|MB_ICONERROR);
	   Error(0,0,true); return false;
	  }
	  if(P->R->Nam){
	   if(FindVary(P->R->Nam,&V)) YKmin[i]=Val(V->Znach);
	   else{
		swprintf(Inf,L"В инструкции %s для обобщенной координаты %s\n"
		 L"указана первая граница\nотсутствующей в исходных данных перменной - %s",
		 I->Name,q->V->Name,P->R->Nam);
		Application->MessageBox(Inf,L"Ошибка!",MB_OK|MB_ICONERROR);
		Error(0,0,true); return false;
	  }}
	  else YKmin[i]=P->R->Val;
	  if(P->R->Sled->Nam){
	   if(FindVary(P->R->Sled->Nam,&V)) YKmax[i]=Val(V->Znach);
	   else{
		swprintf(Inf,L"В инструкции %s для обобщенной координаты %s\n"
		 L"указана первая граница\nотсутствующей в исходных данных перменной - %s",
		 I->Name,q->V->Name,P->R->Sled->Nam);
		Application->MessageBox(Inf,L"Ошибка!",MB_OK|MB_ICONERROR);
		Error(0,0,true); return false;
	  }}
	  else YKmax[i]=P->R->Sled->Val;
	  if(YKmax[i]<YKmin[i]){
	   P->R->Val=YKmax[i]; YKmax[i]=YKmin[i]; YKmin[i]=P->R->Val;
	}}}
	for(q=L->VK,i=0;q;q=q->Sled,i++){
	 if(FindParm(I->First,q->V->Name,&P)){
	  if(!P->R){
	   swprintf(Inf,L"В инструкции %s для обобщенной скорости %s\n"
		L"не указаны границы",I->Name,q->V->Name);
	   Application->MessageBox(Inf,L"Ошибка!",MB_OK|MB_ICONERROR);
	   Error(0,0,true); return false;
	  }
	  if(KolElem(P->R)!=2){
	   swprintf(Inf,L"В инструкции %s для обобщенной скорости %s\n"
		L"указана только одна граница",I->Name,q->V->Name);
	   Application->MessageBox(Inf,L"Ошибка!",MB_OK|MB_ICONERROR);
	   Error(0,0,true); return false;
	  }
	  if(P->R->Nam){
	   if(FindVary(P->R->Nam,&V)) YKmin[i+N]=Val(V->Znach);
	   else{
		swprintf(Inf,L"В инструкции %s для обобщенной координаты %s\n"
		 L"указана первая граница\nотсутствующей в исходных данных перменной - %s",
		 I->Name,q->V->Name,P->R->Nam);
		Application->MessageBox(Inf,L"Ошибка!",MB_OK|MB_ICONERROR);
		Error(0,0,true); return false;
	  }}
	  else YKmin[i+N]=P->R->Val;
	  if(P->R->Sled->Nam){
	   if(FindVary(P->R->Sled->Nam,&V)) YKmax[i+N]=Val(V->Znach);
	   else{
		swprintf(Inf,L"В инструкции %s для обобщенной координаты %s\n"
		 L"указана первая граница\nотсутствующей в исходных данных перменной - %s",
		 I->Name,q->V->Name,P->R->Sled->Nam);
		Application->MessageBox(Inf,L"Ошибка!",MB_OK|MB_ICONERROR);
		Error(0,0,true); return false;
	  }}
	  else YKmax[i+N]=P->R->Sled->Val;
	  if(YKmax[i+N]<YKmin[i+N]){
	   P->R->Val=YKmax[i+N]; YKmax[i+N]=YKmin[i+N]; YKmin[i+N]=P->R->Val;
   }}}}
   if((I=FindInstLast(L"OTLADKA"))||(I=FindInstLast(L"ОТЛАДКА"))){
	OTLADKA=true; _wunlink(L"Otladka.");
   }
   if(!(I=FindInstLast(L"НАЧАЛЬНОЕ ПОЛОЖЕНИЕ"))&&
	  !(I=FindInstLast(L"НАЧАЛЬНОЕ ПРИБЛИЖЕНИЕ"))){
	swprintf(Inf,L"В исходных данных нет инструкции\n"
	 L"\"НАЧАЛЬНОЕ ПОЛОЖЕНИЕ\" или\n"
	 L"\НАЧАЛЬНОЕ ПРИБЛИЖЕНИЕ\"!\n"
	 L"В этой инструкции должны быть перечисленны\n"
	 L"обобщенные координаты с начальными значениями");
	Application->MessageBox(Inf,L"Ошибка!",MB_OK|MB_ICONERROR);
	Error(0,0,true); return false;
   }
   else{
	wchar_t S[256]; Magazine *m,*MOK=NULL,*Mlv=NULL; Parm *P; Cord *q;
	for(Cord *K=L->K;K;K=K->Sled){
	 if(!FindParm(I->First,K->V->Name,&P)) TakeMagazine(&MOK,K->V->Name);
	}
	if(m=MOK){ wchar_t *inf=Inf; int l,L=0;
	 if(MOK->Sled){
	  inf+=swprintf(inf,L"В инструкции \"%s\" не задано\n"
	   L"начальное приближение для обобщенных координат:\n",I->Name);
	  for(;m->Sled;m=m->Sled){
	   l=wcslen(m->S);
	   if((L+l)>102){ inf+=swprintf(inf,L"%s,\n",m->S); L=0; }
	   else{ l=swprintf(inf,L"%s, ",m->S); L+=l; inf+=l; }
	 }}
	 else
	  inf+=swprintf(inf=Inf,L"В инструкции \"%s\" не задано\n"
	   L"начальное приближение для обобщенной координаты %s\n",I->Name,m->S);
	 inf+=swprintf(inf,L"Формат инструкции должен быть таким:\n"
	  L"\"НАЧАЛЬНОЕ ПОЛОЖЕНИЕ=q1(0),q2(0),...;\"");
	 Atten(I->Line,0);
	 DelAllMagazine(&MOK);
	}
	for(q=L->K,i=0;q;q=q->Sled,i++){ q->V->P=0; q->V->Val=0.0; }
	for(P=I->First,i=1;P;P=P->Sled,i++){
	 wchar_t *s; Form F; Vary *VV; int Rez; Cnst *C;
	 if(!P->Nam||!P->R||(KolElem(P->R)>1)){
	  swprintf(Inf,L"В инструкции \"%s\" %d-й параметр должен быть задан\n"
	   L"именем обощенной координаты и значением в скобках или после знака \'=\'.\n"
	   L"Начальное значение для него принимается равным нулю.",I->Name,i);
	  Atten(I->Line,0); break;
	 }
	 if(FindCord(P->Nam,&q)){
	  Vary *V=q->V;
	  if(P->R->Nam){
	   Rez=FindVary(P->R->Nam,&VV); if(!Rez) Rez=FindCnst(P->R->Nam,&C);
	   F=StrToForm(P->R->Nam);
	   if((F.V->Atr==4)&&!Rez){
		swprintf(Inf,L"В инструкции \"%s\" параметр\n"
		 L"\"%s\"\nне найден среди переменных задачи.",I->Name,P->R->Nam);
		Atten(I->Line,0);
	   }
	   V->Val=Val(F);
	  }
	  else if(P->R->F.C&&(P->R->F!=Pust))
	   V->Val=Val(P->R->F);
	  else
	   V->Val=P->R->Val;
	 }
	 else TakeMagazine(&Mlv,P->Nam);
	}
	if(Mlv){ wchar_t *inf=Inf; int l,L=0;
	 if(Mlv->Sled){
	  inf+=swprintf(inf,
	   L"В инструкции \"%s\" следующие переменные\n"
	   L"не являются обобщенной координатой:\n",I->Name);
	  for(m=Mlv;m->Sled;m=m->Sled){
	   l=wcslen(m->S);
	   if((L+l)>55){ inf+=swprintf(inf,L"%s,\n",m->S); L=0; }
	   else{ l=swprintf(inf,L"%s, ",m->S); L+=l; inf+=l; }
	  }
	  inf+=swprintf(inf,L"%s.\nФормат инструкции должен быть таким:\n"
	   L"\"НАЧАЛЬНОЕ ПРИБЛИЖЕНИЕ=q1(0),q2(0),...;\"",m->S);
	 }
	 else
	  swprintf(Inf,L"В инструкции \"%s\" параметр\n"
	   L"\"%s\"\nне является обобщенной координатой!\n"
	   L"Формат инструкции должен быть таким:\n"
	   L"\"НАЧАЛЬНОЕ ПОЛОЖЕНИЕ:=q1(0),q2(0),...;\"",I->Name,Mlv->S);
	 Atten(I->Line,0);
	 DelAllMagazine(&Mlv);
	}
   //запомним начальное приближение
	for(i=0,q=L->K;q;q=q->Sled,i++) q0[i]=YK[i]=q->V->Val;
   }
   if(!(I=FindInstLast(L"УРАВНЕНИЯ"))&&!(I=FindInstLast(L"УРАВНЕНИЕ"))){
	if(N>1)
	 swprintf(Inf,L"В исходных данных нет инструкции\n"
	  L"\"УРАВНЕНИЯ\":=...;!\n"
	  L"В этой инструкции должны быть перечисленны\n"
	  L"переменные, корни формул которых ищутся");
	else
	 swprintf(Inf,L"В исходных данных нет инструкции\n"
	  L"\УРАВНЕНИЕ:=...;\"!\n"
	  L"В этой инструкции должна быть указана\n"
	  L"переменная, корень формулы которой ищется");
	Application->MessageBox(Inf,L"Ошибка!",MB_OK|MB_ICONERROR);
	Error(0,0,true); return false;
   }
   else{
	if(KolElem(I->First)!=N){
	 swprintf(Inf,L"В исходных данных в инструкции\n"
	  L"\УРАВНЕНИЯ:=...;\" число перечисленых переменных\n"
	  L"не равно числу обобщенных координат системы");
	 Application->MessageBox(Inf,L"Ошибка!",MB_OK|MB_ICONERROR);
	 Error(0,0,true); return false;
	}
	else{
	 int i=0,Sum=0; Vary *V;
	 bool *VeloKin=(bool *)calloc(N,sizeof(bool));
	 for(P=I->First;P;i++,P=P->Sled){
	  if(!P->Nam){
	   swprintf(Inf,L"В инструкции %s\n%d-й параметр\nне является именем",I->Name,i+1);
	   Application->MessageBox(Inf,L"Ошибка!",MB_OK|MB_ICONERROR);
	   Error(0,0,true);
	   break;
	  }
	  else if(!FindVary(P->Nam,&V)){
	   swprintf(Inf,L"В инструкции %s\nпараметр %s\nне найден среди переменных задачи",
		I->Name,P->Nam);
	   Application->MessageBox(Inf,L"Ошибка!",MB_OK|MB_ICONERROR);
	   Error(0,0,true);
	   break;
	  }
	  else{ Cord *K; int j;
	   Uravn[i]=V->Znach;
	   for(K=L->VK;K;K=K->Sled){
		if(VarInForm(true,V->Znach,K->V)){
		 VeloKin[i]=true; break;
	 }}}}
	 for(i=0;i<N;i++) Sum+=VeloKin[i];
	 if(Sum>0){
	  if(Sum!=N){ int It=-1,If=-1;
	   for(i=0;i<N;i++){
		if(!VeloKin[i]){ if(It==-1) It=i; }
		else{ if(If==-1) If=i; }
	   }
	   swprintf(Inf,L"В инструкции %s\n"
		L"перечисленные переменные\n"
		L"либо ВСЕ НЕ ДОЛЖНЫ зависеть от обобщенных скоростей\n"
		L"либо ВСЕ ДОЛЖНЫ зависеть от обобщенных скоростей.\n"
		L"Например, %d-я переменная зависит от обобщенных скоростей,\n"
		L"а %d-я переменная - не зависит от обобщенных скоростей",I->Name,It,If);
	   Application->MessageBox(Inf,L"Ошибка!",MB_OK|MB_ICONERROR);
	   Error(0,0,true);
	  }
	  else VELOKIN=1;
	 }
//     else if(FindInstLast(L"КООРДИНАТЫ ЦЕНТРА МАСС")) VELOKIN=2;
	 else VELOKIN=0;
	 free(VeloKin);
   }}
   if(VELOKIN==1){//Обратная кинематика по обобщенным скоростям
	for(i=0,P=I->First;P;i++,P=P->Sled){
	 FindVary(P->Nam,&V);
	 for(K=L->VK,j=0;K;K=K->Sled)
	  Af[i][j++]=DifBy(V->Znach,K->V);
	 for(K=L->VK;K;K=K->Sled) K->V->Znach.C=Nul;
	 Bf[i]=ObNul(Uravn[i]);
	 for(K=L->VK;K;K=K->Sled) K->V->Znach.C=Pust;
   }}
   else{//Обратная кинематика по координатам или по скоростям ЦМ
	for(i=0,P=I->First;P;i++,P=P->Sled){
	 Bf[i]=Uravn[i];
	 FindVary(P->Nam,&V);
	 for(K=L->K,j=0;K;K=K->Sled)
	  Af[i][j++]=DifBy(V->Znach,K->V);
 }}}}
 else{
//  mToProc->Visible=false;
  if(N){
   swprintf(Inf,L"\n%s есть переменные без определяющих формул.\n"
	L"Такие переменные считаются обобщенными координатами.\n"
	L"См. список \"Обобщенные координаты\" в главном меню или\n"
	L"в закладке \"Диагностика\"",Lst);
   Error(0,0,true);
 }}
 for(Magazine *m=MTabAdd;m;m=m->Sled)
  TabControl->Tabs->Add(m->S);
//вернем переменным интегрирования и аргументам функций поиска корней значения Pust
 for(Intg *I=L->I;I;I=I->Sled) I->x->Znach.C=Pust;
 for(Solu *c=L->C;c;c=c->Sled) c->x->Znach.C=Pust;
 swprintf(Time2,L"%s'%s",L->Time->Name,L->Time->Name);
 swprintf(Time3,L"%s'%s'%s",L->Time->Name,L->Time->Name,L->Time->Name);
//вставлено 4.04.2014, существенно убыстряющее счет благодаря замене
//формул для переменных, независящих от времени, их один раз вычисленными
//числовыми значениями
 ClearVar();
 wcscpy(SBold,StatusBar->SimpleText.c_str());
 StatusBar->SimpleText=L"Вычисление переменных с постоянными значениями";
// if(CGauge){
//  int K=0;
//  for(Vary *V=L->V;V;V=V->Sled)
//   if(!VarInForm(true,V->Znach,L->Time)&&
//	  !VassalFromVaryDiap(V->Znach)&&
//	  !VassalOfKoord(L,V->Znach))
//	   K++;
//  CGauge->MaxValue=K; CGauge->Progress=0; //CGauge->ShowText=true;
// }
 ReplaceVarsToConst(StatusBar,CGauge,false);
 StatusBar->SimpleText=SBold;
 SLH->Add(L"<h4 align=\"left\"><font color=\"#009900\">"
  L"2. Формулы для переменных, не равных нулю:</font></h4>");
 SLH->Add(L"<table width=\"100%\" border=\"1\" style=\"word-wrap: break-word;\">");
// fwprintf(HTMFILE,
//  L"<h4 align=\"left\"><font color=\"#009900\">"
//  L"2. Формулы для переменных, не равных нулю:</font></h4>\n");
// fwprintf(HTMFILE,L"<table width=\"100%\" border=\"1\">");
 if(CGauge){
  CGauge->MaxValue=KolElem(L->V);
  CGauge->Progress=0; //CGauge->ShowText=true;
 }
 wcscpy(SBold,StatusBar->SimpleText.c_str());
 StatusBar->SimpleText=L"Вывод в отчет формул для переменных не равных нулю";
 for(i=1,V=L->V;V;V=V->Sled){
  if(CGauge) CGauge->Progress++;
  if(V->Znach.C->Atr&&((V->Znach.C->Atr!=3)||V->Znach.C!=Nul)){
   if(i>KColumn){
	i=1;
//	if(V!=L->V) fwprintf(HTMFILE,L"</tr>\n");
	if(V!=L->V) SLH->Add(L"</tr>");
   }
  // if(i==1) fwprintf(HTMFILE,L"<tr>\n");
   if(i==1)	SLH->Add(L"<tr>");
   shtm+=swprintf(shtm=Shtm,
	L"<td width=\"%0.0f%%\">",Pr);
//   fwprintf(HTMFILE,L"<td width=\"%0.0f%%\">",Round(100/KColumn));
   if(SMALLFONT) shtm+=swprintf(shtm,L"<font size=-1>");
//   if(SMALLFONT) fwprintf(HTMFILE,L"<font size=-%d>",1);
   shtm=FormToStringList(V->Znach,V->Name,Shtm,shtm,SLH,false,&SF);
//   PrintForm(false,HTMFILE,V->Name,V->Znach);
   if(!V->P&&V->Znach.C->Atr>CNST){
	shtm+=swprintf(shtm,L" = %g",V->Val);
//	fwprintf(HTMFILE,L" = %g",V->Val);
	if(V->Razm.C!=Pust){
	 shtm+=swprintf(shtm,L", ");
//	 fputws(L", ",HTMFILE);
	 shtm=FormToStringList(V->Razm,NULL,Shtm,shtm,SLH,false,&SF);
//	 PrintForm(false,HTMFILE,NULL,V->Razm);
	}
	*shtm++=';'; *shtm='\0';
//	fputwc(';',HTMFILE);
   }
   if(SMALLFONT){
	shtm+=swprintf(shtm,L"</font>");
	SLH->Add(Shtm); *Shtm='\0'; shtm=Shtm;
   }
//   if(SMALLFONT) fputws(L"</font>\n",HTMFILE);
   shtm+=swprintf(shtm,L"</td>"); i++;
   SLH->Add(Shtm); shtm=Shtm;
//   fputws(L"</td>\n",HTMFILE); i++;
 }}
 StatusBar->SimpleText=SBold;
 if(CGauge) CGauge->Progress=0;
 shtm+=swprintf(shtm,L"</tr></table>");
 SLH->Add(Shtm); *Shtm='\0'; shtm=Shtm;
// fwprintf(HTMFILE,L"</tr></table>\n");
// ReplaceVarsToConst(StatusBar,CGauge,false);
 for(Tv=L->Inst;Tv;Tv=Tv->Sled){
  LowToUpp(Tv->Name);
  if(!wcscmp(Tv->Name,L"СОВМЕСТИТЬ")){
   wchar_t Buf[256],*buf; Parm *p; int K; Vary *V; bool Err=false;
   for(p=Tv->First;p;p=p->Sled){ //wchar_t *Arg=NULL;
	if(!p->R){
	 wcscpy(Inf,L"В правой части инструкции \"СОВМЕСТИТЬ\"\n"
	  L"должен стоять составной список:\n"
	  L"название_1(список из 2-х переменных),L"
	  L"название_2(список из 2-х переменных),...;");
	 Atten(0,0); Err=true; break;
	}
	for(Parm *pp=p->R;pp;pp=pp->Sled){
	 if(!FindVary(pp->Nam,&V)){
	  swprintf(Inf,L"Имя \"%s\" из списка \"%s\"\n"
	   L"не найдено среди переменных задачи",pp->Nam,Tv->Name);
	  Atten(0,0); Err=true; break;
	 }
	 if(pp->R){
	  if(KolElem(pp->R)>1){
	   swprintf(Inf,L"Аргумент у переменной \"%s\" из списка \"%s\"\n"
		L"должен быть один",pp->Nam,Tv->Name);
	   Atten(0,0); Err=true; break;
	  }
	  if(pp->R->Val!=0.0){
	   swprintf(Inf,L"Аргументом у переменной \"%s\" из списка \"%s\"\n"
		L"должна быть переменная",pp->Nam,Tv->Name);
	   Atten(0,0); Err=true; break;
   }}}}
   for(p=Tv->First,L->Time->P=0;p;p=p->Sled){
	K=KolElem(p->R);
	if(K!=2){
	 swprintf(Inf,
	  L"Число переменных для совместного отображения на графике\n"
	  L"в инструкции \"%s\" должно быть равно 2!",Tv->Name);
	 Atten(0,0); /*Err=true;*/ break;
	}
	else if(!Err){
	 buf+=swprintf(buf=Buf,L"%s(",p->Nam);
	 for(Parm *pp=p->R;pp;pp=pp->Sled){
	  buf+=swprintf(buf,L"%s",pp->Nam);
	  if(pp->R) buf+=swprintf(buf,L"(%s)",pp->R->Nam);
	  buf+=swprintf(buf,L"%c",pp->Sled?',':')');
	 }
	 swprintf(Inf,L"%d",TabControl->Tabs->Count);
	 TabControl->Tabs->Add(Buf);
	 TakeMagazine(&Sovm,Inf);
 }}}}
 for(Tv=L->Inst;Tv;Tv=Tv->Sled){
  LowToUpp(Tv->Name);
  if(!wcscmp(Tv->Name,L"СРАВНИТЬ")){
   wchar_t Buf[256],*buf; Parm *p; bool Err=false;
   for(p=Tv->First;p;p=p->Sled){ //wchar_t *Arg=NULL;
	if(!p->R){
	 wcscpy(Inf,L"В правой части инструкции \"СРАВНИТЬ\"\n"
	  L"должен стоять составной список:\n"
	  L"название_1(список переменных),L"
	  L"название_2(список переменных),...;");
	 Atten(0,0); Err=true; break;
	}
	for(Parm *pp=p->R;pp;pp=pp->Sled){
	 if(!FindVary(pp->Nam,&V)){
	  swprintf(Inf,L"Имя \"%s\" из списка \"%s\"\n"
	   L"не найдено среди переменных задачи",pp->Nam,Tv->Name);
	  Atten(0,0); Err=true; break;
	 }
	 if(pp->R){
	  if(KolElem(pp->R)>1){
	   swprintf(Inf,L"Аргумент у переменной \"%s\" из списка \"%s\"\n"
		L"должен быть один",pp->Nam,Tv->Name);
	   Atten(0,0); Err=true; break;
	  }
	  if(pp->R->Val!=0.0){
	   swprintf(Inf,L"Аргументом у переменной \"%s\" из списка \"%s\"\n"
		L"должна быть переменная",pp->Nam,Tv->Name);
	   Atten(0,0); Err=true; break;
   }}}}
   for(p=Tv->First,L->Time->P=0;p;p=p->Sled){ wchar_t Buf[512];
	if(KolElem(p->R)>5){
	 swprintf(Inf,
	  L"Число переменных для совместного отображения на графике\n"
	  L"в инструкции \"%s\" не должно быть больше 5!",Tv->Name);
	 Atten(0,0); /*Err=true;*/ break;
	}
	else if(!Err){
	 buf+=swprintf(buf=Buf,L"%s(",p->Nam);
	 for(Parm *pp=p->R;pp;pp=pp->Sled){
	  buf+=swprintf(buf,L"%s",pp->Nam);
	  if(pp->R) buf+=swprintf(buf,L"(%s)",pp->R->Nam);
	  buf+=swprintf(buf,L"%c",pp->Sled?',':')');
	 }
	 swprintf(Inf,L"%d",TabControl->Tabs->Count);
	 TabControl->Tabs->Add(Buf);
	 TakeMagazine(&Sravn,Inf);
 }}}}
 if(!InsTime){
  if((InsTime=FindInstLast(L"ВРЕМЯ"))||(InsTime=FindInstLast(L"ВАРЬИРОВАТЬ"))){
   Vary *V; Parm *p=InsTime->First->R;
   if(KolElem(p)==2){//границы в скобках
	if(p->Nam){ V=TakeVary(p->Nam); Tn=Val(V->Znach); }
	else if(p->F.C&&(p->F.C!=Pust)) Tn=Val(p->F);
	else Tn=p->Val;
	p=p->Sled;
	if(p->Nam){ V=TakeVary(p->Nam); Tk=Tend=Val(V->Znach); }
	else if(p->F.C&&(p->F.C!=Pust)) Tk=Tend=Val(p->F);
	else Tk=Tend=p->Val;
   }
   else if(KolElem(p)==1){//границы в скобках
	swprintf(Inf,L"\n%s в инструкции \"%s\" надо указать нижнюю и верхнюю границу времени",
	 Lst,InsTime->Name);
	Error(0,0,true);
   }
   else{ p=InsTime->First->Sled;//границы подряд в списке
	if(!p){
	 swprintf(Inf,L"\n%s в инструкции \"%s\" не указана нижняя граница времени",
	  Lst,InsTime->Name);
	 Error(0,0,true);
	}
	if(p->Nam){ V=TakeVary(p->Nam); Tn=Val(V->Znach); }
	else if(p->F.C&&(p->F.C!=Pust)) Tn=Val(p->F);
	else Tn=p->Val;
	p=p->Sled;
	if(!p){
	 swprintf(Inf,L"\n%s в инструкции \"%s\" не указана верхняя граница времени",
	  Lst,InsTime->Name);
	 Error(0,0,true);
	}
	if(p->Nam){ V=TakeVary(p->Nam); Tk=Tend=Val(V->Znach); }
	else if(p->F.C&&(p->F.C!=Pust)) Tk=Tend=Val(p->F);
	else Tk=Tend=p->Val;
   }
   if(Tn>Tk){
	swprintf(Inf,L"\n%s конечный момент времени задан меньше начального",Lst);
	Error(0,0,true);
  }}
  else{
   swprintf(Inf,L"\n%s нет инструкции \"ВРЕМЯ\" или \"ВАРЬИРОВАТЬ\"",Lst);
   Error(0,0,true);
 }}
 if(FindInstLast(L"ВСЕ УГЛЫ"))
  ALL_ANGLES=true;
 else if(Tv=FindInstLast(L"УГЛЫ")){
  for(Parm *P=Tv->First;P;P=P->Sled)
   if(P->Nam) TakeMagazine(&Mangles,P->Nam);
 }
 if(!(Tv=FindInstLast(L"ПЕЧАТАТЬ"))&&!(Tv=FindInstLast(L"ПОКАЗАТЬ"))) {
  swprintf(Inf,L"\n%s нет инструкции \"ПОКАЗАТЬ\" или \"ПЕЧАТАТЬ\"",Lst);
  Atten(0,0);
 }
 else{ wchar_t Buf[256],*b;
  LowToUpp(Tv->Name);
  for(p=Tv->First,L->Time->P=0;p;p=p->Sled){ int K=0;
   b+=swprintf(b=Buf,L"%s",p->Nam);
   if(p->R){
	*b++='(';
	for(Parm *pp=p->R;pp;pp=pp->Sled){ K++;
	 b+=swprintf(b,L"%s%c",pp->Nam,pp->Sled?',':')');
   }}
   if(K>2){
	swprintf(Inf,L"Нельзя отобразить %d-мерную линию - \"%s\"!",K+1,Buf);
	Atten(0,0);
   }
   else if(K>1){
	if(FindVary(p->Nam,&VgZ)){
	 if(FindVary(p->R->Nam,&VgX)){
	  if(FindVary(p->R->Sled->Nam,&VgY))
	   TabControl->Tabs->Add(Buf);
	  else{
	   swprintf(Inf,L"Имя \"%s\" из списка \"%s\"\n"
		 L"не найдено среди переменных задачи",p->R->Sled->Nam,Tv->Name);
	   Atten(0,0);
	 }}
	 else{
	  swprintf(Inf,L"Имя \"%s\" из списка \"%s\"\n"
		L"не найдено среди переменных задачи",p->R->Nam,Tv->Name);
	  Atten(0,0);
	}}
	else{
	 swprintf(Inf,L"Имя \"%s\" из списка \"%s\"\n"
	   L"не найдено среди переменных задачи",p->Nam,Tv->Name);
	 Atten(0,0);
   }}
   else if(K>0){
	if(FindVary(p->Nam,&VgY)){
	 if(FindVary(p->R->Nam,&VgX))
	  TabControl->Tabs->Add(Buf);
	 else{
	  swprintf(Inf,L"Имя \"%s\" из списка \"%s\"\n"
		L"не найдено среди переменных задачи",p->R->Nam,Tv->Name);
	  Atten(0,0);
	}}
	else{
	 swprintf(Inf,L"Имя \"%s\" из списка \"%s\"\n"
	   L"не найдено среди переменных задачи",p->Nam,Tv->Name);
	 Atten(0,0);
   }}
   else{
	if(FindVary(p->Nam,&Vp))
	 TabControl->Tabs->Add(p->Nam);
	else{
	 swprintf(Inf,L"Имя \"%s\" из списка \"%s\"\n"
	  L"не найдено среди переменных задачи",p->Nam,Tv->Name);
	 Atten(0,0);
 }}}}
 if((Tv=FindInstLast(L"АНИМАЦИЯ"))||(Tv=FindInstLast(L"АНИМАЦИЯ3D"))){
  if(!MultFilms){
   MultFilms=new TMultFilms(MMultFilms,mMultFilm0,mMultFilm1,mMultFilm2,
	mMultFilm3,mMultFilm4,mMultFilm5,mMultFilm6,mMultFilm7,mMultFilm8,mMultFilm9,
	&WidthPen,&TMult,&Kkadr,Image,PhonImage,
	CGauge,&ColorGraph,&ColorHead,&ColorPhon,
	&ColorGrid,&ColorCifrGrid,&ColorNameOs,&ColorRamka,StatusBar,Tn,Tk,NULL,NULL,
	NULL,NULL,MG,NULL,&NoCalc,&IsNoCalc,&SHLEIF,TimerMultFilm,CppWebBrowser,
	ToolButtonTable,ToolButtonToBMP,ToolButtonPrint,ToolButtonInclude,NULL,
	ToolButtonRead,ToolButtonGame,&TOJPEG,&TOBMP);
 }}
 if(Tv=FindInstLast(L"ТАБЛИЦА"))
  DiagnosTabl(Tv);
 if(INVERSE&&(OK=FindInstLast(L"ОБОБЩЕННЫЕ КООРДИНАТЫ В ФАЙЛ")))
  DiagnosOK(OK);
M:
//можно все переменные, не зависящие от времени заменить константами
//  for(Vary *V=L->V;V;V=V->Sled){ double D;
//   if(!VarInForm(true,V->Znach,L->Time)){
//    D=Val(V->Znach); V->Znach.C=TakeCnst(D);
//  }}
// Lpodv=fwprintf(HTMFILE,
//  L"<hr><font face=\"Georgia, Times New Roman, Times, serif\""
//  L" color=\"#326464\">&copy; %s, %d<br></font><br>\n",Autor,NYear+1900)+1;
// Lpodv+=fwprintf(HTMFILE,L"</body>\n</html>\n")+2;
 SLP=new TStringList;
 swprintf(Shtm,
  L"<hr><font face=\"Georgia, Times New Roman, Times, serif\""
  L" color=\"#326464\">&copy; %s, %d<br></font><br>\n",
  Autor,NYear+1900);
 SLP->Add(Shtm); SLP->Add(L"</body>"); SLP->Add(L"</html>");
 Kpodv=SLP->Count;
 for(int i=0;i<Kpodv;i++) SLH->Add(SLP->Strings[i]);
// fclose(HTMFILE);
 SLH->SaveToFile(HtmFile);
 TabControl->TabIndex=-1;
 MReportClick(NULL);
 return true;
}
//---------------------------------------------------------------------------
bool LoadFromFileDOS(TStringList *SL,wchar_t *File){
 wchar_t ANSI[256],*s,Exec[256],S[256]; int Rez;
 wcscpy(ANSI,File);
 if(s=wcsrchr(ANSI,'.')) wcscpy(s,L".ansi");
 swprintf(Exec,L"%s\\ConvertASCIItoANSI.exe %s",WORKDIR,File);
 Rez=WinExec(AnsiString(Exec).c_str(),0);
 if(Rez>31)
  SL->LoadFromFile(ANSI);
 _wunlink(ANSI);
 return Rez>31;
}
//------------------------------------------------------------------------
wchar_t *ReadStrOpt(wchar_t *S,TStringList *SO,int i){
 if(i<SO->Count){
  wchar_t *s; wcscpy(S,SO->Strings[i].w_str());
  if((s=wcschr(S,'#'))||(s=wcschr(S,'\n'))) s--;
  else s=S+wcslen(S)-1;
  while(*s==' ') s--; *(++s)='\0';
  return S;
 }
 else return NULL;
}
//---------------------------------------------------------------------------
bool SaveToFileDOS(TStringList *SL,wchar_t *File){
 wchar_t ASCII[256],*s,Exec[256],S[256]; int Rez;
 wcscpy(ASCII,File);
 if(s=wcsrchr(ASCII,'.')) wcscpy(s,L".ascii");
 SL->SaveToFile(File);
 swprintf(Exec,L"%s\\ConvertANSItoASCII.exe %s",WORKDIR,File);
 Rez=WinExec(AnsiString(Exec).c_str(),0);
 if(Rez>31){
  _wunlink(File);
  _wrename(ASCII,File);
 }
 return Rez>31;
}
//------------------------------------------------------------------------
void __fastcall TFormKinema::FileToOpt(){
 wchar_t *s,FileOpt[256],S[256]; int i=0;
 TStringList *SLo=new TStringList;
 ColorHead=clRed;         ColorNameOs=clPurple;    ColorTabl=clTeal;
 ColorGrid=clBlue;        ColorCifrGrid=clBlue;    ColorRamka=clNavy;
 ColorGraph=clRed;        CountTabl=50;            ColorGraph2=clLime;
 ColorGraph3=clYellow;    ColorGraph4=clBlue;      ColorGraph5=clFuchsia;
 KTime=1.0;               ColorPrint=false;        Perspect=true;
 WidthPen=3;              ColorBMP=true;           TMult=5000;
 Kkadr=50;                SHLEIF=false;            Eps1=Eps2=Eta=Eps=1e-6;
 VISIBLETIMEINTEGR=false; NORM=0;                  PRINTABL=true;
 NameFont=L"Courier New";  ItFont=ExFont=UnFont=0;  SizeFont=12;
 ColorFont=clBlack;       ColorPhon=clWhite;       PRINTOP=false;
 RENOVCALC=false;         Kbmp=5.0/4.0;            TOJPEG=TOBMP=false;
 ALLOW=PERMUT=false;      KEqvScal=2.0;            TabControl->MultiLine=false;
 PRNTOPRIGHT=true;        PRINTHEAD=true;          KColumn=3;
 VISIBLEKINETICENERGY=VISIBLEDISSIPFUNCTION=false;
 VISIBLEPOTENENERGY=VISIBLEAPPELFUNCTION=false;
 HTMLREPORT=true;
 ecvt(Eta,NDIG,&NDIG,&sign); NDIG=abs(NDIG)+1;
 swprintf(FileOpt,L"%s\\kidym.opt",WORKDIR);
 if(!_waccess(FileOpt,0)){
  wchar_t FileOptDir[256];
  swprintf(FileOptDir,L"%s\\kidym.opt",CurrDir);
  _wunlink(FileOptDir);
  CopyFile(AnsiString(FileOpt).c_str(),
   AnsiString(FileOptDir).c_str(),true);
  LoadFromFileDOS(SLo,FileOptDir);
  if(ReadStrOpt(S,SLo,i++)) NameFontEd=S;
  else NameFontEd=L"Courier New";
  ItFontEd=ReadStrOpt(S,SLo,i++)?_wtoi(S):0;
  ExFontEd=ReadStrOpt(S,SLo,i++)?_wtoi(S):0;
  UnFontEd=ReadStrOpt(S,SLo,i++)?_wtoi(S):0;
  SizeFontEd=ReadStrOpt(S,SLo,i++)?_wtoi(S):12;
  ColorFontEd=ReadStrOpt(S,SLo,i++)?(TColor)_wtol(S):clBlack;
  ColorPhonEd=ReadStrOpt(S,SLo,i++)?(TColor)_wtol(S):clWhite;
  for(int j=0;j<5;j++) ReadStrOpt(S,SLo,i++);
  LANGUAGELITERA=S[0];
  for(int j=0;j<2;j++) ReadStrOpt(S,SLo,i++);
  wcscpy(BrowserName,S);
 }
 if(!_waccess(OptFile,0)){ i=0;
  SLo->Clear();
  LoadFromFileDOS(SLo,OptFile);
  if(ReadStrOpt(S,SLo,i++)) NameFontEd=S;
  else NameFontEd=L"Courier New";
  ItFontEd=ReadStrOpt(S,SLo,i++)?_wtoi(S):0;
  ExFontEd=ReadStrOpt(S,SLo,i++)?_wtoi(S):0;
  UnFontEd=ReadStrOpt(S,SLo,i++)?_wtoi(S):0;
  SizeFontEd=ReadStrOpt(S,SLo,i++)?_wtoi(S):12;
  ColorFontEd=ReadStrOpt(S,SLo,i++)?(TColor)_wtol(S):clBlack;
  ColorPhonEd=ReadStrOpt(S,SLo,i++)?(TColor)_wtol(S):clWhite;
  ColorHead=ReadStrOpt(S,SLo,i++)?(TColor)_wtol(S):clRed;
  ColorNameOs=ReadStrOpt(S,SLo,i++)?(TColor)_wtol(S):clPurple;
  ColorTabl=ReadStrOpt(S,SLo,i++)?(TColor)_wtol(S):clTeal;
  ColorGrid=ReadStrOpt(S,SLo,i++)?(TColor)_wtol(S):clBlue;
  ColorCifrGrid=ReadStrOpt(S,SLo,i++)?(TColor)_wtol(S):clBlue;
  ColorRamka=ReadStrOpt(S,SLo,i++)?(TColor)_wtol(S):clNavy;
  ColorGraph=ReadStrOpt(S,SLo,i++)?(TColor)_wtol(S):clRed;
  CountTabl=ReadStrOpt(S,SLo,i++)?_wtoi(S):50;
  ColorGraph2=ReadStrOpt(S,SLo,i++)?(TColor)_wtol(S):clLime;
  ColorGraph3=ReadStrOpt(S,SLo,i++)?(TColor)_wtol(S):clYellow;
  ColorGraph4=ReadStrOpt(S,SLo,i++)?(TColor)_wtol(S):clBlue;
  ColorGraph5=ReadStrOpt(S,SLo,i++)?(TColor)_wtol(S):clFuchsia;
  KTime=ReadStrOpt(S,SLo,i++)?_wtof(S):1.0; if(KTime>10) KTime=1;
  ColorPrint=(bool)ReadStrOpt(S,SLo,i++)?_wtoi(S):0;
  Perspect=(bool)ReadStrOpt(S,SLo,i++)?_wtoi(S):0;
  WidthPen=ReadStrOpt(S,SLo,i++)?_wtoi(S):3;
  ColorBMP=(bool)ReadStrOpt(S,SLo,i++)?_wtoi(S):0;
  TMult=ReadStrOpt(S,SLo,i++)?_wtol(S):0;
  Kkadr=ReadStrOpt(S,SLo,i++)?_wtoi(S):0;
  SHLEIF=(bool)(ReadStrOpt(S,SLo,i++)?_wtoi(S):0);
  mStroboSkop->ImageIndex=45-SHLEIF;
  Eta=Eps2=Eps=Eps1=ReadStrOpt(S,SLo,i++)?_wtof(S):1e-6;
  if(Eps1>0.01){ Eps1=1e-6; ChangeOpt=true; }
  ecvt(Eta,NDIG,&NDIG,&sign); NDIG=abs(NDIG)+1;
  VISIBLETIMEINTEGR=(bool)(ReadStrOpt(S,SLo,i++)?_wtoi(S):0);
  NORM=ReadStrOpt(S,SLo,i++)?_wtoi(S):true;
  PRINTABL=(bool)(ReadStrOpt(S,SLo,i++)?_wtoi(S):0);
  mPrintTabl->ImageIndex=45-PRINTABL;
  if(ReadStrOpt(S,SLo,i++)&&(s=wcschr(S,'#'))){
   for(s--;(*s==' ')&&(s>S);s--); s++; *s='\0'; NameFont=S;
  }
  else NameFont=L"Courier New";
  ItFont=ReadStrOpt(S,SLo,i++)?_wtoi(S):0;
  ExFont=ReadStrOpt(S,SLo,i++)?_wtoi(S):0;
  UnFont=ReadStrOpt(S,SLo,i++)?_wtoi(S):0;
  SizeFont=ReadStrOpt(S,SLo,i++)?_wtoi(S):12; SzFont=&SizeFont;
  ColorFont=ReadStrOpt(S,SLo,i++)?(TColor)_wtol(S):clBlack;
  ColorPhon=ReadStrOpt(S,SLo,i++)?(TColor)_wtol(S):clWhite;
  PRINTOP=(bool)(ReadStrOpt(S,SLo,i++)?_wtoi(S):1);
  RENOVCALC=(bool)(ReadStrOpt(S,SLo,i++)?_wtoi(S):0);
  Kbmp=ReadStrOpt(S,SLo,i++)?_wtof(S):5.0/4.0; if(Kbmp<10) Kbmp=5.0/4.0;
  TOJPEG=(bool)(ReadStrOpt(S,SLo,i++)?_wtoi(S):0); mToJPEG->ImageIndex=45-TOJPEG;
  TOBMP=(bool)(ReadStrOpt(S,SLo,i++)?_wtoi(S):0);  mToBMP->ImageIndex=45-TOBMP;
  ALLOW=(bool)(ReadStrOpt(S,SLo,i++)?_wtoi(S):0);
  PERMUT=(bool)(ReadStrOpt(S,SLo,i++)?_wtoi(S):0);
  TabControl->MultiLine=!(bool)(ReadStrOpt(S,SLo,i++)?_wtoi(S):0);
  if(ReadStrOpt(S,SLo,i++)) KEqvScal=_wtof(S);
  PRNTOPRIGHT=(bool)(ReadStrOpt(S,SLo,i++)?_wtoi(S):1);
  PRINTHEAD=(bool)(ReadStrOpt(S,SLo,i++)?_wtoi(S):1);
  KColumn=(ReadStrOpt(S,SLo,i++)?_wtoi(S):3); if(!KColumn) KColumn=3;
  VISIBLEKINETICENERGY=(ReadStrOpt(S,SLo,i++)?_wtoi(S):0);
  VISIBLEDISSIPFUNCTION=(ReadStrOpt(S,SLo,i++)?_wtoi(S):0);
  VISIBLEPOTENENERGY=(ReadStrOpt(S,SLo,i++)?_wtoi(S):0);
  VISIBLEAPPELFUNCTION=(ReadStrOpt(S,SLo,i++)?_wtoi(S):0);
 }
 else
  OptToFile();
 mPrintOp->ImageIndex=45-PRINTOP;
 mPrintTabl->ImageIndex=45-PRINTABL;
 mPrintTopRight->ImageIndex=45-PRNTOPRIGHT;
 mPrintHead->ImageIndex=45-PRINTHEAD;
 UpDownKTime->Position=Round(KTime*10.0);
 mMarksInRow->ImageIndex=44+TabControl->MultiLine;
 mRenovCalc->ImageIndex=45-RENOVCALC;
 delete SLo;
}
//---------------------------------------------------------------------------
void __fastcall TFormKinema::OptToFile(){
 if(ChangeOpt){
  TStringList *SLo=new TStringList;
  swprintf(Inf,L"%-40s#Шрифт\n",NameFontEd); SLo->Add(Inf);
  swprintf(Inf,L"%-40d#Наклон шрифта: 1 - есть, 0 - нет\n",ItFontEd); SLo->Add(Inf);
  swprintf(Inf,L"%-40d#Жирность шрифта: 1 - есть, 0 - нет\n",ExFontEd);
  SLo->Add(Inf);
  swprintf(Inf,L"%-40d#Подчеркивание шрифта: 1 - есть, 0 - нет\n",UnFontEd);
  SLo->Add(Inf);
  swprintf(Inf,L"%-40d#Размер шрифта\n",SizeFontEd); SLo->Add(Inf);
  swprintf(Inf,L"%-40ld#Цвет шрифта\n",ColorFontEd); SLo->Add(Inf);
  swprintf(Inf,L"%-40ld#Цвет фона\n",ColorPhonEd); SLo->Add(Inf);
  swprintf(Inf,L"%-40ld#Цвет заголовка\n",ColorHead); SLo->Add(Inf);
  swprintf(Inf,L"%-40ld#Цвет названий осей\n",ColorNameOs); SLo->Add(Inf);
  swprintf(Inf,L"%-40ld#Цвет таблицы\n",ColorTabl); SLo->Add(Inf);
  swprintf(Inf,L"%-40ld#Цвет сетки\n",ColorGrid); SLo->Add(Inf);
  swprintf(Inf,L"%-40ld#Цвет цифр сетки\n",ColorCifrGrid); SLo->Add(Inf);
  swprintf(Inf,L"%-40ld#Цвет рамки\n",ColorRamka); SLo->Add(Inf);
  swprintf(Inf,L"%-40ld#Цвет графика\n",ColorGraph); SLo->Add(Inf);
  swprintf(Inf,L"%-40d#Число значений в таблицу\n",CountTabl); SLo->Add(Inf);
  swprintf(Inf,L"%-40ld#Цвет графика 2\n",ColorGraph2); SLo->Add(Inf);
  swprintf(Inf,L"%-40ld#Цвет графика 3\n",ColorGraph3); SLo->Add(Inf);
  swprintf(Inf,L"%-40ld#Цвет графика 4\n",ColorGraph4); SLo->Add(Inf);
  swprintf(Inf,L"%-40ld#Цвет графика 5\n",ColorGraph5); SLo->Add(Inf);
  swprintf(Inf,L"%-40.2f#Коэффициент времени\n",KTime); SLo->Add(Inf);
  swprintf(Inf,L"%-40d#Цветная печать\n",ColorPrint);   SLo->Add(Inf);
  swprintf(Inf,L"%-40d#3D-перспектива: 1 - есть, 0 - нет\n",Perspect); SLo->Add(Inf);
  swprintf(Inf,L"%-40d#Толщина линий и точек графиков\n",WidthPen); SLo->Add(Inf);
  swprintf(Inf,L"%-40d#Цветное BMP\n",ColorBMP);   SLo->Add(Inf);
  swprintf(Inf,L"%-40ld#Длительность анимации, миллисек\n",TMult); SLo->Add(Inf);
  swprintf(Inf,L"%-40d#Число кадров в сек\n",Kkadr); SLo->Add(Inf);
  swprintf(Inf,L"%-40d#Шлейф: 1 - есть, 0 - нет\n",SHLEIF); SLo->Add(Inf);
  swprintf(Inf,L"%-40g#Точность интегрирования\n",Eps1); SLo->Add(Inf);
  swprintf(Inf,
   L"%-40d#Выводить время интегрированя динамического процесса: 1 - да, 0 - нет\n",
   VISIBLETIMEINTEGR);
  SLo->Add(Inf);
  swprintf(Inf,L"%-40d#Нормировка форм колебаний\n",NORM); SLo->Add(Inf);
  swprintf(Inf,L"%-40d#Печатать таблицу значений: 1 - да, 0 - нет\n",PRINTABL);
  SLo->Add(Inf);
  swprintf(Inf,L"%-40s#Шрифт приложения\n",NameFont.c_str()); SLo->Add(Inf);
  swprintf(Inf,L"%-40d#Наклон шрифта приложения: 1 - есть, 0 - нет\n",ItFont);
  SLo->Add(Inf);
  swprintf(Inf,L"%-40d#Жирность шрифта приложения: 1 - есть, 0 - нет\n",ExFont);
  SLo->Add(Inf);
  swprintf(Inf,L"%-40d#Подчеркивание шрифта приложения: 1 - есть, 0 - нет\n",UnFont);
  SLo->Add(Inf);
  swprintf(Inf,L"%-40d#Размер шрифта приложения\n",SizeFont); SLo->Add(Inf);
  swprintf(Inf,L"%-40ld#Цвет шрифта приложения\n",ColorFont); SLo->Add(Inf);
  swprintf(Inf,L"%-40ld#Цвет фона приложения\n",ColorPhon); SLo->Add(Inf);
  swprintf(Inf,L"%-40d#Печатать число операций в формулах: 1 - да, 0 - нет\n",
   PRINTOP); SLo->Add(Inf);
  swprintf(Inf,
   L"%-40d#Предлагать воспользоваться старыми результатами: 1 - да, 0 - нет\n",
   RENOVCALC); SLo->Add(Inf);
  swprintf(Inf,L"%-40g#Коэффициент сжатия BMP\n",Kbmp); SLo->Add(Inf);
  swprintf(Inf,L"%-40d#Писать анимацию в JPEG-файл: 1 - есть, 0 - нет\n",TOJPEG);
  SLo->Add(Inf);
  swprintf(Inf,L"%-40d#Анимацию писать в BMP-файл: 1 - да, 0 - нет\n",TOBMP);
  SLo->Add(Inf);
  swprintf(Inf,
   L"%-40d#Выводить уравнения, разрешенные относительно ускорений: 1 - да, 0 - нет\n",0);
  SLo->Add(Inf);
  swprintf(Inf,
   L"%-40d#Выводить уравнения с подстановкой значений переменных: 1 - да, 0 - нет\n",0);
  SLo->Add(Inf);
  swprintf(Inf,
   L"%-40d#Закладки графиков показывать в один ряд: 1 - да, 0 - нет\n",
   !TabControl->MultiLine);
  SLo->Add(Inf);
  swprintf(Inf,L"%-40g#Коэффициет для выравнивания масштабов по осям\n",KEqvScal);
  SLo->Add(Inf);
  swprintf(Inf,L"%-40d#Печатать значения справа и сверху графика: 1 - да, 0 - нет\n",
   PRNTOPRIGHT);
  SLo->Add(Inf);
  swprintf(Inf,L"%-40d#Печатать заголовок графика: 1 - да, 0 - нет\n",PRINTHEAD);
  SLo->Add(Inf);
  swprintf(Inf,L"%-40d#Число столбцов для формул переменных\n",KColumn); SLo->Add(Inf);
  swprintf(Inf,L"%-40d#Выводить в отчет кинетическую энергию: 1 - да, 0 - нет\n",
   VISIBLEKINETICENERGY);
  SLo->Add(Inf);
  swprintf(Inf,L"%-40d#Выводить в отчет диссипативную функцию: 1 - да, 0 - нет\n",
   VISIBLEDISSIPFUNCTION);
  SLo->Add(Inf);
  swprintf(Inf,L"%-40d#Выводить в отчет потенциальную энергию: 1 - да, 0 - нет\n",
   VISIBLEPOTENENERGY);
  SLo->Add(Inf);
  swprintf(Inf,L"%-40d#Выводить в отчет функцию Аппеля: 1 - да, 0 - нет\n",
   VISIBLEAPPELFUNCTION);
  SLo->Add(Inf);
  SaveToFileDOS(SLo,OptFile);
  ChangeOpt=false;
}}
//------------------------------------------------------------------------
bool WhiteSpace(wchar_t B){
 if(B==L' '||B==L'\t'||B==L'\n'||B==L'\v') return true;
 return false;
}
//------------------------------------------------------------------------
bool Here(wchar_t *t,wchar_t *Word){
 int L=wcslen(Word); wchar_t B=t[L],Test[256]; t[L]=L'\0';
 wcscpy(Test,t); t[L]=B;
 return !wcscmp(LowToUpp(Test),Word);
}
//------------------------------------------------------------------------
bool LoadFromFileDOS(TRichEdit *R,wchar_t *File){
 wchar_t ANSI[256],*s,Exec[256],S[256]; int Rez;
 wcscpy(ANSI,File);
 if(s=wcsrchr(ANSI,'.')) wcscpy(s,L".ansi");
 swprintf(Exec,L"%s\\ConvertASCIItoANSI.exe %s",WORKDIR,File);
 Rez=WinExec(AnsiString(Exec).c_str(),0);
// Rez=_wspawnl(P_WAIT,Exec,Exec,File.w_str(),NULL);
 if(Rez>31){
  R->Lines->LoadFromFile(ANSI);
  if(R->Lines->Count){ R->SelStart=0; R->SelLength=0; }
 }
 _wunlink(ANSI);
 return Rez>31;
}
//------------------------------------------------------------------------
__fastcall TFormKinema::TFormKinema(TComponent* Owner):TForm(Owner){
 time_t timer; struct tm *tblock; FILE *F;
 int Kinst=0,FigSk=0,L,T,W,H,L1,L2;
 wchar_t *s,*t,*n,B,*Calc1,*Calc2,*End,ConvExe[256];
 timer=time(NULL); tblock=localtime(&timer);
 NYear=tblock->tm_year; Reader=Gamer=false;
 TRegistry *reg=new TRegistry(KEY_READ); reg->RootKey=HKEY_CLASSES_ROOT;
 if(reg->OpenKey("Kidym\\shell\\open\\command",false)){
  String EXE=reg->ReadString(L"");
  if(!EXE.IsEmpty()){
   wcscpy(WORKDIR,EXE.w_str());
   if(s=wcsrchr(WORKDIR,'\\')) *s='\0';
 }}
 FormSizes(L,T,W,H); Left=L; Top=T; Width=W; Height=H;
 ToolButtonPrint->Enabled=Printer()->Printers->Count>0;
 wcscpy(HeadCalc,L"Кинематика");
 if((s=wcsrchr(InpFile,'/'))||(s=wcsrchr(InpFile,'\\'))){
  B=*s; *s='\0'; wcscpy(CurrDir,InpFile); *s=B;
 }
 else _wgetcwd(CurrDir,255);
// wcscpy(ANSIFile,InpFile);
// if(s=wcsrchr(ANSIFile,L'.')) wcscpy(s+1,L"ansi");
// swprintf(ConvExe,L"%s\\ASCIItoANSI.exe %s",WORKDIR,InpFile);
// WinExec(AnsiString(ConvExe).c_str(),0);
// RichEditInp->Lines->LoadFromFile(ANSIFile);
 if(RichEditInp->Lines->Count) RichEditInp->Clear();
 RichEditInp->Visible=false;
 if(F=_wfopen(InpFile,L"r")){ wchar_t Cod[8];
  for(int i=0;i<5;i++) Inf[i]=fgetwc(F); Inf[5]='\0';
  fclose(F);
  wcsncpy(Cod,Inf+2,3); Cod[3]='\0';
  UNICOD=!wcscmp(Cod,L"rtf");
  if(UNICOD)
   RichEditInp->Lines->LoadFromFile(InpFile);
  else
   LoadFromFileDOS(RichEditInp,InpFile);
 }
 SLM=new TStringList;
 SLM->Assign(RichEditInp->Lines);
// for(int i=0;i<RichEditInp->Lines->Count;i++)
//  SLM->Add(RichEditInp->Lines->Strings[i]);
 RichEditInp->Clear();
// RichEditInp->Visible=true;
// Lfile=RichEditInp->Text.Length();
 End=TextFromFile(WORKDIR,L"EditKiDyM",&SLK,112);
 Calc1=TextFromFile(WORKDIR,L"EditKiDyM",&SLK,111); L1=wcslen(Calc1);
 Calc2=TextFromFile(WORKDIR,L"EditKiDyM",&SLK,159); L2=wcslen(Calc2);
 for(t=SLM->Text.w_str();*t;t++){
  if(Here(t,End))
   break;
  if(Here(t,Calc1)){
   t+=wcslen(Calc1);
   while(*t&&WhiteSpace(*t)) t++;
   if(*t==':'){
	for(t++;*t&&WhiteSpace(*t);t++);
	if(*t=='=') Kinst++;
	for(t++;*t&&WhiteSpace(*t);t++);
	for(n=Inf;*t&&(*t!=L';');*n++=*t++); *n=L'\0';
	CleanStr(Inf,NameCalc);
  }}
  if(Here(t,Calc2)){
   t+=wcslen(Calc2);
   while(*t&&WhiteSpace(*t)) t++;
   if(*t==':'){
	for(t++;*t&&WhiteSpace(*t);t++);
	if(*t=='=') Kinst++;
	for(t++;*t&&WhiteSpace(*t);t++);
	for(n=Inf;*t&&(*t!=L';');*n++=*t++); *n=L'\0';
	CleanStr(Inf,NameCalc);
  }}
  B=*t;
  if(B==L'#'){
   for(t++,B=*t;*t&&(B!=L'#');t++)
	B=*t;
   if(!(*t)) break;
  }
  else if(B==L'{'){
   FigSk++;
   for(t++,B=*t;*t&&FigSk;t++){
	B=*t;
	if(B==L'{') FigSk++; else if(B==L'}') FigSk--;
   }
   if(!(*t)) break;
 }}
 INVERSE=wcscmp(NameCalc,TextFromFile(WORKDIR,L"EditKiDyM",&SLK,197));
 if(t=wcsrchr(InpFile,'.')) *t='\0';
 if(Kinst>1){
  swprintf(ErrFile,L"%s№%d.err",InpFile,Kinst);
  swprintf(OutFile,L"%s№%d.out",InpFile,Kinst);
  swprintf(OprFile,L"%s№%d.opr",InpFile,Kinst);
  swprintf(OptFile,L"%s№%d.opt",InpFile,Kinst);
  swprintf(HtmFile,L"%s№%d.htm",InpFile,Kinst);
  swprintf(DgsFile,L"%s№%d.dgs",InpFile,Kinst);
 }
 else{
  swprintf(ErrFile,L"%s.err",InpFile);
  swprintf(OutFile,L"%s.out",InpFile);
  swprintf(OprFile,L"%s.opr",InpFile);
  swprintf(OptFile,L"%s.opt",InpFile);
  swprintf(HtmFile,L"%s.htm",InpFile);
  swprintf(DgsFile,L"%s.dgs",InpFile);
 }
 if(t) *t='.';
 if(!_waccess(ErrFile,0)) _wunlink(ErrFile);
 if(!_waccess(DgsFile,0)) _wunlink(DgsFile);
 if(!_waccess(OutFile,0)) _wunlink(OutFile);
 if(!_waccess(OprFile,0)) _wunlink(OprFile);
 if(!_waccess(HtmFile,0)) _wunlink(HtmFile);
 FileToOpt();
// RichEditInp->Clear();
 InitCA();
 RichEditInp->Font->Name=NameFontEd; RichEditInp->Font->Size=SizeFontEd;
 RichEditInp->Font->Color=ColorFontEd;
 RichEditInp->Font->Style=ExFontEd?RichEditInp->Font->Style<<fsBold:
								   RichEditInp->Font->Style>>fsBold;
 RichEditInp->Font->Style=ItFontEd?RichEditInp->Font->Style<<fsItalic:
								   RichEditInp->Font->Style>>fsItalic;
 RichEditInp->Font->Style=UnFontEd?RichEditInp->Font->Style<<fsUnderline:
								   RichEditInp->Font->Style>>fsUnderline;
 RichEditInp->Align=alClient; RichEditInp->Color=ColorPhonEd;
 FontDialog->Font->Name=NameFont; FontDialog->Font->Size=SizeFont;
 FontDialog->Font->Color=ColorFont;
 FontDialog->Font->Style=ExFont?FontDialog->Font->Style<<fsBold:
								FontDialog->Font->Style>>fsBold;
 FontDialog->Font->Style=ItFont?FontDialog->Font->Style<<fsItalic:
								FontDialog->Font->Style>>fsItalic;
 FontDialog->Font->Style=UnFont?FontDialog->Font->Style<<fsUnderline:
								FontDialog->Font->Style>>fsUnderline;
 Caption=INVERSE?"Обратная задача кинематики: ":"Кинематика: ";
 if(InpFile[0]){ wchar_t *inf;
  swprintf(Inf,InpFile); inf=wcsrchr(Inf,'.'); inf+=swprintf(inf,L".kdm");
//  Kinst=KolInst(TextFromFile(WORKDIR,L"EditKiDyM",&SLK,111))+
//   KolInst(TextFromFile(WORKDIR,L"EditKiDyM",&SLK,159));
  if(Kinst>1) swprintf(inf,L" (расчет №%d)",Kinst);
  Caption=Caption+Inf;
  wcscpy(Inf,(s=wcsrchr(InpFile,'\\'))?(s+1):InpFile);
  if(t=wcsrchr(Inf,'.')) wcscpy(t,L".KDM"); else wcscat(Inf,L".KDM");
//  Application->Title=Inf;
 }
 MOperators->Visible=false;
 TabControl->Align=alClient;
 StatusBar->Width=Panel->ClientWidth-203; CGauge->ShowText=false;
 TabControl->Canvas->Brush->Color=clWindow;
 TimerModel->Enabled=true;
}
//------------------------------------------------------------------------
void __fastcall TFormKinema::FormClose(TObject *Sender, TCloseAction &Action){
 OptToFile(); RESIZE=false;
 DelAllMagazine(&NamesBMP); DelAllMagazine(&NamesTAB);
 DelAllMagazine(&NamesInclude);
 TabControl->TabIndex=-1;
}
//---------------------------------------------------------------------------
void __fastcall TFormKinema::ToolButtonExitClick(TObject *Sender){
 OptToFile(); RESIZE=false;
 DelAllMagazine(&NamesBMP); DelAllMagazine(&NamesTAB);
 DelAllMagazine(&NamesInclude);
 TabControl->TabIndex=-1;
 Application->Terminate();
}
//---------------------------------------------------------------------------
void __fastcall TFormKinema::mColorsClick(TObject *Sender){
 bool Prsp=Perspect;
 if(!NameZ.IsEmpty()) Timer3D->Enabled=false;
 Application->CreateForm(__classid(TFormColors),&FormColors);
 FormColors->GetColors(&ColorHead,&ColorNameOs,&ColorTabl,&ColorGrid,
  &ColorCifrGrid,&ColorRamka,&ColorPhon,&ColorGraph,&ColorGraph2,
  &ColorGraph3,&ColorGraph4,&ColorGraph5,RichEditInp->Font,&ColorPrint,
  &ColorBMP,&Perspect);
 ToolButtonToBMP->Enabled=true; DelAllMagazine(&NamesBMP);
 if(FormColors->ShowModal()){
  if(FormColors->ModalResult>0){ ChangeOpt=true; OptToFile(); }
 }
 if(NameZ.IsEmpty()){
  if((FormColors->ModalResult>0)&&(TabControl->TabIndex>=2*(!_waccess(OprFile,0))))
   TabControlChange(Sender);
 }
 else{
  if((FormColors->ModalResult>0)&&(Prsp!=Perspect)){
   Perspect=Prsp;
   DrawCurve3D(Image->Canvas,ColorPhon);
   DrawBar3D(Image->Canvas,false);
   DrawVectors(Image->Canvas,false);
   Perspect=!Prsp;
  }
  TabControlChange(NULL);
}}
//---------------------------------------------------------------------------
void __fastcall TFormKinema::mCountTablClick(TObject *Sender){
 GroupBox->Caption=L" Дискретность таблиц ";
 UpDownCountTab->Min=1;
 if(CountTabl) UpDownCountTab->Max=CountTabl;
 else if(Wg) UpDownCountTab->Max=Wg;
 DiscretTyp=COUNTTABL;
 GroupBox->Height=86; RadioGroupSec->Visible=false; GroupBox->Visible=true;
 EditCountTab->Text=UpDownCountTab->Position;
 EditCountTab->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TFormKinema::ButtonOKCountTabClick(TObject *Sender){
 wchar_t Buf[256],*b=Buf,*s=Inf;
 int N=KolElem(Names),i,Val=UpDownCountTab->Position,Kt;
 wcscpy(Inf,EditCountTab->Text.c_str());
 for(;*s;s++) if(int(*s)!=160) *b++=*s; *b='\0';
 switch(DiscretTyp){
  case COUNTTABL:
   if(CountTabl!=Val){
	CountTabl=Val;
	DelAllMagazine(&NamesTAB);
	DelAllMagazine(&NamesSpline);
	DelAllMagazine(&NamesFurie);
	tbSpline->Enabled=true;
	tbFurie->Enabled=true;
	ToolButtonTable->Enabled=true;
	ToolButtonInclude->Enabled=true;
	ChangeOpt=true;
	TabControlChange(NULL);
   }
   break;
  case WIDTHPEN:
   if(WidthPen!=Val){
    WidthPen=Val;
    if(TabControl->TabIndex>(1+!_waccess(OprFile,0)))
     TabControlChange(Sender);
	ToolButtonToBMP->Enabled=true; ToolButtonInclude->Enabled=true;
    DelAllMagazine(&NamesTAB); DelAllMagazine(&NamesInclude);
    ChangeOpt=true;
   }
   break;
  case TMULT:
   switch(RadioGroupSec->ItemIndex){
    case 0: TMult=(long)Val*1000;     break; //сек
    case 1: TMult=(long)Val;          break; //миллисек
   }
   if(!TMult) TMult=(long)(Tk-Tn)*1000;
   ChangeOpt=true;
   if(Qmf){ for(i=0;i<N;i++) free(Qmf[i]); free(Qmf); Qmf=NULL; }
   break;
  case KKADR:
   if(Kkadr!=Val){
	Kkadr=Val; if(!Kkadr) Kkadr=25;
    if(Qmf){ for(i=0;i<N;i++) free(Qmf[i]); free(Qmf); Qmf=NULL; }
    ChangeOpt=true;
   }
   break;
  case KBMP:
   if(Round(Kbmp)!=Val){
    Kbmp=0.01*Val; if(!Kbmp) Kbmp=5.0/4.0;
	ToolButtonToBMP->Enabled=true; DelAllMagazine(&NamesBMP);
    ChangeOpt=true;
   }
   break;
  case KCOLUMN:
   if(UpDownCountTab->Position!=KColumn){
    KColumn=UpDownCountTab->Position; if(!KColumn) KColumn=3;
    Application->MessageBox(L"Опция будет действовать при следующих расчетах!",
     L"Внимание!",MB_OK|MB_ICONWARNING);
    ChangeOpt=true;
   }
   break;
 }
 if((DiscretTyp==TMULT)||(DiscretTyp==KKADR)){
  Kt=TMult*Kkadr/1000+1;
  for(Trajectory *tr=Rtraek;tr;tr=tr->Sled){
   if(Kt>tr->Kt){
	tr->X=(int *)realloc(tr->X,Kt*SzI);
	tr->Y=(int *)realloc(tr->Y,Kt*SzI);
	tr->x=(double *)realloc(tr->x,Kt*SzD);
	tr->y=(double *)realloc(tr->y,Kt*SzD);
   }
   tr->Kt=Kt;
 }}
 if(ChangeOpt) OptToFile();
 GroupBox->Visible=false;
}
//---------------------------------------------------------------------------
void __fastcall TFormKinema::FontDialogApply(TObject *Sender, HWND Wnd){
 RichEditInp->Font=FontDialog->Font;
// RichEditOpr->Font=FontDialog->Font;
 ColorFont=FontDialog->Font->Color;
 TabControl->Canvas->Font=FontDialog->Font;
 NameFont=FontDialog->Font->Name;
 SizeFont=FontDialog->Font->Size;
 ExFont=FontDialog->Font->Style.Contains(fsBold);
 ItFont=FontDialog->Font->Style.Contains(fsItalic);
 UnFont=FontDialog->Font->Style.Contains(fsUnderline);
 if(TabControl->TabIndex>(1+!_waccess(OprFile,0))) TabControlChange(Sender);
 ChangeOpt=true; OptToFile();
}
//---------------------------------------------------------------------------
void __fastcall FontToRichEdit(TRichEdit *RichEdit,TFont *Font){
 int SelStart,SelLength; bool Visible=RichEdit->Visible;
 if(RichEdit->Visible) RichEdit->Visible=false;
 RichEdit->Font->Assign(Font);
 SelStart=RichEdit->SelStart;
 SelLength=RichEdit->SelLength;
 RichEdit->SelectAll();
 RichEdit->SelAttributes->Assign(RichEdit->DefAttributes);
 RichEdit->SelStart=SelStart;
 RichEdit->SelLength=SelLength;
 if(Visible) RichEdit->Visible=true;
}
//---------------------------------------------------------------------------
void __fastcall TFormKinema::mFontClick(TObject *Sender){
 if(FontDialog->Execute()){
  FontDialogApply(Sender,0);
  ToolButtonToBMP->Enabled=true; DelAllMagazine(&NamesBMP);
  ChangeOpt=true; OptToFile();
  if(TabControl->TabIndex==-1)
   MReportClick(NULL);
}}
//---------------------------------------------------------------------------
struct StrTabl{
 StrTabl *Sled; TColor C; Vary *Vy,*Vx; double Vl,Vr,Vmin,tmin,Vmax,tmax;
} *Tabl;
//---------------------------------------------------------------------------
static int A,B,C,D,E,G,H;
//---------------------------------------------------------------------------
void __fastcall TFormKinema::SelectSizeFont(TCanvas *Canvas){
 TRect Rect=Canvas->ClipRect; StrTabl *t;
 int i,l,m,k=0,W=Rect.Width()-PoleLeft-PoleRight;
 Canvas->Font->Size=SizeFont;
 if(Names) do{
  k++; Canvas->Font->Size=Canvas->Font->Size-1;
  A=Canvas->TextWidth(L"Линия");
  B=Canvas->TextWidth(L"Переменная");
  C=Canvas->TextWidth(L"левая");
  swprintf(Inf,L"(t=%g)",Tn);
  if(C<Canvas->TextWidth(Inf)) C=Canvas->TextWidth(Inf);
  D=Canvas->TextWidth(L"правая");
  swprintf(Inf,L"(t=%g)",Tk);
  if(D<Canvas->TextWidth(Inf)) D=Canvas->TextWidth(Inf);
  E=Canvas->TextWidth(L"нижняя");
  I=Canvas->TextWidth(L"верхняя");
  G=Canvas->TextWidth(L"Масштаб");
  if(G<Canvas->TextWidth(L"в 1 мм")) G=Canvas->TextWidth(L"в 1 мм");
  for(t=Tabl,i=0;t;i++,t=t->Sled){
   if(B<Canvas->TextWidth(t->Vy->Name)) B=Canvas->TextWidth(t->Vy->Name);
   swprintf(Inf,L"%g",t->Vl);
   if(C<Canvas->TextWidth(Inf)) C=Canvas->TextWidth(Inf);
   swprintf(Inf,L"%g",t->Vr);
   if(D<Canvas->TextWidth(Inf)) D=Canvas->TextWidth(Inf);
   swprintf(Inf,L"%g/t=%g",t->Vmin,t->tmin);
   if(E<Canvas->TextWidth(Inf)) E=Canvas->TextWidth(Inf);
   swprintf(Inf,L"%g/t=%g",t->Vmax,t->tmax);
   if(I<Canvas->TextWidth(Inf)) I=Canvas->TextWidth(Inf);
   swprintf(Inf,L"%g",MstT[i>1?2:(i+1)]);
   if(G<Canvas->TextWidth(Inf)) G=Canvas->TextWidth(Inf);
  }
  l=Canvas->TextWidth(L"Значения характерных точек");
  while(l>(C+D+E+I)){ C++; D++; E++; I++; }
  H=Canvas->TextWidth(L"Единица");
  if(H<Canvas->TextWidth(L"измерения")) H=Canvas->TextWidth(L"измерения");
  if(H<Canvas->TextWidth(RazmX)) H=Canvas->TextWidth(RazmX);
  m=Canvas->TextWidth(L"M");
  if(k==5) return;
 }while((A+B+C+D+E+I+G+H+9+8*m)>W);
 else if(NameZ.IsEmpty()) do{
  k++;
  A=Canvas->TextWidth(TabControl->Tabs->Strings[TabControl->TabIndex]);
  if(A<Canvas->TextWidth(L"Название оси")) A=Canvas->TextWidth(L"Название оси");
  if(A<Canvas->TextWidth(NameX)) A=Canvas->TextWidth(NameX);
  if(A<Canvas->TextWidth(NameY)) A=Canvas->TextWidth(NameY);
  B=Canvas->TextWidth(L"левая");
  swprintf(Inf,L"%g",Slv[0]);
  if(B<Canvas->TextWidth(Inf)) B=Canvas->TextWidth(Inf);
  swprintf(Inf,L"%g",Slv[1]);
  if(B<Canvas->TextWidth(Inf)) B=Canvas->TextWidth(Inf);
  C=Canvas->TextWidth(L"правая");
  swprintf(Inf,L"%g",Spr[0]);
  if(C<Canvas->TextWidth(Inf)) C=Canvas->TextWidth(Inf);
  swprintf(Inf,L"%g",Spr[1]);
  if(C<Canvas->TextWidth(Inf)) C=Canvas->TextWidth(Inf);
  D=Canvas->TextWidth(L"нижняя");
  swprintf(Inf,L"%g",Min[0]);
  if(D<Canvas->TextWidth(Inf)) D=Canvas->TextWidth(Inf);
  swprintf(Inf,L"%g",Min[1]);
  if(D<Canvas->TextWidth(Inf)) D=Canvas->TextWidth(Inf);
  E=Canvas->TextWidth(L"верхняя");
  swprintf(Inf,L"%g",Max[0]);
  if(E<Canvas->TextWidth(Inf)) E=Canvas->TextWidth(Inf);
  swprintf(Inf,L"%g",Max[1]);
  if(E<Canvas->TextWidth(Inf)) E=Canvas->TextWidth(Inf);
  l=Canvas->TextWidth(L"Значения характерных точек");
  while(l>(B+C+D+E)){ B++; C++; D++; E++; }
  I=Canvas->TextWidth(L"Масштаб");
  if(I<Canvas->TextWidth(L"в 1 мм")) I=Canvas->TextWidth(L"в 1 мм");
  swprintf(Inf,L"%g",MstT[0]);
  if(I<Canvas->TextWidth(Inf)) I=Canvas->TextWidth(Inf);
  swprintf(Inf,L"%g",MstT[1]);
  if(I<Canvas->TextWidth(Inf)) I=Canvas->TextWidth(Inf);
  G=Canvas->TextWidth(L"Единица");
  if(G<Canvas->TextWidth(L"измерения")) G=Canvas->TextWidth(L"измерения");
  if(G<Canvas->TextWidth(RazmX)) G=Canvas->TextWidth(RazmX);
  m=Canvas->TextWidth(L"M");
  if(k==5) return;
 }while((A+B+C+D+E+I+G+8+7*m)>W);
 else do{//3D
  k++;
  A=Canvas->TextWidth(NameZ);
  if(A<Canvas->TextWidth(L"Название оси")) A=Canvas->TextWidth(L"Название оси");
  if(A<Canvas->TextWidth(NameX)) A=Canvas->TextWidth(NameX);
  if(A<Canvas->TextWidth(NameY)) A=Canvas->TextWidth(NameY);
  B=Canvas->TextWidth(L"ближняя");
  swprintf(Inf,L"%g",Ner[0]);
  if(B<Canvas->TextWidth(Inf)) B=Canvas->TextWidth(Inf);
  swprintf(Inf,L"%g",Ner[1]);
  if(B<Canvas->TextWidth(Inf)) B=Canvas->TextWidth(Inf);
  swprintf(Inf,L"%g",Ner[2]);
  if(B<Canvas->TextWidth(Inf)) B=Canvas->TextWidth(Inf);
  C=Canvas->TextWidth(L"дальняя");
  swprintf(Inf,L"%g",Far[0]);
  if(C<Canvas->TextWidth(Inf)) C=Canvas->TextWidth(Inf);
  swprintf(Inf,L"%g",Far[1]);
  if(C<Canvas->TextWidth(Inf)) C=Canvas->TextWidth(Inf);
  swprintf(Inf,L"%g",Far[2]);
  if(C<Canvas->TextWidth(Inf)) C=Canvas->TextWidth(Inf);
  D=Canvas->TextWidth(L"левая");
  swprintf(Inf,L"%g",Slv[0]);
  if(D<Canvas->TextWidth(Inf)) D=Canvas->TextWidth(Inf);
  swprintf(Inf,L"%g",Slv[1]);
  if(D<Canvas->TextWidth(Inf)) D=Canvas->TextWidth(Inf);
  swprintf(Inf,L"%g",Slv[2]);
  if(D<Canvas->TextWidth(Inf)) D=Canvas->TextWidth(Inf);
  E=Canvas->TextWidth(L"правая");
  swprintf(Inf,L"%g",Spr[0]);
  if(E<Canvas->TextWidth(Inf)) E=Canvas->TextWidth(Inf);
  swprintf(Inf,L"%g",Spr[1]);
  if(E<Canvas->TextWidth(Inf)) E=Canvas->TextWidth(Inf);
  swprintf(Inf,L"%g",Spr[1]);
  if(E<Canvas->TextWidth(Inf)) E=Canvas->TextWidth(Inf);
  G=Canvas->TextWidth(L"нижняя");
  swprintf(Inf,L"%g",Low[0]);
  if(G<Canvas->TextWidth(Inf)) G=Canvas->TextWidth(Inf);
  swprintf(Inf,L"%g",Low[1]);
  if(E<Canvas->TextWidth(Inf)) E=Canvas->TextWidth(Inf);
  swprintf(Inf,L"%g",Low[2]);
  if(E<Canvas->TextWidth(Inf)) E=Canvas->TextWidth(Inf);
  H=Canvas->TextWidth(L"верхняя");
  swprintf(Inf,L"%g",Hig[0]);
  if(H<Canvas->TextWidth(Inf)) H=Canvas->TextWidth(Inf);
  swprintf(Inf,L"%g",Hig[1]);
  if(H<Canvas->TextWidth(Inf)) H=Canvas->TextWidth(Inf);
  swprintf(Inf,L"%g",Hig[2]);
  if(H<Canvas->TextWidth(Inf)) H=Canvas->TextWidth(Inf);
  l=Canvas->TextWidth(L"Значения характерных точек");
  I=Canvas->TextWidth(L"Масштаб");
  if(I<Canvas->TextWidth(L"в 1 мм")) I=Canvas->TextWidth(L"в 1 мм");
  swprintf(Inf,L"%g",MstT[0]);
  if(I<Canvas->TextWidth(Inf)) I=Canvas->TextWidth(Inf);
  J=Canvas->TextWidth(L"Единица");
  if(J<Canvas->TextWidth(L"измерения")) J=Canvas->TextWidth(L"измерения");
  m=Canvas->TextWidth(L"M");
  if(k==5) return;
 }while((A+B+C+D+E+H+G+I+J+10+9*m)>W);
}
//---------------------------------------------------------------------------
void __fastcall TFormKinema::Metrica(TCanvas *Canvas,Magazine *Names){
 TRect Rect=Canvas->ClipRect; Magazine *m;
 int W=Rect.Width(),H=Rect.Height(),i,j,Ps,LPx,LPy,Wgmm,Hgmm,HorzSize,VertSize;
 HorzSize=GetDeviceCaps(Canvas->Handle,HORZSIZE); //mm
 VertSize=GetDeviceCaps(Canvas->Handle,VERTSIZE); //mm
 LPx=GetDeviceCaps(Canvas->Handle,LOGPIXELSX);
 LPy=GetDeviceCaps(Canvas->Handle,LOGPIXELSY);
 Canvas->Font->Size=SizeFont;
 h=Round(6.0*Canvas->TextHeight(L"M")/5.0);
 if(PRINTHEAD){
  Canvas->Font->Size=SizeFont*3/2;
  while(Canvas->TextWidth(Head)>W)
   Canvas->Font->Size=Canvas->Font->Size-1;
  Hh=Round(6.0*Canvas->TextHeight(L"M")/5.0);
  Canvas->Font->Size=SizeFont;
 }
 else Hh=h/2;
 Ht=!PRINTABL?0:
  Names?(2+KolElem(Names))*(h+1):NameZ.IsEmpty()?(4*h+5):(5*h+6);
 swprintf(Inf,L"%d",TabControl->TabIndex);
 Wo=Canvas->TextWidth(L"000.000E+00"); Wu=PRNTOPRIGHT?Wo:0;
 if(NameZ.IsEmpty()){ Hy=h; /*PRNTOPRIGHT?h:PRINTABL?(h/2):0;*/ Hx=h; }
 else Hy=Hx=0;
 Hc=h;
 Wg=W-PoleLeft-PoleRight-Wo-Wu; Wgmm=Wg*25.4/LPx;
 if(VertSize>HorzSize) Hg=H/2;//принтер, книжная ориентация
 else Hg=H;//экран или принтер, альбомная ориентация
 Hg-=PoleTop+PoleBottom+Hh+Ht+Hy+Hx;
 Hgmm=Hg*25.4/LPx; KP[0]=1+Wgmm/Sg; KP[1]=1+Hgmm/Sg;
 dS[0]=Round((double)(Sg*LPx)/25.4);
 dS[1]=Round((double)(Sg*LPy)/25.4);
 Xk=PoleLeft+Wo; Yk=PoleTop+Hh+Ht+Hy+Hg-5;
}
//---------------------------------------------------------------------------
void __fastcall TFormKinema::DrawHead(TCanvas *Canvas,String Head){
 TRect Rect=Canvas->ClipRect; TColor Color=Canvas->Font->Color;
 int W=Rect.Width()-PoleLeft-PoleRight,H=Rect.Height(),
  Size=Canvas->Font->Size,Back=0;
 if(!Prntr||(Prntr->Canvas!=Canvas)) Canvas->Rectangle(0,0,W-1,H-1);
 if(Prntr&&(Canvas==Prntr->Canvas)&&!ColorPrint||
	ImageBMP&&(Canvas==ImageBMP->Canvas)&&!ColorBMP)
  Canvas->Font->Color=clBlack;
 else Canvas->Font->Color=ColorHead;
 if(!Canvas->Font->Style.Contains(fsBold)){
  Canvas->Font->Style=Canvas->Font->Style<<fsBold; Back=1;
 }
 Canvas->Font->Size=Size*3/2;
 while(Canvas->TextWidth(Head)>W)
  Canvas->Font->Size=Canvas->Font->Size-1;
 DrawGrekText(Canvas,PoleLeft,PoleTop+Hh/18,W,Head.w_str(),SimbInd);
 Canvas->Font->Size=Size;
 if(Back) Canvas->Font->Style=Canvas->Font->Style>>fsBold;
 Canvas->Font->Color=Color;
 Canvas->MoveTo(0,0); Canvas->LineTo(W-1,0);
}
//---------------------------------------------------------------------------
bool __fastcall TFormKinema::DefMSTGraf(){
 double dt=(Tk-Tn)/(Wg-1),t=Tn,v; Form Fp; int Ps;
//Найдем характерные точки
 Max[1]=Spr[1]=-MAXDOUBLE; Min[1]=Slv[1]=MAXDOUBLE;
 Max[0]=Slv[0]=Min[0]=Tn; Spr[0]=Tk;
 StatusBar->SimpleText=L"Расчет масштабов";
 CGauge->Progress=0; CGauge->MaxValue=Wg;
 for(int i=0;i<Wg;i++){
  t=Tn+i*dt; ClearVar(); L->Time->Val=t;
  v=Func(t); if(!i) Slv[1]=v;
  CGauge->Progress++;
  if(NoCalc){ Tend=t; return false; }
  if(v>Max[1]){ Max[1]=v; Max[0]=t; }
  if(v<Min[1]){ Min[1]=v; Min[0]=t; }
 }
 CGauge->Progress=0;
 swprintf(Inf,L"%s: %s",HeadVersion[0]?HeadVersion:HeadWork,Autor);
 StatusBar->SimpleText=Inf;
 Spr[1]=v;
// if(NoCalc) return false;
 if(v>Max[1]){ Max[1]=v; Max[0]=t; }
 if(v<Min[1]){ Min[1]=v; Min[0]=t; }
//Найдем масштабы
 if((Spr[0]-Slv[0])<1e-5*fabs(Spr[0]+Slv[0])) MSt[0]=0;
 else MSt[0]=(Spr[0]-Slv[0])/(KP[0]-1);
 if(MSt[0]==0.0) MSt[0]=fabs(Spr[0]+Slv[0])/(KP[0]-1);
 if((Max[1]-Min[1])<1e-5*fabs(Max[1]+Min[1])) MSt[1]=0;
 else MSt[1]=(Max[1]-Min[1])/(KP[1]-1);
 if(MSt[1]==0.0) MSt[1]=fabs(Max[1]+Min[1])/(KP[1]-1);
 if(MSt[1]==0.0) MSt[1]=MSt[0];
 if((MSt[0]==0.0)||(MSt[1]==0.0)) return false;
 for(int i=0;i<2;i++){
  for(Ps=0;MSt[i]>10.0;Ps++) MSt[i]/=10.0;
  for(;MSt[i]<1.0;Ps--) MSt[i]*=10.0;
  for(int j=1;j<Kmg;j++) if(MSt[i]<=MG[j]) { MSt[i]=MG[j]; break; }
  for(int j=abs(Ps);j--;) if(Ps>0) MSt[i]*=10;
  else MSt[i]/=10;
  MstT[i]=MSt[i]/Sg; MSt[i]/=dS[i];
 }
 return true;
}
//---------------------------------------------------------------------------
bool __fastcall TFormKinema::DefMSTGraf(Magazine *Names){
 double dt=(Tk-Tn)/(Wg-1),t,v,x; Magazine *m; wchar_t *s,*s0;
 int i,K=KolElem(Names),SOVM=0,Ps;
 if(Names){
  if(s=wcschr(Names->S,'(')){
   GodoGraf=true;
   s++; if(s0=wcschr(s,')')) *s0='\0'; VgX=TakeVary(s); if(s0) *s0=')';
  }
  else GodoGraf=false;
  swprintf(Inf,L"%d",TabControl->TabIndex);
  if(FindMagazine(Sovm,Inf,&m)) SOVM=1;
 }
//Найдем характерные точки
 Tabl=(StrTabl *)calloc(K,sizeof(StrTabl));
 for(i=0;i<K-1;i++) Tabl[i].Sled=Tabl+i+1;
 Tabl[0].C=ColorGraph;
 if(K>4) Tabl[4].C=ColorGraph5; if(K>3) Tabl[3].C=ColorGraph4;
 if(K>2) Tabl[2].C=ColorGraph3; if(K>1) Tabl[1].C=ColorGraph2;
 for(m=Names,i=0;m;m=m->Sled,i++){
  if(s=wcschr(m->S,'(')) *s='\0';
  FindVary(m->S,&Vp); Tabl[i].Vy=Vp;
  if(s){
   *s++='(';
   if(s0=wcschr(s,')')){ *s0='\0'; FindVary(s,&Vp); Tabl[i].Vx=Vp; *s0=')'; }
  }
  Tabl[i].Vmin=MAXDOUBLE; Tabl[i].Vmax=-MAXDOUBLE;
 }
 if(GodoGraf){ Min[0]=Spr[0]=-MAXDOUBLE; Max[0]=Slv[0]=MAXDOUBLE; }
 else{ Spr[0]=Tk; Slv[0]=Tn; }
 Max[1]=Spr[1]=-MAXDOUBLE; Min[1]=Slv[1]=MAXDOUBLE;
 Max[2]=-MAXDOUBLE; Min[2]=MAXDOUBLE;
 for(int j=0;j<Wg;j++){
  t=Tn+j*dt; ClearVar(); L->Time->Val=t;
  if(GodoGraf){
   x=FuncX(0,t); if(x<Slv[0]) Slv[0]=x; if(x>Spr[0]) Spr[0]=x;
  }
  if(NoCalc){ Tend=t; return false; }
  Vp=Tabl[0].Vy; v=Func(t);
  if(NoCalc){ Tend=t; return false; }
  if(v<Tabl[0].Vmin){ Tabl[0].Vmin=v; Tabl[0].tmin=GodoGraf?x:t; }
  if(v>Tabl[0].Vmax){ Tabl[0].Vmax=v; Tabl[0].tmax=GodoGraf?x:t; }
  if(!j) Tabl[0].Vl=v; else if(j==Wg-1) Tabl[0].Vr=v;
  if(v>Max[1]) Max[1]=v; if(v<Min[1]) Min[1]=v;
  for(m=Names->Sled,i=1;m;m=m->Sled,i++){
   if(Vp=Tabl[i].Vx){
    x=Func(t); if(x<Slv[0]) Slv[0]=x; if(x>Spr[0]) Spr[0]=x;
   }
   Vp=Tabl[i].Vy; v=Func(t);
   if(NoCalc){ Tend=t; return false; }
   if(v<Tabl[i].Vmin){ Tabl[i].Vmin=v; Tabl[i].tmin=GodoGraf?x:t; }
   if(v>Tabl[i].Vmax){ Tabl[i].Vmax=v; Tabl[i].tmax=GodoGraf?x:t; }
   if(!j) Tabl[i].Vl=v; else if(j==Wg-1) Tabl[i].Vr=v;
   if(v>Max[1+SOVM]) Max[1+SOVM]=v; if(v<Min[1+SOVM]) Min[1+SOVM]=v;
 }}
//Найдем масштабы
 MSt[0]=(Spr[0]-Slv[0])/(KP[0]-1);
 if((Max[1]-Min[1])<1e-5*fabs(Max[1]+Min[1])) MSt[1]=0;
 else MSt[1]=(Max[1]-Min[1])/(KP[1]-1-SOVM);
 if(MSt[1]==0.0) MSt[1]=fabs(Max[1]+Min[1])/(KP[1]-1-SOVM);
 if(MSt[1]==0.0) MSt[1]=MSt[0];
 if((MSt[0]==0.0)||(MSt[1]==0.0)) return false;
 if(SOVM){
  if((Max[2]-Min[2])<1e-5*fabs(Max[2]+Min[2])) MSt[2]=0;
  else MSt[2]=(Max[2]-Min[2])/(KP[1]-2);
  if(MSt[2]==0.0) MSt[2]=fabs(Max[2]+Min[2])/(KP[1]-2);
  if(MSt[2]==0.0) MSt[2]=MSt[1];
 }
 for(int i=0;i<2+SOVM;i++){
  for(Ps=0;MSt[i]>10.0;Ps++) MSt[i]/=10.0;
  for(;MSt[i]<1.0;Ps--) MSt[i]*=10.0;
  for(int j=1;j<Kmg;j++) if(MSt[i]<=MG[j]){ MSt[i]=MG[j]; break; }
  for(int j=abs(Ps);j--;) if(Ps>0) MSt[i]*=10; else MSt[i]/=10;
  MstT[i]=MSt[i]/Sg; MSt[i]/=dS[i==2?1:i];
 }
 if(!SOVM) MstT[2]=MstT[1];
 return true;
}
//---------------------------------------------------------------------------
void __fastcall TFormKinema::DrawTabl(TCanvas *Canvas,Magazine *Names){
 TRect Rect=Canvas->ClipRect; StrTabl *t; bool SOVM=false; Magazine *m;
 int l,W=Rect.Width()-PoleLeft-PoleRight,d1,d2,K=KolElem(Names),d,i,l1,l2,SF;
 TColor Color=Canvas->Font->Color; Form F; Vary *V;
 Canvas->Font->Color=clBlack;
 if(Names){
  swprintf(Inf,L"%d",TabControl->TabIndex);
  if(FindMagazine(Sovm,Inf,&m)) SOVM=true;
  d1=(W-A-B-C-D-E-I-G-H-9)/16; d2=(W-A-B-C-D-E-I-G-H-9-8*d1)/8; A+=d1+d2;
  B+=d1+d2; C+=d1+d2; D+=d1+d2; E+=d1+d2; I+=d1+d2; G+=d1+d2; H+=d1+d2;
  wcscpy(Inf,L"Линия"); l=Canvas->TextWidth(Inf);
  Canvas->TextOut(PoleLeft+1+(A-l)/2,PoleTop+Hh+3+h/2,Inf);
  wcscpy(Inf,L"Переменная"); l=Canvas->TextWidth(Inf);
  Canvas->TextOut(PoleLeft+A+2+(B-l)/2,PoleTop+Hh+3+h/2,Inf);
  wcscpy(Inf,L"Значения координат характерных точек"); l=Canvas->TextWidth(Inf);
  Canvas->TextOut(PoleLeft+A+B+3+(C+D+E+I+3-l)/2,PoleTop+Hh+3,Inf);
  wcscpy(Inf,L"Масштаб"); l=Canvas->TextWidth(Inf);
  Canvas->TextOut(PoleLeft+A+B+C+D+E+I+7+(G-l)/2,PoleTop+Hh+3,Inf);
  wcscpy(Inf,L"Единица"); l=Canvas->TextWidth(Inf);
  Canvas->TextOut(PoleLeft+A+B+C+D+E+I+G+8+(H-l)/2,PoleTop+Hh+3,Inf);
  wcscpy(Inf,L"левая"); l=Canvas->TextWidth(Inf);
  Canvas->TextOut(PoleLeft+A+B+3+(C-l)/2,PoleTop+Hh+h+4,Inf);
  wcscpy(Inf,L"правая"); l=Canvas->TextWidth(Inf);
  Canvas->TextOut(PoleLeft+A+B+C+4+(D-l)/2,PoleTop+Hh+h+4,Inf);
  wcscpy(Inf,L"нижняя"); l=Canvas->TextWidth(Inf);
  Canvas->TextOut(PoleLeft+A+B+C+D+5+(E-l)/2,PoleTop+Hh+h+4,Inf);
  wcscpy(Inf,L"верхняя"); l=Canvas->TextWidth(Inf);
  Canvas->TextOut(PoleLeft+A+B+C+D+E+6+(I-l)/2,PoleTop+Hh+h+4,Inf);
  wcscpy(Inf,L"в 1 мм"); l=Canvas->TextWidth(Inf);
  Canvas->TextOut(PoleLeft+A+B+C+D+E+I+7+(G-l)/2,PoleTop+Hh+h+4,Inf);
  wcscpy(Inf,L"измерения"); l=Canvas->TextWidth(Inf);
  Canvas->TextOut(PoleLeft+A+B+C+D+E+I+G+8+(H-l)/2,PoleTop+Hh+h+4,Inf);
  d=Round(3.0*A/4.0); Canvas->Pen->Width=1;
  for(t=Tabl,i=0;t;t=t->Sled,i++){
   int y=PoleTop+Hh+2*h+5+i+i*h;
   if(Prntr&&(Canvas==Prntr->Canvas)&&!ColorPrint||
      ImageBMP&&(Canvas==ImageBMP->Canvas)&&!ColorBMP){
    Canvas->Pen->Color=clBlack;
    switch(i){
     case 0: Canvas->Pen->Style=psSolid;       break;
     case 1: Canvas->Pen->Style=psDash;        break;
     case 2: Canvas->Pen->Style=psDot;         break;
     case 3: Canvas->Pen->Style=psDashDot;     break;
     case 4: Canvas->Pen->Style=psDashDotDot;  break;
    }
    for(int j=0;j<5;j++){
     Canvas->MoveTo(PoleLeft+3+(A-d)/2,y-2+j+h/2);
     Canvas->LineTo(PoleLeft+3+(A+d)/2,y-2+j+h/2);
   }}
   else{ Canvas->Pen->Color=t->C;
    for(int j=0;j<3;j++){
     Canvas->MoveTo(PoleLeft+3+(A-d)/2,y-1+j+h/2);
     Canvas->LineTo(PoleLeft+3+(A+d)/2,y-1+j+h/2);
   }}
   if(FindVary(t->Vy->Name,&V)){
    F.V=V; if(V->Razm.C!=Pust) FormToStr(V->Razm,&RazmY);
   }
   DrawGrekText(Canvas,PoleLeft+A+2+d1,y-4,0,t->Vy->Name,SimbInd);
   swprintf(Inf,L"%g",t->Vl); l=Canvas->TextWidth(Inf);
   Canvas->TextOut(PoleLeft+A+B+3+C-l-d2,y,Inf);
   swprintf(Inf,L"%g",t->Vr); l=Canvas->TextWidth(Inf);
   Canvas->TextOut(PoleLeft+A+B+C+4+D-l-d2,y,Inf);
   swprintf(Inf,L"%g/",t->Vmin); l1=Canvas->TextWidth(Inf);
   SF=Canvas->Font->Size; Canvas->Font->Size=4*Canvas->Font->Size/5;
   swprintf(Inf,L"t=%g",t->tmin); l2=Canvas->TextWidth(Inf);
   swprintf(Inf,L"%g/",t->Vmin); Canvas->Font->Size=SF;
   Canvas->TextOut(PoleLeft+A+B+C+D+5+E-l1-l2-d2,y,Inf);
   swprintf(Inf,L"t=%g",t->tmin); Canvas->Font->Size=4*Canvas->Font->Size/5;
   Canvas->TextOut(PoleLeft+A+B+C+D+5+E-l2-d2,y+h/4,Inf);
   Canvas->Font->Size=SF;
   swprintf(Inf,L"%g/",t->Vmax); l1=Canvas->TextWidth(Inf);
   SF=Canvas->Font->Size; Canvas->Font->Size=4*Canvas->Font->Size/5;
   swprintf(Inf,L"t=%g",t->tmax); l2=Canvas->TextWidth(Inf);
   swprintf(Inf,L"%g/",t->Vmax); Canvas->Font->Size=SF;
   Canvas->TextOut(PoleLeft+A+B+C+D+E+6+I-l1-l2-d2,y,Inf);
   swprintf(Inf,L"t=%g",t->tmax); Canvas->Font->Size=4*Canvas->Font->Size/5;
   Canvas->TextOut(PoleLeft+A+B+C+D+E+6+I-l2-d2,y+h/4,Inf);
   Canvas->Font->Size=SF;
   swprintf(Inf,L"%g",MstT[SOVM?i+1:1]); l=Canvas->TextWidth(Inf);
   Canvas->TextOut(PoleLeft+A+B+C+D+E+I+7+G-l-d2,y,Inf);
   wcscpy(Inf,RazmY.c_str());
   Canvas->TextOut(PoleLeft+A+B+C+D+E+I+7+d1,PoleTop+Hh+2*h+5,Inf);
  }
  if(Prntr&&(Canvas==Prntr->Canvas)&&!ColorPrint||
     ImageBMP&&(Canvas==ImageBMP->Canvas)&&!ColorBMP)
   Canvas->Pen->Color=clBlack;
  else Canvas->Pen->Color=ColorTabl;
  Canvas->Pen->Width=2;
  Canvas->MoveTo(PoleLeft+1,PoleTop+Hh+1);
  Canvas->LineTo(PoleLeft+1,PoleTop+Hh+(K+2)*h+K+3);
  Canvas->LineTo(PoleLeft+W-3,PoleTop+Hh+(K+2)*h+K+3);
  Canvas->LineTo(PoleLeft+W-3,PoleTop+Hh+1);
  Canvas->LineTo(PoleLeft+1,PoleTop+Hh+1);
  Canvas->MoveTo(PoleLeft+1,PoleTop+Hh+2*h+3);
  Canvas->LineTo(PoleLeft+W-3,PoleTop+Hh+2*h+3);
  Canvas->Pen->Width=1;
  Canvas->MoveTo(PoleLeft+A+B+3,PoleTop+Hh+h+2);
  Canvas->LineTo(PoleLeft+A+B+C+D+E+I+6,PoleTop+Hh+h+2);
  for(int i=0;i<K;i++){
   Canvas->MoveTo(PoleLeft+1,PoleTop+Hh+(3+i)*h+4+i);
   Canvas->LineTo(PoleLeft+W-3,PoleTop+Hh+(3+i)*h+4+i);
  }
  Canvas->MoveTo(PoleLeft+A+2,PoleTop+Hh+2);
  Canvas->LineTo(PoleLeft+A+2,PoleTop+Hh+(K+2)*h+K+3);
  Canvas->MoveTo(PoleLeft+A+B+3,PoleTop+Hh+2);
  Canvas->LineTo(PoleLeft+A+B+3,PoleTop+Hh+(K+2)*h+K+3);
  Canvas->MoveTo(PoleLeft+A+B+C+4,PoleTop+Hh+h+3);
  Canvas->LineTo(PoleLeft+A+B+C+4,PoleTop+Hh+(K+2)*h+K+3);
  Canvas->MoveTo(PoleLeft+A+B+C+D+5,PoleTop+Hh+h+3);
  Canvas->LineTo(PoleLeft+A+B+C+D+5,PoleTop+Hh+(K+2)*h+K+3);
  Canvas->MoveTo(PoleLeft+A+B+C+D+E+6,PoleTop+Hh+h+3);
  Canvas->LineTo(PoleLeft+A+B+C+D+E+6,PoleTop+Hh+(K+2)*h+K+3);
  Canvas->MoveTo(PoleLeft+A+B+C+D+E+I+7,PoleTop+Hh+2);
  Canvas->LineTo(PoleLeft+A+B+C+D+E+I+7,PoleTop+Hh+(K+2)*h+K+3);
  Canvas->MoveTo(PoleLeft+A+B+C+D+E+I+G+8,PoleTop+Hh+2);
  Canvas->LineTo(PoleLeft+A+B+C+D+E+I+G+8,PoleTop+Hh+(K+2)*h+K+3);
  if(Prntr&&(Canvas==Prntr->Canvas)&&!ColorPrint||
     ImageBMP&&(Canvas==ImageBMP->Canvas)&&!ColorBMP)
   Canvas->Pen->Style=psSolid;
 }
 else if(NameZ.IsEmpty()){
  d1=(W-A-B-C-D-E-I-G-8)/14; d2=(W-A-B-C-D-E-I-G-8-7*d1)/7;
  A+=d1+d2; B+=d1+d2; C+=d1+d2; D+=d1+d2; E+=d1+d2; I+=d1+d2; G+=d1+d2;
  wcscpy(Inf,L"Название оси"); l=Canvas->TextWidth(Inf);
  Canvas->TextOut(PoleLeft+1+(A-l)/2,PoleTop+Hh+3+h/2,Inf);
  wcscpy(Inf,L"Значения координат характерных точек"); l=Canvas->TextWidth(Inf);
  Canvas->TextOut(PoleLeft+A+2+(B+C+D+E+3-l)/2,PoleTop+Hh+3,Inf);
  wcscpy(Inf,L"Масштаб"); l=Canvas->TextWidth(Inf);
  Canvas->TextOut(PoleLeft+A+B+C+D+E+7+(I-l)/2,PoleTop+Hh+3,Inf);
  wcscpy(Inf,L"Единица"); l=Canvas->TextWidth(Inf);
  Canvas->TextOut(PoleLeft+A+B+C+D+E+I+8+(G-l)/2,PoleTop+Hh+3,Inf);
  wcscpy(Inf,L"левая"); l=Canvas->TextWidth(Inf);
  Canvas->TextOut(PoleLeft+A+2+(B-l)/2,PoleTop+Hh+h+4,Inf);
  wcscpy(Inf,L"правая"); l=Canvas->TextWidth(Inf);
  Canvas->TextOut(PoleLeft+A+B+3+(C-l)/2,PoleTop+Hh+h+4,Inf);
  wcscpy(Inf,L"нижняя"); l=Canvas->TextWidth(Inf);
  Canvas->TextOut(PoleLeft+A+B+C+4+(D-l)/2,PoleTop+Hh+h+4,Inf);
  wcscpy(Inf,L"верхняя"); l=Canvas->TextWidth(Inf);
  Canvas->TextOut(PoleLeft+A+B+C+D+5+(E-l)/2,PoleTop+Hh+h+4,Inf);
  wcscpy(Inf,L"в 1 мм"); l=Canvas->TextWidth(Inf);
  Canvas->TextOut(PoleLeft+A+B+C+D+E+6+(I-l)/2,PoleTop+Hh+h+4,Inf);
  wcscpy(Inf,L"измерения"); l=Canvas->TextWidth(Inf);
  Canvas->TextOut(PoleLeft+A+B+C+D+E+I+7+(G-l)/2,PoleTop+Hh+h+4,Inf);
  DrawGrekText(Canvas,PoleLeft+1+d1,PoleTop+Hh+2*h+2,0,NameX.c_str(),SimbInd);
  swprintf(Inf,L"%g",Slv[0]); l=Canvas->TextWidth(Inf);
  Canvas->TextOut(PoleLeft+A+2+B-l-d2,PoleTop+Hh+2*h+5,Inf);
  swprintf(Inf,L"%g",Spr[0]); l=Canvas->TextWidth(Inf);
  Canvas->TextOut(PoleLeft+A+B+3+C-l-d2,PoleTop+Hh+2*h+5,Inf);
  swprintf(Inf,L"%g",Min[0]); l=Canvas->TextWidth(Inf);
  Canvas->TextOut(PoleLeft+A+B+C+4+D-l-d2,PoleTop+Hh+2*h+5,Inf);
  swprintf(Inf,L"%g",Max[0]); l=Canvas->TextWidth(Inf);
  Canvas->TextOut(PoleLeft+A+B+C+D+5+E-l-d2,PoleTop+Hh+2*h+5,Inf);
  swprintf(Inf,L"%g",MstT[0]); l=Canvas->TextWidth(Inf);
  Canvas->TextOut(PoleLeft+A+B+C+D+E+6+I-l-d2,PoleTop+Hh+2*h+5,Inf);
  wcscpy(Inf,RazmX.c_str());
  Canvas->TextOut(PoleLeft+A+B+C+D+E+I+7+d1,PoleTop+Hh+2*h+5,Inf);
  DrawGrekText(Canvas,PoleLeft+1+d1,PoleTop+Hh+3*h+2,0,NameY.c_str(),SimbInd);
  swprintf(Inf,L"%g",Slv[1]); l=Canvas->TextWidth(Inf);
  Canvas->TextOut(PoleLeft+A+2+B-l-d2,PoleTop+Hh+3*h+6,Inf);
  swprintf(Inf,L"%g",Spr[1]); l=Canvas->TextWidth(Inf);
  Canvas->TextOut(PoleLeft+A+B+3+C-l-d2,PoleTop+Hh+3*h+6,Inf);
  swprintf(Inf,L"%g",Min[1]); l=Canvas->TextWidth(Inf);
  Canvas->TextOut(PoleLeft+A+B+C+4+D-l-d2,PoleTop+Hh+3*h+6,Inf);
  swprintf(Inf,L"%g",Max[1]); l=Canvas->TextWidth(Inf);
  Canvas->TextOut(PoleLeft+A+B+C+D+5+E-l-d2,PoleTop+Hh+3*h+6,Inf);
  swprintf(Inf,L"%g",MstT[1]); l=Canvas->TextWidth(Inf);
  Canvas->TextOut(PoleLeft+A+B+C+D+E+6+I-l-d2,PoleTop+Hh+3*h+6,Inf);
  wcscpy(Inf,RazmY.c_str());
  Canvas->TextOut(PoleLeft+A+B+C+D+E+I+7+d1,PoleTop+Hh+3*h+6,Inf);
  if(Prntr&&(Canvas==Prntr->Canvas)&&!ColorPrint||
     ImageBMP&&(Canvas==ImageBMP->Canvas)&&!ColorBMP)
   Canvas->Pen->Color=clBlack;
  else Canvas->Pen->Color=ColorTabl;
  Canvas->Pen->Width=2;
  Canvas->MoveTo(PoleLeft+1,PoleTop+Hh+1);
  Canvas->LineTo(PoleLeft+1,PoleTop+Hh+4*h+5);
  Canvas->LineTo(PoleLeft+W-3,PoleTop+Hh+4*h+5);
  Canvas->LineTo(PoleLeft+W-3,PoleTop+Hh+1);
  Canvas->LineTo(PoleLeft+1,PoleTop+Hh+1);
  Canvas->MoveTo(PoleLeft+1,PoleTop+Hh+2*h+3);
  Canvas->LineTo(PoleLeft+W-3,PoleTop+Hh+2*h+3);
  Canvas->Pen->Width=1;
  Canvas->MoveTo(PoleLeft+A+2,PoleTop+Hh+h+2);
  Canvas->LineTo(PoleLeft+A+B+C+D+E+5,PoleTop+Hh+h+2);
  Canvas->MoveTo(PoleLeft+1,PoleTop+Hh+3*h+4);
  Canvas->LineTo(PoleLeft+W-3,PoleTop+Hh+3*h+4);
  Canvas->MoveTo(PoleLeft+A+2,PoleTop+Hh+2);
  Canvas->LineTo(PoleLeft+A+2,PoleTop+Hh+4*h+5);
  Canvas->MoveTo(PoleLeft+A+B+3,PoleTop+Hh+h+3);
  Canvas->LineTo(PoleLeft+A+B+3,PoleTop+Hh+4*h+5);
  Canvas->MoveTo(PoleLeft+A+B+C+4,PoleTop+Hh+h+3);
  Canvas->LineTo(PoleLeft+A+B+C+4,PoleTop+Hh+4*h+5);
  Canvas->MoveTo(PoleLeft+A+B+C+D+5,PoleTop+Hh+h+3);
  Canvas->LineTo(PoleLeft+A+B+C+D+5,PoleTop+Hh+4*h+5);
  Canvas->MoveTo(PoleLeft+A+B+C+D+E+6,PoleTop+Hh+2);
  Canvas->LineTo(PoleLeft+A+B+C+D+E+6,PoleTop+Hh+4*h+5);
  Canvas->MoveTo(PoleLeft+A+B+C+D+E+I+6,PoleTop+Hh+2);
  Canvas->LineTo(PoleLeft+A+B+C+D+E+I+6,PoleTop+Hh+4*h+5);
 }
 else{
  d1=(W-A-B-C-D-E-G-H-I-J-10)/18; d2=(W-A-B-C-D-E-G-H-I-J-10-9*d1)/9;
  A+=d1+d2; B+=d1+d2; C+=d1+d2; D+=d1+d2; E+=d1+d2; G+=d1+d2;
  H+=d1+d2; I+=d1+d2; J+=d1+d2;
  wcscpy(Inf,L"Название оси"); l=Canvas->TextWidth(Inf);
  Canvas->TextOut(PoleLeft+1+(A-l)/2,PoleTop+Hh+3+h/2,Inf);
  wcscpy(Inf,L"Значения координат характерных точек"); l=Canvas->TextWidth(Inf);
  Canvas->TextOut(PoleLeft+A+2+(B+C+D+E+G+H+5-l)/2,PoleTop+Hh+3,Inf);
  wcscpy(Inf,L"Масштаб"); l=Canvas->TextWidth(Inf);
  Canvas->TextOut(PoleLeft+A+B+C+D+E+G+H+9+(I-l)/2,PoleTop+Hh+3,Inf);
  wcscpy(Inf,L"Единица"); l=Canvas->TextWidth(Inf);
  Canvas->TextOut(PoleLeft+A+B+C+D+E+G+H+I+10+(G-l)/2,PoleTop+Hh+3,Inf);
  wcscpy(Inf,L"ближняя"); l=Canvas->TextWidth(Inf);
  Canvas->TextOut(PoleLeft+A+2+(B-l)/2,PoleTop+Hh+h+4,Inf);
  wcscpy(Inf,L"дальняя"); l=Canvas->TextWidth(Inf);
  Canvas->TextOut(PoleLeft+A+B+3+(C-l)/2,PoleTop+Hh+h+4,Inf);
  wcscpy(Inf,L"левая"); l=Canvas->TextWidth(Inf);
  Canvas->TextOut(PoleLeft+A+B+C+4+(D-l)/2,PoleTop+Hh+h+4,Inf);
  wcscpy(Inf,L"правая"); l=Canvas->TextWidth(Inf);
  Canvas->TextOut(PoleLeft+A+B+C+D+5+(E-l)/2,PoleTop+Hh+h+4,Inf);
  wcscpy(Inf,L"нижняя"); l=Canvas->TextWidth(Inf);
  Canvas->TextOut(PoleLeft+A+B+C+D+E+6+(G-l)/2,PoleTop+Hh+h+4,Inf);
  wcscpy(Inf,L"верхняя"); l=Canvas->TextWidth(Inf);
  Canvas->TextOut(PoleLeft+A+B+C+D+E+G+7+(H-l)/2,PoleTop+Hh+h+4,Inf);
  wcscpy(Inf,L"в 1 мм"); l=Canvas->TextWidth(Inf);
  Canvas->TextOut(PoleLeft+A+B+C+D+E+G+H+8+(I-l)/2,PoleTop+Hh+h+4,Inf);
  wcscpy(Inf,L"измерения"); l=Canvas->TextWidth(Inf);
  Canvas->TextOut(PoleLeft+A+B+C+D+E+G+H+I+9+(J-l)/2,PoleTop+Hh+h+4,Inf);
  for(int i=0;i<3;i++){
   wcscpy(Inf,i>1?NameZ.c_str():i?NameY.c_str():NameX.c_str());
   DrawGrekText(Canvas,PoleLeft+1+d1,PoleTop+Hh+(2+i)*h+5+i,0,Inf,SimbInd);
   swprintf(Inf,L"%g",Ner[i]); l=Canvas->TextWidth(Inf);
   Canvas->TextOut(PoleLeft+A+2+B-l-d2,PoleTop+Hh+(2+i)*h+5+i,Inf);
   swprintf(Inf,L"%g",Far[i]); l=Canvas->TextWidth(Inf);
   Canvas->TextOut(PoleLeft+A+B+3+C-l-d2,PoleTop+Hh+(2+i)*h+5+i,Inf);
   swprintf(Inf,L"%g",Slv[i]); l=Canvas->TextWidth(Inf);
   Canvas->TextOut(PoleLeft+A+B+C+4+D-l-d2,PoleTop+Hh+(2+i)*h+5+i,Inf);
   swprintf(Inf,L"%g",Spr[i]); l=Canvas->TextWidth(Inf);
   Canvas->TextOut(PoleLeft+A+B+C+D+5+E-l-d2,PoleTop+Hh+(2+i)*h+5+i,Inf);
   swprintf(Inf,L"%g",Low[i]); l=Canvas->TextWidth(Inf);
   Canvas->TextOut(PoleLeft+A+B+C+D+E+6+G-l-d2,PoleTop+Hh+(2+i)*h+5+i,Inf);
   swprintf(Inf,L"%g",Hig[i]); l=Canvas->TextWidth(Inf);
   Canvas->TextOut(PoleLeft+A+B+C+D+E+G+7+H-l-d2,PoleTop+Hh+(2+i)*h+5+i,Inf);
   swprintf(Inf,L"%g",MstT[0]); l=Canvas->TextWidth(Inf);
   Canvas->TextOut(PoleLeft+A+B+C+D+E+G+H+8+I-l-d2,PoleTop+Hh+(2+i)*h+5+i,Inf);
  }
  if(Prntr&&(Canvas==Prntr->Canvas)&&!ColorPrint||
     ImageBMP&&(Canvas==ImageBMP->Canvas)&&!ColorBMP)
   Canvas->Pen->Color=clBlack;
  else Canvas->Pen->Color=ColorTabl;
  Canvas->Pen->Width=2;
  Canvas->MoveTo(PoleLeft+1,PoleTop+Hh+1);
  Canvas->LineTo(PoleLeft+1,PoleTop+Hh+5*h+6);
  Canvas->LineTo(PoleLeft+W-3,PoleTop+Hh+5*h+6);
  Canvas->LineTo(PoleLeft+W-3,PoleTop+Hh+1);
  Canvas->LineTo(PoleLeft+1,PoleTop+Hh+1);
  Canvas->MoveTo(PoleLeft+1,PoleTop+Hh+2*h+3);
  Canvas->LineTo(PoleLeft+W-3,PoleTop+Hh+2*h+3);
  Canvas->Pen->Width=1;
  Canvas->MoveTo(PoleLeft+A+2,PoleTop+Hh+h+2);
  Canvas->LineTo(PoleLeft+A+B+C+D+E+G+H+7,PoleTop+Hh+h+2);
  Canvas->MoveTo(PoleLeft+1,PoleTop+Hh+3*h+4);
  Canvas->LineTo(PoleLeft+W-3,PoleTop+Hh+3*h+4);
  Canvas->MoveTo(PoleLeft+1,PoleTop+Hh+4*h+5);
  Canvas->LineTo(PoleLeft+W-3,PoleTop+Hh+4*h+5);
  Canvas->MoveTo(PoleLeft+A+2,PoleTop+Hh+2);
  Canvas->LineTo(PoleLeft+A+2,PoleTop+Hh+5*h+6);
  Canvas->MoveTo(PoleLeft+A+B+3,PoleTop+Hh+h+3);
  Canvas->LineTo(PoleLeft+A+B+3,PoleTop+Hh+5*h+6);
  Canvas->MoveTo(PoleLeft+A+B+C+4,PoleTop+Hh+h+3);
  Canvas->LineTo(PoleLeft+A+B+C+4,PoleTop+Hh+5*h+6);
  Canvas->MoveTo(PoleLeft+A+B+C+D+5,PoleTop+Hh+h+3);
  Canvas->LineTo(PoleLeft+A+B+C+D+5,PoleTop+Hh+5*h+6);
  Canvas->MoveTo(PoleLeft+A+B+C+D+E+6,PoleTop+Hh+h+3);
  Canvas->LineTo(PoleLeft+A+B+C+D+E+6,PoleTop+Hh+5*h+6);
  Canvas->MoveTo(PoleLeft+A+B+C+D+E+G+6,PoleTop+Hh+h+3);
  Canvas->LineTo(PoleLeft+A+B+C+D+E+G+6,PoleTop+Hh+5*h+6);
  Canvas->MoveTo(PoleLeft+A+B+C+D+E+G+H+7,PoleTop+Hh+2);
  Canvas->LineTo(PoleLeft+A+B+C+D+E+G+H+7,PoleTop+Hh+5*h+6);
  Canvas->MoveTo(PoleLeft+A+B+C+D+E+G+H+I+8,PoleTop+Hh+2);
  Canvas->LineTo(PoleLeft+A+B+C+D+E+G+H+I+8,PoleTop+Hh+5*h+6);
 }
 Canvas->Font->Color=Color;
}
//---------------------------------------------------------------------------
void __fastcall TFormKinema::DrawSetka(TCanvas *Canvas,Magazine *Names){
 TColor Color=Canvas->Font->Color,ColorBrush,ColorBr; wchar_t *inf=Inf;
 Magazine *m; bool SOVM=false; Vary *V; Form F;
 int i,j,x,y,l,x0,y0,B=Canvas->TextWidth(L"M"),Wgo=Wg,L,Lmax=0,Lmaxp=0;
 if(Names){
  swprintf(Inf,L"%d",TabControl->TabIndex);
  if(FindMagazine(Sovm,Inf,&m)) SOVM=true;
 }
//для графика
 Wg=1+Round((Spr[0]-Slv[0])/MSt[0]);
 Xk+=7*(Wgo-Wg)/16;
//формулы преобразования координат
//координаты начала кадра в абсолютной системе
 xk=(Spr[0]+Slv[0]-(Wg-1)*MSt[0])/2;
 yk[0]=(Max[1]+Min[1]-(Hg-1)*MSt[1])/2;
 if(SOVM){ int d;
  yk[1]=(Max[2]+Min[2]-(Hg-1)*MSt[2])/2;
  d=(int)(Round(yk[0]/MSt[1])-Round(yk[1]/MSt[2]));
  yk[1]+=(d%dS[1])*MSt[2];
 }
//координаты начала абсолютной системы в экранной системе
 x0=Xk-Round(xk/MSt[0]); y0=Yk+Round(yk[0]/MSt[1]);
//экранные координаты точки с координатами x,y в абс.системе
// xe=Xk+(x-xk)/MSt[0]; ye=Yk-(y-yk)/MSt[1];
 ColorBrush=Canvas->Brush->Color;
 if(Prntr&&(Canvas==Prntr->Canvas)&&!ColorPrint||
    ImageBMP&&(Canvas==ImageBMP->Canvas)&&!ColorBMP){
  Canvas->Brush->Color=clWhite; Canvas->Pen->Color=clWhite;
 }
 else{
  Canvas->Brush->Color=ColorPhon; Canvas->Pen->Color=ColorPhon;
 }
 Canvas->Pen->Width=1;
 Canvas->Rectangle(Xk,Yk-Hg,Xk+Wg,Yk);
 Canvas->Brush->Color=ColorBrush;
 i=ceil((double)(Xk-x0)/dS[0]);
 for(x=x0+i*dS[0],j=0;x<Xk+Wg;x+=dS[0],i++){
  if(Prntr&&(Canvas==Prntr->Canvas)&&!ColorPrint||
     ImageBMP&&(Canvas==ImageBMP->Canvas)&&!ColorBMP)
   Canvas->Pen->Color=clBlack;
  else Canvas->Pen->Color=ColorGrid;
  Canvas->MoveTo(x,Yk-1); Canvas->LineTo(x,Yk-Hg);
  if(Prntr&&(Canvas==Prntr->Canvas)&&!ColorPrint||
     ImageBMP&&(Canvas==ImageBMP->Canvas)&&!ColorBMP)
   Canvas->Font->Color=clBlack;
  else Canvas->Font->Color=ColorCifrGrid;
  if(i%2){
   Canvas->MoveTo(x-1,Yk-5); Canvas->LineTo(x-1,Yk+5);
   Canvas->MoveTo(x+1,Yk-5); Canvas->LineTo(x+1,Yk+5);
   swprintf(Inf,L"%g",i*MstT[0]*Sg);
   l=Canvas->TextWidth(Inf);
   Canvas->TextOut(x-l/2,Yk+2,Inf);
  }
  else if(PRNTOPRIGHT){
   Canvas->MoveTo(x-1,Yk-Hg); Canvas->LineTo(x-1,Yk-Hg+5);
   Canvas->MoveTo(x+1,Yk-Hg); Canvas->LineTo(x+1,Yk-Hg+5);
   swprintf(Inf,L"%g",i*MstT[0]*Sg);
   l=Canvas->TextWidth(Inf);
   Canvas->TextOut(x-l/2,Yk-Hg-Hy+3,Inf);
 }}
 i=ceil((double)(y0-Yk)/dS[1]);
 for(y=y0-i*dS[1];y>Yk-Hg;y-=dS[1],i++){
  if(Prntr&&(Canvas==Prntr->Canvas)&&!ColorPrint||
     ImageBMP&&(Canvas==ImageBMP->Canvas)&&!ColorBMP)
   Canvas->Pen->Color=clBlack;
  else Canvas->Pen->Color=ColorGrid;
  Canvas->MoveTo(Xk+1,y); Canvas->LineTo(Xk+Wg-1,y);
  if(Prntr&&(Canvas==Prntr->Canvas)&&!ColorPrint||
     ImageBMP&&(Canvas==ImageBMP->Canvas)&&!ColorBMP)
   Canvas->Font->Color=clBlack;
  else Canvas->Font->Color=SOVM?ColorGraph:ColorCifrGrid;
  if(i%2){
   Canvas->MoveTo(Xk,y-1); Canvas->LineTo(Xk+5,y-1);
   Canvas->MoveTo(Xk,y+1); Canvas->LineTo(Xk+5,y+1);
   L=swprintf(Inf,L"%g",i*MstT[1]*Sg);
   if((L>9)&&(wcschr(Inf,'e')||wcschr(Inf,'E')))
    swprintf(Inf,L"%10.3e",i*MstT[1]*Sg);
   l=Canvas->TextWidth(Inf); if(l>Lmax) Lmax=l;
   Canvas->TextOut(Xk-l-B,y-h/2,Inf);
   if(SOVM){
    Canvas->MoveTo(Xk+Wg,y-1); Canvas->LineTo(Xk+Wg-5,y-1);
    Canvas->MoveTo(Xk+Wg,y+1); Canvas->LineTo(Xk+Wg-5,y+1);
    j=Round((yk[1]/MSt[2]+(i*MstT[1]*Sg-yk[0])/MSt[1])/dS[1]);
    L=swprintf(Inf,L"%g",j*MstT[2]*Sg);
    if(L>9){
     if(wcschr(Inf,'e')||wcschr(Inf,'E'))
      L=swprintf(Inf,L"%10.3e",j*MstT[2]*Sg);
    }
    if(Prntr&&(Canvas==Prntr->Canvas)&&!ColorPrint||
       ImageBMP&&(Canvas==ImageBMP->Canvas)&&!ColorBMP)
     Canvas->Font->Color=clBlack;
    else Canvas->Font->Color=ColorGraph2;
    l=Canvas->TextWidth(Inf); if(l>Lmaxp) Lmaxp=l;
    Canvas->TextOut(Xk+Wg+B,y-h/2,Inf);
  }}
  else if(!SOVM&&PRNTOPRIGHT){
   Canvas->MoveTo(Xk+Wg,y-1); Canvas->LineTo(Xk+Wg-5,y-1);
   Canvas->MoveTo(Xk+Wg,y+1); Canvas->LineTo(Xk+Wg-5,y+1);
   L=swprintf(Inf,L"%g",i*MstT[1]*Sg);
   if((L>9)&&(wcschr(Inf,'e')||wcschr(Inf,'E')))
    swprintf(Inf,L"%10.3e",i*MstT[1]*Sg);
   l=Canvas->TextWidth(Inf); if(l>Lmaxp) Lmaxp=l;
   Canvas->TextOut(Xk+Wg+B,y-h/2,Inf);
 }}
 if(Prntr&&(Canvas==Prntr->Canvas)&&!ColorPrint||
    ImageBMP&&(Canvas==ImageBMP->Canvas)&&!ColorBMP)
  Canvas->Font->Color=clBlack;
 else Canvas->Font->Color=ColorNameOs;
 y=Yk-Hg-Hy;
 if(Names){
  if(SOVM){
   if(FindVary(Names->S,&V)){
    F.V=V; if(V->Razm.C!=Pust) FormToStr(V->Razm,&RazmY);
   }
   if(RazmY.IsEmpty())
    swprintf(Inf,L"%s",Names->S);
   else
    swprintf(Inf,L"%s[%s]",Names->S,RazmY);
   l=LengthGrek(Canvas,Inf,SimbInd); x=Xk-l-2*B;
   ColorBr=Canvas->Pen->Color;
   if(Prntr&&(Canvas==Prntr->Canvas)&&!ColorPrint||
      ImageBMP&&(Canvas==ImageBMP->Canvas)&&!ColorBMP){
    Canvas->Font->Color=clBlack;
    Canvas->Pen->Color=clWhite;
   }
   else{
    Canvas->Font->Color=SOVM?ColorGraph:ColorCifrGrid;
    Canvas->Pen->Color=Canvas->Brush->Color;
   }
   Canvas->Rectangle(x,Yk-Hg-Hy,x+l,Yk-Hg);
   Canvas->Pen->Color=ColorBr;
   Canvas->Font->Color=ColorGraph;
   DrawGrekText(Canvas,x,y,0,Inf,SimbInd);
   if(FindVary(Names->Sled->S,&V)){
    F.V=V; if(V->Razm.C!=Pust) FormToStr(V->Razm,&RazmY);
   }
   if(RazmY.IsEmpty())
    swprintf(Inf,L"%s",Names->Sled->S);
   else
    swprintf(Inf,L"%s[%s]",Names->Sled->S,RazmY);
   l=LengthGrek(Canvas,Inf,SimbInd);
   x=Xk+Wg+Lmaxp-l-2*B;
   ColorBr=Canvas->Pen->Color;
   Canvas->Pen->Color=Canvas->Brush->Color;
   Canvas->Rectangle(x,Yk-Hg-Hy,x+l,Yk-Hg);
   if(Prntr&&(Canvas==Prntr->Canvas)&&!ColorPrint||
      ImageBMP&&(Canvas==ImageBMP->Canvas)&&!ColorBMP){
    Canvas->Font->Color=clBlack;
    Canvas->Pen->Color=clBlack;
   }
   else{
    Canvas->Pen->Color=ColorBr;
    Canvas->Font->Color=ColorGraph2;
   }
   DrawGrekText(Canvas,x,y,0,Inf,SimbInd);
  }
  else{
   if(Tabl){
    inf+=swprintf(inf=Inf,Tabl[0].Vy->Name);
    if(FindVary(Tabl[0].Vy->Name,&V)){
     if(V->Razm.C!=Pust){
      FormToStr(V->Razm,&RazmY);
      inf+=swprintf(inf=Inf,L"%s[%s]",V->Name,RazmY);
     }
     else
      swprintf(Inf,L"%s",V->Name);
    }
    for(int K=KolElem(Names),i=1;i<K;i++){
     if(FindVary(Tabl[i].Vy->Name,&V)){
      if(V->Razm.C!=Pust){
       FormToStr(V->Razm,&RazmY);
       inf+=swprintf(inf,L", %s[%s]",V->Name,RazmY);
      }
	  else
       inf+=swprintf(inf,L", %s",V->Name);
   }}}
   l=LengthGrek(Canvas,Inf,SimbInd); x=Xk-Lmax-2*B; if(x<(B+1)) x=B+1;
   ColorBr=Canvas->Pen->Color;
   Canvas->Pen->Color=Canvas->Brush->Color;
   Canvas->Rectangle(x,Yk-Hg-Hy,x+l,Yk-Hg);
   Canvas->Pen->Color=ColorBr;
   DrawGrekText(Canvas,x,y,0,Inf,SimbInd);
 }}
 else{
  if(RazmY.IsEmpty())
   swprintf(Inf,L"%s",NameY);
  else
   swprintf(Inf,L"%s[%s]",NameY,RazmY);
  l=LengthGrek(Canvas,Inf,SimbInd); x=Xk-l-2*B;
  ColorBr=Canvas->Pen->Color;
  Canvas->Pen->Color=Canvas->Brush->Color;
  Canvas->Rectangle(x,Yk-Hg-Hy,x+l,Yk-Hg);
  Canvas->Pen->Color=ColorBr;
  DrawGrekText(Canvas,x,y,0,Inf,SimbInd);
 }
 if(Prntr&&(Canvas==Prntr->Canvas)&&!ColorPrint||
    ImageBMP&&(Canvas==ImageBMP->Canvas)&&!ColorBMP)
  Canvas->Font->Color=clBlack;
 else Canvas->Font->Color=ColorNameOs;
 if(RazmX.IsEmpty())
  inf+=swprintf(inf=Inf,L"%s",NameX);
 else
  inf+=swprintf(inf=Inf,L"%s[%s]",NameX,RazmX); //%s[%s]
 if(Names&&Tabl&&Tabl[1].Vx){
  for(int K=KolElem(Names),i=1;i<K;i++)
   if(wcscmp(Tabl[i].Vx->Name,NameX.c_str()))
    inf+=swprintf(inf,L",%s",Tabl[i].Vx->Name);
  x=B;
 }
 else x=0;
 l=LengthGrek(Canvas,Inf,SimbInd);
 h=Canvas->TextHeight(Inf);
 x+=Xk+Wg-l; //if(PRNTOPRIGHT) x+=B+(Lmaxp?Lmaxp:Lmax);
 if(Prntr&&(Canvas==Prntr->Canvas)&&!ColorPrint||
    ImageBMP&&(Canvas==ImageBMP->Canvas)&&!ColorBMP)
  Canvas->Pen->Color=clWhite;
 else Canvas->Pen->Color=ColorPhon;
 Canvas->Rectangle(x-B,Yk,x+l+B,Yk+h);
 DrawGrekText(Canvas,x,Yk+1,0,Inf,SimbInd);
 if(Prntr&&(Canvas==Prntr->Canvas)&&!ColorPrint||
    ImageBMP&&(Canvas==ImageBMP->Canvas)&&!ColorBMP)
  Canvas->Pen->Color=clBlack;
 else Canvas->Pen->Color=ColorRamka;
 Canvas->MoveTo(Xk,Yk-Hg);
 Canvas->LineTo(Xk,Yk); Canvas->LineTo(Xk+Wg,Yk);
 Canvas->LineTo(Xk+Wg,Yk-Hg); Canvas->LineTo(Xk,Yk-Hg);
 Canvas->MoveTo(Xk+Wg+1,Yk-Hg+1); Canvas->LineTo(Xk+Wg+1,Yk+1);
 Canvas->MoveTo(Xk+1,Yk+1); Canvas->LineTo(Xk+Wg+1,Yk+1);
 Canvas->Font->Color=Color;
}
//---------------------------------------------------------------------------
void __fastcall TFormKinema::DrawGrafik(TCanvas *Canvas,Magazine *Names){
 TColor Color=Canvas->Pen->Color; wchar_t Buf[256];
 int i,x,y,Width=Canvas->Pen->Width;//,LPx,LPy;
 double dt=(Tk-Tn)/(Wg-1),t=Tn,d,l,X; bool SOVM=false; Magazine *m;
// LPx=GetDeviceCaps(Canvas->Handle,LOGPIXELSX);
// LPy=GetDeviceCaps(Canvas->Handle,LOGPIXELSY);
 if(Names){
  swprintf(Inf,L"%d",TabControl->TabIndex);
  if(FindMagazine(Sovm,Inf,&m)) SOVM=true;
 }
 Canvas->Pen->Width=WidthPen;
 if(Prntr&&(Canvas==Prntr->Canvas)&&!ColorPrint||
    ImageBMP&&(Canvas==ImageBMP->Canvas)&&!ColorBMP)
  Canvas->Pen->Color=clBlack;
 CGauge->MaxValue=Wg;
 wcscpy(Buf,StatusBar->SimpleText.c_str());
 if(Names){
  int K=KolElem(Names);
  double yk0=yk[0],X,MSt1=MSt[1]; //16.03.2019
  TPoint **P=(TPoint **)calloc(WidthPen*K,SzV);
  for(i=0;i<K;i++){
   P[i]=(TPoint *)calloc(Wg,sizeof(TPoint));
   if(Prntr&&(Canvas==Prntr->Canvas)&&!ColorPrint||
      ImageBMP&&(Canvas==ImageBMP->Canvas)&&!ColorBMP){
    for(int k=1;k<WidthPen;k++)
     P[i+k*K]=(TPoint *)calloc(Wg,sizeof(TPoint));
  }}
  for(int j=0;j<Wg;j++){
   t=Tn+j*dt; ClearVar(); L->Time->Val=t;
   X=GodoGraf?FuncX(0,t):t;
   Vp=Tabl[0].Vy; d=Func(t);
   if(!NoCalc){
    P[0][j].x=Xk+(int)Round((X-xk)/MSt[0]);
    P[0][j].y=Yk-(int)Round((d-yk0)/MSt1);
    if(SOVM){ yk0=yk[1]; MSt1=MSt[2]; }
    if(Prntr&&(Canvas==Prntr->Canvas)&&!ColorPrint||
       ImageBMP&&(Canvas==ImageBMP->Canvas)&&!ColorBMP){
     for(int k=1;k<WidthPen;k++){
	  P[k*K][j].x=P[0][j].x;
      P[k*K][j].y=P[(k-1)*K][j].y+k*pow(-1,k);
   }}}
   for(i=1;i<K;i++){ //yk0=yk[i];
    if(Vp=Tabl[i].Vx) X=Func(t);
    Vp=Tabl[i].Vy; d=Func(t);
    if(!NoCalc){
     P[i][j].x=Xk+(int)Round((X-xk)/MSt[0]);
     P[i][j].y=Yk-(int)Round((d-yk0)/MSt1);
     if(SOVM){ yk0=yk[0]; MSt1=MSt[1]; }
     if(Prntr&&(Canvas==Prntr->Canvas)&&!ColorPrint||
        ImageBMP&&(Canvas==ImageBMP->Canvas)&&!ColorBMP){
      for(int k=1;k<WidthPen;k++){
       P[i+k*K][j].x=P[i][j].x;
       P[i+k*K][j].y=P[i+(k-1)*K][j].y+k*pow(-1,k);
  }}}}}
  for(i=0;i<K;i++){
   if(Prntr&&(Canvas==Prntr->Canvas)&&!ColorPrint||
      ImageBMP&&(Canvas==ImageBMP->Canvas)&&!ColorBMP){
    Canvas->Pen->Width=1;
    switch(i){
     case 0: Canvas->Pen->Style=psSolid;      break;
     case 1: Canvas->Pen->Style=psDash;       break;
     case 2: Canvas->Pen->Style=psDot;        break;
	 case 3: Canvas->Pen->Style=psDashDot;    break;
     case 4: Canvas->Pen->Style=psDashDotDot; break;
   }}
   else
    Canvas->Pen->Color=Tabl[i].C;
   Canvas->Polyline(P[i],Wg-1);
   if(Prntr&&(Canvas==Prntr->Canvas)&&!ColorPrint||
      ImageBMP&&(Canvas==ImageBMP->Canvas)&&!ColorBMP){
    for(int k=1;k<WidthPen;k++){
     Canvas->Polyline(P[i+k*K],Wg-1);
     free(P[i+k*K]);
   }}
   free(P[i]);
  }
  free(P);
  if(Prntr&&(Canvas==Prntr->Canvas)&&!ColorPrint||
     ImageBMP&&(Canvas==ImageBMP->Canvas)&&!ColorBMP)
   Canvas->Pen->Style=psSolid;
 }
 else{
  swprintf(Inf,L"Строится график %s(%s)",Vp->Name,L->Time->Name);
  StatusBar->SimpleText=Inf;
  CGauge->Progress=0; CGauge->ForeColor=ColorGraph;
  if(Prntr&&(Canvas==Prntr->Canvas)&&!ColorPrint||
	 ImageBMP&&(Canvas==ImageBMP->Canvas)&&!ColorBMP)
   Canvas->Pen->Color=clBlack;
  else Canvas->Pen->Color=ColorGraph;
  L->Time->Val=Tn; ClearVar(); d=Func(Tn);
  if(!NoCalc){
   x=Xk+Round((Tn-xk)/MSt[0]); y=Yk-Round((d-yk[0])/MSt[1]);
   Canvas->MoveTo(x,y);
  }
  for(int j=1;j<Wg;j++){
   t=Tn+j*dt; L->Time->Val=t; ClearVar();
   d=Func(t);
//PRINTVARS=false;
   if(!NoCalc){
    x=Xk+Round((t-xk)/MSt[0]); y=Yk-Round((d-yk[0])/MSt[1]);
    Canvas->LineTo(x,y);
   }
   CGauge->Progress++;
 }}
 Canvas->Pen->Width=Width; Canvas->Pen->Color=Color;
 CGauge->Progress=0;
 swprintf(Inf,L"%s: %s",HeadVersion[0]?HeadVersion:HeadWork,Autor);
 StatusBar->SimpleText=Inf;
}
//---------------------------------------------------------------------------
void __fastcall TFormKinema::GrafFunc(TCanvas *Canvas){
 Metrica(Canvas,NULL);
// CppWebBrowser->Height=0; CppWebBrowser->Width=0;
 if(PRINTHEAD) DrawHead(Canvas,Head);
 if(KP[0]==1) return;
 CGauge->ShowText=true;
 if(DefMSTGraf()){
  SelectSizeFont(Canvas);
  DrawSetka(Canvas,NULL);
  if(PRINTABL) DrawTabl(Canvas,NULL);
  for(Intg *I=L->I;I;I=I->Sled){ //подготовим интегралы
   if(I->x==L->Time&&I->b.V==L->Time){
    I->B=Value(I->a); I->Val=0.0;
  }}
  DrawGrafik(Canvas,NULL);
  if(IsNoCalc){
   swprintf(Inf,L"Не для всех значений времени %s\n"
    L"из указанного в инструкции \"%s\"\n"
    L"диапазона [%g,%g] получено решение!",
    L->Time->Name,InsTime->Name,Tn,Tk);
   Application->MessageBox(Inf,L"Внимание!",MB_OK|MB_ICONERROR);
   ToolButtonRead->Enabled=false;
   ToolButtonGame->Enabled=false;
 }}
 CGauge->ShowText=false;
}
//---------------------------------------------------------------------------
bool __fastcall TFormKinema::DefMSTGodo(){
 double dt,t,x,y; bool NoCalcX=false; Root *I; Parm *P; int Ps;
 dt=(Tk-Tn)/(Wg-1);
//Найдем характерные точки
 Min[0]=Spr[0]=-MAXDOUBLE; Max[0]=Slv[0]=MAXDOUBLE;
 Max[1]=Spr[1]=-MAXDOUBLE; Min[1]=Slv[1]=MAXDOUBLE;
 StatusBar->SimpleText=L"Расчет масштабов";
 CGauge->Progress=0; CGauge->MaxValue=Wg;
 for(int i=0;i<Wg;i++){
  ClearVar();
  if(ArgTime){ L->Time->Val=t=Tn+i*dt; x=FuncX(0,t); NoCalcX=NoCalc; y=FuncY(0,t); }
  else{ VgX->Val=x=Tn+i*dt; y=FuncY(0,x); }
  CGauge->Progress++;
  if(NoCalcX||NoCalc){ Tend=x; return false; }
  if(x>Spr[0]){ Spr[0]=x; Spr[1]=y; }
  if(x<Slv[0]){ Slv[0]=x; Slv[1]=y; }
  if(y>Max[1]){ Max[1]=y; Max[0]=x; }
  if(y<Min[1]){ Min[1]=y; Min[0]=x; }
 }
 CGauge->Progress=0;
 swprintf(Inf,L"%s: %s",HeadVersion[0]?HeadVersion:HeadWork,Autor);
 StatusBar->SimpleText=Inf;
//Найдем масштабы
 MSt[0]=(Spr[0]-Slv[0])/(KP[0]-1);
 MSt[1]=(Max[1]-Min[1])/(KP[1]-1);
 if(MSt[0]==0.0) MSt[0]=MSt[1];
 if(MSt[1]==0.0) MSt[1]=MSt[0];
 if((MSt[0]==0.0)||(MSt[1]==0.0)) return false;
 for(int i=0;i<2;i++){
  for(Ps=0;MSt[i]>10.0;Ps++) MSt[i]/=10.0;
  for(;MSt[i]<1.0;Ps--) MSt[i]*=10.0;
  for(int j=1;j<Kmg;j++) if(MSt[i]<=MG[j]) { MSt[i]=MG[j]; break; }
  for(int j=abs(Ps);j--;)
   if(Ps>0) MSt[i]*=10; else MSt[i]/=10;
 }
 if((MSt[1]!=MSt[0])&&(Max[1]!=Min[1]))
  if(((Spr[0]-Slv[0])/(Max[1]-Min[1])<KEqvScal)&&
     ((Spr[0]-Slv[0])/(Max[1]-Min[1])>(1.0/KEqvScal)))
   if(MSt[0]>MSt[1]) MSt[1]=MSt[0]; else if(MSt[1]>MSt[0]) MSt[0]=MSt[1];
 for(int i=0;i<2;i++){ MstT[i]=MSt[i]/Sg; MSt[i]/=dS[i];}
 return true;
}
//---------------------------------------------------------------------------
void __fastcall TFormKinema::DrawGodograf(TCanvas *Canvas){
 TColor Color=Canvas->Pen->Color; int xe,ye,Width=Canvas->Pen->Width,k;
 double dt=(Tk-Tn)/1000,t=Tn,x0,y0,x,y,xt,yt,xtt,ytt,L=0.0; wchar_t Buf[256];
 wcscpy(Buf,StatusBar->SimpleText.c_str());
 StatusBar->SimpleText=L"Строится годограф";
 if(Prntr&&(Canvas==Prntr->Canvas)&&!ColorPrint||
    ImageBMP&&(Canvas==ImageBMP->Canvas)&&!ColorBMP)
  Canvas->Pen->Color=clBlack;
 else Canvas->Pen->Color=ColorGraph;
 Canvas->Pen->Width=WidthPen;
 CGauge->MaxValue=1000; CGauge->Progress=1;
 ClearVar(); ::L->Time->Val=Tn; x0=ArgTime?FuncX(0,Tn):Tn; y0=FuncY(0,Tn);
 xe=Xk+Round((x0-xk)/MSt[0])/*+1*/; ye=Yk-Round((y0-yk[0])/MSt[1]);
 Canvas->MoveTo(xe,ye);
 for(t=Tn+dt;t<Tk;t+=dt){
  CGauge->Progress++;
  ClearVar();
  if(ArgTime){ ::L->Time->Val=t; x=FuncX(0,t); y=FuncY(0,t); }
  else{ VgX->Val=x=t; y=FuncY(0,x); }
  L+=sqrt((x-x0)*(x-x0)+(y-y0)*(y-y0)); x0=x; y0=y;
 }
 k=L*2/(MSt[0]+MSt[1]); dt=(Tk-Tn)/k;
 for(t=Tn;t<Tk;t+=dt) {
//  x=ArgTime?FuncX(0,t):t; y=FuncY(0,t);
  ClearVar();
  if(ArgTime){ ::L->Time->Val=t; x=FuncX(0,t); y=FuncY(0,t); }
  else{ VgX->Val=x=t; y=FuncY(0,x); }
  xe=Xk+Round((x-xk)/MSt[0]); ye=Yk-Round((y-yk[0])/MSt[1]);
  Canvas->LineTo(xe,ye);
 }
// x=ArgTime?FuncX(0,Tk):Tk; y=FuncY(0,Tk);
 if(ArgTime){ ::L->Time->Val=t; x=FuncX(0,t); y=FuncY(0,t); }
 else{ VgX->Val=x=t; y=FuncY(0,x); }
 xe=Xk+(x-xk)/MSt[0]; ye=Yk-(y-yk[0])/MSt[1];
 Canvas->LineTo(xe,ye);
 Canvas->Pen->Width=Width; Canvas->Pen->Color=Color;
 CGauge->Progress=0; StatusBar->SimpleText=Buf;
}
//---------------------------------------------------------------------------
void __fastcall TFormKinema::Godograph(TCanvas *Canvas){
 Metrica(Canvas,NULL);
 if(PRINTHEAD) DrawHead(Canvas,Head);
 if(KP[0]==1) return;
 CGauge->ShowText=true;
 if(DefMSTGodo()){
  SelectSizeFont(Canvas);
  DrawSetka(Canvas,NULL);
  if(PRINTABL) DrawTabl(Canvas,NULL);
  for(Intg *I=L->I;I;I=I->Sled){ //подготовим интегралы
   if(I->x==L->Time&&I->b.V==L->Time){
	I->B=Value(I->a); I->Val=0.0;
  }}
  DrawGodograf(Canvas);
  if(IsNoCalc){
   swprintf(Inf,L"Не для всех значений времени %s\n"
    L"из указанного в инструкции \"%s\"\n"
    L"диапазона [%g,%g] получено решение!",
    L->Time->Name,InsTime->Name,Tn,Tk);
   Application->MessageBox(Inf,L"Внимание!",MB_OK|MB_ICONERROR);
   ToolButtonRead->Enabled=false;
   ToolButtonGame->Enabled=false;
 }}
 CGauge->ShowText=false;
}
//---------------------------------------------------------------------------
bool __fastcall TFormKinema::ReadRezFile(wchar_t *File){ //надо ли загружаться из файла
 wchar_t FileRez[256],*s; FILE *F; long DateRez,TM;
 int Ct,Kk,Kinst=KolInst(TextFromFile(WORKDIR,L"EditKiDyM",&SLK,111))+
  KolInst(TextFromFile(WORKDIR,L"EditKiDyM",&SLK,159));
 double T_n,T_k,Xn,Xk,Yn,Yk;
 WriteRezFile=true;
 if(Kinst){
  s=wcsrchr(File,'.'); *s='\0';
  swprintf(FileRez,L"%s№%d.%s",File,Kinst+1,s+1); *s='.';
 }
 else wcscpy(FileRez,File);
 if(!_waccess(FileRez,0)){
  if(F=_wfopen(FileRez,L"rb")){
   fread(&DateRez,SzL,1,F); fread(&Ct,SzI,1,F);
   fread(&Kk,SzI,1,F);  fread(&TM,SzL,1,F);
   fread(&T_n,SzD,1,F); fread(&T_k,SzD,1,F);
   fread(&Xn,SzD,1,F);  fread(&Xk,SzD,1,F);
   fread(&Yn,SzD,1,F);  fread(&Yk,SzD,1,F);
   fclose(F);
   if((Ct==CountTabl)&&(Kk==Kkadr)&&(TM==TMult)&&(T_n==Tn)&&(T_k==Tk)&&
      (Xn==X_n)&&(Xk==X_k)&&(Yn==Y_n)&&(Yk==Y_k)){
    if(DateInp==DateRez)
     WriteRezFile=false;
    else if(RENOVCALC){
     swprintf(Inf,L"Исходные данные изменены.\n"
      L"Если изменения внесены в модель, следует сделать новый расчет.\n"
      L"Если изменена часть данных, отвечающая за отображение результатов,\n"
      L"можно воспользоваться результатами уже полученными ранее.\n"
      L"Использовать старые результаты?");
     if(Application->MessageBox(Inf,L"Внимание!",MB_YESNO|MB_ICONQUESTION)==ID_YES)
      WriteRezFile=false;
 }}}}
 return !WriteRezFile;
}
//---------------------------------------------------------------------------
void __fastcall TFormKinema::SaveRezFile(wchar_t *File){
 wchar_t FileRez[256],*s; FILE *F;
 int K,i,Kt=CountTabl+1,
  Kinst=KolInst(TextFromFile(WORKDIR,L"EditKiDyM",&SLK,111))+
   KolInst(TextFromFile(WORKDIR,L"EditKiDyM",&SLK,159));
 if(Kinst){
  s=wcsrchr(File,'.'); *s='\0';
  swprintf(FileRez,L"%s№%d.%s",File,Kinst+1,s+1); *s='.';
 }
 else wcscpy(FileRez,File);
 if(F=_wfopen(FileRez,L"wb")){
  fwrite(&DateInp,SzL,1,F); fwrite(&CountTabl,SzI,1,F);
  fwrite(&Kkadr,SzI,1,F);   fwrite(&TMult,SzL,1,F);
  fwrite(&Tn,SzD,1,F);      fwrite(&Tk,SzD,1,F);
  fwrite(&X_n,SzD,1,F);     fwrite(&X_k,SzD,1,F);
  fwrite(&Y_n,SzD,1,F);     fwrite(&Y_k,SzD,1,F);
  fwrite(Qtz,SzD,(Nt+1)*(Kt*Kt+CountTabl*CountTabl+1),F);
  fclose(F);
}}
//---------------------------------------------------------------------------
bool __fastcall TFormKinema::LoadRezFile(wchar_t *File){
//Формат файла: long DateInp; int CountTable,Kkadr; double Tn,Tk,X_n,X_k;
 wchar_t FileRez[256],*s; FILE *F; long DateRez,TM; double T_n,T_k,Xn,Xk,Yn,Yk;
 int K,i,Ct,Kk,Kt=CountTabl+1,
  Kinst=KolInst(TextFromFile(WORKDIR,L"EditKiDyM",&SLK,111))+
   KolInst(TextFromFile(WORKDIR,L"EditKiDyM",&SLK,159));
 if(Kinst){
  s=wcsrchr(File,'.'); *s='\0';
  swprintf(FileRez,L"%s№%d.%s",File,Kinst+1,s+1); *s='.';
 }
 else wcscpy(FileRez,File);
 if(F=_wfopen(FileRez,L"rb")){
  fread(&DateRez,SzL,1,F); fread(&Ct,SzI,1,F);
  fread(&Kk,SzI,1,F);      fread(&TM,SzL,1,F);
  fread(&T_n,SzD,1,F);     fread(&T_k,SzD,1,F);
  fread(&Xn,SzD,1,F);      fread(&Xk,SzD,1,F);
  fread(&Yn,SzD,1,F);      fread(&Yk,SzD,1,F);
  fread(q=Qtz,SzD,(Nt+1)*(Kt*Kt+Ct*Ct+1),F);
  fclose(F);
  return true;
 }
 return false;
}
//---------------------------------------------------------------------------
bool __fastcall TFormKinema::DefMSTGodo3D(){
 int Ps,Kt,i,j,k; double x,y,z,t,D,dt; wchar_t *s;
//Найдем характерные точки
//Min[0,1,2],Max[0,1,2] - минимальные и максимальные значения x,y,z
//Ner[0,1,2],Slv[0,1,2],Low[0,1,2] - координаты ближней (по оси х,
// левой (по оси у) и нижней (по оси z) точек графика
//Far[0,1,2],Spr[0,1,2],Hig[0,1,2] - координаты дальней (по оси х,
// правой (по оси у) и верхней (по оси z) точек графика
 Min[0]=Min[1]=Min[2]=Ner[0]=Slv[0]=Low[0]=MAXDOUBLE;
 Max[0]=Max[1]=Max[2]=Far[0]=Spr[0]=Hig[0]=-MAXDOUBLE;
 StatusBar->SimpleText=L"Расчет масштабов";
 CGauge->Progress=0;
 if(!VarInForm(true,VgX->Znach,L->Time)||!VarInForm(true,VgY->Znach,L->Time)){
  double dx=(X_k-X_n)/CountTabl, dy=(Y_k-Y_n)/CountTabl; Kt=CountTabl+1;
  if(VarInForm(true,VgZ->Znach,L->Time)){//функция зависит от времени
   Nt=Round(0.001*TMult*Kkadr); dt=(Tk-Tn)/Nt;
   CGauge->MaxValue=(Nt+1)*(Kt*Kt+CountTabl*CountTabl+1);
   swprintf(File_tz,L"%s(%s_%s).tz",VgZ->Name,VgX->Name,VgY->Name);
   Min[0]=Ner[0]=X_n; Ner[1]=Y_n; Ner[2]=FuncXYZ(Ner[0], Ner[1]);
   Min[1]=Slv[1]=Y_n; Slv[0]=X_n; Slv[2]=FuncXYZ(Slv[0], Slv[1]);
   Max[0]=Far[0]=X_k; Far[1]=Y_n; Far[2]=FuncXYZ(Far[0], Far[1]);
   Max[1]=Spr[1]=Y_k; Spr[0]=X_n; Spr[2]=FuncXYZ(Spr[0], Spr[1]);
   q=Qtz=(double*)calloc((Nt+1)*(Kt*Kt+CountTabl*CountTabl+1),SzD);
   if(ReadRezFile(File_tz)){
    LoadRezFile(File_tz);
    for(k=0;k<=Nt;k++){
	 t=*q++;
     for(i=0,x=X_n;i<Kt;i++,x+=dx){
      for(j=0,y=Y_n;j<Kt;j++,y+=dy){
       z=*q++;
       if(z<Min[2]){ Min[2]=Low[2]=z; Low[0]=x; Low[1]=y; }
       if(z>Max[2]){ Max[2]=Hig[2]=z; Hig[0]=x; Hig[1]=y; }
       CGauge->Progress++;
     }}
     for(i=0,x=X_n+0.5*dx;i<CountTabl;i++,x+=dx){
      for(j=0,y=Y_n+0.5*dy;j<CountTabl;j++,y+=dy){
       z=*q++;
       if(z<Min[2]){ Min[2]=Low[2]=z; Low[0]=x; Low[1]=y; }
       if(z>Max[2]){ Max[2]=Hig[2]=z; Hig[0]=x; Hig[1]=y; }
       CGauge->Progress++;
   }}}}
   else{
//пишем только аппликаты Q={t,zt00,zt01,...,zt0Ct, zt10,zt11,...,zt1Ct,...,ztCtCt}
    StatusBar->SimpleText=L"Расчет масштабов и кадров анимации";
    for(k=0,L->Time->Val=t=Tn;k<=Nt;k++,t+=dt){
     *q++=L->Time->Val=t;
     for(i=0,x=X_n;i<Kt;i++,x+=dx){
      for(j=0,y=Y_n;j<Kt;j++,y+=dy){
       *q++=z=FuncXYZ(x,y);
       if(NoCalc){ Tend=t; return false; }
	   if(z<Min[2]){ Min[2]=Low[2]=z; Low[0]=x; Low[1]=y; }
       if(z>Max[2]){ Max[2]=Hig[2]=z; Hig[0]=x; Hig[1]=y; }
       CGauge->Progress++;
     }}
     for(i=0,x=X_n+0.5*dx;i<CountTabl;i++,x+=dx){
      for(j=0,y=Y_n+0.5*dy;j<CountTabl;j++,y+=dy){
       *q++=z=FuncXYZ(x,y);
       if(NoCalc) return false;
       if(z<Min[2]){ Min[2]=Low[2]=z; Low[0]=x; Low[1]=y; }
       if(z>Max[2]){ Max[2]=Hig[2]=z; Hig[0]=x; Hig[1]=y; }
       CGauge->Progress++;
    }}}
    SaveRezFile(File_tz);
  }}
  else{
   CGauge->MaxValue=Kt*Kt;
   q=Qtz=(double*)calloc((Kt*Kt+CountTabl*CountTabl+1),SzD);
   for(i=0;i<Kt;i++){
    x=X_n+i*dx;
    for(j=0;j<Kt;j++){
     y=Y_n+j*dy;
     *q++=z=FuncXYZ(x,y);
     if(NoCalc){ Tend=t; return false; }
     if(x<Min[0]){ Min[0]=Ner[0]=x; Ner[1]=y; Ner[2]=z; }
	 if(x>Max[0]){ Max[0]=Far[0]=x; Far[1]=y; Far[2]=z; }
     if(y<Min[1]){ Min[1]=Slv[1]=y; Slv[0]=x; Slv[2]=z; }
     if(y>Max[1]){ Max[1]=Spr[1]=y; Spr[0]=x; Spr[2]=z; }
     if(z<Min[2]){ Min[2]=Low[2]=z; Low[0]=x; Low[1]=y; }
     if(z>Max[2]){ Max[2]=Hig[2]=z; Hig[0]=x; Hig[1]=y; }
     CGauge->Progress++;
  }}}
  q=Qtz;
 }
 else{
  Kt=Wg>Hg?Wg:Hg; double dt=(Tk-Tn)/(Kt-1),t;
  CGauge->MaxValue=Kt;
  if(Qxyz){ for(i=0;i<Kt;i++) free(Qxyz[i]); free(Qxyz); Qxyz=NULL; }
  for(i=0;i<Kt;i++){
   t=Tn+i*dt; FuncXYZ(t,x,y,z); CGauge->Progress++;
   if(NoCalc){ Tend=t; return false; }
   if(x<Min[0]){ Min[0]=Ner[0]=x; Ner[1]=y; Ner[2]=z; }
   if(x>Max[0]){ Max[0]=Far[0]=x; Far[1]=y; Far[2]=z; }
   if(y<Min[1]){ Min[1]=Slv[1]=y; Slv[0]=x; Slv[2]=z; }
   if(y>Max[1]){ Max[1]=Spr[1]=y; Spr[0]=x; Spr[2]=z; }
   if(z<Min[2]){ Min[2]=Low[2]=z; Low[0]=x; Low[1]=y; }
   if(z>Max[2]){ Max[2]=Hig[2]=z; Hig[0]=x; Hig[1]=y; }
 }}
 CGauge->Progress=0;
 swprintf(Inf,L"%s: %s",HeadVersion[0]?HeadVersion:HeadWork,Autor);
 StatusBar->SimpleText=Inf;
//Найдем масштабы
 D=Max[2]-Min[2];
 MSt[2]=D*dS[0]/Hg;
 for(Ps=0;MSt[2]>10.0;Ps++) MSt[2]/=10.0;
 for(;MSt[2]<1.0;Ps--) MSt[2]*=10.0;
 for(int j=1;j<Kmg;j++) if(MSt[2]<=MG[j]) { MSt[2]=MG[j]; break; }
 for(int j=abs(Ps);j--;) if(Ps>0) MSt[2]*=10; else MSt[2]/=10;
 D=ceil(Max[2]/MSt[2]);
 Max[2]=D*MSt[2];
 D=floor(Min[2]/MSt[2]);
 Min[2]=D*MSt[2];
 XC=0.5*(Max[0]+Min[0]); YC=0.5*(Max[1]+Min[1]); ZC=0.5*(Max[2]+Min[2]);
 Diag=sqrt((Max[0]-Min[0])*(Max[0]-Min[0])+
           (Max[1]-Min[1])*(Max[1]-Min[1])+
           (Max[2]-Min[2])*(Max[2]-Min[2]));
 MSt[0]=Diag*dS[0]/Hg;
 for(Ps=0;MSt[0]>10.0;Ps++) MSt[0]/=10.0;
 for(;MSt[0]<1.0;Ps--) MSt[0]*=10.0;
 for(int j=1;j<Kmg;j++) if(MSt[0]<=MG[j]) { MSt[0]=MG[j]; break; }
 for(int j=abs(Ps);j--;) if(Ps>0) MSt[0]*=10; else MSt[0]/=10;
 for(int i=0;i<3;i++)
  KP[i]=floor(Max[i]/MSt[0])-ceil(Min[i]/MSt[0]);
 MstT[0]=MSt[0]/Sg; MSt[0]/=dS[0];
 return true;
}
//---------------------------------------------------------------------------
double __fastcall TFormKinema::ToScreen(double x,double y,double z,
  int &xe,int &ye){//x,y,z - координаты точки в связанной СК,
//X1,Y1,Z1 - координаты этой точки в абсолютной СК, с началом в центре аквариума
//xe,ye - координаты точки на экране
 double X1,Y1,Z1,m;
 x-=XC; y-=YC; z-=ZC;
 X1=x*c11+y*c12+z*c13; Y1=x*c21+y*c22+z*c23; Z1=x*c31+y*c32+z*c33;
 if(Perspect){
  m=3*Hg/(3.1*Diag-Z1); xe=Wo+Wg/2+m*X1; ye=Hh+Ht+Hg/2-Round(m*Y1);
 }
 else{
  xe=Wo+Wg/2+Round(X1/MSt[0]); ye=+Hh+Ht+Hg/2-Round(Y1/MSt[0]);
 }
 return Z1;
}
//---------------------------------------------------------------------------
int CmpSquare(const void *a, const void *b){
 Square *S1=(Square *)a,*S2=(Square *)b;
 if(S1->Zce > S2->Zce) return 1;
 if(S1->Zce < S2->Zce) return -1;
 return 0;
}
//---------------------------------------------------------------------------
void Square::Fill(double Xn,double Yn,double Xp,double Yv){
 double x,y,z; int xe,ye;
 x=Xn; y=Yn; z=FormKinema->FuncXYZ(x,y); FormKinema->ToScreen(x,y,z,xe,ye);
 V[0].x=V[4].x=xe; V[0].y=V[4].y=ye;
 x=Xp; y=Yn; z=FormKinema->FuncXYZ(x,y); FormKinema->ToScreen(x,y,z,xe,ye);
 V[1].x=xe; V[1].y=ye;
 x=Xp; y=Yv; z=FormKinema->FuncXYZ(x,y); FormKinema->ToScreen(x,y,z,xe,ye);
 V[2].x=xe; V[2].y=ye;
 x=Xn; y=Yv; z=FormKinema->FuncXYZ(x,y); FormKinema->ToScreen(x,y,z,xe,ye);
 V[3].x=xe; V[3].y=ye;
 x=(Xn+Xp)/2; y=(Yn+Yv)/2; z=FormKinema->FuncXYZ(x,y);
 Zce=FormKinema->ToScreen(x,y,z,xe,ye); Z=z;
}
//---------------------------------------------------------------------------
void __fastcall TFormKinema::DrawBar3D(TCanvas *Canvas,bool Color){
 int xO,yO,zO,x,y,z,i; double DS=dS[0]*MSt[0],X,Y,Z;
 if(ColorRamka!=ColorPhon){
  Canvas->Pen->Color=Color?ColorRamka:ColorPhon;
  ToScreen(Min[0],Min[1],Min[2],xO,yO); Canvas->MoveTo(xO,yO);
  Canvas->Pen->Width=1+2*((c33<=0.0)||(c32<=0));
  ToScreen(Max[0],Min[1],Min[2],x,y); Canvas->LineTo(x,y);
  Canvas->Pen->Width=1+2*((c33<=0.0)||(c31>0));
  ToScreen(Max[0],Max[1],Min[2],x,y); Canvas->LineTo(x,y);
  Canvas->Pen->Width=1+2*((c33<=0.0)||(c32>0));
  ToScreen(Min[0],Max[1],Min[2],x,y); Canvas->LineTo(x,y);
  Canvas->Pen->Width=1+2*((c33<=0.0)||(c31<=0));
  Canvas->LineTo(xO,yO);
  ToScreen(Min[0],Min[1],Max[2],xO,yO); Canvas->MoveTo(xO,yO);
  Canvas->Pen->Width=1+2*((c33>0.0)||(c32<=0.0));
  ToScreen(Max[0],Min[1],Max[2],x,y); Canvas->LineTo(x,y);
  Canvas->Pen->Width=1+2*((c33>0.0)||(c31>0.0));
  ToScreen(Max[0],Max[1],Max[2],x,y); Canvas->LineTo(x,y);
  Canvas->Pen->Width=1+2*((c33>0.0)||(c32>0.0));
  ToScreen(Min[0],Max[1],Max[2],x,y); Canvas->LineTo(x,y);
  Canvas->Pen->Width=1+2*((c33>0.0)||(c31<=0));
  Canvas->LineTo(xO,yO);
  ToScreen(Min[0],Min[1],Min[2],xO,yO); Canvas->MoveTo(xO,yO);
  Canvas->Pen->Width=1+2*((c31<=0.0)||(c33<=0.0));
  ToScreen(Min[0],Max[1],Min[2],x,y); Canvas->LineTo(x,y);
  Canvas->Pen->Width=1+2*((c31<=0.0)||(c32>0.0));
  ToScreen(Min[0],Max[1],Max[2],x,y); Canvas->LineTo(x,y);
  Canvas->Pen->Width=1+2*((c31<=0.0)||(c33>0.0));
  ToScreen(Min[0],Min[1],Max[2],x,y); Canvas->LineTo(x,y);
  Canvas->Pen->Width=1+2*((c31<=0.0)||(c32<=0.0));
  Canvas->LineTo(xO,yO);
  Canvas->Pen->Width=1+2*(c31>0.0);
  ToScreen(Max[0],Min[1],Min[2],xO,yO); Canvas->MoveTo(xO,yO);
  Canvas->Pen->Width=1+2*((c31>0.0)||(c33<=0.0));
  ToScreen(Max[0],Max[1],Min[2],x,y); Canvas->LineTo(x,y);
  Canvas->Pen->Width=1+2*((c31>0.0)||(c32>0.0));
  ToScreen(Max[0],Max[1],Max[2],x,y); Canvas->LineTo(x,y);
  Canvas->Pen->Width=1+2*((c31>0.0)||(c33>0.0));
  ToScreen(Max[0],Min[1],Max[2],x,y); Canvas->LineTo(x,y);
  Canvas->Pen->Width=1+2*((c31>0.0)||(c32<=0.0));
  Canvas->LineTo(xO,yO);
 }
 if(ColorGrid!=ColorPhon){
  Canvas->Pen->Color=Color?ColorGrid:ColorPhon; Canvas->Pen->Width=1;
  for(i=ceil(Min[2]/DS);i*DS<Max[2];i++){ Z=i*DS;
   ToScreen(Min[0],Min[1],Z,xO,yO); Canvas->MoveTo(xO,yO);
   ToScreen(Max[0],Min[1],Z,x,y); Canvas->LineTo(x,y);
   ToScreen(Max[0],Max[1],Z,x,y); Canvas->LineTo(x,y);
   ToScreen(Min[0],Max[1],Z,x,y); Canvas->LineTo(x,y);
   Canvas->LineTo(xO,yO);
  }
  for(i=ceil(Min[0]/DS);i*DS<Max[0];i++){ X=i*DS;
   ToScreen(X,Min[1],Min[2],xO,yO); Canvas->MoveTo(xO,yO);
   ToScreen(X,Max[1],Min[2],x,y); Canvas->LineTo(x,y);
   ToScreen(X,Max[1],Max[2],x,y); Canvas->LineTo(x,y);
   ToScreen(X,Min[1],Max[2],x,y); Canvas->LineTo(x,y);
   Canvas->LineTo(xO,yO);
  }
  for(i=ceil(Min[1]/DS);i*DS<Max[1];i++){ Y=i*DS;
   ToScreen(Min[0],Y,Min[2],xO,yO); Canvas->MoveTo(xO,yO);
   ToScreen(Min[0],Y,Max[2],x,y); Canvas->LineTo(x,y);
   ToScreen(Max[0],Y,Max[2],x,y); Canvas->LineTo(x,y);
   ToScreen(Max[0],Y,Min[2],x,y); Canvas->LineTo(x,y);
   Canvas->LineTo(xO,yO);
}}}
//---------------------------------------------------------------------------
void __fastcall TFormKinema::DrawVectors(TCanvas *Canvas,bool Color){
 int xO,yO,zO,x,y,z,xt,yt,ang; double a,b;
 if(ColorRamka!=ColorPhon){
  Canvas->Pen->Color=Color?ColorRamka:ColorPhon;
  if((c33<=0.0)&&(c32>0)||(c33>0.0)&&(c32<=0)){
   a=0.3*Max[0]; b=a/4; Canvas->Pen->Width=WidthPen;
   ToScreen((1-a)*Max[0],Min[1]-b,Min[2]-b,x,y); Canvas->MoveTo(x,y);
   ToScreen(Max[0],Min[1],Min[2],x,y);           Canvas->LineTo(x,y);
   ToScreen((1-a)*Max[0],Min[1]+b,Min[2]+b,x,y); Canvas->LineTo(x,y);
  }
}}
//---------------------------------------------------------------------------
void __fastcall TFormKinema::DrawCurve3D(TCanvas *Canvas,TColor Color){
 int X,Y,Z,Kt=Hg>Wg?Hg:Wg; double t=Tn,x,y,z,dt=(Tk-Tn)/(Kt-1);
 FuncXYZ(Tn,x,y,z); ToScreen(x,y,z,X,Y); Canvas->MoveTo(X,Y);
 Canvas->Pen->Color=Color; Canvas->Pen->Width=WidthPen;
 for(int i=1;i<Kt;i++){
  t=Tn+i*dt; FuncXYZ(t,x,y,z); ToScreen(x,y,z,X,Y);
  Canvas->LineTo(X,Y);
}}
//---------------------------------------------------------------------------
void __fastcall TFormKinema::PrepDraw3D(TCanvas *Canvas){
 if(!VarInForm(true,VgX->Znach,L->Time)||!VarInForm(true,VgY->Znach,L->Time)){
  Fi=-M_PI/3; Psi=-M_PI/2; Teta=-M_PI/3;
  //Fi=-M_PI/6; Psi=0;       Teta=M_PI/3;
  //Fi= M_PI/4; Psi=3*Fi;    Teta=M_PI/6;
 }
 cFi=cos(Fi); sFi=sin(Fi); cPsi=cos(Psi); sPsi=sin(Psi);
 cTeta=cos(Teta); sTeta=sin(Teta);
 c11=cPsi*cTeta*cFi-sPsi*sFi; c12=-cPsi*cTeta*sFi-sPsi*cFi; c13=cPsi*sTeta;
 c21=sPsi*cTeta*cFi+cPsi*sFi; c22=-sPsi*cTeta*sFi+cPsi*cFi; c23=sPsi*sTeta;
 c31=-sTeta*cFi; c32=sTeta*sFi; c33=cTeta;
 if(!VarInForm(true,VgX->Znach,L->Time)||!VarInForm(true,VgY->Znach,L->Time)){
  TimerArea->Enabled=VarInForm(true,VgZ->Znach,L->Time);
  if(TimerArea->Enabled){ int Rez;
   CGauge->Progress=0; CGauge->MaxValue=Nt; CGauge->ShowText=true;
   if(TOBMP||TOJPEG){
	if(Kinst>1){
     swprintf(Dir,L"%s\\%s_%d(%s,%s)",CurrDir,FormKinema->NameZ,
      Kinst,FormKinema->NameX,FormKinema->NameY);
     if(_waccess(Dir,0)) _wmkdir(Dir);
    }
    else{
     swprintf(Dir,L"%s\\%s(%s,%s)",CurrDir,FormKinema->NameZ,
      FormKinema->NameX,FormKinema->NameY);
     if(_waccess(Dir,0)) _wmkdir(Dir);
   }}
   if(TOBMP&&!_waccess(Dir,0))
    swprintf(BMPKadrs,L"%s\\%3.3d.bmp",Dir,Nkadr);
   if(TOJPEG&&!_waccess(Dir,0))
    swprintf(JPGKadrs,L"%s\\%3.3d.jpg",Dir,Nkadr);
   Nkadr=0;
   DrawArea(Canvas,true);
   TimerArea->Interval=Round(1000/Kkadr);
   L->Time->Val=Tn;
  }
  else{
   double dFi  =2*M_PI*TimerArea->Interval/15000.0,
    dTeta=2*M_PI*TimerArea->Interval/25000.0,
    dPsi =2*M_PI*TimerArea->Interval/5000.0;
    cFi-=sFi*dFi; sFi+=cFi*dFi; cPsi-=sPsi*dPsi; sPsi+=cPsi*dPsi;
	cTeta-=sTeta*dTeta; sTeta+=cTeta*dTeta;
   DrawArea(Canvas,true);
 }}
 else{
//  DrawCurve3D(Canvas,ColorGraph);
  Timer3D->Enabled=true;
  Timer3D->Interval=Round(1000/Kkadr);
}}
//---------------------------------------------------------------------------
void __fastcall TFormKinema::Timer3DTimer(TObject *Sender){
 double
  dFi  =2*M_PI*Timer3D->Interval/15000.0,
  dTeta=2*M_PI*Timer3D->Interval/25000.0,
  dPsi =2*M_PI*Timer3D->Interval/5000.0;
 DrawCurve3D(Image->Canvas,ColorPhon);
 DrawBar3D(Image->Canvas,false);
 cFi-=sFi*dFi; sFi+=cFi*dFi; cPsi-=sPsi*dPsi; sPsi+=cPsi*dPsi;
 cTeta-=sTeta*dTeta; sTeta+=cTeta*dTeta;
 c11=cPsi*cTeta*cFi-sPsi*sFi; c12=-cPsi*cTeta*sFi-sPsi*cFi; c13=cPsi*sTeta;
 c21=sPsi*cTeta*cFi+cPsi*sFi; c22=-sPsi*cTeta*sFi+cPsi*cFi; c23=sPsi*sTeta;
 c31=-sTeta*cFi; c32=sTeta*sFi; c33=cTeta;
 DrawCurve3D(Image->Canvas,ColorGraph);
 DrawBar3D(Image->Canvas,true);
}
//---------------------------------------------------------------------------
void __fastcall TFormKinema::Godograph3D(TCanvas *Canvas){
 Metrica(Canvas,NULL);
 if(PRINTHEAD) DrawHead(Canvas,Head);
 CGauge->ShowText=true;
 if(DefMSTGodo3D()){
  SelectSizeFont(Canvas);
  if(PRINTABL) DrawTabl(Canvas,NULL);
  Fi=Psi=Teta=0;
  for(Intg *I=L->I;I;I=I->Sled){ //подготовим интегралы
   if(I->x==L->Time&&I->b.V==L->Time){
    I->B=Value(I->a); I->Val=0.0;
  }}
  PrepDraw3D(Canvas);
  if(IsNoCalc){
   swprintf(Inf,L"Не для всех значений времени %s\n"
    L"из указанного в инструкции \"%s\"\n"
    L"диапазона [%g,%g] получено решение!",
    L->Time->Name,InsTime->Name,Tn,Tk);
   Application->MessageBox(Inf,L"Внимание!",MB_OK|MB_ICONERROR);
 }}
 CGauge->ShowText=false;
}
//---------------------------------------------------------------------------
bool __fastcall TFormKinema::DrawGraf(TCanvas *Canvas){
 int Sk1,Sk2,Zap; Root *I; Parm *P; Form F;
 Name=TabControl->Tabs->Strings[TabControl->TabIndex];
 Sk1=Name.Pos(L"(L"); Sk2=Name.Pos(L")"); Zap=Name.Pos(L",L");
 RazmX=RazmY=L"";
 if(Sk1){
  GodoGraf=true;
  Head=TextFromFile(WORKDIR,L"EditKiDyM",&SLK,157); Head+=L" "; Head+=Name;
  if(Zap){
   ToolButtonToBMP->Enabled=false; ToolButtonInclude->Enabled=false;
   ToolButtonRead->Enabled=false; ToolButtonGame->Enabled=!SHLEIF;
   NameZ=Name.SubString(1,Sk1-1);
   NameX=Name.SubString(Sk1+1,Zap-1-Sk1);
   NameY=Name.SubString(Zap+1,Sk2-1-Zap);
   FindVary(NameZ,&VgZ); FindVary(NameY,&VgY); FindVary(NameX,&VgX);
   if(VarInForm(true,VgX->Znach,L->Time)){
    VgXt=DifBy(VgX->Znach,L->Time);
    VgXtt=DifBy(VgXt,L->Time);
   }
   else{
    if(((I=FindInstLast(L"ДИАПАЗОН"))||(I=FindInstLast(L"ДИАПАЗОНЫ")))&&
         FindParm(I->First,VgX->Name,&P)){
     ToolButtonRot->Visible=true;
     ToolButtonTable->Enabled=false;
	 ToolButtonPrint->Enabled=false;
     tbFurie->Enabled=false;
     tbSpline->Enabled=false; VgX->P=0;
     if(P->R&&(KolElem(P->R)==2)){
      X_n=P->R->Nam ? Val(StrToForm(P->R->Nam)) : P->R->Val;
      X_k=P->R->Sled->Nam ? Val(StrToForm(P->R->Sled->Nam)) : P->R->Sled->Val;
     }
     else if(P->Sled&&P->Sled->Sled){ P=P->Sled;
      X_n=P->Nam ? Val(StrToForm(P->Nam)) : P->Val; P=P->Sled;
      X_k=P->Nam ? Val(StrToForm(P->Nam)) : P->Val;
     }
     else{
      swprintf(Inf,L"Нельзя получить значения для границ диапазона\n"
       L"изменения переменной \"%s\" в инструкции %s",VgX->Name,I->Name);
      Application->MessageBox(Inf,L"Ошибка",MB_OK);
      return false;
   }}}
   if(VarInForm(true,VgY->Znach,L->Time)){
    VgYt=DifBy(VgY->Znach,L->Time);
    VgYtt=DifBy(VgYt,L->Time);
   }
   else{
    if(((I=FindInstLast(L"ДИАПАЗОН"))||(I=FindInstLast(L"ДИАПАЗОНЫ")))&&
         FindParm(I->First,VgY->Name,&P)){
	 VgY->P=0;
     if(P->R&&(KolElem(P->R)==2)){
      Y_n=P->R->Nam ? Val(StrToForm(P->R->Nam)) : P->R->Val;
      Y_k=P->R->Sled->Nam ? Val(StrToForm(P->R->Sled->Nam)) : P->R->Sled->Val;
     }
     else if(P->Sled&&P->Sled->Sled){ P=P->Sled;
      Y_n=P->Nam ? Val(StrToForm(P->Nam)) : P->Val; P=P->Sled;
      Y_k=P->Nam ? Val(StrToForm(P->Nam)) : P->Val;
     }
     else{
      swprintf(Inf,L"Нельзя получить значения для границ диапазона\n"
       L"изменения переменной \"%s\" в инструкции %s",VgX->Name,I->Name);
      Application->MessageBox(Inf,L"Ошибка",MB_OK);
      return false;
   }}}
   if(VarInForm(true,VgZ->Znach,L->Time)){
    VgZt=DifBy(VgZ->Znach,L->Time);
    VgZtt=DifBy(VgZt,L->Time);
   }
   Godograph3D(Canvas);
  }
  else{
   Timer3D->Enabled=false; ToolButtonGame->Enabled=true;
   NameY=Name.SubString(1,Sk1-1);
   NameX=Name.SubString(Sk1+1,(Sk2?Sk2:Name.Length())-Sk1-1);
   if(!Sk2) Head+=L")";
   FindVary(NameX,&VgX); F.V=VgX;
   if(VgX->Razm.C!=Pust) FormToStr(VgX->Razm,&RazmX); else RazmX=L"";
   FindVary(NameY,&VgY); F.V=VgY;
   if(VgY->Razm.C!=Pust) FormToStr(VgY->Razm,&RazmY); else RazmY=L"";
   if(VgX->Znach==Pust){
    double Tno=Tn,Tko=Tk; Parm *P; int PP=VgX->P; Root *I;
    if(((I=FindInstLast(L"ДИАПАЗОН"))||(I=FindInstLast(L"ДИАПАЗОНЫ")))&&
       FindParm(I->First,VgX->Name,&P)){
     if(P->R&&(KolElem(P->R)==2)){
      Tn=P->R->Nam ? Val(StrToForm(P->R->Nam)) : P->R->Val;
      Tk=Tend=P->R->Sled->Nam ? Val(StrToForm(P->R->Sled->Nam)) : P->R->Sled->Val;
     }
     else if(P->Sled&&P->Sled->Sled){ P=P->Sled;
      Tn=P->Nam ? Val(StrToForm(P->Nam)) : P->Val; P=P->Sled;
      Tk=Tend=P->Nam ? Val(StrToForm(P->Nam)) : P->Val;
     }
     else{
      swprintf(Inf,L"Нельзя получить значения для границ диапазона\n"
       L"изменения переменной \"%s\" в инструкции %s",VgX->Name,I->Name);
      Application->MessageBox(Inf,L"Ошибка",MB_OK);
      return false;
     }
	 ArgTime=false; VgX->P=0;
     Godograph(Canvas);
     Tn=Tno; Tk=Tend=Tko; VgX->P=PP;
     ArgTime=true;
    }
    else{
     swprintf(Inf,L"Не заданы границы диапазона изменения\n"
      L"переменной \"%s\" инструкцией \"ДИАПАЗОН[Ы]\"",VgX->Name);
     Application->MessageBox(Inf,L"Ошибка",MB_OK);
     return false;
   }}
   else{
    VgXt=DifBy(VgX->Znach,L->Time); VgYt=DifBy(VgY->Znach,L->Time);
    VgXtt=DifBy(VgXt,L->Time); VgYtt=DifBy(VgYt,L->Time);
    Godograph(Canvas);
 }}}
 else{
  Timer3D->Enabled=false; ToolButtonGame->Enabled=false;
  Head=TextFromFile(WORKDIR,L"EditKiDyM",&SLK,156); Head+=L" ";
  Head+=TabControl->Tabs->Strings[TabControl->TabIndex];
  Head+=L"(t)";
  FindVary(Name,&Vp);
  NameX=L->Time->Name; NameY=Name;
  if(L->Time->Razm.C&&(L->Time->Razm.C!=Pust)) FormToStr(L->Time->Razm,&RazmX);
  F.V=Vp;
  if(Vp->Razm.C!=Pust) FormToStr(Vp->Razm,&RazmY);
  GrafFunc(Canvas);
 }
 return true;
}
//---------------------------------------------------------------------------
void __fastcall TFormKinema::SravnFunc(TCanvas *Canvas,Magazine *Names){
 Metrica(Canvas,Names);
 if(PRINTHEAD) DrawHead(Canvas,Head);
 if(KP[0]==1) return;
 if(DefMSTGraf(Names)){
  SelectSizeFont(Canvas);
  DrawSetka(Canvas,Names);
  if(PRINTABL) DrawTabl(Canvas,Names);
  DrawGrafik(Canvas,Names);
  if(IsNoCalc){
   swprintf(Inf,L"Не для всех значений времени %s\n"
    L"из указанного в инструкции \"%s\"\n"
    L"диапазона [%g,%g] получено решение!",
    L->Time->Name,InsTime->Name,Tn,Tk);
   Application->MessageBox(Inf,L"Внимание!",MB_OK|MB_ICONERROR);
}}}
//---------------------------------------------------------------------------
void __fastcall TFormKinema::SravnGraf(TCanvas *Canvas,wchar_t *Name){
 wchar_t *s,*s0; Magazine *m,**Pm=&Names;
 s=wcschr(Name,'('); *s++='\0'; Head=Name; Head+=L": ";
 while(s0=wcschr(s,',')){
  *Pm=m=(Magazine *)calloc(1,sizeof(Magazine)); Pm=(Magazine **)m;
  *s0++='\0';
  m->S=wcscpy((wchar_t *)malloc(SzC*(wcslen(s)+1)),s); s=s0;
  Head+=m->S; Head+=L", ";
 }
 if(s0=wcschr(s,')')){
  *Pm=m=(Magazine *)calloc(1,sizeof(Magazine)); Pm=(Magazine **)m;
  if(s0[1]==')') s0++; *s0='\0';
  m->S=wcscpy((wchar_t *)malloc(SzC*(wcslen(s)+1)),s);
  Head+=m->S;
 }
 if(!(s=wcschr(Names->S,'('))){
  NameX=L->Time->Name;
  if(L->Time->Razm.C!=Pust) FormToStr(L->Time->Razm,&RazmX);
 }
 else{ s++; if(s0=wcschr(s,')')) *s0='\0'; NameX=s; *s0=')'; }
 SravnFunc(Canvas,Names);
 DelAllMagazine(&Names); //if(Tabl){ free(Tabl); Tabl=NULL; }
}
//---------------------------------------------------------------------------
void SaveTabl(void){
//Таблица:=BaseData20.06.txt,t,N,
// epsilon°$R,nu°$R,beta°$R,gamma°$R,teta°$R,alfa°$R,epsilon°$L,nu°$L,beta°$L,
//gamma°$L,teta°$L,alfa°$L,X@t6R,Y@t6R,Z@t6R,X@t6L,Y@t6L,Z@t6L,X@AR,Y@AR,Z@AR,
//X@CR,Y@CR,Z@CR,X@AL,Y@AL,Z@AL,X@CL,Y@CL,Z@CL;
 wchar_t File[256],FileNew[256],FileOld[256],*inf,*typ; int N,i,n; FILE *Fi,*Fo;
 Root *I=FindInstLast(L"ТАБЛИЦА"); Parm *P; Vary *V; double t;
 swprintf(File,L"%s\\%s",CurrDir,I->First->Nam);
 P=I->First->Sled;
 if(!P->Nam) t=P->Val; else if(P->Nam&&FindVary(P->Nam,&V)) t=Value(V->Znach);
 L->Time->P=0; L->Time->Val=t; ClearVar();
 P=P->Sled;
 FindVary(P->Nam,&V); P=P->Sled;
 N=Round(Value(V->Znach));
 if(_waccess(File,0)){//нет файла с таблицей
  Fo=_wfopen(File,L"w");
  fwprintf(Fo,L"%2.2d\xb3",N);
  for(;P;P=P->Sled){
   FindVary(P->Nam,&V);
   fwprintf(Fo,L"%12.8f%c",Val(V->Znach),P->Sled?'\xb3':'\n');
  }
  for(Cord *q=L->K;q;q=q->Sled){
   fwprintf(Fo,L"%s=%12.8f; ",q->V->Name,q->V->Val);
  }
  fclose(Fo);
 }
 else{
  typ=wcsrchr(File,'.'); *typ='\0';
  wcscpy(FileNew,File); wcscpy(FileOld,File);
  *typ='.';
  wcscat(FileNew,L".new"); wcscat(FileOld,L".old");
  Fi=_wfopen(File,L"r"); Fo=_wfopen(FileNew,L"w");
  for(i=1;i<N;i++)
   if(fgetws(Inf,1023,Fi)&&((n=_wtoi(Inf))<N))
    fputws(Inf,Fo);
  fwprintf(Fo,L"%2.2d\xb3",N);
  for(;P;P=P->Sled){
   FindVary(P->Nam,&V);
   fwprintf(Fo,L"%12.8f%c",Val(V->Znach),P->Sled?'\xb3':'\n');
  }
  if(n>N) fputws(Inf,Fo);
  while(fgetws(Inf,1023,Fi)) if((n=_wtoi(Inf))>N) fputws(Inf,Fo);
  for(Cord *q=L->K;q;q=q->Sled){
   fwprintf(Fo,L"%s=%12.8f; ",q->V->Name,q->V->Val);
  }
  fclose(Fi); fclose(Fo);
  _wunlink(FileOld);
  _wrename(File,FileOld); _wrename(FileNew,File);
 }
 TABLTOFILE=false;
}
//---------------------------------------------------------------------------
void SaveOK(void){
//ОБОБЩЕННЫЕ КООРДИНАТЫ В ФАЙЛ := OK_01.08.txt,tn,N;
 wchar_t File[256],FileNew[256],FileOld[256],*inf,*typ; int N,i,n; FILE *Fi,*Fo;
 Root *I=FindInstLast(L"ОБОБЩЕННЫЕ КООРДИНАТЫ В ФАЙЛ"); Parm *P; Vary *V;
 double t,d; Magazine *M;
 swprintf(File,L"%s\\%s",CurrDir,I->First->Nam);
 P=I->First->Sled;
 if(!P->Nam) t=P->Val; else if(P->Nam&&FindVary(P->Nam,&V)) t=Value(V->Znach);
 L->Time->P=0; L->Time->Val=t; ClearVar();
 P=P->Sled;
 FindVary(P->Nam,&V); P=P->Sled;
 N=Round(Value(V->Znach));
 if(_waccess(File,0)){//нет файла со значениями обобщенных координат
  Fo=_wfopen(File,L"w");
  fwprintf(Fo,L"%2.2d; ",N);
  for(Cord *q=L->K;q;q=q->Sled){
   Vp=q->V; d=Func(t);
//   if(ALL_ANGLES||FindMagazine(Mangles,Vp->Name,&M))
//    d=NormAngle(d);
   fwprintf(Fo,L"%s=%12.8f; ",Vp->Name,d);
  }
  fputwc('\n',Fo);
  fclose(Fo);
 }
 else{
  typ=wcsrchr(File,'.'); *typ='\0';
  wcscpy(FileNew,File); wcscpy(FileOld,File);
  *typ='.';
  wcscat(FileNew,L".new"); wcscat(FileOld,L".old");
  Fi=_wfopen(File,L"r"); Fo=_wfopen(FileNew,L"w");
  for(i=1;i<N;i++)
   if(fgetws(Inf,1023,Fi)&&((n=_wtoi(Inf))<N))
    fputws(Inf,Fo);
  fwprintf(Fo,L"%2.2d; ",N);
  for(Cord *q=L->K;q;q=q->Sled){
   Vp=q->V; d=Func(t);
//   if(ALL_ANGLES||Mangles&&FindMagazine(Mangles,Vp->Name,&M))
//    d=NormAngle(d);
   fwprintf(Fo,L"%s=%12.8f; ",Vp->Name,d);
  }
  fputwc('\n',Fo);
  if(n>N) fputws(Inf,Fo);
  while(fgetws(Inf,1023,Fi)) if((n=_wtoi(Inf))>N) fputws(Inf,Fo);
  fclose(Fi); fclose(Fo);
  _wunlink(FileOld);
  _wrename(File,FileOld); _wrename(FileNew,File);
 }
 OKTOFILE=false;
}
//---------------------------------------------------------------------------
void __fastcall TFormKinema::TabControlChange(TObject *Sender){
 int Ind=-1; wchar_t *s2,*s3; TRect R=Canvas->ClipRect;
 if(TabControl->TabIndex==-1){
//  MReportClick(NULL);
  return;
 }
 ToolButtonToBMP->Enabled=true; ToolButtonTable->Enabled=true;
 ToolButtonPrint->Enabled=true; ToolButtonInclude->Enabled=false;
 Timer3D->Enabled=false; if(Tabl){ free(Tabl); Tabl=NULL; }
 if(MultFilms) MultFilms->TimerMultFilm->Enabled=false;
 IsNoCalc=false; NameZ=L"";
 if(Gamer) ToolButtonGameClick(Sender);
 if(Reader) ToolButtonReadClick(Sender);
 if(Image) delete Image; Image=new TImage(TabControl);
 Image->Parent=TabControl;
 Image->Align=alClient;
 Image->Canvas->Font->Assign(FontDialog->Font);
 Image->OnMouseDown=FormKinema->ImageGraphMouseDown;
 Metrica(Image->Canvas,NULL);
 if(!IsSyntError&&((TabControl->TabIndex>Ind))){
// if(!IsSyntError&&((TabControl->TabIndex>Ind)||
//     wcscmp(BrowserName,L"Встроенный браузер"))){
  Magazine *m; wchar_t Name[256];
  RESIZE=true;
  if(RichEditInp->Visible) RichEditInp->Visible=false;
  CppWebBrowser->Align=alNone;
  CppWebBrowser->Height=0; CppWebBrowser->Width=0;
  if(!HTMLREPORT){
   ScrollBox->Visible=false;
   DataKDMPaintBox->Visible=false;
  }
  PoleLeft=PoleRight=PoleTop=PoleBottom=0;
  pmColors->Visible=true; pmDiskret->Visible=true;
  wcscpy(Name,TabControl->Tabs->Strings[TabControl->TabIndex].c_str());
  ToolButtonToBMP->Enabled=!FindMagazine(NamesBMP,Name,&m);
  ToolButtonInclude->Enabled=!FindMagazine(NamesInclude,Name,&m);
  ToolButtonTable->Enabled=!FindMagazine(NamesTAB,Name,&m);
  tbFurie->Enabled=!FindMagazine(NamesFurie,Name,&m);
  tbSpline->Enabled=!FindMagazine(NamesSpline,Name,&m);
  swprintf(Inf,L"%d",TabControl->TabIndex);
  Image->Visible=true;
  if(INVERSE){
   if(!LAGRANGE){
    if(!Q) CalcQSplns();
	if(NoCalc) return;
    swprintf(Inf,L"%d",TabControl->TabIndex);
    if(FindMagazine(Sravn,Inf,&m)||FindMagazine(Sovm,Inf,&m)){
     ToolButtonRead->Enabled=true;
     SravnGraf(Image->Canvas,Name);
     ToolButtonGame->Enabled=GodoGraf;
    }
    else{
     ToolButtonInclude->Enabled=!FindMagazine(NamesInclude,Name,&m);
     ToolButtonToBMP->Enabled=!FindMagazine(NamesBMP,Name,&m);
     ToolButtonTable->Enabled=!FindMagazine(NamesTAB,Name,&m);
     tbFurie->Enabled=!FindMagazine(NamesFurie,Name,&m);
     tbSpline->Enabled=!FindMagazine(NamesSpline,Name,&m);
     ToolButtonRead->Enabled=true;
     DrawGraf(Image->Canvas);
     ToolButtonGame->Enabled=true;
   }}
   if(OKTOFILE) SaveOK();
  }
  else{
   if(FindMagazine(Sravn,Inf,&m)||FindMagazine(Sovm,Inf,&m)){
    ToolButtonRead->Enabled=true;
    SravnGraf(Image->Canvas,Name);
    ToolButtonGame->Enabled=GodoGraf;
   }
   else{ Root *I;//Vary *Vsy2; Form Fsy2; double D;
    ToolButtonRead->Enabled=true;
    GodoGraf=false;
    if(!DrawGraf(Image->Canvas)){ TabControl->TabIndex=0; goto M; }
  }}
  if(TABLTOFILE) SaveTabl();
 }
 else{
M:ToolButtonToBMP->Enabled=false; ToolButtonTable->Enabled=false;
  ToolButtonRead->Enabled=false;  ToolButtonGame->Enabled=false;
  pmColors->Visible=false; pmDiskret->Visible=false;
  ToolButtonPrint->Enabled=false;
  tbSpline->Enabled=false; tbFurie->Enabled=false;
  switch(TabControl->TabIndex){
   case 0://отчет
    if(IsSyntError){//синт.ошибки->показываем исходник
     if(!RichEditInp->Text.IsEmpty()){
      ToolButtonPrint->Enabled=true;
      if(HTMLREPORT){
       CppWebBrowser->Align=alNone;
       CppWebBrowser->Height=0; CppWebBrowser->Width=0;
      }
      else{
	   ScrollBox->Visible=false;
       DataKDMPaintBox->Visible=false;
      }
      RichEditInp->Visible=true; RichEditInp->SetFocus();
    }}
    else{//отчет
     if(RichEditInp->Visible) RichEditInp->Visible=false;
     CppWebBrowser->Align=alNone;
     CppWebBrowser->Height=0; CppWebBrowser->Width=0;
	 MReportClick(NULL);
    }
    break;
}}}
//---------------------------------------------------------------------------
void __fastcall TFormKinema::ToolButtonTableClick(TObject *Sender){
 wchar_t Name[256]; bool Godograf; Magazine *m;
 wcscpy(Name,TabControl->Tabs->Strings[TabControl->TabIndex].c_str());
 swprintf(Inf,L"%d",TabControl->TabIndex);
 if(FindMagazine(Sravn,Inf,&m)||FindMagazine(Sovm,Inf,&m)) Godograf=false;
 else Godograf=wcschr(Name,'(')?true:false;
 TablToReport(Name,::L->Time->Name,CountTabl,Tn,Tk,Godograf,Funcs,&NamesInclude,
  &NamesTAB,&NamesSpline,&NamesFurie);
 ToolButtonTable->Enabled=false;
 CppWebBrowser->Navigate(HtmFile);
}
//---------------------------------------------------------------------------
void __fastcall TFormKinema::ToolButtonPrintClick(TObject *Sender){
 Application->CreateForm(__classid(TFormParamPrint),&FormParamPrint);
 if(TabControl->TabIndex>(1+!_waccess(OprFile,0))){
  Prntr=Printer(); Magazine *m;
  int LPx=GetDeviceCaps(Prntr->Handle,LOGPIXELSX),
      LPy=GetDeviceCaps(Prntr->Handle,LOGPIXELSY);
  int XK=Xk,YK=Yk; double xK=xk,yK=yk[0],MSt0=MSt[0],MSt1=MSt[1];
  PoleLeft=Round((double)(FormParamPrint->MarginLeftmm*LPx)/25.4);
  PoleRight=Round((double)(FormParamPrint->MarginRightmm*LPx)/25.4);
  PoleTop=Round((double)(FormParamPrint->MarginTopmm*LPy)/25.4);
  PoleBottom=Round((double)(FormParamPrint->MarginBotmm*LPy)/25.4);
  Prntr->BeginDoc();
  swprintf(Inf,L"%d",TabControl->TabIndex);
  if(FindMagazine(Sravn,Inf,&m)||FindMagazine(Sovm,Inf,&m)){
   SravnGraf(Prntr->Canvas,
    TabControl->Tabs->Strings[TabControl->TabIndex].c_str());
  }
  else
   DrawGraf(Prntr->Canvas);
  Prntr->EndDoc();
  Xk=XK; Yk=YK; xk=xK; yk[0]=yK; MSt[0]=MSt0; MSt[1]=MSt1;
 }
 else{
  FormParamPrint->FilePrn=InpFile;
  FormParamPrint->TakeParam();
  Application->CreateForm(__classid(TFormPrintRichEdit),&FormPrintRichEdit);
  RichEdit=RichEditInp;
  FormPrintRichEdit->FileKDM=InpFile;
  FormPrintRichEdit->ShowModal();
}}
//---------------------------------------------------------------------------
void __fastcall TFormKinema::ToolButtonToBMPClick(TObject *Sender){
 if((TabControl->TabIndex<0)/*&&Image*/){
  MultFilms->ToBMP(TabControl,true); return;
 }
  wchar_t File[256],*s,Inf[256],Name[256]; Magazine *m;
  int XK=Xk,YK=Yk; double xK=xk,yK=yk[0],MSt0=MSt[0],MSt1=MSt[1];
  if(ImageBMP) delete ImageBMP; ImageBMP=new TImage(FormKinema);
  ImageBMP->Parent=TabControl; ImageBMP->Visible=false;
  ImageBMP->Height=Round((double)ClientRect.Height()/Kbmp);
  ImageBMP->Width=Round((double)ClientRect.Width()/Kbmp);
  ImageBMP->Canvas->Font->Assign(FontDialog->Font);
  wcscpy(File,TabControl->Tabs->Strings[TabControl->TabIndex].c_str());
  swprintf(BMPFile,L"%s\\%s.BMP",CurrDir,File);
  for(int i=1;!_waccess(BMPFile,0);i++){
   if(*CurrDir) swprintf(BMPFile,L"%s\\%s№%d.BMP",CurrDir,File,i);
   else swprintf(BMPFile,L"%s№%d.BMP",File,i);
  }
  PoleLeft=PoleRight=PoleTop=PoleBottom=0;
  wcscpy(Name,TabControl->Tabs->Strings[TabControl->TabIndex].c_str());
  TakeMagazine(&NamesBMP,Name);
  swprintf(Inf,L"%d",TabControl->TabIndex);
  if(FindMagazine(Sravn,Inf,&m)||FindMagazine(Sovm,Inf,&m))
   SravnGraf(ImageBMP->Canvas,Name);
  else DrawGraf(ImageBMP->Canvas);
  ImageBMP->Picture->SaveToFile(BMPFile);
  TJPEGImage *jp = new TJPEGImage();
  jp->Assign(ImageBMP->Picture->Bitmap);
  wcscpy(JPGFile,BMPFile);
  if(s=wcsrchr(JPGFile,'.')) wcscpy(s+1,L"JPG");
  else wcscat(JPGFile,L".JPG");
  jp->SaveToFile(JPGFile);
  delete jp;
  if(Sender){
   swprintf(Inf,
    L"График сохранен в файлах\n\"%s\" и\n\"%s\".\n"
    L"Их можно просмотреть и отредактировать соответствующим\n"
    L"графическим редактором, вставить в документ WORD и т.д.",
    BMPFile,JPGFile);
   Application->MessageBox(Inf,FormKinema->Caption.c_str(),MB_OK);
  }
  ToolButtonToBMP->Enabled=false;
  Xk=XK; Yk=YK; xk=xK; yk[0]=yK; MSt[0]=MSt0; MSt[1]=MSt1;
}//}
//---------------------------------------------------------------------------
void __fastcall TFormKinema::ToolButtonHelpClick(TObject *Sender){
 wchar_t Help[256],*s; wcscpy(Help,WORKDIR);
 if(s=wcsrchr(Help,'\\')) wcscpy(s+1,L"Help\\KinemaHelp.htm");
 else wcscat(Help,L"\\Help\\KinemaHelp.htm");
 if(_waccess(Help,0)){
  swprintf(Inf,L"Не найден файл помощи\n\"%s\"",Help);
  Application->MessageBox(Inf,L"Системная ошибка!",MB_OK);
  return;
 }
 Application->CreateForm(__classid(TFormOtchet),&FormOtchet);
 FormOtchet->cbURL->Text=Help;
 FormOtchet->ShowModal();
}
//---------------------------------------------------------------------------
void __fastcall TFormKinema::FormKeyUp(TObject *Sender,WORD &Key,TShiftState Shift){
 switch(Key){
  case 112://F1
   ToolButtonHelpClick(Sender); break;
  case 'P':
   if(Shift.Contains(ssCtrl)) ToolButtonPrintClick(Sender);
}}
//---------------------------------------------------------------------------
void __fastcall TFormKinema::ToolButtonAboutClick(TObject *Sender){
 if(!NameZ.IsEmpty()) Timer3D->Enabled=false;
 Application->CreateForm(__classid(TAboutFormKin),&AboutFormKin);
 AboutFormKin->ShowModal();
 if(!NameZ.IsEmpty()) Timer3D->Enabled=true;
}
//---------------------------------------------------------------------------
void __fastcall TFormKinema::EditCountTabExit(TObject *Sender){
 short OldPos=UpDownCountTab->Position;
 if(!EditCountTab->Text.IsEmpty()){
  short NewPos=_wtoi(EditCountTab->Text.c_str());
  if(NewPos<UpDownCountTab->Min){
   EditCountTab->Text=OldPos; return;
  }
  UpDownCountTab->Position=NewPos;
 }
 else{
  EditCountTab->Text=OldPos;
  EditCountTab->SetFocus();
}}
//---------------------------------------------------------------------------
void __fastcall TFormKinema::EditCountTabKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift){
 if(Key==VK_RETURN) EditCountTabExit(Sender);
 if(Key==VK_ESCAPE){
  GroupBox->Visible=false; UpDownCountTab->Position=CountTabl;
  if(Image->Visible) TabControl->SetFocus();
  else
   RichEditInp->SetFocus();
}}
//---------------------------------------------------------------------------
void __fastcall TFormKinema::EditCountTabKeyPress(TObject *Sender, wchar_t &Key){
 if(isdigit(Key)){
  AnsiString Text=EditCountTab->Text; Text+=Key;
  if(Text.ToInt()>UpDownCountTab->Max) Key='\0';
  return;
 }
 if((Key!=VK_BACK)&&(Key!=VK_ESCAPE)&&(Key!=VK_RETURN))
  Key='\0';
}
//---------------------------------------------------------------------------
void __fastcall TFormKinema::FormPaint(TObject *Sender){
 CppWebBrowser->Align=alNone;
 CppWebBrowser->Width=0; CppWebBrowser->Height=0;
 Canvas->Font->Assign(FontDialog->Font);
 if(TabControl&&TabControl->Visible)
  TabControlChange(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TFormKinema::TimerGamesTimer(TObject *Sender){
// if((!GodoGraf||NameZ.IsEmpty())&&!SHLEIF)
 int K=KolElem(Tabl),SOVM=0; Magazine *m;
 swprintf(Inf,L"%d",TabControl->TabIndex);
 if(FindMagazine(Sovm,Inf,&m)) SOVM=1;
 if(!SHLEIF){
  if(Tabl){
   Vp=Tabl[0].Vx; xdt=Func(tt);
   Vp=Tabl[0].Vy; ydt=Func(tt);
  }
  else{
   xdt=FuncX(0,tt);
   ydt=FuncY(0,tt);
  }
  xt=Xk/*+1*/+Round((xdt-xk)/MSt[0]);
  yt=Yk/*+1*/-Round((ydt-yk[SOVM])/MSt[1+SOVM]);
  Image->Canvas->Rectangle(xt-3,yt-3,xt+3,yt+3);
  if(K>1){
   for(int i=1;i<K;i++){
    if(Vp=Tabl[i].Vx) xdt=Func(tt);
    Vp=Tabl[i].Vy; ydt=Func(tt);
    xt=Xk+(int)Round((xdt-xk)/MSt[0]);
    yt=Yk-(int)Round((ydt-yk[SOVM])/MSt[1+SOVM]);
    Image->Canvas->Rectangle(xt-3,yt-3,xt+3,yt+3);
 }}}
 if(tt==Tk){
  tt=Tn; TimerGames->Interval=Round(1000/Kkadr);
 }
 else{
  tt+=dtt; if(tt>=Tk){ tt=Tk; TimerGames->Interval=2000; }
 }
 ClearVar(); L->Time->Val=tt;
 if(GodoGraf){
  if(NameZ.IsEmpty()){
   if(Tabl){
    Vp=Tabl[0].Vx; xdt=Func(tt);
    Vp=Tabl[0].Vy; ydt=Func(tt);
   }
   else{
    xdt=FuncX(0,tt);
    ydt=FuncY(0,tt);
   }
   xt=Xk/*+1*/+Round((xdt-xk)/MSt[0]);
   yt=Yk/*+1*/-Round((ydt-yk[SOVM])/MSt[1+SOVM]);
   swprintf(Inf,L"Время: %2.2d:%2.2d:%2.2d,%4.4d с;    %s: %15.5f;    "
    L"%s: %15.5f    Быстрота=%0.4f",
    (int)(tt/3600),((int)tt)%3600/60,((int)tt)%3600%60,
    (int)(10000*(tt-floor(tt))),VgX->Name,xdt,VgY->Name,ydt,KTime);
  }
  else{
   FuncXYZ(tt,xdt,ydt,zdt); ToScreen(xdt,ydt,zdt,xt,yt);
   swprintf(Inf,L"Время: %2.2d:%2.2d:%2.2d,%4.4d с;    %s: %15.5f;    "
    L"%s: %15.5f    %s: %15.5f    Быстрота=%0.4f",
    (int)(tt/3600),((int)tt)%3600/60,((int)tt)%3600%60,
    (int)(10000*(tt-floor(tt))),VgX->Name,xdt,VgY->Name,ydt,VgZ->Name,zdt,KTime);
 }}
 else{
  xdt=tt; xt=Xk/*+1*/+Round((xdt-xk)/MSt[SOVM]);
  ydt=Func(tt); yt=Yk/*+1*/-Round((ydt-yk[0])/MSt[1+SOVM]);
  swprintf(Inf,L"Время: %2.2d:%2.2d:%2.2d,%4.4d с;    %s: %15.5f,L"
   L"    Быстрота=%0.4f",
   (int)(tt/3600),((int)tt)%3600/60,((int)tt)%3600%60,
   (int)(10000*(tt-floor(tt))),
   Vp->Name,ydt,KTime);
 }
 if(SHLEIF){
  if(tt==Tn){
   Image->Canvas->Pen->Width=1;
   if(NameZ.IsEmpty())
    DrawSetka(Image->Canvas,NULL);
   Image->Canvas->Pen->Width=WidthPen;
   Image->Canvas->Pen->Color=ColorGraph;
   Image->Canvas->MoveTo(xt,yt);
  }
  else{
   Image->Canvas->LineTo(xt,yt); Nkadr++;
   if(NameZ.IsEmpty()&&(Nkadr<=Kt)){
    if(TOBMP){
     if(Kinst>1)
      swprintf(BMPKadrs,L"%s\\%s(?%d)\\%3.3d.bmp",CurrDir,BMPKadr,Kinst,Nkadr);
     else
      swprintf(BMPKadrs,L"%s\\%s\\%3.3d.bmp",CurrDir,BMPKadr,Nkadr);
     Image->Picture->SaveToFile(BMPKadrs);
    }
    if(TOJPEG){
     if(JPEG) delete JPEG; JPEG = new TJPEGImage();
     if(Kinst>1)
      swprintf(JPGKadrs,L"%s\\%s(?%d)\\%3.3d.jpg",CurrDir,BMPKadr,Kinst,Nkadr);
     else
      swprintf(JPGKadrs,L"%s\\%s\\%3.3d.jpg",CurrDir,BMPKadr,Nkadr);
     JPEG->Assign(Image->Picture->Bitmap);
     JPEG->SaveToFile(JPGKadrs);
 }}}}
 else{
  Image->Canvas->Rectangle(xt-3,yt-3,xt+3,yt+3);
  if(K>1){
   for(int i=1;i<K;i++){
    if(Vp=Tabl[i].Vx) xdt=Func(tt);
    Vp=Tabl[i].Vy; ydt=Func(tt);
    xt=Xk+(int)Round((xdt-xk)/MSt[0]);
    yt=Yk-(int)Round((ydt-yk[SOVM])/MSt[1+SOVM]);
    Image->Canvas->Rectangle(xt-3,yt-3,xt+3,yt+3);
 }}}
 StatusBar->Canvas->Brush->Color=clBtnFace;
 StatusBar->Canvas->FillRect(StatusBar->Canvas->ClipRect);
 DrawGrekText(StatusBar->Canvas,StatusBar->Left,StatusBar->Top-4,StatusBar->Width,
  Inf,SimbInd);
}
//---------------------------------------------------------------------------
void __fastcall TFormKinema::ToolButtonGameClick(TObject *Sender){
 Kt=(int)Round(0.001*Kkadr*TMult);
 if(Kt==0){
  swprintf(Inf,
   L"Увеличьте значение параметра \"Длительность анимации\"\n"
   L"или \"Кадров в секунду\" в НАСТРОЙКЕ");
  Application->MessageBox(Inf,L"Ошибка!",MB_OK|MB_ICONERROR); return;
 }
 if(Reader) ToolButtonReadClick(Sender);
 Gamer=!Gamer; ToolButtonGame->Down=Gamer;
 UpDownKTime->Enabled=Gamer;
 if(!NameZ.IsEmpty()) Timer3D->Enabled=!Gamer;
 TimerGames->Enabled=Gamer;
 if(Gamer){
  if(SHLEIF){
   if(NameZ.IsEmpty())
    DrawSetka(Image->Canvas,NULL);
   Image->Canvas->Pen->Width=WidthPen;
   Image->Canvas->Pen->Color=ColorGraph;
  }
  else
   Image->Canvas->Pen->Mode=pmNot;
  TimerGames->Interval=Round(1000/Kkadr);
  dtt=(Tk-Tn)*KTime*TimerGames->Interval/TMult;
  tt=Tn; L->Time->Val=tt; ClearVar();
  if(GodoGraf){
   if(NameZ.IsEmpty()){
    if(Tabl){
     Vp=Tabl[0].Vx; xdt=Func(tt);
     Vp=Tabl[0].Vy; ydt=Func(tt);
    }
    else{
     xdt=FuncX(0,tt);
     ydt=FuncY(0,tt);
    }
    xt=Xk/*+1*/+Round((xdt-xk)/MSt[0]);
    yt=Yk/*+1*/-Round((ydt-yk[0])/MSt[1]);
	swprintf(Inf,L"Время: %2.2d:%2.2d:%2.2d,%4.4d сек;    %s: %15.5f;    "
     L"%s: %15.5f    Быстрота=%0.4f",
     (int)(tt/3600),((int)tt)%3600/60,((int)tt)%3600%60,
     (int)(10000*(tt-floor(tt))),VgX->Name,xdt,VgY->Name,ydt,KTime);
    swprintf(BMPKadr,L"%s(%s)",VgY->Name,VgX->Name);
   }
   else{
    FuncXYZ(tt,xdt,ydt,zdt); ToScreen(xdt,ydt,zdt,xt,yt);
    swprintf(Inf,L"Время: %2.2d:%2.2d:%2.2d,%4.4d сек;    %s: %15.5f;    "
     L"%s: %15.5f    %s: %15.5f    Быстрота=%0.4f",
     (int)(tt/3600),((int)tt)%3600/60,((int)tt)%3600%60,
     (int)(10000*(tt-floor(tt))),VgX->Name,xdt,VgY->Name,ydt,VgZ->Name,zdt,KTime);
  }}
  else{
   xdt=tt; ydt=Func(tt);
   xt=Xk/*+1*/+Round((xdt-xk)/MSt[0]);
   yt=Yk/*+1*/-Round((ydt-yk[0])/MSt[1]);
   swprintf(Inf,L"Время: %2.2d:%2.2d:%2.2d,%4.4d сек;    %s: %15.5f,L"
    L"    Быстрота=%0.4f",
    (int)(tt/3600),((int)tt)%3600/60,((int)tt)%3600%60,
    (int)(10000*(tt-floor(tt))),Vp->Name,ydt,KTime);
   swprintf(BMPKadr,L"%s(%s)",Vp->Name,L->Time->Name);
  }
  if(SHLEIF){
   Image->Canvas->MoveTo(xt,yt);
   if(NameZ.IsEmpty()&&(TOBMP||TOJPEG)){
    Kinst=KolInst(TextFromFile(WORKDIR,L"EditKiDyM",&SLK,111))+
	 KolInst(TextFromFile(WORKDIR,L"EditKiDyM",&SLK,159))+1;
    Nkadr=0;
    if(TOBMP||TOJPEG){
     if(Kinst>1){
      swprintf(Dir,L"%s\\%s(№%d)",CurrDir,BMPKadr,Kinst);
      if(_waccess(Dir,0)) _wmkdir(Dir);
     }
     else{
      swprintf(Dir,L"%s\\%s",CurrDir,BMPKadr);
      if(_waccess(Dir,0)) _wmkdir(Dir);
    }}
    if(TOBMP&&!_waccess(Dir,0)){
     swprintf(BMPKadrs,L"%s\\%3.3d.bmp",Dir,Nkadr);
     Image->Picture->SaveToFile(BMPKadrs);
    }
    if(TOJPEG&&!_waccess(Dir,0)){
     swprintf(JPGKadrs,L"%s\\%3.3d.jpg",Dir,Nkadr);
     JPEG->Assign(Image->Picture->Bitmap);
     JPEG->SaveToFile(JPGKadrs);
  }}}
  else{
   int K=KolElem(Tabl),SOVM=0; Magazine *m;
   Image->Canvas->Rectangle(xt-3,yt-3,xt+3,yt+3);
   if(K>1){
    swprintf(Inf,L"%d",TabControl->TabIndex);
    if(FindMagazine(Sovm,Inf,&m)) SOVM=1;
    for(int i=1;i<K;i++){
     Vp=Tabl[i].Vx; xdt=Func(tt);
     Vp=Tabl[i].Vy; ydt=Func(tt);
     xt=Xk+(int)Round((xdt-xk)/MSt[0]);
     yt=Yk-(int)Round((ydt-yk[SOVM])/MSt[1+SOVM]);
     Image->Canvas->Rectangle(xt-3,yt-3,xt+3,yt+3);
 }}}}
 else{
  if(!SHLEIF)
   Image->Canvas->Rectangle(xt-3,yt-3,xt+3,yt+3);
  Image->Canvas->Pen->Mode=pmCopy;
  swprintf(Inf,L"%s: %s",HeadVersion[0]?HeadVersion:HeadWork,Autor);
  StatusBar->Canvas->Brush->Color=clBtnFace;
  StatusBar->Canvas->FillRect(StatusBar->Canvas->ClipRect);
  DrawGrekText(StatusBar->Canvas,StatusBar->Left,StatusBar->Top-4,StatusBar->Width,
   Inf,SimbInd);
}}
//---------------------------------------------------------------------------
void __fastcall TFormKinema::ToolButtonCalculatorClick(TObject *Sender){
 ShellExecute(NULL,"open","Calc",NULL,NULL,SW_RESTORE);
}
//---------------------------------------------------------------------------
void __fastcall TFormKinema::FindPoint(int x,int y,int &I,int &J){
 int K=KolElem(Tabl); double DistMin,Dist;
 DistMin=(Xs[0][0]-x)*(Xs[0][0]-x)+(Ys[0][0]-y)*(Ys[0][0]-y);
 if(!K) K=1;
 for(int i=0;i<K;i++){
  for(int j=1;j<Wg;j++){
   Dist=(Xs[i][j]-x)*(Xs[i][j]-x)+(Ys[i][j]-y)*(Ys[i][j]-y);
   if(Dist<DistMin){
    DistMin=Dist; I=i; J=j;
}}}}
//---------------------------------------------------------------------------
void __fastcall TFormKinema::DrawCross(int i,int j){ int r=5;
 if(Xs&&Ys){
  Image->Canvas->MoveTo(Xk,Ys[i][j]); Image->Canvas->LineTo(Xk+Wg,Ys[i][j]);
  Image->Canvas->MoveTo(Xs[i][j],Yk); Image->Canvas->LineTo(Xs[i][j],Yk-Hg);
  Image->Canvas->Ellipse(Xs[i][j]-r,Ys[i][j]-r,Xs[i][j]+r,Ys[i][j]+r);
}}
//---------------------------------------------------------------------------
void __fastcall TFormKinema::ToolButtonReadClick(TObject *Sender){
 int K=KolElem(Tabl),SOVM=0; Magazine *m;
 if(Gamer) ToolButtonGameClick(Sender);
 Reader=!Reader; ToolButtonRead->Down=Reader;
 if(!NameZ.IsEmpty()){ Timer3D->Enabled=!Reader; return; }
 if(K>1){
  swprintf(Inf,L"%d",TabControl->TabIndex);
  if(FindMagazine(Sovm,Inf,&m)) SOVM=1;
 }
 else K=1;
 if(IsNoCalc) return;
 if(Reader){//включаемся
  int i=0;
  Image->Canvas->Pen->Mode=pmNot;
  Image->Canvas->Brush->Style=bsClear;
  Xs=(int **)calloc(K,SzV); Ys=(int **)calloc(K,SzV);
  for(int i=0;i<K;i++){
   Xs[i]=(int *)calloc(Wg,SzI); Ys[i]=(int *)calloc(Wg,SzI);
  }
  tt=Tn; dtt=(Tk-Tn)/(Wg-1);
  do{
   ClearVar(); L->Time->Val=tt;
   if(Tabl){
    if(Tabl[0].Vx){ Vp=Tabl[0].Vx; xdt=Func(tt); } else xdt=tt;
    Vp=Tabl[0].Vy; ydt=Func(tt);
   }
   else{
    if(GodoGraf){ xdt=FuncX(0,tt); ydt=FuncY(0,tt); }
    else{ xdt=tt; ydt=Func(tt); }
   }
   Xs[0][i]=Xk+Round((xdt-xk)/MSt[0])/*+1*/;
   Ys[0][i]=Yk-Round((ydt-yk[SOVM])/MSt[1+SOVM])/*+1*/;
   for(int j=1;j<K;j++){
    if(Tabl[j].Vx){ Vp=Tabl[0].Vx; xdt=Func(tt); } else xdt=tt;
    Vp=Tabl[j].Vy; ydt=Func(tt);
    Xs[j][i]=Xk/*+1*/+Round((xdt-xk)/MSt[0]);
    Ys[j][i]=Yk/*+1*/-Round((ydt-yk[SOVM])/MSt[1+SOVM]);
   }
   tt+=dtt; i++;
  }while(tt<Tk);
  FindPoint(Xk+Wg/2,Yk-Hg/2,I,J);
  DrawCross(I,J);
  tt=Tn+J*dtt; ClearVar(); L->Time->Val=tt;
  if(Tabl){
   if(Tabl[I].Vx){ Vp=VgX=Tabl[I].Vx; xdt=Func(tt); }
   else{ VgX=L->Time; xdt=tt; }
   Vp=VgY=Tabl[I].Vy; ydt=Func(tt);
  }
  else{
   if(GodoGraf){
    xdt=FuncX(0,tt); ydt=FuncY(0,tt);
   }
   else{
    xdt=tt; ydt=Func(tt); VgX=L->Time; VgY=Vp;
  }}
  swprintf(Inf,L"Время: %2.2d:%2.2d:%2.2d,%4.4d сек; %s: %15.5f; %s: %15.5f",
   (int)(tt/3600),((int)tt)%3600/60,((int)tt)%3600%60,
   (int)Round(10000*(tt-floor(tt))),VgX->Name,xdt,VgY->Name,ydt);
  StatusBar->Canvas->Brush->Color=clBtnFace;
  StatusBar->Canvas->FillRect(StatusBar->Canvas->ClipRect);
  DrawGrekText(StatusBar->Canvas,StatusBar->Left,StatusBar->Top-4,StatusBar->Width,
   Inf,SimbInd);
 }
 else{//выключаемся
  DrawCross(I,J);
  swprintf(Inf,L"%s: %s",HeadVersion[0]?HeadVersion:HeadWork,Autor);
  StatusBar->SimpleText=Inf;
  Image->Canvas->Brush->Style=bsSolid;
  Image->Canvas->Pen->Mode=pmCopy;
  if(Xs&&Ys){
   for(int i=0;i<K;i++){ if(Xs[i]) free(Xs[i]); if(Ys[i]) free(Ys[i]); }
   free(Xs); free(Ys);
}}}
//---------------------------------------------------------------------------
void __fastcall TFormKinema::ButtonCancelClick(TObject *Sender){
 GroupBox->Visible=false;
}
//---------------------------------------------------------------------------
void __fastcall TFormKinema::OtchetClick(TObject *Sender){
 Application->CreateForm(__classid(TFormOtchet),&FormOtchet);
 FormOtchet->cbURL->Text=HtmFile;
 FormOtchet->ShowModal();
}
//---------------------------------------------------------------------------
void __fastcall TFormKinema::ToolButtonIncludeClick(TObject *Sender){
 wchar_t Name[80];
 if((TabControl->TabIndex<0)){
  MultFilms->Include(TabControl); return;
 }
 wcscpy(Name,TabControl->Tabs->Strings[TabControl->TabIndex].c_str());
 if(Gamer) ToolButtonGameClick(Sender);
 if(Reader) ToolButtonReadClick(Sender);
// if(HTMFILE=_wfopen(HtmFile,L"r+")){
 if(SLH){ int K_t=Kt;
//  if(!fseek(HTMFILE,-Lpodv,2)){
  double MSt0=MSt[0],MSt1=MSt[1],X_k=Xk,Y_k=Yk,x_k=xk,y_k=yk[0],W_g=Wg,H_g=Hg;
  for(int i=0;i<Kpodv;i++) SLH->Delete(SLH->Count-1);
  if(!NamesInclude&&!NamesTAB)
   swprintf(Shtm,L"<h4 align=\"left\"><font color=\"#009900\">"
	L"3. Избранные графики и таблицы:</font></h4>");
   SLH->Add(Shtm); *Shtm='\0'; shtm=Shtm;
//   fwprintf(HTMFILE,
//	L"<h4 align=\"left\"><font color=\"#009900\">"
//	L"3. Избранные графики и таблицы:</font></h4>\n");
  TakeMagazine(&NamesInclude,Name);
  ToolButtonToBMPClick(NULL);
  swprintf(Shtm,L"<p align=\"left\"><img src=\"%s\"",BMPFile);
  SLH->Add(Shtm);
  swprintf(Shtm,L" alt=\"Этот график см. в файле%s или",BMPFile);
  SLH->Add(Shtm);
  swprintf(Shtm,L"%s\">",JPGFile);
  SLH->Add(Shtm);
  swprintf(Shtm,L"</p><br>");
  SLH->Add(Shtm); *Shtm='\0'; shtm=Shtm;
//  fwprintf(HTMFILE,
//   L"<p align=\"left\">"
//   L"<img src=\"%s\" alt=\"Этот график см. в файле\n%s или\n%s\">"
//   L"</p><br>\n",BMPFile,BMPFile,JPGFile);
  for(int i=0;i<Kpodv;i++) SLH->Add(SLP->Strings[i]);
//  fwprintf(HTMFILE,
//   L"<hr><font face=\"Georgia, Times New Roman, Times, serif\""
//   L" color=\"#326464\">&copy; %s, %d<br></font><br>\n",Autor,NYear+1900)+1;
//  fwprintf(HTMFILE,L"</body>\n</html>\n");
  MSt[0]=MSt0; MSt[1]=MSt1;
  Xk=X_k; Yk=Y_k; xk=x_k; yk[0]=y_k; Wg=W_g; Hg=H_g; Kt=K_t;
//  }
  SLH->SaveToFile(HtmFile);
//  fclose(HTMFILE);
  CppWebBrowser->Navigate(HtmFile);
 }
 ToolButtonInclude->Enabled=false;
}
//---------------------------------------------------------------------------
void __fastcall TFormKinema::UpDownKTimeClick(TObject *Sender,
	  TUDBtnType Button){
 KTime=0.1*UpDownKTime->Position; ChangeOpt=true; OptToFile();
}
//---------------------------------------------------------------------------
void __fastcall TFormKinema::PopupMenuPopup(TObject *Sender){
 if(RichEditInp->Visible){
  pmToBMP->Visible=false;  pmToTabl->Visible=false;
  pmToOtch->Visible=false; pmColors->Visible=false;
  pmGame->Visible=false;   pmValue->Visible=false;
 }
 else{
  wchar_t Name[256]; Magazine *m;
  pmColors->Visible=true; pmGame->Visible=true; pmValue->Visible=true;
  wcscpy(Name,TabControl->Tabs->Strings[TabControl->TabIndex].c_str());
  pmToOtch->Visible=!FindMagazine(NamesInclude,Name,&m);
  pmToBMP->Visible==!FindMagazine(NamesBMP,Name,&m);
  pmToTabl->Visible=!FindMagazine(NamesTAB,Name,&m);
}}
//---------------------------------------------------------------------------
void __fastcall TFormKinema::FormCreate(TObject *Sender){
 Reader=Gamer=false; Prntr=NULL;
 tbSpline->Enabled=false; tbFurie->Enabled=false;
}
//---------------------------------------------------------------------------
void __fastcall TFormKinema::MReportClick(TObject *Sender){
 TabControl->TabIndex=-1; ToolButtonGame->Enabled=false;
 if(HTMLREPORT){
  if(!_waccess(HtmFile,0)){//делаем HTML-отчет
   Application->CreateForm(__classid(TFormOtchet),&FormOtchet);
   FormOtchet->cbURL->Text=HtmFile;
   FormOtchet->Left=0;
   FormOtchet->Width=Width;
   FormOtchet->Top=Top+24;
   FormOtchet->Height=Height-24;
   FormOtchet->ShowModal();
/*   if(ScrollBox) ScrollBox->Visible=false;
   if(DataKDMPaintBox) DataKDMPaintBox->Visible=false;
   if(!_waccess(HtmFile,0))
	CppWebBrowser->Navigate(HtmFile);
//   CppWebBrowser->Align=alClient;
   CppWebBrowser->Visible=true;
   CppWebBrowser->Align=alNone;
   CppWebBrowser->Left=0;  CppWebBrowser->Width=200;
   CppWebBrowser->Top=0;   CppWebBrowser->Height=200;
   RichEditInp->Visible=false;
   PhonImage->Visible=false;
   ImageGraph->Visible=false;
*/
 }}
 else{//делаем графическое окно
  CppWebBrowser->Visible=false;
  CppWebBrowser->Align=alNone;
  CppWebBrowser->Width=0; CppWebBrowser->Height=0;
  Canvas->Font->Assign(FontDialog->Font);
  if(!ScrollBox&&!DataKDMPaintBox){
   int hm=Canvas->TextHeight(L"M");
//создаем полосы прокрутки
   ScrollBox=new TScrollBox(TabControl); ScrollBox->Parent=TabControl;
   ScrollBox->Align=alClient;
   ImageKDM->AreaSizes(Canvas,H,W); W=ClientWidth-32;
   ImageKDM->VaryHeight(Canvas,H);
   H+=2*hm;
   if(ImageKDM->FindLastInst(L"РАБОТА")) H+=2*hm;
   if(ImageKDM->FindLastInst(L"ВЫПОЛНИЛ")||
      ImageKDM->FindLastInst(L"ВЫПОЛНИЛА")||
      ImageKDM->FindLastInst(L"ВЫПОЛНИЛИ")) H+=2*hm;
   if(FindInstLast(L"РИСУНОК")) H+=0.5*ClientHeight;
   ScrollBox->Align=alNone;
   ScrollBox->Width=W+4;
   ScrollBox->Height=H+124;
   DataKDMPaintBox=new TPaintBox(ScrollBox); DataKDMPaintBox->Parent=ScrollBox;
   DataKDMPaintBox->Align=alNone;
   DataKDMPaintBox->Top=0; DataKDMPaintBox->Left=0;
   DataKDMPaintBox->Width=W; DataKDMPaintBox->Height=H+120;
   ScrollBox->Align=alClient;
   DataKDMPaintBox->Canvas->Font->Assign(FontDialog->Font);
   DataKDMPaintBox->OnPaint=PaintBoxPaint;
  }
  else{
   DataKDMPaintBox->Canvas->Font->Assign(FontDialog->Font);
   ImageKDM->AreaSizes(Canvas,H,W);
   DataKDMPaintBox->Width=W; DataKDMPaintBox->Height=H+120;
   ScrollBox->Visible=true; DataKDMPaintBox->Visible=true;
}}}
//---------------------------------------------------------------------------
void __fastcall TFormKinema::MDiagnosClick(TObject *Sender){
 TabControl->TabIndex=-1; ToolButtonGame->Enabled=false;
 if(RichEditInp->Visible) RichEditInp->Visible=false;
 if(!_waccess(DgsFile,0)){//открываем отчет диагностики
  Application->CreateForm(__classid(TFormOtchet),&FormOtchet);
  FormOtchet->cbURL->Text=DgsFile;
  FormOtchet->Left=Left;
  FormOtchet->Width=Width;
  FormOtchet->Top=Top+24;
  FormOtchet->Height=Height-24;
  FormOtchet->ShowModal();
 }
// if(!HTMLREPORT){
//  ScrollBox->Visible=false;
//  DataKDMPaintBox->Visible=false;
// }
// CppWebBrowser->Navigate(DgsFile);
// CppWebBrowser->Align=alClient; CppWebBrowser->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TFormKinema::mWidthPenClick(TObject *Sender){
 GroupBox->Caption=L" Толина пера L";
 UpDownCountTab->Min=1; UpDownCountTab->Max=10;
 UpDownCountTab->Position=WidthPen; DiscretTyp=WIDTHPEN;
 GroupBox->Height=73; RadioGroupSec->Visible=false; GroupBox->Visible=true;
 EditCountTab->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TFormKinema::mMultTimeClick(TObject *Sender){
 GroupBox->Caption=L" Длительность анимации ";
 UpDownCountTab->Min=0; UpDownCountTab->Max=32000;
 if(TMult<=(long)UpDownCountTab->Max){
  UpDownCountTab->Position=(int)TMult; RadioGroupSec->ItemIndex=1;
 }
 else if((TMult/1000)<=(long)UpDownCountTab->Max){
  UpDownCountTab->Position=(int)TMult/1000; RadioGroupSec->ItemIndex=0;
 }
 DiscretTyp=TMULT;
 GroupBox->Height=105; RadioGroupSec->Visible=true; GroupBox->Visible=true;
 EditCountTab->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TFormKinema::mKadrProSecClick(TObject *Sender){
 GroupBox->Caption=L" Число кадров в секунду ";
 UpDownCountTab->Min=1; UpDownCountTab->Max=1000;
 UpDownCountTab->Position=Kkadr; DiscretTyp=KKADR;
 GroupBox->Height=73; RadioGroupSec->Visible=false; GroupBox->Visible=true;
 EditCountTab->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TFormKinema::mStroboSkopClick(TObject *Sender){
 SHLEIF=!SHLEIF; mStroboSkop->ImageIndex=45-SHLEIF;
 if(!SHLEIF) TabControlChange(Sender);
 if(GodoGraf&&!NameZ.IsEmpty()&&!SHLEIF||!GodoGraf&&SHLEIF)
  ToolButtonGame->Enabled=true;
 else
 if(GodoGraf&&!NameZ.IsEmpty()&&SHLEIF||!GodoGraf&&!SHLEIF)
  ToolButtonGame->Enabled=false;
 ChangeOpt=true; OptToFile();
}
//---------------------------------------------------------------------------
void __fastcall TFormKinema::mMultFilm0Click(TObject *Sender){
 TabControl->TabIndex=-1; ToolButtonGame->Enabled=false;
 if(Image) delete Image; Image=new TImage(TabControl);
 Image->Parent=TabControl;
 Image->Align=alClient;
 Image->Canvas->Font->Assign(FontDialog->Font);
 MultFilms->Image=Image;
 Metrica(Image->Canvas,NULL);
 ToolButtonGame->Enabled=false;
 MultFilms->mMFClick(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TFormKinema::tbSplineClick(TObject *Sender){
 wchar_t Name[256],*s; bool Godograf; Magazine *m;
 wcscpy(Name,TabControl->Tabs->Strings[TabControl->TabIndex].c_str());
 swprintf(Inf,L"%d",TabControl->TabIndex);
 if(FindMagazine(Sravn,Inf,&m)||FindMagazine(Sovm,Inf,&m)) Godograf=false;
 else Godograf=wcschr(Name,'(')?true:false;
 if(Godograf&&(s=wcschr(Name,'('))&&!wcschr(s,','))
  SplGdrfToReport(Name,CountTabl,Tn,Tk,Funcs,&NamesInclude,
   &NamesTAB,&NamesSpline,&NamesFurie,CGauge);
 else
  SplToReport(Name,CountTabl,Tn,Tk,Godograf,Funcs,&NamesInclude,
   &NamesTAB,&NamesSpline,&NamesFurie,CGauge);
 tbSpline->Enabled=false;
 CppWebBrowser->Navigate(HtmFile);
}
//---------------------------------------------------------------------------
void __fastcall TFormKinema::tbFurieClick(TObject *Sender){
 wchar_t Name[256]; bool Godograf; Magazine *m;
 wcscpy(Name,TabControl->Tabs->Strings[TabControl->TabIndex].c_str());
 swprintf(Inf,L"%d",TabControl->TabIndex);
 if(FindMagazine(Sravn,Inf,&m)||FindMagazine(Sovm,Inf,&m)) Godograf=false;
 else Godograf=wcschr(Name,'(')?true:false;
 FurieToReport(Name,CountTabl,Tn,Tk,Godograf,Funcs,&NamesInclude,
  &NamesTAB,&NamesSpline,&NamesFurie,CGauge);
 tbFurie->Enabled=false;
 CppWebBrowser->Navigate(HtmFile);
}
//---------------------------------------------------------------------------
void __fastcall TFormKinema::mPrintTablClick(TObject *Sender){
 PRINTABL=!PRINTABL; mPrintTabl->ImageIndex=45-PRINTABL;
 ChangeOpt=true; OptToFile();
 TabControlChange(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TFormKinema::ButtonMaxClick(TObject *Sender){
 if(Wg){
  UpDownCountTab->Max=Wg;
  UpDownCountTab->Position=Wg;
  EditCountTab->Text=UpDownCountTab->Position;
  EditCountTab->SetFocus();
 }
}
//---------------------------------------------------------------------------
void __fastcall TFormKinema::mKBMPClick(TObject *Sender){
 GroupBox->Caption=L" Сжатие BMP в %";
 UpDownCountTab->Min=1; UpDownCountTab->Max=200;
 UpDownCountTab->Position=Round(Kbmp*100); DiscretTyp=KBMP;
 DelAllMagazine(&NamesBMP); ToolButtonToBMP->Enabled=true;
 GroupBox->Height=73; RadioGroupSec->Visible=false; GroupBox->Visible=true;
 EditCountTab->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TFormKinema::mToJPEGClick(TObject *Sender){
 TOJPEG=!TOJPEG; mToJPEG->ImageIndex=45-TOJPEG;
 ChangeOpt=true; OptToFile();
}
//---------------------------------------------------------------------------
void __fastcall TFormKinema::mToBMPClick(TObject *Sender){
 TOBMP=!TOBMP; mToBMP->ImageIndex=45-TOBMP;
 ChangeOpt=true; OptToFile();
}
//---------------------------------------------------------------------------
void __fastcall TFormKinema::TabControlDrawTab(TCustomTabControl *Control,
      int TabIndex, const TRect &Rect, bool Active){
 DrawGrekText(Control->Canvas,Rect.Left,Rect.Top,Rect.Width(),
  TabControl->Tabs->Strings[TabIndex].c_str(),SimbInd);
}
//---------------------------------------------------------------------------
void __fastcall TFormKinema::mMarksInRowClick(TObject *Sender){
 TabControl->MultiLine=!TabControl->MultiLine;
 mMarksInRow->ImageIndex=44+TabControl->MultiLine;
 ChangeOpt=true; OptToFile();
 TabControlChange(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TFormKinema::TimerModelTimer(TObject *Sender){
 ImageKDM=new TImageKDM; Root *I;
 TImage *Image=new TImage(TabControl);
 Image->Parent=TabControl;
 Image->Align=alClient;
 TimerModel->Enabled=false;
 Kmg=10; Sg=10;
 MG[0]=1; MG[1]=1.25; MG[2]=1.5; MG[3]=2; MG[4]=2.5; MG[5]=3; MG[6]=4; MG[7]=5;
 MG[8]=7.5; MG[9]=10;
 CGauge->ShowText=true;
 MReport->Visible=true; MReport->Enabled=true; TabControl->TabIndex=-1;
 if(!VvodJob(SLM,RichEdit=RichEditInp,CGauge,StatusBar,
	ImageKDM,TabControl,Image->Canvas)){
  TabControl->Tabs->Add(L"Исходник");
  ToolButtonTable->Enabled=false; ToolButtonToBMP->Enabled=false;
  ToolButtonRead->Enabled=false;  ToolButtonGame->Enabled=false;
  TabControlChange(Sender);
  return;
 }
 if(FindInst(L"ЦЕЛЕВАЯ ФУНКЦИЯ",&I)) LAGRANGE=true;
 CGauge->Progress=0;
 if(IsSyntError){
  TabControl->Tabs->Add(L"Исходник");
  RichEditInp->Visible=true;
 }
 else{
  if(!Diagnos(CGauge,StatusBar,TabControl)){
   Application->Terminate(); return;
  }
  if(!Kinemat()){
   Application->Terminate(); return;
  }
  MReport->Visible=true;//false;
  MDiagnos->Visible=true;
  if(!_waccess(OprFile,0)) MOperators->Visible=true;
}}
//---------------------------------------------------------------------------
void __fastcall TFormKinema::FormResize(TObject *Sender){
 if(RESIZE) TabControlChange(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TFormKinema::mPrintOpClick(TObject *Sender){
 PRINTOP=!PRINTOP; mPrintOp->ImageIndex=45-PRINTOP;
 ChangeOpt=true; OptToFile();
 Application->MessageBox(L"Опция будет действовать при следующих расчетах!",
  L"Внимание!",MB_OK|MB_ICONWARNING);
}
//---------------------------------------------------------------------------
void __fastcall TFormKinema::MOperatorsClick(TObject *Sender){
 TabControl->TabIndex=-1; ToolButtonGame->Enabled=false;
 if(RichEditInp->Visible) RichEditInp->Visible=false;
 if(!_waccess(OprFile,0)){//открываем отчет операторов
  Application->CreateForm(__classid(TFormOtchet),&FormOtchet);
  FormOtchet->cbURL->Text=OprFile;
  FormOtchet->Left=Left;
  FormOtchet->Width=Width;
  FormOtchet->Top=Top+24;
  FormOtchet->Height=Height-24;
  FormOtchet->ShowModal();
 }
// CppWebBrowser->Align=alNone;
// CppWebBrowser->Height=0; CppWebBrowser->Width=0;
// if(!HTMLREPORT){
//  ScrollBox->Visible=false;
//  DataKDMPaintBox->Visible=false;
// }
// CppWebBrowser->Navigate(OprFile);
// CppWebBrowser->Align=alClient; CppWebBrowser->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TFormKinema::ButtonEpsOKClick(TObject *Sender){
 double E; wchar_t FileRez[256],*s,S[256];
 wcscpy(S,EditEps->Text.c_str()); if(s=wcschr(S,',')) *s='.'; E=_wtof(S);
 if(E>0){
  switch(DiscretTyp){
   case KEQVSCAL:
    if(Round(KEqvScal)!=E){
     KEqvScal=E; if(KEqvScal==0.0) KEqvScal=2.0;
     ToolButtonToBMP->Enabled=true; DelAllMagazine(&NamesBMP);
     ChangeOpt=true;
     TabControlChange(NULL);
    }
    break;
   case EPS:
    if(E!=Eps1){ int sign;
     Eps1=Eps=E; Eps2=Eta=E; ChangeOpt=true;
     ecvt(Eta,NDIG,&NDIG,&sign); NDIG=abs(NDIG)+1;
     if(ReplaceVarsToConst(StatusBar,CGauge,true)){
      swprintf(Inf,
       L"Секция\n\"Переменные, формулы которых не зависят от времени,...\"\n"
       L"в файле Диагностики ОБНОВЛЕНА! См. результат в следующем окне!");
      Application->MessageBox(Inf,L"Внимание!",MB_OK|MB_ICONWARNING);
       TabControl->TabIndex=1; TabControlChange(Sender);
    }}
    break;
  }
  GroupBoxEps->Visible=false;
  if(ChangeOpt) OptToFile(); ChangeOpt=false;
 }
 else{
  swprintf(Inf,L"Введенный параметр\nдолжен быть положительным числом.");
  Application->MessageBox(Inf,L"Внимательнее!!",MB_OK);
}}
//---------------------------------------------------------------------------
void __fastcall TFormKinema::ButtonEpsCancelClick(TObject *Sender){
 GroupBoxEps->Visible=false;
}
//---------------------------------------------------------------------------
void __fastcall TFormKinema::mEpsClick(TObject *Sender){
 GroupBoxEps->Top=64; UpDownEps->Min=0; UpDownEps->Visible=false;
 GroupBoxEps->Left=(this->Width-GroupBoxEps->Width)/4;
 GroupBoxEps->Caption=L" Точность интегрирования ";
 EditEps->Text=Eps;
 DiscretTyp=EPS;
 GroupBoxEps->Visible=true;
 EditEps->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TFormKinema::mEquaScalClick(TObject *Sender){
 GroupBoxEps->Top=48;
 UpDownEps->Min=1; UpDownEps->Position=Round(KEqvScal); UpDownEps->Visible=true;
 GroupBoxEps->Left=(this->Width-GroupBoxEps->Width)/4;
 GroupBoxEps->Caption=L" Коэффициент выравнивания масштабов ";
 EditEps->Text=KEqvScal;
 DiscretTyp=KEQVSCAL;
 GroupBoxEps->Visible=true;
 ToolButtonToBMP->Enabled=true;
 EditEps->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TFormKinema::mPrintTopRightClick(TObject *Sender){
 PRNTOPRIGHT=PRNTOPRIGHT?false:true;
 mPrintTopRight->ImageIndex=45-PRNTOPRIGHT;
 ChangeOpt=true; OptToFile();
 TabControlChange(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TFormKinema::mPrintHeadClick(TObject *Sender){
 PRINTHEAD=PRINTHEAD?false:true;
 mPrintHead->ImageIndex=45-PRINTHEAD;
 ChangeOpt=true; OptToFile();
 TabControlChange(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TFormKinema::PaintBoxPaint(TObject *Sender){
 DataKDMPaintBox->Canvas->Font->Assign(FontDialog->Font);
 ImageKDM->ToImage(/*ImageZam*/NULL,DataKDMPaintBox);
}
//---------------------------------------------------------------------------
void __fastcall TFormKinema::MouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int x, int y){ double m;
//вычислим кватернион по углам Эйлера
 p0=cos(0.5*Teta)*cos(0.5*(Psi+Fi)); p1=sin(0.5*Teta)*cos(0.5*(Psi-Fi));
 p2=sin(0.5*Teta)*sin(0.5*(Psi-Fi)); p3=cos(0.5*Teta)*sin(0.5*(Psi+Fi));
 xA=x; yA=y; //зафиксировали положение мыши
//считаем масштаб и координаты точки щелчка мыши в связанной с объектом СК
 m = Perspect ? 0.67*Diag/Hg : MSt[0];
 XA=m*(x-Wo-Wg/2); YA=m*(Hh+Hg/2-y);
//включаем отслеживание координат движения мыши при нажатой ее клавиши
 Image->OnMouseMove=MouseMove;
}
//---------------------------------------------------------------------------
void AngleTextOut(TCanvas *Canvas,int Angle,int X,int Y,AnsiString Text){
//Процедура рисования текста, наклоненого под углом Angle к горизонту
//Canvas - холст компонента, на котором пишем, у нас это Image,
//Angle - угол в градусах повернутого против часовой стрелки текста,
//X, Y - координаты начала поля текста,
//Text - выводимый текст
 LOGFONT LogRec;
 GetObject(Canvas->Font->Handle,sizeof(LogRec),&LogRec);
//угол задаем в десятых градуса
 LogRec.lfEscapement=Angle*10.0;
 Canvas->Font->Handle=CreateFontIndirect(&LogRec);
 Canvas->TextOut(X,Y,Text);
}
//---------------------------------------------------------------------------
class TTop{
 public: double X,Y,Z; int x,y;
};
//---------------------------------------------------------------------------
class TAqua{
 public:
//вершины аквариума, проекции ортов на экран, углы с осями, шаги по сетке
  TTop T[8]; double ix,iy,jx,jy,kx,ky,fi[3],dS[3]; String Head;
  int xAp,yAp,xBp,yBp,xCp,yCp,l[3],d,h; //длины записей на сетке, размер буквы
  TCanvas *Canvas; double m[3],M[3];
 public:
  TAqua(TCanvas *Canvas,double Min[3],double Max[3]);
  void Draw(bool Color);
  void Inscriptions(int p,int l,double Znach);
  void Shtrih(int p);
  void AxisTitles();
  void AxisTitl(AnsiString NameOs,int k,int n);
  void Grani(double a,double b,int i,int p);
};
//---------------------------------------------------------------------------
TAqua::TAqua(TCanvas *canvas,double Min[3],double Max[3]){ int x,y;
 double X,Y,Z,z,zmax;
 for(int i=0;i<3;i++){ m[i]=Min[i]; M[i]=Max[i]; }
 zmax=FormKinema->ToScreen(T[0].X=m[0],T[0].Y=m[1],T[0].Z=m[2],T[0].x,T[0].y);
 Canvas=canvas;
//Найдем самую дальнюю вершину аквариума и вычислим ее координаты на экране:
 for(int i=0;i<2;i++) for(int j=0;j<2;j++) for(int k=0;k<2;k++){
  X=i==0?m[0]:M[0]; Y=j==0?m[1]:M[1]; Z=k==0?m[2]:M[2];
  z=FormKinema->ToScreen(X,Y,Z,x,y);
  if(z<zmax){
   T[0].X=X; T[0].Y=Y; T[0].Z=Z; T[0].x=x; T[0].y=y;
 }}
//Вычислим координаты на экране остальных вершин (кроме ближайшей)
 T[1].X=T[0].X; T[1].Y=T[0].Y; T[1].Z=(T[0].Z==m[2])?M[2]:m[2];
 FormKinema->ToScreen(T[1].X,T[1].Y,T[1].Z,T[1].x,T[1].y);
 T[2].X=T[0].X; T[2].Y=(T[0].Y==m[1])?M[1]:m[1]; T[2].Z=T[0].Z;
 FormKinema->ToScreen(T[2].X,T[2].Y,T[2].Z,T[2].x,T[2].y);
 T[4].X=(T[0].X==m[0])?M[0]:m[0]; T[4].Y=T[0].Y; T[4].Z=T[0].Z;
 FormKinema->ToScreen(T[4].X,T[4].Y,T[4].Z,T[4].x,T[4].y);
 T[3].X=T[0].X; T[3].Y=(T[0].Y==m[1])?M[1]:m[1]; T[3].Z=(T[0].Z==m[2])?M[2]:m[2];
 FormKinema->ToScreen(T[3].X,T[3].Y,T[3].Z,T[3].x,T[3].y);
 T[5].X=(T[0].X==m[0])?M[0]:m[0]; T[5].Y=T[0].Y; T[5].Z=(T[0].Z==m[2])?M[2]:m[2];
 FormKinema->ToScreen(T[5].X,T[5].Y,T[5].Z,T[5].x,T[5].y);
 T[6].X=(T[0].X==m[0])?M[0]:m[0]; T[6].Y=(T[0].Y==m[1])?M[1]:m[1]; T[6].Z=T[0].Z;
 FormKinema->ToScreen(T[6].X,T[6].Y,T[6].Z,T[6].x,T[6].y);
//проекции ортов на экран:
 ix=T[4].x-T[0].x; iy=T[4].y-T[0].y; fi[0]=atan2(iy,ix); ix=cos(fi[0]); iy=sin(fi[0]);
 jx=T[2].x-T[0].x; jy=T[2].y-T[0].y; fi[1]=atan2(jy,jx); jx=cos(fi[1]); jy=sin(fi[1]);
 kx=T[1].x-T[0].x; ky=T[1].y-T[0].y; fi[2]=atan2(ky,kx); kx=cos(fi[2]); ky=sin(fi[2]);
//определим ширины записей чисел на сетке
 dS[0]=T[4].X-T[0].X; dS[1]=T[2].Y-T[0].Y; dS[2]=T[1].Z-T[0].Z;
 for(int i=0;i<3;i++){ l[i]=0; dS[i]/=FormKinema->CountTabl; }
 d=Canvas->TextWidth(L"M"); h=Canvas->TextHeight(L"M");
 for(int i=0;i<3;i++){ double Znach=(i==0?T[0].X:i==1?T[0].Y:T[0].Z);
  for(int p=0;p<=FormKinema->CountTabl;p++){
   Head.sprintf(L"%g",Znach); Znach+=dS[i];
   if(Canvas->TextWidth(Head)>l[i]) l[i]=Canvas->TextWidth(Head);
}}}
//---------------------------------------------------------------------------
void TAqua::AxisTitl(AnsiString NameOs,int k,int n){
 double r,K,fi,fig; int xN,yN,xV,yV,x,y,l;
 r=sqrt(pow((T[k].x-T[n].x),2.0)+pow((T[k].y-T[n].y),2.0));
 l=Canvas->TextWidth(NameOs);
 K=(T[k].x-T[n].x)/r; xN=T[k].x+Round(d*K); xV=T[k].x+Round((d+l)*K);
 K=(T[k].y-T[n].y)/r; yN=T[k].y+Round(d*K); yV=T[k].y+Round((d+l)*K);
 if(xV<xN){
  fi=atan2(yV-yN,xN-xV); x=xV-Round(0.5*h*sin(fi)); y=yV-Round(0.5*h*cos(fi));
 }
 else{
  fi=atan2(yN-yV,xV-xN); x=xN-Round(0.5*h*sin(fi)); y=yN-Round(0.5*h*cos(fi));
 }
 fig=fi*180.0/M_PI; AngleTextOut(Canvas,fig,x,y,NameOs);
}
//---------------------------------------------------------------------------
void TAqua::AxisTitles(){
 AxisTitl(FormKinema->NameX,4,0);
 AxisTitl(FormKinema->NameY,2,0);
 AxisTitl(FormKinema->NameZ,1,0);
}
//---------------------------------------------------------------------------
void TAqua::Inscriptions(int p,int l,double Znach){
 double r,K,fi,fig; int xN,yN,xV,yV,x,y;
 if(p%2){
  r=sqrt(pow((xAp-xBp),2.0)+pow((yAp-yBp),2.0));
  K=(xAp-xBp)/r; xN=xAp+Round(d*K); xV=xAp+Round((d+l)*K);
  K=(yAp-yBp)/r; yN=yAp+Round(d*K); yV=yAp+Round((d+l)*K);
  if(xV<=xN){
   fi=atan2(yV-yN,xN-xV); x=xV-Round(0.5*h*sin(fi)); y=yV-Round(0.5*h*cos(fi));
  }
  else{
   fi=atan2(yN-yV,xN-xV); x=xN-Round(0.5*h*sin(fi)); y=yN-Round(0.5*h*cos(fi));
  }
  fig=fi*180.0/M_PI; Head.sprintf(L"%g",Znach); AngleTextOut(Canvas,fig,x,y,Head);
 }
 else{
  r=sqrt(pow((xCp-xBp),2.0)+pow((yCp-yBp),2.0));
  K=(xCp-xBp)/r; xN=xCp+Round(d*K); xV=xCp+Round((d+l)*K);
  K=(yCp-yBp)/r; yN=yCp+Round(d*K); yV=yCp+Round((d+l)*K);
  if(xV<=xN){
   fi=atan2(yV-yN,xN-xV); x=xV-Round(0.5*h*sin(fi)); y=yV-Round(0.5*h*cos(fi));
  }
  else{
   fi=atan2(yN-yV,xV-xN); x=xN-Round(0.5*h*sin(fi)); y=yN-Round(0.5*h*cos(fi));
  }
  fig=fi*180.0/M_PI; Head.sprintf(L"%g",Znach); AngleTextOut(Canvas,fig,x,y,Head);
}}
//---------------------------------------------------------------------------
void TAqua::Shtrih(int p){
 if(p%2){
  Canvas->Pen->Width=3; Canvas->MoveTo(xAp,yAp);
  xAp=Round(0.97*xAp)+Round(0.03*xBp); yAp=Round(0.97*yAp)+Round(0.03*yBp);
  Canvas->LineTo(xAp,yAp); Canvas->Pen->Width=1;
 }
 else{
  Canvas->Pen->Width=3; Canvas->MoveTo(xCp,yCp);
  xCp=Round(0.97*xCp)+Round(0.03*xBp); yCp=Round(0.97*yCp)+Round(0.03*yBp);
  Canvas->LineTo(xCp,yCp); Canvas->Pen->Width=1;
}}
//---------------------------------------------------------------------------
void TAqua::Grani(double a,double b,int i,int p){
 int I,J,K,L,M,N; double Znach;
 switch(i){
  case 0: I=5; J=1; K=4; L=0; M=6; N=2; Znach=T[0].X+p*dS[0]; break;
  case 1: I=3; J=1; K=2; L=0; M=6; N=4; Znach=T[0].Y+p*dS[1]; break;
  case 2: I=5; J=4; K=1; L=0; M=3; N=2; Znach=T[0].Z+p*dS[2]; break;
 }
 xAp=Round(a*T[I].x)+Round(b*T[J].x); yAp=Round(a*T[I].y)+Round(b*T[J].y);
 xBp=Round(a*T[K].x)+Round(b*T[L].x); yBp=Round(a*T[K].y)+Round(b*T[L].y);
 xCp=Round(a*T[M].x)+Round(b*T[N].x); yCp=Round(a*T[M].y)+Round(b*T[N].y);
 Inscriptions(p,l[i],Znach);
 Shtrih(p);
 Canvas->MoveTo(xAp,yAp); Canvas->LineTo(xBp,yBp); Canvas->LineTo(xCp,yCp);
}
//---------------------------------------------------------------------------
void TAqua::Draw(bool Color){
 double x,y,alfa=1.0/FormKinema->CountTabl,a=alfa,b;
 Canvas->Pen->Width=3;
 Canvas->Pen->Color=Color?FormKinema->ColorGrid:FormKinema->ColorPhon;
 Canvas->Font->Color=Color?FormKinema->ColorCifrGrid:FormKinema->ColorPhon;
//Нарисуем оси
 Canvas->MoveTo(T[0].x,T[0].y); Canvas->LineTo(T[4].x,T[4].y);
 Canvas->MoveTo(T[0].x,T[0].y); Canvas->LineTo(T[2].x,T[2].y);
 Canvas->MoveTo(T[0].x,T[0].y); Canvas->LineTo(T[1].x,T[1].y);
//Нарисум контур аквариума
 Canvas->LineTo(T[3].x,T[3].y); Canvas->LineTo(T[2].x,T[2].y);
 Canvas->LineTo(T[6].x,T[6].y); Canvas->LineTo(T[4].x,T[4].y);
 Canvas->LineTo(T[5].x,T[5].y); Canvas->LineTo(T[1].x,T[1].y);
 Canvas->Pen->Width=1;
//Нарисуем сетку, сделаем записи чисел на сетке
 for(int p=1;p<FormKinema->CountTabl;p++,a+=alfa){
  b=1-a;
  Grani(a,b,0,p);
  Grani(a,b,1,p);
  Grani(a,b,2,p);
 }
 xAp=T[5].x; yAp=T[5].y; xBp=T[4].x; yBp=T[4].y; xCp=T[6].x; yCp=T[6].y;
 Inscriptions(FormKinema->CountTabl,l[0],T[4].X);
 xAp=T[3].x; yAp=T[3].y; xBp=T[2].x; yBp=T[2].y; xCp=T[6].x; yCp=T[6].y;
 Inscriptions(FormKinema->CountTabl,l[1],T[2].Y);
 xAp=T[5].x; yAp=T[5].y; xBp=T[1].x; yBp=T[1].y; xCp=T[3].x; yCp=T[3].y;
 Inscriptions(FormKinema->CountTabl,l[2],T[1].Z);
 AxisTitles();
}
//---------------------------------------------------------------------------
void __fastcall TFormKinema::DrawArea(TCanvas *Canvas,bool Color){
 int xO,yO,zO,x,y,z,i,alfa,beta,gama,Ct=CountTabl;
 double DS,X,Y,Z,x0,y0,dsx=(Max[0]-Min[0])/Ct,dsy=(Max[1]-Min[1])/Ct;
 static double *qst;
 CppWebBrowser->Height=0; CppWebBrowser->Width=0;
// bool Pr=Perspect;
 if(ColorGrid!=ColorPhon){
  TAqua Aquarium(Canvas,Min,Max);
  Aquarium.Draw(Color);
 }
 Canvas->Pen->Width=1;
 static Square *square=NULL; static int *Mxe,*Mye; static double *MzCe,*MzC;
 Kt=Ct+1;
 if(!square){
  square=(Square *)calloc(Ct*Ct,sizeof(Square)); square[0].Count=Ct;
  Mxe=(int *)calloc(Kt*Kt,SzI);     Mye=(int *)calloc(Kt*Kt,SzI);
  MzCe=(double *)calloc(Ct*Ct,SzD); MzC=(double *)calloc(Ct*Ct,SzD);
 }
 else if(square[0].Count!=Ct){
  square=(Square *)realloc(square,Ct*Ct*sizeof(Square)); square[0].Count=Ct;
  Mxe=(int *)realloc(Mxe,Kt*Kt*SzI);      Mye=(int *)realloc(Mye,Kt*Kt*SzI);
  MzCe=(double *)realloc(MzCe,Ct*Ct*SzD); MzC=(double *)realloc(MzC,Ct*Ct*SzD);
 }
 if(Color) qst=q; else q=qst;
 if(VarInForm(true,VgZ->Znach,L->Time))//функция зависит от времени
  L->Time->Val=*q++;
 X=Min[0];
 for(int i=0;i<Kt;i++,X+=dsx){ int xe,ye;
  Y=Min[1];
  for(int j=0,k=i*Kt+j;j<Kt;j++,k++,Y+=dsy){
   Z=*q++;
   FormKinema->ToScreen(X,Y,Z,xe,ye);
   Mxe[k]=xe; Mye[k]=ye;
 }}
 X=Min[0]+0.5*dsx;
 for(i=0;i<Ct;i++,X+=dsx){ int xe,ye;
  Y=Min[1]+0.5*dsy;
  for(int j=0,k=i*Ct+j; j<Ct; j++,k++,Y+=dsy){
   Z=*q++;
   MzCe[k]=FormKinema->ToScreen(X,Y,Z,xe,ye);
   MzC[k]=Z;
 }}
 for(int i=0; i<Ct; i++){
  for(int j=0,k=i*Ct+j,l=i*Kt+j,m=l+Kt,n=l+1,p=m+1;j<Ct;j++,k++){
   square[k].V[0].x=square[k].V[4].x=Mxe[l];
   square[k].V[0].y=square[k].V[4].y=Mye[l++];
   square[k].V[1].x=Mxe[m]; square[k].V[1].y=Mye[m++];
   square[k].V[2].x=Mxe[p]; square[k].V[2].y=Mye[p++];
   square[k].V[3].x=Mxe[n]; square[k].V[3].y=Mye[n++];
   square[k].Zce=MzCe[k];   square[k].Z=MzC[k];
 }}
 qsort(square,Ct*Ct,sizeof(Square),CmpSquare);
 double MinZ,MaxZ=MinZ=fabs(square[0].Z),D;
 TColor MinC[3],MaxC[3],Color1,Color2,ColorOld=Canvas->Brush->Color;
 for(int i=1; i<Ct*Ct; i++){
  if((D=fabs(square[i].Z))<MinZ) MinZ=D;
  if((D=fabs(square[i].Z))>MaxZ) MaxZ=D;
 }
 D=MaxZ-MinZ; //диапазон высот
 Color1=FormKinema->ColorGraph%256;//состояние красной пушки в первом цвете
 Color2=FormKinema->ColorGraph2%256;//состояние красной пушки во втором цвете
 MinC[0]=Color1; MaxC[0]=Color2;
 Color1=(FormKinema->ColorGraph/256)%256;//то же для зеленой пушки
 Color2=(FormKinema->ColorGraph2/256)%256;
 MinC[1]=Color1; MaxC[1]=Color2;
 Color1=FormKinema->ColorGraph/256/256;//то же для синей
 Color2=FormKinema->ColorGraph2/256/256;
 MinC[2]=Color1; MaxC[2]=Color2;
 for(int i=0; i<Ct*Ct; i++){
  double a=(fabs(square[i].Z)-MinZ)/D;
//состояния пушек для раскраски текущего прямоугольника
  TColor ColorR=MinC[0]+(TColor)Round(a*(MaxC[0]-MinC[0])),
   ColorG=MinC[1]+(TColor)Round(a*(MaxC[1]-MinC[1])),
   ColorB=MinC[2]+(TColor)Round(a*(MaxC[2]-MinC[2]));
//суммируем цвет
  Canvas->Brush->Color=Color?ColorR+256*(ColorG+256*ColorB):ColorPhon;
  Canvas->Polygon(square[i].V,4);
 }
 Canvas->Brush->Color=ColorOld;
// Perspect=Pr;
 if(TOBMP&&!_waccess(Dir,0))
  swprintf(BMPKadrs,L"%s\\%3.3d.bmp",Dir,Nkadr);
 if(TOJPEG&&!_waccess(Dir,0))
  swprintf(JPGKadrs,L"%s\\%3.3d.jpg",Dir,Nkadr);
 if(TOBMP)
  Image->Picture->SaveToFile(BMPKadrs);
 if(TOJPEG){
  if(JPEG) delete JPEG; JPEG = new TJPEGImage();
  JPEG->Assign(Image->Picture->Bitmap);
  JPEG->SaveToFile(JPGKadrs);
}}
//---------------------------------------------------------------------------
void __fastcall TFormKinema::MouseMove(TObject *Sender,
      TShiftState Shift, int x, int y){
 double dp0,dp1,dp2,dp3,m,Z,rArB,Zn,alpha,l,fi1,fi2;
//зафиксировали новое положение мыши и вычислим расстояние ее в пикселах от старого
 xB=x; yB=y; l=sqrt((xB-xA)*(xB-xA)+(yB-yA)*(yB-yA));
 if(l>10.0){ double q0,q1,q2,q3;
//погасим предыдущий кадр
  DrawArea(Image->Canvas,false);
//считаем масштаб и расстояние от экрана до центра аквариума,
//а также координаты точки щелчка мыши в связанной с аквариумом СК
  if(Perspect){ m=0.67*Diag/Hg; Z=3*Diag; } else{ m=MSt[0]; Z=Diag; }
  XB=m*(x-Wo-Wg/2); YB=m*(Hh+Hg/2-y);
//составляющие кватерниона поворота аквариума считаем через
//скалярное и векторное произведение векторов rA и rB
  rArB=sqrt((XA*XA+YA*YA+Z*Z)*(XB*XB+YB*YB+Z*Z));
  alpha=acos((XA*XB+YA*YB+Z*Z)/rArB);
  dp0=cos(0.5*alpha); Zn=2*rArB*dp0;
  dp1=Z*(YA-YB)/Zn; dp2=Z*(XB-XA)/Zn; dp3=(XA*YB-YA*XB)/Zn;
//перемножаем кватернионы
  q0=dp0*p0-dp1*p1-dp2*p2-dp3*p3; q1=dp0*p1+dp1*p0+dp2*p3-dp3*p2;
  q2=dp0*p2+dp2*p0+dp3*p1-dp1*p3; q3=dp0*p3+dp3*p0+dp1*p2-dp2*p1;
//вычисляем матрицу поворота для нового положения аквариума
  c11=2*(q0*q0+q1*q1)-1; c12=2*(q1*q2-q0*q3);   c13=2*(q1*q3+q0*q2);
  c21=2*(q1*q2+q0*q3);   c22=2*(q0*q0+q2*q2)-1; c23=2*(q3*q2-q0*q1);
  c31=2*(q1*q3-q0*q2);   c32=2*(q3*q2+q0*q1);   c33=2*(q0*q0+q3*q3)-1;
//рисуем новое положение объекта и аквариума
  DrawArea(Image->Canvas,true);
//сохраняем новый кватернион и положение мыши
  p0=q0; p1=q1; p2=q2; p3=q3;
  XA=XB; YA=YB;
}}
//---------------------------------------------------------------------------
void __fastcall TFormKinema::MouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int x, int y){
 double fi1,fi2;
//выключаем отслеживание координат движения мыши при нажатой ее клавиши
// Image->OnMouseMove=NULL;
//фиксируем углы Эйлера и матрицу поворота для дальнейших вращений
 Teta=2*acos(sqrt(p0*p0+p3*p3));
 fi1=p0==0?M_PI_2:atan(p3/p0); if(p0<0) fi1+=M_PI;
 fi2=p1==0?M_PI_2:atan(p2/p1); if(p1<0) fi2+=M_PI;
 Fi=fi1-fi2; Psi=fi1+fi2;
 cFi=cos(Fi); sFi=sin(Fi); cPsi=cos(Psi); sPsi=sin(Psi);
 cTeta=cos(Teta); sTeta=sin(Teta);
 c11=cPsi*cFi-sPsi*cTeta*sFi; c12=-cPsi*sFi-sPsi*cTeta*cFi; c13= sPsi*sTeta;
 c21=sPsi*cFi+cPsi*cTeta*sFi; c22= cPsi*cTeta*cFi-sPsi*sFi; c23=-cPsi*sTeta;
 c31=sTeta*sFi;               c32= sTeta*cFi;               c33= cTeta;
}
//---------------------------------------------------------------------------
void __fastcall TFormKinema::TimerAreaTimer(TObject *Sender){ static int k;
 if(L->Time->Val==Tn){ k=0; CGauge->Progress=0; }
 k++; CGauge->Progress++;
 DrawArea(Image->Canvas,false);
 if(k==Nt){
  CGauge->Progress=0; CGauge->ShowText=false;
  DrawArea(Image->Canvas,true);
  Nkadr=0;
  TimerArea->Enabled=false;
  free(Qtz); Qtz=NULL;
 }
 else{
//  Fi  =2*M_PI*Nkadr/250.0/15.0; if(Fi  >2*M_PI) Fi  -=2*M_PI;
//  Teta=2*M_PI*Nkadr/250.0/25.0; if(Teta>2*M_PI) Teta-=2*M_PI;
//  Psi =2*M_PI*Nkadr/250.0/5.0;  if(Psi >2*M_PI) Psi -=2*M_PI;
//  cFi=cos(Fi); sFi=sin(Fi); cPsi=cos(Psi); sPsi=sin(Psi);
//  cTeta=cos(Teta); sTeta=sin(Teta);
//  c11=cPsi*cTeta*cFi-sPsi*sFi; c12=-cPsi*cTeta*sFi-sPsi*cFi; c13=cPsi*sTeta;
//  c21=sPsi*cTeta*cFi+cPsi*sFi; c22=-sPsi*cTeta*sFi+cPsi*cFi; c23=sPsi*sTeta;
//  c31=-sTeta*cFi; c32=sTeta*sFi; c33=cTeta;
  DrawArea(Image->Canvas,true); Nkadr++;
}}
//---------------------------------------------------------------------------
void __fastcall TFormKinema::mKColumnClick(TObject *Sender){
 GroupBox->Caption=L" Число столбцов в переменных ";
 UpDownCountTab->Min=1; UpDownCountTab->Max=10; UpDownCountTab->Increment=1;
 UpDownCountTab->Position=KColumn; DiscretTyp=KCOLUMN;
 GroupBox->Visible=true; ButtonMax->Visible=true; RadioGroupSec->Visible=false;
 EditCountTab->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TFormKinema::ImageGraphMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y){
 if(Reader&&Shift.Contains(ssLeft)){
  DrawCross(I,J);
  FindPoint(X,Y,I,J);
  DrawCross(I,J);
  tt=Tn+J*dtt; ClearVar(); L->Time->Val=tt;
  if(Tabl){
   if(Tabl[0].Vx){ Vp=Tabl[0].Vx; xdt=Func(tt); } else xdt=tt;
   Vp=VgY=Tabl[I].Vy; ydt=Func(tt);
  }
  else{
   if(GodoGraf){
    xdt=FuncX(0,tt); ydt=FuncY(0,tt);
   }
   else{
    xdt=tt; ydt=Func(tt); VgX=L->Time; VgY=Vp;
  }}
  swprintf(Inf,L"Время: %2.2d:%2.2d:%2.2d,%4.4d сек; %s: %15.5f; %s: %15.5f",
   (int)(tt/3600),((int)tt)%3600/60,((int)tt)%3600%60,
   (int)Round(10000*(tt-floor(tt))),VgX->Name,xdt,VgY->Name,ydt);
  StatusBar->Canvas->Brush->Color=clBtnFace;
  StatusBar->Canvas->FillRect(StatusBar->Canvas->ClipRect);
  DrawGrekText(StatusBar->Canvas,StatusBar->Left,StatusBar->Top-4,StatusBar->Width,
   Inf,SimbInd);
}}
//---------------------------------------------------------------------------
void __fastcall TFormKinema::mRenovCalcClick(TObject *Sender){
 RENOVCALC=RENOVCALC?false:true; mRenovCalc->ImageIndex=45-RENOVCALC;
 ChangeOpt=true; OptToFile();
 TabControlChange(Sender);
}
//---------------------------------------------------------------------------
int CmpVary(const void *pM1,const void *pM2){
 Magazine *m1=*(Magazine **)pM1,*m2=*(Magazine **)pM2; Vary *V1,*V2;
 FindVary(m1->S,&V1); FindVary(m2->S,&V2);
 if(VarInForm(true,V1->Znach,V2)) return  1;
 if(VarInForm(true,V2->Znach,V1)) return -1;
 return 0;
}
//---------------------------------------------------------------------------
void MagazinToFile(Magazine *M,wchar_t *File,bool Add){
 FILE *F; unsigned int L=80,l,n;
 if(M&&(F=_wfopen(File,Add?L"a":L"w"))){
  if(Add) fputws(L"\n\n",F);
  l=fwprintf(F,M->S);
  for(Magazine *m=M->Sled;m;m=m->Sled){
   if(l+wcslen(m->S)>L){
    if(m->Sled) fputws(L",\n",F); else fputwc('\n',F);
    l=fwprintf(F,m->S);
   }
   else
    l+=fwprintf(F,L", %s",m->S);
  }
  fclose(F);
}}
//---------------------------------------------------------------------------
//КООРДИНАТЫ ТОЧЕК:=XC0,YC0,ZC0,XR6,YR6,ZR6,XL6,YL6,ZL6;
//РАЗМЕРЫ ЗВЕНЬЕВ:=A,G,B,C,D,D1,F,H,K,E,Sx,Sz;
//по новому
void CreateProcInv(){
//составим список имен переменных, входящих в уравнения
 Cord *Cv=NULL,*Ce=NULL,*q; FILE *Fo,*Fi; int i; Root *I; Vary *V;
//сначала составим список исключений по инструкциям КООРДИНАТЫ ТОЧЕК и РАЗМЕРЫ ЗВЕНЬЕВ
 if(I=FindInstLast(L"КООРДИНАТЫ ТОЧЕК"))
  for(Parm *P=I->First;P;P=P->Sled)
   if(P->Nam&&FindVary(P->Nam,&V)) TakeCordDefVars(&Ce,V);
 if(I=FindInstLast(L"РАЗМЕРЫ ЗВЕНЬЕВ"))
  for(Parm *P=I->First;P;P=P->Sled)
   if(P->Nam&&FindVary(P->Nam,&V)) TakeCordDefVars(&Ce,V);
//отсортируем имена переменных по зависимости их друг от друга
 for(int i=0;i<N;i++)
  VarsFromFormDefVars(Uravn[i],&Cv,Ce);
 if(Fo=_wfopen(L"tmp.cpp",L"w")){
  fputws(L"void CalcUravn(long double *qk,long double *r0,long double *r6R,L"
   L"long double *r6L,long double *S,long double *U){\n",Fo);
  fputws(L"//r0={XC0,YC0,ZC0}, r6R={X6R,Y6R,Z6R}, r6L={X6L,Y6L,Z6L}\n",Fo);
  fputws(L"//S={A,G,B,C,D,D1,F,H,K,E,Sx,Sy} - размеры звеньев\n",Fo);
  fwprintf(Fo,L" long double ");
  for(q=L->K,i=0;q;i++,q=q->Sled) fwprintf(Fo,L"%s, ",q->V->Name);
  if(I=FindInstLast(L"КООРДИНАТЫ ТОЧЕК")){
   for(Parm *P=I->First;P;P=P->Sled)
    if(P->Nam&&FindVary(P->Nam,&V))
     fwprintf(Fo,L"%s, ",P->Nam);
  }
  if(I=FindInstLast(L"РАЗМЕРЫ ЗВЕНЬЕВ")){
   for(Parm *P=I->First;P;P=P->Sled)
    if(P->Nam&&FindVary(P->Nam,&V))
     fwprintf(Fo,L"%s, ",P->Nam);
  }
  for(q=Cv;q;q=q->Sled){
   fwprintf(Fo,q->Sled ? L"%s, " : L"%s;\n",q->V->Name);
  }
  fputws(L"\n//Присвоение значений обобщенным координатам\n",Fo);
  for(q=L->K,i=0;q;i++,q=q->Sled){
   swprintf(Inf,L" %s=qk[%d]; ",q->V->Name,i);
   fputws(Inf,Fo);
  }
  fputws(L"\n//Присвоение значений координатам точек\n",Fo);
  if(I=FindInstLast(L"КООРДИНАТЫ ТОЧЕК")){ Parm *P; int i;
   for(P=I->First,i=0;P;P=P->Sled,i++)
    fwprintf(Fo,i<3 ? L" %s=r0[%d];" : i<6? L" %s=r6R[%d];" : L" %s=r6L[%d];",
     P->Nam,i%3);
   }
  fputws(L"\n//Присвоение значений размерам звеньев\n",Fo);
  if(I=FindInstLast(L"РАЗМЕРЫ ЗВЕНЬЕВ")){ Parm *P; int i;
   for(P=I->First,i=0;P;P=P->Sled,i++)
    fwprintf(Fo,L" %s=S[%d];",P->Nam,i);
   }
  fputws(L"\n\n//Переменные",Fo);
  for(q=Cv;q;q=q->Sled){
   fputws(L"\n ",Fo);
   PrintForm(true,Fo,q->V->Name,q->V->Znach);
  }
  fputws(L"\n\n//Уравнения:",Fo);
  for(int i=0;i<N;i++){
   swprintf(Inf,L"U[%2d]",i); fputws(L"\n ",Fo);
   PrintForm(true,Fo,Inf,Uravn[i]);
  }
  fputws(L"\n}",Fo);
  fclose(Fo);
 }
 if((Fi=_wfopen(L"tmp.cpp",L"r"))&&(Fo=_wfopen(L"CalcUravn.cpp",L"w"))){
  while(fgetws(Inf,4095,Fi))
   CopyZam(Fo,Inf);
  fclose(Fi); fclose(Fo);
 }
 DelAllList((void **)&Cv);
 for(int i=0;i<N;i++)
//отсортируем имена переменных по зависимости их друг от друга
  for(int j=0;j<N;j++)
   VarsFromFormDefVars(Af[i][j],&Cv,Ce);
 if(Fo=_wfopen(L"tmp.cpp",L"w")){
  fputws(L"void CalcUq(long double *qk,long double *S,long double **Uq){\n",Fo);
  fputws(L"//S={A,G,B,C,D,D1,F,H,K,E,Sx,Sy} - размеры звеньев\n",Fo);
  fwprintf(Fo,L" long double ");
  for(q=L->K,i=0;q;i++,q=q->Sled) fwprintf(Fo,L"%s, ",q->V->Name);
  if(I=FindInstLast(L"РАЗМЕРЫ ЗВЕНЬЕВ")){
   for(Parm *P=I->First;P;P=P->Sled)
    if(P->Nam&&FindVary(P->Nam,&V))
     fwprintf(Fo,L"%s, ",P->Nam);
  }
  for(q=Cv;q;q=q->Sled){
   fwprintf(Fo,q->Sled ? L"%s, " : L"%s;\n",q->V->Name);
  }
  fputws(L"\n//Присвоение значений обобщенным координатам\n",Fo);
  for(q=L->K,i=0;q;i++,q=q->Sled){
   swprintf(Inf,L" %s=qk[%d]; ",q->V->Name,i);
   fputws(Inf,Fo);
  }
  fputws(L"\n//Присвоение значений размерам звеньев\n",Fo);
  if(I=FindInstLast(L"РАЗМЕРЫ ЗВЕНЬЕВ")){ Parm *P; int i;
   for(P=I->First,i=0;P;P=P->Sled,i++)
    fwprintf(Fo,L" %s=S[%d];",P->Nam,i);
   }
  fputws(L"\n\n//Переменные",Fo);
  for(q=Cv;q;q=q->Sled){
   fputws(L"\n ",Fo);
   PrintForm(true,Fo,q->V->Name,q->V->Znach);
  }
  fputws(L"\n\n//Матрица (якобиан уравнений):",Fo);
  for(int i=0;i<N;i++){
   for(int j=0;j<N;j++){
    swprintf(Inf,L"Uq[%2d][%2d]",i,j); fputws(L"\n ",Fo);
    PrintForm(true,Fo,Inf,Af[i][j]);
  }}
  fputws(L"\n}\n",Fo);
  fclose(Fo);
 }
 DelAllList((void **)&Cv);
 if((Fi=_wfopen(L"tmp.cpp",L"r"))&&(Fo=_wfopen(L"CalcUq.cpp",L"w"))){
  while(fgetws(Inf,4095,Fi))
   CopyZam(Fo,Inf);
  fclose(Fi); fclose(Fo);
}}
//---------------------------------------------------------------------------
//КООРДИНАТЫ ЦЕНТРА МАСС:=XC,YC,ZC;
//КООРДИНАТЫ ТОЧЕК:=XR6,YR6,ZR6,XL6,YL6,ZL6;
//РАЗМЕРЫ ЗВЕНЬЕВ НИЗА:=A,G,B,C,D,D1,F,H,K,E,Sx,Sz;
//РАЗМЕРЫ ЗВЕНЬЕВ ВЕРХА:=L,L1,M,Nz1,Nz2,Nz3,Nz4,Nz5,Nz,Nx1,
//                       U,Q,Q1,Q2,Q3,P,P1,R,S,T;
//КООРДИНАТЫ ЦЕНТРОВ МАСС НИЗА:=Ax,Az,Gx,Gy,Gz,By,Bz,Cz,Dx,Dz,Sx,Sz;
//КООРДИНАТЫ ЦЕНТРОВ МАСС ВЕРХА:=Krx,Krz,Hrx,Hry,Hrz,Shx,Shy,Shz,
//                               Lpx,Lpy,Lpz,Pry,Prz,Efy,Ply,Plz,Lky,Lkz;
//УГЛЫ ВЕРХА:=omega,tau,ro,hi,lambda$L,mu$L,jota$L,kappa$L,
// lambda$R,mu$R,jota$R,kappa$R;
//МАССЫ:=mt,me,mb,mg,mp,ms,mkr,mhr,msh,mlp,mpl,mef,mpp,mlk;
void CreateProcInvCOM(){
//составим список имен переменных, входящих в уравнения
 Cord *CvUr=NULL,*CvUq=NULL,*Ce=NULL,*q,*CuUr=NULL,*CuUq=NULL;
 FILE *Fo,*Fi; int i; Root *I; Vary *V; bool REPUNOPNAVARY=true;
//сначала составим список исключений по инструкциям КООРДИНАТЫ ЦЕНТРА МАСС,
// КООРДИНАТЫ ТОЧЕК, РАЗМЕРЫ ЗВЕНЬЕВ, КООРДИНАТЫ ЦЕНТРОВ МАСС и МАССЫ
 if(I=FindInstLast(L"КООРДИНАТЫ ЦЕНТРА МАСС"))
  for(Parm *P=I->First;P;P=P->Sled)
   if(P->Nam&&FindVary(P->Nam,&V)) TakeCordDefVars(&Ce,V);
 if(I=FindInstLast(L"КООРДИНАТЫ ТОЧЕК"))
  for(Parm *P=I->First;P;P=P->Sled)
   if(P->Nam&&FindVary(P->Nam,&V)) TakeCordDefVars(&Ce,V);
 if(I=FindInstLast(L"РАЗМЕРЫ ЗВЕНЬЕВ НИЗА"))
  for(Parm *P=I->First;P;P=P->Sled)
   if(P->Nam&&FindVary(P->Nam,&V)) TakeCordDefVars(&Ce,V);
 if(I=FindInstLast(L"РАЗМЕРЫ ЗВЕНЬЕВ ВЕРХА"))
  for(Parm *P=I->First;P;P=P->Sled)
   if(P->Nam&&FindVary(P->Nam,&V)) TakeCordDefVars(&Ce,V);
 if(I=FindInstLast(L"КООРДИНАТЫ ЦЕНТРОВ МАСС НИЗА"))
  for(Parm *P=I->First;P;P=P->Sled)
   if(P->Nam&&FindVary(P->Nam,&V)) TakeCordDefVars(&Ce,V);
 if(I=FindInstLast(L"КООРДИНАТЫ ЦЕНТРОВ МАСС ВЕРХА"))
  for(Parm *P=I->First;P;P=P->Sled)
   if(P->Nam&&FindVary(P->Nam,&V)) TakeCordDefVars(&Ce,V);
 if(I=FindInstLast(L"УГЛЫ ВЕРХА"))
  for(Parm *P=I->First;P;P=P->Sled)
   if(P->Nam&&FindVary(P->Nam,&V)) TakeCordDefVars(&Ce,V);
 if(I=FindInstLast(L"МАССЫ"))
  for(Parm *P=I->First;P;P=P->Sled)
   if(P->Nam&&FindVary(P->Nam,&V)) TakeCordDefVars(&Ce,V);
// for(int i=0;i<N;i++){
//  UnOpToVary(Uravn[i],&CuUr);
//  for(int j=0;j<N;j++)
//   UnOpToVary(Af[i][j],&CuUq);
// }
 for(int i=0;i<N;i++){
//отсортируем имена переменных из уравнений по зависимости их друг от друга
  VarsFromFormDefVars(Uravn[i],&CvUr,Ce);
//отсортируем имена переменных из якобиана по зависимости их друг от друга
  for(int j=0;j<N;j++)
   VarsFromFormDefVars(Af[i][j],&CvUq,Ce);
 }
 if(REPUNOPNAVARY){//
  for(Unop *u=L->U;u;u=u->Sled) u->O=0;
  for(Vary *v=L->V;v;v=v->Sled) v->O=v->U=0;
  for(q=CvUr;q;q=q->Sled){
   Form F; F.V=q->V;
   UnOpToVary(F,&CuUr);
  }
  for(Vary *v=L->V;v;v=v->Sled) v->O=v->U=0;
  for(Unop *u=L->U;u;u=u->Sled) u->O=0;
  for(q=CvUq;q;q=q->Sled){
   Form F; F.V=q->V;
   UnOpToVary(F,&CuUq);
 }}
 if(Fo=_wfopen(L"tmp.cpp",L"w")){ int i;
  fputws(L"void CalcUravnCOM(long double *qk,long double *uv,long double *rC,\n"
   L" long double *r6R,long double *r6L,long double *Ss,long double *Cm,\n"
   L" long double *Ms,long double *Ur){\n",Fo);
  if(FindInstLast(L"УГЛЫ ВЕРХА")){
   fputws(L"//uv={",Fo);
   for(Parm *P=I->First;P;P=P->Sled){ int i=0;
    if(P->Nam&&FindVary(P->Nam,&V)){
     if(i>6){ fputwc('\n',Fo); i=0; } else i++;
     fputws(P->Nam,Fo);
     if(P->Sled) fputwc(',',Fo); else fputws(L"} - углы верха\n",Fo);
  }}}
  fputws(L"//rC={XC,YC,ZC}, r6R={X6R,Y6R,Z6R}, r6L={X6L,Y6L,Z6L}\n",Fo);
  if(I=FindInstLast(L"РАЗМЕРЫ ЗВЕНЬЕВ НИЗА")){ Parm *P=I->First;
   fwprintf(Fo,L"//Ss={");
   for(i=1;P;P=P->Sled,i++){
    if(P->Nam) fwprintf(Fo,L"%s",P->Nam);
    if(P->Sled){
     fwprintf(Fo,L", ",P->Nam);
     if(i>9){ i=0; fputws(L"\n//",Fo); }
    }
    else fputwc('}',Fo);
   }
   if(I=FindInstLast(L"РАЗМЕРЫ ЗВЕНЬЕВ ВЕРХА")){
    P=I->First;
    for(;P;P=P->Sled,i++){
     if(P->Nam) fwprintf(Fo,L"%s",P->Nam);
     if(P->Sled){
      fputws(L", ",Fo);
      if(i>9){ i=0; fputws(L"\n//",Fo); }
     }
     else fputwc('}',Fo);
    }
   }
   fputws(L" - размеры звеньев\n",Fo);
  }
  fputws(L"\n/*{",Fo);
  if(I=FindInstLast(L"РАЗМЕРЫ ЗВЕНЬЕВ НИЗА")){ Parm *P=I->First;
   for(i=1;P;P=P->Sled,i++){
    fwprintf(Fo,L"%0.9f",P->Nam&&FindVary(P->Nam,&V)?Val(V->Znach):0);
    if(P->Sled) fputws(L", ",Fo);
    if(i>4){ i=0; fputws(L"\n    ",Fo); }
  }}
  if(I=FindInstLast(L"РАЗМЕРЫ ЗВЕНЬЕВ ВЕРХА")){ Parm *P=I->First;
   fputws(L", ",Fo);
   for(;P;P=P->Sled,i++){
    fwprintf(Fo,L"%0.9f",P->Nam&&FindVary(P->Nam,&V)?Val(V->Znach):0);
    if(P->Sled) fputws(L", ",Fo);
    if(i>4){ i=0; fputws(L"\n    ",Fo); }
  }}
  fputws(L"}*/\n",Fo);
  if(I=FindInstLast(L"КООРДИНАТЫ ЦЕНТРОВ МАСС НИЗА")){ Parm *P=I->First;
   fwprintf(Fo,L"//Cm={");
   for(i=1;P;P=P->Sled,i++){
    if(P->Nam) fwprintf(Fo,L"%s, ",P->Nam);
    if(i>9){ i=0; fputws(L"\n//",Fo); }
   }
   if(I=FindInstLast(L"КООРДИНАТЫ ЦЕНТРОВ МАСС ВЕРХА")){
    P=I->First;
    for(;P;P=P->Sled,i++){
     if(P->Nam) fwprintf(Fo,L"%s",P->Nam);
     if(P->Sled) fputws(L", ",Fo);
     if(i>9){ i=0; fputws(L"\n//",Fo); }
    }
    fputws(L"} - координаты центров масс\n",Fo);
  }}
  fputws(L"/*\n{",Fo);
  if(I=FindInstLast(L"КООРДИНАТЫ ЦЕНТРОВ МАСС НИЗА")){ Parm *P=I->First;
   for(i=1;P;P=P->Sled,i++){
    fwprintf(Fo,L"%0.9f",P->Nam&&FindVary(P->Nam,&V)?Val(V->Znach):0);
    if(P->Sled) fputws(L", ",Fo);
    if(i>4){ i=0; fputws(L"\n    ",Fo); }
  }}
  if(I=FindInstLast(L"КООРДИНАТЫ ЦЕНТРОВ МАСС ВЕРХА")){ Parm *P=I->First;
   for(;P;P=P->Sled,i++){
    fwprintf(Fo,L"%0.9f",P->Nam&&FindVary(P->Nam,&V)?Val(V->Znach):0);
    if(P->Sled) fputws(L", ",Fo);
    if(i>4){ i=0; fputws(L"\n    ",Fo); }
  }}
  fputws(L"}*/\n",Fo);
  if(I=FindInstLast(L"МАССЫ")){ Parm *P=I->First;
   fwprintf(Fo,L"//Ms={");
   for(i=1;P;P=P->Sled,i++){
    if(P->Nam) fwprintf(Fo,L"%s",P->Nam);
    if(P->Sled){
     fputws(L", ",Fo);
     if(i>9){ i=0; fputws(L"\n//",Fo); }
   }}
   fputws(L"} - массы\n",Fo);
  }
  fputws(L"/*\n{",Fo);
  if(I=FindInstLast(L"МАССЫ")){ Parm *P=I->First;
   for(i=1;P;P=P->Sled,i++){
    if(P->Nam&&FindVary(P->Nam,&V)){
     fwprintf(Fo,L"%0.9f",V->Val);
     if(P->Sled) fputws(L", ",Fo);
     if(i>4){ i=0; fputws(L"\n    ",Fo); }
  }}}
  fputws(L"}*/\n\n",Fo);

  fwprintf(Fo,L" long double ");
  for(q=L->K,i=0;q;i++,q=q->Sled) fwprintf(Fo,L"%s, ",q->V->Name);
  fputws(L"\n    ",Fo);
  if(I=FindInstLast(L"КООРДИНАТЫ ЦЕНТРА МАСС")){
   for(Parm *P=I->First;P;P=P->Sled)
    if(P->Nam&&FindVary(P->Nam,&V))
     fwprintf(Fo,L"%s, ",P->Nam);
  }
  if(I=FindInstLast(L"КООРДИНАТЫ ТОЧЕК")){
   for(Parm *P=I->First;P;P=P->Sled)
    if(P->Nam&&FindVary(P->Nam,&V))
     fwprintf(Fo,L"%s, ",P->Nam);
  }
  fputws(L"\n    ",Fo);
  if(I=FindInstLast(L"РАЗМЕРЫ ЗВЕНЬЕВ НИЗА")){
   for(Parm *P=I->First;P;P=P->Sled)
    if(P->Nam&&FindVary(P->Nam,&V))
     fwprintf(Fo,L"%s, ",P->Nam);
  }
  fputws(L"\n    ",Fo);
  if(I=FindInstLast(L"РАЗМЕРЫ ЗВЕНЬЕВ ВЕРХА")){ Parm *P=I->First;
   for(int i=0;P;P=P->Sled){
    if(P->Nam&&FindVary(P->Nam,&V)){
     fwprintf(Fo,L"%s, ",P->Nam); i++; if(!(i%10)) fputws(L"\n    ",Fo);
  }}}
  if(I=FindInstLast(L"КООРДИНАТЫ ЦЕНТРОВ МАСС НИЗА")){
   for(Parm *P=I->First;P;P=P->Sled)
    if(P->Nam&&FindVary(P->Nam,&V))
     fwprintf(Fo,L"%s, ",P->Nam);
  }
  fputws(L"\n    ",Fo);
  if(I=FindInstLast(L"КООРДИНАТЫ ЦЕНТРОВ МАСС ВЕРХА")){ Parm *P=I->First;
   for(int i=0;P;P=P->Sled){
    if(P->Nam&&FindVary(P->Nam,&V)){
     fwprintf(Fo,L"%s, ",P->Nam); i++; if(!(i%10)) fputws(L"\n    ",Fo);
  }}}
  fputws(L"\n    ",Fo);
  if(I=FindInstLast(L"УГЛЫ ВЕРХА")){ Parm *P=I->First;
   for(int i=0;P;P=P->Sled){
    if(P->Nam&&FindVary(P->Nam,&V)){
     fwprintf(Fo,L"%s, ",P->Nam); i++; if(!(i%8)) fputws(L"\n    ",Fo);
  }}}
  fputws(L"\n    ",Fo);
  if(I=FindInstLast(L"МАССЫ")){
   for(Parm *P=I->First;P;P=P->Sled)
    if(P->Nam&&FindVary(P->Nam,&V))
     fwprintf(Fo,L"%s, ",P->Nam);
  }
  fputws(L"\n    ",Fo);
  for(q=CvUr;q;q=q->Sled)//основные переменные уравнений
   fwprintf(Fo,q->Sled ? L"%s, " : L"%s,\n",q->V->Name);
  if(REPUNOPNAVARY){
   for(q=CuUr;q;q=q->Sled)//переменные, равные унарным операциям
	fwprintf(Fo,q->Sled ? L"%s, " : L"%s;",q->V->Name);
  }
  fputws(L"\n//Присвоение значений обобщенным координатам\n",Fo);
  for(q=L->K,i=0;q;q=q->Sled){
   swprintf(Inf,L" %s=qk[%d];",q->V->Name,i); fputws(Inf,Fo);
   i++; if(!(i%4)) fputwc('\n',Fo);
  }
  if(I=FindInstLast(L"УГЛЫ ВЕРХА")){ Parm *P=I->First;
   fputws(L"\n//Присвоение значений углам верха\n",Fo);
   for(int i=0;P;P=P->Sled){
    fwprintf(Fo,L" %s=uv[%d];",P->Nam,i); i++; if(!(i%4)) fputwc('\n',Fo);
  }}
  if(I=FindInstLast(L"КООРДИНАТЫ ЦЕНТРА МАСС")){ Parm *P=I->First;
   fputws(L"\n//Присвоение значений координатам центра масс\n",Fo);
   for(int i=0;P;P=P->Sled,i++)
    fwprintf(Fo,L" %s=rC[%d];",P->Nam,i);
  }
  if(I=FindInstLast(L"КООРДИНАТЫ ТОЧЕК")){ Parm *P=I->First;
   fputws(L"\n//Присвоение значений координатам точек\n",Fo);
   for(int i=0;P;P=P->Sled,i++)
    fwprintf(Fo,i<3? L" %s=r6R[%d];" : L" %s=r6L[%d];",P->Nam,i%3);
  }
  if(I=FindInstLast(L"РАЗМЕРЫ ЗВЕНЬЕВ НИЗА")){ int i=0;
   fputws(L"\n//Присвоение значений размерам звеньев низа\n",Fo);
   for(Parm *P=I->First;P;P=P->Sled){
    fwprintf(Fo,L" %s=Ss[%d];",P->Nam,i); i++; if(!(i%7)) fputwc('\n',Fo);
   }
   if(I=FindInstLast(L"РАЗМЕРЫ ЗВЕНЬЕВ ВЕРХА")){
    fputws(L"\n//Присвоение значений размерам звеньев верха\n",Fo);
    for(Parm *P=I->First;P;P=P->Sled){
     fwprintf(Fo,L" %s=Ss[%d];",P->Nam,i); i++; if(!(i%7)) fputwc('\n',Fo);
   }}
   if(I=FindInstLast(L"КООРДИНАТЫ ЦЕНТРОВ МАСС НИЗА")){
    fputws(L"\n//Присвоение значений координатам центров масс низа\n",Fo); i=0;
    for(Parm *P=I->First;P;P=P->Sled){
     fwprintf(Fo,L" %s=Cm[%d];",P->Nam,i); i++; if(!(i%7)) fputwc('\n',Fo);
   }}
   if(I=FindInstLast(L"КООРДИНАТЫ ЦЕНТРОВ МАСС ВЕРХА")){
    fputws(L"\n//Присвоение значений координатам центров масс верха\n",Fo);
    for(Parm *P=I->First;P;P=P->Sled){
     fwprintf(Fo,L" %s=Cm[%d];",P->Nam,i); i++; if(!(i%7)) fputwc('\n',Fo);
  }}}
  if(I=FindInstLast(L"МАССЫ")){ Parm *P=I->First;
   fputws(L"\n//Присвоение значений массам\n",Fo);
   for(int i=0;P;P=P->Sled){
    fwprintf(Fo,L" %s=Ms[%d];",P->Nam,i); i++; if(!(i%7)) fputwc('\n',Fo);
  }}
  fputws(L"\n\n//Переменные",Fo);
  if(REPUNOPNAVARY){
   for(q=CuUr;q;q=q->Sled){//переменные, равные унарным операциям
    fputws(L"\n ",Fo); q->V->U=1;
    PrintForm(true,Fo,q->V->Name,q->V->Znach);
  }}
  for(q=CvUr;q;q=q->Sled){ Form F;
   if(REPUNOPNAVARY){
    F.V=q->V;
    UnOpRepVary(F,CuUr);
   }
   fputws(L"\n ",Fo);
   PrintForm(true,Fo,q->V->Name,q->V->Znach);
  }
  fputws(L"\n\n//Уравнения:",Fo);
  for(int i=0;i<N;i++){
   swprintf(Inf,L"Ur[%2d]",i); fputws(L"\n ",Fo);
   PrintForm(true,Fo,Inf,Uravn[i]);
  }
  fputws(L"\n}",Fo);
  fclose(Fo);
 }
 if((Fi=_wfopen(L"tmp.cpp",L"r"))&&(Fo=_wfopen(L"CalcUravnCOM.cpp",L"w"))){
  while(fgetws(Inf,4095,Fi))
   CopyZam(Fo,Inf);
  fclose(Fi); fclose(Fo);
 }
 if(Fo=_wfopen(L"tmp.cpp",L"w")){
  fputws(L"void CalcUqCOM(long double *qk,long double *Ss,long double *Cm,L"
   L"\n               long double *Ms,long double **Uq){\n",Fo);
  if(I=FindInstLast(L"РАЗМЕРЫ ЗВЕНЬЕВ НИЗА")){ Parm *P=I->First; int i;
   fwprintf(Fo,L"//Ss={");
   for(i=1;P;P=P->Sled,i++){
    if(P->Nam) fwprintf(Fo,L"%s",P->Nam);
    if(P->Sled){
     fputws(L", ",Fo);
     if(i>9){ i=0; fputws(L"\n//",Fo); }
   }}
   fputws(L"} - размеры звеньев низа\n",Fo);
  }
  fputws(L"/*\n{",Fo);
  if(I=FindInstLast(L"РАЗМЕРЫ ЗВЕНЬЕВ НИЗА")){ Parm *P=I->First;
   for(i=1;P;P=P->Sled,i++){
    fwprintf(Fo,L"%0.9f",P->Nam&&FindVary(P->Nam,&V)?Val(V->Znach):0);
    if(P->Sled){
     fputws(L", ",Fo);
     if(i>4){ i=0; fputws(L"\n",Fo); }
  }}}
  fputws(L"}*/\n",Fo);
  if(I=FindInstLast(L"КООРДИНАТЫ ЦЕНТРОВ МАСС НИЗА")){ Parm *P=I->First;
   fwprintf(Fo,L"//Cm={");
   for(i=1;P;P=P->Sled,i++){
    if(P->Nam) fwprintf(Fo,L"%s",P->Nam);
    if(P->Sled){
     fputws(L", ",Fo);
     if(i>9){ i=0; fputws(L"\n//",Fo); }
   }}
   fputws(L"} - координаты центров масс низа\n",Fo);
  }
  fputws(L"/*\n{",Fo);
  if(I=FindInstLast(L"КООРДИНАТЫ ЦЕНТРОВ МАСС НИЗА")){ Parm *P=I->First;
   for(i=1;P;P=P->Sled,i++){
    fwprintf(Fo,L"%0.9f",P->Nam&&FindVary(P->Nam,&V)?Val(V->Znach):0);
    if(P->Sled){
     fputws(L", ",Fo);
     if(i>4){ i=0; fputws(L"\n",Fo); }
  }}}
  fputws(L"}*/\n",Fo);
  if(I=FindInstLast(L"МАССЫ")){ Parm *P=I->First;
   fwprintf(Fo,L"//Ms={");
   for(i=0;P;P=P->Sled,i++){
    if(P->Nam) fwprintf(Fo,L"%s",P->Nam);
    if(P->Sled){
     fputws(L", ",Fo);
     if(i>9){ i=0; fputws(L"\n//",Fo); }
   }}
   fputws(L"} - массы\n",Fo);
  }
  fputws(L"/*\n{",Fo);
  if(I=FindInstLast(L"МАССЫ")){ Parm *P=I->First;
   for(i=1;P;P=P->Sled,i++){
    if(P->Nam&&FindVary(P->Nam,&V)){
     fwprintf(Fo,L"%0.9f",V->Val);
     if(P->Sled){
      fputws(L", ",Fo);
      if(i>4){ i=0; fputws(L"\n",Fo); }
  }}}}
  fputws(L"}*/\n\n",Fo);
  fwprintf(Fo,L" long double ");
  for(q=L->K,i=0;q;q=q->Sled){
   fwprintf(Fo,L"%s, ",q->V->Name); i++; if(!(i%4)) fputws(L"\n    ",Fo);
  }
  fputws(L"\n    ",Fo);
  if(I=FindInstLast(L"РАЗМЕРЫ ЗВЕНЬЕВ НИЗА")){
   for(Parm *P=I->First;P;P=P->Sled)
    if(P->Nam&&FindVary(P->Nam,&V))
     fwprintf(Fo,L"%s, ",P->Nam);
  }
  fputws(L"\n    ",Fo);
  if(I=FindInstLast(L"КООРДИНАТЫ ЦЕНТРОВ МАСС НИЗА")){
   for(Parm *P=I->First;P;P=P->Sled)
    if(P->Nam&&FindVary(P->Nam,&V))
     fwprintf(Fo,L"%s, ",P->Nam);
  }
  fputws(L"\n    ",Fo);
  if(I=FindInstLast(L"МАССЫ")){
   for(Parm *P=I->First;P;P=P->Sled){
    if(P->Nam&&FindVary(P->Nam,&V)){
     fwprintf(Fo,L"%s,L",P->Nam);
  }}}
  fputws(L"\n    ",Fo);
  if(REPUNOPNAVARY){
   for(q=CuUq;q;q=q->Sled)//переменные, равные унарным операциям
    fwprintf(Fo,L"%s, ",q->V->Name);
  }
  for(q=CvUq;q;q=q->Sled){
   fwprintf(Fo,q->Sled ? L"%s, " : L"%s;\n",q->V->Name);
  }
  fputws(L"\n//Присвоение значений обобщенным координатам\n",Fo);
  for(q=L->K,i=0;q;q=q->Sled){
   fwprintf(Fo,L" %s=qk[%d];",q->V->Name,i);
   i++; if(!(i%4)) fputwc('\n',Fo);
  }
  fputws(L"\n//Присвоение значений размерам звеньев низа\n",Fo);
  if(I=FindInstLast(L"РАЗМЕРЫ ЗВЕНЬЕВ НИЗА")){ Parm *P=I->First;
   for(int i=0;P;P=P->Sled){
    fwprintf(Fo,L" %s=Ss[%d];",P->Nam,i); i++; if(!(i%7)) fputwc('\n',Fo);
  }}
  fputws(L"\n//Присвоение значений координатам центров масс низа\n",Fo);
  if(I=FindInstLast(L"КООРДИНАТЫ ЦЕНТРОВ МАСС НИЗА")){ Parm *P=I->First;
   for(int i=0;P;P=P->Sled){
    fwprintf(Fo,L" %s=Cm[%d];",P->Nam,i); i++; if(!(i%7)) fputwc('\n',Fo);
  }}
  fputws(L"\n//Присвоение значений массам\n",Fo);
  if(I=FindInstLast(L"МАССЫ")){ Parm *P=I->First;
   for(int i=0;P;P=P->Sled){
    fwprintf(Fo,L" %s=Ms[%d];",P->Nam,i); i++; if(!(i%7)) fputwc('\n',Fo);
  }}
  fputws(L"\n\n//Переменные",Fo);
  if(REPUNOPNAVARY){
   for(q=CuUq;q;q=q->Sled){//переменные, равные унарным операциям
    fputws(L"\n ",Fo); q->V->U=1;
    PrintForm(true,Fo,q->V->Name,q->V->Znach);
  }}
  for(q=CvUq;q;q=q->Sled){ Form F;
   if(REPUNOPNAVARY){
    F.V=q->V;
    UnOpRepVary(F,CuUq);
   }
   fputws(L"\n ",Fo);
   PrintForm(true,Fo,q->V->Name,q->V->Znach);
  }
  fputws(L"\n\n//Матрица (якобиан уравнений):",Fo);
  for(int i=0;i<N;i++){
   for(int j=0;j<N;j++){
    swprintf(Inf,L"Uq[%2d][%2d]",i,j); fputws(L"\n ",Fo);
    PrintForm(true,Fo,Inf,Af[i][j]);
  }}
  fputws(L"\n}\n",Fo);
  fclose(Fo);
 }
 if((Fi=_wfopen(L"tmp.cpp",L"r"))&&(Fo=_wfopen(L"CalcUqCOM.cpp",L"w"))){
  while(fgetws(Inf,4095,Fi))
   CopyZam(Fo,Inf);
  fclose(Fi); fclose(Fo);
 }
 DelAllList((void **)&CvUr);
 DelAllList((void **)&CuUr);
 DelAllList((void **)&CvUq);
 DelAllList((void **)&CuUq);
 DelAllList((void **)&Ce);
}
//---------------------------------------------------------------------------
void CreateProcInvCOMVel2(){
}
//---------------------------------------------------------------------------
void CreateProcInvCOMVel(){
//составим список имен переменных, входящих в уравнения
 Cord *Cv=NULL,*Ce=NULL,*q; FILE *Fo,*Fi; int i; Root *I; Vary *V;
//сначала составим список исключений по инструкциям КООРДИНАТЫ ЦЕНТРА МАСС,
// КООРДИНАТЫ ТОЧЕК, РАЗМЕРЫ ЗВЕНЬЕВ, КООРДИНАТЫ ЦЕНТРОВ МАСС и МАССЫ
 if(I=FindInstLast(L"КООРДИНАТЫ ЦЕНТРА МАСС"))
  for(Parm *P=I->First;P;P=P->Sled)
   if(P->Nam&&FindVary(P->Nam,&V)) TakeCordDefVars(&Ce,V);
 if(I=FindInstLast(L"КООРДИНАТЫ ТОЧЕК"))
  for(Parm *P=I->First;P;P=P->Sled)
   if(P->Nam&&FindVary(P->Nam,&V)) TakeCordDefVars(&Ce,V);
 if(I=FindInstLast(L"РАЗМЕРЫ ЗВЕНЬЕВ НИЗА"))
  for(Parm *P=I->First;P;P=P->Sled)
   if(P->Nam&&FindVary(P->Nam,&V)) TakeCordDefVars(&Ce,V);
 if(I=FindInstLast(L"РАЗМЕРЫ ЗВЕНЬЕВ ВЕРХА"))
  for(Parm *P=I->First;P;P=P->Sled)
   if(P->Nam&&FindVary(P->Nam,&V)) TakeCordDefVars(&Ce,V);
 if(I=FindInstLast(L"КООРДИНАТЫ ЦЕНТРОВ МАСС НИЗА"))
  for(Parm *P=I->First;P;P=P->Sled)
   if(P->Nam&&FindVary(P->Nam,&V)) TakeCordDefVars(&Ce,V);
 if(I=FindInstLast(L"КООРДИНАТЫ ЦЕНТРОВ МАСС ВЕРХА"))
  for(Parm *P=I->First;P;P=P->Sled)
   if(P->Nam&&FindVary(P->Nam,&V)) TakeCordDefVars(&Ce,V);
 if(I=FindInstLast(L"УГЛЫ ВЕРХА"))
  for(Parm *P=I->First;P;P=P->Sled)
   if(P->Nam&&FindVary(P->Nam,&V)) TakeCordDefVars(&Ce,V);
 if(I=FindInstLast(L"МАССЫ"))
  for(Parm *P=I->First;P;P=P->Sled)
   if(P->Nam&&FindVary(P->Nam,&V)) TakeCordDefVars(&Ce,V);
//отсортируем имена переменных по зависимости их друг от друга
 for(int i=0;i<N;i++){
  VarsFromFormDefVars(Bf[i],&Cv,Ce);
  for(int j=0;j<N;j++)
   VarsFromFormDefVars(Af[i][j],&Cv,Ce);
 }
 if(Fo=_wfopen(L"tmp.cpp",L"w")){
  fwprintf(Fo,L"#include <alloc.h>\n");
  fwprintf(Fo,L"#include <math.h>\n\n");
  fwprintf(Fo,L" long double ");
  for(q=L->K,i=0;q;i++,q=q->Sled) fwprintf(Fo,L"%s, ",q->V->Name);
  fputws(L"\n    ",Fo);
  if(I=FindInstLast(L"КООРДИНАТЫ ЦЕНТРА МАСС")){
   for(Parm *P=I->First;P;P=P->Sled)
    if(P->Nam&&FindVary(P->Nam,&V))
     fwprintf(Fo,L"%s, ",P->Nam);
  }
  if(I=FindInstLast(L"КООРДИНАТЫ ТОЧЕК")){
   for(Parm *P=I->First;P;P=P->Sled)
    if(P->Nam&&FindVary(P->Nam,&V))
     fwprintf(Fo,L"%s, ",P->Nam);
  }
  fputws(L"\n    ",Fo);
  if(I=FindInstLast(L"РАЗМЕРЫ ЗВЕНЬЕВ НИЗА")){
   for(Parm *P=I->First;P;P=P->Sled)
    if(P->Nam&&FindVary(P->Nam,&V))
     fwprintf(Fo,L"%s, ",P->Nam);
  }
  fputws(L"\n    ",Fo);
  if(I=FindInstLast(L"РАЗМЕРЫ ЗВЕНЬЕВ ВЕРХА")){ Parm *P=I->First;
   for(int i=0;P;P=P->Sled){
    if(P->Nam&&FindVary(P->Nam,&V)){
     fwprintf(Fo,L"%s, ",P->Nam); i++; if(!(i%10)) fputws(L"\n    ",Fo);
  }}}
  if(I=FindInstLast(L"КООРДИНАТЫ ЦЕНТРОВ МАСС НИЗА")){
   for(Parm *P=I->First;P;P=P->Sled)
    if(P->Nam&&FindVary(P->Nam,&V))
     fwprintf(Fo,L"%s, ",P->Nam);
  }
  fputws(L"\n    ",Fo);
  if(I=FindInstLast(L"КООРДИНАТЫ ЦЕНТРОВ МАСС ВЕРХА")){ Parm *P=I->First;
   for(int i=0;P;P=P->Sled){
    if(P->Nam&&FindVary(P->Nam,&V)){
     fwprintf(Fo,L"%s, ",P->Nam); i++; if(!(i%10)) fputws(L"\n    ",Fo);
  }}}
  fputws(L"\n    ",Fo);
  if(I=FindInstLast(L"УГЛЫ ВЕРХА")){ Parm *P=I->First;
   for(int i=0;P;P=P->Sled){
    if(P->Nam&&FindVary(P->Nam,&V)){
     fwprintf(Fo,L"%s, ",P->Nam); i++; if(!(i%8)) fputws(L"\n    ",Fo);
  }}}
  fputws(L"\n    ",Fo);
  if(I=FindInstLast(L"МАССЫ")){
   for(Parm *P=I->First;P;P=P->Sled)
    if(P->Nam&&FindVary(P->Nam,&V))
     fwprintf(Fo,L"%s,L",P->Nam);
  }
  fputws(L"\n    ",Fo);
  for(q=Cv;q;q=q->Sled){
   fwprintf(Fo,q->Sled ? L"%s, " : L"%s;\n",q->V->Name);
  }
  fclose(Fo);
 }
 if((Fi=_wfopen(L"tmp.cpp",L"r"))&&(Fo=_wfopen(L"InvKinByVel.cpp",L"w"))){
  while(fgetws(Inf,4095,Fi))
   CopyZam(Fo,Inf);
  fclose(Fi); fclose(Fo);
 }
 if(Fo=_wfopen(L"InvKinByVel.cpp",L"a")){
//запишем процедуру Рунге-Кутты
  if(!(Fi=_wfopen(L"InvKinByVelRep.cpp",L"r"))){
   Application->MessageBoxA(L"Не найден файл \"InvKinByVelRep.cpp\"",
    L"Опроцедурить уравнения и якобиан",MB_OK);
   return;
  }
  while(fgetws(Inf,1023,Fi)) fputws(Inf,Fo);
  fclose(Fo);
 }
 if(Fo=_wfopen(L"tmp.cpp",L"w")){ int i,j;
  fputws(L"void CalcAB(long double t){\n",Fo);
  fputws(L"//Переменные",Fo);
  for(q=Cv;q;q=q->Sled){
   fputws(L"\n ",Fo);
   PrintForm(true,Fo,q->V->Name,q->V->Znach);
  }
  for(int i=0;i<N;i++){
   swprintf(Inf,L"   B[%2d]",i); fputws(L"\n ",Fo);
   PrintForm(true,Fo,Inf,Bf[i]);
   for(int j=0;j<N;j++){
    swprintf(Inf,L"   A[%2d][%2d]",i,j); fputws(L"\n ",Fo);
    PrintForm(true,Fo,Inf,Af[i][j]);
  }}
  fputws(L"\n}\n",Fo);
  fclose(Fo);
 }
 if((Fi=_wfopen(L"tmp.cpp",L"r"))&&(Fo=_wfopen(L"InvKinByVel.cpp",L"a"))){
  while(fgetws(Inf,4095,Fi))
   CopyZam(Fo,Inf);
  fclose(Fi); fclose(Fo);
 }
 if(Fo=_wfopen(L"InvKinByVel.cpp",L"a")){
  fputws(L"//-------------------------------------------------------------\n",Fo);
  fputws(L"void CalcR(long double t){\n",Fo);
  fputws(L" static Rez=1; int i,j;\n",Fo);
  fputws(L" if(!Rez) return;\n",Fo);
  fputws(L" CalcAB(t);\n",Fo);
  fputws(L" Rez=SLAU(N,A,B,RK_z,false);\n",Fo);
  fputws(L" if(!Rez){//\"Система уравнений несовместна!\" }\n",Fo);
  fputws(L"\n}\n",Fo);
  fclose(Fo);
 }
//---------------------------------------------------------------------------
 if(Fo=_wfopen(L"tmp.cpp",L"w")){ int i;
  fputws(L"//-------------------------------------------------------------\n",Fo);
  fputws(L"void InverseKinByVelos(long double Tn,long double Tk,long double Eps,int Kt,\n",Fo);
  fputws(L" long double *qk,long double *uv,long double *rC,\n",Fo);
  fputws(L" long double *r6R,long double *r6L,long double *Ss,long double *Cm,\n",Fo);
  fputws(L" long double *Ms){\n",Fo);
  fputws(L" long double dt=(Tk-Tn)/Kt,t=Tn,*yk; int i,N=12;\n",Fo);
  if(FindInstLast(L"УГЛЫ ВЕРХА")){
   fputws(L"//uv={",Fo);
   for(Parm *P=I->First;P;P=P->Sled){ int i=0;
    if(P->Nam&&FindVary(P->Nam,&V)){
     if(i>6){ fputwc('\n',Fo); i=0; } else i++;
     fputws(P->Nam,Fo);
     if(P->Sled) fputwc(',',Fo); else fputws(L"} - углы верха\n",Fo);
//  fputws(L"//uv={omega,tau,ro,hi,psi,lambda$L,mu$L,jota$L,kappa$L,\n"
//        L"//    lambda$R,mu$R,jota$R,kappa$R} - углы верха\n",Fo);
  }}}
  fputws(L"//rC={XC,YC,ZC}, r6R={X6R,Y6R,Z6R}, r6L={X6L,Y6L,Z6L}\n",Fo);
  if(I=FindInstLast(L"РАЗМЕРЫ ЗВЕНЬЕВ НИЗА")){ Parm *P=I->First;
   fwprintf(Fo,L"//Ss={");
   for(i=1;P;P=P->Sled,i++){
    if(P->Nam) fwprintf(Fo,L"%s, ",P->Nam);
    if(i>9){ i=0; fputws(L"\n//",Fo); }
   }
   if(I=FindInstLast(L"РАЗМЕРЫ ЗВЕНЬЕВ ВЕРХА")){
    P=I->First; fwprintf(Fo,L"%s",P->Nam);
    for(;P;P=P->Sled,i++){
     if(P->Nam) fwprintf(Fo,L"%s",P->Nam);
     if(P->Sled) fputws(L", ",Fo);
     if(i>9){ i=0; fputws(L"\n//",Fo); }
    }
    fputws(L"} - размеры звеньев\n",Fo);
  }}
  fputws(L"/*\n{",Fo);
  if(I=FindInstLast(L"РАЗМЕРЫ ЗВЕНЬЕВ НИЗА")){ Parm *P=I->First;
   for(i=1;P;P=P->Sled,i++){
    fwprintf(Fo,L"%0.9f",P->Nam&&FindVary(P->Nam,&V)?Val(V->Znach):0);
    if(P->Sled) fputws(L", ",Fo);
    if(i>4){ i=0; fputws(L"\n    ",Fo); }
  }}
  if(I=FindInstLast(L"РАЗМЕРЫ ЗВЕНЬЕВ ВЕРХА")){ Parm *P=I->First;
   for(;P;P=P->Sled,i++){
    fwprintf(Fo,L"%0.9f",P->Nam&&FindVary(P->Nam,&V)?Val(V->Znach):0);
    if(P->Sled) fputws(L", ",Fo);
    if(i>4){ i=0; fputws(L"\n    ",Fo); }
  }}
  fputws(L"}*/\n",Fo);
  if(I=FindInstLast(L"КООРДИНАТЫ ЦЕНТРОВ МАСС НИЗА")){ Parm *P=I->First;
   fwprintf(Fo,L"//Cm={");
   for(i=1;P;P=P->Sled,i++){
    if(P->Nam) fwprintf(Fo,L"%s, ",P->Nam);
    if(i>9){ i=0; fputws(L"\n//",Fo); }
   }
   if(I=FindInstLast(L"КООРДИНАТЫ ЦЕНТРОВ МАСС ВЕРХА")){
    P=I->First;
    for(;P;P=P->Sled,i++){
     if(P->Nam) fwprintf(Fo,L"%s",P->Nam);
     if(P->Sled) fputws(L", ",Fo);
     if(i>9){ i=0; fputws(L"\n//",Fo); }
    }
    fputws(L"} - координаты центров масс\n",Fo);
  }}
  fputws(L"/*\n{",Fo);
  if(I=FindInstLast(L"КООРДИНАТЫ ЦЕНТРОВ МАСС НИЗА")){ Parm *P=I->First;
   for(i=1;P;P=P->Sled,i++){
    fwprintf(Fo,L"%0.9f",P->Nam&&FindVary(P->Nam,&V)?Val(V->Znach):0);
    if(P->Sled) fputws(L", ",Fo);
    if(i>4){ i=0; fputws(L"\n    ",Fo); }
  }}
  if(I=FindInstLast(L"КООРДИНАТЫ ЦЕНТРОВ МАСС ВЕРХА")){ Parm *P=I->First;
   for(;P;P=P->Sled,i++){
    fwprintf(Fo,L"%0.9f",P->Nam&&FindVary(P->Nam,&V)?Val(V->Znach):0);
    if(P->Sled) fputws(L", ",Fo);
    if(i>4){ i=0; fputws(L"\n    ",Fo); }
  }}
  fputws(L"}*/\n",Fo);
  if(I=FindInstLast(L"МАССЫ")){ Parm *P=I->First;
   fwprintf(Fo,L"//Ms={");
   for(i=1;P;P=P->Sled,i++){
    if(P->Nam) fwprintf(Fo,L"%s",P->Nam);
    if(P->Sled){
     fputws(L", ",Fo);
     if(i>9){ i=0; fputws(L"\n//",Fo); }
   }}
   fputws(L"} - массы\n",Fo);
  }
  fputws(L"/*\n{",Fo);
  if(I=FindInstLast(L"МАССЫ")){ Parm *P=I->First;
   for(i=1;P;P=P->Sled,i++){
    if(P->Nam&&FindVary(P->Nam,&V)){
     fwprintf(Fo,L"%0.9f",V->Val);
     if(P->Sled) fputws(L", ",Fo);
     if(i>4){ i=0; fputws(L"\n    ",Fo); }
  }}}
  fputws(L"}*/\n",Fo);
  fputws(L"\n//Присвоение значений обобщенным координатам\n",Fo);
  for(q=L->K,i=0;q;q=q->Sled){
   swprintf(Inf,L" %s=qk[%d];",q->V->Name,i); fputws(Inf,Fo);
   i++; if(!(i%4)) fputwc('\n',Fo);
  }
  fputws(L"\n//Присвоение значений углам верха\n",Fo);
  if(I=FindInstLast(L"УГЛЫ ВЕРХА")){ Parm *P=I->First;
   for(int i=0;P;P=P->Sled){
    fwprintf(Fo,L" %s=uv[%d];",P->Nam,i); i++; if(!(i%4)) fputwc('\n',Fo);
  }}
  fputws(L"\n//Присвоение значений координатам центра масс\n",Fo);
  if(I=FindInstLast(L"КООРДИНАТЫ ЦЕНТРА МАСС")){ Parm *P=I->First;
   for(int i=0;P;P=P->Sled,i++)
    fwprintf(Fo,L" %s=rC[%d];",P->Nam,i);
  }
  fputws(L"\n//Присвоение значений координатам точек\n",Fo);
  if(I=FindInstLast(L"КООРДИНАТЫ ТОЧЕК")){ Parm *P=I->First;
   for(int i=0;P;P=P->Sled,i++)
    fwprintf(Fo,i<3? L" %s=r6R[%d];" : L" %s=r6L[%d];",P->Nam,i%3);
  }
  fputws(L"\n//Присвоение значений размерам звеньев низа\n",Fo);
  if(I=FindInstLast(L"РАЗМЕРЫ ЗВЕНЬЕВ НИЗА")){ int i=0;
   for(Parm *P=I->First;P;P=P->Sled){
    fwprintf(Fo,L" %s=Ss[%d];",P->Nam,i); i++; if(!(i%7)) fputwc('\n',Fo);
   }
   fputws(L"\n//Присвоение значений размерам звеньев верха\n",Fo);
   if(I=FindInstLast(L"РАЗМЕРЫ ЗВЕНЬЕВ ВЕРХА")){
    for(Parm *P=I->First;P;P=P->Sled){
     fwprintf(Fo,L" %s=Ss[%d];",P->Nam,i); i++; if(!(i%7)) fputwc('\n',Fo);
   }}
   fputws(L"\n//Присвоение значений координатам центров масс низа\n",Fo); i=0;
   if(I=FindInstLast(L"КООРДИНАТЫ ЦЕНТРОВ МАСС НИЗА")){
    for(Parm *P=I->First;P;P=P->Sled){
     fwprintf(Fo,L" %s=Cm[%d];",P->Nam,i); i++; if(!(i%7)) fputwc('\n',Fo);
   }}
   fputws(L"\n//Присвоение значений координатам центров масс верха\n",Fo);
   if(I=FindInstLast(L"КООРДИНАТЫ ЦЕНТРОВ МАСС ВЕРХА")){
    for(Parm *P=I->First;P;P=P->Sled){
     fwprintf(Fo,L" %s=Cm[%d];",P->Nam,i); i++; if(!(i%7)) fputwc('\n',Fo);
  }}}
  fputws(L"\n//Присвоение значений массам\n",Fo);
  if(I=FindInstLast(L"МАССЫ")){ Parm *P=I->First;
   for(int i=0;P;P=P->Sled){
    fwprintf(Fo,L" %s=Ms[%d];",P->Nam,i); i++; if(!(i%7)) fputwc('\n',Fo);
  }}
  fclose(Fo);
 }
 if((Fi=_wfopen(L"tmp.cpp",L"r"))&&(Fo=_wfopen(L"InvKinByVel.cpp",L"a"))){
  while(fgetws(Inf,4095,Fi))
   CopyZam(Fo,Inf);
  fclose(Fi); fclose(Fo);
 }
 if(Fo=_wfopen(L"InvKinByVel.cpp",L"a")){
  fputws(L" Y=(long double *)calloc(N,sizeof(long double));\n",Fo);
  fputws(L" YK=(long double *)calloc(N,sizeof(long double));\n",Fo);
  fputws(L" RK_z=(long double *)calloc(N,sizeof(long double));\n",Fo);
  fputws(L" RK_y1=(long double *)calloc(N,sizeof(long double));\n",Fo);
  fputws(L" RK_y2=(long double *)calloc(N,sizeof(long double));\n",Fo);
  fputws(L" RK_y3=(long double *)calloc(N,sizeof(long double));\n",Fo);
  fputws(L" RK_w=(long double *)calloc(N,sizeof(long double)); h_RK=-Eps;\n",Fo);
  fputws(L" for(int K=1;K<Kt;K++){\n",Fo);
  fputws(L"//Присвоение значений обобщенным координатам\n",Fo);
  fputws(L"  Y[0]=epsilon_bax_R; Y[1]=nu_bax_R;    Y[2] =beta_bax_R;    Y[3] =gamma_bax_R;\n",Fo);
  fputws(L"  Y[4]=teta_bax_R;    Y[5]=alfa_bax_R;  Y[6] =epsilon_bax_L; Y[7] =nu_bax_L;\n",Fo);
  fputws(L"  Y[8]=beta_bax_L;    Y[9]=gamma_bax_L; Y[10]=teta_bax_L;    Y[11]=alfa_bax_L;\n",Fo);
  fputws(L"  RUNGE_Ex(t,Y,N,CalcR,Eps,Eps,t+dt,YK);\n",Fo);
  fputws(L"  t+=dt; if(t>Tk) t=Tk;\n",Fo);
  fputws(L"//Присвоение значений обобщенным координатам\n",Fo);
  fputws(L"  epsilon_bax_R=YK[0]; nu_bax_R   =YK[1]; beta_bax_R   =YK[2];  gamma_bax_R=YK[3];\n",Fo);
  fputws(L"  teta_bax_R   =YK[4]; alfa_bax_R =YK[5]; epsilon_bax_L=YK[6];  nu_bax_L   =YK[7];\n",Fo);
  fputws(L"  beta_bax_L   =YK[8]; gamma_bax_L=YK[9]; teta_bax_L   =YK[10]; alfa_bax_L =YK[11];\n",Fo);
  fputws(L" }\n",Fo);
  fputws(L" qk[0]=epsilon_bax_R; qk[1]=nu_bax_R;    qk[2] =beta_bax_R;    qk[3] =gamma_bax_R;\n",Fo);
  fputws(L" qk[4]=teta_bax_R;    qk[5]=alfa_bax_R;  qk[6] =epsilon_bax_L; qk[7] =nu_bax_L;\n",Fo);
  fputws(L" qk[8]=beta_bax_L;    qk[9]=gamma_bax_L; qk[10]=teta_bax_L;    qk[11]=alfa_bax_L;\n",Fo);
  fputws(L" free(Y); free(YK); free(RK_z); free(RK_y1); free(RK_y2); free(RK_y3); free(RK_w);\n",Fo);
  fputws(L"}\n",Fo);
  fclose(Fo);
 }
 DelAllList((void **)&Cv);
}
//---------------------------------------------------------------------------
void CreateProcDir(){
 Cord *Cv=NULL; FILE *Fo,*Fi; int i; Root *I; Vary *V; Form F;
//составим список имен переменных, входящих в переменные,
//перечисленные в инструкции ВКЛЮЧИТЬ В ПРОЦЕДУРУ и отсортируем их
//по зависимости их друг от друга

 if(I=FindInstLast(L"ВКЛЮЧИТЬ В ПРОЦЕДУРУ")){
  for(Parm *P=I->First;P;P=P->Sled){
   if(P->Nam&&FindVary(P->Nam,&V)){
    F.V=V;
    VarsFromFormDefVars(F,&Cv,NULL);
  }}
  if(Fo=_wfopen(L"tmp.cpp",L"w")){ Cord *q;
   fputws(L"void GaitKoords(long double t,long double *r0,L"
    L"long double *r6R,long double *r6L){\n",Fo);
   fwprintf(Fo,L" long double ");
   for(q=Cv;q->Sled;q=q->Sled) fwprintf(Fo,L"%s, ",q->V->Name);
   fwprintf(Fo,L"%s;\n",q->V->Name);
   fputws(L"//Переменные",Fo);
   for(q=Cv;q;q=q->Sled){
    fputws(L"\n ",Fo);
    PrintForm(true,Fo,q->V->Name,q->V->Znach);
   }
   fputws(L"\n",Fo);
   fputws(L"//Сохранение значений координат\n",Fo);
   if(I=FindInstLast(L"ВКЛЮЧИТЬ В ПРОЦЕДУРУ")){ Parm *P; int i;
    for(P=I->First,i=0;P;P=P->Sled,i++)
	 fwprintf(Fo,i<3 ? L" r0[%d]=%s;" : i<6? L" r6R[%d]=%s;" : L" r6L[%d]=%s;",
      i%3,P->Nam);
    }
   fputws(L"\n}",Fo);
   fclose(Fo);
  }
  DelAllList((void **)&Cv);
  if((Fi=_wfopen(L"tmp.cpp",L"r"))&&(Fo=_wfopen(L"GaitKoords.cpp",L"w"))){
   while(fgetws(Inf,4095,Fi))
    CopyZam(Fo,Inf);
   fclose(Fi); fclose(Fo);
}}}
//---------------------------------------------------------------------------
void __fastcall TFormKinema::mToProcClick(TObject *Sender){
 CPP=true;
 if(INVERSE){
  if(!LAGRANGE){
   if(FindInstLast(L"КООРДИНАТЫ ЦЕНТРА МАСС")){
    if(VELOKIN==1)
     CreateProcInvCOMVel();
    if(VELOKIN==2)
     CreateProcInvCOMVel2();
    else
     CreateProcInvCOM();
   }
   else
    CreateProcInv();
 }}
 else CreateProcDir();
 Application->MessageBoxA(L"Задание успешно завершено!",L"ToProc",MB_OK);
 CPP=false;
}
//---------------------------------------------------------------------------
void __fastcall TFormKinema::FormShow(TObject *Sender){
 MReportClick(NULL);
}
//---------------------------------------------------------------------------
void __fastcall TFormKinema::mToProcKoordPointClick(TObject *Sender){
// if(CreateProcCalcKontrPoint())
//  mToProcKoordPoint->ImageIndex=44;
}
//---------------------------------------------------------------------------
void __fastcall TFormKinema::mCreateProcIdentSizesClick(TObject *Sender){
// if(CreateProcIdentSizes())
//  mCreateProcIdentSizes->ImageIndex=44;
}
//---------------------------------------------------------------------------
void __fastcall TFormKinema::mCreateProcIdentNewtonsClick(TObject *Sender){
// if(CreateProcIdentNewtons())
//  mCreateProcIdentNewtons->ImageIndex=44;
}
//---------------------------------------------------------------------------

