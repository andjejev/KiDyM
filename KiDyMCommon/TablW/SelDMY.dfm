object FormSelDMY: TFormSelDMY
  Left = 484
  Top = 108
  Width = 268
  Height = 81
  Caption = #1042#1099#1073#1077#1088#1080#1090#1077' '#1076#1072#1090#1091
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poOwnerFormCenter
  OnClose = FormClose
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object ButtonCancelDate: TButton
    Left = 0
    Top = 24
    Width = 60
    Height = 20
    Cancel = True
    Caption = #1054#1090#1084#1077#1085#1072
    TabOrder = 2
    OnClick = ButtonCancelDateClick
  end
  object ButtonOKDate: TButton
    Left = 196
    Top = 24
    Width = 60
    Height = 20
    Caption = 'OK'
    Default = True
    TabOrder = 1
    OnClick = ButtonOKDateClick
  end
  object DateTimePicker: TDateTimePicker
    Left = 0
    Top = 0
    Width = 257
    Height = 21
    CalAlignment = dtaLeft
    Date = 39008.9892111458
    Time = 39008.9892111458
    DateFormat = dfLong
    DateMode = dmComboBox
    Kind = dtkDate
    ParseInput = False
    TabOrder = 0
  end
end
