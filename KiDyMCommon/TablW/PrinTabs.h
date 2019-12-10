//---------------------------------------------------------------------------

#ifndef FormPrTab
#define FormPrTab
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Menus.hpp>
#include <ComCtrls.hpp>
#include <Dialogs.hpp>
#include <ImgList.hpp>
#include <ToolWin.hpp>
#include <ExtCtrls.hpp>
#include <Printers.hpp>
//---------------------------------------------------------------------------
class TFormPrintTabs:public TForm{
__published:	// IDE-managed Components
 TTabControl *TabControlPrint;
 TToolBar *ToolBar;
 TToolButton *ToolButtonPrint;
 TToolButton *ToolButtonDevide;
 TToolButton *ToolButtonSetup;
 TImageList *ImageList;
 TToolButton *ToolButtonDiv2;
 TToolButton *ToolButtonExit;
 TRichEdit *RichEditPrint;
 TImage *Image;
 TToolButton *ToolButtonDOSPrn;
 TToolButton *ToolButtonDiv3;
 TToolButton *ToolButtonHelp;
 void __fastcall ButtonSetupClick(TObject *Sender);
 void __fastcall TabControlPrintChange(TObject *Sender);
 void __fastcall FormShow(TObject *Sender);
 void __fastcall ToolButtonExitClick(TObject *Sender);
 void __fastcall ToolButtonPrintClick(TObject *Sender);
        void __fastcall ToolButtonDOSPrnClick(TObject *Sender);
        void __fastcall ToolButtonHelpClick(TObject *Sender);
private:	// User declarations
 TPrinter *Prntr; char Otstup[256];
 int
  MarginTop,MarginLeft,MarginRight,MarginBottom,//отступы в пикселях
  X0,Y0,X,Y,LPx,LPy,LPiy,LPix,Wimm,Wpmm,Himm,Hpmm,
  Ks,//число строк текста на странице
  Kp,//число страниц
  Hsi,Hs;//высота строки в пикселях на экране и на принтере
public:		// User declarations
 AnsiString FileKDM;
      __fastcall TFormPrintTabs(TComponent* Owner);
 void __fastcall PrepareTabControl();
 void __fastcall ToPrn();
 int  __fastcall KPage(int Kstr);
 void __fastcall ToCanvas(TCanvas *Canvas,int Np,int Kstr,int Hs);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormPrintTabs *FormPrintTabs;
//---------------------------------------------------------------------------
#endif
