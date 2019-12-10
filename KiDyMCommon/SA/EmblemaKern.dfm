object FormKidyMEmblema: TFormKidyMEmblema
  Left = 192
  Top = 114
  BorderStyle = bsNone
  Caption = 'FormKidyMEmblema'
  ClientHeight = 228
  ClientWidth = 370
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poDefault
  Visible = True
  OnKeyPress = FormKeyPress
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object ProgramIcon: TImage
    Left = 0
    Top = 0
    Width = 370
    Height = 228
    Align = alClient
  end
  object BufferIcon: TImage
    Left = 144
    Top = 56
    Width = 105
    Height = 105
    Visible = False
  end
  object Timer: TTimer
    Enabled = False
    Interval = 20
    OnTimer = TimerTimer
    Left = 304
    Top = 112
  end
end
