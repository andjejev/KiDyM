//---------------------------------------------------------------------------

#ifndef SelDateH
#define SelDateH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------
class TFormDate:public TForm{
__published:	// IDE-managed Components
        TGroupBox *GroupBoxDate;
        TButton *ButtonOKDate;
        TButton *ButtonCancelDate;
        TDateTimePicker *DateTimePicker;
        TDateTimePicker *DateTimePickerEnd;
        TLabel *LabelDate;
        TLabel *LabelDateEnd;
        void __fastcall ButtonCancelDateClick(TObject *Sender);
        void __fastcall FormShow(TObject *Sender);
        void __fastcall ButtonOKDateClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
   int NY,NM,ND,NYmin,NMmin,NDmin,NYmax,NMmax,NDmax; bool Period;
 __fastcall TFormDate(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormDate *FormDate;
//---------------------------------------------------------------------------
#endif
