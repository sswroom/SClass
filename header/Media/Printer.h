#ifndef _SM_MEDIA_PRINTER
#define _SM_MEDIA_PRINTER
#include "Media/DrawEngine.h"
#include "Media/PrintDocument.h"

namespace Media
{
	class Printer
	{
	private:
		void *hPrinter;
		UInt8 *devMode;
		NN<Text::String> printerName;
		
	public:
		static UOSInt GetPrinterCount();
		static UnsafeArrayOpt<UTF8Char> GetPrinterName(UnsafeArray<UTF8Char> sbuff, UOSInt index);
		static Printer *SelectPrinter(void *hWnd);

	private:
		Printer(const WChar *printerName, UInt8 *devMode, UOSInt devModeSize);
	public:
		Printer(NN<Text::String> printerName);
		Printer(Text::CStringNN printerName);
		~Printer();
		
		Bool IsError();
		Bool ShowPrintSettings(void *hWnd);
		Optional<PrintDocument> StartPrint(NN<PrintHandler> hdlr, NN<Media::DrawEngine> eng);
		void EndPrint(NN<PrintDocument> doc);
	};
}
#endif
