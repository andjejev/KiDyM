object FormDate: TFormDate
  Left = 235
  Top = 132
  BorderStyle = bsNone
  Caption = '�������� ����'
  ClientHeight = 197
  ClientWidth = 304
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -13
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  Icon.Data = {
    0000010001002020100000000000E80200001600000028000000200000004000
    0000010004000000000080020000000000000000000000000000000000000000
    0000000080000080000000808000800000008000800080800000C0C0C0008080
    80000000FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF004444
    4444444444444444444444444444033333333333333333333333333333300383
    8383838383838383838383838380009999999999300000399999999999000009
    99999930000000003999999990000000999330000FC0F0000039999900000000
    3300000F7C70FFF000003390000000000000F7CFC0C0F0F7F440000700000000
    0F7CFCFCFC0F7FFFF7664000870000045FCFCFCFCFE0FFFFFFF766440000000D
    DFFCFCFCFC70FFFFFFFF78748000000DDFCFCFCFCFE0FFFFFFFFF8F48000000D
    DFFCFCFCFC70FFFFFFFFF8F480000003BFCFCFCFCFE0FF7FFFFFF8F48000000B
    BFFCFCFCFC70FFF77FFFF8F48000000BBFCFCFCFC0E0F0FFF77FF8F480000003
    BFFCFCFC7C0F7FFFFFF7F8F480000006EFCFCFCFCFE0FFFFFFFFF8F48000000E
    EFFCFCFCFC70FFFFFFFFF8F48000000EEFCFCFCFCFE0FFFFFFFFF8F480000006
    EFFCFCFCFC70FFFFFFFFF8F4800000095FCFCFCFCFE0FF7FFFFFF8F480000009
    1FFCFCFCFC70FFF77FFFF8F4800000051FCFCFCFCFE0FFFFF77FF8F48000000A
    2FFCFCFCC070F0FFFFF7F8F48000000AAFCFCFCCC70F7FFFFFFFF8F48000000A
    AFFCFCFCFE777FFFFFFFF8F480000002AFCFCFCFE77077FFFFFFF8F48000000C
    CFFCFCF770000077FFFFF8F48000000C4FCFC7700000000077FFF8F48000000C
    CFFE7000000000000077F8748800000C47700000000000000000777000000000
    0000000000000000000080000001C0000003E0000007F000000FF000000FC000
    0003C0000003C0000003C0000003C0000003C0000003C0000003C0000003C000
    0003C0000003C0000003C0000003C0000003C0000003C0000003C0000003C000
    0003C0000003C0000003C0010003C007C003C01FF003C07FFC03E1FFFF1F}
  OldCreateOrder = False
  OnShow = FormShow
  PixelsPerInch = 120
  TextHeight = 16
  object GroupBoxDate: TGroupBox
    Left = 0
    Top = 0
    Width = 304
    Height = 197
    Align = alClient
    Caption = ' �������� ���� '
    TabOrder = 0
    object LabelDate: TLabel
      Left = 8
      Top = 16
      Width = 111
      Height = 16
      Caption = '������ �������'
    end
    object LabelDateEnd: TLabel
      Left = 8
      Top = 72
      Width = 101
      Height = 16
      Caption = '����� �������'
    end
    object ButtonOKDate: TButton
      Left = 214
      Top = 40
      Width = 75
      Height = 25
      Caption = 'OK'
      Default = True
      TabOrder = 2
      OnClick = ButtonOKDateClick
    end
    object ButtonCancelDate: TButton
      Left = 214
      Top = 96
      Width = 75
      Height = 25
      Cancel = True
      Caption = '������'
      TabOrder = 3
      OnClick = ButtonCancelDateClick
    end
    object DateTimePicker: TDateTimePicker
      Left = 8
      Top = 40
      Width = 186
      Height = 24
      CalAlignment = dtaLeft
      Date = 37890.70330625
      Time = 37890.70330625
      DateFormat = dfShort
      DateMode = dmComboBox
      Kind = dtkDate
      ParseInput = False
      TabOrder = 0
    end
    object DateTimePickerEnd: TDateTimePicker
      Left = 8
      Top = 96
      Width = 186
      Height = 24
      CalAlignment = dtaLeft
      Date = 37890.70330625
      Time = 37890.70330625
      DateFormat = dfShort
      DateMode = dmComboBox
      Kind = dtkDate
      ParseInput = False
      TabOrder = 1
      Visible = False
    end
  end
end
