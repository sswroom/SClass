#ifndef _SM_PARSER_FILEPARSER_MLHPARSER
#define _SM_PARSER_FILEPARSER_MLHPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class MLHParser : public IO::FileParser
		{
		public:
			MLHParser();
			virtual ~MLHParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType);
		};
	};
};
#endif
