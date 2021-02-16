#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Core/DefaultDrawEngine.h"
#include "IO/ConsoleWriter.h"
#include "Media/Printer.h"

#define PRINTERNAME (const UTF8Char*)"PDFCreator"

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
		return true;
	}

	virtual Bool PrintPage(Media::DrawImage *printPage)
	{
		Media::DrawFont *font = printPage->NewFont((const UTF8Char*)"Arial", 12, Media::DrawEngine::DFS_NORMAL);
		Media::DrawBrush *brush = printPage->NewBrushARGB(0xff000000);
		printPage->DrawString(100, 100, (const UTF8Char*)"Testing", font, brush);
		printPage->DelBrush(brush);
		printPage->DelFont(font);
		return false;
	}
	virtual Bool EndPrint(Media::IPrintDocument *doc)
	{
		return true;
	}
};

Int32 MyMain(Core::IProgControl *progCtrl)
{
	IO::ConsoleWriter *console;
	OSInt i;
	OSInt j;
	UTF8Char sbuff[256];
	Media::DrawEngine *eng;
	Media::Printer *printer;
	Media::IPrintDocument *doc;
	PrintObj *prtobj;

	NEW_CLASS(console, IO::ConsoleWriter());
	console->WriteLine((const UTF8Char*)"Found Printers:");
	i = 0;
	j = Media::Printer::GetPrinterCount();
	while (i < j)
	{
		Media::Printer::GetPrinterName(sbuff, i);
		console->WriteLine(sbuff);
		i++;
	}
	eng = Core::DefaultDrawEngine::CreateDrawEngine();
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
	DEL_CLASS(eng);

	DEL_CLASS(console);
	return 0;
}
