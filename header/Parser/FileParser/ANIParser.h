#ifndef _SM_PARSER_FILEPARSER_ANIPARSER
#define _SM_PARSER_FILEPARSER_ANIPARSER
#include "Parser/FileParser/ICOParser.h"

namespace Parser
{
	namespace FileParser
	{
		class ANIParser : public IO::FileParser
		{
		private:
			Parser::FileParser::ICOParser icop;
		public:
			ANIParser();
			virtual ~ANIParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(NotNullPtr<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFileHdr(NotNullPtr<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr);
		};
	};
};
#endif
