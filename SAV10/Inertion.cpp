//---------------------------------------------------------------------------
#pragma hdrstop

#include <vcl.h>
#include <sys/stat.h>
#include <io.h>
#include "math.h"
#include "EditList.h"
#include "Inertion.h"
#include "Manipul.h"

extern TRichEdit *RichEdit;
//---------------------------------------------------------------------------
#pragma package(smart_init)

//Оператор служит для вычисления и генерации переменных массы и элементов
//тензора инерции твердого тела. Тело разбивается на элементы согласно
//справочника Фаворин М.В. Моменты инерции. Справочник. Под ред. М.М. Гернета.
//Изд., перераб. и доп. М., Машиностроение, 1977. Список таких элементов
//вместе со значениями геометрических параметров, характерных для этого элемента
//и составляет правую часть оператора. Таким образом, синтаксис оператора
//следующий

//ТЕНЗОР ИНЕРЦИИ : <Название тела>[(ro=<значение плотности тела>)],
//                 <э|ЭNNN[b]>(простой список вида <параметр>=<значение>, ... ;

//Здесь ro - обозначение параметра "плотность" (менять нельзя!),
//      <значение плотности тела> - число или переменная, имеющая формульное
//                                  или числовое значение в исходных данных,
//                                  задающие значение плотности тела;
//                                  эта плотность указывается,
//                                  если плотность элементов тела одинакова;
//      <э|ЭNNN[b]> - обозначение элемента из Справочника - 'э'('Э') - начальная
//                    буква слова "элемент", NNN - трехзначный номер элемента по
//                    Справочнику - если номер содержит меньше цифр, чем три, то
//                    слева дописываются нули, [b] - необязательный символ -
//                    буква, использующаяся иногда в Справочнике, например, 'а',
//                    'б', ... ;
//      <параметр> - имя геометрического параметра, используемого в формулах
//                   Справочника для этого элемента и имеющего область видимости
//                   только в формулах для этого элемента;
//      <значение> - число или переменная, имеющая формульное или числовое
//                   значение в исходных данных, задающие значение параметра;
//                   таких пар <параметр>=<значение> в списке элемента должно
//                   быть столько, сколько требуется для этого элемента согласно
//                   его рисунка и формул для инерционных параметров в Справочнике,
//                   плюс задания плотности (если она не задана в названии тела или
//                   отличается от заданной там), плюс последовательности сдигов и
//                   поворотов системы координат (СК) тела до совмещения ее со
//                   связанной с элементом СК, причем эта последняя система
//                   выбирается так, чтобы она отстояла от центра масс элемента
//                   только на нетривиальные значения вдоль осей координат xo,
//                   yo, zo, приведенные в Справочнике,
//                   например, для однородного цилиндра эта связанная СК уже
//                   будет распложена в ц.м., а для сегмента шара - смещенной
//                   вдоль его оси симметрии на заданную в Справочнике величину.
//Если в качестве значений используются переменные или формулы, то надо иметь
//в виду, что переменные, входящие в эти формулы (т.е. стоящие справа от знака
//равенства) должны быть определены в основном файле данных, а имена переменных
//слева от знака равенства должны использоваться в выражениях Справочника.
TInertBD *PInertBD;
extern TFormEditList *FormEditList;
extern char Inf[],WORKDIR[];
extern List *L,*LBeg;
extern TEditList *EditList;
extern Cnst *Odin,*Nul,*Pust;
Form Fro,Fm,FV,Fxo,Fyo,Fzo,FJyoOzo,FJzoOxo,FJxoyo,FJYoOZo,FJZoOXo,FJXoOYo;
extern bool OTLADKA;
extern FILE *OPRFILE; extern char OprFile[];
//---------------------------------------------------------------------------
static void PrintFormMatr(char *File,char *Comm,char *NameMatr,Form M[3][3]){
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
static void PrintFormVect(char *File,char *Comm,char *NameVect,Form V[3]){
 FILE *F=fopen(File,"a"); char Name[256];
 sprintf(Name,"\n%s\n",Comm); fputs(StrWINtoDOS(Name),F);
 for(int i=0;i<3;i++){
  sprintf(Name,"%s[%d]",NameVect,i);
  PrintForm(true,F,Name,V[i]); fputc('\n',F);
 }
 fclose(F);
}
//---------------------------------------------------------------------------
InertElem::InertElem(char *S,Magazine **Mc,Magazine **Mt){ char *s,Buf[256];
 Sled=NULL;
 s=Pole(S); strcpy(Buf,S); StrDOStoWIN(Buf); Clas=TakeMagazine(Mc,Buf)->S; S=s;
 s=Pole(S); strcpy(Buf,S); StrDOStoWIN(Buf); Typ =TakeMagazine(Mt,Buf)->S; S=s;
 s=Pole(S);   Numb=StrDOStoWIN(strcpy((char *)calloc(strlen(S)+1,SzC),S)); S=s;
 s=Pole(S);      V=StrDOStoWIN(strcpy((char *)calloc(strlen(S)+1,SzC),S)); S=s;
 s=Pole(S);     xo=StrDOStoWIN(strcpy((char *)calloc(strlen(S)+1,SzC),S)); S=s;
 s=Pole(S);     yo=StrDOStoWIN(strcpy((char *)calloc(strlen(S)+1,SzC),S)); S=s;
 s=Pole(S);     zo=StrDOStoWIN(strcpy((char *)calloc(strlen(S)+1,SzC),S)); S=s;
 s=Pole(S); JyoOzo=StrDOStoWIN(strcpy((char *)calloc(strlen(S)+1,SzC),S)); S=s;
 s=Pole(S); JzoOxo=StrDOStoWIN(strcpy((char *)calloc(strlen(S)+1,SzC),S)); S=s;
 s=Pole(S); Jxoyo =StrDOStoWIN(strcpy((char *)calloc(strlen(S)+1,SzC),S)); S=s;
 s=Pole(S); JYoOZo=StrDOStoWIN(strcpy((char *)calloc(strlen(S)+1,SzC),S)); S=s;
 s=Pole(S); JZoOXo=StrDOStoWIN(strcpy((char *)calloc(strlen(S)+1,SzC),S)); S=s;
 s=Pole(S); JXoOYo=StrDOStoWIN(strcpy((char *)calloc(strlen(S)+1,SzC),S)); S=s;
 s=Pole(S);   Prim=StrDOStoWIN(strcpy((char *)calloc(strlen(S)+1,SzC),S));
}
//---------------------------------------------------------------------------
InertElem::~InertElem(){
 if(Numb) free(Numb); if(V) free(V);
 if(xo) free(xo); if(yo) free(yo); if(zo) free(zo);
 if(JyoOzo) free(JyoOzo); if(JzoOxo) free(JzoOxo); if(Jxoyo)  free(Jxoyo);
 if(JYoOZo) free(JYoOZo); if(JZoOXo) free(JZoOXo); if(JXoOYo) free(JXoOYo);
 if(Prim) free(Prim);
}
//---------------------------------------------------------------------------
TInertBD::~TInertBD(){
 for(InertElem *r;r=R;){ R=r->Sled; delete r; }
 DelAllMagazine(&MC); DelAllMagazine(&MT);
}
//---------------------------------------------------------------------------
void TInertBD::Load(){
 FILE *F; char S[1024]; InertElem *r,**P=&R;
 sprintf(File,"%s\\InertBD.dat",WORKDIR);
 if(F=fopen(File,"r")){
  while(fgets(S,1023,F)){
   r=*P=new InertElem(S,&MC,&MT); P=(InertElem **)r;
  }
  fclose(F);
}}
//---------------------------------------------------------------------------
long InertElem::Save(FILE *F){
 char Buf[256]; long L;
 strcpy(Buf,Clas?Clas:"\0");     StrWINtoDOS(Buf); L =fprintf(F,"%s\xb3",Buf);
 strcpy(Buf,Typ?Typ:"\0");       StrWINtoDOS(Buf); L+=fprintf(F,"%s\xb3",Buf);
 strcpy(Buf,Numb?Numb:"\0");     StrWINtoDOS(Buf); L+=fprintf(F,"%s\xb3",Buf);
 strcpy(Buf,V?V:"\0");           StrWINtoDOS(Buf); L+=fprintf(F,"%s\xb3",Buf);
 strcpy(Buf,xo?xo:"\0");         StrWINtoDOS(Buf); L+=fprintf(F,"%s\xb3",Buf);
 strcpy(Buf,yo?yo:"\0");         StrWINtoDOS(Buf); L+=fprintf(F,"%s\xb3",Buf);
 strcpy(Buf,zo?zo:"\0");         StrWINtoDOS(Buf); L+=fprintf(F,"%s\xb3",Buf);
 strcpy(Buf,JyoOzo?JyoOzo:"\0"); StrWINtoDOS(Buf); L+=fprintf(F,"%s\xb3",Buf);
 strcpy(Buf,JzoOxo?JzoOxo:"\0"); StrWINtoDOS(Buf); L+=fprintf(F,"%s\xb3",Buf);
 strcpy(Buf,Jxoyo?Jxoyo:"\0");   StrWINtoDOS(Buf); L+=fprintf(F,"%s\xb3",Buf);
 strcpy(Buf,JYoOZo?JYoOZo:"\0"); StrWINtoDOS(Buf); L+=fprintf(F,"%s\xb3",Buf);
 strcpy(Buf,JZoOXo?JZoOXo:"\0"); StrWINtoDOS(Buf); L+=fprintf(F,"%s\xb3",Buf);
 strcpy(Buf,JXoOYo?JXoOYo:"\0"); StrWINtoDOS(Buf); L+=fprintf(F,"%s\xb3",Buf);
 strcpy(Buf,Prim?Prim:"\0");     StrWINtoDOS(Buf); L+=fprintf(F,"%s\n",Buf)+1;
 return L;
}
//---------------------------------------------------------------------------
bool TestIBD(TStringGrid *SG,int Row,int NF,char *Old){
 List *Lo; char *s,*inf,Buf[256];
 if(NF<15) PInertBD->CHANGEBD=true; else return false;
 switch(NF){
  case -1: break;
  case  0: break;
  case  1: break;
  case  2:
   strcpy(Inf,SG->Cells[NF-1][Row].c_str());
   if(strcmp(Inf,"I")&&strcmp(Inf,"II")&&strcmp(Inf,"III")){
    sprintf(Inf,"Тип элемента должен обозначаться римским числом:\n"
    "I, II или III, в соответствии со справочником Фаворина М.В.");
    Application->MessageBox(Inf,"Ошибка!",MB_OK); return true;
   }
   break;
  case  3:
   if(!isdigit(SG->Cells[NF-1][Row].c_str()[0])){
    sprintf(Inf,"Номер элемента представляет собой число, возможно,\n"
    "с буквой 'а', 'б' и т.д. в конце, в соответствии с\n"
    "обозначением в справочнике Фаворина М.В.");
    Application->MessageBox(Inf,"Ошибка!",MB_OK); return true;
   }
   break;
  case 4:case 5:case 6:case 7:case 8:case 9:case 10:case 11:case 12:case 13:
   StrToForm(SG->Cells[NF-1][Row].c_str());
   break;
  case 14:
   strcpy(Inf,SG->Cells[NF-1][Row].c_str());
   for(inf=Inf;s=strchr(inf,';');){
    *s++='\0'; CleanStr(inf,Buf); StrToForm(Buf); inf=s;
   }
   if(*inf){ CleanStr(inf,Buf); StrToForm(Buf); }
   break;
 }
 return false;
}
//---------------------------------------------------------------------------
void SaveIBD(void){ PInertBD->Save(); }
//---------------------------------------------------------------------------
void TInertBD::Edit(){
//char *Clas,*Typ,*Numb,*V,*xo,*yo,*zo,*JyoOzo,*JzoOxo,*JxoOyo,
// *JYoOZo,*JZoOXo,*JXoOYo,*Prim;
 char *FileSh="shapkabd",**Head=(char **)calloc(1,SzV),*t,
  *Format="%-s%-s%-s%-s%-s%-s%-s%-s%-s%-s%-s%-s%-s%-s",
  *Fprn="database.prn",*Help=(char *)calloc(256,SzV);
 int i,*MNN=(int *)calloc(15,SzI); Magazine ***M=(Magazine ***)calloc(14,SzV);
 bool *AddM=(bool *)calloc(14,sizeof(bool)); Head[0]=(char *)calloc(256,SzC);
 M[0]=&MC; M[1]=&MT; AddM[0]=AddM[1]=true;
 for(i=0;i<14;i++) MNN[i]=i+1;
 strcpy(Head[0],"Элементы Фаворина М.В.");
 strcpy(Help,WORKDIR); t=strrchr(Help,'\\'); strcpy(t,"\\Help\\InertBD.htm");
 InitCA();
 EditList=new TEditList((void **)&R,MNN,false,NULL,false,M,Format,FileSh,Head,
  Fprn,SaveIBD,TestIBD,NULL,0,-1,-1,-1,-1,NULL,NULL,1,Help,AddM,NULL);
 if(EditList->Diagnos()){
  Application->CreateForm(__classid(TFormEditList),&FormEditList);
  FormEditList->ShowModal();
 }
 free(L); LBeg=NULL;
 if(MNN) free(MNN); if(Help) free(Help); if(M) free(M); if(AddM) free(AddM);
 if(Head){ if(Head[0]) free(Head[0]); free(Head); }
 delete EditList;
}
//---------------------------------------------------------------------------
void TInertBD::Save(){
 FILE *F; char FileNew[256],FileBak[256],*t; long L=0l,Lf=0l; struct stat St;
 if(!CHANGEBD) return;
 strcpy(FileNew,File); strcpy(FileBak,File);
 t=strchr(FileNew,'.'); strcpy(t,".new");
 t=strchr(FileBak,'.'); strcpy(t,".bak");
 if(F=fopen(FileNew,"w")){
  for(InertElem *r=R;r;r=r->Sled) L+=r->Save(F);
  fclose(F);
 }
 if(!access(FileNew,0)){ stat(FileNew,&St); Lf=St.st_size; }
 if(L!=Lf){
  sprintf(Inf,"Не удалось обновить файл базы данных (%s)",File);
  Application->MessageBox(Inf,"Системная ошибка!",MB_OK|MB_ICONERROR);
  return;
 }
 if(!access(FileBak,0)) unlink(FileBak); rename(File,FileBak);
 rename(FileNew,File);
 strcpy(FileNew,"D:\\E\\AJU\\CiDyMNew\\Kinematika\\BC5\SAV\InertB.dat");
 if(!access(FileNew,0))
  CopyFiles(File,FileNew);
}
//---------------------------------------------------------------------------
InertElem *TInertBD::Find(char *N){
 InertElem *r; for(r=R;r;r=r->Sled) if(!strcmp(r->Numb,N)) break;
 return r;
}
//---------------------------------------------------------------------------
void TTetraedr::Fill(){
 Form FJxoxo,FJyoyo,Falfa,Fa,Fb,Fd; Vary *V;
 V=TakeVary("Jxoxo"); V->Znach=FJzoOxo+FJXoOYo; FJxoxo.V=V;
 V=TakeVary("Jyoyo"); V->Znach=FJXoOYo+FJyoOzo; FJyoyo.V=V;
 V=TakeVary("_d");    V->Znach=sqrt(FJXoOYo/Fm/3);  Fd.V=V;
 if((FJxoyo.V->Znach.C->Atr!=CNST)||(FJxoyo.V->Znach.C->Val!=0.0)){
  V=TakeVary("_alfa"); Falfa.V=V; V->Znach=0.5*arctg(2*FJxoyo/(FJyoyo-FJxoxo));
  V=TakeVary("_a"); Fa.V=V;
  V->Znach=sqrt(2/Fm*((FJyoOzo*((sin(Falfa))^2))+(FJzoOxo*((cos(Falfa))^2))-
   (FJXoOYo*sin(2*Falfa))));
  V=TakeVary("_b"); Fb.V=V;
  V->Znach=sqrt(2/3/Fm*((FJyoOzo*((cos(Falfa))^2))+(FJzoOxo*((sin(Falfa))^2))+
   (FJXoOYo*sin(2*Falfa))));
  P[0].Fr[0]=Fb*cos(Falfa)+Fa*sin(Falfa);
  P[1].Fr[0]=Fb*cos(Falfa)-Fa*sin(Falfa);
  P[2].Fr[0]=-2*Fb*cos(Falfa);
  P[3].Fr[0].C=Nul;
  P[0].Fr[1]=Fb*sin(Falfa)-Fa*cos(Falfa);
  P[1].Fr[1]=Fb*sin(Falfa)+Fa*cos(Falfa);
  P[2].Fr[1]=-2*Fb*sin(Falfa);
  P[3].Fr[1].C=Nul;
  P[0].Fr[2]=-Fd; P[1].Fr[2]=-Fd; P[2].Fr[2]=-Fd; P[3].Fr[2]=3*Fd;
 }
 else{
  V=TakeVary("_alfa"); Falfa.V=V; V->Znach.C=Nul;
  V=TakeVary("_a"); V->Znach=sqrt(2*FJZoOXo/Fm); Fa.V=V;
  V=TakeVary("_b"); V->Znach=sqrt(2*FJYoOZo/3/Fm); Fb.V=V;
  P[0].Fr[0]=Fb;  P[1].Fr[0]=Fb;  P[2].Fr[0]=-2*Fb; P[3].Fr[0].C=Nul;
  P[0].Fr[1]=-Fa; P[1].Fr[1]=Fa;  P[2].Fr[1].C=Nul; P[3].Fr[1].C=Nul;
  P[0].Fr[2]=-Fd; P[1].Fr[2]=-Fd; P[2].Fr[2]=-Fd;   P[3].Fr[2]=3*Fd;
 }
 FM=Fm;
if(OTLADKA){
 FILE *F=fopen("Otladka","a"); double D;
 if(Calcul(Fro,D)) fprintf(F,"\nro=%16g\n",D);
 else PrintForm(true,F,"\nFro",Fro.V->Znach);
 if(Calcul(FV,D)) fprintf(F,"V=%16g\n",D);
 else PrintForm(true,F,"\nFV",FV.V->Znach);
 if(Calcul(Fxo,D)) fprintf(F,"xo=%16g\n",D);
 else PrintForm(true,F,"\nFxo",Fxo.V->Znach);
 if(Calcul(Fyo,D)) fprintf(F,"yo=%16g\n",D);
 else PrintForm(true,F,"\nFyo",Fyo.V->Znach);
 if(Calcul(Fzo,D)) fprintf(F,"zo=%16g\n",D);
 else PrintForm(true,F,"\nFzo",Fzo.V->Znach);
 if(Calcul(FJyoOzo,D)) fprintf(F,"JyoOzo=%16g\n",D);
 else PrintForm(true,F,"\nFJyoOzo",FJyoOzo.V->Znach);
 if(Calcul(FJzoOxo,D)) fprintf(F,"JzoOxo=%16g\n",D);
 else PrintForm(true,F,"\nFJzoOxo",FJzoOxo.V->Znach);
 if(Calcul(FJxoyo,D)) fprintf(F,"Jxoyo=%16g\n",D);
 else PrintForm(true,F,"\nFJxoyo",FJxoyo.V->Znach);
 if(Calcul(FJYoOZo,D)) fprintf(F,"JYoOZo=%16g\n",D);
 else PrintForm(true,F,"\nFJYoOZo",FJYoOZo.V->Znach);
 if(Calcul(FJZoOXo,D)) fprintf(F,"JZoOXo=%16g\n",D);
 else PrintForm(true,F,"\nFJZoOXo",FJZoOXo.V->Znach);
 if(Calcul(FJXoOYo,D)) fprintf(F,"JXoOYo=%16g\n",D);
 else PrintForm(true,F,"\nFJXoOYo",FJXoOYo.V->Znach);
 if(Calcul(FJxoxo,D)) fprintf(F,"Jxoxo=%16g\n",D);
 else PrintForm(true,F,"\nFJxoxo",FJxoxo.V->Znach);
 if(Calcul(FJyoyo,D)) fprintf(F,"Jyoyo=%16g\n",D);
 else PrintForm(true,F,"\nFJyoyo",FJyoyo.V->Znach);
 if(Calcul(Falfa,D)) fprintf(F,"alfa=%16g\n",D);
 else PrintForm(true,F,"\nFalfa",Falfa.V->Znach);
 if(Calcul(Fa,D)) fprintf(F,"a=%16g\n",D);
 else PrintForm(true,F,"\nFa",Fa.V->Znach);
 if(Calcul(Fb,D)) fprintf(F,"b=%16g\n",D);
 else PrintForm(true,F,"\nFb",Fb.V->Znach);
 if(Calcul(Fd,D)) fprintf(F,"d=%16g\n",D);
 else PrintForm(true,F,"\nFd",Fd.V->Znach);
// PrintForm(true,F,"Fro",Fro.V->Znach);
// PrintForm(true,F,"Fm",Fm.V->Znach);
// PrintForm(true,F,"FV",FV.V->Znach);
 fclose(F);
}}
//---------------------------------------------------------------------------
void AddVect(Form A[3],Form B[3],Form C[3]){
 for(int i=0;i<3;i++) C[i]=A[i]+B[i];
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
void TTetraedr::TransCord(Parm *Par){
 Form S[3][3],dr[3],F,F1,F2; Parm *p; int i,j,Ind;
 for(p=Par;p;p=p->Sled)
  if((strlen(p->Nam)==2)&&
     ((p->Nam[0]=='R')||(p->Nam[0]=='S'))&&
     ((p->Nam[1]=='x')||(p->Nam[1]=='y')||(p->Nam[1]=='z'))) break;
 while(p){
//Построим добавочный вектор и матрицу поворота
  for(i=0;i<3;i++){//начальные значения
   S[i][i].C=Odin; dr[i].C=Nul; for(j=i+1;j<3;j++) S[i][j].C=S[j][i].C=Nul;
  }
  for(;p;p=p->Sled){
   if(!strcmp(p->Nam,"Rx")||!strcmp(p->Nam,"Ry")||!strcmp(p->Nam,"Rz")) break;
   if(!strcmp(p->Nam,"Sx")||!strcmp(p->Nam,"Sy")||!strcmp(p->Nam,"Sz")){
    if(p->R->Nam) F=StrToForm(p->R->Nam); else F.C=TakeCnst(p->R->Val);
    switch(p->Nam[1]){
     case 'x': Ind=0; break;
     case 'y': Ind=1; break;
     case 'z': Ind=2; break;
    }
    dr[Ind]=dr[Ind]+F;
  }}
  for(i=0;i<4;i++){
   AddVect(P[i].Fr,dr,P[i].Fr);
   sprintf(Inf,"P[%d].Fr",i);
   if(OTLADKA) PrintFormVect("Otladka","Смещение",Inf,P[i].Fr);
  }
  for(;p;p=p->Sled){
   if(!strcmp(p->Nam,"Sx")||!strcmp(p->Nam,"Sy")||!strcmp(p->Nam,"Sz")) break;
   if(!strcmp(p->Nam,"Rx")||!strcmp(p->Nam,"Ry")||!strcmp(p->Nam,"Rz")){
    if(p->R->Nam) F=StrToForm(p->R->Nam);
    else F.C=TakeCnst(p->R->Val*M_PI/180.0);
 //накопим матрицу поворота
    switch(p->Nam[1]){
     case 'x':
      for(i=0;i<3;i++){
       F1= S[i][1]*cos(F)+S[i][2]*sin(F);
       F2=-S[i][1]*sin(F)+S[i][2]*cos(F);
       S[i][1]=F1; S[i][2]=F2;
      }
      if(OTLADKA) PrintFormMatr("Otladka","Rx","S",S);
      break;
     case 'y':
      for(i=0;i<3;i++){
       F1=S[i][0]*cos(F)-S[i][2]*sin(F);
       F2=S[i][0]*sin(F)+S[i][2]*cos(F);
       S[i][0]=F1; S[i][2]=F2;
      }
      if(OTLADKA) PrintFormMatr("Otladka","Ry","S",S);
      break;
     case 'z':
      for(i=0;i<3;i++){
       F1=-S[i][0]*cos(F)+S[i][1]*sin(F);
       F2=-S[i][0]*sin(F)+S[i][1]*cos(F);
       S[i][0]=F1; S[i][1]=F2;
      }
      if(OTLADKA) PrintFormMatr("Otladka","Rz","S",S);
      break;
  }}}
  for(i=0;i<4;i++){
   MultMatrVect(S,P[i].Fr);
   sprintf(Inf,"P[%d].Fr",i);
   if(OTLADKA) PrintFormVect("Otladka","Поворот",Inf,(Form *)(P[i].Fr));
}}}
//---------------------------------------------------------------------------
bool Inertion(Parm *Par,TRichEdit *R){
 Parm *P,*NamePar=NULL,*p; TInertBD InertBD; int K,i,j; Form F; double D;
 InertElem *r; List *Lo=L; bool NoRo; Vary *V; Root *I;
 char *s,*n,Name[256],TmpFile[]="tmp.tmp";
 FILE *TMPFILE;
 OTLADKA=FindInst("ОТЛАДКА",&I); RichEdit=R;
 if(OTLADKA) unlink("Otladka");
 InertBD.Load();
 for(P=Par;P;P=P->Sled){//проверка имен членов списка оператора
  if(!P->Nam||!P->Nam[0]){
   sprintf(Inf,"В операторе %s строки %d %d-й параметр не имеет имени",
    "ТЕНЗОР ИНЕРЦИИ",Par->Line,i);
   Application->MessageBox(Inf,"Ошибка!",MB_OK|MB_ICONERROR);
   return false;
  }
  if((P->Nam[0]!='э')&&(P->Nam[0]!='Э')){
   if(NamePar){
    sprintf(Inf,
     "В операторе %s строки %d только один член списка должен обозначать\n"
     "наименование тела, остальные - №№ элементов из Справочника, которые\n"
     "должны начинаться с буквы 'э' или 'Э' и затем, собственно, № элемента",
     "ТЕНЗОР ИНЕРЦИИ",Par->Line);
    Application->MessageBox(Inf,"Ошибка!",MB_OK|MB_ICONERROR);
    return false;
   }
   else{
    NamePar=P;
    if(FindParm(P->R,"ro",&p)){
     if(!p->R){
      sprintf(Inf,"В операторе %s строки %d не задано значение\n"
      "для параметра плотности - \"ro\"","ТЕНЗОР ИНЕРЦИИ",Par->Line);
      Application->MessageBox(Inf,"Ошибка!",MB_OK|MB_ICONERROR);
      return false;
     }
     else NoRo=false;
    }
    else NoRo=true;
 }}}
 if(NoRo){//если нет плотности в наименовании тела, она должна быть в каждом элементе
  for(P=Par;P;P=P->Sled){
   if(P!=NamePar){
    if(FindParm(P->R,"ro",&p)){
     if(!p->R){
      sprintf(Inf,"В операторе %s строки %d не задано значение\n"
      "для параметра плотности - \"ro\"","ТЕНЗОР ИНЕРЦИИ",Par->Line);
      Application->MessageBox(Inf,"Ошибка!",MB_OK|MB_ICONERROR);
      return false;
    }}
    else{
     sprintf(Inf,"В операторе %s строки %d для элемента %s\n"
      "не задана плотность","ТЕНЗОР ИНЕРЦИИ",Par->Line,P->Nam);
     Application->MessageBox(Inf,"Ошибка!",MB_OK|MB_ICONERROR);
     return false;
 }}}}
 K=KolElem(Par)-1;
 TTetraedr *T=(TTetraedr *)calloc(K,sizeof(TTetraedr));
 List **Li=(List **)calloc(K,sizeof(List *));
 for(P=Par,i=0;P;P=P->Sled){
  if((P->Nam[0]=='э')||(P->Nam[0]=='Э')){
   r=InertBD.Find(P->Nam+1); //найдем в базе данных такой элемент
   if(!r){
    sprintf(Inf,"В операторе %s строки %d элемент %s не найден в базе данных",
     "ТЕНЗОР ИНЕРЦИИ",Par->Line,P->Nam);
    Application->MessageBox(Inf,"Ошибка!",MB_OK|MB_ICONERROR);
    free(T); return false;
   }
   L=Li[i]=(List *)calloc(1,sizeof(List)); //сделаем List для элемента
   Li[i]->Time=TakeVary("t"); Li[i]->Freq=TakeVary("w");
   Li[i]->V=TakeVary("g"); Li[i]->V->P=0; Li[i]->V->Val=9.8062;
   Li[i]->V->Znach.C=TakeCnst(9.8062);
//получим формулы для объема, плотности, массы
   FV.V=TakeVary("V");
   if(r->V) FV.V->Znach=StrToForm(r->V);
   else     FV.V->Znach.C=Nul;
if(OTLADKA){
 FILE *F=fopen("Otladka","a"); PrintForm(true,F,"V",FV.V->Znach); fclose(F);
}
   Fm.V=TakeVary("m"); Fro.V=TakeVary("ro");
   L=Lo;
   if(FindParm(P->R,"ro",&p)){
    if(p->R){
     if(p->R->Nam) Fro.V->Znach=StrToForm(p->R->Nam);
     else Fro.V->Znach.C=TakeCnst(p->R->Val);
    }
    else{
     sprintf(Inf,"В операторе %s строки %d не задано значение\n"
     "для параметра плотности - \"ro\"","ТЕНЗОР ИНЕРЦИИ",Par->Line);
     Application->MessageBox(Inf,"Внимание!",MB_OK|MB_ICONERROR);
   }}
   if(Fro.V->Znach.C==Pust){
    if(FindParm(NamePar->R,"ro",&p)){
     if(p->R){
      if(p->R->Nam) Fro.V->Znach=StrToForm(p->R->Nam);
      else Fro.V->Znach.C=TakeCnst(p->R->Val);
   }}}
   L=Li[i];
//получим формулы для координат центра масс, моментов инерции
   Fm.V->Znach=Fro*FV;
if(OTLADKA){
 FILE *F=fopen("Otladka","a");
 PrintForm(true,F,"ro",Fro.V->Znach); PrintForm(true,F,"m",Fm.V->Znach);
 fclose(F);
}
   Fxo.V=TakeVary("xo");
   if(r->xo&&*r->xo) Fxo.V->Znach=StrToForm(r->xo); else Fxo.V->Znach.C=Nul;
   Fyo.V=TakeVary("yo");
   if(r->yo&&*r->yo) Fyo.V->Znach=StrToForm(r->yo); else Fyo.V->Znach.C=Nul;
   Fzo.V=TakeVary("zo");
   if(r->zo&&*r->zo) Fzo.V->Znach=StrToForm(r->zo); else Fzo.V->Znach.C=Nul;
   FJyoOzo.V=TakeVary("JyoOzo");
   if(r->JyoOzo&&*r->JyoOzo) FJyoOzo.V->Znach=StrToForm(r->JyoOzo);
   else FJyoOzo.V->Znach.C=Nul;
   FJzoOxo.V=TakeVary("JzoOxo");
   if(r->JzoOxo&&*r->JzoOxo) FJzoOxo.V->Znach=StrToForm(r->JzoOxo);
   else FJzoOxo.V->Znach.C=Nul;
   FJxoyo.V=TakeVary("Jxoyo");
   if(r->Jxoyo&&*r->Jxoyo) FJxoyo.V->Znach=StrToForm(r->Jxoyo);
   else FJxoyo.V->Znach.C=Nul;
   FJYoOZo.V=TakeVary("JYoOZo");
   if(r->JYoOZo&&*r->JYoOZo) FJYoOZo.V->Znach=StrToForm(r->JYoOZo);
   else FJYoOZo.V->Znach.C=Nul;
   FJZoOXo.V=TakeVary("JZoOXo");
   if(r->JZoOXo&&*r->JZoOXo) FJZoOXo.V->Znach=StrToForm(r->JZoOXo);
   else FJZoOXo.V->Znach.C=Nul;
   FJXoOYo.V=TakeVary("JXoOYo");
   if(r->JXoOYo&&*r->JXoOYo) FJXoOYo.V->Znach=StrToForm(r->JXoOYo);
   else FJXoOYo.V->Znach.C=Nul;
//получим формулы из примечания
   if(r->Prim&&*r->Prim){
    char *s=r->Prim,*s0,*n,Name[256];
    while(*s){
     for(;*s==' ';s++);
     for(n=Name;*s&&(*s!=' ')&&(*s!='=');*n++=*s++); *n='\0';
     while(*s&&(*s!='=')) s++; s++;
     for(s0=s;*s0&&(*s0!=';');s0++); *s0='\0';
     V=TakeVary(Name); V->Znach=StrToForm(s); *s0=';';
     s=s0+1;
   }}
//проверим, чтобы обобщенные координаты листа (формул из Справочника) были
//определены в списке текущего элемента оператора
   Coord(Li[i]);
   L=Lo;
   for(Cord *K=Li[i]->K;K;K=K->Sled){
    if(!FindParm(P->R,K->V->Name,&p)){ Vary *V;
     sprintf(Inf,"В операторе %s строки %d элемент %s\n"
     "не содержит значения необходимого параметра %s",
     "ТЕНЗОР ИНЕРЦИИ",Par->Line,P->Nam,K->V->Name);
     Application->MessageBox(Inf,"Ошибка!",MB_OK|MB_ICONERROR);
     while(i--){
      while(V=Li[i]->V){ Li[i]->V=V->Sled; free(V->Name); free(V); }
      while(K=Li[i]->K){ Li[i]->K=K->Sled; free(K); }
      free(Li[i]);
     }
     free(Li); free(T); return false;
    }
    else{
     if(p->R->Nam) K->V->Znach=StrToForm(p->R->Nam);
     else K->V->Znach.C=TakeCnst(p->R->Val);
   }}
   L=Li[i];
   T[i].Fill();
   L=Lo;
   T[i].TransCord(P->R);
   i++;
 }}
 for(s=NamePar->Nam,n=Name;*s;s++) *n++=(*s==' ')?'_':*s; *n='\0';
 sprintf(Inf,"m@%s",Name); V=TakeVary(Inf);
 for(i=0,F.C=Nul;i<K;i++) F=F+T[i].FM;
 if(Calcul(F,D)) V->Znach.C=TakeCnst(D); else V->Znach=F;
 sprintf(Inf,"Jxx@%s",Name); V=TakeVary(Inf);
 for(i=0,F.C=Nul;i<K;i++){ Form f;
  for(j=0,f.C=Nul;j<4;j++){
   f=f+((T[i].P[j].Fr[1])^2)+((T[i].P[j].Fr[2])^2);
  }
  F=F+T[i].FM*f;
 }
 F=0.25*F;
 if(Calcul(F,D)) V->Znach.C=TakeCnst(D); else V->Znach=F;
 sprintf(Inf,"Jyy@%s",Name); V=TakeVary(Inf);
 for(i=0,F.C=Nul;i<K;i++){ Form f;
  for(j=0,f.C=Nul;j<4;j++){
   f=f+((T[i].P[j].Fr[0])^2)+((T[i].P[j].Fr[2])^2);
if(OTLADKA){
 FILE *F=fopen("Otladka","a");
 sprintf(Inf,"\nT[%d].P[%d].Fr[0]",i,j); PrintForm(true,F,Inf,T[i].P[j].Fr[0]);
 sprintf(Inf,"\nT[%d].P[%d].Fr[2]",i,j); PrintForm(true,F,Inf,T[i].P[j].Fr[2]);
 PrintForm(true,F,"\nf",f);
 fclose(F);
}
  }
  F=F+T[i].FM*f;
 }
 F=0.25*F;
if(OTLADKA){
 FILE *f=fopen("Otladka","a"); PrintForm(true,f,"F",F); fclose(f);
}
 if(Calcul(F,D)) V->Znach.C=TakeCnst(D); else V->Znach=F;
 sprintf(Inf,"Jzz@%s",Name); V=TakeVary(Inf);
 for(i=0,F.C=Nul;i<K;i++){ Form f;
  for(j=0,f.C=Nul;j<4;j++) f=f+((T[i].P[j].Fr[0])^2)+((T[i].P[j].Fr[1])^2);
  F=F+T[i].FM*f;
 }
 F=0.25*F;
 if(Calcul(F,D)) V->Znach.C=TakeCnst(D); else V->Znach=F;
 sprintf(Inf,"Jxy@%s",Name); V=TakeVary(Inf);
 for(i=0,F.C=Nul;i<K;i++){ Form f;
  for(j=0,f.C=Nul;j<4;j++) f=f+T[i].P[j].Fr[0]*T[i].P[j].Fr[1];
  F=F+T[i].FM*f;
 }
 F=0.25*F;
 if(Calcul(F,D)) V->Znach.C=TakeCnst(D); else V->Znach=F;
 sprintf(Inf,"Jyz@%s",Name); V=TakeVary(Inf);
 for(i=0,F.C=Nul;i<K;i++){ Form f;
  for(j=0,f.C=Nul;j<4;j++) f=f+T[i].P[j].Fr[1]*T[i].P[j].Fr[2];
  F=F+T[i].FM*f;
 }
 F=0.25*F;
 if(Calcul(F,D)) V->Znach.C=TakeCnst(D); else V->Znach=F;
 sprintf(Inf,"Jxz@%s",Name); V=TakeVary(Inf);
 for(i=0,F.C=Nul;i<K;i++){ Form f;
  for(j=0,f.C=Nul;j<4;j++) f=f+T[i].P[j].Fr[0]*T[i].P[j].Fr[2];
  F=F+T[i].FM*f;
 }
 F=0.25*F;
 if(Calcul(F,D)) V->Znach.C=TakeCnst(D); else V->Znach=F;
 TMPFILE=fopen(TmpFile,"w");
 if(OPRFILE=fopen(OprFile,access(OprFile,0)?"w":"a")){
  fprintf(OPRFILE,
   "<html>\n<head>\n<title>ПРОТОКОЛ ОПЕРАТОРА \"ТЕНЗОР ИНЕРЦИИ\" строки %d</title>\n"
   "<meta http-equiv=\"Content-Type\" content=\"text/html;"
   "charset=windows-1251\">\n</head>\n"
   "<body bgcolor=\"#FFFFFF\" text=\"#800000\""
   " background=\"%s/fon2.jpg\""
   " font face=\"Times New Roman, Times, serif\">\n"
   "ПРОТОКОЛ ОПЕРАТОРА \"ТЕНЗОР ИНЕРЦИИ\"<br><br>",
   L->Oper->Line,WORKDIR);
  fprintf(OPRFILE,"Сгенерированы следующие инерционные параметры:<hr>\n");
  if(RichEdit){
   RichEdit->Lines->Add("ПРОТОКОЛ ОПЕРАТОРА \"ТЕНЗОР ИНЕРЦИИ\"\n\n");
   RichEdit->Lines->Add("Сгенерированы следующие инерционные параметры:\n");
  }
  sprintf(Inf,"m@%s",Name);
  if(FindVary(Inf,&V)){
   PrintForm(false,OPRFILE,Inf,V->Znach); fputs("<br>\n",OPRFILE);
   if(TMPFILE){ PrintForm(true,TMPFILE,Inf,V->Znach); fputc('\n',TMPFILE); }
  }
  sprintf(Inf,"Jxx@%s",Name);
  if(FindVary(Inf,&V)){
   PrintForm(false,OPRFILE,Inf,V->Znach); fputs("<br>\n",OPRFILE);
   if(TMPFILE){ PrintForm(true,TMPFILE,Inf,V->Znach); fputc('\n',TMPFILE); }
  }
  sprintf(Inf,"Jyy@%s",Name);
  if(FindVary(Inf,&V)){
   PrintForm(false,OPRFILE,Inf,V->Znach); fputs("<br>\n",OPRFILE);
   if(TMPFILE){ PrintForm(true,TMPFILE,Inf,V->Znach); fputc('\n',TMPFILE); }
  }
  sprintf(Inf,"Jzz@%s",Name);
  if(FindVary(Inf,&V)){
   PrintForm(false,OPRFILE,Inf,V->Znach); fputs("<br>\n",OPRFILE);
   if(TMPFILE){ PrintForm(true,TMPFILE,Inf,V->Znach); fputc('\n',TMPFILE); }
  }
  sprintf(Inf,"Jxy@%s",Name);
  if(FindVary(Inf,&V)){
   PrintForm(false,OPRFILE,Inf,V->Znach); fputs("<br>\n",OPRFILE);
   if(TMPFILE){ PrintForm(true,TMPFILE,Inf,V->Znach); fputc('\n',TMPFILE); }
  }
  sprintf(Inf,"Jyz@%s",Name);
  if(FindVary(Inf,&V)){
   PrintForm(false,OPRFILE,Inf,V->Znach); fputs("<br>\n",OPRFILE);
   if(TMPFILE){ PrintForm(true,TMPFILE,Inf,V->Znach); fputc('\n',TMPFILE); }
  }
  sprintf(Inf,"Jxz@%s",Name);
  if(FindVary(Inf,&V)){
   PrintForm(false,OPRFILE,Inf,V->Znach); fputs("<br>\n",OPRFILE);
   if(TMPFILE){ PrintForm(true,TMPFILE,Inf,V->Znach); fputc('\n',TMPFILE); }
  }
  fputs("</font></body>\n</html>\n",OPRFILE);
  fclose(OPRFILE);
 }
 if(TMPFILE) fclose(TMPFILE);
 return true;
}
//---------------------------------------------------------------------------


