#ifndef _SM_PARSER_FILEPARSER_AC3PARSER
#define _SM_PARSER_FILEPARSER_AC3PARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class AC3Parser : public IO::FileParser
		{
		public:
			AC3Parser();
			virtual ~AC3Parser();

			virtual Int32 GetName();
			virtual void PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual Optional<IO::ParsedObject> ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr);
		};
	}
}
#endif
