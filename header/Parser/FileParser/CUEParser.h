#ifndef _SM_PARSER_FILEPARSER_CUEPARSER
#define _SM_PARSER_FILEPARSER_CUEPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class CUEParser : public IO::FileParser
		{
		private:
			Parser::ParserList *parsers;
		public:
			CUEParser();
			virtual ~CUEParser();

			virtual Int32 GetName();
			virtual void SetParserList(Parser::ParserList *parsers);
			virtual void PrepareSelector(NotNullPtr<IO::FileSelector> selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFileHdr(NotNullPtr<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr);
		private:
			UTF8Char *ReadString(UTF8Char *sbuff, const UTF8Char *cueStr);
			UInt32 ReadTime(const UTF8Char *timeStr);
		};
	}
}
#endif
