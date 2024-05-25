#ifndef _SM_PARSER_FILEPARSER_ICOPARSER
#define _SM_PARSER_FILEPARSER_ICOPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class ICOParser : public IO::FileParser
		{
		public:
			ICOParser();
			virtual ~ICOParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual Optional<IO::ParsedObject> ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr);
		};
	};
};
#endif
