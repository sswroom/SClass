#ifndef _SM_MEDIA_PDFDOCUMENT
#define _SM_MEDIA_PDFDOCUMENT
#include "Data/FastMap.h"
#include "IO/ParsedObject.h"
#include "Media/PDFObject.h"

namespace Media
{
	class PDFDocument : public IO::ParsedObject
	{
	private:
		Data::UInt64FastMap<PDFObject*> objMap;
		Text::String *version;
	public:
		PDFDocument(Text::String *sourceName, Text::CString version);
		virtual ~PDFDocument();

		virtual IO::ParserType GetParserType() const;

		PDFObject *AddObject(UInt32 id);
	};
}
#endif
