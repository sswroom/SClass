#ifndef _SM_PARSER_FILEPARSER_NS2PARSER
#define _SM_PARSER_FILEPARSER_NS2PARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class NS2Parser : public IO::FileParser
		{
		public:
			NS2Parser();
			virtual ~NS2Parser();

			virtual Int32 GetName();
			virtual void PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual Optional<IO::ParsedObject> ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr);
		};
	}
}
#endif
