#ifndef _SM_MEDIA_PDFDOCUMENT
#define _SM_MEDIA_PDFDOCUMENT
#include "Data/FastMap.h"
#include "IO/ParsedObject.h"
#include "Media/ImageList.h"
#include "Media/PDFObject.h"
#include "Parser/ParserList.h"

namespace Media
{
	class PDFDocument : public IO::ParsedObject, public Data::ReadingList<PDFObject*>
	{
	private:
		Data::UInt32FastMap<PDFObject*> objMap;
		Text::String *version;

		IO::ParsedObject *SetPObjName(IO::ParsedObject *pobj, UInt32 objId, Text::CString ext);
	public:
		PDFDocument(Text::String *sourceName, Text::CString version);
		virtual ~PDFDocument();

		virtual IO::ParserType GetParserType() const;
		virtual UOSInt GetCount() const;
		virtual PDFObject *GetItem(UOSInt index) const;
		
		PDFObject *AddObject(UInt32 id);
		Media::ImageList *CreateImage(UInt32 id, Parser::ParserList *parsers);
	};
}
#endif
