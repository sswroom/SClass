#ifndef _SM_PARSER_FILEPARSER_SMAKEPARSER
#define _SM_PARSER_FILEPARSER_SMAKEPARSER
#include "IO/IFileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class SMakeParser : public IO::IFileParser
		{
		public:
			SMakeParser();
			virtual ~SMakeParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t);
			virtual IO::ParsedObject::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType);
		};
	}
}
#endif
