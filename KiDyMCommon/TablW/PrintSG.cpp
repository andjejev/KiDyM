//‘айл печати StringGrid
//---------------------------------------------------------------------------
#include "PrintSG.h"
//---------------------------------------------------------------------------
int KSimbStr (char *S,char C){
 int K=0; for(;*S;S++) if(*S==C) K++; return K;
}
//---------------------------------------------------------------------------
void PrintStringGrid(int *Align,TStringGrid *SG,TPrintDialog *PD,
  char *Head,char *Podval) {
 int IndPrnOld,IndPrn,HPaper,WPaper,LPxOld,LPyOld,OrientOld,Orient,d,
  LPx,LPy,Wse,Wsp,Y,Hs,i,j,k,l,ik,kk,Kp=0,L,*Lmax,Ks,Lh,Hh,Hp,lh,Lp,lp;
 TPrinter *Prn=Printer(); char *n,*h,Prod[256];
 IndPrnOld=Prn->PrinterIndex;
 HPaper=Prn->PageHeight; WPaper=Prn->PageWidth;
 LPxOld=GetDeviceCaps(Prn->Handle,LOGPIXELSX);
 LPyOld=GetDeviceCaps(Prn->Handle,LOGPIXELSY);
 OrientOld=Prn->Orientation;
 Lmax=(int *)calloc(SG->ColCount,sizeof(int));
//¬ыбираем параметры печати StringGrid (какой он получилс€)
 Prn->Canvas->Font->Name=SG->Canvas->Font->Name;
 Prn->Canvas->Font->Size=SG->Canvas->Font->Size;
 for(i=0;i<SG->RowCount;i++){
  for(j=0;j<SG->ColCount;j++){  //ширина столбцов
   L=Prn->Canvas->TextWidth(SG->Cells[j][i]);
   if(L>Lmax[j]) Lmax[j]=L; //ширина в пиксел€х
 }}
 d=Prn->Canvas->Font->Size*LPyOld/216;
 for(j=0,L=SG->GridLineWidth;j<SG->ColCount;j++)//ширина таблицы
  L+=Lmax[j]+SG->GridLineWidth+2*d;
 //”берем справа '\n' из заголовка:
 for(n=Head+strlen(Head)-1;n>Head&&*n=='\n';n--); *n='\0';
 Hh=KSimbStr(Head,'\n')+1;//число строк в заголовке
//”берем справа '\n' из подвала:
 for(n=Podval+strlen(Podval)-1;n>Podval&&*n=='\n';n--); *n='\0';
 Hp=KSimbStr(Podval,'\n')+1;//число строк в подвале
 for(i=0,Lh=0,h=Head;i<Hh;i++){//Ќайдем ширину заголовка
  if(n=strchr(h,'\n')) *n='\0';
  lh=Prn->Canvas->TextWidth(h); if(lh>Lh) Lh=lh;
  if(n){ *n++='\n'; h=n; }
 }
 for(i=0,Lp=0,h=Podval;i<Hp;i++){//Ќайдем ширину подвала
  if(n=strchr(h,'\n')) *n='\0';
  lp=Prn->Canvas->TextWidth(h); if(lp>Lp) Lp=lp;
  if(n){ *n++='\n'; h=n; }
 }
 if(Lh>L) L=Lh; if(Lp>L) L=Lp;
 Prn->Canvas->Font->Size=(Prn->Canvas->Font->Size*WPaper)/L;
 if(Prn->Canvas->Font->Size>14) Prn->Canvas->Font->Size=14;
//–асчет новой ширины таблицы
 for(j=0;j<SG->ColCount;j++) Lmax[j]=0;//ширина j-го столбца в пиксел€х
 for(i=0;i<SG->RowCount;i++){
  for(j=0;j<SG->ColCount;j++){
   L=Prn->Canvas->TextWidth(SG->Cells[j][i]);
   if(L>Lmax[j]) Lmax[j]=L; //ширина в пиксел€х
 }}
 d=Prn->Canvas->Font->Size*LPyOld/216;
 for(j=0,L=SG->GridLineWidth;j<SG->ColCount;j++)//ширина таблицы
  L+=Lmax[j]+SG->GridLineWidth+2*d;
 Hs=Prn->Canvas->Font->Size*LPyOld/72;
 Ks=HPaper/Hs;//количество строк на листе
/* while(Ks<=2){
  Prn->Canvas->Font->Size--;
  Hs=Prn->Canvas->Font->Size*LPyOld/72;
  Ks=HPaper/Hs;//количество строк на листе
 }*/
 if(Ks>=(Hh+Hp+SG->RowCount+2)) Kp=1;
 else {
  Kp=(SG->RowCount-2+Hh+Hp)/(Ks-3);
  if((SG->RowCount-2+Hh+Hp)%(Ks-3)) Kp++;
 }
 PD->MinPage=1;  PD->MaxPage=Kp;
 PD->FromPage=1; PD->ToPage=Kp;
 if(Kp>1) PD->Options<<poPageNums;
 if(PD->Execute()){
  Prn=Printer();
  Prn->BeginDoc();
  IndPrn=Prn->PrinterIndex;
  LPx=GetDeviceCaps(Prn->Handle,LOGPIXELSX);
  LPy=GetDeviceCaps(Prn->Handle,LOGPIXELSY);
  Orient=Prn->Orientation;
 if(IndPrn!=IndPrnOld||HPaper!=Prn->PageHeight||WPaper!=Prn->PageWidth||
     LPx!=LPxOld||LPy!=LPyOld||Orient!=OrientOld){
   for(j=0;j<SG->ColCount;j++) Lmax[j]=0;
   for(i=0;i<SG->RowCount;i++){//пересчитаем ширину таблицы
    for(j=0;j<SG->ColCount;j++){
     L=Prn->Canvas->TextWidth(SG->Cells[j][i]);
     if(L>Lmax[j]) Lmax[j]=L; //ширина в пиксел€х
   }}
   d=Prn->Canvas->Font->Size*LPy/216;
   for(j=0,L=SG->GridLineWidth;j<SG->ColCount;j++)//ширина таблицы
    L+=Lmax[j]+SG->GridLineWidth+2*d;
   for(i=0,Lh=0,h=Head;i<Hh;i++){//Ќайдем ширину заголовка
    if(n=strchr(h,'\n')) *n='\0';
    lh=Prn->Canvas->TextWidth(h); if(lh>Lh) Lh=lh;
    if(n){ *n++='\n'; h=n; }
   }
   for(i=0,Lp=0,h=Podval;i<Hp;i++){//Ќайдем ширину подвала
    if(n=strchr(h,'\n')) *n='\0';
    lp=Prn->Canvas->TextWidth(h); if(lp>Lp) Lp=lp;
    if(n){ *n++='\n'; h=n; }
   }
   while(Lh>L){
    for(j=0,L=SG->GridLineWidth;j<SG->ColCount;j++){
     Lmax[j]++; L+=Lmax[j]+SG->GridLineWidth+2*d;
   }}
   while(Lp>L){
    for(j=0,L=SG->GridLineWidth;j<SG->ColCount;j++){
     Lmax[j]++; L+=Lmax[j]+SG->GridLineWidth+2*d;
   }}
   HPaper=Prn->PageHeight; WPaper=Prn->PageWidth;
   Prn->Canvas->Font->Size=(Prn->Canvas->Font->Size*WPaper)/L;
   if(Prn->Canvas->Font->Size>14) Prn->Canvas->Font->Size=14;
  //–асчет новой ширины таблицы
   for(j=0;j<SG->ColCount;j++) Lmax[j]=0;//ширина j-го столбца в пиксел€х
   for(i=0;i<SG->RowCount;i++){
    for(j=0;j<SG->ColCount;j++){
     L=Prn->Canvas->TextWidth(SG->Cells[j][i]);
     if(L>Lmax[j]) Lmax[j]=L; //ширина в пиксел€х
   }}
   d=Prn->Canvas->Font->Size*LPy/216;
   for(j=0,L=SG->GridLineWidth;j<SG->ColCount;j++)//ширина таблицы
    L+=Lmax[j]+SG->GridLineWidth+2*d;
   Hs=Prn->Canvas->Font->Size*LPy/72;
   Ks=HPaper/Hs;//количество строк на листе
/*   while(Ks<=2){
    Prn->Canvas->Font->Size--;
    Hs=Prn->Canvas->Font->Size*LPy/72;
    Ks=HPaper/Hs;//количество строк на листе
   } */
   if(Ks>=(Hh+Hp+SG->RowCount+2)) Kp=1;
   else {
    Kp=(SG->RowCount-2+Hh+Hp)/(Ks-3);
    if((SG->RowCount-2+Hh+Hp)%(Ks-3)) Kp++;
   }
   PD->MinPage=1;  PD->MaxPage=Kp;
/*   PD->FromPage=1; PD->ToPage=Kp;
   if(Kp>1) PD->Options<<poPageNums;*/
   d=Hs/3;
  }
//  Prn->BeginDoc();
  if(Kp==1){//одна страница
//печатаем заголовок
   for(i=0,h=Head;i<Hh;i++){
    if(n=strchr(h,'\n')) *n='\0';
    lh=Prn->Canvas->TextWidth(h);
    Prn->Canvas->TextOut((L-lh)/2,Hs*i,h);
    if(n){ *n++='\n'; h=n; }
   }
   Y=Hs*i+Hs/2;
//печатаем таблицу
   Prn->Canvas->MoveTo(0,Y-Hs/4);
   Prn->Canvas->LineTo(L,Y-Hs/4);
   Prn->Canvas->LineTo(L,Y+Hs+Hs/4);
   Prn->Canvas->LineTo(0,Y+Hs+Hs/4);
   Prn->Canvas->LineTo(0,Y-Hs/4);
   for(j=1,L=Lmax[0]+SG->GridLineWidth+2*d;j<SG->ColCount;j++){
    Prn->Canvas->MoveTo(L,Y-Hs/4+1); Prn->Canvas->LineTo(L,Y+Hs+Hs/4-1);
    L+=Lmax[j]+SG->GridLineWidth+2*d;
   }
   for(j=0,L=SG->GridLineWidth+d;j<SG->ColCount;j++){//заполн€ем шапку
    int l=Prn->Canvas->TextWidth(SG->Cells[j][0]);
    Prn->Canvas->TextOut(L+(Lmax[j]-l)/2,Y,SG->Cells[j][0]);
    L+=Lmax[j]+SG->GridLineWidth+2*d;
   }
   Y+=Hs+Hs/2;
   for(i=1;i<SG->RowCount;i++){
    for(j=0,L=SG->GridLineWidth+d;j<SG->ColCount;j++){
     int l=Prn->Canvas->TextWidth(SG->Cells[j][i]),X=L;
     switch(Align[j]){
      case 0:  X+=(Lmax[j]-l)/2; break;
      case 1:  X+=Lmax[j]-l; break;
     }
     Prn->Canvas->TextOut(X,Y,SG->Cells[j][i]);
     L+=Lmax[j]+SG->GridLineWidth+2*d;
    }
    Y+=Hs;
   }
   Prn->Canvas->MoveTo(0,Y+Hs/4); Prn->Canvas->LineTo(L,Y+Hs/4); Y+=Hs;
//печатаем подвал
   for(i=0,h=Podval;i<Hp;i++){
    if(n=strchr(h,'\n')) *n='\0';
//    lp=Prn->Canvas->TextWidth(h);
    Prn->Canvas->TextOut(0/*(L-lp)/2*/,Y,h);
    if(n){ *n++='\n'; h=n; } Y+=Hs;
   }
  }//конец печати единственной страницы
  else{ int k,Ns=1+(Ks-Hh-3)*(PD->FromPage-1);//номер печатаемой строки
//печать заполненных страниц
   for(k=PD->FromPage;k<=PD->ToPage;k++){
    if(k>PD->FromPage) Prn->NewPage();
    if(k>1) sprintf(Prod,"ѕродолжение, с.%d",k);
    for(i=0,h=Head;i<Hh;i++){
     if(n=strchr(h,'\n')) *n='\0';
     lh=Prn->Canvas->TextWidth(h);
     Prn->Canvas->TextOut((L-lh)/2,Hs*i,h);
     if(n){ *n++='\n'; h=n; }
    }
/*    for(i=0,h=(k>1?Prod:Head);i<(k>1?1:Hh);i++){
     if(n=strchr(h,'\n')) *n='\0';
     lh=Prn->Canvas->TextWidth(h);
     Prn->Canvas->TextOut(k>1?(L-lh):(L-lh)/2,Hs*i,h);
     if(n){ *n++='\n'; h=n; }
    }*/
    if(k>1){ lh=Prn->Canvas->TextWidth(Prod);
     Prn->Canvas->TextOut(L-lh,0,Prod);
    }
 //печатаем таблицу
    Y=Hs*i+Hs/2;
    Prn->Canvas->MoveTo(0,Y-Hs/4);    Prn->Canvas->LineTo(L,Y-Hs/4);
    Prn->Canvas->LineTo(L,Y+Hs+Hs/4); Prn->Canvas->LineTo(0,Y+Hs+Hs/4);
    Prn->Canvas->LineTo(0,Y-Hs/4);
    for(j=1,L=Lmax[0]+SG->GridLineWidth+2*d;j<SG->ColCount;j++){
     Prn->Canvas->MoveTo(L,Y-Hs/4+1); Prn->Canvas->LineTo(L,Y+Hs+Hs/4-1);
     L+=Lmax[j]+SG->GridLineWidth+2*d;
    }
    for(j=0,L=SG->GridLineWidth+d;j<SG->ColCount;j++){//заполн€ем шапку
     int l=Prn->Canvas->TextWidth(SG->Cells[j][0]);
     Prn->Canvas->TextOut(L+(Lmax[j]-l)/2,Y,SG->Cells[j][0]);
     L+=Lmax[j]+SG->GridLineWidth+2*d;
    }
    Y+=Hs+Hs/2;
    for(i=1;i<Ks-(k>1?1:Hh)-2;i++){
     for(j=0,L=SG->GridLineWidth+d;j<SG->ColCount;j++){
      int l=Prn->Canvas->TextWidth(SG->Cells[j][Ns]),X=L;
      switch(Align[j]){
       case 0:  X+=(Lmax[j]-l)/2; break;
       case 1:  X+=Lmax[j]-l; break;
      }
      Prn->Canvas->TextOut(X,Y,SG->Cells[j][Ns]);
      L+=Lmax[j]+SG->GridLineWidth+2*d;
     }
     Ns++; Y+=Hs; if(Ns==SG->RowCount) break;
   }}
   if(k>=PD->MaxPage){
    if((Y+Hs/4)>Prn->PageHeight) Prn->NewPage();
    Prn->Canvas->MoveTo(0,Y+Hs/4); Prn->Canvas->LineTo(L,Y+Hs/4); Y+=Hs;
    if((Y+Hp*Hs)>Prn->PageHeight) Prn->NewPage();
//печатаем подвал
    for(i=0,h=Podval;i<Hp;i++){
     if(n=strchr(h,'\n')) *n='\0';
     Prn->Canvas->TextOut(0,Y,h);
     if(n){ *n++='\n'; h=n; } Y+=Hs;
  }}}
  Prn->EndDoc();
 }
 free(Lmax);
}
//---------------------------------------------------------------------------

