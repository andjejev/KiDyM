#include "CA.h"
#include <io.h>

//�������� ������� ������ ��� ���������� �� ������ ��������� ��� ����������
// ������������� � ���, �������������, ��������� ��� ����������.
//�������� ������� ������ ��� �������������� � ������ ��������� ��� ����������
// ������������� � ���, ����� ����������� ���������� �������, �������������,
// ��������� ��� ����������.
//
// ��������� ���������� ����������
//      ������� : <������_����_���������_���_����������_���_����������>;
//      ������� : <������_����_���������_���_����������_���_��������������>;
//�������� ������� ������ �������� � ����������, ������������� � ���
// � ��������� ������, �������� ������� ��������� �� ������

extern List *L; extern char SimbInd;
extern char ErrFile[],OprFile[],Inf[],WORKDIR[],FILESHAP[];
extern Cnst *Pust; extern FILE *OPRFILE;

bool ToClose(Parm *Par,TRichEdit *R){
// char *inf=Inf,*s,NameInstr[256]; Vary *V,*W=NULL; Elem *e,*E=NULL;
/* char *inf=Inf,*s; Vary *V; Elem *E;
// FILE *F; Magazine *Mi=NULL,*m; Root *I; bool Mode;
//�����������
 if(OPRFILE=fopen(OprFile,"w")){
  fprintf(OPRFILE,
   "<html>\n<head>\n<title>�������� ��������� \"����������\"</title>\n"
   "<meta http-equiv=\"Content-Type\" content=\"text/html;"
   "charset=windows-1251\">\n</head>\n"
   "<body bgcolor=\"#FFFFFF\" text=\"#800000\""
   " background=\"%s/fon2.jpg\""
   " font face=\"Times New Roman, Times, serif\">\n",
    WORKDIR);
 }
 Inds=new TInds;
 for(Parm *p=Par;p;p=p->Sled){
  if(!p->Nam||!p->R){ Inds->~TInds(); goto Ex; }
  Inds->AddInd(p);
 }
 for(Vary *V=L->V;V;V=V->Sled)
  if((s=strchr(V->Name,SimbInd))&&(s[1]=='(')) IndExp->AddInd(s);
//������� ��������������� ��������� �� ���������
 for(Elem *E=L->E;E;E=E->Sled){
  if((s=strchr(E->Name,SimbInd))&&(s[1]=='(')) IndExp->AddInd(s); //*t='.';
 }
 if(W||E){
  fprintf(OPRFILE,
   "�������� \"����������\" ������������ �� ��������������� ���������<hr>\n");
  if(W){
   fprintf(OPRFILE,"<b>����������:</b><br>\n");
   fputs("<table width=\"100%\" border=\"1\">\n",OPRFILE);
   fputs("<tr>",OPRFILE);
   fputs("<td><div align=\"center\"><b>�������� ���������</b></div></td>\n"
    "<td><div align=\"center\"><b>����������</b></div></td>\n</tr>\n",OPRFILE);
   for(V=W;V;V=V->Sled){
    if(V->Znach.C!=Pust){
     ListVE.AddProt(V);
     s=strchr(V->Name,SimbInd);
     if(s[1]=='('){
      if(!IndExp->IndexForm(V,ListVE)){
       fclose(OPRFILE); goto Ex3;
     }}
     else{
      if(!Inds->Index(V,ListVE)){
       fclose(OPRFILE); goto Ex3;
   }}}}
   ListVE.VtoFile();
   fputs("</table>\n",OPRFILE);
  }
  if(E){
   fprintf(OPRFILE,"<b>��������:</b><br>\n");
   fputs("<table width=\"100%\" border=\"1\">\n",OPRFILE);
   fputs("<tr>",OPRFILE);
   fputs("<td><div align=\"center\"><b>�������� ���������</b></div></td>\n"
    "<td><div align=\"center\"><b>��������</b></div></td>\n</tr>",OPRFILE);
   for(e=E;e;e=e->Sled){
    ListVE.AddProt(e);
    s=strchr(E->Name,SimbInd);
    if(s[1]=='('){
     if(!IndExp->IndexForm(e,ListVE)){
      fclose(OPRFILE); goto Ex3;
    }}
    else{
     if(!Inds->Index(e,ListVE)){ fclose(OPRFILE); goto Ex3; }
   }}
   ListVE.EtoFile();
   fputs("</table\n",OPRFILE);
  }
  fputs("</font></body>\n</html>\n",OPRFILE);
 }
 fclose(OPRFILE);
 delete Inds; delete IndExp;
 return true;
Ex:sprintf(inf,
 "�������� \"����������\" ������ ���� ������� � ����:<br>\n"
 "���������� : <������-������1>(<��1>,<��1>[,<���1>]),<br>\n"
 "             <������-������2>(<��2>,<��2>[,<���2>]),...;,<br>\n"
 "��������,    ���������� : i(1,5),k(2,5,1);<br>\n");
 if(F=fopen(OprFile,"a")){ Fputs(Inf,F); fclose(F); }
 Application->MessageBox(Inf,"������ � ��������� \*����������\*:",MB_OK);
 return false;
Ex3: delete Inds; delete IndExp;
*/
 return false;
}

bool ToOpen(Parm *Par,TRichEdit *R){
 return false;
}

