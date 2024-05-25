#ifndef _SM_PARSER_FILEPARSER_SLOCPARSER
#define _SM_PARSER_FILEPARSER_SLOCPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class SLOCParser : public IO::FileParser
		{
		public:
			SLOCParser();
			virtual ~SLOCParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual Optional<IO::ParsedObject> ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr);
		};
	}
}
#endif
