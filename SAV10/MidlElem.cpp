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
extern char Inf[],OprFile[],WORKDIR[];

//extern FILE *TXTFILE;
//---------------------------------------------------------------------------
/*    Для линеаризации, например, приведенных к коленчатому валу переменных
инерционных характеристик шатунов и поршней кривошипно-шатунных механизмов
(КШМ) используется оператор "УСРЕДНИТЬ".
Это позволяет задать структуру и геометрические размеры КШМ, массу поршня,
моменты инерции шатуна и кривошипа, а необходимые инерционные элементы,
описывающие усредненные на одном обороте коленчатого вала инерционные
характеристики КШМ, программа построит с помощью этого оператора.
Синтаксис оператора "УСРЕДНИТЬ" состоит в следующем.

  УСРЕДНИТЬ :
  	Э[ЛЕМЕНТЫ](<список имен элементов в кавычках>),
	И[НТЕГРИРОВАТЬ ПО](<переменная интегрирования>,<начальное значение>,
                           <конечное значение>,<количество интервалов>),
        К[ООРДИНАТЫ](<имя координаты>,<имя координаты>);

Список инструкции "Э[ЛЕМЕНТЫ]" определяет инерционные элементы,
которые необходимо усреднить.
Список инструкции "И[НТЕГРИРОВАТЬ]" содержит имя координаты,
по которой будет производиться интегрирование (к которой следует
привести инерционные характеристики деталей КШМ), а также её начальное
и конечное значения, количество подынтервалов, на которые будет разбит
интервал интегрирования.
Список инструкции "К[ООРДИНАТЫ]" содержит имена координат полной механической
модели, на которые следует поместить полученные в результате осреднения
инерционные элементы. */
//---------------------------------------------------------------------------
bool MidlElem(Parm *Par,TRichEdit *R){
 int i,Ki,Pv,NLine=0; Parm *p,*P; Vary *V,*K; Elem *E; Root *I;
 double Bg,En,Sum=0.0,h; Form Fe; Fe.C=Nul; FILE *F; Cnst *C;
 char Mes[10][512],Name[256],TxtFile[256],*s;
 Magazine *Me=NULL,*Mk=NULL; bool NoE=true,NoI=true,NoK=true,Rez=true;
 if(OTLADKA=FindInst("ОТЛАДКА",&I)) unlink("Otladka");
 for(Root *o=L->Oper;o;o=o->Sled) if(o->First==Par){ NLine=o->Line; break; }
 sprintf(Mes[0],"Оператор \"УСРЕДНИТЬ\" должен содержать 3 составных списка\n"
  "Э[ЛЕМЕНТЫ](<список имен элементов>),\n"
  "И[НТЕГРИРОВАТЬ ПО](<переменная интегрирования>,<начальное значение>,\n"
  "                   <конечное значение>,<количество интервалов>),\n"
  "К[ООРДИНАТЫ](<имя координаты1>,<имя координаты2,...>);");
 sprintf(Mes[1],"Оператор \"УСРЕДНИТЬ\" должен содержать в составном списке\n"
  "Э[ЛЕМЕНТЫ] имена существующих в исходных данных инерционных элементов,\n"
  "которые необходимо привести, усреднить и закрыть для дальнейшего");
 sprintf(Mes[2],"Оператор \"УСРЕДНИТЬ\" должен содержать в составном списке\n"
  "И[НТЕГРИРОВАТЬ ПО] имя переменной интегрирования,\n"
  "начальное, конечное значение, количество подинтервалов\n"
  "интервала усреднения");
 sprintf(Mes[3],"Оператор \"УСРЕДНИТЬ\" содержит в составном списке\n"
  "И[НТЕГРИРОВАТЬ ПО] в качестве имени переменной интегрирования\n"
  "имя несуществующей переменной");
 sprintf(Mes[4],"Оператор \"УСРЕДНИТЬ\" содержит в составном списке\n"
  "И[НТЕГРИРОВАТЬ ПО] в качестве начального значения\n"
  "имя несуществующей переменной");
 sprintf(Mes[5],"Оператор \"УСРЕДНИТЬ\" содержит в составном списке\n"
  "И[НТЕГРИРОВАТЬ ПО] в качестве конечного значения\n"
  "имя несуществующей переменной");
 sprintf(Mes[6],"Оператор \"УСРЕДНИТЬ\" содержит в составном списке\n"
  "И[НТЕГРИРОВАТЬ ПО] в качестве числа интервалов\n"
  "имя несуществующей переменной");
 sprintf(Mes[7],"Оператор \"УСРЕДНИТЬ\" содержит в составном списке\n"
  "И[НТЕГРИРОВАТЬ ПО] в качестве числа интервалов ноль,\n"
  "что недопустимо!");
 sprintf(Mes[8],"Оператор \"УСРЕДНИТЬ\" содержит в составном списке\n"
  "К[ООРДИНАТЫ] вместо имени координаты приведения число");
 for(P=Par;P;P=P->Sled){
  if(!P->Nam){
   Application->MessageBox(Mes[0],"ОШИБКА",MB_OK); Rez=false; goto Ex;
  }
  strcpy(Name,P->Nam); LowToUpp(Name);
  switch(Name[0]){
   case 'Э': NoE=false;
    for(p=P->R;p;p=p->Sled){
     if(!p->Nam||!FindElem(p->Nam,&E)&&!FindCloseElem(p->Nam,&E)){
      Application->MessageBox(Mes[1],"ОШИБКА",MB_OK); Rez=false; goto Ex;
     }
     TakeMagazine(&Me,p->Nam);
    }
    break;
   case 'И': NoI=false;
    for(p=P->R,i=0;p;i++,p=p->Sled){
     switch(i){
      case 0:
       if(!p->Nam){
        Application->MessageBox(Mes[2],"ОШИБКА",MB_OK); Rez=false; goto Ex;
       }
       if(!FindVary(p->Nam,&K)){
        Application->MessageBox(Mes[3],"ОШИБКА",MB_OK); Rez=false; goto Ex;
       }
       Pv=K->P; K->P=0;
       break;
      case 1:
       if(p->Nam){
        if(FindVary(p->Nam,&V)) Bg=Val(V->Znach);
        else{
         Application->MessageBox(Mes[4],"ОШИБКА",MB_OK); Rez=false; goto Ex;
        }
       }
       break;
      case 2:
       if(p->Nam){
        if(FindVary(p->Nam,&V)) En=Val(V->Znach);
        else{
         Application->MessageBox(Mes[5],"ОШИБКА",MB_OK); Rez=false; goto Ex;
        }
       }
       else
        En=p->Val;
       break;
      case 3:
       if(p->Nam){
        if(FindVary(p->Nam,&V)) Ki=Round(Val(V->Znach));
        else{
         Application->MessageBox(Mes[6],"ОШИБКА",MB_OK); Rez=false; goto Ex;
       }}
       else
        Ki=Round(p->Val);
       if(!Ki){
        Application->MessageBox(Mes[7],"ОШИБКА",MB_OK); Rez=false; goto Ex;
       }
       break;
      default:
       Application->MessageBox(Mes[2],"ОШИБКА",MB_OK); Rez=false; goto Ex;
    }}
    break;
   case 'К': NoK=false;
    for(p=P->R;p;p=p->Sled){
     if(!p->Nam){
      Application->MessageBox(Mes[8],"ОШИБКА",MB_OK); Rez=false; goto Ex;
     }
     TakeVary(p->Nam); TakeMagazine(&Mk,p->Nam);
    }
    break;
   default:
    Application->MessageBox(Mes[0],"ОШИБКА",MB_OK); Rez=false;
 }}
 if(NoE||NoK||NoI){
  Application->MessageBox(Mes[0],"ОШИБКА",MB_OK); Rez=false; goto Ex;
 }
 h=(En-Bg)/(Ki-1);
// strcpy(TxtFile,OprFile); if(s=strrchr(TxtFile,'.')) strcpy(s+1,"txt");
// TXTFILE=fopen(TxtFile,access(OprFile,0)?"w":"a");
 if(access(OprFile,0)){
  if(OPRFILE=fopen(OprFile,"w"))
   fprintf(OPRFILE,
    "<html>\n<head>\n<title>ПРОТОКОЛ ОПЕРАТОРОВ</title>\n"
    "<meta http-equiv=\"Content-Type\" content=\"text/html;"
    "charset=windows-1251\">\n</head>\n"
    "<body bgcolor=\"#FFFFFF\" text=\"#800000\""
    " background=\"%s/fon2.jpg\""
    " font face=\"Times New Roman, Times, serif\">\n",WORKDIR);
 }
 else
  OPRFILE=fopen(OprFile,"a");
// if(OPRFILE=fopen(OprFile,access(OprFile,0)?"w":"a")){
//  char NameK[1024]; strcpy(NameK,K->Name); Grek(NameK);
//  fprintf(OPRFILE,
//   "<html>\n<head>\n<title>ПРОТОКОЛ ОПЕРАТОРА \"УСРЕДНИТЬ\" строки %d</title>\n"
//   "<meta http-equiv=\"Content-Type\" content=\"text/html;"
//   "charset=windows-1251\">\n</head>\n"
//   "<body bgcolor=\"#FFFFFF\" text=\"#800000\""
//   " background=\"%s/fon2.jpg\""
//   " font face=\"Times New Roman, Times, serif\">\n",NLine,WORKDIR);
//  if(TXTFILE){
//   sprintf(Inf,"ПРОТОКОЛ ОПЕРАТОРА \"УСРЕДНИТЬ\" строки %d\n\n\n",NLine);
//   Fputs(Inf,TXTFILE);
//  }
 if(OPRFILE){
  char NameK[1024]; strcpy(NameK,K->Name); Grek(NameK);
  fprintf(OPRFILE,"<h3 align=\"center\"><font color=\"#800000\">"
   "Результат работы оператора \"УСРЕДНИТЬ\" строки %d:</font></h3>\n",NLine);
  fprintf(OPRFILE,
   "Приведением к координате %s<br>\n"
   "получена характеристика эквивалентного инерционного элемента:<br>\n<br>\n",
   NameK);
//  if(TXTFILE){
//   sprintf(Inf,
//    "Приведением к координате %s\n"
//    "получена характеристика эквивалентного инерционного элемента:\n\n",
//    K->Name);
//   Fputs(Inf,TXTFILE);
//  }
  if(OTLADKA){ F=fopen("Otladka","w"); fclose(F); }
  for(Magazine *m=Me;m;m=m->Sled){ Form FK,f,f1; int L;
   if(!FindElem(m->S,&E)) FindCloseElem(m->S,&E); FK.V=E->K;
   Fe=Fe+E->Znach*((DifBy(FK,K))^2);
   if(OTLADKA){ F=fopen("Otladka","a"); PrintForm(true,F,"Fe",Fe); fclose(F); }
   if(E->Name[0]!='@'){
    L=sprintf(Name,"@%s",E->Name);
    E->Name=(char *)calloc(L+1,SzC); strcpy(E->Name,Name);
  }}
//  DelAllMagazine(&VarsInForm);
  PrintForm(false,OPRFILE,NULL,Fe); fputs("<br>\n<br>\nЗдесь:<br>\n<br>\n",OPRFILE);
//  if(TXTFILE){
//   PrintForm(true,TXTFILE,NULL,Fe); strcpy(Inf,"\n\nЗдесь:\n\n"); Fputs(Inf,TXTFILE);
//  }
//  for(Magazine *m=VarsInForm;m;m=m->Sled){
  for(V=L->V;V;V=V->Sled){
   if(VarInForm(true,Fe,V)){
//   if(FindVary(m->S,&V)){
    PrintForm(false,OPRFILE,V->Name,V->Znach); fputs("<br>\n",OPRFILE);
//    if(TXTFILE){
//     PrintForm(true,TXTFILE,V->Name,V->Znach); fputs("\n",TXTFILE);
//    }
  }}
  fputs("<br>\nВ результате создан",OPRFILE);
//  if(TXTFILE){
//   strcpy(Inf,"\nВ результате создан"); Fputs(Inf,TXTFILE);
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
   fprintf(OPRFILE,"ы следующие усредненные элементы:<br>\n");
//   if(TXTFILE){
//    sprintf(Inf,"ы следующие усредненные элементы:\n"); Fputs(Inf,TXTFILE);
//   }
  }
  else{
   fprintf(OPRFILE," следующий усредненный элемент:<br>\n");
//   if(TXTFILE){
//    sprintf(Inf," следующий усредненный элемент:\n"); Fputs(Inf,TXTFILE);
//   }
  }
  for(Magazine *m=Mk;m;m=m->Sled){
   V=TakeVary(m->S);
   sprintf(Name,"J@.%s",m->S); E=TakeElem(Name); E->Znach.C=C; E->K=V;
   PrintForm(false,OPRFILE,E->Name,E->Znach); fputs("<br>\n",OPRFILE);
//   if(TXTFILE){
//    PrintForm(true,TXTFILE,E->Name,E->Znach); fputs("\n",TXTFILE);
//   }
  }
  fprintf(OPRFILE,"<hr></body>\n</html>\n"); fclose(OPRFILE);
//  if(TXTFILE){ fputs("\n\n",TXTFILE); fclose(TXTFILE); }
 }
// if(R&&(TXTFILE=fopen(TxtFile,"r"))){
//  R->Clear();
//  while(Fgets(Inf,1023,TXTFILE)){ Inf[strlen(Inf)-1]='\0'; R->Lines->Add(Inf); }
//  fclose(TXTFILE);
// }
Ex: DelAllMagazine(&Me); DelAllMagazine(&Mk);
return Rez;
}
//---------------------------------------------------------------------------

