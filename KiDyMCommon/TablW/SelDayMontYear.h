//---------------------------------------------------------------------------

#ifndef SelDayMontYearH
#define SelDayMontYearH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------
class TFormSelDMY : public TForm
{
__published:	// IDE-managed Components
        TButton *ButtonCancelDate;
        TButton *ButtonOKDate;
        TDateTimePicker *DateTimePicker;
        void __fastcall FormCreate(TObject *Sender);
        void __fastcall ButtonCancelDateClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
 int NY,NM,ND;
 __fastcall TFormSelDMY(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormSelDMY *FormSelDMY;
//---------------------------------------------------------------------------
#endif
