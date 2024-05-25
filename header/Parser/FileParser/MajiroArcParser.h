#ifndef _SM_PARSER_FILEPARSER_MAJIROARCPARSER
#define _SM_PARSER_FILEPARSER_MAJIROARCPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class MajiroArcParser : public IO::FileParser
		{
		public:
			MajiroArcParser();
			virtual ~MajiroArcParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual Optional<IO::ParsedObject> ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr);
		};
	}
}
#endif
