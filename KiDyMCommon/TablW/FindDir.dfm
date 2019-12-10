object FormFindDir: TFormFindDir
  Left = 554
  Top = 147
  BorderStyle = bsDialog
  Caption = #1054#1090#1082#1088#1086#1081#1090#1077' '#1085#1091#1078#1085#1091#1102' '#1087#1072#1087#1082#1091' '#1076#1074#1086#1081#1085#1099#1084' '#1097#1077#1083#1095#1082#1086#1084' '#1084#1099#1096#1080
  ClientHeight = 368
  ClientWidth = 530
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -13
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poMainFormCenter
  PixelsPerInch = 120
  TextHeight = 16
  object DriveComboBox: TDriveComboBox
    Left = 9
    Top = 9
    Width = 168
    Height = 22
    DirList = DirectoryListBox
    TabOrder = 1
  end
  object DirectoryListBox: TDirectoryListBox
    Left = 9
    Top = 41
    Width = 400
    Height = 320
    ItemHeight = 16
    TabOrder = 0
  end
  object ButtonOK: TButton
    Left = 432
    Top = 41
    Width = 75
    Height = 24
    Caption = 'OK'
    Default = True
    TabOrder = 2
    OnClick = ButtonOKClick
  end
  object ButtonCancel: TButton
    Left = 441
    Top = 336
    Width = 73
    Height = 25
    Cancel = True
    Caption = #1054#1090#1084#1077#1085#1072
    TabOrder = 3
    OnClick = ButtonCancelClick
  end
end
