object FormMagListBox: TFormMagListBox
  Left = 193
  Top = 132
  Width = 247
  Height = 253
  Caption = 'FormMagListBox'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -13
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnClose = FormClose
  OnShow = FormShow
  PixelsPerInch = 120
  TextHeight = 16
  object Panel: TPanel
    Left = 0
    Top = 0
    Width = 239
    Height = 213
    Align = alClient
    Caption = 'Panel'
    TabOrder = 0
    object ListBox: TListBox
      Left = 1
      Top = 1
      Width = 237
      Height = 211
      Align = alClient
      ItemHeight = 16
      TabOrder = 0
      OnDblClick = ListBoxDblClick
      OnKeyDown = ListBoxKeyDown
    end
  end
end
