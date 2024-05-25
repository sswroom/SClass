#ifndef _SM_PARSER_FILEPARSER_MAIPACKPARSER
#define _SM_PARSER_FILEPARSER_MAIPACKPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class MAIPackParser : public IO::FileParser
		{
		public:
			MAIPackParser();
			virtual ~MAIPackParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual Optional<IO::ParsedObject> ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr);
		};
	}
}
#endif
