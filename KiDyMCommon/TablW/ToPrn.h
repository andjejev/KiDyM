//---------------------------------------------------------------------------

#ifndef ToPrnH
#define ToPrnH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------
class TFormToPrn : public TForm
{
__published:	// IDE-managed Components
        TGroupBox *GroupBoxCopies;
        TLabel *LabelCount;
        TCheckBox *CheckBoxCollate;
        TImage *ImageCollate;
        TButton *ButtonOk;
        TButton *ButtonCancel;
        TButton *ButtonHelp;
        TLabel *Label1;
        TLabel *Label2;
        TLabel *Label3;
        TUpDown *UpDownBegin;
        TUpDown *UpDownEnd;
        TUpDown *UpDownCount;
        TEdit *EditBegin;
        TEdit *EditEnd;
        TEdit *EditCount;
        void __fastcall FormShow(TObject *Sender);
        void __fastcall ButtonCancelClick(TObject *Sender);
        void __fastcall CheckBoxCollateClick(TObject *Sender);
        void __fastcall ButtonHelpClick(TObject *Sender);
        void __fastcall ButtonOkClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
 int MaxPage,FromPage,ToPage,Copies;  bool Collate;
 wchar_t FileHelp[256];
//процедура печати (вызывается по ОК, когда надо печатать):
        void __fastcall (*ToPrinter)();
             __fastcall TFormToPrn(TComponent* Owner);
        void __fastcall DrawCollate();
};
//---------------------------------------------------------------------------
extern PACKAGE TFormToPrn *FormToPrn;
//---------------------------------------------------------------------------
#endif
