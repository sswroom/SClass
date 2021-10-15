#ifndef _SM_PARSER_FILEPARSER_GIFPARSER
#define _SM_PARSER_FILEPARSER_GIFPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class GIFParser : public IO::FileParser
		{
		public:
			GIFParser();
			virtual ~GIFParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType);
		};
	};
};
#endif
