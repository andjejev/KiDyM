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

extern List *L; extern wchar_t SimbInd;
extern wchar_t ErrFile[],OprFile[],Inf[],WORKDIR[],FILESHAP[];
extern Cnst *Pust; extern FILE *OPRFILE;

bool ToClose(Parm *Par,TRichEdit *R,TCanvas *Canvas){
// wchar_t *inf=Inf,*s,NameInstr[256]; Vary *V,*W=NULL; Elem *e,*E=NULL;
/* wchar_t *inf=Inf,*s; Vary *V; Elem *E;
// FILE *F; Magazine *Mi=NULL,*m; Root *I; bool Mode;
//�����������
 if(OPRFILE=_wfopen(OprFile,L"w")){
  fwprintf(OPRFILE,
   L"<html>\n<head>\n<title>�������� ��������� \"����������\"</title>\n"
   L"<meta http-equiv=\"Content-Type\" content=\"text/html;"
   L"wchar_tset=windows-1251\">\n</head>\n"
   L"<body bgcolor=\"#FFFFFF\" text=\"#800000\""
   L" background=\"%s/fon2.jpg\""
   L" font face=\"Times New Roman, Times, serif\">\n",
    WORKDIR);
 }
 Inds=new TInds;
 for(Parm *p=Par;p;p=p->Sled){
  if(!p->Nam||!p->R){ Inds->~TInds(); goto Ex; }
  Inds->AddInd(p);
 }
 for(Vary *V=L->V;V;V=V->Sled)
  if((s=wcschr(V->Name,SimbInd))&&(s[1]=='(')) IndExp->AddInd(s);
//������� ��������������� ��������� �� ���������
 for(Elem *E=L->E;E;E=E->Sled){
  if((s=wcschr(E->Name,SimbInd))&&(s[1]=='(')) IndExp->AddInd(s); //*t='.';
 }
 if(W||E){
  fwprintf(OPRFILE,
   L"�������� \"����������\" ������������ �� ��������������� ���������<hr>\n");
  if(W){
   fwprintf(OPRFILE,L"<b>����������:</b><br>\n");
   fputws(L"<table width=\"100%\" border=\"1\">\n",OPRFILE);
   fputws(L"<tr>",OPRFILE);
   fputws(L"<td><div align=\"center\"><b>�������� ���������</b></div></td>\n"
    L"<td><div align=\"center\"><b>����������</b></div></td>\n</tr>\n",OPRFILE);
   for(V=W;V;V=V->Sled){
    if(V->Znach.C!=Pust){
     ListVE.AddProt(V);
     s=wcschr(V->Name,SimbInd);
     if(s[1]=='('){
      if(!IndExp->IndexForm(V,ListVE)){
       fclose(OPRFILE); goto Ex3;
     }}
     else{
      if(!Inds->Index(V,ListVE)){
       fclose(OPRFILE); goto Ex3;
   }}}}
   ListVE.VtoFile();
   fputws(L"</table>\n",OPRFILE);
  }
  if(E){
   fwprintf(OPRFILE,L"<b>��������:</b><br>\n");
   fputws(L"<table width=\"100%\" border=\"1\">\n",OPRFILE);
   fputws(L"<tr>",OPRFILE);
   fputws(L"<td><div align=\"center\"><b>�������� ���������</b></div></td>\n"
    L"<td><div align=\"center\"><b>��������</b></div></td>\n</tr>",OPRFILE);
   for(e=E;e;e=e->Sled){
    ListVE.AddProt(e);
    s=wcschr(E->Name,SimbInd);
    if(s[1]=='('){
     if(!IndExp->IndexForm(e,ListVE)){
      fclose(OPRFILE); goto Ex3;
    }}
    else{
     if(!Inds->Index(e,ListVE)){ fclose(OPRFILE); goto Ex3; }
   }}
   ListVE.EtoFile();
   fputws(L"</table\n",OPRFILE);
  }
  fputws(L"</font></body>\n</html>\n",OPRFILE);
 }
 fclose(OPRFILE);
 delete Inds; delete IndExp;
 return true;
Ex:swprintf(inf,
 L"�������� \"����������\" ������ ���� ������� � ����:<br>\n"
 L"���������� : <������-������1>(<��1>,<��1>[,<���1>]),<br>\n"
 L"             <������-������2>(<��2>,<��2>[,<���2>]),...;,<br>\n"
 L"��������,    ���������� : i(1,5),k(2,5,1);<br>\n");
 if(F=_wfopen(OprFile,L"a")){ Fputs(Inf,F); fclose(F); }
 Application->MessageBox(Inf,L"������ � ��������� \*����������\*:",MB_OK);
 return false;
Ex3: delete Inds; delete IndExp;
*/
 return false;
}

bool ToOpen(Parm *Par,TRichEdit *R,TCanvas *Canvas){
 return false;
}

