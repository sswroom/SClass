#ifndef _SM_PARSER_FILEPARSER_NFPPARSER
#define _SM_PARSER_FILEPARSER_NFPPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class NFPParser : public IO::FileParser
		{
		public:
			NFPParser();
			virtual ~NFPParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType);
		};
	};
};
#endif
