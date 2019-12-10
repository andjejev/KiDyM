//---------------------------------------------------------------------------

#ifndef AnimationH
#define AnimationH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TFormAnimation : public TForm
{
__published:	// IDE-managed Components
        TImage *Image;
        TImage *BuferImage;
        TTimer *Timer;
private:	// User declarations
public:		// User declarations
        __fastcall TFormAnimation(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormAnimation *FormAnimation;
//---------------------------------------------------------------------------
#endif
