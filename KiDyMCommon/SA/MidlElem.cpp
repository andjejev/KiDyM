//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "TablW.h"
#include "CA.h"
#include <stdio.h>
#include <io.h>
//---------------------------------------------------------------------------

#pragma package(smart_init)
extern Cnst *Nul;
extern List *L; 
extern bool OTLADKA;
extern FILE *OPRFILE;
extern Magazine *VarsInForm;
extern wchar_t Inf[],OprFile[],WORKDIR[];

//extern FILE *TXTFILE;
//---------------------------------------------------------------------------
/*    Для линеаризации, например, приведенных к коленчатому валу переменных
инерционных характеристик шатунов и поршней кривошипно-шатунных механизмов
(КШМ) используется оператор L"УСРЕДНИТЬ".
Это позволяет задать структуру и геометрические размеры КШМ, массу поршня,
моменты инерции шатуна и кривошипа, а необходимые инерционные элементы,
описывающие усредненные на одном обороте коленчатого вала инерционные
характеристики КШМ, программа построит с помощью этого оператора.
Синтаксис оператора L"УСРЕДНИТЬ" состоит в следующем.

  УСРЕДНИТЬ :
  	Э[ЛЕМЕНТЫ](<список имен элементов в кавычках>),
	И[НТЕГРИРОВАТЬ ПО](<переменная интегрирования>,<начальное значение>,
                           <конечное значение>,<количество интервалов>),
        К[ООРДИНАТЫ](<имя координаты>,<имя координаты>);

Список инструкции L"Э[ЛЕМЕНТЫ]" определяет инерционные элементы,
которые необходимо усреднить.
Список инструкции L"И[НТЕГРИРОВАТЬ]" содержит имя координаты,
по которой будет производиться интегрирование (к которой следует
привести инерционные характеристики деталей КШМ), а также её начальное
и конечное значения, количество подынтервалов, на которые будет разбит
интервал интегрирования.
Список инструкции L"К[ООРДИНАТЫ]" содержит имена координат полной механической
модели, на которые следует поместить полученные в результате осреднения
инерционные элементы. */
//---------------------------------------------------------------------------
bool MidlElem(Parm *Par,TRichEdit *R,TCanvas *TC){
 int i,Ki,Pv,NLine=0; Parm *p,*P; Vary *V,*K; Elem *E; Root *I;
 double Bg,En,Sum=0.0,h; Form Fe; Fe.C=Nul; FILE *F; Cnst *C;
 wchar_t Mes[10][512],Name[256],TxtFile[256],*s;
 Magazine *Me=NULL,*Mk=NULL; bool NoE=true,NoI=true,NoK=true,Rez=true;
 if(OTLADKA=FindInst(L"ОТЛАДКА",&I)) _wunlink(L"Otladka");
 for(Root *o=L->Oper;o;o=o->Sled) if(o->First==Par){ NLine=o->Line; break; }
 swprintf(Mes[0],L"Оператор \"УСРЕДНИТЬ\" должен содержать 3 составных списка\n"
  L"Э[ЛЕМЕНТЫ](<список имен элементов>),\n"
  L"И[НТЕГРИРОВАТЬ ПО](<переменная интегрирования>,<начальное значение>,\n"
  L"                   <конечное значение>,<количество интервалов>),\n"
  L"К[ООРДИНАТЫ](<имя координаты1>,<имя координаты2,...>);");
 swprintf(Mes[1],L"Оператор \"УСРЕДНИТЬ\" должен содержать в составном списке\n"
  L"Э[ЛЕМЕНТЫ] имена существующих в исходных данных инерционных элементов,\n"
  L"которые необходимо привести, усреднить и закрыть для дальнейшего");
 swprintf(Mes[2],L"Оператор \"УСРЕДНИТЬ\" должен содержать в составном списке\n"
  L"И[НТЕГРИРОВАТЬ ПО] имя переменной интегрирования,\n"
  L"начальное, конечное значение, количество подинтервалов\n"
  L"интервала усреднения");
 swprintf(Mes[3],L"Оператор \"УСРЕДНИТЬ\" содержит в составном списке\n"
  L"И[НТЕГРИРОВАТЬ ПО] в качестве имени переменной интегрирования\n"
  L"имя несуществующей переменной");
 swprintf(Mes[4],L"Оператор \"УСРЕДНИТЬ\" содержит в составном списке\n"
  L"И[НТЕГРИРОВАТЬ ПО] в качестве начального значения\n"
  L"имя несуществующей переменной");
 swprintf(Mes[5],L"Оператор \"УСРЕДНИТЬ\" содержит в составном списке\n"
  L"И[НТЕГРИРОВАТЬ ПО] в качестве конечного значения\n"
  L"имя несуществующей переменной");
 swprintf(Mes[6],L"Оператор \"УСРЕДНИТЬ\" содержит в составном списке\n"
  L"И[НТЕГРИРОВАТЬ ПО] в качестве числа интервалов\n"
  L"имя несуществующей переменной");
 swprintf(Mes[7],L"Оператор \"УСРЕДНИТЬ\" содержит в составном списке\n"
  L"И[НТЕГРИРОВАТЬ ПО] в качестве числа интервалов ноль,\n"
  L"что недопустимо!");
 swprintf(Mes[8],L"Оператор \"УСРЕДНИТЬ\" содержит в составном списке\n"
  L"К[ООРДИНАТЫ] вместо имени координаты приведения число");
 for(P=Par;P;P=P->Sled){
  if(!P->Nam){
   Application->MessageBox(Mes[0],L"ОШИБКА",MB_OK); Rez=false; goto Ex;
  }
  wcscpy(Name,P->Nam); LowToUpp(Name);
  switch(Name[0]){
   case 'Э': NoE=false;
    for(p=P->R;p;p=p->Sled){
     if(!p->Nam||!FindElem(p->Nam,&E)&&!FindCloseElem(p->Nam,&E)){
      Application->MessageBox(Mes[1],L"ОШИБКА",MB_OK); Rez=false; goto Ex;
     }
     TakeMagazine(&Me,p->Nam);
    }
    break;
   case 'И': NoI=false;
    for(p=P->R,i=0;p;i++,p=p->Sled){
     switch(i){
      case 0:
       if(!p->Nam){
        Application->MessageBox(Mes[2],L"ОШИБКА",MB_OK); Rez=false; goto Ex;
       }
       if(!FindVary(p->Nam,&K)){
        Application->MessageBox(Mes[3],L"ОШИБКА",MB_OK); Rez=false; goto Ex;
       }
       Pv=K->P; K->P=0;
       break;
      case 1:
       if(p->Nam){
        if(FindVary(p->Nam,&V)) Bg=Val(V->Znach);
        else{
         Application->MessageBox(Mes[4],L"ОШИБКА",MB_OK); Rez=false; goto Ex;
        }
       }
       break;
      case 2:
       if(p->Nam){
        if(FindVary(p->Nam,&V)) En=Val(V->Znach);
        else{
         Application->MessageBox(Mes[5],L"ОШИБКА",MB_OK); Rez=false; goto Ex;
        }
       }
       else
        En=p->Val;
       break;
      case 3:
       if(p->Nam){
        if(FindVary(p->Nam,&V)) Ki=Round(Val(V->Znach));
        else{
         Application->MessageBox(Mes[6],L"ОШИБКА",MB_OK); Rez=false; goto Ex;
       }}
       else
        Ki=Round(p->Val);
       if(!Ki){
        Application->MessageBox(Mes[7],L"ОШИБКА",MB_OK); Rez=false; goto Ex;
       }
       break;
      default:
       Application->MessageBox(Mes[2],L"ОШИБКА",MB_OK); Rez=false; goto Ex;
    }}
    break;
   case 'К': NoK=false;
    for(p=P->R;p;p=p->Sled){
     if(!p->Nam){
      Application->MessageBox(Mes[8],L"ОШИБКА",MB_OK); Rez=false; goto Ex;
     }
     TakeVary(p->Nam); TakeMagazine(&Mk,p->Nam);
    }
    break;
   default:
    Application->MessageBox(Mes[0],L"ОШИБКА",MB_OK); Rez=false;
 }}
 if(NoE||NoK||NoI){
  Application->MessageBox(Mes[0],L"ОШИБКА",MB_OK); Rez=false; goto Ex;
 }
 h=(En-Bg)/(Ki-1);
// wcscpy(TxtFile,OprFile); if(s=wcsrchr(TxtFile,'.')) wcscpy(s+1,L"txt");
// TXTFILE=_wfopen(TxtFile,_waccess(OprFile,0)?"w":"a");
 if(_waccess(OprFile,0)){
  if(OPRFILE=_wfopen(OprFile,L"w"))
   fwprintf(OPRFILE,
    L"<html>\n<head>\n<title>ПРОТОКОЛ ОПЕРАТОРОВ</title>\n"
    L"<meta http-equiv=\"Content-Type\" content=\"text/html;"
    L"wchar_tset=windows-1251\">\n</head>\n"
    L"<body bgcolor=\"#FFFFFF\" text=\"#800000\""
    L" background=\"%s/fon2.jpg\""
    L" font face=\"Times New Roman, Times, serif\">\n",WORKDIR);
 }
 else
  OPRFILE=_wfopen(OprFile,L"a");
// if(OPRFILE=_wfopen(OprFile,_waccess(OprFile,0)?"w":"a")){
//  wchar_t NameK[1024]; wcscpy(NameK,K->Name); LiterGrekToHtm(NameK);
//  fwprintf(OPRFILE,
//   L"<html>\n<head>\n<title>ПРОТОКОЛ ОПЕРАТОРА \"УСРЕДНИТЬ\" строки %d</title>\n"
//   L"<meta http-equiv=\"Content-Type\" content=\"text/html;"
//   L"wchar_tset=windows-1251\">\n</head>\n"
//   L"<body bgcolor=\"#FFFFFF\" text=\"#800000\""
//   L" background=\"%s/fon2.jpg\""
//   L" font face=\"Times New Roman, Times, serif\">\n",NLine,WORKDIR);
//  if(TXTFILE){
//   swprintf(Inf,L"ПРОТОКОЛ ОПЕРАТОРА \"УСРЕДНИТЬ\" строки %d\n\n\n",NLine);
//   Fputs(Inf,TXTFILE);
//  }
 if(OPRFILE){
  wchar_t NameK[1024]; wcscpy(NameK,K->Name); LiterGrekToHtm(NameK);
  fwprintf(OPRFILE,L"<h3 align=\"center\"><font color=\"#800000\">"
   L"Результат работы оператора \"УСРЕДНИТЬ\" строки %d:</font></h3>\n",NLine);
  fwprintf(OPRFILE,
   L"Приведением к координате %s<br>\n"
   L"получена характеристика эквивалентного инерционного элемента:<br>\n<br>\n",
   NameK);
//  if(TXTFILE){
//   swprintf(Inf,
//    L"Приведением к координате %s\n"
//    L"получена характеристика эквивалентного инерционного элемента:\n\n",
//    K->Name);
//   Fputs(Inf,TXTFILE);
//  }
  if(OTLADKA){ F=_wfopen(L"Otladka",L"w"); fclose(F); }
  for(Magazine *m=Me;m;m=m->Sled){ Form FK,f,f1; int L;
   if(!FindElem(m->S,&E)) FindCloseElem(m->S,&E); FK.V=E->K;
   Fe=Fe+E->Znach*((DifBy(FK,K))^2);
   if(OTLADKA){ F=_wfopen(L"Otladka",L"a"); PrintForm(true,F,L"Fe",Fe); fclose(F); }
   if(E->Name[0]!='@'){
    L=swprintf(Name,L"@%s",E->Name);
    E->Name=(wchar_t *)calloc(L+1,SzC); wcscpy(E->Name,Name);
  }}
//  DelAllMagazine(&VarsInForm);
  PrintForm(false,OPRFILE,NULL,Fe); fputws(L"<br>\n<br>\nЗдесь:<br>\n<br>\n",OPRFILE);
//  if(TXTFILE){
//   PrintForm(true,TXTFILE,NULL,Fe); wcscpy(Inf,L"\n\nЗдесь:\n\n"); Fputs(Inf,TXTFILE);
//  }
//  for(Magazine *m=VarsInForm;m;m=m->Sled){
  for(V=L->V;V;V=V->Sled){
   if(VarInForm(true,Fe,V)){
//   if(FindVary(m->S,&V)){
    PrintForm(false,OPRFILE,V->Name,V->Znach); fputws(L"<br>\n",OPRFILE);
//    if(TXTFILE){
//     PrintForm(true,TXTFILE,V->Name,V->Znach); fputws(L"\n",TXTFILE);
//    }
  }}
  fputws(L"<br>\nВ результате создан",OPRFILE);
//  if(TXTFILE){
//   wcscpy(Inf,L"\nВ результате создан"); Fputs(Inf,TXTFILE);
//  }
  K->Val=Bg; Sum+=0.5*Value(Fe);
  K->Val=En; Sum+=0.5*Value(Fe);
  for(i=1;i<Ki;i++){
   K->Val=Bg+h*i;
   Sum+=Value(Fe);
  }
  K->P=Pv; Sum/=Ki; Pv=0;
  while(Sum>10.0){ Sum/=10.0; Pv++; }
  while(Sum<1.0){ Sum*=10.0; Pv--; }
  Sum=pow(10.0,Pv)*Round(Sum,0.01);
  C=TakeCnst(Sum);
  if(KolElem(Mk)>1){
   fwprintf(OPRFILE,L"ы следующие усредненные элементы:<br>\n");
//   if(TXTFILE){
//    swprintf(Inf,L"ы следующие усредненные элементы:\n"); Fputs(Inf,TXTFILE);
//   }
  }
  else{
   fwprintf(OPRFILE,L" следующий усредненный элемент:<br>\n");
//   if(TXTFILE){
//    swprintf(Inf,L" следующий усредненный элемент:\n"); Fputs(Inf,TXTFILE);
//   }
  }
  for(Magazine *m=Mk;m;m=m->Sled){
   V=TakeVary(m->S);
   swprintf(Name,L"J@.%s",m->S); E=TakeElem(Name); E->Znach.C=C; E->K=V;
   PrintForm(false,OPRFILE,E->Name,E->Znach); fputws(L"<br>\n",OPRFILE);
//   if(TXTFILE){
//    PrintForm(true,TXTFILE,E->Name,E->Znach); fputws(L"\n",TXTFILE);
//   }
  }
  fwprintf(OPRFILE,L"<hr></body>\n</html>\n"); fclose(OPRFILE);
//  if(TXTFILE){ fputws(L"\n\n",TXTFILE); fclose(TXTFILE); }
 }
// if(R&&(TXTFILE=_wfopen(TxtFile,L"r"))){
//  R->Clear();
//  while(Fgets(Inf,1023,TXTFILE)){ Inf[wcslen(Inf)-1]='\0'; R->Lines->Add(Inf); }
//  fclose(TXTFILE);
// }
Ex: DelAllMagazine(&Me); DelAllMagazine(&Mk);
return Rez;
}
//---------------------------------------------------------------------------

