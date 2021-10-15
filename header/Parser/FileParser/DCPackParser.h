#ifndef _SM_PARSER_FILEPARSER_DCPACKPARSER
#define _SM_PARSER_FILEPARSER_DCPACKPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class DCPackParser : public IO::FileParser
		{
		public:
			DCPackParser();
			virtual ~DCPackParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType);
		};
	};
};
#endif
