// CodeGear C++Builder
// Copyright (c) 1995, 2009 by Embarcadero Technologies, Inc.
// All rights reserved

// (DO NOT EDIT: machine generated header) 'Printers.pas' rev: 21.00

#ifndef PrintersHPP
#define PrintersHPP

#pragma delphiheader begin
#pragma option push
#pragma option -w-      // All warnings off
#pragma option -Vx      // Zero-length empty class member functions
#pragma pack(push,8)
#include <System.hpp>	// Pascal unit
#include <Sysinit.hpp>	// Pascal unit
#include <Windows.hpp>	// Pascal unit
#include <Winspool.hpp>	// Pascal unit
#include <Sysutils.hpp>	// Pascal unit
#include <Classes.hpp>	// Pascal unit
#include <Graphics.hpp>	// Pascal unit
#include <Forms.hpp>	// Pascal unit

//-- user supplied -----------------------------------------------------------
#if defined(_VCL_ALIAS_RECORDS)
#if !defined(UNICODE)
#pragma alias "@Printers@TPrinter@GetPrinterA$qqrpbt1t1rui"="@Printers@TPrinter@GetPrinter$qqrpbt1t1rui"
#pragma alias "@Printers@TPrinter@SetPrinterA$qqrpbt1t1ui"="@Printers@TPrinter@SetPrinter$qqrpbt1t1ui"
#else
#pragma alias "@Printers@TPrinter@GetPrinterW$qqrpbt1t1rui"="@Printers@TPrinter@GetPrinter$qqrpbt1t1rui"
#pragma alias "@Printers@TPrinter@SetPrinterW$qqrpbt1t1ui"="@Printers@TPrinter@SetPrinter$qqrpbt1t1ui"
#endif
#endif

namespace Printers
{
//-- type declarations -------------------------------------------------------
class DELPHICLASS EPrinter;
class PASCALIMPLEMENTATION EPrinter : public Sysutils::Exception
{
	typedef Sysutils::Exception inherited;
	
public:
	/* Exception.Create */ inline __fastcall EPrinter(const System::UnicodeString Msg) : Sysutils::Exception(Msg) { }
	/* Exception.CreateFmt */ inline __fastcall EPrinter(const System::UnicodeString Msg, System::TVarRec const *Args, const int Args_Size) : Sysutils::Exception(Msg, Args, Args_Size) { }
	/* Exception.CreateRes */ inline __fastcall EPrinter(int Ident)/* overload */ : Sysutils::Exception(Ident) { }
	/* Exception.CreateResFmt */ inline __fastcall EPrinter(int Ident, System::TVarRec const *Args, const int Args_Size)/* overload */ : Sysutils::Exception(Ident, Args, Args_Size) { }
	/* Exception.CreateHelp */ inline __fastcall EPrinter(const System::UnicodeString Msg, int AHelpContext) : Sysutils::Exception(Msg, AHelpContext) { }
	/* Exception.CreateFmtHelp */ inline __fastcall EPrinter(const System::UnicodeString Msg, System::TVarRec const *Args, const int Args_Size, int AHelpContext) : Sysutils::Exception(Msg, Args, Args_Size, AHelpContext) { }
	/* Exception.CreateResHelp */ inline __fastcall EPrinter(int Ident, int AHelpContext)/* overload */ : Sysutils::Exception(Ident, AHelpContext) { }
	/* Exception.CreateResFmtHelp */ inline __fastcall EPrinter(System::PResStringRec ResStringRec, System::TVarRec const *Args, const int Args_Size, int AHelpContext)/* overload */ : Sysutils::Exception(ResStringRec, Args, Args_Size, AHelpContext) { }
	/* Exception.Destroy */ inline __fastcall virtual ~EPrinter(void) { }
	
};


#pragma option push -b-
enum TPrinterState { psNoHandle, psHandleIC, psHandleDC };
#pragma option pop

#pragma option push -b-
enum TPrinterOrientation { poPortrait, poLandscape };
#pragma option pop

#pragma option push -b-
enum TPrinterCapability { pcCopies, pcOrientation, pcCollation };
#pragma option pop

typedef Set<TPrinterCapability, pcCopies, pcCollation>  TPrinterCapabilities;

class DELPHICLASS TPrinter;
class PASCALIMPLEMENTATION TPrinter : public System::TObject
{
	typedef System::TObject inherited;
	
private:
	Graphics::TCanvas* FCanvas;
	Classes::TStrings* FFonts;
	int FPageNumber;
	Classes::TStrings* FPrinters;
	int FPrinterIndex;
	System::UnicodeString FTitle;
	bool FPrinting;
	bool FAborted;
	TPrinterCapabilities FCapabilities;
	TPrinterState State;
	HDC DC;
	unsigned FPrinterHandle;
	_devicemodeW *DevMode;
	unsigned DeviceMode;
	void __fastcall SetState(TPrinterState Value);
	Graphics::TCanvas* __fastcall GetCanvas(void);
	int __fastcall GetNumCopies(void);
	Classes::TStrings* __fastcall GetFonts(void);
	HDC __fastcall GetHandle(void);
	TPrinterOrientation __fastcall GetOrientation(void);
	int __fastcall GetPageHeight(void);
	int __fastcall GetPageWidth(void);
	int __fastcall GetPrinterIndex(void);
	void __fastcall SetPrinterCapabilities(int Value);
	void __fastcall SetPrinterIndex(int Value);
	Classes::TStrings* __fastcall GetPrinters(void);
	void __fastcall SetNumCopies(int Value);
	void __fastcall SetOrientation(TPrinterOrientation Value);
	void __fastcall SetToDefaultPrinter(void);
	void __fastcall CheckPrinting(bool Value);
	void __fastcall FreePrinters(void);
	void __fastcall FreeFonts(void);
	
public:
	__fastcall TPrinter(void);
	__fastcall virtual ~TPrinter(void);
	void __fastcall Abort(void);
	void __fastcall BeginDoc(void);
	void __fastcall EndDoc(void);
	void __fastcall NewPage(void);
	void __fastcall Refresh(void);
	void __fastcall GetPrinter(System::WideChar * ADevice, System::WideChar * ADriver, System::WideChar * APort, unsigned &ADeviceMode);
	void __fastcall SetPrinter(System::WideChar * ADevice, System::WideChar * ADriver, System::WideChar * APort, unsigned ADeviceMode);
	__property bool Aborted = {read=FAborted, nodefault};
	__property Graphics::TCanvas* Canvas = {read=GetCanvas};
	__property TPrinterCapabilities Capabilities = {read=FCapabilities, nodefault};
	__property int Copies = {read=GetNumCopies, write=SetNumCopies, nodefault};
	__property Classes::TStrings* Fonts = {read=GetFonts};
	__property HDC Handle = {read=GetHandle, nodefault};
	__property TPrinterOrientation Orientation = {read=GetOrientation, write=SetOrientation, nodefault};
	__property int PageHeight = {read=GetPageHeight, nodefault};
	__property int PageWidth = {read=GetPageWidth, nodefault};
	__property int PageNumber = {read=FPageNumber, nodefault};
	__property int PrinterIndex = {read=GetPrinterIndex, write=SetPrinterIndex, nodefault};
	__property bool Printing = {read=FPrinting, nodefault};
	__property Classes::TStrings* Printers = {read=GetPrinters};
	__property System::UnicodeString Title = {read=FTitle, write=FTitle};
         _devicemodeW *GetDevMode(){ return DevMode; };//добавлено мной 22.12.2013
};


//-- var, const, procedure ---------------------------------------------------
extern PACKAGE void __fastcall AssignPrn(System::TextFile &F);
extern PACKAGE TPrinter* __fastcall Printer(void);
extern PACKAGE TPrinter* __fastcall SetPrinter(TPrinter* NewPrinter);

}	/* namespace Printers */
using namespace Printers;
#pragma pack(pop)
#pragma option pop

#pragma delphiheader end.
//-- end unit ----------------------------------------------------------------
#endif	// PrintersHPP
