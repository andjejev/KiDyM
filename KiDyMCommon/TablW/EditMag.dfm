object FormEditMag: TFormEditMag
  Left = 192
  Top = 114
  Width = 326
  Height = 480
  Caption = #1054#1090#1088#1077#1076#1072#1082#1090#1080#1088#1091#1081#1090#1077' '#1089#1087#1080#1089#1086#1082
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -14
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnClose = FormClose
  OnCreate = FormCreate
  OnShow = FormShow
  PixelsPerInch = 120
  TextHeight = 16
  object Memo: TMemo
    Left = 0
    Top = 30
    Width = 318
    Height = 410
    Align = alClient
    Lines.Strings = (
      'Memo')
    TabOrder = 0
  end
  object ToolBar: TToolBar
    Left = 0
    Top = 0
    Width = 318
    Height = 30
    AutoSize = True
    ButtonHeight = 26
    Caption = 'ToolBar'
    TabOrder = 1
    object ComboBoxFont: TComboBox
      Left = 0
      Top = 3
      Width = 145
      Height = 24
      ItemHeight = 16
      TabOrder = 0
      Text = 'ComboBoxFont'
      OnSelect = ComboBoxFontSelect
    end
    object CSpinEditSize: TCSpinEdit
      Left = 145
      Top = 2
      Width = 48
      Height = 26
      MaxValue = 24
      MinValue = 6
      TabOrder = 1
      Value = 12
      OnChange = CSpinEditSizeChange
    end
  end
end
