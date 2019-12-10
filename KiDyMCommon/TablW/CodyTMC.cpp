//---------------------------------------------------------------------------

#pragma hdrstop

#include "CodyTMC.h"
#include <stdio.h>
#include <sys\stat.h>
#include <process.h>
//#include "SnabKern.h"
#include "EditList.h"
#include "ConvDW.h"
//#include "EditCodif.h"
//#include "EditSnab.h"
#include <dir.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)

extern int NM,NY,KolMont,NKeeps;
extern char DISK[],WORKDIR[],*NameMont[],*NameMonta[],Inf[];
/*, Ac40[],Ac05[],*NameBN,LHA[]*/;

//extern Product *CurrKeep,*MontKeep-временно/,*Curr201,*Mont201*/;
extern Magazine *MIdKod,*MName;
extern TEditList *EditList;

//extern bool CHANGEGRPTMC,CHANGEDATA;

//extern TListIzdelie *ListIzdelie; //где должно быть объявлено???
//extern TIdentCod *IdentCod; -временно

//---------------------------------------------------------------------------
TCodTMC::TCodTMC(char *N,char *NN){
 int l=strlen(NN); Sled=NULL; CodOld=CodNew=NULL;
 Name=(strcpy((char *)malloc(strlen(N)+1),N));
 (l>4?CodNew:CodOld)=(strcpy((char *)malloc(strlen(NN)+1),NN));
};
//---------------------------------------------------------------------------
TCodTMC::~TCodTMC(){
 if(Name) free(Name); if(CodOld) free(CodOld); if(CodNew) free(CodNew);
}
//---------------------------------------------------------------------------
TListCodyTMC::TListCodyTMC(){ R=NULL; };
//---------------------------------------------------------------------------
TListCodyTMC::~TListCodyTMC(){
 TCodTMC *r; while(r=R){ R=r->Sled; delete(r); }
}
//---------------------------------------------------------------------------
int TListCodyTMC::FindTMC(char *Name,TCodTMC **t){
 if(!(*t=R)) return 0;
 if(strcmp(Name,R->GetName())<0) { *t=NULL; return 0; }
 for(;;*t=(*t)->Sled){
  if(!strcmp((*t)->GetName(),Name)) return 1;
  if(!((*t)->Sled)) return 0;
  if(strcmp((*t)->Sled->GetName(),Name)>0) return 0;
}}
//---------------------------------------------------------------------------
TCodTMC *TListCodyTMC::TakeTMC(char *Name,char *NN){
 TCodTMC *T,*t;
 if(FindTMC(Name,&T)){
  T->Fill(NN); return T;
 }
 t=new TCodTMC(Name,NN);
 if(!T) { t->Sled=R;       return       R=t; }
 else   { t->Sled=T->Sled; return T->Sled=t; }
}
//---------------------------------------------------------------------------
void TCodTMC::Fill(char *NN){
 int l=strlen(NN);
 if(l>4){
  if(CodNew) CodNew=(char *)realloc(CodNew,(l+1)*sizeof(char));
  else CodNew=(char *)calloc(l+1,sizeof(char));
  strcpy(CodNew,NN);
 }
 else{
  if(CodOld) CodOld=(char *)realloc(CodOld,(l+1)*sizeof(char));
  else CodOld=(char *)calloc(l+1,sizeof(char));
  strcpy(CodOld,NN);
}}
//---------------------------------------------------------------------------
//int WorkAc,WorkNk;
//---------------------------------------------------------------------------
//bool CHANGECODS;
//---------------------------------------------------------------------------
void TCodTMC::SetCodNew(char *N){
 if(CodNew) free(CodNew); CodNew=strcpy((char *)calloc(strlen(N)+1,SzC),N);
};
//---------------------------------------------------------------------------
void TListCodyTMC::LoadSklad(int Ac,int Nk){
/* char File[256],Arhiv[256],S[256],*s,*s0,*Name,Kod[16],*k; FILE *F;
 int K,j,l;
 WorkAc=Ac; WorkNk=Nk;
 FormSnab->StatusBar->SimplePanel=true;
 FormSnab->CGauge->MaxValue=2*KolMont; FormSnab->CGauge->ForeColor=clGreen;
 FormSnab->CGauge->ShowText=true; FormSnab->CGauge->Progress=0;
 for(int i=0;i<KolMont;i++){//загрузим из данных за полгода
  int nm=NM-i>0?NM-i:NM-i+12,ny=NM-i>0?NY:NY-1;
  sprintf(Arhiv,"%s\\%d%x\\%d%x%3.3d%c.lzh",DISK,ny,nm,ny,nm,Ac,IntToChar(Nk));
  sprintf(File,"%d%x%3.3d%d.mon",ny,nm,Ac,Nk);
//  spawnl(P_WAIT,LHA,LHA,"x",Arhiv,File,NULL);
  UnPack(LHA,Arhiv,File);
  sprintf(File,"%d%x%3.3d%d.mon",ny,nm,Ac,Nk);
  if(!access(File,0)&&(F=fopen(File,"r"))){
   sprintf(Inf,"Загрузка данных (наименование ТМЦ, код) из файла %s",File);
   FormSnab->StatusBar->SimpleText=Inf; FormSnab->CGauge->Progress++;
   fgets(s=S,255,F);
   while(fgets(s=S,255,F)){
    s0=Pole(s); Name=s; s=s0;
    for(j=0;j<2;j++){ s0=Pole(s); s=s0; }
    s0=Pole(s); StrDOStoWIN(s);
    if(strchr(s,'.')){
     for(k=Kod;*s;s++) if(*s=='.') continue; *k++=*s; *k='\0';
    }
    else strcpy(Kod,s);
    StrDOStoWIN(Name);
    TakeTMC(Name,Kod);
   }
   fclose(F);
  }
  sprintf(File,"%d%x%3.3d%d.mov",ny,nm,Ac,Nk);
  UnPack(LHA,Arhiv,File);
  if(!access(File,0)&&(F=fopen(File,"r"))){
   sprintf(Inf,"Загрузка данных (наименование ТМЦ, код) из файла %s",File);
   FormSnab->StatusBar->SimpleText=Inf; FormSnab->CGauge->Progress++;
   fgets(s=S,255,F);
   while(fgets(s=S,255,F)){
    s0=Pole(s); Name=s; s=s0;
    for(j=0;j<2;j++){ s0=Pole(s); s=s0; }
    s0=Pole(s); StrDOStoWIN(s);
    if(strchr(s,'.')){
     for(k=Kod;*s;s++) if(*s=='.') continue; *k++=*s; *k='\0';
    }
    else strcpy(Kod,s); s=s0;
    StrDOStoWIN(Name); TakeTMC(Name,Kod);
    for(j=0;j<7;j++){ s0=Pole(s); s=s0; }
    switch(K=atoi(s)){
     case 0: break;
     case 1: fgets(S,255,F); break;
     default: for(j=1;j<K;j++) fgets(S,255,F);
   }}
   fclose(F);
  }
  if(!strcmp(NameBN,Ac40)&&(WorkAc==atoi(Ac05))){
   sprintf(Arhiv,"%s\\%d%x\\%d%x%s0.lzh",DISK,ny,nm,ny,nm,Ac40);
   sprintf(File,"%d%x%s0.mov",ny,nm,Ac40);
//   spawnl(P_WAIT,LHA,LHA,"x",Arhiv,File,NULL);
   UnPack(LHA,Arhiv,File);
   if(!access(File,0)&&(F=fopen(File,"r"))){
    sprintf(Inf,"Загрузка данных (наименование ТМЦ, код) из файла %s",File);
    FormSnab->StatusBar->SimplePanel=Inf;
    fgets(s=S,255,F);
    while(fgets(s=S,255,F)){
     for(j=0;j<11;j++){ s0=Pole(s); s=s0; }
     switch(K=atoi(s)){
      case 0: break;
      case 1: fgets(S,255,F); break;
      default:
       for(j=1;j<K;j++){
        fgets(s=S,255,F); s0=Pole(s); s=s0; s0=Pole(s);
        strcpy(Kod,s); s=s0; s0=Pole(s); Name=s; s=s0;
        StrDOStoWIN(Name);
        TakeTMC(Name,Kod);
        for(l=0;l<5;l++){ s0=Pole(s); s=s0; }
        s0=Pole(s); strcpy(Kod,s); s=s0; s0=Pole(s);
        Name=s; s=s0; StrDOStoWIN(Name);
        TakeTMC(Name,Kod);
    }} }
    fclose(F);
 }}}
 FormSnab->CGauge->Progress=0;  FormSnab->CGauge->ShowText=false;
 FormSnab->StatusBar->SimplePanel=false;
 strcpy(File,"IzdKomp.lst");
 if(ListIzdelie) delete ListIzdelie;
 ListIzdelie=new TListIzdelie(File);
 ListIzdelie->Load();
 for(TIzdelie *r=ListIzdelie->GetR();r;r=r->Sled)
  if(WorkAc==atoi(Ac05))
   for(TKomplekt *t=r->GetR();t;t=t->Sled)
    TakeTMC(t->GetName(),t->GetKod());
  else
   TakeTMC(r->GetName(),r->GetKod());
 strcpy(File,"Identif.kod");
 if(IdentCod) delete IdentCod;
 IdentCod=new TIdentCod(File);
 if(IdentCod->LoadIdentCod(FormSnab->CGauge,FormSnab->StatusBar)){
  for(TCodTMC *t=R;t;t=t->Sled){
   TGroupTMC *G; TPodGroupTMC *P;
   if(IdentCod->FindKod(t->GetName(),Kod,&G,&P)){
    if(!t->GetCodNew()||strcmp(t->GetCodNew(),Kod)){
     t->SetCodNew(Kod); CHANGECODS=true;
}}}}*/
}
//---------------------------------------------------------------------------
/*void TListCodyTMC::Load(int Ac,int Nk){
 char File[256],Arhiv[256],S[256],*s,*s0,*Name,Kod[16],*k; FILE *F;
 int K,j,l;
 WorkAc=Ac; WorkNk=Nk;
 FormSnab->StatusBar->SimplePanel=true;
 FormSnab->CGauge->MaxValue=2*KolMont; FormSnab->CGauge->ForeColor=clGreen;
 FormSnab->CGauge->ShowText=true; FormSnab->CGauge->Progress=0;
 for(int i=0;i<KolMont;i++){//загрузим из данных за полгода
  int nm=NM-i>0?NM-i:NM-i+12,ny=NM-i>0?NY:NY-1;
  sprintf(Arhiv,"%s\\%d%x\\%d%x%3.3d%c.lzh",DISK,ny,nm,ny,nm,Ac,IntToChar(Nk));
  sprintf(File,"%d%x%3.3d%d.mon",ny,nm,Ac,Nk);
//  spawnl(P_WAIT,LHA,LHA,"x",Arhiv,File,NULL);
  UnPack(LHA,Arhiv,File);
  sprintf(File,"%d%x%3.3d%d.mon",ny,nm,Ac,Nk);
  if(!access(File,0)&&(F=fopen(File,"r"))){
   sprintf(Inf,"Загрузка данных (наименование ТМЦ, код) из файла %s",File);
   FormSnab->StatusBar->SimpleText=Inf; FormSnab->CGauge->Progress++;
   fgets(s=S,255,F);
   while(fgets(s=S,255,F)){
    s0=Pole(s); Name=s; s=s0;
    for(j=0;j<2;j++){ s0=Pole(s); s=s0; }
    s0=Pole(s); StrDOStoWIN(s);
    if(strchr(s,'.')){
     for(k=Kod;*s;s++) if(*s=='.') continue; *k++=*s; *k='\0';
    }
    else strcpy(Kod,s);
    StrDOStoWIN(Name);
    TakeTMC(Name,Kod);
   }
   fclose(F);
  }
  sprintf(File,"%d%x%3.3d%d.mov",ny,nm,Ac,Nk);
  UnPack(LHA,Arhiv,File);
  if(!access(File,0)&&(F=fopen(File,"r"))){
   sprintf(Inf,"Загрузка данных (наименование ТМЦ, код) из файла %s",File);
   FormSnab->StatusBar->SimpleText=Inf; FormSnab->CGauge->Progress++;
   fgets(s=S,255,F);
   while(fgets(s=S,255,F)){
    s0=Pole(s); Name=s; s=s0;
    for(j=0;j<2;j++){ s0=Pole(s); s=s0; }
    s0=Pole(s); StrDOStoWIN(s);
    if(strchr(s,'.')){
     for(k=Kod;*s;s++) if(*s=='.') continue; *k++=*s; *k='\0';
    }
    else strcpy(Kod,s); s=s0;
    StrDOStoWIN(Name); TakeTMC(Name,Kod);
    for(j=0;j<7;j++){ s0=Pole(s); s=s0; }
    switch(K=atoi(s)){
     case 0: break;
     case 1: fgets(S,255,F); break;
     default: for(j=1;j<K;j++) fgets(S,255,F);
   }}
   fclose(F);
  }
  if(!strcmp(NameBN,Ac40)&&(WorkAc==atoi(Ac05))){
   sprintf(Arhiv,"%s\\%d%x\\%d%x%s0.lzh",DISK,ny,nm,ny,nm,Ac40);
   sprintf(File,"%d%x%s0.mov",ny,nm,Ac40);
//   spawnl(P_WAIT,LHA,LHA,"x",Arhiv,File,NULL);
   UnPack(LHA,Arhiv,File);
   if(!access(File,0)&&(F=fopen(File,"r"))){
    sprintf(Inf,"Загрузка данных (наименование ТМЦ, код) из файла %s",File);
    FormSnab->StatusBar->SimplePanel=Inf;
    fgets(s=S,255,F);
    while(fgets(s=S,255,F)){
     for(j=0;j<11;j++){ s0=Pole(s); s=s0; }
     switch(K=atoi(s)){
      case 0: break;
      case 1: fgets(S,255,F); break;
      default:
       for(j=1;j<K;j++){
        fgets(s=S,255,F); s0=Pole(s); s=s0; s0=Pole(s);
        strcpy(Kod,s); s=s0; s0=Pole(s); Name=s; s=s0;
        StrDOStoWIN(Name);
        TakeTMC(Name,Kod);
        for(l=0;l<5;l++){ s0=Pole(s); s=s0; }
        s0=Pole(s); strcpy(Kod,s); s=s0; s0=Pole(s);
        Name=s; s=s0; StrDOStoWIN(Name);
        TakeTMC(Name,Kod);
    }} }
    fclose(F);
 }}}
 FormSnab->CGauge->Progress=0;  FormSnab->CGauge->ShowText=false;
 FormSnab->StatusBar->SimplePanel=false;
 strcpy(File,"IzdKomp.lst");
 if(ListIzdelie) delete ListIzdelie;
 ListIzdelie=new TListIzdelie(File);
 ListIzdelie->Load();
 for(TIzdelie *r=ListIzdelie->GetR();r;r=r->Sled)
  if(WorkAc==atoi(Ac05))
   for(TKomplekt *t=r->GetR();t;t=t->Sled)
    TakeTMC(t->GetName(),t->GetKod());
  else
   TakeTMC(r->GetName(),r->GetKod());
 strcpy(File,"Identif.kod");
 if(IdentCod) delete IdentCod;
 IdentCod=new TIdentCod(File);
 if(IdentCod->LoadIdentCod(FormSnab->CGauge,FormSnab->StatusBar)){
  for(TCodTMC *t=R;t;t=t->Sled){
   TGroupTMC *G; TPodGroupTMC *P;
   if(IdentCod->FindKod(t->GetName(),Kod,&G,&P)){
    if(!t->GetCodNew()||strcmp(t->GetCodNew(),Kod)){
     t->SetCodNew(Kod); CHANGECODS=true;
}}}}}*/
//---------------------------------------------------------------------------
/*bool TestCodyTMC(TStringGrid *SG,int Row,int NF,char *Old){
 char Kod[16],*s; int l,L=WorkAc==atoi(Ac40)?10:8;
 switch(NF){
  case -1:case 0: return true;
  case 3:
   strcpy(Inf,SG->Cells[2][Row].c_str()); CleanStr(Inf,Kod);
   for(s=Kod;*s;s++)
    if(!isdigit(*s)){
     Application->MessageBox(
      "Идентификационный код ТМЦ должен состоять только из цифр!",
      "Ошибка!",MB_OK|MB_ICONERROR);
     return true;
    }
   if(((l=strlen(Kod))!=L)&&l){
    sprintf(Inf,"Идентификационный код должен быть %d-значным.",L);
    Application->MessageBox(Inf,"Ошибка!",MB_OK|MB_ICONERROR);
    return true;
   }
   break;
  case 5:
   for(int i=0;i<SG->RowCount;i++){
    strcpy(Inf,SG->Cells[2][i].c_str()); CleanStr(Inf,Kod);
    for(s=Kod;*s;s++){
     if(!isdigit(*s)){
      sprintf(Inf,
       "Идентификационный код для\n"
       "%s\nдолжен состоять только из цифр!\n"
       "Будете исправлять?",SG->Cells[0][i].c_str());
      if(Application->MessageBox(Inf,"Ошибка?",MB_YESNO|MB_ICONQUESTION)==ID_YES){
       SG->Row=i; return true;
    }}}
    if(((l=strlen(Kod))!=L)&&l){
     sprintf(Inf,"Идентификационный код для\n"
      "%s\nдолжен быть %d-значным.\n"
      "Будете исправлять?",SG->Cells[0][i].c_str(),L);
     if(Application->MessageBox(Inf,"Ошибка?",MB_YESNO|MB_ICONQUESTION)==ID_YES){
      SG->Row=i; return true;
 }}}}
 if(NF<4) CHANGECODS=true;
 return false;
} */
//---------------------------------------------------------------------------
/*void TListCodyTMC::Edit(){
//Редактирование кодов ТМЦ
 char *FileSh="shapcody",*Format="%-s%-s%-s",
  **Head=(char **)calloc(1,SzV),
  *Fprn="cody.prn",*Help=(char *)calloc(256,SzV);
 int i,*MNN,H;
 Head[0]=(char *)calloc(256,SzC); MNN=(int *)calloc(4,SzI);
 for(i=0;i<3;i++) MNN[i]=i+1;
 sprintf(Head[0],
  "Коды ТМЦ, взятые из базы данных за период с %s %d г. по %s %d г.",
  NameMonta[NM-KolMont<1?12+NM-KolMont:NM-KolMont-1],
  (NM-KolMont<1?NY-1:NY)+1900,NameMont[NM-1],NY+1900);
 sprintf(Help,"%s\\SnabHelp\\KodirTMC.htm",WORKDIR);
 CHANGECODS=false;
 EditList=new TEditList((void **)&R,MNN,false,NULL,false,NULL,Format,FileSh,
  Head,Fprn,/*SaveListIzdelie*//*NULL,TestCodyTMC,NULL,2,-1,-1,-1,-1,
  NULL,NULL,1,Help,NULL,NULL);
 if(EditList->Diagnos()){
  Application->CreateForm(__classid(TFormEditList),&FormEditList);
  FormEditList->ShowModal();
 }
 if(MNN) free(MNN); if(Help) free(Help);
 if(Head[0]) free(Head[0]); if(Head) free(Head);
 delete EditList;
} */
//---------------------------------------------------------------------------
/*void TListCodyTMC::ChangeKeeps(){
 TCodTMC *t; Product *p;
 for(p=(WorkAc==atoi(NameBN)?CurrKeep:Curr201);p;p=p->Sled)
  if(FindTMC(p->Name,&t)&&t->GetCodNew()&&(strlen(t->GetCodNew())>4))
   p->NomNumb=TakeMagazine(&MIdKod,t->GetCodNew())->S;
 for(p=(WorkAc==atoi(NameBN)?MontKeep:Mont201);p;p=p->Sled)
  if(FindTMC(p->Name,&t)&&t->GetCodNew()&&(strlen(t->GetCodNew())>4))
   p->NomNumb=TakeMagazine(&MIdKod,t->GetCodNew())->S;
 CHANGEDATA=true;
 FormSnab->StatusBar->Panels->Items[3]->Text="Данные изменились";
}
//---------------------------------------------------------------------------
void TListCodyTMC::ChangeProducts(char *File){
 TCodTMC *t; bool Rez=false; Product *R; FILE *F;
 if(F=fopen(File,"r")){
  R=FileToProduct(F,&MName);
  fclose(F);
  for(Product *p=R;p;p=p->Sled){ Rez=false;
   if(FindTMC(p->Name,&t)&&t->GetCodNew()&&(strlen(t->GetCodNew())>4)){
    p->NomNumb=TakeMagazine(&MIdKod,t->GetCodNew())->S; Rez=true;
  }}
  if(Rez) ProductToFile(R,File);
  DelAllProduct(&R);
}}
//---------------------------------------------------------------------------
void TListCodyTMC::ChangeFile(char *File){
 long Lf,L=0l; FILE *Fi,*Fo; char S[256],*s,*s0,Name[80],NameWin[80],
  *Uch,*BN,*NN;
 TCodTMC *t; struct stat St; bool IsChange=false; int j,K;
 if(!access(File,0)){
  if(Fi=fopen(File,"r")){
   if(Fo=fopen("temp","w")){
    fgets(S,255,Fi); L+=1+fprintf(Fo,S);
    while(fgets(s=S,255,Fi)){
     s0=Pole(s); strcpy(Name,s); s=s0; StrDOStoWIN(Name);
     s0=Pole(s); Uch=s; s=s0;
     s0=Pole(s); BN=s;  s=s0;
     s0=Pole(s); NN=s;  s=s0;
     if(FindTMC(Name,&t)&&t->GetCodNew()&&(strlen(t->GetCodNew())>4)){
      IsChange=true;
      L+=1+fprintf(Fo,"%s\xb3%s\xb3%s\xb3%s\xb3%s",
       StrWINtoDOS(Name),Uch,BN,t->GetCodNew(),s0);
     }
     else
      L+=1+fprintf(Fo,"%s\xb3%s\xb3%s\xb3%s\xb3%s",
       StrWINtoDOS(Name),Uch,BN,NN,s0);
     for(j=0;j<7;j++){ s0=Pole(s); s=s0; }
     switch(K=atoi(s)){
      case 0: break;
      case 1: fgets(S,255,Fi); L+=1+fprintf(Fo,S); break;
      default:
      for(j=1;j<K;j++){
       fgets(s=S,255,Fi);
       s0=Pole(s); L+=fprintf(Fo,"%s\xb3",s); s=s0;
       s0=Pole(s); NN=s; s=s0;
       s0=Pole(s); strcpy(Name,s); s=s0;
       strcpy(NameWin,Name); StrDOStoWIN(NameWin);
       if(FindTMC(NameWin,&t)&&t->GetCodNew()){
        IsChange=true; NN=t->GetCodNew();
       }
       L+=fprintf(Fo,"%s\xb3%s\xb3",NN,Name);
       for(int l=0;l<5;l++){
        s0=Pole(s);
        L+=fprintf(Fo,"%s\xb3",s);
        s=s0;
       }
       s0=Pole(s); NN=s; s=s0;
       s0=Pole(s); strcpy(Name,s); s=s0;
       strcpy(NameWin,Name); StrDOStoWIN(NameWin);
       if(FindTMC(NameWin,&t)&&t->GetCodNew()){
        IsChange=true; NN=t->GetCodNew();
       }
       L+=fprintf(Fo,"%s\xb3%s\xb3",NN,Name);
       L+=1+fprintf(Fo,s);
    }}}
    fclose(Fo);
   }
   else{
    sprintf(Inf,"Не открылся файл <\"temp\"> для преобразования файла\n%s",File);
    Application->MessageBox(Inf,"Внимание",MB_OK|MB_ICONERROR); return;
   }
   fclose(Fi);
  }
  else{
   sprintf(Inf,"Не открылся файл\n%s",File);
   Application->MessageBox(Inf,"Внимание",MB_OK|MB_ICONERROR); return;
 }}
 else{
  sprintf(Inf,"Не найден файл\n%s",File);
  Application->MessageBox(Inf,"Внимание",MB_OK|MB_ICONERROR); return;
 }
 if(IsChange){
  stat("temp",&St); Lf=St.st_size;
  if(L!=Lf) {
   sprintf(Inf,"Длина (%d) вновь образованного файла <%s>\n"
    "не совпала с объемом (%d) записываемой информации",Lf,File,L);
   Application->MessageBox(Inf,"Ошибка",MB_OK|MB_ICONERROR);
   return;
  }
  if(!access(File,0)){
   if(Fi=fopen(File,"w")) fclose(Fi);
   else{
    sprintf(Inf,"Не открыт преобразуемый файл\n<%s>\nдля уничтожения",File);
    Application->MessageBox(Inf,"Ошибка",MB_OK|MB_ICONERROR);
   }
   if(unlink(File)){
    sprintf(Inf,"Не удален преобразуемый файл\n<%s>",File);
    Application->MessageBox(Inf,"Ошибка",MB_OK|MB_ICONERROR);
  }}
  if(rename("temp",File)){
   sprintf(Inf,"Не переименован файл \"temp\" в файл\n<%s>",File);
   Application->MessageBox(Inf,"Ошибка",MB_OK|MB_ICONERROR);
}}}
//---------------------------------------------------------------------------
void TListCodyTMC::Replace(){
 TGroupTMC *g; TPodGroupTMC *p; bool NoGorP=false; Magazine *Mgp=NULL;
 if(Application->MessageBox("Сделать замену кодов в базах данных?","",
     MB_YESNO|MB_ICONQUESTION)==ID_YES){
  char File[16],Arhiv[256]; Magazine *Files=NULL,*mf; struct ffblk FBL; int i,j;
  sprintf(File,"????%3.3d%d.mo?",WorkAc,WorkNk);
  for(j=findfirst(File,&FBL,0);!j;j=findnext(&FBL))
   TakeMagazine(&Files,FBL.ff_name);
  sprintf(File,"???????%c.%3.3d",IntToChar(NKeeps),atoi(NameBN));
  for(j=findfirst(File,&FBL,0);!j;j=findnext(&FBL))
   TakeMagazine(&Files,FBL.ff_name);
  for(mf=Files;mf;mf=mf->Sled) unlink(mf->S);
  DelAllMagazine(&Files);
  ChangeKeeps();
  FormSnab->StatusBar->SimplePanel=true;
  FormSnab->CGauge->MaxValue=KolMont;
  FormSnab->CGauge->ForeColor=clGradientActiveCaption;
  FormSnab->CGauge->ShowText=true;
  FormSnab->CGauge->Progress=0; FormSnab->CGauge->Visible=true;
  for(i=1;i<KolMont;i++){
   int nm=NM-i>0?NM-i:NM-i+12,ny=NM-i>0?NY:NY-1;
   FormSnab->CGauge->Progress++;
   sprintf(Arhiv,"%s\\%d%x\\%d%x%3.3d%d.lzh",DISK,ny,nm,ny,nm,WorkAc,WorkNk);
//   sprintf(File,"%d%x%3.3d%d.mo?",ny,nm,WorkAc,WorkNk);
//   spawnl(P_WAIT,LHA,LHA,"x",Arhiv,File,NULL);
   sprintf(File,"%d%x%3.3d%d.mon",ny,nm,WorkAc,WorkNk);
   UnPack(LHA,Arhiv,File);
   ChangeProducts(File);
//   ChangeFile(File);
   sprintf(File,"%d%x%3.3d%d.mov",ny,nm,WorkAc,WorkNk);
   UnPack(LHA,Arhiv,File);
   ChangeProducts(File);
//   ChangeFile(File);
   sprintf(File,"%c%c?????%c.%3.3d",
    IntToChar(ny%100),IntToChar(nm),IntToChar(NKeeps),atoi(NameBN));
//   spawnl(P_WAIT,LHA,LHA,"x",Arhiv,File,NULL);
   UnPack(LHA,Arhiv,File);
   for(j=findfirst(File,&FBL,0);!j;j=findnext(&FBL))
    TakeMagazine(&Files,FBL.ff_name);
   for(mf=Files;mf;mf=mf->Sled){
    ChangeProducts(mf->S);
//    ChangeFile(mf->S);
   }
//   spawnl(P_WAIT,LHA,LHA,"u",Arhiv,File,NULL);
   Pack(LHA,Arhiv,File);
   DelAllMagazine(&Files);
   sprintf(File,"%d%x%3.3d%d.mo*",ny,nm,WorkAc,WorkNk);
//   spawnl(P_WAIT,LHA,LHA,"u",Arhiv,File,NULL);
   Pack(LHA,Arhiv,File);
  }
  CHANGECODS=false;
  FormSnab->StatusBar->SimplePanel=false;
  FormSnab->CGauge->Progress=0; FormSnab->CGauge->ShowText=false;
 }
 for(TCodTMC *t=R;t;t=t->Sled){
  TGroupTMC *G; TPodGroupTMC *P; TTMC *T; char Kod[16];
  if(t->GetCodNew()){
   if(T=IdentCod->FindKod(t->GetName(),Kod,&G,&P)){
    if(strcmp(t->GetCodNew(),Kod)){
     IdentCod->Replace(G,P,T,t->GetCodNew(),NULL);
     CHANGEGRPTMC=true;
   }}
   else{
    IdentCod->Replace(NULL,NULL,NULL,t->GetCodNew(),t->GetName());
    CHANGEGRPTMC=true;
 }}}
 for(g=IdentCod->GetRg();g;g=g->Sled){
  if(!g->GetNameGr()||!g->GetNameGr()[0]){
   sprintf(Inf,"%s.",g->GetKodGroupTMC());
   TakeMagazine(&Mgp,Inf); NoGorP=true;
  }
  for(p=g->GetRp();p;p=p->Sled)
   if(!p->GetNamePGr()||!p->GetNamePGr()[0]){
    sprintf(Inf,"%s.%s.",g->GetKodGroupTMC(),p->GetKodPodGroupTMC());
    TakeMagazine(&Mgp,Inf); NoGorP=true;
 }}
 if(NoGorP){
  sprintf(Inf,"В результате пополнения в базах данных кодов ТМЦ\n"
   "образовались безымянные группы и (или) подгруппы.\n\n"
   "Просмотрите и распечатайте, если надо, их список в следующем окне...\n\n"
   "Затем проименуйте их в редакторе кодов...");
  Application->MessageBox(Inf,"Внимание!",MB_OK|MB_ICONWARNING);
  sprintf(Inf,"Коды групп и подгрупп без наименования");
  ViewMagazine(Mgp,Inf,"Коды");
  IdentCod->EditIdentCod();
 }
 IdentCod->SaveIdentCod();
 DelAllMagazine(&Mgp);
} */
//---------------------------------------------------------------------------

