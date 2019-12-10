//---------------------------------------------------------------------------

#ifndef EditListPrnH
#define EditListPrnH
//---------------------------------------------------------------------------
#include "ToPrn.h"
#include <Controls.hpp>
#include <ComCtrls.hpp>
#include <ToolWin.hpp>
#include <Forms.hpp>
#include <Classes.hpp>
#include <ImgList.hpp>
//---------------------------------------------------------------------------
class TFormEditListPrn : public TForm{
__published:	// IDE-managed Components
        TToolBar *ToolBar;
        TImageList *ImageList;
        TToolButton *ToolButton2;
        TToolButton *ToolButtonSave;
        TToolButton *ToolButton4;
        TToolButton *ToolButtonHelp;
        TToolButton *ToolButton6;
        TToolButton *ToolButtonExit;
        TStatusBar *StatusBar;
        TToolButton *ToolButtonPrnOpt;
        TToolButton *ToolButtonToPrn;
        TTabControl *TabControlPages;
        TScrollBox *ScrollBox;
        void __fastcall ToolButtonHelpClick(TObject *Sender);
        void __fastcall ToolButtonExitClick(TObject *Sender);
        void __fastcall FormShow(TObject *Sender);
        void __fastcall TabControlPagesChange(TObject *Sender);
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall ScrollBarPageScroll(TObject *Sender,
          TScrollCode ScrollCode, int &ScrollPos);
        void __fastcall TabControlPagesChanging(TObject *Sender,
          bool &AllowChange);
//        void __fastcall ToolButtonPrnClick(TObject *Sender);
        void __fastcall ScrollBarPageKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
        void __fastcall ToolButtonPrnOptClick(TObject *Sender);
        void __fastcall ToolButtonToPrnClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
 TImage *ImagePage;
 TPrinter *Prn; double Ktw;
 wchar_t FileHelp[256];
 double Kx,Ky,//������������ �������� �� ���������� � �������� �������
  Mp;//������� �� ��������
 int
  MarginLeft,MarginRight,MarginTop,MarginBot,//������� � ��������
  LPxscreen,LPyscreen,//���������� ������
  LPx,LPy,X,Y,//���������� �������� �� ����������� � ���������
//������ � �������� ���������, �����, �������, �������,
//������� ������ �� ���., ������:
  Hh,Hfh,HSh,Ht,Hp,hp,H;
//����� ��������� ��� ������ ��������,������ ����.�����, ��������,
//��������, ������� �����
//�������������� ����� �������, ������� ������ ��������, �������:
 int ColCount,Lfs,*Li,*LiDo,*LiPo,Lt,lp,L,
//�����. ����� � ������� ������ (�� ��������),
//����������� ��������,����� ����� �������,����� ������ �����
//� ��������� � � �������, ����� ������ �� �����������:
  Ks,Kfs,KP,Knfh,Knh,Knp,Kp,
//��� ������� - ����, �����-�������, ������-�������, ���������
//��������� ������� ������
  VidTablPrn,Y0;
 int Kss,Kps,Hsp,Dl,PhW,PhH,PhL,PhT,PhR,PhB,PhWmm,PhHmm,PhLmm,PhTmm,PhRmm,
  PhBmm,Wmm,Hmm;
 Magazine *MStlb;
      __fastcall TFormEditListPrn(TComponent* Owner);
 void __fastcall DrawOnePage(TCanvas *Canvas);
 void __fastcall DrawFullHead(TCanvas *Canvas);
 void __fastcall DrawHead(TCanvas *Canvas,int W);
 void __fastcall DrawManyPage(TCanvas *Canvas,int Np);
 int  __fastcall DrawBroadTabl(TCanvas *Canvas,int Hfh,int Hp,int Nsn,int Nsk,
         int HIt,int NStartPage,int Np);
 void __fastcall DrawHugeTabl(TCanvas *Canvas,int Np);
 int  __fastcall Mashtabs(int Hp);
 int  __fastcall CountPagesBroadTabl(int Hp,int HIt);
 void __fastcall CountPagesHugeTabl();
 void __fastcall ToPrint();
};
//---------------------------------------------------------------------------
extern PACKAGE TFormEditListPrn *FormEditListPrn;
//---------------------------------------------------------------------------
#endif
