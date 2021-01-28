#ifndef _SM_PARSER_FILEPARSER_RLOCPARSER
#define _SM_PARSER_FILEPARSER_RLOCPARSER
#include "IO/IFileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class RLOCParser : public IO::IFileParser
		{
		public:
			RLOCParser();
			virtual ~RLOCParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t);
			virtual IO::ParsedObject::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType);
		};
	};
};
#endif
