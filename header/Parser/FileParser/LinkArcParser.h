#ifndef _SM_PARSER_FILEPARSER_LINKARCPARSER
#define _SM_PARSER_FILEPARSER_LINKARCPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class LinkArcParser : public IO::FileParser
		{
		public:
			LinkArcParser();
			virtual ~LinkArcParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual Optional<IO::ParsedObject> ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr);
		};
	}
}
#endif
