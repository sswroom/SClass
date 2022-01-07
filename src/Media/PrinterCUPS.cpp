#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/Path.h"
#include "Math/Math.h"
#include "Math/Unit/Distance.h"
#include "Media/GTKDrawEngine.h"
#include "Media/PaperSize.h"
#include "Media/Printer.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include <cairo/cairo.h>
#include <cairo/cairo-ps.h>
#include <cups/cups.h>
#include <stdio.h>
//#include <wchar.h>

// https://stackoverflow.com/questions/14518004/linux-cups-printing-example-tutorial
// https://refspecs.linuxfoundation.org/LSB_4.0.0/LSB-Printing/LSB-Printing/libcupsman.html

namespace Media
{
	class CUPSPrintDocument : public Media::IPrintDocument
	{
	private:
		IPrintHandler *hdlr;
		Media::GTKDrawEngine *eng;
		Text::String *docName;
		Bool started;
		Bool running;
		PageOrientation po;
		Double paperWidth;
		Double paperHeight;
		Text::String *printerName;

		static UInt32 __stdcall PrintThread(void *userObj);
	public:
		CUPSPrintDocument(Text::String *printerName, Media::GTKDrawEngine *eng, IPrintHandler *hdlr);
		virtual ~CUPSPrintDocument();

		Bool IsError();

		virtual void SetDocName(Text::String *docName);
		virtual void SetDocName(const UTF8Char *docName);
		virtual void SetNextPagePaperSizeMM(Double width, Double height);
		virtual void SetNextPageOrientation(PageOrientation po);
		void Start();
		virtual void WaitForEnd();
	};
}

UInt32 __stdcall Media::CUPSPrintDocument::PrintThread(void *userObj)
{
	Media::CUPSPrintDocument *me = (Media::CUPSPrintDocument*)userObj;
	Media::DrawImage *img;
	Bool hasMorePage;

	UTF8Char fileName[512];
	UTF8Char *sptr;
	Int64 t;
	UOSInt i;
	Double paperWidth;
	Double paperHeight;
	Data::DateTime dt;
	dt.SetCurrTimeUTC();
	t = dt.ToTicks();
	IO::Path::GetProcessFileName(fileName);
	i = Text::StrLastIndexOf(fileName, IO::Path::PATH_SEPERATOR);
	sptr = &fileName[i + 1];
	Text::StrConcatC(Text::StrInt64(Text::StrConcatC(sptr, UTF8STRC("CUPS_")), t), UTF8STRC(".tmp"));

	if (me->po == PageOrientation::Landscape)
	{
		paperWidth = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_MILLIMETER, Math::Unit::Distance::DU_INCH, me->paperHeight);
		paperHeight = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_MILLIMETER, Math::Unit::Distance::DU_INCH, me->paperWidth);
	}
	else
	{
		paperWidth = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_MILLIMETER, Math::Unit::Distance::DU_INCH, me->paperWidth);
		paperHeight = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_MILLIMETER, Math::Unit::Distance::DU_INCH, me->paperHeight);
	}

	cairo_surface_t *surface = cairo_ps_surface_create((const Char*)fileName, Math::Double2Int32(paperWidth * 72.0), Math::Double2Int32(paperHeight * 72.0));
	cairo_t *cr = cairo_create(surface);

	i = 1;
	while (true)
	{
		if (me->po == PageOrientation::Landscape)
		{
			paperWidth = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_MILLIMETER, Math::Unit::Distance::DU_INCH, me->paperHeight);
			paperHeight = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_MILLIMETER, Math::Unit::Distance::DU_INCH, me->paperWidth);
		}
		else
		{
			paperWidth = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_MILLIMETER, Math::Unit::Distance::DU_INCH, me->paperWidth);
			paperHeight = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_MILLIMETER, Math::Unit::Distance::DU_INCH, me->paperHeight);
		}
		cairo_ps_surface_set_size(surface, Math::Double2Int32(paperWidth * 72.0), Math::Double2Int32(paperHeight * 72.0));

//		wprintf(L"Printing page size: %lf, %lf\r\n", paperWidth, paperHeight);
		img = me->eng->CreateImageScn(cr, 0, 0, Math::Double2Int32(paperWidth * 72.0), Math::Double2Int32(paperHeight * 72.0));
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
	if (me->docName)
	{
		sb2.Append(me->docName);
	}
	else
	{
		sb2.AppendC(UTF8STRC("Untitled"));
	}
	sptr = fileName;
	cupsPrintFiles((Char*)sb1.ToString(), 1, (const Char**)&sptr, (Char*)sb2.ToString(), 0, 0);

	IO::Path::DeleteFile(fileName);
	me->hdlr->EndPrint(me);

	me->running = false;
	return 0;
}

Media::CUPSPrintDocument::CUPSPrintDocument(Text::String *printerName, Media::GTKDrawEngine *eng, IPrintHandler *hdlr)
{
	this->eng = eng;
	this->hdlr = hdlr;
	this->docName = 0;
	this->started = false;
	this->running = false;
	this->printerName = printerName;
	this->po = PageOrientation::Portrait;
	Media::PaperSize psize(Media::PaperSize::PT_A4);
	this->paperWidth = psize.GetWidthMM();
	this->paperHeight = psize.GetHeightMM();
}

Media::CUPSPrintDocument::~CUPSPrintDocument()
{
	WaitForEnd();
	SDEL_STRING(this->docName);
}

Bool Media::CUPSPrintDocument::IsError()
{
	return false;
}

void Media::CUPSPrintDocument::SetDocName(Text::String *docName)
{
	SDEL_STRING(this->docName);
	this->docName = SCOPY_STRING(docName);
}

void Media::CUPSPrintDocument::SetDocName(const UTF8Char *docName)
{
	SDEL_STRING(this->docName);
	this->docName = Text::String::NewOrNull(docName);
}

void Media::CUPSPrintDocument::SetNextPagePaperSizeMM(Double width, Double height)
{
	this->paperWidth = width;
	this->paperHeight = height;
}

void Media::CUPSPrintDocument::SetNextPageOrientation(PageOrientation po)
{
	this->po = po;
}

void Media::CUPSPrintDocument::Start()
{
	if (this->started)
		return;

	if (this->hdlr->BeginPrint(this))
	{
		
		this->started = true;
		this->running = true;
		Sync::Thread::Create(PrintThread, this);
	}
}

void Media::CUPSPrintDocument::WaitForEnd()
{
	while (this->running)
	{
		Sync::Thread::Sleep(10);
	}
}

UOSInt Media::Printer::GetPrinterCount()
{
	cups_dest_t *dests;
	int cnt = cupsGetDests(&dests);
	cupsFreeDests(cnt, dests);
	return (UOSInt)cnt;
}

UTF8Char *Media::Printer::GetPrinterName(UTF8Char *sbuff, UOSInt index)
{
	if (index < 0)
		return 0;
	UTF8Char *ret = 0;
	cups_dest_t *dests;
	int cnt = cupsGetDests(&dests);
	if (index < (UOSInt)cnt)
	{
		ret = Text::StrConcat(sbuff, (const UTF8Char*)dests[index].name);
	}
	cupsFreeDests(cnt, dests);
	return ret;
}

Media::Printer *Media::Printer::SelectPrinter(void *hWnd)
{
	return 0;
}

Media::Printer::Printer(Text::String *printerName)
{
	this->printerName = printerName->Clone();
}

Media::Printer::Printer(const UTF8Char *printerName)
{
	this->printerName = Text::String::NewNotNull(printerName);
}

Media::Printer::~Printer()
{
	this->printerName->Release();
}

Bool Media::Printer::IsError()
{
	return this->printerName == 0;
}

Bool Media::Printer::ShowPrintSettings(void *hWnd)
{
	return false;
}

Media::IPrintDocument *Media::Printer::StartPrint(IPrintHandler *hdlr, Media::DrawEngine *eng)
{
	Media::CUPSPrintDocument *doc;
	if (eng == 0)
		return 0;
	NEW_CLASS(doc, Media::CUPSPrintDocument(this->printerName, (Media::GTKDrawEngine*)eng, hdlr));
	if (doc->IsError())
	{
		DEL_CLASS(doc);
		return 0;
	}
	doc->Start();
	return doc;
}

void Media::Printer::EndPrint(IPrintDocument *doc)
{
	DEL_CLASS(doc);
}

