#ifndef _SM_PARSER_FILEPARSER_ZWEIPARSER
#define _SM_PARSER_FILEPARSER_ZWEIPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class ZWEIParser : public IO::FileParser
		{
		public:
			ZWEIParser();
			virtual ~ZWEIParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType);
		};
	};
};
#endif
