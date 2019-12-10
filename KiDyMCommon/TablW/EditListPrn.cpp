//---------------------------------------------------------------------------

#include <vcl.h>
#include <io.h>
#include <stdio.h>
#include "EditList.h"
#include <string.h>
#include "SzFormat.h"
#include "EditListPrn.h"
#include "OtchetForm.h"
#include "ParamPrint.h"
#include "ConvDW.h"

#pragma hdrstop

#define ONE   1
#define SMALL 2
#define LARGE 3
#define HUGE  4

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormEditListPrn *FormEditListPrn;
extern TFormToPrn *FormToPrn;
extern TFormEditList *FormEditList;
extern wchar_t Inf[],WORKDIR[];
extern TEditList *EditList;
extern TFormOtchet *FormOtchet;
extern TFormParamPrint *FormParamPrint;
//---------------------------------------------------------------------------
__fastcall TFormEditListPrn::TFormEditListPrn(TComponent* Owner):TForm(Owner){
 wchar_t *s; ImagePage=NULL; VidTablPrn=0; Y0=0;
 wcscpy(FileHelp,EditList->FileHelp);
 if(s=wcsrchr(FileHelp,'\\')) wcscpy(s+1,L"EditlistPrn.htm");
 else wcscpy(FileHelp,L"EditlistPrn.htm");
}
//---------------------------------------------------------------------------
void __fastcall TFormEditListPrn::ToolButtonHelpClick(TObject *Sender){
 if(_waccess(FileHelp,0)){
  swprintf(Inf,L"Не найден файл помощи\n\"%s\"",FileHelp);
  Application->MessageBox(Inf,L"Системная ошибка!",MB_OK);
  return;
 }
 if(!FormOtchet)
  Application->CreateForm(__classid(TFormOtchet),&FormOtchet);
 FormOtchet->cbURL->Text=FileHelp;
 FormOtchet->ShowModal();
}
//---------------------------------------------------------------------------
void __fastcall TFormEditListPrn::ToolButtonExitClick(TObject *Sender){
 Close();
}
//---------------------------------------------------------------------------
void  __fastcall TFormEditListPrn::DrawFullHead(TCanvas *Canvas){
 int L,X; wchar_t *h=EditList->FullHead,*n;
 if(!EditList->FullHead) return;
 for(int i=0;i<Knfh;i++){
  n=wcschr(h,'\n'); *n='\0';
  L=Canvas->TextWidth(h); X=(Round(lp*Kx)-L)/2;
  if(X<0) X=0;
  Canvas->TextOut(X+Round(MarginLeft*Kx),Round(Y*Ky),h);
  Y+=H;
  *n++='\n'; h=n;
 }
 L=Canvas->TextWidth(h); X=(Round(lp*Kx)-L)/2;
 if(X<0) X=0;
 Canvas->TextOut(X+Round(MarginLeft*Kx),Round(Y*Ky),h);
 Y+=H;
}
//---------------------------------------------------------------------------
void  __fastcall TFormEditListPrn::DrawHead(TCanvas *Canvas,int W){
 int L,X; wchar_t *h,*n;
 if(!EditList->Head) return;
 h=EditList->Head[0];
 for(int i=0;i<Knh;i++){
  n=wcschr(h,'\n'); *n='\0';
  CleanStr(h,Inf);
  L=Canvas->TextWidth(Inf); X=(Round((W?W:lp)*Kx)-L)/2;
  if(X<0) X=0;
  Canvas->TextOut(X+Round(MarginLeft*Kx),Round(Y*Ky),Inf);
  Y+=H;
  *n++='\n'; h=n;
 }
 CleanStr(h,Inf);
 L=Canvas->TextWidth(Inf); X=(Round((W?W:lp)*Kx)-L)/2;
 if(X<0) X=0;
 Canvas->TextOut(X+Round(MarginLeft*Kx),Round(Y*Ky),Inf);
 Y+=H;
}
//---------------------------------------------------------------------------
void __fastcall TFormEditListPrn::DrawOnePage(TCanvas *Canvas){
 int NN=0,NK=FormEditList->StringGridEditList->ColCount-1,
  /*L=Round(10.0*Canvas->Font->PixelsPerInch/254),*/LTab=L; Magazine *m;
 Canvas->Font->Style=TFontStyles()<<fsBold;
 DrawFullHead(Canvas);
 DrawHead(Canvas,0);
 Y+=H;
 for(int i=0;i<NK+1;i++){
  swprintf(Inf,L"%d",i); if(FindMagazine(MStlb,Inf,&m)) LTab+=Li[i]+L;
 }
 for(int i=0;i<NK+1;i++){
  swprintf(Inf,L"%d",i);
  if(FindMagazine(MStlb,Inf,&m)) Li[i]+=(lp-LTab)/ColCount;
 }
 EditList->Shapk->DrawShapka(Canvas,L,H,MarginLeft,Y,Li,NN,NK,Kx,Ky,MStlb);
 Y+=H/5;
 Canvas->Font->Style=TFontStyles()>>fsBold;
 FormEditList->DrawStringGrid(Canvas,L,H,MarginLeft,Y,Li,NN,NK,0,
  FormEditList->StringGridEditList->RowCount-1,Kx,Ky,MStlb);
 Canvas->Font->Style=TFontStyles()<<fsBold;
 FormEditList->DrawHeaderControl(Canvas,L,H,MarginLeft,Y,Li,NN,NK,Kx,Ky,MStlb);
 Y+=H;
 if(EditList->Podval&&EditList->Podval[0])
  EditList->DrawPodval(Knp,Canvas,H,MarginLeft,MarginTop,Hp,hp,Y,Kx,Ky);
 Canvas->Font->Style=TFontStyles()>>fsBold;
}
//---------------------------------------------------------------------------
void __fastcall TFormEditListPrn::DrawManyPage(TCanvas *Canvas,int Np){
 int i,NN=0,NK=FormEditList->StringGridEditList->ColCount-1,LTab=L,Kss,Kp,Kps;
 Magazine *m;
 for(i=0;i<NK+1;i++){
  swprintf(Inf,L"%d",i); if(FindMagazine(MStlb,Inf,&m)) LTab+=Li[i]+L;
 }
 for(i=0;i<NK+1;i++){
  swprintf(Inf,L"%d",i);
  if(FindMagazine(MStlb,Inf,&m)) Li[i]+=(lp-LTab)/ColCount;
 }
 Kps=(hp-Hfh-Hh-H-HSh)/H;
 if(!Np){//рисуем первую страницу
  Canvas->Font->Style=TFontStyles()<<fsBold;
  DrawFullHead(Canvas);
  DrawHead(Canvas,0);
  Y+=H;
  EditList->Shapk->DrawShapka(Canvas,L,H,MarginLeft,Y,Li,NN,NK,Kx,Ky,MStlb);
  Canvas->Font->Style=TFontStyles()>>fsBold;
  Y+=H/5;
  FormEditList->DrawStringGrid(Canvas,L,H,MarginLeft,Y,Li,NN,NK,0,Kps-1,Kx,Ky,
   MStlb);
  return;
 }
//найдем число оставшихся полных страниц
 Kss=(hp-Hh-H-HSh)/H;
 Kp=(FormEditList->StringGridEditList->RowCount-Kps)/Kss;
 for(i=1;i<Kp+1;i++){
  if(i==Np){
   Canvas->Font->Style=TFontStyles()<<fsBold;
   DrawHead(Canvas,0);
   Canvas->Font->Style=TFontStyles()>>fsBold;
   swprintf(Inf,L"Продолжение. Стр.%d",i+1);
   Canvas->TextOut(Round(Kx*MarginLeft),Round(Ky*Y),Inf);
   Y+=H;
   Canvas->Font->Style=TFontStyles()<<fsBold;
   EditList->Shapk->DrawShapka(Canvas,L,H,MarginLeft,Y,Li,NN,NK,Kx,Ky,MStlb);
   Canvas->Font->Style=TFontStyles()>>fsBold;
   Y+=H/5;
   FormEditList->DrawStringGrid(Canvas,L,H,MarginLeft,Y,Li,NN,NK,Kps,Kps+Kss-1,
    Kx,Ky,MStlb);
   return;
  }
  Kps+=Kss;
 }
 if(i==Np){
  if(FormEditList->StringGridEditList->RowCount>Kps){
   Canvas->Font->Style=TFontStyles()<<fsBold;
   DrawHead(Canvas,0);
   Canvas->Font->Style=TFontStyles()>>fsBold;
   swprintf(Inf,L"Продолжение. Стр.%d",i+1);
   Canvas->TextOut(Round(Kx*MarginLeft),Round(Ky*Y),Inf);
   Y+=H;
   Canvas->Font->Style=TFontStyles()<<fsBold;
   EditList->Shapk->DrawShapka(Canvas,L,H,MarginLeft,Y,Li,NN,NK,Kx,Ky,MStlb);
   Canvas->Font->Style=TFontStyles()>>fsBold;
   Y+=H/5;
   FormEditList->DrawStringGrid(Canvas,L,H,MarginLeft,Y,Li,NN,NK,Kps,Kps+Kss-1,
    Kx,Ky,MStlb);
  }
  Canvas->Font->Style=TFontStyles()<<fsBold;
  FormEditList->DrawHeaderControl(Canvas,L,H,MarginLeft,Y,Li,NN,NK,Kx,Ky,MStlb);
  Y+=H;
  if(EditList->Podval&&EditList->Podval[0])
   EditList->DrawPodval(Knp,Canvas,H,MarginLeft,MarginTop,Hp,hp,Y,Kx,Ky);
  Canvas->Font->Style=TFontStyles()>>fsBold;
}}
//---------------------------------------------------------------------------
int __fastcall TFormEditListPrn::Mashtabs(int Hchp){
//возвращаем высоту окна, входной параметр - высота строки в принтере
//в пикселях
 int /*Hpagemm,*/Wpagemm,Himp,Wimp,dH,dW,H,W,Hmax,Wmax,Dl,Dls,l,L=0,Dl0,Size;
// wchar_t TestStr[]=L"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
 wchar_t TestStr[256],Cell[256];
 for(int i=0;i<FormEditList->StringGridEditList->RowCount;i++){
//  for(int j=0;j<FormEditList->StringGridEditList->ColCount;j++){
  for(Magazine *m=MStlb;m;m=m->Sled){ int j=_wtoi(m->S);
   wcscpy(Inf,FormEditList->StringGridEditList->Cells[j][i].c_str());
   CleanStr(Inf,Cell); l=Round(Prn->Canvas->TextWidth(Cell));
   if(l>L){ L=l; wcscpy(TestStr,Cell); }
 }}
 Wpagemm=GetDeviceCaps(Prn->Handle,HORZSIZE);//ширина и
// Hpagemm=GetDeviceCaps(Prn->Handle,VERTSIZE);//высота страницы в мм
 LPxscreen=GetDeviceCaps(Canvas->Handle,LOGPIXELSX);//разрешение экрана
 LPyscreen=GetDeviceCaps(Canvas->Handle,LOGPIXELSY);//в пикселах на L"
// LPx=GetDeviceCaps(Prn->Handle,LOGPIXELSX);//разрешение принтера
// LPy=GetDeviceCaps(Prn->Handle,LOGPIXELSY);//в пикселах на L"
 Canvas->Font->Name=Prn->Canvas->Font->Name;
 Canvas->Font->Size=Round((double)LPx*Prn->Canvas->Font->Size/LPxscreen);
// Canvas->Font->Size=12; Size=Prn->Canvas->Font->Size;
// Prn->Canvas->Font->Size=12; //Prn->Canvas->Font->Name=L"Courier New";
 Dl0=Canvas->TextWidth(TestStr);
 Dl=Prn->Canvas->TextWidth(TestStr);
 Ktw=(double)Dl0/Dl;
// Prn->Canvas->Font->Name=Canvas->Font->Name;
// Prn->Canvas->Font->Size=Size;
 Dl=Round(Ktw*Prn->Canvas->TextWidth(TestStr));
 Canvas->Font->Size=Prn->Canvas->Font->Size;
 if(Wpagemm>210){
  if(Wpagemm>310) Canvas->Font->Size=Round(Prn->Canvas->Font->Size/2);
  else Canvas->Font->Size=Round(3*Prn->Canvas->Font->Size/4);
 }
 Dls=Canvas->TextWidth(TestStr);
 Kx=(double)Dls/Dl;
 Ky=(double)Canvas->Font->Size*LPyscreen/Prn->Canvas->Font->Size/LPy;
 TabControlPages->Align=alClient;
// TabControlPages->Align=alNone;
// TabControlPages->ClientWidth=Round(Kx*PhW);
 ScrollBox->Align=alClient;
 dW=Width-ScrollBox->ClientWidth;
 if(Round(Kx*PhW)+dW>ClientWidth){
  Width+=Round(Kx*PhW)+dW-ClientWidth;
  if(Width>Screen->Width) Width=Screen->Width;
 }
 else Width=Round(Kx*PhW)+dW;
 Left=(Screen->Width-Width)/2;
// dH=Height-TabControlPages->ClientHeight;
// TabControlPages->ClientHeight=Round(Ky*PhH);
 dH=Height-ScrollBox->ClientHeight;
// TabControlPages->ClientHeight=Round(Ky*PhH);
// Height=TabControlPages->ClientHeight+dH;
 if(Round(Ky*PhH)+dH>ClientHeight){
  Height+=Round(Ky*PhH)+dH-ClientHeight;
  if(Height>(Screen->Height-31)){
   Top=0; Height=Screen->Height-31; Width+=8;
 }}
 else Top=(Screen->Height-31-Height)/2;
 Top=(Screen->Height-Height-31)/2;
// if(TabControlPages->Height<ClientHeight)
//  ClientWidth=TabControlPages->Width;
 Mp=ClientWidth/PhW;
 return ClientHeight;
}
//---------------------------------------------------------------------------
int __fastcall TFormEditListPrn::CountPagesBroadTabl(int Hp,int HIt){
 int i,j,Kt1,Kt2,Ktn,Dlp,Yn,Xsh[100],Kp; Magazine *m;
//Kt1,Kt2,Ktn,Kp,KP - соотв. число порций таблицы на первой,
//средней и последней страницах; общее число порций и общее
//число страниц печати
//Определим общее число порций Kp и заполним массивы номеров
//начальных и конечных столбцов порций
//Строка итогов имеет высоту HIt=H
 if(!Ht) return KP=1;//!!!
 Dlp=Lfs;//длина порции: неподвижные + переменные
 Kp=0; Xsh[0]=0;
 for(i=Kfs;i<FormEditList->StringGridEditList->ColCount;i++){
  swprintf(Inf,L"%d",i); if(!FindMagazine(MStlb,Inf,&m)) continue;
  if(Kfs&&!Kp){
   if((Lfs+Li[i]+L)>lp) {
    swprintf(Inf,L"Ширина %d-го столбца вместе с непрокручиваем%s столбц%s\n"
                  L"превосходит зону печати! ",i+1,Kfs>1?"ыми":"ым",
                  Kfs>1?"ами":"ом");
    Application->MessageBox(Inf,L"Печать невозможна!",MB_OK|MB_ICONWARNING);
    return 0;
  }}
  else{
   if((Li[i]+2*L)>lp) {
    swprintf(Inf,L"Ширина %d-го столбца превосходит зону печати! ",i+1);
    Application->MessageBox(Inf,L"Печать невозможна!",MB_OK|MB_ICONWARNING);
    return 0;
  }}
  if((Dlp+Li[i]+L)>lp){
   Kp++; Xsh[Kp]=Xsh[Kp-1]+Dlp-Lfs; Dlp=Lfs;
  }
  Dlp+=Li[i]+L;
 }
 Kp++; Xsh[Kp]=Xsh[Kp-1]+Dlp-Lfs;
 Kt1=(hp-Hfh-Hp-Hh-H)/(HSh+Ht+HIt+H+H/5);
 Kt2=(hp-H-Hh)/(HSh+Ht+HIt+H+H/5);
 Ktn=(hp-Hp-H-Hh)/(HSh+Ht+HIt+H+H/5);
 if(Kt1>=Kp) KP=1;
 else {
  Kt1=(hp-Hfh-Hh)/(HSh+Ht+H+1);
  if((Kt1+Ktn)>=Kp) KP=2;
  else {
   KP=2+(Kp-Kt1-Ktn)/Kt2; if((Kp-Kt1-Ktn)%Kt2) KP++;
 }}
 return KP;
}
//---------------------------------------------------------------------------
void __fastcall TFormEditListPrn::CountPagesHugeTabl(){
 int i,Ks1,Ks2,Ksn,Kr,KSh; Magazine *m;
//Ks1,Ks2,Ksn,Kr - соотв. число строк Stringgrid на первой,
//средней и последней страницах; общее число одностраничных
//широких полос
//Определим общее число одностраничных широких полос Kr
 KSh=HSh/H; if(HSh%H) KSh++;
 Ks1=Ks-(Knfh+1)-(Knh+1)-KSh-2;
 Ks2=Ks-3-(Knh+1)-KSh;
 Ksn=(FormEditList->StringGridEditList->RowCount-Ks1)%Ks2;
 Kr=2+(FormEditList->StringGridEditList->RowCount-Ks1-Ksn)/Ks2;
// if(!Ksn){ Ksn=Ks2; Kr--; }//!!!
//Первая полоса
 Ht=H*Ks1;
 Kp=CountPagesBroadTabl(0,0);
 if(!Kp)
  return;
//средние полосы
 Ht=H*Ks2;
 for(i=0;i<Kr-2;i++) Kp+=CountPagesBroadTabl(0,0);
//последняя полоса с подвалом
 Ht=H*Ksn;
 Kp+=CountPagesBroadTabl(Hp,H);
}
//---------------------------------------------------------------------------
void __fastcall TFormEditListPrn::FormShow(TObject *Sender){
//Подготовка постраничной печати
//число строк на средней и первой стр., высота окна
 wchar_t PrnName[256],Port[256],*s; //DEVMODE DevMode;
 Hh=Hfh=Hp=Knfh=Knh=Knp=0;
//прочитаем опции печати
 FormParamPrint->FilePrn=EditList->FilePrn;
 wcscpy(FormParamPrint->FontName,L"Courier New");
 FormParamPrint->FontSize=10;
 FormParamPrint->OrientKnig=true;
 FormParamPrint->ReadOptPrn();
 if(FormParamPrint->Prn) Prn=FormParamPrint->Prn;
 else Prn=Printer();
 Kfs=EditList->GetKolFixStlb();
 Prn->Title=EditList->FilePrn;
 Prn->Canvas->Font->Name= FormParamPrint->FontName;
 Prn->Canvas->Font->Size=FormParamPrint->FontSize;
 Prn->Canvas->Font->Color=FormParamPrint->FontColor;
 if(FormParamPrint->IsSetOrient(Prn))
  Prn->Orientation=FormParamPrint->OrientKnig?poPortrait:poLandscape;
 LPx=GetDeviceCaps(Prn->Handle,LOGPIXELSX);
 LPy=GetDeviceCaps(Prn->Handle,LOGPIXELSY);
 PhW=GetDeviceCaps(Prn->Handle,PHYSICALWIDTH);
 PhH=GetDeviceCaps(Prn->Handle,PHYSICALHEIGHT);
 Wmm=GetDeviceCaps(Prn->Handle,HORZSIZE);
 Hmm=GetDeviceCaps(Prn->Handle,VERTSIZE);
 PhL=GetDeviceCaps(Prn->Handle,PHYSICALOFFSETX);
 PhT=GetDeviceCaps(Prn->Handle,PHYSICALOFFSETY);
 PhWmm=Round(PhW*25.4/LPx); PhHmm=Round(PhH*25.4/LPy);
 PhTmm=Round(PhT*25.4/LPy); PhBmm=PhHmm-PhTmm-Hmm;
 PhLmm=Round(PhL*25.4/LPx); PhRmm=PhWmm-PhLmm-Wmm;
 PhB=Round((double)PhBmm*LPy/25.4);
 PhR=Round((double)PhRmm*LPx/25.4);
 StatusBar->Canvas->Font->Assign(Prn->Canvas->Font);
 StatusBar->Panels->Items[0]->Text=Prn->Printers->Strings[Prn->PrinterIndex];
 swprintf(Inf,L"%0.0fx%0.0f мм",
  Prn->PageHeight*25.4/LPy,Prn->PageWidth*25.4/LPx);
 StatusBar->Panels->Items[1]->Text=Inf;
 StatusBar->Panels->Items[2]->Text=
  FormParamPrint->IsSetOrient(Prn)?Prn->Orientation==poPortrait?
   L"Книжная":L"Альбомная":L"??????";
 StatusBar->Height=-2*StatusBar->Canvas->Font->Height;
 StatusBar->Panels->Items[3]->Text=StatusBar->Canvas->Font->Name;
 StatusBar->Panels->Items[4]->Text=StatusBar->Canvas->Font->Size;
 for(int i=0;i<4;i++){
  StatusBar->Panels->Items[i]->Width=
   12*StatusBar->Canvas->TextWidth(StatusBar->Panels->Items[i]->Text)/10;
 }
 MarginLeft =Round((double)(FormParamPrint->MarginLeftmm*LPx)/25.4);
 MarginRight=Round((double)(FormParamPrint->MarginRightmm*LPx)/25.4);
 MarginTop  =Round((double)(FormParamPrint->MarginTopmm*LPy)/25.4);
 MarginBot  =Round((double)(FormParamPrint->MarginBotmm*LPy)/25.4);
 TabControlPages->Tabs->Clear();
 TabControlPages->Tabs->Add(L"стр.1");
 ColCount=0; DelAllMagazine(&MStlb);
 for(TKnopka *K=EditList->Shapk->GetR();K;K=K->Sled){
  if(K->GetB()->Down){
//   if(K->GetNn()>ColCount) ColCount=K->GetNn(); 24/03/2013
   swprintf(Inf,L"%d",K->GetNn()); TakeMagazine(&MStlb,Inf);
 }}
 if(!ColCount){
  ColCount=FormEditList->StringGridEditList->ColCount;
  for(int i=0;i<ColCount;i++){ swprintf(Inf,L"%d",i); TakeMagazine(&MStlb,Inf); }
 }
 else ColCount++;
//массив ширин столбцов в пикселях
 if(Li) free(Li); if(LiDo) free(LiDo); if(LiPo) free(LiPo);
 Li=(int *)calloc(ColCount,SzI); LiDo=(int *)calloc(ColCount,SzI);
 LiPo=(int *)calloc(ColCount,SzI);
//высоту по формуле для шрифта увелич.на 20%, т.е 12*(Size*LPy/72)/10
// H=Prn->Canvas->Font->Size*LPy/60+1;
 H=Round(FormParamPrint->Interval*LPy/6);
 lp=Round(Prn->PageWidth*0.9827)-MarginLeft-MarginRight+PhL+PhR;
 hp=Prn->PageHeight-MarginTop-MarginBot+PhT+PhB;
 Ks=hp/H;
 Hsp=Mashtabs(H);
//ширину уменьш.в 1,5,т.е.2*(Size*LPx/72)/3
// L=Prn->Canvas->Font->Size*LPx/108;
 L=Prn->Canvas->Font->Size*LPx/120; Lt=0;
//Посчитаем метрику таблицы
 EditList->DefL(Prn,L,Lfs,Li,LiDo,LiPo,Lt,Ktw,MStlb);
 HSh=EditList->Shapk->SetHeightShapkaPrn(H);
 Ht=FormEditList->StringGridEditList->RowCount*H;
 if(EditList->Head&&EditList->Head[0]){
  Knh=KSimbStr(EditList->Head[0],'\n'); Hh=(Knh+1)*H;
 }
 if(EditList->FullHead&&EditList->FullHead[0]){
  Knfh=KSimbStr(EditList->FullHead,'\n'); Hfh=(Knfh+1)*H;
 }
 if(EditList->Podval&&EditList->Podval[0]){
  Knp=KSimbStr(EditList->Podval,'\n'); Hp=(Knp+1)*H;
 }
 if(lp>=(Lfs+Lt)){
  if(hp>=(Hh+Hfh+HSh+Ht+Hp+2*H)){
   VidTablPrn=ONE; Kp=1;
  }
  else{//Таблица узкая
//Если более одной, но по ширине помещается на стр.
   VidTablPrn=SMALL;
   Kps=(hp-Hfh-Hh-H-HSh)/H; Kss=(hp-Hh-H-HSh)/H;
   Kp=1+(FormEditList->StringGridEditList->RowCount-Kps)/Kss;
   if((FormEditList->StringGridEditList->RowCount-Kps)%Kss) Kp++;
 }}
 else {//Таблица широкая
  if(Lfs>lp) {
   swprintf(Inf,L"Ширина непрокручиваем%s столбц%s\n"
    L"превосходит зону печати! ",Kfs>1?"ых":"ого",Kfs>1?"ов":"а");
   Application->MessageBox(Inf,L"Печать невозможна!",
    MB_OK|MB_ICONWARNING);
   return;
  }
  if(hp>=(Hh+Hfh+HSh+Ht+2*H)){
//По высоте одна стр.
   VidTablPrn=LARGE;
   Kp=CountPagesBroadTabl(Hp,H);
  }
  else{
// в одну страницу не помещается и широкая(огромная таблица)
   VidTablPrn=HUGE;
   CountPagesHugeTabl();
 }}
 for(int i=1;i<Kp;i++){
  swprintf(Inf,L"стр.%d",i+1); TabControlPages->Tabs->Add(Inf);
 }
 TabControlPagesChange(Sender); 
}
//---------------------------------------------------------------------------
int __fastcall TFormEditListPrn::DrawBroadTabl(TCanvas *Canvas,int Hfh,
   int Hp,int Nsn,int Nsk,int HIt,int NStartPage,int Np){
//NStartPage - абсолютный номер первой страницы в полосе
//Np - номер страницы, кот. надо вывести на канвас
 int i,j,NN,NK,Kt1,Kp,Kt2,Ktn,KP,nn[200],nk[200],Dlp,Yn,Xsh[200],NPg=0;
 Magazine *m;
 if(!Ht){//!!!
  if(Np==(TabControlPages->Tabs->Count-1)){
   Y+=H;
   if(Hp&&EditList->Podval&&EditList->Podval[0]){
    Canvas->Font->Style=TFontStyles()<<fsBold;
    EditList->DrawPodval(Knp,Canvas,H,MarginLeft,MarginTop,Hp,hp,Y,Kx,Ky);
    Canvas->Font->Style=TFontStyles()>>fsBold;
  }}
  return NPg;
 }
//Kt1,Kt2,Ktn,Kp,KP - соотв. число порций таблицы на первой,
//средней и последней страницах; общее число порций и общее
//число страниц вывода на экран
//Определим общее число порций Kp и заполним массивы номеров
//начальных и конечных столбцов порций
 Dlp=Lfs;//длина порции: неподвижные + переменные
 Kp=0; nn[0]=Kfs; Xsh[0]=0;
 for(i=Kfs;i<FormEditList->StringGridEditList->ColCount;i++){
  swprintf(Inf,L"%d",i); if(!FindMagazine(MStlb,Inf,&m)) continue;
  if(Kfs&&!Np){
   if((Lfs+Li[i]+L)>lp) {
    swprintf(Inf,L"Ширина %d-го столбца вместе с непрокручиваем%s столбц%s\n"
                  L"превосходит зону печати! ",i+1,Kfs>1?"ыми":"ым",
                  Kfs>1?"ами":"ом");
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
 nk[Kp]=i-1; Kp++; nk[Kp]=nn[Kp]=0;
 Xsh[Kp]=Xsh[Kp-1]+Dlp-Lfs;
 Kt1=(hp-Hfh-Hp-Hh-H)/(HSh+Ht+HIt+H+2*(H/5));
 Kt2=(hp-H-Hh)/(HSh+Ht+HIt+H+2*(H/5));
 Ktn=(hp-Hp-H-Hh)/(HSh+Ht+HIt+H+2*(H/5));
 if(Kt1>=Kp) KP=1;
 else {
  Kt1=(hp-Hfh-Hh)/(HSh+Ht+H+2*(H/5));
  if((Kt1+Ktn)>=Kp) KP=2;
  else {
   KP=2+(Kp-Kt1-Ktn)/Kt2;
   if((Kp-Kt1-Ktn)%Kt2) KP++;
 }}
 //Первый лист
 NPg++;
 if(Np==NStartPage){
  if(Kt1){
   Canvas->Font->Style=TFontStyles()<<fsBold;
   if(!Np) DrawFullHead(Canvas);
   DrawHead(Canvas,0);
   Canvas->Font->Style=TFontStyles()>>fsBold;
   if(Np){
    swprintf(Inf,L"Продолжение. Стр.%d",NStartPage+1);
    Canvas->TextOut(Round(Kx*MarginLeft),Round(Ky*Y),Inf);
   }
   Y+=H;
  }
  for(i=0;i<min(Kt1,Kp);i++){
   NN=nn[i]; NK=nk[i];
   Yn=Y;
   Canvas->Font->Style=TFontStyles()<<fsBold;
   EditList->Shapk->DrawShapka(Canvas,L,H,MarginLeft,Y,Li,0,Kfs-1,Kx,Ky,MStlb);
   Canvas->Font->Style=TFontStyles()>>fsBold;
   Y+=H/5;
   FormEditList->DrawStringGrid(Canvas,L,H,MarginLeft,Y,Li,0,Kfs-1,
    Nsn,Nsk,Kx,Ky,MStlb);
   Canvas->Font->Style=TFontStyles()<<fsBold;
   if(HIt)
    FormEditList->DrawHeaderControl(Canvas,L,H,MarginLeft,Y,Li,0,Kfs-1,Kx,Ky,
     MStlb);
   Y=Yn;
   EditList->Shapk->DrawShapka(Canvas,L,H,MarginLeft-Xsh[i],Y,Li,NN,NK,Kx,Ky,
    MStlb);
   Canvas->Font->Style=TFontStyles()>>fsBold;
   Y+=H/5;
   FormEditList->DrawStringGrid(Canvas,L,H,MarginLeft+(Lfs?Lfs-L:0),Y,Li,NN,NK,
    Nsn,Nsk,Kx,Ky,MStlb);
   if(HIt){
    Canvas->Font->Style=TFontStyles()<<fsBold;
    FormEditList->DrawHeaderControl(Canvas,L,H,MarginLeft+(Lfs?Lfs-L:0),Y,Li,
     NN,NK,Kx,Ky,MStlb);
    Canvas->Font->Style=TFontStyles()>>fsBold;
   }
   Y+=H;
 }}
//вывод на канвас средних страниц
 for(i=0;i<KP-2;i++){
  NPg++;
  if(Np==(NStartPage+i+1)){
   Canvas->Font->Style=TFontStyles()<<fsBold;
   DrawHead(Canvas,0);
   Canvas->Font->Style=TFontStyles()>>fsBold;
   swprintf(Inf,L"Продолжение. Стр.%d",NStartPage+i+2);
   Canvas->TextOut(Round(Kx*MarginLeft),Round(Ky*Y),Inf);
   Y+=H;
   for(int j=0;j<Kt2;j++){
    NN=nn[j+i*Kt2+Kt1]; NK=nk[j+i*Kt2+Kt1]; Yn=Y;
    Canvas->Font->Style=TFontStyles()<<fsBold;
    EditList->Shapk->DrawShapka(Canvas,L,H,MarginLeft,Y,Li,0,Kfs-1,Kx,Ky,MStlb);
    Canvas->Font->Style=TFontStyles()>>fsBold;
    Y+=H/5;
    FormEditList->DrawStringGrid(Canvas,L,H,MarginLeft,Y,Li,0,Kfs-1,
     Nsn,Nsk,Kx,Ky,MStlb);
    Canvas->Font->Style=TFontStyles()<<fsBold;
    if(HIt)
     FormEditList->DrawHeaderControl(Canvas,L,H,MarginLeft,Y,Li,0,Kfs-1,Kx,Ky,
      MStlb);
    Y=Yn;
    EditList->Shapk->DrawShapka(Canvas,L,H,MarginLeft-Xsh[j+i*Kt2+Kt1],Y,Li,
     NN,NK,Kx,Ky,MStlb);
    Canvas->Font->Style=TFontStyles()>>fsBold;
    Y+=H/5;
    FormEditList->DrawStringGrid(Canvas,L,H,MarginLeft+(Lfs?Lfs-L:0),Y,Li,
     NN,NK,Nsn,Nsk,Kx,Ky,MStlb);
    if(HIt){
     Canvas->Font->Style=TFontStyles()<<fsBold;
     FormEditList->DrawHeaderControl(Canvas,L,H,MarginLeft+(Lfs?Lfs-L:0),Y,Li,
      NN,NK,Kx,Ky,MStlb);
     Canvas->Font->Style=TFontStyles()>>fsBold;
    }
    Y+=H;
 }}}
 if(KP>1){
  NPg++;
  if(Np==(NStartPage+i+1)){ int WHead=0;
   NN=nn[(KP-2)*Kt2+Kt1]; NK=nk[(KP-2)*Kt2+Kt1];
   for(int ii=0;ii<Kfs;ii++) WHead+=Li[ii]+L;
   for(int ii=NN;ii<=NK;ii++) WHead+=Li[ii]+L;
   Canvas->Font->Style=TFontStyles()<<fsBold;
   if(NK&&NN){
    DrawHead(Canvas,WHead);
    Canvas->Font->Style=TFontStyles()>>fsBold;
    swprintf(Inf,L"Продолжение. Стр.%d",NStartPage+i+2);
    Canvas->TextOut(Round(Kx*MarginLeft),Round(Ky*Y),Inf);
    Y+=H;
    for(int j=0;j<min(Ktn,Kp-Kt1-(KP-2)*Kt2);j++){
     NN=nn[j+(KP-2)*Kt2+Kt1]; NK=nk[j+(KP-2)*Kt2+Kt1]; Yn=Y;
     Canvas->Font->Style=TFontStyles()<<fsBold;
     EditList->Shapk->DrawShapka(Canvas,L,H,MarginLeft,Y,Li,0,Kfs-1,Kx,Ky,MStlb);
     Canvas->Font->Style=TFontStyles()>>fsBold;
     Y+=H/5;
     FormEditList->DrawStringGrid(Canvas,L,H,MarginLeft,Y,Li,0,Kfs-1,
      Nsn,Nsk,Kx,Ky,MStlb);
     Canvas->Font->Style=TFontStyles()<<fsBold;
     if(HIt)
      FormEditList->DrawHeaderControl(Canvas,L,H,MarginLeft,Y,Li,0,Kfs-1,Kx,Ky,
       MStlb);
     Y=Yn;
     EditList->Shapk->DrawShapka(Canvas,L,H,MarginLeft-Xsh[j+(KP-2)*Kt2+Kt1],Y,
      Li,NN,NK,Kx,Ky,MStlb);
     Canvas->Font->Style=TFontStyles()>>fsBold;
     Y+=H/5;
     FormEditList->DrawStringGrid(Canvas,L,H,MarginLeft+(Lfs?Lfs-L:0),Y,Li,
      NN,NK,Nsn,Nsk,Kx,Ky,MStlb);
     if(HIt){
      Canvas->Font->Style=TFontStyles()<<fsBold;
      FormEditList->DrawHeaderControl(Canvas,L,H,MarginLeft+(Lfs?Lfs-L:0),Y,Li,
       NN,NK,Kx,Ky,MStlb);
      Canvas->Font->Style=TFontStyles()>>fsBold;
     }
     Y+=H;
 }}}}
 if(Np==(TabControlPages->Tabs->Count-1)){
  Y+=H;
  if(Hp&&EditList->Podval&&EditList->Podval[0]){
   Canvas->Font->Style=TFontStyles()<<fsBold;
   EditList->DrawPodval(Knp,Canvas,H,MarginLeft,MarginTop,Hp,hp,Y,Kx,Ky);
   Canvas->Font->Style=TFontStyles()>>fsBold;
 }}
 return NPg;
}
//---------------------------------------------------------------------------
void __fastcall TFormEditListPrn::DrawHugeTabl(TCanvas *Canvas,int Np){
 int i,Ks1,Ks2,Ksn,Kr,KSh,NPg;
//Ks1,Ks2,Ksn,Kr - соотв. число строк Stringgrid на первой,
//средней и последней страницах; общее число одностраничных
//широких полос
//Определим общее число одностраничных широких полос Kr
 KSh=HSh/H; if(HSh%H) KSh++;
 Ks1=Ks-(Knfh+1)-(Knh+1)-KSh-2;
 Ks2=Ks-3-(Knh+1)-KSh;
 Ksn=(FormEditList->StringGridEditList->RowCount-Ks1)%Ks2;
 Kr=2+(FormEditList->StringGridEditList->RowCount-Ks1-Ksn)/Ks2;
// if(!Ksn){ Ksn=Ks2; Kr--; }//!!!!
 if(Ksn<0) Ksn=0;
//Первая полоса
 Ht=H*Ks1;
 NPg=DrawBroadTabl(Canvas,Hfh,0,0,Ks1-1,0,0,Np);
 if(!NPg)
  return;
//вывод на canvas средних полос
 Ht=H*Ks2;
 for(i=0;i<Kr-2;i++){
//  Prn->NewPage(); Y=MarginTop;
  NPg+=DrawBroadTabl(Canvas,0,0,Ks1+i*Ks2,Ks1+(i+1)*Ks2-1,0,NPg,Np);
 }
//Вывод последней полосы с подвалом
// Prn->NewPage(); Y=MarginTop;
 Ht=H*Ksn;
 DrawBroadTabl(Canvas,0,Hp,Ks1+i*Ks2,
  FormEditList->StringGridEditList->RowCount-1,H,NPg,Np);
}
//---------------------------------------------------------------------------
void __fastcall TFormEditListPrn::TabControlPagesChange(TObject *Sender){
//Рисование на канвасе ImagePage
 ScrollBox->Align=alNone;
 ScrollBox->Height=Round(Ky*PhH);
 ScrollBox->Width=Round(Kx*PhW);
 if(ImagePage) delete ImagePage;
 ImagePage=new TImage(ScrollBox);
 ImagePage->Parent=ScrollBox;
 ImagePage->Align=alClient;
 ImagePage->ShowHint=true; ImagePage->Hint=L"Образ страницы для печати";
 ImagePage->Canvas->Font->Name=Canvas->Font->Name;
 ImagePage->Canvas->Font->Size=Canvas->Font->Size;
 ImagePage->Left=0; ImagePage->Top=0;
// ImagePage->Height=Round(Ky*PhH);
// ImagePage->Width=Round(Kx*PhW);
 ImagePage->Canvas->Brush->Color=clWhite;
 ImagePage->Align=alNone;
 ScrollBox->Align=alClient;
 ImagePage->Visible=true;
 Y=MarginTop-Y0;
 switch (VidTablPrn) {
  case ONE:
   DrawOnePage(ImagePage->Canvas); break;
  case SMALL:
   DrawManyPage(ImagePage->Canvas,TabControlPages->TabIndex); break;
  case LARGE:
   DrawBroadTabl(ImagePage->Canvas,Hfh,Hp,0,
    FormEditList->StringGridEditList->RowCount-1,H,0,
     TabControlPages->TabIndex);
   break;
  case HUGE:
   DrawHugeTabl(ImagePage->Canvas,TabControlPages->TabIndex); break;
 }
// if(ScrollBarPage->Visible) ScrollBarPage->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TFormEditListPrn::FormClose(TObject *Sender,
      TCloseAction &Action){
 if(Li)
  free(Li);
}
//---------------------------------------------------------------------------
void __fastcall TFormEditListPrn::ScrollBarPageScroll(TObject *Sender,
      TScrollCode ScrollCode, int &ScrollPos){ int Y1=Y0;
 switch(ScrollCode){
  case scLineUp: case scLineDown: case scPageUp: case scPageDown:
  case scPosition: case scTop: case scBottom:
   Y0=ScrollPos*H; break;
 }
 if(Y0!=Y1) TabControlPagesChange(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TFormEditListPrn::TabControlPagesChanging(TObject *Sender,
      bool &AllowChange){
 Y0=0; //ScrollBarPage->Position=0;
}
//---------------------------------------------------------------------------
void __fastcall TFormEditListPrn::ScrollBarPageKeyDown(TObject *Sender,
      WORD &Key, TShiftState Shift){
 switch(Key){
  case 'P':
   if(Shift.Contains(ssCtrl))
    ToolButtonToPrnClick(Sender); break;
}}
//---------------------------------------------------------------------------
void __fastcall TFormEditListPrn::ToolButtonPrnOptClick(TObject *Sender){
 wchar_t *s,S[256];
 wcscpy(S,EditList->FileHelp);
 if(s=wcsrchr(S,'\\')) { *s='\0';
  swprintf(FormParamPrint->FileHelp,L"%s\\ChangParmPrn.htm",S);
 }
 else
  swprintf(FormParamPrint->FileHelp,L"%s\\ChangParmPrn.htm",WORKDIR);
 FormParamPrint->FilePrn=EditList->FilePrn;
 FormParamPrint->ShowModal();
 if(FormParamPrint->Change) FormShow(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TFormEditListPrn::ToPrint(){
 double Kxo=Kx,Kyo=Ky,i,j; Kx=Ky=1.00;
 Prn->BeginDoc();
 if(FormToPrn->Collate){
  for(i=0;i<FormToPrn->Copies-1;i++){
   switch(VidTablPrn){
    case ONE:
     Y=MarginTop; DrawOnePage(Prn->Canvas); break;
    case SMALL:
     for(j=FormToPrn->FromPage-1;j<FormToPrn->ToPage;j++){
      Y=MarginTop; DrawManyPage(Prn->Canvas,j);
      Prn->NewPage();
     }
     break;
    case LARGE:
     for(j=FormToPrn->FromPage-1;j<FormToPrn->ToPage;j++){
      Y=MarginTop;
      DrawBroadTabl(Prn->Canvas,Hfh,Hp,0,
       FormEditList->StringGridEditList->RowCount-1,H,0,j);
      Prn->NewPage();
     }
     break;
    case HUGE:
     for(j=FormToPrn->FromPage-1;j<FormToPrn->ToPage;j++){
      Y=MarginTop; DrawHugeTabl(Prn->Canvas,j);
      Prn->NewPage();
     }
     break;
  }}
  switch (VidTablPrn) {
   case ONE:
    Y=MarginTop; DrawOnePage(Prn->Canvas); break;
   case SMALL:
    for(j=FormToPrn->FromPage-1;j<FormToPrn->ToPage-1;j++){
     Y=MarginTop; DrawManyPage(Prn->Canvas,j);
     Prn->NewPage();
    }
    Y=MarginTop; DrawManyPage(Prn->Canvas,j);
    break;
   case LARGE:
    for(j=FormToPrn->FromPage-1;j<FormToPrn->ToPage-1;j++){
     Y=MarginTop;
     DrawBroadTabl(Prn->Canvas,Hfh,Hp,0,
      FormEditList->StringGridEditList->RowCount-1,H,0,j);
     Prn->NewPage();
    }
    Y=MarginTop;
    DrawBroadTabl(Prn->Canvas,Hfh,Hp,0,
     FormEditList->StringGridEditList->RowCount-1,H,0,j);
    break;
   case HUGE:
    for(j=FormToPrn->FromPage-1;j<FormToPrn->ToPage-1;j++){
     Y=MarginTop; DrawHugeTabl(Prn->Canvas,j);
     Prn->NewPage();
    }
    Y=MarginTop; DrawHugeTabl(Prn->Canvas,j);
    break;
 }}
 else{
  switch (VidTablPrn) {
   case ONE:
    for(i=0;i<FormToPrn->Copies-1;i++){
     Y=MarginTop; DrawOnePage(Prn->Canvas);
     Prn->NewPage();
    }
    Y=MarginTop; DrawOnePage(Prn->Canvas);
    break;
   case SMALL:
    for(j=FormToPrn->FromPage-1;j<FormToPrn->ToPage-1;j++){
     for(i=0;i<FormToPrn->Copies;i++){
      Y=MarginTop; DrawManyPage(Prn->Canvas,j);
      Prn->NewPage();
    }}
    for(i=0;i<FormToPrn->Copies-1;i++){
     Y=MarginTop; DrawManyPage(Prn->Canvas,j);
     Prn->NewPage();
    }
    Y=MarginTop; DrawManyPage(Prn->Canvas,j);
    break;
   case LARGE:
    for(j=FormToPrn->FromPage-1;j<FormToPrn->ToPage-1;j++){
     for(i=0;i<FormToPrn->Copies;i++){
      Y=MarginTop;
      DrawBroadTabl(Prn->Canvas,Hfh,Hp,0,
       FormEditList->StringGridEditList->RowCount-1,H,0,j);
      Prn->NewPage();
    }}
    for(i=0;i<FormToPrn->Copies-1;i++){
     Y=MarginTop;
     DrawBroadTabl(Prn->Canvas,Hfh,Hp,0,
      FormEditList->StringGridEditList->RowCount-1,H,0,j);
     Prn->NewPage();
    }
    Y=MarginTop;
    DrawBroadTabl(Prn->Canvas,Hfh,Hp,0,
     FormEditList->StringGridEditList->RowCount-1,H,0,j);
    break;
   case HUGE:
    for(j=FormToPrn->FromPage-1;j<FormToPrn->ToPage-1;j++){
     for(i=0;i<FormToPrn->Copies;i++){
      Y=MarginTop; DrawHugeTabl(Prn->Canvas,j);
      Prn->NewPage();
    }}
    for(i=0;i<FormToPrn->Copies-1;i++){
     Y=MarginTop; DrawHugeTabl(Prn->Canvas,j);
     Prn->NewPage();
    }
    Y=MarginTop; DrawHugeTabl(Prn->Canvas,j);
    break;
 }}
 Prn->EndDoc();
 Kx=Kxo; Ky=Kyo;
}
//---------------------------------------------------------------------------
void __fastcall ToPrinter(){
 FormEditListPrn->ToPrint();
}
//---------------------------------------------------------------------------
void __fastcall TFormEditListPrn::ToolButtonToPrnClick(TObject *Sender){
 wchar_t *s,S[256];
 Application->CreateForm(__classid(TFormToPrn), &FormToPrn);
 FormToPrn->MaxPage=TabControlPages->Tabs->Count;
 FormToPrn->FromPage=1;
 FormToPrn->ToPage=TabControlPages->Tabs->Count;
 FormToPrn->ToPrinter=ToPrinter;
 wcscpy(S,EditList->FileHelp);
 if(s=wcsrchr(S,'\\')) { *s='\0';
  swprintf(FormToPrn->FileHelp,L"%s\\toprn.htm",S);
 }
 else
  swprintf(FormToPrn->FileHelp,L"%s\\toprn.htm",WORKDIR);
 FormToPrn->ShowModal();
}
//---------------------------------------------------------------------------

