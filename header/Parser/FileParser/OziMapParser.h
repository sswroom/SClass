#ifndef _SM_PARSER_FILEPARSER_OZIMAPPARSER
#define _SM_PARSER_FILEPARSER_OZIMAPPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class OziMapParser : public IO::FileParser
		{
		private:
			Parser::ParserList *parsers;
		public:
			OziMapParser();
			virtual ~OziMapParser();

			virtual Int32 GetName();
			virtual void SetParserList(Parser::ParserList *parsers);
			virtual void PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFileHdr(NN<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr);
		};
	}
}
#endif
