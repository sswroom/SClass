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
		const UTF8Char *printerName;
		
	public:
		static OSInt GetPrinterCount();
		static UTF8Char *GetPrinterName(UTF8Char *sbuff, OSInt index);
		static Printer *SelectPrinter(void *hWnd);

	private:
		Printer(const WChar *printerName, UInt8 *devMode, OSInt devModeSize);
	public:
		Printer(const UTF8Char *printerName);
		~Printer();
		
		Bool IsError();
		Bool ShowPrintSettings(void *hWnd);
		IPrintDocument *StartPrint(IPrintHandler *hdlr, Media::DrawEngine *eng);
		void EndPrint(IPrintDocument *doc);
	};
};
#endif
