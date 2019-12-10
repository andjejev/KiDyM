//---------------------------------------------------------------------------

#ifndef FindDirH
#define FindDirH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <FileCtrl.hpp>
//---------------------------------------------------------------------------
class TFormFindDir : public TForm{
__published:	// IDE-managed Components
        TDriveComboBox *DriveComboBox;
        TDirectoryListBox *DirectoryListBox;
        TButton *ButtonOK;
        TButton *ButtonCancel;
        void __fastcall ButtonCancelClick(TObject *Sender);
        void __fastcall ButtonOKClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
 bool Select;
        __fastcall TFormFindDir(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormFindDir *FormFindDir;
//---------------------------------------------------------------------------
#endif
