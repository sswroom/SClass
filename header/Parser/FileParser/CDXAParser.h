#ifndef _SM_PARSER_FILEPARSER_CDXAPARSER
#define _SM_PARSER_FILEPARSER_CDXAPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class CDXAParser : public IO::FileParser
		{
		private:
			Optional<ParserList> parsers;

		public:
			CDXAParser();
			virtual ~CDXAParser();

			virtual Int32 GetName();
			virtual void SetParserList(Optional<Parser::ParserList> parsers);
			virtual void PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual Optional<IO::ParsedObject> ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr);
		};
	}
}
#endif
