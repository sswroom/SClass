#ifndef _SM_MEDIA_PDFIMAGE
#define _SM_MEDIA_PDFIMAGE
#include "Media/ImageList.h"
#include "Parser/ParserList.h"

namespace Media
{
	class PDFDocument;
	class PDFImage
	{
	private:
		static UInt8 IndexedHexValue(UTF8Char c);
	public:
		static Optional<Media::ImageList> Parse(NN<PDFDocument> pdf, UInt32 id, NN<Parser::ParserList> parsers);
	};
}
#endif
