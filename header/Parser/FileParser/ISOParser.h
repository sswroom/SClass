#ifndef _SM_PARSER_FILEPARSER_ISOPARSER
#define _SM_PARSER_FILEPARSER_ISOPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class ISOParser : public IO::FileParser
		{
		private:
			Optional<ParserList> parsers;

		public:
			ISOParser();
			virtual ~ISOParser();

			virtual Int32 GetName();
			virtual void SetParserList(Optional<Parser::ParserList> parsers);
			virtual void PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual Optional<IO::ParsedObject> ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr);
		};
	}
}
#endif
