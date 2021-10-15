#ifndef _SM_PARSER_FILEPARSER_OZF2PARSER
#define _SM_PARSER_FILEPARSER_OZF2PARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class OZF2Parser : public IO::FileParser
		{
		public:
			OZF2Parser();
			virtual ~OZF2Parser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType);
		};
	};
};
#endif
