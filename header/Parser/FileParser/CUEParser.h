#ifndef _SM_PARSER_FILEPARSER_CUEPARSER
#define _SM_PARSER_FILEPARSER_CUEPARSER
#include "IO/IFileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class CUEParser : public IO::IFileParser
		{
		private:
			Parser::ParserList *parsers;
		public:
			CUEParser();
			virtual ~CUEParser();

			virtual Int32 GetName();
			virtual void SetParserList(Parser::ParserList *parsers);
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t);
			virtual IO::ParsedObject::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType);
		private:
			UTF8Char *ReadString(UTF8Char *sbuff, const UTF8Char *cueStr);
			Int32 ReadTime(const UTF8Char *timeStr);
		};
	};
};
#endif
