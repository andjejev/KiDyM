//---------------------------------------------------------------------------
#pragma hdrstop

#include "MultFilm.h"
#include <values.h>
#include <graphics.hpp>
#include <sys/stat.h>
#include <io.h>
#include <math.h>
#include "Animation.h"
#include "TablW.h"
#include <JPEG.hpp>
#include <stdio.h>
#include <types.hpp>
#include <dir.h>
#include <math.h>
#include <math.hpp>
#include <time.h>

//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
extern List *L;
extern int Kpodv,PUNKT,NYear,N,NzK,NPV,USEDLL;
extern bool __fastcall EndInt();
extern void FuncsQ(double x,Magazine *Names,double *y,bool First);
extern wchar_t InpFile[],HtmFile[],Inf[],HeadWork[],Autor[],CurrDir[],BMPFile[],
 JPGFile[],WORKDIR[],SimbInd,Shtm[];
extern double Kbmp,**Q,*YK,Tend;
extern FILE *HTMFILE;
extern Magazine *NamesKadr;
extern Cnst *Nul,*Pust;
extern TStringList *SLH;

Root *Irez; Parm *ParmMF,*ParmF; int Nkadr,Kinst;
TMenuItem *MultFilmItem; static long *Tmult; static int *Kkadr;
TJPEGImage *JPEG;
wchar_t BMPKadrs[256],JPGKadrs[256],Dir[256],BMPKadr[256],MtfFile[256];
double  d_x,d_sp,c_sp,b_sp,a_sp; bool ANIMATION3D;
AnsiString NameFont; int ItFont,ExFont,UnFont,SizeFont; TColor ColorFont;
Centroid *Rcentr; //корень списка центроид
Trajectory *Rtraek; //корень списка траекторий
//---------------------------------------------------------------------------
void FileToOpt(){
 wchar_t FileOpt[256],*s; FILE *F;
 wcscpy(FileOpt,InpFile);
 if(s=wcsrchr(FileOpt,'.')) wcscpy(s+1,L"opt");
 else wcscat(FileOpt,L".opt");
 if(!_waccess(FileOpt,0)&&(F=_wfopen(FileOpt,L"r"))){
  for(int i=0;i<31;i++) fgetws(Inf,1023,F);
  if(Fgets(Inf,1023,F)&&(s=wcschr(Inf,'#'))){
   for(s--;(*s==' ')&&(s>Inf);s--); s++; *s='\0'; NameFont=Inf;
  }
  else NameFont=L"Courier New";
  ItFont=Fgets(Inf,1023,F)?_wtoi(Inf):0;
  ExFont=Fgets(Inf,1023,F)?_wtoi(Inf):0;
  UnFont=Fgets(Inf,1023,F)?_wtoi(Inf):0;
  SizeFont=Fgets(Inf,1023,F)?_wtoi(Inf):12;
  ColorFont=Fgets(Inf,1023,F)?(TColor)_wtol(Inf):clBlack;
  fclose(F);
}}
//---------------------------------------------------------------------------
void __fastcall TMultFilms::mMFClick(TObject *Sender){
 wchar_t NameMult[256],*s,*b; Root *I; int K=KolElem(HeadMult); long Cl=clock();
 if(!Q){
  if(USEDLL) TakeDLL();
  CalcQSplns();
 }
 Cl=clock()-Cl;
 MultFilmItem=dynamic_cast<TMenuItem *>(Sender);
 swprintf(s=Inf,L"%s",MultFilmItem->Caption);
 for(b=BMPKadr;*s;s++) if(*s!='&') *b++=*s; *b='\0';
 if(*Tmult<=0){
  swprintf(Inf,L"Задайте длительность анимации в НАСТРОЙКЕ");
  Application->MessageBox(Inf,L"Ошибка!",MB_OK|MB_ICONERROR); return;
 }
 if(*Kkadr<=0){
  swprintf(Inf,L"Задайте частоту кадров в НАСТРОЙКЕ");
  Application->MessageBox(Inf,L"Ошибка!",MB_OK|MB_ICONERROR); return;
 }
 if((*Kkadr*(*Tmult))==1){
  swprintf(Inf,
   L"Задайте правильно частоту кадров и длительность анимации в НАСТРОЙКЕ");
  Application->MessageBox(Inf,L"Ошибка!",MB_OK|MB_ICONERROR); return;
 }
 if((int)Round(0.001*(*Kkadr)*(*Tmult))==0){
  swprintf(Inf,
   L"Увеличьте значение параметра \"Длительность анимации\" в НАСТРОЙКЕ");
  Application->MessageBox(Inf,L"Ошибка!",MB_OK|MB_ICONERROR); return;
 }
 if(Vals){ free(Vals); Vals=NULL; }
 DelAllMagazine(&NamesVarys);
 Head=BMPKadr;
 wcscpy(NameMult,Head.w_str());
 if(K>1){
  for(I=L->Inst;I;I=I->Sled){
   if((wcsstr(I->Name,L"АНИМАЦИЯ")==I->Name)&&(!wcscmp(I->First->Nam,NameMult))){
    Irez=I;
    ANIMATION3D=!wcscmp(I->Name,L"АНИМАЦИЯ3D");
    break;
  }}
  if(!Irez) return; I=Irez; ParmMF=I->First->Sled;
  wcscpy(NameX,I->First->R&&I->First->R->Nam?I->First->R->Nam:L"x");
  wcscpy(NameY,I->First->R&&I->First->R->Sled&&I->First->R->Sled->Nam?
   I->First->R->Sled->Nam:L"y");
  for(I=L->Inst;I;I=I->Sled){
   if((wcsstr(I->Name,L"ФОН")==I->Name)&&(!wcscmp(I->First->Nam,NameMult))) break;
 }}
 else{
  for(I=L->Inst;I;I=I->Sled)
   if(wcsstr(I->Name,L"АНИМАЦИЯ")==I->Name){
    Irez=I;
    ANIMATION3D=!wcscmp(I->Name,L"АНИМАЦИЯ3D");
    break;
   }
  if(!Irez) return; I=Irez;
  wcscpy(NameX,I->First->R&&I->First->R->Nam?I->First->R->Nam:L"x");
  wcscpy(NameY,I->First->R&&I->First->R->Sled&&I->First->R->Sled->Nam?
   I->First->R->Sled->Nam:L"y");
  ParmMF=I->First->Sled;
  for(I=L->Inst;I;I=I->Sled) if(wcsstr(I->Name,L"ФОН")==I->Name) break;
 }
 ParmF=I?I->First->Sled:NULL;
 CppWebBrowser->Align=alNone; CppWebBrowser->Width=0; CppWebBrowser->Height=0;
 Image->Visible=true;
 for(Parm *P=ParmF;P;P=P->Sled){
  if(!wcscmp(P->Nam,L"МАНИПУЛЯТОР3D")){
   int K; Parm *p=P->R; wchar_t NameX[16],NameY[16],Bx,By,Buf[16];
   for(int i=0;i<4;i++,p=p->Sled)
    if(p->Nam)
     TakeMagazine(&NamesVarys,p->Nam);
   if(p->Nam){ Bx=p->Nam[0]; TakeMagazine(&NamesVarys,p->Nam); }
   p=p->Sled;
   if(p->Nam){ By=p->Nam[0]; TakeMagazine(&NamesVarys,p->Nam); }
   p=p->Sled;
   K=KolElem(p);
   for(int i=0;i<K;i++,p=p->Sled){
    swprintf(Buf,L"%cO@%s",Bx,p->Nam);
    TakeMagazine(&NamesVarys,Buf);
    swprintf(Buf,L"%cO@%s",By,p->Nam);
    TakeMagazine(&NamesVarys,Buf);
  }}
  else
   for(Parm *p=P->R;p;p=p->Sled)
    if(p->Nam)
     TakeMagazine(&NamesVarys,p->Nam);
 }
 for(Parm *P=ParmMF;P;P=P->Sled){
  if(!wcscmp(P->Nam,L"МАНИПУЛЯТОР3D")){
   int K; Parm *p=P->R; wchar_t Bx,By,Buf[16];
   for(int i=0;i<4;i++,p=p->Sled)
    if(p->Nam)
     TakeMagazine(&NamesVarys,p->Nam);
   if(p->Nam){ Bx=p->Nam[0]; TakeMagazine(&NamesVarys,p->Nam); }
   p=p->Sled;
   if(p->Nam){ By=p->Nam[0]; TakeMagazine(&NamesVarys,p->Nam); }
   p=p->Sled;
   K=KolElem(p);
   for(int i=0;i<K;i++,p=p->Sled){
    swprintf(Buf,L"%cO@%s",Bx,p->Nam);
    TakeMagazine(&NamesVarys,Buf);
    swprintf(Buf,L"%cO@%s",By,p->Nam);
    TakeMagazine(&NamesVarys,Buf);
  }}
  else
   for(Parm *p=P->R;p;p=p->Sled)
    if(p->Nam)
     TakeMagazine(&NamesVarys,p->Nam);
 }
 Vals=(double *)calloc(KolElem(NamesVarys),SzD);
 TB_Table->Enabled=false;  TB_ToBMP->Enabled=true;
 TB_Print->Enabled=false;  ToolButtonInclude->Enabled=true;
 if(tbFormula) tbFormula->Enabled=false; ToolButtonRead->Enabled=false;
 FileToOpt();
 Image->Canvas->Font->Name=NameFont;
 Image->Canvas->Font->Size=SizeFont;
 Image->Canvas->Font->Style=
  ExFont?Image->Canvas->Font->Style<<fsBold:Image->Canvas->Font->Style>>fsBold;
 Image->Canvas->Font->Style=
  ItFont?Image->Canvas->Font->Style<<fsItalic:Image->Canvas->Font->Style>>fsItalic;
 Image->Canvas->Font->Style=
  UnFont?Image->Canvas->Font->Style<<fsUnderline:Image->Canvas->Font->Style>>fsUnderline;
 Metrica(Image->Canvas);
 if(CGauge) CGauge->ShowText=true;
 DefMSTMultFilm(Image->Canvas);
 if(*IsNoCalc){
  swprintf(Inf,L"Не для всех значений времени %s\n"
   L"из требуемого инструкциями \"НАЧАЛЬНЫЕ УСЛОВИЯ\"\n"
   L"и \"КОНЕЧНЫЕ УСЛОВИЯ\" диапазона [%g,%g]\n"
   L"получено решение!",L->Time->Name,Tn,Tend);
  Application->MessageBox(Inf,L"Внимание!",MB_OK|MB_ICONERROR);
  return;
 }
 PenWidth=Image->Canvas->Pen->Width;
 FontSize=Image->Canvas->Font->Size;
 FontColor=Image->Canvas->Font->Color;
 BrushColor=Image->Canvas->Brush->Color;
 PenColor=Image->Canvas->Pen->Color;
 PrepMultFilm(Image);
 TimerMultFilm->Enabled=true; TimerMultFilm->Interval=Round(1000.0/(*Kkadr));
 if(CGauge){
  CGauge->MaxValue=(*Tmult)*(*Kkadr)/1000; CGauge->ForeColor=*ColorGraph;
  CGauge->Progress=1; CGauge->Visible=true;
 }
 StatusBar->SimpleText=L"Анимация";
}
//---------------------------------------------------------------------------
void DelMagazine(Magazine **R,Magazine *M){
 if(!*R) return;
 Magazine *m=M;
 if(M==*R) *R=M->Sled;
 else
  for(m=*R;m->Sled;m=m->Sled) if(m->Sled==M){ m->Sled=M->Sled; break; }
 if(M) free(M);
}
//---------------------------------------------------------------------------
void __fastcall TMultFilms::PointTest(Root *Tv,Parm *p,Magazine *m){
 int i; Parm *pp; Vary *V;
 if(KolElem(p->R)!=3){
  swprintf(Inf,L"Примитив \"%s\" в инструкции \"%s\" строки %d\n"
   L"должен содержать 2 переменные, задающие декартовые координаты\n"
   L"точки в единицах длины задачи и параметр, кодирующий цвет точки",
   p->Nam,Tv->Name,Tv->Line);
  Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
 }
 for(i=0,pp=p->R;i<3;i++,pp=pp->Sled){
  if(pp->Nam){
   if(!FindVary(pp->Nam,&V)){
    swprintf(Inf,L"Параметр \"%s\" примитива \"%s\" инструкции \"%s\" строки %d\n"
     L"не найден среди переменных задачи",pp->Nam,p->Nam,Tv->Name,Tv->Line);
    Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
  }}
  else if((i==2)&&(pp->Val<0)){
   swprintf(Inf,
    L"Третий параметр \"%g\" примитива \"%s\" инструкции \"%s\" строки %d\n"
    L"должен быть целым числом в диапазоне [-1,16777215]",
    pp->Val,p->Nam,Tv->Name,Tv->Line);
   Atten(Tv->Line,0); DelMagazine(&HeadMult,m);
}}}
//---------------------------------------------------------------------------
int FindTrajectory(Trajectory *Rout,wchar_t *S,Trajectory **M){
 if(!(*M=Rout)) return 0;
 if(wcscmp(S,Rout->Name)<0){ *M=NULL; return 0; }
 for(;;*M=(*M)->Sled){
  if(!wcscmp((*M)->Name,S)) return 1;
  if(!((*M)->Sled)) return 0;
  if(wcscmp((*M)->Sled->Name,S)>0) return 0;
}}
//---------------------------------------------------------------------------
Trajectory *TakeTrajectory(Trajectory **Rout,wchar_t *S,int K){
 Trajectory *M,*m; if(!S) return NULL;
 if(FindTrajectory(*Rout,S,&M)) return M;
 else{
  m=(Trajectory *)calloc(1,sizeof(Trajectory));
  m->Name=wcscpy((wchar_t *)malloc(SzC*(wcslen(S)+1)),S); m->Kt=K;
  m->X=(int *)calloc(K,SzI); m->Y=(int *)calloc(K,SzI);
  m->x=(double *)calloc(K,SzD); m->y=(double *)calloc(K,SzD);
  m->C=(TColor *)calloc(K,sizeof(TColor));
  if(!M){ m->Sled=*Rout;   return   *Rout=m; }
  else  { m->Sled=M->Sled; return M->Sled=m; }
}}
//---------------------------------------------------------------------------
void __fastcall TMultFilms::TraectorTest(Root *Tv,Parm *p,Magazine *m){
 int i,Kt; Parm *pp; Vary *V; wchar_t Name[256];
 if(KolElem(p->R)!=4){
  swprintf(Inf,L"Примитив \"%s\" в инструкции \"%s\" строки %d должен\n"
   L"содержать 2 переменные, задающие декартовые координаты точки в\n"
   L"единицах длины задачи и параметры, задающие толщину и цвет траектории",
   p->Nam,Tv->Name,Tv->Line);
  Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
 }
 for(i=0,pp=p->R;i<4;i++,pp=pp->Sled){
  if(pp->Nam){
   if(!FindVary(pp->Nam,&V)){
    swprintf(Inf,L"Параметр \"%s\" примитива \"%s\" инструкции \"%s\" строки %d\n"
     L"не найден среди переменных задачи",pp->Nam,p->Nam,Tv->Name,Tv->Line);
    Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
  }}
  else if((i==2)&&(pp->Val<0)){
   swprintf(Inf,
    L"Третий параметр \"%g\" примитива \"%s\" инструкции \"%s\" строки %d\n"
    L"должен быть целым числом",
    pp->Val,p->Nam,Tv->Name,Tv->Line);
   Atten(Tv->Line,0); DelMagazine(&HeadMult,m);
  }
  else if((i==3)&&(pp->Val<0)){
   swprintf(Inf,
    L"Четвертый параметр \"%g\" примитива \"%s\" инструкции \"%s\" строки %d\n"
    L"должен быть целым числом в диапазоне [-1,16777215]",
    pp->Val,p->Nam,Tv->Name,Tv->Line);
   Atten(Tv->Line,0); DelMagazine(&HeadMult,m);
 }}
 Kt=(*Tmult)*(*Kkadr)/1000+1;
 swprintf(Name,L"%s_%s",p->R->Nam,p->R->Sled->Nam);
 TakeTrajectory(&Rtraek,Name,Kt);
}
//---------------------------------------------------------------------------
void __fastcall TMultFilms::Traector3dTest(Root *Tv,Parm *p,Magazine *m){
 int i,Kt; Parm *pp; Vary *V; wchar_t Name[256];
 if(KolElem(p->R)!=5){
  swprintf(Inf,L"Примитив \"%s\" в инструкции \"%s\" строки %d должен\n"
   L"содержать 3 переменные, задающие декартовые координаты точки в\n"
   L"единицах длины задачи и параметры, задающие толщину и цвет траектории",
   p->Nam,Tv->Name,Tv->Line);
  Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
 }
 for(i=0,pp=p->R;i<5;i++,pp=pp->Sled){
  if(pp->Nam){
   if(!FindVary(pp->Nam,&V)){
    swprintf(Inf,L"Параметр \"%s\" примитива \"%s\" инструкции \"%s\" строки %d\n"
     L"не найден среди переменных задачи",pp->Nam,p->Nam,Tv->Name,Tv->Line);
    Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
  }}
  else if(i<3){
   swprintf(Inf,
    L"Первые три параметра примитива \"%s\"\n"
    L"инструкции \"%s\" строки %d\nдолжны быть именами переменных,\n"
    L"значения которых определяются временем\nи(или) обобщенными координатами",
    p->Nam,Tv->Name,Tv->Line);
   Atten(Tv->Line,0); DelMagazine(&HeadMult,m);
  }
  else if((i==3)&&(pp->Val<0)){
   swprintf(Inf,
    L"Четвертый параметр \"%g\" примитива \"%s\" инструкции \"%s\" строки %d\n"
    L"должен быть целым числом",
    pp->Val,p->Nam,Tv->Name,Tv->Line);
   Atten(Tv->Line,0); DelMagazine(&HeadMult,m);
  }
  else if((i==4)&&(pp->Val<0)){
   swprintf(Inf,
    L"Пятый параметр \"%g\" примитива \"%s\" инструкции \"%s\" строки %d\n"
    L"должен быть целым числом в диапазоне [-1,16777215]",
    pp->Val,p->Nam,Tv->Name,Tv->Line);
   Atten(Tv->Line,0); DelMagazine(&HeadMult,m);
 }}
 Kt=(*Tmult)*(*Kkadr)/1000+1;
 swprintf(Name,L"%s_%s_%s",p->R->Nam,p->R->Sled->Nam,p->R->Sled->Sled->Nam);
 TakeTrajectory(&Rtraek,Name,Kt);
}
//---------------------------------------------------------------------------
void __fastcall TMultFilms::CircleTest(Root *Tv,Parm *p,Magazine *m){
 int i; Parm *pp; Vary *V;
 if(KolElem(p->R)!=6){
  swprintf(Inf,L"Примитив \"%s\" инструкции \"%s\" строки %d должен\n"
   L"содержать 2 декартовые координаты центра, радиус и толщину контура,\n"
   L"2 кода цвета контура и внутренней области.\n"
   L"Координаты и радиус задаются в единицах длины задачи",
   p->Nam,Tv->Name,Tv->Line);
  Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
 }
 for(i=0,pp=p->R;i<6;i++,pp=pp->Sled){
  if(pp->Nam){
   if(!FindVary(pp->Nam,&V)){
    swprintf(Inf,L"Параметр \"%s\" примитива \"%s\" инструкции \"%s\" строки %d\n"
     L"не найден среди переменных задачи",pp->Nam,p->Nam,Tv->Name,Tv->Line);
    Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
  }}
  else{
   switch(i){
    case 2: case 3:
     if(pp->Val<0){
      swprintf(Inf,L"Параметр \"%g\" примитива \"%s\" инструкции \"%s\" строки %d\n"
       L"должен быть положительным числом",pp->Val,p->Nam,Tv->Name,Tv->Line);
      Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
     }
     break;
    case 4:
     if(pp->Val<0||pp->Val>16777215){
      swprintf(Inf,L"Цвет контура, заданный значением \"%g\" в примитиве \"%s\"\n"
       L"инструкции \"%s\" строки %d\n"
       L"должен быть числом в диапазоне [0,16777215]",
       pp->Val,p->Nam,Tv->Name,Tv->Line);
      Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
     }
     break;
    case 5:
     if(pp->Val<-1||pp->Val>16777215){
      swprintf(Inf,L"Цвет заполнения, заданный значением \"%g\" в примитиве \"%s\"\n"
       L"инструкции \"%s\" строки %d\n"
       L"должен быть числом в диапазоне [-1,16777215],\n"
       L"причем -1 делает прозрачной внутреннюю область",
       pp->Val,p->Nam,Tv->Name,Tv->Line);
      Atten(Tv->Line,0); DelMagazine(&HeadMult,m);
}}}}}
//---------------------------------------------------------------------------
void __fastcall TMultFilms::Circle3dTest(Root *Tv,Parm *p,Magazine *m){
 int i; Parm *pp; Vary *V;
 if(KolElem(p->R)!=10){
  swprintf(Inf,L"Примитив \"%s\" инструкции \"%s\" строки %d должен\n"
   L"содержать 3 декартовые координаты центра, 3 проекции\n"
   L"вектора-нормали к плоскости окружности, ее радиус и толщину\n"
   L"контура, 2 кода цвета - контура и внутренней области.\n"
   L"Координаты и радиус задаются в единицах длины задачи",
   p->Nam,Tv->Name,Tv->Line);
  Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
 }
 for(i=0,pp=p->R;i<10;i++,pp=pp->Sled){
  if(pp->Nam){
   if(!FindVary(pp->Nam,&V)){
    swprintf(Inf,L"Параметр \"%s\" примитива \"%s\" инструкции \"%s\" строки %d\n"
     L"не найден среди переменных задачи",pp->Nam,p->Nam,Tv->Name,Tv->Line);
    Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
  }}
  else{
   switch(i){
    case 6: case 7:
     if(pp->Val<0){
      swprintf(Inf,L"Параметр \"%g\" примитива \"%s\" инструкции \"%s\" строки %d\n"
       L"должен быть положительным числом",pp->Val,p->Nam,Tv->Name,Tv->Line);
      Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
     }
     break;
    case 8:
     if(pp->Val<0||pp->Val>16777215){
      swprintf(Inf,L"Цвет контура, заданный значением \"%g\" в примитиве \"%s\"\n"
       L"инструкции \"%s\" строки %d\n"
       L"должен быть числом в диапазоне [0,16777215]",
       pp->Val,p->Nam,Tv->Name,Tv->Line);
      Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
     }
     break;
    case 9:
     if(pp->Val<-1||pp->Val>16777215){
      swprintf(Inf,L"Цвет заполнения, заданный значением \"%g\" в примитиве \"%s\"\n"
       L"инструкции \"%s\" строки %d\n"
       L"должен быть числом в диапазоне [-1,16777215],\n"
       L"причем -1 делает прозрачной внутреннюю область",
       pp->Val,p->Nam,Tv->Name,Tv->Line);
      Atten(Tv->Line,0); DelMagazine(&HeadMult,m);
}}}}}
//---------------------------------------------------------------------------
void __fastcall TMultFilms::KolesoTest(Root *Tv,Parm *p,Magazine *m){
 int i; Parm *pp; Vary *V;
 if(KolElem(p->R)!=7){
  swprintf(Inf,L"Примитив \"%s\" инструкции \"%s\" строки %d должен\n"
   L"содержать 2 декартовые координаты центра и радиус колеса,\n"
   L"угол в радианах его поворота при вращении, толщину контура\n"
   L"и спиц, 2 кода цвета - контура, спиц и внутренней области.\n"
   L"Координаты и радиус задаются в единицах длины задачи",
   p->Nam,Tv->Name,Tv->Line);
  Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
 }
 for(i=0,pp=p->R;i<7;i++,pp=pp->Sled){
  if(pp->Nam){
   if(!FindVary(pp->Nam,&V)){
    swprintf(Inf,L"Параметр \"%s\" примитива \"%s\" инструкции \"%s\" строки %d\n"
     L"не найден среди переменных задачи",pp->Nam,p->Nam,Tv->Name,Tv->Line);
    Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
  }}
  else{
   switch(i){
    case 2: case 4:
     if(pp->Val<0){
      swprintf(Inf,L"Параметр \"%g\" примитива \"%s\" инструкции \"%s\" строки %d\n"
       L"должен быть положительным числом",pp->Val,p->Nam,Tv->Name,Tv->Line);
      Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
     }
     break;
    case 5:
     if(pp->Val<0||pp->Val>16777215){
      swprintf(Inf,L"Цвет контура, заданный значением \"%g\" в примитиве \"%s\"\n"
       L"инструкции \"%s\" строки %d\n"
       L"должен быть числом в диапазоне [0,16777215]",
       pp->Val,p->Nam,Tv->Name,Tv->Line);
      Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
     }
     break;
    case 6:
     if(pp->Val<-1||pp->Val>16777215){
      swprintf(Inf,L"Цвет заполнения, заданный значением \"%g\" в примитиве \"%s\"\n"
       L"инструкции \"%s\" строки %d\n"
       L"должен быть числом в диапазоне [-1,16777215],\n"
       L"причем -1 делает прозрачной внутреннюю область",
       pp->Val,p->Nam,Tv->Name,Tv->Line);
      Atten(Tv->Line,0); DelMagazine(&HeadMult,m);
}}}}}
//---------------------------------------------------------------------------
void __fastcall TMultFilms::VorotTest(Root *Tv,Parm *p,Magazine *m){
 int i; Parm *pp; Vary *V;
 if(KolElem(p->R)!=8){
  swprintf(Inf,L"Примитив \"%s\" инструкции \"%s\" строки %d должен\n"
   L"содержать 2 декартовые координаты центра и радиус ворота,\n"
   L"длину нити и угол дуги от верхней точки ворота до точки схода нити,\n"
   L"толщину контура и спиц, 2 кода цвета - контура (и спиц),\n"
   L"внутренней области. Координаты, радиус и длина задаются\n"
   L"в единицах длины задачи, угол - в градусах против часовой стрелки",
   p->Nam,Tv->Name,Tv->Line);
  Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
 }
 for(i=0,pp=p->R;i<8;i++,pp=pp->Sled){
  if(pp->Nam){
   if(!FindVary(pp->Nam,&V)){
    swprintf(Inf,L"Параметр \"%s\" примитива \"%s\" инструкции \"%s\" строки %d\n"
     L"не найден среди переменных задачи",pp->Nam,p->Nam,Tv->Name,Tv->Line);
    Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
  }}
  else{
   switch(i){
    case 2: case 3: case 5:
     if(pp->Val<0){
      swprintf(Inf,L"Параметр \"%g\" примитива \"%s\" инструкции \"%s\" строки %d\n"
       L"должен быть положительным числом",pp->Val,p->Nam,Tv->Name,Tv->Line);
      Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
     }
     break;
    case 6:
     if(pp->Val<0||pp->Val>16777215){
      swprintf(Inf,L"Цвет контура, заданный значением \"%g\" в примитиве \"%s\"\n"
       L"инструкции \"%s\" строки %d\n"
       L"должен быть числом в диапазоне [0,16777215]",
       pp->Val,p->Nam,Tv->Name,Tv->Line);
      Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
     }
     break;
    case 7:
     if(pp->Val<-1||pp->Val>16777215){
      swprintf(Inf,L"Цвет заполнения, заданный значением \"%g\" в примитиве \"%s\"\n"
       L"инструкции \"%s\" строки %d\n"
       L"должен быть числом в диапазоне [-1,16777215],\n"
       L"причем -1 делает прозрачной внутреннюю область",
       pp->Val,p->Nam,Tv->Name,Tv->Line);
      Atten(Tv->Line,0); DelMagazine(&HeadMult,m);
}}}}}
//---------------------------------------------------------------------------
void __fastcall TMultFilms::GateWithLoadTest(Root *Tv,Parm *p,Magazine *m){
 int i; Parm *pp; Vary *V;
 if(KolElem(p->R)!=12){
  swprintf(Inf,L"Примитив \"%s\" инструкции \"%s\" строки %d должен содержать:\n"
   L"2 декартовые координаты центра, радиус и угол поворота ворота,\n"
   L"угол дуги от верхней точки ворота до точки схода нити,\n"
   L"начальную длину троса, высоту и длину прямоугольника груза,\n"
   L"толщину контура и спиц, 2 кода цвета - контура (и спиц),\n"
   L"внутренней области. Координаты, радиус и длина задаются\n"
   L"в единицах длины задачи, угол - в градусах против часовой стрелки",
   p->Nam,Tv->Name,Tv->Line);
  Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
 }
 for(i=0,pp=p->R;i<12;i++,pp=pp->Sled){
  if(pp->Nam){
   if(!FindVary(pp->Nam,&V)){
    swprintf(Inf,L"Параметр \"%s\" примитива \"%s\" инструкции \"%s\" строки %d\n"
     L"не найден среди переменных задачи",pp->Nam,p->Nam,Tv->Name,Tv->Line);
    Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
  }}
  else{
   switch(i){
    case 2: case 5: case 6: case 7:
     if(pp->Val<0){
      swprintf(Inf,L"Параметр \"%g\" примитива \"%s\" инструкции \"%s\" строки %d\n"
       L"должен быть положительным числом",pp->Val,p->Nam,Tv->Name,Tv->Line);
      Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
     }
     break;
    case 9:
     if(pp->Val<0||pp->Val>16777215){
      swprintf(Inf,L"Цвет контура, заданный значением \"%g\" в примитиве \"%s\"\n"
       L"инструкции \"%s\" строки %d\n"
       L"должен быть числом в диапазоне [0,16777215]",
       pp->Val,p->Nam,Tv->Name,Tv->Line);
      Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
     }
     break;
    case 10: case 11:
     if(pp->Val<-1||pp->Val>16777215){
      swprintf(Inf,L"Цвет заполнения, заданный значением \"%g\" в примитиве \"%s\"\n"
       L"инструкции \"%s\" строки %d\n"
       L"должен быть числом в диапазоне [-1,16777215],\n"
       L"причем -1 делает прозрачной внутреннюю область",
       pp->Val,p->Nam,Tv->Name,Tv->Line);
      Atten(Tv->Line,0); DelMagazine(&HeadMult,m);
}}}}}
//---------------------------------------------------------------------------
void __fastcall TMultFilms::GearTest(Root *Tv,Parm *p,Magazine *m){
 int i; Parm *pp; Vary *V;
 if(KolElem(p->R)!=11){
  swprintf(Inf,L"Примитив \"%s\" инструкции \"%s\" строки %d должен\n"
   L"содержать 2 декартовые координаты центра и радиус колес,\n"
   L"флаг перехлеста ремней (=1, когда перехлестываются),\n"
   L"угол поворота первого колеса (второго рассчитывается через радиусы),\n"
   L"толщину контура и спиц, 2 кода цвета - контура (и спиц),\n"
   L"внутренней области. Координаты и радиусы задаются\n"
   L"в единицах длины задачи, угол - в радианах",p->Nam,Tv->Name,Tv->Line);
  Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
 }
 for(i=0,pp=p->R;i<8;i++,pp=pp->Sled){
  if(pp->Nam){
   if(!FindVary(pp->Nam,&V)){
    swprintf(Inf,L"Параметр \"%s\" примитива \"%s\" инструкции \"%s\" строки %d\n"
     L"не найден среди переменных задачи",pp->Nam,p->Nam,Tv->Name,Tv->Line);
    Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
  }}
  else{
   switch(i){
    case 2: case 5: case 6: case 8:
     if(pp->Val<0){
      swprintf(Inf,L"Параметр \"%g\" примитива \"%s\" инструкции \"%s\" строки %d\n"
       L"должен быть положительным числом",pp->Val,p->Nam,Tv->Name,Tv->Line);
      Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
     }
     break;
    case 9:
     if(pp->Val<0||pp->Val>16777215){
      swprintf(Inf,L"Цвет контура, заданный значением \"%g\" в примитиве \"%s\"\n"
       L"инструкции \"%s\" строки %d\n"
       L"должен быть числом в диапазоне [0,16777215]",
       pp->Val,p->Nam,Tv->Name,Tv->Line);
      Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
     }
     break;
    case 10:
     if(pp->Val<-1||pp->Val>16777215){
      swprintf(Inf,L"Цвет заполнения, заданный значением \"%g\" в примитиве \"%s\"\n"
       L"инструкции \"%s\" строки %d\n"
       L"должен быть числом в диапазоне [-1,16777215],\n"
       L"причем -1 делает прозрачной внутреннюю область",
       pp->Val,p->Nam,Tv->Name,Tv->Line);
      Atten(Tv->Line,0); DelMagazine(&HeadMult,m);
}}}}}
//---------------------------------------------------------------------------
void __fastcall TMultFilms::EllipsTest(Root *Tv,Parm *p,Magazine *m){
 int i; Parm *pp; Vary *V;
 if(KolElem(p->R)!=7){
  swprintf(Inf,L"Примитив \"%s\" инструкции \"%s\" строки %d должен\n"
   L"содержать 2 декартовые координаты центра, 2 полуоси - горизонтальную и\n"
   L"вертикальную, толщину контура, 2 кода цвета контура и внутренней области.\n"
   L"Координаты и полуоси задаются в единицах длины задачи",
   p->Nam,Tv->Name,Tv->Line);
  Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
 }
 for(i=0,pp=p->R;i<7;i++,pp=pp->Sled){
  if(pp->Nam){
   if(!FindVary(pp->Nam,&V)){
    swprintf(Inf,L"Параметр \"%s\" примитива \"%s\" инструкции \"%s\" строки %d\n"
     L"не найден среди переменных задачи",pp->Nam,p->Nam,Tv->Name,Tv->Line);
    Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
  }}
  else{
   switch(i){
    case 2: case 3: case 4:
     if(pp->Val<0){
      swprintf(Inf,L"Параметр \"%g\" примитива \"%s\" инструкции \"%s\" строки %d\n"
       L"должен быть положительным числом",pp->Val,p->Nam,Tv->Name,Tv->Line);
      Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
     }
     break;
    case 5:
     if(pp->Val<0||pp->Val>16777215){
      swprintf(Inf,L"Цвет контура, заданный значением \"%g\" в примитиве \"%s\"\n"
       L"инструкции \"%s\" строки %d\n"
       L"должен быть числом в диапазоне [0,16777215]",
       pp->Val,p->Nam,Tv->Name,Tv->Line);
      Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
     }
     break;
    case 6:
     if(pp->Val<-1||pp->Val>16777215){
      swprintf(Inf,L"Цвет заполнения, заданный значением \"%g\" в примитиве \"%s\"\n"
       L"инструкции \"%s\" строки %d\n"
       L"должен быть числом в диапазоне [-1,16777215],\n"
       L"причем -1 делает прозрачной внутреннюю область",
       pp->Val,p->Nam,Tv->Name,Tv->Line);
      Atten(Tv->Line,0); DelMagazine(&HeadMult,m);
}}}}}
//---------------------------------------------------------------------------
void __fastcall TMultFilms::LineTest(Root *Tv,Parm *p,Magazine *m){
 int i; Parm *pp; Vary *V;
 if(KolElem(p->R)!=6){
  swprintf(Inf,L"Примитив \"%s\" инструкции \"%s\" строки %d\n"
   L"должен содержать 4 декартовые координаты начала и\n"
   L"конца, толщину в пикселях и код цвета\n"
   L"Координаты задаются в единицах длины задачи",p->Nam,Tv->Name,Tv->Line);
  Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
 }
 for(i=0,pp=p->R;i<6;i++,pp=pp->Sled){
  if(pp->Nam){
   if(!FindVary(pp->Nam,&V)){
    swprintf(Inf,L"Параметр \"%s\" примитива \"%s\" инструкции \"%s\" строки %d\n"
     L"не найден среди переменных задачи",pp->Nam,p->Nam,Tv->Name,Tv->Line);
    Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
  }}
  else{
   switch(i){
    case 4:
     if(pp->Val<0){
      swprintf(Inf,L"Параметр \"%g\" примитива \"%s\" инструкции \"%s\" строки %d\n"
       L"должен быть положительным числом",pp->Val,p->Nam,Tv->Name,Tv->Line);
      Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
     }
     break;
    case 5:
     if(pp->Val<0||pp->Val>16777215){
      swprintf(Inf,L"Цвет прямой, заданный значением \"%g\" в примитиве \"%s\"\n"
       L"инструкции \"%s\" строки %d\n"
       L"должен быть числом в диапазоне [0,16777215]",
       pp->Val,p->Nam,Tv->Name,Tv->Line);
      Atten(Tv->Line,0); DelMagazine(&HeadMult,m);
}}}}}
//---------------------------------------------------------------------------
void __fastcall TMultFilms::FingerTest(Root *Tv,Parm *p,Magazine *m){
 int i; Parm *pp; Vary *V;
 if(KolElem(p->R)!=7){
  swprintf(Inf,L"Примитив \"%s\" инструкции \"%s\" строки %d\n"
   L"должен содержать 2 декартовые координаты начала и\n"
   L"2 декартовые проекции вектора, масштаб, толщину в пикселях и код цвета\n"
   L"Координаты и проекции задаются в единицах длины задачи,\n"
   L"масштаб задается числом, на которое следует умножить длину вектора,L"
   L"чтобы получить длину изображаемой стрелки",p->Nam,Tv->Name,Tv->Line);
  Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
 }
 for(i=0,pp=p->R;i<7;i++,pp=pp->Sled){
  if(pp->Nam){
   if(!FindVary(pp->Nam,&V)){
    swprintf(Inf,L"Параметр \"%s\" примитива \"%s\" инструкции \"%s\" строки %d\n"
     L"не найден среди переменных задачи",pp->Nam,p->Nam,Tv->Name,Tv->Line);
    Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
  }}
  else{
   switch(i){
    case 4: case 5:
     if(pp->Val<0){
      swprintf(Inf,L"Параметр \"%g\" примитива \"%s\" инструкции \"%s\" строки %d\n"
       L"должен быть положительным числом",pp->Val,p->Nam,Tv->Name,Tv->Line);
      Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
     }
     break;
    case 6:
     if(pp->Val<0||pp->Val>16777215){
      swprintf(Inf,L"Цвет стрелки, заданный значением \"%g\" в примитиве \"%s\"\n"
       L"инструкции \"%s\" строки %dдолжен быть числом в диапазоне [0,16777215]",
       pp->Val,p->Nam,Tv->Name,Tv->Line);
      Atten(Tv->Line,0); DelMagazine(&HeadMult,m);
}}}}}
//---------------------------------------------------------------------------
void __fastcall TMultFilms::KinParTest(Root *Tv,Parm *p,Magazine *m,int That){
 int i; Parm *pp; Vary *V; wchar_t NameV[256];
 if(KolElem(p->R)!=5){
  swprintf(Inf,L"Примитив \"%s\" инструкции \"%s\" строки %d\n"
   L"должен содержать 2 переменные, формулы которых задают\n"
   L"декартовые координаты точки, %s которой отображается,\n"
   L"масштаб вектора %s, его толщину в пикселях и код цвета.\n"
   L"Координаты задаются в единицах длины задачи,\n"
   L"масштаб задается числом, на которое следует умножить модуль %s,L"
   L"чтобы получить длину изображаемого вектора %s",
   p->Nam,Tv->Name,Tv->Line,
   That>2?L"нормальное ускорение":That>1?L"касательное ускорение":L"скорость",
   That>2?L"нормального ускорения":That>1?L"касательного ускорения":L"скорости",
   That>2?L"нормального ускорения":That>1?L"касательного ускорения":L"скорости",
   That>2?L"нормального ускорения":That>1?L"касательного ускорения":L"скорости");
  Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
 }
 for(i=0,pp=p->R;i<2;i++,pp=pp->Sled){
  if(pp->Nam){
   if(FindVary(pp->Nam,&V)){ Vary *VV; Form F;
    swprintf(NameV,L"%s'%s",V->Name,L->Time->Name);
    if(!FindVary(NameV,&VV))
     F=DifByT(V);
    if(That>1){
     swprintf(NameV,L"%s'%s",VV->Name,L->Time->Name);
     if(!FindVary(NameV,&V))
      F=DifByT(VV);
   }}
   else{
    swprintf(Inf,L"Параметр \"%s\" примитива \"%s\" инструкции \"%s\" строки %d\n"
     L"не найден среди переменных задачи",pp->Nam,p->Nam,Tv->Name,Tv->Line);
    Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
  }}
  else{
   swprintf(Inf,
    L"%d-й параметр примитива \"%s\" инструкции \"%s\"\n"
    L" строки %d должен быть переменной, задающей %sальную\n"
    L"координату точки, %s которой отображается",
    i+1,p->Nam,Tv->Name,Tv->Line,i?L"вертик":L"горизонт",
    That>2?L"нормальное ускорение":That>1?L"касательное ускорение":L"скорость");
   Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
 }}
 for(;i<5;i++,pp=pp->Sled){
  if(pp->Nam){
   if(!FindVary(pp->Nam,&V)){
    swprintf(Inf,L"Параметр \"%s\" примитива \"%s\" инструкции \"%s\" строки %d\n"
     L"не найден среди переменных задачи",pp->Nam,p->Nam,Tv->Name,Tv->Line);
    Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
  }}
  else{
   switch(i){
    case 2:case 3:
     if(pp->Val<0){
      swprintf(Inf,L"Параметр \"%g\" примитива \"%s\" инструкции \"%s\" строки %d\n"
       L"должен быть положительным числом",pp->Val,p->Nam,Tv->Name,Tv->Line);
      Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
     }
     break;
    case 4:
     if(pp->Val<0||pp->Val>16777215){
      swprintf(Inf,L"Цвет вектора %s, заданный значением \"%g\"\n"
       L"в примитиве \"%s\" инструкции \"%s\" строки %d\n"
       L"должен быть числом в диапазоне [0,16777215]",
       That>2?L"нормального ускорения":That>1?L"касательного ускорения":L"скорости",
       pp->Val,p->Nam,Tv->Name,Tv->Line);
      Atten(Tv->Line,0); DelMagazine(&HeadMult,m);
}}}}}
//---------------------------------------------------------------------------
void __fastcall TMultFilms::SpeedTest(Root *Tv,Parm *p,Magazine *m){
 KinParTest(Tv,p,m,1);
}
//---------------------------------------------------------------------------
void __fastcall TMultFilms::AtangTest(Root *Tv,Parm *p,Magazine *m){
 KinParTest(Tv,p,m,2);
}
//---------------------------------------------------------------------------
void __fastcall TMultFilms::AnormTest(Root *Tv,Parm *p,Magazine *m){
 KinParTest(Tv,p,m,3);
}
//---------------------------------------------------------------------------
void __fastcall TMultFilms::StringTest(Root *Tv,Parm *p,Magazine *m){
 int i; Parm *pp; Vary *V;
 if(KolElem(p->R)!=8){
  swprintf(Inf,L"Примитив \"%s\" инструкции \"%s\" строки %d\n"
   L"должен содержать 2 декартовые координаты начала, длину,\n"
   L"угол наклона к горизонту в градусах, радиус концов, толщину\n"
   L"в пикселях и коды цвета линии и заполнения концов стержня\n"
   L"Координаты, длина и радиус задаются в единицах длины задачи",
   p->Nam,Tv->Name,Tv->Line);
  Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
 }
 for(i=0,pp=p->R;i<8;i++,pp=pp->Sled){
  if(pp->Nam){
   if(!FindVary(pp->Nam,&V)){
    swprintf(Inf,
     L"Параметр \"%s\" примитива \"%s\" не найден среди переменных задачи",
      pp->Nam,p->Nam);
//    Application->MessageBox(Inf,L"Внимание!",MB_OK|MB_ICONWARNING);
    Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
  }}
  else{
   switch(i){
    case 2: case 4: case 5:
     if(pp->Val<0){
      swprintf(Inf,L"Параметр \"%g\" примитива \"%s\" инструкции \"%s\" строки %d\n"
       L"должен быть положительным числом",
       pp->Val,p->Nam,Tv->Name,Tv->Line);
      Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
     }
     break;
    case 3:
     if((pp->Val<-360)||(pp->Val>360)){
      swprintf(Inf,L"Угол наклона \"%g\" примитива \"%s\" инструкции \"%s\" строки %d\n"
       L"должен быть числом в диапазоне [-360,360]",
       pp->Val,p->Nam,Tv->Name,Tv->Line);
      Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
     }
     break;
    case 6:
     if(pp->Val<0||pp->Val>16777215){
      swprintf(Inf,L"Цвет линии стержня, заданный значением \"%g\"\n"
       L"в примитиве \"%s\" инструкции \"%s\" строки %d\n"
       L"должен быть числом в диапазоне [0,16777215]",
       pp->Val,p->Nam,Tv->Name,Tv->Line);
      Atten(Tv->Line,0); DelMagazine(&HeadMult,m);
}}}}}
//---------------------------------------------------------------------------
void __fastcall TMultFilms::MultiLinkTest(Root *Tv,Parm *p,Magazine *m){
 int i,K; Parm *pp=p->R; Vary *V;
 for(i=0;i<5;i++,pp=pp->Sled){
  if(pp->Nam){
   if(!FindVary(pp->Nam,&V)){
    swprintf(Inf,L"Параметр \"%s\" примитива \"%s\" не найден среди переменных задачи",
      pp->Nam,p->Nam);
//    Application->MessageBox(Inf,L"Внимание!",MB_OK|MB_ICONWARNING);
    Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
   }
   K=Round(V->Val);
  }
  else{
   if(pp->Val<0){
    swprintf(Inf,L"Параметр \"%g\" примитива \"%s\" инструкции \"%s\" строки %d\n"
     L"должен быть положительным числом",pp->Val,p->Nam,Tv->Name,Tv->Line);
    Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
   }
   K=Round(pp->Val);
 }}
 if(KolElem(p->R)!=(5+2*(K+1))){
  swprintf(Inf,L"Примитив \"%s\" инструкции \"%s\" строки %d\n"
   L"должен содержать радиус шарниров на концах, толщину стержней в пикселах,\n"
   L"2 кода цвета линий и внутренности шарниров на концах,\n"
   L"число стержней многозвенника и список пар декартовых координат их узлов.\n"
   L"Координаты и радиус задаются в единицах длины задачи",
   p->Nam,Tv->Name,Tv->Line);
  Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
 }
 for(i=0,pp=p->R;i<(5+2*(K+1));i++,pp=pp->Sled){
  if(pp->Nam){
   if(!FindVary(pp->Nam,&V)){
    swprintf(Inf,
     L"Параметр \"%s\" примитива \"%s\" не найден среди переменных задачи",
      pp->Nam,p->Nam);
//    Application->MessageBox(Inf,L"Внимание!",MB_OK|MB_ICONWARNING);
    Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
  }}
  else{
   switch(i){
    case 0: case 1:
     if(pp->Val<0){
      swprintf(Inf,L"Параметр \"%g\" примитива \"%s\" инструкции \"%s\" строки %d\n"
       L"должен быть положительным числом",
       pp->Val,p->Nam,Tv->Name,Tv->Line);
      Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
     }
     break;
     case 2: case 3:
     if(pp->Val<0||pp->Val>16777215){
      swprintf(Inf,L"Цвет линии стержня, заданный значением \"%g\"\n"
       L"в примитиве \"%s\" инструкции \"%s\" строки %d\n"
       L"должен быть числом в диапазоне [0,16777215]",
       pp->Val,p->Nam,Tv->Name,Tv->Line);
      Atten(Tv->Line,0); DelMagazine(&HeadMult,m);
}}}}}
//---------------------------------------------------------------------------
void __fastcall TMultFilms::Polygon3dTest(Root *Tv,Parm *p,Magazine *m){
 int i,K; Parm *pp=p->R; Vary *V; double D;
 for(i=0;i<5;i++,pp=pp->Sled){
  if(pp->Nam){
   if(!FindVary(pp->Nam,&V)){
    swprintf(Inf,L"Параметр \"%s\" примитива \"%s\" не найден среди переменных задачи",
      pp->Nam,p->Nam);
    Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
   }
   D=Val(V->Znach);
  }
  else{
   if(pp->F.C&&(pp->F.C!=Pust))
    D=Val(pp->F);
   else D=pp->Val;
  }
  if(i==3) K=Round(D);
  if((D<0)&&(i<2||i==3)){
   swprintf(Inf,L"Параметр \"%g\" примитива \"%s\" инструкции \"%s\" строки %d\n"
    L"должен быть положительным числом",pp->Val,p->Nam,Tv->Name,Tv->Line);
   Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
 }}
 if(KolElem(p->R)!=(5+2*K)){
  swprintf(Inf,
   L"Примитив \"%s\" инструкции \"%s\" строки %d\n"
   L"должен содержать толщину линий в пикселах, 2 кода\n"
   L"цвета линий и внутренности полигона, число сторон\n"
   L"и аппликату центра (расстояние от экрана до центра)\n"
   L"его и список экранных декартовых координат вершин.\n"
   L"Координаты задаются в единицах длины задачи",
   p->Nam,Tv->Name,Tv->Line);
  Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
 }
 for(i=0,pp=p->R;i<(5+2*K);i++,pp=pp->Sled){
  if(pp->Nam){
   if(!FindVary(pp->Nam,&V)){
    swprintf(Inf,
     L"Параметр \"%s\" примитива \"%s\" не найден среди переменных задачи",
      pp->Nam,p->Nam);
    Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
   }
   else D=Val(V->Znach);
  }
  else{
   if(pp->F.C&&(pp->F.C!=Pust)) D=Val(pp->F);
   else D=pp->Val;
   if(!i){
    if(D<0){
     swprintf(Inf,L"Толщина линий \"%g\" примитива \"%s\" инструкции \"%s\"\n"
      L"строки %d должна быть положительным числом",
      D,p->Nam,Tv->Name,Tv->Line);
     Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
   }}
   else if(i<2&&((D<0)||(D>16777215))){
    swprintf(Inf,L"Цвет ребра, заданный значением \"%g\"\n"
     L"в примитиве \"%s\" инструкции \"%s\" строки %d\n"
     L"должен быть числом в диапазоне [0,16777215]",
     D,p->Nam,Tv->Name,Tv->Line);
    Atten(Tv->Line,0); DelMagazine(&HeadMult,m);
    break;
}}}}
//---------------------------------------------------------------------------
void __fastcall TMultFilms::SplineTest(Root *Tv,Parm *p,Magazine *m){
//name, x0, xk,w, c, n, x1, y1,...,xn, yn
 int i,K; Parm *pp=p->R; Vary *V; wchar_t *Name=pp->Nam; Arra *Arr;
 if(!pp->Nam){
  swprintf(Inf,L"Описание примитива \"%s\" должно начинаться с имени",p->Nam);
  Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
 }
 for(i=0,pp=pp->Sled;i<4;i++,pp=pp->Sled){
  if(pp->Nam){
   if(!FindVary(pp->Nam,&V)){
    swprintf(Inf,L"Параметр \"%s\" сплайна \"%s\" не найден среди переменных задачи",
     Name,p->Nam);
    Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
  }}
  else{
   switch(i){
    case 2:
     if(pp->Val<0){
      swprintf(Inf,L"Параметр \"%g\" сплайна \"%s\" инструкции \"%s\" строки %d\n"
       L"должен быть положительным числом",pp->Val,Name,Tv->Name,Tv->Line);
      Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
     }
     break;
    case 3:
     if(pp->Val<0||pp->Val>16777215){
      swprintf(Inf,L"Цвет линии сплайна \"%s\", заданный значением \"%g\"\n"
       L"в инструкции \"%s\" строки %d\n"
       L"должен быть числом в диапазоне [0,16777215]",
       Name,pp->Val,Tv->Name,Tv->Line);
      Atten(Tv->Line,0); DelMagazine(&HeadMult,m);
 }}}}
 if(pp->Nam){
  if(!FindVary(pp->Nam,&V)){
   swprintf(Inf,L"Параметр \"%s\" сплайна \"%s\" не найден среди переменных задачи",
     pp->Nam,Name);
   Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
  }
  K=Round(V->Val);
 }
 else{
  if(pp->Val<0){
   swprintf(Inf,L"Параметр \"%g\" сплайна \"%s\" инструкции \"%s\" строки %d\n"
    L"должен быть положительным числом",pp->Val,Name,Tv->Name,Tv->Line);
   Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
  }
  K=Round(pp->Val);
 }
 if(KolElem(p->R)!=(6+2*K)){
  swprintf(Inf,L"Примитив \"%s\" (\"%s\") инструкции \"%s\" строки %d\n"
   L"должен содержать наименоание, 2 абсциссы - границы для рисования сплайна,\n"
   L"толщину в пикселах, код цвета линии, число точек сплайна и\n"
   L"список пар декартовых координат узлов.\n"
   L"Координаты задаются в единицах длины задачи",p->Nam,Name,Tv->Name,Tv->Line);
  Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
 }
 for(i=0,pp=pp->Sled;i<2*K;i++,pp=pp->Sled){
  if(pp->Nam){
   if(!FindVary(pp->Nam,&V)){
    swprintf(Inf,
     L"Параметр \"%s\" примитива \"%s\" не найден среди переменных задачи",
      pp->Nam,p->Nam);
    Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
 }}}
 Arr=(Arra *)calloc(1,sizeof(Arra));
 Arr->N=2*K; Arr->A=(double *)calloc(Arr->N,SzD);
 TakeSpln(&Rsp,Name,Arr);
}
//---------------------------------------------------------------------------
void __fastcall TMultFilms::HingeTest(Root *Tv,Parm *p,Magazine *m){
 int i; Parm *pp; Vary *V;
 if(KolElem(p->R)!=9){
   swprintf(Inf,L"   Примитив \"%s\"\n"
   L"инструкции \"%s\" строки %d\n"
   L"должен содержать:\n"
   L"2 декартовые координаты центра, его радиус,\n"
   L"высоту, угол наклона оси к горизонту в градусах,\n"
   L"флаг рисования штриховки, толщину в пикселях\n"
   L"и коды цвета линии и заполнения центра шарнира.\n"
   L"   Координаты, высота и радиус задаются\n"
   L"в единицах длины задачи",
   p->Nam,Tv->Name,Tv->Line);
  Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
 }
 for(i=0,pp=p->R;i<9;i++,pp=pp->Sled){
  if(pp->Nam){
   if(!FindVary(pp->Nam,&V)){
    swprintf(Inf,L"Параметр \"%s\" примитива \"%s\" инструкции \"%s\" строки %d\n"
     L"не найден среди переменных задачи",pp->Nam,p->Nam,Tv->Name,Tv->Line);
    Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
  }}
  else{
   switch(i){
    case 2: case 3:
     if(pp->Val<0){
      swprintf(Inf,L"Параметр \"%g\" примитива \"%s\" инструкции \"%s\" строки %d\n"
       L"должен быть положительным числом",
       pp->Val,p->Nam,Tv->Name,Tv->Line);
      Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
     }
     break;
    case 4:
     if((pp->Val<-360)||(pp->Val>360)){
      swprintf(Inf,L"Угол наклона \"%g\" примитива \"%s\" инструкции \"%s\" строки %d\n"
       L"должен быть числом в диапазоне [-360,360]",
       pp->Val,p->Nam,Tv->Name,Tv->Line);
      Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
     }
     break;
    case 5:
     if((pp->Val!=0)&&(pp->Val!=1)){
      swprintf(Inf,L"Флаг рисования штриховки примитива \"%s\"\n"
       L"инструкции \"%s\" строки %d должен быть равен 1 или 0!",
       p->Nam,Tv->Name,Tv->Line);
      Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
     }
     break;
    case 7:
     if(pp->Val<0||pp->Val>16777215){
      swprintf(Inf,L"Цвет линии шарнира, заданный значением \"%g\"\n"
       L"в примитиве \"%s\" инструкции \"%s\" строки %d\n"
       L"должен быть числом в диапазоне [0,16777215]",
       pp->Val,p->Nam,Tv->Name,Tv->Line);
      Atten(Tv->Line,0); DelMagazine(&HeadMult,m);
}}}}}
//---------------------------------------------------------------------------
void __fastcall TMultFilms::BarTest(Root *Tv,Parm *p,Magazine *m){
 int i; Parm *pp; Vary *V;
 if(KolElem(p->R)!=8){
  swprintf(Inf,L"Примитив \"%s\" инструкции \"%s\" строки %d\n"
   L"должен содержать 2 декартовые координаты центра,\n"
   L"угол поворота основания от горизонта (в радианах),\n"
   L"высоту и длину прямоугольника, толщину его контура,\n"
   L"коды цвета линии и заливки. Координаты центра,\n"
   L"высота и длина задаются в единицах длины задачи",p->Nam,Tv->Name,Tv->Line);
  Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
 }
 for(i=0,pp=p->R;i<8;i++,pp=pp->Sled){
  if(pp->Nam){
   if(!FindVary(pp->Nam,&V)){
    swprintf(Inf,L"Параметр \"%s\" примитива \"%s\" инструкции \"%s\" строки %d\n"
     L"не найден среди переменных задачи",pp->Nam,p->Nam,Tv->Name,Tv->Line);
    Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
  }}
  else{
   switch(i){
    case 3: case 4: case 5:
     if(pp->Val<0){
      swprintf(Inf,L"Параметр \"%g\" примитива \"%s\" инструкции \"%s\" строки %d\n"
       L"должен быть положительным числом",pp->Val,p->Nam,Tv->Name,Tv->Line);
      Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
     }
     break;
    case 6:
     if(pp->Val<0||pp->Val>16777215){
      swprintf(Inf,L"Цвет контура, заданный значением \"%g\" в примитиве \"%s\"\n"
       L"инструкции \"%s\" строки %d\n"
       L"должен быть числом в диапазоне [0,16777215]",
       pp->Val,p->Nam,Tv->Name,Tv->Line);
      Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
     }
     break;
    case 7:
     if(pp->Val<-1||pp->Val>16777215){
      swprintf(Inf,L"Цвет заполнения, заданный значением \"%g\" в примитиве \"%s\"\n"
       L"инструкции \"%s\" строки %d\n"
       L"должен быть числом в диапазоне [-1,16777215],\n"
       L"причем -1 делает прозрачной внутреннюю область",
       pp->Val,p->Nam,Tv->Name,Tv->Line);
      Atten(Tv->Line,0); DelMagazine(&HeadMult,m);
}}}}}
//---------------------------------------------------------------------------
void __fastcall TMultFilms::TriAnglTest(Root *Tv,Parm *p,Magazine *m){
 int i; Parm *pp; Vary *V;
 if(KolElem(p->R)!=9){
  swprintf(Inf,L"Примитив \"%s\" инструкции \"%s\" строки %d\n"
   L"должен содержать 6 декартовых координат вершин,\n"
   L"толщину его контура, коды цвета линии и заливки.\n"
   L"Координаты вершин задаются в единицах длины задачи",p->Nam,Tv->Name,Tv->Line);
  Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
 }
 for(i=0,pp=p->R;i<9;i++,pp=pp->Sled){
  if(pp->Nam){
   if(!FindVary(pp->Nam,&V)){
    swprintf(Inf,L"Параметр \"%s\" примитива \"%s\" инструкции \"%s\" строки %d\n"
     L"не найден среди переменных задачи",pp->Nam,p->Nam,Tv->Name,Tv->Line);
    Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
  }}
  else{
   switch(i){
    case 6:
     if(pp->Val<0){
      swprintf(Inf,L"Параметр \"%g\" примитива \"%s\" инструкции \"%s\" строки %d\n"
       L"должен быть положительным числом",pp->Val,p->Nam,Tv->Name,Tv->Line);
      Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
     }
     break;
    case 7:
     if(pp->Val<0||pp->Val>16777215){
      swprintf(Inf,L"Цвет контура, заданный значением \"%g\" в примитиве \"%s\"\n"
       L"инструкции \"%s\" строки %d\n"
       L"должен быть числом в диапазоне [0,16777215]",
       pp->Val,p->Nam,Tv->Name,Tv->Line);
      Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
     }
     break;
    case 8:
     if(pp->Val<-1||pp->Val>16777215){
      swprintf(Inf,L"Цвет заполнения, заданный значением \"%g\" в примитиве \"%s\"\n"
       L"инструкции \"%s\" строки %d\n"
       L"должен быть числом в диапазоне [-1,16777215],\n"
       L"причем -1 делает прозрачной внутреннюю область",
       pp->Val,p->Nam,Tv->Name,Tv->Line);
      Atten(Tv->Line,0); DelMagazine(&HeadMult,m);
}}}}}
//---------------------------------------------------------------------------
void __fastcall TMultFilms::QuadrantTest(Root *Tv,Parm *p,Magazine *m){
 int i; Parm *pp; Vary *V;
 if(KolElem(p->R)!=8){
  swprintf(Inf,L"Примитив \"%s\" инструкции \"%s\" строки %d\n"
   L"должен содержать 2 декартовые координаты центра, радиус,\n"
   L"начальный и конечный угол в градусах, толщину контура\n"
   L"в пикселях, коды цвета линии и заливки.\n"
   L"Координаты и радиус задаются в единицах длины задачи",
   p->Nam,Tv->Name,Tv->Line);
  Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
 }
 for(i=0,pp=p->R;i<8;i++,pp=pp->Sled){
  if(pp->Nam){
   if(!FindVary(pp->Nam,&V)){
    swprintf(Inf,L"Параметр \"%s\" примитива \"%s\" инструкции \"%s\" строки %d\n"
     L"не найден среди переменных задачи",pp->Nam,p->Nam,Tv->Name,Tv->Line);
    Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
  }}
  else{
   switch(i){
    case 2: case 5:
     if(pp->Val<0){
      swprintf(Inf,L"Параметр \"%g\" примитива \"%s\" инструкции \"%s\" строки %d\n"
       L"должен быть положительным числом",pp->Val,p->Nam,Tv->Name,Tv->Line);
      Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
     }
     break;
    case 6:
     if(pp->Val<0||pp->Val>16777215){
      swprintf(Inf,L"Цвет контура, заданный значением \"%g\" в примитиве \"%s\"\n"
       L"инструкции \"%s\" строки %d\n"
       L"должен быть числом в диапазоне [0,16777215]",
       pp->Val,p->Nam,Tv->Name,Tv->Line);
      Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
     }
     break;
    case 7:
     if(pp->Val<-1||pp->Val>16777215){
      swprintf(Inf,L"Цвет заполнения, заданный значением \"%g\" в примитиве \"%s\"\n"
       L"инструкции \"%s\" строки %d\n"
       L"должен быть числом в диапазоне [-1,16777215],\n"
       L"причем -1 делает прозрачной внутреннюю область",
       pp->Val,p->Nam,Tv->Name,Tv->Line);
      Atten(Tv->Line,0); DelMagazine(&HeadMult,m);
}}}}}
//---------------------------------------------------------------------------
void __fastcall TMultFilms::ArcTest(Root *Tv,Parm *p,Magazine *m){
 int i; Parm *pp; Vary *V;
 if(KolElem(p->R)!=7){
  swprintf(Inf,L"Примитив \"%s\" инструкции \"%s\" строки %d\n"
   L"должен содержать 2 декартовые координаты центра, радиус,\n"
   L"начальный и конечный угол в градусах, толщину контура\n"
   L"в пикселях, код цвета линии.\n"
   L"Координаты и радиус задаются в единицах длины задачи",
   p->Nam,Tv->Name,Tv->Line);
  Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
 }
 for(i=0,pp=p->R;i<7;i++,pp=pp->Sled){
  if(pp->Nam){
   if(!FindVary(pp->Nam,&V)){
    swprintf(Inf,L"Параметр \"%s\" примитива \"%s\" инструкции \"%s\" строки %d\n"
     L"не найден среди переменных задачи",pp->Nam,p->Nam,Tv->Name,Tv->Line);
    Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
  }}
  else{
   switch(i){
    case 2: case 5:
     if(pp->Val<0){
      swprintf(Inf,L"Параметр \"%g\" примитива \"%s\" инструкции \"%s\" строки %d\n"
       L"должен быть положительным числом",pp->Val,p->Nam,Tv->Name,Tv->Line);
      Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
     }
     break;
    case 6:
     if(pp->Val<0||pp->Val>16777215){
      swprintf(Inf,L"Цвет контура, заданный значением \"%g\" в примитиве \"%s\"\n"
       L"инструкции \"%s\" строки %d\nдолжен быть числом в диапазоне [0,16777215]",
       pp->Val,p->Nam,Tv->Name,Tv->Line);
      Atten(Tv->Line,0); DelMagazine(&HeadMult,m);
}}}}}
//---------------------------------------------------------------------------
void __fastcall TMultFilms::ArcEllipsTest(Root *Tv,Parm *p,Magazine *m){
 int i; Parm *pp; Vary *V;
 if(KolElem(p->R)!=8){
  swprintf(Inf,L"Примитив \"%s\" инструкции \"%s\" строки %d\n"
   L"должен содержать 2 декартовые координаты центра, 2 полуоси -\n"
   L"горизонтальную и вертикальную, начальный и конечный угол в градусах,\n"
   L"толщину контура в пикселях, код цвета линии.\n"
   L"Координаты и полуоси задаются в единицах длины задачи",
   p->Nam,Tv->Name,Tv->Line);
  Atten(Tv->Line,0);
  DelMagazine(&HeadMult,m); return;
 }
 for(i=0,pp=p->R;i<8;i++,pp=pp->Sled){
  if(pp->Nam){
   if(!FindVary(pp->Nam,&V)){
    swprintf(Inf,L"Параметр \"%s\" примитива \"%s\" инструкции \"%s\" строки %d\n"
     L"не найден среди переменных задачи",pp->Nam,p->Nam,Tv->Name,Tv->Line);
    Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
  }}
  else{
   switch(i){
    case 2: case 3: case 6:
     if(pp->Val<0){
      swprintf(Inf,L"Параметр \"%g\" примитива \"%s\" инструкции \"%s\" строки %d\n"
       L"должен быть положительным числом",pp->Val,p->Nam,Tv->Name,Tv->Line);
      Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
     }
     break;
    case 7:
     if(pp->Val<0||pp->Val>16777215){
      swprintf(Inf,L"Цвет контура, заданный значением \"%g\" в примитиве \"%s\"\n"
       L"инструкции \"%s\" строки %d\nдолжен быть числом в диапазоне [0,16777215]",
       pp->Val,p->Nam,Tv->Name,Tv->Line);
      Atten(Tv->Line,0); DelMagazine(&HeadMult,m);
}}}}}
//---------------------------------------------------------------------------
void __fastcall TMultFilms::SpringTest(Root *Tv,Parm *p,Magazine *m){
 int i; Parm *pp; Vary *V;
 if(KolElem(p->R)!=8){
  swprintf(Inf,L"Примитив \"%s\" инструкции \"%s\" строки %d\n"
   L"должен содержать 4 декартовые координаты концов,\n"
   L"ширину, толщину и код цвета линии, число витков.\n"
   L"Координаты и ширина задаются в единицах длины задачи",
   p->Nam,Tv->Name,Tv->Line);
  Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
 }
 for(i=0,pp=p->R;i<8;i++,pp=pp->Sled){
  if(pp->Nam){
   if(!FindVary(pp->Nam,&V)){
    swprintf(Inf,L"Параметр \"%s\" примитива \"%s\" инструкции \"%s\" строки %d\n"
     L"не найден среди переменных задачи",pp->Nam,p->Nam,Tv->Name,Tv->Line);
    Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
  }}
  else{
   switch(i){
    case 4: case 5:
     if(pp->Val<0){
      swprintf(Inf,L"Параметр \"%g\" примитива \"%s\" инструкции \"%s\" строки %d\n"
       L"должен быть положительным числом",pp->Val,p->Nam,Tv->Name,Tv->Line);
      Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
     }
     break;
    case 6:
     if(pp->Val<0||pp->Val>16777215){
      swprintf(Inf,L"Цвет линии, заданный значением \"%g\" в примитиве \"%s\"\n"
       L"инструкции \"%s\" строки %d\n"
       L"должен быть числом в диапазоне [0,16777215]",
       pp->Val,p->Nam,Tv->Name,Tv->Line);
      Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
     }
     break;
    case 7:
     if(pp->Val<0||pp->Val>16777215){
      swprintf(Inf,L"Число витков \"%g\" должно быть задано положительным числом\n"
       L"в примитиве \"%s\" инструкции \"%s\" строки %d. Витком\n"
       L"считается полный зигзаг (синусоида) из начальной точки.",
       pp->Val,p->Nam,Tv->Name,Tv->Line);
      Atten(Tv->Line,0); DelMagazine(&HeadMult,m);
}}}}}
//---------------------------------------------------------------------------
void __fastcall TMultFilms::ScrewRackTest(Root *Tv,Parm *p,Magazine *m){
 int i; Parm *pp; Vary *V;
 if(KolElem(p->R)!=8){
  swprintf(Inf,L"Примитив \"%s\" инструкции \"%s\" строки %d\n"
   L"должен содержать абсциссу, ординату и угол наклона касательной\n"
   L"на левом и правом концах рейки, толщину и код цвета линии.\n"
   L"Координаты и углы наклона касательной задаются в единицах задачи",
   p->Nam,Tv->Name,Tv->Line);
  Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
 }
 for(i=0,pp=p->R;i<8;i++,pp=pp->Sled){
  if(pp->Nam){
   if(!FindVary(pp->Nam,&V)){
    swprintf(Inf,L"Параметр \"%s\" примитива \"%s\" инструкции \"%s\" строки %d\n"
     L"не найден среди переменных задачи",pp->Nam,p->Nam,Tv->Name,Tv->Line);
    Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
  }}
  else{
   switch(i){
    case 6:
     if(pp->Val<0){
      swprintf(Inf,L"Параметр \"%g\" примитива \"%s\" инструкции \"%s\" строки %d\n"
       L"должен быть положительным числом",pp->Val,p->Nam,Tv->Name,Tv->Line);
      Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
     }
     break;
    case 7:
     if(pp->Val<0||pp->Val>16777215){
      swprintf(Inf,L"Цвет линии, заданный значением \"%g\" в примитиве \"%s\"\n"
       L"инструкции \"%s\" строки %d\n"
       L"должен быть числом в диапазоне [0,16777215]",
       pp->Val,p->Nam,Tv->Name,Tv->Line);
      Atten(Tv->Line,0); DelMagazine(&HeadMult,m);
}}}}}
//---------------------------------------------------------------------------
void __fastcall TMultFilms::TextTest(Root *Tv,Parm *p,Magazine *m){
 int i; Parm *pp; Vary *V;
 if(KolElem(p->R)!=5){
  swprintf(Inf,L"Примитив \"%s\" инструкции \"%s\" строки %d\n"
   L"должен содержать 2 декартовые координаты левого верхнего\n"
   L"угла области вывода текста, размер и цвет шрифта, сам текст",
   p->Nam,Tv->Name,Tv->Line);
  Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
 }
 for(i=0,pp=p->R;i<4;i++,pp=pp->Sled){
  if(pp->Nam){
   if(!FindVary(pp->Nam,&V)){
    swprintf(Inf,L"Параметр \"%s\" примитива \"%s\" инструкции \"%s\" строки %d\n"
     L"не найден среди переменных задачи",pp->Nam,p->Nam,Tv->Name,Tv->Line);
    Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
  }}
  else{
   switch(i){
    case 2:
     if(pp->Val<0){
      swprintf(Inf,L"Размер шрифта, заданный значением \"%g\" примитива \"%s\"\n"
       L"инструкции \"%s\" строки %d\n"
       L"должен быть положительным числом",pp->Val,p->Nam,Tv->Name,Tv->Line);
      Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
     }
     break;
    case 3:
     if(pp->Val<0||pp->Val>16777215){
      swprintf(Inf,L"Цвет шрифта, заданный значением \"%g\" в примитиве \"%s\"\n"
       L"инструкции \"%s\" строки %d\n"
       L"должен быть числом в диапазоне [0,16777215]",
       pp->Val,p->Nam,Tv->Name,Tv->Line);
      Atten(Tv->Line,0); DelMagazine(&HeadMult,m);
}}}}}
//---------------------------------------------------------------------------
int FindCentroid(Centroid *Rout,wchar_t *S,Centroid **M){
 if(!(*M=Rout)) return 0;
 if(wcscmp(S,Rout->Name)<0){ *M=NULL; return 0; }
 for(;;*M=(*M)->Sled){
  if(!wcscmp((*M)->Name,S)) return 1;
  if(!((*M)->Sled)) return 0;
  if(wcscmp((*M)->Sled->Name,S)>0) return 0;
}}
//---------------------------------------------------------------------------
Centroid *TakeCentroid(Centroid **Rout,wchar_t *S,int K){
 Centroid *M,*m; if(!S) return NULL;
 if(FindCentroid(*Rout,S,&M)){
  if(K>M->Kt){
   M->XP=(double *)realloc(M->XP,K*SzD);
   M->YP=(double *)realloc(M->YP,K*SzD);
   M->xp=(double *)realloc(M->xp,K*SzD);
   M->yp=(double *)realloc(M->yp,K*SzD);
  }
  M->Kt=K;
  return M;
 }
 m=(Centroid *)calloc(1,sizeof(Centroid)); m->Kt=K;
 m->Name=wcscpy((wchar_t *)malloc(SzC*(SzC*(wcslen(S)+1))),S);
 m->XP=(double *)calloc(K,SzD); m->YP=(double *)calloc(K,SzD);
 m->xp=(double *)calloc(K,SzD); m->yp=(double *)calloc(K,SzD);
 if(!M){ m->Sled=*Rout;   return   *Rout=m; }
 else  { m->Sled=M->Sled; return M->Sled=m; }
}
//---------------------------------------------------------------------------
void __fastcall TMultFilms::CentroidsTest(Root *Tv,Parm *p,Magazine *m){
 int i,Kt; Parm *pp; Vary *V; wchar_t Name[256]; Centroid *c;
 if(KolElem(p->R)!=6){
  swprintf(Inf,L"Примитив \"%s\" инструкции \"%s\" строки %d\n"
   L"должен содержать 3 переменные, формулы которых задают\n"
   L"2 декартовые координаты полюса тела, центроиды которого\n"
   L" отображаются, и угол его поворота от горизонтальной оси,\n"
   L"затем должны быть заданы толщина и коды цвета линий\n"
   L"неподвижной и подвижной центроид",p->Nam,Tv->Name,Tv->Line);
  Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
 }
 for(i=0,pp=p->R;i<3;i++,pp=pp->Sled){
  if(pp->Nam){
   if(!FindVary(pp->Nam,&V)){ Vary *VV; Form F;
    swprintf(Inf,L"Параметр \"%s\" примитива \"%s\" инструкции \"%s\" строки %d\n"
     L"не найден среди переменных задачи",pp->Nam,p->Nam,Tv->Name,Tv->Line);
    Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
  }}
  else if(i==2){
   swprintf(Inf,L"3-й параметр примитива \"%s\" инструкции \"%s\"\n"
    L"строки %d должен быть переменной, задающей угол поворота тела\n"
    L"центроиды которого отображаются, и не может быть константой",
    p->Nam,Tv->Name,Tv->Line);
   Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
 }}
 for(;i<6;i++,pp=pp->Sled){
  if(pp->Nam){
   if(!FindVary(pp->Nam,&V)){
    swprintf(Inf,L"Параметр \"%s\" примитива \"%s\" инструкции \"%s\" строки %d\n"
     L"не найден среди переменных задачи",pp->Nam,p->Nam,Tv->Name,Tv->Line);
    Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
  }}
  else{
   switch(i){
    case 3:
     if(pp->Val<0){
      swprintf(Inf,L"Параметр \"%g\" примитива \"%s\" инструкции \"%s\" строки %d\n"
       L"должен быть положительным числом",pp->Val,p->Nam,Tv->Name,Tv->Line);
      Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
     }
     break;
    case 4: case 5:
     if(pp->Val<0||pp->Val>16777215){
      swprintf(Inf,L"Цвет шрифта, заданный значением \"%g\" в примитиве \"%s\"\n"
       L"инструкции \"%s\" строки %d\nдолжен быть числом в диапазоне [0,16777215]",
       pp->Val,p->Nam,Tv->Name,Tv->Line);
      Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
 }}}}
 Kt=(*Tmult)*(*Kkadr)/1000+1;
 swprintf(Name,L"%s_%s_%s",p->R->Nam,p->R->Sled->Nam,p->R->Sled->Sled->Nam);
 c=TakeCentroid(&Rcentr,Name,Kt);
 for(i=0,pp=p->R;i<3;i++,pp=pp->Sled){
  Vary *VV; Form F;
  if(pp->Nam){
   FindVary(pp->Nam,&V); swprintf(Name,L"%s'%s",V->Name,L->Time->Name);
   if(!FindVary(Name,&VV)){
    F=DifByT(V);
    if((F==Nul)&&(i==2)){
     swprintf(Inf,L"3-й параметр примитива \"%s\" инструкции \"%s\"\n"
      L"строки %d должен быть переменной, задающей угол поворота тела\n"
      L"центроиды которого отображаются, и не может быть константой",
      p->Nam,Tv->Name,Tv->Line);
     Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
   }}
   else F.V=VV;
   (i<1?c->FVxA:i<2?c->FVyA:c->FWfi)=F;
  }
  else (i<1?c->FVxA.C:c->FVyA.C)=Nul;
}}
//---------------------------------------------------------------------------
void __fastcall TMultFilms::Manipulator3dTest(Root *Tv,Parm *p,Magazine *m){
 int i; Parm *pp=p->R; Vary *V; double D;
 for(i=0;i<4;i++,pp=pp->Sled){
  if(pp->Nam){
   if(!FindVary(pp->Nam,&V)){
    swprintf(Inf,L"Параметр \"%s\" примитива \"%s\" не найден среди переменных задачи",
    pp->Nam,p->Nam);
    Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
   }
   D=Val(V->Znach);
  }
  else if(pp->F.C&&(pp->F.C!=Pust)) D=Val(pp->F);
  else D=pp->Val;
   if(D<0){
    swprintf(Inf,L"Параметр \"%g\" примитива\"%s\" инструкции\"%s\" строки %d\n"
     L"должен быть положительным числом",pp->Val,p->Nam,Tv->Name,Tv->Line);
    Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
 }}
 for(i=0;i<2;i++,pp=pp->Sled){
  if(pp->Nam){
   if(!FindVary(pp->Nam,&V)){
    swprintf(Inf,L"Параметр \"%s\" примитива \"%s\" не найден среди переменных задачи",
    pp->Nam,p->Nam);
    Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
 }}}
 for(;pp;pp=pp->Sled){ Body *B;
  if(!FindBody(pp->Nam,&B)){
   swprintf(Inf,L"Параметр \"%s\" примитива \"%s\" не найден среди твердых тел задачи",
    pp->Nam,p->Nam);
   Atten(Tv->Line,0); DelMagazine(&HeadMult,m); return;
}}}
/*Надо проверить, что координаты конца определяются через переменные, имеющиеся
в списке переменных, а имена тел соответствуют имеющимся телам в списке тел.*/
//---------------------------------------------------------------------------
TMultFilms::TMultFilms(TMenuItem *mMF,TMenuItem *mMF0,TMenuItem *mMF1,
  TMenuItem *mMF2,TMenuItem *mMF3,TMenuItem *mMF4,TMenuItem *mMF5,
  TMenuItem *mMF6,TMenuItem *mMF7,TMenuItem *mMF8,TMenuItem *mMF9,
  int *WP,long *Tm,int *Kk,TImage *Im,TImage *PIm,TCGauge *CG,
  TColor *clG,TColor *clH,TColor *clP,TColor *clS,TColor *clC,TColor *clN,
  TColor *clR,TStatusBar *SB,
  double tn,double tk,double *qo,double *Vqo,double *zo,double *wo,double *mg,
  Form *DT,bool *NC,bool *IsNC,bool *Shlf,TTimer *TimerMF,TCppWebBrowser *CppWB,
  TToolButton *Table,TToolButton *ToBMP,TToolButton *Print,
  TToolButton *TBI,TToolButton *TBF,TToolButton *TBR,TToolButton *TBG,
  bool *ToJpg,bool *ToBmp){
 MMFs=mMF; mMFi[0]=mMF0; mMFi[1]=mMF1; mMFi[2]=mMF2; mMFi[3]=mMF3; mMFi[4]=mMF4;
 mMFi[5]=mMF5; mMFi[6]=mMF6; mMFi[7]=mMF7; mMFi[8]=mMF8; mMFi[9]=mMF9;
 SHLEIF=Shlf; WidthPen=WP; Tmult=Tm; Kkadr=Kk; Tn=tn; Tk=t; Rsp=NULL;
 q0=qo; Vq0=Vqo; z0=zo; w0=wo;
 Image=Im;
 PhonImage=PIm;
// AnimaPaintBox=Im; PhonPaintBox=PIm;
 CGauge=CG;
 ColorGraph=clG; ColorHead=clH; ColorPhon=clP; //TBG->Enabled=false;
 ColorGrid=clS; ColorCifrGrid=clC; ColorNameOs=clN; ColorRamka=clR;
 StatusBar=SB; for(int i=0;i<10;i++) MG[i]=mg[i];
// if(!CGauge)
//  Termometr=new TTermometr(StatusBar->Color,*ColorGraph,StatusBar);
 HeadMult=NULL; NamesVarys=NULL; Vals=NULL; DsetaT=DT; NoCalc=NC; IsNoCalc=IsNC;
 TimerMultFilm=TimerMF; CppWebBrowser=CppWB;
 TB_Table=Table; TB_ToBMP=ToBMP; TB_Print=Print; ToolButtonInclude=TBI;
 tbFormula=TBF; ToolButtonRead=TBR; TOJPEG=ToJpg; TOBMP=ToBmp;
 TimerMF->OnTimer=TimerMultFilmTimer;
 for(Root *Tv=L->Inst;Tv;Tv=Tv->Sled){
  LowToUpp(Tv->Name); Parm *p,*pn,*pp; Magazine *m; int i; Vary *V;
  if((wcsstr(Tv->Name,L"АНИМАЦИЯ")==Tv->Name)||
     (wcsstr(Tv->Name,L"ФОН")==Tv->Name)){
   if(Tv->First&&Tv->First->Nam){
    if(Tv->First->R){
     int K=KolElem(Tv->First->R);
     if(K!=2){
      swprintf(Inf,L"В инструкции \"%s\" (строка %d)\n"
       L"после названия в скобках можно указать 2 элемента\n"
       L"- название оси абсцисс и название оси ординат",Tv->Name,Tv->Line);
      wcscpy(Inf,L"После названия в скобках можно указать 2 элемента\n"
       L"- название оси абсцисс и название оси ординат");
      Atten(Tv->Line,0);
     }
     else if(!Tv->First->R->Nam||!Tv->First->R->Sled->Nam){
      wcscpy(Inf,L"После названия в скобках может стоять только два имени\n"
       L"- название оси абсцисс и название оси ординат");
      Atten(Tv->Line,0);
    }}
    m=TakeMagazine(&HeadMult,Tv->First->Nam);
   }
   else{
    swprintf(Inf,L"Инструкция \"%s\" (строка %d)\n"
     L"должна начинаться с названия",Tv->Name,Tv->Line);
    Atten(Tv->Line,0);
    swprintf(Inf,L"Анимация строки %d",Tv->Line); m=TakeMagazine(&HeadMult,Inf);
    return;
   }
   for(p=Tv->First->Sled;p;p=p->Sled){
    wchar_t UppHead[256],*Head;
    if(!p->R){
     swprintf(Inf,L"В инструкции \"%s\" строки %d не задано название примитива",
      Tv->Name,Tv->Line);
     Atten(Tv->Line,0); DelStruct((void **)&HeadMult,m); continue;
    }
    else{
     Head=p->Nam; LowToUpp(Head);
     if(!wcscmp(Head,             L"ТОЧКА")){ PointTest(Tv,p,m);      continue; }
     if(!wcscmp(Head,        L"ОКРУЖНОСТЬ")||
        !wcscmp(Head,              L"КРУГ")){ CircleTest(Tv,p,m);     continue; }
     if(!wcscmp(Head,      L"ОКРУЖНОСТЬ3D")||
        !wcscmp(Head,            L"КРУГ3D")){ Circle3dTest(Tv,p,m);   continue; }
     if(!wcscmp(Head,            L"ЭЛЛИПС")){ EllipsTest(Tv,p,m);     continue; }
     if(!wcscmp(Head,             L"ВОРОТ")){ VorotTest(Tv,p,m);      continue; }
     if(!wcscmp(Head,    L"ВОРОТ С ГРУЗОМ")){ GateWithLoadTest(Tv,p,m);continue;}
     if(!wcscmp(Head,            L"КОЛЕСО")){ KolesoTest(Tv,p,m);     continue; }
     if(!wcscmp(Head,L"ПЕРЕДАЧА КЛИНОРЕМЕННАЯ")){ GearTest(Tv,p,m);   continue; }
     if(!wcscmp(Head,            L"ПРЯМАЯ")||
        !wcscmp(Head,            L"ВЕКТОР")){ LineTest(Tv,p,m);       continue; }
     if(!wcscmp(Head,           L"СТРЕЛКА")){ FingerTest(Tv,p,m);     continue; }
     if(!wcscmp(Head,          L"СКОРОСТЬ")){ SpeedTest(Tv,p,m);      continue; }
     if(!wcscmp(Head,L"УСКОРЕНИЕ КАСАТЕЛЬНОЕ")||
        !wcscmp(Head,L"УСКОРЕНИЕ ТАНГЕНЦИАЛЬНОЕ")){
                                             AtangTest(Tv,p,m);      continue; }
     if(!wcscmp(Head,L"УСКОРЕНИЕ НОРМАЛЬНОЕ")){
                                             AnormTest(Tv,p,m);      continue; }
     if(!wcscmp(Head,          L"СТЕРЖЕНЬ")){ StringTest(Tv,p,m);     continue; }
     if(!wcscmp(Head,      L"МНОГОЗВЕННИК")){ MultiLinkTest(Tv,p,m);  continue; }
     if(!wcscmp(Head,         L"ПОЛИГОН3D")){ Polygon3dTest(Tv,p,m);  continue; }
     if(!wcscmp(Head,            L"СПЛАЙН")){ SplineTest(Tv,p,m);     continue; }
     if(!wcscmp(Head,L"ШАРНИР НЕПОДВИЖНЫЙ")){ HingeTest(Tv,p,m);      continue; }
     if(!wcscmp(Head,  L"ШАРНИР ПОДВИЖНЫЙ")){ HingeTest(Tv,p,m);      continue; }
     if(!wcscmp(Head,     L"ПРЯМОУГОЛЬНИК")){ BarTest(Tv,p,m);        continue; }
     if(!wcscmp(Head,       L"ТРЕУГОЛЬНИК")){ TriAnglTest(Tv,p,m);    continue; }
     if(!wcscmp(Head,            L"СЕКТОР")){ QuadrantTest(Tv,p,m);   continue; }
     if(!wcscmp(Head,              L"ДУГА")){ ArcTest(Tv,p,m);        continue; }
     if(!wcscmp(Head,      L"ДУГА ЭЛЛИПСА")){ ArcEllipsTest(Tv,p,m);  continue; }
     if(!wcscmp(Head,           L"ПРУЖИНА")){ SpringTest(Tv,p,m);     continue; }
     if(!wcscmp(Head,             L"РЕЙКА")){ ScrewRackTest(Tv,p,m);  continue; }
     if(!wcscmp(Head,         L"ЦЕНТРОИДЫ")){ CentroidsTest(Tv,p,m);  continue; }
     if(!wcscmp(Head,             L"ТЕКСТ")){ TextTest(Tv,p,m);       continue; }
     if(!wcscmp(Head,        L"ТРАЕКТОРИЯ")){ TraectorTest(Tv,p,m);   continue; }
     if(!wcscmp(Head,      L"ТРАЕКТОРИЯ3D")){ Traector3dTest(Tv,p,m); continue; }
     if(!wcscmp(Head,     L"МАНИПУЛЯТОР3D")){ Manipulator3dTest(Tv,p,m); continue; }
 }}}}
 if(HeadMult){
  if(KolElem(HeadMult)>10){ int i; Magazine *m,*M;
   swprintf(Inf,L"Число анимаций не может быть больше 10.");
   Application->MessageBox(Inf,L"Внимание!",MB_OK|MB_ICONWARNING);
   for(i=0,m=HeadMult;m&&(i<9);m=m->Sled,i++); M=m->Sled; m->Sled=NULL;
   DelAllMagazine(&M);
  }
  MMFs->Caption=KolElem(HeadMult)>1?L"Анимации":L"Анимация";
  MMFs->Enabled=true;
  for(Magazine *m=HeadMult;m;m=m->Sled){
   if(mMFi[8]->Visible){
    mMFi[9]->Visible=true; mMFi[9]->Caption=m->S; continue;
   }
   if(mMFi[7]->Visible){
    mMFi[8]->Visible=true; mMFi[8]->Caption=m->S; continue;
   }
   if(mMFi[6]->Visible){
    mMFi[7]->Visible=true; mMFi[7]->Caption=m->S; continue;
   }
   if(mMFi[5]->Visible){
    mMFi[6]->Visible=true; mMFi[6]->Caption=m->S; continue;
   }
   if(mMFi[4]->Visible){
    mMFi[5]->Visible=true; mMFi[5]->Caption=m->S; continue;
   }
   if(mMFi[3]->Visible){
    mMFi[4]->Visible=true; mMFi[4]->Caption=m->S; continue;
   }
   if(mMFi[2]->Visible){
    mMFi[3]->Visible=true; mMFi[3]->Caption=m->S; continue;
   }
   if(mMFi[1]->Visible){
    mMFi[2]->Visible=true; mMFi[2]->Caption=m->S; continue;
   }
   if(mMFi[0]->Visible){
    mMFi[1]->Visible=true; mMFi[1]->Caption=m->S; continue;
   }
   if(!mMFi[0]->Visible){
    mMFi[0]->Visible=true; mMFi[0]->Caption=m->S;
}}}}
//---------------------------------------------------------------------------
Parm *__fastcall TMultFilms::ParmText(Parm *P,int &X,int &Y,int &Size,
  TColor &Color,wchar_t **Text){ Vary *V;
 double x,y; int Ind;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); x=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) x=Val(P->F); else x=P->Val;
 X=Round((x-xk)/MSt[0])+Xk; P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); y=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) y=Val(P->F); else y=P->Val;
 Y=Yk-Round((y-yk)/MSt[1]); P=P->Sled;
 Size=Round(P->Nam?FindVary(P->Nam,&V),Val(V->Znach):(P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val);
 if(Size<0) Size=12; P=P->Sled;
 Color=(TColor)Round(P->Nam?FindVary(P->Nam,&V),Val(V->Znach):
  (P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val);
 P=P->Sled; *Text=P->Nam; P=P->Sled; return P;
}
//---------------------------------------------------------------------------
Parm *__fastcall TMultFilms::DrawText(TCanvas *Canvas,Parm *P){
 int X,Y,S; TColor C,Cp=Canvas->Font->Color; wchar_t *T;
 P=ParmText(P,X,Y,S,C,&T); Canvas->Font->Size=S; Canvas->Font->Color=C;
 Canvas->TextOut(X,Y,T);
 Canvas->Font->Color=Cp;
 return P;
}
//---------------------------------------------------------------------------
/*
Parm *__fastcall TMultFilms::ClearText(TCanvas *Canvas,Parm *P){
 int X,Y,S; TColor C; wchar_t *T;
 P=ParmText(P,X,Y,S,C,&T); Canvas->Font->Size=S; Canvas->Font->Color=*ColorPhon;
 Canvas->TextOut(X,Y,T);
 return P;
}*/
//---------------------------------------------------------------------------
Parm *__fastcall TMultFilms::GabaritPoint(Parm *P){
 double x,y; int Ind;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); x=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) x=Val(P->F); else x=P->Val;
 P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); y=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) y=Val(P->F); else y=P->Val;
 P=P->Sled; P=P->Sled;
 if(x>Spr[0]){ Spr[0]=x; Spr[1]=y; } if(x<Slv[0]){ Slv[0]=x; Slv[1]=y; }
 if(y>Max[1]){ Max[1]=y; Max[0]=x; } if(y<Min[1]){ Min[1]=y; Min[0]=x; }
 return P;
}
//---------------------------------------------------------------------------
Parm *__fastcall TMultFilms::GabaritTrajectory(Parm *P){
 double x,y; int Ind; wchar_t Name[256]; TColor C; Trajectory *r;
 if(P->Nam){ wcscpy(Name,P->Nam); Ind=NStrMagCmp(NamesVarys,P->Nam); x=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) x=Val(P->F); else x=P->Val;
 P=P->Sled; wcscat(Name,L"_");
 if(P->Nam){ wcscat(Name,P->Nam); Ind=NStrMagCmp(NamesVarys,P->Nam); y=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) y=Val(P->F); else y=P->Val;
 P=P->Sled; P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); C=(TColor)Round(Vals[Ind]); }
 else if(P->F.C&&P->F.C!=Pust) C=(TColor)Round(Val(P->F));
 else C=(TColor)Round(P->Val);
 P=P->Sled;
 if(x>Spr[0]){ Spr[0]=x; Spr[1]=y; } if(x<Slv[0]){ Slv[0]=x; Slv[1]=y; }
 if(y>Max[1]){ Max[1]=y; Max[0]=x; } if(y<Min[1]){ Min[1]=y; Min[0]=x; }
 if(FindTrajectory(Rtraek,Name,&r)){
  r->x[Nkadr]=x; r->y[Nkadr]=y; r->C[Nkadr]=C;
 }
 return P;
}
//---------------------------------------------------------------------------
Parm *__fastcall TMultFilms::GabaritTrajectory3d(Parm *P){
 double x,y; int Ind; wchar_t Name[256]; TColor C; Trajectory *r;
 if(P->Nam){ wcscpy(Name,P->Nam); Ind=NStrMagCmp(NamesVarys,P->Nam); x=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) x=Val(P->F); else x=P->Val;
 P=P->Sled; wcscat(Name,L"_");
 if(P->Nam){ wcscat(Name,P->Nam); Ind=NStrMagCmp(NamesVarys,P->Nam); y=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) y=Val(P->F); else y=P->Val;
 P=P->Sled; wcscat(Name,L"_");
 if(P->Nam) wcscat(Name,P->Nam); P=P->Sled; P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); C=(TColor)Round(Vals[Ind]); }
 else if(P->F.C&&P->F.C!=Pust) C=(TColor)Round(Val(P->F));
 else C=(TColor)Round(P->Val);
 P=P->Sled;
 if(x>Spr[0]){ Spr[0]=x; Spr[1]=y; } if(x<Slv[0]){ Slv[0]=x; Slv[1]=y; }
 if(y>Max[1]){ Max[1]=y; Max[0]=x; } if(y<Min[1]){ Min[1]=y; Min[0]=x; }
 if(FindTrajectory(Rtraek,Name,&r)){
  r->x[Nkadr]=x; r->y[Nkadr]=y; r->C[Nkadr]=C;
 }
 return P;
}
//---------------------------------------------------------------------------
Parm *__fastcall TMultFilms::ParmPoint(Parm *P,int &X,int &Y,TColor &Color){
 Vary *V; double x,y; int Ind;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); x=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) x=Val(P->F); else x=P->Val;
 X=Round((x-xk)/MSt[0])+Xk; P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); y=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) y=Val(P->F); else y=P->Val;
 Y=Yk-Round((y-yk)/MSt[1]); P=P->Sled;
 Color=(TColor)Round(P->Nam?FindVary(P->Nam,&V),Val(V->Znach):(P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val);
 if(Color<0) Color=*ColorGraph; P=P->Sled;
 return P;
}
//---------------------------------------------------------------------------
Parm *__fastcall TMultFilms::ParmPoint4(Parm *P,int &X,int &Y,int &W,TColor &Color){
 Vary *V; double x,y; int Ind;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); x=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) x=Val(P->F); else x=P->Val;
 X=Round((x-xk)/MSt[0])+Xk; P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); y=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) y=Val(P->F); else y=P->Val;
 Y=Yk-Round((y-yk)/MSt[1]); P=P->Sled;
 W=Round(P->Nam?FindVary(P->Nam,&V),Val(V->Znach):(P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val);
 if(W<0) W=1; P=P->Sled;
 Color=(TColor)Round(P->Nam&&FindVary(P->Nam,&V)?Val(V->Znach):(P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val);
 if(Color<0) Color=*ColorPhon;
 P=P->Sled;
 return P;
}
//---------------------------------------------------------------------------
Parm *__fastcall TMultFilms::DrawPoint(TCanvas *Canvas,Parm *P){
 int X,Y,R=*WidthPen,W=1; TColor C,Cp=Canvas->Pen->Color,Cb=Canvas->Brush->Color;
 if(R==0) R=1;
 P=ParmPoint(P,X,Y,C);
 Canvas->Pen->Width=W; Canvas->Pen->Color=C; Canvas->Brush->Color=C;
 Canvas->Ellipse(X-R,Y-R,X+R,Y+R);
 Canvas->Pen->Width=*WidthPen;
 Canvas->Pen->Color=Cp; Canvas->Brush->Color=Cb;
 return P;
}
//---------------------------------------------------------------------------
Parm *__fastcall TMultFilms::DrawTrajectory(TCanvas *Canvas,Parm *P){
 bool Int=Q?false:true; Trajectory *r; wchar_t Name[256]; Vary *V;
 int W; TColor C,Cp=Canvas->Pen->Color;
 if(P->Nam){ wcscpy(Name,P->Nam); wcscat(Name,L"_"); } P=P->Sled;
 if(P->Nam) wcscat(Name,P->Nam); P=P->Sled;
 W=Round(P->Nam?FindVary(P->Nam,&V),Val(V->Znach):(P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val);
 if(W<0) W=1; P=P->Sled;
 C=(TColor)Round(P->Nam&&FindVary(P->Nam,&V)?Val(V->Znach):(P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val);
 if(C<0) C=*ColorPhon;
 P=P->Sled;
 Canvas->Pen->Width=W; Canvas->Pen->Color=C;
 if(FindTrajectory(Rtraek,Name,&r)){
  Canvas->MoveTo(r->X[0],r->Y[0]);
  for(int i=1;i<=Nkadr;i++){
   if((r->C[i]<0)||(r->C[i]==*ColorPhon)) Canvas->MoveTo(r->X[i],r->Y[i]);
   else Canvas->LineTo(r->X[i],r->Y[i]);
 }}
 Canvas->Pen->Color=Cp; Canvas->Pen->Width=*WidthPen;
 return P;
}
//---------------------------------------------------------------------------
Parm *__fastcall TMultFilms::DrawTrajectory3d(TCanvas *Canvas,Parm *P){
 bool Int=Q?false:true; Trajectory *r; wchar_t Name[256]; Vary *V;
 int W; TColor C,Cp=Canvas->Pen->Color;
 if(P->Nam){ wcscpy(Name,P->Nam); wcscat(Name,L"_"); } P=P->Sled;
 if(P->Nam){ wcscat(Name,P->Nam); wcscat(Name,L"_"); } P=P->Sled;
 if(P->Nam) wcscat(Name,P->Nam); P=P->Sled;
 W=Round(P->Nam?FindVary(P->Nam,&V),Val(V->Znach):(P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val);
 if(W<0) W=1; P=P->Sled;
 C=(TColor)Round(P->Nam&&FindVary(P->Nam,&V)?Val(V->Znach):(P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val);
 if(C<0) C=*ColorPhon;
 P=P->Sled;
 Canvas->Pen->Width=W; Canvas->Pen->Color=C;
 if(FindTrajectory(Rtraek,Name,&r)){
  Canvas->MoveTo(r->X[0],r->Y[0]);
  for(int i=1;i<=Nkadr;i++){
   if((r->C[i]<0)||(r->C[i]==*ColorPhon)) Canvas->MoveTo(r->X[i],r->Y[i]);
   else Canvas->LineTo(r->X[i],r->Y[i]);
 }}
 Canvas->Pen->Color=Cp; Canvas->Pen->Width=*WidthPen;
 return P;
}
//---------------------------------------------------------------------------
/*
Parm *__fastcall TMultFilms::ClearPoint(TCanvas *Canvas,Parm *P){
 int X,Y,R=*WidthPen,W=1; TColor C;
 if(R==0) R=1;
 P=ParmPoint(P,X,Y,C);
 Canvas->Pen->Width=W; Canvas->Pen->Color=*ColorPhon;
 Canvas->Brush->Color=*ColorPhon;
 Canvas->Ellipse(X-R,Y-R,X+R,Y+R);
 Canvas->Pen->Width=*WidthPen;
 return P;
}*/
//---------------------------------------------------------------------------
Parm *__fastcall TMultFilms::ParmSector(Parm *P,int &X1,int &Y1,int &X2,int &Y2,
  int &X3,int &Y3,int &X4,int &Y4,int &Width,TColor &cL,TColor &cF){
 double xC,yC,r,a0,a1; int Ind; Vary *V;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); xC=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) xC=Val(P->F); else xC=P->Val;
 P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); yC=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) yC=Val(P->F); else yC=P->Val;
 P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); r=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) r=Val(P->F); else r=P->Val;
 P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); a0=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) a0=Val(P->F); else a0=P->Val;
 P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); a1=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) a1=Val(P->F); else a1=P->Val;
 P=P->Sled;
 X1=Round((xC-r-xk)/MSt[0])+Xk; Y1=Yk-Round((yC+r-yk)/MSt[1]);
 X2=Round((xC+r-xk)/MSt[0])+Xk; Y2=Yk-Round((yC-r-yk)/MSt[1]);
 X3=Round((xC+r*cos(a0*M_PI/180)-xk)/MSt[0])+Xk;
 Y3=Yk-Round((yC+r*sin(a0*M_PI/180)-yk)/MSt[1]);
 X4=Round((xC+r*cos(a1*M_PI/180)-xk)/MSt[0])+Xk;
 Y4=Yk-Round((yC+r*sin(a1*M_PI/180)-yk)/MSt[1]);
 Width=Round(P->Nam?FindVary(P->Nam,&V),Val(V->Znach):(P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val);
 if(Width<0) Width=1; P=P->Sled;
 cL=(TColor)Round(P->Nam?FindVary(P->Nam,&V),Val(V->Znach):(P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val);
 if(cL<0) cL=*ColorGraph; P=P->Sled;
 cF=(TColor)Round(P->Nam?FindVary(P->Nam,&V),Val(V->Znach):(P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val);
 P=P->Sled;
 return P;
}
//---------------------------------------------------------------------------
Parm *__fastcall TMultFilms::ParmArcEllips(Parm *P,int &X1,int &Y1,int &X2,
  int &Y2,int &X3,int &Y3,int &X4,int &Y4,int &Width,TColor &cL){
 double xC,yC,a,b,r,a0,a1; int Ind; Vary *V;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); xC=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) xC=Val(P->F); else xC=P->Val;
 P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); yC=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) yC=Val(P->F); else yC=P->Val;
 P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); a=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) a=Val(P->F); else a=P->Val;
 P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); b=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) b=Val(P->F); else b=P->Val;
 P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); a0=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) a0=Val(P->F); else a0=P->Val;
 P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); a1=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) a1=Val(P->F); else a1=P->Val;
 P=P->Sled;
 X1=Round((xC-a-xk)/MSt[0])+Xk; Y1=Yk-Round((yC+b-yk)/MSt[1]);
 X2=Round((xC+a-xk)/MSt[0])+Xk; Y2=Yk-Round((yC-b-yk)/MSt[1]);
 r=a>b?a:b;
 X3=Round((xC+r*cos(a0*M_PI/180)-xk)/MSt[0])+Xk;
 Y3=Yk-Round((yC+r*sin(a0*M_PI/180)-yk)/MSt[1]);
 X4=Round((xC+r*cos(a1*M_PI/180)-xk)/MSt[0])+Xk;
 Y4=Yk-Round((yC+r*sin(a1*M_PI/180)-yk)/MSt[1]);
 Width=Round(P->Nam?FindVary(P->Nam,&V),Val(V->Znach):
       (P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val);
 if(Width<0) Width=1; P=P->Sled;
 cL=(TColor)Round(P->Nam?FindVary(P->Nam,&V),Val(V->Znach):
      (P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val);
 if(cL<0) cL=*ColorGraph; P=P->Sled;
 return P;
}
//---------------------------------------------------------------------------
Parm *__fastcall TMultFilms::ParmArc(Parm *P,int &X1,int &Y1,int &X2,int &Y2,
  int &X3,int &Y3,int &X4,int &Y4,int &Width,TColor &cL){
 double xC,yC,r,a0,a1; int Ind; Vary *V;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); xC=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) xC=Val(P->F); else xC=P->Val;
 P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); yC=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) yC=Val(P->F); else yC=P->Val;
 P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); r=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) r=Val(P->F); else r=P->Val;
 P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); a0=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) a0=Val(P->F); else a0=P->Val;
 P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); a1=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) a1=Val(P->F); else a1=P->Val;
 P=P->Sled;
 X1=Round((xC-r-xk)/MSt[0])+Xk; Y1=Yk-Round((yC+r-yk)/MSt[1]);
 X2=Round((xC+r-xk)/MSt[0])+Xk; Y2=Yk-Round((yC-r-yk)/MSt[1]);
 X3=Round((xC+r*cos(a0*M_PI/180)-xk)/MSt[0])+Xk;
 Y3=Yk-Round((yC+r*sin(a0*M_PI/180)-yk)/MSt[1]);
 X4=Round((xC+r*cos(a1*M_PI/180)-xk)/MSt[0])+Xk;
 Y4=Yk-Round((yC+r*sin(a1*M_PI/180)-yk)/MSt[1]);
 Width=Round(P->Nam?FindVary(P->Nam,&V),Val(V->Znach):
        (P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val);
 if(Width<0) Width=1; P=P->Sled;
 cL=(TColor)Round(P->Nam?FindVary(P->Nam,&V),Val(V->Znach):
             (P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val);
 if(cL<0) cL=*ColorGraph; P=P->Sled;
 return P;
}
//---------------------------------------------------------------------------
Parm *__fastcall TMultFilms::GabaritSector(Parm *P){        
 double xC,yC,x,y,r,a0,a1,a; int Ind;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); xC=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) xC=Val(P->F); else xC=P->Val;
 P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); yC=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) yC=Val(P->F); else yC=P->Val;
 P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); r=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) r=Val(P->F); else r=P->Val;
 P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); a0=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) a0=Val(P->F); else a0=P->Val;
 P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); a1=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) a1=Val(P->F); else a1=P->Val;
 P=P->Sled; P=P->Sled; P=P->Sled; P=P->Sled;
 if(xC>Spr[0]){ Spr[0]=xC; Spr[1]=yC; } if(yC>Max[1]){ Max[1]=yC; Max[0]=xC; }
 for(a=a0;a<a1;a++){
  x=xC+r*cos(a*M_PI/180); y=yC+r*sin(a*M_PI/180);
  if(x>Spr[0]){ Spr[0]=x; Spr[1]=y; } if(x<Slv[0]){ Slv[0]=x; Slv[1]=y; }
  if(y>Max[1]){ Max[1]=y; Max[0]=x; } if(y<Min[1]){ Min[1]=y; Min[0]=x; }
 }
 return P;
}
//---------------------------------------------------------------------------
Parm *__fastcall TMultFilms::GabaritArc(Parm *P){
 double xC,yC,x,y,r,a0,a1,a; int Ind;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); xC=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) xC=Val(P->F); else xC=P->Val;
 P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); yC=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) yC=Val(P->F); else yC=P->Val;
 P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); r=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) r=Val(P->F); else r=P->Val;
 P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); a0=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) a0=Val(P->F); else a0=P->Val;
 P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); a1=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) a1=Val(P->F); else a1=P->Val;
 P=P->Sled->Sled->Sled;
 for(a=a0;a<a1;a++){
  x=xC+r*cos(a*M_PI/180); y=yC+r*sin(a*M_PI/180);
  if(x>Spr[0]){ Spr[0]=x; Spr[1]=y; } if(x<Slv[0]){ Slv[0]=x; Slv[1]=y; }
  if(y>Max[1]){ Max[1]=y; Max[0]=x; } if(y<Min[1]){ Min[1]=y; Min[0]=x; }
 }
 return P;
}
//---------------------------------------------------------------------------
Parm *__fastcall TMultFilms::GabaritArcEllips(Parm *P){
 double xC,yC,x,y,a,b,a0,a1,alfa; int Ind;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); xC=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) xC=Val(P->F); else xC=P->Val;
 P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); yC=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) yC=Val(P->F); else yC=P->Val;
 P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); a=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) a=Val(P->F); else a=P->Val;
 P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); b=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) b=Val(P->F); else b=P->Val;
 P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); a0=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) a0=Val(P->F); else a0=P->Val;
 P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); a1=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) a1=Val(P->F); else a1=P->Val;
 P=P->Sled->Sled->Sled;
 for(alfa=a0;alfa<a1;alfa++){
  x=xC+a*cos(alfa*M_PI/180); y=yC+b*sin(alfa*M_PI/180);
  if(x>Spr[0]){ Spr[0]=x; Spr[1]=y; } if(x<Slv[0]){ Slv[0]=x; Slv[1]=y; }
  if(y>Max[1]){ Max[1]=y; Max[0]=x; } if(y<Min[1]){ Min[1]=y; Min[0]=x; }
 }
 return P;
}
//---------------------------------------------------------------------------
 Parm *__fastcall TMultFilms::DrawSector(TCanvas *Canvas,Parm *P){
 int X1,Y1,X2,Y2,X3,Y3,X4,Y4,W;
 TColor cL,cF,cP=Canvas->Pen->Color,cB=Canvas->Brush->Color;
 P=ParmSector(P,X1,Y1,X2,Y2,X3,Y3,X4,Y4,W,cL,cF);
 Canvas->Pen->Width=W; Canvas->Pen->Color=cL; Canvas->Brush->Color=cF;
 if(cF<0){
  Canvas->MoveTo((X1+X2)/2,(Y1+Y2)/2); Canvas->LineTo(X3,Y3);
  Canvas->MoveTo((X1+X2)/2,(Y1+Y2)/2); Canvas->LineTo(X4,Y4);
  Canvas->Arc(X1,Y1,X2,Y2,X3,Y3,X4,Y4);
 }
 else Canvas->Pie(X1,Y1,X2,Y2,X3,Y3,X4,Y4);
 Canvas->Pen->Width=*WidthPen; Canvas->Pen->Color=cP; Canvas->Brush->Color=cB;
 return P;
}
//---------------------------------------------------------------------------
 Parm *__fastcall TMultFilms::DrawArc(TCanvas *Canvas,Parm *P){
 int X1,Y1,X2,Y2,X3,Y3,X4,Y4,W; TColor cL,cP=Canvas->Pen->Color;
 P=ParmArc(P,X1,Y1,X2,Y2,X3,Y3,X4,Y4,W,cL);
 Canvas->Pen->Width=W; Canvas->Pen->Color=cL;
 Canvas->Arc(X1,Y1,X2,Y2,X3,Y3,X4,Y4);
 Canvas->Pen->Width=*WidthPen; Canvas->Pen->Color=cP;
 return P;
}
//---------------------------------------------------------------------------
 Parm *__fastcall TMultFilms::DrawArcEllips(TCanvas *Canvas,Parm *P){
 int X1,Y1,X2,Y2,X3,Y3,X4,Y4,W; TColor cL,cP=Canvas->Pen->Color;
 P=ParmArcEllips(P,X1,Y1,X2,Y2,X3,Y3,X4,Y4,W,cL);
 Canvas->Pen->Width=W; Canvas->Pen->Color=cL;
 Canvas->Arc(X1,Y1,X2,Y2,X3,Y3,X4,Y4);
 Canvas->Pen->Width=*WidthPen; Canvas->Pen->Color=cP;
 return P;
}
//---------------------------------------------------------------------------
/*
Parm *__fastcall TMultFilms::ClearSector(TCanvas *Canvas,Parm *P){
 int X1,Y1,X2,Y2,X3,Y3,X4,Y4,W; TColor cL,cF;
 P=ParmSector(P,X1,Y1,X2,Y2,X3,Y3,X4,Y4,W,cL,cF);
 Canvas->Pen->Width=W; Canvas->Pen->Color=*ColorPhon;
 Canvas->Brush->Color=*ColorPhon;
 Canvas->Pie(X1,Y1,X2,Y2,X3,Y3,X4,Y4);
 Canvas->Pen->Width=*WidthPen;
 return P;
}
//---------------------------------------------------------------------------
Parm *__fastcall TMultFilms::ClearArc(TCanvas *Canvas,Parm *P){
 int X1,Y1,X2,Y2,X3,Y3,X4,Y4,W; TColor cL;
 P=ParmArc(P,X1,Y1,X2,Y2,X3,Y3,X4,Y4,W,cL);
 Canvas->Pen->Width=W; Canvas->Pen->Color=*ColorPhon;
 Canvas->Brush->Color=*ColorPhon;
 Canvas->Pie(X1,Y1,X2,Y2,X3,Y3,X4,Y4);
 Canvas->Pen->Width=*WidthPen;
 return P;
}*/
//---------------------------------------------------------------------------
Parm *__fastcall TMultFilms::ParmCircle(Parm *P,int &X,int &Y,int &R,
  int &Width,TColor &ColorK,TColor &ColorF){
 double x,y,r; int Ind; Vary *V;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); x=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) x=Val(P->F); else x=P->Val;
 X=Round((x-xk)/MSt[0])+Xk; P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); y=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) y=Val(P->F); else y=P->Val;
 Y=Yk-Round((y-yk)/MSt[1]); P=P->Sled;
 if(P->Nam){ FindVary(P->Nam,&V); r=Val(V->Znach); }
 else if(P->F.C&&P->F.C!=Pust) r=Val(P->F); else r=P->Val;
 r/=MSt[0]; if(r<=0) r=1; R=Round(r); P=P->Sled;
 Width=Round(P->Nam?FindVary(P->Nam,&V),Val(V->Znach):(P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val);
 if(Width<0) Width=1; P=P->Sled;
 ColorK=(TColor)Round(P->Nam?FindVary(P->Nam,&V),Val(V->Znach):(P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val);
 if(ColorK<0) ColorK=*ColorGraph; P=P->Sled;
 ColorF=(TColor)Round(P->Nam ? FindVary(P->Nam,&V),Val(V->Znach):
  P->F.C&&(P->F.C!=Pust) ? Val(P->F) : P->Val);
 P=P->Sled;
 return P;
}
//---------------------------------------------------------------------------
Parm *__fastcall TMultFilms::ParmKoleso(Parm *P,int &X,int &Y,int &R,double &Fi,
  int &Width,TColor &ColorK,TColor &ColorF){
 double x,y,r; int Ind; Vary *V;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); x=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) x=Val(P->F); else x=P->Val;
 X=Round((x-xk)/MSt[0])+Xk; P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); y=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) y=Val(P->F); else y=P->Val;
 Y=Yk-Round((y-yk)/MSt[1]); P=P->Sled;
 if(P->Nam){ FindVary(P->Nam,&V); r=Val(V->Znach); }
 else if(P->F.C&&P->F.C!=Pust) r=Val(P->F); else r=P->Val;
 r/=MSt[0]; if(r<=0) r=1; R=Round(r); P=P->Sled;
 if(P->Nam){ FindVary(P->Nam,&V); Fi=Val(V->Znach); }
 else if(P->F.C&&P->F.C!=Pust) Fi=Val(P->F); else Fi=P->Val;
 P=P->Sled;
 Width=Round(P->Nam?FindVary(P->Nam,&V),Val(V->Znach):(P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val);
 if(Width<0) Width=1; P=P->Sled;
 ColorK=(TColor)Round(P->Nam?FindVary(P->Nam,&V),Val(V->Znach):(P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val);
 if(ColorK<0) ColorK=*ColorGraph; P=P->Sled;
 ColorF=(TColor)Round(P->Nam?FindVary(P->Nam,&V),Val(V->Znach):
  P->F.C&&(P->F.C!=Pust)?Val(P->F):P->Val);
 P=P->Sled;
 return P;
}
//---------------------------------------------------------------------------
Parm *__fastcall TMultFilms::ParmVorot(Parm *P,int &X,int &Y,int &R,double &L,
  double &Al,double &Fi,int &Width,TColor &ColorK,TColor &ColorF){
 double x,y,r; int Ind; Vary *V;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); x=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) x=Val(P->F); else x=P->Val;
 X=Round((x-xk)/MSt[0])+Xk; P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); y=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) y=Val(P->F); else y=P->Val;
 Y=Yk-Round((y-yk)/MSt[1]); P=P->Sled;
 if(P->Nam){ FindVary(P->Nam,&V); r=Val(V->Znach); }
 else if(P->F.C&&P->F.C!=Pust) r=Val(P->F); else r=P->Val; P=P->Sled;
 if(P->Nam){ FindVary(P->Nam,&V); L=Val(V->Znach); }
 else if(P->F.C&&P->F.C!=Pust) L=Val(P->F); else L=P->Val;
 Fi=L/r;
 r/=MSt[0]; if(r<=0) r=1; R=Round(r); L/=MSt[0]; P=P->Sled;
 if(P->Nam){ FindVary(P->Nam,&V); Al=Val(V->Znach); }
 else if(P->F.C&&P->F.C!=Pust) Al=Val(P->F); else Al=P->Val;
 P=P->Sled;
 Width=Round(P->Nam?FindVary(P->Nam,&V),Val(V->Znach):(P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val);
 if(Width<0) Width=1; P=P->Sled;
 ColorK=(TColor)Round(P->Nam?FindVary(P->Nam,&V),Val(V->Znach):(P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val);
 if(ColorK<0) ColorK=*ColorGraph; P=P->Sled;
 ColorF=(TColor)Round(P->Nam?FindVary(P->Nam,&V),Val(V->Znach):
  P->F.C&&(P->F.C!=Pust) ? Val(P->F) : P->Val);
 P=P->Sled;
 return P;
}
//---------------------------------------------------------------------------
Parm *__fastcall TMultFilms::ParmGateWithLoad(Parm *P,int &X,int &Y,int &R,
  double &Fi,double &Al,double &l0,double &h,double &l,int &W,TColor &CK,
  TColor &CG,TColor &CB){
 double x,y,r; int Ind; Vary *V;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); x=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) x=Val(P->F); else x=P->Val;
 X=Round((x-xk)/MSt[0])+Xk; P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); y=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) y=Val(P->F); else y=P->Val;
 Y=Yk-Round((y-yk)/MSt[1]); P=P->Sled;
 if(P->Nam){ FindVary(P->Nam,&V); r=Val(V->Znach); }
 else if(P->F.C&&P->F.C!=Pust) r=Val(P->F); else r=P->Val;
 r/=MSt[0]; if(r<=0) r=1; R=Round(r); P=P->Sled;
 if(P->Nam){ FindVary(P->Nam,&V); Fi=Val(V->Znach); }
 else if(P->F.C&&P->F.C!=Pust) Fi=Val(P->F); else Fi=P->Val;
 P=P->Sled;
 if(P->Nam){ FindVary(P->Nam,&V); Al=Val(V->Znach); }
 else if(P->F.C&&(P->F.C!=Pust)) Al=Val(P->F);
 else Al=P->Val;
 P=P->Sled;
 if(P->Nam){ FindVary(P->Nam,&V); l0=Val(V->Znach); }
 else if(P->F.C&&P->F.C!=Pust) l0=Val(P->F); else l0=P->Val;
 P=P->Sled;
 if(P->Nam){ FindVary(P->Nam,&V); h=Val(V->Znach); }
 else if(P->F.C&&P->F.C!=Pust) h=Val(P->F); else h=P->Val;
 P=P->Sled;
 if(P->Nam){ FindVary(P->Nam,&V); l=Val(V->Znach); }
 else if(P->F.C&&P->F.C!=Pust) l=Val(P->F); else l=P->Val;
 P=P->Sled;
 W=Round(P->Nam?FindVary(P->Nam,&V),Val(V->Znach):(P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val);
 if(W<0) W=1; P=P->Sled;
 CK=(TColor)Round(P->Nam?FindVary(P->Nam,&V),Val(V->Znach):(P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val);
 if(CK<0) CK=*ColorGraph; P=P->Sled;
 CG=(TColor)Round(P->Nam?FindVary(P->Nam,&V),Val(V->Znach):
  P->F.C&&(P->F.C!=Pust) ? Val(P->F) : P->Val);
 P=P->Sled;
 CB=(TColor)Round(P->Nam?FindVary(P->Nam,&V),Val(V->Znach):
  P->F.C&&(P->F.C!=Pust) ? Val(P->F) : P->Val);
 P=P->Sled;
 return P;
}
//---------------------------------------------------------------------------
Parm *__fastcall TMultFilms::ParmGear(Parm *P,int &X1,int &Y1,int &R1,
  double &Fi1,int &X2,int &Y2,int &R2,double &Fi2,int &F,int &Width,
  TColor &ColorK,TColor &ColorF){
 double x,y,r1,r2,f; int Ind; Vary *V;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); x=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) x=Val(P->F); else x=P->Val;
 X1=Round((x-xk)/MSt[0])+Xk; P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); y=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) y=Val(P->F); else y=P->Val;
 Y1=Yk-Round((y-yk)/MSt[1]); P=P->Sled;
 if(P->Nam){ FindVary(P->Nam,&V); r1=Val(V->Znach); }
 else if(P->F.C&&P->F.C!=Pust) r1=Val(P->F); else r1=P->Val;
 r1/=MSt[0]; if(r1<=0) r1=1; R1=Round(r1); P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); x=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) x=Val(P->F); else x=P->Val;
 X2=Round((x-xk)/MSt[0])+Xk; P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); y=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) y=Val(P->F); else y=P->Val;
 Y2=Yk-Round((y-yk)/MSt[1]); P=P->Sled;
 if(P->Nam){ FindVary(P->Nam,&V); r2=Val(V->Znach); }
 else if(P->F.C&&P->F.C!=Pust) r2=Val(P->F); else r2=P->Val;
 r2/=MSt[0]; if(r2<=0) r2=1; R2=Round(r2); P=P->Sled;
 if(P->Nam){ FindVary(P->Nam,&V); f=Val(V->Znach); }
 else if(P->F.C&&P->F.C!=Pust) f=Val(P->F); else f=P->Val;
 F=f>0.0?1:0; P=P->Sled;
 if(P->Nam){ FindVary(P->Nam,&V); Fi1=Val(V->Znach); }
 else if(P->F.C&&P->F.C!=Pust) Fi1=Val(P->F); else Fi1=P->Val;
 Fi2=Fi1*r1/r2; if(F) Fi2*=-1; P=P->Sled;
 Width=Round(P->Nam?FindVary(P->Nam,&V),Val(V->Znach):(P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val);
 if(Width<0) Width=1; P=P->Sled;
 ColorK=(TColor)Round(P->Nam?FindVary(P->Nam,&V),Val(V->Znach):(P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val);
 if(ColorK<0) ColorK=*ColorGraph; P=P->Sled;
 ColorF=(TColor)Round(P->Nam?FindVary(P->Nam,&V),Val(V->Znach):
  P->F.C&&(P->F.C!=Pust) ? Val(P->F) : P->Val);
 P=P->Sled;
 return P;
}
//---------------------------------------------------------------------------
Parm *__fastcall TMultFilms::ParmEllips(Parm *P,int &X,int &Y,int &A,int &B,
  int &Width,TColor &ColorK,TColor &ColorF){
 double x,y,a,b; int Ind; Vary *V;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); x=Vals[Ind]; } else x=P->Val;
 X=Round((x-xk)/MSt[0])+Xk; P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); y=Vals[Ind]; } else y=P->Val;
 Y=Yk-Round((y-yk)/MSt[1]); P=P->Sled;
 if(P->Nam){ FindVary(P->Nam,&V); a=Val(V->Znach); } else a=P->Val;
 a/=MSt[0]; if(a<=0) a=1; A=Round(a); P=P->Sled;
 if(P->Nam){ FindVary(P->Nam,&V); b=Val(V->Znach); } else b=P->Val;
 b/=MSt[0]; if(b<=0) b=1; B=Round(b); P=P->Sled;
 Width=Round(P->Nam?FindVary(P->Nam,&V),Val(V->Znach):(P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val);
 if(Width<0) Width=1; P=P->Sled;
 ColorK=(TColor)Round(P->Nam?FindVary(P->Nam,&V),Val(V->Znach):(P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val);
 if(ColorK<0) ColorK=*ColorGraph; P=P->Sled;
 ColorF=(TColor)Round(P->Nam?FindVary(P->Nam,&V),Val(V->Znach):
  P->F.C&&(P->F.C!=Pust) ? Val(P->F) : P->Val);
 P=P->Sled;
 return P;
}
//---------------------------------------------------------------------------
Parm *__fastcall TMultFilms::GabaritCircle(Parm *P){
 double x,y,r; int Ind;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); x=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) x=Val(P->F); else x=P->Val;
 P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); y=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) y=Val(P->F); else y=P->Val;
 P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); r=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) r=Val(P->F); else r=P->Val;
 if((x+r)>Spr[0]){ Spr[0]=x+r; Spr[1]=y; }
 if((x-r)<Slv[0]){ Slv[0]=x-r; Slv[1]=y; }
 if((y+r)>Max[1]){ Max[1]=y+r; Max[0]=x; }
 if((y-r)<Min[1]){ Min[1]=y-r; Min[0]=x; }
 return P->Sled->Sled->Sled->Sled;
}
//---------------------------------------------------------------------------
Parm *__fastcall TMultFilms::GabaritCircle3d(Parm *P){
 double x,y,r; int Ind;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); x=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) x=Val(P->F); else x=P->Val;
 P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); y=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) y=Val(P->F); else y=P->Val;
 P=P->Sled->Sled->Sled->Sled->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); r=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) r=Val(P->F); else r=P->Val;
 if((x+r)>Spr[0]){ Spr[0]=x+r; Spr[1]=y; }
 if((x-r)<Slv[0]){ Slv[0]=x-r; Slv[1]=y; }
 if((y+r)>Max[1]){ Max[1]=y+r; Max[0]=x; }
 if((y-r)<Min[1]){ Min[1]=y-r; Min[0]=x; }
 return P->Sled->Sled->Sled->Sled;
}
//---------------------------------------------------------------------------
Parm *__fastcall TMultFilms::GabaritKoleso(Parm *P){
 double x,y,r; int Ind;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); x=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) x=Val(P->F); else x=P->Val;
 P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); y=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) y=Val(P->F); else y=P->Val;
 P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); r=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) r=Val(P->F); else r=P->Val;
 if((x+r)>Spr[0]){ Spr[0]=x+r; Spr[1]=y; }
 if((x-r)<Slv[0]){ Slv[0]=x-r; Slv[1]=y; }
 if((y+r)>Max[1]){ Max[1]=y+r; Max[0]=x; }
 if((y-r)<Min[1]){ Min[1]=y-r; Min[0]=x; }
 return P->Sled->Sled->Sled->Sled->Sled;
}
//---------------------------------------------------------------------------
Parm *__fastcall TMultFilms::GabaritVorot(Parm *P){
 double x,y,r,xk,yk,l,al,alr; int Ind;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); x=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) x=Val(P->F); else x=P->Val;
 P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); y=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) y=Val(P->F); else y=P->Val;
 P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); r=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) r=Val(P->F); else r=P->Val;
 if((x+r)>Spr[0]){ Spr[0]=x+r; Spr[1]=y; }
 if((x-r)<Slv[0]){ Slv[0]=x-r; Slv[1]=y; }
 if((y+r)>Max[1]){ Max[1]=y+r; Max[0]=x; }
 if((y-r)<Min[1]){ Min[1]=y-r; Min[0]=x; }
 P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); l=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) l=Val(P->F); else l=P->Val;
 P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); al=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) al=Val(P->F); else al=P->Val;
 alr=al*M_PI/180;
 xk=x-r*sin(alr)-l*cos(alr); yk=y+r*cos(alr)-l*sin(alr);
 if(xk>Spr[0]){ Spr[0]=xk; Spr[1]=yk; }
 if(xk<Slv[0]){ Slv[0]=xk; Slv[1]=yk; }
 if(yk>Max[1]){ Max[1]=yk; Max[0]=xk; }
 if(yk<Min[1]){ Min[1]=yk; Min[0]=xk; }
 return P->Sled->Sled->Sled->Sled;
}
//---------------------------------------------------------------------------
Parm *__fastcall TMultFilms::GabaritGateWithLoad(Parm *P){
 double x,y,r,xC,yC,l0,h,l,al,alr,fi,xK,yK,xN,yN; int Ind;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); x=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) x=Val(P->F); else x=P->Val;
 P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); y=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) y=Val(P->F); else y=P->Val;
 P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); r=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) r=Val(P->F); else r=P->Val;
 if((x+r)>Spr[0]){ Spr[0]=x+r; Spr[1]=y; }
 if((x-r)<Slv[0]){ Slv[0]=x-r; Slv[1]=y; }
 if((y+r)>Max[1]){ Max[1]=y+r; Max[0]=x; }
 if((y-r)<Min[1]){ Min[1]=y-r; Min[0]=x; }
 P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); fi=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) fi=Val(P->F); else fi=P->Val;
 P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); al=Vals[Ind]; }
 else if(P->F.C&&(P->F.C!=Pust))al =Val(P->F);
 else al=P->Val;
 if(al>180) al-=180; else if(al<-180) al+=180; alr=al*M_PI/180;
 P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); l0=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) l0=Val(P->F); else l0=P->Val;
 P=P->Sled; if(al>=0) l0+=fi*r; else l0-=fi*r;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); h=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) h=Val(P->F); else h=P->Val;
 P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); l=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) l=Val(P->F); else l=P->Val;
 if(al>0){ int S=al>90 ? -1 : 1;
  xK=x-r*sin(alr)-(l0+l)*cos(alr)-0.5*S*h*sin(alr);
  yK=y+r*cos(alr)-(l0+l)*sin(alr)+0.5*S*h*cos(alr);
  xN=x-r*sin(alr)-(l0+l)*cos(alr)+0.5*S*h*sin(alr);
  yN=y+r*cos(alr)-(l0+l)*sin(alr)-0.5*S*h*cos(alr);
 }
 else{ int S=al<-90 ? -1 : 1;
  xK=x-r*sin(alr)+(l0+l)*cos(alr)-0.5*S*h*sin(alr);
  yK=y+r*cos(alr)+(l0+l)*sin(alr)+0.5*S*h*cos(alr);
  xN=x-r*sin(alr)+(l0+l)*cos(alr)+0.5*S*h*sin(alr);
  yN=y+r*cos(alr)+(l0+l)*sin(alr)-0.5*S*h*cos(alr);
 }
 if(al>=0&&al<=90||al<=-90){
  if(xK<Slv[0]){ Slv[0]=xK; Slv[1]=yK; }
 }
 else{
  if(xK>Spr[0]){ Spr[0]=xK; Spr[1]=yK; }
 }
 if(yN<Min[1]){ Min[1]=yN; Min[0]=xN; }
 return P->Sled->Sled->Sled->Sled->Sled;
}
//---------------------------------------------------------------------------
Parm *__fastcall TMultFilms::GabaritGear(Parm *P){
 double x1,y1,r1,x2,y2,r2; int Ind;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); x1=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) x1=Val(P->F); else x1=P->Val;
 P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); y1=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) y1=Val(P->F); else y1=P->Val;
 P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); r1=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) r1=Val(P->F); else r1=P->Val;
 P=P->Sled;
 if((x1+r1)>Spr[0]){ Spr[0]=x1+r1; Spr[1]=y1; }
 if((x1-r1)<Slv[0]){ Slv[0]=x1-r1; Slv[1]=y1; }
 if((y1+r1)>Max[1]){ Max[1]=y1+r1; Max[0]=x1; }
 if((y1-r1)<Min[1]){ Min[1]=y1-r1; Min[0]=x1; }
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); x2=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) x2=Val(P->F); else x2=P->Val;
 P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); y2=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) x2=Val(P->F); else y2=P->Val;
 P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); r2=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) r2=Val(P->F); else r2=P->Val;
 if((x2+r2)>Spr[0]){ Spr[0]=x2+r2; Spr[1]=y2; }
 if((x2-r2)<Slv[0]){ Slv[0]=x2-r2; Slv[1]=y2; }
 if((y2+r2)>Max[1]){ Max[1]=y2+r2; Max[0]=x2; }
 if((y2-r2)<Min[1]){ Min[1]=y2-r2; Min[0]=x2; }
 return P->Sled->Sled->Sled->Sled->Sled->Sled;
}
//---------------------------------------------------------------------------
Parm *__fastcall TMultFilms::GabaritEllips(Parm *P){
 double x,y,a,b; int Ind;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); x=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) x=Val(P->F); else x=P->Val;
 P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); y=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) y=Val(P->F); else y=P->Val;
 P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); a=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) a=Val(P->F); else a=P->Val;
 P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); b=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) b=Val(P->F); else b=P->Val;
 if((x+a)>Spr[0]){ Spr[0]=x+a; Spr[1]=y; }
 if((x-a)<Slv[0]){ Slv[0]=x-a; Slv[1]=y; }
 if((y+b)>Max[1]){ Max[1]=y+b; Max[0]=x; }
 if((y-b)<Min[1]){ Min[1]=y-b; Min[0]=x; }
 return P->Sled->Sled->Sled->Sled;
}
//---------------------------------------------------------------------------
Parm *__fastcall TMultFilms::DrawCircle(TCanvas *Canvas,Parm *P){
 int X,Y,R,W; TColor Ck,Cf,Cp=Canvas->Pen->Color,Cb=Canvas->Brush->Color;
 P=ParmCircle(P,X,Y,R,W,Ck,Cf);
 Canvas->Pen->Width=W; Canvas->Pen->Color=Ck;
 Canvas->Brush->Color=Cf>-1?Cf:-1;//bsClear;//*ColorPhon;
 if(Cf<0)
  Canvas->Arc(X-R,Y-R,X+R,Y+R,X+R,0,X+R,0);
 else
  Canvas->Ellipse(X-R,Y-R,X+R,Y+R);
 Canvas->Pen->Width=*WidthPen; Canvas->Pen->Color=Cp; Canvas->Brush->Color=Cb;
 return P;
}
//---------------------------------------------------------------------------
Parm *__fastcall TMultFilms::DrawKoleso(TCanvas *Canvas,Parm *P){
 int X,Y,R,W,X1,Y1,X2,Y2,X3,Y3,X4,Y4; double Fi;
 TColor Ck,Cf,Cp=Canvas->Pen->Color,Cb=Canvas->Brush->Color;
 P=ParmKoleso(P,X,Y,R,Fi,W,Ck,Cf);
 Canvas->Pen->Width=W; Canvas->Pen->Color=Ck;
 Canvas->Brush->Color=Cf>-1?Cf:-1;
 if(Cf<0)
  Canvas->Arc(X-R,Y-R,X+R,Y+R,X+R,0,X+R,0);
 else
  Canvas->Ellipse(X-R,Y-R,X+R,Y+R);
 X1=X+Round(cos(Fi)*R); X3=X-Round(cos(Fi)*R);
 X2=X-Round(sin(Fi)*R); X4=X+Round(sin(Fi)*R);
 Y1=Y-Round(sin(Fi)*R); Y3=Y+Round(sin(Fi)*R);
 Y2=Y-Round(cos(Fi)*R); Y4=Y+Round(cos(Fi)*R);
 Canvas->MoveTo(X1,Y1); Canvas->LineTo(X3,Y3);
 Canvas->MoveTo(X2,Y2); Canvas->LineTo(X4,Y4);
 Canvas->Pen->Width=*WidthPen; Canvas->Pen->Color=Cp; Canvas->Brush->Color=Cb;
 return P;
}
//---------------------------------------------------------------------------
Parm *__fastcall TMultFilms::DrawVorot(TCanvas *Canvas,Parm *P){
 int X,Y,R,W,X1,Y1,X2,Y2,X3,Y3,X4,Y4,X5,Y5,X6,Y6; double L,Al,Alr,Fi;
 TColor Ck,Cf,Cp=Canvas->Pen->Color,Cb=Canvas->Brush->Color;
 P=ParmVorot(P,X,Y,R,L,Al,Fi,W,Ck,Cf); Alr=Al*M_PI/180;
 Canvas->Pen->Width=W; Canvas->Pen->Color=Ck;
 Canvas->Brush->Color=Cf>-1?Cf:-1;
 if(Cf<0)
  Canvas->Arc(X-R,Y-R,X+R,Y+R,X+R,0,X+R,0);
 else
  Canvas->Ellipse(X-R,Y-R,X+R,Y+R);
 X1=X+Round(cos(Fi)*R);  X3=X-Round(cos(Fi)*R);
 X2=X-Round(sin(Fi)*R);  X4=X+Round(sin(Fi)*R);
 Y1=Y-Round(sin(Fi)*R);  Y3=Y+Round(sin(Fi)*R);
 Y2=Y-Round(cos(Fi)*R);  Y4=Y+Round(cos(Fi)*R);
 X5=X-Round(sin(Alr)*R); Y5=Y-Round(cos(Alr)*R);//точка схода нити
 X6=X5-Round(cos(Alr)*L);Y6=Y5+Round(sin(Alr)*L);//конец нити
 Canvas->MoveTo(X1,Y1);  Canvas->LineTo(X3,Y3);
 Canvas->MoveTo(X2,Y2);  Canvas->LineTo(X4,Y4);
 Canvas->MoveTo(X5,Y5);  Canvas->LineTo(X6,Y6);
 Canvas->Pen->Width=*WidthPen; Canvas->Pen->Color=Cp; Canvas->Brush->Color=Cb;
 return P;
}
//--------------------------------------------------------------------------
Parm *__fastcall TMultFilms::DrawGateWithLoad(TCanvas *Canvas,Parm *P){
 int X,Y,R,W,X1,Y1,X2,Y2,X3,Y3,X4,Y4,X5,Y5,X6,Y6,XC,YC,H,D,L;
 double l0,h,l,Al,Alr,Fi;
 TColor Ck,CG,CB,Cp=Canvas->Pen->Color,Cb=Canvas->Brush->Color; TPoint tP[4];
 P=ParmGateWithLoad(P,X,Y,R,Fi,Al,l0,h,l,W,Ck,CG,CB); Alr=Al*M_PI/180;
 Canvas->Pen->Width=W; Canvas->Pen->Color=Ck;
 Canvas->Brush->Color=CG>-1?CG:-1;
 if(CG<0)
  Canvas->Arc(X-R,Y-R,X+R,Y+R,X+R,0,X+R,0);
 else
  Canvas->Ellipse(X-R,Y-R,X+R,Y+R);
//координаты концов спиц
 X1=X+Round(R*cos(Fi));  X3=X-Round(R*cos(Fi));
 X2=X-Round(R*sin(Fi));  X4=X+Round(R*sin(Fi));
 Y1=Y-Round(R*sin(Fi));  Y3=Y+Round(R*sin(Fi));
 Y2=Y-Round(R*cos(Fi));  Y4=Y+Round(R*cos(Fi));
 X5=X-Round(R*sin(Alr)); Y5=Y-Round(R*cos(Alr));//точка схода нити
 H=Round(h/MSt[0]); D=Round(l/MSt[0]);
 if(Al>=0&&Al<=180){
  L=Round(l0/MSt[0])+Fi*R;
  X6=X5-Round(L*cos(Alr)); Y6=Y5+Round(L*sin(Alr));//конец нити
 }
 else if(Al<0&&Al>=-180){
  L=Round(l0/MSt[0])-Fi*R;
  X6=X5+Round(L*cos(Alr)); Y6=Y5-Round(L*sin(Alr));//конец нити
 }
 Canvas->MoveTo(X1,Y1);  Canvas->LineTo(X3,Y3);
 Canvas->MoveTo(X2,Y2);  Canvas->LineTo(X4,Y4);
 Canvas->MoveTo(X5,Y5);  Canvas->LineTo(X6,Y6);
//координаты центра груза
 if(Al>=0&&Al<=180){
  XC=X5-Round((L+0.5*D)*cos(Alr)); YC=Y5+Round((L+0.5*D)*sin(Alr));
 }
 else if(Al<0&&Al>=-180){
  XC=X5+Round((L+0.5*D)*cos(Alr)); YC=Y5-Round((L+0.5*D)*sin(Alr));
 }
 tP[0].x=XC-Round(0.5*(D*cos(Alr)+H*sin(Alr)));
 tP[1].x=XC-Round(0.5*(D*cos(Alr)-H*sin(Alr)));
 tP[2].x=XC+Round(0.5*(D*cos(Alr)+H*sin(Alr)));
 tP[3].x=XC+Round(0.5*(D*cos(Alr)-H*sin(Alr)));
 tP[0].y=YC+Round(0.5*(D*sin(Alr)-H*cos(Alr)));
 tP[1].y=YC+Round(0.5*(D*sin(Alr)+H*cos(Alr)));
 tP[2].y=YC-Round(0.5*(D*sin(Alr)-H*cos(Alr)));
 tP[3].y=YC-Round(0.5*(D*sin(Alr)+H*cos(Alr)));
 Canvas->Brush->Color=CB>-1?CB:-1;
 if(CB<0){
  Canvas->MoveTo(tP[0].x,tP[0].y); Canvas->LineTo(tP[1].x,tP[1].y);
  Canvas->LineTo(tP[2].x,tP[2].y); Canvas->LineTo(tP[3].x,tP[3].y);
  Canvas->LineTo(tP[0].x,tP[0].y);
 }
 else
  Canvas->Polygon(tP,3);
 Canvas->Pen->Width=*WidthPen; Canvas->Pen->Color=Cp; Canvas->Brush->Color=Cb;
 return P;
}
//---------------------------------------------------------------------------
Parm *__fastcall TMultFilms::DrawGear(TCanvas *Canvas,Parm *P){
 int X1,Y1,R1,X2,Y2,R2,W,Xp1,Yp1,Xp2,Yp2,Xp3,Yp3,Xp4,Yp4,F;
 double L,Al,Bt,Fi1,Fi2; TColor Ck,Cf,Cp=Canvas->Pen->Color,Cb=Canvas->Brush->Color;
 P=ParmGear(P,X1,Y1,R1,Fi1,X2,Y2,R2,Fi2,F,W,Ck,Cf);
 Canvas->Pen->Width=W; Canvas->Pen->Color=Ck;
 Canvas->Brush->Color=Cf>-1?Cf:-1;
 if(Cf<0)
  Canvas->Arc(X1-R1,Y1-R1,X1+R1,Y1+R1,X1+R1,0,X1+R1,0);
 else
  Canvas->Ellipse(X1-R1,Y1-R1,X1+R1,Y1+R1);
 if(Cf<0) Canvas->Arc(X2-R2,Y2-R2,X2+R2,Y2+R2,X2+R2,0,X2+R2,0);
 else Canvas->Ellipse(X2-R2,Y2-R2,X2+R2,Y2+R2);
 L=sqrt((X2-X1)*(X2-X1)+(Y2-Y1)*(Y2-Y1));
 Bt=atan2(Y2-Y1,X2-X1);
 if(F){
  Al=asin((R1+R2)/L);
  Xp1=X1-Round(sin(Bt-Al)*R1); Yp1=Y1-Round(cos(Bt-Al)*R1);
  Xp2=X2+Round(sin(Bt-Al)*R2); Yp2=Y2+Round(cos(Bt-Al)*R2);
  Xp3=X1+Round(sin(Al+Bt)*R1); Yp3=Y1+Round(cos(Al+Bt)*R1);
  Xp4=X2-Round(sin(Al+Bt)*R2); Yp4=Y2-Round(cos(Al+Bt)*R2);
 }
 else{
  Al=atan((R2-R1)/L);
  Xp1=X1-Round(sin(Al+Bt)*R1); Yp1=Y1-Round(cos(Al+Bt)*R1);
  Xp2=X2-Round(sin(Al+Bt)*R2); Yp2=Y2-Round(cos(Al+Bt)*R2);
  Xp3=X1+Round(sin(Bt-Al)*R1); Yp3=Y1+Round(cos(Bt-Al)*R1);
  Xp4=X2+Round(sin(Bt-Al)*R2); Yp4=Y2+Round(cos(Bt-Al)*R2);
 }
 Canvas->MoveTo(Xp1,Yp1); Canvas->LineTo(Xp2,Yp2);
 Canvas->MoveTo(Xp3,Yp3); Canvas->LineTo(Xp4,Yp4);
 Xp1=X1+Round(cos(Fi1)*R1); Xp3=X1-Round(cos(Fi1)*R1);
 Xp2=X1-Round(sin(Fi1)*R1); Xp4=X1+Round(sin(Fi1)*R1);
 Yp1=Y1-Round(sin(Fi1)*R1); Yp3=Y1+Round(sin(Fi1)*R1);
 Yp2=Y1-Round(cos(Fi1)*R1); Yp4=Y1+Round(cos(Fi1)*R1);
 Canvas->MoveTo(Xp1,Yp1); Canvas->LineTo(Xp3,Yp3);
 Canvas->MoveTo(Xp2,Yp2); Canvas->LineTo(Xp4,Yp4);
 Xp1=X2+Round(cos(Fi2)*R2); Xp3=X2-Round(cos(Fi2)*R2);
 Xp2=X2-Round(sin(Fi2)*R2); Xp4=X2+Round(sin(Fi2)*R2);
 Yp1=Y2-Round(sin(Fi2)*R2); Yp3=Y2+Round(sin(Fi2)*R2);
 Yp2=Y2-Round(cos(Fi2)*R2); Yp4=Y2+Round(cos(Fi2)*R2);
 Canvas->MoveTo(Xp1,Yp1); Canvas->LineTo(Xp3,Yp3);
 Canvas->MoveTo(Xp2,Yp2); Canvas->LineTo(Xp4,Yp4);
 Canvas->Pen->Width=*WidthPen; Canvas->Pen->Color=Cp; Canvas->Brush->Color=Cb;
 return P;
}
//---------------------------------------------------------------------------
Parm *__fastcall TMultFilms::DrawEllips(TCanvas *Canvas,Parm *P){
 int X,Y,A,B,W; TColor Ck,Cf,Cp=Canvas->Pen->Color,Cb=Canvas->Brush->Color;
 P=ParmEllips(P,X,Y,A,B,W,Ck,Cf);
 Canvas->Pen->Width=W; Canvas->Pen->Color=Ck;
 Canvas->Brush->Color=Cf>-1?Cf:-1;//bsClear;//*ColorPhon;
 if(Cf<0) Canvas->Arc(X-A,Y-B,X+A,Y+B,X+A,0,X+A,0);
 else Canvas->Ellipse(X-A,Y-B,X+A,Y+B);
 Canvas->Pen->Width=*WidthPen; Canvas->Pen->Color=Cp; Canvas->Brush->Color=Cb;
 return P;
}
//---------------------------------------------------------------------------
/*
Parm *__fastcall TMultFilms::ClearCircle(TCanvas *Canvas,Parm *P){
 int X,Y,R,W; TColor Ck,Cf;
 if(R==0) R=1;
 P=ParmCircle(P,X,Y,R,W,Ck,Cf);
 Canvas->Pen->Width=W; Canvas->Pen->Color=*ColorPhon;
 Canvas->Brush->Color=*ColorPhon;
 Canvas->Ellipse(X-R,Y-R,X+R,Y+R);
 Canvas->Pen->Width=*WidthPen;
 return P;
}*/
//---------------------------------------------------------------------------
Parm *__fastcall TMultFilms::GabaritVector(Parm *P){
 return GabaritLine(P);
}
//---------------------------------------------------------------------------
Parm *__fastcall TMultFilms::GabaritFinger(Parm *P){
 double xA,yA,dx,dy,xB,yB,mu; int Ind;
 xA=P->Nam?Vals[NStrMagCmp(NamesVarys,P->Nam)]:(P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val;
 P=P->Sled;
 yA=P->Nam?Vals[NStrMagCmp(NamesVarys,P->Nam)]:(P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val;
 P=P->Sled;
 dx=P->Nam?Vals[NStrMagCmp(NamesVarys,P->Nam)]:(P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val;
 P=P->Sled;
 dy=P->Nam?Vals[NStrMagCmp(NamesVarys,P->Nam)]:(P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val;
 P=P->Sled;
 mu=P->Nam?Vals[NStrMagCmp(NamesVarys,P->Nam)]:(P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val;
 P=P->Sled; P=P->Sled; P=P->Sled; xB=xA+mu*dx; yB=yA+mu*dy;
 if(xA>Spr[0]){ Spr[0]=xA; Spr[1]=yA; }
 if(xA<Slv[0]){ Slv[0]=xA; Slv[1]=yA; }
 if(xB>Spr[0]){ Spr[0]=xB; Spr[1]=yB; }
 if(xB<Slv[0]){ Slv[0]=xB; Slv[1]=yB; }
 if(yA>Max[1]){ Max[1]=yA; Max[0]=xA; }
 if(yA<Min[1]){ Min[1]=yA; Min[0]=xA; }
 if(yB>Max[1]){ Max[1]=yB; Max[0]=xB; }
 if(yB<Min[1]){ Min[1]=yB; Min[0]=xB; }
 return P;
}
//---------------------------------------------------------------------------
Parm *__fastcall TMultFilms::GabaritSpeed(Parm *P){
 double xA,yA,dx,dy,xB,yB,mu; int Ind; wchar_t Speed[256]; Vary *V; Form F;
 xA=Vals[NStrMagCmp(NamesVarys,P->Nam)];
 swprintf(Speed,L"%s'%s",P->Nam,L->Time->Name);
 if(FindVary(Speed,&V)){ F.V=V; dx=Val(F); } else dx=0; P=P->Sled;
 yA=Vals[NStrMagCmp(NamesVarys,P->Nam)];
 swprintf(Speed,L"%s'%s",P->Nam,L->Time->Name);
 if(FindVary(Speed,&V)){ F.V=V; dy=Val(F); } else dy=0; P=P->Sled;
 mu=P->Nam?Vals[NStrMagCmp(NamesVarys,P->Nam)]:(P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val;
 P=P->Sled; P=P->Sled; P=P->Sled; xB=xA+mu*dx; yB=yA+mu*dy;
 if(xA>Spr[0]){ Spr[0]=xA; Spr[1]=yA; }
 if(xA<Slv[0]){ Slv[0]=xA; Slv[1]=yA; }
 if(xB>Spr[0]){ Spr[0]=xB; Spr[1]=yB; }
 if(xB<Slv[0]){ Slv[0]=xB; Slv[1]=yB; }
 if(yA>Max[1]){ Max[1]=yA; Max[0]=xA; }
 if(yA<Min[1]){ Min[1]=yA; Min[0]=xA; }
 if(yB>Max[1]){ Max[1]=yB; Max[0]=xB; }
 if(yB<Min[1]){ Min[1]=yB; Min[0]=xB; }
 return P;
}
//---------------------------------------------------------------------------
Parm *__fastcall TMultFilms::GabaritAtang(Parm *P){
 double xA,yA,xt,yt,xtt,ytt,atV,xB,yB,mu; int Ind; wchar_t Speed[256],Accel[256];
 Vary *V; Form F;
 xA=Vals[NStrMagCmp(NamesVarys,P->Nam)];
 swprintf(Speed,L"%s'%s",P->Nam,L->Time->Name);
 if(FindVary(Speed,&V)){ F.V=V; xt=Val(F); } else xt=0.0;
 swprintf(Accel,L"%s'%s",Speed,L->Time->Name);
 if(FindVary(Accel,&V)){ F.V=V; xtt=Val(F); } else xtt=0.0;
 P=P->Sled;
 yA=Vals[NStrMagCmp(NamesVarys,P->Nam)];
 swprintf(Speed,L"%s'%s",P->Nam,L->Time->Name);
 if(FindVary(Speed,&V)){ F.V=V; yt=Val(F); } else yt=0.0;
 swprintf(Accel,L"%s'%s",Speed,L->Time->Name);
 if(FindVary(Accel,&V)){ F.V=V; ytt=Val(F); } else ytt=0.0;
 P=P->Sled;
 mu=P->Nam?Vals[NStrMagCmp(NamesVarys,P->Nam)]:(P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val;
 P=P->Sled;
 P=P->Sled; P=P->Sled;
 atV=mu*(xt*xtt+yt*ytt)/(xt*xt+yt*yt); xB=xA+xt*atV; yB=yA+yt*atV;
 if(xA>Spr[0]){ Spr[0]=xA; Spr[1]=yA; }
 if(xA<Slv[0]){ Slv[0]=xA; Slv[1]=yA; }
 if(xB>Spr[0]){ Spr[0]=xB; Spr[1]=yB; }
 if(xB<Slv[0]){ Slv[0]=xB; Slv[1]=yB; }
 if(yA>Max[1]){ Max[1]=yA; Max[0]=xA; }
 if(yA<Min[1]){ Min[1]=yA; Min[0]=xA; }
 if(yB>Max[1]){ Max[1]=yB; Max[0]=xB; }
 if(yB<Min[1]){ Min[1]=yB; Min[0]=xB; }
 return P;
}
//---------------------------------------------------------------------------
Parm *__fastcall TMultFilms::GabaritAnorm(Parm *P){
 double xA,yA,xt,yt,xtt,ytt,atV,xB,yB,mu; int Ind; wchar_t Speed[256],Accel[256];
 Vary *V; Form F;
 xA=Vals[NStrMagCmp(NamesVarys,P->Nam)];
 swprintf(Speed,L"%s'%s",P->Nam,L->Time->Name);
 if(FindVary(Speed,&V)){ F.V=V; xt=Val(F); } else xt=0.0;
 swprintf(Accel,L"%s'%s",Speed,L->Time->Name);
 if(FindVary(Accel,&V)){ F.V=V; xtt=Val(F); } else xtt=0.0;
 P=P->Sled;
 yA=Vals[NStrMagCmp(NamesVarys,P->Nam)];
 swprintf(Speed,L"%s'%s",P->Nam,L->Time->Name);
 if(FindVary(Speed,&V)){ F.V=V; yt=Val(F); } else yt=0.0;
 swprintf(Accel,L"%s'%s",Speed,L->Time->Name);
 if(FindVary(Accel,&V)){ F.V=V; ytt=Val(F); } else ytt=0.0;
 P=P->Sled;
 mu=P->Nam?Vals[NStrMagCmp(NamesVarys,P->Nam)]:(P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val;
 P=P->Sled; P=P->Sled; P=P->Sled;
 atV=(xt*xtt+yt*ytt)/(xt*xt+yt*yt); xB=xA+mu*(xtt-xt*atV); yB=yA+mu*(ytt-yt*atV);
 if(xA>Spr[0]){ Spr[0]=xA; Spr[1]=yA; }
 if(xA<Slv[0]){ Slv[0]=xA; Slv[1]=yA; }
 if(xB>Spr[0]){ Spr[0]=xB; Spr[1]=yB; }
 if(xB<Slv[0]){ Slv[0]=xB; Slv[1]=yB; }
 if(yA>Max[1]){ Max[1]=yA; Max[0]=xA; }
 if(yA<Min[1]){ Min[1]=yA; Min[0]=xA; }
 if(yB>Max[1]){ Max[1]=yB; Max[0]=xB; }
 if(yB<Min[1]){ Min[1]=yB; Min[0]=xB; }
 return P;
}
//---------------------------------------------------------------------------
Parm *__fastcall TMultFilms::GabaritLine(Parm *P){
 double xA,yA,xB,yB,x,y; int Ind;
 xA=P->Nam?Vals[NStrMagCmp(NamesVarys,P->Nam)]:P->F.C&&(P->F.C!=Pust)?Val(P->F):P->Val;
 P=P->Sled;
 yA=P->Nam?Vals[NStrMagCmp(NamesVarys,P->Nam)]:P->F.C&&(P->F.C!=Pust)?Val(P->F):P->Val;
 P=P->Sled;
 xB=P->Nam?Vals[NStrMagCmp(NamesVarys,P->Nam)]:P->F.C&&(P->F.C!=Pust)?Val(P->F):P->Val;
 P=P->Sled;
 yB=P->Nam?Vals[NStrMagCmp(NamesVarys,P->Nam)]:P->F.C&&(P->F.C!=Pust)?Val(P->F):P->Val;
 P=P->Sled; P=P->Sled; P=P->Sled;
 if(xA>Spr[0]){ Spr[0]=xA; Spr[1]=yA; }
 if(xA<Slv[0]){ Slv[0]=xA; Slv[1]=yA; }
 if(xB>Spr[0]){ Spr[0]=xB; Spr[1]=yB; }
 if(xB<Slv[0]){ Slv[0]=xB; Slv[1]=yB; }
 if(yA>Max[1]){ Max[1]=yA; Max[0]=xA; }
 if(yA<Min[1]){ Min[1]=yA; Min[0]=xA; }
 if(yB>Max[1]){ Max[1]=yB; Max[0]=xB; }
 if(yB<Min[1]){ Min[1]=yB; Min[0]=xB; }
 return P;
}
//---------------------------------------------------------------------------
Parm *__fastcall TMultFilms::GabaritString(Parm *P){
 double xA,yA,xB,yB,l,r,alfa;
 xA=P->Nam?Vals[NStrMagCmp(NamesVarys,P->Nam)]:(P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val;
 P=P->Sled;
 yA=P->Nam?Vals[NStrMagCmp(NamesVarys,P->Nam)]:(P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val;
 P=P->Sled;
 l=P->Nam?Vals[NStrMagCmp(NamesVarys,P->Nam)]:(P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val;
 P=P->Sled;
 alfa=P->Nam?Vals[NStrMagCmp(NamesVarys,P->Nam)]:(P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val;
 P=P->Sled;
 r=P->Nam?Vals[NStrMagCmp(NamesVarys,P->Nam)]:(P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val;
 P=P->Sled; P=P->Sled; P=P->Sled; P=P->Sled;
 xB=xA+l*cos(alfa*M_PI/180.0); yB=yA+l*sin(alfa*M_PI/180.0);
 if((xA+r)>Spr[0]){ Spr[0]=xA+r; Spr[1]=yA; }
 if((xA-r)<Slv[0]){ Slv[0]=xA-r; Slv[1]=yA; }
 if((xB+r)>Spr[0]){ Spr[0]=xB+r; Spr[1]=yB; }
 if((xB-r)<Slv[0]){ Slv[0]=xB-r; Slv[1]=yB; }
 if((yA+r)>Max[1]){ Max[1]=yA+r; Max[0]=xA; }
 if((yA-r)<Min[1]){ Min[1]=yA-r; Min[0]=xA; }
 if((yB+r)>Max[1]){ Max[1]=yB+r; Max[0]=xB; }
 if((yB-r)<Min[1]){ Min[1]=yB-r; Min[0]=xB; }
 return P;
}
//---------------------------------------------------------------------------
Parm *__fastcall TMultFilms::GabaritMultiLink(Parm *P){
 Vary *V; double r,k,x,y; int Ind,K;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); r=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) r=Val(P->F); else r=P->Val;
 P=P->Sled; P=P->Sled; P=P->Sled; P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); k=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) k=Val(P->F); else k=P->Val;
 P=P->Sled; K=Round(k);
 for(int i=0;i<K+1;i++){
  if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); x=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) x=Val(P->F); else x=P->Val;
  P=P->Sled;
  if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); y=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) y=Val(P->F); else y=P->Val;
  P=P->Sled;
  if((x+r)>Spr[0]){ Spr[0]=x+r; Spr[1]=y; }
  if((x-r)<Slv[0]){ Slv[0]=x-r; Slv[1]=y; }
  if((y+r)>Max[1]){ Max[1]=y+r; Max[0]=x; }
  if((y-r)<Min[1]){ Min[1]=y-r; Min[0]=x; }
 }
 return P;
}
//---------------------------------------------------------------------------
Parm *__fastcall TMultFilms::GabaritManipulator3d(Parm *P){
 Vary *V; double r,k,x,y; int Ind,K,w; long C1,C2; Parm **Pml;
 wchar_t Bx,By,NameX[16],NameY[16],Buf[16];
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); r=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) r=Val(P->F); else r=P->Val;
 P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); w=Round(Vals[Ind]); }
 else if(P->F.C&&P->F.C!=Pust) w=Round(Val(P->F)); else w=Round(P->Val);
 P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); C1=Round(Vals[Ind]); }
 else if(P->F.C&&P->F.C!=Pust) C1=Round(Val(P->F)); else C1=Round(P->Val);
 P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); C2=Round(Vals[Ind]); }
 else if(P->F.C&&P->F.C!=Pust) C2=Round(Val(P->F)); else C2=Round(P->Val);
 P=P->Sled;
 K=KolElem(P->Sled->Sled);
 Pml=(Parm **)calloc(2*K+7,sizeof(Parm *));
 for(int i=0;i<2*K+7;i++) Pml[i]=(Parm *)calloc(1,sizeof(Parm));
 for(int i=1;i<2*K+7;i++) Pml[i-1]->Sled=Pml[i];
 Pml[0]->Val=r; Pml[1]->Val=w; Pml[2]->Val=C1; Pml[3]->Val=C2; Pml[4]->Val=K;
 if(P->Nam) Bx=P->Nam[0]; wcscpy(NameX,P->Nam);
 Pml[5]->Nam=P->Nam;
 P=P->Sled;
 if(P->Nam) By=P->Nam[0]; wcscpy(NameY,P->Nam);
 Pml[6]->Nam=P->Nam;
 P=P->Sled;
 for(int i=0;i<K;i++,P=P->Sled){
  swprintf(Buf,L"%cO@%s",Bx,P->Nam);
  FindVary(Buf,&V);
  Pml[7+i*2]->Nam=V->Name;
  swprintf(Buf,L"%cO@%s",By,P->Nam);
  FindVary(Buf,&V);
  Pml[7+i*2+1]->Nam=V->Name;
 }
 GabaritMultiLink(Pml[0]);
 for(int i=0;i<2*K+7;i++) free(Pml[i]); free(Pml);
 return P;
}
//---------------------------------------------------------------------------
Parm *__fastcall TMultFilms::GabaritPolygon3d(Parm *P){
 Vary *V; double k,x,y; int Ind,K;
 P=P->Sled; P=P->Sled; P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); k=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) k=Val(P->F); else k=P->Val;
 K=Round(k);
 P=P->Sled; P=P->Sled;
 for(int i=0;i<K;i++){
  if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); x=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) x=Val(P->F); else x=P->Val;
  P=P->Sled;
  if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); y=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) y=Val(P->F); else y=P->Val;
  P=P->Sled;
  if(x>Spr[0]){ Spr[0]=x; Spr[1]=y; }
  if(x<Slv[0]){ Slv[0]=x; Slv[1]=y; }
  if(y>Max[1]){ Max[1]=y; Max[0]=x; }
  if(y<Min[1]){ Min[1]=y; Min[0]=x; }
 }
 return P;
}
//---------------------------------------------------------------------------
Parm *__fastcall TMultFilms::GabaritSpline(Parm *P){
//name, x0, xk,w, c, n, x1, y1,...,xn, yn
 Spln *Sp; wchar_t *Name=P->Nam; Vary *V; double n,xn,yn,x,y,dx; int i,Ind,N;
 P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); xn=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) xn=Val(P->F); else xn=P->Val;
 P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); xk=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) xk=Val(P->F); else xk=P->Val;
 P=P->Sled; P=P->Sled; P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); n=Vals[Ind]; }
 else if(P->F.C&&P->F.C!=Pust) n=Val(P->F); else n=P->Val;
 N=Round(n); P=P->Sled;
 if(FindSpln(Rsp,Name,&Sp)){
  for(int i=0;i<2*N;i++,P=P->Sled){
   if(P->Nam){
    if(FindVary(P->Nam,&V)) Sp->A->A[i]=V->Val;
   }
   else if(P->F.C&&P->F.C!=Pust) Sp->A->A[i]=Val(P->F);
   else Sp->A->A[i]=P->Val;
  }
  Sp->CalcSpln();
  dx=(xk-xn)/100;
  for(i=0,x=xn;i<101;i++){
   x=xn+i*dx; y=Spl(Sp,x);
   if(x>Spr[0]){ Spr[0]=x; Spr[1]=y; } if(x<Slv[0]){ Slv[0]=x; Slv[1]=y; }
   if(y>Max[1]){ Max[1]=y; Max[0]=x; } if(y<Min[1]){ Min[1]=y; Min[0]=x; }
 }}
 else for(i=0;i<2*N;i++) P=P->Sled;
 return P;
}
//---------------------------------------------------------------------------
Parm *__fastcall TMultFilms::GabaritHinge(Parm *P,bool Living){ int i,sh;
 double xD,yD,xE,yE,xF,yF,xG,yG,xH,yH,x[10],y[10],h,r,alfa,
  XO,YO,XE,YE,XF,YF,X[10],Y[10],XG,YG,XH,YH;
 XO=P->Nam?Vals[NStrMagCmp(NamesVarys,P->Nam)]:(P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val;
 P=P->Sled;
 YO=P->Nam?Vals[NStrMagCmp(NamesVarys,P->Nam)]:(P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val;
 P=P->Sled;
 r=P->Nam?Vals[NStrMagCmp(NamesVarys,P->Nam)]:(P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val;
 P=P->Sled;
 h=P->Nam?Vals[NStrMagCmp(NamesVarys,P->Nam)]:(P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val;
 P=P->Sled;
 alfa=P->Nam?Vals[NStrMagCmp(NamesVarys,P->Nam)]:(P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val;
 P=P->Sled;
 sh=P->Nam?Vals[NStrMagCmp(NamesVarys,P->Nam)]:(P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val;
 P=P->Sled; P=P->Sled; P=P->Sled; P=P->Sled; alfa*=M_PI/180.0;
 yE=yF=-h; if(Living) yE=yF=-h-2*r;
 if(sh){
  xE=xG=-h; xF=xH=h; yG=yH=yE-h/2;
//  for(i=0;i<10;i++) x[i]=xE+i*2*h/9;
//  for(i=0;i<5;i++){ y[2*i]=yE-0.5*h; y[2*i+1]=yE; }
  XE=XO+xE*cos(alfa)-yE*sin(alfa); YE=YO+xE*sin(alfa)+yE*cos(alfa);
  XF=XO+xF*cos(alfa)-yF*sin(alfa); YF=YO+xF*sin(alfa)+yF*cos(alfa);
  XG=XO+xG*cos(alfa)-yG*sin(alfa); YG=YO+xG*sin(alfa)+yG*cos(alfa);
  XH=XO+xH*cos(alfa)-yH*sin(alfa); YH=YO+xH*sin(alfa)+yH*cos(alfa);
//  for(i=0;i<10;i++){
//   X[i]=xO+x[i]*cos(alfa)-y[i]*sin(alfa);
//   Y[i]=yO+x[i]*sin(alfa)+y[i]*cos(alfa);
 }//}
 else{
  xE=-0.5*h; xF=0.5*h;
  XE=XO+xE*cos(alfa)-yE*sin(alfa); YE=YO+xE*sin(alfa)+yE*cos(alfa);
  XF=XO+xF*cos(alfa)-yF*sin(alfa); YF=YO+xF*sin(alfa)+yF*cos(alfa);
 }
 if((XO+r)>Spr[0]){ Spr[0]=XO+r; Spr[1]=YO; }
 if((XO-r)<Slv[0]){ Slv[0]=XO-r; Slv[1]=YO; }
 if((YO+r)>Max[1]){ Max[1]=YO+r; Max[0]=XO; }
 if((YO-r)<Min[1]){ Min[1]=YO-r; Min[0]=XO; }
 if(XE>Spr[0]){ Spr[0]=XE; Spr[1]=YE; }
 if(XF<Slv[0]){ Slv[0]=XF; Slv[1]=YF; }
 if(YE>Max[1]){ Max[1]=YE; Max[0]=XE; }
 if(YE<Min[1]){ Min[1]=YE; Min[0]=XE; }
 if(YF>Max[1]){ Max[1]=YF; Max[0]=XF; }
 if(YF<Min[1]){ Min[1]=YF; Min[0]=XF; }
 if(sh){
  if(XG>Spr[0]){ Spr[0]=XG; Spr[1]=YG; }
  if(XH>Spr[0]){ Spr[0]=XH; Spr[1]=YH; }
  if(XG<Slv[0]){ Slv[0]=XG; Slv[1]=YG; }
  if(XH<Slv[0]){ Slv[0]=XH; Slv[1]=YH; }
  if(YG>Max[1]){ Max[1]=YG; Max[0]=XG; }
  if(YG<Min[1]){ Min[1]=YG; Min[0]=XG; }
  if(YH>Max[1]){ Max[1]=YH; Max[0]=XH; }
  if(YH<Min[1]){ Min[1]=YH; Min[0]=XH; }
 }
 return P;
}
//---------------------------------------------------------------------------
void A_B_C(double xA,double yA,double a1,double xB,double yB,double a2,
  double &a,double &b,double &c,double &Ca,double &Sa,double &L){
 L=sqrt((xA-xB)*(xA-xB)+(yA-yB)*(yA-yB));
 if(L==0.0){ a=b=c=0.0; return; }
 Ca=(xB-xA)/L; Sa=(yB-yA)/L;
 a=atan2(Sa,Ca);
 c=tan(a1-a);
 b=-(2*c+tan(a2-a))/L;
 a=-(b+c/L)/L;
}
//---------------------------------------------------------------------------
Parm *__fastcall TMultFilms::GabaritRejka(Parm *P){
 double L,Ca,Sa,xA,yA,a1,xB,yB,a2,x,y,a,b,c,dksi,ksi=0.0,eta; int Ind;
 xA=P->Nam?Vals[NStrMagCmp(NamesVarys,P->Nam)]:(P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val;
 P=P->Sled;
 yA=P->Nam?Vals[NStrMagCmp(NamesVarys,P->Nam)]:(P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val;
 P=P->Sled;
 a1=P->Nam?Vals[NStrMagCmp(NamesVarys,P->Nam)]:(P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val;
 P=P->Sled;
 xB=P->Nam?Vals[NStrMagCmp(NamesVarys,P->Nam)]:(P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val;
 P=P->Sled;
 yB=P->Nam?Vals[NStrMagCmp(NamesVarys,P->Nam)]:(P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val;
 P=P->Sled;
 a2=P->Nam?Vals[NStrMagCmp(NamesVarys,P->Nam)]:(P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val;
 P=P->Sled; P=P->Sled; P=P->Sled;
 A_B_C(xA,yA,a1,xB,yB,a2,a,b,c,Ca,Sa,L); dksi=L/100;
 for(int i=0;i<100;i++){
  eta=ksi*(a*ksi*ksi+b*ksi+c);
  x=xA+Ca*ksi-Sa*eta; y=yA+Sa*ksi+Ca*eta;
  if(x>Spr[0]){ Spr[0]=x; Spr[1]=y; } if(x<Slv[0]){ Slv[0]=x; Slv[1]=y; }
  if(y>Max[1]){ Max[1]=y; Max[0]=x; } if(y<Min[1]){ Min[1]=y; Min[0]=x; }
  ksi+=dksi;
 }
 return P;
}
//---------------------------------------------------------------------------
Parm *__fastcall TMultFilms::ParmLine(Parm *P,int &XA,int &YA,int &XB,int &YB,
  int &Width,TColor &Color){ Vary *V;
 double xA,yA,xB,yB; int Ind;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); xA=Vals[Ind]; }
 else if(P->F.C&&(P->F.C!=Pust)) xA=Val(P->F); else xA=P->Val;
 P=P->Sled; XA=Round((xA-xk)/MSt[0])+Xk;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); yA=Vals[Ind]; }
 else if(P->F.C&&(P->F.C!=Pust)) yA=Val(P->F); else yA=P->Val;
 P=P->Sled; YA=Yk-Round((yA-yk)/MSt[1]);
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); xB=Vals[Ind]; }
 else if(P->F.C&&(P->F.C!=Pust)) xB=Val(P->F); else xB=P->Val;
 P=P->Sled; XB=Round((xB-xk)/MSt[0])+Xk;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); yB=Vals[Ind]; }
 else if(P->F.C&&(P->F.C!=Pust)) yB=Val(P->F); else yB=P->Val;
 P=P->Sled; YB=Yk-Round((yB-yk)/MSt[1]);
 Width=Round(P->Nam?FindVary(P->Nam,&V),Val(V->Znach):
      (P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val);
 if(Width<0) Width=0; P=P->Sled;
 Color=(TColor)Round(P->Nam?FindVary(P->Nam,&V),Val(V->Znach):
  P->F.C&&(P->F.C!=Pust)?Val(P->F):P->Val);
// if(Color<0) Color=*ColorGraph;
 P=P->Sled;
 return P;
}
//---------------------------------------------------------------------------
Parm *__fastcall TMultFilms::ParmFinger(Parm *P,int &XA,int &YA,int &XB,int &YB,
  int &Width,TColor &Color){ Vary *V;
 double xA,yA,dx,dy,mu,xB,yB; int Ind;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); xA=Vals[Ind]; }
 else if(P->F.C&&(P->F.C!=Pust)) xA=Val(P->F);  else xA=P->Val;
 P=P->Sled; XA=Round((xA-xk)/MSt[0])+Xk;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); yA=Vals[Ind]; }
 else if(P->F.C&&(P->F.C!=Pust)) yA=Val(P->F);  else yA=P->Val;
 P=P->Sled; YA=Yk-Round((yA-yk)/MSt[1]);
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); dx=Vals[Ind]; }
 else if(P->F.C&&(P->F.C!=Pust)) dx=Val(P->F);  else dx=P->Val;
 P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); dy=Vals[Ind]; }
 else if(P->F.C&&(P->F.C!=Pust)) dy=Val(P->F);  else dy=P->Val;
 P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); mu=Vals[Ind]; }
 else if(P->F.C&&(P->F.C!=Pust)) mu=Val(P->F);  else mu=P->Val;
 P=P->Sled;
 xB=xA+mu*dx; XB=Round((xB-xk)/MSt[0])+Xk;
 yB=yA+mu*dy; YB=Yk-Round((yB-yk)/MSt[1]);
 Width=Round(P->Nam?FindVary(P->Nam,&V),Val(V->Znach):
       (P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val);
 if(Width<0) Width=0; P=P->Sled;
 Color=(TColor)Round(P->Nam?FindVary(P->Nam,&V),Val(V->Znach):
                (P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val);
 P=P->Sled;
 return P;
}
//---------------------------------------------------------------------------
Parm *__fastcall TMultFilms::ParmSpeed(Parm *P,int &XA,int &YA,int &XB,int &YB,
  int &Width,TColor &Color){
 double xA,yA,dx,dy,mu,xB,yB; int Ind; wchar_t Speed[256]; Vary *V; Form F;
 xA=Vals[NStrMagCmp(NamesVarys,P->Nam)];
 swprintf(Speed,L"%s'%s",P->Nam,L->Time->Name);
 if(FindVary(Speed,&V)){ F.V=V; dx=Val(F); } else dx=0.0;
 XA=Round((xA-xk)/MSt[0])+Xk; P=P->Sled;
 yA=Vals[NStrMagCmp(NamesVarys,P->Nam)];
 swprintf(Speed,L"%s'%s",P->Nam,L->Time->Name);
 if(FindVary(Speed,&V)){ F.V=V; dy=Val(F); } else dy=0.0;
 YA=Yk-Round((yA-yk)/MSt[1]); P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); mu=Vals[Ind]; }
 else if(P->F.C&&(P->F.C!=Pust)) mu=Val(P->F);  else mu=P->Val;
 P=P->Sled;
 xB=xA+mu*dx; XB=Round((xB-xk)/MSt[0])+Xk;
 yB=yA+mu*dy; YB=Yk-Round((yB-yk)/MSt[1]);
 Width=Round(P->Nam?FindVary(P->Nam,&V),Val(V->Znach):
        (P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val);
 if(Width<0) Width=0; P=P->Sled;
 Color=(TColor)Round(P->Nam?FindVary(P->Nam,&V),Val(V->Znach):
                (P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val);
 P=P->Sled;
 return P;
}
//---------------------------------------------------------------------------
Parm *__fastcall TMultFilms::ParmAtang(Parm *P,int &XA,int &YA,int &XB,int &YB,
  int &Width,TColor &Color){
 double xA,yA,xt,yt,xtt,ytt,atV,xB,yB,mu; int Ind; wchar_t Speed[256],Accel[256];
 Vary *V; Form F;
 xA=Vals[NStrMagCmp(NamesVarys,P->Nam)];
 swprintf(Speed,L"%s'%s",P->Nam,L->Time->Name);
 if(FindVary(Speed,&V)){ F.V=V; xt=Val(F); } else xt=0.0;
 swprintf(Accel,L"%s'%s",Speed,L->Time->Name);
 if(FindVary(Accel,&V)){ F.V=V; xtt=Val(F); } else xtt=0.0;
 P=P->Sled;
 yA=Vals[NStrMagCmp(NamesVarys,P->Nam)];
 swprintf(Speed,L"%s'%s",P->Nam,L->Time->Name);
 if(FindVary(Speed,&V)){ F.V=V; yt=Val(F); } else yt=0.0;
 swprintf(Accel,L"%s'%s",Speed,L->Time->Name);
 if(FindVary(Accel,&V)){ F.V=V; ytt=Val(F); } else ytt=0.0;
 P=P->Sled;
 mu=P->Nam?Vals[NStrMagCmp(NamesVarys,P->Nam)]:
    (P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val;
 P=P->Sled;
 atV=mu*(xt*xtt+yt*ytt)/(xt*xt+yt*yt); xB=xA+xt*atV; yB=yA+yt*atV;
 XA=Round((xA-xk)/MSt[0])+Xk; YA=Yk-Round((yA-yk)/MSt[1]);
 XB=Round((xB-xk)/MSt[0])+Xk; YB=Yk-Round((yB-yk)/MSt[1]);
 Width=Round(P->Nam?FindVary(P->Nam,&V),Val(V->Znach):
       (P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val);
 if(Width<0) Width=0; P=P->Sled;
 Color=(TColor)Round(P->Nam?FindVary(P->Nam,&V),Val(V->Znach):
                (P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val);
 P=P->Sled;
 return P;
}
//---------------------------------------------------------------------------
Parm *__fastcall TMultFilms::ParmAnorm(Parm *P,int &XA,int &YA,int &XB,int &YB,
  int &Width,TColor &Color){
 double xA,yA,xt,yt,xtt,ytt,atV,xB,yB,mu; int Ind; wchar_t Speed[256],Accel[256];
 Vary *V; Form F;
 xA=Vals[NStrMagCmp(NamesVarys,P->Nam)];
 swprintf(Speed,L"%s'%s",P->Nam,L->Time->Name);
 if(FindVary(Speed,&V)){ F.V=V; xt=Val(F); } else xt=0.0;
 swprintf(Accel,L"%s'%s",Speed,L->Time->Name);
 if(FindVary(Accel,&V)){ F.V=V; xtt=Val(F); } else xtt=0.0;
 P=P->Sled;
 yA=Vals[NStrMagCmp(NamesVarys,P->Nam)];
 swprintf(Speed,L"%s'%s",P->Nam,L->Time->Name);
 if(FindVary(Speed,&V)){ F.V=V; yt=Val(F); } else yt=0.0;
 swprintf(Accel,L"%s'%s",Speed,L->Time->Name);
 if(FindVary(Accel,&V)){ F.V=V; ytt=Val(F); } else ytt=0.0;
 P=P->Sled;
 mu=P->Nam?Vals[NStrMagCmp(NamesVarys,P->Nam)]:
    (P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val;
 P=P->Sled;
 atV=(xt*xtt+yt*ytt)/(xt*xt+yt*yt); xB=xA+mu*(xtt-xt*atV); yB=yA+mu*(ytt-yt*atV);
 XA=Round((xA-xk)/MSt[0])+Xk; YA=Yk-Round((yA-yk)/MSt[1]);
 XB=Round((xB-xk)/MSt[0])+Xk; YB=Yk-Round((yB-yk)/MSt[1]);
 Width=Round(P->Nam?FindVary(P->Nam,&V),Val(V->Znach):
        (P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val);
 if(Width<0) Width=0; P=P->Sled;
 Color=(TColor)Round(P->Nam?FindVary(P->Nam,&V),Val(V->Znach):
                (P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val);
 P=P->Sled;
 return P;
}
//---------------------------------------------------------------------------
Parm *__fastcall TMultFilms::ParmString(Parm *P,int &XA,int &YA,int &XB,int &YB,
  int &R,int &Width,TColor &Color,TColor &ColorKrug){ Vary *V;
 double xA,yA,xB,yB,l,a,r; int Ind;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); xA=Vals[Ind]; }
 else if(P->F.C&&(P->F.C!=Pust)) xA=Val(P->F);  else xA=P->Val;
 P=P->Sled; XA=Round((xA-xk)/MSt[0])+Xk;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); yA=Vals[Ind]; }
 else if(P->F.C&&(P->F.C!=Pust)) yA=Val(P->F);  else yA=P->Val;
 P=P->Sled; YA=Yk-Round((yA-yk)/MSt[1]);
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); l=Vals[Ind]; }
 else if(P->F.C&&(P->F.C!=Pust)) l=Val(P->F);  else l=P->Val;
 P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); a=Vals[Ind]; }
 else if(P->F.C&&(P->F.C!=Pust)) a=Val(P->F);  else a=P->Val;
 P=P->Sled; xB=xA+l*cos(a*M_PI/180.0); yB=yA+l*sin(a*M_PI/180.0);
 XB=Round((xB-xk)/MSt[0])+Xk; YB=Yk-Round((yB-yk)/MSt[1]);
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); r=Vals[Ind]; }
 else if(P->F.C&&(P->F.C!=Pust)) r=Val(P->F);  else r=P->Val;
 r/=MSt[0]; if(r<=0) r=1; R=Round(r); P=P->Sled;
 Width=Round(P->Nam?FindVary(P->Nam,&V),Val(V->Znach):
        (P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val);
 if(Width<0) Width=1; P=P->Sled;
 Color=(TColor)Round(P->Nam?FindVary(P->Nam,&V),Val(V->Znach):
                (P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val);
 P=P->Sled;
 ColorKrug=(TColor)Round(P->Nam?FindVary(P->Nam,&V),Val(V->Znach):
                    (P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val);
 if(ColorKrug<0) ColorKrug=*ColorGraph;
 P=P->Sled;
 return P;
}
//---------------------------------------------------------------------------
Parm *__fastcall TMultFilms::ParmHinge(Parm *P,int &XO,int &YO,
  int &XA,int &YA,int &XB,int &YB,int &XC,int &YC,int &XD,int &YD,
  int &XE,int &YE,int &XF,int &YF,int &XG,int &YG,int &XH,int &YH,
  int *X,int *Y,int &R,int &H,int &SH,int &Width,TColor &Color,
  TColor &ColorKrug,bool Living){
 Vary *V; int i,Ind;
 double xO,yO,xA,yA,xB,yB,xC,yC,xD,yD,xE,yE,xF,yF,xG,yG,xH,yH,x[11],y[11],h,r,a;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); xO=Vals[Ind]; }
 else if(P->F.C&&(P->F.C!=Pust)) xO=Val(P->F);  else xO=P->Val;
 P=P->Sled; XO=Round((xO-xk)/MSt[0])+Xk;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); yO=Vals[Ind]; }
 else if(P->F.C&&(P->F.C!=Pust)) xB=Val(P->F);  else yO=P->Val;
 P=P->Sled; YO=Yk-Round((yO-yk)/MSt[1]);
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); r=Vals[Ind]; }
 else if(P->F.C&&(P->F.C!=Pust)) r=Val(P->F);  else r=P->Val;
 P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); h=Vals[Ind]; }
 else if(P->F.C&&(P->F.C!=Pust)) h=Val(P->F);  else h=P->Val;
 P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); a=Vals[Ind]; }
 else if(P->F.C&&(P->F.C!=Pust)) a=Val(P->F);  else a=P->Val;
 P=P->Sled; a*=M_PI/180.0;
 SH=Round(P->Nam?FindVary(P->Nam,&V),Val(V->Znach):
     (P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val);
 P=P->Sled;
 Width=Round(P->Nam?FindVary(P->Nam,&V),Val(V->Znach):
       (P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val);
 if(Width<0) Width=1; P=P->Sled;
 Color=(TColor)Round(P->Nam?FindVary(P->Nam,&V),Val(V->Znach):
                (P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val);
 P=P->Sled;
 ColorKrug=(TColor)Round(P->Nam?FindVary(P->Nam,&V),Val(V->Znach):
                    (P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val);
 P=P->Sled; if(ColorKrug<0) ColorKrug=*ColorGraph;
 xA=-r*0.7; xB=r*0.7; yB=yA=-r*0.5;
 xC=-0.5*h; xD=h*0.5; xE=-h; xF=h; xG=-1.1*r; xH=1.1*r;
 if(Living){ yE=yF=-h-2*r; yD=yC=-h; yG=yH=yC-r; }
 else yE=yF=yD=yC=-h;
 if(SH){
  for(i=0;i<10;i++) x[i]=xE+i*2*h/9;
  for(i=0;i<5;i++){ y[2*i]=yE-0.5*h; y[2*i+1]=yE; }
 }
 XA=Xk+Round((xO+xA*cos(a)-yA*sin(a)-xk)/MSt[0]);
 YA=Yk-Round((yO+xA*sin(a)+yA*cos(a)-yk)/MSt[1]);
 XB=Xk+Round((xO+xB*cos(a)-yB*sin(a)-xk)/MSt[0]);
 YB=Yk-Round((yO+xB*sin(a)+yB*cos(a)-yk)/MSt[1]);
 XC=Xk+Round((xO+xC*cos(a)-yC*sin(a)-xk)/MSt[0]);
 YC=Yk-Round((yO+xC*sin(a)+yC*cos(a)-yk)/MSt[1]);
 XD=Xk+Round((xO+xD*cos(a)-yD*sin(a)-xk)/MSt[0]);
 YD=Yk-Round((yO+xD*sin(a)+yD*cos(a)-yk)/MSt[1]);
 XE=Xk+Round((xO+xE*cos(a)-yE*sin(a)-xk)/MSt[0]);
 YE=Yk-Round((yO+xE*sin(a)+yE*cos(a)-yk)/MSt[1]);
 XF=Xk+Round((xO+xF*cos(a)-yF*sin(a)-xk)/MSt[0]);
 YF=Yk-Round((yO+xF*sin(a)+yF*cos(a)-yk)/MSt[1]);
 if(Living){
  XG=Xk+Round((xO+xG*cos(a)-yG*sin(a)-xk)/MSt[0]);
  YG=Yk-Round((yO+xG*sin(a)+yG*cos(a)-yk)/MSt[1]);
  XH=Xk+Round((xO+xH*cos(a)-yH*sin(a)-xk)/MSt[0]);
  YH=Yk-Round((yO+xH*sin(a)+yH*cos(a)-yk)/MSt[1]);
 }
 if(SH){
  for(i=0;i<10;i++){
   X[i]=Xk+Round((xO+x[i]*cos(a)-y[i]*sin(a)-xk)/MSt[0]);
   Y[i]=Yk-Round((yO+x[i]*sin(a)+y[i]*cos(a)-yk)/MSt[1]);
 }}
 r/=MSt[0]; if(r<=0) r=1; R=Round(r);
 h/=MSt[0]; H=Round(h);
 return P;
}
//---------------------------------------------------------------------------
Parm *__fastcall TMultFilms::ParmRejka(Parm *P,double &xA,double &yA,
  double &a1,double &xB,double &yB,double &a2,int &W,TColor &Color){
 Vary *V; int Ind; double w;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); xA=Vals[Ind]; }
 else if(P->F.C&&(P->F.C!=Pust)) xA=Val(P->F);  else xA=P->Val;
 P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); yA=Vals[Ind]; }
 else if(P->F.C&&(P->F.C!=Pust)) yA=Val(P->F);  else yA=P->Val;
 P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); a1=Vals[Ind]; }
 else if(P->F.C&&(P->F.C!=Pust)) a1=Val(P->F);  else a1=P->Val;
 P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); xB=Vals[Ind]; }
 else if(P->F.C&&(P->F.C!=Pust)) xB=Val(P->F);  else xB=P->Val;
 P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); yB=Vals[Ind]; }
 else if(P->F.C&&(P->F.C!=Pust)) yB=Val(P->F);  else yB=P->Val;
 P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); a2=Vals[Ind]; }
 else if(P->F.C&&(P->F.C!=Pust)) a2=Val(P->F);  else a2=P->Val;
 P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); w=Vals[Ind]; }
 else if(P->F.C&&(P->F.C!=Pust)) w=Val(P->F);  else w=P->Val;
 P=P->Sled; W=Round(w);
 Color=(TColor)Round(P->Nam?FindVary(P->Nam,&V),Val(V->Znach):
        (P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val);
// if(Color<0) Color=*ColorGraph;
 P=P->Sled;
 return P;
}
//---------------------------------------------------------------------------
Parm *__fastcall TMultFilms::DrawLine(TCanvas *Canvas,Parm *P){
 int Ind,XA,YA,XB,YB,W; TColor C,Cp=Canvas->Pen->Color;
 P=ParmLine(P,XA,YA,XB,YB,W,C);
 if(W>0){
  Canvas->Pen->Color=C; Canvas->Pen->Width=W;
  Canvas->MoveTo(XA,YA); Canvas->LineTo(XB,YB);
  Canvas->Pen->Width=*WidthPen; Canvas->Pen->Color=Cp;
 }
 return P;
}
//---------------------------------------------------------------------------
Parm *__fastcall TMultFilms::DrawString(TCanvas *Canvas,Parm *P){
 int Ind,XA,YA,XB,YB,R,W;
 TColor C,Cf,Cp=Canvas->Pen->Color,Cb=Canvas->Brush->Color;
 P=ParmString(P,XA,YA,XB,YB,R,W,C,Cf);
 Canvas->Pen->Color=C; Canvas->Brush->Color=Cf; Canvas->Pen->Width=W;
 Canvas->MoveTo(XA,YA); Canvas->LineTo(XB,YB);
 if(Cf<0) Canvas->Arc(XA-R,YA-R,XA+R,YA+R,XA+R,0,XA+R,0);
 else Canvas->Ellipse(XA-R,YA-R,XA+R,YA+R);
 if(Cf<0) Canvas->Arc(XB-R,YB-R,XB+R,YB+R,XB+R,0,XB+R,0);
 else Canvas->Ellipse(XB-R,YB-R,XB+R,YB+R);
 Canvas->Pen->Width=*WidthPen; Canvas->Pen->Color=Cp; Canvas->Brush->Color=Cb;
 return P;
}
//---------------------------------------------------------------------------
Parm *__fastcall TMultFilms::DrawMultiLink(TCanvas *Canvas,Parm *P){
 int Ind,K,X0,Y0,X,Y,R,W; Vary *V; double d,x,y,r;
 TColor C,Cf,Cp=Canvas->Pen->Color,Cb=Canvas->Brush->Color;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); r=Vals[Ind]; }
 else if(P->F.C&&(P->F.C!=Pust)) r=Val(P->F);  else r=P->Val;
 r/=MSt[0]; if(r<=0) r=1; R=Round(r); P=P->Sled;
 W=Round(P->Nam?FindVary(P->Nam,&V),Val(V->Znach):
    (P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val);
 if(W<0) W=1; P=P->Sled;
 C=(TColor)Round(P->Nam?FindVary(P->Nam,&V),Val(V->Znach):
           (P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val);
 P=P->Sled;
 Cf=(TColor)Round(P->Nam?FindVary(P->Nam,&V),Val(V->Znach):
            (P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val);
// if(Cf<0) Cf=*ColorGraph;
 P=P->Sled;
 Canvas->Pen->Color=C; Canvas->Brush->Color=Cf; Canvas->Pen->Width=W;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); d=Vals[Ind]; }
 else if(P->F.C&&(P->F.C!=Pust)) d=Val(P->F);  else d=P->Val;
 P=P->Sled; K=Round(d);
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); x=Vals[Ind]; }
 else if(P->F.C&&(P->F.C!=Pust)) x=Val(P->F);  else x=P->Val;
 P=P->Sled; X0=Round((x-xk)/MSt[0])+Xk;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); y=Vals[Ind]; }
 else if(P->F.C&&(P->F.C!=Pust)) y=Val(P->F);  else y=P->Val;
 P=P->Sled; Y0=Yk-Round((y-yk)/MSt[1]);
 Canvas->MoveTo(X0,Y0);
 for(int i=0;i<K;i++){
  if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); x=Vals[Ind]; }
 else if(P->F.C&&(P->F.C!=Pust)) x=Val(P->F);  else x=P->Val;
  P=P->Sled; X=Round((x-xk)/MSt[0])+Xk;
  if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); y=Vals[Ind]; }
 else if(P->F.C&&(P->F.C!=Pust)) y=Val(P->F);  else y=P->Val;
  P=P->Sled; Y=Yk-Round((y-yk)/MSt[1]);
  Canvas->LineTo(X,Y);
  if(Cf<0) Canvas->Arc(X0-R,Y0-R,X0+R,Y0+R,X0+R,0,X0+R,0);
  else     Canvas->Ellipse(X0-R,Y0-R,X0+R,Y0+R);
  X0=X; Y0=Y;
 }
 if(Cf<0) Canvas->Arc(X-R,Y-R,X+R,Y+R,X+R,0,X+R,0);
 else     Canvas->Ellipse(X-R,Y-R,X+R,Y+R);
 Canvas->Pen->Width=*WidthPen; Canvas->Pen->Color=Cp; Canvas->Brush->Color=Cb;
 return P;
}
//---------------------------------------------------------------------------
Parm *__fastcall TMultFilms::DrawManipulator3d(TCanvas *Canvas,Parm *P){
 int Ind,K,X0,Y0,X,Y,R,W; Vary *V; double d,x,y,r; Parm **Pml;
 wchar_t Bx,By,NameX[16],NameY[16],Buf[16];
 TColor C1,C2,Cp=Canvas->Pen->Color,Cb=Canvas->Brush->Color;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); r=Vals[Ind]; }
 else if(P->F.C&&(P->F.C!=Pust)) r=Val(P->F);  else r=P->Val;
// r/=MSt[0]; if(r<=0) r=1; R=Round(r);
 P=P->Sled;
 W=Round(P->Nam?FindVary(P->Nam,&V),Val(V->Znach):
    (P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val);
 if(W<0) W=1; P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); C1=(TColor)Round(Vals[Ind]); }
 else C1=(TColor)Round(P->Val);
 P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); C2=(TColor)Round(Vals[Ind]); }
 else C2=(TColor)Round(P->Val);
 P=P->Sled;
 K=KolElem(P->Sled->Sled);
 Pml=(Parm **)calloc(2*K+7,sizeof(Parm *));
// C=(TColor)Round(P->Nam?FindVary(P->Nam,&V),Val(V->Znach):P->Val); P=P->Sled;
// Cf=(TColor)Round(P->Nam?FindVary(P->Nam,&V),Val(V->Znach):P->Val);
// P=P->Sled;
 Canvas->Pen->Color=C1; Canvas->Brush->Color=C2; Canvas->Pen->Width=W;
 for(int i=0;i<2*K+7;i++) Pml[i]=(Parm *)calloc(1,sizeof(Parm));
 for(int i=1;i<2*K+7;i++) Pml[i-1]->Sled=Pml[i];
 Pml[0]->Val=r; Pml[1]->Val=W; Pml[2]->Val=C1; Pml[3]->Val=C2; Pml[4]->Val=K;
 if(P->Nam) Bx=P->Nam[0]; wcscpy(NameX,P->Nam);
 Pml[5+K*2]->Nam=P->Nam;
 P=P->Sled;
 if(P->Nam) By=P->Nam[0]; wcscpy(NameY,P->Nam);
 Pml[6+K*2]->Nam=P->Nam;
 P=P->Sled;
 for(int i=0;i<K;i++,P=P->Sled){
  swprintf(Buf,L"%cO@%s",Bx,P->Nam);
  FindVary(Buf,&V);
  Pml[5+i*2]->Nam=V->Name;
  swprintf(Buf,L"%cO@%s",By,P->Nam);
  FindVary(Buf,&V);
  Pml[6+i*2]->Nam=V->Name;
 }
 DrawMultiLink(Canvas,Pml[0]);
 for(int i=0;i<2*K+7;i++) free(Pml[i]); free(Pml);
 return P;
}
//---------------------------------------------------------------------------
TPoint *__fastcall TMultFilms::ParmCircle3d(Parm *P,int &W,TColor &Ck,
  TColor &Cf,int K){
 int i,Ind,X,Y,R; Vary *V;
 TPoint *tP=(TPoint *)calloc(K+1,sizeof(TPoint));
 double x,y,z,dx,dy,dz,r,alfa,beta,a,b,xe,ye,
  *tx=(double *)calloc(K,SzD),*ty=(double *)calloc(K,SzD);
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); x=Vals[Ind]; }
 else if(P->F.C&&(P->F.C!=Pust)) x=Val(P->F);  else x=P->Val;
 P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); y=Vals[Ind]; }
 else if(P->F.C&&(P->F.C!=Pust)) y=Val(P->F);  else y=P->Val;
 P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); z=Vals[Ind]; }
 else if(P->F.C&&(P->F.C!=Pust)) z=Val(P->F);  else z=P->Val;
 P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); dx=Vals[Ind]; }
 else if(P->F.C&&(P->F.C!=Pust)) dx=Val(P->F);  else dx=P->Val;
 P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); dy=Vals[Ind]; }
 else if(P->F.C&&(P->F.C!=Pust)) dy=Val(P->F);  else dy=P->Val;
 P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); dz=Vals[Ind]; }
 else if(P->F.C&&(P->F.C!=Pust)) dz=Val(P->F);  else dz=P->Val;
 P=P->Sled;
 if(P->Nam){ FindVary(P->Nam,&V); r=Val(V->Znach); }
 else if(P->F.C&&(P->F.C!=Pust)) r=Val(P->F);  else r=P->Val;
 P=P->Sled;
 W=Round(P->Nam?FindVary(P->Nam,&V),Val(V->Znach):
    (P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val);
 P=P->Sled;
 Ck=(TColor)Round(P->Nam?FindVary(P->Nam,&V),Val(V->Znach):
            (P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val);
 P=P->Sled;
 Cf=(TColor)Round(P->Nam?FindVary(P->Nam,&V),Val(V->Znach):
             (P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val);
 P=P->Sled;
//определим углы нормали с вертикальной и горизонтальной плоскостями
 alfa=ArcTan2(dx,dy);
 a=r; b=a*dz/sqrt(dx*dx+dy*dy+dz*dz);
 for(i=0;i<K;i++){
  tx[i]=a*cos(2*M_PI*i/K);
  ty[i]=b*sin(2*M_PI*i/K);
  xe=x-tx[i]*cos(alfa)-ty[i]*sin(alfa);
  ye=y+tx[i]*sin(alfa)-ty[i]*cos(alfa);
  tP[i].x=Xk+Round((xe-xk)/MSt[0]);
  tP[i].y=Yk-Round((ye-yk)/MSt[1]);
 }
 tP[i]=tP[0];
 return tP;
}
//---------------------------------------------------------------------------
double ZnachParm(Parm *P){ double Rez; Vary *V;
 if(P->Nam){ FindVary(P->Nam,&V); Rez=Val(V->Znach); }
 else if(P->F.C&&(P->F.C!=Pust)) Rez=Val(P->F);
 else Rez=P->Val;
 return Rez;
}
//---------------------------------------------------------------------------
TPoint *__fastcall TMultFilms::ParmPolygon3d(Parm *P,int &Width,
  TColor &ColorL,TColor &ColorF,int &K){
 Vary *V; TPoint *tP; double x,y,d; int Ind,i;
 Width=Round(P->Nam?FindVary(P->Nam,&V),Val(V->Znach):
        (P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val);
 if(Width<0) Width=1; P=P->Sled;
 ColorL=(TColor)Round(ZnachParm(P)); P=P->Sled;
 ColorF=(TColor)Round(ZnachParm(P)); P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); d=Vals[Ind]; }
 else if(P->F.C&&(P->F.C!=Pust)) d=Val(P->F);  else d=P->Val;
 P=P->Sled; P=P->Sled; K=Round(d);
 tP=(TPoint *)calloc(K+1,sizeof(TPoint));
 for(i=0;i<K;i++){
  if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); x=Vals[Ind]; }
 else if(P->F.C&&(P->F.C!=Pust)) x=Val(P->F);  else x=P->Val;
  P=P->Sled; tP[i].x=Round((x-xk)/MSt[0])+Xk;
  if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); y=Vals[Ind]; }
 else if(P->F.C&&(P->F.C!=Pust)) y=Val(P->F);  else y=P->Val;
  P=P->Sled; tP[i].y=Yk-Round((y-yk)/MSt[1]);
 }
 tP[i]=tP[0];
 return tP;
}
//---------------------------------------------------------------------------
Parm *__fastcall TMultFilms::DrawCircle3d(TCanvas *Canvas,Parm *P){
 int X,Y,R,W,K=24; TColor Ck,Cf,Cp=Canvas->Pen->Color,Cb=Canvas->Brush->Color;
 TPoint *tP=ParmCircle3d(P,W,Ck,Cf,K);
 Canvas->Pen->Width=W; Canvas->Pen->Color=Ck;
 Canvas->Brush->Color=Cf>-1?Cf:-1;//bsClear;//*ColorPhon;
 if(Cf<0){
  Canvas->MoveTo(tP[0].x,tP[0].y);
  for(int i=1;i<=K;i++) Canvas->LineTo(tP[i].x,tP[i].y);
 }
 else Canvas->Polygon(tP,K);
 Canvas->Pen->Width=*WidthPen; Canvas->Pen->Color=Cp; Canvas->Brush->Color=Cb;
 if(tP) free(tP);
 return P;
}
//---------------------------------------------------------------------------
Parm *__fastcall TMultFilms::DrawPolygon3d(TCanvas *Canvas,Parm *P){
 int Ind,K,X0,Y0,X,Y,W; Vary *V; double d,x,y;
 TColor C,Cf,Cp=Canvas->Pen->Color,Cb=Canvas->Brush->Color;
 TPoint *tP=ParmPolygon3d(P,W,C,Cf,K);
 Canvas->Pen->Color=C; Canvas->Brush->Color=Cf>-1?Cf:*ColorPhon;
 Canvas->Pen->Width=W;
// if(Cf<0){
//  Canvas->MoveTo(tP[0].x,tP[0].y);
//  for(int i=1;i<=K;i++) Canvas->LineTo(tP[i].x,tP[i].y);
// }
// else
  Canvas->Polygon(tP,K);
 Canvas->Pen->Width=*WidthPen; Canvas->Pen->Color=Cp; Canvas->Brush->Color=Cb;
 if(tP) free(tP);
 return P;
}
//---------------------------------------------------------------------------
Parm *__fastcall TMultFilms::DrawSpline(TCanvas *Canvas,Parm *P){
//name, x0, xk,w, c, n, x1, y1,...,xn, yn
 Spln *Sp; wchar_t *Name=P->Nam; Vary *V; double w,c,n,xb,yb,xe,ye,x,y;
 int i,Ind,K,W,Xb,Yb,Xe,N,X,Y; TColor C,Cp=Canvas->Pen->Color;
 P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); xb=Vals[Ind]; }
 else if(P->F.C&&(P->F.C!=Pust)) xb=Val(P->F);  else xb=P->Val;
 Xb=Round((xb-xk)/MSt[0])+Xk; P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); xe=Vals[Ind]; }
 else if(P->F.C&&(P->F.C!=Pust)) xe=Val(P->F);  else xe=P->Val;
 Xe=Round((xe-xk)/MSt[0])+Xk; P=P->Sled;
 W=Round(P->Nam?FindVary(P->Nam,&V),Val(V->Znach):
    (P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val);
 if(W<0) W=1; P=P->Sled;
 C=(TColor)Round(P->Nam?FindVary(P->Nam,&V),Val(V->Znach):
           (P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val);
 P=P->Sled;
 Canvas->Pen->Color=C; Canvas->Pen->Width=W;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); n=Vals[Ind]; }
 else if(P->F.C&&(P->F.C!=Pust)) n=Val(P->F);  else n=P->Val;
 P=P->Sled; N=Round(n);
 if(FindSpln(Rsp,Name,&Sp)){
  for(i=0;i<2*N;i++,P=P->Sled){
   if(P->Nam){
    if(FindVary(P->Nam,&V)) Sp->A->A[i]=V->Val;
   }
   else Sp->A->A[i]=Round(P->Val);
  }
  Sp->CalcSpln();
  yb=Spl(Sp,xb);
  Xb=Round((xb-xk)/MSt[0])+Xk; Yb=Yk-Round((yb-yk)/MSt[1]);
  Canvas->MoveTo(Xb,Yb);
  for(X=Xb+1;X<=Xe;X++){
   x=xk+(X-Xk)*MSt[0]; y=Spl(Sp,x); Y=Yk-Round((y-yk)/MSt[1]);
   Canvas->LineTo(X,Y);
 }}
 else for(i=0;i<2*N;i++) P=P->Sled;
 Canvas->Pen->Width=*WidthPen; Canvas->Pen->Color=Cp;
 return P;
}
//---------------------------------------------------------------------------
Parm *__fastcall TMultFilms::DrawHinge(TCanvas *Canvas,Parm *P,bool Living){
 TColor C,Cf,Cp=Canvas->Pen->Color,Cb=Canvas->Brush->Color;
 int Ind,XO,YO,XA,YA,XB,YB,XC,YC,XD,YD,XE,YE,XF,YF,XG,YG,XH,YH,X[10],Y[10],
  R,H,A,SH,W;
 P=ParmHinge(P,XO,YO,XA,YA,XB,YB,XC,YC,XD,YD,XE,YE,XF,YF,XG,YG,XH,YH,X,Y,R,H,SH,
             W,C,Cf,Living);
 Canvas->Pen->Color=C; Canvas->Brush->Color=Cf; Canvas->Pen->Width=W;
 Canvas->MoveTo(XA,YA); Canvas->LineTo(XC,YC);
 Canvas->LineTo(XD,YD); Canvas->LineTo(XB,YB);
 if(SH){
  for(int i=0;i<5;i++){
   Canvas->MoveTo(X[2*i],Y[2*i]); Canvas->LineTo(X[2*i+1],Y[2*i+1]);
 }}
 Canvas->MoveTo(XE,YE); Canvas->LineTo(XF,YF);
 if(Cf<0){
  Canvas->Arc(XO-R,YO-R,XO+R,YO+R,XO+R,0,XO+R,0);
  if(Living){
   Canvas->Arc(XG-R,YG-R,XG+R,YG+R,XG+R,0,XG+R,0);
   Canvas->Arc(XH-R,YH-R,XH+R,YH+R,XH+R,0,XH+R,0);
 }}
 else{
  Canvas->Ellipse(XO-R,YO-R,XO+R,YO+R);
  if(Living){
   Canvas->Ellipse(XG-R,YG-R,XG+R,YG+R);
   Canvas->Ellipse(XH-R,YH-R,XH+R,YH+R);
 }}
 Canvas->Pen->Width=*WidthPen; Canvas->Pen->Color=Cp; Canvas->Brush->Color=Cb;
 return P;
}
//---------------------------------------------------------------------------
Parm *__fastcall TMultFilms::DrawRejka(TCanvas *Canvas,Parm *P){
 int i,XA,YA,XB,YB,kx,ky,k,W;
 double xA,yA,a1,xB,yB,a2,w,a,b,c,Ca,Sa,l,eta,ksi=0.0,x,y,dksi;
 TColor C,Cp=Canvas->Pen->Color; //Parm *p=P;
 P=ParmRejka(P,xA,yA,a1,xB,yB,a2,W,C);
// W=(Round(w/MSt[0])+Round(w/MSt[1]))/2; if(W<1) W=1; W=2;
 Canvas->Pen->Color=C; Canvas->Pen->Width=W;
 XA=Round((xA-xk)/MSt[0])+Xk; YA=Yk-Round((yA-yk)/MSt[1]);
 XB=Round((xB-xk)/MSt[0])+Xk; YB=Yk-Round((yB-yk)/MSt[1]);
 A_B_C(xA,yA,a1,xB,yB,a2,a,b,c,Ca,Sa,l);
 kx=abs(XB-XA);
 ky=abs(YB-YA);
 k=1+(kx>ky?kx:ky);
 if(k>1) dksi=l/(k-1); else dksi=1;
 Canvas->MoveTo(XA,YA);
 for(i=0;i<k;i++){
  eta=ksi*(a*ksi*ksi+b*ksi+c);
  x=xA+Ca*ksi-Sa*eta; y=yA+Sa*ksi+Ca*eta;
  XB=Round((x-xk)/MSt[0])+Xk; YB=Yk-Round((y-yk)/MSt[1]);
  Canvas->LineTo(XB,YB);
  ksi+=dksi;
 }
 Canvas->Pen->Width=*WidthPen; Canvas->Pen->Color=Cp;
 return P;
}
//---------------------------------------------------------------------------
/*
Parm *__fastcall TMultFilms::ClearRejka(TCanvas *Canvas,Parm *P){
 int i,XA,YA,XB,YB,kx,ky,k,W;
 double xA,yA,a1,xB,yB,a2,w,a,b,c,Ca,Sa,l,eta,ksi=0.0,x,y,dksi;
 TColor C; //Parm *p=P;
 P=ParmRejka(P,xA,yA,a1,xB,yB,a2,W,C);
 W=(Round(w/MSt[0])+Round(w/MSt[1]))/2; if(W<1) W=1; W=2;
 Canvas->Pen->Color=*ColorPhon; Canvas->Pen->Width=W;
 XA=Round((xA-xk)/MSt[0])+Xk; YA=Yk-Round((yA-yk)/MSt[1]);
 XB=Round((xB-xk)/MSt[0])+Xk; YB=Yk-Round((yB-yk)/MSt[1]);
 A_B_C(xA,yA,a1,xB,yB,a2,a,b,c,Ca,Sa,l);
 kx=abs(XB-XA); ky=abs(YB-YA); k=1+(kx>ky?kx:ky);
 if(k>1) dksi=l/(k-1); else dksi=1;
 Canvas->MoveTo(XA,YA);
 for(i=0;i<k;i++){
  eta=ksi*(a*ksi*ksi+b*ksi+c);
  x=xA+Ca*ksi-Sa*eta; y=yA+Sa*ksi+Ca*eta;
  XB=Round((x-xk)/MSt[0])+Xk; YB=Yk-Round((y-yk)/MSt[1]);
  Canvas->LineTo(XB,YB);
  ksi+=dksi;
 }
 Canvas->Pen->Width=*WidthPen;
 return P;
}
//---------------------------------------------------------------------------
Parm *__fastcall TMultFilms::ClearLine(TCanvas *Canvas,Parm *P){
 int Ind,XA,YA,XB,YB,W; TColor C;
 P=ParmLine(P,XA,YA,XB,YB,W,C);
 Canvas->Pen->Color=*ColorPhon; Canvas->Pen->Width=W;
 Canvas->MoveTo(XA,YA); Canvas->LineTo(XB,YB);
 return P;
}
//---------------------------------------------------------------------------
Parm *__fastcall TMultFilms::ClearString(TCanvas *Canvas,Parm *P){
 int Ind,XA,YA,XB,YB,W; TColor C;
 P=ParmLine(P,XA,YA,XB,YB,W,C);
 Canvas->Pen->Color=*ColorPhon; Canvas->Pen->Width=W;
 Canvas->MoveTo(XA,YA); Canvas->LineTo(XB,YB);
 return P;
}
//---------------------------------------------------------------------------
Parm *__fastcall TMultFilms::ClearFixHinge(TCanvas *Canvas,Parm *P){
 int Ind,XO,YO,XA,YA,XB,YB,XC,YC,XD,YD,XE,YE,XF,YF,X[10],Y[10],R,H,A,SH,W;
 TColor C,Cf;
 P=ParmHinge(P,XO,YO,XA,YA,XB,YB,XC,YC,XD,YD,XE,YE,XF,YF,X,Y,R,H,SH,W,C,Cf);
 Canvas->Pen->Color=*ColorPhon; Canvas->Pen->Width=W;
 Canvas->MoveTo(XA,YA); Canvas->LineTo(XC,YC);
 Canvas->MoveTo(XB,YB); Canvas->LineTo(XD,YD);
 Canvas->MoveTo(XE,YE); Canvas->LineTo(XF,YF);
 if(Cf<0) Canvas->Arc(XO-R,YO-R,XO+R,YO+R,XO+R,0,XO+R,0);
 else Canvas->Ellipse(XO-R,YO-R,XO+R,YO+R);
 return P;
}*/
//---------------------------------------------------------------------------
Parm *__fastcall TMultFilms::GabaritSpring(Parm *P){
//xA,yA,xB,yB - координаты концов, d - ширина, w - толщина линии,
//с - цвет, n - число витков
 double xA,yA,xB,yB,d,x1,y1,x2,y2,x3,y3,x4,y4,a,d2,l; int Ind;
 xA=P->Nam?Vals[NStrMagCmp(NamesVarys,P->Nam)]:(P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val;
 P=P->Sled;
 yA=P->Nam?Vals[NStrMagCmp(NamesVarys,P->Nam)]:(P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val;
 P=P->Sled;
 xB=P->Nam?Vals[NStrMagCmp(NamesVarys,P->Nam)]:(P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val;
 P=P->Sled;
 yB=P->Nam?Vals[NStrMagCmp(NamesVarys,P->Nam)]:(P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val;
 P=P->Sled;
 d =P->Nam?Vals[NStrMagCmp(NamesVarys,P->Nam)]:(P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val;
 P=P->Sled; P=P->Sled; P=P->Sled; P=P->Sled;
 d2=d/2;
 if(xB==xA) a=yB>yA?M_PI_2:3*M_PI_2;
 else{
  a=atan((yB-yA)/(xB-xA)); if((xB-xA)<0.0) a+=M_PI;
 } 
 x1=xA-d2*sin(a); y1=yA+d2*cos(a);  x2=xB-d2*sin(a); y2=yB+d2*cos(a);
 x3=xB+d2*sin(a); y3=yB-d2*cos(a);  x4=xA+d2*sin(a); y4=yA-d2*cos(a);
 if(x1>Spr[0]){ Spr[0]=x1; Spr[1]=y1; } if(x1<Slv[0]){ Slv[0]=x1; Slv[1]=y1; }
 if(x2>Spr[0]){ Spr[0]=x2; Spr[1]=y2; } if(x2<Slv[0]){ Slv[0]=x2; Slv[1]=y2; }
 if(x3>Spr[0]){ Spr[0]=x3; Spr[1]=y3; } if(x3<Slv[0]){ Slv[0]=x3; Slv[1]=y3; }
 if(x4>Spr[0]){ Spr[0]=x4; Spr[1]=y4; } if(x4<Slv[0]){ Slv[0]=x4; Slv[1]=y4; }
 if(y1>Max[1]){ Max[1]=y1; Max[0]=x1; } if(y1<Min[1]){ Min[1]=y1; Min[0]=x1; }
 if(y2>Max[1]){ Max[1]=y2; Max[0]=x2; } if(y2<Min[1]){ Min[1]=y2; Min[0]=x2; }
 if(y3>Max[1]){ Max[1]=y3; Max[0]=x3; } if(y3<Min[1]){ Min[1]=y3; Min[0]=x3; }
 if(y4>Max[1]){ Max[1]=y4; Max[0]=x4; } if(y4<Min[1]){ Min[1]=y4; Min[0]=x4; }
 return P;
}
//---------------------------------------------------------------------------
Parm *__fastcall TMultFilms::ParmSpring(Parm *P,double &xA,double &yA,
  double &xB,double &yB,double &d,int &Width,TColor &Color,int &N){ Vary *V;
 if(P->Nam) xA=Vals[NStrMagCmp(NamesVarys,P->Nam)];
 else if(P->F.C&&(P->F.C!=Pust)) xA=Val(P->F);  else xA=P->Val;
 P=P->Sled;
 if(P->Nam) yA=Vals[NStrMagCmp(NamesVarys,P->Nam)];
 else if(P->F.C&&(P->F.C!=Pust)) yA=Val(P->F);  else yA=P->Val;
 P=P->Sled;
 if(P->Nam) xB=Vals[NStrMagCmp(NamesVarys,P->Nam)];
 else if(P->F.C&&(P->F.C!=Pust)) xB=Val(P->F);  else xB=P->Val;
 P=P->Sled;
 if(P->Nam) yB=Vals[NStrMagCmp(NamesVarys,P->Nam)];
 else if(P->F.C&&(P->F.C!=Pust)) yB=Val(P->F);  else yB=P->Val;
 P=P->Sled;
 if(P->Nam)  d=Vals[NStrMagCmp(NamesVarys,P->Nam)];
 else if(P->F.C&&(P->F.C!=Pust)) d=Val(P->F); else d=P->Val;
 P=P->Sled;
 Width=Round(P->Nam?FindVary(P->Nam,&V),Val(V->Znach):
        (P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val);
 P=P->Sled;
 if(Width<0) Width=1;
 Color=(TColor)Round(P->Nam?FindVary(P->Nam,&V),Val(V->Znach):
        (P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val); P=P->Sled;
// if(Color<0) Color=*ColorGraph;
 N=(TColor)Round(P->Nam?FindVary(P->Nam,&V),Val(V->Znach):
    (P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val); P=P->Sled;
 return P;
}
//---------------------------------------------------------------------------
Parm *__fastcall TMultFilms::DrawSpring(TCanvas *Canvas,Parm *P){
 int Ind,W,N,X,Y; TColor C,Cp=Canvas->Pen->Color;
 double xA,yA,xB,yB,d,x1,x2,x3,x4,y1,y2,y3,y4,h,l,a;
 P=ParmSpring(P,xA,yA,xB,yB,d,W,C,N);
 d/=2; h=sqrt(pow(xB-xA,2)+pow(yB-yA,2))/(4*N);
 if(xB==xA) a=yB>yA?M_PI_2:3*M_PI_2;
 else{
  a=atan((yB-yA)/(xB-xA)); if((xB-xA)<0.0) a+=M_PI;
 }
 Canvas->Pen->Color=C; Canvas->Pen->Width=W;
 x1=xA; y1=yA; X=Xk+Round((xA-xk)/MSt[0]); Y=Yk-Round((yA-yk)/MSt[1]);
 Canvas->MoveTo(X,Y);
 for(int i=0;i<N;i++){
  x2=x1+  h*cos(a)-d*sin(a); y2=y1+  h*sin(a)+d*cos(a);
  x3=x1+3*h*cos(a)+d*sin(a); y3=y1+3*h*sin(a)-d*cos(a);
  x4=x1+4*h*cos(a);          y4=y1+4*h*sin(a);
  X=Xk+Round((x2-xk)/MSt[0]); Y=Yk-Round((y2-yk)/MSt[1]); Canvas->LineTo(X,Y);
  X=Xk+Round((x3-xk)/MSt[0]); Y=Yk-Round((y3-yk)/MSt[1]); Canvas->LineTo(X,Y);
  X=Xk+Round((x4-xk)/MSt[0]); Y=Yk-Round((y4-yk)/MSt[1]); Canvas->LineTo(X,Y);
  x1=x4; y1=y4;
 }
 Canvas->Pen->Width=*WidthPen; Canvas->Pen->Color=Cp;
 return P;
}
//---------------------------------------------------------------------------
/*
Parm *__fastcall TMultFilms::ClearSpring(TCanvas *Canvas,Parm *P){
 int Ind,W,N,X,Y; TColor C; double xA,yA,xB,yB,d,x1,x2,x3,x4,y1,y2,y3,y4,h,l,a;
 P=ParmSpring(P,xA,yA,xB,yB,d,W,C,N);
 d/=2; h=sqrt(pow(xB-xA,2)+pow(yB-yA,2))/(4*N);
 a=atan((yB-yA)/(xB-xA)); if(a<0.0) a+=M_PI;
 Canvas->Pen->Color=*ColorPhon; Canvas->Pen->Width=W;
 x1=xA; y1=yA; X=Xk+Round((xA-xk)/MSt[0]); Y=Yk-Round((yA-yk)/MSt[1]);
 Canvas->MoveTo(X,Y);
 for(int i=0;i<N;i++){
  x2=x1+  h*cos(a)-d*sin(a); y2=y1+  h*sin(a)+d*cos(a);
  x3=x1+3*h*cos(a)+d*sin(a); y3=y1+3*h*sin(a)-d*cos(a);
  x4=x1+4*h*cos(a);          y4=y1+4*h*sin(a);
  X=Xk+Round((x2-xk)/MSt[0]); Y=Yk-Round((y2-yk)/MSt[1]); Canvas->LineTo(X,Y);
  X=Xk+Round((x3-xk)/MSt[0]); Y=Yk-Round((y3-yk)/MSt[1]); Canvas->LineTo(X,Y);
  X=Xk+Round((x4-xk)/MSt[0]); Y=Yk-Round((y4-yk)/MSt[1]); Canvas->LineTo(X,Y);
  x1=x4; y1=y4;
 }
 Canvas->Pen->Width=*WidthPen;
 return P;
}*/
//---------------------------------------------------------------------------
Parm *__fastcall TMultFilms::DrawVector(TCanvas *Canvas,Parm *P){
 int Ind,XA,YA,XB,YB,W,a=4,b=3*a; TColor C,Cp=Canvas->Pen->Color; double L,l,Ca,Sa;
 P=ParmLine(P,XA,YA,XB,YB,W,C);
 if(W>0){
  L=sqrt((XB-XA)*(XB-XA)+(YB-YA)*(YB-YA)); l=Round(L);
  if(l){
   Ca=(XB-XA)/L; Sa=(YB-YA)/L;
   Canvas->Pen->Color=C; Canvas->Pen->Width=W;
   Canvas->MoveTo(XA,YA); Canvas->LineTo(XB,YB);
   Canvas->LineTo(XA+(l-b)*Ca-a*Sa,YA+(l-b)*Sa+a*Ca); Canvas->MoveTo(XB,YB);
   Canvas->LineTo(XA+(l-b)*Ca+a*Sa,YA+(l-b)*Sa-a*Ca);
 }}
 Canvas->Pen->Width=*WidthPen; Canvas->Pen->Color=Cp;
 return P;
}
//---------------------------------------------------------------------------
Parm *__fastcall TMultFilms::DrawFinger(TCanvas *Canvas,Parm *P){
 int Ind,XA,YA,XB,YB,W,a=4,b=3*a; TColor C,Cp=Canvas->Pen->Color; double L,l,Ca,Sa;
 P=ParmFinger(P,XA,YA,XB,YB,W,C);
 if(W>0){
  L=sqrt((XB-XA)*(XB-XA)+(YB-YA)*(YB-YA)); l=Round(L);
  if(l){
   Ca=(XB-XA)/L; Sa=(YB-YA)/L;
   Canvas->Pen->Color=C; Canvas->Pen->Width=W;
   Canvas->MoveTo(XA,YA); Canvas->LineTo(XB,YB);
   Canvas->LineTo(XA+(l-b)*Ca-a*Sa,YA+(l-b)*Sa+a*Ca); Canvas->MoveTo(XB,YB);
   Canvas->LineTo(XA+(l-b)*Ca+a*Sa,YA+(l-b)*Sa-a*Ca);
 }}
 Canvas->Pen->Width=*WidthPen; Canvas->Pen->Color=Cp;
 return P;
}
//---------------------------------------------------------------------------
Parm *__fastcall TMultFilms::DrawSpeed(TCanvas *Canvas,Parm *P){
 int Ind,XA,YA,XB,YB,W,a=4,b=3*a; TColor C,Cp=Canvas->Pen->Color; double L,l,Ca,Sa;
 P=ParmSpeed(P,XA,YA,XB,YB,W,C);
 if(W>0){
  L=sqrt((XB-XA)*(XB-XA)+(YB-YA)*(YB-YA)); l=Round(L);
  if(l){
   Ca=(XB-XA)/L; Sa=(YB-YA)/L;
   Canvas->Pen->Color=C; Canvas->Pen->Width=W;
   Canvas->MoveTo(XA,YA); Canvas->LineTo(XB,YB);
   Canvas->LineTo(XA+(l-b)*Ca-a*Sa,YA+(l-b)*Sa+a*Ca); Canvas->MoveTo(XB,YB);
   Canvas->LineTo(XA+(l-b)*Ca+a*Sa,YA+(l-b)*Sa-a*Ca);
 }}
 Canvas->Pen->Width=*WidthPen; Canvas->Pen->Color=Cp;
 return P;
}
//---------------------------------------------------------------------------
Parm *__fastcall TMultFilms::DrawAtang(TCanvas *Canvas,Parm *P){
 int Ind,XA,YA,XB,YB,W,a=4,b=3*a; TColor C,Cp=Canvas->Pen->Color; double L,l,Ca,Sa;
 P=ParmAtang(P,XA,YA,XB,YB,W,C);
 if(W>0){
  L=sqrt((XB-XA)*(XB-XA)+(YB-YA)*(YB-YA)); l=Round(L);
  if(l){
   Ca=(XB-XA)/L; Sa=(YB-YA)/L;
   Canvas->Pen->Color=C; Canvas->Pen->Width=W;
   Canvas->MoveTo(XA,YA); Canvas->LineTo(XB,YB);
   Canvas->LineTo(XA+(l-b)*Ca-a*Sa,YA+(l-b)*Sa+a*Ca); Canvas->MoveTo(XB,YB);
   Canvas->LineTo(XA+(l-b)*Ca+a*Sa,YA+(l-b)*Sa-a*Ca);
 }}
 Canvas->Pen->Width=*WidthPen; Canvas->Pen->Color=Cp;
 return P;
}
//---------------------------------------------------------------------------
Parm *__fastcall TMultFilms::DrawAnorm(TCanvas *Canvas,Parm *P){
 int Ind,XA,YA,XB,YB,W,a=4,b=3*a; TColor C,Cp=Canvas->Pen->Color; double L,l,Ca,Sa;
 P=ParmAnorm(P,XA,YA,XB,YB,W,C);
 if(W>0){
  L=sqrt((XB-XA)*(XB-XA)+(YB-YA)*(YB-YA)); l=Round(L);
  if(l){
   Ca=(XB-XA)/L; Sa=(YB-YA)/L;
   Canvas->Pen->Color=C; Canvas->Pen->Width=W;
   Canvas->MoveTo(XA,YA); Canvas->LineTo(XB,YB);
   Canvas->LineTo(XA+(l-b)*Ca-a*Sa,YA+(l-b)*Sa+a*Ca); Canvas->MoveTo(XB,YB);
   Canvas->LineTo(XA+(l-b)*Ca+a*Sa,YA+(l-b)*Sa-a*Ca);
 }}
 Canvas->Pen->Width=*WidthPen; Canvas->Pen->Color=Cp;
 return P;
}
//---------------------------------------------------------------------------
/*
Parm *__fastcall TMultFilms::ClearVector(TCanvas *Canvas,Parm *P){
 int Ind,XA,YA,XB,YB,W,a=4,b=3*a; TColor C; double L,l,Ca,Sa;
 P=ParmLine(P,XA,YA,XB,YB,W,C);
 L=sqrt((XB-XA)*(XB-XA)+(YB-YA)*(YB-YA)); l=Round(L);
 if(l){
  Ca=(XB-XA)/L; Sa=(YB-YA)/L;
  Canvas->Pen->Color=*ColorPhon; Canvas->Pen->Width=W;
  Canvas->MoveTo(XA,YA); Canvas->LineTo(XB,YB);
  Canvas->LineTo(XA+(l-b)*Ca-a*Sa,YA+(l-b)*Sa+a*Ca); Canvas->MoveTo(XB,YB);
  Canvas->LineTo(XA+(l-b)*Ca+a*Sa,YA+(l-b)*Sa-a*Ca);
 }
 return P;
}
//---------------------------------------------------------------------------
Parm *__fastcall TMultFilms::ClearFinger(TCanvas *Canvas,Parm *P){
 int Ind,XA,YA,XB,YB,W,a=4,b=3*a; TColor C; double L,l,Ca,Sa;
 P=ParmFinger(P,XA,YA,XB,YB,W,C);
 L=sqrt((XB-XA)*(XB-XA)+(YB-YA)*(YB-YA)); l=Round(L);
 if(l){
  Ca=(XB-XA)/L; Sa=(YB-YA)/L;
  Canvas->Pen->Color=*ColorPhon; Canvas->Pen->Width=W;
  Canvas->MoveTo(XA,YA); Canvas->LineTo(XB,YB);
  Canvas->LineTo(XA+(l-b)*Ca-a*Sa,YA+(l-b)*Sa+a*Ca); Canvas->MoveTo(XB,YB);
  Canvas->LineTo(XA+(l-b)*Ca+a*Sa,YA+(l-b)*Sa-a*Ca);
 }
 return P;
}*/
//---------------------------------------------------------------------------
Parm *__fastcall TMultFilms::GabaritBar(Parm *P){
 double xA,yA,xB,yB,xC,yC,xD,yD,xE,yE,h,g,fi,Cfi,Sfi,gCfi,gSfi,hCfi,hSfi; int Ind;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); xC=Vals[Ind]; }
 else if(P->F.C&&(P->F.C!=Pust)) xC=Val(P->F);  else xC=P->Val;
 P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); yC=Vals[Ind]; }
 else if(P->F.C&&(P->F.C!=Pust)) yC=Val(P->F);  else yC=P->Val;
 P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); fi=Vals[Ind]; }
 else if(P->F.C&&(P->F.C!=Pust)) fi=Val(P->F);  else fi=P->Val;
 P=P->Sled; Cfi=cos(fi); Sfi=sin(fi);
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); h=Vals[Ind]; }
 else if(P->F.C&&(P->F.C!=Pust)) h=Val(P->F);  else h=P->Val;
 P=P->Sled; hCfi=h/2*Cfi; hSfi=h/2*Sfi;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); g=Vals[Ind]; }
 else if(P->F.C&&(P->F.C!=Pust)) g=Val(P->F);  else g=P->Val;
 P=P->Sled; gCfi=g/2*Cfi; gSfi=g/2*Sfi;
 xA=xC+gCfi-hSfi; yA=yC+gSfi+hCfi; xB=xC-gCfi-hSfi; yB=yC-gSfi+hCfi;
 xE=xC+gCfi+hSfi; yE=yC+gSfi-hCfi; xD=xC-gCfi+hSfi; yD=yC-gSfi-hCfi;
 if(xA>Spr[0]){ Spr[0]=xA; Spr[1]=yA; } if(xA<Slv[0]){ Slv[0]=xA; Slv[1]=yA; }
 if(xB>Spr[0]){ Spr[0]=xB; Spr[1]=yB; } if(xB<Slv[0]){ Slv[0]=xB; Slv[1]=yB; }
 if(xD>Spr[0]){ Spr[0]=xD; Spr[1]=yD; } if(xD<Slv[0]){ Slv[0]=xD; Slv[1]=yD; }
 if(xE>Spr[0]){ Spr[0]=xE; Spr[1]=yE; } if(xE<Slv[0]){ Slv[0]=xE; Slv[1]=yE; }
 if(yA>Max[1]){ Max[1]=yA; Max[0]=xA; } if(yA<Min[1]){ Min[1]=yA; Min[0]=xA; }
 if(yB>Max[1]){ Max[1]=yB; Max[0]=xB; } if(yB<Min[1]){ Min[1]=yB; Min[0]=xB; }
 if(yD>Max[1]){ Max[1]=yD; Max[0]=xD; } if(yD<Min[1]){ Min[1]=yD; Min[0]=xD; }
 if(yE>Max[1]){ Max[1]=yE; Max[0]=xE; } if(yE<Min[1]){ Min[1]=yE; Min[0]=xE; }
 P=P->Sled; P=P->Sled; P=P->Sled;
 return P;
}
//---------------------------------------------------------------------------
Parm *__fastcall TMultFilms::GabaritTriangle(Parm *P){
 double xA,yA,xB,yB,xC,yC; int Ind;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); xA=Vals[Ind]; }
 else if(P->F.C&&(P->F.C!=Pust)) xA=Val(P->F);  else xA=P->Val;
 P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); yA=Vals[Ind]; }
 else if(P->F.C&&(P->F.C!=Pust)) yA=Val(P->F);  else yA=P->Val;
 P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); xB=Vals[Ind]; }
 else if(P->F.C&&(P->F.C!=Pust)) xB=Val(P->F);  else xB=P->Val;
 P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); yB=Vals[Ind]; }
 else if(P->F.C&&(P->F.C!=Pust)) yB=Val(P->F);  else yB=P->Val;
 P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); xC=Vals[Ind]; }
 else if(P->F.C&&(P->F.C!=Pust)) xC=Val(P->F);  else xC=P->Val;
 P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); yC=Vals[Ind]; }
 else if(P->F.C&&(P->F.C!=Pust)) yC=Val(P->F);  else yC=P->Val;
 P=P->Sled; P=P->Sled;  P=P->Sled; P=P->Sled;
 if(xA>Spr[0]){ Spr[0]=xA; Spr[1]=yA; } if(xA<Slv[0]){ Slv[0]=xA; Slv[1]=yA; }
 if(xB>Spr[0]){ Spr[0]=xB; Spr[1]=yB; } if(xB<Slv[0]){ Slv[0]=xB; Slv[1]=yB; }
 if(xC>Spr[0]){ Spr[0]=xC; Spr[1]=yC; } if(xC<Slv[0]){ Slv[0]=xC; Slv[1]=yC; }
 if(yA>Max[1]){ Max[1]=yA; Max[0]=xA; } if(yA<Min[1]){ Min[1]=yA; Min[0]=xA; }
 if(yB>Max[1]){ Max[1]=yB; Max[0]=xB; } if(yB<Min[1]){ Min[1]=yB; Min[0]=xB; }
 if(yC>Max[1]){ Max[1]=yC; Max[0]=xC; } if(yC<Min[1]){ Min[1]=yC; Min[0]=xC; }
 return P;
}                                                                              
//---------------------------------------------------------------------------
Parm *__fastcall TMultFilms::GabaritCentroids(Parm *P){
 double xA,yA,fi,XP,YP,xp,yp,VxA,VyA,W; int Ind; wchar_t Name[256],*n=Name; Centroid *c;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); xA=Vals[Ind]; }
 else if(P->F.C&&(P->F.C!=Pust)) xA=Val(P->F);  else xA=P->Val;
 n+=swprintf(Name,P->Nam); P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); yA=Vals[Ind]; }
 else if(P->F.C&&(P->F.C!=Pust)) yA=Val(P->F);  else yA=P->Val;
 n+=swprintf(n,L"_%s",P->Nam); P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); fi=Vals[Ind]; }
 else if(P->F.C&&(P->F.C!=Pust)) fi=Val(P->F);  else fi=P->Val;
 swprintf(n,L"_%s",P->Nam); P=P->Sled; P=P->Sled; P=P->Sled; P=P->Sled;
 FindCentroid(Rcentr,Name,&c);
 VxA=Val(c->FVxA); VyA=Val(c->FVyA); W=Val(c->FWfi);
 c->XP[Nkadr]=XP=xA-Val(c->FVyA/c->FWfi);
 c->YP[Nkadr]=YP=yA+Val(c->FVxA/c->FWfi);
 c->xp[Nkadr]=xp=(XP-xA)*cos(fi)+(YP-yA)*sin(fi);
 c->yp[Nkadr]=yp=-(XP-xA)*sin(fi)+(YP-yA)*cos(fi);
 return P;
}
//---------------------------------------------------------------------------
Parm *__fastcall TMultFilms::DrawFixedCentroids(TCanvas *Canvas,Parm *P){
 int W,w=Canvas->Pen->Width,X,Y,KSectors=24,Sector; wchar_t Name[256];
 Centroid *c; Vary *V; TColor Cf,Cm,Cp=Canvas->Pen->Color;
 swprintf(Name,L"%s_%s_%s",P->Nam,P->Sled->Nam,P->Sled->Sled->Nam);
 FindCentroid(Rcentr,Name,&c); Sector=(c->Kt-1)/KSectors+1; P=P->Sled->Sled->Sled;
 W=Round(P->Nam?FindVary(P->Nam,&V),Val(V->Znach):
    (P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val);
 P=P->Sled;
 Cf=(TColor)Round(P->Nam?FindVary(P->Nam,&V),Val(V->Znach):
     (P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val);
 P=P->Sled;
 Cm=(TColor)Round(P->Nam?FindVary(P->Nam,&V),Val(V->Znach):
  (P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val);
 P=P->Sled;
 Canvas->Pen->Width=W; Canvas->Pen->Color=Cf;
 X=Round((c->XP[0]-xk)/MSt[0])+Xk; Y=Yk-Round((c->YP[0]-yk)/MSt[1]);
 Canvas->MoveTo(X,Y);
 for(int i=1;i<c->Kt;i++){
  if(!(i%Sector)) Canvas->Pen->Color=(Canvas->Pen->Color==Cf)?Cm:Cf;
  X=Round((c->XP[i]-xk)/MSt[0])+Xk; Y=Yk-Round((c->YP[i]-yk)/MSt[1]);
  Canvas->LineTo(X,Y);
 }
 Canvas->Pen->Width=w; Canvas->Pen->Color=Cp;
 return P;
}
//---------------------------------------------------------------------------
Parm *__fastcall TMultFilms::DrawMobileCentroids(TCanvas *Canvas,Parm *P){
 int W,w=Canvas->Pen->Width,X,Y,KSectors=24,Sector,Ind; wchar_t Name[256];
 Centroid *c; Vary *V; TColor Cb=Canvas->Brush->Color,Cf,Cm,Cp=Canvas->Pen->Color;
 double x,y,xA,yA,fi;
 swprintf(Name,L"%s_%s_%s",P->Nam,P->Sled->Nam,P->Sled->Sled->Nam);
 FindCentroid(Rcentr,Name,&c); Sector=(c->Kt-1)/KSectors+1;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); xA=Vals[Ind]; }
 else if(P->F.C&&(P->F.C!=Pust)) xA=Val(P->F);  else xA=P->Val;
 P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); yA=Vals[Ind]; }
 else if(P->F.C&&(P->F.C!=Pust)) yA=Val(P->F);  else yA=P->Val;
 P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); fi=Vals[Ind]; }
 else if(P->F.C&&(P->F.C!=Pust)) fi=Val(P->F);  else fi=P->Val;
 P=P->Sled;
 W=Round(P->Nam?FindVary(P->Nam,&V),Val(V->Znach):
    (P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val);
 P=P->Sled;
 Cf=(TColor)Round(P->Nam?FindVary(P->Nam,&V),Val(V->Znach):
     (P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val);
 P=P->Sled;
 Cm=(TColor)Round(P->Nam?FindVary(P->Nam,&V),Val(V->Znach):
     (P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val);
 P=P->Sled;
 Canvas->Pen->Width=W; Canvas->Pen->Color=Cf;
 x=xA+c->xp[0]*cos(fi)-c->yp[0]*sin(fi);
 y=yA+c->xp[0]*sin(fi)+c->yp[0]*cos(fi);
 X=Round((x-xk)/MSt[0])+Xk; Y=Yk-Round((y-yk)/MSt[1]);
 Canvas->MoveTo(X,Y);
 for(int i=1;i<c->Kt;i++){
  if(!(i%Sector))
   Canvas->Pen->Color=(Canvas->Pen->Color==Cf)?Cm:Cf;
  x=xA+c->xp[i]*cos(fi)-c->yp[i]*sin(fi);
  y=yA+c->xp[i]*sin(fi)+c->yp[i]*cos(fi);
  X=Round((x-xk)/MSt[0])+Xk; Y=Yk-Round((y-yk)/MSt[1]);
  Canvas->LineTo(X,Y);
 }
 Canvas->Pen->Width=w; Canvas->Pen->Color=Cp;
 return P;
}
//---------------------------------------------------------------------------
Parm *__fastcall TMultFilms::ParmBar(Parm *P,TPoint *tP,int &Width,
  TColor &ColorL,TColor &ColorF){ Vary *V;
 double xC,yC,fi,h,g,Sfi,Cfi,xA,yA,xB,yB,xD,yD,xE,yE,gCfi,gSfi,hCfi,hSfi; int Ind;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); xC=Vals[Ind]; }
 else if(P->F.C&&(P->F.C!=Pust)) xC=Val(P->F);  else xC=P->Val;
 P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); yC=Vals[Ind]; }
 else if(P->F.C&&(P->F.C!=Pust)) yC=Val(P->F);  else yC=P->Val;
 P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); fi=Vals[Ind]; }
 else if(P->F.C&&(P->F.C!=Pust)) fi=Val(P->F);  else fi=P->Val;
 P=P->Sled; Sfi=sin(fi); Cfi=cos(fi);
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); h=Vals[Ind]; }
 else if(P->F.C&&(P->F.C!=Pust)) h=Val(P->F);  else h=P->Val;
 P=P->Sled; hCfi=h/2*Cfi; hSfi=h/2*Sfi;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); g=Vals[Ind]; }
 else if(P->F.C&&(P->F.C!=Pust)) g=Val(P->F);  else g=P->Val;
 P=P->Sled; gCfi=g/2*Cfi; gSfi=g/2*Sfi;
 Width=Round(P->Nam?FindVary(P->Nam,&V),Val(V->Znach):
        (P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val);
 if(Width<0) Width=1; P=P->Sled;
 ColorL=(TColor)Round(P->Nam?FindVary(P->Nam,&V),Val(V->Znach):
         (P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val);
 if(ColorL<0) ColorL=*ColorGraph;
 P=P->Sled;
 ColorF=(TColor)Round(P->Nam?FindVary(P->Nam,&V),Val(V->Znach):
  P->F.C&&(P->F.C!=Pust) ? Val(P->F) : P->Val);
 P=P->Sled;
 xA=xC+gCfi-hSfi; yA=yC+gSfi+hCfi; xB=xC-gCfi-hSfi; yB=yC-gSfi+hCfi;
 xE=xC+gCfi+hSfi; yE=yC+gSfi-hCfi; xD=xC-gCfi+hSfi; yD=yC-gSfi-hCfi;
 tP[0].x=Round((xA-xk)/MSt[0])+Xk; tP[0].y=Yk-Round((yA-yk)/MSt[1]);
 tP[1].x=Round((xB-xk)/MSt[0])+Xk; tP[1].y=Yk-Round((yB-yk)/MSt[1]);
 tP[2].x=Round((xD-xk)/MSt[0])+Xk; tP[2].y=Yk-Round((yD-yk)/MSt[1]);
 tP[3].x=Round((xE-xk)/MSt[0])+Xk; tP[3].y=Yk-Round((yE-yk)/MSt[1]);
 return P;
}
//---------------------------------------------------------------------------
Parm *__fastcall TMultFilms::DrawBar(TCanvas *Canvas,Parm *P){
 int XC,YC,H,G,W; TColor CL,CF,Cp=Canvas->Pen->Color,Cb=Canvas->Brush->Color;
 TPoint tP[4];
 P=ParmBar(P,tP,W,CL,CF);
 Canvas->Pen->Color=CL; Canvas->Pen->Width=W;
 Canvas->Brush->Color=CF>-1?CF:*ColorPhon;
 if(CF<0){
  Canvas->MoveTo(tP[0].x,tP[0].y); Canvas->LineTo(tP[1].x,tP[1].y);
  Canvas->LineTo(tP[2].x,tP[2].y); Canvas->LineTo(tP[3].x,tP[3].y);
  Canvas->LineTo(tP[0].x,tP[0].y);
 }
 else Canvas->Polygon(tP,3);
 Canvas->Pen->Width=*WidthPen; Canvas->Pen->Color=Cp; Canvas->Brush->Color=Cb;
 return P;
}
//---------------------------------------------------------------------------


Parm *__fastcall TMultFilms::ParmTriangle(Parm *P,TPoint *tP,int &Width,
  TColor &ColorL,TColor &ColorF){ Vary *V;
 double xC,yC,xA,yA,xB,yB; int Ind;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); xA=Vals[Ind]; }
 else if(P->F.C&&(P->F.C!=Pust)) xA=Val(P->F);  else xA=P->Val;
 P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); yA=Vals[Ind]; }
 else if(P->F.C&&(P->F.C!=Pust)) yA=Val(P->F);  else yA=P->Val;
 P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); xB=Vals[Ind]; }
 else if(P->F.C&&(P->F.C!=Pust)) xB=Val(P->F);  else xB=P->Val;
 P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); yB=Vals[Ind]; }
 else if(P->F.C&&(P->F.C!=Pust)) yB=Val(P->F);  else yB=P->Val;
 P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); xC=Vals[Ind]; }
 else if(P->F.C&&(P->F.C!=Pust)) xC=Val(P->F);  else xC=P->Val;
 P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); yC=Vals[Ind]; }
 else if(P->F.C&&(P->F.C!=Pust)) yC=Val(P->F);  else yC=P->Val;
 P=P->Sled;
 Width=Round(P->Nam?FindVary(P->Nam,&V),Val(V->Znach):
       (P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val);
 if(Width<0) Width=1; P=P->Sled;
 ColorL=(TColor)Round(P->Nam?FindVary(P->Nam,&V),Val(V->Znach):
         (P->F.C&&P->F.C!=Pust)?Val(P->F):P->Val);
 if(ColorL<0) ColorL=*ColorGraph;
 P=P->Sled;
 ColorF=(TColor)Round(P->Nam?FindVary(P->Nam,&V),Val(V->Znach):
  P->F.C&&(P->F.C!=Pust) ? Val(P->F) : P->Val);
 P=P->Sled;
 tP[0].x=Round((xA-xk)/MSt[0])+Xk; tP[0].y=Yk-Round((yA-yk)/MSt[1]);
 tP[1].x=Round((xB-xk)/MSt[0])+Xk; tP[1].y=Yk-Round((yB-yk)/MSt[1]);
 tP[2].x=Round((xC-xk)/MSt[0])+Xk; tP[2].y=Yk-Round((yC-yk)/MSt[1]);
 return P;
}
//---------------------------------------------------------------------------
Parm *__fastcall TMultFilms::DrawTriangle(TCanvas *Canvas,Parm *P){
 int W; TColor CL,CF,Cp=Canvas->Pen->Color,Cb=Canvas->Brush->Color; TPoint tP[3];
 P=ParmTriangle(P,tP,W,CL,CF);
 Canvas->Pen->Color=CL; Canvas->Pen->Width=W;
 Canvas->Brush->Color=CF>-1?CF:*ColorPhon;
 if(CF<0){
  Canvas->MoveTo(tP[0].x,tP[0].y); Canvas->LineTo(tP[1].x,tP[1].y);
  Canvas->LineTo(tP[2].x,tP[2].y); Canvas->LineTo(tP[0].x,tP[0].y);
 }
 else Canvas->Polygon(tP,2);
 Canvas->Pen->Width=*WidthPen; Canvas->Pen->Color=Cp; Canvas->Brush->Color=Cb;
 return P;
}
//---------------------------------------------------------------------------
/*
Parm *__fastcall TMultFilms::ClearBar(TCanvas *Canvas,Parm *P){
 int Ind,XC,YC,H,G,W; TColor CL,CF; TPoint tP[4];
 P=ParmBar(P,tP,W,CL,CF);
 Canvas->Pen->Color=*ColorPhon; Canvas->Brush->Color=*ColorPhon;
 Canvas->Pen->Width=W;
 Canvas->Polygon(tP,3);
 return P;
}*/
//---------------------------------------------------------------------------
void __fastcall TMultFilms::DefMSTMultFilm(TCanvas *Canvas){
 double Ps,Diag; Parm *p; Cord *q; bool Int=Q?false:true;
 int i,Wgo=Wg; Kt=(*Tmult)*(*Kkadr)/1000; dt=(Tend-Tn)/Kt;
M:Min[0]=Min[1]=Slv[0]=MAXDOUBLE; Max[0]=Max[1]=Spr[0]=-MAXDOUBLE;
 Spr[1]=Slv[1]=0.0;
 if(CGauge){
  CGauge->MaxValue=Kt; CGauge->ForeColor=*ColorGraph; CGauge->Progress=0;
  CGauge->Visible=true;
 }
 StatusBar->SimpleText=L"Расчет масштабов";
 L->Time->Val=Tn;
 for(i=0,q=L->K;q&&q0;i++,q=q->Sled){
  q->V->Val=q0[i]; DsetaT[i].V->Val=Vq0[i];
 }
 for(i=0,q=L->W;q;q=q->Sled,i++) q->V->Val=w0[i];
 for(i=0,q=L->Z;q;q=q->Sled,i++) q->V->Val=z0[i];
 t=Tn;  Nkadr=0; FuncsQ(t,NamesVarys,Vals,Int);
 if(!*NoCalc){
  for(p=ParmF;p;p=p->Sled){
   if(!wcscmp(p->Nam,            L"ВЕКТОР"))  {GabaritVector(p->R);      continue;}
   if(!wcscmp(p->Nam,             L"ВОРОТ"))  {GabaritVorot(p->R);       continue;}
   if(!wcscmp(p->Nam,    L"ВОРОТ С ГРУЗОМ"))  {GabaritGateWithLoad(p->R);continue;}
   if(!wcscmp(p->Nam,              L"ДУГА"))  {GabaritArc(p->R);         continue;}
   if(!wcscmp(p->Nam,      L"ДУГА ЭЛЛИПСА"))  {GabaritArcEllips(p->R);   continue;}
   if(!wcscmp(p->Nam,            L"КОЛЕСО"))  {GabaritKoleso(p->R);      continue;}
   if(!wcscmp(p->Nam,      L"МНОГОЗВЕННИК"))  {GabaritMultiLink(p->R);   continue;}
   if(!wcscmp(p->Nam,         L"ПОЛИГОН3D"))  {GabaritPolygon3d(p->R);   continue;}
   if(!wcscmp(p->Nam,        L"ОКРУЖНОСТЬ")||
      !wcscmp(p->Nam,              L"КРУГ"))  {GabaritCircle(p->R);      continue;}
   if(!wcscmp(p->Nam,      L"ОКРУЖНОСТЬ3D")||
      !wcscmp(p->Nam,            L"КРУГ3D"))  {GabaritCircle3d(p->R);    continue;}
   if(!wcscmp(p->Nam,
                   L"ПЕРЕДАЧА КЛИНОРЕМЕННАЯ")){GabaritGear(p->R);        continue;}
   if(!wcscmp(p->Nam,           L"ПРУЖИНА"))  {GabaritSpring(p->R);      continue;}
   if(!wcscmp(p->Nam,            L"ПРЯМАЯ"))  {GabaritLine(p->R);        continue;}
   if(!wcscmp(p->Nam,     L"ПРЯМОУГОЛЬНИК"))  {GabaritBar(p->R);         continue;}
   if(!wcscmp(p->Nam,             L"РЕЙКА"))  {GabaritRejka(p->R);       continue;}
   if(!wcscmp(p->Nam,            L"СЕКТОР"))  {GabaritSector(p->R);      continue;}
   if(!wcscmp(p->Nam,          L"СКОРОСТЬ"))  {GabaritSpeed(p->R);       continue;}
   if(!wcscmp(p->Nam,            L"СПЛАЙН"))  {GabaritSpline(p->R);      continue;}
   if(!wcscmp(p->Nam,          L"СТЕРЖЕНЬ"))  {GabaritString(p->R);      continue;}
   if(!wcscmp(p->Nam,           L"СТРЕЛКА"))  {GabaritFinger(p->R);      continue;}
   if(!wcscmp(p->Nam,             L"ТОЧКА"))  {GabaritPoint(p->R);       continue;}
   if(!wcscmp(p->Nam,        L"ТРАЕКТОРИЯ"))  {GabaritTrajectory(p->R);  continue;}
   if(!wcscmp(p->Nam,      L"ТРАЕКТОРИЯ3D"))  {GabaritTrajectory3d(p->R);continue;}
   if(!wcscmp(p->Nam,       L"ТРЕУГОЛЬНИК"))  {GabaritTriangle(p->R);    continue;}
   if(!wcscmp(p->Nam,L"УСКОРЕНИЕ КАСАТЕЛЬНОЕ")||
!wcscmp(p->Nam,L"УСКОРЕНИЕ ТАНГЕНЦИАЛЬНОЕ"))  {GabaritAtang(p->R);       continue;}
   if(!wcscmp(p->Nam,L"УСКОРЕНИЕ НОРМАЛЬНОЕ")){GabaritAnorm(p->R);       continue;}
   if(!wcscmp(p->Nam,L"ШАРНИР НЕПОДВИЖНЫЙ"))  {GabaritHinge(p->R,false); continue;}
   if(!wcscmp(p->Nam,  L"ШАРНИР ПОДВИЖНЫЙ"))  {GabaritHinge(p->R,true);  continue;}
   if(!wcscmp(p->Nam,            L"ЭЛЛИПС"))  {GabaritEllips(p->R);      continue;}
   if(!wcscmp(p->Nam,     L"МАНИПУЛЯТОР3D"))  {GabaritManipulator3d(p->R);continue;}
 }}
 for(Nkadr=0;(Nkadr<=Kt)&&(!EndInt());Nkadr++){
  t=Tn+Nkadr*dt;
  if(CGauge) CGauge->Progress++;
//  else Termometr->StepIn();
  FuncsQ(t,NamesVarys,Vals,Int);
//  FuncsSpln(t,NamesVarys,Vals);
  if(*NoCalc){ Tend=t; break; }
  for(p=ParmMF;p;p=p->Sled){
   if(!wcscmp(p->Nam,            L"ВЕКТОР"))  {GabaritVector(p->R);      continue;}
   if(!wcscmp(p->Nam,             L"ВОРОТ"))  {GabaritVorot(p->R);       continue;}
   if(!wcscmp(p->Nam,    L"ВОРОТ С ГРУЗОМ"))  {GabaritGateWithLoad(p->R);continue;}
   if(!wcscmp(p->Nam,              L"ДУГА"))  {GabaritArc(p->R);         continue;}
   if(!wcscmp(p->Nam,      L"ДУГА ЭЛЛИПСА"))  {GabaritArcEllips(p->R);   continue;}
   if(!wcscmp(p->Nam,            L"КОЛЕСО"))  {GabaritKoleso(p->R);      continue;}
   if(!wcscmp(p->Nam,      L"МНОГОЗВЕННИК"))  {GabaritMultiLink(p->R);   continue;}
   if(!wcscmp(p->Nam,         L"ПОЛИГОН3D"))  {GabaritPolygon3d(p->R);   continue;}
   if(!wcscmp(p->Nam,        L"ОКРУЖНОСТЬ")||
      !wcscmp(p->Nam,              L"КРУГ"))  {GabaritCircle(p->R);      continue;}
   if(!wcscmp(p->Nam,      L"ОКРУЖНОСТЬ3D")||
      !wcscmp(p->Nam,            L"КРУГ3D"))  {GabaritCircle3d(p->R);    continue;}
   if(!wcscmp(p->Nam,
                 L"ПЕРЕДАЧА КЛИНОРЕМЕННАЯ"))  {GabaritGear(p->R);        continue;}
   if(!wcscmp(p->Nam,           L"ПРУЖИНА"))  {GabaritSpring(p->R);      continue;}
   if(!wcscmp(p->Nam,            L"ПРЯМАЯ"))  {GabaritLine(p->R);        continue;}
   if(!wcscmp(p->Nam,     L"ПРЯМОУГОЛЬНИК"))  {GabaritBar(p->R);         continue;}
   if(!wcscmp(p->Nam,             L"РЕЙКА"))  {GabaritRejka(p->R);       continue;}
   if(!wcscmp(p->Nam,             L"ТОЧКА"))  {GabaritPoint(p->R);       continue;}
   if(!wcscmp(p->Nam,          L"СКОРОСТЬ"))  {GabaritSpeed(p->R);       continue;}
   if(!wcscmp(p->Nam,            L"СЕКТОР"))  {GabaritSector(p->R);      continue;}
   if(!wcscmp(p->Nam,            L"СПЛАЙН"))  {GabaritSpline(p->R);      continue;}
   if(!wcscmp(p->Nam,          L"СТЕРЖЕНЬ"))  {GabaritString(p->R);      continue;}
   if(!wcscmp(p->Nam,           L"СТРЕЛКА"))  {GabaritFinger(p->R);      continue;}
   if(!wcscmp(p->Nam,             L"ТОЧКА"))  {GabaritPoint(p->R);       continue;}
   if(!wcscmp(p->Nam,        L"ТРАЕКТОРИЯ"))  {GabaritTrajectory(p->R);  continue;}
   if(!wcscmp(p->Nam,      L"ТРАЕКТОРИЯ3D"))  {GabaritTrajectory3d(p->R);continue;}
   if(!wcscmp(p->Nam,       L"ТРЕУГОЛЬНИК"))  {GabaritTriangle(p->R);    continue;}
   if(!wcscmp(p->Nam,L"УСКОРЕНИЕ КАСАТЕЛЬНОЕ")||
  !wcscmp(p->Nam,L"УСКОРЕНИЕ ТАНГЕНЦИАЛЬНОЕ")){GabaritAtang(p->R);       continue;}
   if(!wcscmp(p->Nam,L"УСКОРЕНИЕ НОРМАЛЬНОЕ")){GabaritAnorm(p->R);       continue;}
   if(!wcscmp(p->Nam,         L"ЦЕНТРОИДЫ"))  {GabaritCentroids(p->R);   continue;}
   if(!wcscmp(p->Nam,L"ШАРНИР НЕПОДВИЖНЫЙ"))  {GabaritHinge(p->R,false); continue;}
   if(!wcscmp(p->Nam,  L"ШАРНИР ПОДВИЖНЫЙ"))  {GabaritHinge(p->R,true);  continue;}
   if(!wcscmp(p->Nam,            L"ЭЛЛИПС"))  {GabaritEllips(p->R);      continue;}
   if(!wcscmp(p->Nam,     L"МАНИПУЛЯТОР3D"))  {GabaritManipulator3d(p->R);continue;}
 }}
 Tend=t;
// if(i<=Kt){ Tend=t-dt; goto M; }
//Найдем масштабы
 MSt[0]=(Spr[0]-Slv[0])/(KP[0]-1);
 MSt[1]=(Max[1]-Min[1])/(KP[1]-1);
 if(MSt[0]==0.0) MSt[0]=MSt[1];
 if(MSt[1]==0.0) MSt[1]=MSt[0];
 if((MSt[0]==0.0)||(MSt[1]==0.0)) return;
 for(int i=0;i<2;i++){
  for(Ps=0;MSt[i]>10.0;Ps++) MSt[i]/=10.0;
  for(;MSt[i]<1.0;Ps--) MSt[i]*=10.0;
  for(int j=1;j<10;j++) if(MSt[i]<=MG[j]){ MSt[i]=MG[j]; break; }
  for(int j=abs(Ps);j--;)
   if(Ps>0) MSt[i]*=10; else MSt[i]/=10;
 }
 if(MSt[0]>MSt[1]) MSt[1]=MSt[0]; else if(MSt[1]>MSt[0]) MSt[0]=MSt[1];
 for(int i=0;i<2;i++){ MstT[i]=MSt[i]/Sg; MSt[i]/=dS[i];}
 swprintf(Inf,L"%s: %s",HeadWork,Autor); StatusBar->SimpleText=Inf;
 if(CGauge){ CGauge->Progress=0; CGauge->Visible=false; }
// else{ Termometr->Pos=0; Termometr->Min=0; }
//для графика
 Wg=1+Round((Spr[0]-Slv[0])/MSt[0]); Xk+=(Wgo-Wg)/2;
//формулы преобразования координат
//координаты начала кадра в абсолютной системе
 xk=(Spr[0]+Slv[0]-(Wg-1)*MSt[0])/2; yk=(Max[1]+Min[1]-(Hg-1)*MSt[1])/2;
 for(Trajectory *tr=Rtraek;tr;tr=tr->Sled){
  for(int i=0;i<=Kt;){
   tr->X[i]=Round((tr->x[i]-xk)/MSt[0])+Xk;
   tr->Y[i]=Yk-Round((tr->y[i]-yk)/MSt[1]);
   i++;
}}}
//---------------------------------------------------------------------------
double __fastcall TMultFilms::ZPolygon3d(Parm *P){
 int Ind; double z;
 for(int i=0;i<4;i++) P=P->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); z=Vals[Ind]; }
 else if(P->F.C&&(P->F.C!=Pust)) z=Val(P->F);  else z=P->Val;
 return z;
}
//---------------------------------------------------------------------------
double __fastcall TMultFilms::ZCircle3d(Parm *P){
 int Ind; double z;
 P=P->Sled->Sled;
 if(P->Nam){ Ind=NStrMagCmp(NamesVarys,P->Nam); z=Vals[Ind]; }
 else if(P->F.C&&(P->F.C!=Pust)) z=Val(P->F);  else z=P->Val;
 return z;
}
//---------------------------------------------------------------------------
int CmpPrim(const void *G1,const void *G2){
 Parm **p1=(Parm **)G1,**p2=(Parm **)G2;
 if((*p1)->Val>(*p2)->Val) return  1;
 if((*p1)->Val<(*p2)->Val) return -1;
 return 0;
}
//---------------------------------------------------------------------------
void __fastcall TMultFilms::SortZ(void){ Parm **Mmf; int K;
 for(Parm *p=ParmMF;p;p=p->Sled){
  if(!wcscmp(p->Nam,      L"ОКРУЖНОСТЬ3D")||
	 !wcscmp(p->Nam,            L"КРУГ3D")){ p->Val=ZCircle3d(p->R);  continue;}
  if(!wcscmp(p->Nam,         L"ПОЛИГОН3D")){ p->Val=ZPolygon3d(p->R); continue;}
  if(!wcscmp(p->Nam,      L"ТРАЕКТОРИЯ3D")){ p->Val=ZCircle3d(p->R);  continue;}
 }
 K=KolElem(ParmMF);
 if(Mmf=(Parm **)LtoM(ParmMF)){
  qsort(Mmf,K,SzV,CmpPrim);
  MtoL((void **)Mmf,K); Irez->First->Sled=ParmMF=*Mmf; free(Mmf);
}}
//---------------------------------------------------------------------------
void __fastcall TMultFilms::DrawKadr(TCanvas *Canvas){
 Parm *p;
 ClearVar();
 if(!*NoCalc){
  if(ANIMATION3D) SortZ();
  for(p=ParmMF;p;p=p->Sled){
   if(!wcscmp(p->Nam,            L"ВЕКТОР")){DrawVector(Canvas,p->R);          continue;}
   if(!wcscmp(p->Nam,             L"ВОРОТ")){DrawVorot(Canvas,p->R);           continue;}
   if(!wcscmp(p->Nam,    L"ВОРОТ С ГРУЗОМ")){DrawGateWithLoad(Canvas,p->R);    continue;}
   if(!wcscmp(p->Nam,              L"ДУГА")){DrawArc(Canvas,p->R);             continue;}
   if(!wcscmp(p->Nam,      L"ДУГА ЭЛЛИПСА")){DrawArcEllips(Canvas,p->R);       continue;}
   if(!wcscmp(p->Nam,            L"КОЛЕСО")){DrawKoleso(Canvas,p->R);          continue;}
   if(!wcscmp(p->Nam,      L"МНОГОЗВЕННИК")){DrawMultiLink(Canvas,p->R);       continue;}
   if(!wcscmp(p->Nam,         L"ПОЛИГОН3D")){DrawPolygon3d(Canvas,p->R);       continue;}
   if(!wcscmp(p->Nam,        L"ОКРУЖНОСТЬ")||
	  !wcscmp(p->Nam,              L"КРУГ")){DrawCircle(Canvas,p->R);          continue;}
   if(!wcscmp(p->Nam,      L"ОКРУЖНОСТЬ3D")||
	  !wcscmp(p->Nam,            L"КРУГ3D")){DrawCircle3d(Canvas,p->R);        continue;}
   if(!wcscmp(p->Nam,
				 L"ПЕРЕДАЧА КЛИНОРЕМЕННАЯ")){DrawGear(Canvas,p->R);            continue;}
   if(!wcscmp(p->Nam,           L"ПРУЖИНА")){DrawSpring(Canvas,p->R);          continue;}
   if(!wcscmp(p->Nam,            L"ПРЯМАЯ")){DrawLine(Canvas,p->R);            continue;}
   if(!wcscmp(p->Nam,     L"ПРЯМОУГОЛЬНИК")){DrawBar(Canvas,p->R);             continue;}
   if(!wcscmp(p->Nam,             L"РЕЙКА")){DrawRejka(Canvas,p->R);           continue;}
   if(!wcscmp(p->Nam,            L"СЕКТОР")){DrawSector(Canvas,p->R);          continue;}
   if(!wcscmp(p->Nam,          L"СКОРОСТЬ")){DrawSpeed(Canvas,p->R);           continue;}
   if(!wcscmp(p->Nam,            L"СПЛАЙН")){DrawSpline(Canvas,p->R);          continue;}
   if(!wcscmp(p->Nam,          L"СТЕРЖЕНЬ")){DrawString(Canvas,p->R);          continue;}
   if(!wcscmp(p->Nam,           L"СТРЕЛКА")){DrawFinger(Canvas,p->R);          continue;}
   if(!wcscmp(p->Nam,             L"ТЕКСТ")){DrawText(Canvas,p->R);            continue;}
   if(!wcscmp(p->Nam,             L"ТОЧКА")){DrawPoint(Canvas,p->R);           continue;}
   if(!wcscmp(p->Nam,        L"ТРАЕКТОРИЯ")){DrawTrajectory(Canvas,p->R);      continue;}
   if(!wcscmp(p->Nam,      L"ТРАЕКТОРИЯ3D")){DrawTrajectory3d(Canvas,p->R);    continue;}
   if(!wcscmp(p->Nam,       L"ТРЕУГОЛЬНИК")){DrawTriangle(Canvas,p->R);        continue;}
   if(!wcscmp(p->Nam,         L"ЦЕНТРОИДЫ")){DrawMobileCentroids(Canvas,p->R); continue;}
   if(!wcscmp(p->Nam,L"УСКОРЕНИЕ КАСАТЕЛЬНОЕ")||
	  !wcscmp(p->Nam,
			   L"УСКОРЕНИЕ ТАНГЕНЦИАЛЬНОЕ")){DrawAtang(Canvas,p->R);           continue;}
   if(!wcscmp(p->Nam,
				   L"УСКОРЕНИЕ НОРМАЛЬНОЕ")){DrawAnorm(Canvas,p->R);           continue;}
   if(!wcscmp(p->Nam,L"ШАРНИР НЕПОДВИЖНЫЙ")){DrawHinge(Canvas,p->R,false);     continue;}
   if(!wcscmp(p->Nam,  L"ШАРНИР ПОДВИЖНЫЙ")){DrawHinge(Canvas,p->R,true);      continue;}
   if(!wcscmp(p->Nam,            L"ЭЛЛИПС")){DrawEllips(Canvas,p->R);          continue;}
   if(!wcscmp(p->Nam,     L"МАНИПУЛЯТОР3D")){DrawManipulator3d(Canvas,p->R);   continue;}
}}}
//---------------------------------------------------------------------------
void __fastcall TMultFilms::DrawPhon(TCanvas *Canvas){ Parm *p;
 for(p=ParmF;p;p=p->Sled){
  LowToUpp(p->Nam);
  if(!wcscmp(p->Nam,            L"ВЕКТОР")){DrawVector(Canvas,p->R);      continue;}
  if(!wcscmp(p->Nam,    L"ВОРОТ С ГРУЗОМ")){DrawGateWithLoad(Canvas,p->R);continue;}
  if(!wcscmp(p->Nam,              L"ДУГА")){DrawArc(Canvas,p->R);         continue;}
  if(!wcscmp(p->Nam,      L"ДУГА ЭЛЛИПСА")){DrawArcEllips(Canvas,p->R);   continue;}
  if(!wcscmp(p->Nam,            L"КОЛЕСО")){DrawKoleso(Canvas,p->R);      continue;}
  if(!wcscmp(p->Nam,      L"МНОГОЗВЕННИК")){DrawMultiLink(Canvas,p->R);   continue;}
  if(!wcscmp(p->Nam,        L"ОКРУЖНОСТЬ")||
     !wcscmp(p->Nam,              L"КРУГ")){DrawCircle(Canvas,p->R);      continue;}
  if(!wcscmp(p->Nam,L"ПЕРЕДАЧА КЛИНОРЕМЕННАЯ")){DrawGear(Canvas,p->R);    continue;}
  if(!wcscmp(p->Nam,           L"ПРУЖИНА")){DrawSpring(Canvas,p->R);      continue;}
  if(!wcscmp(p->Nam,            L"ПРЯМАЯ")){DrawLine(Canvas,p->R);        continue;}
  if(!wcscmp(p->Nam,     L"ПРЯМОУГОЛЬНИК")){DrawBar(Canvas,p->R);         continue;}
  if(!wcscmp(p->Nam,             L"РЕЙКА")){DrawRejka(Canvas,p->R);       continue;}
  if(!wcscmp(p->Nam,            L"СЕКТОР")){DrawSector(Canvas,p->R);      continue;}
  if(!wcscmp(p->Nam,          L"СКОРОСТЬ")){DrawSpeed(Canvas,p->R);       continue;}
  if(!wcscmp(p->Nam,            L"СПЛАЙН")){DrawSpline(Canvas,p->R);      continue;}
  if(!wcscmp(p->Nam,          L"СТЕРЖЕНЬ")){DrawString(Canvas,p->R);      continue;}
  if(!wcscmp(p->Nam,           L"СТРЕЛКА")){DrawFinger(Canvas,p->R);      continue;}
  if(!wcscmp(p->Nam,             L"ТЕКСТ")){DrawText(Canvas,p->R);        continue;}
  if(!wcscmp(p->Nam,             L"ТОЧКА")){DrawPoint(Canvas,p->R);       continue;}
  if(!wcscmp(p->Nam,       L"ТРЕУГОЛЬНИК")){DrawTriangle(Canvas,p->R);    continue;}
  if(!wcscmp(p->Nam,L"УСКОРЕНИЕ КАСАТЕЛЬНОЕ")||
     !wcscmp(p->Nam,L"УСКОРЕНИЕ ТАНГЕНЦИАЛЬНОЕ")){
                                           DrawAtang(Canvas,p->R);       continue;}
  if(!wcscmp(p->Nam,L"УСКОРЕНИЕ НОРМАЛЬНОЕ")){
                                           DrawAnorm(Canvas,p->R);       continue;}
  if(!wcscmp(p->Nam,L"ШАРНИР НЕПОДВИЖНЫЙ")){DrawHinge(Canvas,p->R,false); continue;}
  if(!wcscmp(p->Nam,  L"ШАРНИР ПОДВИЖНЫЙ")){DrawHinge(Canvas,p->R,true);  continue;}
  if(!wcscmp(p->Nam,            L"ЭЛЛИПС")){DrawEllips(Canvas,p->R);      continue;}
  if(!wcscmp(p->Nam,    L"МАНИПУЛЯТОР3D")){DrawManipulator3d(Canvas,p->R);continue;}
 }
 for(p=ParmMF;p;p=p->Sled){
  if(!wcscmp(p->Nam,L"ЦЕНТРОИДЫ")){ DrawFixedCentroids(Canvas,p->R);   continue;}
}}
//---------------------------------------------------------------------------
void __fastcall TMultFilms::PrepMultFilm(TImage *Image){
 t=Tn;
 Image->Canvas->Pen->Color=*ColorGraph;
 PhonImage->Top=Image->Top; PhonImage->Left=Image->Left;
 PhonImage->Width=Image->Width; PhonImage->Height=Image->Height;
 PhonImage->Canvas->Rectangle(PhonImage->ClientRect);
 PhonImage->Visible=false;
 DrawSetka(PhonImage->Canvas);
 DrawHead(PhonImage->Canvas);
 DrawPhon(PhonImage->Canvas); t=Tn;
 Image->Canvas->CopyRect(Image->ClientRect,PhonImage->Canvas,PhonImage->ClientRect);
 DrawHead(Image->Canvas);
 Nkadr=0;
 DrawKadr(Image->Canvas);
 if(*TOBMP){
  if(Kinst>1){
   swprintf(Dir,L"%s\\%s(№%d)",CurrDir,BMPKadr,Kinst);
   if(_waccess(Dir,0)) _wmkdir(Dir);
   swprintf(BMPKadrs,L"%s\\%s(№%d)\\%3.3d.bmp",CurrDir,BMPKadr,Kinst,Nkadr);
  }
  else{
   swprintf(Dir,L"%s\\%s",CurrDir,BMPKadr);
   if(_waccess(Dir,0)) _wmkdir(Dir);
   swprintf(BMPKadrs,L"%s\\%s\\%3.3d.bmp",CurrDir,BMPKadr,Nkadr);
  }
 }
 if(*TOJPEG){
  if(JPEG) delete JPEG; JPEG = new TJPEGImage();
  if(Kinst>1){
   swprintf(Dir,L"%s\\%s(№%d)",CurrDir,BMPKadr,Kinst);
   if(_waccess(Dir,0)) _wmkdir(Dir);
   swprintf(JPGKadrs,L"%s\\%s(№%d)\\%3.3d.jpg",CurrDir,BMPKadr,Kinst,Nkadr);
  }
  else{
   swprintf(Dir,L"%s\\%s",CurrDir,BMPKadr);
   if(_waccess(Dir,0)) _wmkdir(Dir);
   swprintf(JPGKadrs,L"%s\\%s\\%3.3d.jpg",CurrDir,BMPKadr,Nkadr);
  }
  JPEG->Assign(Image->Picture->Bitmap);
  JPEG->SaveToFile(JPGKadrs);
}}
//---------------------------------------------------------------------------
void __fastcall TMultFilms::TimerMultFilmTimer(TObject *Sender){
 int Hou,Min,Sec,Dec,Houk,Mink,Seck,Deck;
 if(!Nkadr){ ToolButtonInclude->Enabled=true; TB_Table->Enabled=false; }
 if(!*SHLEIF){
  Image->Canvas->CopyRect(Image->ClientRect,PhonImage->Canvas,Image->ClientRect);
 }
 t+=dt; Nkadr++;
 Sec=(int)floor(t); Dec=(int)Round(100.0*(t-floor(t)));
 Seck=(int)floor(Tend); Deck=(int)Round(100.0*(Tend-floor(Tend)));
 if(Tend>3600){
  Min=Sec/60; Hou=Min/60; Min=Min%60; Sec=Sec%60;
  Mink=Seck/60; Houk=Mink/60; Mink=Seck/60; Seck=Seck%60;
  swprintf(Inf,
   L"Длительность: %2.2d:%2.2d:%2.2d:%2.2d, истекло: %2.2d:%2.2d:%2.2d:%2.2d",
   Houk,Mink,Seck,Deck,Hou,Min,Sec,Dec);
 }
 else if(Tend>60){
  Min=Sec/60; Sec=Sec%60; Mink=Seck/60; Seck=Seck%60;
  swprintf(Inf,L"Длительность: %2.2d:%2.2d:%2.2d, истекло: %2.2d:%2.2d:%2.2d",
   Mink,Seck,Deck,Min,Sec,Dec);
 }
 else{
  swprintf(Inf,L"Длительность: %2.2d:%2.2d, истекло: %2.2d:%2.2d",
   (int)floor(Tend),(int)Round(100.0*(Tend-floor(Tend))),Sec,Dec);
 }
 StatusBar->SimpleText=Inf;
 if(CGauge) CGauge->Progress++;
 DrawKadr(Image->Canvas);
 if((Nkadr==Kt)||EndInt()||t>Tend){
  if(CGauge) CGauge->Progress++;
  TimerMultFilm->Enabled=false;  if(CGauge) CGauge->ShowText=false;
  Image->Canvas->Pen->Width=PenWidth; Image->Canvas->Font->Size=FontSize;
  Image->Canvas->Font->Color=FontColor; Image->Canvas->Brush->Color=BrushColor;
  Image->Canvas->Pen->Color=PenColor;
  if(CGauge){ CGauge->Progress=0; CGauge->Visible=false; }
  swprintf(Inf,L"%s: %s",HeadWork,Autor); StatusBar->SimpleText=Inf;
 }
 if(*TOBMP){
  if(Kinst>1)
   swprintf(BMPKadrs,L"%s\\%s(№%d)\\%3.3d.bmp",CurrDir,BMPKadr,Kinst,Nkadr);
  else
   swprintf(BMPKadrs,L"%s\\%s\\%3.3d.bmp",CurrDir,BMPKadr,Nkadr);
  Image->Picture->SaveToFile(BMPKadrs);
 }
 if(*TOJPEG){
  if(JPEG) delete JPEG; JPEG = new TJPEGImage();
  if(Kinst>1)
   swprintf(JPGKadrs,L"%s\\%s(№%d)\\%3.3d.jpg",CurrDir,BMPKadr,Kinst,Nkadr);
  else
   swprintf(JPGKadrs,L"%s\\%s\\%3.3d.jpg",CurrDir,BMPKadr,Nkadr);
  JPEG->Assign(Image->Picture->Bitmap);
  JPEG->SaveToFile(JPGKadrs);
}}
//---------------------------------------------------------------------------
void __fastcall TMultFilms::Metrica(TCanvas *Canvas){
 TRect SpriteRect=Canvas->ClipRect;
 int W=SpriteRect.Width(),H=SpriteRect.Height(),LPx,LPy,Wgmm,Hgmm,
  HorzSize=GetDeviceCaps(Canvas->Handle,HORZSIZE), //mm
  VertSize=GetDeviceCaps(Canvas->Handle,VERTSIZE); //mm
 LPx=GetDeviceCaps(Canvas->Handle,LOGPIXELSX);
 LPy=GetDeviceCaps(Canvas->Handle,LOGPIXELSY);
 Canvas->Font->Size=3*Canvas->Font->Size/2;
 while(Canvas->TextWidth(Head)>W) Canvas->Font->Size--;
 Hh=Round(6.0*Canvas->TextHeight(L"M")/5.0);
 Canvas->Font->Size=SizeFont;
 Hs=Round(6.0*Canvas->TextHeight(L"M")/5.0);
 Hy=Hc=Hx=Hs; Sg=10;
 Wu=Wo=Canvas->TextWidth(L"000.000E+00");
 Wg=W-Wo-Wu; Wgmm=Wg*25.4/LPx;
 if(VertSize>HorzSize) Hg=H/2;//принтер, книжная ориентация
 else Hg=H;//экран или принтер, альбомная ориентация
 Hg-=Hh+Hy+Hx; Hgmm=Hg*25.4/LPy;
 KP[0]=1+Wgmm/Sg; KP[1]=1+Hgmm/Sg;
 dS[0]=Round((double)(Sg*LPx)/25.4);
 dS[1]=Round((double)(Sg*LPy)/25.4);
 Xk=Wo; Yk=Hh+Hy+Hg;
}
//---------------------------------------------------------------------------
void __fastcall TMultFilms::DrawHead(TCanvas *Canvas){
 bool Back=false; wchar_t GrekHead[256],*s;
 TRect SpriteRect=Canvas->ClipRect; TColor Color=Canvas->Font->Color;
 int l,W=SpriteRect.Width()/*,H=SpriteRect.Height()*/,Size;
 Canvas->Pen->Color=clBlack; Canvas->Pen->Width=1;
// Canvas->Rectangle(0,0,W-1,H-1);
 Canvas->Font->Color=*ColorHead;
 if(!Canvas->Font->Style.Contains(fsBold)){
  Canvas->Font->Style=Canvas->Font->Style<<fsBold; Back=true;
 }
 Size=Canvas->Font->Size;
 Canvas->Font->Size=3*Canvas->Font->Size/2;
 if(!(s=wcschr(Head.c_str(),'('))) wcscpy(GrekHead,Head.c_str());
 else{ wcscpy(GrekHead,s+1); if(s=wcschr(GrekHead,')')) *s='\0'; }
 DrawGrekText(Canvas,0,1,W,GrekHead,SimbInd);
 Canvas->Font->Size=Size;
 if(Back) Canvas->Font->Style=Canvas->Font->Style>>fsBold;
 Canvas->Font->Color=Color;
 Canvas->MoveTo(0,0); Canvas->LineTo(W-1,0);
}
//---------------------------------------------------------------------------
void __fastcall TMultFilms::DrawSetka(TCanvas *Canvas){
 int i,j,x,y,l,x0,y0,B=Canvas->TextWidth(L"M");
 TColor Color=Canvas->Font->Color,ColorBrush; Magazine *m;
 Canvas->Font->Name=NameFont; Canvas->Font->Size=SizeFont;
 Canvas->Font->Color=ColorFont;
 Canvas->Font->Style=
  ExFont?Canvas->Font->Style<<fsBold:Canvas->Font->Style>>fsBold;
 Canvas->Font->Style=
  ItFont?Canvas->Font->Style<<fsItalic:Canvas->Font->Style>>fsItalic;
 Canvas->Font->Style=
  UnFont?Canvas->Font->Style<<fsUnderline:Canvas->Font->Style>>fsUnderline;
 //координаты начала абсолютной системы в экранной системе
 x0=Xk-Round(xk/MSt[0]); y0=Yk+Round(yk/MSt[1]);
 Canvas->Brush->Color=*ColorPhon; Canvas->Pen->Width=1;
 ColorBrush=Canvas->Brush->Color;
 Canvas->Brush->Color=*ColorPhon;
 if(ParmF) Canvas->Brush->Style=bsClear;
 Canvas->Rectangle(Xk,Yk-Hg,Xk+Wg,Yk);
 Canvas->Pen->Color=*ColorPhon;
 Canvas->Brush->Color=*ColorPhon;
 Canvas->Rectangle(Canvas->ClipRect.Left,Canvas->ClipRect.Top,Xk-2,Canvas->ClipRect.Bottom);
 Canvas->Rectangle(Xk+Wg+2,Canvas->ClipRect.Top,Canvas->ClipRect.Right,Canvas->ClipRect.Bottom);
 Canvas->Rectangle(Xk-2,Canvas->ClipRect.Top,Xk+Wg+2,Yk-Hg-1);
 Canvas->Rectangle(Xk-2,Yk+2,Xk+Wg+2,Canvas->ClipRect.Bottom);
 Canvas->Brush->Color=ColorBrush;
 i=ceil((double)(Xk-x0)/dS[0]);
 for(x=x0+i*dS[0],j=0;x<Xk+Wg;x+=dS[0],i++,j++){
  Canvas->Pen->Color=*ColorGrid;
  Canvas->MoveTo(x,Yk-1); Canvas->LineTo(x,Yk-Hg);
  Canvas->Font->Color=*ColorCifrGrid;
  if(i%2){
   Canvas->MoveTo(x-1,Yk-5); Canvas->LineTo(x-1,Yk);
   Canvas->MoveTo(x+1,Yk-5); Canvas->LineTo(x+1,Yk);
   Canvas->MoveTo(x-1,Yk-Hg+5); Canvas->LineTo(x-1,Yk-Hg);
   Canvas->MoveTo(x+1,Yk-Hg+5); Canvas->LineTo(x+1,Yk-Hg);
   swprintf(Inf,L"%g",i*MstT[1]*Sg); l=Canvas->TextWidth(Inf);
   Canvas->TextOut(x-l/2,Yk+1,Inf);
   if(j) Canvas->TextOut(x-l/2,Yk-Hg-Hy+3,Inf);
 }}
 i=ceil((double)(y0-Yk)/dS[1]);
 for(y=y0-i*dS[1];y>Yk-Hg;y-=dS[1],i++){ int L,j;
  Canvas->Pen->Color=*ColorGrid;
  Canvas->MoveTo(Xk+1,y); Canvas->LineTo(Xk+Wg-1,y);
  Canvas->Font->Color=*ColorCifrGrid;
  L=swprintf(Inf,L"%g",i*MstT[0]*Sg);
  if(L>9){
   if(wcschr(Inf,'e')||wcschr(Inf,'E'))
    swprintf(Inf,L"%10.3e",i*MstT[0]*Sg);
  }
  l=Canvas->TextWidth(Inf);
  Canvas->TextOut(Xk-l-B,y-Hs/2,Inf);
  L=swprintf(Inf,L"%g",i*MstT[0]*Sg);
  if(L>9){
   if(wcschr(Inf,'e')||wcschr(Inf,'E'))
    swprintf(Inf,L"%10.3e",i*MstT[0]*Sg);
  }
  Canvas->TextWidth(Inf);
  Canvas->TextOut(Xk+Wg+B,y-Hs/2,Inf);
 }
 Canvas->Font->Color=*ColorNameOs; Canvas->Font->Style=TFontStyles()<<fsBold;
 l=Canvas->TextWidth(NameY); x=Xk-l-1;
 DrawGrekText(Canvas,x,Yk-Hg-Hy+1,0,NameY,SimbInd);
 Canvas->TextWidth(NameX); x=Xk+Wg+1;
 DrawGrekText(Canvas,x,Yk,0,NameX,SimbInd);
 Canvas->Font->Style=TFontStyles()>>fsBold; Canvas->Pen->Color=*ColorRamka;
 Canvas->MoveTo(Xk,Yk-Hg); Canvas->LineTo(Xk,Yk);
 Canvas->LineTo(Xk+Wg,Yk);
 Canvas->LineTo(Xk+Wg,Yk-Hg);
 Canvas->LineTo(Xk,Yk-Hg);
 Canvas->MoveTo(Xk+Wg+1,Yk-Hg+1); Canvas->LineTo(Xk+Wg+1,Yk+1);
 Canvas->MoveTo(Xk+1,Yk+1);
 Canvas->LineTo(Xk+Wg+1,Yk+1);
 Canvas->Font->Color=Color;
}
//---------------------------------------------------------------------------
void __fastcall TMultFilms::ToBMP(TTabControl *TabControl,bool Message){
 wchar_t S[256],Name[256],File[256],*s; Magazine *m; int Wgo;
 if(Kinst>1){
  swprintf(BMPFile,L"%s\\%s%3.3d(№%d).bmp",CurrDir,BMPKadr,Nkadr,Kinst);
  swprintf(JPGFile,L"%s\\%s%3.3d(№%d).jpg",CurrDir,BMPKadr,Nkadr,Kinst);
 }
 else{
  swprintf(BMPFile,L"%s\\%s%3.3d.bmp",CurrDir,BMPKadr,Nkadr);
  swprintf(JPGFile,L"%s\\%s%3.3d.jpg",CurrDir,BMPKadr,Nkadr);
 }
 TImage *ImageBMP=new TImage(TabControl); double Ps;
 TimerMultFilm->Enabled=false; Nkadr++;
 ImageBMP->Parent=TabControl; ImageBMP->Visible=false;
 ImageBMP->Height=Round((double)Screen->Height/Kbmp);
 ImageBMP->Width=Round((double)Screen->Width/Kbmp);
 double MSt0=MSt[0],MSt1=MSt[1],Xk_=Xk,Yk_=Yk,xk_=xk,yk_=yk,Wg_=Wg,Hg_=Hg,t_=t;
 Metrica(ImageBMP->Canvas);
 DrawHead(ImageBMP->Canvas);
 MSt[0]=(Spr[0]-Slv[0])/(KP[0]-1);
 MSt[1]=(Max[1]-Min[1])/(KP[1]-1);
 if(MSt[0]==0.0) MSt[0]=MSt[1];
 if(MSt[1]==0.0) MSt[1]=MSt[0];
 if((MSt[0]==0.0)||(MSt[1]==0.0)) return;
 for(int i=0;i<2;i++){
  for(Ps=0;MSt[i]>10.0;Ps++) MSt[i]/=10.0;
  for(;MSt[i]<1.0;Ps--) MSt[i]*=10.0;
  for(int j=1;j<10;j++) if(MSt[i]<=MG[j]){ MSt[i]=MG[j]; break; }
  for(int j=abs(Ps);j--;)
   if(Ps>0) MSt[i]*=10; else MSt[i]/=10;
 }
 if(MSt[0]>MSt[1]) MSt[1]=MSt[0]; else if(MSt[1]>MSt[0]) MSt[0]=MSt[1];
 for(int i=0;i<2;i++){ MstT[i]=MSt[i]/Sg; MSt[i]/=dS[i];}
 Wgo=Wg; Wg=1+Round((Spr[0]-Slv[0])/MSt[0]); Xk+=(Wgo-Wg)/2;
 xk=(Spr[0]+Slv[0]-(Wg-1)*MSt[0])/2; yk=(Max[1]+Min[1]-(Hg-1)*MSt[1])/2;
 DrawPhon(ImageBMP->Canvas);
 DrawSetka(ImageBMP->Canvas);
 t=t_+dt;
 DrawKadr(ImageBMP->Canvas);
 MSt[0]=MSt0; MSt[1]=MSt1; Xk=Xk_; Yk=Yk_; xk=xk_; yk=yk_; Wg=Wg_; Hg=Hg_;
 ImageBMP->Picture->SaveToFile(BMPFile);
 TJPEGImage *jp = new TJPEGImage();
 jp->Assign(ImageBMP->Picture->Bitmap);
 jp->SaveToFile(JPGFile);
 if(Message){
  swprintf(Inf,L"Кадр сохранен в файлах\n\"%s\" и\n\"%s\".\n"
   L"Эти файлы можно просмотреть и отредактировать\n"
   L"графическим редактором, вставить в документ WORD и т.д.",BMPFile,JPGFile);
  Application->MessageBox(Inf,L"Внимание",MB_OK|MB_ICONWARNING);
 }
 if((t>=Tend)||EndInt()){
  TimerMultFilm->Enabled=false; ToolButtonInclude->Enabled=false;
  TB_ToBMP->Enabled=false;
 }
 else TimerMultFilm->Enabled=true;
}
//---------------------------------------------------------------------------
void __fastcall TMultFilms::Include(TTabControl *TabControl){
 ToBMP(TabControl,false);
// if(HTMFILE=_wfopen(HtmFile,L"r+")){
 if(SLH){
//   if(!fseek(HTMFILE,-Lpodv,2)){
  for(int i=0;i<Kpodv;i++) SLH->Delete(SLH->Count-1);
  if(!NamesKadr){
//	swprintf(HTMFILE,
//	 L"<h4 align=\"left\"><font color=\"#009900\">"
//	 L"%d. Избранные кадры:</font></h4>\n",PUNKT++);
   swprintf(Shtm,
	L"<h4 align=\"left\"><font color=\"#009900\">"
	L"%d. Избранные кадры:</font></h4>",PUNKT++);
   SLH->Add(Shtm);
  }
  TakeMagazine(&NamesKadr,BMPFile);
//  fwprintf(HTMFILE,
//   L"<h3 align=\"center\"><font color=\"#990000\">"
//   L"%s (Кадр № %d)</font></h4>\n",MultFilmItem->Caption,Nkadr);
//  fwprintf(HTMFILE,
//   L"<p align=\"center\">"
//   L"<img src=\"%s\" alt=\"Этот кадр см. в файле\n%s или\n%s\">"
//   L"</p><br>\n",BMPFile,BMPFile,JPGFile);
//  fwprintf(HTMFILE,
//   L"<hr><font face=\"Georgia, Times New Roman, Times, serif\""
//   L" color=\"#326464\">&copy; %s, %d<br></font><br>\n",Autor,NYear+1900)+1;
//  fputws(L"</body>\n</html>\n",HTMFILE);
  swprintf(Shtm,
   L"<h3 align=\"center\"><font color=\"#990000\">"
   L"%s (Кадр № %d)</font></h4>",MultFilmItem->Caption,Nkadr);
  SLH->Add(Shtm);
  swprintf(Shtm,
   L"<p align=\"center\">"
   L"<img src=\"%s\" alt=\"Этот кадр см. в файле\n%s или\n%s\">"
   L"</p><br>",BMPFile,BMPFile,JPGFile);
  SLH->Add(Shtm);
  swprintf(Shtm,
   L"<hr><font face=\"Georgia, Times New Roman, Times, serif\""
   L" color=\"#326464\">&copy; %s, %d<br></font><br>",Autor,NYear+1900)+1;
  SLH->Add(Shtm); SLH->Add(L"</body>"); SLH->Add(L"</html>");
 }
// fclose(HTMFILE);
 SLH->SaveToFile(HtmFile);
}
//---------------------------------------------------------------------------

