#ifndef _SM_PARSER_FILEPARSER_TGAPARSER
#define _SM_PARSER_FILEPARSER_TGAPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class TGAParser : public IO::FileParser
		{
		public:
			TGAParser();
			virtual ~TGAParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType);
		};
	};
};
#endif
