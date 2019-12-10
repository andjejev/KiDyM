//---------------------------------------------------------------------------

#ifndef EditTextButtonH
#define EditTextButtonH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ImgList.hpp>
#include <ToolWin.hpp>
//---------------------------------------------------------------------------
class TEditTextButton {
 public:
  AnsiString Text;
  wchar_t NameFile[256];
  TEditTextButton(){ Text=L""; NameFile[0]='\0';}
  void LoadDosFile();
  void SaveDosFile(){};
};
//---------------------------------------------------------------------------
class TFormEditTextButton:public TForm{
__published:	// IDE-managed Components
        TRichEdit *RichEditText;
        TToolBar *ToolBar1;
        TToolButton *ToolButtonNew;
        TToolButton *ToolButtonSave;
        TImageList *ImageListEdtitTextButton;
        TToolButton *ToolButton4;
        TToolButton *ToolButtonCut;
        TToolButton *ToolButtonCopy;
        TToolButton *ToolButtonIns;
        TToolButton *ToolButton10;
        TToolButton *ToolButtonHelp;
        TToolButton *ToolButton1;
        TToolButton *ToolButtonExit;
        void __fastcall FormShow(TObject *Sender);
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall ToolButtonHelpClick(TObject *Sender);
        void __fastcall ToolButtonExitClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TFormEditTextButton(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormEditTextButton *FormEditTextButton;
//---------------------------------------------------------------------------
#endif
