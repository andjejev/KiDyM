#include "EditList.h"
#include "Structurs.h"
#include <io.h>

extern TRichEdit *RichEdit;

//Оператор служит для генерации структур некоторого множества точек
//твердого тела. При необходимости рассмотрения нескольких тел надо
//записать несколько таких операторов. Если к этим точкам приложены силы,
//то в результате получим структуры силовых элементов, задающих эти силы.
//Если это центр масс, получим структуры инерционных элементов, с ним связанных.
//Предполагается, что положение тела определяется смещениями некоторой его точки
//по осям считающейся неподвижной системы координат и углами малых поворотов
//вокруг осей такой системы. С этой точкой совпадает начало связанной системы
//координат. Поэтому в начале оператора (в секции "Обобщенные координаты(...),"
//или, просто, "Координаты(...)," задаются вариации начала координат связанной
//системы координат (абсциссы, ординаты и, в трехмерном случае, аппликаты)
//и вариации углов (угла в плоском случае) поворота тела вокруг осей
//(оси аппликаты) связанной системы. После этого в секции "Оси(...)" могут
//быть указаны наименования осей неподвижной системы координат, если это не
//"x", "y", "z" (в этом случае эту секцию можно не писать).
//Затем перечисляются точки, для которых следует сгенерировать структуры.
//Структуры - это выражения декартовых координат точки через указанные выше
//вариации (координаты) начала связанной системы, вариации углов поворота
//тела вокруг осей такой системы и заданные в связанной системе координаты
//точки. Для точек указываются имена и в скобках обозначение трех (двух)
//их координат. Если обозначений осей неподвижной системы не вводилось
//в секции "Оси(...)", то имена координат строятся по правилу
//   <имя_оси_x><имя_точки>, <имя_оси_y><имя_точки>[, <имя_оси_z><имя_точки>],
//иначе - по правилу     x<имя_точки>, y<имя_точки>[, z<имя_точки>].
//Таким образом, синтаксис оператора представляется следующим образом:
//      СТРУКТУРЫ:
//       [Обобщенные]Координаты(dxO,dyO[,dzO],[dalfa,dbeta,]dgamma),
//       [Оси(X,Y[,Z]),]
//       A(xA,yA[,zA]),B(xB,yB[,zB]),...;
//Здесь dxO,dyO,dzO,dalfa,dbeta,dgamma - переменные, задающие вариации
//координат или координаты начала и вариации углов поворота вокруг
//осей связаной системы координат или сами эти углы,
//A,B,... - обозначения точек множества,
//xA,yA,zA,xB,yB,zB,... - числовые константы или обозначения переменных,
//формулы которых определяют значения координат точек множества в связанной
//системе координат,
//X,Y,Z - обозначения осей координат точек, если они не есть x,y,z
//Всюду аппликаты указываются для трехмерного случая.

#define SKOBKA1  40
#define SKOBKA2  41
#define MINUS    45
#define TOCHKA   46

extern char ZNAC[],Inf[],OprFile[],WORKDIR[]; extern int MAXZNAC;
extern bool IsError,IsSyntError; extern Cnst *Pust,*Nul;
extern TEditList *EditList;
extern FILE *OPRFILE;

static char FullHead[256],*sp,*SP,S[512],B,Nam[256],NamOs[3][256];
static int Line;

char *Compress(char *S){ char *s;
 for(s=S;*s;s++); for(--s;*s==' ';s--); *(s+1)='\0'; return S;
}

int Znac(void){ int i=MAXZNAC; for(; i>0&&B!=ZNAC[i]; i--); return i; }

int LogZnac(void) { int i; char B1=B;
 switch(B=*sp++,B1){
  case '<': if(B=='=') return 3; i=1; break;
  case '>': if(B=='=') return 4; i=2; break;
  case '/': if(B=='=') return 5; i=0; break;
  default :
   IsSyntError=true; i=0;
   Application->MessageBox(L"Ожидалось: '<', '>', '<=', '>=', '/='.",
	L"СТРУКТУРЫ",MB_OK);
 }
 sp--; B=B1; return i;
}

int ReadStr(void){
 int Chto=0,i=0; bool Numb; wchar_t Inf[256];
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
	 swprintf(Inf,L"Запрещенный символ - '%c' в формуле \"%s\".",B,SP);
	 Application->MessageBox(Inf,L"SAVZarp",MB_OK);
	 IsError=true;
	 return 0;
 }} }
 L: if(B=*sp++){
  switch(B){
   case  ' ': goto L;
   case  '(':
	if(Numb){
	 swprintf(Inf,L"Вместо \"%s\" в формуле \"%s\" ожидалось имя.",sp-1,SP);
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
	 swprintf(Inf,L"Вместо \"%s\" в формуле \"%s\" ожидалась: цифра, 'E', 'e'.",
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
  char *Poin,*Form;
 public:
  void FreePoin(){ if(Poin) free(Poin); };
  void FreeForm(){ if(Form) free(Form); };
  void SetPoin(char *T);
  void SetForm(Vary *V);
};

void PointVar::SetPoin(char *T){
 Poin=(char *)calloc(strlen(T)+1,SzC); strcpy(Poin,T);
}

void PointVar::SetForm(Vary *V){
 AnsiString S1,S2;
 S1=V->Name; S1+="="; FormToStr(V->Znach,&S2); S1+=S2;
 Form=(char *)calloc(S1.Length()+1,SzC); strcpy(Form,S1.c_str());
}

class TListTV{
 private:
  PointVar *V,**Pv;
 public:
  TListTV(){ V=NULL; Pv=&V; };
  void Add(Vary *v);
  void AddPoin(char *t);
//  void View();
 ~TListTV();
};

TListTV::~TListTV(){
 for(PointVar *v;v=V;){ v->FreePoin(); v->FreeForm(); V=v->Sled; free(v); }
 Pv=&V;
}

void TListTV::AddPoin(char *T){ PointVar *v;
 *Pv=v=(PointVar *)calloc(1,sizeof(PointVar)); Pv=(PointVar **)v;
 v->SetPoin(T);
}

void TListTV::Add(Vary *V){ PointVar *v;
 *Pv=v=(PointVar *)calloc(1,sizeof(PointVar)); Pv=(PointVar **)v;
 v->SetForm(V);
}

class Tochka{
 public:
  Tochka *Sled; char *Name; Form F[3]; Vary *V[3];
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
   strcpy(Nam,P->Nam); LowToUpp(Nam);
   if(!strcmp(Nam,"ОБОБЩЕННЫЕ КООРДИНАТЫ")||!strcmp(Nam,"КООРДИНАТЫ")){
    IsOK=true; continue;
  }}
  if(!IsOS){
   strcpy(Nam,P->Nam); LowToUpp(Nam);
   if(!strcmp(Nam,"ОСИ")){ IsOS=true; continue; }
  }
  r=*PR=(Tochka *)calloc(1,sizeof(Tochka)); PR=(Tochka **)r; r->Name=P->Nam;
  for(i=0,p=P->R;p;i++,p=p->Sled){
   sprintf(Nam,"%s%s",NamOs[i],r->Name); r->V[i]=TakeVary(Nam);
   if(p->Nam) r->F[i]=StrToForm(p->Nam); else r->F[i].C=TakeCnst(p->Val);
}}}

void PointSet::Create(Tochka &S,Tochka &Rot){
 for(Tochka *t=R;t;t=t->Sled){
  t->V[0]->Znach=/*t->F[0]+*/S.F[0]+Rot.F[1]*t->F[2]-Rot.F[2]*t->F[1];
  t->V[1]->Znach=/*t->F[1]+*/S.F[1]+Rot.F[2]*t->F[0]-Rot.F[0]*t->F[2];
  if(_3D){
   t->V[2]->Znach=/*t->F[2]+*/S.F[2]+Rot.F[0]*t->F[1]-Rot.F[1]*t->F[0];
 }}
 if(OPRFILE=fopen(OprFile,access(OprFile,0)?"w":"a")){
  fprintf(OPRFILE,
   "<html>\n<head>\n<title>ПРОТОКОЛ ОПЕРАТОРА \"СТРУКТУРЫ\" строки %d</title>\n"
   "<meta http-equiv=\"Content-Type\" content=\"text/html;"
   "charset=windows-1251\">\n</head>\n"
   "<body bgcolor=\"#FFFFFF\" text=\"#800000\""
   " background=\"%s/fon2.jpg\""
   " font face=\"Times New Roman, Times, serif\">\n"
   "ПРОТОКОЛ ОПЕРАТОРА \"СТРУКТУРЫ\" строки %d<br>\n<br>\n",
    Line,WORKDIR,Line);
  sprintf(Inf,"ПРОТОКОЛ ОПЕРАТОРА \"СТРУКТУРЫ\" строки %d\n",Line);
//  RichEdit->Lines->Add(Inf);
  if(KolElem(R)>1){
   fprintf(OPRFILE,
    "Оператором созданы формулы координат точек:<br>\n");
//   RichEdit->Lines->Add("Оператором созданы формулы координат точек:\n");
  }
  else{
   fprintf(OPRFILE,
    "Оператором созданы формулы координат точки:<br>\n");
//   RichEdit->Lines->Add("Оператором созданы формулы координат точки:\n");
  }
  fputs("<table width=\"100%\" border=\"1\">\n",OPRFILE);
  fputs("<tr>",OPRFILE);
  fputs("<td><div align=\"center\"><b>Точка</b></div></td>\n"
   "<td><div align=\"center\"><b>Координаты</b></div></td>\n",OPRFILE);
  fputs("</tr>",OPRFILE);
  for(Tochka *t=R;t;t=t->Sled){
   fprintf(OPRFILE,"<tr>\n <td>%s</td>\n<td>&nbsp;</td>\n",t->Name);
   fputs("<tr>\n",OPRFILE);
   fprintf(OPRFILE," <td>&nbsp;</td>\n<td>");
   PrintForm(false,OPRFILE,t->V[0]->Name,t->V[0]->Znach);
   fputs("</td>\n</tr>\n",OPRFILE);
   fputs("<tr>\n",OPRFILE);
   fprintf(OPRFILE," <td>&nbsp;</td>\n<td>");
   PrintForm(false,OPRFILE,t->V[1]->Name,t->V[1]->Znach);
   fputs("</td>\n</tr>\n",OPRFILE);
   if(_3D){
    fputs("<tr>\n",OPRFILE);
    fprintf(OPRFILE," <td>&nbsp;</td>\n<td>");
    PrintForm(false,OPRFILE,t->V[2]->Name,t->V[2]->Znach);
    fputs("</td>\n</tr>\n",OPRFILE);
 }}}
 fputs("</table></font>",OPRFILE);
 fprintf(OPRFILE,"</body>\n</html>\n");
 fclose(OPRFILE);
}

int KolElem(Parm *R,char *Head){
 for(Parm *P=R;P;P=P->Sled){
  strcpy(Nam,P->Nam); LowToUpp(Nam);
  if(!strcmp(Nam,Head)) return KolElem(P->R);
 }
 return 0;
}

bool Structurs(Parm *Par,TRichEdit *RE){ wchar_t Inf[1024];
 Parm *P; int K,Ks; Tochka S,R; FILE *F;
 K=KolElem(Par,"ОБОБЩЕННЫЕ КООРДИНАТЫ"); if(!K) K=KolElem(Par,"КООРДИНАТЫ");
 RichEdit=RE; Line=Par->Line;
 if(K!=6&&K!=3){
  swprintf(Inf,
   L"Твердое тело в пространстве имеет 6 степеней свободы,\n"
   "на плоскости - 3, поэтому число элементов в списке\n"
   "инструкции \"ОБОБЩЕННЫЕ КООРДИНАТЫ\"\n"
   "оператора \"СТРУКТУРЫ\" строки %d\n"
   "должно равняться, соответственно, 6 или 3!",Par->Line);
  Application->MessageBox(Inf,L"ОШИБКА!",MB_OK); return false;
 }
 if((Ks=KolElem(Par,"ОСИ"))&&((K==6)&&(Ks!=3)||(K==3)&&(Ks!=2))){
  swprintf(Inf,
   L"Несогласоано число элементов в списках секций\n"
   "\"[ОБОБЩЕННЫЕ ]КООРДИНАТЫ\" и \"ОСИ\" оператора\n"
   "\"СТРУКТУРЫ\" строки %d.\n"
   "Для пространственного случая это должно быть 6 и 3,\n"
   "для плоского - 3 и 2, соответственно!",Par->Line);
  Application->MessageBox(Inf,L"ОШИБКА!",MB_OK); return false;
 }
 strcpy(NamOs[0],"x"); strcpy(NamOs[1],"y"); strcpy(NamOs[2],"z");
 PS._3D=(K==6);
 for(P=Par;P;P=P->Sled){ Parm *p;
  strcpy(Nam,P->Nam); LowToUpp(Nam);
  if(!strcmp(Nam,"ОБОБЩЕННЫЕ КООРДИНАТЫ")||!strcmp(Nam,"КООРДИНАТЫ")){
   p=S.Fill(P->R);
   if(PS._3D) R.Fill(p); else{ R.F[2]=S.F[2]; S.F[2].C=Nul; }
  }
  if(!strcmp(Nam,"ОСИ")){ int i;
   for(p=P->R,i=0;i<(PS._3D?3:2);i++,p=p->Sled){
	if(p->Nam) strcpy(NamOs[i],p->Nam);
	else{
	 swprintf(Inf,L"Наименование оси координат в секции\n"
	  "\"ОСИ\" оператора \"СТРУКТУРЫ\" строки %d должно быть именем!",Par->Line);
	 Application->MessageBox(Inf,L"ОШИБКА!",MB_OK); return false;
   }}
   break;
 }}
 PS.Fill(Par);
 PS.Create(S,R);
 return true;
}
