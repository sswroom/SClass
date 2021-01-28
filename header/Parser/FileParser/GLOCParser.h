#ifndef _SM_PARSER_FILEPARSER_GLOCPARSER
#define _SM_PARSER_FILEPARSER_GLOCPARSER
#include "IO/IFileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class GLOCParser : public IO::IFileParser
		{
		public:
			GLOCParser();
			virtual ~GLOCParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t);
			virtual IO::ParsedObject::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType);
		};
	}
}
#endif
