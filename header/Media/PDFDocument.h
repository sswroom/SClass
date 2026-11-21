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
		Bool encryptionRequired;
		UIntOS encryptionKeyLen;
		UInt8 encryptionKey[16];

		static void SkipPDFWhitespace(UnsafeArray<const UTF8Char> str, UIntOS length, UIntOS &pos);
		static Bool ReadPDFNumberToken(UnsafeArray<const UTF8Char> str, UIntOS length, UIntOS &pos, UIntOS &start, UIntOS &end);
		static Bool IsPDFDecodeEndpoint(UnsafeArray<const UTF8Char> str, UIntOS start, UIntOS end, UInt8 endpoint);
		static Bool IsReverseDecodeArray(Text::CStringNN value);
		Optional<IO::ParsedObject> SetPObjName(Optional<IO::ParsedObject> pobj, UInt32 objId, Text::CStringNN ext);
	public:
		PDFDocument(NN<Text::String> sourceName, Text::CStringNN version);
		virtual ~PDFDocument();

		virtual IO::ParserType GetParserType() const;
		virtual UIntOS GetCount() const;
		virtual NN<PDFObject> GetItemNoCheck(UIntOS index) const;
		virtual Optional<PDFObject> GetItem(UIntOS index) const;
		
		NN<PDFObject> AddObject(UInt32 id);
		Optional<PDFObject> GetObjectById(UInt32 id) const;
		void SetEncryptionRequired();
		Bool SetEncryptionKey(UnsafeArray<const UInt8> key, UIntOS keyLen);
		Bool DecryptStreamData(UInt32 objId, UnsafeArray<UInt8> data, UIntOS dataSize) const;
		Optional<Media::ImageList> CreateImage(UInt32 id, NN<Parser::ParserList> parsers);
	};
}
#endif
