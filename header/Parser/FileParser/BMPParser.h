#ifndef _SM_PARSER_FILEPARSER_BMPPARSER
#define _SM_PARSER_FILEPARSER_BMPPARSER
#include "IO/IFileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class BMPParser : public IO::IFileParser
		{
		private:
			Parser::ParserList *parsers;
		public:
			BMPParser();
			virtual ~BMPParser();

			virtual Int32 GetName();
			virtual void SetParserList(Parser::ParserList *parsers);
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t);
			virtual IO::ParsedObject::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType);
		};
	};
};
#endif
