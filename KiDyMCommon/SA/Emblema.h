//---------------------------------------------------------------------------

#ifndef EmblemaH
#define EmblemaH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TFormKiyMEmblema : public TForm
{
__published:	// IDE-managed Components
        TImage *ProgramIcon;
        TImage *BuferIcon;
        TTimer *Timer;
private:	// User declarations
public:		// User declarations
        __fastcall TFormKiyMEmblema(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormKiyMEmblema *FormKiyMEmblema;
//---------------------------------------------------------------------------
#endif
