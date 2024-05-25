#ifndef _SM_PARSER_FILEPARSER_MMSPARSER
#define _SM_PARSER_FILEPARSER_MMSPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class MMSParser : public IO::FileParser
		{
		public:
			MMSParser();
			virtual ~MMSParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual Optional<IO::ParsedObject> ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr);
		};
	}
}
#endif
