#ifndef _SM_PARSER_FILEPARSER_YKCPARSER
#define _SM_PARSER_FILEPARSER_YKCPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class YKCParser : public IO::FileParser
		{
		public:
			YKCParser();
			virtual ~YKCParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t);
			virtual IO::ParsedObject::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType);
		};
	};
};
#endif
