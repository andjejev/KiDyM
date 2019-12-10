//---------------------------------------------------------------------------

#ifndef SelectForMagH
#define SelectForMagH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TFormMag : public TForm{
__published:	// IDE-managed Components
        TPanel *PanelMag;
        TComboBox *ComboBoxMag;
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall ComboBoxMagDblClick(TObject *Sender);
        void __fastcall ComboBoxMagKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
        void __fastcall FormShow(TObject *Sender);
private:	// User declarations
public:
 bool Select;		// User declarations
 __fastcall TFormMag(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormMag *FormMag;
//---------------------------------------------------------------------------
#endif
