#ifndef _SM_PARSER_FILEPARSER_HEIFPARSER
#define _SM_PARSER_FILEPARSER_HEIFPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class HEIFParser : public IO::FileParser
		{
		public:
			HEIFParser();
			virtual ~HEIFParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType);
		};
	}
}
#endif