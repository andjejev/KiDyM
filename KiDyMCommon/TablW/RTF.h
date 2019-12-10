//---------------------------------------------------------------------------

#ifndef RTFH
#define RTFH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ImgList.hpp>
#include <Menus.hpp>
#include <ToolWin.hpp>
//---------------------------------------------------------------------------
class TFormRTF : public TForm {
__published:	// IDE-managed Components
        TToolBar *ToolBar;
        TImageList *ImageList;
        TMainMenu *MainMenu;
        TMenuItem *MFile;
        TToolButton *tbHelp;
        TToolButton *ToolButton2;
        TToolButton *tbCalculator;
        TToolButton *ToolButton4;
        TToolButton *tbExit;
        TRichEdit *RichEdit;
        void __fastcall FormShow(TObject *Sender);
        void __fastcall tbExitClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
 char File[256];
        __fastcall TFormRTF(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormRTF *FormRTF;
//---------------------------------------------------------------------------
#endif
