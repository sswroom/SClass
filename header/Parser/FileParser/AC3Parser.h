#ifndef _SM_PARSER_FILEPARSER_AC3PARSER
#define _SM_PARSER_FILEPARSER_AC3PARSER
#include "IO/IFileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class AC3Parser : public IO::IFileParser
		{
		public:
			AC3Parser();
			virtual ~AC3Parser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t);
			virtual IO::ParsedObject::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType);
		};
	};
};
#endif
