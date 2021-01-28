#ifndef _SM_PARSER_FILEPARSER_AOSPARSER
#define _SM_PARSER_FILEPARSER_AOSPARSER
#include "IO/IFileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class AOSParser : public IO::IFileParser
		{
		public:
			AOSParser();
			virtual ~AOSParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t);
			virtual IO::ParsedObject::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType);
		};
	};
};
#endif
