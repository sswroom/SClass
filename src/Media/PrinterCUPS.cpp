#include "Stdafx.h"
#include "AnyType.h"
#include "MyMemory.h"
#include "IO/Path.h"
#include "Math/Math_C.h"
#include "Math/Unit/Distance.h"
#include "Media/GTKDrawEngine.h"
#include "Media/PaperSize.h"
#include "Media/Printer.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include <cairo/cairo.h>
#include <cairo/cairo-ps.h>
#include <cups/cups.h>
#include <stdio.h>
//#include <wchar.h>

// https://stackoverflow.com/questions/14518004/linux-cups-printing-example-tutorial
// https://refspecs.linuxfoundation.org/LSB_4.0.0/LSB-Printing/LSB-Printing/libcupsman.html
#define VERBOSE

namespace Media
{
	class CUPSPrintDocument : public Media::PrintDocument
	{
	private:
		NN<PrintHandler> hdlr;
		NN<Media::GTKDrawEngine> eng;
		Optional<Text::String> docName;
		Bool started;
		Bool running;
		PageOrientation po;
		Math::Size2DDbl paperSizeMM;
		NN<Text::String> printerName;

		static UInt32 __stdcall PrintThread(AnyType userObj);
	public:
		CUPSPrintDocument(NN<Text::String> printerName, NN<Media::GTKDrawEngine> eng, NN<PrintHandler> hdlr);
		virtual ~CUPSPrintDocument();

		Bool IsError();

		virtual void SetDocName(Optional<Text::String> docName);
		virtual void SetDocName(Text::CString docName);
		virtual void SetNextPagePaperSizeMM(Double width, Double height);
		virtual void SetNextPageOrientation(PageOrientation po);
		void Start();
		virtual void WaitForEnd();
	};
}

UInt32 __stdcall Media::CUPSPrintDocument::PrintThread(AnyType userObj)
{
	NN<Media::CUPSPrintDocument> me = userObj.GetNN<Media::CUPSPrintDocument>();
	NN<Media::DrawImage> img;
	Bool hasMorePage;

	UTF8Char fileName[512];
	UnsafeArray<UTF8Char> sptr;
	UTF8Char *sptr2;
	Int64 t;
	UOSInt i;
	Double paperWidth;
	Double paperHeight;
	Data::DateTime dt;
	NN<Text::String> s;
	dt.SetCurrTimeUTC();
	t = dt.ToTicks();
	sptr = IO::Path::GetProcessFileName(fileName).Or(fileName);
	i = Text::StrLastIndexOfCharC(fileName, (UOSInt)(sptr - fileName), IO::Path::PATH_SEPERATOR);
	sptr = &fileName[i + 1];
	Text::StrConcatC(Text::StrInt64(Text::StrConcatC(sptr, UTF8STRC("CUPS_")), t), UTF8STRC(".tmp"));

	if (me->po == PageOrientation::Landscape)
	{
		paperWidth = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_MILLIMETER, Math::Unit::Distance::DU_INCH, me->paperSizeMM.y);
		paperHeight = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_MILLIMETER, Math::Unit::Distance::DU_INCH, me->paperSizeMM.x);
	}
	else
	{
		paperWidth = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_MILLIMETER, Math::Unit::Distance::DU_INCH, me->paperSizeMM.x);
		paperHeight = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_MILLIMETER, Math::Unit::Distance::DU_INCH, me->paperSizeMM.y);
	}

	cairo_surface_t *surface = cairo_ps_surface_create((const Char*)fileName, Double2Int32(paperWidth * 72.0), Double2Int32(paperHeight * 72.0));
	cairo_t *cr = cairo_create(surface);

	i = 1;
	while (true)
	{
		if (me->po == PageOrientation::Landscape)
		{
			paperWidth = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_MILLIMETER, Math::Unit::Distance::DU_INCH, me->paperSizeMM.y);
			paperHeight = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_MILLIMETER, Math::Unit::Distance::DU_INCH, me->paperSizeMM.x);
		}
		else
		{
			paperWidth = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_MILLIMETER, Math::Unit::Distance::DU_INCH, me->paperSizeMM.x);
			paperHeight = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_MILLIMETER, Math::Unit::Distance::DU_INCH, me->paperSizeMM.y);
		}
		cairo_ps_surface_set_size(surface, Double2Int32(paperWidth * 72.0), Double2Int32(paperHeight * 72.0));

//		wprintf(L"Printing page size: %lf, %lf\r\n", paperWidth, paperHeight);
		img = me->eng->CreateImageScn(cr, Math::Coord2D<OSInt>(0, 0), Math::Coord2D<OSInt>(Double2OSInt(paperWidth * 72.0), Double2OSInt(paperHeight * 72.0)), nullptr);
		img->SetHDPI(72.0);
		img->SetVDPI(72.0);
		hasMorePage = me->hdlr->PrintPage(img);
		me->eng->DeleteImage(img);

		cairo_show_page(cr);
		if (!hasMorePage)
			break;
		i++;
	}
	cairo_destroy(cr);
	cairo_surface_flush(surface);
	cairo_surface_destroy(surface);

	Text::StringBuilderUTF8 sb1;
	Text::StringBuilderUTF8 sb2;
	sb1.Append(me->printerName);
	if (me->docName.SetTo(s))
	{
		sb2.Append(s);
	}
	else
	{
		sb2.AppendC(UTF8STRC("Untitled"));
	}
	sptr2 = fileName;
	cupsPrintFiles((Char*)sb1.ToPtr(), 1, (const Char**)&sptr2, (Char*)sb2.ToPtr(), 0, 0);

	IO::Path::DeleteFile(fileName);
	me->hdlr->EndPrint(me);

	me->running = false;
	return 0;
}

Media::CUPSPrintDocument::CUPSPrintDocument(NN<Text::String> printerName, NN<Media::GTKDrawEngine> eng, NN<PrintHandler> hdlr)
{
	this->eng = eng;
	this->hdlr = hdlr;
	this->docName = nullptr;
	this->started = false;
	this->running = false;
	this->printerName = printerName;
	this->po = PageOrientation::Portrait;
	this->paperSizeMM = Media::PaperSize::PaperTypeGetSizeMM(Media::PaperSize::PT_A4);
}

Media::CUPSPrintDocument::~CUPSPrintDocument()
{
	WaitForEnd();
	OPTSTR_DEL(this->docName);
}

Bool Media::CUPSPrintDocument::IsError()
{
	return false;
}

void Media::CUPSPrintDocument::SetDocName(Optional<Text::String> docName)
{
	OPTSTR_DEL(this->docName);
	this->docName = Text::String::CopyOrNull(docName);
}

void Media::CUPSPrintDocument::SetDocName(Text::CString docName)
{
	OPTSTR_DEL(this->docName);
	this->docName = Text::String::NewOrNull(docName);
}

void Media::CUPSPrintDocument::SetNextPagePaperSizeMM(Double width, Double height)
{
	this->paperSizeMM = Math::Size2DDbl(width, height);
}

void Media::CUPSPrintDocument::SetNextPageOrientation(PageOrientation po)
{
	this->po = po;
}

void Media::CUPSPrintDocument::Start()
{
	if (this->started)
		return;

	if (this->hdlr->BeginPrint(*this))
	{
		
		this->started = true;
		this->running = true;
		Sync::ThreadUtil::Create(PrintThread, this);
	}
}

void Media::CUPSPrintDocument::WaitForEnd()
{
	while (this->running)
	{
		Sync::SimpleThread::Sleep(10);
	}
}

UOSInt Media::Printer::GetPrinterCount()
{
	cups_dest_t *dests;
	int cnt = cupsGetDests(&dests);
	cupsFreeDests(cnt, dests);
	return (UOSInt)cnt;
}

UnsafeArrayOpt<UTF8Char> Media::Printer::GetPrinterName(UnsafeArray<UTF8Char> sbuff, UOSInt index)
{
	if (index < 0)
		return nullptr;
	UnsafeArrayOpt<UTF8Char> ret = nullptr;
	cups_dest_t *dests;
	int cnt = cupsGetDests(&dests);
	if (index < (UOSInt)cnt)
	{
		ret = Text::StrConcat(sbuff, (const UTF8Char*)dests[index].name);
	}
	cupsFreeDests(cnt, dests);
	return ret;
}

Media::Printer *Media::Printer::SelectPrinter(Optional<ControlHandle> hWnd)
{
	return 0;
}

Media::Printer::Printer(NN<Text::String> printerName)
{
	this->printerName = printerName->Clone();
}

Media::Printer::Printer(Text::CStringNN printerName)
{
	this->printerName = Text::String::New(printerName);
}

Media::Printer::~Printer()
{
	this->printerName->Release();
}

Bool Media::Printer::IsError()
{
	return this->printerName->leng == 0;
}

Bool Media::Printer::ShowPrintSettings(Optional<ControlHandle> hWnd)
{
	return false;
}

Optional<Media::PrintDocument> Media::Printer::StartPrint(NN<PrintHandler> hdlr, NN<Media::DrawEngine> eng)
{
	Media::CUPSPrintDocument *doc;
	NEW_CLASS(doc, Media::CUPSPrintDocument(this->printerName, NN<Media::GTKDrawEngine>::ConvertFrom(eng), hdlr));
	if (doc->IsError())
	{
		DEL_CLASS(doc);
		return nullptr;
	}
	doc->Start();
	return doc;
}

void Media::Printer::EndPrint(NN<PrintDocument> doc)
{
	doc.Delete();
}

