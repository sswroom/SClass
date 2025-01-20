#ifndef _SM_MEDIA_PRINTDOCUMENT
#define _SM_MEDIA_PRINTDOCUMENT
#include "Media/DrawEngine.h"

namespace Media
{
	class PrintDocument
	{
	public:
		enum class PageOrientation
		{
			Landscape,
			Portrait
		};
	public:
		virtual ~PrintDocument() {};

		virtual void SetDocName(Optional<Text::String> docName) = 0;
		virtual void SetDocName(Text::CString docName) = 0;
		virtual void SetNextPagePaperSizeMM(Double width, Double height) = 0;
		virtual void SetNextPageOrientation(PageOrientation po) = 0;
		virtual void WaitForEnd() = 0;
	};

	class PrintHandler
	{
	public:
		virtual Bool BeginPrint(NN<PrintDocument> doc) = 0;
		virtual Bool PrintPage(NN<Media::DrawImage> printPage) = 0; //return has more pages 
		virtual Bool EndPrint(NN<PrintDocument> doc) = 0;
	};
}
#endif
