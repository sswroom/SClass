#ifndef _SM_PARSER_FILEPARSER_ADXPARSER
#define _SM_PARSER_FILEPARSER_ADXPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class ADXParser : public IO::FileParser
		{
		public:
			ADXParser();
			virtual ~ADXParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType);
		};
	}
}
#endif
