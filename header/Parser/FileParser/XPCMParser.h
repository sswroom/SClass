#ifndef _SM_PARSER_FILEPARSER_XPCMPARSER
#define _SM_PARSER_FILEPARSER_XPCMPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class XPCMParser : public IO::FileParser
		{
		public:
			XPCMParser();
			virtual ~XPCMParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType);
		};
	};
};
#endif
