#ifndef _SM_PARSER_FILEPARSER_OZIMAPPARSER
#define _SM_PARSER_FILEPARSER_OZIMAPPARSER
#include "IO/IFileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class OziMapParser : public IO::IFileParser
		{
		private:
			Parser::ParserList *parsers;
		public:
			OziMapParser();
			virtual ~OziMapParser();

			virtual Int32 GetName();
			virtual void SetParserList(Parser::ParserList *parsers);
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t);
			virtual IO::ParsedObject::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType);
		};
	};
};
#endif
