//---------------------------------------------------------------------------

#ifndef EditListPrnH
#define EditListPrnH
//---------------------------------------------------------------------------
#include "ToPrn.h"
#include <Controls.hpp>
#include <ComCtrls.hpp>
#include <ToolWin.hpp>
#include <Forms.hpp>
#include <Classes.hpp>
#include <ImgList.hpp>
//---------------------------------------------------------------------------
class TFormEditListPrn : public TForm{
__published:	// IDE-managed Components
        TToolBar *ToolBar;
        TImageList *ImageList;
        TToolButton *ToolButton2;
        TToolButton *ToolButtonSave;
        TToolButton *ToolButton4;
        TToolButton *ToolButtonHelp;
        TToolButton *ToolButton6;
        TToolButton *ToolButtonExit;
        TStatusBar *StatusBar;
        TToolButton *ToolButtonPrnOpt;
        TToolButton *ToolButtonToPrn;
        TTabControl *TabControlPages;
        TScrollBox *ScrollBox;
        void __fastcall ToolButtonHelpClick(TObject *Sender);
        void __fastcall ToolButtonExitClick(TObject *Sender);
        void __fastcall FormShow(TObject *Sender);
        void __fastcall TabControlPagesChange(TObject *Sender);
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall ScrollBarPageScroll(TObject *Sender,
          TScrollCode ScrollCode, int &ScrollPos);
        void __fastcall TabControlPagesChanging(TObject *Sender,
          bool &AllowChange);
//        void __fastcall ToolButtonPrnClick(TObject *Sender);
        void __fastcall ScrollBarPageKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
        void __fastcall ToolButtonPrnOptClick(TObject *Sender);
        void __fastcall ToolButtonToPrnClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
 TImage *ImagePage;
 TPrinter *Prn; double Ktw;
 wchar_t FileHelp[256];
 double Kx,Ky,//коэффициенты перевода из принтерных в экранные пикселы
  Mp;//масштаб по пикселям
 int
  MarginLeft,MarginRight,MarginTop,MarginBot,//отступы в пикселях
  LPxscreen,LPyscreen,//разрешение экрана
  LPx,LPy,X,Y,//разрешение принтера по горизонтали и вертикали
//Высоты в пикселях заголовка, шапки, таблицы, подвала,
//области печати на стр., строки:
  Hh,Hfh,HSh,Ht,Hp,hp,H;
//число выбранных для печати столбцов,ширины фикс.части, столбцов,
//мантиссы, дробной части
//прокручиваемой части таблицы, области печати страницы, символа:
 int ColCount,Lfs,*Li,*LiDo,*LiPo,Lt,lp,L,
//колич. строк в области печати (на странице),
//фиксируемых столбцов,общее число страниц,число полных строк
//в заголовке и в подвале, число порций по гирозонтали:
  Ks,Kfs,KP,Knfh,Knh,Knp,Kp,
//вид таблицы - одна, узкая-высокая, низкая-широкая, громадная
//начальная позиция вывода
  VidTablPrn,Y0;
 int Kss,Kps,Hsp,Dl,PhW,PhH,PhL,PhT,PhR,PhB,PhWmm,PhHmm,PhLmm,PhTmm,PhRmm,
  PhBmm,Wmm,Hmm;
 Magazine *MStlb;
      __fastcall TFormEditListPrn(TComponent* Owner);
 void __fastcall DrawOnePage(TCanvas *Canvas);
 void __fastcall DrawFullHead(TCanvas *Canvas);
 void __fastcall DrawHead(TCanvas *Canvas,int W);
 void __fastcall DrawManyPage(TCanvas *Canvas,int Np);
 int  __fastcall DrawBroadTabl(TCanvas *Canvas,int Hfh,int Hp,int Nsn,int Nsk,
         int HIt,int NStartPage,int Np);
 void __fastcall DrawHugeTabl(TCanvas *Canvas,int Np);
 int  __fastcall Mashtabs(int Hp);
 int  __fastcall CountPagesBroadTabl(int Hp,int HIt);
 void __fastcall CountPagesHugeTabl();
 void __fastcall ToPrint();
};
//---------------------------------------------------------------------------
extern PACKAGE TFormEditListPrn *FormEditListPrn;
//---------------------------------------------------------------------------
#endif
