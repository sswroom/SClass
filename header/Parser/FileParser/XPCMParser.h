#ifndef _SM_PARSER_FILEPARSER_XPCMPARSER
#define _SM_PARSER_FILEPARSER_XPCMPARSER
#include "IO/IFileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class XPCMParser : public IO::IFileParser
		{
		public:
			XPCMParser();
			virtual ~XPCMParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t);
			virtual IO::ParsedObject::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType);
		};
	};
};
#endif
