//---------------------------------------------------------------------------

#ifndef ColorsH
#define ColorsH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Dialogs.hpp>
//---------------------------------------------------------------------------
class TFormColors : public TForm{
__published:	// IDE-managed Components
        TGroupBox *GroupBoxColors;
        TImage *ImageHead;
        TImage *ImageOs;
        TImage *ImageTabl;
        TImage *ImageSetka;
        TImage *ImageDigit;
        TImage *ImageRamka;
        TImage *ImagePhon;
        TImage *ImageGr;
        TImage *ImageGr2;
        TImage *ImageGr3;
        TImage *ImageGr4;
        TImage *ImageGr5;
        TButton *ButtonHead;
        TButton *ButtonOs;
        TButton *ButtonTabl;
        TButton *ButtonSetka;
        TButton *ButtonDigit;
        TButton *ButtonRamka;
        TButton *ButtonPhon;
        TButton *ButtonGr;
        TButton *ButtonGr2;
        TButton *ButtonGr3;
        TButton *ButtonGr4;
        TButton *ButtonGr5;
        TButton *ButtonOK;
        TButton *ButtonCancel;
        TColorDialog *ColorDialog;
        TCheckBox *CheckBoxColorPrint;
        TCheckBox *CheckBoxPerspect;
        TCheckBox *CheckBoxColorBMP;
        void __fastcall ButtonHeadClick(TObject *Sender);
        void __fastcall ButtonOsClick(TObject *Sender);
        void __fastcall ButtonDigitClick(TObject *Sender);
        void __fastcall ButtonTablClick(TObject *Sender);
        void __fastcall ButtonSetkaClick(TObject *Sender);
        void __fastcall ButtonRamkaClick(TObject *Sender);
        void __fastcall ButtonGrClick(TObject *Sender);
        void __fastcall ButtonGr2Click(TObject *Sender);
        void __fastcall ButtonGr3Click(TObject *Sender);
        void __fastcall ButtonGr4Click(TObject *Sender);
        void __fastcall ButtonGr5Click(TObject *Sender);
        void __fastcall ButtonPhonClick(TObject *Sender);
        void __fastcall ButtonCancelClick(TObject *Sender);
        void __fastcall ButtonOKClick(TObject *Sender);
        void __fastcall FormShow(TObject *Sender);
private:	// User declarations
 TColor ColorHead,ColorOs,ColorTabl,ColorSetka,ColorDigit,ColorRamka,
  ColorPhon,ColorGr,ColorGr2,ColorGr3,ColorGr4,ColorGr5,
  *ch,*co,*ct,*cs,*cd,*cr,*cp,*cg,*c2,*c3,*c4,*c5,*cb;
 TFont *Font; bool *cP,*Pr,*cB;
public:		// User declarations
   __fastcall void GetColors(TColor *CH,TColor *CO,TColor *CT,TColor *CS,
      TColor *CD,TColor *CR,TColor *CP,TColor *CG,TColor *C2,TColor *C3,
      TColor *C4,TColor *C5,TFont *F,bool *Cp,bool *Cb,bool *Prsp){
    ch=CH; co=CO; ct=CT; cs=CS; cd=CD; cr=CR; cp=CP; cg=CG;
    c2=C2; c3=C3; c4=C4; c5=C5; Font=F; cP=Cp; cB=Cb; Pr=Prsp;
    ColorHead=*CH; ColorOs=*CO; ColorTabl=*CT; ColorSetka=*CS;
    ColorDigit=*CD; ColorRamka=*CR; ColorPhon=*CP; ColorGr=*CG;
    ColorGr2=*C2; ColorGr3=*C3; ColorGr4=*C4; ColorGr5=*C5;
    CheckBoxColorPrint->Checked=*Cp; CheckBoxPerspect->Checked=*Prsp;
    CheckBoxColorBMP->Checked=*cB;
   };
   __fastcall void SetColors(){
    *ch=ColorHead; *co=ColorOs; *ct=ColorTabl; *cs=ColorSetka;
    *cd=ColorDigit; *cr=ColorRamka; *cp=ColorPhon; *cg=ColorGr;
    *c2=ColorGr2; *c3=ColorGr3; *c4=ColorGr4; *c5=ColorGr5;
    *cP=CheckBoxColorPrint->Checked; *Pr=CheckBoxPerspect->Checked;
    *cB=CheckBoxColorBMP->Checked;
   };
   __fastcall void ShowColorHead();
   __fastcall void ShowColorOs();
   __fastcall void ShowColorTabl();
   __fastcall void ShowColorSetka();
   __fastcall void ShowColorDigit();
   __fastcall void ShowColorRamka();
   __fastcall void ShowColorPhon();
   __fastcall void ShowColorGr(TImage *Image,TColor Color);
   __fastcall TFormColors(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormColors *FormColors;
//---------------------------------------------------------------------------
#endif
