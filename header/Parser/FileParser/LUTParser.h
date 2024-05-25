#ifndef _SM_PARSER_FILEPARSER_LUTPARSER
#define _SM_PARSER_FILEPARSER_LUTPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class LUTParser : public IO::FileParser
		{
		public:
			LUTParser();
			virtual ~LUTParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual Optional<IO::ParsedObject> ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr);
		};
	}
}
#endif
