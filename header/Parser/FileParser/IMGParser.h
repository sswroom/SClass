#ifndef _SM_PARSER_FILEPARSER_IMGPARSER
#define _SM_PARSER_FILEPARSER_IMGPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class IMGParser : public IO::FileParser
		{
		public:
			IMGParser();
			virtual ~IMGParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual Optional<IO::ParsedObject> ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr);
		};
	}
}
#endif
