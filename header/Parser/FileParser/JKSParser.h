#ifndef _SM_PARSER_FILEPARSER_JKSPARSER
#define _SM_PARSER_FILEPARSER_JKSPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class JKSParser : public IO::FileParser
		{
		public:
			JKSParser();
			virtual ~JKSParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType);
		};
	}
}
#endif
