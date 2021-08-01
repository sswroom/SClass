#ifndef _SM_PARSER_FILEPARSER_SPREDPARSER
#define _SM_PARSER_FILEPARSER_SPREDPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class SPREDParser : public IO::FileParser
		{
		public:
			SPREDParser();
			virtual ~SPREDParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t);
			virtual IO::ParsedObject::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType);
		};
	};
};
#endif
