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
			Parser::ParserList *parsers;
		public:
			TIFFParser();
			virtual ~TIFFParser();

			virtual Int32 GetName();
			virtual void SetParserList(Parser::ParserList *parsers);
			virtual void PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFileHdr(NN<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr);

		private:
			static UInt32 GetUInt(NN<Media::EXIFData> exif, UInt32 id);
			static UInt32 GetUInt0(NN<Media::EXIFData> exif, UInt32 id);
			static UInt32 GetUIntSum(NN<Media::EXIFData> exif, UInt32 id, OptOut<UOSInt> nChannels);
		};
	}
}
#endif
