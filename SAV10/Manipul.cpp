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
extern Cnst *Nul,*Odin,*Pust,*MinusOdin,*Sqrt2,*Halb;
extern char Inf[],OprFile[],WORKDIR[]; extern List *L; extern bool IsError;
extern bool OTLADKA; extern Form TLarin,*DsetaT,*DsetaTT;
static Form cosFi,sinFi,msinFi,Fi,Fit,Tkk_1[4][4],S[4][4],Sb[6][4][4],U[6][4][4],
 Fi2,cosFi2,sinFi2;
extern FILE *OPRFILE;
//---------------------------------------------------------------------------
// �������� ������� ������ ����������� ��������� �� ������ ������� ���,
//��������� ��������
//
// Name_i[~Name_i-1]|[Rx|y|z(a),][Sx|y|z(d),]...=
//                                [m(m_i),][Jx(Jix),][Jy(Jiy),][Jz(Jiz)]
//                                [,Jxy(Jixy)][,Jyz(Jiyz)][,Jxz(Jixz)];,
//
//��� Name_i, Name_i-1 - ����� �������� � "��������" ���,
//    Rx|y|z(a), Sx|y|z(d),... - ������������������ ��������� �� ���� "�"
//    ������ � �������� "d" ����� ���� ��������� ��� ��������������
//    ��������� � ����� i-1 �� ��� ���������� �� � ��, ��������� � ����� i;
//    m(m_i),Jx(Jix),Jy(Jiy),Jz(Jiz),Jxy(Jixy),Jyz(Jiyz),Jxz(Jixz) - �����������
//    � �������� ����� i-�� ����, ������� ����������� ������ � ������������
//    �������� ������� ���.
// ���� � �������� ������ ����� �������� ���������� "��������� ��", ��
//���������� ������ ���� ����� �������� � ��������� � ����� i ��, � ��������
//��������� ����������� ��������� ����� �������� �������� ������ ���� �� ���
//��������� ������� ���������, �����, �.�. ����� ���������� ���, ��
//���������� ������ ���� ����� �������� � ����������.
//---------------------------------------------------------------------------
void TakeCoSinFi(Parm *P){
 if(P->Nam){
  char *Name=P->Nam; if(*Name=='-') Name++;
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
void TakeCoSinFi2(Parm *P){
 if(P->Nam){
  Form Half; char *Name=P->Nam; if(*Name=='-') Name++; Half.C=Halb;
  Fi2.V=TakeVary(Name); Fi2.B=TakeBnop(3,Half,Fi2);
  cosFi2=cos(Fi2); sinFi2=*P->Nam=='-' ? -sin(Fi2) : sin(Fi2);
 }
 else{ Form F;
  if((P->Val==180.0)||(P->Val==-180.0)){ cosFi2.C=Nul; sinFi2.C=Odin; }
  else if(P->Val==90.0) cosFi2.C=sinFi2.C=Sqrt2;
  else if(P->Val==-90.0){ cosFi2.C=Sqrt2; sinFi2=-cosFi2; }
  else{
   F.C=TakeCnst(cos(P->Val*M_PI/360)); cosFi2=F;
   F.C=TakeCnst(sin(P->Val*M_PI/360)); sinFi2=F;
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
    sprintf(Inf,"���������� \"%s\" �� ������ �������� � �������� ������",Name);
    Application->MessageBox(Inf,"������",MB_OK); F.C=Pust;
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
    sprintf(Inf,"���������� \"%s\" �� ������ �������� � �������� ������",Name);
    Application->MessageBox(Inf,"������",MB_OK); F.C=Pust;
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
void MultTransMatrVect(Form M[4][4],Form V[4]){
 Form Rez[4]; int i,j;
 for(i=0;i<4;i++){
  Rez[i].C=Nul; for(j=0;j<4;j++) Rez[i]=Rez[i]+V[j]*M[j][i];
 }
 for(i=0;i<4;i++) V[i]=Rez[i];
}
//---------------------------------------------------------------------------
void MultMatrVect(Form M[4][4],Form V[4]){
 Form Rez[4]; int i,j;
 for(i=0;i<4;i++){
  Rez[i].C=Nul; for(j=0;j<4;j++) Rez[i]=Rez[i]+V[j]*M[i][j];
 }
 for(i=0;i<4;i++) V[i]=Rez[i];
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
void AddVect(Form A[4],Form B[4],Form C[4]){
 for(int i=0;i<4;i++) C[i]=A[i]+B[i];
}
//---------------------------------------------------------------------------
void MultMatrMatr(Form A[4][4],Form B[4][4],Form C[4][4]){
 int i,j,k;
 for(i=0;i<4;i++) for(j=0;j<4;j++){
  C[i][j].C=Nul;
  for(k=0;k<4;k++) C[i][j]=C[i][j]+B[k][j]*A[i][k];
}}
//---------------------------------------------------------------------------
void MultMatrMatr(Form A[4][4],Form B[4][4]){ Form C[4][4];
 MultMatrMatr(A,B,C);
 for(int i=0;i<4;i++) for(int j=0;j<4;j++) A[i][j]=C[i][j];
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
void PrintFormVect3(char *File,char *Comm,char *NameVect,Form V[3]){
 FILE *F=fopen(File,"a"); char Name[256];
 sprintf(Name,"\n%s\n",Comm); fputs(StrWINtoDOS(Name),F);
 for(int i=0;i<3;i++){
  sprintf(Name,"%s[%d]",NameVect,i);
  PrintForm(true,F,Name,V[i]); fputc('\n',F);
 }
 fclose(F);
}
//---------------------------------------------------------------------------
void PrintVect3(char *File,char *Comm,char *NameVect,Form V[3]){
 FILE *F=fopen(File,"a"); char Name[256];
 sprintf(Name,"\n%s",Comm); fputs(StrWINtoDOS(Name),F);
 for(int i=0;i<3;i++){
  sprintf(Name,"%s[%d]",NameVect,i);
  PrintForm(true,F,Name,V[i]);
 }
 fclose(F);
}
//---------------------------------------------------------------------------
void PrintFormVect4(char *File,char *Comm,char *NameVect,Form V[4]){
 FILE *F=fopen(File,"a"); char Name[256];
 sprintf(Name,"\n%s\n",Comm); fputs(StrWINtoDOS(Name),F);
 for(int i=0;i<4;i++){
  sprintf(Name,"%s[%d]",NameVect,i);
  PrintForm(true,F,Name,V[i]); fputc('\n',F);
 }
 fclose(F);
}
//---------------------------------------------------------------------------
void PrintVect4(char *File,char *Comm,char *NameVect,Form V[4]){
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
void PrintFormMatr(char *File,char *Comm,char *NameMatr,Form M[4][4]){
 FILE *F=fopen(File,"a"); char Name[256];
 sprintf(Name,"\n%s\n",Comm); fputs(StrWINtoDOS(Name),F);
 for(int i=0;i<4;i++){
  for(int j=0;j<4;j++){
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
 Application->MessageBox("���������� ��������� ����������� �����������!",
 "��������",MB_OK); return false;
}
//---------------------------------------------------------------------------
char *TestBody(Body *b,Magazine **M){
 Magazine *m; Body *bb;
 if(!b->BaseName) return NULL; //����� �� �����
//���� �� ���� ��� ���� ������ �� ����, ���������� ��� ��� - ��� ������������
 if(FindMagazine(*M,b->BaseName,&m)) return b->BaseName;
//����� ������, �������� � �������, ������� ��������� ���� � ��������� ���
 TakeMagazine(M,b->BaseName);
 FindBody(b->BaseName,&bb);
 return TestBody(bb,M);
}
//---------------------------------------------------------------------------
bool Accumulate(Body *B){
 Form wkk_1[4],rCkk_1[4],F,Fp0,Fp1,Fp2,Fp3,Fp0t,Fp1t,Fp2t,Fp3t; Body *b;
 int i,j,Ind; char *Name,Mes[512]; Vary *V,*p0,*p1,*p2,*p3,*p0t,*p1t,*p2t,*p3t;
//�������� ���������� ������� � ������� �������������� �� ������� k-1 � k
 for(i=0;i<4;i++){//��������� ��������
  Tkk_1[i][i].C=Odin; wkk_1[i].C=rCkk_1[i].C=Nul;
  for(j=i+1;j<4;j++) Tkk_1[i][j].C=Tkk_1[j][i].C=Nul;
 }
 for(Parm *P=B->KoordSys;P;P=P->Sled){
  switch(P->Nam[0]){ Parm *p,*pp,**Pp;
   case 'Q':
    if(KolElem(P->R)!=4){
     sprintf(Inf,"��������� �������-���������� ������ ���� ������� � ������\n"
      "������� ���� (������ %d) � ���������� 4-�!",B->Line);
     Error(B->Line,0,true); goto Err;
    }
    for(p=P->R;p;p=p->Sled) if(!p->Nam||p->R) goto Err;
    Fp0.V=p0=TakeVary(P->R->Nam); Fp3.V=p3=TakeVary(P->R->Sled->Sled->Sled->Nam);
    Fp1.V=p1=TakeVary(P->R->Sled->Nam); Fp2.V=p2=TakeVary(P->R->Sled->Sled->Nam);
    sprintf(Inf,"%s'%s",p0->Name,L->Time->Name); Fp0t.V=p0t=TakeVary(Inf);
    sprintf(Inf,"%s'%s",p1->Name,L->Time->Name); Fp1t.V=p1t=TakeVary(Inf);
    sprintf(Inf,"%s'%s",p2->Name,L->Time->Name); Fp2t.V=p2t=TakeVary(Inf);
    sprintf(Inf,"%s'%s",p3->Name,L->Time->Name); Fp3t.V=p3t=TakeVary(Inf);
    if(FindParmUppStr(P,"������",&p)){//���������� �������� �������������� �����������
     Form Fwx,Fwy,Fwz; Root *I;
     sprintf(Inf,"omegax@%s",B->Name); Fwx.V=TakeVary(Inf);
     sprintf(Inf,"omegay@%s",B->Name); Fwy.V=TakeVary(Inf);
     sprintf(Inf,"omegaz@%s",B->Name); Fwz.V=TakeVary(Inf);
     p0t->Znach=-0.5*(Fp1*Fwx+Fp2*Fwy+Fp3*Fwz);
     p1t->Znach= 0.5*(Fp0*Fwx-Fp3*Fwy+Fp2*Fwz);
     p2t->Znach= 0.5*(Fp3*Fwx+Fp0*Fwy-Fp1*Fwz);
     p3t->Znach=-0.5*(Fp2*Fwx-Fp1*Fwy-Fp0*Fwz);
     if(I=FindInstLast("��������������")){//��������� ��������� ����������
      for(p=I->First;p->Sled;p=p->Sled);
      p->Sled=(Parm *)calloc(1,sizeof(Parm)); p=p->Sled;
     }
     else{//������� ���������� "��������������"
      I=TakeInst("��������������"); I->Line=B->Line;
      I->First=p=(Parm *)calloc(1,sizeof(Parm));
     }
     p->Nam=strcpy((char *)calloc(strlen(Fwx.V->Name)+1,SzC),Fwx.V->Name);
     p->Sled=(Parm *)calloc(1,sizeof(Parm)); p=p->Sled;
     p->Nam=strcpy((char *)calloc(strlen(Fwy.V->Name)+1,SzC),Fwy.V->Name);
     p->Sled=(Parm *)calloc(1,sizeof(Parm)); p=p->Sled;
     p->Nam=strcpy((char *)calloc(strlen(Fwz.V->Name)+1,SzC),Fwz.V->Name);
    }
    else{//���������� ������ �������������� �����������
     wkk_1[0]=2*(Fp0*Fp1t-Fp1*Fp0t-Fp2*Fp3t+Fp3*Fp2t);
     wkk_1[1]=2*(Fp0*Fp2t-Fp2*Fp0t-Fp3*Fp1t+Fp1*Fp3t);
     wkk_1[2]=2*(Fp0*Fp3t-Fp3*Fp0t-Fp1*Fp2t+Fp2*Fp1t);
    }
    Tkk_1[0][0]=2*Fp0*Fp0+2*Fp1*Fp1-1; Tkk_1[0][1]=2*Fp1*Fp2-2*Fp0*Fp3;
    Tkk_1[0][2]=2*Fp1*Fp3+2*Fp0*Fp2;   Tkk_1[1][0]=2*Fp1*Fp2+2*Fp0*Fp3;
    Tkk_1[1][1]=2*Fp0*Fp0+2*Fp2*Fp2-1; Tkk_1[1][2]=2*Fp3*Fp2-2*Fp0*Fp1;
    Tkk_1[2][0]=2*Fp1*Fp3-2*Fp0*Fp2;   Tkk_1[2][1]=2*Fp3*Fp2+2*Fp0*Fp1;
    Tkk_1[2][2]=2*Fp0*Fp0+2*Fp3*Fp3-1;
    break;
   case 'R':
    TakeCoSinFi(P->R);
    switch(P->Nam[1]){
     case 'x':
//������� ������� ��� �������-�������
      for(i=0;i<3;i++){
       F=Tkk_1[i][1]*cosFi+Tkk_1[i][2]*sinFi;
       Tkk_1[i][2]=Tkk_1[i][1]*msinFi+Tkk_1[i][2]*cosFi;
       Tkk_1[i][1]=F;
      }
      if(OTLADKA){
       if(!P->R->Nam)
        sprintf(Inf,"Rx: ������� ������ ��� x �� ���� %g�",P->R->Val);
       else
        sprintf(Inf,"Rx: ������� ������ ��� x �� ���� %s",P->R->Nam);
       PrintFormMatr("Otladka",Inf,"Tkk_1",Tkk_1);
      }
//�������� ����������� ������� ��������
      F=wkk_1[1]*cosFi+wkk_1[2]*sinFi;
      wkk_1[2]=wkk_1[1]*msinFi+wkk_1[2]*cosFi;
      wkk_1[1]=F;
/*      if(P->R->Nam&&
        VassalOfKoord(L,TakeVary(*P->R->Nam=='-'?P->R->Nam+1:P->R->Nam))||
         FindInst("���������� ����������",&I)&&
         FindParm(I->First,P->R->Nam,&p)){//������� �� ���������� ����������*/
      if(P->R->Nam){
       sprintf(Inf,"%s'%s",P->R->Nam,L->Time->Name);
       Fit.V=TakeVary(Inf);
       wkk_1[0]=wkk_1[0]+Fit;
      }
      if(OTLADKA){
       if(P->R->Nam)
        sprintf(Inf,"�������� wkk_1 �� ���� %s ������ ��� x",P->R->Nam);
       else
        sprintf(Inf,"�������� wkk_1 �� ���� %0.1f ������ ��� x",P->R->Val);
       PrintFormVect4("Otladka",Inf,"wkk_1",wkk_1);
      }
      break;
     case 'y':
//������� ������� ��� �������-�������
      for(i=0;i<3;i++){
       F=Tkk_1[i][0]*cosFi+Tkk_1[i][2]*msinFi;
       Tkk_1[i][2]=Tkk_1[i][0]*sinFi+Tkk_1[i][2]*cosFi;
       Tkk_1[i][0]=F;
      }
      if(OTLADKA){
       if(!P->R->Nam)
        sprintf(Inf,"Ry: ������� ������ ��� y �� ���� %g�",P->R->Val);
       else
        sprintf(Inf,"Ry: ������� ������ ��� y �� ���� %s",P->R->Nam);
       PrintFormMatr("Otladka",Inf,"Tkk_1",Tkk_1);
      }
//�������� ����������� ������� ��������
      F=wkk_1[0]*cosFi+wkk_1[2]*msinFi;
      wkk_1[2]=wkk_1[0]*sinFi+wkk_1[2]*cosFi;
      wkk_1[0]=F;
/*      if(P->R->Nam&&
        VassalOfKoord(L,TakeVary(P->R->Nam))||
         FindInst("���������� ����������",&I)&&
         FindParm(I->First,P->R->Nam,&p)){//������� �� ���������� ����������*/
      if(P->R->Nam){
       sprintf(Inf,"%s'%s",P->R->Nam,L->Time->Name);
       Fit.V=TakeVary(Inf);
       wkk_1[1]=wkk_1[1]+Fit;
      }
      if(OTLADKA){
       if(P->R->Nam)
        sprintf(Inf,"�������� wkk_1 �� ���� %s ������ ��� y",P->R->Nam);
       else
        sprintf(Inf,"�������� wkk_1 �� ���� %0.1f ������ ��� y",P->R->Val);
       PrintFormVect4("Otladka",Inf,"wkk_1",wkk_1);
      }
      break;
     case 'z':
//������� ������� ��� �������-�������
      for(i=0;i<3;i++){
       F=Tkk_1[i][0]*cosFi+Tkk_1[i][1]*sinFi;
       Tkk_1[i][1]=Tkk_1[i][0]*msinFi+Tkk_1[i][1]*cosFi;
       Tkk_1[i][0]=F;
      }
      if(OTLADKA){
       if(!P->R->Nam)
        sprintf(Inf,"Rz: ������� ������ ��� z �� ���� %g�",P->R->Val);
       else
        sprintf(Inf,"Rz: ������� ������ ��� z �� ���� %s",P->R->Nam);
       PrintFormMatr("Otladka",Inf,"Tkk_1",Tkk_1);
      }
//�������� ����������� ������� ��������
      F=wkk_1[0]*cosFi+wkk_1[1]*sinFi;
      wkk_1[1]=wkk_1[0]*msinFi+wkk_1[1]*cosFi;
      wkk_1[0]=F;
/*      if(P->R->Nam&&
        VassalOfKoord(L,TakeVary(P->R->Nam))||
         FindInst("���������� ����������",&I)&&
         FindParm(I->First,P->R->Nam,&p)){//������� �� ���������� ����������*/
      if(P->R->Nam){
       sprintf(Inf,"%s'%s",P->R->Nam,L->Time->Name);
       Fit.V=TakeVary(Inf);
       wkk_1[2]=wkk_1[2]+Fit;
      }
      if(OTLADKA){
       if(P->R->Nam)
        sprintf(Inf,"�������� wkk_1 �� ���� %s ������ ��� z",P->R->Nam);
       else
        sprintf(Inf,"�������� wkk_1 �� ���� %0.1f ������ ��� z",P->R->Val);
       PrintFormVect4("Otladka",Inf,"wkk_1",wkk_1);
      }
      break;
     default:
      sprintf(Inf,"������ ������������ ������� ��������� �������\n"
       "������ ���� ����������� ����� �� ������� ����:\n"
       "Rx(����),Ry(����),Rz(����).\n"
       "� ������� ����� ����� ���������� ��� �����.");
//      Application->MessageBox(Mes,"��������",MB_OK);
//      IsError=true;
      Error(B->Line,0,true);
      return false;
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
      sprintf(Inf,"������ ����� ������� ������� ���� ������������ \"��������\"\n"
       "������ ���� ����������� ����� �� ������� ����:\n"
       "Sx(�����),Sy(�����),Sz(�����).\n"
       "� ������� ����� ����� ���������� ��� �����.");
      Error(B->Line,0,true);
//      Application->MessageBox(Mes,"��������",MB_OK);
//      IsError=true;
      return false;
    }
    for(i=0;i<3;i++) rCkk_1[i]=rCkk_1[i]+F*Tkk_1[i][Ind];
    if(OTLADKA) PrintFormVect4("Otladka","������� ������ �.�.","rCkk_1",rCkk_1);
    for(i=0;i<3;i++) Tkk_1[i][3]=Tkk_1[i][3]+F*Tkk_1[i][Ind];
    break;
   default:
    strcpy(Inf,P->Nam); LowToUpp(Inf);
    if(strstr(Inf,"������")==Inf) break;
Err:sprintf(Inf,"�������� �������� ���� - ��� ������ ����:\n"
     "<���>[~<��� ����>]|<������ � ��������>|<��������� �������>;\n"
     "��� <���> � <��� ����> - ����� ��� - �������� � \"��������\";\n"
     "<������ � ��������> - ������ ��������� � ������ ������� ����\n"
     "Rx(���� �������� ������ �),Ry(���� �������� ������ y),\n"
     "Rz(���� �������� ������ z),Sx(����� ����� �),Sy(����� ����� y),\n"
     "Sz(����� ����� z), ������ ����������������� ��������� (Rx(...),Ry(...),\n"
     "Rz(...))����� ���� ������� ��������� �������-���������� � ����:;\n"
     " Q(p0,p1,p2,p3), ��� p0,p1,p2,p3 - ����������, ������������ ��� ���������;"
     "<��������� �������> - ������ ����������� ����������� ���������� ����\n"
     "m(�����), Jx(������ ������� ������������ ��� x), Jy(������ �������\n"
     "������������ ��� y), Jz(������ ������� ������������ ��� z).\n"
     "����� � ������� ����� ����� ���������� ��� �����.");
    Error(B->Line,0,true);
//    Application->MessageBox(Mes,"��������",MB_OK);
//    IsError=true;
   return false;
 }}
//���������� ������-������ �� ����, ��� ������� �������� � ������� ��������������
 if(B->BaseName){
  FindBody(B->BaseName,&b);
  if(!b->Accum&&!Accumulate(b)) return false;
  MultMatrVect(b->Tk0,rCkk_1);
  AddVect(b->rCk,rCkk_1,B->rCk);
  for(i=0;i<3;i++) B->wkk[i]=b->wkk[i];
  MultTransMatrVect(Tkk_1,B->wkk);
  AddVect(B->wkk,wkk_1,B->wkk);
  MultMatrMatr(b->Tk0,Tkk_1,B->Tk0);
 }
 else{
  for(i=0;i<4;i++){
   B->rCk[i]=rCkk_1[i]; B->wkk[i]=wkk_1[i];
   for(j=0;j<4;j++) B->Tk0[i][j]=Tkk_1[i][j];
 }}
//������� 4.7.2007
 for(i=0;i<3;i++){ Parm *p;
  sprintf(Inf,"%cC@%s",i==2?'Z':i==1?'Y':'X',B->Name);
  if((!FindVary(Inf,&V)||(V->Znach.C==Pust))&&(V=TakeVary(Inf))) V->Znach=B->rCk[i];
  B->rCk[i].V=V;
  if(!FindParmUppStr(B->KoordSys,"������",&p)){//���������� �������� �������������� �����������
   sprintf(Inf,"omega%c@%s",i==2?'z':i==1?'y':'x',B->Name);
   if((!FindVary(Inf,&V)||(V->Znach.C==Pust))&&(V=TakeVary(Inf))) V->Znach=B->wkk[i];
   B->wkk[i].V=V;
 }}
//����� ������� 4.7.2007
 for(i=0;i<4;i++) for(j=0;j<4;j++){
  sprintf(Inf,"T%d%d@%s",i+1,j+1,B->Name);
  if(V=TakeVary(Inf)) V->Znach=B->Tk0[i][j];
 }
 if(OTLADKA){
  PrintFormVect4("Otladka",B->Name,"B->rCk",B->rCk);
  PrintFormVect4("Otladka","","B->wkk",B->wkk);
  PrintFormMatr("Otladka","","B->Tk0",B->Tk0);
 }
 B->Accum=true; return true;
}
//---------------------------------------------------------------------------
bool Accumulate2(Body *B){
 Form Sk_1k[4][4],wkk[4],rCkk[4],F,F1,F2; Body *b=NULL; int i,j,Ind;
 Parm *p; Root *I; char *Name,Mes[512];
//Sk_1k - ������� �������� �� �� k-1-�� ���� � �� k-��
//wkk - ������ ������� �������� k-�� ���� � ������. ��
//rCkk - ������ �� k-�� ���� � ������. ��
//�������� ����������������� ������� �������� �� ������� k-1 � k-�
//� ������� �� � ������� �������� � ��������� �� ������� ����������� ��� ����
 if(B->BaseName){
  FindBody(B->BaseName,&b);
  if(!b->Accum&&!Accumulate2(b)) return false;
 }
 for(i=0;i<3;i++){//��������� �������� - ��. ������� �������� �� �� k-1 � �� k,
                  //������-������ �� � ������� �������� k-1-�� ����
  Sk_1k[i][i].C=Odin; wkk[i].C=rCkk[i].C=Nul;
  if(b){
   rCkk[i]=b->rCk[i]; wkk[i]=b->wkk[i];
  }
  for(j=i+1;j<3;j++) Sk_1k[i][j].C=Sk_1k[j][i].C=Nul;
 }
 for(Parm *P=B->KoordSys;P;P=P->Sled){
  switch(P->Nam[0]){
   case 'R':
    TakeCoSinFi(P->R);
    switch(P->Nam[1]){
     case 'x':
//������� ������� ��� �������-�������
      for(i=0;i<3;i++){
       F1=Sk_1k[1][i]*cosFi+Sk_1k[2][i]*sinFi;
       F2=Sk_1k[1][i]*msinFi+Sk_1k[2][i]*cosFi;
       Sk_1k[1][i]=F1; Sk_1k[2][i]=F2;
      }
      if(OTLADKA) PrintFormMatr("Otladka","Rx","Sk_1k",Sk_1k);
//�������� ����������� ������-������
      F1=rCkk[1]*cosFi+rCkk[2]*sinFi;
      F2=rCkk[1]*msinFi+rCkk[2]*cosFi;
      rCkk[1]=F1; rCkk[2]=F2;
      if(OTLADKA){
       if(P->R->Nam)
        sprintf(Inf,"�������� rCkk �� ���� %s ������ ��� x",P->R->Nam);
       else
        sprintf(Inf,"�������� rCkk �� ���� %0.1f ������ ��� x",P->R->Val);
       PrintFormVect4("Otladka",Inf,"rCkk",rCkk);
      }
//�������� ����������� ������� ��������
      F1=wkk[1]*cosFi+wkk[2]*sinFi;
      F2=wkk[1]*msinFi+wkk[2]*cosFi;
      wkk[1]=F1; wkk[2]=F2;
      if(P->R->Nam&&
        VassalOfKoord(L,TakeVary(*P->R->Nam=='-'?P->R->Nam+1:P->R->Nam))||
         FindInst("���������� ����������",&I)&&
         FindParm(I->First,P->R->Nam,&p)){//������� �� ���������� ����������
       sprintf(Inf,"%s'%s",P->R->Nam,L->Time->Name);
       Fit.V=TakeVary(Inf);
       wkk[0]=wkk[0]+Fit;
      }
      if(OTLADKA){
       if(P->R->Nam)
        sprintf(Inf,"�������� wkk �� ���� %s ������ ��� x",P->R->Nam);
       else
        sprintf(Inf,"�������� wkk �� ���� %0.1f ������ ��� x",P->R->Val);
       PrintFormVect4("Otladka",Inf,"wkk",wkk);
      }
      break;
     case 'y':
//������� ������� ��� �������-�������
      for(i=0;i<3;i++){
       F1=Sk_1k[0][i]*cosFi+Sk_1k[2][i]*msinFi;
       F2=Sk_1k[0][i]*sinFi+Sk_1k[2][i]*cosFi;
       Sk_1k[0][i]=F1; Sk_1k[2][i]=F2;
      }
      if(OTLADKA) PrintFormMatr("Otladka","Ry","Sk_1k",Sk_1k);
//�������� ����������� ������-������
      F1=rCkk[0]*cosFi+rCkk[2]*msinFi;
      F2=rCkk[0]*sinFi+rCkk[2]*cosFi;
      rCkk[0]=F1; rCkk[2]=F2;
      if(OTLADKA){
       if(P->R->Nam)
        sprintf(Inf,"�������� rCkk �� ���� %s ������ ��� y",P->R->Nam);
       else
        sprintf(Inf,"�������� rCkk �� ���� %0.1f ������ ��� y",P->R->Val);
       PrintFormVect4("Otladka",Inf,"rCkk",rCkk);
      }
//�������� ����������� ������� ��������
      F1=wkk[0]*cosFi+wkk[2]*msinFi;
      F2=wkk[0]*sinFi+wkk[2]*cosFi;
      wkk[0]=F1; wkk[2]=F2;
      if(P->R->Nam&&
        VassalOfKoord(L,TakeVary(P->R->Nam))||
         FindInst("���������� ����������",&I)&&
         FindParm(I->First,P->R->Nam,&p)){//������� �� ���������� ����������
       sprintf(Inf,"%s'%s",P->R->Nam,L->Time->Name);
       Fit.V=TakeVary(Inf);
       wkk[1]=wkk[1]+Fit;
      }
      if(OTLADKA){
       if(P->R->Nam)
        sprintf(Inf,"�������� wkk �� ���� %s ������ ��� y",P->R->Nam);
       else
        sprintf(Inf,"�������� wkk �� ���� %0.1f ������ ��� y",P->R->Val);
       PrintFormVect4("Otladka",Inf,"wkk",wkk);
      }
      break;
     case 'z':
//������� ������� ��� �������-�������
      for(i=0;i<3;i++){
       F1=Sk_1k[0][i]*cosFi+Sk_1k[1][i]*sinFi;
       F2=Sk_1k[0][i]*msinFi+Sk_1k[1][i]*cosFi;
       Sk_1k[0][i]=F1; Sk_1k[1][i]=F2;
      }
      if(OTLADKA) PrintFormMatr("Otladka","Rz","Sk_1k",Sk_1k);
//�������� ����������� ������-������
      F1=rCkk[0]*cosFi+rCkk[1]*sinFi;
      F2=rCkk[0]*msinFi+rCkk[1]*cosFi;
      rCkk[0]=F1; rCkk[1]=F2;
      if(OTLADKA){
       if(P->R->Nam)
        sprintf(Inf,"�������� rCkk �� ���� %s ������ ��� z",P->R->Nam);
       else
        sprintf(Inf,"�������� rCkk �� ���� %0.1f ������ ��� z",P->R->Val);
       PrintFormVect4("Otladka",Inf,"rCkk",rCkk);
      }
//�������� ����������� ������� ��������
      F1=wkk[0]*cosFi+wkk[1]*sinFi;
      F2=wkk[0]*msinFi+wkk[1]*cosFi;
      wkk[0]=F1; wkk[1]=F2;
      if(P->R->Nam&&
        VassalOfKoord(L,TakeVary(P->R->Nam))||
         FindInst("���������� ����������",&I)&&
         FindParm(I->First,P->R->Nam,&p)){//������� �� ���������� ����������
       sprintf(Inf,"%s'%s",P->R->Nam,L->Time->Name);
       Fit.V=TakeVary(Inf);
       wkk[2]=wkk[2]+Fit;
      }
      if(OTLADKA){
       if(P->R->Nam)
        sprintf(Inf,"�������� wkk �� ���� %s ������ ��� z",P->R->Nam);
       else
        sprintf(Inf,"�������� wkk �� ���� %0.1f ������ ��� z",P->R->Val);
       PrintFormVect4("Otladka",Inf,"wkk",wkk);
      }
      break;
     default:
      sprintf(Inf,"������ ������������ ������� ��������� �������\n"
       "������ ���� ����������� ����� �� ������� ����:\n"
       "Rx(����),Ry(����),Rz(����).\n"
       "� ������� ����� ����� ��� ����� ����������, ��������, �� ������ \"-\".");
      Error(B->Line,0,true);
//      Application->MessageBox(Mes,"��������",MB_OK);
//      IsError=true;
     return false;
    }
    break;
   case 'S':
    if(P->R->Nam){
     Name=P->R->Nam; if(*Name=='-') Name++;
     F.V=TakeVary(Name); if(P->R->Nam[0]=='-') F=-F;
    }
    else F.C=TakeCnst(P->R->Val);
    switch(P->Nam[1]){
     case 'x': rCkk[0]=rCkk[0]+F; break;
     case 'y': rCkk[1]=rCkk[1]+F; break;
     case 'z': rCkk[2]=rCkk[2]+F; break;
     default:
      sprintf(Inf,"������ ����� ������� ������� ���� ������������ \"��������\"\n"
       "������ ���� ����������� ����� �� ������� ����:\n"
       "Sx(�����),Sy(�����),Sz(�����).\n"
       "� ������� ����� ����� ��� ����� ����������, ��������, �� ������ \"-\".");
      Error(B->Line,0,true);
//      Application->MessageBox(Mes,"��������",MB_OK);
//      IsError=true;
      return false;
    }
    if(OTLADKA) PrintFormVect4("Otladka","������� ������ �.�.","rCkk",rCkk);
    break;
   default:
    sprintf(Inf,"�������� �������� ���� - ��� ������ ����:\n"
     "<���>[~<��� ����>]|<������ � ��������>|<��������� �������>;\n"
     "��� <���> � <��� ����> - ����� ��� - �������� � \"��������\";\n"
     "<������ � ��������> - ������ ��������� � ������ ������� ����\n"
     "Rx(���� �������� ������ �),Ry(���� �������� ������ y),\n"
     "Rz(���� �������� ������ z),Sx(����� ����� �),Sy(����� ����� y),\n"
     "Sz(����� ����� z), ������ ����������������� ��������� (Rx(...),Ry(...),\n"
     "Rz(...))����� ���� ������� ��������� �������-���������� � ����:;\n"
     " Q(p0,p1,p2,p3), ��� p0,p1,p2,p3 - ����������, ������������ ��� ���������;"
     "<��������� �������> - ������ ����������� ����������� ���������� ����\n"
     "m(�����), Jx(������ ������� ������������ ��� x), Jy(������ �������\n"
     "������������ ��� y), Jz(������ ������� ������������ ��� z).\n"
     "����� � ������� ����� ����� ���������� ��� �����.");
    Error(B->Line,0,true);
//    Application->MessageBox(Mes,"��������",MB_OK);
//    IsError=true;
    return false;
 }}
//���������� ������-������ �� ����, ��� ������� �������� � ������� ��������
//������� 4.7.2007
 for(i=0;i<3;i++){ Vary *V;
  B->rCk[i]=rCkk[i];
/*  if(rCkk[i]!=Nul){
   sprintf(Inf,"%cC@%s",i==2?'Z':i==1?'Y':'X',B->Name);
   if(V=TakeVary(Inf)){ V->Znach=rCkk[i]; B->rCk[i].V=V; }
  }*/
  B->wkk[i]=wkk[i];
/*  if(wkk[i]!=Nul){
   sprintf(Inf,"omega%c@%s",i==2?'z':i==1?'y':'x',B->Name);
   if(V=TakeVary(Inf)){ V->Znach=wkk[i]; B->wkk[i].V=V; }
  }*/
 }
//����� ������� 4.7.2007
/* for(i=0;i<3;i++){ Vary *V;
  if(rCkk[i]!=Nul){
   B->rCk[i]=rCkk[i];
//   switch(rCkk[i].C->Atr){
//    case 3:
//     B->rCk[i].C=rCkk[i].C; break;
//    case 4:
//     B->rCk[i]=rCkk[i].V->Znach.C==Pust?rCkk[i]:rCkk[i].V->Znach; break;
//    case 5: if(rCkk[i].U->Func==1) B->rCk[i]=rCkk[i].V->Znach.C==Pust?rCkk[i]:rCkk[i].V->Znach; break;
//    default:
//     sprintf(Inf,"%cC@%s",i==2?'Z':i==1?'Y':'X',B->Name);
//     if(V=TakeVary(Inf)){ V->Znach=rCkk[i]; B->rCk[i].V=V; }
//   }
  }
  if(wkk[i]!=Nul){
   B->wkk[i]=wkk[i];
//   switch(wkk[i].C->Atr){
//    case 3: B->wkk[i].C=wkk[i].C; break;
//    case 4: B->wkk[i]=wkk[i].V->Znach.C==Pust?wkk[i]:wkk[i].V->Znach; break;
//    default:
//     sprintf(Inf,"omega%c@%s",i==2?'z':i==1?'y':'x',B->Name);
//     if(V=TakeVary(Inf)){ V->Znach=wkk[i]; B->wkk[i].V=V; }
//   }
//  }
 }*/
 if(OTLADKA){
  PrintFormVect4("Otladka",B->Name,"B->rCk",B->rCk);
  PrintFormVect4("Otladka","","B->wkk",B->wkk);
  PrintFormMatr("Otladka","","B->Tk0",B->Tk0);
 }
 B->Accum=true; return true;
}
//---------------------------------------------------------------------------
void RotationVector(char Os,char *Head,Form V[],Parm *P){
//������������� ��������� ������� � ����� ��
 Form F1,F2;
 switch(Os){
  case 'x':
   F1=V[1]*cosFi+V[2]*sinFi;  F2=V[1]*msinFi+V[2]*cosFi; V[1]=F1; V[2]=F2;
   break;
  case 'y':
   F1=V[0]*cosFi+V[2]*msinFi; F2=V[0]*sinFi+V[2]*cosFi;  V[0]=F1; V[2]=F2;
   break;
  case 'z':
   F1=V[0]*cosFi+V[1]*sinFi;  F2=V[0]*sinFi+V[1]*cosFi;  V[0]=F1; V[1]=F2;
   break;
 }
 if(OTLADKA){
  if(P->R->Nam)
   sprintf(Inf,"�������� %s �� ���� %s ������ ��� %c",Head,P->R->Nam,Os);
  else
   sprintf(Inf,"�������� %s �� ���� %0.1f ������ ��� %c",Head,P->R->Val,Os);
  PrintFormVect3("OTLADKA",Inf,Head,V);
}}
//---------------------------------------------------------------------------
bool Accumulate2v(Body *B){
 Form wkk[4],rCkk[4],F; Cord *K; Vary *V;
 Body *b=NULL; int i,j,Ind; Parm *p; Root *I; char *Name,Mes[512];
//wkk - ������ ������� �������� k-�� ���� � ������. ��
//rCkk - ������-������ �� k-�� ���� � ������. ��
//vCkk - ������ �������� �� k-�� ���� � ������. ��
//�������� ����������������� ������� ������������� ��������� �� ������� k-1 � k-�,
//������-������ ��, ������� �������� �� � ������� �������� k-�� ���� � ���������
//�� ������� ����������� ��� ������� ����
 if(B->BaseName){
  FindBody(B->BaseName,&b);
  if(!b->Accum&&!Accumulate2v(b)) return false;
  for(i=0;i<4;i++){//��������� �������� - ��. ������� �������������� �� �� k-1 � �� k,
                   //������-������ ��, ������ �������� �� � ������� �������� �����,
                   //�����. ��������� k-1-�� ���� ��� ���� ��� ������
   rCkk[i]=b->rCk[i]; wkk[i]=b->wkk[i];
   for(j=0;j<4;j++) Tkk_1[i][j]=b->Tk0[i][j];
 }}
 else{
  for(i=0;i<4;i++){//��������� �������� - ��. ������� �������������� �� �� k-1 � �� k,
                  //������-������ ��, ������ �������� �� � ������� �������� �����,
                  //�����. ��������� k-1-�� ���� ��� ���� ��� ������
   Tkk_1[i][i].C=Odin; rCkk[i].C=wkk[i].C=Nul;
   for(j=i+1;j<4;j++) Tkk_1[i][j].C=Tkk_1[j][i].C=Nul;
 }}
 for(Parm *P=B->KoordSys;P;P=P->Sled){
  switch(P->Nam[0]){
   case 'R':
    TakeCoSinFi(P->R);
    switch(P->Nam[1]){
     case 'x':
//������� ������� �������������� ���������
      for(i=0;i<3;i++){
       F=Tkk_1[i][1]*cosFi+Tkk_1[i][2]*sinFi;
       Tkk_1[i][2]=Tkk_1[i][1]*msinFi+Tkk_1[i][2]*cosFi;
       Tkk_1[i][1]=F;
      }
      if(OTLADKA){
       if(!P->R->Nam)
        sprintf(Inf,"Rx: ������� ������ ��� x �� ���� %g�",P->R->Val);
       else
        sprintf(Inf,"Rx: ������� ������ ��� x �� ���� %s",P->R->Nam);
       PrintFormMatr("OTLADKA",Inf,"Tkk_1",Tkk_1);
      }
//�������� ����������� ������-������ ��
      F=rCkk[1]*cosFi+rCkk[2]*sinFi;
      rCkk[2]=rCkk[1]*msinFi+rCkk[2]*cosFi;
      rCkk[1]=F;
      if(OTLADKA){
       if(P->R->Nam)
        sprintf(Inf,"�������� rCkk �� ���� %s ������ ��� x",P->R->Nam);
       else
        sprintf(Inf,"�������� rCkk �� ���� %0.1f ������ ��� x",P->R->Val);
       PrintFormVect4("OTLADKA",Inf,"rCkk",rCkk);
      }
//�������� ����������� ������� ��������
      F=wkk[1]*cosFi+wkk[2]*sinFi;
      wkk[2]=wkk[1]*msinFi+wkk[2]*cosFi;
      wkk[1]=F;
      if(P->R->Nam&&
        VassalOfKoord(L,TakeVary(*P->R->Nam=='-'?P->R->Nam+1:P->R->Nam))||
         FindInst("���������� ����������",&I)&&
         FindParm(I->First,P->R->Nam,&p)){//������� �� ���������� ����������
       sprintf(Inf,"%s'%s",P->R->Nam,L->Time->Name);
       Fit.V=TakeVary(Inf);
       wkk[0]=wkk[0]+Fit;
      }
      if(OTLADKA){
       if(P->R->Nam)
        sprintf(Inf,"�������� wkk �� ���� %s ������ ��� x",P->R->Nam);
       else
        sprintf(Inf,"�������� wkk �� ���� %0.1f ������ ��� x",P->R->Val);
       PrintFormVect4("OTLADKA",Inf,"wkk",wkk);
      }
      break;
     case 'y':
//������� ������� ��� �������-�������
      for(i=0;i<3;i++){
       F=Tkk_1[i][0]*cosFi+Tkk_1[i][2]*msinFi;
       Tkk_1[i][2]=Tkk_1[i][0]*sinFi+Tkk_1[i][2]*cosFi;
       Tkk_1[i][0]=F;
      }
      if(OTLADKA){
       if(!P->R->Nam)
        sprintf(Inf,"Ry: ������� ������ ��� y �� ���� %g�",P->R->Val);
       else
        sprintf(Inf,"Ry: ������� ������ ��� y �� ���� %s",P->R->Nam);
       PrintFormMatr("OTLADKA",Inf,"Tkk_1",Tkk_1);
      }
//�������� ����������� ������-������ ��
      F=rCkk[0]*cosFi+rCkk[2]*msinFi;
      rCkk[2]=rCkk[0]*sinFi+rCkk[2]*cosFi;
      rCkk[0]=F;
      if(OTLADKA){
       if(P->R->Nam)
        sprintf(Inf,"�������� rCkk �� ���� %s ������ ��� y",P->R->Nam);
       else
        sprintf(Inf,"�������� rCkk �� ���� %0.1f ������ ��� y",P->R->Val);
       PrintFormVect4("OTLADKA",Inf,"rCkk",rCkk);
      }
//�������� ����������� ������� ��������
      F=wkk[0]*cosFi+wkk[2]*msinFi;
      wkk[2]=wkk[0]*sinFi+wkk[2]*cosFi;
      wkk[0]=F;
      if(P->R->Nam&&
        VassalOfKoord(L,TakeVary(P->R->Nam))||
         FindInst("���������� ����������",&I)&&
         FindParm(I->First,P->R->Nam,&p)){//������� �� ���������� ����������
       sprintf(Inf,"%s'%s",P->R->Nam,L->Time->Name);
       Fit.V=TakeVary(Inf);
       wkk[1]=wkk[1]+Fit;
      }
      if(OTLADKA){
       if(P->R->Nam)
        sprintf(Inf,"�������� wkk �� ���� %s ������ ��� y",P->R->Nam);
       else
        sprintf(Inf,"�������� wkk �� ���� %0.1f ������ ��� y",P->R->Val);
       PrintFormVect4("OTLADKA",Inf,"wkk",wkk);
      }
      break;
     case 'z':
      for(i=0;i<3;i++){
       F=Tkk_1[i][0]*cosFi+Tkk_1[i][1]*sinFi;
       Tkk_1[i][1]=Tkk_1[i][0]*msinFi+Tkk_1[i][1]*cosFi;
       Tkk_1[i][0]=F;
      }
      if(OTLADKA){
       if(!P->R->Nam)
        sprintf(Inf,"Rz: ������� ������ ��� z �� ���� %g�",P->R->Val);
       else
        sprintf(Inf,"Rz: ������� ������ ��� z �� ���� %s",P->R->Nam);
       PrintFormMatr("OTLADKA",Inf,"Tkk_1",Tkk_1);
      }
//�������� ����������� ������-������ ��
      F=rCkk[0]*cosFi+rCkk[1]*sinFi;
      rCkk[1]=rCkk[0]*msinFi+rCkk[1]*cosFi;
      rCkk[0]=F;
      if(OTLADKA){
       if(P->R->Nam)
        sprintf(Inf,"�������� rCkk �� ���� %s ������ ��� z",P->R->Nam);
       else
        sprintf(Inf,"�������� rCkk �� ���� %0.1f ������ ��� z",P->R->Val);
       PrintFormVect4("OTLADKA",Inf,"rCkk",rCkk);
      }
//�������� ����������� ������� ��������
      F=wkk[0]*cosFi+wkk[1]*sinFi;
      wkk[1]=wkk[0]*msinFi+wkk[1]*cosFi;
      wkk[0]=F;
      if(P->R->Nam&&
        VassalOfKoord(L,TakeVary(P->R->Nam))||
         FindInst("���������� ����������",&I)&&
         FindParm(I->First,P->R->Nam,&p)){//������� �� ���������� ����������
       sprintf(Inf,"%s'%s",P->R->Nam,L->Time->Name);
       Fit.V=TakeVary(Inf);
       wkk[2]=wkk[2]+Fit;
      }
      if(OTLADKA){
       if(P->R->Nam)
        sprintf(Inf,"�������� wkk �� ���� %s ������ ��� z",P->R->Nam);
       else
        sprintf(Inf,"�������� wkk �� ���� %0.1f ������ ��� z",P->R->Val);
       PrintFormVect4("OTLADKA",Inf,"wkk",wkk);
      }
      break;
     default:
      sprintf(Inf,"������ ������������ ������� ��������� �������\n"
       "������ ���� ����������� ����� �� ������� ����:\n"
       "Rx(����),Ry(����),Rz(����).\n"
       "� ������� ����� ����� ��� ����� ����������, ��������, �� ������ \"-\".");
      Error(B->Line,0,true);
//      Application->MessageBox(Mes,"��������",MB_OK);
//      IsError=true;
      return false;
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
      sprintf(Inf,"������ ����� ������� ������� ���� ������������ \"��������\"\n"
       "������ ���� ����������� ����� �� ������� ����:\n"
       "Sx(�����),Sy(�����),Sz(�����).\n"
       "� ������� ����� ����� ��� ����� ����������, ��������, �� ������ \"-\".");
      Error(B->Line,0,true);
//      Application->MessageBox(Mes,"��������",MB_OK);
//      IsError=true;
     return false;
    }
    rCkk[Ind]=rCkk[Ind]+F;
    if(OTLADKA){
     if(P->R->Nam)
      sprintf(Inf,"������� rCkk �� �������� %s ����� ��� %c",
       P->R->Nam,Ind>1?'z':Ind?'y':'x');
     else
      sprintf(Inf,"������� rCkk �� �������� %0.1f ����� ��� %s",
       P->R->Val,Ind>1?'z':Ind?'y':'x');
     PrintFormVect4("OTLADKA",Inf,"rCkk",rCkk);
    }
    for(i=0;i<3;i++) Tkk_1[i][3]=Tkk_1[i][3]+F*Tkk_1[i][Ind];
    break;
   default:
    sprintf(Inf,"�������� �������� ���� - ��� ������ ����:\n"
     "<���>[~<��� ����>]|<������ � ��������>|<��������� �������>;\n"
     "��� <���> � <��� ����> - ����� ��� - �������� � \"��������\";\n"
     "<������ � ��������> - ������ ��������� � ������ ������� ����\n"
     "Rx(���� �������� ������ �),Ry(���� �������� ������ y),\n"
     "Rz(���� �������� ������ z),Sx(����� ����� �),Sy(����� ����� y),\n"
     "Sz(����� ����� z), ������ ����������������� ��������� (Rx(...),Ry(...),\n"
     "Rz(...))����� ���� ������� ��������� �������-���������� � ����:;\n"
     " Q(p0,p1,p2,p3), ��� p0,p1,p2,p3 - ����������, ������������ ��� ���������;"
     "<��������� �������> - ������ ����������� ����������� ���������� ����\n"
     "m(�����), Jx(������ ������� ������������ ��� x), Jy(������ �������\n"
     "������������ ��� y), Jz(������ ������� ������������ ��� z).\n"
     "����� � ������� ����� ����� ���������� ��� �����.");
    Error(B->Line,0,true);
//    Application->MessageBox(Mes,"��������",MB_OK);
//    IsError=true;
    return false;
 }}
 for(i=0;i<3;i++){
  B->rCk[i]=rCkk[i]; B->wkk[i]=wkk[i];
  for(j=0;j<4;j++) B->Tk0[i][j]=Tkk_1[i][j];
 }
//���������� �������� �� ���� � ��������� ��
//������� ������ ��������� �����������
 for(i=0;i<3;i++){
  B->vCk[i]=DifBy(B->rCk[i],L->Time);
  for(K=L->K,j=0;K;K=K->Sled,j++){
   if(!strchr(K->V->Name,'@')){
    sprintf(Inf,"%s'%s",K->V->Name,L->Time->Name);
    if(!FindVary(Inf,&V)) V=TakeVary(Inf);
    F.V=V;
    B->vCk[i]=B->vCk[i]+F*DifBy(B->rCk[i],K->V);
 }}}
// if(OTLADKA) PrintFormVect3("OTLADKA",B->Name,"B->vCk",B->vCk);
//����� ��������� ������� ��
 B->vCk[0]=B->vCk[0]+wkk[1]*B->rCk[2]-wkk[2]*B->rCk[1];
 B->vCk[1]=B->vCk[1]+wkk[2]*B->rCk[0]-wkk[0]*B->rCk[2];
 B->vCk[2]=B->vCk[2]+wkk[0]*B->rCk[1]-wkk[1]*B->rCk[0];
// if(OTLADKA) PrintFormVect3("OTLADKA",B->Name,"B->vCk",B->vCk);
//���������� ��������� �� ���� � ��������� ��
//������� ������ ��������� �����������
 for(i=0;i<3;i++){ Form Fv; Vary *V;
  B->aCk[i]=DifBy(B->vCk[i],L->Time);
  for(K=L->K,j=0;K;K=K->Sled,j++){
   if(!strchr(K->V->Name,'@')){
    sprintf(Inf,"%s'%s",K->V->Name,L->Time->Name);
    if(!FindVary(Inf,&V)) TakeVary(Inf); Fv.V=V;
    sprintf(Inf,"%s'%s'%s",K->V->Name,L->Time->Name,L->Time->Name);
    if(!FindVary(Inf,&V)) TakeVary(Inf); F.V=V;
    B->aCk[i]=B->aCk[i]+F*DifBy(B->vCk[i],Fv.V)+Fv*DifBy(B->vCk[i],K->V);
 }}}
//����� ��������� ������� ��
 B->aCk[0]=B->aCk[0]+wkk[1]*B->vCk[2]-wkk[2]*B->vCk[1];
 B->aCk[1]=B->aCk[1]+wkk[2]*B->vCk[0]-wkk[0]*B->vCk[2];
 B->aCk[2]=B->aCk[2]+wkk[0]*B->vCk[1]-wkk[1]*B->vCk[0];
/* MultMatrVect(Sk_1k,rCkk);
 MultMatrVect(Sk_1k,wkk);
 B->rCk[0]=rCkk[0]+wkk[1]*Ck_1Ck[2]-wkk[2]*Ck_1Ck[1];
 B->rCk[1]=rCkk[1]+wkk[2]*Ck_1Ck[0]-wkk[0]*Ck_1Ck[2];
 B->rCk[2]=rCkk[2]+wkk[0]*Ck_1Ck[1]-wkk[1]*Ck_1Ck[0];
//������������� � ���������� ��������
 B->rCk[0]=B->rCk[0]+Vqk[0]+wqk[1]*AkCk[2]-wqk[2]*AkCk[1];
 B->rCk[1]=B->rCk[1]+Vqk[1]+wqk[2]*AkCk[0]-wqk[0]*AkCk[2];
 B->rCk[2]=B->rCk[2]+Vqk[2]+wqk[0]*AkCk[1]-wqk[1]*AkCk[0];
 for(i=0;i<3;i++)
  B->wkk[i]=wkk[i]+wqk[i];                                  */
//������� 4.7.2007
 for(i=0;i<4;i++) for(j=0;j<4;j++){
  sprintf(Inf,"T%d%d@%s",i+1,j+1,B->Name);
  if(V=TakeVary(Inf)) V->Znach=B->Tk0[i][j];
 }
 for(i=0;i<3;i++){
  if((B->Tk0[i][3]!=Nul)&&(B->Tk0[i][3].C->Atr>=4)){
   sprintf(Inf,"%cC@%s",i==2?'Z':i==1?'Y':'X',B->Name);
   if((!FindVary(Inf,&V)||(V->Znach.C==Pust))&&(V=TakeVary(Inf))) V->Znach=B->Tk0[i][3];
   if(B->Tk0[i][3].C->Atr>4) B->Tk0[i][3].V=V;
  }
  if((B->vCk[i]!=Nul)&&(B->vCk[i].C->Atr>=4)){
   sprintf(Inf,"vC%c@%s",i==2?'z':i==1?'y':'x',B->Name);
   if((!FindVary(Inf,&V)||(V->Znach.C==Pust))&&(V=TakeVary(Inf))) V->Znach=B->vCk[i];
   if(B->vCk[i].C->Atr>4) B->vCk[i].V=V;
  }
  if((B->aCk[i]!=Nul)&&(B->aCk[i].C->Atr>=4)){
   sprintf(Inf,"aC%c@%s",i==2?'z':i==1?'y':'x',B->Name);
   if((!FindVary(Inf,&V)||(V->Znach.C==Pust))&&(V=TakeVary(Inf))) V->Znach=B->aCk[i];
   if(B->aCk[i].C->Atr>4) B->aCk[i].V=V;
  }
  if((B->wkk[i]!=Nul)&&(B->wkk[i].C->Atr>=4)){
   sprintf(Inf,"omega%c@%s",i==2?'z':i==1?'y':'x',B->Name);
   if((!FindVary(Inf,&V)||(V->Znach.C==Pust))&&(V=TakeVary(Inf))) V->Znach=B->wkk[i];
   if(B->wkk[i].C->Atr>4) B->wkk[i].V=V;
 }}
//����� ������� 4.7.2007
 if(OTLADKA){
  PrintFormVect3("OTLADKA",B->Name,"B->rCk",B->rCk);
  PrintFormVect3("OTLADKA",B->Name,"B->vCk",B->vCk);
  PrintFormVect3("OTLADKA",B->Name,"B->aCk",B->aCk);
  PrintFormVect3("OTLADKA","","B->wkk",B->wkk);
  PrintFormMatr("OTLADKA","","B->Tk0",B->Tk0);
 }
 B->Accum=true; return true;
}
//---------------------------------------------------------------------------
bool Accumulate3(Body *B){
 Form wkk[4],rCkk[4],F; Body *b=NULL; int i,j,Ind;
 Parm *p; Root *I; char *Name,Mes[512]; Vary *V;
//wkk - ������ ���������� ������� �������� k-�� ���� � ������. ��
//rCkk - ������ �� k-�� ���� � ������. ��
//�������� ������� �������������� �� ������� k-1 � k-�
//� ������� �� � ������� �������� � ��������� �� ������� ����������� ��� ����
 if(B->BaseName){
  FindBody(B->BaseName,&b);
  if(!b->Accum&&!Accumulate3(b)) return false;
 }
 for(i=0;i<4;i++){//��������� �������� - ��. ������� �������� �� �� k-1 � �� k,
                  //������-������ �� � ������� �������� k-1-�� ����
  Tkk_1[i][i].C=Odin;
  if(b){ rCkk[i]=b->rCk[i]; wkk[i]=b->wkk[i]; }
  else rCkk[i].C=wkk[i].C=Nul;
  for(j=i+1;j<4;j++) Tkk_1[i][j].C=Tkk_1[j][i].C=Nul;
 }
 for(Parm *P=B->KoordSys;P;P=P->Sled){
  switch(P->Nam[0]){
   case 'R':
    TakeCoSinFi(P->R);
    switch(P->Nam[1]){
     case 'x':
//������� ������� ��� �������-�������
      for(i=0;i<3;i++){
       F=Tkk_1[i][1]*cosFi+Tkk_1[i][2]*sinFi;
       Tkk_1[i][2]=Tkk_1[i][1]*msinFi+Tkk_1[i][2]*cosFi;
       Tkk_1[i][1]=F;
      }
      if(OTLADKA){
       if(!P->R->Nam)
        sprintf(Inf,"Rx: ������� ������ ��� x �� ���� %g�",P->R->Val);
       else
        sprintf(Inf,"Rx: ������� ������ ��� x �� ���� %s",P->R->Nam);
       PrintFormMatr("OTLADKA",Inf,"Tkk_1",Tkk_1);
      }
//�������� ����������� ������-������
      F=rCkk[1]*cosFi+rCkk[2]*sinFi;
      rCkk[2]=rCkk[1]*msinFi+rCkk[2]*cosFi;
      rCkk[1]=F;
      if(OTLADKA){
       if(P->R->Nam)
        sprintf(Inf,"�������� rCkk �� ���� %s ������ ��� x",P->R->Nam);
       else
        sprintf(Inf,"�������� rCkk �� ���� %0.1f ������ ��� x",P->R->Val);
       PrintFormVect4("OTLADKA",Inf,"rCkk",rCkk);
      }
//�������� ����������� ������� ��������
      F=wkk[1]*cosFi+wkk[2]*sinFi;
      wkk[2]=wkk[1]*msinFi+wkk[2]*cosFi;
      wkk[1]=F;
      if(P->R->Nam/*&&
        VassalOfKoord(L,TakeVary(*P->R->Nam=='-'?P->R->Nam+1:P->R->Nam))||
         FindInst("���������� ����������",&I)&&
         FindParm(I->First,P->R->Nam,&p)*/){//������� �� ���������� ����������
       sprintf(Inf,"%s'%s",P->R->Nam,L->Time->Name);
       Fit.V=TakeVary(Inf);
       wkk[0]=wkk[0]+Fit;
      }
      if(OTLADKA){
       if(P->R->Nam)
        sprintf(Inf,"�������� wkk �� ���� %s ������ ��� x",P->R->Nam);
       else
        sprintf(Inf,"�������� wkk �� ���� %0.1f ������ ��� x",P->R->Val);
       PrintFormVect4("OTLADKA",Inf,"wkk",wkk);
      }
      break;
     case 'y':
//������� ������� ��� �������-�������
      for(i=0;i<3;i++){
       F=Tkk_1[i][0]*cosFi+Tkk_1[i][2]*msinFi;
       Tkk_1[i][2]=Tkk_1[i][0]*sinFi+Tkk_1[i][2]*cosFi;
       Tkk_1[i][0]=F;
      }
      if(OTLADKA){
       if(!P->R->Nam)
        sprintf(Inf,"Ry: ������� ������ ��� y �� ���� %g�",P->R->Val);
       else
        sprintf(Inf,"Ry: ������� ������ ��� y �� ���� %s",P->R->Nam);
       PrintFormMatr("OTLADKA",Inf,"Tkk_1",Tkk_1);
      }
//�������� ����������� ������-������
      F=rCkk[0]*cosFi+rCkk[2]*msinFi;
      rCkk[2]=rCkk[0]*sinFi+rCkk[2]*cosFi;
      rCkk[0]=F;
      if(OTLADKA){
       if(P->R->Nam)
        sprintf(Inf,"�������� rCkk �� ���� %s ������ ��� y",P->R->Nam);
       else
        sprintf(Inf,"�������� rCkk �� ���� %0.1f ������ ��� y",P->R->Val);
       PrintFormVect4("OTLADKA",Inf,"rCkk",rCkk);
      }
//�������� ����������� ������� ��������
      F=wkk[0]*cosFi+wkk[2]*msinFi;
      wkk[2]=wkk[0]*sinFi+wkk[2]*cosFi;
      wkk[0]=F;
      if(P->R->Nam/*&&
        VassalOfKoord(L,TakeVary(P->R->Nam))||
         FindInst("���������� ����������",&I)&&
         FindParm(I->First,P->R->Nam,&p)*/){//������� �� ���������� ����������
       sprintf(Inf,"%s'%s",P->R->Nam,L->Time->Name);
       Fit.V=TakeVary(Inf);
       wkk[1]=wkk[1]+Fit;
      }
      if(OTLADKA){
       if(P->R->Nam)
        sprintf(Inf,"�������� wkk �� ���� %s ������ ��� y",P->R->Nam);
       else
        sprintf(Inf,"�������� wkk �� ���� %0.1f ������ ��� y",P->R->Val);
       PrintFormVect4("OTLADKA",Inf,"wkk",wkk);
      }
      break;
     case 'z':
      for(i=0;i<3;i++){
       F=Tkk_1[i][0]*cosFi+Tkk_1[i][1]*sinFi;
       Tkk_1[i][1]=Tkk_1[i][0]*msinFi+Tkk_1[i][1]*cosFi;
       Tkk_1[i][0]=F;
      }
      if(OTLADKA){
       if(!P->R->Nam)
        sprintf(Inf,"Rz: ������� ������ ��� z �� ���� %g�",P->R->Val);
       else
        sprintf(Inf,"Rz: ������� ������ ��� z �� ���� %s",P->R->Nam);
       PrintFormMatr("OTLADKA",Inf,"Tkk_1",Tkk_1);
      }
//�������� ����������� ������-������
      F=rCkk[0]*cosFi+rCkk[1]*sinFi;
      rCkk[1]=rCkk[0]*msinFi+rCkk[1]*cosFi;
      rCkk[0]=F;
      if(OTLADKA){
       if(P->R->Nam)
        sprintf(Inf,"�������� rCkk �� ���� %s ������ ��� z",P->R->Nam);
       else
        sprintf(Inf,"�������� rCkk �� ���� %0.1f ������ ��� z",P->R->Val);
       PrintFormVect4("OTLADKA",Inf,"rCkk",rCkk);
      }
//�������� ����������� ������� ��������
      F=wkk[0]*cosFi+wkk[1]*sinFi;
      wkk[1]=wkk[0]*msinFi+wkk[1]*cosFi;
      wkk[0]=F;
      if(P->R->Nam/*&&
        VassalOfKoord(L,TakeVary(P->R->Nam))||
         FindInst("���������� ����������",&I)&&
         FindParm(I->First,P->R->Nam,&p)*/){//������� �� ���������� ����������
       sprintf(Inf,"%s'%s",P->R->Nam,L->Time->Name);
       Fit.V=TakeVary(Inf);
       wkk[2]=wkk[2]+Fit;
      }
      if(OTLADKA){
       if(P->R->Nam)
        sprintf(Inf,"�������� wkk �� ���� %s ������ ��� z",P->R->Nam);
       else
        sprintf(Inf,"�������� wkk �� ���� %0.1f ������ ��� z",P->R->Val);
       PrintFormVect4("OTLADKA",Inf,"wkk",wkk);
      }
      break;
     default:
      sprintf(Inf,"������ ������������ ������� ��������� �������\n"
       "������ ���� ����������� ����� �� ������� ����:\n"
       "Rx(����),Ry(����),Rz(����).\n"
       "� ������� ����� ����� ��� ����� ����������, ��������, �� ������ \"-\".");
      Error(B->Line,0,true);
//      Application->MessageBox(Mes,"��������",MB_OK);
//     IsError=true;
      return false;
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
      sprintf(Inf,"������ ����� ������� ������� ���� ������������ \"��������\"\n"
       "������ ���� ����������� ����� �� ������� ����:\n"
       "Sx(�����),Sy(�����),Sz(�����).\n"
       "� ������� ����� ����� ��� ����� ����������, ��������, �� ������ \"-\".");
      Error(B->Line,0,true);
//      Application->MessageBox(Mes,"��������",MB_OK);
//      IsError=true;
      return false;
    }
    rCkk[Ind]=rCkk[Ind]+F;
    if(OTLADKA) PrintFormVect4("OTLADKA","������� ������ �.�.","rCkk",rCkk);
    for(i=0;i<3;i++) Tkk_1[i][3]=Tkk_1[i][3]+F*Tkk_1[i][Ind];
    break;
   default:
    sprintf(Inf,"�������� �������� ���� - ��� ������ ����:\n"
     "<���>[~<��� ����>]|<������ � ��������>|<��������� �������>;\n"
     "��� <���> � <��� ����> - ����� ��� - �������� � \"��������\";\n"
     "<������ � ��������> - ������ ��������� � ������ ������� ����\n"
     "Rx(���� �������� ������ �),Ry(���� �������� ������ y),\n"
     "Rz(���� �������� ������ z),Sx(����� ����� �),Sy(����� ����� y),\n"
     "Sz(����� ����� z), ������ ����������������� ��������� (Rx(...),Ry(...),\n"
     "Rz(...))����� ���� ������� ��������� �������-���������� � ����:;\n"
     " Q(p0,p1,p2,p3), ��� p0,p1,p2,p3 - ����������, ������������ ��� ���������;"
     "<��������� �������> - ������ ����������� ����������� ���������� ����\n"
     "m(�����), Jx(������ ������� ������������ ��� x), Jy(������ �������\n"
     "������������ ��� y), Jz(������ ������� ������������ ��� z).\n"
     "����� � ������� ����� ����� ���������� ��� �����.");
    Error(B->Line,0,true);
//    Application->MessageBox(Mes,"��������",MB_OK);
//    IsError=true;
   return false;
 }}
//���������� ������-������ �� ����, ��� ������� �������� � ������� ��������������
 for(i=0;i<4;i++){
  B->wkk[i]=wkk[i]; B->rCk[i]=rCkk[i];
 }
 if(B->BaseName)
  MultMatrMatr(b->Tk0,Tkk_1,B->Tk0);
 else{
  for(i=0;i<4;i++) for(j=0;j<4;j++) B->Tk0[i][j]=Tkk_1[i][j];
 }
//������� 4.7.2007
 for(i=0;i<3;i++){ Vary *V;
  if(rCkk[i]!=Nul){
   sprintf(Inf,"%cC@%s",i==2?'z':i==1?'y':'x',B->Name);
   if((!FindVary(Inf,&V)||(V->Znach.C==Pust))&&(V=TakeVary(Inf))) V->Znach=rCkk[i];
   B->rCk[i].V=V;
  }
  if(wkk[i]!=Nul){
   sprintf(Inf,"omega%c@%s",i==2?'z':i==1?'y':'x',B->Name);
   if((!FindVary(Inf,&V)||(V->Znach.C==Pust))&&(V=TakeVary(Inf))) V->Znach=wkk[i];
   B->wkk[i].V=V;
 }}
//����� ������� 4.7.2007
 for(i=0;i<4;i++) for(j=0;j<4;j++){
  sprintf(Inf,"T%d%d@%s",i+1,j+1,B->Name);
  if(V=TakeVary(Inf)) V->Znach=B->Tk0[i][j];
 }
/* for(i=0;i<3;i++){ Vary *V;
  if(rCkk[i]!=Nul){
   B->rCk[i]=rCkk[i];
//   switch(rCkk[i].C->Atr){
//    case 3:
//     B->rCk[i].C=rCkk[i].C; break;
//    case 4:
//     B->rCk[i]=rCkk[i].V->Znach.C==Pust?rCkk[i]:rCkk[i].V->Znach; break;
//    case 5: if(rCkk[i].U->Func==1) B->rCk[i]=rCkk[i].V->Znach.C==Pust?rCkk[i]:rCkk[i].V->Znach; break;
//    default:
//     sprintf(Inf,"%cC@%s",i==2?'z':i==1?'y':'x',B->Name);
//     if(V=TakeVary(Inf)){ V->Znach=rCkk[i]; B->rCk[i].V=V; }
//   }
  }
  if(wkk[i]!=Nul){
   B->wkk[i]=wkk[i];
//   switch(wkk[i].C->Atr){
//    case 3: B->wkk[i].C=wkk[i].C; break;
//    case 4: B->wkk[i]=wkk[i].V->Znach.C==Pust?wkk[i]:wkk[i].V->Znach; break;
//    default:
//     sprintf(Inf,"omega%c@%s",i==2?'z':i==1?'y':'x',B->Name);
//     if(V=TakeVary(Inf)){ V->Znach=wkk[i]; B->wkk[i].V=V; }
//   }
//  }
 }*/
 if(OTLADKA){
  PrintFormVect4("OTLADKA",B->Name,"B->rCk",B->rCk);
  PrintFormVect4("OTLADKA","","B->wkk",B->wkk);
  PrintFormMatr("OTLADKA","","B->Tk0",B->Tk0);
 }
 B->Accum=true; return true;
}
//---------------------------------------------------------------------------
void LarinT(Body *b){
 Form F,J[4],**D,Fi,Fj; Parm *p; Vary *V;
 int i,j,k,l,N=KolElem(L->K); Cord *Ki,*Kj;
 D=(Form **)calloc(N,SzV);
 for(i=0;i<N;i++){
  D[i]=(Form *)calloc(N,sizeof(Form));
  for(j=0;j<N;j++) D[i][j].C=Nul;
 }
 for(i=0;i<4;i++) J[i].C=Nul;
 for(p=b->Inert,i=0;p;p=p->Sled,i++){
  if(p->Nam){
   if(p->R){
    switch(p->Nam[0]){
     case 'm':
      if(p->R->Nam){
       if(FindVary(p->R->Nam,&V)) J[3].V=V;
      }
      else J[3].C=TakeCnst(p->R->Val);
      break;
     case 'J':
      if(p->R->Nam){
       if(FindVary(p->R->Nam,&V)) F.V=V;
      }
      else F.C=TakeCnst(p->R->Val);
      switch(p->Nam[1]){
       case 'x': J[0]=J[0]-F; J[1]=J[1]+F; J[2]=J[2]+F; break;
       case 'y': J[0]=J[0]+F; J[1]=J[1]-F; J[2]=J[2]+F; break;
       case 'z': J[0]=J[0]+F; J[1]=J[1]+F; J[2]=J[2]-F; break;
   }} }
   else{
    if(p->Nam){
     if(FindVary(p->Nam,&V)) F.V=V;
    }
    else
     F.C=TakeCnst(p->R->Val);
     switch(i){
      case 0: J[3]=F; break;
      case 1: J[0]=J[0]-F; J[1]=J[1]+F; J[2]=J[2]+F; break;
      case 2: J[0]=J[0]+F; J[1]=J[1]-F; J[2]=J[2]+F; break;
      case 3: J[0]=J[0]+F; J[1]=J[1]+F; J[2]=J[2]-F; break;
 }}} }
 for(i=0;i<3;i++) J[i]=0.5*J[i];
 for(Ki=L->K,i=0;Ki;Ki=Ki->Sled,i++)
  for(Kj=L->K,j=0;Kj;Kj=Kj->Sled,j++)
   for(l=0;l<4;l++){ Form d; d.C=Nul;
    for(k=0;k<4;k++)
     d=d+DifBy(b->Tk0[k][l],Ki->V)*DifBy(b->Tk0[k][l],Kj->V);
    D[i][j]=J[k]*d;
   }
 for(Ki=L->K,i=0;Ki;Ki=Ki->Sled,i++){
  sprintf(Inf,"%s'%s",Ki->V->Name,L->Time->Name); Fi.V=TakeVary(Inf);
  for(Kj=L->K,j=0;Kj;Kj=Kj->Sled,j++){
   sprintf(Inf,"%s'%s",Kj->V->Name,L->Time->Name); Fj.V=TakeVary(Inf);
   TLarin=TLarin+D[i][j]*Fi*Fj;
}}}
//---------------------------------------------------------------------------
void LarinMetod(Body *B){
 Form A[4][4],To[4][4],U[6][4][4],J[4],D[6][6],TLarin; FILE *FF; Root *I;
 TLarin.C=Nul;
 for(Body *b=B;b;b=b->Sled){ Vary *V; Form F; int i,j,k;
  for(int i=0;i<4;i++){
   To[i][i].C=Odin; for(int j=i+1;j<4;j++) To[i][j].C=To[j][i].C=Nul;
  }
  for(Parm *p,*P=b->KoordSys;P;P=P->Sled){
   if(P->Nam&&(TakeVary(P->Nam)->Znach.C==Pust)||
     FindInst("���������� ����������",&I)&&FindParm(I->First,P->Nam,&p)){
//�������� ��� ������� �� ���������� ����������
    for(int i=0;i<4;i++){
     A[i][i].C=Odin; for(int j=i+1;j<4;j++) A[i][j].C=A[j][i].C=Nul;
    }
    if(P->R)
     if(P->R->Nam) F.V=TakeVary(P->R->Nam);
     else F.C=TakeCnst(P->R->Val);
    if(P->Nam[0]=='R'){
     switch(P->Nam[1]){
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
     switch(P->Nam[1]){
      case 'x': A[0][3]=F; break;
      case 'y': A[1][3]=F; break;
      case 'z': A[2][3]=F; break;
    }}
if(OTLADKA) PrintMatr("OTLADKA","","A",A);
if(OTLADKA) PrintMatr("OTLADKA","�� To*A","To",To);
    MatrMultMatr(To,A);
if(OTLADKA) PrintMatr("OTLADKA","����� To*A","To",To);
  }}
  for(int k=0;k<6;k++) for(int i=0;i<4;i++) for(int j=0;j<4;j++) U[k][i][j].C=Nul;
  k=0;
  for(Parm *p,*P=b->KoordSys;P;P=P->Sled){
   if(P->Nam&&(TakeVary(P->Nam)->Znach.C==Pust)||
     FindInst("���������� ����������",&I)&&FindParm(I->First,P->Nam,&p)){
    if(P->R&&P->R->Nam){
     Vary *V=TakeVary(P->R->Nam);
     for(i=0;i<4;i++) for(j=0;j<4;j++) U[k][i][j]=DifBy(To[i][j],V);
sprintf(Inf,"U[%d][%d][%d]",k,i,j);
if(OTLADKA) PrintMatr("OTLADKA","U",Inf,U[k]);
     k++;
  }}}
  k=0; J[0].C=J[1].C=J[2].C=J[3].C=Nul;
  for(Parm *P=b->Inert;P;P=P->Sled){
   Vary *V=TakeVary(P->Nam); Form F; F.V=V;
   switch(k++){
    case 0: J[3]=F; break;
    case 1: J[0]=J[0]-0.5*F; J[1]=J[1]+0.5*F; J[2]=J[2]+0.5*F; break;
    case 2: J[0]=J[0]+0.5*F; J[1]=J[1]-0.5*F; J[2]=J[2]+0.5*F; break;
    case 3: J[0]=J[0]+0.5*F; J[1]=J[1]+0.5*F; J[2]=J[2]-0.5*F; break;
  }}
if(OTLADKA){
PrintVect3("OTLADKA","","J",J);
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
if(OTLADKA){
sprintf(Inf,"D[%d][%d]",i,j);
PrintForm(true,FF,Inf,D[i][j]);
}
  }}
  i=0;
  for(Parm *p,*P=b->KoordSys;P;P=P->Sled){
   if(P->Nam&&(TakeVary(P->Nam)->Znach.C==Pust)||
      FindInst("���������� ����������",&I)&&
      FindParm(I->First,P->Nam,&p)){
    Form FVqi; sprintf(Inf,"%s't",P->Nam); FVqi.V=TakeVary(Inf); j=0;
    for(Parm *p,*P=b->KoordSys;P;P=P->Sled){
     if(P->Nam&&(TakeVary(P->Nam)->Znach.C==Pust)||
        FindInst("���������� ����������",&I)&&
        FindParm(I->First,p->Nam,&p)){
      Form FVqj; sprintf(Inf,"%s't",P->Nam); FVqj.V=TakeVary(Inf);
      TLarin=TLarin+FVqi*FVqj*D[i][j++];
if(OTLADKA){
PrintForm(true,FF,"FVqi",FVqi);
PrintForm(true,FF,"FVqj",FVqj);
sprintf(Inf,"D[%d][%d]",i,j);
PrintForm(true,FF,Inf,D[i][j]);
PrintForm(true,FF,"TLarin",TLarin);
}
    }}
    i++;
 }}}
if(OTLADKA) fclose(FF);
// Vary *V=TakeVary("������"); V->Znach=L;
}
//---------------------------------------------------------------------------
bool Bodys(Body *B){
 char Mes[512],NameCord[256],NameElem[256],*inf; bool ErrorInert=false;
 int i,j,k,N; Vary *K; Parm *P=B->KoordSys; Root *I; Magazine *M=NULL,*m;
 Elem *Jmx,*Jmy,*Jmz,*Jwx,*Jwy,*Jwz;
 Form FomegaX,FomegaY,FomegaZ; Vary *Kx=NULL,*Ky=NULL,*Kz=NULL;
 OTLADKA=FindInst("�������",&I); Body *b,*bb; Inf[0]='\0';
 if(OTLADKA&&!access("OTLADKA",0)) unlink("OTLADKA");
 CoordWithoutDT(L);
//��������, ��� ���� ��� ����
 for(*Mes='\0',inf=Mes,b=B;b;b=b->Sled){
  if(b->BaseName&&!FindBody(b->BaseName,&bb))
   inf+=sprintf(inf,"%s~%s, ",b->Name,b->BaseName);
 }
 if(*Mes){
  Mes[strlen(Mes)-2]='\0';
  sprintf(Inf,"��������� ����\n%s\n��������� �� ��������������",Mes);
  Error(B->Line,0,true);
  return false;
 }
//��������, ���� �� ������
 Mes[0]='\0'; inf=Mes;
 for(b=B;b;b=b->Sled){
  DelAllMagazine(&M);
  if(TestBody(b,&M)) inf+=sprintf(inf,"%s, ",b->BaseName);
  if(KolElem(b->Inert)>7)
   ErrorInert=true;
  else if(KolElem(b->Inert)<4){
   for(P=b->Inert;P;P=P->Sled)
    if(!P->R||P->R->Sled){
     sprintf(Inf,"��� �������� �������� ���� (������ %d) ���� ������\n"
      "����������� ��������� � ����: ���(��������)",b->Line);
     Error(b->Line,0,true);
     ErrorInert=true; break;
  }}
  if(ErrorInert){
   sprintf(Inf,
    "� �������� �������� ���� ������ ��������� ������� - ����� � �������\n"
    "������� - ������������ ��� ������� �������. ���� ������ ������� ����\n"
    "����������, �� ����������� ������ ������ ������� �������, ����� - ���\n"
    "������������. ����� ��������� �������������� ������� � ����,\n"
    " m(<��������_m>),Jx(<��������_Jx>),Jy(<��������_Jy>),Jz(<��������_Jz>),\n"
    "Jxy(<��������_Jxy>),Jyz(<��������_Jyz>),Jxz(<��������_Jxz>)\n"
    " ��� � ����\n"
    "<��������_m>,<��������_Jx>,<��������_Jy>,<��������_Jz>,\n"
    "<��������_Jxy>,<��������_Jyz>,<��������_Jxz>.\n"
    "� ������ ������ ����� ���������� ��������� �������� ��� �������� ��\n"
    "� ������������ �������, �� ������ - �� ������� �������� � ���������\n"
    "�������. ����� ����� ��� <��������> ���������� ����� ��� ����������,\n"
    "���������� �������, ������ ��������������� ��������");
   Error(b->Line,0,true);
   return false;
 }}
 if(*Mes){
  Mes[strlen(Mes)-2]='\0';
  sprintf(Inf,"��������� ����\n%s\n��������� ���� �� ����",Inf);
  Error(B->Line,0,true);
  return false;
 }
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
 else OPRFILE=fopen(OprFile,"a");
//������� ������������� ������� � �������
 for(int i=0;i<4;i++){
  S[i][i].C=Odin; for(int j=i+1;j<4;j++) S[i][j].C=S[j][i].C=Nul;
 }
 for(b=B;b;b=b->Sled){
  for(int i=0;i<4;i++){
   b->rCk[i].C=b->wkk[i].C=Nul; b->Tk0[i][i].C=Odin;
   for(int j=i+1;j<4;j++) b->Tk0[i][j].C=b->Tk0[j][i].C=Nul;
 }}
 if(FindInst("����� ������",&I))
  TLarin.C=Nul;
 if(OTLADKA){
  FILE *F=fopen("Otladka","w"); Fputs("���������� ����������\n",F); fclose(F);
 }
//������� ���������� � ������� ����������� ���������
 for(b=B;b;b=b->Sled){ Root *I;
  fprintf(OPRFILE,"<h3 align=\"center\"><font color=\"#800000\">"
   "������ ������� ���� � ������ \"%s\" ������ %d ������������ �������� � �������:"
   "</font></h3>\n",b->Name,b->Line);
  if(FindInst("��������� ��",&I)){
   if(FindInst("������� ���������",&I)){
    if(!Accumulate3(b)){ IsError=true; return false; }
   }
   else{
    if(!Accumulate2v(b)){ IsError=true; return false; }
  }}
  else{
   if(FindInst("������� ���������",&I)){
    sprintf(Inf,"���������� \"������� ���������\"\n"
     "�������������� ������ ��� �������\n���������� \"��������� ��\"");
    Atten(b->Line,0);
   }
   if(!Accumulate(b)){ IsError=true; return false; }
   if(FindInst("����� ������",&I))
    LarinT(b);
  }
  if(b->Inert->R&&b->Inert->Nam){//���� ������ �� �����
   for(P=b->Inert;P;P=P->Sled){
    if(!strcmp(P->Nam,"m")){
     if(FindInst("��������� ��",&I))
      if(FindInst("������� ���������",&I))
       sprintf(NameCord,"aCx@%s",b->Name);
      else sprintf(NameCord,"vCx@%s",b->Name);
     else sprintf(NameCord,"XC@%s",b->Name);
     if(!(K=TakeVary(NameCord))){ IsError=true; return false; }
     if(K->Znach!=Nul){
      sprintf(NameElem,"J%s.%s",b->Name,NameCord);
      if(!(Jmx=TakeElem(NameElem))){ IsError=true; return false; }
      Jmx->Znach=TakeZnach(P);
      Jmx->K=K;
      PrintForm(false,OPRFILE,Jmx->Name,Jmx->Znach); fputc(' ',OPRFILE);
      PrintForm(false,OPRFILE,Jmx->K->Name,Jmx->K->Znach); fputs("<br>\n",OPRFILE);
     }
     if(FindInst("��������� ��",&I))
      if(FindInst("������� ���������",&I))
       sprintf(NameCord,"aCy@%s",b->Name);
      else sprintf(NameCord,"vCy@%s",b->Name);
     else sprintf(NameCord,"YC@%s",b->Name);
     if(!(K=TakeVary(NameCord))){ IsError=true; return false; }
     if(K->Znach!=Nul){
      sprintf(NameElem,"J%s.%s",b->Name,NameCord);
      if(!(Jmy=TakeElem(NameElem))){ IsError=true; return false; }
      Jmy->Znach=Jmx->Znach;
      Jmy->K=K;
      PrintForm(false,OPRFILE,Jmy->Name,Jmy->Znach); fputc(' ',OPRFILE);
      PrintForm(false,OPRFILE,Jmy->K->Name,Jmy->K->Znach); fputs("<br>\n",OPRFILE);
     }
     if(FindInst("��������� ��",&I))
      if(FindInst("������� ���������",&I))
       sprintf(NameCord,"aCz@%s",b->Name);
      else sprintf(NameCord,"vCz@%s",b->Name);
     else sprintf(NameCord,"ZC@%s",b->Name);
     if(!(K=TakeVary(NameCord))){ IsError=true; return false; }
     if(K->Znach!=Nul){
      sprintf(NameElem,"J%s.%s",b->Name,NameCord);
      if(!(Jmz=TakeElem(NameElem))){ IsError=true; return false; }
      Jmz->Znach=Jmx->Znach;
      Jmz->K=K;
      PrintForm(false,OPRFILE,Jmz->Name,Jmz->Znach); fputc(' ',OPRFILE);
      PrintForm(false,OPRFILE,Jmz->K->Name,Jmz->K->Znach); fputs("<br>\n",OPRFILE);
    }}
    else if(!strcmp(P->Nam,"Jx")){
     sprintf(NameCord,"omegax@%s",b->Name);
     if(!(K=TakeVary(NameCord))){ IsError=true; return false; }
     sprintf(NameElem,"J%s.%s",b->Name,NameCord);
     if(!(Jwx=TakeElem(NameElem))){ IsError=true; return false; }
     Jwx->Znach=TakeZnach(P);
     Jwx->K=K;
     PrintForm(false,OPRFILE,Jwx->Name,Jwx->Znach); fputc(' ',OPRFILE);
     PrintForm(false,OPRFILE,Jwx->K->Name,Jwx->K->Znach); fputs("<br>\n",OPRFILE);
    }
    else if(!strcmp(P->Nam,"Jy")){
     sprintf(NameCord,"omegay@%s",b->Name);
     if(!(K=TakeVary(NameCord))){ IsError=true; return false; }
     sprintf(NameElem,"J%s.%s",b->Name,NameCord);
     if(!(Jwy=TakeElem(NameElem))){ IsError=true; return false; }
     Jwy->Znach=TakeZnach(P);
     Jwy->K=K;
     PrintForm(false,OPRFILE,Jwy->Name,Jwy->Znach); fputc(' ',OPRFILE);
     PrintForm(false,OPRFILE,Jwy->K->Name,Jwy->K->Znach); fputs("<br>\n",OPRFILE);
    }
    else if(!strcmp(P->Nam,"Jz")){
     sprintf(NameCord,"omegaz@%s",b->Name);
     if(!(K=TakeVary(NameCord))){ IsError=true; return false; }
     sprintf(NameElem,"J%s.%s",b->Name,NameCord);
     if(!(Jwz=TakeElem(NameElem))){ IsError=true; return false; }
     Jwz->Znach=TakeZnach(P);
     Jwz->K=K;
     PrintForm(false,OPRFILE,Jwz->Name,Jwz->Znach); fputc(' ',OPRFILE);
     PrintForm(false,OPRFILE,Jwz->K->Name,Jwz->K->Znach); fputs("<br>\n",OPRFILE);
    }
    else if(!strcmp(P->Nam,"Jxy")){
     char NameOmX[256],NameOmY[256],NameKx[256],NameKy[256];
     sprintf(NameOmX,"omegax@%s",b->Name); FomegaX.V=TakeVary(NameOmX);
     sprintf(NameOmY,"omegay@%s",b->Name); FomegaY.V=TakeVary(NameOmY);
     sprintf(NameKx,"Kx@%s",b->Name); Kx=TakeVary(NameKx);
     sprintf(NameKy,"Ky@%s",b->Name); Ky=TakeVary(NameKy);
     b->Jxy=TakeZnach(P);
     Kx->Znach=Jwx->Znach*FomegaX-b->Jxy*FomegaY; Ky->Znach=-b->Jxy*FomegaX;
     b->Kx.V=Kx;
     b->Ky.V=Ky;
    }
    else if(!strcmp(P->Nam,"Jyz")){
     char NameOmY[256],NameOmZ[256],NameKz[256];
     sprintf(NameOmY,"omegay@%s",b->Name); FomegaY.V=TakeVary(NameOmY);
     sprintf(NameOmZ,"omegaz@%s",b->Name); FomegaZ.V=TakeVary(NameOmZ);
     sprintf(NameKz,"Kz@%s",b->Name); Kz=TakeVary(NameKz);
     b->Jyz=TakeZnach(P);
     Ky->Znach=Ky->Znach+Jwy->Znach*FomegaY-b->Jyz*FomegaZ;
     Kz->Znach=-b->Jyz*FomegaY+Jwz->Znach*FomegaZ;
     b->Kz.V=Kz;
    }
    else if(!strcmp(P->Nam,"Jxz")){
     char NameOmX[256],NameOmZ[256];
     sprintf(NameOmX,"omegax@%s",b->Name); FomegaX.V=TakeVary(NameOmX);
     sprintf(NameOmZ,"omegaz@%s",b->Name); FomegaZ.V=TakeVary(NameOmZ);
     b->Jxz=TakeZnach(P);
     Kx->Znach=Kx->Znach-b->Jxz*FomegaZ;
     Kz->Znach=-b->Jxz*FomegaX+Kz->Znach;
    }
    if(Kx){
     PrintForm(false,OPRFILE,Kx->Name,Kx->Znach); fputs("<br>\n",OPRFILE);
     Kx=NULL;
    }
    if(Ky){
     PrintForm(false,OPRFILE,Ky->Name,Ky->Znach); fputs("<br>\n",OPRFILE);
     Ky=NULL;
    }
    if(Kz){
     PrintForm(false,OPRFILE,Kz->Name,Kz->Znach); fputs("<br>\n",OPRFILE);
     Kz=NULL;
  }}}
  else{
   if(FindInst("��������� ��",&I))
    if(FindInst("������� ���������",&I))
     sprintf(NameCord,"aCx@%s",b->Name);
    else sprintf(NameCord,"vCx@%s",b->Name);
   else sprintf(NameCord,"XC@%s",b->Name);
   if(!(K=TakeVary(NameCord))){ IsError=true; return false; }
   if(K->Znach!=Nul){
    sprintf(NameElem,"J%s.%s",b->Name,NameCord);
    if(!(Jmx=TakeElem(NameElem))){ IsError=true; return false; }
    Jmx->Znach=TakeZnach(b->Inert);
    Jmx->K=K;
    PrintForm(false,OPRFILE,Jmx->Name,Jmx->Znach); fputc(' ',OPRFILE);
    PrintForm(false,OPRFILE,Jmx->K->Name,Jmx->K->Znach); fputs("<br>\n",OPRFILE);
   }
   if(FindInst("��������� ��",&I))
    if(FindInst("������� ���������",&I))
     sprintf(NameCord,"aCy@%s",b->Name);
    else sprintf(NameCord,"vCy@%s",b->Name);
   else sprintf(NameCord,"YC@%s",b->Name);
   if(!(K=TakeVary(NameCord))){ IsError=true; return false; }
   if(K->Znach!=Nul){
    sprintf(NameElem,"J%s.%s",b->Name,NameCord);
    if(!(Jmy=TakeElem(NameElem))){ IsError=true; return false; }
//    Jmy->Znach=Jmx->Znach;
    Jmy->Znach=TakeZnach(b->Inert);
    Jmy->K=K;
    PrintForm(false,OPRFILE,Jmy->Name,Jmy->Znach); fputc(' ',OPRFILE);
    PrintForm(false,OPRFILE,Jmy->K->Name,Jmy->K->Znach); fputs("<br>\n",OPRFILE);
   }
   if(FindInst("��������� ��",&I))
    if(FindInst("������� ���������",&I))
     sprintf(NameCord,"aCz@%s",b->Name);
    else sprintf(NameCord,"vCz@%s",b->Name);
   else sprintf(NameCord,"ZC@%s",b->Name);
   if(!(K=TakeVary(NameCord))){ IsError=true; return false; }
   if(K->Znach!=Nul){
    sprintf(NameElem,"J%s.%s",b->Name,NameCord);
    if(!(Jmz=TakeElem(NameElem))){ IsError=true; return false; }
//    Jmz->Znach=Jmx->Znach;
    Jmz->Znach=TakeZnach(b->Inert);
    Jmz->K=K;
    PrintForm(false,OPRFILE,Jmz->Name,Jmz->Znach); fputc(' ',OPRFILE);
    PrintForm(false,OPRFILE,Jmz->K->Name,Jmz->K->Znach); fputs("<br>\n",OPRFILE);
   }
//������� ��� ��� �������
   P=b->Inert->Sled;
   sprintf(NameCord,"omegax@%s",b->Name);
   if(!(K=TakeVary(NameCord))){ IsError=true; return false; }
   if(K->Znach!=Nul){
    sprintf(NameElem,"J%s.%s",b->Name,NameCord);
    if(!(Jwx=TakeElem(NameElem))){ IsError=true; return false; }
    Jwx->Znach=TakeZnach(P);
    Jwx->K=K;
    PrintForm(false,OPRFILE,Jwx->Name,Jwx->Znach); fputc(' ',OPRFILE);
    PrintForm(false,OPRFILE,Jwx->K->Name,Jwx->K->Znach); fputs("<br>\n",OPRFILE);
   }
   P=P->Sled;
   sprintf(NameCord,"omegay@%s",b->Name);
   if(!(K=TakeVary(NameCord))){ IsError=true; return false; }
   if(K->Znach!=Nul){
    sprintf(NameElem,"J%s.%s",b->Name,NameCord);
    if(!(Jwy=TakeElem(NameElem))){ IsError=true; return false; }
    Jwy->Znach=TakeZnach(P);
    Jwy->K=K;
    PrintForm(false,OPRFILE,Jwy->Name,Jwy->Znach); fputc(' ',OPRFILE);
    PrintForm(false,OPRFILE,Jwy->K->Name,Jwy->K->Znach); fputs("<br>\n",OPRFILE);
   }
   P=P->Sled;
   sprintf(NameCord,"omegaz@%s",b->Name);
   if(!(K=TakeVary(NameCord))){ IsError=true; return false; }
   if(K->Znach!=Nul){
    sprintf(NameElem,"J%s.%s",b->Name,NameCord);
    if(!(Jwz=TakeElem(NameElem))){ IsError=true; return false; }
    Jwz->Znach=TakeZnach(P);
    Jwz->K=K;
    PrintForm(false,OPRFILE,Jwz->Name,Jwz->Znach); fputc(' ',OPRFILE);
    PrintForm(false,OPRFILE,Jwz->K->Name,Jwz->K->Znach); fputs("<br>\n",OPRFILE);
   }
   if(P=P->Sled){
    char NameOmX[256],NameOmY[256],NameKx[256],NameKy[256];
    sprintf(NameOmX,"omegax@%s",b->Name); FomegaX.V=TakeVary(NameOmX);
    sprintf(NameOmY,"omegay@%s",b->Name); FomegaY.V=TakeVary(NameOmY);
    sprintf(NameKx,"Kx@%s",b->Name); Kx=TakeVary(NameKx);
    sprintf(NameKy,"Ky@%s",b->Name); Ky=TakeVary(NameKy);
    b->Jxy=TakeZnach(P);
    Kx->Znach=Jwx->Znach*FomegaX;
    Kx->Znach=Kx->Znach-b->Jxy*FomegaY;
    Ky->Znach=-b->Jxy*FomegaX;
    b->Kx.V=Kx;
    b->Ky.V=Ky;
    if(P=P->Sled){
     char NameOmY[256],NameOmZ[256],NameKz[256];
     sprintf(NameOmY,"omegay@%s",b->Name); FomegaY.V=TakeVary(NameOmY);
     sprintf(NameOmZ,"omegaz@%s",b->Name); FomegaZ.V=TakeVary(NameOmZ);
     sprintf(NameKz,"Kz@%s",b->Name); Kz=TakeVary(NameKz);
     b->Jyz=TakeZnach(P);
     Ky->Znach=Ky->Znach+Jwy->Znach*FomegaY-b->Jyz*FomegaZ;
     Kz->Znach=-b->Jyz*FomegaY+Jwz->Znach*FomegaZ;
     b->Kz.V=Kz;
     if(P=P->Sled){
      char NameOmX[256],NameOmZ[256];
      sprintf(NameOmX,"omegax@%s",b->Name); FomegaX.V=TakeVary(NameOmX);
      sprintf(NameOmZ,"omegaz@%s",b->Name); FomegaZ.V=TakeVary(NameOmZ);
      b->Jxz=TakeZnach(P);
      Kx->Znach=Kx->Znach-b->Jxz*FomegaZ;
      Kz->Znach=-b->Jxz*FomegaX+Kz->Znach;
   }}}
   if(Kx){
    PrintForm(false,OPRFILE,Kx->Name,Kx->Znach); fputs("<br>\n",OPRFILE);
    Kx=NULL;
   }
   if(Ky){
    PrintForm(false,OPRFILE,Ky->Name,Ky->Znach); fputs("<br>\n",OPRFILE);
    Ky=NULL;
   }
   if(Kz){
    PrintForm(false,OPRFILE,Kz->Name,Kz->Znach); fputs("<br>\n",OPRFILE);
    Kz=NULL;
 }}}
 if(FindInst("����� ������",&I)) TLarin=0.5*TLarin;
//�������� ���������� �����, �������� � �� ��� � ���������� ��������� c ~
 bool EtchoNet=true;
 for(Arra *A=L->A;A;A=A->Sled){ char *s; Vary *V; Form F;
  if((s=strchr(A->Name,'~'))&&(A->N==3)&&FindBody(s+1,&b)){
   *s='\0'; sprintf(Inf,"X@%s",A->Name);
   if(V=TakeVary(Inf)){
    V->Znach=b->Tk0[0][3];
    for(int i=0;i<3;i++){
     if(A->F) F=A->F[i]; else F.C=TakeCnst(A->A[i]);
     V->Znach=V->Znach+F*b->Tk0[0][i];
    }
    if(EtchoNet){
     fprintf(OPRFILE,"<h4 align=\"center\"><font color=\"#000080\">"
      "���������� ����� ���:</font></h4>\n");
     EtchoNet=false;
    }
    PrintForm(false,OPRFILE,V->Name,V->Znach); fputs("<br>\n",OPRFILE);
   }
   sprintf(Inf,"Y@%s",A->Name);
   if(V=TakeVary(Inf)){
    V->Znach=b->Tk0[1][3];
    for(int i=0;i<3;i++){
     if(A->F) F=A->F[i]; else F.C=TakeCnst(A->A[i]);
     V->Znach=V->Znach+F*b->Tk0[1][i];
    }
    if(EtchoNet){
     fprintf(OPRFILE,"<h4 align=\"center\"><font color=\"#000080\">"
      "���������� ����� ���:</font></h4>\n");
     EtchoNet=false;
    }
    PrintForm(false,OPRFILE,V->Name,V->Znach); fputs("<br>\n",OPRFILE);
   }
   sprintf(Inf,"Z@%s",A->Name); *s='~';
   if(V=TakeVary(Inf)){
    V->Znach=b->Tk0[2][3];
    for(int i=0;i<3;i++){
     if(A->F) F=A->F[i]; else F.C=TakeCnst(A->A[i]);
     V->Znach=V->Znach+F*b->Tk0[2][i];
    }
    if(EtchoNet){
     fprintf(OPRFILE,"<h4 align=\"center\"><font color=\"#000080\">"
      "���������� ����� ���:</font></h4>\n");
     EtchoNet=false;
    }
    PrintForm(false,OPRFILE,V->Name,V->Znach); fputs("<br>\n",OPRFILE);
 }}}
 fclose(OPRFILE);
//�������� ������������ ������� �� ������ �.�.������
 if(FindInst("����� ������",&I)) LarinMetod(B);
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
bool Stringer::TakeRGCoords(Parm *P,char *Name){
 Form Fp0_1,Fp1_1,Fp2_1,Fp3_1,Fp0_2,Fp1_2,Fp2_2,Fp3_2,Fdp0,Fdp1,Fdp2,Fdp3,dx,dy,dz;
 if(P->Nam) Fp0_1.V=TakeVary(P->Nam); else return false; P=P->Sled;
 if(P->Nam) Fp1_1.V=TakeVary(P->Nam); else return false; P=P->Sled;
 if(P->Nam) Fp2_1.V=TakeVary(P->Nam); else return false; P=P->Sled;
 if(P->Nam) Fp3_1.V=TakeVary(P->Nam); else return false; P=P->Sled;
 if(P->Nam) Fp0_2.V=TakeVary(P->Nam); else return false; P=P->Sled;
 if(P->Nam) Fp1_2.V=TakeVary(P->Nam); else return false; P=P->Sled;
 if(P->Nam) Fp2_2.V=TakeVary(P->Nam); else return false; P=P->Sled;
 if(P->Nam) Fp3_2.V=TakeVary(P->Nam); else return false;
 fprintf(OPRFILE,"<h4 align=\"center\"><font color=\"#000080\">"
  "��������� �������-���������� ����������� �������� �������"
  "����� �������������� ������ ��� ������� %s:</font></h4>\n",Name);
 sprintf(Inf,"dp0@%s",Name); Fdp0.V=TakeVary(Inf);
 Fdp0.V->Znach=Fp0_1*Fp0_2+Fp1_1*Fp1_2+Fp2_1*Fp2_2+Fp3_1*Fp3_2;
 PrintForm(false,OPRFILE,Inf,Fdp0.V->Znach); fputs("&nbsp",OPRFILE);
 sprintf(Inf,"dp1@%s",Name); Fdp1.V=TakeVary(Inf);
 Fdp1.V->Znach=Fp0_1*Fp1_2-Fp1_1*Fp0_2-Fp2_1*Fp3_2+Fp3_1*Fp2_2;
 PrintForm(false,OPRFILE,Inf,Fdp1.V->Znach); fputs("<br>\n",OPRFILE);
 sprintf(Inf,"dp2@%s",Name); Fdp2.V=TakeVary(Inf);
 Fdp2.V->Znach=Fp0_1*Fp2_2-Fp2_1*Fp0_2-Fp3_1*Fp1_2+Fp1_1*Fp3_2;
 PrintForm(false,OPRFILE,Inf,Fdp2.V->Znach); fputs("&nbsp",OPRFILE);
 sprintf(Inf,"dp3@%s",Name); Fdp3.V=TakeVary(Inf);
 Fdp3.V->Znach=Fp0_1*Fp3_2-Fp3_1*Fp0_2-Fp1_1*Fp2_2+Fp2_1*Fp1_2;
 PrintForm(false,OPRFILE,Inf,Fdp3.V->Znach); fputs("<br>\n",OPRFILE);
 dA=2*(Fdp0*Fdp1-Fdp2*Fdp3);//2*(Fdp0*Fdp1+Fdp2*Fdp3);
 dB=2*(Fdp1*Fdp3+Fdp0*Fdp2);
 dG=2*(Fdp0*Fdp3+Fdp1*Fdp2);//2*(Fdp1*Fdp2-Fdp0*Fdp3);
 if((dX!=Pust)&&(dY!=Pust)&&(dZ!=Pust)){ Form S[3][3];
//������� �������� ��������� ���������� �� � �������������
  fprintf(OPRFILE,"<h4 align=\"center\"><font color=\"#000080\">"
   "������� �������� ��������� ������� � ���������� �� � ����������� � ��������� �� �������� %s ��:"
   "</font></h4>\n",Name);
  sprintf(Inf,"S11@%s",Name); S[0][0].V=TakeVary(Inf);
  S[0][0].V->Znach=2*Fp0_1*Fp0_1+2*Fp1_1*Fp1_1-1;
  PrintForm(false,OPRFILE,Inf,S[0][0].V->Znach);
  fputs("&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp",OPRFILE);
  sprintf(Inf,"S12@%s",Name); S[0][1].V=TakeVary(Inf);
  S[0][1].V->Znach=2*Fp1_1*Fp2_1+2*Fp0_1*Fp3_1;
  PrintForm(false,OPRFILE,Inf,S[0][1].V->Znach); fputs("&nbsp&nbsp",OPRFILE);
  sprintf(Inf,"S13@%s",Name); S[0][2].V=TakeVary(Inf);
  S[0][2].V->Znach=2*Fp1_1*Fp3_1-2*Fp0_1*Fp2_1;
  PrintForm(false,OPRFILE,Inf,S[0][2].V->Znach); fputs("<br>\n",OPRFILE);
  sprintf(Inf,"S21@%s",Name); S[1][0].V=TakeVary(Inf);
  S[1][0].V->Znach=2*Fp1_1*Fp2_1-2*Fp0_1*Fp3_1;
  PrintForm(false,OPRFILE,Inf,S[1][0].V->Znach); fputs("&nbsp&nbsp",OPRFILE);
  sprintf(Inf,"S22@%s",Name); S[1][1].V=TakeVary(Inf);
  S[1][1].V->Znach=2*Fp0_1*Fp0_1+2*Fp2_1*Fp2_1-1;
  PrintForm(false,OPRFILE,Inf,S[1][1].V->Znach);
  fputs("&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp",OPRFILE);
  sprintf(Inf,"S23@%s",Name); S[1][2].V=TakeVary(Inf);
  S[1][2].V->Znach=2*Fp3_1*Fp2_1+2*Fp0_1*Fp1_1;
  PrintForm(false,OPRFILE,Inf,S[1][2].V->Znach); fputs("<br>\n",OPRFILE);
  sprintf(Inf,"S31@%s",Name); S[2][0].V=TakeVary(Inf);
  S[2][0].V->Znach=2*Fp1_1*Fp3_1+2*Fp0_1*Fp2_1;
  PrintForm(false,OPRFILE,Inf,S[2][0].V->Znach); fputs("&nbsp&nbsp",OPRFILE);
  sprintf(Inf,"S32@%s",Name); S[2][1].V=TakeVary(Inf);
  S[2][1].V->Znach=2*Fp3_1*Fp2_1-2*Fp0_1*Fp1_1;
  PrintForm(false,OPRFILE,Inf,S[2][1].V->Znach); fputs("&nbsp&nbsp",OPRFILE);
  sprintf(Inf,"S33@%s",Name); S[2][2].V=TakeVary(Inf);
  S[2][2].V->Znach=2*Fp0_1*Fp0_1+2*Fp3_1*Fp3_1-1;
  PrintForm(false,OPRFILE,Inf,S[2][2].V->Znach); fputs("<br>\n",OPRFILE);
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
bool Stringer::Fill(char *Name){
 Parm *P=Parms; int i=KolElem(P),j,Ind; char Mes[512];
 if(i!=9) goto Err;
 for(i=0;P;P=P->Sled){
  if(P->Nam&&P->R){
   switch(P->Nam[0]){ Parm *p,*pp,**Pp;
    case 'E':
     if(P->R->Sled){
      sprintf(Inf,"������ ��������� (������ %d) ��� ���������� ��� �����!\n",Line);
      Error(Line,0,true); goto Err;
     }
     FE=TakeFormFromParm(P->R); i++;
     break;
    case 'G':
     if(P->R->Sled){
      sprintf(Inf,"������ ������ (������ %d) ��� ���������� ��� �����!\n",Line);
      Error(Line,0,true); goto Err;
     }
     FG=TakeFormFromParm(P->R); i++;
     break;
    case 'L':
     if(P->R->Sled){
      sprintf(Inf,"����� ������� (������ %d) ��� ���������� ��� �����!\n",Line);
      Error(Line,0,true); goto Err;
     }
     FL=TakeFormFromParm(P->R); i++;
     break;
    case 'F':
     if(P->R->Sled){
      sprintf(Inf,"������� ������� (������ %d) ��� ���������� ��� �����!\n",Line);
      Error(Line,0,true); goto Err;
     }
     FF=TakeFormFromParm(P->R); i++;
     break;
    case 'J':
     if(P->R->Sled){
      sprintf(Inf,"������ ������� (������ %d) ��� ���������� ��� �����!\n",Line);
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
      sprintf(Inf,"���������� ������ ������ ���� ������� � ������\n"
       "������� (������ %d) � ���������� 6-��!",Line);
      Error(Line,0,true); goto Err;
     }
     TakeDekartCoords(P->R);
     i++;
     break;
    case 'Q':
     if(KolElem(P->R)!=8){
      sprintf(Inf,"��������� �������-���������� ������� ������ ������ ����\n"
       "������� � ������ ������� (������ %d) � ���������� 8-��!",Line);
      Error(Line,0,true); goto Err;
     }
     if(!TakeRGCoords(P->R,Name)){
      sprintf(Inf,"� ������ ������� (������ %d) ������� ������ ���� �������\n"
       "���������� ���������� ������ ���� \"S(X1,Y1,Z1,X2,Y2,Z2)\"!",Line);
      Error(Line,0,true); goto Err;
     }
     i++;
     break;
    case 'R':
     if(KolElem(P->R)!=6){
      sprintf(Inf,"���� �������� ������� ������ ������ ���� �������\n"
       "� ������ ������� (������ %d) � ���������� 6-��!",Line);
      Error(Line,0,true); goto Err;
     }
     if(!TakeAnglsCoords(P->R)){
      sprintf(Inf,"� ������ ������� (������ %d) ������� ������ ���� �������\n"
       "���������� ���������� ������ ���� \"S(X1,Y1,Z1,X2,Y2,Z2)\"!",Line);
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
 if((FE==Pust)||(FG==Pust)||(FL==Pust)||(FF==Pust)||(FJx==Pust)||(FJy==Pust)||
    (FJz==Pust)||(dA==Pust)||(dB==Pust)||(dG==Pust)||(dX==Pust)||(dY==Pust)||
    (dZ==Pust))
     goto Err;
 return true;
Err:
 sprintf(Inf,"�������� ������� - ��� ������ ����:\n"
  "<���> \\ <���������>,<�����>;\n"
  "��� <���> - ��� �������;\n"
  "<���������> - ������ ���������� � �������������� ���������� ������� ����\n"
  "E(������ ���������), G(������ ������), L(�����),\n"
  "F(������� ����������� �������),\n"
  "Jx(Ix), Jy(Iy), Jz(Iz) - �������������� ������� ������� �������\n"
  "������������, ��������������, ���� ���� ������ � ���������� ���.\n"
  "<�����> - ������ ���������� ��������� �������, ����� ��������\n"
  "(��� ���������� �������-����������) ������ � ������� ������ ����\n"
  "S(3 ���������� ������ � 3 ���������� ������� �����),\n"
  "R(3 ���� ������ � 3 ���� ������� �������) ���\n"
  "Q(4 ��������� �������-���������� ��� ������ � 4 - ��� ������� �����);\n"
  "� ������� ��� ������ <�����> ����� ������ ����� ����������,\n"
  "��� ������ <���������> - ����� ������ ����� ���������� ��� �����.");
 Error(Line,0,true);
 return false;
}
//---------------------------------------------------------------------------
bool Stringer::Elems(){
 Form FEJx=FE*FJx,FEJy=FE*FJy,FGJz=FG*FJz,FEF=FE*FF;
 char NameCord[256],NameElem[256]; Vary *K; Elem *ECix,*ECiy,*ECk,*ECx,*ECy,*ECz;
 Cix=FEJx/FL; Ciy=FEJy/FL; Ck=FGJz/FL;
 Cx=12*FEJx/(FL^3); Cy=12*FEJy/(FL^3); Cz=FEF/FL;
 sprintf(NameCord,"dalfa@%s",Name);
 if(!(K=TakeVary(NameCord))){ IsError=true; return false; } K->Znach=dA;
 sprintf(NameElem,"C.%s",NameCord);
 if(!(ECix=TakeElem(NameElem))){ IsError=true; return false; }
 ECix->Znach=Cix; ECix->K=K;
 PrintForm(false,OPRFILE,ECix->Name,ECix->Znach); fputs("<br>\n",OPRFILE);
 PrintForm(false,OPRFILE,ECix->K->Name,ECix->K->Znach); fputs("<br>\n",OPRFILE);
 sprintf(NameCord,"dbeta@%s",Name);
 if(!(K=TakeVary(NameCord))){ IsError=true; return false; } K->Znach=dB;
 sprintf(NameElem,"C.%s",NameCord);
 if(!(ECiy=TakeElem(NameElem))){ IsError=true; return false; }
 ECiy->Znach=Ciy; ECiy->K=K;
 PrintForm(false,OPRFILE,ECiy->Name,ECiy->Znach); fputs("<br>\n",OPRFILE);
 PrintForm(false,OPRFILE,ECiy->K->Name,ECiy->K->Znach); fputs("<br>\n",OPRFILE);
 sprintf(NameCord,"dgama@%s",Name);
 if(!(K=TakeVary(NameCord))){ IsError=true; return false; } K->Znach=dG;
 sprintf(NameElem,"C.%s",NameCord);
 if(!(ECk=TakeElem(NameElem))){ IsError=true; return false; }
 ECk->Znach=Ck; ECk->K=K;
 PrintForm(false,OPRFILE,ECk->Name,ECk->Znach); fputs("<br>\n",OPRFILE);
 PrintForm(false,OPRFILE,ECk->K->Name,ECk->K->Znach); fputs("<br>\n",OPRFILE);
 sprintf(NameCord,"dx@%s",Name);
 if(!(K=TakeVary(NameCord))){ IsError=true; return false; } K->Znach=dX;
 sprintf(NameElem,"C.%s",NameCord);
 if(!(ECx=TakeElem(NameElem))){ IsError=true; return false; }
 ECx->Znach=Cx; ECx->K=K;
 PrintForm(false,OPRFILE,ECx->Name,ECx->Znach); fputs("<br>\n",OPRFILE);
 PrintForm(false,OPRFILE,ECx->K->Name,ECx->K->Znach); fputs("<br>\n",OPRFILE);
 sprintf(NameCord,"dy@%s",Name);
 if(!(K=TakeVary(NameCord))){ IsError=true; return false; } K->Znach=dY;
 sprintf(NameElem,"C.%s",NameCord);
 if(!(ECy=TakeElem(NameElem))){ IsError=true; return false; }
 ECy->Znach=Cy; ECy->K=K;
 PrintForm(false,OPRFILE,ECy->Name,ECy->Znach); fputs("<br>\n",OPRFILE);
 PrintForm(false,OPRFILE,ECy->K->Name,ECy->K->Znach); fputs("<br>\n",OPRFILE);
 sprintf(NameCord,"dz@%s",Name);
 if(!(K=TakeVary(NameCord))){ IsError=true; return false; } K->Znach=dZ;
 sprintf(NameElem,"C.%s",NameCord);
 if(!(ECz=TakeElem(NameElem))){ IsError=true; return false; }
 ECz->Znach=Cz; ECz->K=K;
 PrintForm(false,OPRFILE,ECz->Name,ECz->Znach); fputs("<br>\n",OPRFILE);
 PrintForm(false,OPRFILE,ECz->K->Name,ECz->K->Znach); fputs("<br>\n",OPRFILE);
 return true;
}
//---------------------------------------------------------------------------
bool Stringers(Stringer *T){ Root *I;
 OTLADKA=FindInst("�������",&I); Stringer *t,*bb; Inf[0]='\0';
 if(OTLADKA&&!access("OTLADKA",0)) unlink("OTLADKA");
 CoordWithoutDT(L);
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
 else OPRFILE=fopen(OprFile,"a");
 if(OTLADKA){
  FILE *F=fopen("Otladka","w"); Fputs("���������� ����������\n",F); fclose(F);
 }
 for(t=T;t;t=t->Sled){
  fprintf(OPRFILE,"<h3 align=\"center\"><font color=\"#800000\">"
   "������ ������� �������� � ������ \"%s\" ������ %d ������������ �������� � �������:"
   "</font></h3>\n",t->Name,t->Line);
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
bool Solids(Parm *Par,TRichEdit *RichEditOpr){   /* ������� ���� */
//��������� ��������� ������� ���� ����������� � ������� ������ ��������
//"������� ����" � ���������� �� ����������� ������ ���������� �����������
//�� ������ ���������. ������ ����� ��������� ��� ��������� "�������� ����"
 Body *b; Parm *p; int Line=0;
 for(Root *o=L->Oper;o;o=o->Sled) if(o->First==Par){ Line=o->Line; break; }
 for(Parm *P=Par;P;P=P->Sled){
  if(!P->Nam||!P->R) goto Err;
  CleanStr(P->Nam,Inf);
  if(b=TakeBody(Inf,strchr(Inf,'~'))){
   b->Line=Line;
   if(*P->R->Nam=='J'){
    b->KoordSys=P->R->Sled->R; P->R->Sled=NULL; b->Inert=P->R;
   }
   else{
    b->KoordSys=P->R;
    for(p=P->R;p;p=p->Sled){
     if(*p->Sled->Nam=='J') break;
     else if(*p->Sled->Nam=='m'){
      sprintf(Inf,"������ ����������� ���������� � ��������� ������ %d\n"
       "������ ���� ��� � ������� � ���������� \'J\'!",Line);
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
   sprintf(Inf,"������ ������� ������ \"������ ����\" \n"
    "� ��������� ������ %d)!",Line);
   Error(Line,0,true); goto Err;
 }}
 return true;
Err:
 sprintf(Inf,"�������� �������� ���� - ��� ������ ����:\n"
  "������� ���� ��� ������� ���� ��� �¨���� ���� ��� �¨���� ���� :\n"
  "<���>[~<��� ����>](<������ � ��������>,J(<��������� �������>));\n"
  "��� <���> � <��� ����> - ����� ��� - �������� � \"��������\";\n"
  "<������ � ��������> - ������ ��������� � ������ ������� ����\n"
  "Rx(���� �������� ������ ��� �),Ry(���� �������� ������ ��� y),\n"
  "Rz(���� �������� ������ ��� z),Sx(����� ����� ��� �),\n"
  "Sy(����� ����� ��� y),Sz(����� ����� ��� z);\n"
  "������� ����������������� ��������� - Rx(...),Ry(...),Rz(...)\n"
  "����� ���� ������� ��������� �������-���������� � ����:\n"
  " Q(p0,p1,p2,p3), ��� p0,p1,p2,p3 - ����������, ������������ ��� ���������;"
  "<��������� �������> - ������ ����������� ����������� ���������� ����\n"
  "m(�����), Jx(������ ������� ������������ ��� x), Jy(������ �������\n"
  "������������ ��� y), Jz(������ ������� ������������ ��� z).\n"
  "����� � ������� ����� ����� ���������� ��� �����. ����� 'J' �����������!\n");
 Error(Line,0,true);
 return false;
}
//---------------------------------------------------------------------------
bool ElasticRods(Parm *Par,TRichEdit *RichEditOpr){   /* ������� ������� */
//��������� ��������� ������� ���� ����������� � �������� ������ ��������
//"������� ��������" � ���������� �� ����������� ������ ���������� �����������
//�� ������ ���������. ������ ����� ��������� ��� ��������� "�������� �������"
 Stringer *r; Parm *p; int Line=0; char Nam[256];
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
   sprintf(Inf,"������ ������� ������ \"������� ��������\"\n"
    "� ��������� ������ %d)!",Line);
   Error(Line,0,true); goto Err;
 }}
 return true;
Err:
 sprintf(Inf,"�������� ������� - ��� ������ ����:\n"
  "������� ������� ��� ������� �������� : "\
  "<���> ( <���������>,<�����> );\n"
  "��� <���> - ��� �������; <���������> - ������ ���������� �\n"
  "�������������� ���������� ������� ���� - E(������ ���������),\n"
  "G(������ ������), L(�����), F(������� ����������� �������),\n"
  "Jx(Ix), Jy(Iy), Jz(Iz) - �������������� ������� ������� �������\n"
  "������������, ��������������, ���� ���� ������ � ���������� ���.\n"
  "<�����> - ������ ���������� ��������� �������, ����� ��������\n"
  "(��� ���������� �������-����������) ������ � ������� ������ ����\n"
  "S(3 ���������� ������ � 3 ���������� ������� �����),\n"
  "R(3 ���� ������ � 3 ���� ������� �������) ���\n"
  "Q(4 ��������� �������-���������� ��� ������ � 4 - ��� ������� �����);\n"
  "� ������� ��� ������ <�����> ����� ������ ����� ����������,\n"
  "��� ������ <���������> - ����� ������ ����� ���������� ��� �����.");
 Error(Line,0,true);
 return false;
}
//---------------------------------------------------------------------------
bool Quaternion(Parm *Par,TRichEdit *RichEditOpr){   /* ���������� */
//��������� ��������� ���������� ����������� � ������������
//������ ��� ���������� �������-���������� ��������� ��������,
//�������������� �������� ������������������ ��������� ��
 Parm *p; int Line=0; char Nam[256];
 Vary *p0=NULL,*p1=NULL,*p2=NULL,*p3=NULL,*V;
 Form Fp0,Fp1,Fp2,Fp3,Fip0,Fip1,Fip2,Fip3,FP0,FP1,FP2,FP3;
 Fp0.C=Odin; Fp1.C=Fp2.C=Fp3.C=Nul; Fip1.C=Fip2.C=Fip3.C=Nul;
 for(Root *o=L->Oper;o;o=o->Sled) if(o->First==Par){ Line=o->Line; break; }
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
 else OPRFILE=fopen(OprFile,"a");
 for(;Par;Par=Par->Sled){
  if(!Par->Nam||!Par->R){
   sprintf(Inf,"� ��������� \"����������\" ������ %s ��� ���������\n"
    "������ �������� � �������, ����� �������� ������\n"
    "������ \"Q\", \"Rx\", \"Ry\" ��� \"Rz\" (��.������)",Line);
   Error(Line,0,true); goto Err;
  }
  CleanStr(Par->Nam,Nam);
  switch(Nam[0]){
   case 'Q':
    if(Nam[1]){
     if(Nam[1]=='0'){
      p=Par->R;
      if(!p->Nam) goto Err; Fp0.V=TakeVary(p->Nam); p=p->Sled;
      if(!p->Nam) goto Err; Fp1.V=TakeVary(p->Nam); p=p->Sled;
      if(!p->Nam) goto Err; Fp2.V=TakeVary(p->Nam); p=p->Sled;
      if(!p->Nam) goto Err; Fp3.V=TakeVary(p->Nam);
     }
     else{
      sprintf(Inf,
       "� ��������� \"����������\" ������ %s ��������� �������-����������\n"
       "�������� ���� ���� ������� � ������� � ������������� \"Q0\" (��.������)",Line);
      Error(Line,0,true); goto Err;
    }}
    else{
     if(KolElem(Par->R)!=4){
      sprintf(Inf,
       "� ��������� \"����������\" ������ %s ���� �������\n"
       "4 ��������� �������-���������� (��.������)",Line);
      Error(Line,0,true); goto Err;
     }
     p=Par->R;
     if(!p->Nam) goto Err; p0=TakeVary(p->Nam); p=p->Sled;
     if(!p->Nam) goto Err; p1=TakeVary(p->Nam); p=p->Sled;
     if(!p->Nam) goto Err; p2=TakeVary(p->Nam); p=p->Sled;
     if(!p->Nam) goto Err; p3=TakeVary(p->Nam);
    }
    break;
   case 'R':
    if(!p0){
     sprintf(Inf,
      "� ��������� \"����������\" ������ %s ���� ��������\n"
      "������� ���������� � ������������ ���������� �������-\n"\
      "����������, � ����� ������������������ ���������",Line);
     Error(Line,0,true); goto Err;
    }
    if((Nam[1]!='x')&&(Nam[1]!='y')&&(Nam[1]!='z')){
      sprintf(Inf,
       "� ��������� \"����������\" ������ %s ��� �������� ���-\n"
       "�������� ������ ������ ('x','y' ��� 'z') ����� ����� 'R'",Line);
      Error(Line,0,true); goto Err;
    }
    TakeCoSinFi2(Par->R); Fip0=cosFi2;
    switch(Nam[1]){
     case 'x': Fip1=sinFi2; Fip2.C=Nul; Fip3.C=Nul; break;
     case 'y': Fip1.C=Nul; Fip2=sinFi2; Fip3.C=Nul; break;
     case 'z': Fip1.C=Nul; Fip2.C=Nul; Fip3=sinFi2; break;
    }
if(OTLADKA&&OPRFILE){
 fprintf(OPRFILE,"����������� �����������:<hr>\n(");
 PrintForm(false,OPRFILE,NULL,Fp0); fputs(", ",OPRFILE);
 PrintForm(false,OPRFILE,NULL,Fp1); fputs(", ",OPRFILE);
 PrintForm(false,OPRFILE,NULL,Fp2); fputs(", ",OPRFILE);
 PrintForm(false,OPRFILE,NULL,Fp3); fputs(") * (",OPRFILE);
 PrintForm(false,OPRFILE,NULL,Fip0); fputs(", ",OPRFILE);
 PrintForm(false,OPRFILE,NULL,Fip1); fputs(", ",OPRFILE);
 PrintForm(false,OPRFILE,NULL,Fip2); fputs(", ",OPRFILE);
 PrintForm(false,OPRFILE,NULL,Fip3); fputs(")\n<br>",OPRFILE);
}
    p0->Znach=Fp0*Fip0-Fp1*Fip1-Fp2*Fip2-Fp3*Fip3;
    p1->Znach=Fp0*Fip1+Fp1*Fip0+Fp2*Fip3-Fp3*Fip2;
    p2->Znach=Fp0*Fip2+Fp2*Fip0+Fp3*Fip1-Fp1*Fip3;
    p3->Znach=Fp0*Fip3+Fp3*Fip0+Fp1*Fip2-Fp2*Fip1;
    Fp0=p0->Znach; Fp1=p1->Znach; Fp2=p2->Znach; Fp3=p3->Znach;
if(OTLADKA&&OPRFILE){
 fprintf(OPRFILE,"�������� ����������:<hr>\n(");
 PrintForm(false,OPRFILE,NULL,p0->Znach); fputs(", ",OPRFILE);
 PrintForm(false,OPRFILE,NULL,p1->Znach); fputs(", ",OPRFILE);
 PrintForm(false,OPRFILE,NULL,p2->Znach); fputs(", ",OPRFILE);
 PrintForm(false,OPRFILE,NULL,p3->Znach); fputs(")<br>",OPRFILE);
 fputs("<br>",OPRFILE);
}
    break;
   default:
    sprintf(Inf,
     "� ��������� \"����������\" ������ %s\n"
     "������ ���������� �������-����������\n"
     "� ���� ��������� ������ ���� �������-\n"
     "�������� ���� ������ 'Q' ���� �������\n"
     "\"Rx\",\"Ry\" ��� \"Rz\")",Line);
    Error(Line,0,true); goto Err;
 }}
 if(OPRFILE){
  fprintf(OPRFILE,
   "<h3 align=\"center\"><font color=\"#800000\">"
   "�������� \"����������\" ������ %d ������������\n"
   "������� ��� ���������� �������-���������� � ����:</font></h3>\n",Line);
  PrintForm(false,OPRFILE,p0->Name,p0->Znach); fputc(' ',OPRFILE);
  PrintForm(false,OPRFILE,p1->Name,p1->Znach); fputc(' ',OPRFILE);
  PrintForm(false,OPRFILE,p2->Name,p2->Znach); fputc(' ',OPRFILE);
  PrintForm(false,OPRFILE,p3->Name,p3->Znach); fputs("<hr>",OPRFILE);
  fclose(OPRFILE);
 }
 return true;
Err:
 sprintf(Inf,
  "�������� ��������� �������� ��� ��������� ����������� - ��� ������ ����:\n"
  "���������� : [<���������_����>,]<���������>,<��������>;\n"
  "���\n"
  "<���������_����> - ������ ����������� ���������� �������-���������� -\n"
  "��������� ����������� �������� ����, ������������ �������� ����� �������-\n"
  "���� ������� ��� ��������, ���������� � ������� ������� � �������\n"
  "������� - ������� ��������� �����, ����� ����������� ��������� �����,\n"
  "����� �������� ������ ������ \"Q0\", ���� �������� ���������� ������������\n"
  "����������� ��, �� ����� ������ �� ����������;\n"
  "<���������> - ������ ����������� ���������� �������-���������� -\n"
  "��������� �������� �����������, ���������� � ������� ������� � �������\n"
  "������� - ������� ��������� �����, ����� ����������� ��������� �����,\n"
  "����� �������� ������ ������ ����� 'Q';\n"
  "<��������> - ������ ��������� ���� Rt(<��������� ��� ����������>) - �����\n"
  "t ����� ����� �������� 'x', 'y' ��� 'z', ��� ���������� ������ ����� ���\n"
  "���� ��������� ��, � <��������� ��� ����������> - �������� (� ��������)\n"
  "��� ���������� (��������� ����� ����������) �������� ���� ����� ��������.\n"
  "��������,\n"
  "���������� : Q0(p0,p1,p2,p3), Q(pA0,pA1,pA2,pA3), Rx(90), Ry(alfa);");
 Error(Line,0,true);
 return false;
}
//---------------------------------------------------------------------------

