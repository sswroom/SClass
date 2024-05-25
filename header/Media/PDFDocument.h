#ifndef _SM_MEDIA_PDFDOCUMENT
#define _SM_MEDIA_PDFDOCUMENT
#include "Data/FastMapNN.h"
#include "IO/ParsedObject.h"
#include "Media/ImageList.h"
#include "Media/PDFObject.h"
#include "Parser/ParserList.h"

namespace Media
{
	class PDFDocument : public IO::ParsedObject, public Data::ReadingListNN<PDFObject>
	{
	private:
		Data::UInt32FastMapNN<PDFObject> objMap;
		NN<Text::String> version;

		Optional<IO::ParsedObject> SetPObjName(Optional<IO::ParsedObject> pobj, UInt32 objId, Text::CString ext);
	public:
		PDFDocument(NN<Text::String> sourceName, Text::CString version);
		virtual ~PDFDocument();

		virtual IO::ParserType GetParserType() const;
		virtual UOSInt GetCount() const;
		virtual NN<PDFObject> GetItemNoCheck(UOSInt index) const;
		virtual Optional<PDFObject> GetItem(UOSInt index) const;
		
		NN<PDFObject> AddObject(UInt32 id);
		Optional<Media::ImageList> CreateImage(UInt32 id, NN<Parser::ParserList> parsers);
	};
}
#endif
