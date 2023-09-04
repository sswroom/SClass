#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "Media/DrawEngineFactory.h"
#include "Media/Printer.h"

#define PRINTERNAME CSTR("PDF")

class PrintObj : public Media::IPrintHandler
{
public:
	PrintObj()
	{
	}

	virtual ~PrintObj()
	{
	}

	virtual Bool BeginPrint(Media::IPrintDocument *doc)
	{
		doc->SetDocName(CSTR("PrinterTest"));
		return true;
	}

	virtual Bool PrintPage(NotNullPtr<Media::DrawImage> printPage)
	{
		Media::DrawFont *font = printPage->NewFontPt(CSTR("Arial"), 12, Media::DrawEngine::DFS_NORMAL, 0);
		Media::DrawBrush *brush = printPage->NewBrushARGB(0xff000000);
		printPage->DrawString(Math::Coord2DDbl(100, 100), CSTR("Testing"), font, brush);
		printPage->DelBrush(brush);
		printPage->DelFont(font);
		return false;
	}
	virtual Bool EndPrint(Media::IPrintDocument *doc)
	{
		return true;
	}
};

Int32 MyMain(NotNullPtr<Core::IProgControl> progCtrl)
{
	IO::ConsoleWriter *console;
	UOSInt i;
	UOSInt j;
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	NotNullPtr<Media::DrawEngine> eng;
	Media::Printer *printer;
	Media::IPrintDocument *doc;
	PrintObj *prtobj;

	NEW_CLASS(console, IO::ConsoleWriter());
	console->WriteLineC(UTF8STRC("Found Printers:"));
	i = 0;
	j = Media::Printer::GetPrinterCount();
	while (i < j)
	{
		sptr = Media::Printer::GetPrinterName(sbuff, i);
		console->WriteLineC(sbuff, (UOSInt)(sptr - sbuff));
		i++;
	}
	eng = Media::DrawEngineFactory::CreateDrawEngine();
	NEW_CLASS(printer, Media::Printer(PRINTERNAME));
	printer->ShowPrintSettings(0);
	NEW_CLASS(prtobj, PrintObj());
	doc = printer->StartPrint(prtobj, eng);
	if (doc)
	{
		doc->WaitForEnd();
		printer->EndPrint(doc);
	}
	DEL_CLASS(prtobj);
	DEL_CLASS(printer);
	eng.Delete();

	DEL_CLASS(console);
	return 0;
}
