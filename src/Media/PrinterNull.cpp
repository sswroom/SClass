#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/Printer.h"

UIntOS Media::Printer::GetPrinterCount()
{
	return 0;
}

UTF8Char *Media::Printer::GetPrinterName(UTF8Char *sbuff, UIntOS index)
{
	return 0;
}

Media::Printer *Media::Printer::SelectPrinter(void *hWnd)
{
	return 0;
}

Media::Printer::Printer(NN<Text::String> printerName)
{
}

Media::Printer::Printer(Text::CString printerName)
{
}

Media::Printer::~Printer()
{
}

Bool Media::Printer::IsError()
{
	return true;
}

Bool Media::Printer::ShowPrintSettings(void *hWnd)
{
	return false;
}

Media::PrintDocument *Media::Printer::StartPrint(PrintHandler *hdlr, NN<Media::DrawEngine> eng)
{
	return 0;
}

void Media::Printer::EndPrint(PrintDocument *doc)
{
}

