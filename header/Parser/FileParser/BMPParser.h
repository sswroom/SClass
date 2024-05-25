#ifndef _SM_PARSER_FILEPARSER_BMPPARSER
#define _SM_PARSER_FILEPARSER_BMPPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class BMPParser : public IO::FileParser
		{
		private:
			Optional<Parser::ParserList> parsers;
		public:
			BMPParser();
			virtual ~BMPParser();

			virtual Int32 GetName();
			virtual void SetParserList(Optional<Parser::ParserList> parsers);
			virtual void PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual Optional<IO::ParsedObject> ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr);
		};
	};
};
#endif
