#pragma hdrstop

#include <vcl.h>
#include <Clipbrd.hpp>
#include "ConvDW.h"
#include "EditList.h"
#include "CalcStr.h"
#include "EditTextButton.h"
#include <stdio.h>
#include <ctype.h>
#include <sys\stat.h>
#include <io.h>
#include <dir.h>
#include <process.h>
#include <errno.h>
#include "EditListPrn.h"
#include <math.h>
#include "ParamPrint.h"
#include "OtchetForm.h"
#include "SelectForMag.h"
#include <conio.h>
//#include <ComObj.hpp>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "cdiroutl"
#pragma resource "*.dfm"

extern TFormOtchet *FormOtchet;
extern TEditTextButton *EditTextButton;
TFormEditList *FormEditList;
extern TEditList *EditList;
extern wchar_t Inf[],FILESHAP[],WORKDIR[];
TListBox *ListBoxMag; TGroupBox *GroupBoxMag; TStringGrid *ActivSG;
extern TFormEditListPrn *FormEditListPrn;
extern TFormMag *FormMag;
extern TFormParamPrint *FormParamPrint;
int RowFind,ColFind,FontSize=8;
wchar_t FileColorOpt[]=L"coloredit.opt";
static int PoRostu,Ind; static wchar_t *PoleFormat;
bool GREK;
//---------------------------------------------------------------------------
void __fastcall TEditList::SetTabs(TTabControl *T) {
 T->Tabs->Clear();
 for(int i=0;i<Kp+1;i++)
  if((Head&&Head[i])){ wchar_t *s,*s1;
   for(s=Head[i],s1=Inf;*s;s++) *s1++=(*s=='\n')?' ':*s; *s1='\0';
   T->Tabs->Add(Inf);
  }
  else T->Tabs->Add(L" ");
}
//---------------------------------------------------------------------------
__fastcall TFormEditList::TFormEditList(TComponent* Owner):TForm(Owner){
}
//---------------------------------------------------------------------------
void __fastcall TFormEditList::FormCreate(TObject *Sender){
 wchar_t PrintDosExe[256]; int ColCount;
// bool Rez=false; HWND hwnd=Handle; Rez=BringWindowToTop(hwnd);
// ToolButtonPrint->Enabled=false;
 for(ColCount=0;EditList->MasNN[ColCount];ColCount++);
 EditList->VERT=1-EditList->VERT; ToolButtonDirectionClick(Sender);
 Application->CreateForm(__classid(TFormParamPrint),&FormParamPrint);
 FormParamPrint->FilePrn=EditList->FilePrn;
 FormParamPrint->TakeParam(); PoRostu=32000;
 if(!FormMag) Application->CreateForm(__classid(TFormMag), &FormMag);
 if(EditList->FullHead&&EditList->FullHead[0]) {
  int KStr=1+KSimbStr(EditList->FullHead,'\n'); AnsiString Caption;
  long BtnStyle=GetWindowLong(ButtonFullHead->Handle,GWL_STYLE);
  BtnStyle|=BS_MULTILINE;
  SetWindowLong(ButtonFullHead->Handle,GWL_STYLE,BtnStyle);
  ButtonFullHead->Height=12*(KStr+1)*abs(ButtonFullHead->Font->Height)/10;
//  ButtonFullHead->Caption=EditList->FullHead;
  for(wchar_t *s=EditList->FullHead;*s;s++){
   if(*s=='\t') for(int i=0;i<8;i++) Caption+=' ';
   else Caption+=*s;
  }
  ButtonFullHead->Caption=Caption;
  ButtonFullHead->Visible=true;
  TabControlEditList->Align=alNone;
  TabControlEditList->Top=ButtonFullHead->Top+ButtonFullHead->Height;
 }
 else{
  TabControlEditList->Align=alClient;
  ButtonFullHead->Height=0;
 }
 ReadColorOpt();
 if(EditList->Podval&&EditList->Podval[0]){
  ButtonPodval->Height=2*abs(ButtonPodval->Font->Height);
  ButtonPodval->Caption=
   KSimbStr(EditList->Podval,'\n')?L"Подвал":EditList->Podval;
  ButtonPodval->Top=
   TabControlEditList->ClientHeight-ButtonPodval->Height;
  ButtonPodval->Visible=true;
  Caption=L"Просмотр табличных данных";
  StringGridEditList->Color=ColorPhonView;//clWindow;
  StringGridEditList->FixedColor=ColorPhonFixView;
  StringGridEditList->Font->Color=ColorTextView;
 }
 else if(EditList->FullHead&&EditList->FullHead[0]){
  ButtonPodval->Height=0;
  ButtonPodval->Visible=false;
  Caption=L"Просмотр табличных данных";
  StringGridEditList->Color=ColorPhonView;//clWindow;
  StringGridEditList->FixedColor=ColorPhonFixView;
  StringGridEditList->Font->Color=ColorTextView;
 }
 else{
  ButtonPodval->Height=0;
  ButtonPodval->Visible=false;
  Caption=L"Редактирование табличных данных";
  StringGridEditList->Color=ColorPhonEdit;//clInfoBk;
  StringGridEditList->FixedColor=ColorPhonFixEdit;
  StringGridEditList->Font->Color=ColorTextEdit;
 }
 switch(VvodEnter){
  case -1:
   pmVvodEnter->Visible=false;
   if(!wcsstr(Caption.c_str(),L"Просмотр"))
    StatusBarEditList->Panels->Items[1]->Text=L"Ввод по Enter";
   break;
  case  0:
   pmVvodEnter->Caption=L"Сделать Ввод по Enter";
   StatusBarEditList->Panels->Items[1]->Text=L"Ввод без Enter";
   break;
  case  1:
   pmVvodEnter->Caption=L"Сделать Ввод без Enter";
   StatusBarEditList->Panels->Items[1]->Text=L"Ввод по Enter";
   break;
  default:
   pmVvodEnter->Caption=L"Сделать Ввод по Enter";
   StatusBarEditList->Panels->Items[1]->Text=L"Ввод без Enter";
 }
 pmLine->Visible=false;  pmList1->Visible=false;
 pmList2->Visible=false; pmList3->Visible=false;
 pmList4->Visible=false; pmList5->Visible=false;
 if(EditList->Kp){
  pmLine->Visible=true;
  pmList1->Visible=true;
  if(EditList->Head&&EditList->Head[1])
   pmList1->Caption=EditList->Head[1];
 }
 if(EditList->Kp>1){
  pmList2->Visible=true;
  if(EditList->Head&&EditList->Head[2])
   pmList2->Caption=EditList->Head[2];
 }
 if(EditList->Kp>2){
  pmList3->Visible=true;
  if(EditList->Head&&EditList->Head[3])
   pmList3->Caption=EditList->Head[3];
 }
 if(EditList->Kp>3){
  pmList4->Visible=true;
  if(EditList->Head&&EditList->Head[4])
   pmList4->Caption=EditList->Head[4];
 }
 if(EditList->Kp>4){
  pmList5->Visible=true;
  if(EditList->Head&&EditList->Head[5])
   pmList5->Caption=EditList->Head[5];
 }
 pmFind1->Visible =true;  pmFind2->Visible =false; pmFind3->Visible =false;
 pmFind4->Visible =false; pmFind5->Visible =false; pmFind6->Visible =false;
 pmFind7->Visible =false; pmFind8->Visible =false; pmFind9->Visible =false;
 pmFind10->Visible=false; pmFind11->Visible=false; pmFind12->Visible=false;
 if(ColCount> 1) pmFind2->Visible =true; if(ColCount> 2) pmFind3->Visible =true;
 if(ColCount> 3) pmFind4->Visible =true; if(ColCount> 4) pmFind5->Visible =true;
 if(ColCount> 5) pmFind6->Visible =true; if(ColCount> 6) pmFind7->Visible =true;
 if(ColCount> 7) pmFind8->Visible =true; if(ColCount> 8) pmFind9->Visible =true;
 if(ColCount> 9) pmFind10->Visible=true; if(ColCount>10) pmFind11->Visible=true;
 if(ColCount>11) pmFind12->Visible=true;
 pmFromMag->Visible=EditList->Mag(0)?true:false;
 Canvas->Font->Name=L"Courier New";
 LeftColOld=EditList->GetKolFixStlb();
 StringGridEditList->Font->Size=FontSize;
 if(EditList->GetNSumm()&&EditList->GetNSumm(0)){
  HeaderControlEditList->Font->Size=FontSize;
  if(wcsstr(Caption.c_str(),L"Просмотр")){
   HeaderControlEditList->Canvas->Brush->Color=ColorPhonFixView;
   HeaderControlEditList->Canvas->Rectangle(HeaderControlEditList->ClientRect);
   HeaderControlEditList->Font->Color=ColorTextView;
  }
  else {
   HeaderControlEditList->Canvas->Brush->Color=ColorPhonFixEdit;
   HeaderControlEditList->Canvas->Rectangle(HeaderControlEditList->ClientRect);
   HeaderControlEditList->Font->Color=ColorTextEdit;
 }}
 swprintf(PrintDosExe,L"%s\\PrintDos.exe",WORKDIR);
 if(_waccess(PrintDosExe,0)) wcscpy(PrintDosExe,L"PrintDos.exe");
 if(_waccess(PrintDosExe,0)) ToolButtonDOSPrn->Visible=false;
// Select=false;
 if(EditList->GetSelect()){
  ToolButtonSelect->Visible=true; ToolButton5->Visible=true;
 }
 else{
  ToolButtonSelect->Visible=false; ToolButton5->Visible=false;
}}
//---------------------------------------------------------------------------
int FindShap (FILE *FSh,wchar_t *FileSh) {
 wchar_t S[80],SS[1024],Drv[8],Dir[80],Name[16],Ext[4];
 _wfnsplit(FileSh,Drv,Dir,Name,Ext);
 swprintf(S,L"<%s>",Name); _wcsupr(S);
 while(fgetws(SS,1023,FSh)) { if(wcsstr(SS,S)) return 1; } return 0;
}
//---------------------------------------------------------------------------
FILE *OpenShap(wchar_t *AllShap,wchar_t *FileSh){
 FILE *Fi,*Fo; wchar_t S[1024]; long Tv=0,Tn=0; struct _stat Sv,Sn;
 if(!FileSh) return NULL;
 if(!_waccess(AllShap,0)){ _wstat(AllShap,&Sv); Tv=Sv.st_mtime; }
 if(!_waccess( FileSh,0)){ _wstat( FileSh,&Sn); Tn=Sn.st_mtime; }
 if(Tv>Tn){
  if(!(Fi=_wfopen(AllShap,L"r"))) return NULL;
  if(!FindShap(Fi,FileSh)){ fclose(Fi); return NULL; }
  if(Fo=_wfopen(FileSh,L"w")){
   while(fgetws(S,1023,Fi)&&S[0]!='<'&&!wcschr(S,'>')) fputws(S,Fo);
   fclose(Fi); fclose(Fo);
 }}
 return _wfopen(FileSh,L"r");
}
//---------------------------------------------------------------------------
bool __fastcall TEditList::Diagnos(){
// Проверки:
 bool IsMinus=false; wchar_t /**f=FullFormat,*/*s;
 int k,*n,i,K,KolProc,It,KFSh,N/*=0*/,MaxMNN=0; FILE *FSh;
 for(n=MasNN,k=0;*n;n++,k++){
  if(*n<0) IsMinus=true; if(*n>MaxMNN) MaxMNN=*n;
 }
/*
 for(K=k-MaxMNN-1,i=1;*f;f++)
  switch(*f) {
   case 'a': case 'A': case'n':case 'K': case 'ф':case 'К'://русское К
    K+=N-1; break;
   case '%': K++; N=abs(_wtoi(f+1));
 }
*/
 if(IsMinus){
  swprintf(Inf,L"Индекс поля в массиве номеров отображаемых полей\n"
                L"%d-й структуры отрицательный!",i+1);
  Application->MessageBox(Inf,
   (EditList->Podval&&EditList->Podval[0]||
	EditList->FullHead&&EditList->FullHead[0])?
   L"Просмотр невозможен!":L"Коррекция невозможна!",
   MB_OK|MB_ICONWARNING);
  return false;
 }
 KolProc=KSimbStr(FullFormat,'%');
 for(s=FullFormat;*s;s++){
  if(*s=='-'||*s=='%'||*s=='.'||isdigit(*s)) continue;
  if(!SizePole(*s)) {
   swprintf(Inf,L"Запрещенный символ (%c) в строке\n"
    L"спецификаций полей структуры\n(%s)",*s,FullFormat);
  Application->MessageBox(Inf,
   (EditList->Podval&&EditList->Podval[0]||
    EditList->FullHead&&EditList->FullHead[0])?
   L"Просмотр невозможен!":L"Коррекция невозможна!",
   MB_OK|MB_ICONWARNING);
   return false;
 }}
 for(n=MasNN;*n;n++)
  if(*n>KolProc){
   swprintf(Inf,L"Номер поля (%d) в списке отображаемых полей структуры\n"
               L"не должен быть больше общего\nчисла полей (%d) в структуре",
               *n,KolProc);
   Application->MessageBox(Inf,
    (EditList->Podval&&EditList->Podval[0]||
	 EditList->FullHead&&EditList->FullHead[0])?
	L"Просмотр невозможен!":L"Коррекция невозможна!",
    MB_OK|MB_ICONWARNING);
   return false;
 }
 if(!(FSh=OpenShap(FILESHAP,FileSh))) {
  swprintf(Inf,L"Не открылся файл шапки - \"%s\".",FileSh);
  Application->MessageBox(Inf,
   (EditList->Podval&&EditList->Podval[0]||
	EditList->FullHead&&EditList->FullHead[0])?
   L"Просмотр невозможен!":L"Коррекция невозможна!",
   MB_OK|MB_ICONWARNING);
  return false;
 }
 else{/* Число полей в шапке: */
  N=1; KFSh=0;//текущее и общее число столбцов в шапке
  while(!feof(FSh))
   switch(fgetwc(FSh)) {
    case '\n': case EOF:
     if(N>=KFSh) KFSh=N; N=1; break;
    case  179: case 186: N++; break;
  }
  fclose(FSh);
 }
 if(NSumm){
  for(int *n=NSumm;*n;*n++){
   if((*n<0)||(*n>KFSh)){
    if(*n<0)
     swprintf(Inf,L"Значение в массиве индексов суммируемых\n"
      L"столбцов не может быть отрицательным!");
    else
     swprintf(Inf,L"Значение в массиве индексов суммируемых столбцов (%d)\n"
      L"не может быть больше их общего количества (%d)!",*n,KFSh);
    Application->MessageBox(Inf,(EditList->Podval&&EditList->Podval[0]||
	  EditList->FullHead&&EditList->FullHead[0])?
	 L"Просмотр невозможен!":L"Коррекция невозможна!",
     MB_OK|MB_ICONWARNING);
    return false;
 }}}
 return true;
}
//---------------------------------------------------------------------------
void __fastcall TEditList::MakeTab(TTabControl *TabControl,TStringGrid *StringGrid,
  THeaderControl *HeaderControl){
 wchar_t *s,*s1,*Format,*SForm=NULL,**StrSh,*f;
 int *MasF,*MasD0,*MasD,*MasL0,*MasL,*Size,LTab=0,N,SGTop0,HSh,
  KolProcShow,*DL,i,j,*MasF0,H,L,Lhead,T,*msf,*HeaderSum;
 FILE *FSh; void *R;
 StringGrid->Canvas->Font->Name=L"Courier New";
//строим шапку
 if(Shapk) delete Shapk;
 Shapk=new TShapka(FileSh);
 Shapk->Fill(TabControl);//первоначально прописываем кнопки
 if(wcsstr(FormEditList->Caption.c_str(),L"Просмотр"))
  Shapk->Color(FormEditList->GetColorPhonFixView(),
   FormEditList->GetColorTextView());//Настройка цвета для просмотра
 else
  Shapk->Color(FormEditList->GetColorPhonFixEdit(),
   FormEditList->GetColorTextEdit());//Настройка цвета для редактирования
 for(i=0,KolProcShow=0;MasNN[i];i++) KolProcShow++;
 MasF0=(int *)calloc(KolProcShow,SzI);
//строим MasF0
 for(i=0;i<KolProcShow;i++){
  MasF0[i]=SzV; s=FullFormat;
  for(j=0;j<MasNN[i]-1;j++){
   while(*s=='-'||*s=='%'||*s=='.'||isdigit(*s)) s++;
   MasF0[i]+=SizePole(*s++);
 }}
 for(i=KolProcShow-1;i>0;i--) MasF0[i]-=MasF0[i-1];
//создаем Format
 f=Format=(wchar_t *)calloc(wcslen(FullFormat)+1,SzC);
 for(i=0;i<KolProcShow;i++){
  s=FullFormat;
  for(j=0;j<MasNN[i]-1;j++) for(s++;*s&&(*s!='%');) s++;
  *f++=*s++;
  for(;*s&&(*s!='%');) *f++=*s++; *f='\0';
 }
 FSh=OpenShap(FILESHAP,FileSh);
 if(!FSh) return;
M:if(!SizeFormatFild(*Root,Format,NSumm,FSh,MasF0,&MasF,&MasD0,&MasD,
 &MasL0,&MasL,&Size,&SForm,&EMasForm,&EKForm,&HSh,&LTab,StringGrid,
 KolFixStlb,Show0)){
  if(FSh) fclose(FSh); if(Format)free(Format); if(SForm)free(SForm);
  return;
 }
//запомним указатели на элементы исходного списка
 for(R=*Root,i=0;R;i++,R=Sled(R)){
  FormEditList->StringGridEditList->Objects[0][i]=(TObject *)R;
 }
 L=StringGrid->Canvas->TextWidth(L" ");
 for(i=0;i<EKForm;i++) StringGrid->ColWidths[i]=L*(3*MasL[i]+5)/3;
 Shapk->SetNnNk();
 HSh=Shapk->SetHeightShapka();
 Shapk->SetTopShapka(5*TabControl->Font->Size/2);
//Строим итоги
 HeaderSum=NSumm;
 if(HeaderSum&&HeaderSum[0]) { //если есть суммируемые столбцы
  double Sum; wchar_t Q[80]; int l,Pt,pt,W;
  if(HeaderControl->Sections) HeaderControl->Sections->Clear();
  for(i=0;i<EKForm;i++) HeaderControl->Sections->Add();
  HeaderControl->Sections->Items[0]->Width=StringGrid->ColWidths[0]+3;
  for(i=1;i<EKForm;i++)
   HeaderControl->Sections->Items[i]->Width=StringGrid->ColWidths[i]+1;
  W=12*HeaderControl->Canvas->TextWidth(L"Итого")/10;
  if(W<=HeaderControl->Sections->Items[0]->Width)
   HeaderControl->Sections->Items[0]->Text=L"Итого";
  else if(W<=HeaderControl->Sections->Items[1]->Width)
   HeaderControl->Sections->Items[1]->Text=L"Итого";
  for(i=0;HeaderSum[i];i++){
   Sum=0.0; L=Pt=0;
   for(j=0;j<StringGrid->RowCount;j++){
    wcscpy(Q,StringGrid->Cells[HeaderSum[i]-1][j].c_str());
    if(s=wcschr(Q,',')) *s='.'; Sum+=_wtof(Q);
    if(s=wcschr(Q,'.')){
     *s='\0'; pt=wcslen(s+1); *s='.'; if(pt>Pt) Pt=pt;
    }
    l=wcslen(Q); if(l>L) L=l;
   }
   if(fabs(Sum)<1e-8)
    HeaderControl->Sections->Items[HeaderSum[i]-1]->Text=L"";
   else{
    if(wcschr(EMasForm[HeaderSum[i]-1],'s')||
       wcschr(EMasForm[HeaderSum[i]-1],'k')){
     swprintf(Q,L"%%%d.%df",L,Pt); swprintf(Inf,Q,Sum);
    }
    else
     swprintf(Inf,EMasForm[HeaderSum[i]-1],Sum);
    HeaderControl->Sections->Items[HeaderSum[i]-1]->Text=Inf;
  }}
  FormEditList->HeaderControlEditList->Height=24;
  HeaderControl->Visible=true;
 }
 else {
  HeaderControl->Height=0;
  FormEditList->HeaderControlEditList->Visible=false;
 }
 Shapk->Ext(StringGrid);
 for(i=0;i<StringGrid->FixedCols;i++) StringGrid->TabStops[i]=false;
 for(;i<StringGrid->ColCount;i++) StringGrid->TabStops[i]=true;
 StringGrid->SetFocus();
Ex:
 if(FSh) fclose(FSh);
 if(Format) free(Format); if(SForm) free(SForm);
 if(MasF) free(MasF); if(MasD0) free(MasD0); if(MasD) free(MasD);
 if(MasL0) free(MasL0); if(MasL) free(MasL); if(Size) free(Size);
}
//---------------------------------------------------------------------------
int Index(int *Mas,int I){
 for(int i=0;Mas[i];i++) if(Mas[i]==I) return i; return -1;
}
//---------------------------------------------------------------------------
int IndexWithF(int *Mas,wchar_t *FullFormat,int I){
 for(int i=0,l=0;Mas[i];i++)
  if(Mas[i]==I) return i+l;
  else{
   wchar_t *f=FindKSimb(FullFormat,'%',Mas[i]); int r=_wtoi(f+1);
   for(f++;*f&&*f!='%';f++)
    if(*f=='n'||*f=='a'||*f=='A'||*f=='K'||*f=='К'||*f=='ф'||*f=='Ф')
     l+=r-1;
  }
 return -1;
}
//---------------------------------------------------------------------------
static int StrCmp(wchar_t *P,wchar_t *S){
 if(!P){
  if(!S) return 0; if(!S[0]) return 0; return -1;
 }
 if(!S){
  if(!P) return 0; if(!P[0]) return 0; return 1;
 }
 return wcscmp(P,S);
}
//---------------------------------------------------------------------------
void __fastcall TEditList::StrSGToRout(TStringGrid *SG,int i,wchar_t *r){
 int j,k,K; wchar_t *f,*Format,*s,Buf[2048]; long *L;
 for(f=FullFormat,k=0;*f;f++){
  switch(*f){
   case '%': k++; K=_wtoi(f+1); break;
   case 'c':
    if((j=IndexWithF(MasNN,FullFormat,k))!=-1){
     for(s=SG->Cells[j][i].c_str();*s&&(*s==' ');s++);
     *(wchar_t *)r=*s?*s:'\0';
    }
    r+=2*SzC; break;
   case 'd':
    if((j=IndexWithF(MasNN,FullFormat,k))!=-1)
     *(int *)r=_wtoi(SG->Cells[j][i].c_str());
    r+=SzI; break;
   case 'l':
    if((j=IndexWithF(MasNN,FullFormat,k))!=-1)
     *(long *)r=_wtoi(SG->Cells[j][i].c_str());
    r+=SzL; break;
   case 'k':
    if((j=IndexWithF(MasNN,FullFormat,k))!=-1)
     *(long *)r=DtoK(_wtof(SG->Cells[j][i].c_str()));
    r+=SzL; break;
   case 's':case 'm':
    if((j=IndexWithF(MasNN,FullFormat,k))!=-1) {
     CleanStr(SG->Cells[j][i].c_str(),Buf);
     if(StrCmp(*(wchar_t **)r,Buf)){
//      if(MM&&*MM&&(*MM)[j])
//       *(wchar_t **)r=TakeMagazine((*MM)+j,Buf)->S;
      if(MM&&MM[j]&&AddMM&&AddMM[j])
       *(wchar_t **)r=TakeMagazine(MM[j],Buf)->S;
      else{ int l;
//если был указатель, разберем его
      // if(*(wchar_t **)r) free(*(wchar_t **)r);
        l=wcslen(Buf);
       if(l){ //если строка не пуста, заполним ее, иначе обNULLим
        s=(wchar_t *)calloc(l+1,SzC);
        wcscpy(s,Buf); *(wchar_t **)r=s;
       }
       else *(wchar_t **)r=NULL;
    }}}
    r+=SzV; break;
   case 'f':case 'g':case 'e':
//    if(((j=IndexWithF(MasNN,FullFormat,k))!=-1)&&(j>=KolFixStlb)) {
    if((j=IndexWithF(MasNN,FullFormat,k))!=-1) {
     wcscpy(Buf,SG->Cells[j][i].c_str());
     ReplaceChar(Buf,',','.');
     *(double *)r=_wtof(Buf);
    }
    r+=SzD; break;
   case 'a':
    if((j=IndexWithF(MasNN,FullFormat,k))!=-1) {
     double *D=*(double **)r;
     for(int l=0;l<K;l++){
      wcscpy(Buf,SG->Cells[j+l][i].c_str()); ReplaceChar(Buf,',','.');
      D[l]=_wtof(Buf);
    }}
    r+=SzV; break;
   case 'U':
    if((j=IndexWithF(MasNN,FullFormat,k))!=-1) {
     DouSt *D=*(DouSt **)r;
     for(int l=0;l<K;l++){
      if(D[l].Atr==IsD){
       wcscpy(Buf,SG->Cells[j+l][i].c_str()); ReplaceChar(Buf,',','.');
       D[l].U.D=_wtof(Buf);
      }
      else{
       D[l].U.S=(wchar_t *)calloc(SG->Cells[j+l][i].Length()+1,SzC);
       wcscpy(D[l].U.S,SG->Cells[j+l][i].c_str());
    }}}
    r+=SzV; break;
   case 'К'://русское К
    if((j=IndexWithF(MasNN,FullFormat,k))!=-1) {
     MasLinF *M=(MasLinF *)r;
     L=(long *)calloc(K,SzL);
     for(int l=0;l<K;l++){
      wcscpy(Buf,SG->Cells[j+l][i].c_str());
      ReplaceChar(Buf,',','.');
      L[l]=DtoK(_wtof(Buf));
     }
     M->WriteML(K,L);
     free(L);
    }
    r+=sizeof(MasLinF);
    break;
}}}
//---------------------------------------------------------------------------
void __fastcall TEditList::StringGridToRout(TStringGrid *SG){
 int i,j,k,K; wchar_t *f,*Format,*s,*r; long *L;
 if((SG->RowCount==1)&&(SG->Objects[0][0]==(TObject *)Kukla)){ wchar_t B='\0';
  for(i=0;i<SizeR;i++) B+=((wchar_t *)(*Root))[i];
  if(!B){ if(Kukla) free(Kukla); *Root=NULL; return;}
 }
 for(i=0;i<SG->RowCount;i++){
  r=(wchar_t *)SG->Objects[0][i];
  if(r){
   *(void **)r=i<(SG->RowCount-1)?(void *)SG->Objects[0][i+1]:NULL;
   StrSGToRout(SG,i,r+SzV);
 }}
 *Root=(void *)SG->Objects[0][0];
}
//---------------------------------------------------------------------------
void __fastcall TFormEditList::TabControlEditListChange(TObject *Sender){
//Процедура (событие) построения страницы
 int Ind=TabControlEditList->TabIndex,L,T,W,H;
 if(Ind&&EditList->ProcList&&EditList->ProcList[Ind-1]){
//страница второго плана
  wchar_t *r,*s; TFormEditList *FormEditListOld=FormEditList;
  int i=0,Offs=SzV,Row=StringGridEditList->Row,porostu=PoRostu,
   TopRow=StringGridEditList->TopRow,Col=StringGridEditList->Col;
  FormSizes(L,T,W,H);
  if(EditList->TopF==-1) Top=T;
  EditList->StringGridToRout(StringGridEditList);
  StringGridEditList->TopRow=Row;
  r=(wchar_t *)StringGridEditList->Objects[0][StringGridEditList->Row];
  for(s=EditList->FullFormat;*s;s++){
   switch(*s){
    case 'c': Offs+=2*SzC; break;
    case 'd': Offs+=SzI; break;
    case 'l': Offs+=SzL; break;
    case 's': case 'm': Offs+=SzV; break;
    case 'f': case 'e': case 'g': Offs+=SzD; break;
    case 'p': i++; if(i==Ind) goto M; Offs+=SzV; break;
    case 'K': Offs+=SzV; break;
    case 'A': case 'a': case 'ф': case 'U':
    case 'n': Offs+=SzV; break;
    case 'К': Offs+=sizeof(MasLinF); break;//русское или не русское ??
  }}
M:EditList->ProcList[Ind-1]((void **)(r+Offs));
  FormEditList=FormEditListOld;
  StringGridEditList->Row=Row; StringGridEditList->Col=Col;
  EditList->MakeTab(TabControlEditList,StringGridEditList,HeaderControlEditList);
  StringGridEditList->TopRow=TopRow;
  FormEditList->TabControlEditList->TabIndex=0;
  ActivSG=StringGridEditList;
  PoRostu=porostu;
 }
 else{
  if(StringGridEditList->Color==clSilver)
   StringGridEditList->Color=clWhite;
  StringGridEditList->Enabled=true;
  ActivSG=StringGridEditList;
  if(StringGridEditList->RowCount==1||
     StringGridEditList->ColCount==1){
   EditList->MakeTab(TabControlEditList,StringGridEditList,HeaderControlEditList);
   if(ListBoxMag){ delete(ListBoxMag); ListBoxMag=NULL;}
   if(GroupBoxMag){ delete(GroupBoxMag); GroupBoxMag=NULL;}
   GroupBoxMag=new TGroupBox(this);
   GroupBoxMag->Parent=this;
   GroupBoxMag->Align=alNone; GroupBoxMag->Color=clSilver;
   ListBoxMag=new TListBox(GroupBoxMag);
   ListBoxMag->Parent=GroupBoxMag;
   ListBoxMag->Align=alClient;
   GroupBoxMag->Visible=false;
   ListBoxMag->OnKeyDown=ListBoxMagKeyDown;
   ListBoxMag->OnDblClick=ListBoxMagDblClick;
  }
  else StringGridEditList->SetFocus();
}}
//---------------------------------------------------------------------------
int TEditList::WhatLarge(int J,wchar_t *Format,wchar_t *SPole){
 wchar_t B,B1='\0',*f=Format,S[2048],Form[16]; int Ndo,Npos=0,L; double D;
 B=Format[wcslen(Format)-1];
 if(wcschr(Format,'-')) B1='-';
 if(wcschr(Format,'+')) B1='+';
 Ndo=_wtoi(f+(B1?2:1));
 if(f=wcschr(Format,'.')) Npos=_wtoi(f+1);
 CleanStr(SPole,S);
 switch(B){
  case 'd': L=_wtoi(S); swprintf(S,L"%d",L); break;
  case 'f':
   D=_wtof(S); swprintf(Form,L"%%0.%df",Npos); swprintf(S,Form,D); break;
  case 'e':
   D=_wtof(S); swprintf(Form,L"%%0.%d.3e",Npos); swprintf(S,Form,D); break;
 }
 L=wcslen(S);
 if(L>=Ndo){ //если больше, чем ширина колонки
  switch(B){
   case 's':case 'm':case 'd':
    if(B1)
     swprintf(Form,L"%%%c%d%c",B1,L,B);
    else
     swprintf(Form,L"%%%d%c",L,B);
    break;
   case 'f':
    if(B1)
     swprintf(Form,L"%%%c%d.%d%c",B1,L,Npos,B);
    else
     swprintf(Form,L"%%%d.%d%c",L,Npos,B);
    break;
   case 'e':
    if(B1)
     swprintf(Form,L"%%%c%d.%d.3%c",B1,L,Npos,B);
    else
     swprintf(Form,L"%%%d.%d%c",L,Npos,B);
    break;
   case 'g':
    if(B1) swprintf(Form,L"%%%c%d%c",B1,L,B);
    else swprintf(Form,L"%%%d%c",L,B);
    break;
  }
  wcscpy(Format,Form); SetEMasFormJ(J,Form);
  return 1;
 }
 return 0;
}
//---------------------------------------------------------------------------
bool __fastcall TFormEditList::NoClose(){
 if(EditList->Shapk){
  if(EditList->TestField){
   int ColOld=EditList->Col,RowOld=EditList->Row;
   if((StringGridEditList->Cells[ColOld][RowOld]!=EditList->Value)&&
       EditList->TestField(StringGridEditList,RowOld,ColOld+1,
        EditList->Value.w_str()))
//Вернуть старое, редактирование запрещено
    StringGridEditList->Cells[ColOld][RowOld]=EditList->Value;
   if(EditList->TestField(StringGridEditList,-1,
      StringGridEditList->ColCount+2,L"")) return true;
  }
  if(PoRostu!=32000) BackSort();
  EditList->StringGridToRout(StringGridEditList);
  delete EditList->Shapk; EditList->Shapk=NULL;
 }
 return false;
}
//---------------------------------------------------------------------------
void __fastcall TFormEditList::FormClose(TObject *Sender,TCloseAction &Action){
 if(NoClose()) Action=caNone;
}
//---------------------------------------------------------------------------
void __fastcall TFormEditList::StringGridEditListKeyDown(TObject *Sender,
      WORD &Key, TShiftState Shift){
 THeaderControl *ActivHeaderControl; wchar_t *Format; KEY=Key;
 switch(Key){
  case VK_DELETE:
   if(!StringGridEditList->EditorMode) pmCutClick(Sender); break;
  case VK_F3:
   if(EditList->Kp){
    TabControlEditList->TabIndex=1; TabControlEditListChange(Sender);
   }
   break;
  case VK_F4:
   if(Shift.Contains(ssAlt)){ Close(); return; }
   if(EditList->Kp>1){
    TabControlEditList->TabIndex=2; TabControlEditListChange(Sender);
   }
   break;
  case VK_F5:
   if(EditList->Kp>2){
    TabControlEditList->TabIndex=3; TabControlEditListChange(Sender);
   }
   break;
  case VK_F6:
   if(EditList->Kp>3){
    TabControlEditList->TabIndex=4; TabControlEditListChange(Sender);
   }
   break;
  case 'H':
   if(Shift.Contains(ssCtrl)) ToolButtonRepClick(Sender); break;
  case 'P':
   if(Shift.Contains(ssCtrl)) ToolButtonPrintClick(Sender); break;
  case 'G':
   if(Shift.Contains(ssCtrl)) ToolButtonDOSPrnClick(Sender); break;
  case 'M':
   if(Shift.Contains(ssAlt))  pmFromMagClick(Sender); break;
  case VK_ESCAPE://закрытие формы EditList путем просто скрытия
   Close(); break;
//изменение порядка перемещения активной ячейки(гориз.-0/вертик.-1)
  case VK_TAB:
   if(Shift.Contains(ssCtrl)) ToolButtonDirectionClick(Sender); break;
  case VK_RETURN:
   if(EditList->GetSelect())
 //  Shift.Contains(ssCtrl)||Shift.Contains(ssAlt))
    ToolButtonSelectClick(Sender);
 //Работа с магазином
 //перемещение активной ячейки
   else if(EditList->VERT){ //перемещ. по верт.
   //если не последняя строка
    if(ActivSG->Row<ActivSG->RowCount-1) ActivSG->Row++;
    else {
     if(ActivSG->Col<ActivSG->ColCount-1) ActivSG->Col++;
     else {
      if(ActivSG->Col) ActivSG->Col--;
      if(ActivSG->Col<ActivSG->FixedCols)
       ActivSG->Col=ActivSG->FixedCols;
   }}}
   else{//перемещ. по горизонт.
    if(ActivSG->Col<ActivSG->ColCount-1) ActivSG->Col++;
    else {//если последний столбец
     if(ActivSG->Row<ActivSG->RowCount-1){//если не последняя строка
      ActivSG->Row++; ActivSG->Col=ActivSG->FixedCols;
     }
     else{//увеличиваем количество строк на 1
      if(EditList->TestField&&
         EditList->TestField(ActivSG,ActivSG->Row,0,NULL)){
       ActivSG->Col--;
       if(ActivSG->Col<ActivSG->FixedCols) ActivSG->Col=ActivSG->FixedCols;
      }
      else{
/*       if(PoRostu!=32000)
        Application->MessageBox(
         L"Нельзя редактировать в пересортированной таблице",
         L"Ошибка!",MB_OK|MB_ICONERROR);
       else*/
       if(PoRostu==32000)
        pmAddStrClick(Sender);
}}}}}}
//---------------------------------------------------------------------------
void __fastcall TEditList::ChangeWidthCol(int *HeaderSumm,int Col,
  THeaderControl *ActivHeaderControl,TStringGrid *ActivSG){
 int i,L,W=0,j,NewWidth,l; wchar_t S[1024],B,*Format;
 Format=FormatCol(Col); B=Format[wcslen(Format)-1];
//заполняем столбец по новому формату
 for(i=0;i<ActivSG->RowCount;i++) {
  wcscpy(Inf,ActivSG->Cells[Col][i].c_str()); CleanStr(Inf,S);
  switch(B){
   double D; int d;
   case 's':case 'm':
    l=swprintf(Inf,Format,S); break;
   case 'f':
    ReplaceChar(S,',','.'); D=_wtof(S);
    if((D!=0.0)||Show0) l=swprintf(Inf,Format,D); else l=Inf[0]='\0'; break;
   case 'e':
    ReplaceChar(S,',','.'); D=_wtof(S);
    if((D!=0.0)||Show0) l=swprintf(Inf,Format,D); else l=Inf[0]='\0'; break;
   case 'g':
    ReplaceChar(S,',','.'); D=_wtof(S);
    if((D!=0.0)||Show0) l=swprintf(Inf,Format,D); else l=Inf[0]='\0'; break;
   case 'd':
    d=_wtoi(S); if(d||Show0) l=swprintf(Inf,Format,d); else l=Inf[0]='\0'; break;
  }
  ActivSG->Cells[Col][i]=Inf; if(l>W) W=l;
 }
//изменяем ширину столбца
 L=ActivSG->Canvas->TextWidth(L" ");
// W=abs(_wtoi(Format+1));
// if(HeaderSumm&&HeaderSumm[0]){
//  j=Index(HeaderSumm,Col+1); if(j>-1) W++;
// }
 NewWidth=L*(3*W+5)/3;
 if(NewWidth>ActivSG->ColWidths[Col]){
  ActivSG->Width+=NewWidth-ActivSG->ColWidths[Col];
  ActivSG->ColWidths[Col]=NewWidth;
  if(HeaderSumm&&HeaderSumm[0]) { //если есть суммируемые столбцы
   ActivHeaderControl->Sections->Items[Col]->Width=
    ActivSG->ColWidths[Col]+(Col?1:3);
  }
  //изменяем ширину кнопки шапки
  Shapk->ExtFromCol(Col,ActivSG);
}}
//---------------------------------------------------------------------------
void __fastcall TFormEditList::ListBoxMagKeyDown(TObject *Sender,
      WORD &Key, TShiftState Shift){
 if(Key==VK_ESCAPE){
  StringGridEditList->SetFocus();
  GroupBoxMag->Visible=false;
 }
 if(Key==VK_RETURN) ListBoxMagDblClick(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TFormEditList::ListBoxMagDblClick(TObject *Sender){
//
 WORD Key=VK_RETURN; TShiftState ST;
 int Row=StringGridEditList->Row,Col=StringGridEditList->Col;
 StringGridEditList->Cells[Col][Row]=
  ListBoxMag->Items->Strings[ListBoxMag->ItemIndex];
 StringGridEditListKeyDown(Sender,Key,ST);
 GroupBoxMag->Visible=false;
 StringGridEditList->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TFormEditList::ToolButtonQuitClick(TObject *Sender){ Close(); }
//---------------------------------------------------------------------------
void __fastcall TFormEditList::StringGridEditListTopLeftChanged(
      TObject *Sender){
// Обработка события сразу после изменения свойств TopRow или LeftCol
 TStringGrid *SG=StringGridEditList;
 THeaderControl *HC=HeaderControlEditList;
 if(SG->LeftCol!=LeftColOld){
  if(EditList->GetNSumm()&&EditList->GetNSumm()[0]){
   for(int i=EditList->GetKolFixStlb();i<SG->ColCount;i++)
    HC->Sections->Items[i]->Width=(i<SG->LeftCol)?0:(SG->ColWidths[i]+1);
  }
  EditList->Shapk->SetLeft(SG);
//  EditList->Shapk->SetLeftExt(LeftColOld,SG);
  LeftColOld=SG->LeftCol;
}}
//---------------------------------------------------------------------------
void __fastcall TFormEditList::ToolButtonDirectionClick(TObject *Sender){
 EditList->VERT=1-EditList->VERT;
 if(EditList->VERT){
  StatusBarEditList->Panels->Items[0]->Text=L"Вниз";
  ToolButtonDirection->ImageIndex=10;
 }
 else{
  StatusBarEditList->Panels->Items[0]->Text=L"Вправо";
  ToolButtonDirection->ImageIndex=11;
}}
//---------------------------------------------------------------------------
void __fastcall TFormEditList::CalcNewSums(bool ReCalc){
 int *HeaderSumm=EditList->GetNSumm(),ChangFormatColHead=0; wchar_t *Format;
 if(!HeaderSumm||!(HeaderSumm[0])) return;
 //если есть суммируемые столбцы
 for(int j=0;j<ActivSG->ColCount;j++){
  Format=EditList->FormatCol(j);
  int N=Index(HeaderSumm,j+1);
  if(N>-1){
   double Sum=0.0; int i; wchar_t S[256],*s;
   for(i=0;i<ActivSG->RowCount;i++) {
    wcscpy(S,ActivSG->Cells[j][i].c_str());
    if(s=wcschr(S,',')) *s='.'; Sum+=_wtof(S);
   }
   ChangFormatColHead=ReCalc||EditList->WhatLarge(j,Format,S);
   swprintf(S,Format,Sum);
   if(fabs(Sum)>1e-8)
    HeaderControlEditList->Sections->Items[j]->Text=S;
   else
    HeaderControlEditList->Sections->Items[j]->Text=L"";
  }
  if(ChangFormatColHead) { //перестраиваем ширину столбца таблицы (ЗАГОЛОВОК, ЗНАЧЕНИЯ И ИТОГИ)
   ChangFormatColHead=0;
   EditList->ChangeWidthCol(HeaderSumm,j,HeaderControlEditList,ActivSG);
}}}
//---------------------------------------------------------------------------
void __fastcall TEditList::DeleteStr(int N){
 void *r,*Sld; int i; if(!*Root) return;
 if(!N) {
  r=Sled(*Root);
  if(*Root==Kukla) Kukla=NULL; free(*Root); *Root=r;
 }
 else {
  for(r=*Root,i=0;r&&(i<(N-1));i++,r=Sled(r));
  if(r){
   Sld=Sled(r); r=Sled(Sld);
   if(Sld==Kukla) Kukla=NULL; free(Sld);
}}}
//---------------------------------------------------------------------------
void *__fastcall TEditList::InsStr(int N,TStringGrid *SG){
 wchar_t *f; int i; void *p,*r=calloc(1,SizeR);
 if(!N){
  r=*Root; *Root=r;
  if(Row>-1)
  if(SG) StrSGToRout(SG,SG->Row,(wchar_t *)r+SzV);
  return r;
 }
 for(p=*Root,i=0;p&&i<N-1;i++,p=Sled(p));
 if(p) {
  r=Sled(p);
  p=r;
  if(SG) StrSGToRout(SG,SG->Row,(wchar_t *)r+SzV);
  return r;
 }
 return NULL;
}
//---------------------------------------------------------------------------
void __fastcall TFormEditList::pmDelStrClick(TObject *Sender){
//удаление строки
/* if(PoRostu!=32000){
  Application->MessageBox(
   L"Нельзя редактировать в пересортированной таблице",
   L"Ошибка!",MB_OK|MB_ICONERROR); return;
 }*/
 int Row,in,ik; bool(*T)(TStringGrid *,int,int,wchar_t *)=EditList->TestField;
 ActivSG=StringGridEditList;
 in=ActivSG->Selection.Left; ik=ActivSG->Selection.Right;
 if((ik-in+1)==ActivSG->ColCount){
  in=ActivSG->Selection.Top; ik=ActivSG->Selection.Bottom;
 }
 else if(ActivSG->Selection.Top==ActivSG->Selection.Bottom)
  in=ik=ActivSG->Row;
 else{
  Application->MessageBox(L"Непонятно, что удалять?\n"
   L" - строки или содержимое выделенных ячеек?",L"Ошибка!",MB_OK|MB_ICONERROR);
  return;
 }
 ActivSG->Row=in;
 for(int i=in;i<=ik;i++){
  if(EditList->TestField&&
     EditList->TestField(ActivSG,ActivSG->Row,-1,NULL))
   return;
  EditList->DeleteStr(ActivSG->Row);
  for(int j=ActivSG->Row;j<ActivSG->RowCount-1;j++){
   for(int k=0;k<ActivSG->ColCount;k++)
    ActivSG->Cells[k][j]=ActivSG->Cells[k][j+1];
   ActivSG->Objects[0][j]=ActivSG->Objects[0][j+1];
   ActivSG->Objects[1][j]=ActivSG->Objects[1][j+1];
  }
  if(ActivSG->RowCount==1){
   EditList->Kukla=calloc(EditList->SizeR,SzC);
   ActivSG->Objects[0][0]=(TObject *)EditList->Kukla;
   EditList->SetR(&EditList->Kukla);
   for(int j=0;j<ActivSG->ColCount;j++) ActivSG->Cells[j][0]=L"";
  }
  else ActivSG->RowCount--;
 }
 CalcNewSums(false);
 EditList->Value=StringGridEditList->Cells[ActivSG->Col][ActivSG->Row];
 Row=ActivSG->Row;
 EditList->TestField=NULL;
// FormShow(Sender);
 TabControlEditListChange(Sender);
 EditList->TestField=T;
 ActivSG->Row=Row;
}
//---------------------------------------------------------------------------
void InsCopyStr(){
 ActivSG->RowCount++;
 for(int i=ActivSG->RowCount-1;i>ActivSG->Row;i--){
  for(int j=0;j<ActivSG->ColCount;j++)
   ActivSG->Cells[j][i]=ActivSG->Cells[j][i-1];
  ActivSG->Objects[0][i]=ActivSG->Objects[0][i-1];
}}
//---------------------------------------------------------------------------
void __fastcall TFormEditList::pmInsCopyStrClick(TObject *Sender){
 //вставить копию текущей строки перед ней
 if(PoRostu!=32000){
  Application->MessageBox(
   L"Нельзя вставлять строки в пересортированной таблице",
   L"Ошибка!",MB_OK|MB_ICONERROR); return;
 }
 int Row; bool(*T)(TStringGrid *,int,int,wchar_t *)=EditList->TestField;
 if(EditList->TestField&&
    EditList->TestField(ActivSG,ActivSG->Row,0,NULL))
  return;
 InsCopyStr();
 ActivSG->Objects[0][ActivSG->Row]=(TObject *)EditList->InsStr(ActivSG->Row,ActivSG);
 if(EditList->TestField)
  EditList->TestField(ActivSG,ActivSG->Row,0,L"Новая");
 CalcNewSums(false);
 Row=ActivSG->Row;
 EditList->TestField=NULL;
// FormShow(Sender);
 TabControlEditListChange(Sender);
 EditList->TestField=T;
 ActivSG->Row=Row;
}
//---------------------------------------------------------------------------
void __fastcall TFormEditList::pmInsStrClick(TObject *Sender){
//вставить пустую строку перед текущей
 if(PoRostu!=32000){
  Application->MessageBox(
   L"Нельзя вставлять строки в пересортированной таблице",
   L"Ошибка!",MB_OK|MB_ICONERROR); return;
 }
 int Row; bool(*T)(TStringGrid *,int,int,wchar_t *)=EditList->TestField;
 if(EditList->TestField&&
    EditList->TestField(ActivSG,ActivSG->Row,0,NULL))
  return;
 InsCopyStr();
 for(int j=0;j<ActivSG->ColCount;j++)
  ActivSG->Cells[j][ActivSG->Row]=L"";
 ActivSG->Objects[0][ActivSG->Row]=(TObject *)EditList->InsStr(ActivSG->Row,ActivSG);
 if(EditList->TestField)
  EditList->TestField(ActivSG,ActivSG->Row,0,L"Новая");
 Row=ActivSG->Row;
 EditList->Value=ActivSG->Cells[ActivSG->Col][ActivSG->Row];
 EditList->TestField=NULL;
// FormShow(Sender);
 TabControlEditListChange(Sender);
 EditList->TestField=T;
 ActivSG->Row=Row;
}
//---------------------------------------------------------------------------
void __fastcall TFormEditList::pmAddCopyStrClick(TObject *Sender){
 //вставить копию текущей строки после нее
 if(PoRostu!=32000){
  Application->MessageBox(
   L"Нельзя ставлять строки в пересортированной таблице",
   L"Ошибка!",MB_OK|MB_ICONERROR); return;
 }
 int Row; bool(*T)(TStringGrid *,int,int,wchar_t *)=EditList->TestField;
 if(EditList->TestField&&
    EditList->TestField(ActivSG,ActivSG->Row,0,NULL))
  return;
 InsCopyStr(); ActivSG->Row++; Row=ActivSG->Row;
 ActivSG->Objects[0][ActivSG->Row]=(TObject *)EditList->InsStr(ActivSG->Row,ActivSG);
 if(EditList->TestField)
  EditList->TestField(ActivSG,ActivSG->Row,0,L"Новая");
 CalcNewSums(false);
 EditList->TestField=NULL;
// FormShow(Sender);
 TabControlEditListChange(Sender);
 EditList->TestField=T;
 ActivSG->Row=Row;
}
//---------------------------------------------------------------------------
void __fastcall TFormEditList::pmAddStrClick(TObject *Sender){
 //вставить пустую строку после текущей
 if(PoRostu!=32000){
  Application->MessageBox(
   L"Нельзя вставлять строки в пересортированной таблице",
   L"Ошибка!",MB_OK|MB_ICONERROR); return;
 }
 int Row; bool(*T)(TStringGrid *,int,int,wchar_t *)=EditList->TestField;
 if(EditList->TestField&&
    EditList->TestField(ActivSG,ActivSG->Row,0,NULL))
  return;
 InsCopyStr();
 for(int j=0;j<ActivSG->ColCount;j++)
  ActivSG->Cells[j][ActivSG->Row+1]=L"";
 ActivSG->Row++; Row=ActivSG->Row;
 ActivSG->Objects[0][ActivSG->Row]=(TObject *)EditList->InsStr(ActivSG->Row,ActivSG);
 if(EditList->TestField)
  EditList->TestField(ActivSG,ActivSG->Row,0,L"Новая");
 EditList->Value=ActivSG->Cells[ActivSG->Col][ActivSG->Row];
 EditList->TestField=NULL;
// EditList->StringGridToRout(StringGridEditList);
// FormShow(Sender);
 TabControlEditListChange(Sender);
 EditList->TestField=T;
 ActivSG->Row=Row;
}
//---------------------------------------------------------------------------
void __fastcall TFormEditList::pmSaveClick(TObject *Sender){
 if(EditList->Save){
  EditList->StringGridToRout(StringGridEditList);
  EditList->Save();
}}
//---------------------------------------------------------------------------
void __fastcall TFormEditList::StringGridEditListSelectCell(
      TObject *Sender, int ACol, int ARow, bool &CanSelect){
 int ColOld=EditList->Col,RowOld=EditList->Row; double D; int d;
// int ColOld=ActivSG->Col,RowOld=ActivSG->Row; double D; int d;
 wchar_t S[2048],*Format=EditList->FormatCol(ColOld),Buf[2048];
 bool Edit=(EditList->Value!=StringGridEditList->Cells[ColOld][RowOld]);
 wcscpy(Buf,ActivSG->Cells[ColOld][RowOld].c_str());
 CleanStr(Buf,S);
 if((Format[wcslen(Format)-1]=='f')||(Format[wcslen(Format)-1]=='g')){
  ReplaceChar(S,',','.'); D=CalcStr(S);
  if(D!=0.0||EditList->Show0){
   swprintf(Buf,Format,D); ActivSG->Cells[ColOld][RowOld]=Buf;
 }}
// if(EditList->Value!=StringGridEditList->Cells[ColOld][RowOld]||
 if(Edit||(KEY==VK_RETURN)){
//Было редактирование ячейки
//если есть итоги - пересчитываем сумму
//при нажатии Enter на числовом поле
  int ChangFormatCol=0,ChangFormatColHead/*=0*/;
/*  if(PoRostu!=32000){
   Application->MessageBox(
    L"Нельзя редактировать в пересортированной таблице",
    L"Ошибка!",MB_OK|MB_ICONERROR);
   StringGridEditList->Cells[ColOld][RowOld]=EditList->Value;
   ACol=ColOld; ARow=RowOld;
   CanSelect=false;
   KEY=0; return;
  }*/
  if(VvodEnter&&(KEY!=VK_RETURN)){//вышли из ячейки без Enter
   StringGridEditList->Cells[ColOld][RowOld]=EditList->Value;
//   ACol=ColOld; ARow=RowOld;
//   CanSelect=false;
   return; //goto M;
  }
  KEY=0;
  if(EditList->TestField&&
     EditList->TestField(StringGridEditList,RowOld,ColOld+1,
      EditList->Value.c_str())){
  //Вернуть старое, редактирование запрещено
   StringGridEditList->Cells[ColOld][RowOld]=EditList->Value;
//   if(EditList->VERT) StringGridEditList->Row--;
//   else StringGridEditList->Col--;
//   ACol=ColOld; ARow=RowOld;
   CanSelect=false;
   return; //goto M;
  }
  EditList->StrSGToRout(ActivSG,RowOld,((wchar_t *)ActivSG->Objects[0][RowOld])+SzV);
  wcscpy(Inf,ActivSG->Cells[ColOld][RowOld].c_str());
  CleanStr(Inf,S);
  switch(Format[wcslen(Format)-1]){
   case 's':case 'm': //а если больше, чем ширина колонки
    ChangFormatCol=EditList->WhatLarge(ColOld,Format,S);
    swprintf(Buf,Format,S);
    break;
   case 'f':
//    ReplaceChar(S,',','.'); D=CalcStr(S); swprintf(Inf,Format,D);
    ChangFormatCol=EditList->WhatLarge(ColOld,Format,Buf);
    if((D!=0.0)||EditList->show0()) swprintf(Buf,Format,D);
    else Buf[0]='\0';
    break;
   case 'e':
//    ReplaceChar(S,',','.'); D=CalcStr(S); swprintf(Inf,Format,D);
    ChangFormatCol=EditList->WhatLarge(ColOld,Format,Buf);
    if((D!=0.0)||EditList->show0()) swprintf(Buf,Format,D);
    else Buf[0]='\0';
    break;
   case 'g':
//    ReplaceChar(S,',','.'); D=CalcStr(S); swprintf(Inf,Format,D);
    ChangFormatCol=EditList->WhatLarge(ColOld,Format,Buf);
    if((D!=0.0)||EditList->show0()) swprintf(Buf,Format,D);
    else Buf[0]='\0';
    break;
   case 'd':
    d=Round(CalcStr(S)); swprintf(Buf,Format,d);
    ChangFormatCol=EditList->WhatLarge(ColOld,Format,Buf);
    if(d||EditList->show0()) swprintf(Buf,Format,d);
    else Buf[0]='\0';
    break;
  }
  if(ChangFormatCol)
  //перестраиваем ширину столбца таблицы (ЗАГОЛОВОК, ЗНАЧЕНИЯ И ИТОГИ)
   EditList->ChangeWidthCol(EditList->GetNSumm(),ColOld,HeaderControlEditList,ActivSG);
  if(EditList->GetNSumm()&&EditList->GetNSumm(0)){//если есть суммируемые столбцы
   for(int N,j=0;N=EditList->GetNSumm(j);j++){
    double Sum=0.0; N--;
    for(int i=0;i<ActivSG->RowCount;i++){ wchar_t *s;
     wcscpy(Inf,ActivSG->Cells[N][i].c_str());
     if(s=wcschr(Inf,',')) *s='.'; Sum+=_wtof(Inf);
    }
    swprintf(Buf,EditList->FormatCol(N),Sum);
    ChangFormatColHead=EditList->WhatLarge(N,EditList->FormatCol(N),Buf);
//    for(s=Buf;*s&&(*s==' ');s++);
//     HeaderControlEditList->Sections->Items[N]->Text=fabs(Sum)<1e-8?"":s;
    HeaderControlEditList->Sections->Items[N]->Text=Buf;
    if(ChangFormatColHead)
  //перестраиваем ширину столбца таблицы (ЗАГОЛОВОК, ЗНАЧЕНИЯ И ИТОГИ)
     EditList->ChangeWidthCol(EditList->GetNSumm(),N,HeaderControlEditList,ActivSG);
  }}
//  ChangFormatCol=0; ChangFormatColHead=0;
 }
 pmFromMag->Visible=EditList->Mag(ACol)?true:false;
 EditList->Row=ARow; EditList->Col=ACol;
 EditList->Value=StringGridEditList->Cells[ACol][ARow];
}
//---------------------------------------------------------------------------
void __fastcall TFormEditList::StringGridEditListEnter(TObject *Sender){
/* bool CanSelect;*/
 if((EditList->Row0>-1)&&(EditList->Row0<StringGridEditList->RowCount)){
  StringGridEditList->Row=EditList->Row0;
//  EditList->Row=StringGridEditList->Row;
//  EditList->Col=StringGridEditList->Col;
//  StringGridEditListSelectCell(Sender,EditList->Col,EditList->Row0,&CanSelect);
 }
 EditList->Row=StringGridEditList->Row;
 EditList->Col=StringGridEditList->Col;
 EditList->Value=
  StringGridEditList->Cells[EditList->Col][EditList->Row];
}
//---------------------------------------------------------------------------
void  __fastcall TEditList::PrintFullHead(int Knfh,TPrinter *Prn,
        int lp,int H,int MarginLeft,int &Y){
 int L,X; wchar_t *h=FullHead,*n;
 if(!FullHead) return;
 Prn->Canvas->Font->Style=TFontStyles()<<fsBold;
 for(int i=0;i<Knfh;i++){
  if(n=wcschr(h,'\n')) *n='\0';
  L=Prn->Canvas->TextWidth(h);
  X=(lp-L)/2; if(X<0) X=0;
  Prn->Canvas->TextOut(X+MarginLeft,Y,h);
  Y+=H; if(n){ *n++='\n'; h=n; }
 }
 L=Prn->Canvas->TextWidth(h);
 X=(lp-L)/2; if(X<0) X=0;
 Prn->Canvas->TextOut(X+MarginLeft,Y,h);
 Y+=H;
 Prn->Canvas->Font->Style=TFontStyles()>>fsBold;
}
//---------------------------------------------------------------------------
void  __fastcall TEditList::PrintHead(int Knh,TPrinter *Prn,
        int lp,int H,int MarginLeft,int &Y){
 int L,X; wchar_t *h,*n; if(!Head) return; h=Head[0];
 Prn->Canvas->Font->Style=TFontStyles()<<fsBold;
 for(int i=0;i<Knh;i++){
  n=wcschr(h,'\n'); *n='\0';
  L=Prn->Canvas->TextWidth(h);
  X=(lp-L)/2; if(X<0) X=0;
  Prn->Canvas->TextOut(X+MarginLeft,Y,h);
  Y+=H; *n++='\n'; h=n;
 }
 L=Prn->Canvas->TextWidth(h);
 X=(lp-L)/2; if(X<0) X=0;
 Prn->Canvas->TextOut(X+MarginLeft,Y,h);
 Y+=H;
 Prn->Canvas->Font->Style=TFontStyles()>>fsBold;
}
//---------------------------------------------------------------------------
void  __fastcall TEditList::PrintPodval(int Knp,TPrinter *Prn,
        int H,int MarginLeft,int MarginTop,int Hp,int hp,int &Y){
 wchar_t *h=Podval,*n;
 Prn->Canvas->Font->Style=TFontStyles()<<fsBold;
 for(int i=0;i<Knp;i++){
  n=wcschr(h,'\n'); *n='\0';
  if((Y-MarginTop+Hp)>hp){ Prn->NewPage(); Y=MarginTop; }
  Prn->Canvas->TextOut(MarginLeft,Y,h);
  Y+=H; *n++='\n'; h=n;
 }
 Prn->Canvas->TextOut(MarginLeft,Y,h);
 Y+=H;
 Prn->Canvas->Font->Style=TFontStyles()>>fsBold;
}
//---------------------------------------------------------------------------
void  __fastcall TEditList::DrawPodval(int Knp,TCanvas *Canvas,int H,
  int MarginLeft,int MarginTop,int Hp,int hp,int &Y,double Kx,double Ky){
 wchar_t *h=Podval,*n;
 for(int i=0;i<Knp;i++){
  n=wcschr(h,'\n'); *n='\0';
  Canvas->TextOut(Round(Kx*MarginLeft),Round(Ky*Y),h);
  Y+=H; *n++='\n'; h=n;
 }
 Canvas->TextOut(Round(Kx*MarginLeft),Round(Ky*Y),h);
 Y+=H;
}
//---------------------------------------------------------------------------
void __fastcall TEditList::DefWidthSGCols(TPrinter *Prn,int *Li,
  int *LiDo,int *LiPo,double Ktw,Magazine *Ms){
 wchar_t Cell[256],*t; int l,lt;
// wchar_t TestStr[]=L"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
// lt=Round(Ktw*Prn->Canvas->TextWidth(TestStr));
 lt=Round(Ktw*Prn->Canvas->TextWidth(L"."));
 for(int i=0;i<ActivSG->RowCount;i++)
//  for(int j=0;j<ActivSG->ColCount;j++){
  for(Magazine *m=Ms;m;m=m->Sled){
   int j=_wtoi(m->S);
   wcscpy(Inf,ActivSG->Cells[j][i].c_str());
   if(!FormEditList->ButtonFullHead->Height||(j>=KolFixStlb))
    CleanStr(Inf,Cell);
   else wcscpy(Cell,Inf);
   if(wcschr(EMasForm[j],'f')&&(t=wcschr(Cell,'.'))){//есть плавающая точка
    *t++='\0';
    l=Round(Ktw*Prn->Canvas->TextWidth(Cell));
    if(l>LiDo[j]) LiDo[j]=l;
    l=Round(Ktw*Prn->Canvas->TextWidth(t));
    if(l>LiPo[j]) LiPo[j]=l;
    l=LiDo[j]+LiPo[j]+lt;
   }
   else//нет плавающей точки
    l=Round(Ktw*Prn->Canvas->TextWidth(Cell));
   if(l>Li[j]) Li[j]=l;
  }
 if(GetNSumm()&&GetNSumm(0)){//есть итоги
  Prn->Canvas->Font->Style=TFontStyles()<<fsBold;
//  for(int j=0;j<ActivSG->ColCount;j++){
  for(Magazine *m=Ms;m;m=m->Sled){
   int j=_wtoi(m->S);
   wcscpy(Inf,
    FormEditList->HeaderControlEditList->Sections->Items[j]->Text.c_str());
   if(!FormEditList->ButtonFullHead->Height||(j>=KolFixStlb))
    CleanStr(Inf,Cell);
   else wcscpy(Cell,Inf);
   if(wcschr(EMasForm[j],'f')&&(t=wcschr(Cell,'.'))){//есть плавающая точка
    *t++='\0';
    l=Round(Ktw*Prn->Canvas->TextWidth(Cell));
    if(l>LiDo[j]) LiDo[j]=l;
    l=Round(Ktw*Prn->Canvas->TextWidth(t));
    if(l>LiPo[j]) LiPo[j]=l;
    l=LiDo[j]+LiPo[j]+lt;
   }
   else//нет плавающей точки
    l=Round(Ktw*Prn->Canvas->TextWidth(Cell));
   if(l>Li[j]) Li[j]=l;
  }
  Prn->Canvas->Font->Style=TFontStyles()>>fsBold;
}}
//---------------------------------------------------------------------------
void __fastcall TEditList::DefWidthSGCols(TPrinter *Prn,int *Li,
  int *LiDo,int *LiPo,int LChar,Magazine *Ms){
 wchar_t Cell[256],*t; int l,lt;
 lt=Prn->Canvas->TextWidth(L".");
 for(int i=0;i<ActivSG->RowCount;i++)
//  for(int j=0;j<ActivSG->ColCount;j++){
  for(Magazine *m=Ms;m;m=m->Sled){ int j=_wtoi(m->S);
   wcscpy(Inf,ActivSG->Cells[j][i].c_str());
   if(!FormEditList->ButtonFullHead->Height||(j>=KolFixStlb))
    CleanStr(Inf,Cell);
   else wcscpy(Cell,Inf);
   if(wcschr(EMasForm[j],'f')&&(t=wcschr(Cell,'.'))){//есть плавающая точка
    *t++='\0';
    l=wcslen(Cell)*LChar;
    if(l>LiDo[j]) LiDo[j]=l;
    l=wcslen(t)*LChar;
    if(l>LiPo[j]) LiPo[j]=l;
    l=LiDo[j]+LiPo[j]+lt;
   }
   else//нет плавающей точки
    l=wcslen(Cell)*LChar;
   if(l>Li[j]) Li[j]=l;
  }
 if(GetNSumm()&&GetNSumm(0)){//есть итоги
//  for(int j=0;j<ActivSG->ColCount;j++){
  for(Magazine *m=Ms;m;m=m->Sled){ int j=_wtoi(m->S);
   wcscpy(Inf,
    FormEditList->HeaderControlEditList->Sections->Items[j]->Text.c_str());
   if(!FormEditList->ButtonFullHead->Height||(j>=KolFixStlb))
    CleanStr(Inf,Cell);
   else wcscpy(Cell,Inf);
   if(wcschr(EMasForm[j],'f')&&(t=wcschr(Cell,'.'))){//есть плавающая точка
    *t++='\0';
    l=wcslen(Cell)*LChar;
    if(l>LiDo[j]) LiDo[j]=l;
    l=wcslen(t)*LChar;
    if(l>LiPo[j]) LiPo[j]=l;
    l=LiDo[j]+LiPo[j]+lt;
   }
   else//нет плавающей точки
    l=wcslen(Cell)*LChar;
   if(l>Li[j]) Li[j]=l;
}}}
//---------------------------------------------------------------------------
void __fastcall TEditList::DefL(TPrinter *Prn,int L,int &Lfs,
  int *Li,int *LiDo,int *LiPo,int &Lt,double Ktw,Magazine *Ms){
 int i;
 Shapk->DefWidthButtonPrn(Prn->Canvas,Ktw,Ms);
 Shapk->DefWidthButtonColsPrn(Li,Ms);
 DefWidthSGCols(Prn,Li,LiDo,LiPo,Ktw,Ms);
// for(i=0;i<KolFixStlb;i++) Lfs+=Li[i]+L;
// for(;i<EKForm;i++) Lt+=Li[i]+L;
 for(;Ms;Ms=Ms->Sled){
  i=_wtoi(Ms->S); if(i<KolFixStlb) Lfs+=Li[i]+L; else Lt+=Li[i]+L;
 }
 if(Lfs) Lfs+=L; if(Lt) Lt+=L;
}
//---------------------------------------------------------------------------
void __fastcall TFormEditList::PrintHeaderControl(TPrinter *Prn,int L,
          int H,int MarginLeft,int &Y,int *Li,int NN,int NK,double Ktw){
 int j,X=MarginLeft,LTab=0,dX;
 for(j=NN;j<=NK;j++) LTab+=Li[j]+L;
 Prn->Canvas->Font->Style=TFontStyles()<<fsBold;
 if(EditList->GetNSumm()&&EditList->GetNSumm(0)) {
  Prn->Canvas->MoveTo(X,Y); Prn->Canvas->LineTo(X,Y+H+H/5);//|
  for(j=NN;j<=NK;j++){
   CleanStr(HeaderControlEditList->Sections->Items[j]->Text.c_str(),Inf);
   if(wcschr(EditList->GetEMasForm()[j],'-'))
    Prn->Canvas->TextOut(X+L/2,Y+H/5,Inf);
   else{
    dX=Li[j]-Round(Ktw*Prn->Canvas->TextWidth(Inf));
    Prn->Canvas->TextOut(X+dX+L/2,Y+H/5,Inf);
   }
   X+=Li[j]+L;
   Prn->Canvas->MoveTo(X,Y); Prn->Canvas->LineTo(X,Y+H+2*H/5);//|
  }
  Y+=H+H/5+2;
  X=MarginLeft;
  Prn->Canvas->MoveTo(X,Y+H/5); Prn->Canvas->LineTo(X+LTab,Y+H/5);//--------
 }
 Prn->Canvas->Font->Style=TFontStyles()>>fsBold;
}
//---------------------------------------------------------------------------
void __fastcall TFormEditList::DrawHeaderControl(TCanvas *Canvas,int L,int H,
  int MarginLeft,int &Y,int *Li,int NN,int NK,double Kx,double Ky,Magazine *Ms){
 int j,X=MarginLeft,LTab=0,dX; Magazine *m;
 for(j=NN;j<=NK;j++){
  swprintf(Inf,L"%d",j); if(FindMagazine(Ms,Inf,&m)) LTab+=Li[j]+L;
 }
 Canvas->MoveTo(Round(Kx*X),Round(Ky*Y));
 if(EditList->GetNSumm()&&EditList->GetNSumm(0)) {
  Canvas->MoveTo(Round(Kx*X),Round(Ky*Y));
  Canvas->LineTo(Round(Kx*X),Round(Ky*(Y+2*H/5+H)));//|
  for(j=NN;j<=NK;j++){
   swprintf(Inf,L"%d",j); if(!FindMagazine(Ms,Inf,&m)) continue;
   if(!ButtonFullHead->Height||(j>=EditList->GetKolFixStlb()))
    CleanStr(HeaderControlEditList->Sections->Items[j]->Text.c_str(),Inf);
   else wcscpy(Inf,HeaderControlEditList->Sections->Items[j]->Text.c_str());
   if(wcschr(EditList->GetEMasForm()[j],'-'))
    Canvas->TextOut(Round(Kx*(X+L/2)),Round(Ky*(Y+H/5)),Inf);
   else{
    dX=Li[j]-Canvas->TextWidth(Inf)/Kx;
    Canvas->TextOut(Round(Kx*(X+dX+L/2)),Round(Ky*(Y+H/5)),Inf);
   }
   X+=Li[j]+L;
   Canvas->MoveTo(Round(Kx*X),Round(Ky*Y));
   Canvas->LineTo(Round(Kx*X),Round(Ky*(Y+2*H/5+H)));//|
  }
  Y+=H+H/5;
  X=MarginLeft;
  Canvas->MoveTo(Round(Kx*X),Round(Ky*(Y+H/5)));
  Canvas->LineTo(Round(Kx*(X+LTab)),Round(Ky*(Y+H/5)));//----------
}}
//---------------------------------------------------------------------------
void __fastcall TFormEditList::PrintStringGrid(TPrinter *Prn,int L,
  int H,int MarginLeft,int &Y,int *Li,int NN,int NK,int SN,int SK,
  double Ktw){
 int i,j,X,dX,LTab=0;
 for(i=SN;i<min(SK+1,StringGridEditList->RowCount);i++){
  X=MarginLeft;
  Prn->Canvas->MoveTo(X,Y); Prn->Canvas->LineTo(X,Y+H+H/7);//|
  for(j=NN;j<=NK;j++){
//   Prn->Canvas->TextOut(X+L/2,Y,StringGridEditList->Cells[j][i]);
   if(!ButtonFullHead->Height||(j>=EditList->GetKolFixStlb()))
    CleanStr(StringGridEditList->Cells[j][i].c_str(),Inf);
   else wcscpy(Inf,StringGridEditList->Cells[j][i].c_str());
   if(wcschr(EditList->GetEMasForm()[j],'-'))
    Prn->Canvas->TextOut(X+L/2,Y,Inf);
   else{
    dX=Li[j]-Round(Ktw*Prn->Canvas->TextWidth(Inf));
    Prn->Canvas->TextOut(X+dX+L/2,Y,Inf);
   }
   if(FormParamPrint->Podch){
    Prn->Canvas->MoveTo(X,Y+H-3);
    Prn->Canvas->LineTo(X+Li[j]+L,Y+H-3);
   }
   X+=Li[j]+L;
   Prn->Canvas->MoveTo(X,Y); Prn->Canvas->LineTo(X,Y+H+H/7);//|
  }
  Y+=H;
 }
 for(j=NN;j<=NK;j++) LTab+=Li[j]+L; X=MarginLeft;
 Prn->Canvas->MoveTo(X,Y+H/7); Prn->Canvas->LineTo(X+LTab,Y+H/7);//----------
}
//---------------------------------------------------------------------------
void __fastcall TFormEditList::DrawStringGrid(TCanvas *Canvas,int L,
  int H,int MarginLeft,int &Y,int *Li,int NN,int NK,int SN,int SK,
  double Kx,double Ky,Magazine *Ms){ Magazine *m;
 int i,j,X,dX,LTab=0;
 for(i=SN;i<min(SK+1,StringGridEditList->RowCount);i++){
  X=MarginLeft;
  Canvas->MoveTo(Round(Kx*X),Round(Ky*Y));
  Canvas->LineTo(Round(Kx*X),Round(Ky*(Y+H+H/5)));//|
  for(j=NN;j<=NK;j++){
   swprintf(Inf,L"%d",j); if(!FindMagazine(Ms,Inf,&m)) continue;
   if(!ButtonFullHead->Height||(j>=EditList->GetKolFixStlb()))
    CleanStr(StringGridEditList->Cells[j][i].c_str(),Inf);
   else wcscpy(Inf,StringGridEditList->Cells[j][i].c_str());
//   CleanStr(StringGridEditList->Cells[j][i].c_str(),Inf);
   if(wcschr(EditList->GetEMasForm()[j],'-'))
    Canvas->TextOut(Round(Kx*(X+L/2)),Round(Ky*Y),Inf);
   else{
//    dX=Li[j]-wcslen(Inf)*L;
    dX=Li[j]-Canvas->TextWidth(Inf)/Kx;
    Canvas->TextOut(Round(Kx*(X+dX+L/2)),Round(Ky*Y),Inf);
   }
   if(FormParamPrint->Podch){
    Canvas->MoveTo(Round(Kx*X),Round(Ky*(Y+H-3)));
    Canvas->LineTo(Round(Kx*(X+Li[j]+L)),Round(Ky*(Y+H-3)));
   }
   X+=Li[j]+L;
   Canvas->MoveTo(Round(Kx*X),Round(Ky*Y));
   Canvas->LineTo(Round(Kx*X),Round(Ky*(Y+H+H/5)));//|
  }
  Y+=H;
 }
 for(j=NN;j<=NK;j++){
  swprintf(Inf,L"%d",j); if(FindMagazine(Ms,Inf,&m)) LTab+=Li[j]+L;
 }
 X=MarginLeft;
 Canvas->MoveTo(Kx*X,Ky*(Y+H/10)); Canvas->LineTo(Kx*(X+LTab),Ky*(Y+H/10));//----------
}
//---------------------------------------------------------------------------
void __fastcall TEditList::PrintOnePage(int Knfh,int Knh,
  TPrinter *Prn,int lp,int L,int H,int MarginLeft,int MarginTop,
  int Hp,int hp,int &Y,int *Li,int Knp,double Ktw){
 int NN=0,NK=FormEditList->StringGridEditList->ColCount-1,LTab=L;
 PrintFullHead(Knfh,Prn,lp,H,MarginLeft,Y);
 PrintHead(Knh,Prn,lp,H,MarginLeft,Y);
 Y+=H;
 for(int i=0;i<NK+1;i++) LTab+=Li[i]+L;
 for(int i=0;i<NK+1;i++) Li[i]+=(lp-LTab)/(NK+1);
 Shapk->PrintShapka(Prn,L,H,MarginLeft,Y,Li,NN,NK);
 Y+=H/5;
 FormEditList->PrintStringGrid(Prn,L,H,MarginLeft,Y,Li,NN,NK,0,
  FormEditList->StringGridEditList->RowCount-1,Ktw);
 FormEditList->PrintHeaderControl(Prn,L,H,MarginLeft,Y,Li,NN,NK,Ktw);
 Y+=H;
 if(Podval&&Podval[0]) PrintPodval(Knp,Prn,H,MarginLeft,MarginTop,Hp,hp,Y);
}
//---------------------------------------------------------------------------
void __fastcall TEditList::PrintManyPage(int Knfh,int Knh,
   TPrinter *Prn,int lp,int L,int H,int MarginLeft,int MarginTop,
   int Hp,int hp,int &Y,int *Li,int Knp,int Ks,int Hfh,int Hh,
   int HSh,double Ktw){
 int i,NN=0,NK=FormEditList->StringGridEditList->ColCount-1,LTab=L,
  Kss,Kp,Kps;
 PrintFullHead(Knfh,Prn,lp,H,MarginLeft,Y);
 PrintHead(Knh,Prn,lp,H,MarginLeft,Y);
 Y+=H;
 for(i=0;i<NK+1;i++) LTab+=Li[i]+L;
 for(i=0;i<NK+1;i++) Li[i]+=(lp-LTab)/(NK+1);
 Shapk->PrintShapka(Prn,L,H,MarginLeft,Y,Li,NN,NK); Y+=H/5;
 Kps=(hp-Hfh-Hh-H-HSh)/H;
 FormEditList->PrintStringGrid(Prn,L,H,MarginLeft,Y,Li,NN,NK,0,
  Kps-1,Ktw);
//найдем число оставшихся полных страниц
 Kss=(hp-Hh-H-HSh)/H;
 Kp=(FormEditList->StringGridEditList->RowCount-Kps)/Kss;
 for(i=0;i<Kp;i++){
  Prn->NewPage(); Y=MarginTop;
  PrintHead(Knh,Prn,lp,H,MarginLeft,Y);
  swprintf(Inf,L"Продолжение. Стр.%d",i+2);
  Prn->Canvas->TextOut(MarginLeft,Y,Inf);
  Y+=H;
  Shapk->PrintShapka(Prn,L,H,MarginLeft,Y,Li,NN,NK); Y+=H/5;
  FormEditList->PrintStringGrid(Prn,L,H,MarginLeft,Y,Li,NN,NK,Kps,
   Kps+Kss-1,Ktw);
  Kps+=Kss;
 }
 Prn->NewPage(); Y=MarginTop;
 if(FormEditList->StringGridEditList->RowCount>Kps){
  PrintHead(Knh,Prn,lp,H,MarginLeft,Y);
  swprintf(Inf,L"Продолжение. Стр.%d",i+2);
  Prn->Canvas->TextOut(MarginLeft,Y,Inf);
  Y+=H;
  Shapk->PrintShapka(Prn,L,H,MarginLeft,Y,Li,NN,NK); Y+=H/5;
  FormEditList->PrintStringGrid(Prn,L,H,MarginLeft,Y,Li,NN,NK,Kps,
   Kps+Kss-1,Ktw);
 }
 FormEditList->PrintHeaderControl(Prn,L,H,MarginLeft,Y,Li,NN,NK,Ktw);
 Y+=H;
 if(Podval&&Podval[0]) PrintPodval(Knp,Prn,H,MarginLeft,MarginTop,Hp,hp,Y);
}
//---------------------------------------------------------------------------
int __fastcall TEditList::PrintBroadTabl(int Knfh,int Knh,TPrinter *Prn,
   int lp,int L,int H,int MarginLeft,int MarginTop,int Hp,int hp,int &Y,
   int *Li,int Knp,int Ks,int Hfh,int Hh,int HSh,int Kfs,int Lfs,int Ht,
   int Nsn,int Nsk,int HIt,int NPage,double Ktw){
 int i,j,NN,NK,Kt1,Kp,Kt2,Ktn,KP,nn[100],nk[100],Dlp,Yn,Xsh[100],NPg=0;
//Kt1,Kt2,Ktn,Kp,KP - соотв. число порций таблицы на первой,
//средней и последней страницах; общее число порций и общее
//число страниц печати
//Определим общее число порций Kp и заполним массивы номеров
//начальных и конечных столбцов порций
 Dlp=Lfs;//длина порции: неподвижные + переменные
 Kp=0; nn[0]=Kfs; Xsh[0]=0;
 for(i=Kfs;i<FormEditList->StringGridEditList->ColCount;i++){
  if(Kfs){
   if((Lfs+Li[i]+L)>lp) {
    swprintf(Inf,L"Ширина %d-го столбца и непрокручиваем%s перв%s столбц%s\n"
                  L"превосходит зону печати! ",i+1,Kfs>1?"ых":"ого",
                  Kfs>1?"ых":"ого",Kfs>1?"ов":"а");
    Application->MessageBox(Inf,L"Печать невозможна!",MB_OK|MB_ICONWARNING);
    return NPg;
  }}
  else{
   if((Li[i]+2*L)>lp) {
    swprintf(Inf,L"Ширина %d-го столбца превосходит зону печати! ",i+1);
    Application->MessageBox(Inf,L"Печать невозможна!",MB_OK|MB_ICONWARNING);
    return NPg;
  }}
  if((Dlp+Li[i]+L)>lp){
   nk[Kp]=i-1; Kp++; Xsh[Kp]=Xsh[Kp-1]+Dlp-Lfs; nn[Kp]=i; Dlp=Lfs;
  }
  Dlp+=Li[i]+L;
 }
 nk[Kp]=i-1; Kp++; Xsh[Kp]=Xsh[Kp-1]+Dlp-Lfs;
 Kt1=(hp-Hfh-Hp-Hh-H)/(HSh+Ht+HIt+H+2*(H/5));
 Kt2=(hp-H-Hh)/(HSh+Ht+HIt+H+2*(H/5));
 Ktn=(hp-Hp-H-Hh)/(HSh+Ht+HIt+H+2*(H/5));
 if(Kt1>=Kp) KP=1;
 else {
  Kt1=(hp-Hfh-Hh)/(HSh+Ht+H+2*(H/5));
  if((Kt1+Ktn)>=Kp) KP=2;
  else {
   KP=2+(Kp-Kt1-Ktn)/Kt2; if((Kp-Kt1-Ktn)%Kt2) KP++;
 }}
//Первый лист
 if(Kt1){
  PrintFullHead(Knfh,Prn,lp,H,MarginLeft,Y);
  PrintHead(Knh,Prn,lp,H,MarginLeft,Y);
  Y+=H; NPg++;
 }
 for(i=0;i<min(Kt1,Kp);i++){
  NN=nn[i]; NK=nk[i]; Yn=Y;
  Shapk->PrintShapka(Prn,L,H,MarginLeft,Y,Li,0,Kfs-1);
  Y+=H/5;
  FormEditList->PrintStringGrid(Prn,L,H,MarginLeft,Y,Li,0,Kfs-1,
   Nsn,Nsk,Ktw);
  if(HIt)
   FormEditList->PrintHeaderControl(Prn,L,H,MarginLeft,Y,Li,0,Kfs-1,Ktw);
  Y=Yn;
  Shapk->PrintShapka(Prn,L,H,MarginLeft-Xsh[i],Y,Li,NN,NK);
  Y+=H/5;
  FormEditList->PrintStringGrid(Prn,L,H,MarginLeft+Lfs-L,Y,Li,NN,
   NK,Nsn,Nsk,Ktw);
  if(HIt)
   FormEditList->PrintHeaderControl(Prn,L,H,MarginLeft+Lfs-L,Y,Li,NN,NK,Ktw);
  Y+=H;
 }
 if(Kt1&&KP>2) Prn->NewPage();
//печать средних страниц
 for(i=0;i<KP-2;i++){
  if(i) Prn->NewPage(); Y=MarginTop; NPg++;
  PrintHead(Knh,Prn,lp,H,MarginLeft,Y);
  swprintf(Inf,L"Продолжение. Стр.%d",NPage+i+2);
  Prn->Canvas->TextOut(MarginLeft,Y,Inf);
  Y+=H;
  for(int j=0;j<Kt2;j++){
   NN=nn[j+i*Kt2+Kt1]; NK=nk[j+i*Kt2+Kt1]; Yn=Y;
   Shapk->PrintShapka(Prn,L,H,MarginLeft,Y,Li,0,Kfs-1);
   Y+=H/5;
   FormEditList->PrintStringGrid(Prn,L,H,MarginLeft,Y,Li,0,Kfs-1,
    Nsn,Nsk,Ktw);
   if(HIt)
    FormEditList->PrintHeaderControl(Prn,L,H,MarginLeft,Y,Li,0,Kfs-1,Ktw);
   Y=Yn;
   Shapk->PrintShapka(Prn,L,H,MarginLeft-Xsh[j+i*Kt2+Kt1],Y,Li,NN,NK);
   Y+=H/5;
   FormEditList->PrintStringGrid(Prn,L,H,MarginLeft+Lfs-L,Y,Li,NN,
    NK,Nsn,Nsk,Ktw);
   if(HIt)
    FormEditList->PrintHeaderControl(Prn,L,H,MarginLeft+Lfs-L,Y,Li,NN,NK,Ktw);
   Y+=H;
 }}
 if(KP>1){
  Prn->NewPage(); Y=MarginTop; NPg++;
  PrintHead(Knh,Prn,lp,H,MarginLeft,Y);
  swprintf(Inf,L"Продолжение. Стр.%d",NPage+i+2);
  Prn->Canvas->TextOut(MarginLeft,Y,Inf);
  Y+=H;
  for(int j=0;j<min(Ktn,Kp-Kt1-(KP-2)*Kt2);j++){
   NN=nn[j+(KP-2)*Kt2+Kt1]; NK=nk[j+(KP-2)*Kt2+Kt1]; Yn=Y;
   Shapk->PrintShapka(Prn,L,H,MarginLeft,Y,Li,0,Kfs-1);
   Y+=H/5;
   FormEditList->PrintStringGrid(Prn,L,H,MarginLeft,Y,Li,0,Kfs-1,
    Nsn,Nsk,Ktw);
   if(HIt)
    FormEditList->PrintHeaderControl(Prn,L,H,MarginLeft,Y,Li,0,Kfs-1,Ktw);
   Y=Yn;
   Shapk->PrintShapka(Prn,L,H,MarginLeft-Xsh[j+(KP-2)*Kt2+Kt1],Y,Li,NN,NK);
   Y+=H/5;
   FormEditList->PrintStringGrid(Prn,L,H,MarginLeft+Lfs-L,Y,Li,NN,
    NK,Nsn,Nsk,Ktw);
   if(HIt)
    FormEditList->PrintHeaderControl(Prn,L,H,MarginLeft+Lfs-L,Y,Li,NN,NK,Ktw);
   Y+=H;
 }}
 Y+=H;
 if(Hp&&Podval&&Podval[0])
  PrintPodval(Knp,Prn,H,MarginLeft,MarginTop,Hp,hp,Y);
 return NPg;
}
//---------------------------------------------------------------------------
void __fastcall TEditList::PrintHugeTabl(int Knfh,int Knh,TPrinter *Prn,
   int lp,int L,int H,int MarginLeft,int MarginTop,int Hp,int hp,int &Y,
   int *Li,int Knp,int Ks,int Hfh,int Hh,int HSh,int Kfs,int Lfs,int Ht,
   double Ktw){
 int i,Ks1,Ks2,Ksn,Kr,KSh,NPg;
//Ks1,Ks2,Ksn,Kr - соотв. число строк Stringgrid на первой,
//средней и последней страницах; общее число одностраничных
//широких полос
//Определим общее число одностраничных широких полос Kr
 KSh=HSh/H; if(HSh%H) KSh++;
 Ks1=Ks-(Knfh+1)-(Knh+1)-KSh-3;
 Ks2=Ks-3-(Knh+1)-KSh;
 Ksn=(FormEditList->StringGridEditList->RowCount-Ks1)%Ks2;
 Kr=2+(FormEditList->StringGridEditList->RowCount-Ks1-Ksn)/Ks2;
//Первая полоса
 Ht=H*Ks1;
 NPg=PrintBroadTabl(Knfh,Knh,Prn,lp,L,H,MarginLeft,MarginTop,0,hp,Y,
   Li,Knp,Ks,Hfh,Hh,HSh,Kfs,Lfs,Ht,0,Ks1-1,0,0,Ktw);
//печать средних полос
 Ht=H*Ks2;
 for(i=0;i<Kr-2;i++){
  Prn->NewPage(); Y=MarginTop;
  NPg=PrintBroadTabl(0,Knh,Prn,lp,L,H,MarginLeft,MarginTop,0,hp,Y,
   Li,Knp,Ks,Hfh,Hh,HSh,Kfs,Lfs,Ht,Ks1+i*Ks2,Ks1+(i+1)*Ks2-1,0,
   NPg,Ktw);
 }
//Печать последней полосы с подвалом
 Prn->NewPage(); Y=MarginTop;
 Ht=H*Ksn;
 PrintBroadTabl(0,Knh,Prn,lp,L,H,MarginLeft,MarginTop,Hp,hp,Y,
  Li,Knp,Ks,Hfh,Hh,HSh,Kfs,Lfs,Ht,Ks1+i*Ks2,
  FormEditList->StringGridEditList->RowCount-1,H,NPg,Ktw);
}
//---------------------------------------------------------------------------
void __fastcall TFormEditList::ToolButtonPrintClick(TObject *Sender){
//Вызов печати без предварительных настроек
 TPrinter *Prn = Printer();
 if(!Prn||!Prn->Printers->Count){
  Application->MessageBox(L"В системе нет принтеров!\n"
   L"Поэтому печать невозможна!",L"ОШИБКА!",MB_OK|MB_ICONERROR);
  return;
 }
 int MarginLeft,MarginRight,MarginTop,MarginBot,//отступы в пикселях
  LPx,LPy,X,Y;//разрешение принтера по горизонтали и вертикали
//Высоты в пикселях заголовка, шапки, таблицы, подвала,
//области печати на стр., строки:
 int Hh=0,Hfh=0,HSh,Ht,Hp=0,hp,H/*=0*/;
//число выбранных для печати столбцов,
//ширины фикс.части, столбцов, мантиссы, дробной части
//прокручиваемой части таблицы, области печати страницы, символа:
 int ColCount,Lfs=0,*Li,*LiDo,*LiPo,Lt=0,lp,L;
//колич. строк в области печати (на странице),
//фиксируемых столбцов,общее число страниц,число полных строк
//в заголовке и в подвале:
 int Ks,Kfs,Kp,Knfh=0,Knh=0,Knp=0,PhW,PhH,Wmm,Hmm,PhL,PhT,PhWmm,PhHmm,
  PhTmm,PhBmm,PhLmm,PhRmm,PhB,PhR;
 Magazine *MStlb=NULL;
 ColCount=0; DelAllMagazine(&MStlb);
 for(TKnopka *K=EditList->Shapk->GetR();K;K=K->Sled){
  if(K->GetB()->Down){
   ColCount++; swprintf(Inf,L"%d",K->GetNn()); TakeMagazine(&MStlb,Inf);
 }}
 if(!ColCount){
  ColCount=FormEditList->StringGridEditList->ColCount;
  for(int i=0;i<ColCount;i++){ swprintf(Inf,L"%d",i); TakeMagazine(&MStlb,Inf); }
 }
 //прочитаем опции печати
 FormParamPrint->FilePrn=EditList->FilePrn;
 FormParamPrint->ReadOptPrn();
 if(FormParamPrint->Prn) Prn=FormParamPrint->Prn;
 Prn->Canvas->Font->Name=FormParamPrint->FontName;
 Prn->Canvas->Font->Size=FormParamPrint->FontSize;
 Prn->Canvas->Font->Color=FormParamPrint->FontColor;
 Prn->Orientation=FormParamPrint->OrientKnig?poPortrait:poLandscape;
 Prn->BeginDoc(); // для отладки
 LPx=GetDeviceCaps(Prn->Handle,LOGPIXELSX);
 LPy=GetDeviceCaps(Prn->Handle,LOGPIXELSY);
 PhW=GetDeviceCaps(Prn->Handle,PHYSICALWIDTH);
 PhH=GetDeviceCaps(Prn->Handle,PHYSICALHEIGHT);
 Wmm=GetDeviceCaps(Prn->Handle,HORZSIZE);
 Hmm=GetDeviceCaps(Prn->Handle,VERTSIZE);
 PhL=GetDeviceCaps(Prn->Handle,PHYSICALOFFSETX);
 PhT=GetDeviceCaps(Prn->Handle,PHYSICALOFFSETY);
 PhWmm=Round(PhW*25.4/LPx);
 PhHmm=Round(PhH*25.4/LPy);
 PhTmm=Round(PhT*25.4/LPy);
 PhBmm=PhHmm-PhTmm-Hmm;
 PhLmm=Round(PhL*25.4/LPx);
 PhRmm=PhWmm-PhLmm-Wmm;
 PhB=Round((double)PhBmm*LPy/25.4);
 PhR=Round((double)PhRmm*LPx/25.4);
 MarginLeft =Round((double)(FormParamPrint->MarginLeftmm*LPx)/25.4);
 MarginRight=Round((double)(FormParamPrint->MarginRightmm*LPy)/25.4);
 MarginTop  =Round((double)(FormParamPrint->MarginTopmm*LPy)/25.4);
 MarginBot  =Round((double)(FormParamPrint->MarginBotmm*LPy)/25.4);
 Y=MarginTop;
// H=Prn->Canvas->TextHeight(L"M"); L=Prn->Canvas->TextWidth(L"M");
 Kfs=EditList->GetKolFixStlb();
 H=Round(FormParamPrint->Interval*LPy/6);
 lp=Round(Prn->PageWidth*0.9827)-MarginLeft-MarginRight+PhL+PhR;
 hp=Prn->PageHeight-MarginTop-MarginBot+PhT+PhB;
 L=Prn->Canvas->Font->Size*LPx/120;
//Посчитаем метрику таблицы
// lp=Prn->PageWidth-MarginLeft-MarginRight;
// hp=Prn->PageHeight-MarginTop-MarginBot;
 Ks=hp/H;
 HSh=EditList->Shapk->SetHeightShapkaPrn(H);
 Ht=StringGridEditList->RowCount*H;
 if(EditList->Head&&EditList->Head[0]){
  Knh=KSimbStr(EditList->Head[0],'\n'); Hh=(Knh+1)*H;
 }
 if(EditList->FullHead&&EditList->FullHead[0]){
  Knfh=KSimbStr(EditList->FullHead,'\n'); Hfh=(Knfh+1)*H;
 }
 if(EditList->Podval&&EditList->Podval[0]){
  Knp=KSimbStr(EditList->Podval,'\n'); Hp=(Knp+1)*H;
 }
 Li=(int *)calloc(StringGridEditList->ColCount,SzI);
 LiDo=(int *)calloc(StringGridEditList->ColCount,SzI);
 LiPo=(int *)calloc(StringGridEditList->ColCount,SzI);
 EditList->DefL(Prn,L,Lfs,Li,LiDo,LiPo,Lt,1,MStlb);
//Собственно печать
 if(lp>=(Lfs+Lt)) {//Таблица узкая
 //Если одна страница
  if(hp>=(Hh+Hfh+HSh+Ht+Hp+2*H))
   EditList->PrintOnePage(Knfh,Knh,Prn,lp,L,H,MarginLeft,
    MarginTop,Hp,hp,Y,Li,Knp,1);
  else {//Если более одной, но по ширине помещается на стр.
   EditList->PrintManyPage(Knfh,Knh,Prn,lp,L,H,MarginLeft,
    MarginTop,Hp,hp,Y,Li,Knp,Ks,Hfh,Hh,HSh,1);
 }}
 else {//Таблица широкая
  if(Lfs>lp) {
   swprintf(Inf,L"Ширина непрокручиваем%s перв%s столбц%s\n"
                 L"превосходит зону печати! ",Kfs>1?"ых":"ого",
                 Kfs>1?"ых":"ого",Kfs>1?"ов":"а");
   Application->MessageBox(Inf,L"Печать невозможна!",MB_OK|MB_ICONWARNING);
  }
  if(hp>=(Hh+Hfh+HSh+Ht+Hp+2*H)) //По высоте одна стр.
   EditList->PrintBroadTabl(Knfh,Knh,Prn,lp,L,H,MarginLeft,
    MarginTop,Hp,hp,Y,Li,Knp,Ks,Hfh,Hh,HSh,Kfs,Lfs,Ht,0,
    FormEditList->StringGridEditList->RowCount-1,H,0,1);
  else // в одну страницу не помещается и широкая(огромная таблица)
   EditList->PrintHugeTabl(Knfh,Knh,Prn,lp,L,H,MarginLeft,
    MarginTop,Hp,hp,Y,Li,Knp,Ks,Hfh,Hh,HSh,Kfs,Lfs,Ht,1);
 }
 if(Li) free(Li); if(LiDo) free(LiDo); if(LiPo) free(LiPo);
 Prn->EndDoc();
}
//---------------------------------------------------------------------------
String Del_r(String S){
 wchar_t Buff[2048],*s,*b=Buff;
 for(s=S.w_str();*s;s++)
  if(*s!='\r'){
   if(*s=='\t') for(int i=0;i<8;i++) *b++=' ';
   else *b++=*s;
  } 
 *b='\0';
 S=Buff;
 return  S;
}
//---------------------------------------------------------------------------
void __fastcall TFormEditList::ButtonFullHeadClick(TObject *Sender){
//Возможность редактирования надписи кнопки
 int K,K0=KSimbStr(ButtonFullHead->Caption.c_str(),'\n')+2;
 EditTextButton=new TEditTextButton;
 EditTextButton->Text=ButtonFullHead->Caption;
 Application->CreateForm(__classid(TFormEditTextButton), &FormEditTextButton);
 FormEditTextButton->ShowModal();
 if(FormEditTextButton->RichEditText->Modified) {
  ButtonFullHead->Caption=Del_r(FormEditTextButton->RichEditText->Text);
  wcscpy(EditList->FullHead,ButtonFullHead->Caption.c_str());
  K=KSimbStr(ButtonFullHead->Caption.c_str(),'\n')+2;
  ButtonFullHead->Height=-12*K*ButtonFullHead->Font->Height/10;
  if(K!=K0) TabControlEditListChange(Sender);// FormShow(Sender);
 }
 delete EditTextButton;
}
//---------------------------------------------------------------------------
void __fastcall TFormEditList::ToolButtonSaveClick(TObject *Sender){
 if(EditList->Save){
  EditList->StringGridToRout(StringGridEditList);
  EditList->Save();
}}
//---------------------------------------------------------------------------
void __fastcall TFormEditList::ButtonPodvalClick(TObject *Sender){
//Возможность редактирования надписи в подвале
 int Pp=0;
 if(!wcscmp(L"Подвал",ButtonPodval->Caption.c_str()))
  Pp=1;//подвал многострочный
 EditTextButton=new TEditTextButton;
 if(!Pp) EditTextButton->Text=ButtonPodval->Caption;
 else EditTextButton->Text=EditList->Podval;
 Application->CreateForm(__classid(TFormEditTextButton), &FormEditTextButton);
 FormEditTextButton->ShowModal();
 if(FormEditTextButton->RichEditText->Modified){
  if(Pp) ButtonPodval->Caption=L"Подвал";
  else ButtonPodval->Caption=FormEditTextButton->RichEditText->Text;
  Del_r(FormEditTextButton->RichEditText->Text);
  wcscpy(EditList->Podval,FormEditTextButton->RichEditText->Text.c_str());
 }
 delete EditTextButton;
}
//---------------------------------------------------------------------------
void __fastcall TEditList::SetKukla(){
 Kukla=*Root=calloc(1,SizeR);
}
//---------------------------------------------------------------------------
void __fastcall TFormEditList::ToolButtonHelpClick(TObject *Sender){
 wchar_t *s,Buf[256]; AnsiString FileHelp;
 wcscpy(Buf,EditList->FileHelp);
 if(s=wcsrchr(Buf,'\\')) wcscpy(s+1,L"ViewTabl.htm");
 else wcscpy(Buf,L"ViewTabl.htm");
 if(!_waccess(EditList->FileHelp,0)){ FileHelp=EditList->FileHelp;
  if(!FormOtchet)
   ShellExecute(NULL,"open",FileHelp.c_str(),NULL,NULL,SW_SHOW);
  else{
   FormOtchet->cbURL->Text=EditList->FileHelp; FormOtchet->ShowModal();
  }
  return;
 }
 else{
  if(!_waccess(Buf,0)){ FileHelp=Buf;
   if(!FormOtchet)
	ShellExecute(NULL,"open",FileHelp.c_str(),NULL,NULL,SW_SHOW);
   else{
	FormOtchet->cbURL->Text=Buf; FormOtchet->ShowModal();
   }
   return;
 }}
 swprintf(Inf,L"Не найдено ни файла\n<%s>\nни файла\n<%s>",
  EditList->FileHelp,Buf);
 Application->MessageBox(Inf,L"Просмотр справки невозможен!",
  MB_OK|MB_ICONWARNING);
}
//---------------------------------------------------------------------------
TEditList::TEditList(void **R,int *MNN,bool Pch,int *NS,bool Sh0,
  Magazine ***M,wchar_t *F,wchar_t *FSh,wchar_t **H,wchar_t *Fprn,
  void (*SaveR)(void),bool(*Test)(TStringGrid *,int,int,wchar_t *),
  void(**Proc)(void **),int KFStlb,int lf,int tf,int wf,int hf,
  wchar_t *FH,wchar_t *Pd,int Vert,wchar_t *FileH,bool *AddM,void(*CalCel)(int,int)){
 wchar_t *f; EMasForm=NULL; Shapk=NULL; Kukla=NULL; CallCell=CalCel;
 LeftF=lf; TopF=tf; WidthF=wf; HeightF=hf; AddMM=AddM;
 Root=R; MasNN=MNN; Podch=Pch; NSumm=NS; Show0=Sh0; Row0=0;
 MM=M;
 FullFormat=F;
 FileSh=FSh;
 Head=H;
 FilePrn=Fprn;
 Save=SaveR;
 TestField=Test;
 KolFixStlb=KFStlb;
 Kp=KSimbStr(FullFormat,'p');
 ProcList=Proc;
 FullHead=FH;
 Podval=Pd;
 if(FileH)
  wcscpy(FileHelp,FileH);
 Col=Row=0;/*Row0=-1*/;
 VERT=Vert;
 Select=false;
 SelectMad=false;
//считаем длину единицы структуры
 for(f=FullFormat,SizeR=SzV;*f;f++){
  switch(*f){
   case 'c': SizeR+=2*SzC; break;
   case 'd': SizeR+=SzI; break;
   case 'l': SizeR+=SzL; break;
   case 's': case 'm': SizeR+=SzV; break;
   case 'f': SizeR+=SzD; break;
   case 'e': SizeR+=SzD; break;
   case 'g': SizeR+=SzD; break;
   case 'p': SizeR+=SzV; break;
   case 'K': case 'U': SizeR+=SzV; break;
//   case 'E': case 'F': case 'G': N+=SzF; break;
   case 'A': case 'a': case 'ф':
   case 'n': SizeR+=SzV; break;
   case 'К': SizeR+=sizeof(MasLinF); break;//русское или не русское ??
}}}
//---------------------------------------------------------------------------
void __fastcall TFormEditList::ToolButtonCalculClick(TObject *Sender){
 ShellExecute(NULL,"open","Calc",NULL,NULL,SW_RESTORE);
}
//---------------------------------------------------------------------------
void __fastcall TFormEditList::ToolButtonFindeClick(TObject *Sender){
 FindDialogEditList->FindText=StringGridEditList->Cells[ColFind][RowFind];
 FindDialogEditList->Execute();
}
//---------------------------------------------------------------------------
void __fastcall TFormEditList::FindDialogEditListFind(TObject *Sender){
 AnsiString WhatFind,WhereFind;//что искать, где искать
 int i,j,Col=ColFind,//StringGridEditList->Col,
  FindAtRow=FindDialogEditList->Options.Contains(frFindNext)?
   (StringGridEditList->Row<(StringGridEditList->RowCount-1)?
    StringGridEditList->Row+1:0):0,
  FindUntilRow=StringGridEditList->RowCount;//искать от, искать до
 bool NoRegistr=!FindDialogEditList->Options.Contains(frMatchCase);
 WhatFind=NoRegistr?FindDialogEditList->FindText.LowerCase():
  FindDialogEditList->FindText;
M: for(i=FindAtRow;i<FindUntilRow;i++){
  WhereFind=NoRegistr?StringGridEditList->Cells[Col][i].LowerCase():
   StringGridEditList->Cells[Col][i];
  if(WhereFind.Pos(WhatFind)) break;
 }
 if(i<StringGridEditList->RowCount)
  StringGridEditList->Row=i;
 else {
  if(FindAtRow){
   swprintf(Inf,L"текст  \"%s\"  не найден.\nИскать сначала столбца?",
    FindDialogEditList->FindText.c_str());
   if(Application->MessageBox(Inf,L"В этом столбце",MB_YESNO)==IDYES){
    FindAtRow=0; goto M;
  }}
  else {
   swprintf(Inf,L"текст  \"%s\"  не найден.",FindDialogEditList->FindText.c_str());
   Application->MessageBox(Inf,L"В этом столбце",MB_OK);
}}}
//---------------------------------------------------------------------------
void __fastcall TFormEditList::ToolButtonRepClick(TObject *Sender){
 int Row=StringGridEditList->Row,Col=StringGridEditList->Col;
 ReplaceDialogEditList->FindText=StringGridEditList->Cells[Col][Row];
 ReplaceDialogEditList->Execute();
}
//---------------------------------------------------------------------------
void __fastcall TFormEditList::ReplaceDialogEditListFind(TObject *Sender){
 AnsiString WhatFind,WhereFind;//что искать, где искать
 int i,j,Col=StringGridEditList->Col,
  FindAtRow=//ReplaceDialogEditList->Options.Contains(frFindNext)?
   (StringGridEditList->Row<(StringGridEditList->RowCount-1)?
//    StringGridEditList->Row+1:0):0,
    StringGridEditList->Row:StringGridEditList->RowCount),//искать от
  FindUntilRow=StringGridEditList->RowCount;//искать до
 bool NoRegistr=!ReplaceDialogEditList->Options.Contains(frMatchCase);//без учета регистра
 WhatFind=NoRegistr?ReplaceDialogEditList->FindText.LowerCase():
  ReplaceDialogEditList->FindText;
M: for(i=FindAtRow;i<FindUntilRow;i++){
  WhereFind=NoRegistr?StringGridEditList->Cells[Col][i].LowerCase():
   StringGridEditList->Cells[Col][i];
  if(WhereFind.Pos(WhatFind)) break;
 }
 if(i<StringGridEditList->RowCount) {
  StringGridEditList->Row=i;
  if(ReplaceDialogEditList->Options.Contains(frReplaceAll))
   ReplaceDialogEditListReplace(Sender);
 }
 else {
  if(FindAtRow){
   swprintf(Inf,L"текст  \"%s\"  не найден.\nИскать сначала столбца?",
    ReplaceDialogEditList->FindText.c_str());
   if(Application->MessageBox(Inf,L"В этом столбце",MB_YESNO)==IDYES){
    FindAtRow=0; goto M;
  }}
  else {
   swprintf(Inf,L"текст  \"%s\"  не найден.",ReplaceDialogEditList->FindText.c_str());
   Application->MessageBox(Inf,L"В этом столбце",MB_OK);
}}}
//---------------------------------------------------------------------------
void __fastcall TFormEditList::ReplaceDialogEditListReplace(TObject *Sender){
 wchar_t *r; String WhereFind,Rez;//где искать, результат
 int i,j,Col=StringGridEditList->Col,Row=StringGridEditList->Row,L;
 WhereFind=StringGridEditList->Cells[Col][Row];
 if(ReplaceDialogEditList->FindText.Compare(WhatFind)||
	ReplaceDialogEditList->ReplaceText.Compare(RepText)) KRep=0;
 WhatFind=ReplaceDialogEditList->FindText; //искомый текст
 RepText=ReplaceDialogEditList->ReplaceText;//текс-заменитель
 if(!ReplaceDialogEditList->Options.Contains(frMatchCase)){
  WhereFind=WhereFind.LowerCase(); WhatFind=WhatFind.LowerCase();
 }
M:if(ReplaceDialogEditList->Options.Contains(frReplaceAll)){
  for(Row=0;Row<StringGridEditList->RowCount;Row++){
   WhereFind=StringGridEditList->Cells[Col][Row];
   if(!ReplaceDialogEditList->Options.Contains(frMatchCase))
    WhereFind=WhereFind.LowerCase();
   if(i=WhereFind.Pos(WhatFind)){//позиция, с кот.начинается заменяемый текст
    Rez=StringGridEditList->Cells[Col][Row].SubString(1,i-1)+
      ReplaceDialogEditList->ReplaceText+
      StringGridEditList->Cells[Col][Row].SubString(i+
      ReplaceDialogEditList->FindText.Length(),
      StringGridEditList->Cells[Col][Row].Length());
    L=abs(_wtoi(EditList->GetEMasForm()[Col]+1));
    StringGridEditList->Cells[Col][Row]=Rez;
    r=((wchar_t *)StringGridEditList->Objects[0][Row])+SzV;
    EditList->StrSGToRout(StringGridEditList,Row,r);
    StringGridEditList->Row=Row;
    if(Rez.Length()>L) TabControlEditListChange(Sender);// FormShow(Sender);
    KRep++;
  }}
  if(KRep){
   swprintf(Inf,L"Сделано %d замен",KRep);
   Application->MessageBox(Inf,L"В этом столбце",MB_OK);
  }
  else{
   swprintf(Inf,L"текст  \"%s\"  не найден.\nИскать сначала столбца?",WhatFind);
   if(Application->MessageBox(Inf,L"В этом столбце",MB_YESNO)==IDYES){
    Row=0; goto M;
 }}}
 else{
  for(;Row<StringGridEditList->RowCount;Row++){
   WhereFind=StringGridEditList->Cells[Col][Row];
   if(!ReplaceDialogEditList->Options.Contains(frMatchCase))
    WhereFind=WhereFind.LowerCase();
   if(i=WhereFind.Pos(WhatFind)){//позиция, с кот.начинается заменяемый текст
    Rez=StringGridEditList->Cells[Col][Row].SubString(1,i-1)+
      ReplaceDialogEditList->ReplaceText+
      StringGridEditList->Cells[Col][Row].SubString(i+
      ReplaceDialogEditList->FindText.Length(),
      StringGridEditList->Cells[Col][Row].Length());
    L=abs(_wtoi(EditList->GetEMasForm()[Col]+1));
    StringGridEditList->Cells[Col][Row]=Rez;
    r=((wchar_t *)StringGridEditList->Objects[0][Row])+SzV;
    EditList->StrSGToRout(StringGridEditList,Row,r);
    if(Rez.Length()>L) TabControlEditListChange(Sender);// FormShow(Sender);
    StringGridEditList->Row=Row;
    KRep=1;
    break;
  }}
  if(!KRep){
   swprintf(Inf,L"текст  \"%s\"  не найден.\nИскать сначала столбца?",
    WhatFind);
   if(Application->MessageBox(Inf,L"В этом столбце",MB_YESNO)==IDYES){
     Row=0; goto M;
}}}}
//---------------------------------------------------------------------------
void __fastcall TFormEditList::pmFindClick(TObject *Sender){
 RowFind=StringGridEditList->Row; ColFind=StringGridEditList->Col;
 ToolButtonFindeClick(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TFormEditList::pmReplaceClick(TObject *Sender){
 ToolButtonRepClick(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TFormEditList::ToolButtonImagePrnClick(TObject *Sender){
 Application->CreateForm(__classid(TFormEditListPrn),&FormEditListPrn);
 TPrinter *Prn=Printer();
 if(!Prn||!Prn->Printers->Count){
  Application->MessageBox(L"В системе нет принтеров!\n"
   L"Поэтому печать невозможна!",L"ОШИБКА!",MB_OK|MB_ICONERROR);
 }
 else
  FormEditListPrn->ShowModal();
}
//---------------------------------------------------------------------------
TEditList::~TEditList(){
// if(EMasForm){
//  for(int i=0;i<EKForm;i++) if(EMasForm[i]) free(EMasForm[i]);
//  free(EMasForm);
//  EMasForm=NULL;
}//};
//---------------------------------------------------------------------------
void __fastcall TFormEditList::ToolButtonDOSPrnClick(TObject *Sender){
 //Печать под DOS для особо привередливых с плохими принтерами
 int i,j,KolProcShow; FILE *F;
 wchar_t *f,*s,FileForPrnDOS[256],PrintDosExe[256];
 TStringGrid *SG=FormEditList->StringGridEditList;
 swprintf(FileForPrnDOS,L"printDOS.tmp");
 if(F=_wfopen(FileForPrnDOS,L"w")) {
  wchar_t **MasForm=EditList->GetEMasForm();
  for(i=0;i<SG->ColCount;i++)
   fputwc(MasForm[i][1]=='-'?'-':'+',F);//Format
  fputwc('\n',F);
  fwprintf(F,L"#\n");
  if(EditList->FullHead){
   wcscpy(Inf,EditList->FullHead);
   fwprintf(F,L"%s\n",StrWINtoPRN(Inf));//FullHead
  }
  if(EditList->Head&&*EditList->Head){
   wcscpy(Inf,*EditList->Head);
   fwprintf(F,L"%s\n",StrWINtoPRN(Inf));//HeaD
  }
  fwprintf(F,L"#\n");
  fwprintf(F,L"%s\n",EditList->FileSh?EditList->FileSh:L"\0");//FileSh
  if(EditList->GetNSumm())
   for(i=0;EditList->GetNSumm(i);i++)//NSumm
	fwprintf(F,L"%d\xb3",EditList->GetNSumm(i));
  fwprintf(F,L"0\n");
  fwprintf(F,L"%s\n",EditList->FilePrn?EditList->FilePrn:L"\0");//FilePrn
  fwprintf(F,L"%d\n%d\n",EditList->Podch,EditList->Show0);//подчеркив. и показ.нули
  wcscpy(Inf,EditList->Podval?EditList->Podval:L"\0");
  fwprintf(F,L"#\n%s\n#\n",StrWINtoPRN(Inf));//Podval
  for(i=0;i<SG->RowCount;i++){
   for(j=0;j<SG->ColCount-1;j++){
    CleanStr(SG->Cells[j][i].c_str(),Inf);
    fwprintf(F,L"%s\xb3",StrWINtoPRN(Inf));
   }
   CleanStr(SG->Cells[j][i].c_str(),Inf);
   fwprintf(F,L"%s\n",StrWINtoPRN(Inf));
  }
  fputws(L"#\n",F);
  fclose(F);
 }
 swprintf(PrintDosExe,L"%s\\PrintDos.exe",WORKDIR);
 if(_waccess(PrintDosExe,0)) wcscpy(PrintDosExe,L"PrintDos.exe");
 FormEditList->Visible=false;
 _wspawnl(P_WAIT,PrintDosExe,PrintDosExe,FileForPrnDOS,NULL);
 FormEditList->Visible=true;
}
//---------------------------------------------------------------------------
void __fastcall TFormEditList::pmFromMagClick(TObject *Sender){
 Magazine *m,*Mag;
 int LeftForm,hUp,hDw,hMag,*HeaderSumm,w,TopForm,H1,H2,W=0; //,ColDo,RowDo;
 Mag=EditList->Mag(ActivSG->Col)?*EditList->Mag(ActivSG->Col):NULL;
 if(Mag){ //если магазин не пустой
//  if(!FormMag)
  Application->CreateForm(__classid(TFormMag), &FormMag);
//  RowDo=FormEditList->StringGridEditList->Row,
//  ColDo=FormEditList->StringGridEditList->Col;
  FormMag->ComboBoxMag->Clear();
  for(m=Mag;m;m=m->Sled){
   FormMag->ComboBoxMag->Items->Add(m->S);
   w=Canvas->TextWidth(m->S); if(w>W) W=w;
  }
  H1=2+FormEditList->Top+FormEditList->ToolBarEditList->Height+
      TabControlEditList->Top+ActivSG->Top+
      (ActivSG->Row-ActivSG->TopRow)*(ActivSG->RowHeights[ActivSG->Row]+1);
  H2=Screen->Height-30-H1-ActivSG->RowHeights[ActivSG->Row]-ActivSG->GridLineWidth;
  FormMag->Height=H1>H2?H1:H2;
  FormMag->ComboBoxMag->Height=FormMag->Height-24;
  FormMag->ComboBoxMag->DropDownCount=
   (FormMag->ComboBoxMag->Height-FormMag->ComboBoxMag->ItemHeight-8)/
    FormMag->ComboBoxMag->ItemHeight-1;
  if(FormMag->ComboBoxMag->Items->Count<FormMag->ComboBoxMag->DropDownCount)
   FormMag->ComboBoxMag->DropDownCount=FormMag->ComboBoxMag->Items->Count;
  FormMag->ComboBoxMag->Height=
   (FormMag->ComboBoxMag->DropDownCount+1)*FormMag->ComboBoxMag->ItemHeight+
   FormMag->ComboBoxMag->ItemHeight+8;
  FormMag->Height=FormMag->ComboBoxMag->Height+24;
  FormMag->ComboBoxMag->Width=W+
   (FormMag->ComboBoxMag->Items->Count>FormMag->ComboBoxMag->DropDownCount?24:0);
  FormMag->Width=FormMag->ComboBoxMag->Width+12;
  if(FormMag->Width<(FormMag->Canvas->TextWidth(FormMag->Caption)+110)){
   FormMag->Width=FormMag->Canvas->TextWidth(FormMag->Caption)+110;
   FormMag->ComboBoxMag->Width=FormMag->Width-12;
  }
//Определяем местоположение активной строки
  LeftForm=FormEditList->Left+TabControlEditList->Left+ActivSG->Left;
  for(int i=0;i<ActivSG->FixedCols;i++)
   LeftForm+=ActivSG->ColWidths[i]+ActivSG->GridLineWidth;
  for(int i=ActivSG->LeftCol;i<ActivSG->Col;i++)
   LeftForm+=ActivSG->ColWidths[i]+ActivSG->GridLineWidth;
  FormMag->Left=LeftForm;
  FormMag->Top=H2>H1?(Screen->Height-30-H2):H1-FormMag->Height;
//  for(int i=ActivSG->TopRow;i<=ActivSG->Row;i++)
//   TopForm+=ActivSG->RowHeights[i]+ActivSG->GridLineWidth;
//  if((TopForm+FormMag->Height)>(Screen->Height-30))
//   FormMag->Top-=FormMag->Height+ActivSG->RowHeights[ActivSG->Row]+
//    2*ActivSG->GridLineWidth+
//    (FormMag->ComboBoxMag->Items->Count>8?8:FormMag->ComboBoxMag->Items->Count)*
//    FormMag->ComboBoxMag->ItemHeight;
  FormMag->ShowModal();
//  if(FormMag->ComboBoxMag->ItemIndex>-1){
  if(FormMag->Select){
   WORD Key=VK_RETURN; TShiftState ST;
   int Row=FormEditList->StringGridEditList->Row,
    Col=FormEditList->StringGridEditList->Col;
   FormEditList->StringGridEditList->Cells[Col][Row]=
    FormMag->ComboBoxMag->ItemIndex>-1?
     FormMag->ComboBoxMag->Items->Strings[FormMag->ComboBoxMag->ItemIndex]:
     FormMag->ComboBoxMag->Text;
/*   else{
    for(int i=0;i<ComboBoxMag->Items->Count;i++){
     AnsiString Stri=ComboBoxMag->Items->Strings[i],
      Text=ComboBoxMag->Text;
     if(Stri.LowerCase().AnsiPos(Text.LowerCase())==1){
      FormEditList->StringGridEditList->Cells[Col][Row]=
       ComboBoxMag->Items->Strings[i];
      break;
   }}}*/
   StringGridEditListKeyDown(Sender,Key,ST);
  }
  FormMag->ComboBoxMag->Items->Clear();
  StringGridEditList->SetFocus();
}}
//---------------------------------------------------------------------------
void __fastcall TFormEditList::pmList1Click(TObject *Sender){
 if(EditList->Kp){
  TabControlEditList->TabIndex=1; TabControlEditListChange(Sender);
}}
//---------------------------------------------------------------------------
void __fastcall TFormEditList::pmList2Click(TObject *Sender){
 if(EditList->Kp>1){
  TabControlEditList->TabIndex=2; TabControlEditListChange(Sender);
}}
//---------------------------------------------------------------------------
void __fastcall TFormEditList::pmList3Click(TObject *Sender){
 if(EditList->Kp>2){
  TabControlEditList->TabIndex=3; TabControlEditListChange(Sender);
}}
//---------------------------------------------------------------------------
void __fastcall TFormEditList::pmList4Click(TObject *Sender){
 if(EditList->Kp>3){
  TabControlEditList->TabIndex=4; TabControlEditListChange(Sender);
}}
//---------------------------------------------------------------------------
void __fastcall TFormEditList::ToolButtonPrnOptClick(TObject *Sender){
 FormParamPrint->FilePrn=EditList->FilePrn;
 wchar_t *s,S[256];
 wcscpy(S,EditList->FileHelp);
 if(s=wcsrchr(S,'\\')) { *s='\0';
  swprintf(FormParamPrint->FileHelp,L"%s\\ChangParmPrn.htm",S);
 }
 else
  swprintf(FormParamPrint->FileHelp,L"%s\\ChangParmPrn.htm",WORKDIR);
 FormParamPrint->ShowModal();
 if(FormParamPrint->Prn->Printers->Count&&FormParamPrint->Change){
  EditList->Podch=FormParamPrint->Podch;
  EditList->Interval=FormParamPrint->Interval;
  wcscpy(EditList->FontName,FormParamPrint->Font->Name.c_str());
 }
 else{
  EditList->Interval=1.0;
  wcscpy(EditList->FontName,L"Courier New");
}}
//---------------------------------------------------------------------------
void __fastcall TFormEditList::pmFind1Click(TObject *Sender){
 RowFind=StringGridEditList->Row; ColFind=0;
 ToolButtonFindeClick(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TFormEditList::pmFind2Click(TObject *Sender){
 RowFind=StringGridEditList->Row; ColFind=1;
 ToolButtonFindeClick(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TFormEditList::pmFind3Click(TObject *Sender){
 RowFind=StringGridEditList->Row; ColFind=2;
 ToolButtonFindeClick(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TFormEditList::pmFind4Click(TObject *Sender){
 RowFind=StringGridEditList->Row; ColFind=3;
 ToolButtonFindeClick(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TFormEditList::pmFind5Click(TObject *Sender){
 RowFind=StringGridEditList->Row; ColFind=4;
 ToolButtonFindeClick(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TFormEditList::pmFind6Click(TObject *Sender){
 RowFind=StringGridEditList->Row; ColFind=5;
 ToolButtonFindeClick(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TFormEditList::pmFind7Click(TObject *Sender){
 RowFind=StringGridEditList->Row; ColFind=6;
 ToolButtonFindeClick(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TFormEditList::pmFind8Click(TObject *Sender){
 RowFind=StringGridEditList->Row; ColFind=7;
 ToolButtonFindeClick(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TFormEditList::pmFind9Click(TObject *Sender){
 RowFind=StringGridEditList->Row; ColFind=8;
 ToolButtonFindeClick(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TFormEditList::pmFind10Click(TObject *Sender){
 RowFind=StringGridEditList->Row; ColFind=9;
 ToolButtonFindeClick(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TFormEditList::pmFind11Click(TObject *Sender){
 RowFind=StringGridEditList->Row; ColFind=10;
 ToolButtonFindeClick(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TFormEditList::pmFind12Click(TObject *Sender){
 RowFind=StringGridEditList->Row; ColFind=11;
 ToolButtonFindeClick(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TFormEditList::ReadColorOpt(void) {
 FILE *F; wchar_t S[256];
 StringGridEditList->Color=clWhite;
 StringGridEditList->FixedColor=clBtnFace;
 StringGridEditList->Font->Color=clWindowText;
 ColorPhonEdit=clInfoBk;
 ColorPhonView=clWhite;
 ColorPhonFixEdit=clBtnFace;
 ColorPhonFixView=clBtnFace;
 ColorTextView=clWindowText;
 ColorTextEdit=clWindowText;
 VvodEnter=0;
 if(F=_wfopen(FileColorOpt,L"r")){
  if(!ReadStrOpt(S,F)) goto Exit; ColorPhonEdit=(TColor)_wtol(S);
  if(!ReadStrOpt(S,F)) goto Exit; ColorPhonView=(TColor)_wtol(S);
  if(!ReadStrOpt(S,F)) goto Exit; ColorPhonFixEdit=(TColor)_wtol(S);
  if(!ReadStrOpt(S,F)) goto Exit; ColorPhonFixView=(TColor)_wtol(S);
  if(!ReadStrOpt(S,F)) goto Exit; ColorTextEdit=(TColor)_wtol(S);
  if(!ReadStrOpt(S,F)) goto Exit; ColorTextView=(TColor)_wtol(S);
  if(!ReadStrOpt(S,F)) goto Exit; VvodEnter=_wtoi(S);
  Exit:fclose(F);
}}
//---------------------------------------------------------------------------
bool __fastcall TFormEditList::SaveColorOpt(void) {
 FILE *F=_wfopen(FileColorOpt,L"w");
 if(!F) {
  swprintf(Inf,L"Не открылся файл опций - \"%s\"! Запоминание их невозможно!",
   FileColorOpt);
  Application->MessageBox(Inf,L"Ошибка",MB_OK|MB_ICONERROR);
  return false;
 }
 swprintf(Inf,L"%-40ld # Цвет фона редактирования\n",ColorPhonEdit);
 Fputs(Inf,F);
 swprintf(Inf,L"%-40ld # Цвет фона просмотра\n",ColorPhonView);
 Fputs(Inf,F);
 swprintf(Inf,L"%-40ld # Цвет фона фиксированных ячеек редактора\n",ColorPhonFixEdit);
 Fputs(Inf,F);
 swprintf(Inf,L"%-40ld # Цвет фона фиксированных ячеек просмотра\n",ColorPhonFixView);
 Fputs(Inf,F);
 swprintf(Inf,L"%-40ld # Цвет текста редактора\n",ColorTextEdit); Fputs(Inf,F);
 swprintf(Inf,L"%-40ld # Цвет текста просмотра\n",ColorTextView); Fputs(Inf,F);
 swprintf(Inf,
  L"%-40d # Ввод по Enter: 1 - да, 0 - нет, -1 - да (нет в контекстном меню)\n",
  VvodEnter); Fputs(Inf,F);
 fclose(F); return true;
}
//---------------------------------------------------------------------------
void __fastcall TFormEditList::pmColorPhonClick(TObject *Sender){
 ColorDialog->Color=StringGridEditList->Color;//фон редактирования
 if(ColorDialog->Execute()){
  StringGridEditList->Color=ColorDialog->Color;
   if(wcsstr(Caption.c_str(),L"Просмотр"))
    ColorPhonView=ColorDialog->Color;
   else
    ColorPhonEdit=ColorDialog->Color;
  SaveColorOpt();
}}
//---------------------------------------------------------------------------
void __fastcall TFormEditList::pmColorFixPhonClick(TObject *Sender){
 ColorDialog->Color=StringGridEditList->FixedColor;//фон редактирования
 if(ColorDialog->Execute()){
  StringGridEditList->FixedColor=ColorDialog->Color;
   if(wcsstr(Caption.c_str(),L"Просмотр"))
    ColorPhonFixView=ColorDialog->Color;
   else
    ColorPhonFixEdit=ColorDialog->Color;
  SaveColorOpt();
}}
//---------------------------------------------------------------------------
void __fastcall TFormEditList::pmColorTextClick(TObject *Sender){
 ColorDialog->Color=StringGridEditList->Font->Color;//фон редактирования
 if(ColorDialog->Execute()){
  StringGridEditList->Font->Color=ColorDialog->Color;
  if(wcsstr(Caption.c_str(),L"Просмотр"))
   ColorTextView=ColorDialog->Color;
  else
   ColorTextEdit=ColorDialog->Color;
  SaveColorOpt();
}}
//---------------------------------------------------------------------------
void __fastcall TFormEditList::pmVvodEnterClick(TObject *Sender){
 if(VvodEnter){
  VvodEnter=0;
  pmVvodEnter->Caption=L"Сделать Ввод по Enter";
  StatusBarEditList->Panels->Items[1]->Text=L"Ввод без Enter";
 }
 else{
  VvodEnter=1;
  pmVvodEnter->Caption=L"Сделать Ввод без Enter";
  StatusBarEditList->Panels->Items[1]->Text=L"Ввод по Enter";
 }
 SaveColorOpt();
}
//---------------------------------------------------------------------------
void NewSumma(double OldD,double NewD,int Col){
 double Sum=
  _wtof(FormEditList->HeaderControlEditList->Sections->Items[Col]->Text.c_str());
 Sum+=NewD-OldD;
 swprintf(Inf,EditList->FormatCol(Col),Sum);
 FormEditList->HeaderControlEditList->Sections->Items[Col]->Text=Inf;
}
//---------------------------------------------------------------------------
void NewCol(double OldD,double NewD,int Col,int Row){
 swprintf(Inf,EditList->FormatCol(Col),NewD);
 FormEditList->StringGridEditList->Cells[Col][Row]=Inf;
}
//---------------------------------------------------------------------------
int CmpTable (const void *B1, const void *B2) {
 TTable *t1=(TTable *)B1,*t2=(TTable *)B2; int Rez;
 if(PoRostu==32000) return t1->N>t2->N?1:-1;
 if(wcschr(PoleFormat,'s')){
  if(PoRostu==(Ind+1)){
   Rez=wcscmp(t1->S[Ind].c_str(),t2->S[Ind].c_str());
   if(Rez) return Rez;
  }
  else{
   Rez=wcscmp(t2->S[Ind].c_str(),t1->S[Ind].c_str());
   if(Rez) return Rez;
 }}
 else{
  double a=t1->S[Ind].IsEmpty()?0.0:_wtof(t1->S[Ind].c_str()),
   b=t2->S[Ind].IsEmpty()?0.0:_wtof(t2->S[Ind].c_str());
  if(PoRostu==(Ind+1)){
   if(a>b) Rez=1; else if(a<b) Rez=-1;
  }
  else{
   if(a>b) Rez=-1; else if(a<b) Rez=1;
  }
  if(Rez) return Rez;
 }
 if((unsigned long)B1>(unsigned long)B2) return 1;
 else return -1;
}
//---------------------------------------------------------------------------
void __fastcall TFormEditList::BackSort(){
 int i,j,h=FormEditList->StringGridEditList->RowCount,
  l=FormEditList->StringGridEditList->ColCount;
 if(l==1) return;
 String *s; TTable *t,*Tabl=(TTable *)calloc(h,sizeof(TTable));
 PoRostu=32000;
 for(i=0,t=Tabl;i<h;i++,t++){
  t->r=(void *)StringGridEditList->Objects[0][i];
  t->N=(int)(unsigned long)StringGridEditList->Objects[1][i];
  t->S=(String *)calloc(l,sizeof(AnsiString));
  for(j=0,s=t->S;j<l;j++) *s++=StringGridEditList->Cells[j][i];
 }
 qsort(Tabl,h,sizeof(TTable),CmpTable);
 for(i=0,t=Tabl;i<h;i++,t++){
  StringGridEditList->Objects[0][i]=(TObject *)t->r;
  StringGridEditList->Objects[1][i]=(TObject *)(unsigned long)t->N;
  for(j=0,s=t->S;j<l;j++) StringGridEditList->Cells[j][i]=*s++;
  free(t->S);
 }
 free(Tabl);
 EditList->Value=StringGridEditList->Cells[ActivSG->Col][ActivSG->Row];
 StatusBarEditList->Panels->Items[2]->Text=L"";
}
//---------------------------------------------------------------------------
struct NewList{
 NewList *Sled; wchar_t Val[1];
 void Fill(wchar_t *Format,int *MNN,int Ind,TStringGrid *SG,int i,wchar_t **MasForm);
 void Back(wchar_t *Format,int *MNN,int Ind,TStringGrid *SG,int i,wchar_t **MasForm);
};
//---------------------------------------------------------------------------
void NewList::Back(wchar_t *Format,int *MNN,int Ind,TStringGrid *SG,int i,
  wchar_t **MasForm){
 wchar_t Buf[256],*f,*p,B,*s; int j=0,k; DouSt *D;
 for(int *mnn=MNN;*mnn;mnn++){
  int dV=SzV,dv;
  for(f=Format+1,k=0;k<*mnn;k++){
   if(p=wcschr(f,'%')) f=p+1;
   B=p?p[-1]:f[wcslen(f)-1];
   switch(B){
    case 'f': dv=SzD; dV+=dv; break;
    case 'l': dv=SzL; dV+=dv; break;
    case 's': dv=SzV; dV+=dv; break;
    case 'U': dv=SzV; dV+=dv; break;
  }}
  dV-=dv;
  switch(B){
   case 'f':
    if(j==Ind) j++;
    swprintf(Inf,MasForm[j],*(double *)((wchar_t *)this+dV));
    SG->Cells[j][i]=Inf; j++;
    break;
   case 'l':
    if(j==Ind) j++;
    swprintf(Inf,MasForm[j],*(long *)((wchar_t *)this+dV));
    SG->Cells[j][i]=Inf; j++;
    break;
   case 's':
    if(j==Ind) j++;
    if(s=(wchar_t *)*(unsigned long *)((wchar_t *)this+dV)){
     swprintf(Inf,MasForm[j],s); SG->Cells[j][i]=Inf; free(s);
    }
    else SG->Cells[j][i]=L"";
    j++;
    break;
   case 'U':
    k=_wtoi(f);
    D=(DouSt *)*(unsigned long *)((wchar_t *)this+dV);
    for(int K=j+k+1;j<K;j++){
     if(j!=Ind){
      if(D[0].Atr==IsS){
       swprintf(Inf,MasForm[j],D[0].U.S);
       free(D[0].U.S); D[0].U.S=NULL;
      }
      else{
       if(D[0].U.D!=0.0)
        swprintf(Inf,MasForm[j],D[0].U.D);
       else wcscpy(Inf,L"\0");
      }
      SG->Cells[j][i]=Inf;
      *D++;
    }}
    free(D);
    break;
}}}
//---------------------------------------------------------------------------
void NewList::Fill(wchar_t *Format,int *MNN,int Ind,TStringGrid *SG,int i,
  wchar_t **MasForm){
 wchar_t Buf[256],*f,*p,B; int j=0,k; DouSt *D;
 for(int *mnn=MNN;*mnn;mnn++){
  int dV=SzV,dv;
  for(f=Format+1,k=0;k<*mnn;k++){
   if(p=wcschr(f,'%')) f=p+1;
   B=p?p[-1]:f[wcslen(f)-1];
   switch(B){
    case 'f': dv=SzD; dV+=dv; break;
    case 'l': dv=SzL; dV+=dv; break;
    case 's': dv=SzV; dV+=dv; break;
    case 'U': dv=SzV; dV+=dv; break;
  }}
  dV-=dv;
  switch(B){
   case 'f':
    if(j==Ind) j++;
    wcscpy(Inf,SG->Cells[j][i].c_str()); CleanStr(Inf,Buf);
    *(double *)((wchar_t *)this+dV)=_wtof(Buf); j++;
    break;
   case 'l':
    if(j==Ind) j++;
    wcscpy(Inf,SG->Cells[j][i].c_str()); CleanStr(Inf,Buf);
    *(long *)((wchar_t *)this+dV)=_wtol(Buf); j++;
    break;
   case 's':
    if(j==Ind) j++;
    wcscpy(Inf,SG->Cells[j][i].c_str()); CleanStr(Inf,Buf);
	*(wchar_t **)((wchar_t *)this+dV)=
     wcscpy((wchar_t *)calloc(wcslen(Buf)+1,SzC),Buf);
    j++;
    break;
   case 'U':
    k=_wtoi(f);
	*(DouSt **)((wchar_t *)this+dV)=
     D=(DouSt *)calloc(k,sizeof(DouSt));
    for(int K=j+k+1;j<K;j++){
     if(j!=Ind){
      wcscpy(Inf,SG->Cells[j][i].c_str()); CleanStr(Inf,Buf);
      switch(MasForm[j][wcslen(MasForm[j])-1]){
       case 's': *D++=Buf; break;
       case 'f': case 'g': *D++=_wtof(Buf); break;
    }}}
    break;
}}}
//---------------------------------------------------------------------------
wchar_t *ShapkaNew(int Ind,wchar_t *Name){
 FILE *Fi,*Fo; wchar_t *Shapka=(wchar_t *)calloc(16,SzC),S[1024],*s,*ss;
 int i,N1,N2,lp=0,l;
 if(Fi=_wfopen(EditList->FileSh,L"r")){
  int N=wcsstr(EditList->FileSh,L"shapka")?_wtoi(EditList->FileSh+6)+1:0;
  swprintf(Shapka,L"shapka%d",N);
  if(Fo=_wfopen(Shapka,L"w")){
   for(N=0;fgetws(Inf,1023,Fi);N++){
    wcscpy(S,Inf); s=wcsrchr(S,'\xb3'); ss=wcsrchr(S,'\n');
    l=ss-(s?s:S); if(l>lp) lp=l;
   }
   for(i=0,s=S;i<Ind;i++){ for(;*s&&*s!='\xb3';s++); s++; }
   N1=s-S; for(;*s&&*s!='\xb3'&&*s!='\n';s++);
   if(*s!='\n') s++; N2=s-S; if(N2==N1) N2+=lp;
   rewind(Fi);
   while(fgetws(s=S,1023,Fi)){
    if(N1){ s[N1-1]='\0'; fputws(S,Fo); fputws(S+N2-1,Fo); s[N1-1]='\xb3'; }
    else fputws(S+N2,Fo);
    StrDOStoWIN(S);
    memcpy(Inf,S+N1,N2-N1-1); Inf[N2-N1-1]='\0'; CleanStr(Inf,S);
    if(S[wcslen(S)-1]=='-') S[wcslen(S)-1]='\0'; else wcscat(S,L" ");
    wcscat(Name,S);
   }
   if(Name[wcslen(Name)-1]==' ')  Name[wcslen(Name)-1]='\0';
   if(Name[wcslen(Name)-1]=='\n') Name[wcslen(Name)-1]='\0';
   fclose(Fo);
  }
  fclose(Fi);
 }
 return Shapka;
}
//---------------------------------------------------------------------------
wchar_t *FormatNew(int N,int Ind){
 wchar_t *Format=(wchar_t *)calloc(N*3,SzC); int i;
 for(i=0;i<N;i++){
  if(i==Ind) continue;
  wcscat(Format,EditList->GetEMasForm()[i][1]=='-'?L"%-s":L"%s");
 }
 return Format;
}
//---------------------------------------------------------------------------
wchar_t *FormatMNNNew(wchar_t *FullFormat,int *MasNN,int Ind,int **MNN){
 int n=KSimbStr(FullFormat,'%'),l,d,i,j,k,*mnn,*masnn;
 wchar_t **FormatMas=(wchar_t **)calloc(n,SzV),*ff,*fp,*f,
  *Format=(wchar_t *)calloc(wcslen(FullFormat)+1,SzC);
//все форматы сохраняем
 for(ff=FullFormat,i=0;fp=wcschr(ff+1,'%');i++,ff=fp){
  *fp='\0';
  FormatMas[i]=wcscpy((wchar_t *)calloc(wcslen(ff)+1,SzC),ff);
  *fp='%';
 }
 FormatMas[i]=wcscpy((wchar_t *)calloc(wcslen(ff)+1,SzC),ff);
 for(k=0,masnn=MasNN;*masnn;k++,masnn++);
 *MNN=mnn=(int *)calloc(k+1,SzI);
 for(j=0,masnn=MasNN;*masnn;masnn++){
  for(f=FormatMas[*masnn-1];f[1];f++);
  if((*f=='s')||(*f=='f')||(*f=='l')){
   j++;
   if((j-1)==Ind) continue;
   *mnn++=*masnn;
  }
  else if(*f=='U'){
 //размер массива уменьшаем на 1
   f=FormatMas[*masnn-1]+1;
   l=_wtoi(f);
   if((j<=Ind)&&((j+l)>Ind)){
    if(f=wcschr(FormatMas[*masnn-1],'.')) d=_wtoi(f+1);
    swprintf(FormatMas[*masnn-1],L"%%%d.%dU",l-1,d);
    j+=l;
    if(l==1) continue;
   }
   *mnn++=*masnn;
  }
  else{
   swprintf(Inf,L"поля типа \'%c\' не обрабатываются!",*f);
   Application->MessageBoxA(Inf,L"Обратитесь к авторам программы",MB_OK);
   free(Format); free(*MNN);
   return NULL;
 }}
 for(i=0,f=Format;i<n;i++){
  f+=swprintf(f,L"%s",FormatMas[i]);
  free(FormatMas[i]);
 }
 free(FormatMas);
 return Format;
}
//---------------------------------------------------------------------------
int SizeR(wchar_t *Format){ int sizeR=SzV;
 for(wchar_t *f=Format;*f;f++){
  switch(*f){
   case 'c': sizeR+=2*SzC; break;
   case 'd': sizeR+=SzI; break;
   case 'l': sizeR+=SzL; break;
   case 's': case 'm': sizeR+=SzV; break;
   case 'f': case 'e': case 'g': sizeR+=SzD; break;
   case 'p': case 'K': case 'U': case 'A': case 'a': case 'ф': case 'n':
    sizeR+=SzV;
    break;
 }}
 return sizeR;
}
//---------------------------------------------------------------------------
void __fastcall TFormEditList::MouseDown(TObject *Sender,
  TMouseButton Button,TShiftState Shift,int X,int Y){
 int i,h=StringGridEditList->RowCount,j,l=StringGridEditList->ColCount;
 TSpeedButton *tsB=(TSpeedButton *)Sender;
 tsB->Down=true;
 if(Shift.Contains(ssAlt)){
  wchar_t Buf[256],Name[256]=L"\0"; Magazine *MVal=NULL,*m;
  int N,Ind=((TSpeedButton *)Sender)->HelpContext,LeftForm,TopForm,Wmin,W=0,w,
   *NSumm=NULL,KFixStlb=0;
  if(PoRostu!=32000) BackSort();
  if(!FormMag) Application->CreateForm(__classid(TFormMag), &FormMag);
  for(i=0;i<h;i++){
   wcscpy(Inf,StringGridEditList->Cells[Ind][i].c_str()); CleanStr(Inf,Buf);
   TakeMagazine(&MVal,Buf);
  }
  FormMag->ComboBoxMag->Clear();
  if(KolElem(MVal)==1){
   FormMag->ComboBoxMag->Items->Add(MVal->S);
   FormMag->ComboBoxMag->ItemIndex=0;
  }
  else{
   for(m=MVal;m;m=m->Sled){
    FormMag->ComboBoxMag->Items->Add(m->S);
    w=Canvas->TextWidth(m->S); if(w>W) W=w;
   }
   FormMag->ComboBoxMag->Width=W+
    (FormMag->ComboBoxMag->Items->Count>FormMag->ComboBoxMag->DropDownCount?24:0);
   Wmin=FormMag->Canvas->TextWidth(FormMag->Caption);
   if(FormMag->ComboBoxMag->Width<Wmin)
    FormMag->ComboBoxMag->Width=Wmin;
   FormMag->ComboBoxMag->Height=
    (FormMag->ComboBoxMag->Items->Count>FormMag->ComboBoxMag->DropDownCount?
      FormMag->ComboBoxMag->Items->Count:FormMag->ComboBoxMag->DropDownCount)*
     FormMag->ComboBoxMag->ItemHeight;
   FormMag->PanelMag->Width=FormMag->ComboBoxMag->Width+2;
   FormMag->PanelMag->Height=FormMag->ComboBoxMag->Height+8;
   FormMag->ClientWidth=FormMag->PanelMag->Width;
   FormMag->ClientHeight=FormMag->PanelMag->Height;
   LeftForm=FormEditList->Left+TabControlEditList->Left+ActivSG->Left;
   for(int i=0;i<ActivSG->FixedCols;i++)
    LeftForm+=ActivSG->ColWidths[i]+ActivSG->GridLineWidth;
   for(int i=ActivSG->LeftCol;i<Ind;i++)
    LeftForm+=ActivSG->ColWidths[i]+ActivSG->GridLineWidth;
   FormMag->Left=LeftForm;
   TopForm=FormEditList->Top+FormEditList->ToolBarEditList->Height+
            TabControlEditList->Top+ActivSG->Top;
   FormMag->Top=TopForm;
   if((TopForm+FormMag->Height)>(Screen->Height-30))
    FormMag->Height=Screen->Height-36-TopForm;
   FormMag->ClientHeight=FormMag->Height-32;
   FormMag->PanelMag->Height=FormMag->ClientHeight;
   FormMag->ComboBoxMag->Height=FormMag->PanelMag->Height-8;
   FormMag->ClientWidth=FormMag->Width-8;
   FormMag->PanelMag->Width=FormMag->ClientWidth;
   FormMag->ComboBoxMag->Width=FormMag->PanelMag->Width-8;
   FormMag->ComboBoxMag->ItemIndex=0;
   FormMag->ShowModal();
  }
  if(FormMag->ComboBoxMag->ItemIndex>-1){
   int i,Ns,*MNN,sizeR=SizeR(EditList->FullFormat);
   wchar_t *FileSh=ShapkaNew(Ind,Name),
    *Format=FormatMNNNew(EditList->FullFormat,EditList->MasNN,Ind,&MNN),
    **Head=(wchar_t **)calloc(1,SzV),*Fprn=L"file.prn",
    *Help=EditList->FileHlp(),FullHead[2048]=L"\0",Val[256];
   TEditList *EditListOld=EditList; NewList *R=NULL,**Pr=&R,*r;
   wcscpy(Val,
    FormMag->ComboBoxMag->Items->Strings[FormMag->ComboBoxMag->ItemIndex].c_str());
   if(!Format) goto M;
   for(Ns=0;EditList->GetNSumm()&&EditList->GetNSumm()[Ns];Ns++);//сколько сумматоров
   if(Ns) NSumm=(int *)calloc(Ns+1,SzI);
   for(i=0;i<Ns;i++){
    if((N=EditList->GetNSumm()[i])==(Ind+1)) continue;//выбранный столбец не суммируем
    if(N<(Ind+1)) NSumm[i]=N; else NSumm[i]=N-1;
   }
   if(EditList->FH())
    swprintf(FullHead,L"%s\n%s: %s",EditList->FH(),Name,Val);
   else if(EditList->Head[0]){
    swprintf(Inf,L"%s. %s: %s",EditList->Head[0],Name,Val);
    Head[0]=wcscpy((wchar_t *)calloc(wcslen(Inf)+1,SzC),Inf);
   }
   if(Ind<EditList->GetKolFixStlb()) KFixStlb=EditList->GetKolFixStlb()-1;
   for(i=0;i<h;i++){
    wcscpy(Inf,StringGridEditList->Cells[Ind][i].c_str()); CleanStr(Inf,Buf);
    if(!wcscmp(Buf,Val)){
     r=*Pr=(NewList *)calloc(1,sizeR); Pr=(NewList **)r;
     r->Fill(Format,MNN,Ind,StringGridEditList,i,EditList->GetEMasForm());
   }}
   EditList=new TEditList((void **)&R,MNN,false,NSumm,false,NULL,Format,
    FileSh,Head,Fprn,NULL,EditListOld->TestField,NULL,KFixStlb,-1,-1,-1,-1,
    FullHead,EditList->Podv(),0,Help,NULL,NULL);
   if(EditList->Diagnos()){
    Application->CreateForm(__classid(TFormEditList),&FormEditList);
    FormEditList->ShowModal();
   }
   delete EditList;
   EditList=EditListOld;
   if(!EditList->FH()){//возможно, было редактирование
    for(i=0,r=R;i<h;i++){
     wcscpy(Inf,StringGridEditList->Cells[Ind][i].c_str()); CleanStr(Inf,Buf);
     if(!wcscmp(Buf,Val)){
      r->Back(Format,MNN,Ind,StringGridEditList,i,EditList->GetEMasForm());
      r=r->Sled;
   }}}
   while(r=R){ R=r->Sled; free(r); }
   if(MNN) free(MNN); if(NSumm) free(NSumm); if(Format) free(Format);
M: if(Head){ if(Head[0]) free(Head[0]); free(Head); }
   if(FileSh) free(FileSh);
  }
  DelAllMagazine(&MVal);
  FormMag->ComboBoxMag->Items->Clear();
 }
 else if(!Shift.Contains(ssCtrl)&&!Shift.Contains(ssShift)){
  if(l==1){
   wcscpy(Inf,L"Для одностолбцовой таблицы сортировка запрещена!");
   Application->MessageBox(Inf,L"Внимание",MB_OK|MB_ICONWARNING);
   return;
  }
  Ind=((TSpeedButton *)Sender)->HelpContext;
  PoleFormat=EditList->FormatCol(Ind);
  TTable *Tabl,*t; String *s;
  if((PoRostu!=32000)&&(abs(PoRostu)!=(Ind+1))) BackSort();
  if(PoRostu==32000){
   PoRostu=Ind+1;
   Tabl=(TTable *)calloc(h,sizeof(TTable));
   for(i=0,t=Tabl;i<h;i++,t++){
    StringGridEditList->Objects[1][i]=(TObject *)(unsigned long)(t->N=i);
    t->r=(void *)StringGridEditList->Objects[0][i];
	t->S=(String *)calloc(l,sizeof(String));
	for(j=0,s=t->S;j<l;j++) *s++=StringGridEditList->Cells[j][i];
   }
   qsort(Tabl,h,sizeof(TTable),CmpTable);
   for(i=0,t=Tabl;i<h;i++,t++){
    StringGridEditList->Objects[0][i]=(TObject *)t->r;
    StringGridEditList->Objects[1][i]=(TObject *)(unsigned long)t->N;
    for(j=0,s=t->S;j<l;j++) StringGridEditList->Cells[j][i]=*s++;
    free(t->S);
   }
   free(Tabl);
   EditList->Value=StringGridEditList->Cells[ActivSG->Col][ActivSG->Row];
   swprintf(Inf,L"Элементы %d-го столбца растут",Ind+1);
   StatusBarEditList->Panels->Items[2]->Text=Inf;
  }
  else if(PoRostu==(((TSpeedButton *)Sender)->HelpContext+1)){
   PoRostu*=-1;
   Tabl=(TTable *)calloc(h,sizeof(TTable));
   for(i=0,t=Tabl;i<h;i++,t++){
    t->r=(void *)StringGridEditList->Objects[0][h-1-i];
    t->N=(int)(unsigned long)StringGridEditList->Objects[1][h-1-i];
	t->S=(String *)calloc(l,sizeof(String));
    for(j=0,s=t->S;j<l;j++) *s++=StringGridEditList->Cells[j][h-i-1];
   }
   for(i=0,t=Tabl;i<h;i++,t++){
    StringGridEditList->Objects[0][i]=(TObject *)t->r;
    StringGridEditList->Objects[1][i]=(TObject *)(unsigned long)t->N;
    for(j=0,s=t->S;j<l;j++) StringGridEditList->Cells[j][i]=*s++;
    free(t->S);
   }
   free(Tabl);
   swprintf(Inf,L"Элементы %d-го столбца убывают",Ind+1);
   StatusBarEditList->Panels->Items[2]->Text=Inf;
  }
  else BackSort();
  StringGridEditList->SetFocus();
}}
//---------------------------------------------------------------------------
struct TClip { TClip *Sled; wchar_t Str[1]; };
//---------------------------------------------------------------------------
void __fastcall TFormEditList::ContextPopupClipping(TObject *Sender,
  const TPoint &MousePos, bool &Handled){
 for(TKnopka *k=EditList->Shapk->GetR();k;k=k->Sled)
  if(k->GetNn()!=k->GetNk()){
   wcscpy(Inf,L"Для многоуровневой шапки этот режим пока не работает");
   Application->MessageBox(Inf,L"Ошибка",MB_OK|MB_ICONERROR);
   return;//шапка непростая
  }
 if(EditList->Podval&&EditList->Podval[0]||
    EditList->FullHead&&EditList->FullHead[0]){//если просмотр!!!
  int LeftForm,w,W=0,TopForm,Wmin,i,j,h=StringGridEditList->RowCount,
   l=StringGridEditList->ColCount;
  wchar_t *s; Magazine *PoleZnach=NULL; bool Show0;
  Ind=((TSpeedButton *)Sender)->HelpContext;
  for(i=0;i<h;i++){//соберем магазин значений столбца
   wcscpy(s=Inf,ActivSG->Cells[Ind][i].c_str());
   while(*s&&*s==' ') s++; TakeMagazine(&PoleZnach,s);
  }
  if(!FormMag) Application->CreateForm(__classid(TFormMag), &FormMag);
  for(Magazine *m=PoleZnach;m;m=m->Sled){//перенесем магазин в ComboBox
   FormMag->ComboBoxMag->Items->Add(m->S);
   w=Canvas->TextWidth(m->S); if(w>W) W=w;
  }
//Определим ширину формы
  FormMag->ComboBoxMag->Width=W+
   (FormMag->ComboBoxMag->Items->Count>FormMag->ComboBoxMag->DropDownCount?24:0);
  Wmin=2*(FormMag->Canvas->TextWidth(FormMag->Caption));
  if(FormMag->ComboBoxMag->Width<Wmin) FormMag->ComboBoxMag->Width=Wmin;
  FormMag->PanelMag->Width=FormMag->ComboBoxMag->Width+6;
  FormMag->ClientWidth=FormMag->PanelMag->Width;
//Определим ее левый край
  LeftForm=Left+TabControlEditList->Left+
   StringGridEditList->Left+StringGridEditList->GridLineWidth+6;
  if(Ind<ActivSG->LeftCol)//если среди фиксированных столбцов
   for(i=0;i<=Ind;i++)
    LeftForm+=ActivSG->ColWidths[i]+ActivSG->GridLineWidth;
  else{//если среди прокурчиваемых
   for(i=0;i<ActivSG->FixedCols;i++)
    LeftForm+=ActivSG->ColWidths[i]+ActivSG->GridLineWidth;
   for(i=ActivSG->LeftCol;i<=Ind;i++)
    LeftForm+=ActivSG->ColWidths[i]+ActivSG->GridLineWidth;
  }
  if((LeftForm+FormMag->Width)>Screen->Width)//если вылезли справа
   LeftForm-=StringGridEditList->ColWidths[Ind]+
    StringGridEditList->GridLineWidth+FormMag->Width;
  FormMag->Left=LeftForm;
//Определим верхний край формы
  TopForm=Top+TabControlEditList->Top+ActivSG->Top;
//  for(int i=ActivSG->TopRow;i<=ActivSG->Row;i++)
//   TopForm+=ActivSG->RowHeights[i]+ActivSG->GridLineWidth;
  FormMag->Top=TopForm;
//Определим высоту формы
  FormMag->ComboBoxMag->Height=24+FormMag->ComboBoxMag->DropDownCount+
   FormMag->ComboBoxMag->Items->Count*FormMag->ComboBoxMag->ItemHeight;
  FormMag->PanelMag->Height=FormMag->ComboBoxMag->Height+6;
  FormMag->ClientHeight=FormMag->PanelMag->Height;
  if((TopForm+FormMag->Height)>(Screen->Height-60)){//урежем снизу
   FormMag->Height=Screen->Height-60-TopForm;
   FormMag->PanelMag->Height=FormMag->ClientHeight;
   FormMag->ComboBoxMag->Height=FormMag->PanelMag->Height-6;
  }
  FormMag->ComboBoxMag->ItemIndex=0;
  FormMag->ShowModal();
  if(FormMag->ComboBoxMag->ItemIndex>-1){
//Для полученного значения сформируем список структур
   wchar_t *FileSh=L"shapka",*Format=(wchar_t *)calloc(1024,SzC),Form[16],Znach[256],
    **Head=(wchar_t **)calloc(1,SzV),*Fprn=L"clip.prn",*Help=(wchar_t *)calloc(256,SzV),
    FullHead[256],*ss,Cap[256],*Podval,B,*t;
   int *MNN=(int *)calloc(l,SzI),*NSumm,L=0,TopForms,K=0,KF,D;
   double Z=0.0; FILE *F;
   wcscpy(Znach,
    FormMag->ComboBoxMag->Items->Strings[FormMag->ComboBoxMag->ItemIndex].c_str());
   for(i=0;i<h;i++){//посчитаем число требуемых элементов
    wcscpy(s=Inf,StringGridEditList->Cells[Ind][i].c_str());
    while(*s&&*s==' ') s++;
    if(!wcscmp(s,Znach)) K++;
   }
   TClip **Clip=(TClip **)calloc(K,SzV),*c,*C;
   TEditList *EditListOld=EditList; TStringGrid *ActivSGOld=ActivSG;
   for(j=0,s=Format;j<l;j++){
    if(j!=Ind){
     wcscpy(Form,EditList->FormatCol(j));
//     wcscpy(s,Form); s+=wcslen(Form);
     switch(Form[wcslen(Form)-1]){
      case 'c':
       L+=2*SzC;
       if(Form[1]=='-'){ wcscpy(s,L"%-c"); s+=3; } else{ wcscpy(s,L"%c"); s+=2; }
      break;
      case 'd':
       L+=SzI;
       if(Form[1]=='-'){ wcscpy(s,L"%-d"); s+=3; } else{ wcscpy(s,L"%d"); s+=2; }
       break;
      case 'l':
       L+=SzL;
       if(Form[1]=='-'){ wcscpy(s,L"%-ld"); s+=4; } else{ wcscpy(s,L"%ld"); s+=3; }
       break;
      case 's':
       L+=SzV;
       if(Form[1]=='-'){ wcscpy(s,L"%-s"); s+=3; } else{ wcscpy(s,L"%s"); s+=2; }
       break;
      case 'f':
       L+=SzD;
       if(t=wcschr(Form,'.')) D=_wtoi(t+1); else D=0;
       s+=swprintf(s,Form[1]=='-'?L"%%-0.%df":L"%%0.%df",D);
       break;
      case 'e':
       L+=SzD;
       if(t=wcschr(Form,'.')) D=_wtoi(t+1); else D=0;
	   s+=swprintf(s,Form[1]=='-'?L"%%-0.%d.3e":L"%%0.%d.3e",D);
       break;
   }}}
   for(i=0;i<K;i++) Clip[i]=(TClip *)calloc(L+4,SzC);
   for(i=0;i<K-1;i++) Clip[i]->Sled=Clip[i+1];
   for(c=Clip[0],i=0;c;i++,c=c->Sled){
    wchar_t S[80],C; int I; long LL; double D;
    for(;i<h;i++){
     wcscpy(ss=Inf,StringGridEditList->Cells[Ind][i].c_str());
     while(*ss&&*ss==' ') ss++;
     if(!wcscmp(ss,Znach)) break;
    }
    for(j=0,s=c->Str;j<l;j++) if(j!=Ind){
     wcscpy(Form,EditList->FormatCol(j));
     switch(Form[wcslen(Form)-1]){
      case 'c':
       if(ActivSG->Cells[j][i].IsEmpty()) C='\0';
       else{
        wcscpy(S,ActivSG->Cells[j][i].c_str());
        for(wchar_t *ss=S;*ss&&*ss!=' ';ss++); C=ss?*ss:'\0';
       }
       *s=C; s+=2*SzC; break;
      case 'd':
       if(ActivSG->Cells[j][i].IsEmpty()) I=0;
       else I=_wtoi(ActivSG->Cells[j][i].c_str());
       *(int *)s=I; s+=SzI; break;
      case 'l':
       if(ActivSG->Cells[j][i].IsEmpty()) LL=0;
       else LL=_wtol(ActivSG->Cells[j][i].c_str());
       *(long *)s=LL; s+=SzL; break;
      case 's':
       if(ActivSG->Cells[j][i].IsEmpty()) ss=NULL;
       else{
        ss=(wchar_t *)calloc(ActivSG->Cells[j][i].Length()+1,SzC);
        wcscpy(ss,ActivSG->Cells[j][i].c_str());
       }
       *(wchar_t **)s=ss; s+=SzV; break;
      case 'f':
       if(ActivSG->Cells[j][i].IsEmpty()) D=0;
       else D=_wtof(ActivSG->Cells[j][i].c_str());
       *(double *)s=D;
       s+=SzD;
       break;
      case 'e':
       if(ActivSG->Cells[j][i].IsEmpty()) D=0;
       else D=_wtof(ActivSG->Cells[j][i].c_str());
       *(double *)s=D;
       s+=SzD;
       break;
   }}}
 //Покажем список EditListом
   Head[0]=(wchar_t *)calloc(256,SzC); for(j=0;j<l-1;j++) MNN[j]=j+1;
   wcscpy(s=Inf,((TSpeedButton *)Sender)->Caption.c_str());
   for(ss=Cap;*s;s++){
    switch(*s){
     case '-': if(s[1]!='\r') *ss++=*s;
     case ' ':case '\x0D':case '\x0A': break;
     default: *ss++=*s;
   }}
   *ss='\0';
   wcscpy(Form,EditList->FormatCol(Ind)); B=Form[wcslen(Form)-1];
   if(FormMag->ComboBoxMag->Items->Strings[FormMag->ComboBoxMag->ItemIndex].IsEmpty()&&
      B!='s'){
    switch(B){
     case 'c': wcscpy(Znach,L"\\0"); break;
     case 'd': wcscpy(Znach,L"0"); break;
     case 'l': wcscpy(Znach,L"0L"); break;
     case 'f':
      if(t=wcschr(Form,'.')) D=_wtoi(t+1); else D=0;
      swprintf(Form,L"%%0.%df",D); swprintf(Znach,Form,Z); break;
     case 'e':
      if(t=wcschr(Form,'.')) D=_wtoi(t+1); else D=0;
      swprintf(Form,L"%%0.%d.3e",D); swprintf(Znach,Form,Z); break;
   }}
   else
    wcscpy(Znach,
     FormMag->ComboBoxMag->Items->Strings[FormMag->ComboBoxMag->ItemIndex].c_str());
   swprintf(FullHead,L"%s\n%s=%s",EditList->FH(),Cap,Znach);
   swprintf(Head[0],L"%s, %s=%s",EditList->H0(),Cap,Znach);
   swprintf(Help,L"%s",EditList->FileHlp()); Show0=EditList->show0();
   Podval=EditList->Podv();
   if(EditList->GetNSumm()){
    for(i=0,K=0;EditList->GetNSumm(i);i++)
     if(EditList->GetNSumm(i)!=(Ind+1)) K++;
    NSumm=(int *)calloc(K+1,SzI);
    for(i=0;i<K+1;i++)
     if(EditList->GetNSumm(i)>(Ind+1))
      NSumm[i-1]=EditList->GetNSumm(i)-1;
     else
      if(EditList->GetNSumm(i)<(Ind+1))
       NSumm[i]=EditList->GetNSumm(i);
   }
   else NSumm=NULL;
//найдем кнопку над выбранным столбцом и возьмем ее ширину
//   for(TKnopka *k=EditList->Shapk->GetR();k;k=k->Sled){
//    if((k->GetNn()==k->GetNk())&&(k->GetNn()==Ind)){
//     L=k->GetXk()-k->GetXn(); break;
//   }}
//для всех кнопок над выбранным столбцом уточним возможность их сжатия
//   for(TKnopka *k=EditList->Shapk->GetR();k;k=k->Sled){
//    if(k->GetNn()<=Ind||k->GetNk()>=Ind
//   }
//запишем простую шапку
   if(F=_wfopen(FileSh,L"w")){
    TKnopka *k=EditList->Shapk->GetR(); int K=k->GetYk()-k->GetYn()-1;
    wchar_t **Str=(wchar_t **)calloc(K,SzV),**str=(wchar_t **)calloc(K,SzV);
	wchar_t *ss;
    for(i=0;i<K;i++) str[i]=Str[i]=(wchar_t *)calloc(1024,SzC);
    for(;k;k=k->Sled){
     if(k->GetNn()==Ind) continue;
     wcscpy(s=Inf,k->GetB()->Caption.c_str());
     for(i=0,ss=str[0];*s;s++){
      switch(*s){
       case '\r':
        *ss++=k->Sled?'\xb3':'\n'; s++; str[i++]=ss; ss=str[i]; break;
         default: *ss++=ConvChrWINtoDOS(*s);
     }}
     *ss++=k->Sled?'\xb3':'\n'; s++; str[i++]=ss; ss=str[i];
    }
    for(i=0;i<K;i++) fputws(Str[i],F); fclose(F);
    for(i=0;i<K;i++) free(Str[i]); free(Str);
   }
   KF=EditList->GetKolFixStlb();
   if(Ind<KF) KF--;
   EditList=new TEditList((void **)Clip,MNN,false,NSumm,Show0,NULL,Format,FileSh,
    Head,Fprn,NULL,NULL,NULL,KF,-1,0,-1,-1,FullHead,Podval,0,Help,NULL,NULL);
   if(EditList->Diagnos()){
    Application->CreateForm(__classid(TFormEditList),&FormEditList);
    FormEditList->ShowModal();
   }
   if(MNN) free(MNN);
   if(Head[0]) free(Head[0]); if(Head) free(Head);
   if(Help) free(Help); if(NSumm) free(NSumm);
   delete EditList;
   EditList=EditListOld; StringGridEditList=ActivSG=ActivSGOld;
   for(c=Clip[0],i=0;c;i++,c=c->Sled){
    for(j=0,s=(wchar_t *)c->Str;j<l;j++) if(j!=Ind){
     wcscpy(Form,EditList->FormatCol(j));
     switch(Form[wcslen(Form)-1]){
      case 'c': s+=2*SzC; break;
      case 'd': s+=SzI;   break;
      case 'l': s+=SzL;   break;
      case 's': if(*(wchar_t **)s) free(*(wchar_t **)s); break;
      case 'f': s+=SzD;   break;
      case 'e': s+=SzD;   break;
   }}}
   for(c=Clip[0];C=c;){ c=C->Sled; free(C); } free(Clip);
  }
  FormMag->ComboBoxMag->Items->Clear(); DelAllMagazine(&PoleZnach);
}}
//---------------------------------------------------------------------------
void __fastcall TFormEditList::StringGridEditListDblClick(TObject *Sender){
 int C=((TStringGrid *)Sender)->Col,R=((TStringGrid *)Sender)->Row;
 if(EditList->CallCell) (*EditList->CallCell)(R,C);
 else if(TabControlEditList->Tabs->Count>1){
  TabControlEditList->TabIndex=1;
  TabControlEditListChange(Sender);
 }
 else if(EditList->GetSelect()) ToolButtonSelectClick(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TFormEditList::tbEnterTablClick(TObject *Sender){
 int C=ActivSG->Col,R=ActivSG->Row;
 if(EditList->CallCell) (*EditList->CallCell)(R,C);
}
//---------------------------------------------------------------------------
void __fastcall TFormEditList::ToolButtonSelectClick(TObject *Sender){
 if(EditList->TestField&&
    EditList->TestField(StringGridEditList,EditList->Row,EditList->Col+1,
     EditList->Value.c_str()))
//Вернуть старое, редактирование запрещено
  StringGridEditList->Cells[EditList->Col][EditList->Row]=EditList->Value;
 EditList->SelectMad=true;
 Close();
}
//---------------------------------------------------------------------------
void __fastcall TFormEditList::ToolButtonInsertClick(TObject *Sender){
 pmPasteClick(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TFormEditList::pmCopyClick(TObject *Sender){//скопировать
 TEdit *Edit=new TEdit(StringGridEditList); wchar_t *inf=Inf,Buf[256]; int i,j;
 TGridRect SelRect=StringGridEditList->Selection;
 Edit->Parent=StringGridEditList; Edit->Visible=false;
 if(SelRect.Left!=SelRect.Right||SelRect.Top!=SelRect.Bottom){
  for(i=SelRect.Top;i<=SelRect.Bottom;i++){
   for(j=SelRect.Left;j<=SelRect.Right;j++){
    CleanStr(StringGridEditList->Cells[j][i].c_str(),Buf);
    inf+=swprintf(inf,L"%s\t",Buf);
   }
   wcscpy(inf-1,L"\r\n"); inf++;
 }}
 else{
  j=StringGridEditList->Col; i=StringGridEditList->Row;
  CleanStr(StringGridEditList->Cells[j][i].c_str(),Inf);
 }
 Edit->Text=Inf;
 Edit->SelectAll();
 Edit->CopyToClipboard();
 delete Edit;
}
//---------------------------------------------------------------------------
void __fastcall TFormEditList::pmCutClick(TObject *Sender){//вырезать
 int i,j; TGridRect SelRect=StringGridEditList->Selection;
 pmCopyClick(Sender);
 if((SelRect.Left==EditList->GetKolFixStlb())&&
    (SelRect.Right==StringGridEditList->ColCount-1)){
  pmDelStrClick(Sender); return;
 }
 if((SelRect.Left!=SelRect.Right)||(SelRect.Top!=SelRect.Bottom)){
  for(i=SelRect.Top;i<=SelRect.Bottom;i++){
   for(j=SelRect.Left;j<=SelRect.Right;j++){
    EditList->Value=StringGridEditList->Cells[j][i];
    StringGridEditList->Cells[j][i]=L"";
    if(EditList->TestField&&
       EditList->TestField(StringGridEditList,i,j+1,EditList->Value.c_str()))
     StringGridEditList->Cells[j][i]=EditList->Value;
 }}}
 else{
  j=StringGridEditList->Col; i=StringGridEditList->Row;
  EditList->Value=StringGridEditList->Cells[j][i];
  StringGridEditList->Cells[j][i]=L"";
  if(EditList->TestField&&
     EditList->TestField(StringGridEditList,i,j+1,EditList->Value.c_str()))
   StringGridEditList->Cells[j][i]=EditList->Value;
}}
//---------------------------------------------------------------------------
void __fastcall TFormEditList::pmPasteClick(TObject *Sender){//вставить
 if(Clipboard()->HasFormat(CF_TEXT)){
//  bool(*T)(TStringGrid *,int,int,wchar_t *)=EditList->TestField;
  String S=Clipboard()->AsText; wchar_t Cell[256],*s=S.w_str(),*c;
  int Rows=KSimbStr(s,'\r'),Cols=(Rows>0?KSimbStr(s,'\t')/Rows:0)+1,k,i,j,
   Row=StringGridEditList->Row;
  if(!Rows) Rows=1;
  if(StringGridEditList->Row+Rows>StringGridEditList->RowCount)
   for(int i=StringGridEditList->Row+Rows-StringGridEditList->RowCount;i--;)
    pmAddStrClick(Sender);
  if(StringGridEditList->Col+Cols>StringGridEditList->ColCount){
   swprintf(Inf,L"Число столбцов вставляемого фрагмента таблицы (%d)\n"
    L"выходит за границы указанного места в принимаемой таблице\n"
    L"Вставка невозможна.",Rows);
   Application->MessageBox(Inf,L"Ошибка!",MB_OK|MB_ICONSTOP);
   return;
  }
  StringGridEditList->Row=Row;
  for(i=0;i<Rows;i++){
   for(j=0;j<Cols;j++){
    for(c=Cell;*s&&(*s!='\t')&&(*s!='\r');*c++=*s++); *c='\0'; s++;
    EditList->Value=
     StringGridEditList->Cells[StringGridEditList->Col+j][StringGridEditList->Row+i];
    StringGridEditList->Cells[StringGridEditList->Col+j][StringGridEditList->Row+i]=Cell;
    if(EditList->TestField&&
       EditList->TestField(StringGridEditList,
        StringGridEditList->Row+i,StringGridEditList->Col+j+1,
        EditList->Value.c_str()))
     StringGridEditList->Cells[StringGridEditList->Col+j][StringGridEditList->Row+i]=
      EditList->Value;
    if(*s=='\n') s++;
  }}
  EditList->Value=
   StringGridEditList->Cells[StringGridEditList->Col][StringGridEditList->Row];
}}
//---------------------------------------------------------------------------
void __fastcall TFormEditList::ToolButtonExelClick(TObject *Sender){
 wchar_t HtmFile[256]=L"EditList.htm",*s,Sep,CurrDir[256],Browser[256]; FILE *HTMFILE;
 int Cols,Rows,*Mkc/*,Rez*/; TKnopka ***Mk; //массив чисел ячеек в столбце и матрица кнопок
 String S; AnsiString AS; double a=0.5; S=a; Sep=S.w_str()[1];
 if(!(HTMFILE=_wfopen(HtmFile,L"w"))){
  swprintf(Inf,L"Нельзя открыть HTM-файл для передачи таблицы в Excel!");
  Application->MessageBox(Inf,L"Системная ошибка!",MB_OK|MB_ICONERROR);
 }
 else{
  fwprintf(HTMFILE,L"<html>\n<head>\n<title>Таблица EditList</title>\n"
   L"<meta http-equiv=\"Content-Type\" content=\"text/html;"
   L"wchar_tset=windows-1251\">\n</head>\n"
   L"<body bgcolor=\"#FFFFFF\" text=\"#800000\""
   L" background=\"file:///%s\\fon.jpg\""
   L" font face=\"Times New Roman, Times, serif\">\n",
	WORKDIR);
  fwprintf(HTMFILE,L"<TABLE BORDER=2>\n");
  if(EditList->FullHead&&EditList->Head&&EditList->Head[0])
   swprintf(Inf,L"%s %s",EditList->FullHead,EditList->Head[0]);
  else if(EditList->FullHead)
   swprintf(Inf,L"%s",EditList->FullHead);
  else if(EditList->Head&&EditList->Head[0])
   swprintf(Inf,L"%s",EditList->Head[0]);
  fwprintf(HTMFILE,L" <TR>\n");
  fwprintf(HTMFILE,L"  <TH ALIGN=center COLSPAN=%d>%s</TH>\n",
   StringGridEditList->ColCount,Inf);
  fwprintf(HTMFILE,L" </TR>\n");
  Cols=0;
  for(TKnopka *k=EditList->Shapk->GetR();k;k=k->Sled){
   if(k->GetNn()>Cols) Cols=k->GetNn();
   if(k->GetNk()>Cols) Cols=k->GetNk();
  }
  Mkc=(int *)calloc(Cols+1,SzI);
  for(TKnopka *k=EditList->Shapk->GetR();k;k=k->Sled){
   for(int i=k->GetNn();i<=k->GetNk();i++) Mkc[i]+=1;
  }
  Rows=0; for(int i=0;i<=Cols;i++) if(Mkc[i]>Rows) Rows=Mkc[i];
  Mk=(TKnopka ***)calloc(Rows,SzV);
  for(int i=0;i<Rows;i++) Mk[i]=(TKnopka **)calloc(Cols+1,SzV);
  for(TKnopka *k=EditList->Shapk->GetR();k;k=k->Sled)
   for(int j=k->GetNn();j<=k->GetNk();j++)
	for(int i=0;i<Rows;i++) if(!Mk[i][j]){ Mk[i][j]=k; break; }
  for(int i=0;i<Rows;i++){
   fwprintf(HTMFILE,L" <TR>\n");
   for(int j=0;j<=Cols;j++){ TKnopka *k=Mk[i][j];
	if(k){
	 fwprintf(HTMFILE,L"  <TH ALIGN=center COLSPAN=%d ROWSPAN=%d>%s</TH>\n",
	  k->GetNk()-k->GetNn()+1,Rows-Mkc[j]-i+1,k->GetB()->Caption.c_str());
	 j=k->GetNk();
   }}
   fwprintf(HTMFILE,L" </TR>\n");
  }
  for(int i=0;i<StringGridEditList->RowCount;i++){
   fwprintf(HTMFILE,L" <TR>\n");
   for(int j=0;j<StringGridEditList->ColCount;j++){
	if(Sep=='.')
	 fwprintf(HTMFILE,L"  <TD>%s</TD>\n",
	  StringGridEditList->Cells[j][i].IsEmpty()?L"&nbsp;":
	   StringGridEditList->Cells[j][i].c_str());
	else{
	 if(StringGridEditList->Cells[j][i].IsEmpty())
	  fputws(L"  <TD>&nbsp;</TD>\n",HTMFILE);
	 else{
	  swprintf(Inf,StringGridEditList->Cells[j][i].c_str());
	  for(s=Inf;*s&&(*s==' ');s++);
	  if(!*s) fputws(L"  <TD>&nbsp;</TD>\n",HTMFILE);
	  else{
//       if(((*s=='.')||(*s=='+')||(*s=='-')||isdigit(*s))&&
//           (s=wcschr(Inf,'.'))&&(KSimbStr(Inf,'.')==1))
	   if(Number(Inf)&&(s=wcschr(Inf,'.')))
		*s=',';
		fwprintf(HTMFILE,L"  <TD>%s</TD>\n",Inf);
   }}}}
   fwprintf(HTMFILE,L" </TR>\n");
  }
  fwprintf(HTMFILE,L"</TABLE>\n");
  fwprintf(HTMFILE,L"</body>\n</html>");
  fclose(HTMFILE);
 }
// swprintf(Inf,L"Iexplore.exe %s",HtmFile);
 _wgetcwd(CurrDir,255);
// swprintf(Inf,L"%s\\%s",CurrDir,HtmFile);
// FindExecutable(Inf,NULL,Browser);
 wcscpy(Browser,L"c:\\Program Files\\Internet Explorer\\Iexplore.exe");
 if(_waccess(Browser,0)){
  wcscpy(Inf,L"У Вас, по-видимому, не установлен Internet Explorer.\n"
   L"Поэтому передать в Excel таблицу не удастся!");
  Application->MessageBox(Inf,L"Ошибка!",MB_OK);
  return;
 }
 swprintf(Inf,L"%s %s\\%s",Browser,CurrDir,HtmFile);
 switch(AS=Inf,/*Rez=*/WinExec(AS.c_str(),SW_SHOW)){
  case 33:
   return;
  case 0: swprintf(Inf,L"The system is out of memory or resources."); break;
  case ERROR_BAD_FORMAT:
   swprintf(Inf,L"The .EXE file is invalid (non-Win32 .EXE or error in .EXE image).");
   break;
  case ERROR_FILE_NOT_FOUND:
   swprintf(Inf,L"The specified file was not found."); break;
  case ERROR_PATH_NOT_FOUND:
   swprintf(Inf,L"The specified path was not found."); break;
 }
 Application->MessageBox(Inf,L"Ошибка!",MB_OK);
// ShellExecute(NULL,L"open",HtmFile,NULL,NULL,SW_SHOW);
}
//---------------------------------------------------------------------------
void __fastcall TFormEditList::UpDownClick(TObject *Sender,TUDBtnType Button){
 if(UpDown->Position!=StringGridEditList->FixedCols){
  int Col=StringGridEditList->Col;
  StringGridEditList->Col=StringGridEditList->FixedCols;
  StringGridEditList->FixedCols=UpDown->Position;
  if(Col>=StringGridEditList->FixedCols) StringGridEditList->Col=Col;
}}
//---------------------------------------------------------------------------
void __fastcall TFormEditList::pmList5Click(TObject *Sender){
 if(EditList->Kp>4){
  TabControlEditList->TabIndex=5; TabControlEditListChange(Sender);
}}
//---------------------------------------------------------------------------
void __fastcall TFormEditList::FormShow(TObject *Sender){
 int L=0,i,T,H=0,dH,dL,SumWidthToolBut=0,W,
  SumWidthHeadTabs=
   EditList->Head?TabControlEditList->Canvas->TextWidth(EditList->Head[0]):0;
 TStringGrid *SG=StringGridEditList; HSg=0;
 if(!EditList->GetR()) EditList->SetKukla();
 if(!EditList->FileHelp||_waccess(EditList->FileHelp,0))
  ToolButtonHelp->Enabled=false;
 SumWidthToolBut+=ToolButtonQuit->Left+ToolButtonQuit->Width;
 for(i=1;i<EditList->Kp+1;i++)
  SumWidthHeadTabs+=TabControlEditList->Canvas->TextWidth(EditList->Head[i]);
 minWidth=max(11*SumWidthHeadTabs/10,SumWidthToolBut)+50;
 FormSizes(L,T,W,H);
 Left=EditList->LeftF>-1?EditList->LeftF:L;
 if(EditList->WidthF==-1) Width=W;//Screen->Width-Left;
 else if(!EditList->WidthF) Width=W-2*Left;//Screen->Width-2*Left;
 else Width=EditList->WidthF;
 Top=EditList->TopF==-1?T:EditList->TopF;
 if(EditList->HeightF==-1) Height=H-Top;
 else if(!EditList->HeightF){
  Height=H-2*Top;
  if((Top+Height)>H)
   Height=H-Top;
 }
 else Height=EditList->HeightF;
 SG->RowCount=0; SG->ColCount=0;
 EditList->SetTabs(TabControlEditList);
 TabControlEditList->TabIndex=0;
 TabControlEditListChange(Sender);
 if(!EditList->Save){ ToolButtonSave->Enabled=false; pmSave->Visible=false; }
 HSh=EditList->Shapk->SetHeightShapka();
 Hi=ToolBarEditList->Height; Hht=24;
 Hfh=EditList->FullHead?ButtonFullHead->Height:0;
 Hhc=EditList->GetNSumm()?HeaderControlEditList->Height:0;
 Hp=EditList->Podval?ButtonPodval->Height:0;
 Hsb=StatusBarEditList->Height;
 TabControlEditList->Align=alNone;
 TabControlEditList->Top=Hi+Hfh;
 TabControlEditList->Height=ClientHeight-Hi-Hfh-Hp-Hsb;
 TabControlEditList->Width=ClientWidth;
 SG->Align=alNone; SG->Top=HSh+5*TabControlEditList->Font->Size/2;
 SG->Height=TabControlEditList->ClientHeight-SG->Top-Hhc;
 SG->Width=TabControlEditList->ClientWidth;
 HeaderControlEditList->Align=alBottom;
 UpDown->Max=SG->VisibleColCount+SG->FixedCols-1; UpDown->Position=SG->FixedCols;
 if((SG->VisibleColCount+SG->FixedCols)==SG->ColCount){
  for(i=0,L=SG->GridLineWidth;i<SG->ColCount;i++)
   L+=SG->ColWidths[i]+SG->GridLineWidth;
  dL=SG->ClientWidth-L; SG->Width-=dL;
  TabControlEditList->Width-=dL; Width-=dL;
  if(TabControlEditList->Width<minWidth){
   dL=minWidth-TabControlEditList->Width;
   TabControlEditList->Width+=dL; Width+=dL;
   Width+=TabControlEditList->Width/10;
   TabControlEditList->Width=ClientWidth;
  }
//  Left=EditList->LeftF>-1?EditList->LeftF:(W-Width)/2;//(Screen->Width-Width)/2;
//  if((Left+Width)>W) Left=W-Width;
 }
 if((SG->VisibleRowCount+SG->FixedRows)==SG->RowCount){
  for(i=0,H=SG->GridLineWidth+1;i<max(3,SG->RowCount);i++)
   H+=SG->RowHeights[i]+SG->GridLineWidth+1;
  dH=SG->ClientHeight-H;
  SG->Height-=dH;
  TabControlEditList->Height-=dH;
  Height-=dH;
 }
 if(EditList->FullHead){
  ButtonFullHead->Left=0; ButtonFullHead->Top=Hi;
  ButtonFullHead->Width=ClientWidth;
 }
 if(EditList->Podval){
  ButtonPodval->Left=0; ButtonPodval->Top=ClientHeight-Hp-Hsb;
  ButtonPodval->Width=ClientWidth;
 }
 EditList->LeftF=Left; EditList->TopF=Top;
 EditList->WidthF=Width; EditList->HeightF=Height;
 StatusBarEditList->Panels->Items[2]->Text=L"";
 if(EditList->CallCell) tbEnterTabl->Visible=true;
 SetFocus();
 HWND hwnd=Handle;
 BringWindowToTop(hwnd);
 HSg=SG->Height;
}
//---------------------------------------------------------------------------
void __fastcall TFormEditList::FormResize(TObject *Sender){
 if(HSg){
  int L=0,i,T,H=0,dH,dL; TStringGrid *SG=StringGridEditList;
  FormEditList->OnResize=NULL;
  EditList->LeftF=Left; EditList->TopF=Top;
  EditList->WidthF=Width; EditList->HeightF=Height;
  TabControlEditList->Width=ClientWidth;
  SG->Width=TabControlEditList->ClientWidth;
  if((SG->VisibleColCount+SG->FixedCols)==SG->ColCount){
   for(i=0,L=SG->GridLineWidth;i<SG->ColCount;i++)
    L+=SG->ColWidths[i]+SG->GridLineWidth;
   if(SG->ClientWidth>L){
    dL=SG->ClientWidth-L; SG->Width-=dL;
  }}
  TabControlEditList->Height=ClientHeight-Hi-Hfh-Hp-Hsb;
  SG->Height=TabControlEditList->ClientHeight-SG->Top-Hhc;
  if((SG->VisibleRowCount+SG->FixedRows)==SG->RowCount){
   for(i=0,H=SG->GridLineWidth+1;i<max(3,SG->RowCount);i++)
    H+=SG->RowHeights[i]+SG->GridLineWidth+1;
   if(SG->ClientHeight>H){
    dH=SG->ClientHeight-H; SG->Height-=dH;
    TabControlEditList->Height-=dH; Height-=dH;
  }}
  if(EditList->FullHead) ButtonFullHead->Width=ClientWidth;
  if(EditList->Podval) ButtonPodval->Width=ClientWidth;
  UpDown->Max=SG->VisibleColCount+SG->FixedCols-1;
  UpDown->Position=SG->FixedCols;
  Left=EditList->LeftF; Top=EditList->TopF;
  FormEditList->OnResize=FormResize;
}}
//---------------------------------------------------------------------------
void __fastcall TFormEditList::StringGridEditListDrawCell(TObject *Sender,
      int ACol, int ARow, TRect &Rect, TGridDrawState State){
 if(GREK){
  TStringGrid *SG=(TStringGrid *)Sender; wchar_t S[256],*s=S;
  wcscpy(S,SG->Cells[ACol][ARow].c_str());
  while(*s==' ') s++;
  if((*s=='.')||(*s=='+')||(*s=='-')||isdigit(*s)) return;
  SG->Canvas->Rectangle(Rect);
  DrawGrekText(SG->Canvas,Rect.Left,Rect.Top,0,s,'$');
}}
//---------------------------------------------------------------------------

