//---------------------------------------------------------------------------

#ifndef OperMessH
#define OperMessH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ImgList.hpp>
#include <ToolWin.hpp>
//---------------------------------------------------------------------------
class TFormOper : public TForm{
__published:	// IDE-managed Components
        TToolBar *ToolBar;
        TToolButton *tbPrint;
        TToolButton *ToolButton2;
        TToolButton *tbExit;
        TImageList *ImageList;
        TRichEdit *RichEditOper;
        void __fastcall tbExitClick(TObject *Sender);
        void __fastcall tbPrintClick(TObject *Sender);
        void __fastcall FormShow(TObject *Sender);
        void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
private:	// User declarations
public:		// User declarations
 char FilePrn[256];
        __fastcall TFormOper(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormOper *FormOper;
//---------------------------------------------------------------------------
#endif
