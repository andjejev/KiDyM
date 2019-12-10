//---------------------------------------------------------------------------

#ifndef EditMagH
#define EditMagH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ToolWin.hpp>
#include "CSPIN.h"

//---------------------------------------------------------------------------
class TFormEditMag : public TForm{
 __published:	// IDE-managed Components
        TMemo *Memo;
        TToolBar *ToolBar;
        TComboBox *ComboBoxFont;
        TCSpinEdit *CSpinEditSize;
        void __fastcall FormShow(TObject *Sender);
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall FormCreate(TObject *Sender);
        void __fastcall ComboBoxFontSelect(TObject *Sender);
        void __fastcall CSpinEditSizeChange(TObject *Sender);
 private:	// User declarations
 public:		// User declarations
  bool DOS;
  char File[256];
  __fastcall TFormEditMag(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormEditMag *FormEditMag;
//---------------------------------------------------------------------------
#endif
