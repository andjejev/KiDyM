object FormMultiSelect: TFormMultiSelect
  Left = 303
  Top = 116
  BorderStyle = bsDialog
  Caption = 'FormMultiSelect'
  ClientHeight = 151
  ClientWidth = 207
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -13
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poMainFormCenter
  OnClose = FormClose
  OnShow = FormShow
  PixelsPerInch = 120
  TextHeight = 16
  object CheckListBox: TCheckListBox
    Left = 0
    Top = 8
    Width = 121
    Height = 137
    Flat = False
    ItemHeight = 16
    TabOrder = 0
    OnKeyDown = CheckListBoxKeyDown
  end
  object ButtonOK: TButton
    Left = 128
    Top = 8
    Width = 75
    Height = 25
    Caption = 'OK'
    Default = True
    TabOrder = 1
    OnClick = ButtonOKClick
  end
  object ButtonCancel: TButton
    Left = 128
    Top = 40
    Width = 75
    Height = 25
    Cancel = True
    Caption = #1054#1090#1084#1077#1085#1072
    TabOrder = 2
    OnClick = ButtonCancelClick
  end
  object ButtonAll: TButton
    Left = 128
    Top = 72
    Width = 75
    Height = 25
    Caption = #1042#1089#1077
    TabOrder = 3
    OnClick = ButtonAllClick
  end
end
