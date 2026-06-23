#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/Printer.h"

UIntOS Media::Printer::GetPrinterCount()
{
	return 0;
}

UnsafeArrayOpt<UTF8Char> Media::Printer::GetPrinterName(UnsafeArray<UTF8Char> sbuff, UIntOS index)
{
	return nullptr;
}

Optional<Media::Printer> Media::Printer::SelectPrinter(Optional<ControlHandle> hWnd)
{
	return nullptr;
}

Media::Printer::Printer(NN<Text::String> printerName)
{
}

Media::Printer::Printer(Text::CStringNN printerName)
{
}

Media::Printer::~Printer()
{
}

Bool Media::Printer::IsError()
{
	return true;
}

Bool Media::Printer::ShowPrintSettings(Optional<ControlHandle> hWnd)
{
	return false;
}

Optional<Media::PrintDocument> Media::Printer::StartPrint(NN<PrintHandler> hdlr, NN<Media::DrawEngine> eng)
{
	return nullptr;
}

void Media::Printer::EndPrint(NN<Media::PrintDocument> doc)
{
}

