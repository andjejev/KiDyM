//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "math.h"
#include "Manipul.h"
#include "ConvDW.h"
#include <stdio.h>
#include <io.h>
//---------------------------------------------------------------------------

#pragma package(smart_init)
extern Cnst *Nul,*Odin,*Pust,*MinusOdin;
extern char Inf[]; extern List *L; extern bool IsError;
Form cosFi,sinFi,msinFi,Fi,Fit; extern bool Otladka;
//---------------------------------------------------------------------------
void TakeCoSinFi(Parm *P){ char *Name;
 if(P->Nam){
  Name=P->Nam; if(*Name=='-') Name++;
  Fi.V=TakeVary(Name);
  cosFi=cos(Fi);
  if(*P->Nam=='-'){ sinFi=-sin(Fi); msinFi=sinFi; }
  else{ sinFi=sin(Fi); msinFi=-sinFi; }
 }
 else{ Form F;
  if(P->Val==90.0){ cosFi.C=Nul; sinFi.C=Odin; msinFi=-sinFi; }
  else if(P->Val==180.0){
   cosFi.C=Odin; cosFi=-cosFi; sinFi.C=Nul; msinFi.C=Nul;
  }
  else if(P->Val==-90.0){ cosFi.C=Nul; msinFi.C=Odin; sinFi=-msinFi; }
  else{
   F.C=TakeCnst(cos(P->Val*M_PI/180)); cosFi=F;
   F.C=TakeCnst(sin(P->Val*M_PI/180)); sinFi=F; msinFi=-sinFi;
}}}
//---------------------------------------------------------------------------
Form MakeZnach(Parm *P){ Form F; char *Name;
 if(P->Nam){
  Name=P->Nam; if(*Name=='-') Name++;
  F.V=TakeVary(Name); if(P->Nam[0]=='-') F=-F;
 }
 else F.C=TakeCnst(P->Val);
 return F;
}
//---------------------------------------------------------------------------
Form TakeZnach(Parm *P){ Form F; char *Name; Vary *V;
 if(P->R){
  if(P->R->Nam){
   Name=P->R->Nam; if(*Name=='-') Name++;
   if(!FindVary(Name,&V)){
    sprintf(Inf,"Переменной \"%s\" не задано значение в исходных данных",Name);
    Application->MessageBox(Inf,"ОШИБКА",MB_OK); F.C=Pust;
   }
   else
    F.V=TakeVary(Name);
   if(P->R->Nam[0]=='-') F=-F;
  }
  else F.C=TakeCnst(P->R->Val);
 }
 else{
  if(P->Nam){
   Name=P->Nam; if(*Name=='-') Name++;
   if(!FindVary(Name,&V)){
    sprintf(Inf,"Переменной \"%s\" не задано значение в исходных данных",Name);
    Application->MessageBox(Inf,"ОШИБКА",MB_OK); F.C=Pust;
   }
   else
    F.V=TakeVary(Name);
   if(P->Nam[0]=='-') F=-F;
  }
  else F.C=TakeCnst(P->Val);
 }
 return F;
}
//---------------------------------------------------------------------------
Form MinusMakeZnach(Parm *P){ Form F; char *Name;
 if(P->Nam){
  Name=P->Nam; if(*Name=='-') Name++;
  F.V=TakeVary(Name);
  if(P->Nam[0]!='-') F=-F;
 }
 else F.C=TakeCnst(-P->Val);
 return F;
}
//---------------------------------------------------------------------------
void MultTransMatrVect(Form M[3][3],Form V[3]){
 Form Rez[3]; int i,j;
 for(i=0;i<3;i++){
  Rez[i].C=Nul; for(j=0;j<3;j++) Rez[i]=Rez[i]+V[j]*M[j][i];
 }
 for(i=0;i<3;i++) V[i]=Rez[i];
}
//---------------------------------------------------------------------------
void MultMatrVect(Form M[3][3],Form V[3]){
 Form Rez[3]; int i,j;
 for(i=0;i<3;i++){
  Rez[i].C=Nul; for(j=0;j<3;j++) Rez[i]=Rez[i]+V[j]*M[i][j];
 }
 for(i=0;i<3;i++) V[i]=Rez[i];
}
//---------------------------------------------------------------------------
void AddVect(Form A[3],Form B[3],Form C[3]){
 for(int i=0;i<3;i++) C[i]=A[i]+B[i];
}
//---------------------------------------------------------------------------
void MultMatrMatr(Form A[3][3],Form B[3][3],Form C[3][3]){
 int i,j,k;
 for(i=0;i<3;i++) for(j=0;j<3;j++){
  C[i][j].C=Nul;
  for(k=0;k<3;k++) C[i][j]=C[i][j]+B[k][j]*A[i][k];
}}
//---------------------------------------------------------------------------
void MultMatrMatr(Form A[3][3],Form B[3][3]){ Form C[3][3];
 MultMatrMatr(A,B,C);
 for(int i=0;i<3;i++) for(int j=0;j<3;j++) A[i][j]=C[i][j];
}
//---------------------------------------------------------------------------
void MatrMultMatr(Form A[4][4],Form B[4][4]){ Form C[4][4];
 int i,j,k;
 for(i=0;i<4;i++) for(j=0;j<4;j++){
  C[i][j].C=Nul;
  for(k=0;k<4;k++) C[i][j]=C[i][j]+B[k][j]*A[i][k];
 }
 for(i=0;i<4;i++) for(j=0;j<4;j++) A[i][j]=C[i][j];
}
//---------------------------------------------------------------------------
void PrintFormVect(char *File,char *Comm,char *NameVect,Form V[3]){
 FILE *F=fopen(File,"a"); char Name[256];
 sprintf(Name,"\n%s\n",Comm); fputs(StrWINtoDOS(Name),F);
 for(int i=0;i<3;i++){
  sprintf(Name,"%s[%d]",NameVect,i);
  PrintForm(true,F,Name,V[i]); fputc(' ',F);
 }
 fclose(F);
}
//---------------------------------------------------------------------------
void PrintVect(char *File,char *Comm,char *NameVect,Form V[4]){
 FILE *F=fopen(File,"a"); char Name[256];
 sprintf(Name,"\n%s",Comm); fputs(StrWINtoDOS(Name),F);
 for(int i=0;i<4;i++){
  sprintf(Name,"%s[%d]",NameVect,i);
  PrintForm(true,F,Name,V[i]);
 }
 fclose(F);
}
//---------------------------------------------------------------------------
void PrintFormMatr(char *File,char *Comm,char *NameMatr,Form M[3][3]){
 FILE *F=fopen(File,"a"); char Name[256];
 sprintf(Name,"\n%s\n",Comm); fputs(StrWINtoDOS(Name),F);
 for(int i=0;i<3;i++){
  for(int j=0;j<3;j++){
   sprintf(Name,"%s[%d][%d]",NameMatr,i,j);
   PrintForm(true,F,Name,M[i][j]); fputc(' ',F);
  }
  fputc('\n',F);
 }
 fclose(F);
}
//---------------------------------------------------------------------------
void PrintMatr(char *File,char *Comm,char *NameMatr,Form M[4][4]){
 FILE *F=fopen(File,"a"); char Name[256];
 sprintf(Name,"\n%s",Comm); fputs(StrWINtoDOS(Name),F);
 for(int i=0;i<4;i++){
  for(int j=0;j<4;j++){
   sprintf(Name,"%s[%d][%d]",NameMatr,i,j);
   PrintForm(true,F,Name,M[i][j]);
 }}
 fclose(F);
}
//---------------------------------------------------------------------------
bool Manipulator(Parm *Par,TRichEdit *R){
 char *LinkName=NULL,Mes1[512],Mes2[512],Mes3[512],Mes4[512],Mes5[512],
  NameCord[256],NameElem[256];
 Form wk_1[3],Sk_1[3][3],Skk_1[3][3],Sk_10[3][3],F1,F2,Ck_1Ok[3],CkOk[3],rCk[3];
 int i,j,N; Vary *K; Parm *P=Par; Root *I;
 Elem *Jmx,*Jmy,*Jmz,*Jwx,*Jwy,*Jwz;
 Otladka=FindInst("ОТЛАДКА",&I);
 if(Otladka) unlink("Otladka");
 Coord(L);
 for(i=0;i<3;i++){//начальные значения
  wk_1[i].C=Nul; Sk_10[i][i].C=Odin; rCk[i].C=Nul;
  Ck_1Ok[i].C=Nul; CkOk[i].C=Nul;
  for(j=i+1;j<3;j++) Sk_10[i][j].C=Sk_10[j][i].C=Nul;
 }
 if(Otladka){
  PrintFormVect("Otladka","Начало","wk_1",wk_1);
  PrintFormVect("Otladka","","rCk",rCk);
  PrintFormVect("Otladka","","Ck_1Ok",Ck_1Ok);
  PrintFormVect("Otladka","","CkOk",CkOk);
  PrintFormMatr("Otladka","","Sk_10",Sk_10);
 }
 sprintf(Mes2,
  "В секции \"Инерция\" надо перечислить в скобках название\n"
  "инерционного параметра (\"m\", \"Jx\", \"Zy\" или \"Jz\") и его\n"
  "значение, причем в качестве значения может быть число или\n"
  "переменная, формула для вычисления которой приведена в ис-\n"
  "ходных данных.");
 sprintf(Mes3,
  "В секции \"Поворот\" надо перечислить в скобках пары\n"
  "ось-угол элементарного поворота, обозначающих последо-\n"
  "вательность поворотов систем координат - от предыдущего\n"
  "звена к текущему. Оси поворотов обозначаются: \"Rx\", \"Ry\"\n"
  "или \"Rz\". Значение углов поворота задаются в градусах\n"
  "или именем переменной, формула для вычисления которой\n"
  "приведена в исходных данных.");
 sprintf(Mes4,
  "Оператор \"Манипулятор\" должен содержать последовательное\n"
  "описание звеньев, каждое содержит следующий порядок секций:\n\n"
  "\"Звено №(Название),Поворот(...),Сдвиг(...),Инерция(...)\",\n"
  "где № - цифра 1,2,..., оюозначающая порядковый номер звена."
 );
 sprintf(Mes5,
  "В секции \"Сдвиг\" надо перечислить координаты - сначала\n"
  "промежуточной точки на оси кинематической пары в системе\n"
  "координат предыдущего звена, затем вектора эта промежу-\n"
  "точная точка - центр масс текущего звена в системе коор-\n"
  "динат текущего звена. Координаты могут задаваться числами\n"
  "или переменными, формулы для вычисления которых приведены\n"
  "в исходных данных.");
//unlink("Otladka");
 do{
  if((KolElem(P->R)==1)&&(strstr(P->Nam,"Звено")==P->Nam)){
   if(P->Nam) LinkName=P->Nam;
   else{
    sprintf(Mes1,"Надо бы дать название звену - \"%s\"!",P->Nam);
    Application->MessageBox(Mes1,"ВНИМАНИЕ",MB_OK);
  }}
  else{
   Application->MessageBox(Mes4,"ВНИМАНИЕ",MB_OK); return false;
  }
  P=P->Sled;
  if(P){
   if(P->R){
    if(!strcmp(P->Nam,"Поворот")){
     N=KolElem(P->R);
     if(N%2){
      Application->MessageBox(Mes3,"ОШИБКА",MB_OK); return false;
     }
     for(i=0;i<3;i++){
      Skk_1[i][i].C=Odin; Sk_1[i][i].C=Odin;
      for(j=i+1;j<3;j++)
       Skk_1[i][j].C=Skk_1[j][i].C=Sk_1[i][j].C=Sk_1[j][i].C=Nul;
     }
     if(Otladka){
      PrintFormMatr("Otladka","Поворот","Skk_1",Skk_1);
      PrintFormMatr("Otladka","","Sk_1",Sk_1);
     }
     for(Parm *p=P->R;p;p=p->Sled){ char *Name;
      if(!p->Nam){
       Application->MessageBox(Mes3,"ОШИБКА",MB_OK); return false;
      }
      if(strcmp(p->Nam,"Rx")&&strcmp(p->Nam,"Ry")&&strcmp(p->Nam,"Rz")){
       Application->MessageBox(Mes3,"ОШИБКА",MB_OK); return false;
      }
      else{ int Ind;
       Name=p->Nam; //p=p->Sled;
       TakeCoSinFi(p->R);
       //накопим матрицу для радиуса-вектора
       if(!strcmp(Name,"Rx")){
        for(i=0;i<3;i++){
         F1=Skk_1[i][1]*cosFi+Skk_1[i][2]*sinFi;
         F2=Skk_1[i][1]*msinFi+Skk_1[i][2]*cosFi;
         Skk_1[i][1]=F1; Skk_1[i][2]=F2;
        }
        if(Otladka) PrintFormMatr("Otladka","Rx","Skk_1",Skk_1);
       }
       else if(!strcmp(Name,"Ry")){
        for(i=0;i<3;i++){
         F1=Skk_1[i][0]*cosFi+Skk_1[i][2]*msinFi;
         F2=Skk_1[i][0]*sinFi+Skk_1[i][2]*cosFi;
         Skk_1[i][0]=F1; Skk_1[i][2]=F2;
        }
        if(Otladka) PrintFormMatr("Otladka","Ry","Skk_1",Skk_1);
       }
       else{
        for(i=0;i<3;i++){
         F1=Skk_1[i][0]*cosFi+Skk_1[i][1]*sinFi;
         F2=Skk_1[i][0]*msinFi+Skk_1[i][1]*cosFi;
         Skk_1[i][0]=F1; Skk_1[i][1]=F2;
        }
        if(Otladka) PrintFormMatr("Otladka","Rz","Skk_1",Skk_1);
       }
       if(p->Nam&&VassalOfKoord(L,TakeVary(p->Nam))){
//        TakeVary(P->Name)->Znach.C==Pust){
//поворот на об.координату, добавим матрицу поворота
        if(!strcmp(Name,"Rx")){
         for(i=0;i<3;i++){
          F1=Sk_1[1][i]*cosFi+Sk_1[2][i]*sinFi;
          F2=Sk_1[1][i]*msinFi+Sk_1[2][i]*cosFi;
          Sk_1[1][i]=F1; Sk_1[2][i]=F2;
         }
         if(Otladka) PrintFormMatr("Otladka","Rx(об.к-та)","Sk_1",Sk_1);
        }
        else if(!strcmp(Name,"Ry")){
         for(i=0;i<3;i++){
          F1=Sk_1[0][i]*cosFi+Sk_1[2][i]*msinFi;
          F2=Sk_1[0][i]*sinFi+Sk_1[2][i]*cosFi;
          Sk_1[0][i]=F1; Sk_1[2][i]=F2;
         }
         if(Otladka) PrintFormMatr("Otladka","Ry(об.к-та)","Sk_1",Sk_1);
        }
        else{
         for(i=0;i<3;i++){
          F1=Sk_1[0][i]*cosFi+Sk_1[1][i]*sinFi;
          F2=Sk_1[0][i]*msinFi+Sk_1[1][i]*cosFi;
          Sk_1[0][i]=F1; Sk_1[1][i]=F2;
         }
         if(Otladka) PrintFormMatr("Otladka","Rz(об.к-та)","Sk_1",Sk_1);
        }
        //повернем вектор накопленной угловой скорости и добавим новую
        MultMatrVect(Sk_1,wk_1);
        for(i=0;i<3;i++){
         Sk_1[i][i].C=Odin; for(j=i+1;j<3;j++) Sk_1[i][j].C=Sk_1[j][i].C=Nul;
        }
        if(Otladka) PrintFormVect("Otladka","повернем","wk_1",wk_1);
        if(!strcmp(Name,"Rx")) Ind=0;
        else if(!strcmp(Name,"Ry")) Ind=1;
        else Ind=2;
        sprintf(Inf,"%s'%s",P->Nam,L->Time->Name);
        Fit.V=TakeVary(Inf);
        wk_1[Ind]=wk_1[Ind]+Fit;
        if(Otladka) PrintFormVect("Otladka","добавим","wk_1",wk_1);
       }
       else{//поворот на постоянный угол
        if(!strcmp(Name,"Rx")){
         F1=wk_1[1]*cosFi+wk_1[2]*sinFi;
         F2=wk_1[1]*msinFi+wk_1[2]*cosFi;
         wk_1[1]=F1; wk_1[2]=F2;
         if(Otladka) PrintFormVect("Otladka","повернем на пост.угол Rx","wk_1",wk_1);
        }
        else if(!strcmp(Name,"Ry")){
         F1=wk_1[0]*cosFi+wk_1[2]*msinFi;
         F2=wk_1[0]*sinFi+wk_1[2]*cosFi;
         wk_1[0]=F1; wk_1[2]=F2;
         if(Otladka) PrintFormVect("Otladka","повернем на пост.угол Ry","wk_1",wk_1);
        }
        else{
         F1=wk_1[0]*cosFi+wk_1[1]*sinFi;
         F2=wk_1[0]*msinFi+wk_1[1]*cosFi;
         wk_1[0]=F1; wk_1[1]=F2;
         if(Otladka) PrintFormVect("Otladka","повернем на пост.угол Rz","wk_1",wk_1);
     }}}}
     if(Otladka) PrintFormMatr("Otladka","Sk_10*=Skk_1","Sk_10",Sk_10);
    }
    if(!strcmp(P->Nam,"Сдвиг")){ Parm *p;
     N=KolElem(P->R);
     if(N!=6){ Application->MessageBox(Mes5,"ОШИБКА",MB_OK); return false; }
     for(i=0,p=P->R;p&&i<3;i++,p=p->Sled)
      Ck_1Ok[i]=MakeZnach(p);
     if(Otladka) PrintFormVect("Otladka","Сдвиг, прочитали","Ck_1Ok",Ck_1Ok);
     for(i=0,p=P->R;p&&i<3;i++,p=p->Sled)
      CkOk[i]=MinusMakeZnach(p);
     if(Otladka) PrintFormVect("Otladka","Сдвиг, прочитали","CkOk",CkOk);
   //Определение положения центра масс
     if((CkOk[0].C!=Nul)||(CkOk[1].C!=Nul)||(CkOk[2].C!=Nul)){
//if(Otladka) PrintFormMatr("Otladka",LinkName,"Skk_1",Skk_1);
      MultMatrVect(Skk_1,CkOk);
     if(Otladka) PrintFormVect("Otladka","CkOk=Skk_1*CkOk","CkOk",CkOk);
//if(Otladka) PrintFormVect("Otladka",LinkName,"OkCk",OkCk);
      AddVect(Ck_1Ok,CkOk,Ck_1Ok);
      if(Otladka) PrintFormVect("Otladka","Ck_1Ok+=CkOk","Ck_1Ok",Ck_1Ok);
     }
     if((Ck_1Ok[0].C!=Nul)||(Ck_1Ok[1].C!=Nul)||(Ck_1Ok[2].C!=Nul)){
//if(Otladka) PrintFormMatr("Otladka",LinkName,"Sk_10",Sk_10);
      MultMatrVect(Sk_10,Ck_1Ok);
      if(Otladka) PrintFormVect("Otladka","Ck_1Ok=Sk_10*Ck_1Ok","Ck_1Ok",Ck_1Ok);
//if(Otladka) PrintFormVect("Otladka","","rCk",rCk);
      AddVect(rCk,Ck_1Ok,rCk);
      if(Otladka) PrintFormVect("Otladka","rCk+=Ck_1Ok","rCk",rCk);
     }
     MultMatrMatr(Sk_10,Skk_1);
    }
    if(!strcmp(P->Nam,"Инерция")){ Parm *p;
     N=KolElem(P->R);
     if(N%2){ Application->MessageBox(Mes2,"ОШИБКА",MB_OK); return false; }
     for(p=P->R;p;p=p->Sled){
      if(!p->Nam){ Application->MessageBox(Mes2,"ОШИБКА",MB_OK); return false; }
      if(!strcmp(p->Nam,"m")){
       sprintf(NameCord,"xC@%s",LinkName);
       if(!(K=TakeVary(NameCord))) return false;
       sprintf(NameElem,"Jm@%s.%s",LinkName,NameCord);
       if(!(Jmx=TakeElem(NameElem))) return false;
       Jmx->K=K; K->Znach=rCk[0];
       sprintf(NameCord,"yC@%s",LinkName);
       if(!(K=TakeVary(NameCord))) return false;
       sprintf(NameElem,"Jm@%s.%s",LinkName,NameCord);
       if(!(Jmy=TakeElem(NameElem))) return false;
       Jmy->K=K; K->Znach=rCk[1];
       sprintf(NameCord,"zC@%s",LinkName);
       if(!(K=TakeVary(NameCord))) return false;
       sprintf(NameElem,"Jm@%s.%s",LinkName,NameCord);
       if(!(Jmz=TakeElem(NameElem))) return false;
       Jmz->K=K; K->Znach=rCk[2];
//       p=p->Sled;
       Jmx->Znach=TakeZnach(p);
       Jmy->Znach=Jmx->Znach; Jmz->Znach=Jmx->Znach;
      }
      else if(!strcmp(p->Nam,"Jx")){
       sprintf(NameCord,"omegax@%s",LinkName);
       if(!(K=TakeVary(NameCord))) return false;
       sprintf(NameElem,"Jw@%s.%s",LinkName,NameCord);
       if(!(Jwx=TakeElem(NameElem))) return false;
       Jwx->K=K; K->Znach=wk_1[0];
//       p=p->Sled;
       Jwx->Znach=TakeZnach(p);
      }
      else if(!strcmp(p->Nam,"Jy")){
       sprintf(NameCord,"omegay@%s",LinkName);
       if(!(K=TakeVary(NameCord))) return false;
       sprintf(NameElem,"Jw@%s.%s",LinkName,NameCord);
       if(!(Jwy=TakeElem(NameElem))) return false;
       Jwy->K=K; K->Znach=wk_1[1];
//       p=p->Sled;
       Jwy->Znach=TakeZnach(p);
      }
      else if(!strcmp(p->Nam,"Jz")){
       sprintf(NameCord,"omegaz@%s",LinkName);
       if(!(K=TakeVary(NameCord))) return false;
       sprintf(NameElem,"Jw@%s.%s",LinkName,NameCord);
       if(!(Jwz=TakeElem(NameElem))) return false;
       Jwz->K=K; K->Znach=wk_1[2];
//       p=p->Sled;
       Jwz->Znach=TakeZnach(p);
      }
      else{
       Application->MessageBox(Mes2,"ОШИБКА",MB_OK); return false;
   }}}}
   else{
    Application->MessageBox(Mes4,"ВНИМАНИЕ",MB_OK); return false;
  }}
  else{
   Application->MessageBox(Mes4,"ВНИМАНИЕ",MB_OK); return false;
 }}while(P);
 return true;
}
//---------------------------------------------------------------------------
char *TestBody(Body *b,Magazine **M){
 Magazine *m; Body *bb;
 if(!b->BaseName) return NULL; //дошли до конца
//если по пути уже была ссылка на тело, возвращаем его имя - оно закольцовано
 if(FindMagazine(*M,b->BaseName,&m)) return b->BaseName;
//новая ссылка, включаем в магазин, находим ссылочное тело и проверяем его
 TakeMagazine(M,b->BaseName);
 FindBody(b->BaseName,&bb);
 return TestBody(bb,M);
}
//---------------------------------------------------------------------------
bool Accumulate(Body *B){
 Form Skk_1[3][3],wkk_1[3],rCkk_1[3],F,F1,F2; Body *b; int i,j,Ind;
 Parm *p; Root *I; char *Name,Mes[512];
//Построим добавочные векторы и матрицу поворота от системы k-1 к k
 for(i=0;i<3;i++){//начальные значения
  Skk_1[i][i].C=Odin; wkk_1[i].C=rCkk_1[i].C=Nul;
  for(j=i+1;j<3;j++) Skk_1[i][j].C=Skk_1[j][i].C=Nul;
 }
 for(Parm *P=B->KoordSys;P;P=P->Sled){
  switch(P->Nam[0]){
   case 'R':
    TakeCoSinFi(P->R);
    switch(P->Nam[1]){
     case 'x':
//накопим матрицу для радиуса-вектора
      for(i=0;i<3;i++){
       F1=Skk_1[i][1]*cosFi+Skk_1[i][2]*sinFi;
       F2=Skk_1[i][1]*msinFi+Skk_1[i][2]*cosFi;
       Skk_1[i][1]=F1; Skk_1[i][2]=F2;
      }
      if(Otladka) PrintFormMatr("Otladka","Rx","Skk_1",Skk_1);
//повернем накопленную угловую скорость
      F1=wkk_1[1]*cosFi+wkk_1[2]*sinFi;
      F2=wkk_1[1]*msinFi+wkk_1[2]*cosFi;
      wkk_1[1]=F1; wkk_1[2]=F2;
      if(P->R->Nam&&
       VassalOfKoord(L,TakeVary(*P->R->Nam=='-'?P->R->Nam+1:P->R->Nam))||
//       (TakeVary(*P->Name=='-'?P->Name+1:P->Name)->Znach.C==Pust)||
         FindInst("ОБОБЩЕННЫЕ КООРДИНАТЫ",&I)&&
         FindParm(I->First,P->R->Nam,&p)){//поворот на обобщенную координату
       sprintf(Inf,"%s'%s",P->R->Nam,L->Time->Name);
       Fit.V=TakeVary(Inf);
       wkk_1[0]=wkk_1[0]+Fit;
      }
      if(Otladka) PrintFormVect("Otladka","повернем на Rx","wkk_1",wkk_1);
      break;
     case 'y':
//накопим матрицу для радиуса-вектора
      for(i=0;i<3;i++){
       F1=Skk_1[i][0]*cosFi+Skk_1[i][2]*msinFi;
       F2=Skk_1[i][0]*sinFi+Skk_1[i][2]*cosFi;
       Skk_1[i][0]=F1; Skk_1[i][2]=F2;
      }
      if(Otladka) PrintFormMatr("Otladka","Ry","Skk_1",Skk_1);
//повернем накопленную угловую скорость
      F1=wkk_1[0]*cosFi+wkk_1[2]*msinFi;
      F2=wkk_1[0]*sinFi+wkk_1[2]*cosFi;
      wkk_1[0]=F1; wkk_1[2]=F2;
      if(P->R->Nam&&
       VassalOfKoord(L,TakeVary(P->R->Nam))||
//       (TakeVary(P->Name)->Znach.C==Pust)||
         FindInst("ОБОБЩЕННЫЕ КООРДИНАТЫ",&I)&&
         FindParm(I->First,P->R->Nam,&p)){//поворот на обобщенную координату
       sprintf(Inf,"%s'%s",P->R->Nam,L->Time->Name);
       Fit.V=TakeVary(Inf);
       wkk_1[1]=wkk_1[1]+Fit;
      }
      if(Otladka) PrintFormVect("Otladka","повернем на угол Ry","wkk_1",wkk_1);
      break;
     case 'z':
//накопим матрицу для радиуса-вектора
      for(i=0;i<3;i++){
       F1=Skk_1[i][0]*cosFi+Skk_1[i][1]*sinFi;
       F2=Skk_1[i][0]*msinFi+Skk_1[i][1]*cosFi;
       Skk_1[i][0]=F1; Skk_1[i][1]=F2;
      }
      if(Otladka) PrintFormMatr("Otladka","Rz","Skk_1",Skk_1);
//повернем накопленную угловую скорость
      F1=wkk_1[0]*cosFi+wkk_1[1]*sinFi;
      F2=wkk_1[0]*msinFi+wkk_1[1]*cosFi;
      wkk_1[0]=F1; wkk_1[1]=F2;
      if(P->R->Nam&&
       VassalOfKoord(L,TakeVary(P->R->Nam))||
//       (TakeVary(P->Name)->Znach.C==Pust)||
        FindInst("ОБОБЩЕННЫЕ КООРДИНАТЫ",&I)&&
        FindParm(I->First,P->R->Nam,&p)){//поворот на обобщенную координату
       sprintf(Inf,"%s'%s",P->R->Nam,L->Time->Name);
       Fit.V=TakeVary(Inf);
       wkk_1[2]=wkk_1[2]+Fit;
      }
      if(Otladka) PrintFormVect("Otladka","повернем на угол Rz","wkk_1",wkk_1);
      break;
     default:
      sprintf(Mes,"Каждый элементарный поворот связанной системы\n"
       "должен быть представлен одной из записей вида:\n"
       "Rx(угол),Ry(угол),Rz(угол).\n"
       "В скобках стоят имена переменных или числа.");
      Application->MessageBox(Mes,"ВНИМАНИЕ",MB_OK);
      IsError=true; return false;
    }
    break;
   case 'S':
    if(P->R->Nam){
     Name=P->R->Nam; if(*Name=='-') Name++;
     F.V=TakeVary(Name); if(P->R->Nam[0]=='-') F=-F;
    }
    else F.C=TakeCnst(P->R->Val);
    switch(P->Nam[1]){
     case 'x': Ind=0; break;
     case 'y': Ind=1; break;
     case 'z': Ind=2; break;
     default:
      sprintf(Mes,"Каждый сдвиг системы данного тела относительно \"базового\"\n"
       "должен быть представлен одной из записей вида:\n"
       "Sx(сдвиг),Sy(сдвиг),Sz(сдвиг).\n"
       "В скобках стоят имена переменных или числа.");
      Application->MessageBox(Mes,"ВНИМАНИЕ",MB_OK);
      IsError=true; return false;
    }
    for(i=0;i<3;i++)
     rCkk_1[i]=rCkk_1[i]+F*Skk_1[i][Ind];
    if(Otladka) PrintFormVect("Otladka","сдвинем вектор ц.м.","rCkk_1",rCkk_1);
    break;
   default:
    sprintf(Mes,"Описание твердого тела - это запись вида:\n"
     "<Имя>[~<Имя базы>]|<Сдвиги и повороты>|<Параметры инерции>;\n"
     "где <Имя> и <Имя базы> - имена тел - текущего и \"базового\";\n"
     "<Сдвиги и повороты> - список элементов в нужном порядке вида\n"
     "Rx(угол поворота вокруг х),Ry(угол поворота вокруг y),\n"
     "Rz(угол поворота вокруг z),Sx(сдвиг вдоль х),Sy(сдвиг вдоль y),\n"
     "Sz(сдвиг вдоль z);\n"
     "<Параметры инерции> - список необходимых инерционных параметров вида\n"
     "m(масса), Jx(момент инерции относительно оси x), Jy(момент инерции\n"
     "относительно оси y), Jz(момент инерции относительно оси z).\n"
     "Всюду в скобках стоят имена переменных или числа.");
    Application->MessageBox(Mes,"ВНИМАНИЕ",MB_OK);
    IsError=true; return false;
 }}
//сформируем радиус-вектор цм тела, его угловую скорость и матрицу поворота
 if(B->BaseName){ Vary *V;
  FindBody(B->BaseName,&b);
  if(!b->Accum&&!Accumulate(b)) return false;
  MultMatrVect(b->Sk0,rCkk_1);
  AddVect(b->rCk,rCkk_1,B->rCk);
  for(i=0;i<3;i++) B->wkk[i]=b->wkk[i];
  MultTransMatrVect(Skk_1,B->wkk);
  AddVect(B->wkk,wkk_1,B->wkk);
  MultMatrMatr(b->Sk0,Skk_1,B->Sk0);
//вставка 4.7.2007
  for(i=0;i<3;i++){
   if(B->rCk[i]!=Nul){
    sprintf(Inf,"%cC%s~%s",i==2?'z':i==1?'y':'x',B->Name,b->Name);
    if(V=TakeVary(Inf)){ V->Znach=B->rCk[i]; B->rCk[i].V=V; }
   }
   if(B->wkk[i]!=Nul){
    sprintf(Inf,"omega%c%s~%s",i==2?'z':i==1?'y':'x',B->Name,b->Name);
    if(V=TakeVary(Inf)){ V->Znach=B->wkk[i]; B->wkk[i].V=V; }
  }}
//конец вставки 4.7.2007
 }
 else{
  for(i=0;i<3;i++){
   B->rCk[i]=rCkk_1[i]; B->wkk[i]=wkk_1[i];
   for(j=0;j<3;j++) B->Sk0[i][j]=Skk_1[i][j];
 }}
 if(Otladka){
  PrintFormVect("Otladka",B->Name,"B->rCk",B->rCk);
  PrintFormVect("Otladka","","B->wkk",B->wkk);
  PrintFormMatr("Otladka","","B->Sk0",B->Sk0);
 }
 B->Accum=true; return true;
}
//---------------------------------------------------------------------------
bool Bodys(Body *B){
 char Mes[512],NameCord[256],NameElem[256],*inf;
 int i,j,k,N; Vary *K; Parm *P=B->KoordSys; Root *I; Magazine *M=NULL,*m;
 Elem *Jmx,*Jmy,*Jmz,*Jwx,*Jwy,*Jwz;
 Otladka=FindInst("ОТЛАДКА",&I); Body *b,*bb; Inf[0]='\0';
 if(Otladka&&!access("Otladka",0)) unlink("Otladka");
 CoordWithoutDT(L);
//Проверим, что есть все тела
 for(*Inf='\0',inf=Inf,b=B;b;b=b->Sled)
  if(b->BaseName&&!FindBody(b->BaseName,&bb))
   inf+=sprintf(inf,"%s~%s, ",b->Name,b->BaseName);
 if(*inf){
  Inf[strlen(Inf)-2]='\0';
  sprintf(Mes,"Следующие тела\n%s\nссылаются на несуществующие",Inf);
  Application->MessageBox(Mes,"ОШИБКА",MB_OK); IsError=true; return false;
 }
//Проверим, есть ли кольца
 Inf[0]='\0'; inf=Inf;
 for(b=B;b;b=b->Sled){
  DelAllMagazine(&M);
  if(TestBody(b,&M))
   inf+=sprintf(inf,"%s, ",b->BaseName);
 }
 if(*inf){
  Inf[strlen(Inf)-2]='\0';
  sprintf(Mes,"Следующие тела\n%s\nссылаются сами на себя",Inf);
  Application->MessageBox(Mes,"ОШИБКА",MB_OK); IsError=true; return false;
 }
//Сбросим накопительные векторы и матрицу
 for(b=B;b;b=b->Sled){
  for(int i=0;i<3;i++){
   b->rCk[i].C=b->wkk[i].C=Nul; b->Sk0[i][i].C=Odin;
   for(int j=i+1;j<3;j++) b->Sk0[i][j].C=b->Sk0[j][i].C=Nul;
 }}
//Сделаем накопления с помощью рекурсивной процедуры
 for(b=B;b;b=b->Sled){ int Np=0;
  if(!Accumulate(b)){ IsError=true; return false; }
  for(P=b->Inert;P;P=P->Sled){
   if(P->Nam&&!strcmp(P->Nam,"m")||!P->R&&!Np){
    sprintf(NameCord,"xC@%s",b->Name);
    if(!(K=TakeVary(NameCord))){ IsError=true; return false; }
    sprintf(NameElem,"Jm@%s.%s",b->Name,NameCord);
    if(!(Jmx=TakeElem(NameElem))){ IsError=true; return false; }
    Jmx->K=K; K->Znach=b->rCk[0];
    sprintf(NameCord,"yC@%s",b->Name);
    if(!(K=TakeVary(NameCord))){ IsError=true; return false; }
    sprintf(NameElem,"Jm@%s.%s",b->Name,NameCord);
    if(!(Jmy=TakeElem(NameElem))){ IsError=true; return false; }
    Jmy->K=K; K->Znach=b->rCk[1];
    sprintf(NameCord,"zC@%s",b->Name);
    if(!(K=TakeVary(NameCord))){ IsError=true; return false; }
    sprintf(NameElem,"Jm@%s.%s",b->Name,NameCord);
    if(!(Jmz=TakeElem(NameElem))){ IsError=true; return false; }
    Jmz->K=K; K->Znach=b->rCk[2];
    Jmx->Znach=TakeZnach(P);
    Jmy->Znach=Jmx->Znach; Jmz->Znach=Jmx->Znach;
   }
   else if(P->Nam&&!strcmp(P->Nam,"Jx")||!P->R&&(Np==1)){
    sprintf(NameCord,"omegax@%s",b->Name);
    if(!(K=TakeVary(NameCord))){ IsError=true; return false; }
    sprintf(NameElem,"Jw@%s.%s",b->Name,NameCord);
    if(!(Jwx=TakeElem(NameElem))){ IsError=true; return false; }
    Jwx->K=K; K->Znach=b->wkk[0]; Jwx->Znach=TakeZnach(P);
   }
   else if(P->Nam&&!strcmp(P->Nam,"Jy")||!P->R&&(Np==2)){
    sprintf(NameCord,"omegay@%s",b->Name);
    if(!(K=TakeVary(NameCord))){ IsError=true; return false; }
    sprintf(NameElem,"Jw@%s.%s",b->Name,NameCord);
    if(!(Jwy=TakeElem(NameElem))){ IsError=true; return false; }
    Jwy->K=K; K->Znach=b->wkk[1]; Jwy->Znach=TakeZnach(P);
   }
   else if(P->Nam&&!strcmp(P->Nam,"Jz")||!P->R&&(Np==3)){
    sprintf(NameCord,"omegaz@%s",b->Name);
    if(!(K=TakeVary(NameCord))){ IsError=true; return false; }
    sprintf(NameElem,"Jw@%s.%s",b->Name,NameCord);
    if(!(Jwz=TakeElem(NameElem))){ IsError=true; return false; }
    Jwz->K=K; K->Znach=b->wkk[2]; Jwz->Znach=TakeZnach(P);
   }
   else{
    sprintf(Mes,"В описание твердого тела входят\n"
     "Параметры инерции - список необходимых инерционных параметров вида\n"
     "m(масса), Jx(момент инерции), Jy(момент инерции), Jz(момент инерции).\n"
     "Всюду в скобках стоят имена переменных или числа. Если требуется\n"
     "указать все параметры можно пользоваться укороченной записью:\n"
     "m,Jx,Jy,Jz, где членами списка могут быть числа или переменные,\n"
     "стоящие именно в таком порядке - масса, затем главные центральные\n"
     "моменты инерции звена.");
    Application->MessageBox(Mes,"ОШИБКА",MB_OK); IsError=true; return false;
   }
   Np++;
 }}
//построим кинетическую энергию по методу В.Б.Ларина
/*
 Form A[4][4],To[4][4],U[6][4][4],J[4],D[6][6],L; FILE *FF;
 L.C=Nul;
 for(b=B;b;b=b->Sled){ Vary *V; Form F;
  for(i=0;i<4;i++){
   To[i][i].C=Odin; for(j=i+1;j<4;j++) To[i][j].C=To[j][i].C=Nul;
  }
  for(Parm *p,*P=b->KoordSys;P;P=P->Sled){
   if(P->Name&&(TakeVary(P->Name)->Znach.C==Pust)||
      FindInst("ОБОБЩЕННЫЕ КООРДИНАТЫ",&I)&&
      FindParm(I->First,P->Name,&p)){
//смещение или поворот на обобщенную координату
    for(i=0;i<4;i++){
     A[i][i].C=Odin; for(j=i+1;j<4;j++) A[i][j].C=A[j][i].C=Nul;
    }
    F.V=TakeVary(P->Name);
    if(P->Head[0]=='R'){
     switch(P->Head[1]){
      case 'x':
       A[2][1]=sin(F);
       A[1][2]=-A[2][1];
       A[1][1]=cos(F); A[2][2]=cos(F);
       break;
      case 'y':
       A[0][0]=cos(F); A[2][2]=cos(F); A[0][2]=sin(F);
       A[2][0]=-A[0][2]; break;
      case 'z':
       A[0][0]=cos(F); A[1][1]=cos(F); A[1][0]=sin(F);
       A[0][1]=-A[1][0]; break;
    }}
    else{
     switch(P->Head[1]){
      case 'x': A[0][3]=F; break;
      case 'y': A[1][3]=F; break;
      case 'z': A[2][3]=F; break;
    }}
if(Otladka) PrintMatr("Otladka","","A",A);
if(Otladka) PrintMatr("Otladka","До To*A","To",To);
    MatrMultMatr(To,A);
if(Otladka) PrintMatr("Otladka","После To*A","To",To);
  }}
  for(k=0;k<6;k++) for(i=0;i<4;i++) for(j=0;j<4;j++) U[k][i][j].C=Nul;
  k=0;
  for(Parm *p,*P=b->KoordSys;P;P=P->Sled){
   if(P->Name&&(TakeVary(P->Name)->Znach.C==Pust)||
      FindInst("ОБОБЩЕННЫЕ КООРДИНАТЫ",&I)&&
      FindParm(I->First,P->Name,&p)){
    Vary *V=TakeVary(P->Name);
    for(i=0;i<4;i++) for(j=0;j<4;j++) U[k][i][j]=DifBy(To[i][j],V);
sprintf(Inf,"U[%d]",k);
if(Otladka) PrintMatr("Otladka","U",Inf,U[k]);
    k++;
  }}
  k=0; J[0].C=J[1].C=J[2].C=J[3].C=Nul;
  for(Parm *P=b->Inert;P;P=P->Sled){
   Vary *V=TakeVary(P->Name); Form F; F.V=V;
   switch(k++){
    case 0: J[3]=F; break;
    case 1: J[0]=J[0]-0.5*F; J[1]=J[1]+0.5*F; J[2]=J[2]+0.5*F; break;
    case 2: J[0]=J[0]+0.5*F; J[1]=J[1]-0.5*F; J[2]=J[2]+0.5*F; break;
    case 3: J[0]=J[0]+0.5*F; J[1]=J[1]+0.5*F; J[2]=J[2]-0.5*F; break;
  }}
if(Otladka){
PrintVect("Otladka","","J",J);
FF=fopen("Otladka","a");
}
  for(i=0;i<6;i++){ Form u;
   D[i][i].C=Nul;
   for(k=0;k<4;k++){
    u.C=Nul; for(int l=0;l<4;l++) u=u+U[i][k][l]*U[i][k][l];
    D[i][i]=D[i][i]+J[k]*u;
   }
   for(j=i+1;j<6;j++){
    D[i][j].C=Nul;
    for(k=0;k<4;k++){
     u.C=Nul; for(int l=0;l<4;l++) u=u+U[i][k][l]*U[j][k][l];
     D[i][j]=D[i][j]+J[k]*u;
    }
    D[j][i]=D[i][j];
if(Otladka){
sprintf(Inf,"D[%d][%d]",i,j);
PrintForm(true,FF,Inf,D[i][j]);
}
  }}
  i=0;
  for(Parm *p,*P=b->KoordSys;P;P=P->Sled){
   if(P->Name&&(TakeVary(P->Name)->Znach.C==Pust)||
      FindInst("ОБОБЩЕННЫЕ КООРДИНАТЫ",&I)&&
      FindParm(I->First,P->Name,&p)){
    Form FVqi; sprintf(Inf,"%s't",P->Name); FVqi.V=TakeVary(Inf); j=0;
    for(Parm *p,*P=b->KoordSys;P;P=P->Sled){
     if(P->Name&&(TakeVary(P->Name)->Znach.C==Pust)||
        FindInst("ОБОБЩЕННЫЕ КООРДИНАТЫ",&I)&&
        FindParm(I->First,p->Name,&p)){
      Form FVqj; sprintf(Inf,"%s't",P->Name); FVqj.V=TakeVary(Inf);
      L=L+FVqi*FVqj*D[i][j++];
if(Otladka){
PrintForm(true,FF,"FVqi",FVqi);
PrintForm(true,FF,"FVqj",FVqj);
sprintf(Inf,"D[%d][%d]",i,j);
PrintForm(true,FF,Inf,D[i][j]);
PrintForm(true,FF,"L",L);
}
    }}
    i++;
 }}}
if(Otladka) fclose(FF);
 Vary *V=TakeVary("Лямбда"); V->Znach=L;
*/
 return true;
}
//---------------------------------------------------------------------------

