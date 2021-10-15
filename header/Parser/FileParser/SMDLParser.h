#ifndef _SM_PARSER_FILEPARSER_SMDLPARSER
#define _SM_PARSER_FILEPARSER_SMDLPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class SMDLParser : public IO::FileParser
		{
		public:
			SMDLParser();
			virtual ~SMDLParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParserType t);
			virtual IO::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType);
		};
	};
};
#endif
