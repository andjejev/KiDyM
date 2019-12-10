#pragma package(smart_init)
//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include <io.h>
#include "CA.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)

extern Cnst *Nul,*Odin,*Pust,*MinusOdin;
extern List *L; extern bool IsError,Otladka;
extern FILE *OPRFILE;
extern char Inf[],OprFile[],WORKDIR[],AtrInerc,AtrElast,AtrForse,AtrDisip;

extern FILE *TXTFILE; extern char TxtFile[256];
//---------------------------------------------------------------------------
//    �������� ������� ������ �����������, ������� � ������������� ���������
// ���������� ������ ������-���������. ������ ��������� ��������: �������� ����,
// ������� ������������ ������� ����������������, ������ ��� ������ ���������
// � ��������� ������� ������� - h, ���������� ������� �� ���� � - i_max,
// y - j_max, z - k_max, �������� ������� ���� - a, ��������� �������� - ro,
// ������ ���� - �, ������������� ����� ���� ����� ������, �����������
// ���������� ��� ���� - bm � ���������� - bc, ���������� �� ����������
// � ������ �������������� ������������.
//   �������� �������� �������
//
// ������� �������������� : �[��](��������_��������������),�[�����](h),
//  x[-�������](i_max),y[-�������](j_max),z[-�������](k_max),
//  �[������ ����](a),�[��������](ro),�[����� ����](E),�[�]|�[�������� ���],
//  �[���������](bm,bc),�[������������� ������������];,
//
// ��� i_max, j_max, k_max, a, ro, E, bm, bc - �����, ���������� ��� �������,
// �������� ��������������� ��������; ��� ����������� ������ �������� ����
// ������� ����� ������ �� ����� ������ �������� ��� ��������� �����
//---------------------------------------------------------------------------
double MakeVal(Parm *P){ Form F; double Rez;
 if(P->Nam){ F=StrToForm(P->Nam); Rez=Val(F); }
 else Rez=P->Val;
 return Rez;
}
//---------------------------------------------------------------------------
Elem * NewElem(char AttrElem,char *NameCord,double m){
 char NameElem[256]; Elem *E;
 sprintf(NameElem,"%c.%s",AttrElem,NameCord);
 if(FindElem(NameElem,&E)){ double mold=E->Znach.C->Val; m+=mold; }
 else{ E=TakeElem(NameElem); E->K=TakeVary(NameCord); }
 E->Znach.C=TakeCnst(m);
 return E;
}
//---------------------------------------------------------------------------
void ThreeElems(char *NameBar,int n,double m,bool Weight,double b,Parm ***Pnu,
                double x0,double y0,double z0,int NLine){
 char NameCord[256],NameVelo[256]; Parm *p; Elem *E;
 sprintf(NameCord,"x%s@%3.3d",NameBar,n); E=NewElem(AtrInerc,NameCord,m);
 if(b>0.0) NewElem(AtrDisip,NameCord,b*m);
 **Pnu=p=(Parm *)calloc(1,sizeof(Parm)); *Pnu=(Parm **)p; p->Line=NLine;
 p->Nam=E->K->Name; p->R=(Parm *)calloc(1,sizeof(Parm)); p->R->Val=x0;
 sprintf(NameVelo,"%s'%s",NameCord,L->Time->Name);
 **Pnu=p=(Parm *)calloc(1,sizeof(Parm)); *Pnu=(Parm **)p; p->Line=NLine;
 p->Nam=strcpy((char *)calloc(strlen(NameVelo)+1,SzC),NameVelo);
 p->R=(Parm *)calloc(1,sizeof(Parm));
 sprintf(NameCord,"y%s@%3.3d",NameBar,n); E=NewElem(AtrInerc,NameCord,m);
 if(b>0.0) NewElem(AtrDisip,NameCord,b*m);
 **Pnu=p=(Parm *)calloc(1,sizeof(Parm)); *Pnu=(Parm **)p; p->Line=NLine;
 p->Nam=E->K->Name; p->R=(Parm *)calloc(1,sizeof(Parm)); p->R->Val=y0;
 sprintf(NameVelo,"%s'%s",NameCord,L->Time->Name);
 **Pnu=p=(Parm *)calloc(1,sizeof(Parm)); *Pnu=(Parm **)p; p->Line=NLine;
 p->Nam=strcpy((char *)calloc(strlen(NameVelo)+1,SzC),NameVelo);
 p->R=(Parm *)calloc(1,sizeof(Parm));
 sprintf(NameCord,"z%s@%3.3d",NameBar,n); E=NewElem(AtrInerc,NameCord,m);
 if(b>0.0) NewElem(AtrDisip,NameCord,b*m);
 **Pnu=p=(Parm *)calloc(1,sizeof(Parm)); *Pnu=(Parm **)p; p->Line=NLine;
 p->Nam=E->K->Name; p->R=(Parm *)calloc(1,sizeof(Parm)); p->R->Val=z0;
 sprintf(NameVelo,"%s'%s",NameCord,L->Time->Name);
 **Pnu=p=(Parm *)calloc(1,sizeof(Parm)); *Pnu=(Parm **)p; p->Line=NLine;
 p->Nam=strcpy((char *)calloc(strlen(NameVelo)+1,SzC),NameVelo);
 p->R=(Parm *)calloc(1,sizeof(Parm));
 if(Weight) NewElem(AtrForse,NameCord,m*9.81);
}
//---------------------------------------------------------------------------
void ElastElem(char *NameBar,int n1,int n2,double C,double a,double b,bool GN){
 Elem *E; char Cord[256],Def[256],Name[256],N1[16],N2[16],Name1[256],Name2[256];
 Cnst *A=TakeCnst(a); Form F,Fc; Fc.C=A;
 sprintf(N1,"%3.3d",n1<n2?n1:n2);   sprintf(N2,"%3.3d",n1<n2?n2:n1);
 sprintf(Name1,"%s@%s",NameBar,N1); sprintf(Name2,"%s@%s",NameBar,N2);
 sprintf(Name,"%s@%s_%s",NameBar,N1,N2);
 if(GN){//�������������� ������������
  sprintf(Cord,"delta%s",Name); E=NewElem(AtrElast,Cord,C);
  sprintf(Def,"sqrt((x%s-x%s)^2+(y%s-y%s)^2+(z%s-z%s)^2)",
   Name1,Name2,Name1,Name2,Name1,Name2);
  F=StrToForm(Def); F=F-Fc;
  if(E->K->Znach==Pust) E->K->Znach=F;
  if(b>0.0) NewElem(AtrDisip,Cord,b*C);
 }
 else{
}}
//---------------------------------------------------------------------------
void Animat(int n1,int n2,int n3,int n4,Parm ***P,char x,char y){
 char Name[256]; int L;
 Parm *p=**P=(Parm *)calloc(1,sizeof(Parm)); *P=(Parm **)p;
 p->Nam=strcpy((char *)calloc(7,SzC),"������");
 p->R=(Parm *)calloc(1,sizeof(Parm));
 L=sprintf(Name,"%c%s@%3.3d",x,n1);
 p->R->Nam=strcpy((char *)calloc(L+1,SzC),Name);

 p->R->Sled=(Parm *)calloc(1,sizeof(Parm));
 L=sprintf(Name,"%c%s@%3.3d",y,n1);
 p->R->Sled->Nam=strcpy((char *)calloc(L+1,SzC),Name);

 p->R->Sled->Sled=(Parm *)calloc(1,sizeof(Parm));
 L=sprintf(Name,"%c%s@%3.3d",x,n1);
 p->R->Sled->Sled->Nam=strcpy((char *)calloc(L+1,SzC),Name);

 p->R->Sled=(Parm *)calloc(1,sizeof(Parm));
 L=sprintf(Name,"%c%s@%3.3d",y,n1);
 p->R->Sled->Nam=strcpy((char *)calloc(L+1,SzC),Name);
}
//---------------------------------------------------------------------------
bool ElastBar(Parm *Par,TRichEdit *R){
 char NameBar[256],*s,B; Vary *K; Parm *p,**Pnu,**Paxy,**Paxz,**Payz; Root *I,*o;
 int i,j,k,i_max=0,j_max=0,k_max=0,n,n1,n2,n3,n4,n5,n6,n7,n8,N,NLine,KStlb=3;
 unsigned int LN;
 double m=0,a2,a=0.0,ro=0.0,E=0.0,mVal,Cp,Cr,bm=0.0,bc=0.0,x0,y0,z0;
 bool Weight,IsWeight=false,Geometry=false; Elem *J,*C,*D,*P;
 Otladka=FindInst("�������",&I);
 if(Otladka&&!access("Otladka",0)) unlink("Otladka");
 ClearVar();
//��������, ��� ���� ���
 for(p=Par;p;p=p->Sled){
  switch(p->Nam[0]){
   case '�': case '�':
    if(p->R&&!p->R->Sled&&p->R->Nam) strcpy(NameBar,p->R->Nam); break;
   case '�': case '�':
    if(p->R&&p->R->Sled&&p->R->Sled->Sled&&!p->R->Sled->Sled->Sled){
     x0=MakeVal(p->R); y0=MakeVal(p->R); z0=MakeVal(p->R);
    }
    else IsError=true; break;
   case 'x': case 'X':
    if(p->R&&!p->R->Sled) i_max=Round(MakeVal(p->R)); else IsError=true; break;
   case 'y': case 'Y':
    if(p->R&&!p->R->Sled) j_max=Round(MakeVal(p->R)); else IsError=true; break;
   case 'z': case 'Z':
    if(p->R&&!p->R->Sled) k_max=Round(MakeVal(p->R)); else IsError=true; break;
   case '�': case '�':
    if(p->R&&!p->R->Sled) a=MakeVal(p->R); else IsError=true; break;
   case '�': case '�':
    if(p->R&&!p->R->Sled) ro=MakeVal(p->R); else IsError=true; break;
   case '�': case '�':
    if(p->R&&!p->R->Sled) E=MakeVal(p->R); else IsError=true; break;
   case '�': case '�': Weight=true; IsWeight=true; break;
   case '�': case '�': Weight=false; IsWeight=true; break;
   case '�': case '�':
    if(p->R&&p->R->Sled&&!p->R->Sled->Sled){
     bm=MakeVal(p->R); bc=MakeVal(p->R->Sled);
    }
    else IsError=true; break;
   case '�': case '�': Geometry=true; break;
   default: IsError=true;
 }}
 if(IsError||!IsWeight||!i_max||!j_max||!k_max||(a==0.0)||(ro==0.0)||(E==0.0)){
  strcpy(Inf,
"   �������� ������� ������ �����������, ������� � ������������� ���������\n"
"�� ���������� ������ ������-���������. ������ ��������: �������� ����,\n"
"������� ������������ ������� ����������������,\n"
"������ ��� ������ ��������� � ��������� ������� ������� - h,\n"
"���������� ������� �� ���� � (i_max),y (j_max), z (k_max),\n"
"�������� ������� ���� - a, ��������� �������� - ro,\n"
"������ ���� - �, ������������� ����� ���� ����� ������, ������������\n"
"���������� ��� ���� � ����������, ���������� �� ���������� � ������\n"
"�������������� ������������.\n"
"   �������� �������� �������\n"
"������� �������������� : �[��](��������_��������������),�[�����](h),\n"
" x[-�������](i_max),y[-�������](j_max),z[-�������](k_max),�[������ ����](a),\n"
" �[��������](ro),�[����� ����](E),�[� ��������� ���]|�[�������� ���],\n"
" �[���������](bm,bc),�[������������ ������������];,\n\n"
" ��� � �������� i_max, j_max, k_max, a, ro, E, bm, bc ����� ��������� �����,\n"
" ���������� ��� �������, �������� ��������������� ��������;\n"
" ��� ����������� ������ �������� ���� ������� ����� ������ �� ����� ������\n"
" �������� ��� ��������� �����");
  Application->MessageBox(Inf,"������",MB_OK);
 }
//��� � ������� - ���������� ��������
 LN=strlen(NameBar);
 m=ro*a*a*a/8; N=i_max*j_max*k_max; Cp=E*a/10; Cr=2*Cp; a2=a*sqrt(2);
 for(o=L->Oper;o;o=o->Sled) if(o->First==Par){ NLine=o->Line; break; }
 for(o=L->Inst;o->Sled;o=o->Sled);
 o->Sled=(Root *)calloc(1,sizeof(Root)); Pnu=&(o->Sled->First);
 o->Sled->Name=strcpy((char *)calloc(20,SzC),"��������� �������");
 o->Sled->Line=NLine; o=o->Sled;
 o->Sled=(Root *)calloc(1,sizeof(Root)); Paxy=&(o->Sled->First);
 o->Sled->Name=strcpy((char *)calloc(20,SzC),"��������");
 o->Sled->Line=NLine; o=o->Sled;
 o->Sled=(Root *)calloc(1,sizeof(Root)); Paxz=&(o->Sled->First);
 o->Sled->Name=strcpy((char *)calloc(20,SzC),"��������");
 o->Sled->Line=NLine; o=o->Sled;
 o->Sled=(Root *)calloc(1,sizeof(Root)); Payz=&(o->Sled->First);
 o->Sled->Name=strcpy((char *)calloc(20,SzC),"��������");
 o->Sled->Line=NLine; o=o->Sled;
 *Pnu=p=(Parm *)calloc(1,sizeof(Parm)); Pnu=(Parm **)p;
 p->Nam=L->Time->Name; p->R=(Parm *)calloc(1,sizeof(Parm));
 *Paxy=p=(Parm *)calloc(1,sizeof(Parm)); Paxy=(Parm **)p;
 p->Nam=strcpy((char *)calloc(7,SzC),"������");
 *Paxz=p=(Parm *)calloc(1,sizeof(Parm)); Paxz=(Parm **)p;
 p->Nam=strcpy((char *)calloc(6,SzC),"�����");
 *Payz=p=(Parm *)calloc(1,sizeof(Parm)); Payz=(Parm **)p;
 p->Nam=strcpy((char *)calloc(6,SzC),"�����");
 z0=0.0;
 for(k=0;k<k_max;k++){ y0=0.0;
  for(j=0;j<j_max;j++){ x0=0.0;
   for(i=0;i<i_max;i++){
    n=i+i_max*(j+j_max*k);
            ThreeElems(NameBar,n,m,Weight,bm,&Pnu,x0,y0,z0,NLine);       n1=n;
    n++;
    if(n<N) ThreeElems(NameBar,n,m,Weight,bm,&Pnu,x0+a,y0,z0,NLine);     n2=n;
    n+=i_max-1;
    if(n<N) ThreeElems(NameBar,n,m,Weight,bm,&Pnu,x0,y0+a,z0,NLine);     n3=n;
    n++;
    if(n<N) ThreeElems(NameBar,n,m,Weight,bm,&Pnu,x0+a,y0+a,z0,NLine);   n4=n;
    n+=i_max*(j_max-1)-1;
    if(n<N) ThreeElems(NameBar,n,m,Weight,bm,&Pnu,x0,y0,z0+a,NLine);     n5=n;
    n++;
    if(n<N) ThreeElems(NameBar,n,m,Weight,bm,&Pnu,x0+a,y0,z0+a,NLine);   n6=n;
    n+=i_max-1;
    if(n<N) ThreeElems(NameBar,n,m,Weight,bm,&Pnu,x0,y0+a,z0+a,NLine);   n7=n;
    n++;
    if(n<N) ThreeElems(NameBar,n,m,Weight,bm,&Pnu,x0+a,y0+a,z0+a,NLine); n8=n;
    if(n1<N&&n2<N) ElastElem(NameBar,n1,n2,Cp,a,bc,Geometry);
    if(n1<N&&n3<N) ElastElem(NameBar,n1,n3,Cp,a,bc,Geometry);
    if(n3<N&&n4<N) ElastElem(NameBar,n3,n4,Cp,a,bc,Geometry);
    if(n2<N&&n4<N) ElastElem(NameBar,n2,n4,Cp,a,bc,Geometry);
    if(n1<N&&n5<N) ElastElem(NameBar,n1,n5,Cp,a,bc,Geometry);
    if(n2<N&&n6<N) ElastElem(NameBar,n2,n6,Cp,a,bc,Geometry);
    if(n5<N&&n6<N) ElastElem(NameBar,n5,n6,Cp,a,bc,Geometry);
    if(n5<N&&n7<N) ElastElem(NameBar,n5,n7,Cp,a,bc,Geometry);
    if(n6<N&&n8<N) ElastElem(NameBar,n6,n8,Cp,a,bc,Geometry);
    if(n3<N&&n7<N) ElastElem(NameBar,n3,n7,Cp,a,bc,Geometry);
    if(n4<N&&n8<N) ElastElem(NameBar,n4,n8,Cp,a,bc,Geometry);
    if(n7<N&&n8<N) ElastElem(NameBar,n7,n8,Cp,a,bc,Geometry);
    if(n1<N&&n6<N) ElastElem(NameBar,n1,n6,Cr,a2,bc,Geometry);
    if(n2<N&&n5<N) ElastElem(NameBar,n2,n5,Cr,a2,bc,Geometry);
    if(n1<N&&n7<N) ElastElem(NameBar,n1,n7,Cr,a2,bc,Geometry);
    if(n3<N&&n5<N) ElastElem(NameBar,n3,n5,Cr,a2,bc,Geometry);
    if(n2<N&&n8<N) ElastElem(NameBar,n2,n8,Cr,a2,bc,Geometry);
    if(n4<N&&n6<N) ElastElem(NameBar,n4,n6,Cr,a2,bc,Geometry);
    if(n1<N&&n4<N) ElastElem(NameBar,n1,n4,Cr,a2,bc,Geometry);
    if(n2<N&&n3<N) ElastElem(NameBar,n2,n3,Cr,a2,bc,Geometry);
    if(n4<N&&n7<N) ElastElem(NameBar,n4,n7,Cr,a2,bc,Geometry);
    if(n3<N&&n8<N) ElastElem(NameBar,n3,n8,Cr,a2,bc,Geometry);
    if(n5<N&&n8<N) ElastElem(NameBar,n5,n8,Cr,a2,bc,Geometry);
    if(n6<N&&n7<N) ElastElem(NameBar,n6,n7,Cr,a2,bc,Geometry);
//    if(!i) Animat(n1,n3,n7,n5,&Payz);//�����
//    else if(!j) Animat(n1,n2,n6,n5,&Paxz);//�����
//    else if(!k) Animat(n1,n2,n4,n3,&Payz);//������
    x0+=a;
   }
   y0+=a;
  }
  z0+=a;
 }
 for(j=0;j<j_max;j++){
  for(k=0;k<k_max;k++){
   n=i_max*(j+j_max*k); n1=n;
   n++; n2=n;
   n+=i_max*j_max-1; n3=n;
   n++; n4=n;
 }}
 strcpy(TxtFile,OprFile); if(s=strrchr(TxtFile,'.')) strcpy(s+1,"txt");
 if((OPRFILE=fopen(OprFile,access(OprFile,0)?"w":"a"))&&
    (TXTFILE=fopen(TxtFile,access(TxtFile,0)?"w":"a"))){
  fprintf(OPRFILE,
   "<html>\n<head>\n<title>�������� ��������� \"������� ��������������\" ������ %d</title>\n"
   "<meta http-equiv=\"Content-Type\" content=\"text/html;"
   "charset=windows-1251\">\n</head>\n"
   "<body bgcolor=\"#FFFFFF\" text=\"#800000\""
   " background=\"%s/fon2.jpg\""
   " font face=\"Times New Roman, Times, serif\">\n",NLine,WORKDIR);
  sprintf(Inf,"�������� ��������� \"������� ��������������\" ������ %d\n\n\n",NLine);
  Fputs(Inf,TXTFILE);
  fputs("������������� ��������� ����������� ��������:<br>\n<br>\n",OPRFILE);
  sprintf(Inf,"������������� ��������� ����������� ��������:\n\n");
  Fputs(Inf,TXTFILE);
  fputs("<table width=\"100%\" border=\"1\">\n",OPRFILE);
  fputs("<tr>",OPRFILE);
  for(i=0;i<KStlb;i++)
   fputs(" <td><div align=\"center\"><b>��������</b></div></td>\n",OPRFILE);
  fputs("</tr>",OPRFILE);
  for(J=L->E,i=0;J;J=J->Sled){
   if(J->Name[0]!=AtrInerc) continue;
   if(strlen(J->Name)<(LN+3)) continue;
   B=J->Name[LN+3]; J->Name[LN+3]='\0';
   if(!strcmp(J->Name+3,NameBar)){
    if(!i) fputs("<tr>\n",OPRFILE);
    else if(!(i%KStlb)) fputs("</tr>\n<tr>\n",OPRFILE);
    J->Name[LN+3]=B; i++;
    fputs(" <td><div align=\"center\"><b>",OPRFILE);
    PrintForm(false,OPRFILE,J->Name,J->Znach);
    fputs("</b></div></td>\n",OPRFILE);
   }
   J->Name[LN+3]=B;
  }
  fputs("</tr>",OPRFILE);
  fputs("</table\n",OPRFILE);
  fputs("������������� ��������� ������� ��������:<br>\n<br>\n",OPRFILE);
  sprintf(Inf,"������������� ��������� ������� ��������:\n\n");
  Fputs(Inf,TXTFILE);
  fputs("<table width=\"100%\" border=\"1\">\n",OPRFILE);
  fputs("<tr>",OPRFILE);
  for(i=0;i<KStlb;i++)
   fputs(" <td><div align=\"center\"><b>��������</b></div></td>\n",OPRFILE);
  fputs("</tr>",OPRFILE);
  for(C=L->E,i=0;C;C=C->Sled){
   if(C->Name[0]!=AtrElast) continue;
   if(strlen(C->Name)<(LN+3)) continue;
   B=C->Name[LN+3]; C->Name[LN+3]='\0';
   if(!strcmp(C->Name+3,NameBar)){
    if(!i) fputs("<tr>\n",OPRFILE);
    else if(!(i%KStlb)) fputs("</tr>\n<tr>\n",OPRFILE);
    C->Name[LN+3]=B; i++;
    fputs(" <td><div align=\"center\"><b>",OPRFILE);
    PrintForm(false,OPRFILE,C->Name,C->Znach);
    fputs("</b></div></td>\n",OPRFILE);
   }
   C->Name[LN+3]=B;
  }
  fputs("</tr>",OPRFILE);
  fputs("</table\n",OPRFILE);
  if((bm>0.0)||(bc>0.0)){
   fputs("������������� ��������� ������������� ��������:<br>\n<br>\n",OPRFILE);
   sprintf(Inf,"������������� ��������� ������������� ��������:\n\n");
   Fputs(Inf,TXTFILE);
   fputs("<table width=\"100%\" border=\"1\">\n",OPRFILE);
   fputs("<tr>",OPRFILE);
   for(i=0;i<KStlb;i++)
    fputs(" <td><div align=\"center\"><b>��������</b></div></td>\n",OPRFILE);
   fputs("</tr>",OPRFILE);
   for(D=L->E,i=0;D;D=D->Sled){
    if(D->Name[0]!=AtrElast) continue;
    if(strlen(D->Name)<(LN+3)) continue;
    B=D->Name[LN+3]; D->Name[LN+3]='\0';
    if(!strcmp(D->Name+3,NameBar)){
     if(!i) fputs("<tr>\n",OPRFILE);
     else if(!(i%KStlb)) fputs("</tr>\n<tr>\n",OPRFILE);
     D->Name[LN+3]=B; i++;
     fputs(" <td><div align=\"center\"><b>",OPRFILE);
     PrintForm(false,OPRFILE,D->Name,D->Znach);
     fputs("</b></div></td>\n",OPRFILE);
    }
    D->Name[LN+3]=B;
   }
   fputs("</tr>",OPRFILE);
   fputs("</table\n",OPRFILE);
  }
  if(Weight){
   fputs("������������� ��������� ������� �������� �������:<br>\n<br>\n",OPRFILE);
   sprintf(Inf,"������������� ��������� ������� �������� �������:\n\n");
   Fputs(Inf,TXTFILE);
   fputs("<table width=\"100%\" border=\"1\">\n",OPRFILE);
   fputs("<tr>",OPRFILE);
   for(i=0;i<KStlb;i++)
    fputs(" <td><div align=\"center\"><b>��������</b></div></td>\n",OPRFILE);
   fputs("</tr>",OPRFILE);
   for(P=L->E,i=0;P;P=P->Sled){
    if(P->Name[0]!=AtrForse) continue;
    if(strlen(P->Name)<(LN+3)) continue;
    B=P->Name[LN+3]; P->Name[LN+3]='\0';
    if(!strcmp(P->Name+3,NameBar)){
     if(!i) fputs("<tr>\n",OPRFILE);
     else if(!(i%KStlb)) fputs("</tr>\n<tr>\n",OPRFILE);
     P->Name[LN+3]=B; i++;
     fputs(" <td><div align=\"center\"><b>",OPRFILE);
     PrintForm(false,OPRFILE,P->Name,P->Znach);
     fputs("</b></div></td>\n",OPRFILE);
    }
    P->Name[LN+3]=B;
   }
   fputs("</tr>",OPRFILE);
   fputs("</table\n",OPRFILE);
  }
  fputs("</font></body>\n</html>\n",OPRFILE);
  fclose(OPRFILE); fclose(TXTFILE);
 }
 return true;
}
//---------------------------------------------------------------------------

