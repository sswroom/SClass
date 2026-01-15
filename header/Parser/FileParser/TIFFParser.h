#ifndef _SM_PARSER_FILEPARSER_TIFFPARSER
#define _SM_PARSER_FILEPARSER_TIFFPARSER
#include "IO/FileParser.h"
#include "Media/EXIFData.h"

namespace Parser
{
	namespace FileParser
	{
		class TIFFParser : public IO::FileParser
		{
		private:
			Optional<Parser::ParserList> parsers;
		public:
			TIFFParser();
			virtual ~TIFFParser();

			virtual Int32 GetName();
			virtual void SetParserList(Optional<Parser::ParserList> parsers);
			virtual void PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual Optional<IO::ParsedObject> ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr);

		private:
			static UInt32 GetUInt(NN<Media::EXIFData> exif, UInt32 id);
			static UInt32 GetUInt0(NN<Media::EXIFData> exif, UInt32 id);
			static UInt32 GetUIntSum(NN<Media::EXIFData> exif, UInt32 id, OptOut<UIntOS> nChannels);
		};
	}
}
#endif
