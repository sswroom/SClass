#ifndef _SM_PARSER_FILEPARSER_UDPPARSER
#define _SM_PARSER_FILEPARSER_UDPPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class UDPParser : public IO::FileParser
		{
		public:
			UDPParser();
			virtual ~UDPParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual Optional<IO::ParsedObject> ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr);
		};
	}
}
#endif
