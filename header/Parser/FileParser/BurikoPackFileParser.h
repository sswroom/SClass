#ifndef _SM_PARSER_FILEPARSER_BURIKOPACKFILEPARSER
#define _SM_PARSER_FILEPARSER_BURIKOPACKFILEPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class BurikoPackFileParser : public IO::FileParser
		{
		public:
			BurikoPackFileParser();
			virtual ~BurikoPackFileParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual Optional<IO::ParsedObject> ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr);
		};
	}
}
#endif
