#ifndef _SM_PARSER_FILEPARSER_AOSPARSER
#define _SM_PARSER_FILEPARSER_AOSPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class AOSParser : public IO::FileParser
		{
		public:
			AOSParser();
			virtual ~AOSParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual Optional<IO::ParsedObject> ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr);
		};
	}
}
#endif
