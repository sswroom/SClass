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
			Parser::ParserList *parsers;
		public:
			BMPParser();
			virtual ~BMPParser();

			virtual Int32 GetName();
			virtual void SetParserList(Parser::ParserList *parsers);
			virtual void PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFileHdr(NN<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr);
		};
	};
};
#endif
