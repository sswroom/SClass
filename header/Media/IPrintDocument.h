#ifndef _SM_MEDIA_IPRINTDOCUMENT
#define _SM_MEDIA_IPRINTDOCUMENT
#include "Media/DrawEngine.h"

namespace Media
{
	class IPrintDocument
	{
	public:
		enum class PageOrientation
		{
			Landscape,
			Portrait
		};
	public:
		virtual ~IPrintDocument() {};

		virtual void SetDocName(Text::String *docName) = 0;
		virtual void SetDocName(Text::CString docName) = 0;
		virtual void SetNextPagePaperSizeMM(Double width, Double height) = 0;
		virtual void SetNextPageOrientation(PageOrientation po) = 0;
		virtual void WaitForEnd() = 0;
	};

	class IPrintHandler
	{
	public:
		virtual Bool BeginPrint(IPrintDocument *doc) = 0;
		virtual Bool PrintPage(Media::DrawImage *printPage) = 0; //return has more pages 
		virtual Bool EndPrint(IPrintDocument *doc) = 0;
	};

};
#endif
