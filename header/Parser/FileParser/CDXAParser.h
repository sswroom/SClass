#ifndef _SM_PARSER_FILEPARSER_CDXAPARSER
#define _SM_PARSER_FILEPARSER_CDXAPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class CDXAParser : public IO::FileParser
		{
		private:
			ParserList *parsers;

		public:
			CDXAParser();
			virtual ~CDXAParser();

			virtual Int32 GetName();
			virtual void SetParserList(Parser::ParserList *parsers);
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t);
			virtual IO::ParsedObject::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType);
		};
	};
};
#endif
