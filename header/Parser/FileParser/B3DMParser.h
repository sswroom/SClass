#ifndef _SM_PARSER_FILEPARSER_B3DMPARSER
#define _SM_PARSER_FILEPARSER_B3DMPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class B3DMParser : public IO::FileParser
		{
		public:
			B3DMParser();
			virtual ~B3DMParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType);
		};
	}
}
#endif
