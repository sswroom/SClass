#ifndef _SM_PARSER_FILEPARSER_TSUYOSHIARCPARSER
#define _SM_PARSER_FILEPARSER_TSUYOSHIARCPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class TsuyoshiArcParser : public IO::FileParser
		{
		public:
			TsuyoshiArcParser();
			virtual ~TsuyoshiArcParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual Optional<IO::ParsedObject> ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr);
		};
	}
}
#endif
