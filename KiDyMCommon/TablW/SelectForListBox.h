//---------------------------------------------------------------------------

#ifndef SelectForListBoxH
#define SelectForListBoxH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TFormMagListBox : public TForm
{
__published:	// IDE-managed Components
        TPanel *Panel;
        TListBox *ListBox;
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall FormShow(TObject *Sender);
        void __fastcall ListBoxDblClick(TObject *Sender);
        void __fastcall ListBoxKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
private:	// User declarations
public:
 bool Select;		// User declarations
 __fastcall TFormMagListBox(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormMagListBox *FormMagListBox;
//---------------------------------------------------------------------------
#endif
