//---------------------------------------------------------------------------

#ifndef FormPrREH
#define FormPrREH
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
class TFormPrintRichEdit:public TForm{
__published:	// IDE-managed Components
 TTabControl *TabControlPrint;
 TToolBar *ToolBar;
 TToolButton *ToolButtonPrint;
 TToolButton *ToolButtonDevide;
 TToolButton *ToolButtonSetup;
 TImageList *ImageList;
 TToolButton *ToolButtonDiv2;
 TToolButton *ToolButtonExit;
        TImage *Image;
 void __fastcall ButtonSetupClick(TObject *Sender);
 void __fastcall TabControlPrintChange(TObject *Sender);
 void __fastcall FormShow(TObject *Sender);
 void __fastcall ToolButtonExitClick(TObject *Sender);
 void __fastcall ToolButtonPrintClick(TObject *Sender);
private:	// User declarations
 TPrinter *Prntr; wchar_t Otstup[256];
 int
  MarginTop,MarginLeft,MarginRight,MarginBottom,//отступы в пикселях
  X,Y,Y0,LPx,LPy,LPiy,LPix,Wimm,Wpmm,Himm,Hpmm,
  Ks,//число строк текста на странице
  Kp,//число страниц
  Hsi,Hs;//высота строки в пикселях на экране и на принтере
public:		// User declarations
 AnsiString FileKDM;
      __fastcall TFormPrintRichEdit(TComponent* Owner);
 void __fastcall PrepareTabControl();
 void __fastcall ToPrntr();
};
//---------------------------------------------------------------------------
extern PACKAGE TFormPrintRichEdit *FormPrintRichEdit;
//---------------------------------------------------------------------------
#endif
