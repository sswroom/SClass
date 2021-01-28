#ifndef _SM_PARSER_FILEPARSER_ANIPARSER
#define _SM_PARSER_FILEPARSER_ANIPARSER
#include "Parser/FileParser/ICOParser.h"

namespace Parser
{
	namespace FileParser
	{
		class ANIParser : public IO::IFileParser
		{
		private:
			Parser::FileParser::ICOParser *icop;
		public:
			ANIParser();
			virtual ~ANIParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t);
			virtual IO::ParsedObject::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType);
		};
	};
};
#endif
