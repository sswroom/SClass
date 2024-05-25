#ifndef _SM_PARSER_FILEPARSER_IPACPARSER
#define _SM_PARSER_FILEPARSER_IPACPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class IPACParser : public IO::FileParser
		{
		public:
			IPACParser();
			virtual ~IPACParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual Optional<IO::ParsedObject> ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr);
		};
	}
}
#endif
