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
		NotNullPtr<Text::String> printerName;
		
	public:
		static UOSInt GetPrinterCount();
		static UTF8Char *GetPrinterName(UTF8Char *sbuff, UOSInt index);
		static Printer *SelectPrinter(void *hWnd);

	private:
		Printer(const WChar *printerName, UInt8 *devMode, UOSInt devModeSize);
	public:
		Printer(NotNullPtr<Text::String> printerName);
		Printer(Text::CString printerName);
		~Printer();
		
		Bool IsError();
		Bool ShowPrintSettings(void *hWnd);
		Optional<IPrintDocument> StartPrint(IPrintHandler *hdlr, NotNullPtr<Media::DrawEngine> eng);
		void EndPrint(NotNullPtr<IPrintDocument> doc);
	};
};
#endif
