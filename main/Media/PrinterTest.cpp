#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "Media/DrawEngineFactory.h"
#include "Media/Printer.h"

#define PRINTERNAME CSTR("PDF")

class PrintObj : public Media::PrintHandler
{
public:
	PrintObj()
	{
	}

	virtual ~PrintObj()
	{
	}

	virtual Bool BeginPrint(NN<Media::PrintDocument> doc)
	{
		doc->SetDocName(CSTR("PrinterTest"));
		return true;
	}

	virtual Bool PrintPage(NN<Media::DrawImage> printPage)
	{
		NN<Media::DrawFont> font = printPage->NewFontPt(CSTR("Arial"), 12, Media::DrawEngine::DFS_NORMAL, 0);
		NN<Media::DrawBrush> brush = printPage->NewBrushARGB(0xff000000);
		printPage->DrawString(Math::Coord2DDbl(100, 100), CSTR("Testing"), font, brush);
		printPage->DelBrush(brush);
		printPage->DelFont(font);
		return false;
	}
	virtual Bool EndPrint(NN<Media::PrintDocument> doc)
	{
		return true;
	}
};

Int32 MyMain(NN<Core::IProgControl> progCtrl)
{
	IO::ConsoleWriter *console;
	UOSInt i;
	UOSInt j;
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	NN<Media::DrawEngine> eng;
	Media::Printer *printer;
	NN<Media::PrintDocument> doc;
	NN<PrintObj> prtobj;

	NEW_CLASS(console, IO::ConsoleWriter());
	console->WriteLine(CSTR("Found Printers:"));
	i = 0;
	j = Media::Printer::GetPrinterCount();
	while (i < j)
	{
		if (Media::Printer::GetPrinterName(sbuff, i).SetTo(sptr))
		{
			console->WriteLine(CSTRP(sbuff, sptr));
		}
		i++;
	}
	eng = Media::DrawEngineFactory::CreateDrawEngine();
	NEW_CLASS(printer, Media::Printer(PRINTERNAME));
	printer->ShowPrintSettings(0);
	NEW_CLASSNN(prtobj, PrintObj());
	if (printer->StartPrint(prtobj, eng).SetTo(doc))
	{
		doc->WaitForEnd();
		printer->EndPrint(doc);
	}
	prtobj.Delete();
	DEL_CLASS(printer);
	eng.Delete();

	DEL_CLASS(console);
	return 0;
}
