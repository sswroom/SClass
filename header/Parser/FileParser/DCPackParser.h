#ifndef _SM_PARSER_FILEPARSER_DCPACKPARSER
#define _SM_PARSER_FILEPARSER_DCPACKPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class DCPackParser : public IO::FileParser
		{
		public:
			DCPackParser();
			virtual ~DCPackParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual Optional<IO::ParsedObject> ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr);
		};
	}
}
#endif
