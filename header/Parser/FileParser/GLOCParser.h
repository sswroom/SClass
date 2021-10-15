#ifndef _SM_PARSER_FILEPARSER_GLOCPARSER
#define _SM_PARSER_FILEPARSER_GLOCPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class GLOCParser : public IO::FileParser
		{
		public:
			GLOCParser();
			virtual ~GLOCParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType);
		};
	}
}
#endif
