#ifndef _SM_PARSER_FILEPARSER_PCAPPARSER
#define _SM_PARSER_FILEPARSER_PCAPPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class PCAPParser : public IO::FileParser
		{
		public:
			PCAPParser();
			virtual ~PCAPParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual Optional<IO::ParsedObject> ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr);
		};
	}
}
#endif
