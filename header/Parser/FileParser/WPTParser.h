#ifndef _SM_PARSER_FILEPARSER_WPTPARSER
#define _SM_PARSER_FILEPARSER_WPTPARSER
#include "IO/IFileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class WPTParser : public IO::IFileParser
		{
		public:
			WPTParser();
			virtual ~WPTParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t);
			virtual IO::ParsedObject::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType);
		};
	};
};
#endif
