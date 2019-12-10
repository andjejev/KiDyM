//---------------------------------------------------------------------------

#ifndef OtchetFormH
#define OtchetFormH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ImgList.hpp>
#include <ToolWin.hpp>
#include "SHDocVw_OCX.h"
#include <OleCtrls.hpp>
#include <Dialogs.hpp>
//---------------------------------------------------------------------------
class TFormOtchet : public TForm
{
__published:	// IDE-managed Components
        TImageList *ImageList;
        TCppWebBrowser *CppWebBrowser;
        TStatusBar *StatusBar;
        TCoolBar *CoolBar;
        TToolBar *ToolBar;
        TToolButton *tbBack;
        TToolButton *tbForward;
        TToolButton *tbExit;
        TComboBox *cbURL;
        TOpenDialog *OpenDialog;
        TToolButton *tbSep2;
        TToolButton *tbOpen;
        TToolButton *tbSep1;
        TToolButton *tbSep3;
        TToolButton *tbHelp;
        void __fastcall FormShow(TObject *Sender);
        void __fastcall cbURLClick(TObject *Sender);
        void __fastcall cbURLKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
        void __fastcall CppWebBrowserBeforeNavigate2(TObject *Sender,
          LPDISPATCH pDisp, Variant *URL, Variant *Flags,
          Variant *TargetFrameName, Variant *PostData, Variant *Headers,
          VARIANT_BOOL *Cancel);
        void __fastcall CppWebBrowserProgressChange(TObject *Sender,
          long Progress, long ProgressMax);
        void __fastcall tbBackClick(TObject *Sender);
        void __fastcall tbForwardClick(TObject *Sender);
        void __fastcall tbOpenClick(TObject *Sender);
        void __fastcall tbExitClick(TObject *Sender);
        void __fastcall tbHelpClick(TObject *Sender);
private:	// User declarations
 void __fastcall Load();
public:		// User declarations
 __fastcall TFormOtchet(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormOtchet *FormOtchet;
//---------------------------------------------------------------------------
#endif
