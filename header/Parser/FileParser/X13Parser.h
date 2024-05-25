#ifndef _SM_PARSER_FILEPARSER_X13PARSER
#define _SM_PARSER_FILEPARSER_X13PARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class X13Parser : public IO::FileParser
		{
		public:
			X13Parser();
			virtual ~X13Parser();

			virtual Int32 GetName();
			virtual void PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual Optional<IO::ParsedObject> ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr);
		};
	}
}
#endif
