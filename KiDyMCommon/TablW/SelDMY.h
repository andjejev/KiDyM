//---------------------------------------------------------------------------

#ifndef SelDMYH
#define SelDMYH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <Menus.hpp>
//---------------------------------------------------------------------------
class TFormSelDMY : public TForm
{
__published:	// IDE-managed Components
        TButton *ButtonCancelDate;
        TButton *ButtonOKDate;
        TDateTimePicker *DateTimePicker;
        void __fastcall ButtonCancelDateClick(TObject *Sender);
        void __fastcall ButtonOKDateClick(TObject *Sender);
        void __fastcall FormShow(TObject *Sender);
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
private:	// User declarations
public:		// User declarations
 int NY,NM,ND;
 __fastcall TFormSelDMY(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormSelDMY *FormSelDMY;
//---------------------------------------------------------------------------
#endif
