#ifndef _SM_PARSER_FILEPARSER_B3DMPARSER
#define _SM_PARSER_FILEPARSER_B3DMPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class B3DMParser : public IO::FileParser
		{
		public:
			B3DMParser();
			virtual ~B3DMParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual Optional<IO::ParsedObject> ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr);
		};
	}
}
#endif
