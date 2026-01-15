#ifndef _SM_MEDIA_PDFDOCUMENT
#define _SM_MEDIA_PDFDOCUMENT
#include "Data/FastMapNN.hpp"
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

		Optional<IO::ParsedObject> SetPObjName(Optional<IO::ParsedObject> pobj, UInt32 objId, Text::CStringNN ext);
	public:
		PDFDocument(NN<Text::String> sourceName, Text::CStringNN version);
		virtual ~PDFDocument();

		virtual IO::ParserType GetParserType() const;
		virtual UIntOS GetCount() const;
		virtual NN<PDFObject> GetItemNoCheck(UIntOS index) const;
		virtual Optional<PDFObject> GetItem(UIntOS index) const;
		
		NN<PDFObject> AddObject(UInt32 id);
		Optional<Media::ImageList> CreateImage(UInt32 id, NN<Parser::ParserList> parsers);
	};
}
#endif
