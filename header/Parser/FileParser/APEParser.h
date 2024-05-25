#ifndef _SM_PARSER_FILEPARSER_APEPARSER
#define _SM_PARSER_FILEPARSER_APEPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class APEParser : public IO::FileParser
		{
		public:
			APEParser();
			virtual ~APEParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual Optional<IO::ParsedObject> ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr);
		};
	}
}
#endif
