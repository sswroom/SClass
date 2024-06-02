#ifndef _SM_MEDIA_PRINTER
#define _SM_MEDIA_PRINTER
#include "Media/DrawEngine.h"
#include "Media/IPrintDocument.h"

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
		Printer(Text::CString printerName);
		~Printer();
		
		Bool IsError();
		Bool ShowPrintSettings(void *hWnd);
		Optional<IPrintDocument> StartPrint(NN<IPrintHandler> hdlr, NN<Media::DrawEngine> eng);
		void EndPrint(NN<IPrintDocument> doc);
	};
}
#endif
