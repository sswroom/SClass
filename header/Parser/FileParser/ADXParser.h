#ifndef _SM_PARSER_FILEPARSER_ADXPARSER
#define _SM_PARSER_FILEPARSER_ADXPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class ADXParser : public IO::FileParser
		{
		public:
			ADXParser();
			virtual ~ADXParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual Optional<IO::ParsedObject> ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr);
		};
	}
}
#endif
