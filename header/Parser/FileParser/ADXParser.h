#ifndef _SM_PARSER_FILEPARSER_ADXPARSER
#define _SM_PARSER_FILEPARSER_ADXPARSER
#include "IO/IFileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class ADXParser : public IO::IFileParser
		{
		public:
			ADXParser();
			virtual ~ADXParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t);
			virtual IO::ParsedObject::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType);
		};
	}
}
#endif
