#include "EditList.h"
#include "Structurs.h"
#include <io.h>

extern TRichEdit *RichEdit;

//�������� ������ ��� ��������� �������� ���������� ��������� �����
//�������� ����. ��� ������������� ������������ ���������� ��� ����
//�������� ��������� ����� ����������. ���� � ���� ������ ��������� ����,
//�� � ���������� ������� ��������� ������� ���������, �������� ��� ����.
//���� ��� ����� ����, ������� ��������� ����������� ���������, � ��� ���������.
//��������������, ��� ��������� ���� ������������ ���������� ��������� ��� �����
//�� ���� ����������� ����������� ������� ��������� � ������ ����� ���������
//������ ���� ����� �������. � ���� ������ ��������� ������ ��������� �������
//���������. ������� � ������ ��������� (� ������ L"���������� ����������(...),L"
//���, ������, L"����������(...),L" �������� �������� ������ ��������� ���������
//������� ��������� (��������, �������� �, � ���������� ������, ���������)
//� �������� ����� (���� � ������� ������) �������� ���� ������ ����
//(��� ���������) ��������� �������. ����� ����� � ������ L"���(...)" �����
//���� ������� ������������ ���� ����������� ������� ���������, ���� ��� ��
//"x", L"y", L"z" (� ���� ������ ��� ������ ����� �� ������).
//����� ������������� �����, ��� ������� ������� ������������� ���������.
//��������� - ��� ��������� ���������� ��������� ����� ����� ��������� ����
//�������� (����������) ������ ��������� �������, �������� ����� ��������
//���� ������ ���� ����� ������� � �������� � ��������� ������� ����������
//�����. ��� ����� ����������� ����� � � ������� ����������� ���� (����)
//�� ���������. ���� ����������� ���� ����������� ������� �� ���������
//� ������ L"���(...)", �� ����� ��������� �������� �� �������
//   <���_���_x><���_�����>, <���_���_y><���_�����>[, <���_���_z><���_�����>],
//����� - �� �������     x<���_�����>, y<���_�����>[, z<���_�����>].
//����� �������, ��������� ��������� �������������� ��������� �������:
//      ���������:
//       [����������]����������(dxO,dyO[,dzO],[dalfa,dbeta,]dgamma),
//       [���(X,Y[,Z]),]
//       A(xA,yA[,zA]),B(xB,yB[,zB]),...;
//����� dxO,dyO,dzO,dalfa,dbeta,dgamma - ����������, �������� ��������
//��������� ��� ���������� ������ � �������� ����� �������� ������
//���� �������� ������� ��������� ��� ���� ��� ����,
//A,B,... - ����������� ����� ���������,
//xA,yA,zA,xB,yB,zB,... - �������� ��������� ��� ����������� ����������,
//������� ������� ���������� �������� ��������� ����� ��������� � ���������
//������� ���������,
//X,Y,Z - ����������� ���� ��������� �����, ���� ��� �� ���� x,y,z
//����� ��������� ����������� ��� ����������� ������.

#define SKOBKA1  40
#define SKOBKA2  41
#define MINUS    45
#define TOCHKA   46

extern wchar_t ZNAC[],Inf[],OprFile[],WORKDIR[]; extern int MAXZNAC;
extern bool IsError,IsSyntError; extern Cnst *Pust,*Nul;
extern TEditList *EditList;
extern FILE *OPRFILE;

static wchar_t FullHead[256],*sp,*SP,S[512],B,Nam[256],NamOs[3][256];
static int Line;

wchar_t *Compress(wchar_t *S){ wchar_t *s;
 for(s=S;*s;s++); for(--s;*s==' ';s--); *(s+1)='\0'; return S;
}

int Znac(void){ int i=MAXZNAC; for(; i>0&&B!=ZNAC[i]; i--); return i; }

int LogZnac(void) { int i; wchar_t B1=B;
 switch(B=*sp++,B1){
  case '<': if(B=='=') return 3; i=1; break;
  case '>': if(B=='=') return 4; i=2; break;
  case '/': if(B=='=') return 5; i=0; break;
  default :
   IsSyntError=true; i=0;
   Application->MessageBox(L"���������: '<', '>', '<=', '>=', '/='.",
	L"���������",MB_OK);
 }
 sp--; B=B1; return i;
}

int ReadStr(void){
 int Chto=0,i=0; bool Numb;
M:if(B=*sp++){
  switch(B){
   case '\t': case  ' ': goto M;
   case '\0': S[i]='\0'; return PUST;
   case  '-': S[i++]=B; S[i]='\0'; return MINUS;
   case  '(': S[i++]=B; S[i]='\0'; return SKOBKA1;
   case  ')': S[i++]=B; S[i]='\0'; return SKOBKA2;
   case  '.': Chto=Double; Numb=true; S[i++]=B; break;
	 default:
	if(isdigit(B)){
	 if(Chto!=Double) Chto=Long; Numb=true; S[i++]=B; break;
	}
	if(isalpha(B)||isciril(B)||B=='['||B==']'||B=='{'||B=='}'){
	 Chto=NAME; Numb=false; S[i++]=B; break;
	}
	else{
	 swprintf(Inf,L"����������� ������ - '%c' � ������� \"%s\".",B,SP);
	 Application->MessageBox(Inf,L"SAVZarp",MB_OK);
	 IsError=true;
	 return 0;
 }} }
 L: if(B=*sp++){
  switch(B){
   case  ' ': goto L;
   case  '(':
	if(Numb){
	 swprintf(Inf,L"������ \"%s\" � ������� \"%s\" ��������� ���.",sp-1,SP);
	 Application->MessageBox(Inf,L"SAVZarp",MB_OK);
	 return 0;
	}
   case  ')': case  ',': case  '+': case  '*': case  '/': case  '%':
   case  '>': case  '<': case  '?': case  '|': case  '!': case  '=': break;
   case  '-': S[i]='\0'; Compress(S);
	if((B=S[i-1]=='E'||B=='e')&&Chto==Double){ S[i++]='-'; goto L; } B='-';
	break;
   case  '.': S[i++]=B; Chto=Double; Numb=true; goto L;
   default  :
	if((isalpha(B)||isciril(B))&&Numb){
	 swprintf(Inf,L"������ \"%s\" � ������� \"%s\" ���������: �����, 'E', 'e'.",
	  sp-1,SP);
	 Application->MessageBox(Inf,L"SAVZarp",MB_OK);
	 IsError=true;
	 return 0;
	}
	S[i++]=B; goto L;
 }}
 S[i]='\0'; Compress(S); return Chto;
}

class PointVar{
 public:
  PointVar *Sled;
 private:
  wchar_t *Poin,*Form;
 public:
  void FreePoin(){ if(Poin) free(Poin); };
  void FreeForm(){ if(Form) free(Form); };
  void SetPoin(wchar_t *T);
  void SetForm(Vary *V);
};

void PointVar::SetPoin(wchar_t *T){
 Poin=(wchar_t *)calloc(wcslen(T)+1,SzC); wcscpy(Poin,T);
}

void PointVar::SetForm(Vary *V){
 String S1,S2;
 S1=V->Name; S1+=L"=L"; FormToStr(V->Znach,&S2,false); S1+=S2;
 Form=(wchar_t *)calloc(S1.Length()+1,SzC); wcscpy(Form,S1.c_str());
}

class TListTV{
 private:
  PointVar *V,**Pv;
 public:
  TListTV(){ V=NULL; Pv=&V; };
  void Add(Vary *v);
  void AddPoin(wchar_t *t);
//  void View();
 ~TListTV();
};

TListTV::~TListTV(){
 for(PointVar *v;v=V;){ v->FreePoin(); v->FreeForm(); V=v->Sled; free(v); }
 Pv=&V;
}

void TListTV::AddPoin(wchar_t *T){ PointVar *v;
 *Pv=v=(PointVar *)calloc(1,sizeof(PointVar)); Pv=(PointVar **)v;
 v->SetPoin(T);
}

void TListTV::Add(Vary *V){ PointVar *v;
 *Pv=v=(PointVar *)calloc(1,sizeof(PointVar)); Pv=(PointVar **)v;
 v->SetForm(V);
}

class Tochka{
 public:
  Tochka *Sled; wchar_t *Name; Form F[3]; Vary *V[3];
  Tochka(){ Sled=NULL; Name=NULL; V[0]=V[1]=V[2]=NULL; F[0].C=F[1].C=F[2].C=Nul;}
  Parm *Fill(Parm *R);
};

Parm *Tochka::Fill(Parm *R){
 for(int i=0;i<3;i++){
  if(R->Nam) F[i]=StrToForm(R->Nam);
  else F[i].C=TakeCnst(R->Val);
  R=R->Sled;
 }
 return R;
}

class PointSet{
 public:
  Tochka *R; bool _3D;
  PointSet(){ R=NULL; _3D=false; }
  void Fill(Parm *R);
  void Create(Tochka &S,Tochka &R);
} PS;

void PointSet::Fill(Parm *P){
 Tochka *r,**PR=&R; R=NULL; int i; Parm *p; bool IsOK=false,IsOS=false;
 for(;P;P=P->Sled){
  if(!IsOK){
   wcscpy(Nam,P->Nam); LowToUpp(Nam);
   if(!wcscmp(Nam,L"���������� ����������")||!wcscmp(Nam,L"����������")){
    IsOK=true; continue;
  }}
  if(!IsOS){
   wcscpy(Nam,P->Nam); LowToUpp(Nam);
   if(!wcscmp(Nam,L"���")){ IsOS=true; continue; }
  }
  r=*PR=(Tochka *)calloc(1,sizeof(Tochka)); PR=(Tochka **)r; r->Name=P->Nam;
  for(i=0,p=P->R;p;i++,p=p->Sled){
   swprintf(Nam,L"%s%s",NamOs[i],r->Name); r->V[i]=TakeVary(Nam);
   if(p->Nam) r->F[i]=StrToForm(p->Nam); else r->F[i].C=TakeCnst(p->Val);
}}}

void PointSet::Create(Tochka &S,Tochka &Rot){
 for(Tochka *t=R;t;t=t->Sled){
  t->V[0]->Znach=/*t->F[0]+*/S.F[0]+Rot.F[1]*t->F[2]-Rot.F[2]*t->F[1];
  t->V[1]->Znach=/*t->F[1]+*/S.F[1]+Rot.F[2]*t->F[0]-Rot.F[0]*t->F[2];
  if(_3D){
   t->V[2]->Znach=/*t->F[2]+*/S.F[2]+Rot.F[0]*t->F[1]-Rot.F[1]*t->F[0];
 }}
 if(OPRFILE=_wfopen(OprFile,_waccess(OprFile,0)?L"w":L"a")){
  fwprintf(OPRFILE,
   L"<html>\n<head>\n<title>�������� ��������� \"���������\" ������ %d</title>\n"
   L"<meta http-equiv=\"Content-Type\" content=\"text/html;"
   L"wchar_tset=windows-1251\">\n</head>\n"
   L"<body bgcolor=\"#FFFFFF\" text=\"#800000\""
   L" background=\"%s/fon2.jpg\""
   L" font face=\"Times New Roman, Times, serif\">\n"
   L"�������� ��������� \"���������\" ������ %d<br>\n<br>\n",
    Line,WORKDIR,Line);
  swprintf(Inf,L"�������� ��������� \"���������\" ������ %d\n",Line);
//  RichEdit->Lines->Add(Inf);
  if(KolElem(R)>1){
   fwprintf(OPRFILE,
    L"���������� ������� ������� ��������� �����:<br>\n");
//   RichEdit->Lines->Add(L"���������� ������� ������� ��������� �����:\n");
  }
  else{
   fwprintf(OPRFILE,
    L"���������� ������� ������� ��������� �����:<br>\n");
//   RichEdit->Lines->Add(L"���������� ������� ������� ��������� �����:\n");
  }
  fputws(L"<table width=\"100%\" border=\"1\">\n",OPRFILE);
  fputws(L"<tr>",OPRFILE);
  fputws(L"<td><div align=\"center\"><b>�����</b></div></td>\n"
   L"<td><div align=\"center\"><b>����������</b></div></td>\n",OPRFILE);
  fputws(L"</tr>",OPRFILE);
  for(Tochka *t=R;t;t=t->Sled){
   fwprintf(OPRFILE,L"<tr>\n <td>%s</td>\n<td>&nbsp;</td>\n",t->Name);
   fputws(L"<tr>\n",OPRFILE);
   fwprintf(OPRFILE,L" <td>&nbsp;</td>\n<td>");
   PrintForm(false,OPRFILE,t->V[0]->Name,t->V[0]->Znach);
   fputws(L"</td>\n</tr>\n",OPRFILE);
   fputws(L"<tr>\n",OPRFILE);
   fwprintf(OPRFILE,L" <td>&nbsp;</td>\n<td>");
   PrintForm(false,OPRFILE,t->V[1]->Name,t->V[1]->Znach);
   fputws(L"</td>\n</tr>\n",OPRFILE);
   if(_3D){
    fputws(L"<tr>\n",OPRFILE);
    fwprintf(OPRFILE,L" <td>&nbsp;</td>\n<td>");
    PrintForm(false,OPRFILE,t->V[2]->Name,t->V[2]->Znach);
    fputws(L"</td>\n</tr>\n",OPRFILE);
 }}}
 fputws(L"</table></font>",OPRFILE);
 fwprintf(OPRFILE,L"</body>\n</html>\n");
 fclose(OPRFILE);
}

int KolElem(Parm *R,wchar_t *Head){
 for(Parm *P=R;P;P=P->Sled){
  wcscpy(Nam,P->Nam); LowToUpp(Nam);
  if(!wcscmp(Nam,Head)) return KolElem(P->R);
 }
 return 0;
}

bool Structurs(Parm *Par,TRichEdit *RE,TCanvas *Canvas){
 Parm *P; int K,Ks; Tochka S,R; FILE *F;
 K=KolElem(Par,L"���������� ����������"); if(!K) K=KolElem(Par,L"����������");
 RichEdit=RE; Line=Par->Line;
 if(K!=6&&K!=3){
  swprintf(Inf,
   L"������� ���� � ������������ ����� 6 �������� �������,\n"
   L"�� ��������� - 3, ������� ����� ��������� � ������\n"
   L"���������� \"���������� ����������\"\n"
   L"��������� \"���������\" ������ %d\n"
   L"������ ���������, ��������������, 6 ��� 3!",Par->Line);
  Application->MessageBox(Inf,L"������!",MB_OK); return false;
 }
 if((Ks=KolElem(Par,L"���"))&&((K==6)&&(Ks!=3)||(K==3)&&(Ks!=2))){
  swprintf(Inf,
   L"������������ ����� ��������� � ������� ������\n"
   L"\"[���������� ]����������\" � \"���\" ���������\n"
   L"\"���������\" ������ %d.\n"
   L"��� ����������������� ������ ��� ������ ���� 6 � 3,\n"
   L"��� �������� - 3 � 2, ��������������!",Par->Line);
  Application->MessageBox(Inf,L"������!",MB_OK); return false;
 }
 wcscpy(NamOs[0],L"x"); wcscpy(NamOs[1],L"y"); wcscpy(NamOs[2],L"z");
 PS._3D=(K==6);
 for(P=Par;P;P=P->Sled){ Parm *p;
  wcscpy(Nam,P->Nam); LowToUpp(Nam);
  if(!wcscmp(Nam,L"���������� ����������")||!wcscmp(Nam,L"����������")){
   p=S.Fill(P->R);
   if(PS._3D) R.Fill(p); else{ R.F[2]=S.F[2]; S.F[2].C=Nul; }
  }
  if(!wcscmp(Nam,L"���")){ int i;
   for(p=P->R,i=0;i<(PS._3D?3:2);i++,p=p->Sled){
	if(p->Nam) wcscpy(NamOs[i],p->Nam);
	else{
	 swprintf(Inf,L"������������ ��� ��������� � ������\n"
	  L"\"���\" ��������� \"���������\" ������ %d ������ ���� ������!",Par->Line);
	 Application->MessageBox(Inf,L"������!",MB_OK); return false;
   }}
   break;
 }}
 PS.Fill(Par);
 PS.Create(S,R);
 return true;
}
