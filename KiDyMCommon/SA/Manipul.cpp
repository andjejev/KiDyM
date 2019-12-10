//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "math.h"
#include "Manipul.h"
#include "ConvDW.h"
#include <stdio.h>
#include <io.h>
#include <time.h>
#include "ControlCOFs.h"
//---------------------------------------------------------------------------

#pragma package(smart_init)
extern Cnst *Nul,*Odin,*Pust,*Halb;
extern wchar_t Inf[],OprFile[],WORKDIR[]; extern List *L;
extern bool IsError,OTLADKA,PRINTOP; extern Form TLarin,*DsetaT,*DsetaTT;
static Form cosFi,sinFi,Fi,Fit,Skk_1[3][3],S[3][3],Fi2,cosFi2,sinFi2;
extern FILE *OPRFILE; extern Magazine *MTabAdd;
extern double Tn,Tk,Tend,*Y,*YK,*RK_z,*RK_w,*RK_y1,*RK_y2,*RK_y3;
extern bool PRINTVARS; extern int N; extern Root *InsTime;
double EpsU=1e-3,deltat; Form FC1,FC2,FV1,FV2,FvW1,FvW2;
static Form A[2][2],B[2],A_B[2],Ucof[2],Fx,Fy,NumX,NumY,Denum,NumX2,NumY2,DW1,DvW2;
int Kff=0,*KO; Vary *vC1,*vC2,*vV1,*vV2,*vW1,*vW2;
double **Av,*Bv,dNumX,dNumY,dDenum,dFx,dFy,dBv0,dBv1;
extern wchar_t Sopr[],*sopr;
extern TStringList *SLO; extern String SF;
//-------------------------------------------------------------------------
// Оператор создает список инерционных элементов из списка твердых тел,
//заданного записями
//
// Name_i[~Name_i-1]|[Rx|y|z(a),][Sx|y|z(d),]...=
//                                [m(m_i),][Jx(Jix),][Jy(Jiy),][Jz(Jiz)]
//                                [,Jxy(Jixy)][,Jyz(Jiyz)][,Jxz(Jixz)];,
//
//где Name_i, Name_i-1 - имена текущего и L"базового" тел,
//    Rx|y|z(a), Sx|y|z(d),... - последовательность поворотов на углы L"а"
//    вокруг и смещений L"d" вдоль осей координат для преобразования
//    связанной с телом i-1 СК для совмещения ее с СК, связанной с телом i;
//    m(m_i),Jx(Jix),Jy(Jiy),Jz(Jiz),Jxy(Jixy),Jyz(Jiyz),Jxz(Jixz) - обозначения
//    и значения массы i-го тела, главных центральных осевых и центробежных
//    моментов инерции его.
// Если в исходных данных будет записана инструкция L"СВЯЗАННАЯ СК", то
//координаты центра масс будут записаны в связанной с телом i СК, в качестве
//координат инерционных элементов будут проекции скорости центра масс на оси
//связанной системы координат, иначе, т.е. такой инструкции нет, то
//координаты центра масс будут записаны в абсолютной.
//---------------------------------------------------------------------------
void TakeCoSinFi(Parm *P){
 if(P->Nam){
  Fi.V=TakeVary(P->Nam); cosFi=cos(Fi); sinFi=sin(Fi);
 }
 else if(P->F.C!=Pust){
  cosFi=cos(P->F); sinFi=sin(P->F);
 }
 else{ Form F;
  if(P->Val==90.0){ cosFi.C=Nul; sinFi.C=Odin; }
  else if(P->Val==180.0){ cosFi.C=Odin; cosFi=-cosFi; sinFi.C=Nul; }
  else if(P->Val==-90.0){ cosFi.C=Nul; sinFi.C=Odin; sinFi=-sinFi; }
  else{
   F.C=TakeCnst(cos(P->Val*M_PI/180)); cosFi=F;
   F.C=TakeCnst(sin(P->Val*M_PI/180)); sinFi=F;
}}}
//---------------------------------------------------------------------------
void TakeCoSinFi2(Parm *P){
 if(P->Nam){
  Form Half; wchar_t *Name=P->Nam; if(*Name=='-') Name++; Half.C=Halb;
  Fi2.V=TakeVary(Name); Fi2.B=TakeBnop(3,Half,Fi2);
  cosFi2=cos(Fi2); sinFi2=*P->Nam=='-' ? -sin(Fi2) : sin(Fi2);
 }
 else if(P->F.C!=Pust){
  Form Half; Half.C=Halb; Fi2=Half*P->F;
  cosFi2=cos(Fi2); sinFi=sin(Fi2);
 }
 else{ Form F;
  if((P->Val==180.0)||(P->Val==-180.0)){ cosFi2.C=Nul; sinFi2.C=Odin; }
  else{
   F.C=TakeCnst(cos(P->Val*M_PI/360)); cosFi2=F;
   F.C=TakeCnst(sin(P->Val*M_PI/360)); sinFi2=F;
}}}
//---------------------------------------------------------------------------
Form MakeZnach(Parm *P){ Form F; wchar_t *Name;
 if(P->Nam){
  Name=P->Nam; if(*Name=='-') Name++;
  F.V=TakeVary(Name); if(P->Nam[0]=='-') F=-F;
 }
 else if(P->F.C!=Pust) F=P->F;
 else F.C=TakeCnst(P->Val);
 return F;
}
//---------------------------------------------------------------------------
Form TakeZnach(Parm *P){ Form F; wchar_t *Name; Vary *V;
 if(P->R){
  if(P->R->Nam){
//   Name=P->R->Nam; if(*Name=='-') Name++;   12.08.2019
   if(!FindVary(P->R->Nam,&V)){            //12.08.2019
    swprintf(Inf,L"Переменной \"%s\" не задано значение в исходных данных",
     P->R->Nam); //12.08.2019
    Application->MessageBox(Inf,L"ОШИБКА",MB_OK); F.C=Pust;
   }
   else
    F.V=TakeVary(P->R->Nam);
//   if(P->R->Nam[0]=='-') F=-F; 12.08.2019
  }
  else if(P->R->F.C!=Pust) F=P->R->F;
  else F.C=TakeCnst(P->R->Val);
 }
 else{
  if(P->Nam){
   Name=P->Nam; if(*Name=='-') Name++;
   if(!FindVary(Name,&V)){
    swprintf(Inf,L"Переменной \"%s\" не задано значение в исходных данных",Name);
    Application->MessageBox(Inf,L"ОШИБКА",MB_OK); F.C=Pust;
   }
   else
    F.V=TakeVary(Name);
   if(P->Nam[0]=='-') F=-F;
  }
  else if(P->F.C!=Pust) F=P->F;
  else F.C=TakeCnst(P->Val);
 }
 return F;
}
//---------------------------------------------------------------------------
Form MinusMakeZnach(Parm *P){ Form F; wchar_t *Name;
 if(P->Nam){
  Name=P->Nam; if(*Name=='-') Name++;
  F.V=TakeVary(Name);
  if(P->Nam[0]!='-') F=-F;
 }
 else if(P->F.C!=Pust) F=-P->F;
 else F.C=TakeCnst(-P->Val);
 return F;
}
//---------------------------------------------------------------------------
void MultMatrVect(Form M[3][3],Form V[3]){
 Form Rez[3],Mult,A,B; int i,j;
 for(i=0;i<3;i++){
  Rez[i].C=Nul;
  for(j=0;j<3;j++){
   A=V[j]; B=M[i][j];
   if(IsVary(B)&&IsCnst(B.V->Znach)){
    if(B.V->Znach.C==Nul) Mult.C=Nul;
    else if(B.V->Znach.C==Odin) Mult=A;
   }
   else Mult=A*B;
   Rez[i]=Rez[i]+Mult;
 }}
 for(i=0;i<3;i++) V[i]=Rez[i];
}
//---------------------------------------------------------------------------
void MultTransMatrVect(Form M[3][3],Form V[3]){
 Form Mt[3][3]; int i,j;
 for(i=0;i<3;i++) for(j=0;j<3;j++) Mt[i][j]=M[j][i];
 MultMatrVect(Mt,V);
}
//---------------------------------------------------------------------------
void VtAV(double **V,double **A,double **Rez){
 int i,j,k; double VtA[3][3];
 for(i=0;i<3;i++) for(j=0;j<3;j++) VtA[i][j]=0.0;
 for(i=0;i<3;i++)
  for(j=0;j<3;j++)
   for(k=0;k<3;k++) VtA[i][j]+=V[k][i]*A[k][j];
 for(i=0;i<3;i++) for(j=0;j<3;j++) Rez[i][j]=0.0;
 for(i=0;i<3;i++)
  for(j=0;j<3;j++)
   for(k=0;k<3;k++) Rez[i][j]+=VtA[i][k]*V[k][j];
}
//---------------------------------------------------------------------------
void AddVect(Form A[3],Form B[3],Form C[3]){
 for(int i=0;i<3;i++)
  if(IsVary(A[i])&&(A[i].V->Znach.C==Nul)) C[i]=B[i];
  else C[i]=A[i]+B[i];
}
//---------------------------------------------------------------------------
void MultMatrMatr(Form A[3][3],Form B[3][3],Form C[3][3]){
 int i,j,k; Form Mult,a,b;
 for(i=0;i<3;i++) for(j=0;j<3;j++){
  C[i][j].C=Nul;
  for(k=0;k<3;k++){
   a=A[i][k]; b=B[k][j];
   if(IsVary(a)&&IsCnst(a.V->Znach)){
    if(a.V->Znach.C==Nul) Mult.C=Nul;
    else if(a.V->Znach.C==Odin) Mult=b;
   }
   else if(IsVary(b)&&IsCnst(b.V->Znach)){
    if(b.V->Znach.C==Nul) Mult.C=Nul;
    else if(b.V->Znach.C==Odin) Mult=a;
   }
   else Mult=b*a;
   C[i][j]=C[i][j]+Mult;
}}}
//---------------------------------------------------------------------------
void MultMatrMatr(Form A[3][3],Form B[3][3]){ Form C[3][3];
 MultMatrMatr(A,B,C);
 for(int i=0;i<3;i++) for(int j=0;j<3;j++) A[i][j]=C[i][j];
}
//---------------------------------------------------------------------------
void PrintFormVect(wchar_t *File,wchar_t *Comm,wchar_t *NameVect,Form V[3]){
 FILE *F=_wfopen(File,L"a"); wchar_t Name[256];
 swprintf(Name,L"\n%s\n",Comm); fputws(StrWINtoDOS(Name),F);
 for(int i=0;i<3;i++){
  swprintf(Name,L"%s[%d]",NameVect,i);
  PrintForm(true,F,Name,V[i]); fputwc('\n',F);
 }
 fclose(F);
}
//---------------------------------------------------------------------------
void PrintVect3(wchar_t *File,wchar_t *Comm,wchar_t *NameVect,Form V[3]){
 FILE *F=_wfopen(File,L"a"); wchar_t Name[256];
 swprintf(Name,L"\n%s",Comm); fputws(StrWINtoDOS(Name),F);
 for(int i=0;i<3;i++){
  swprintf(Name,L"%s[%d]",NameVect,i);
  PrintForm(true,F,Name,V[i]);
 }
 fclose(F);
}
//---------------------------------------------------------------------------
void PrintFormMatr(wchar_t *File,wchar_t *Comm,wchar_t *NameMatr,Form M[3][3]){
 FILE *F=_wfopen(File,L"a"); wchar_t Name[256];
 swprintf(Name,L"\n%s\n",Comm); fputws(StrWINtoDOS(Name),F);
 for(int i=0;i<3;i++){
  for(int j=0;j<3;j++){
   swprintf(Name,L"%s[%d][%d]",NameMatr,i,j);
   PrintForm(true,F,Name,M[i][j]); fputwc(' ',F);
  }
  fputwc('\n',F);
 }
 fclose(F);
}
//---------------------------------------------------------------------------
bool Manipulator(Parm *Par,TRichEdit *R){
 Application->MessageBox(L"Обработчик оператора МАНИПУЛЯТОР отсутствует!",
 L"ВНИМАНИЕ",MB_OK); return false;
}
//---------------------------------------------------------------------------
wchar_t *TestBody(Body *b,Magazine **M){
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
void MakeElemConvVectOk(wchar_t *BName,Form rOk[3]){
 for(int i=0;i<3;i++){ Vary *V;
  swprintf(Inf,L"%cO@%s",i==2?'Z':i==1?'Y':'X',BName);
  if((!FindVary(Inf,&V)||(V->Znach.C==Pust))&&(V=TakeVary(Inf)))
   V->Znach=rOk[i];
  rOk[i].V=V;
}}
//---------------------------------------------------------------------------
void MakeElemConvVectCk(wchar_t *BName,Form rCk[3]){
 for(int i=0;i<3;i++){ Vary *V;
  swprintf(Inf,L"%cC@%s",i==2?'Z':i==1?'Y':'X',BName);
  if((!FindVary(Inf,&V)||(V->Znach.C==Pust))&&(V=TakeVary(Inf)))
   V->Znach=rCk[i];
  rCk[i].V=V;
}}
//---------------------------------------------------------------------------
void MakeElemConvVectWk(wchar_t *BName,Form wkk[3]){
//  if(!FindParmUppStr(B->KoordSys,L"Псевдо",&p)){//используем обратные кинематические соотношения
 for(int i=0;i<3;i++){ Vary *V;
  swprintf(Inf,L"omega%c@%s",i==2?'z':i==1?'y':'x',BName);
  if((!FindVary(Inf,&V)||(V->Znach.C==Pust))&&(V=TakeVary(Inf))) V->Znach=wkk[i];
   wkk[i].V=V;
}}
//---------------------------------------------------------------------------
void MakeElemConvMatr(wchar_t *BName,Form BSk0[3][3]){
//Генерация массива переменных с формульными значениями из массива BSk0
//имена переменных строятся с учетом BName
//В результате формульный массив BSk0 содержит массив этих переменных
 Vary *V;
 for(int i=0;i<3;i++){
  for(int j=0;j<3;j++){
   swprintf(Inf,L"S%d%d@%s",i+1,j+1,BName);
   if(V=TakeVary(Inf)) V->Znach=BSk0[i][j];
   BSk0[i][j].V=V;
}}}
//---------------------------------------------------------------------------
bool Accumulate(Body *B){
 Form wkk_1[3],rOkk_1[3],rCkk[3],F,Fp0,Fp1,Fp2,Fp3,Fp0t,Fp1t,
  Fp2t,Fp3t;
 int i,j,Ind; wchar_t *Name,Mes[512]; String AS; Body *b;
 Vary *V,*p0,*p1,*p2,*p3,*p0t,*p1t,*p2t,*p3t;
//Построим добавочные векторы и матрицу преобразования
//от системы k-1 к k
 for(i=0;i<3;i++){//начальные значения
  Skk_1[i][i].C=Odin;
  wkk_1[i].C=rOkk_1[i].C=rCkk[i].C=Nul; //rCkk[i].C=  11.05.18
  for(j=i+1;j<3;j++)
   Skk_1[i][j].C=Skk_1[j][i].C=Nul;
 }
 for(Parm *P=B->KoordSys;P;P=P->Sled){
  switch(P->Nam[0]){ Parm *p,*pp,**Pp; Form S[3][3];
   case 'Q':
	if(KolElem(P->R)!=4){
	 swprintf(Inf,L"Параметры Родрига-Гамильтона должны быть указаны в записи\n"
	  L"твёрдого тела (строка %d) в количестве 4-х!",B->Line);
	 Error(B->Line,0,true);
	 goto Err;
	}
	for(p=P->R,i=0;p;i++,p=p->Sled){
	 if(!p->Nam){
	  swprintf(Inf,L"Не указано имя %d-го параметра Родрига-Гамильтона\n"
	   L"в записи твёрдого тела (строка %d)!",i,B->Line);
	  Error(B->Line,0,true);
	 }
	 if(p->R){
	  swprintf(Inf,L"В записи твёрдого тела (строка %d) на месте имени\n"
	   L"%d-го параметра Родрига-Гамильтона стоит список!",B->Line,i);
	  Error(B->Line,0,true);
	  goto Err;
	}}
	Fp0.V=p0=TakeVary(P->R->Nam); Fp3.V=p3=TakeVary(P->R->Sled->Sled->Sled->Nam);
	Fp1.V=p1=TakeVary(P->R->Sled->Nam); Fp2.V=p2=TakeVary(P->R->Sled->Sled->Nam);
	swprintf(Inf,L"%s'%s",p0->Name,L->Time->Name); Fp0t.V=p0t=TakeVary(Inf);
	swprintf(Inf,L"%s'%s",p1->Name,L->Time->Name); Fp1t.V=p1t=TakeVary(Inf);
	swprintf(Inf,L"%s'%s",p2->Name,L->Time->Name); Fp2t.V=p2t=TakeVary(Inf);
	swprintf(Inf,L"%s'%s",p3->Name,L->Time->Name); Fp3t.V=p3t=TakeVary(Inf);
	if(FindParmUppStr(P,L"Псевдо",&p)){//используем обратные кинематические соотношения
	 Form Fwx,Fwy,Fwz; Root *I;
	 swprintf(Inf,L"omegax@%s",B->Name); Fwx.V=TakeVary(Inf);
	 swprintf(Inf,L"omegay@%s",B->Name); Fwy.V=TakeVary(Inf);
	 swprintf(Inf,L"omegaz@%s",B->Name); Fwz.V=TakeVary(Inf);
	 p0t->Znach=-0.5*(Fp1*Fwx+Fp2*Fwy+Fp3*Fwz);
	 p1t->Znach= 0.5*(Fp0*Fwx-Fp3*Fwy+Fp2*Fwz);
	 p2t->Znach= 0.5*(Fp3*Fwx+Fp0*Fwy-Fp1*Fwz);
	 p3t->Znach=-0.5*(Fp2*Fwx-Fp1*Fwy-Fp0*Fwz);
	 if(I=FindInstLast(L"ПСЕВДОСКОРОСТИ")){//дополняем найденную инструкцию
	  for(p=I->First;p->Sled;p=p->Sled);
	  p->Sled=(Parm *)calloc(1,sizeof(Parm)); p=p->Sled;
	 }
	 else{//создаем инструкцию L"Псевдоскорости"
	  I=TakeInst(L"ПСЕВДОСКОРОСТИ"); I->Line=B->Line;
	  I->First=p=(Parm *)calloc(1,sizeof(Parm));
	 }
	 p->Nam=wcscpy((wchar_t *)calloc(wcslen(Fwx.V->Name)+1,SzC),Fwx.V->Name);
	 p->Sled=(Parm *)calloc(1,sizeof(Parm)); p=p->Sled;
	 p->Nam=wcscpy((wchar_t *)calloc(wcslen(Fwy.V->Name)+1,SzC),Fwy.V->Name);
	 p->Sled=(Parm *)calloc(1,sizeof(Parm)); p=p->Sled;
	 p->Nam=wcscpy((wchar_t *)calloc(wcslen(Fwz.V->Name)+1,SzC),Fwz.V->Name);
	}
	else{//используем прямые кинематические соотношения
	 wkk_1[0]=wkk_1[0]+2*(Fp0*Fp1t-Fp1*Fp0t-Fp2*Fp3t+Fp3*Fp2t);
	 wkk_1[1]=wkk_1[1]+2*(Fp0*Fp2t-Fp2*Fp0t-Fp3*Fp1t+Fp1*Fp3t);
	 wkk_1[2]=wkk_1[2]+2*(Fp0*Fp3t-Fp3*Fp0t-Fp1*Fp2t+Fp2*Fp1t);
	}
	S[0][0]=2*Fp0*Fp0+2*Fp1*Fp1-1; S[0][1]=2*Fp1*Fp2-2*Fp0*Fp3;
	S[0][2]=2*Fp1*Fp3+2*Fp0*Fp2;   S[1][0]=2*Fp1*Fp2+2*Fp0*Fp3;
	S[1][1]=2*Fp0*Fp0+2*Fp2*Fp2-1; S[1][2]=2*Fp3*Fp2-2*Fp0*Fp1;
	S[2][0]=2*Fp1*Fp3-2*Fp0*Fp2;   S[2][1]=2*Fp3*Fp2+2*Fp0*Fp1;
	S[2][2]=2*Fp0*Fp0+2*Fp3*Fp3-1;
	if(OTLADKA){
	 swprintf(Inf,L"Кватернион: %s %s %s %s",
	  P->R->Nam,P->R->Sled->Nam,P->R->Sled->Sled->Nam,P->R->Sled->Sled->Sled->Nam);
	 PrintFormMatr(L"Otladka",Inf,L"S",S);
	}
	MultMatrMatr(Skk_1,S,B->Sk0);
	for(int i=0;i<3;i++) for(j=0;j<3;j++) Skk_1[i][j]=B->Sk0[i][j];
	if(OTLADKA){
	 swprintf(Inf,L"Кватернион: %s %s %s %s",
	  P->R->Nam,P->R->Sled->Nam,P->R->Sled->Sled->Nam,P->R->Sled->Sled->Sled->Nam);
	 PrintFormMatr(L"Otladka",Inf,L"Skk_1",Skk_1);
	}
	break;
   case 'R':
	TakeCoSinFi(P->R);
	switch(P->Nam[1]){
	 case 'x':
//накопим матрицу для радиуса-вектора
	  for(i=0;i<3;i++){
	   F=Skk_1[i][1]*cosFi+Skk_1[i][2]*sinFi;
	   Skk_1[i][2]=Skk_1[i][2]*cosFi-Skk_1[i][1]*sinFi;
	   Skk_1[i][1]=F;
	  }
//повернем накопленную угловую скорость в текущую СК
	  F=wkk_1[1]*cosFi+wkk_1[2]*sinFi;
	  wkk_1[2]=wkk_1[2]*cosFi-wkk_1[1]*sinFi;
	  wkk_1[1]=F;
	  if(P->R->Nam){
	   swprintf(Inf,L"%s'%s",P->R->Nam,L->Time->Name);
	   Fit=DifByT(TakeVary(P->R->Nam));
	  }
	  else if(P->R->F.C!=Pust)
	   Fit=DifByT(P->R->F);
	  else Fit.C=Nul;
	  wkk_1[0]=wkk_1[0]+Fit;
	  break;
	 case 'y':
//накопим матрицу для радиуса-вектора
	  for(i=0;i<3;i++){
	   F=Skk_1[i][0]*cosFi-Skk_1[i][2]*sinFi;
	   Skk_1[i][2]=Skk_1[i][0]*sinFi+Skk_1[i][2]*cosFi;
	   Skk_1[i][0]=F;
	  }
//повернем накопленную угловую скорость
	  F=wkk_1[0]*cosFi-wkk_1[2]*sinFi;
	  wkk_1[2]=wkk_1[0]*sinFi+wkk_1[2]*cosFi;
	  wkk_1[0]=F;
	  if(P->R->Nam){
	   swprintf(Inf,L"%s'%s",P->R->Nam,L->Time->Name);
	   Fit=DifByT(TakeVary(P->R->Nam));
	  }
	  else if(P->R->F.C!=Pust)
	   Fit=DifByT(P->R->F);
	  else Fit.C=Nul;
	  wkk_1[1]=wkk_1[1]+Fit;
	  break;
	 case 'z':
//накопим матрицу для радиуса-вектора
	  for(i=0;i<3;i++){
	   F=Skk_1[i][0]*cosFi+Skk_1[i][1]*sinFi;
	   Skk_1[i][1]=Skk_1[i][1]*cosFi-Skk_1[i][0]*sinFi;
	   Skk_1[i][0]=F;
	  }
//повернем накопленную угловую скорость
	  F=wkk_1[0]*cosFi+wkk_1[1]*sinFi;
	  wkk_1[1]=wkk_1[1]*cosFi-wkk_1[0]*sinFi;
	  wkk_1[0]=F;
	  if(P->R->Nam){
	   swprintf(Inf,L"%s'%s",P->R->Nam,L->Time->Name);
	   Fit=DifByT(TakeVary(P->R->Nam));
	  }
	  else if(P->R->F.C!=Pust)
	   Fit=DifByT(P->R->F);
	  else Fit.C=Nul;
	  wkk_1[2]=wkk_1[2]+Fit;
	  break;
	 default:
	  swprintf(Inf,
	   L"Каждый элементарный поворот связанной системы\n"
	   L"должен быть представлен одной из записей вида:\n"
	   L"Rx(угол),Ry(угол),Rz(угол).\n"
	   L"В скобках стоят имена переменных,\n"
	   L"формулы или числа (углы в градусах).");
	  Error(B->Line,0,true);
	  return false;
	}
	if(OTLADKA){
	 wchar_t C=Ind==2?'z':Ind==1?'y':'x';
	 if(P->R->Nam)
	  swprintf(Inf,L"R%c: поворот вокруг оси %c на угол %s",C,C,P->R->Nam);
	 else if(P->R->F.C!=Pust)
	  swprintf(Inf,L"R%c: поворот вокруг оси %c на угол %s",C,C,
	   (FormToStr(P->R->F,&AS),AS.c_str()));
	 else
	  swprintf(Inf,L"R%c: поворот вокруг оси %c на угол %g°",C,C,P->R->Val);
	 PrintFormMatr(L"Otladka",Inf,L"Skk_1",Skk_1);
	 if(P->R->Nam)
	  swprintf(Inf,L"получим wkk_1 после поворота на угол %s вокруг оси %c",
	   P->R->Nam,C);
	 else if(P->R->F.C!=Pust)
	  swprintf(Inf,L"получим wkk_1 после поворота на угол %s вокруг оси %c",
	   (FormToStr(P->R->F,&AS),AS.c_str()),C);
	 else
	  swprintf(Inf,L"получим wkk_1 после поворота на угол %0.1f вокруг оси %c",
	   P->R->Val,C);
	 PrintFormVect(L"Otladka",Inf,L"wkk_1",wkk_1);
	}
	break;
   case 'S':
	if(P->R->Nam)
	 F.V=TakeVary(P->R->Nam);
	else if(P->R->F.C!=Pust)
	 F=P->R->F;
	else
	 F.C=TakeCnst(P->R->Val);
	switch(P->Nam[1]){
	 case 'x': Ind=0; break;
	 case 'y': Ind=1; break;
	 case 'z': Ind=2; break;
	 default:
	  swprintf(Inf,L"Каждый сдвиг системы данного тела относительно \"базового\"\n"
	   L"должен быть представлен одной из записей вида:\n"
	   L"Sx(сдвиг),Sy(сдвиг),Sz(сдвиг).\n"
	   L"В скобках стоят имена переменных,\n"
	   L"формулы или числа (смещение в метрах).");
	  Error(B->Line,0,true);
	  return false;
	}
	for(i=0;i<3;i++) rOkk_1[i]=rOkk_1[i]+F*Skk_1[i][Ind];
	if(OTLADKA)
	 PrintFormVect(L"Otladka",L"формируем вектор н.к. в базовой СК",L"rOkk_1",rOkk_1);
	break;
   default:
	wcscpy(Inf,P->Nam); LowToUpp(Inf);
	if(wcsstr(Inf,L"ПСЕВДО")==Inf) break;
	swprintf(Inf,L"В записи твёрдого тела (строка %d) в координатной части\n"
	 L"должен стоять составной список из имен: Rx, Ry, Rz, Sx, Sy, Sz, Q, Псевдо!",
	 B->Line,i);
	Error(B->Line,0,true);
Err:swprintf(Inf,L"Описание твердого тела - это запись вида:\n"
	 L"<Имя>[~<База>]|<Сдвиги и повороты>|<Параметры инерции>;\n"
	 L"где\n"
	 L" <Имя> и <База> - имена текущего и \"базового\" тела;\n"
	 L"<Сдвиги и повороты> - описание преобразования СК тела\n"
	 L" от СК базового тела или абсолютной СК вида\n"
	 L" Rx(угол поворота вокруг х),Ry(угол поворота вокруг y),\n"
	 L" Rz(угол поворота вокруг z),Sx(сдвиг по х),\n"
	 L" Sy(сдвиг по y),Sz(сдвиг по z) - в нужном порядке;\n"
	 L" вместо последоватеьности поворотов\n"
	 L" (Rx(...),Ry(...),Rz(...)) могут быть указаны параметры\n"
	 L" Родрига-Гамильтона (компоненты кватерниона) в виде:\n"
	 L" Q(p0,p1,p2,p3), где p0,p1,p2,p3 - переменные,\n"
	 L" обозначающие эти параметры;"
	 L"<Параметры инерции>- список инерционных параметров вида\n"
	 L" Sx(<значение>), Sy(<значение>), Sz(<значение>).\n"
	 L" m(<значение>),Jx(<значение>),Jy(<значение>),Jz(<значение>),\n"
	 L" [Jxy(<значение>),Jyz(<значение>),Jxz(<значение>)];,\n"
	 L" где Sx(),Sy(),Sz() - координаты цм тела в связанной СК\n"
	 L" m(),Jx(),Jy(),Jz(),Jxy(),Jyz(),Jxz() - масса и\n"
	 L" моменты, инерции в связанной СК, если оси СК главные,\n"
	 L" ценnробежные моменты задавать не нужно.");
	Error(B->Line,0,true);
   return false;
 }}
 for(Parm *P=B->Inert;P;P=P->Sled){//пока обрабатываем только сдвиг св. с цм СК
  if(P->Nam[0]=='S'){ Form F; int Ind;
   if(P->R->Nam)
	F.V=TakeVary(P->R->Nam);
   else if(P->R->F.C!=Pust)
	F=P->R->F;
   else
	F.C=TakeCnst(P->R->Val);
   switch(P->Nam[1]){
	case 'x': Ind=0; break;
	case 'y': Ind=1; break;
	case 'z': Ind=2; break;
   }
   rCkk[Ind]=rCkk[Ind]+F;
 }}
//сформируем радиус-вектор н.к., угловую скорость тела и матрицу поворота
 if(!B->BaseName){
  for(i=0;i<3;i++){
   B->rOk[i]=rOkk_1[i];
   for(j=0;j<3;j++) B->Sk0[i][j]=Skk_1[i][j];
  }
  if(OTLADKA){
   PrintFormMatr(L"Otladka",L"",L"B->Sk0",B->Sk0);
   PrintFormVect(L"Otladka",L"B->rOk=rOkk_1",L"B->rOk",B->rOk);
   PrintFormVect(L"Otladka",L"rCkk",L"rCkk",rCkk);
  }
  MakeElemConvVectOk(B->Name,B->rOk);
  MultMatrVect(B->Sk0,rCkk);//сделать умножение с учетом содержимого
  MakeElemConvMatr(B->Name,B->Sk0);
  AddVect(B->rOk,rCkk,B->rCk);
  if(OTLADKA){
   PrintFormVect(L"Otladka",L"rCkk",L"rCkk",rCkk);
   PrintFormVect(L"Otladka",L"B->rCk=B->rOk+rCkk",L"B->rCk",B->rCk);
  }
  MakeElemConvVectCk(B->Name,B->rCk);
 }
 else{
  FindBody(B->BaseName,&b);
  if(!b->Accum&&!Accumulate(b)) return false;
  if(OTLADKA){
   PrintFormVect(L"Otladka",L"вектор rOkk_1",L"rOkk_1",rOkk_1);
   PrintFormMatr(L"Otladka",L"",L"b->Sk0",b->Sk0);
  }
  MultMatrVect(b->Sk0,rOkk_1);
  if(OTLADKA)
   PrintFormVect(L"Otladka",L"b->Sk0*rOkk_1",L"rOkk_1",rOkk_1);
  AddVect(b->rOk,rOkk_1,B->rOk);
  if(OTLADKA)
   PrintFormVect(L"Otladka",L"B->rOk=b->rOk+rOkk_1",L"B->rOk",B->rOk);
  MakeElemConvVectOk(B->Name,B->rOk);
  MultMatrMatr(b->Sk0,Skk_1,B->Sk0);
  if(OTLADKA)
   PrintFormMatr(L"Otladka",L"B->Sk0=b->Sk0*Skk_1",L"B->Sk0",B->Sk0);
  MakeElemConvMatr(B->Name,B->Sk0);
  MultMatrVect(B->Sk0,rCkk);
  if(OTLADKA)
   PrintFormVect(L"Otladka",L"rCkk=B->Sk0*rCkk",L"rCkk",rCkk);
  AddVect(B->rOk,rCkk,B->rCk);
  if(OTLADKA)
   PrintFormVect(L"Otladka",L"B->rCk=B->rOk+rCkk",L"B->rCk",B->rCk);
  MakeElemConvVectCk(B->Name,B->rCk);
  for(i=0;i<3;i++)
   if(IsVary(b->wkk[i])&&(b->wkk[i].V->Znach.C==Nul)) B->wkk[i].C=Nul;
   else B->wkk[i]=b->wkk[i];
 }
 MultTransMatrVect(Skk_1,B->wkk);
 AddVect(B->wkk,wkk_1,B->wkk);
 MakeElemConvVectWk(B->Name,B->wkk);
 if(OTLADKA){
  PrintFormVect(L"Otladka",B->Name,L"B->rCk",B->rCk);
  PrintFormVect(L"Otladka",B->Name,L"B->rOk",B->rOk);
  PrintFormVect(L"Otladka",L"",L"B->wkk",B->wkk);
  PrintFormMatr(L"Otladka",L"",L"B->Sk0",B->Sk0);
 }
 B->Accum=true; return true;
}
//---------------------------------------------------------------------------
void RotationVector(wchar_t Os,wchar_t *Head,Form V[],Parm *P){
//преобразоание координат вектора к новой СК
 Form F1,F2; String AS;
 switch(Os){
  case 'x':
   F1=V[1]*cosFi+V[2]*sinFi;  F2=V[2]*cosFi-V[1]*sinFi; V[1]=F1; V[2]=F2;
   break;
  case 'y':
   F1=V[0]*cosFi-V[2]*sinFi; F2=V[0]*sinFi+V[2]*cosFi;  V[0]=F1; V[2]=F2;
   break;
  case 'z':
   F1=V[0]*cosFi+V[1]*sinFi;  F2=V[0]*sinFi+V[1]*cosFi;  V[0]=F1; V[1]=F2;
   break;
 }
 if(OTLADKA){
  if(P->R->Nam)
   swprintf(Inf,L"повернем %s на угол %s вокруг оси %c",Head,P->R->Nam,Os);
  else if(P->R->F.C!=Pust)
   swprintf(Inf,L"повернем %s на угол %s вокруг оси %c",Head,
	(FormToStr(P->R->F,&AS),AS.c_str()),Os);
  else
   swprintf(Inf,L"повернем %s на угол %0.1f вокруг оси %c",Head,P->R->Val,Os);
  PrintFormVect(L"OTLADKA",Inf,Head,V);
}}
//---------------------------------------------------------------------------
bool NoGeneral(Parm *P){ int k=0;
 for(;P;P=P->Sled,k++);
 if(k>3) return true;
 return false;
}
//---------------------------------------------------------------------------
int Jacobi(int N,double **A,double **V,double *D,int Increase){
//Increase=1 - с.знач. по возрастанию, =-1 - по убыванию? =0 - как есть
 int i,j,k,p,q,rot=0; double Sm,g,c,s,t,h,tau,theta,tresh,*B,*Z,r; bool Net;
 B=(double *)calloc(N,SzD); Z=(double *)calloc(N,SzD);
 for(int p=0;p<N;p++){
  for(int q=0;q<N;q++)
   V[p][q]=(p==q?1.0:0.0);
  B[p]=D[p]=A[p][p];
  Z[p]=0.0;
 }
 for(i=0;i<50;i++){
SWP: Sm=0.0; for(int p=0;p<N-1;p++) for(int q=p+1;q<N;q++) Sm+=fabs(A[p][q]);
  if(Sm<=0.0) goto M;
  tresh=i<3?0.2*Sm/N/N:0.0;
  for(int p=0;p<N-1;p++) for(int q=p+1;q<N;q++){
   g=100.0*fabs(A[p][q]);
   if(i>3&&((fabs(D[p])+g)==fabs(D[p]))&&
      ((fabs(D[q])+g)==fabs(D[q]))) A[p][q]=0.0;
   else if(fabs(A[p][q])>tresh){
ROTATE:h=D[q]-D[p];
    if((fabs(h)+g)==fabs(h)) t=A[p][q]/h;
    else{
     theta=0.5*h/A[p][q];
     t=1.0/(fabs(theta)+sqrt(1.0+theta*theta));
     if(theta<0.0) t=-t;
    }
    c=1.0/sqrt(1.0+t*t); s=t*c;
    tau=s/(1.0+c); h=t*A[p][q];
    Z[p]-=h; Z[q]+=h; D[p]-=h; D[q]+=h;
    A[p][q]=0.0;
    for(int j=0;j<p;j++){
     g=A[j][p]; h=A[j][q];
     A[j][p]=g-s*(h+g*tau); A[j][q]=h+s*(g-h*tau);
    }
    for(int j=p+1;j<q;j++){
     g=A[p][j]; h=A[j][q];
     A[p][j]=g-s*(h+g*tau); A[j][q]=h+s*(g-h*tau);
    }
    for(int j=q+1;j<N;j++){
     g=A[p][j]; h=A[q][j];
     A[p][j]=g-s*(h+g*tau); A[q][j]=h+s*(g-h*tau);
    }
    for(int j=0;j<N;j++){
     g=V[j][p]; h=V[j][q];
     V[j][p]=g-s*(h+g*tau); V[j][q]=h+s*(g-h*tau);
    }
    rot++;
  }}
  for(int p=0;p<N;p++){
   D[p]=B[p]=B[p]+Z[p]; Z[p]=0.0;
 }}
M:
 Net=true; k=N-1;
 while(Net){
  Net=false;
  for(int i=0;i<k;i++){
   r=D[i];
   if((Increase>0)&&(r>D[i+1])||(Increase<0)&&(r<D[i+1])){
    Net=true; D[i]=D[i+1]; D[i+1]=r;
    for(int j=0;j<N;j++){ r=V[j][i]; V[j][i]=V[j][i+1]; V[j][i+1]=r; }
  }}
  k--;
 }
 free(B); free(Z);
 return rot;
}
//---------------------------------------------------------------------------
void TakeZnach(Parm *P,double **Jc,int i){
 double m; Cnst *C; Vary *V;
 if(P->R){
  if(P->R->Nam){//если в скобках имя
   if(FindCnst(P->R->Nam,&C)) m=C->Val; //имя константы
   else if(FindVary(P->R->Nam,&V)) m=Value(V->Znach); //имя переменной
  }
  else if(P->R->F.C!=Pust)//если в скобках формула
   m=Value(P->R->F);
  else//если в скобках число
   m=P->R->Val;
  if(!wcscmp(P->Nam,L"Jx")) Jc[0][0]=m;
  else if(!wcscmp(P->Nam,L"Jy")) Jc[1][1]=m;
  else if(!wcscmp(P->Nam,L"Jz")) Jc[2][2]=m;
  else if(!wcscmp(P->Nam,L"Jxy")) Jc[0][1]=Jc[1][0]=m;
  else if(!wcscmp(P->Nam,L"Jyz")) Jc[1][2]=Jc[2][1]=m;
  else if(!wcscmp(P->Nam,L"Jxz")) Jc[0][2]=Jc[2][0]=m;
 }
 else{
  if(FindCnst(P->Nam,&C)) m=C->Val; //имя константы
  else if(FindVary(P->Nam,&V)) m=Value(V->Znach); //имя переменной
  switch(i){
   case 0: Jc[0][0]=m; break;
   case 1: Jc[1][1]=m; break;
   case 2: Jc[2][2]=m; break;
   case 3: Jc[0][1]=Jc[1][0]=-m; break;
   case 4: Jc[1][2]=Jc[2][1]=-m; break;
   case 5: Jc[0][2]=Jc[2][0]=-m; break;
}}}
//---------------------------------------------------------------------------
void Rotation(Body *b){
 double **Jc=(double **)calloc(3,SzV),**V=(double **)calloc(3,SzV),
  *J=(double *)calloc(3,SzD);
 int i=0,j; Parm *P; Form Fwx,Fwy,Fwz; Vary *Vwx,*Vwy,*Vwz;
 wchar_t NameWx[256],NameWy[256],NameWz[256]; Vary *v;
 for(;i<3;i++){
  Jc[i]=(double *)calloc(3,SzD);
  V[i]=(double *)calloc(3,SzD);
 }
 for(P=b->Inert->Sled,i=0;P;P=P->Sled,i++){
  if(P->Nam) TakeZnach(P,Jc,i); //если имя перед скобкой
  else switch(i){
   case 0: Jc[0][0]=P->Val; break;
   case 1: Jc[1][1]=P->Val; break;
   case 2: Jc[2][2]=P->Val; break;
   case 3: Jc[0][1]=Jc[1][0]=-P->Val; break;
   case 4: Jc[1][2]=Jc[2][1]=-P->Val; break;
   case 5: Jc[0][2]=Jc[2][0]=-P->Val; break;
 }}
 Jacobi(3,Jc,V,J,false);
 swprintf(NameWx,L"omegax@%s",b->Name); Vwx=TakeVary(NameWx);
 swprintf(NameWy,L"omegay@%s",b->Name); Vwy=TakeVary(NameWy);
 swprintf(NameWz,L"omegaz@%s",b->Name); Vwz=TakeVary(NameWz);
 Fwx=V[0][0]*Vwx->Znach+V[1][0]*Vwy->Znach+V[2][0]*Vwz->Znach;
 Fwy=V[0][1]*Vwx->Znach+V[1][1]*Vwy->Znach+V[2][1]*Vwz->Znach;
 Fwz=V[0][2]*Vwx->Znach+V[1][2]*Vwy->Znach+V[2][2]*Vwz->Znach;
 Vwx->Znach=Fwx; Vwy->Znach=Fwy; Vwz->Znach=Fwz;
 for(P=b->Inert->Sled,i=0;i<3;P=P->Sled,i++){
  if(P->Nam){
   if(P->R){
    P->R->Val=J[i]; P->R->Nam=NULL;
   }
   else
    if(FindVary(P->Nam,&v)){ v->Znach.C=TakeCnst(J[i]); v->Val=J[i]; }
  }
  else P->Val=J[i];
 }
 for(P=b->Inert->Sled,i=0;i<2;P=P->Sled,i++);
 P->Sled=NULL;
 SLO->Add(L"<p>Из-за ненулевых значений центробежных моментов инерции L"
  L"используется главная центральная<br>");
 SLO->Add(L"система координат с матрицей поворота Якоби:</p>");
 SLO->Add(L"<table wideltath=\"100\%\" border=\"0\">");
 for(i=0;i<3;i++){
  SLO->Add(L"<tr>");
  for(j=0;j<3;j++)
   sopr+=swprintf(sopr,L"<td wideltath=\"33%%\">%g</td>",V[i][j]);
  sopr+=swprintf(sopr,L"</tr>");
  SLO->Add(Sopr); *Sopr='\0'; sopr=Sopr;
 }
 SLO->Add(L"</table>");
 SLO->Add(L"<p>При этом главные центральные моменты инерции"
  L" приняли значения:<br></p>");
 SLO->Add(L"<table wideltath=\"100%\" border=\"0\">");
 SLO->Add(L"<tr>");
 sopr+=swprintf(sopr,L"<td wideltath=\"33%%\"><b>Jx</b>=%g</td>",J[0]);
 sopr+=swprintf(sopr,L"<td wideltath=\"33%%\"><b>Jy</b>=%g</td>",J[1]);
 sopr+=swprintf(sopr,L"<td wideltath=\"33%%\"><b>Jz</b>=%g</td>",J[2]);
 sopr+=swprintf(sopr,L"</tr>");
 SLO->Add(Sopr); *Sopr='\0'; sopr=Sopr;
 SLO->Add(L"</table>");
 SLO->Add(L"<p>Проекции угловой скорости тела на главные центральные оси координат"
  L"получаются умножением транспонированной матрицы<br>");
 SLO->Add(L"поворота Якоби на проекции L угловой скорости"
  L" на оси неповернутой центральной системы координат:</p><br>");
 if(Jc&&V&&J){
  for(i=0;i<3;i++){
   if(Jc[i]) free(Jc[i]);
   if(V[i]) free(V[i]);
  }
  free(Jc); free(V); free(J);
}}
//---------------------------------------------------------------------------
bool RUNGE(double x,double *y,int n,void (*f)(double x),
  double eps,double eta,double xfin,double *yfin);
bool RUNGE_Ex(double x,double *y,int n,void (*f)(double x),
  double eps,double eta,double xfin,double *yfin);
bool RUNGE_ORIG(double x,double *y,int n,void (*f)(double x),double eps,double eta,
  double xfin,double *yflin,bool prim,TStatusBar *SB,wchar_t *Head);
int SLAU(int N,double **A,double *p,double *x,bool Repeat);
bool Adams(int &N,double x,double *y,void (*f)(double x),double Eps,
 double xfin,double *yfin);
bool Eiler(double x,double *y,void (*f)(double x),double Eps,double xfin,double *yfin);
//---------------------------------------------------------------------------
void PrintSummand(FILE *F,double t,int k){
 Vary *V,*Vo,*Vt,*Vo_t,*Vt_t,*Vo_t_t,*Vt_t_t;
 double dx1,dx2,dy1,dy2,dfz,dp,dVo,dVt,dVo_t,dVt_t,dVo_t_t,dVt_t_t;
 wchar_t Sx1[16],Sx2[16],Sy1[16],Sy2[16],Fz[16],P[16]; Form Fo,Ft,Fo_t,Ft_t,Fo_t_t,Ft_t_t;
 FindVary(L"omega",&Vo);         FindVary(L"tau",&Vt);         Fo.V=Vo; Ft.V=Vt;
 FindVary(L"omega't",&Vo_t);     FindVary(L"tau't",&Vt_t);     Fo_t.V=Vo_t; Ft_t.V=Vt_t;
 FindVary(L"omega't't",&Vo_t_t); FindVary(L"tau't't",&Vt_t_t); Fo_t_t.V=Vo_t_t; Ft_t_t.V=Vt_t_t;
 ClearVar(); L->Time->Val=t;
 dVo=Val(Fo); dVo_t=Val(Fo_t); dVo_t_t=Val(Fo_t_t);
 dVt=Val(Ft); dVt_t=Val(Ft_t); dVt_t_t=Val(Ft_t_t);
 fwprintf(F,L"t=%6.3f omega=%10.3e omega't=%10.3e omega't't=%10.3e"
  L" tau=%10.3e tau't=%10.3e tau't't=%10.3e\n",t,dVo,dVo_t,dVo_t_t,dVt,dVt_t,dVt_t_t);
 for(Body *b=L->B;b;b=b->Sled){ Form Fs;
//  if((!wcscmp(b->Name,L"BdR"))&&(k==11))
//   PRINTVARS=true;
//Fs.V=V; dy2=Val(Fs); PRINTVARS=false;
//  swprintf(Sx1,L"Sx1@%s",b->Name); V=TakeVary(Sx1); dx1=Val(V->Znach);
//  swprintf(Sx2,L"Sx2@%s",b->Name); V=TakeVary(Sx2); dx2=Val(V->Znach);
  swprintf(Sy1,L"Sy1@%s",b->Name); V=TakeVary(Sy1); dy1=Val(V->Znach);
  swprintf(Sy2,L"Sy2@%s",b->Name); V=TakeVary(Sy2); dy2=Val(V->Znach);
  swprintf(Fz,L"Fz@%s",b->Name); V=TakeVary(Fz); dfz=Val(V->Znach);
  swprintf(P,L"P@%s",b->Name); V=TakeVary(P); dp=Val(V->Znach);
//  fwprintf(F,L"%8s=%16.4e\t%8s=%16.4e\t%8s=%16.4e\t%8s=%16.4e\n",
//   Sx1,dx1,Sx2,dx2,Sy1,dy1,Sy2,dy2);
  fwprintf(F,L"%s=%g;\n%s=%g;\n%s+%s=%g\n",
   Sy1,dy1,Sy2,dy2,Fz,P,dfz+dp);
}}
//---------------------------------------------------------------------------
int FindCord(Cord *R,Vary *V){
 for(Cord *c=R;c;c=c->Sled){
  if((c->V==V)||!wcscmp(c->V->Name,V->Name)) return 1;
 }
 return 0;
}
//---------------------------------------------------------------------------
void BuildCOF(Root *I,TStatusBar *StatusBar,TTabControl *TC){
//процедура создания формул для координат COF
 int Kt=KolElem(L->B); wchar_t *s,NameB[256],Head[256]; FILE *FFF=NULL;
 Cord *OK=NULL,*k,**pok=&OK,*OKt=NULL,*kt,**pokt=&OKt,*OKtt=NULL,*ktt,**poktt=&OKtt;
 bool MOMIN=false,PRINTSL=false,GRADOK=false; Vary *V;
 double t,deltat,D,Dx,Dy,Dnumx=0,Dnumy=0,Ddenum=0,Drezx,Drezy;
 Vary *Vfx,*Vfy,*Vfz,*Vix,*Viy,*Viz,*Vp,*Vsx1,*Vsx2,*Vsx3,*Vsy1,*Vsy2,*Vsy3,*Vx,
  *Vy,*Vzn,*Vnmx,*Vnmy,*Vd,*Vx_,*Vy_,*Xc,*Yc;
 Form Ffx,Ffy,Ffz,Fix,Fiy,Fiz,Fp,Fsx1,Fsx2,Fsx3,Fsy1,Fsy2,Fsy3,F,Fg,Fd,FXc,FYc;
 if(!Kt) return;
 for(Parm *P=I->First;P;P=P->Sled)
  if(P->Nam){
   wcscpy(Inf,P->Nam); LowToUpp(Inf);
   if(!wcscmp(Inf,L"С МОМЕНТАМИ ИНЕРЦИИ")){ MOMIN=true; continue; }
   if(!wcscmp(Inf,L"С ПЕЧАТЬЮ СЛАГАЕМЫХ")){ PRINTSL=true; continue; }
   if(!wcscmp(Inf,L"С ГРАДИЕНТОМ ПО ОБОБЩЕННЫМ КООРДИНАТАМ")){
    GRADOK=true;
    for(Parm *p=P->R;p;p=p->Sled) if(FindVary(p->Nam,&V)){
     k=*pok=(Cord *)calloc(1,sizeof(Cord)); pok=(Cord **)k; k->V=V;
     swprintf(Inf,L"%s'%s",p->Nam,L->Time->Name); V=TakeVary(Inf);
     kt=*pokt=(Cord *)calloc(1,sizeof(Cord)); pokt=(Cord **)kt; kt->V=V;
     swprintf(Inf,L"%s'%s'%s",p->Nam,L->Time->Name,L->Time->Name); V=TakeVary(Inf);
     ktt=*poktt  =(Cord *)calloc(1,sizeof(Cord)); poktt=(Cord **)ktt; ktt->V=V;
 }}}
 if((I=FindInstLast(L"ВРЕМЯ"))||(I=FindInstLast(L"ВАРЬИРОВАТЬ"))){
  if(KolElem(I->First->R)==2){//границы в скобках
   Tn=!I->First->R->Nam ? I->First->R->Val:
      Val(StrToForm(I->First->R->Nam));
   Tk=!I->First->R->Sled->Nam ? I->First->R->Sled->Val:
      Val(StrToForm(I->First->R->Sled->Nam));
  }
  else{//границы подряд в списке
   Tn=!I->First->Sled->Nam ? I->First->Sled->Val :
       Val(StrToForm(I->First->Sled->Nam));
   if(I->First->Sled->Sled)
    Tk=!I->First->Sled->Sled->Nam ? I->First->Sled->Sled->Val :
       Val(StrToForm(I->First->Sled->Sled->Nam));
 }}
 NumX.C=Nul; NumY.C=Nul; NumX2.C=Nul; NumY2.C=Nul; Denum.C=Nul;
if(OTLADKA) FFF=_wfopen(L"PrintSlagsCalc.txt",L"w");
 for(Body *b=L->B;b;b=b->Sled){ Parm *P; int i;
  for(P=b->Inert,i=0;P;P=P->Sled){//определим формулу для m, Jx, Jy, Jz
   if(P->Nam&&(*P->Nam=='S'||*P->Nam=='R')) continue;
   if(P->R){
    if(P->R->Nam) F.V=TakeVary(P->R->Nam);
    else if(P->R->F.C!=Pust) F=P->R->F;
    else F.C=TakeCnst(P->R->Val);
    if(!wcscmp(P->Nam,L"m")) b->m=F;
    else if(!wcscmp(P->Nam,L"Jx")) b->Jx=F;
    else if(!wcscmp(P->Nam,L"Jy")) b->Jy=F;
    else if(!wcscmp(P->Nam,L"Jz")) b->Jz=F;
   }
   else{
    if(P->Nam) F.V=TakeVary(P->Nam);
    else if(P->F.C!=Pust) F=P->F;
    else F.C=TakeCnst(P->Val);
    switch(i++){
     case 0: b->m=F;  break;
     case 1: b->Jx=F; break;
     case 2: b->Jy=F; break;
     case 3: b->Jz=F; break;
  }}}
  wcscpy(NameB,b->Name); //if(s=wcschr(NameB,'~')) *s='\0';
  swprintf(Inf,L"Fx@%s",NameB);  Vfx =TakeVary(Inf); Ffx.V =Vfx;
  if(PRINTSL) TC->Tabs->Add(Inf);
  swprintf(Inf,L"Fy@%s",NameB);  Vfy =TakeVary(Inf); Ffy.V =Vfy;
  if(PRINTSL) TC->Tabs->Add(Inf);
  swprintf(Inf,L"Fz@%s",NameB);  Vfz =TakeVary(Inf); Ffz.V =Vfz;
  if(PRINTSL) TC->Tabs->Add(Inf);
  swprintf(Inf,L"P@%s",NameB);   Vp  =TakeVary(Inf); Fp.V  =Vp;
  if(PRINTSL) TC->Tabs->Add(Inf);
  swprintf(Inf,L"Sx1@%s",NameB); Vsx1=TakeVary(Inf); Fsx1.V=Vsx1;
  if(PRINTSL) TC->Tabs->Add(Inf);
  swprintf(Inf,L"Sx2@%s",NameB); Vsx2=TakeVary(Inf); Fsx2.V=Vsx2;
  if(PRINTSL) TC->Tabs->Add(Inf);
  swprintf(Inf,L"Sy1@%s",NameB); Vsy1=TakeVary(Inf); Fsy1.V=Vsy1;
  if(PRINTSL) TC->Tabs->Add(Inf);
  swprintf(Inf,L"Sy2@%s",NameB); Vsy2=TakeVary(Inf); Fsy2.V=Vsy2;
  if(PRINTSL) TC->Tabs->Add(Inf);
  Vfx->Znach=b->m*b->ACk[0];
  Vfy->Znach=b->m*b->ACk[1];
  Vfz->Znach=b->m*b->ACk[2];
  Fg.V=L->V; Vp->Znach=b->m*Fg;
  Vsx1->Znach=(Ffz+Fp)*b->rCk[0]; Vsx2->Znach=Ffx*b->rCk[2];
  Vsy1->Znach=(Ffz+Fp)*b->rCk[1]; Vsy2->Znach=Ffy*b->rCk[2];
if(FFF){
 L->Time->Val=0.001; L->Time->P=0; ClearVar();
 if(!wcscmp(NameB,L"Bl"))
  PRINTVARS=true;
 D=Val(Fsx1); fwprintf(FFF,L"%s=%g;\n",Vsx1->Name,D); Dnumx+=D;
 D=Val(Fsx2); fwprintf(FFF,L"%s=%g;\n",Vsx2->Name,D); Dnumx-=D;
 D=Val(Fsy1); fwprintf(FFF,L"%s=%g;\n",Vsy1->Name,D); Dnumy+=D;
 D=Val(Fsy2); fwprintf(FFF,L"%s=%g;\n",Vsy2->Name,D); Dnumy-=D;
 PRINTVARS=false;
 D=Val(Ffz+Fp); fwprintf(FFF,L"%s+%s=%g;\n",Vfz->Name,Vp->Name,D); Ddenum+=D;
}
  NumX=NumX+Fsx1; NumX=NumX-Fsx2; NumX2=NumX2-Fsx2;
  NumY=NumY+Fsy1; NumY=NumY-Fsy2; NumY2=NumY2-Fsy2;
  Denum=Denum+Ffz; Denum=Denum+Fp;
  if(MOMIN){
   swprintf(Inf,L"Ix@%s",NameB);  Vix =TakeVary(Inf); Fix.V =Vix;
   if(PRINTSL) TC->Tabs->Add(Inf);
   swprintf(Inf,L"Iy@%s",NameB);  Viy =TakeVary(Inf); Fiy.V =Viy;
   if(PRINTSL) TC->Tabs->Add(Inf);
   swprintf(Inf,L"Iz@%s",NameB);  Viz =TakeVary(Inf); Fiz.V =Viz;
   if(PRINTSL) TC->Tabs->Add(Inf);
   swprintf(Inf,L"Sx3@%s",NameB); Vsx3=TakeVary(Inf); Fsx3.V=Vsx3;
   if(PRINTSL) TC->Tabs->Add(Inf);
   swprintf(Inf,L"Sy3@%s",NameB); Vsy3=TakeVary(Inf); Fsy3.V=Vsy3;
   if(PRINTSL) TC->Tabs->Add(Inf);
   Vix->Znach=b->Jx*b->ekk[0]+(b->Jz-b->Jy)*b->wkk[1]*b->wkk[2];
   Viy->Znach=b->Jy*b->ekk[1]+(b->Jx-b->Jz)*b->wkk[0]*b->wkk[2];
   Viz->Znach=b->Jx*b->ekk[2]+(b->Jy-b->Jx)*b->wkk[0]*b->wkk[1];
   Vsx3->Znach=b->Sk0[1][0]*Vix->Znach+b->Sk0[1][1]*Viy->Znach+b->Sk0[1][2]*Viz->Znach;
   Vsy3->Znach=b->Sk0[0][0]*Vix->Znach+b->Sk0[0][1]*Viy->Znach+b->Sk0[0][2]*Viz->Znach;
   NumX=NumX-Fsx3; NumY=NumY+Fsy3;
 }}
 Vzn =TakeVary(L"Zn");        Vzn->Znach=Denum;
 Vnmx=TakeVary(L"ChX");       Vnmx->Znach=NumX;
 Vnmy=TakeVary(L"ChY");       Vnmy->Znach=NumY;
 Vx  =TakeVary(L"Xcof");      Vy =TakeVary(L"Ycof");
 Vx->Znach=NumX/Denum;       Vy->Znach=NumY/Denum;
 Vx_ =TakeVary(L"Xcof_");     Vy_=TakeVary(L"Ycof_");
 Xc  =TakeVary(L"XC");        Yc =TakeVary(L"YC"); FXc.V=Xc; FYc.V=Yc;
 Vx_->Znach=FXc-NumX2/Denum; Vy_->Znach=FYc-NumY2/Denum;
 TC->Tabs->Add(L"Xcof_");     TC->Tabs->Add(L"Ycof_");
 if(GRADOK&&OK){
  for(k=OK;k;k=k->Sled){
   swprintf(Inf,L"dXcof_po_d%s",k->V->Name); V=TakeVary(Inf);
   V->Znach=DifBy(Vx->Znach,k->V); TC->Tabs->Add(Inf);
   swprintf(Inf,L"dYcof_po_d%s",k->V->Name); V=TakeVary(Inf);
   V->Znach=DifBy(Vy->Znach,k->V); TC->Tabs->Add(Inf);
  }
  for(kt=OKt;kt;kt=kt->Sled){
   swprintf(Inf,L"dXcof_po_d%s",kt->V->Name); V=TakeVary(Inf);
   V->Znach=DifBy(Vx->Znach,kt->V); TC->Tabs->Add(Inf);
   swprintf(Inf,L"dYcof_po_d%s",kt->V->Name); V=TakeVary(Inf);
   V->Znach=DifBy(Vy->Znach,kt->V); TC->Tabs->Add(Inf);
  }
  for(ktt=OKtt;ktt;ktt=ktt->Sled){
   swprintf(Inf,L"dXcof_po_d%s",ktt->V->Name); V=TakeVary(Inf);
   V->Znach=DifBy(Vx->Znach,ktt->V); TC->Tabs->Add(Inf);
   swprintf(Inf,L"dYcof_po_d%s",ktt->V->Name); V=TakeVary(Inf);
   V->Znach=DifBy(Vy->Znach,ktt->V); TC->Tabs->Add(Inf);
 }}
if(FFF) fwprintf(FFF,L"ChX=%g; ChY=%g; Zn=%g;\n",Val(NumX),Val(NumY),Val(Denum));
Drezx=Dnumx/Ddenum; Drezy=Dnumy/Ddenum;
Dx=Val(Vx->Znach); Dy=Val(Vy->Znach);
if(FFF){
 fwprintf(FFF,L"RezX=%g; RezY=%g; Dx=%g; Dy=%g;\n",Drezx,Drezy,Dx,Dy);
 fclose(FFF);
}
//поделим почленно на знаменатель все слагаемые
 for(Body *b=L->B;b;b=b->Sled){
  wcscpy(NameB,b->Name); //if(s=wcschr(NameB,'~')) *s='\0';
  swprintf(Inf,L"Sx1@%s",NameB);
  if(FindVary(Inf,&Vsx1)){
   Fsx1.V=Vsx1;
   swprintf(Inf,L"Sx1@%s_r",NameB); Vsx1=TakeVary(Inf); Vsx1->Znach=Fsx1/Denum;
  }
  swprintf(Inf,L"Sx2@%s",NameB);
  if(FindVary(Inf,&Vsx2)){
   Fsx2.V=Vsx2;
   swprintf(Inf,L"Sx2@%s_r",NameB); Vsx2=TakeVary(Inf); Vsx2->Znach=Fsx2/Denum;
  }
  swprintf(Inf,L"Sy1@%s",NameB);
  if(FindVary(Inf,&Vsy1)){
   Fsy1.V=Vsy1;
   swprintf(Inf,L"Sy1@%s_r",NameB); Vsy1=TakeVary(Inf); Vsy1->Znach=Fsy1/Denum;
  }
  swprintf(Inf,L"Sy2@%s",NameB);
  if(FindVary(Inf,&Vsy2)){
   Fsy2.V=Vsy2;
   swprintf(Inf,L"Sy2@%s_r",NameB); Vsy2=TakeVary(Inf); Vsy2->Znach=Fsy2/Denum;
  }
  if(MOMIN){
   swprintf(Inf,L"Sx3@%s",NameB);
   if(FindVary(Inf,&Vsx3)){
    Fsx3.V=Vsx3;
    swprintf(Inf,L"Sx3@%s_r",NameB); Vsx3=TakeVary(Inf); Vsx3->Znach=Fsx3/Denum;
   }
   swprintf(Inf,L"Sy3@%s",NameB);
   if(FindVary(Inf,&Vsy3)){
    Fsy3.V=Vsy3;
    swprintf(Inf,L"Sy3@%s_r",NameB); Vsy3=TakeVary(Inf); Vsy3->Znach=Fsy3/Denum;
}}}}
//---------------------------------------------------------------------------
bool EqvNul(Form F){
 if((F.C->Atr==CNST)&&(F.C==Nul)) return true;
 if(F.C->Atr==VARY) return EqvNul(F.V->Znach);
 return false;
}
//---------------------------------------------------------------------------
bool EqvOdin(Form F){
 if((F.C->Atr==CNST)&&(F.C==Odin)) return true;
 if(F.C->Atr==VARY) return EqvOdin(F.V->Znach);
 return false;
}
//---------------------------------------------------------------------------
bool Bodys(Body *B,TStatusBar *StatusBar,TTabControl *TC,TCGauge *CGauge){
 wchar_t Mes[512],NameCord[256],NameVelo[256],NameAcce[256],NameElem[256],*inf,SBold[256];
 int i,j,k,N,Progress,MaxValue; Parm *P=B->KoordSys; Root *I; Magazine *M=NULL,*m;
 Elem *Jmx,*Jmy,*Jmz,*Jwx,*Jwy,*Jwz; bool ErrorInert=false;
 Form FomegaX,FomegaY,FomegaZ; Vary *Kx=NULL,*Ky=NULL,*Kz=NULL,*K,*V,*A,*W;
 Body *b,*bb; Inf[0]='\0';
// OTLADKA=FindInst(L"ОТЛАДКА",&I);
// if(OTLADKA&&!_waccess(L"OTLADKA",0)) _wunlink(L"OTLADKA");
//вставка 02.10.2016
 if(I=FindInstLast(L"УПРАВЛЕНИЕ COF")){ Parm *p,*pp; Vary *V;
  if(I->First&&I->First->Nam&&!wcscmp(LowToUpp(I->First->Nam),L"КООРДИНАТЫ COF")&&
	 (p=I->First->R)&&(KolElem(p)==2)&&p->Nam&&p->Sled->Nam&&I->First->Sled&&
	 I->First->Sled->Nam&&!wcscmp(LowToUpp(I->First->Sled->Nam),L"УПРАВЛЯЮЩИЕ КООРДИНАТЫ")&&
	 (pp=I->First->Sled->R)&&(KolElem(pp)==2)&&pp->Nam&&pp->Sled->Nam){
   wchar_t *Coord1=pp->Nam,*Coord2=pp->Sled->Nam,VCoord1[256],WCoord1[256],
	VCoord2[256],WCoord2[256];
   vC1=TakeVary(Coord1); FC1=vC1->Znach; vC1->Znach.C=Pust; TakeCord(vC1);
   vC2=TakeVary(Coord2); FC2=vC2->Znach; vC2->Znach.C=Pust; TakeCord(vC2);
   FV1=DifByT(FC1); FV2=DifByT(FC2);  FvW1=DifByT(FV1); FvW2=DifByT(FV2);
   swprintf(VCoord1,L"%s'%s",Coord1,L->Time->Name);  vV1=TakeVary(VCoord1);
   swprintf(VCoord2,L"%s'%s",Coord2,L->Time->Name);  vV2=TakeVary(VCoord2);
   swprintf(WCoord1,L"%s'%s",VCoord1,L->Time->Name); vW1=TakeVary(WCoord1);
   swprintf(WCoord2,L"%s'%s",VCoord2,L->Time->Name); vW2=TakeVary(WCoord2);
 }}
// CoordWithoutDT(L);
//Проверим, что есть все тела
 for(*Mes='\0',inf=Mes,b=B;b;b=b->Sled){
  if(b->BaseName&&!FindBody(b->BaseName,&bb))
   inf+=swprintf(inf,L"%s~%s, ",b->Name,b->BaseName);
 }
 if(*Mes){
  Mes[wcslen(Mes)-2]='\0';
  swprintf(Inf,L"Следующие тела\n%s\nссылаются на несуществующие",Mes);
  Error(B->Line,0,true);
  return false;
 }
//Проверим, есть ли кольца
 Mes[0]='\0'; inf=Mes;
 for(b=B;b;b=b->Sled){
  DelAllMagazine(&M);
  if(TestBody(b,&M)) inf+=swprintf(inf,L"%s, ",b->BaseName);
  if(KolElem(b->Inert)>10)
   ErrorInert=true;
  else if(KolElem(b->Inert)<1){
//   for(P=b->Inert;P;P=P->Sled){
//    if(!P->R||P->R->Sled){
//     swprintf(Inf,
//      L"При описании твердого тела (строка %d) надо задать\n"
//      L"инерционные параметры в виде списка: имя(значение),\n"
//      L"где имя - одно из обозначений - m,Jx,Jy,Jz,Jxy,Jxz,Jyz",b->Line);
//     Error(b->Line,0,true);
     ErrorInert=true; //break;
  }//}
  if(ErrorInert){
   swprintf(Inf,
    L"В описание твердого тела входит список параметров\n"
    L"инерции - координаты центра масс в связанной системе\n"
    L"координат этого тела, масса и моменты инерции - \n"
    L"составляющие его тензора инерции. Координаты \n"
    L"центра масс указываются элементами списка вида\n"
    L"Sx(абсцисса), Sy(ордината), Sz(аппликата).\n"
    L"Если тензор инерции тела диагонален, указываются только\n"
    L"осевые моменты инерции, иначе - еще центробежные.\n"
    L"Такие параметры представляются списком в виде,\n"
    L"m(<масса>),Jx(<значение>),Jy(<значение>),Jz(<значение>),\n"
    L"Jxy(<значение>),Jyz(<значение>),Jxz(<значение>),\n"
    L"где <масса> и <значение> - число или имя переменной\n"
    L"с соответствующим числовым или формульным значением\n"
    L"или в виде\n"
    L"<значение_m>,<значение_Jx>,<значение_Jy>,<значение_Jz>,\n"
    L"<значение_Jxy>,<значение_Jyz>,<значение_Jxz>.\n"
    L"В первом случае можно пропускать некоторые значения\n"
    L"или задавать их в произвольном порядке, во втором - их\n"
	L"следует записать в указанном порядке.");
   Error(b->Line,0,true);
   return false;
 }}
 if(*Mes){
  Mes[wcslen(Mes)-2]='\0';
  swprintf(Inf,L"Следующие тела\n%s\nссылаются сами на себя",Inf);
  Error(B->Line,0,true);
  return false;
 }
// if(_waccess(OprFile,0)){
//  if(OPRFILE=_wfopen(OprFile,L"w")){
 if(SLO) SLO->Clear();
 else SLO=new TStringList;
 Sopr[0]='\0'; sopr=Sopr;
 SLO->Add(L"<html>"); SLO->Add(L"&nbsp;<head>");
 SLO->Add("&nbsp;&nbsp;<title>ПРОТОКОЛ ОПЕРАТОРОВ</title>");
 SLO->Add(L"&nbsp;&nbsp;<meta http-equiv=\"Content-Type\""
  L" content=\"text/html charset=windows-1251\">");
 SLO->Add(L"&nbsp;</head>");
 sopr+=swprintf(sopr,L"<body bgcolor=\"#FFFFFF\""
  L" text=\"#800000\" background=\"%s/fon2.jpg\""
  L" font face=\"Times New Roman, Times, serif\">",WORKDIR);
 SLO->Add(Sopr); *Sopr='\0'; sopr=Sopr;
 SLO->Add(L"<h2 align=\"center\"><font color=\"#408080\">"
		  L"ПРОТОКОЛ ОПЕРАТОРОВ</font></h2>");
//Сбросим накопительные векторы и матрицу
 for(int i=0;i<3;i++){
  S[i][i].C=Odin;
  for(int j=i+1;j<3;j++) S[i][j].C=S[j][i].C=Nul;
 }
 for(b=B;b;b=b->Sled){
  for(int i=0;i<3;i++){
   b->rCk[i].C=b->wkk[i].C=Nul; b->Sk0[i][i].C=Odin;
 }}
 if(OTLADKA){
  FILE *F=_wfopen(L"Otladka",L"w"); Fputs(L"Отладочная информация\n",F); fclose(F);
 }
//Сделаем накопления с помощью рекурсивной процедуры
 if(CGauge){
  Progress=CGauge->Progress; MaxValue=CGauge->MaxValue;
  CGauge->Progress=0; CGauge->MaxValue=KolElem(B);
 }
 if(StatusBar)
  wcscpy(SBold,StatusBar->SimpleText.c_str());
 if(PRINTOP){  KO=(int *)calloc(6,SzI); ClearOper(); }
 for(b=B;b;b=b->Sled){
  Root *I; wchar_t NameKx[256],NameKy[256],NameKz[256]; FILE *FFF;
  if(StatusBar){
   swprintf(Inf,L"Обработка твердого тела: %s",b->Name);
   StatusBar->SimpleText=Inf;
  }
  if(CGauge) CGauge->Progress++;
  swprintf(Sopr,L"<h3 align=\"center\"><font color=\"#800000\">"
   L"Оператор ТВЕРДОЕ ТЕЛО с именем \"%s\" строки %d сгенерировал элементы и структуры:"
   L"</font></h3>",b->Name,b->Line);
  SLO->Add(Sopr); *(sopr=Sopr)='\0';
  if(!Accumulate(b)){ IsError=true; return false; }
  if(b->Inert->R&&b->Inert->Nam){//надо искать по имени
   for(P=b->Inert;P;P=P->Sled){
	if(!wcscmp(P->Nam,L"m")){
	 swprintf(NameCord,L"XC@%s",b->Name);
	 if(!(K=TakeVary(NameCord))){ IsError=true; return false; }
	 if(K->Znach!=Nul){
	  swprintf(NameElem,L"J%s.%s",b->Name,NameCord);
	  if(!(Jmx=TakeElem(NameElem))){ IsError=true; return false; }
	  Jmx->Znach=b->m=TakeZnach(P); Jmx->K=K;
	  sopr=FormToStringList(Jmx->Znach,Jmx->Name,Sopr,sopr,SLO,true,&SF);
	  sopr+=swprintf(sopr,L"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;");
	  sopr=FormToStringList(Jmx->K->Znach,Jmx->K->Name,Sopr,sopr,SLO,true,&SF);
	  if(PRINTOP) PrintOper(SLO,Jmx->K->Znach,KO);
	  sopr+=swprintf(sopr,L"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;");
	  swprintf(NameCord,L"VCX@%s",b->Name);
	  if(FindVary(NameCord,&K)){
	   if(K->Znach.C->Atr==VARY){
		swprintf(Inf,L"%s=%s",NameCord,K->Znach.V->Name);
		sopr=FormToStringList(K->Znach.V->Znach,Inf,Sopr,sopr,SLO,true,&SF);
	   }
	   else
		sopr=FormToStringList(K->Znach,NameCord,Sopr,sopr,SLO,true,&SF);
	   *sopr='\0';
	   if(PRINTOP){ Form F; F.V=K; PrintOper(SLO,F,KO); }
	   sopr+=swprintf(sopr,L"<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;");
	  }
	  swprintf(NameCord,L"ACX@%s",b->Name);
	  if(FindVary(NameCord,&K)){
	   if(K->Znach.C->Atr==VARY){
		if(K->Znach.V->Znach.C->Atr==VARY){
		 swprintf(Inf,L"%s=%s=%s",NameCord,K->Znach.V->Name,
		  K->Znach.V->Znach.V->Name);
		 sopr=FormToStringList(K->Znach.V->Znach.V->Znach,Inf,Sopr,
		  sopr,SLO,true,&SF);
		}
		else{
		 swprintf(Inf,L"%s=%s",NameCord,K->Znach.V->Name);
		 sopr=FormToStringList(K->Znach.V->Znach,Inf,Sopr,sopr,SLO,
		  true,&SF);
		 swprintf(NameCord,L"XC@%s'%s'%s",b->Name,L->Time->Name,L->Time->Name);
		 if(FindVary(NameCord,&K))
		  sopr=FormToStringList(K->Znach,NameCord,Sopr,sopr,SLO,true,&SF);
	   }}
	   else
		sopr=FormToStringList(K->Znach,NameCord,Sopr,sopr,SLO,true,&SF);
		*sopr='\0';
	   if(PRINTOP){ Form F; F.V=K; PrintOper(SLO,F,KO); }
	  }
	  SLO->Add(L"<br>");
	 }
	 swprintf(NameCord,L"YC@%s",b->Name);
	 if(!(K=TakeVary(NameCord))){ IsError=true; return false; }
	 if(K->Znach!=Nul){
	  swprintf(NameElem,L"J%s.%s",b->Name,NameCord);
	  if(!(Jmy=TakeElem(NameElem))){ IsError=true; return false; }
	  Jmy->Znach=Jmx->Znach; Jmy->K=K;
	  sopr=FormToStringList(Jmy->Znach,Jmy->Name,Sopr,sopr,SLO,true,&SF);
	  sopr+=swprintf(sopr,L"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;");
	  sopr=FormToStringList(Jmy->K->Znach,Jmy->K->Name,Sopr,sopr,SLO,true,&SF);
	  if(PRINTOP) PrintOper(SLO,Jmy->K->Znach,KO);
	  sopr+=swprintf(sopr,L"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;");
	  swprintf(NameCord,L"VCY@%s",b->Name);
	  if(FindVary(NameCord,&K)){
	   if(K->Znach.C->Atr==VARY){
		swprintf(Inf,L"%s=%s",NameCord,K->Znach.V->Name);
		sopr=FormToStringList(K->Znach.V->Znach,Inf,Sopr,sopr,SLO,true,&SF);
	   }
	   else
		sopr=FormToStringList(K->Znach,NameCord,Sopr,sopr,SLO,true,&SF);
	   *sopr='\0';
	   if(PRINTOP){ Form F; F.V=K; PrintOper(SLO,F,KO); }
	   sopr+=swprintf(sopr,L"<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;");
	  }
	  swprintf(NameCord,L"ACY@%s",b->Name);
	  if(FindVary(NameCord,&K)){
	   if(K->Znach.C->Atr==VARY){
		if(K->Znach.V->Znach.C->Atr==VARY){
		 swprintf(Inf,L"%s=%s=%s",NameCord,K->Znach.V->Name,
		  K->Znach.V->Znach.V->Name);
		 sopr=FormToStringList(K->Znach.V->Znach.V->Znach,Inf,Sopr,
		  sopr,SLO,true,&SF);
		}
		else{
		 swprintf(Inf,L"%s=%s",NameCord,K->Znach.V->Name);
		 sopr=FormToStringList(K->Znach.V->Znach,Inf,Sopr,sopr,SLO,
		  true,&SF);
		 swprintf(NameCord,L"YC@%s'%s'%s",b->Name,L->Time->Name,L->Time->Name);
		 if(FindVary(NameCord,&K))
		  sopr=FormToStringList(K->Znach,NameCord,Sopr,sopr,SLO,true,&SF);
	   }}
	   else
		sopr=FormToStringList(K->Znach,NameCord,Sopr,sopr,SLO,true,&SF);
	   *sopr='\0';
	   if(PRINTOP){ Form F; F.V=K; PrintOper(SLO,F,KO); }
	  }
	  SLO->Add(L"<br>");
	 }
	 swprintf(NameCord,L"ZC@%s",b->Name);
	 if(!(K=TakeVary(NameCord))){ IsError=true; return false; }
	 if(K->Znach!=Nul){
	  swprintf(NameElem,L"J%s.%s",b->Name,NameCord);
	  if(!(Jmz=TakeElem(NameElem))){ IsError=true; return false; }
	  Jmz->Znach=Jmx->Znach; Jmz->K=K;
	  sopr=FormToStringList(Jmz->Znach,Jmz->Name,Sopr,sopr,SLO,true,&SF);
	  sopr+=swprintf(sopr,L"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;");
	  sopr=FormToStringList(Jmz->K->Znach,Jmz->K->Name,Sopr,sopr,SLO,true,&SF);
	  if(PRINTOP) PrintOper(SLO,Jmz->K->Znach,KO);
	  sopr+=swprintf(sopr,L"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;");
	  swprintf(NameCord,L"VCZ@%s",b->Name);
	  if(FindVary(NameCord,&K)){
	   if(K->Znach.C->Atr==VARY){
		swprintf(Inf,L"%s=%s",NameCord,K->Znach.V->Name);
		sopr=FormToStringList(K->Znach.V->Znach,Inf,Sopr,sopr,SLO,true,&SF);
	   }
	   else
		sopr=FormToStringList(K->Znach,NameCord,Sopr,sopr,SLO,true,&SF);
	   *sopr='\0';
	   if(PRINTOP){ Form F; F.V=K; PrintOper(SLO,F,KO); }
	   sopr+=swprintf(sopr,L"<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;");
	  }
	  swprintf(NameCord,L"ACZ@%s",b->Name);
	  if(FindVary(NameCord,&K)){
	   if(K->Znach.C->Atr==VARY){
		if(K->Znach.V->Znach.C->Atr==VARY){
		 swprintf(Inf,L"%s=%s=%s",NameCord,K->Znach.V->Name,
		  K->Znach.V->Znach.V->Name);
		 sopr=FormToStringList(K->Znach.V->Znach.V->Znach,Inf,Sopr,
		  sopr,SLO,true,&SF);
		}
		else{
		 swprintf(Inf,L"%s=%s",NameCord,K->Znach.V->Name);
		 sopr=FormToStringList(K->Znach.V->Znach,Inf,Sopr,sopr,SLO,
		  true,&SF);
		 swprintf(NameCord,L"ZC@%s'%s'%s",b->Name,L->Time->Name,L->Time->Name);
		 if(FindVary(NameCord,&K))
		  sopr=FormToStringList(K->Znach,NameCord,Sopr,sopr,SLO,true,&SF);
	   }}
	   else
		sopr=FormToStringList(K->Znach,NameCord,Sopr,sopr,SLO,true,&SF);
	   *sopr='\0';
	   if(PRINTOP){ Form F; F.V=K; PrintOper(SLO,F,KO); }
	  }
	  SLO->Add(L"<br>");
	}}
	else if(!wcscmp(P->Nam,L"Jx")){
	 swprintf(NameCord,L"omegax@%s",b->Name);
	 if(!(K=TakeVary(NameCord))){ IsError=true; return false; }
	 swprintf(NameElem,L"J%s.%s",b->Name,NameCord);
	 if(!(Jwx=TakeElem(NameElem))){ IsError=true; return false; }
	 Jwx->Znach=b->Jx=TakeZnach(P); Jwx->K=K;
	 sopr=FormToStringList(Jwx->Znach,Jwx->Name,Sopr,sopr,SLO,false,&SF);
	 *sopr++=' ';
	 sopr=FormToStringList(Jwx->K->Znach,Jwx->K->Name,Sopr,sopr,SLO,false,&SF);
	 *sopr++=' ';
	 swprintf(NameCord,L"epsilonx@%s",b->Name);
	 if(FindVary(NameCord,&K))
	  sopr=FormToStringList(K->Znach,K->Name,Sopr,sopr,SLO,true,&SF);
	 sopr+=swprintf(sopr,L"<br>");
	 SLO->Add(Sopr); *Sopr='\0'; sopr=Sopr;
	}
	else if(!wcscmp(P->Nam,L"Jy")){
	 swprintf(NameCord,L"omegay@%s",b->Name);
	 if(!(K=TakeVary(NameCord))){ IsError=true; return false; }
	 swprintf(NameElem,L"J%s.%s",b->Name,NameCord);
	 if(!(Jwy=TakeElem(NameElem))){ IsError=true; return false; }
	 Jwy->Znach=b->Jy=TakeZnach(P); Jwy->K=K;
	 sopr=FormToStringList(Jwy->Znach,Jwy->Name,Sopr,sopr,SLO,false,&SF);
	 *sopr++=' ';
	 sopr=FormToStringList(Jwy->K->Znach,Jwy->K->Name,Sopr,sopr,SLO,false,&SF);
	 *sopr++=' ';
	 swprintf(NameCord,L"epsilony@%s",b->Name);
	 if(FindVary(NameCord,&K))
	  sopr=FormToStringList(K->Znach,K->Name,Sopr,sopr,SLO,true,&SF);
	 sopr+=swprintf(sopr,L"<br>");
	 SLO->Add(Sopr); *Sopr='\0'; sopr=Sopr;
	}
	else if(!wcscmp(P->Nam,L"Jz")){
	 swprintf(NameCord,L"omegaz@%s",b->Name);
	 if(!(K=TakeVary(NameCord))){ IsError=true; return false; }
	 swprintf(NameElem,L"J%s.%s",b->Name,NameCord);
	 if(!(Jwz=TakeElem(NameElem))){ IsError=true; return false; }
	 Jwz->Znach=b->Jz=TakeZnach(P); Jwz->K=K;
	 sopr=FormToStringList(Jwz->Znach,Jwz->Name,Sopr,sopr,SLO,false,&SF);
	 *sopr++=' ';
	 sopr=FormToStringList(Jwz->K->Znach,Jwz->K->Name,Sopr,sopr,SLO,false,&SF);
	 *sopr++=' ';
	 swprintf(NameCord,L"epsilonz@%s",b->Name);
	 if(FindVary(NameCord,&K))
	  sopr=FormToStringList(K->Znach,K->Name,Sopr,sopr,SLO,true,&SF);
	 sopr+=swprintf(sopr,L"<br>");
	 SLO->Add(Sopr); *Sopr='\0'; sopr=Sopr;
	}
	else if(!wcscmp(P->Nam,L"Jxy")){
	 wchar_t NameOmX[256],NameOmY[256];
	 swprintf(NameOmX,L"omegax@%s",b->Name); FomegaX.V=TakeVary(NameOmX);
	 swprintf(NameOmY,L"omegay@%s",b->Name); FomegaY.V=TakeVary(NameOmY);
	 if(!Kx){ swprintf(NameKx,L"Kx@%s",b->Name); Kx=TakeVary(NameKx); }
	 if(!Ky){ swprintf(NameKy,L"Ky@%s",b->Name); Ky=TakeVary(NameKy); }
	 b->Jxy=TakeZnach(P);
	 Kx->Znach=Jwx->Znach*FomegaX-b->Jxy*FomegaY; Ky->Znach=-b->Jxy*FomegaX;
	 b->Kx.V=Kx; b->Ky.V=Ky;
	}
	else if(!wcscmp(P->Nam,L"Jyz")){
	 wchar_t NameOmY[256],NameOmZ[256];
	 swprintf(NameOmY,L"omegay@%s",b->Name); FomegaY.V=TakeVary(NameOmY);
	 swprintf(NameOmZ,L"omegaz@%s",b->Name); FomegaZ.V=TakeVary(NameOmZ);
	 if(!Ky){ swprintf(NameKy,L"Ky@%s",b->Name); Ky=TakeVary(NameKy); }
	 if(!Kz){ swprintf(NameKz,L"Kz@%s",b->Name); Kz=TakeVary(NameKz); }
	 b->Jyz=TakeZnach(P);
	 Ky->Znach=Ky->Znach+Jwy->Znach*FomegaY-b->Jyz*FomegaZ;
	 Kz->Znach=-b->Jyz*FomegaY+Jwz->Znach*FomegaZ;
	 b->Kz.V=Kz;
	}
	else if(!wcscmp(P->Nam,L"Jxz")){
	 wchar_t NameOmX[256],NameOmZ[256];
	 swprintf(NameOmX,L"omegax@%s",b->Name); FomegaX.V=TakeVary(NameOmX);
	 swprintf(NameOmZ,L"omegaz@%s",b->Name); FomegaZ.V=TakeVary(NameOmZ);
	 if(!Kx){ swprintf(NameKx,L"Kx@%s",b->Name); Kx=TakeVary(NameKx); }
	 if(!Kz){ swprintf(NameKz,L"Kz@%s",b->Name); Kz=TakeVary(NameKz); }
	 b->Jxz=TakeZnach(P);
	 Kx->Znach=Kx->Znach-b->Jxz*FomegaZ;
	 Kz->Znach=-b->Jxz*FomegaX+Kz->Znach;
	}
	if(Kx){
	 sopr=FormToStringList(Kx->Znach,Kx->Name,Sopr,sopr,SLO,true,&SF);
	 sopr+=swprintf(sopr,L"<br>");
	 SLO->Add(Sopr); *Sopr='\0'; sopr=Sopr;
	 Kx=NULL;
	}
	if(Ky){
	 sopr=FormToStringList(Ky->Znach,Ky->Name,Sopr,sopr,SLO,true,&SF);
	 sopr+=swprintf(sopr,L"<br>");
	 SLO->Add(Sopr); *Sopr='\0'; sopr=Sopr;
	 Ky=NULL;
	}
	if(Kz){
	 sopr=FormToStringList(Kz->Znach,Kz->Name,Sopr,sopr,SLO,true,&SF);
	 sopr+=swprintf(sopr,L"<br>");
	 SLO->Add(Sopr); *Sopr='\0'; sopr=Sopr;
	 Kz=NULL;
  }}}
  else{
   swprintf(NameCord,L"XC@%s",b->Name);
   if(!(K=TakeVary(NameCord))){ IsError=true; return false; }
   if(K->Znach!=Nul){
	swprintf(NameElem,L"J%s.%s",b->Name,NameCord);
	if(!(Jmx=TakeElem(NameElem))){ IsError=true; return false; }
	Jmx->Znach=TakeZnach(b->Inert);	Jmx->K=K;
	sopr=FormToStringList(Jmx->Znach,Jmx->Name,Sopr,sopr,SLO,false,&SF);
	*sopr++=' ';
	sopr=FormToStringList(Jmx->K->Znach,Jmx->K->Name,Sopr,sopr,SLO,false,&SF);
	sopr+=swprintf(sopr,L"<br>");
	SLO->Add(Sopr); *Sopr='\0'; sopr=Sopr;
   }
   swprintf(NameCord,L"YC@%s",b->Name);
   if(!(K=TakeVary(NameCord))){ IsError=true; return false; }
   if(K->Znach!=Nul){
	swprintf(NameElem,L"J%s.%s",b->Name,NameCord);
	if(!(Jmy=TakeElem(NameElem))){ IsError=true; return false; }
	Jmy->Znach=TakeZnach(b->Inert);	Jmy->K=K;
	sopr=FormToStringList(Jmy->Znach,Jmy->Name,Sopr,sopr,SLO,false,&SF);
	*sopr++=' ';
	sopr=FormToStringList(Jmy->K->Znach,Jmy->K->Name,Sopr,sopr,SLO,false,&SF);
	sopr+=swprintf(sopr,L"<br>");
	SLO->Add(Sopr); *Sopr='\0'; sopr=Sopr;
   }
   swprintf(NameCord,L"ZC@%s",b->Name);
   if(!(K=TakeVary(NameCord))){ IsError=true; return false; }
   if(K->Znach!=Nul){
	swprintf(NameElem,L"J%s.%s",b->Name,NameCord);
	if(!(Jmz=TakeElem(NameElem))){ IsError=true; return false; }
	Jmz->Znach=TakeZnach(b->Inert);	Jmz->K=K;
	sopr=FormToStringList(Jmz->Znach,Jmz->Name,Sopr,sopr,SLO,false,&SF);
	*sopr++=' ';
	sopr=FormToStringList(Jmz->K->Znach,Jmz->K->Name,Sopr,sopr,SLO,false,&SF);
	sopr+=swprintf(sopr,L"<br>");
	SLO->Add(Sopr); *Sopr='\0'; sopr=Sopr;
   }
//моменты пар сил инерции
   if(NoGeneral(b->Inert->Sled))
	Rotation(b);
   P=b->Inert->Sled;
   swprintf(NameCord,L"omegax@%s",b->Name);
   if(!(K=TakeVary(NameCord))){ IsError=true; return false; }
   if(K->Znach!=Nul){
	swprintf(NameElem,L"J%s.%s",b->Name,NameCord);
	if(!(Jwx=TakeElem(NameElem))){ IsError=true; return false; }
	Jwx->Znach=TakeZnach(P); Jwx->K=K;
	sopr=FormToStringList(Jwx->Znach,Jwx->Name,Sopr,sopr,SLO,false,&SF);
	*sopr++=' ';
	sopr=FormToStringList(Jwx->K->Znach,Jwx->K->Name,Sopr,sopr,SLO,false,&SF);
	sopr+=swprintf(sopr,L"<br>");
	SLO->Add(Sopr); *Sopr='\0'; sopr=Sopr;
   }
   P=P->Sled;
   swprintf(NameCord,L"omegay@%s",b->Name);
   if(!(K=TakeVary(NameCord))){ IsError=true; return false; }
   if(K->Znach!=Nul){
	swprintf(NameElem,L"J%s.%s",b->Name,NameCord);
	if(!(Jwy=TakeElem(NameElem))){ IsError=true; return false; }
	Jwy->Znach=TakeZnach(P); Jwy->K=K;
	sopr=FormToStringList(Jwy->Znach,Jwy->Name,Sopr,sopr,SLO,false,&SF);
	*sopr++=' ';
	sopr=FormToStringList(Jwy->K->Znach,Jwy->K->Name,Sopr,sopr,SLO,false,&SF);
	sopr+=swprintf(sopr,L"<br>");
	SLO->Add(Sopr); *Sopr='\0'; sopr=Sopr;
   }
   P=P->Sled;
   swprintf(NameCord,L"omegaz@%s",b->Name);
   if(!(K=TakeVary(NameCord))){ IsError=true; return false; }
   if(K->Znach!=Nul){
	swprintf(NameElem,L"J%s.%s",b->Name,NameCord);
	if(!(Jwz=TakeElem(NameElem))){ IsError=true; return false; }
	Jwz->Znach=TakeZnach(P); Jwz->K=K;
	sopr=FormToStringList(Jwz->Znach,Jwz->Name,Sopr,sopr,SLO,false,&SF);
	*sopr++=' ';
	sopr=FormToStringList(Jwz->K->Znach,Jwz->K->Name,Sopr,sopr,SLO,false,&SF);
	sopr+=swprintf(sopr,L"<br>");
	SLO->Add(Sopr); *Sopr='\0'; sopr=Sopr;
   }
   if(P=P->Sled){
	wchar_t NameOmX[256],NameOmY[256];
	swprintf(NameOmX,L"omegax@%s",b->Name); FomegaX.V=TakeVary(NameOmX);
	swprintf(NameOmY,L"omegay@%s",b->Name); FomegaY.V=TakeVary(NameOmY);
	if(!Kx){ swprintf(NameKx,L"Kx@%s",b->Name); Kx=TakeVary(NameKx); }
	if(!Ky){ swprintf(NameKy,L"Ky@%s",b->Name); Ky=TakeVary(NameKy); }
	b->Jxy=TakeZnach(P);
	Kx->Znach=Jwx->Znach*FomegaX;
	Kx->Znach=Kx->Znach-b->Jxy*FomegaY;
	Ky->Znach=-b->Jxy*FomegaX;
	b->Kx.V=Kx;
	b->Ky.V=Ky;
	if(P=P->Sled){
	 wchar_t NameOmY[256],NameOmZ[256];
	 swprintf(NameOmY,L"omegay@%s",b->Name); FomegaY.V=TakeVary(NameOmY);
	 swprintf(NameOmZ,L"omegaz@%s",b->Name); FomegaZ.V=TakeVary(NameOmZ);
	 if(!Ky){ swprintf(NameKy,L"Ky@%s",b->Name); Ky=TakeVary(NameKy); }
	 if(!Kz){ swprintf(NameKz,L"Kz@%s",b->Name); Kz=TakeVary(NameKz); }
	 b->Jyz=TakeZnach(P);
	 Ky->Znach=Ky->Znach+Jwy->Znach*FomegaY-b->Jyz*FomegaZ;
	 Kz->Znach=-b->Jyz*FomegaY+Jwz->Znach*FomegaZ;
	 b->Kz.V=Kz;
	 if(P=P->Sled){
	  wchar_t NameOmX[256],NameOmZ[256];
	  swprintf(NameOmX,L"omegax@%s",b->Name); FomegaX.V=TakeVary(NameOmX);
	  swprintf(NameOmZ,L"omegaz@%s",b->Name); FomegaZ.V=TakeVary(NameOmZ);
	  b->Jxz=TakeZnach(P);
	  if(!Kx){ swprintf(NameKx,L"Kx@%s",b->Name); Kx=TakeVary(NameKx); }
	  if(!Kz){ swprintf(NameKz,L"Kz@%s",b->Name); Kz=TakeVary(NameKz); }
	  Kx->Znach=Kx->Znach-b->Jxz*FomegaZ;
	  Kz->Znach=-b->Jxz*FomegaX+Kz->Znach;
   }}}
   if(Kx){
	sopr=FormToStringList(Kx->Znach,Kx->Name,Sopr,sopr,SLO,true,&SF);
	sopr+=swprintf(sopr,L"<br>");
	SLO->Add(Sopr); *Sopr='\0'; sopr=Sopr;
	Kx=NULL;
   }
   if(Ky){
	sopr=FormToStringList(Ky->Znach,Ky->Name,Sopr,sopr,SLO,true,&SF);
	sopr+=swprintf(sopr,L"<br>");
	SLO->Add(Sopr); *Sopr='\0'; sopr=Sopr;
	Ky=NULL;
   }
   if(Kz){
	sopr=FormToStringList(Kz->Znach,Kz->Name,Sopr,sopr,SLO,true,&SF);
	sopr+=swprintf(sopr,L"<br>");
	SLO->Add(Sopr); *Sopr='\0'; sopr=Sopr;
	Kz=NULL;
  }}
  for(int i=0;i<3;i++){
   swprintf(NameCord,L"%cO@%s",i==0?'X':i==1?'Y':'Z',b->Name);
   if(!(K=TakeVary(NameCord))){ IsError=true; return false; }
   sopr=FormToStringList(K->Znach,NameCord,Sopr,sopr,SLO,true,&SF);
   sopr+=swprintf(sopr,L"<br>");
   SLO->Add(Sopr); *Sopr='\0'; sopr=Sopr;
 }}
 if(CGauge){
  CGauge->Progress=Progress; CGauge->MaxValue=MaxValue;
 }
 if(StatusBar)
  StatusBar->SimpleText=SBold;
//построим координаты точек, заданных в СК тел и записанных массивами c ~
 bool EtchoNet=true;
 for(Arra *A=L->A;A;A=A->Sled){ wchar_t *s; Vary *V; Form F; bool Uge=false;
  if((s=wcschr(A->Name,'~'))&&(A->N==3)&&FindBody(s+1,&b)){
   if(!Uge&&OTLADKA){
	swprintf(Inf,L"X@%s",A->Name);
	PrintFormMatr(L"Otladka",Inf,L"b->Sk0",b->Sk0);
	Uge=true;
   }
   *s='\0';  //далее изменение 31.07.2016
   for(int i=0;i<3;i++){
	swprintf(Inf,L"%c@%s",i==2?'Z':i==1?'Y':'X',A->Name);
	if(V=TakeVary(Inf)){
	 if(b->rOk[i].V->Znach.C==Nul) //было b->rCk[i]
	  V->Znach.C=Nul;
	 else
	  V->Znach=b->rOk[i];
	 for(int j=0;j<3;j++){
	  Form Mult;
	  if(A->F) F=A->F[j]; else F.C=TakeCnst(A->A[j]);
	  if(EqvNul(b->Sk0[i][j])) Mult.C=Nul;
	  else if(EqvOdin(b->Sk0[i][j])) Mult=F;
	  else Mult=F*b->Sk0[i][j];
/*      switch(b->Sk0[i][j].C->Atr){
	   case CNST:
		if(b->Sk0[i][j].C==Nul) Mult.C=Nul;
		else if(b->Sk0[i][j].C==Odin) Mult=F;
		else Mult=F*b->Sk0[i][j];
		break;
	   case VARY:
		if(b->Sk0[i][j].V->Znach.C==Nul) Mult.C=Nul;
		else if(b->Sk0[i][j].V->Znach.C==Odin) Mult=F;
		else Mult=F*b->Sk0[i][j];
		break;
	   default: Mult=F*b->Sk0[i][j];
	  }*/
	  V->Znach=V->Znach+Mult;
	 }
	 if(EtchoNet){
	  SLO->Add(L"<h4 align=\"center\"><font color=\"#000080\">"
	   L"Координаты точек тел:</font></h4>");
	  EtchoNet=false;
	 }
	 sopr=FormToStringList(V->Znach,V->Name,Sopr,sopr,SLO,false,&SF);
	 if(PRINTOP){ Form F; F.V=K; PrintOper(SLO,F,KO); }
	 sopr+=swprintf(sopr,L"<br>");
	 SLO->Add(Sopr); *Sopr='\0'; sopr=Sopr;
   }}
   *s='~';
 }}
 if(PRINTOP){
  SLO->Add(L"<b>Всего операций в полученных формулах:</b>");
  swprintf(Sopr,L"\"+\"=%d, \"-\"=%d, \"*\"=%d, \":\"=%d,"
   L" \"^\"=%d, \"функций\"=%d<br>",KO[0],KO[1],KO[2],KO[3],KO[4],KO[5]);
  SLO->Add(Sopr); *Sopr='\0'; sopr=Sopr;
  free(KO);
 }
// fclose(OPRFILE);
 SLO->SaveToFile(OprFile);
 return true;
}
//---------------------------------------------------------------------------
Form TakeFormFromParm(Parm *P){ Form F;
 if(P->Nam) F.V=TakeVary(P->Nam); else F.C=TakeCnst(P->Val);
 return F;
}
//---------------------------------------------------------------------------
bool Stringer::TakeAnglsCoords(Parm *P){ Form A1,B1,G1,A2,B2,G2;
 A1=TakeFormFromParm(P); P=P->Sled;
 B1=TakeFormFromParm(P); P=P->Sled;
 G1=TakeFormFromParm(P); P=P->Sled;
 A2=TakeFormFromParm(P); P=P->Sled;
 B2=TakeFormFromParm(P); P=P->Sled;
 G2=TakeFormFromParm(P);
 dA=A2-A1; dB=B2-B1; dG=G2-G1;
 if((FL!=Pust)&&(dX!=Pust)&&(dY!=Pust)&&(dZ!=Pust)){
  dX=dX+0.5*FL*dB; dY=dY-0.5*FL*dA; dZ=dZ-FL;
  return true;
 }
 else return false;
}
//---------------------------------------------------------------------------
void Stringer::TakeDekartCoords(Parm *P){
 X1=TakeFormFromParm(P); P=P->Sled;
 Y1=TakeFormFromParm(P); P=P->Sled;
 Z1=TakeFormFromParm(P); P=P->Sled;
 X2=TakeFormFromParm(P); P=P->Sled;
 Y2=TakeFormFromParm(P); P=P->Sled;
 Z2=TakeFormFromParm(P);
 dX=X2-X1; dY=Y2-Y1; dZ=Z2-Z1;
}
//---------------------------------------------------------------------------
bool Stringer::TakeRGCoords(Parm *P,wchar_t *Name){
 Form Fp0_1,Fp1_1,Fp2_1,Fp3_1,Fp0_2,Fp1_2,Fp2_2,Fp3_2,Fdp0,Fdp1,Fdp2,Fdp3,dx,dy,dz;
 if(P->Nam) Fp0_1.V=TakeVary(P->Nam); else return false; P=P->Sled;
 if(P->Nam) Fp1_1.V=TakeVary(P->Nam); else return false; P=P->Sled;
 if(P->Nam) Fp2_1.V=TakeVary(P->Nam); else return false; P=P->Sled;
 if(P->Nam) Fp3_1.V=TakeVary(P->Nam); else return false; P=P->Sled;
 if(P->Nam) Fp0_2.V=TakeVary(P->Nam); else return false; P=P->Sled;
 if(P->Nam) Fp1_2.V=TakeVary(P->Nam); else return false; P=P->Sled;
 if(P->Nam) Fp2_2.V=TakeVary(P->Nam); else return false; P=P->Sled;
 if(P->Nam) Fp3_2.V=TakeVary(P->Nam); else return false;
 fwprintf(OPRFILE,L"<h4 align=\"center\"><font color=\"#000080\">"
  L"Параметры Родрига-Гамильтона кватерниона поворота правого"
  L"конца относительного левого для стержня %s:</font></h4>\n",Name);
 swprintf(Inf,L"dp0@%s",Name); Fdp0.V=TakeVary(Inf);
 Fdp0.V->Znach=Fp0_1*Fp0_2+Fp1_1*Fp1_2+Fp2_1*Fp2_2+Fp3_1*Fp3_2;
 PrintForm(false,OPRFILE,Inf,Fdp0.V->Znach); fputws(L"&nbsp",OPRFILE);
 swprintf(Inf,L"dp1@%s",Name); Fdp1.V=TakeVary(Inf);
 Fdp1.V->Znach=Fp0_1*Fp1_2-Fp1_1*Fp0_2-Fp2_1*Fp3_2+Fp3_1*Fp2_2;
 PrintForm(false,OPRFILE,Inf,Fdp1.V->Znach); fputws(L"<br>\n",OPRFILE);
 swprintf(Inf,L"dp2@%s",Name); Fdp2.V=TakeVary(Inf);
 Fdp2.V->Znach=Fp0_1*Fp2_2-Fp2_1*Fp0_2-Fp3_1*Fp1_2+Fp1_1*Fp3_2;
 PrintForm(false,OPRFILE,Inf,Fdp2.V->Znach); fputws(L"&nbsp",OPRFILE);
 swprintf(Inf,L"dp3@%s",Name); Fdp3.V=TakeVary(Inf);
 Fdp3.V->Znach=Fp0_1*Fp3_2-Fp3_1*Fp0_2-Fp1_1*Fp2_2+Fp2_1*Fp1_2;
 PrintForm(false,OPRFILE,Inf,Fdp3.V->Znach); fputws(L"<br>\n",OPRFILE);
 dA=2*(Fdp0*Fdp1-Fdp2*Fdp3);//2*(Fdp0*Fdp1+Fdp2*Fdp3);
 dB=2*(Fdp1*Fdp3+Fdp0*Fdp2);
 dG=2*(Fdp0*Fdp3+Fdp1*Fdp2);//2*(Fdp1*Fdp2-Fdp0*Fdp3);
 if((dX!=Pust)&&(dY!=Pust)&&(dZ!=Pust)){ Form S[3][3];
//матрица поворота координат абсолютной СК к относительной
  fwprintf(OPRFILE,L"<h4 align=\"center\"><font color=\"#000080\">"
   L"Матрица поворота координат вектора в абсолютной СК к координатам в связанной со стержнем %s СК:"
   L"</font></h4>\n",Name);
  swprintf(Inf,L"S11@%s",Name); S[0][0].V=TakeVary(Inf);
  S[0][0].V->Znach=2*Fp0_1*Fp0_1+2*Fp1_1*Fp1_1-1;
  PrintForm(false,OPRFILE,Inf,S[0][0].V->Znach);
  fputws(L"&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp",OPRFILE);
  swprintf(Inf,L"S12@%s",Name); S[0][1].V=TakeVary(Inf);
  S[0][1].V->Znach=2*Fp1_1*Fp2_1+2*Fp0_1*Fp3_1;
  PrintForm(false,OPRFILE,Inf,S[0][1].V->Znach); fputws(L"&nbsp&nbsp",OPRFILE);
  swprintf(Inf,L"S13@%s",Name); S[0][2].V=TakeVary(Inf);
  S[0][2].V->Znach=2*Fp1_1*Fp3_1-2*Fp0_1*Fp2_1;
  PrintForm(false,OPRFILE,Inf,S[0][2].V->Znach); fputws(L"<br>\n",OPRFILE);
  swprintf(Inf,L"S21@%s",Name); S[1][0].V=TakeVary(Inf);
  S[1][0].V->Znach=2*Fp1_1*Fp2_1-2*Fp0_1*Fp3_1;
  PrintForm(false,OPRFILE,Inf,S[1][0].V->Znach); fputws(L"&nbsp&nbsp",OPRFILE);
  swprintf(Inf,L"S22@%s",Name); S[1][1].V=TakeVary(Inf);
  S[1][1].V->Znach=2*Fp0_1*Fp0_1+2*Fp2_1*Fp2_1-1;
  PrintForm(false,OPRFILE,Inf,S[1][1].V->Znach);
  fputws(L"&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp",OPRFILE);
  swprintf(Inf,L"S23@%s",Name); S[1][2].V=TakeVary(Inf);
  S[1][2].V->Znach=2*Fp3_1*Fp2_1+2*Fp0_1*Fp1_1;
  PrintForm(false,OPRFILE,Inf,S[1][2].V->Znach); fputws(L"<br>\n",OPRFILE);
  swprintf(Inf,L"S31@%s",Name); S[2][0].V=TakeVary(Inf);
  S[2][0].V->Znach=2*Fp1_1*Fp3_1+2*Fp0_1*Fp2_1;
  PrintForm(false,OPRFILE,Inf,S[2][0].V->Znach); fputws(L"&nbsp&nbsp",OPRFILE);
  swprintf(Inf,L"S32@%s",Name); S[2][1].V=TakeVary(Inf);
  S[2][1].V->Znach=2*Fp3_1*Fp2_1-2*Fp0_1*Fp1_1;
  PrintForm(false,OPRFILE,Inf,S[2][1].V->Znach); fputws(L"&nbsp&nbsp",OPRFILE);
  swprintf(Inf,L"S33@%s",Name); S[2][2].V=TakeVary(Inf);
  S[2][2].V->Znach=2*Fp0_1*Fp0_1+2*Fp3_1*Fp3_1-1;
  PrintForm(false,OPRFILE,Inf,S[2][2].V->Znach); fputws(L"<br>\n",OPRFILE);
  dx=S[0][0]*dX+S[0][1]*dY+S[0][2]*dZ;
  dy=S[1][0]*dX+S[1][1]*dY+S[1][2]*dZ;
  dz=S[2][0]*dX+S[2][1]*dY+S[2][2]*dZ; dX=dx; dY=dy; dZ=dz;
  if((FL!=Pust)){
   dX=dx-0.5*FL*dB; dY=dy+0.5*FL*dA; dZ=dz-FL;
  }
  return true;
 }
 else return false;
}
//---------------------------------------------------------------------------
bool Stringer::Fill(wchar_t *Name){
 Parm *P=Parms; int i=KolElem(P),j,Ind; wchar_t Mes[512];
 if(i!=9) goto Err;
 for(i=0;P;P=P->Sled){
  if(P->Nam&&P->R){
   switch(P->Nam[0]){ Parm *p,*pp,**Pp;
    case 'E':
     if(P->R->Sled){
      swprintf(Inf,L"Модуль упругости (строка %d) это переменная или число!\n",Line);
      Error(Line,0,true); goto Err;
     }
     FE=TakeFormFromParm(P->R); i++;
     break;
    case 'G':
     if(P->R->Sled){
      swprintf(Inf,L"Модуль сдвига (строка %d) это переменная или число!\n",Line);
      Error(Line,0,true); goto Err;
     }
     FG=TakeFormFromParm(P->R); i++;
     break;
    case 'L':
     if(P->R->Sled){
      swprintf(Inf,L"Длина стержня (строка %d) это переменная или число!\n",Line);
      Error(Line,0,true); goto Err;
     }
     FL=TakeFormFromParm(P->R); i++;
     break;
    case 'F':
     if(P->R->Sled){
      swprintf(Inf,L"Площадь сечения (строка %d) это переменная или число!\n",Line);
      Error(Line,0,true); goto Err;
     }
     FF=TakeFormFromParm(P->R); i++;
     break;
    case 'J':
     if(P->R->Sled){
      swprintf(Inf,L"Момент инерции (строка %d) это переменная или число!\n",Line);
      Error(Line,0,true); goto Err;
     }
     switch(P->Nam[1]){
      case 'x': FJx=TakeFormFromParm(P->R); i++; break;
      case 'y': FJy=TakeFormFromParm(P->R); i++; break;
      case 'z': FJz=TakeFormFromParm(P->R); i++; break;
     }
     break;
    case 'S':
     if(KolElem(P->R)!=6){
      swprintf(Inf,L"Координаты концов должны быть указаны в записи\n"
       L"стержня (строка %d) в количестве 6-ти!",Line);
      Error(Line,0,true); goto Err;
     }
     TakeDekartCoords(P->R);
     i++;
     break;
    case 'Q':
     if(KolElem(P->R)!=8){
      swprintf(Inf,L"Параметры Родрига-Гамильтона сечений концов должны быть\n"
       L"указаны в записи стержня (строка %d) в количестве 8-ми!",Line);
      Error(Line,0,true); goto Err;
     }
     if(!TakeRGCoords(P->R,Name)){
      swprintf(Inf,L"В записи стержня (строка %d) сначала должны быть указаны\n"
       L"декартовые координаты концов вида \"S(X1,Y1,Z1,X2,Y2,Z2)\"!",Line);
      Error(Line,0,true); goto Err;
     }
     i++;
     break;
    case 'R':
     if(KolElem(P->R)!=6){
      swprintf(Inf,L"Углы поворота сечений концов должны быть указаны\n"
       L"в записи стержня (строка %d) в количестве 6-ти!",Line);
      Error(Line,0,true); goto Err;
     }
     if(!TakeAnglsCoords(P->R)){
      swprintf(Inf,L"В записи стержня (строка %d) сначала должны быть указаны\n"
       L"декартовые координаты концов вида \"S(X1,Y1,Z1,X2,Y2,Z2)\"!",Line);
      Error(Line,0,true); goto Err;
     }
     i++;
     break;
  }}
  else{
   switch(i){
    case 0: FE =TakeFormFromParm(P); i++; break;
    case 1: FG =TakeFormFromParm(P); i++; break;
    case 2: FL =TakeFormFromParm(P); i++; break;
    case 3: FF =TakeFormFromParm(P); i++; break;
    case 4: FJx=TakeFormFromParm(P); i++; break;
    case 5: FJy=TakeFormFromParm(P); i++; break;
    case 6: FJz=TakeFormFromParm(P); i++; break;
 }}}
 if((FE==Pust)||(FG==Pust)||(FL==Pust)||(FF==Pust)||
    (FJx==Pust)||(FJy==Pust)||(FJz==Pust)||
    (dA==Pust)||(dB==Pust)||(dG==Pust)||
    (dX==Pust)||(dY==Pust)||(dZ==Pust))
     goto Err;
 return true;
Err:
 swprintf(Inf,L"Описание стержня - это запись вида:\n"
  L"<Имя> \\ <Параметры>,<Концы>;\n"
  L"где <Имя> - имя стержня;\n"
  L"<Параметры> - список физических и геометрических параметров стержня вида\n"
  L"E(модуль упругости), G(модуль сдвига), L(длина),\n"
  L"F(площадь поперечного сечения),\n"
  L"Jx(Ix), Jy(Iy), Jz(Iz) - геометрические моменты инерции сечения\n"
  L"относительно, соответственно, двух осей изгиба и продольной оси.\n"
  L"<Концы> - список декартовых координат центров, углов поворота\n"
  L"(или параметров Родрига-Гамильтона) левого и правого концов вида\n"
  L"S(3 координаты левого и 3 координаты правого конца),\n"
  L"R(3 угла левого и 3 угла правого сечения) или\n"
  L"Q(4 параметра Родрига-Гамильтона для левого и 4 - для правого конца);\n"
  L"В скобках для списка <Концы> стоят только имена переменных,\n"
  L"для списка <Параметры> - могут стоять имена переменных или числа.");
 Error(Line,0,true);
 return false;
}
//---------------------------------------------------------------------------
bool Stringer::Elems(){
 Form FEJx=FE*FJx,FEJy=FE*FJy,FGJz=FG*FJz,FEF=FE*FF;
 wchar_t NameCord[256],NameElem[256]; Vary *K; Elem *ECix,*ECiy,*ECk,*ECx,*ECy,*ECz;
 Cix=FEJx/FL; Ciy=FEJy/FL; Ck=FGJz/FL;
 Cx=12*FEJx/(FL^3); Cy=12*FEJy/(FL^3); Cz=FEF/FL;
 swprintf(NameCord,L"dalfa@%s",Name);
 if(!(K=TakeVary(NameCord))){ IsError=true; return false; } K->Znach=dA;
 swprintf(NameElem,L"C.%s",NameCord);
 if(!(ECix=TakeElem(NameElem))){ IsError=true; return false; }
 ECix->Znach=Cix; ECix->K=K;
 PrintForm(false,OPRFILE,ECix->Name,ECix->Znach); fputws(L"<br>\n",OPRFILE);
 PrintForm(false,OPRFILE,ECix->K->Name,ECix->K->Znach); fputws(L"<br>\n",OPRFILE);
 swprintf(NameCord,L"dbeta@%s",Name);
 if(!(K=TakeVary(NameCord))){ IsError=true; return false; } K->Znach=dB;
 swprintf(NameElem,L"C.%s",NameCord);
 if(!(ECiy=TakeElem(NameElem))){ IsError=true; return false; }
 ECiy->Znach=Ciy; ECiy->K=K;
 PrintForm(false,OPRFILE,ECiy->Name,ECiy->Znach); fputws(L"<br>\n",OPRFILE);
 PrintForm(false,OPRFILE,ECiy->K->Name,ECiy->K->Znach); fputws(L"<br>\n",OPRFILE);
 swprintf(NameCord,L"dgama@%s",Name);
 if(!(K=TakeVary(NameCord))){ IsError=true; return false; } K->Znach=dG;
 swprintf(NameElem,L"C.%s",NameCord);
 if(!(ECk=TakeElem(NameElem))){ IsError=true; return false; }
 ECk->Znach=Ck; ECk->K=K;
 PrintForm(false,OPRFILE,ECk->Name,ECk->Znach); fputws(L"<br>\n",OPRFILE);
 PrintForm(false,OPRFILE,ECk->K->Name,ECk->K->Znach); fputws(L"<br>\n",OPRFILE);
 swprintf(NameCord,L"dx@%s",Name);
 if(!(K=TakeVary(NameCord))){ IsError=true; return false; } K->Znach=dX;
 swprintf(NameElem,L"C.%s",NameCord);
 if(!(ECx=TakeElem(NameElem))){ IsError=true; return false; }
 ECx->Znach=Cx; ECx->K=K;
 PrintForm(false,OPRFILE,ECx->Name,ECx->Znach); fputws(L"<br>\n",OPRFILE);
 PrintForm(false,OPRFILE,ECx->K->Name,ECx->K->Znach); fputws(L"<br>\n",OPRFILE);
 swprintf(NameCord,L"dy@%s",Name);
 if(!(K=TakeVary(NameCord))){ IsError=true; return false; } K->Znach=dY;
 swprintf(NameElem,L"C.%s",NameCord);
 if(!(ECy=TakeElem(NameElem))){ IsError=true; return false; }
 ECy->Znach=Cy; ECy->K=K;
 PrintForm(false,OPRFILE,ECy->Name,ECy->Znach); fputws(L"<br>\n",OPRFILE);
 PrintForm(false,OPRFILE,ECy->K->Name,ECy->K->Znach); fputws(L"<br>\n",OPRFILE);
 swprintf(NameCord,L"dz@%s",Name);
 if(!(K=TakeVary(NameCord))){ IsError=true; return false; } K->Znach=dZ;
 swprintf(NameElem,L"C.%s",NameCord);
 if(!(ECz=TakeElem(NameElem))){ IsError=true; return false; }
 ECz->Znach=Cz; ECz->K=K;
 PrintForm(false,OPRFILE,ECz->Name,ECz->Znach); fputws(L"<br>\n",OPRFILE);
 PrintForm(false,OPRFILE,ECz->K->Name,ECz->K->Znach); fputws(L"<br>\n",OPRFILE);
 return true;
}
//---------------------------------------------------------------------------
bool Stringers(Stringer *T){ Root *I;
 OTLADKA=FindInst(L"ОТЛАДКА",&I); Stringer *t,*bb; Inf[0]='\0';
 if(OTLADKA&&!_waccess(L"OTLADKA",0)) _wunlink(L"OTLADKA");
 CoordWithoutDT(L);
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
 else OPRFILE=_wfopen(OprFile,L"a");
 if(OTLADKA){
  FILE *F=_wfopen(L"Otladka",L"w"); Fputs(L"Отладочная информация\n",F); fclose(F);
 }
 for(t=T;t;t=t->Sled){
  fwprintf(OPRFILE,L"<h3 align=\"center\"><font color=\"#800000\">"
   L"Объект УПРУГИЙ СТЕРЖЕНЬ с именем \"%s\" строки %d сгенерировал элементы и формулы:"
   L"</font></h3>\n",t->Name,t->Line);
  if(!t->Fill(t->Name)) {
   IsError=true; return false;
  }
  if(!t->Elems()){
   IsError=true; return false;
 }}
 fclose(OPRFILE);
 return true;
}
//---------------------------------------------------------------------------
bool Solids(Parm *Par,TRichEdit *RichEditOpr,TCanvas *Canvas){   /* ТВЕРДЫЕ ТЕЛА */
//Обработка оператора ТВЕРДЫЕ ТЕЛА заключается в содании списка объектов
//"ТВЕРДОЕ ТЕЛО" и заполнении их внутреннего списка параметров параметрами
//из списка оператора. Список будет обработан при обработке L"ТВЕРДОГО ТЕЛА"
 Body *b; Parm *p; int Line=0;
 for(Root *o=L->Oper;o;o=o->Sled) if(o->First==Par){ Line=o->Line; break; }
 for(Parm *P=Par;P;P=P->Sled){
  if(!P->Nam||!P->R) goto Err;
  CleanStr(P->Nam,Inf);
  if(b=TakeBody(Inf,wcschr(Inf,'~'))){
   b->Line=Line;
   if(*P->R->Nam=='J'){
    b->KoordSys=P->R->Sled->R; P->R->Sled=NULL; b->Inert=P->R;
   }
   else{
    b->KoordSys=P->R;
    for(p=P->R;p;p=p->Sled){
     if(*p->Sled->Nam=='J') break;
     else if(*p->Sled->Nam=='m'){
      swprintf(Inf,L"Список инерционных параметров в операторе строки %d\n"
       L"должен быть дан в скобках с заголовком \'J\'!",Line);
      Error(Line,0,true); goto Err;
    }}
    if(p){ b->Inert=p->Sled->R; p->Sled=p->Sled->Sled; }
   }
   for(Parm *P=b->KoordSys;P;P=P->Sled){
    if(P->R){
     for(Parm *p=P->R;p;p=p->Sled){ Cnst *C; Vary *V;
      if(!FindCnst(*p->Nam=='-'?p->Nam+1:p->Nam,&C)){
       V=TakeVary(*p->Nam=='-'?p->Nam+1:p->Nam);
       if(!(V->Line)) V->Line=Line;
  }}}}}
  else{
   swprintf(Inf,L"Нельзя создать объект \"Твёрдое тело\" \n"
    L"в операторе строки %d)!",Line);
   Error(Line,0,true); goto Err;
 }}
 return true;
Err:
 swprintf(Inf,L"Описание твердого тела - это запись вида:\n"
  L"ТВЕРДЫЕ ТЕЛА или ТВЕРДОЕ ТЕЛО или ТВЁРДЫЕ ТЕЛА или ТВЁРДОЕ ТЕЛО :\n"
  L"<Имя>[~<Имя базы>](<Сдвиги и повороты>,J(<Параметры инерции>));\n"
  L"где <Имя> и <Имя базы> - имена тел - текущего и \"базового\";\n"
  L"<Сдвиги и повороты> - список элементов в нужном порядке вида\n"
  L"Rx(угол поворота вокруг оси х),Ry(угол поворота вокруг оси y),\n"
  L"Rz(угол поворота вокруг оси z),Sx(сдвиг вдоль оси х),\n"
  L"Sy(сдвиг вдоль оси y),Sz(сдвиг вдоль оси z);\n"
  L"Вмместо последоватеьности поворотов - Rx(...),Ry(...),Rz(...)\n"
  L"могут быть указаны параметры Родрига-Гамильтона в виде:\n"
  L" Q(p0,p1,p2,p3), где p0,p1,p2,p3 - переменные, обозначающие эти параметры;"
  L"<Параметры инерции> - список необходимых инерционных параметров вида\n"
  L"m(масса), Jx(момент инерции относительно оси x), Jy(момент инерции\n"
  L"относительно оси y), Jz(момент инерции относительно оси z).\n"
  L"Здесь в скобках стоят имена переменных или числа. Буква 'J' обязательна!\n");
 Error(Line,0,true);
 return false;
}
//---------------------------------------------------------------------------
bool ElasticRods(Parm *Par,TRichEdit *RichEditOpr,TCanvas *Canvas){   /* УПРУГИЕ СТЕРЖНИ */
//Обработка оператора УПРУГИЕ СТЕРЖНИ заключается в создании списка объектов
//"УПРУГИЙ СТЕРЖЕНЬ" и заполнении их внутреннего списка параметров параметрами
//из списка оператора. Список будет обработан при обработке L"УПРУГОГО СТЕРЖНЯ"
 Stringer *r; Parm *p; int Line=0; wchar_t Nam[256];
 for(Root *o=L->Oper;o;o=o->Sled) if(o->First==Par){ Line=o->Line; break; }
 for(;Par;Par=Par->Sled){
  if(!Par->Nam||!Par->R) goto Err;
  CleanStr(Par->Nam,Nam);
  if(r=TakeStringer(Nam)){
   r->Line=Line;
   for(Parm *P=r->Parms=Par->R;P;P=P->Sled){
    if(P->R){
     for(Parm *p=P->R;p;p=p->Sled){ Cnst *C; Vary *V;
      if(!FindCnst(*p->Nam=='-'?p->Nam+1:p->Nam,&C)){
       V=TakeVary(*p->Nam=='-'?p->Nam+1:p->Nam);
       if(!(V->Line)) V->Line=Line;
  }}}}}
  else{
   swprintf(Inf,L"Нельзя создать объект \"Упругий стержень\"\n"
    L"в операторе строки %d)!",Line);
   Error(Line,0,true); goto Err;
 }}
 return true;
Err:
 swprintf(Inf,L"Описание стержня - это запись вида:\n"
  L"УПРУГИЕ СТЕРЖНИ или УПРУГИЙ СТЕРЖЕНЬ : L"\
  L"<Имя> ( <Параметры>,<Концы> );\n"
  L"где <Имя> - имя стержня; <Параметры> - список физических и\n"
  L"геометрических параметров стержня вида - E(модуль упругости),\n"
  L"G(модуль сдвига), L(длина), F(площадь поперечного сечения),\n"
  L"Jx(Ix), Jy(Iy), Jz(Iz) - геометрические моменты инерции сечения\n"
  L"относительно, соответственно, двух осей изгиба и продольной оси.\n"
  L"<Концы> - список декартовых координат центров, углов поворота\n"
  L"(или параметров Родрига-Гамильтона) левого и правого концов вида\n"
  L"S(3 координаты левого и 3 координаты правого конца),\n"
  L"R(3 угла левого и 3 угла правого сечения) или\n"
  L"Q(4 параметра Родрига-Гамильтона для левого и 4 - для правого конца);\n"
  L"В скобках для списка <Концы> стоят только имена переменных,\n"
  L"для списка <Параметры> - могут стоять имена переменных или числа.");
 Error(Line,0,true);
 return false;
}
//---------------------------------------------------------------------------
bool Quaternion(Parm *Par,TRichEdit *RichEditOpr,TCanvas *Canvas){   /* Кватернион */
//Обработка оператора КВАТЕРНИОН заключается в формировании
//формул для параметров Родрига-Гамильтона конечного поворота,
//эквивалентного заданной последовательности поворотов СК, а также
//(при необходимости) углов Эйлера, соответствующих этому повороту
 Parm *p; int Line=0; wchar_t Nam[256];
 Vary *p0=NULL,*p1=NULL,*p2=NULL,*p3=NULL,*psi=NULL,*teta=NULL,*fi=NULL,*V;
 Form Fp0,Fp1,Fp2,Fp3,Fip0,Fip1,Fip2,Fip3,FP0,FP1,FP2,FP3;
 Fp0.C=Odin; Fp1.C=Fp2.C=Fp3.C=Nul; Fip1.C=Fip2.C=Fip3.C=Nul;
 for(Root *o=L->Oper;o;o=o->Sled) if(o->First==Par){ Line=o->Line; break; }
 if(!SLO){
  SLO=new TStringList;
  SLO->Add(L"<html>");
  SLO->Add(L"<head>");
  SLO->Add(L"<title>ПРОТОКОЛ ОПЕРАТОРОВ</title>");
  SLO->Add(L"<meta http-equiv=\"Content-Type\" "
   L"content=\"text/html;");
  SLO->Add(L"char_set=windows-1251\">");
  SLO->Add(L"</head>");
  swprintf(Sopr,L"<body bgcolor=\"#FFFFFF\" text=\"#800000\""
   L" background=\"%s/fon2.jpg\""
   L" font face=\"Times New Roman, Times, serif\">",WORKDIR);
  SLO->Add(Sopr); Sopr[0]='\0';
 }
// if(_waccess(OprFile,0)){
//  if(OPRFILE=_wfopen(OprFile,L"w"))
//   fwprintf(OPRFILE,
//	L"<html>\n<head>\n<title>ПРОТОКОЛ ОПЕРАТОРОВ</title>\n"
//	L"<meta http-equiv=\"Content-Type\" content=\"text/html;"
//	L"wchar_tset=windows-1251\">\n</head>\n"
//	L"<body bgcolor=\"#FFFFFF\" text=\"#800000\""
//	L" background=\"%s/fon2.jpg\""
//	L" font face=\"Times New Roman, Times, serif\">\n",WORKDIR);
// }
// else OPRFILE=_wfopen(OprFile,L"a");
 for(;Par;Par=Par->Sled){
  if(!Par->Nam||!Par->R){
   swprintf(Inf,L"В операторе \"КВАТЕРНИОН\" строки %s все параметры\n"
	L"должны вводится в скобках, перед которыми должно\n"
	L"стоять \"P\", \"Q\", \"Rx\", \"Ry\" или \"Rz\" (см.дальше)",Line);
   Error(Line,0,true); goto Err;
  }
  CleanStr(Par->Nam,Nam);
  switch(Nam[0]){
   case 'P':
	if(KolElem(Par->R)!=4){
	 swprintf(Inf,
	  L"В операторе \"КВАТЕРНИОН\" строки %s надо указать\n"
	  L"4 параметра Родрига-Гамильтона базового тела (см.дальше)",Line);
	 Error(Line,0,true); goto Err;
	}
	p=Par->R;
	if(!p->Nam) goto Err; Fp0.V=TakeVary(p->Nam); p=p->Sled;
	if(!p->Nam) goto Err; Fp1.V=TakeVary(p->Nam); p=p->Sled;
	if(!p->Nam) goto Err; Fp2.V=TakeVary(p->Nam); p=p->Sled;
	if(!p->Nam) goto Err; Fp3.V=TakeVary(p->Nam);
	break;
   case 'Q':
	if(Nam[1]){
	 swprintf(Inf,
	  L"В операторе \"КВАТЕРНИОН\" строки %s параметры Родрига-Гамильтона\n"
	  L"базового тела надо указать в скобках с наименованием \"P\" (см.дальше)",Line);
	 Error(Line,0,true); goto Err;
	}
	if(KolElem(Par->R)!=4){
	 swprintf(Inf,
	  L"В операторе \"КВАТЕРНИОН\" строки %s надо указать\n"
	  L"4 параметра Родрига-Гамильтона (см.дальше)",Line);
	 Error(Line,0,true); goto Err;
	}
	p=Par->R;
	if(!p->Nam) goto Err; p0=TakeVary(p->Nam); p=p->Sled;
	if(!p->Nam) goto Err; p1=TakeVary(p->Nam); p=p->Sled;
	if(!p->Nam) goto Err; p2=TakeVary(p->Nam); p=p->Sled;
	if(!p->Nam) goto Err; p3=TakeVary(p->Nam);
	break;
   case 'R':
	if(!p0){
	 swprintf(Inf,
	  L"В операторе \"КВАТЕРНИОН\" строки %d надо записать\n"
	  L"сначала инструкцию с обозначением параметров Родрига-\n"\
	  L"Гамильтона, а затем последовательность поворотов",Line);
	 Error(Line,0,true); goto Err;
	}
	if((Nam[1]!='x')&&(Nam[1]!='y')&&(Nam[1]!='z')){
	  swprintf(Inf,
	   L"В операторе \"КВАТЕРНИОН\" строки %s ось поворота ука-\n"
	   L"зывается второй буквой ('x','y' или 'z') после буквы 'R'",Line);
	  Error(Line,0,true); goto Err;
	}
	TakeCoSinFi2(Par->R); Fip0=cosFi2;
	switch(Nam[1]){
	 case 'x': Fip1=sinFi2; Fip2.C=Nul;  Fip3.C=Nul;  break;
	 case 'y': Fip1.C=Nul;  Fip2=sinFi2; Fip3.C=Nul;  break;
	 case 'z': Fip1.C=Nul;  Fip2.C=Nul;  Fip3=sinFi2; break;
	}
//if(OTLADKA&&OPRFILE){
// fwprintf(OPRFILE,L"Перемножаем кватернионы:<hr>\n(L");
// PrintForm(false,OPRFILE,NULL,Fp0); fputws(L", ",OPRFILE);
// PrintForm(false,OPRFILE,NULL,Fp1); fputws(L", ",OPRFILE);
// PrintForm(false,OPRFILE,NULL,Fp2); fputws(L", ",OPRFILE);
// PrintForm(false,OPRFILE,NULL,Fp3); fputws(L") * (L",OPRFILE);
// PrintForm(false,OPRFILE,NULL,Fip0); fputws(L", ",OPRFILE);
// PrintForm(false,OPRFILE,NULL,Fip1); fputws(L", ",OPRFILE);
// PrintForm(false,OPRFILE,NULL,Fip2); fputws(L", ",OPRFILE);
// PrintForm(false,OPRFILE,NULL,Fip3); fputws(L")\n<br>",OPRFILE);
//}
if(OTLADKA&&SLO){
 SLO->Add(L"Перемножаем кватернионы:<hr>(");
 sopr=FormToStringList(Fp0,NULL,Sopr,sopr,SLO,false,&SF);
 sopr+=swprintf(sopr,L", ");
 sopr=FormToStringList(Fp1,NULL,Sopr,sopr,SLO,false,&SF);
 sopr+=swprintf(sopr,L", ");
 sopr=FormToStringList(Fp2,NULL,Sopr,sopr,SLO,false,&SF);
 sopr+=swprintf(sopr,L", ");
 sopr=FormToStringList(Fp3,NULL,Sopr,sopr,SLO,false,&SF);
 sopr+=swprintf(sopr,L", ");
 sopr=FormToStringList(Fip0,NULL,Sopr,sopr,SLO,false,&SF);
 sopr+=swprintf(sopr,L", ");
 sopr=FormToStringList(Fip1,NULL,Sopr,sopr,SLO,false,&SF);
 sopr+=swprintf(sopr,L", ");
 sopr=FormToStringList(Fip2,NULL,Sopr,sopr,SLO,false,&SF);
 sopr+=swprintf(sopr,L", ");
 sopr=FormToStringList(Fip3,NULL,Sopr,sopr,SLO,false,&SF);
 sopr+=swprintf(sopr,L")");
 SLO->Add(Sopr); Sopr[0]='\0'; sopr=Sopr;
 SLO->Add(L"<br>");
}
	p0->Znach=Fp0*Fip0-Fp1*Fip1-Fp2*Fip2-Fp3*Fip3;
	p1->Znach=Fp0*Fip1+Fp1*Fip0+Fp2*Fip3-Fp3*Fip2;
	p2->Znach=Fp0*Fip2+Fp2*Fip0+Fp3*Fip1-Fp1*Fip3;
	p3->Znach=Fp0*Fip3+Fp3*Fip0+Fp1*Fip2-Fp2*Fip1;
	Fp0=p0->Znach; Fp1=p1->Znach; Fp2=p2->Znach; Fp3=p3->Znach;
//if(OTLADKA&&OPRFILE){
// fwprintf(OPRFILE,L"Получаем кватернион:<hr>\n(L");
// PrintForm(false,OPRFILE,NULL,p0->Znach); fputws(L", ",OPRFILE);
// PrintForm(false,OPRFILE,NULL,p1->Znach); fputws(L", ",OPRFILE);
// PrintForm(false,OPRFILE,NULL,p2->Znach); fputws(L", ",OPRFILE);
// PrintForm(false,OPRFILE,NULL,p3->Znach); fputws(L")<br>",OPRFILE);
// fputws(L"<br>",OPRFILE);
//}
if(OTLADKA&&SLO){
 SLO->Add(L"Получаем кватернион:<hr>(");
 sopr=FormToStringList(p0->Znach,NULL,Sopr,sopr,SLO,false,&SF);
 sopr+=swprintf(sopr,L", ");
 sopr=FormToStringList(p1->Znach,NULL,Sopr,sopr,SLO,false,&SF);
 sopr+=swprintf(sopr,L", ");
 sopr=FormToStringList(p2->Znach,NULL,Sopr,sopr,SLO,false,&SF);
 sopr+=swprintf(sopr,L", ");
 sopr=FormToStringList(p3->Znach,NULL,Sopr,sopr,SLO,false,&SF);
 sopr+=swprintf(sopr,L")");
 SLO->Add(Sopr); Sopr[0]='\0'; sopr=Sopr;
 SLO->Add(L"<br>");
}
	break;
   case 'E':
	if(KolElem(Par->R)!=3){
	 swprintf(Inf,
	  L"В операторе \"КВАТЕРНИОН\" строки %s надо указать\n"
	  L"3 переменных - угла Эйлера (см.дальше)",Line);
	 Error(Line,0,true); goto Err;
	}
	p=Par->R;
	if(!p->Nam) goto Err; psi =TakeVary(p->Nam); p=p->Sled;
	if(!p->Nam) goto Err; teta=TakeVary(p->Nam); p=p->Sled;
	if(!p->Nam) goto Err; fi  =TakeVary(p->Nam);
	break;
   default:
	swprintf(Inf,
	 L"В операторе \"КВАТЕРНИОН\" строки %s\n"
	 L"списки параметров Родрига-Гамильтона\n"
	 L"и углы поворотов вокруг осей озаглав-\n"
	 L"ливаются либо буквой 'Q' либо буквами\n"
	 L"\"Rx\",\"Ry\" или \"Rz\")",Line);
	Error(Line,0,true); goto Err;
 }}
/* if(OPRFILE){
  fwprintf(OPRFILE,
   L"<h3 align=\"center\"><font color=\"#800000\">"
   L"Оператор \"КВАТЕРНИОН\" строки %d сгенерировал\n"
   L"формулы для параметров Родрига-Гамильтона в виде:</font></h3>\n",Line);
  PrintForm(false,OPRFILE,p0->Name,p0->Znach); fputws(L"<br>",OPRFILE);
  PrintForm(false,OPRFILE,p1->Name,p1->Znach); fputws(L"<br>",OPRFILE);
  PrintForm(false,OPRFILE,p2->Name,p2->Znach); fputws(L"<br>",OPRFILE);
  PrintForm(false,OPRFILE,p3->Name,p3->Znach);
  if(psi&&teta&&fi){
   fwprintf(OPRFILE,
	L"<br><h3 align=\"center\"><font color=\"#800000\">"
	L"И формулы для углов Эйлера в виде:</font></h3>\n",Line);
   Fp0.V=p0; Fp1.V=p1; Fp2.V=p2; Fp3.V=p3;
   psi->Znach =arctg(Fp3/Fp0)+arctg(Fp2/Fp1);
   fi->Znach  =arctg(Fp3/Fp0)-arctg(Fp2/Fp1);
   teta->Znach=2*arccos(sqrt(Fp0*Fp0+Fp3*Fp3));
   PrintForm(false,OPRFILE, psi->Name, psi->Znach); fputws(L"<br>",OPRFILE);
   PrintForm(false,OPRFILE,teta->Name,teta->Znach); fputws(L"<br>",OPRFILE);
   PrintForm(false,OPRFILE,  fi->Name,  fi->Znach); fputws(L"<br>",OPRFILE);
  }
  fputws(L"<hr>",OPRFILE);
  fclose(OPRFILE);
 }*/
 if(SLO){
  swprintf(Sopr,L"<h3 align=\"center\"><font color=\"#800000\">"
   L"Оператор \"КВАТЕРНИОН\" строки %d сгенерировал",Line);
  SLO->Add(Sopr);
  SLO->Add(L"формулы для параметров Родрига-Гамильтона в виде:"
   L"</font></h3>");
  sopr=FormToStringList(p0->Znach,p0->Name,Sopr,sopr,SLO,true,&SF);
  sopr+=swprintf(sopr,L"<br>");
  SLO->Add(Sopr); sopr=Sopr;
  sopr=FormToStringList(p1->Znach,p1->Name,Sopr,sopr,SLO,true,&SF);
  sopr+=swprintf(sopr,L"<br>");
  SLO->Add(Sopr); sopr=Sopr;
  sopr=FormToStringList(p2->Znach,p2->Name,Sopr,sopr,SLO,true,&SF);
  sopr+=swprintf(sopr,L"<br>");
  SLO->Add(Sopr); sopr=Sopr;
  sopr=FormToStringList(p3->Znach,p3->Name,Sopr,sopr,SLO,true,&SF);
  sopr+=swprintf(sopr,L"<br>");
  SLO->Add(Sopr); sopr=Sopr;
  if(psi&&teta&&fi){
   swprintf(Sopr,L"<h3 align=\"center\"><font color=\"#800000\">"
	L"И формулы для углов Эйлера в виде:</font></h3>",Line);
   Fp0.V=p0; Fp1.V=p1; Fp2.V=p2; Fp3.V=p3;
   psi->Znach =arctg(Fp3/Fp0)+arctg(Fp2/Fp1);
   fi->Znach  =arctg(Fp3/Fp0)-arctg(Fp2/Fp1);
   teta->Znach=2*arccos(sqrt(Fp0*Fp0+Fp3*Fp3));
   sopr=FormToStringList(psi->Znach,psi->Name,Sopr,sopr,SLO,false,&SF);
   sopr+=swprintf(sopr,L"<br>");
   SLO->Add(Sopr); sopr=Sopr;
   sopr=FormToStringList(teta->Znach,teta->Name,Sopr,sopr,SLO,false,&SF);
   sopr+=swprintf(sopr,L"<br>");
   SLO->Add(Sopr); sopr=Sopr;
   sopr=FormToStringList(fi->Znach,fi->Name,Sopr,sopr,SLO,false,&SF);
   sopr+=swprintf(sopr,L"<br>");
   SLO->Add(Sopr); sopr=Sopr;
  }
  SLO->Add(L"<hr>");
 }
 return true;
Err:
 swprintf(Inf,
  L"Оператор описания конечного поворота для получения соответствующего ему\n"
  L"кватерниона - это запись вида:\n"
  L"    КВАТЕРНИОН : [<БАЗА>,]<ПАРАМЕТРЫ>,<ПОВОРОТЫ>[, E (psi, teta, fi)];\n"
  L"где\n"
  L"<БАЗА> - необязательная часть - список обозначений параметров\n"
  L"Родрига-Гамильтона - компонент кватерниона, задающего конечный поворот\n"
  L"от неподвижной системы координат базового тела. Относительно этого тела\n"
  L"далее указываются повороты данного тела, кватернион которого определяется\n"
  L"этим оператором. Параметры Родрига-Гамильтона записываются в круглых\n"
  L"скобках в обычном порядке - сначала скалярный параметр, затем составляющие\n"
  L"векторной части, перед скобками должно стоять \"P\". Если повороты прово-\n"
  L"дятся относительно абсолютной СК, то такой список не приводится;\n"
  L"<ПАРАМЕТРЫ> - обязательная часть - список обозначений параметров Родрига-\n"
  L"Гамильтона - компонент искомого кватерниона, записанных в круглых скобках\n"
  L"в обычном порядке - сначала скалярный параметр, затем составляющие векторной\n"
  L"части, перед скобками должна стоять буква 'Q';\n"
  L"<ПОВОРОТЫ> - обязательная часть - список элементов вида\n"
  L"           Rt(<константа или переменная>)\n"
  L" - здесь вместо t может быть 'x', 'y' или 'z', что показывает вокруг какой\n"
  L"оси надо повернуть СК, а <константа или переменная> - числовое (в градусах)\n"
  L"или формульное (указанием имени переменной) значение угла этого поворота.\n"
  L"E (psi, teta, fi) - необязательная часть - список переменных, обозначающих\n"
  L"углы Эйлера - прецесии, нутации и собственного вращения, формулы для которых\n"
  L"через параметры Родрига-Гамильтона формируемого кватерниона будут получены\n"
  L"в этом операторе. Перед скобками должна стоять латинская прописная буква E\n"
  L"Например,\n\n"
  L"КВАТЕРНИОН : P(p0,p1,p2,p3), Q(q0,q1,q2,q3), Rx(90), Ry(alfa), E(P,T,F);");
 Error(Line,0,true);
 return false;
}
//---------------------------------------------------------------------------
bool LitlCount(Parm *Par,TRichEdit *RE,TCanvas *Canvas){//обработчик оператора СЧИТАТЬ МАЛЫМИ
 for(Parm *P=Par;P;P=P->Sled){
  if(P->R||!P->Nam){
   swprintf(Inf,L"В правой части оператора \"СЧИТАТЬ МАЛЫМИ\" (строка  № %d)\n"
	L"должен стоять простой список переменных,\nсчитающихся в задаче малыми",
	P->Line);
   Error(P->Line,0,true); return false;
 }}
// RichEdit=RE; Line=Par->Line;
 for(Vary *V=L->V;V;V=V->Sled)
  if(!IsCnst(V->Znach))
   V->Znach=ReplaceLitlVary(V->Znach,Par);
 return true;
}
//---------------------------------------------------------------------------
void PrintOK(wchar_t *File){
 FILE *F=_wfopen(File,L"a"); Cord *K; int i;
 fwprintf(F,L"Неопределенные параметры:\n");
 for(K=L->K,i=0;K;K=K->Sled,i++){
  if(K->V->Znach.C!=Pust) continue;
  if(i>10){ i=0; fputwc('\n',F); }
  fwprintf(F,StrWINtoDOS(K->V->Name));
  if(K->Sled) fputws(L", ",F); else fputws(L";\n",F);
 }
 fclose(F);
}
//---------------------------------------------------------------------------

