//---------------------------------------------------------------------------

#ifndef ParamPrintH
#define ParamPrintH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <Dialogs.hpp>
#include <ImgList.hpp>
#include <ToolWin.hpp>
#include <Mask.hpp>
#include <Printers.hpp>
#include <System.ImageList.hpp>
//---------------------------------------------------------------------------
class TFormParamPrint : public TForm
{
__published:	// IDE-managed Components
		TLabel *LabelInt;
		TEdit *EditInt;
		TGroupBox *GroupBoxMargin;
		TLabel *LabelLeft;
		TLabel *LabelRight;
		TLabel *LabelTop;
		TLabel *LabelBottom;
		TButton *ButtonCancel;
		TButton *ButtonOK;
		TButton *ButtonFont;
		TButton *ButtonPrint;
		TFontDialog *FontDialog;
		TImageList *ImageList;
		TPrinterSetupDialog *PrinterSetupDialog;
		TToolBar *ToolBar;
		TToolButton *ToolButtonHelp;
		TToolButton *ToolButtonExit;
		TUpDown *UpDownTop;
		TUpDown *UpDownLeft;
		TUpDown *UpDownBottom;
		TUpDown *UpDownRight;
		TEdit *EditTop;
		TEdit *EditLeft;
		TEdit *EditRight;
		TEdit *EditBottom;
		TCheckBox *CheckBoxPrinters;
		TButton *Button1;
		void __fastcall ButtonCancelClick(TObject *Sender);
		void __fastcall ButtonPrintClick(TObject *Sender);
		void __fastcall ButtonFontClick(TObject *Sender);
		void __fastcall ButtonOKClick(TObject *Sender);
		void __fastcall ToolButtonHelpClick(TObject *Sender);
		void __fastcall ToolButtonExitClick(TObject *Sender);
		void __fastcall EditTopKeyDown(TObject *Sender, WORD &Key,
		  TShiftState Shift);
		void __fastcall EditLeftKeyDown(TObject *Sender, WORD &Key,
		  TShiftState Shift);
		void __fastcall EditBottomKeyDown(TObject *Sender, WORD &Key,
		  TShiftState Shift);
		void __fastcall EditRightKeyDown(TObject *Sender, WORD &Key,
		  TShiftState Shift);
		void __fastcall EditRightExit(TObject *Sender);
		void __fastcall EditTopExit(TObject *Sender);
		void __fastcall EditLeftExit(TObject *Sender);
		void __fastcall EditBottomExit(TObject *Sender);
		void __fastcall EditBottomKeyPress(TObject *Sender, wchar_t &Key);
		void __fastcall EditRightKeyPress(TObject *Sender, wchar_t &Key);
		void __fastcall EditTopKeyPress(TObject *Sender, wchar_t &Key);
		void __fastcall EditLeftKeyPress(TObject *Sender, wchar_t &Key);
		void __fastcall FormShow(TObject *Sender);
		void __fastcall Button1Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
 bool Change; wchar_t FileHelp[256]; TPrinter *Prn;
 int PhW,PhH,PhL,PhT,PhWmm,PhHmm,PhLmm,PhTmm,PhRmm,PhBmm,Wmm,Hmm,
  LPx,LPy,FontSize,MarginLeftmm,MarginRightmm,MarginTopmm,
  MarginBotmm;
 wchar_t *FilePrn,FontName[256],PrintName[80],PaperName[80];
 bool OrientKnig,Podch,IsWin2000;
 TColor FontColor;
 double Interval;
	   __fastcall TFormParamPrint(TComponent* Owner);
 void  __fastcall ReadOptPrn(void);
 void  __fastcall SaveOptPrn(void);
 void  __fastcall TakeParam(void);
 void  __fastcall Defines();
 bool  __fastcall IsSetOrient(TPrinter *P);
// wchar_t *__fastcall WhatDefPrn(wchar_t *S);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormParamPrint *FormParamPrint;
//---------------------------------------------------------------------------
#endif
