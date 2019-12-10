//---------------------------------------------------------------------------

#ifndef MultiSelectH
#define MultiSelectH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <CheckLst.hpp>
//---------------------------------------------------------------------------
class TFormMultiSelect : public TForm{
__published:	// IDE-managed Components
        TCheckListBox *CheckListBox;
        TButton *ButtonOK;
        TButton *ButtonCancel;
        TButton *ButtonAll;
        void __fastcall ButtonCancelClick(TObject *Sender);
        void __fastcall ButtonAllClick(TObject *Sender);
        void __fastcall CheckListBoxKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall ButtonOKClick(TObject *Sender);
        void __fastcall FormShow(TObject *Sender);
private:	// User declarations
public:		// User declarations
 bool FOkCancel;
 __fastcall TFormMultiSelect(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormMultiSelect *FormMultiSelect;
//---------------------------------------------------------------------------
#endif
