#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Media/GDIEngine.h"
#include "Media/Printer.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include <windows.h>

namespace Media
{
	class GDPrintDocument : public Media::PrintDocument
	{
	private:
		void *hdcPrinter;
		UInt8 *devMode;
		NN<PrintHandler> hdlr;
		NN<Media::GDIEngine> eng;
		Optional<Text::String> docName;
		Bool started;
		Bool running;
		PageOrientation po;
		Double paperWidth;
		Double paperHeight;

		static UInt32 __stdcall PrintThread(AnyType userObj);
	public:
		GDPrintDocument(NN<Text::String> printerName, UInt8 *devMode, NN<Media::GDIEngine> eng, NN<PrintHandler> hdlr);
		virtual ~GDPrintDocument();

		Bool IsError();

		virtual void SetDocName(Optional<Text::String> docName);
		virtual void SetDocName(Text::CString docName);
		virtual void SetNextPagePaperSizeMM(Double width, Double height);
		virtual void SetNextPageOrientation(PageOrientation po);
		void Start();
		virtual void WaitForEnd();
	};
}

UInt32 __stdcall Media::GDPrintDocument::PrintThread(AnyType userObj)
{
	NN<Media::GDPrintDocument> me = userObj.GetNN<Media::GDPrintDocument>();
	NN<Media::GDIImage> img;
	Bool hasMorePage;
	DEVMODEW *devMode = (DEVMODEW*)me->devMode;

	while (true)
	{
		Int32 paperWidth;
		Int32 paperHeight;
		devMode->dmPaperWidth = (Int16)Double2Int32(me->paperWidth * 10.0);
		devMode->dmPaperLength = (Int16)Double2Int32(me->paperHeight * 10.0);
		if (me->po == PageOrientation::Landscape)
		{
			devMode->dmOrientation = DMORIENT_LANDSCAPE;
			paperWidth = devMode->dmPaperLength;
			paperHeight = devMode->dmPaperWidth;
		}
		else
		{
			devMode->dmOrientation = DMORIENT_PORTRAIT;
			paperWidth = devMode->dmPaperWidth;
			paperHeight = devMode->dmPaperLength;
		}
		devMode->dmFields |= DM_PAPERLENGTH | DM_PAPERWIDTH | DM_ORIENTATION;
		ResetDCW((HDC)me->hdcPrinter, devMode);
		NEW_CLASSNN(img, Media::GDIImage(me->eng, Math::Coord2D<OSInt>(0, 0), Math::Size2D<UOSInt>((UOSInt)MulDiv32(MulDiv32(paperWidth, 100, devMode->dmScale), devMode->dmPrintQuality, 254), (UOSInt)MulDiv32(MulDiv32(paperHeight, 100, devMode->dmScale), devMode->dmPrintQuality, 254)), 32, 0, 0, me->hdcPrinter, Media::AT_IGNORE_ALPHA));
		img->SetHDPI(devMode->dmPrintQuality);
		img->SetVDPI(devMode->dmPrintQuality);
		StartPage((HDC)me->hdcPrinter);
		hasMorePage = me->hdlr->PrintPage(img);
		EndPage((HDC)me->hdcPrinter);
		img.Delete();
		if (!hasMorePage)
			break;
	}
	EndDoc((HDC)me->hdcPrinter);

	me->hdlr->EndPrint(me);

	me->running = false;
	return 0;
}

Media::GDPrintDocument::GDPrintDocument(NN<Text::String> printerName, UInt8 *devMode, NN<Media::GDIEngine> eng, NN<PrintHandler> hdlr)
{
	this->devMode = devMode;
	this->eng = eng;
	this->hdlr = hdlr;
	this->docName = 0;
	this->started = false;
	this->running = false;
	UnsafeArray<const WChar> wptr = Text::StrToWCharNew(printerName->v);
	this->hdcPrinter = CreateDCW(L"WINSPOOL", wptr.Ptr(), 0, (DEVMODEW*)this->devMode);
	Text::StrDelNew(wptr);
	DEVMODEW *devM = (DEVMODEW*)devMode;
	if (devM->dmOrientation == DMORIENT_LANDSCAPE)
	{
		this->po = PageOrientation::Landscape;
	}
	else
	{
		this->po = PageOrientation::Portrait;
	}
	this->paperWidth = devM->dmPaperWidth * 0.1;
	this->paperHeight = devM->dmPaperLength * 0.1;
}

Media::GDPrintDocument::~GDPrintDocument()
{
	WaitForEnd();
	if (this->hdcPrinter)
	{
		DeleteDC((HDC)this->hdcPrinter);
		this->hdcPrinter = 0;
	}
	OPTSTR_DEL(this->docName);
}

Bool Media::GDPrintDocument::IsError()
{
	return this->hdcPrinter == 0;
}

void Media::GDPrintDocument::SetDocName(Optional<Text::String> docName)
{
	OPTSTR_DEL(this->docName);
	this->docName = Text::String::CopyOrNull(docName);
}

void Media::GDPrintDocument::SetDocName(Text::CString docName)
{
	OPTSTR_DEL(this->docName);
	this->docName = Text::String::NewOrNull(docName);
}

void Media::GDPrintDocument::SetNextPagePaperSizeMM(Double width, Double height)
{
	this->paperWidth = width;
	this->paperHeight = height;
}

void Media::GDPrintDocument::SetNextPageOrientation(PageOrientation po)
{
	this->po = po;
}

void Media::GDPrintDocument::Start()
{
	if (this->hdcPrinter == 0)
		return;
	if (this->started)
		return;

	if (this->hdlr->BeginPrint(*this))
	{
		UnsafeArrayOpt<const WChar> wptr = 0;
		UnsafeArray<const WChar> nnwptr;
		DOCINFOW docInfo;
		NN<Text::String> s;

		docInfo.cbSize = sizeof(DOCINFOW);
		if (this->docName.SetTo(s))
		{
			nnwptr = Text::StrToWCharNew(s->v);
			docInfo.lpszDocName = nnwptr.Ptr();
			wptr = nnwptr;
		}
		else
		{
			docInfo.lpszDocName = L"Untitled";
		}
		docInfo.lpszOutput = 0;
		docInfo.lpszDatatype = 0;
		docInfo.fwType = 0;
		if (StartDocW((HDC)this->hdcPrinter, &docInfo) > 0)
		{
			this->started = true;
			this->running = true;
			Sync::ThreadUtil::Create(PrintThread, this);
		}
		else
		{
			this->hdlr->EndPrint(*this);
		}

		if (wptr.SetTo(nnwptr))
		{
			Text::StrDelNew(nnwptr);
			wptr = 0;
		}
	}
}

void Media::GDPrintDocument::WaitForEnd()
{
	if (this->hdcPrinter == 0)
		return;
	while (this->running)
	{
		Sync::SimpleThread::Sleep(10);
	}
}


UOSInt Media::Printer::GetPrinterCount()
{
	UInt8 buff[4096];
	UInt32 cbNeeded;
	UInt32 cReturned = 0;
	EnumPrintersW(PRINTER_ENUM_LOCAL, 0, 1, buff, 4096, (LPDWORD)&cbNeeded, (LPDWORD)&cReturned);
	return cReturned;
}

UnsafeArrayOpt<UTF8Char> Media::Printer::GetPrinterName(UnsafeArray<UTF8Char> sbuff, UOSInt index)
{
	UInt8 buff[4096];
	UInt32 cbNeeded;
	UInt32 cReturned = 0;
	EnumPrintersW(PRINTER_ENUM_LOCAL, 0, 1, buff, 4096, (LPDWORD)&cbNeeded, (LPDWORD)&cReturned);
	if (index > cReturned)
		return 0;
	_PRINTER_INFO_1W *info = (_PRINTER_INFO_1W*)buff;
	return Text::StrWChar_UTF8(sbuff, info[index].pName);
}

Media::Printer *Media::Printer::SelectPrinter(Optional<ControlHandle> hWnd)
{
	LPPRINTPAGERANGE pPageRanges = NULL;
	pPageRanges = (LPPRINTPAGERANGE) GlobalAlloc(GPTR, 10 * sizeof(PRINTPAGERANGE));
	if (!pPageRanges)
	{
		return 0;
	}

	Media::Printer *printer = 0;

	PRINTDLGEX pdx;
    pdx.lStructSize = sizeof(PRINTDLGEX);
    pdx.hwndOwner = (HWND)hWnd.OrNull();
    pdx.hDevMode = NULL;
    pdx.hDevNames = NULL;
    pdx.hDC = NULL;
    pdx.Flags = PD_RETURNDC | PD_COLLATE;
    pdx.Flags2 = 0;
    pdx.ExclusionFlags = 0;
    pdx.nPageRanges = 0;
    pdx.nMaxPageRanges = 10;
    pdx.lpPageRanges = pPageRanges;
    pdx.nMinPage = 1;
    pdx.nMaxPage = 1000;
    pdx.nCopies = 1;
    pdx.hInstance = 0;
    pdx.lpPrintTemplateName = NULL;
    pdx.lpCallback = NULL;
    pdx.nPropertyPages = 0;
    pdx.lphPropertyPages = NULL;
    pdx.nStartPage = START_PAGE_GENERAL;
    pdx.dwResultAction = 0;


	HRESULT hResult = PrintDlgExW((LPPRINTDLGEXW)&pdx);
	if ((hResult == S_OK) && pdx.dwResultAction == PD_RESULT_PRINT) 
	{
		DEVNAMES *devNames = (DEVNAMES*)GlobalLock(pdx.hDevNames);
		void *devMode = GlobalLock(pdx.hDevMode);
		const WChar *devName = &((const WChar*)devNames)[devNames->wDeviceOffset];
		NEW_CLASS(printer, Media::Printer(devName, (UInt8*)devMode, GlobalSize(pdx.hDevMode)));
		GlobalUnlock(pdx.hDevMode);
		GlobalUnlock(pdx.hDevNames);
	}

	if (pdx.hDevMode != NULL) 
		GlobalFree(pdx.hDevMode); 
	if (pdx.hDevNames != NULL) 
		GlobalFree(pdx.hDevNames); 
	if (pdx.lpPageRanges != NULL)
		GlobalFree(pPageRanges);

	if (pdx.hDC != NULL)
	{
		DeleteDC(pdx.hDC);
	}

	return printer;
}

Media::Printer::Printer(const WChar *printerName, UInt8 *devMode, UOSInt devModeSize)
{
	this->devMode = 0;
	this->hPrinter = 0;
	this->printerName = Text::String::NewNotNull(printerName);
	if (OpenPrinterW((LPWSTR)printerName, &hPrinter, 0) == 0)
	{
		return;
	}
	this->devMode = MemAlloc(UInt8, devModeSize);
	MemCopyNO(this->devMode, devMode, devModeSize);
}

Media::Printer::Printer(NN<Text::String> printerName)
{
	this->devMode = 0;
	this->hPrinter = 0;
	this->printerName = printerName->Clone();
	UnsafeArray<const WChar> wptr = Text::StrToWCharNew(printerName->v);
	if (OpenPrinterW((LPWSTR)wptr.Ptr(), &hPrinter, 0) == 0)
	{
		Text::StrDelNew(wptr);
		return;
	}

	Int32 lReturn = DocumentPropertiesW(0, (HANDLE)this->hPrinter, (LPWSTR)wptr.Ptr(), (DEVMODEW*)this->devMode, 0, 0);
	UOSInt size = (UOSInt)lReturn;
	this->devMode = MemAlloc(UInt8, size);
	lReturn = DocumentPropertiesW(0, (HANDLE)this->hPrinter, (LPWSTR)wptr.Ptr(), (DEVMODEW*)this->devMode, 0, DM_OUT_BUFFER);
	Text::StrDelNew(wptr);
	if (lReturn < 0)
	{
		MemFree(this->devMode);
		this->devMode = 0;
//		this->hdcPrinter = CreateDCW(L"WINSPOOL", ((DEVMODEW*)this->devMode)->dmDeviceName, 0, (DEVMODEW*)this->devMode);
	}
}

Media::Printer::Printer(Text::CStringNN printerName)
{
	this->devMode = 0;
	this->hPrinter = 0;
	this->printerName = Text::String::New(printerName);
	UnsafeArray<const WChar> wptr = Text::StrToWCharNew(printerName.v);
	if (OpenPrinterW((LPWSTR)wptr.Ptr(), &hPrinter, 0) == 0)
	{
		Text::StrDelNew(wptr);
		return;
	}

	Int32 lReturn = DocumentPropertiesW(0, (HANDLE)this->hPrinter, (LPWSTR)wptr.Ptr(), (DEVMODEW*)this->devMode, 0, 0);
	UOSInt size = (UOSInt)lReturn;
	this->devMode = MemAlloc(UInt8, size);
	lReturn = DocumentPropertiesW(0, (HANDLE)this->hPrinter, (LPWSTR)wptr.Ptr(), (DEVMODEW*)this->devMode, 0, DM_OUT_BUFFER);
	Text::StrDelNew(wptr);
	if (lReturn < 0)
	{
		MemFree(this->devMode);
		this->devMode = 0;
//		this->hdcPrinter = CreateDCW(L"WINSPOOL", ((DEVMODEW*)this->devMode)->dmDeviceName, 0, (DEVMODEW*)this->devMode);
	}
}

Media::Printer::~Printer()
{
	this->printerName->Release();
	if (hPrinter)
	{
		ClosePrinter((HANDLE)hPrinter);
		this->hPrinter = 0;
	}
	if (this->devMode)
	{
		MemFree(this->devMode);
	}
}

Bool Media::Printer::IsError()
{
	if (this->devMode == 0)
		return true;
	return false;
}

Bool Media::Printer::ShowPrintSettings(Optional<ControlHandle> hWnd)
{
	if (this->devMode == 0)
		return false;
	Int32 lReturn = DocumentPropertiesW((HWND)hWnd.OrNull(), (HANDLE)this->hPrinter, ((DEVMODEW*)this->devMode)->dmDeviceName, (DEVMODEW*)this->devMode, (DEVMODEW*)this->devMode, DM_IN_PROMPT | DM_IN_BUFFER);
	return IDOK == lReturn;
}

Optional<Media::PrintDocument> Media::Printer::StartPrint(NN<Media::PrintHandler> hdlr, NN<Media::DrawEngine> eng)
{
	Media::GDPrintDocument *doc;
	if (this->devMode == 0)
		return 0;
	NEW_CLASS(doc, Media::GDPrintDocument(this->printerName, this->devMode, NN<Media::GDIEngine>::ConvertFrom(eng), hdlr));
	if (doc->IsError())
	{
		DEL_CLASS(doc);
		return 0;
	}
	doc->Start();
	return doc;
}

void Media::Printer::EndPrint(NN<Media::PrintDocument> doc)
{
	doc.Delete();
}
