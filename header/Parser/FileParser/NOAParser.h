#ifndef _SM_PARSER_FILEPARSER_NOAPARSER
#define _SM_PARSER_FILEPARSER_NOAPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class NOAParser : public IO::FileParser
		{
		public:
			NOAParser();
			virtual ~NOAParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType);
		};
	};
};
#endif
