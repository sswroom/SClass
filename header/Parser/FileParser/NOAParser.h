#ifndef _SM_PARSER_FILEPARSER_NOAPARSER
#define _SM_PARSER_FILEPARSER_NOAPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class NOAParser : public IO::FileParser
		{
		public:
			NOAParser();
			virtual ~NOAParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual Optional<IO::ParsedObject> ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr);
		};
	}
}
#endif
