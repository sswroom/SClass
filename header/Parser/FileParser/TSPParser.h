#ifndef _SM_PARSER_FILEPARSER_TSPPARSER
#define _SM_PARSER_FILEPARSER_TSPPARSER
#include "IO/IFileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class TSPParser : public IO::IFileParser
		{
		public:
			TSPParser();
			virtual ~TSPParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t);
			virtual IO::ParsedObject::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType);
		};
	};
};
#endif
