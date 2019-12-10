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
/*    ��� ������������, ��������, ����������� � ����������� ���� ����������
����������� ������������� ������� � ������� ����������-�������� ����������
(���) ������������ �������� "���������".
��� ��������� ������ ��������� � �������������� ������� ���, ����� ������,
������� ������� ������ � ���������, � ����������� ����������� ��������,
����������� ����������� �� ����� ������� ����������� ���� �����������
�������������� ���, ��������� �������� � ������� ����� ���������.
��������� ��������� "���������" ������� � ���������.

  ��������� :
  	�[�������](<������ ���� ��������� � ��������>),
	�[������������ ��](<���������� ��������������>,<��������� ��������>,
                           <�������� ��������>,<���������� ����������>),
        �[���������](<��� ����������>,<��� ����������>);

������ ���������� "�[�������]" ���������� ����������� ��������,
������� ���������� ���������.
������ ���������� "�[������������]" �������� ��� ����������,
�� ������� ����� ������������� �������������� (� ������� �������
�������� ����������� �������������� ������� ���), � ����� � ���������
� �������� ��������, ���������� �������������, �� ������� ����� ������
�������� ��������������.
������ ���������� "�[���������]" �������� ����� ��������� ������ ������������
������, �� ������� ������� ��������� ���������� � ���������� ����������
����������� ��������. */
//---------------------------------------------------------------------------
bool MidlElem(Parm *Par,TRichEdit *R){
 int i,Ki,Pv,NLine=0; Parm *p,*P; Vary *V,*K; Elem *E; Root *I;
 double Bg,En,Sum=0.0,h; Form Fe; Fe.C=Nul; FILE *F; Cnst *C;
 char Mes[10][512],Name[256],TxtFile[256],*s;
 Magazine *Me=NULL,*Mk=NULL; bool NoE=true,NoI=true,NoK=true,Rez=true;
 if(OTLADKA=FindInst("�������",&I)) unlink("Otladka");
 for(Root *o=L->Oper;o;o=o->Sled) if(o->First==Par){ NLine=o->Line; break; }
 sprintf(Mes[0],"�������� \"���������\" ������ ��������� 3 ��������� ������\n"
  "�[�������](<������ ���� ���������>),\n"
  "�[������������ ��](<���������� ��������������>,<��������� ��������>,\n"
  "                   <�������� ��������>,<���������� ����������>),\n"
  "�[���������](<��� ����������1>,<��� ����������2,...>);");
 sprintf(Mes[1],"�������� \"���������\" ������ ��������� � ��������� ������\n"
  "�[�������] ����� ������������ � �������� ������ ����������� ���������,\n"
  "������� ���������� ��������, ��������� � ������� ��� �����������");
 sprintf(Mes[2],"�������� \"���������\" ������ ��������� � ��������� ������\n"
  "�[������������ ��] ��� ���������� ��������������,\n"
  "���������, �������� ��������, ���������� �������������\n"
  "��������� ����������");
 sprintf(Mes[3],"�������� \"���������\" �������� � ��������� ������\n"
  "�[������������ ��] � �������� ����� ���������� ��������������\n"
  "��� �������������� ����������");
 sprintf(Mes[4],"�������� \"���������\" �������� � ��������� ������\n"
  "�[������������ ��] � �������� ���������� ��������\n"
  "��� �������������� ����������");
 sprintf(Mes[5],"�������� \"���������\" �������� � ��������� ������\n"
  "�[������������ ��] � �������� ��������� ��������\n"
  "��� �������������� ����������");
 sprintf(Mes[6],"�������� \"���������\" �������� � ��������� ������\n"
  "�[������������ ��] � �������� ����� ����������\n"
  "��� �������������� ����������");
 sprintf(Mes[7],"�������� \"���������\" �������� � ��������� ������\n"
  "�[������������ ��] � �������� ����� ���������� ����,\n"
  "��� �����������!");
 sprintf(Mes[8],"�������� \"���������\" �������� � ��������� ������\n"
  "�[���������] ������ ����� ���������� ���������� �����");
 for(P=Par;P;P=P->Sled){
  if(!P->Nam){
   Application->MessageBox(Mes[0],"������",MB_OK); Rez=false; goto Ex;
  }
  strcpy(Name,P->Nam); LowToUpp(Name);
  switch(Name[0]){
   case '�': NoE=false;
    for(p=P->R;p;p=p->Sled){
     if(!p->Nam||!FindElem(p->Nam,&E)&&!FindCloseElem(p->Nam,&E)){
      Application->MessageBox(Mes[1],"������",MB_OK); Rez=false; goto Ex;
     }
     TakeMagazine(&Me,p->Nam);
    }
    break;
   case '�': NoI=false;
    for(p=P->R,i=0;p;i++,p=p->Sled){
     switch(i){
      case 0:
       if(!p->Nam){
        Application->MessageBox(Mes[2],"������",MB_OK); Rez=false; goto Ex;
       }
       if(!FindVary(p->Nam,&K)){
        Application->MessageBox(Mes[3],"������",MB_OK); Rez=false; goto Ex;
       }
       Pv=K->P; K->P=0;
       break;
      case 1:
       if(p->Nam){
        if(FindVary(p->Nam,&V)) Bg=Val(V->Znach);
        else{
         Application->MessageBox(Mes[4],"������",MB_OK); Rez=false; goto Ex;
        }
       }
       break;
      case 2:
       if(p->Nam){
        if(FindVary(p->Nam,&V)) En=Val(V->Znach);
        else{
         Application->MessageBox(Mes[5],"������",MB_OK); Rez=false; goto Ex;
        }
       }
       else
        En=p->Val;
       break;
      case 3:
       if(p->Nam){
        if(FindVary(p->Nam,&V)) Ki=Round(Val(V->Znach));
        else{
         Application->MessageBox(Mes[6],"������",MB_OK); Rez=false; goto Ex;
       }}
       else
        Ki=Round(p->Val);
       if(!Ki){
        Application->MessageBox(Mes[7],"������",MB_OK); Rez=false; goto Ex;
       }
       break;
      default:
       Application->MessageBox(Mes[2],"������",MB_OK); Rez=false; goto Ex;
    }}
    break;
   case '�': NoK=false;
    for(p=P->R;p;p=p->Sled){
     if(!p->Nam){
      Application->MessageBox(Mes[8],"������",MB_OK); Rez=false; goto Ex;
     }
     TakeVary(p->Nam); TakeMagazine(&Mk,p->Nam);
    }
    break;
   default:
    Application->MessageBox(Mes[0],"������",MB_OK); Rez=false;
 }}
 if(NoE||NoK||NoI){
  Application->MessageBox(Mes[0],"������",MB_OK); Rez=false; goto Ex;
 }
 h=(En-Bg)/(Ki-1);
// strcpy(TxtFile,OprFile); if(s=strrchr(TxtFile,'.')) strcpy(s+1,"txt");
// TXTFILE=fopen(TxtFile,access(OprFile,0)?"w":"a");
 if(access(OprFile,0)){
  if(OPRFILE=fopen(OprFile,"w"))
   fprintf(OPRFILE,
    "<html>\n<head>\n<title>�������� ����������</title>\n"
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
//   "<html>\n<head>\n<title>�������� ��������� \"���������\" ������ %d</title>\n"
//   "<meta http-equiv=\"Content-Type\" content=\"text/html;"
//   "charset=windows-1251\">\n</head>\n"
//   "<body bgcolor=\"#FFFFFF\" text=\"#800000\""
//   " background=\"%s/fon2.jpg\""
//   " font face=\"Times New Roman, Times, serif\">\n",NLine,WORKDIR);
//  if(TXTFILE){
//   sprintf(Inf,"�������� ��������� \"���������\" ������ %d\n\n\n",NLine);
//   Fputs(Inf,TXTFILE);
//  }
 if(OPRFILE){
  char NameK[1024]; strcpy(NameK,K->Name); Grek(NameK);
  fprintf(OPRFILE,"<h3 align=\"center\"><font color=\"#800000\">"
   "��������� ������ ��������� \"���������\" ������ %d:</font></h3>\n",NLine);
  fprintf(OPRFILE,
   "����������� � ���������� %s<br>\n"
   "�������� �������������� �������������� ������������ ��������:<br>\n<br>\n",
   NameK);
//  if(TXTFILE){
//   sprintf(Inf,
//    "����������� � ���������� %s\n"
//    "�������� �������������� �������������� ������������ ��������:\n\n",
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
  PrintForm(false,OPRFILE,NULL,Fe); fputs("<br>\n<br>\n�����:<br>\n<br>\n",OPRFILE);
//  if(TXTFILE){
//   PrintForm(true,TXTFILE,NULL,Fe); strcpy(Inf,"\n\n�����:\n\n"); Fputs(Inf,TXTFILE);
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
  fputs("<br>\n� ���������� ������",OPRFILE);
//  if(TXTFILE){
//   strcpy(Inf,"\n� ���������� ������"); Fputs(Inf,TXTFILE);
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
   fprintf(OPRFILE,"� ��������� ����������� ��������:<br>\n");
//   if(TXTFILE){
//    sprintf(Inf,"� ��������� ����������� ��������:\n"); Fputs(Inf,TXTFILE);
//   }
  }
  else{
   fprintf(OPRFILE," ��������� ����������� �������:<br>\n");
//   if(TXTFILE){
//    sprintf(Inf," ��������� ����������� �������:\n"); Fputs(Inf,TXTFILE);
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

