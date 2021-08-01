#ifndef _SM_PARSER_FILEPARSER_APEPARSER
#define _SM_PARSER_FILEPARSER_APEPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class APEParser : public IO::FileParser
		{
		public:
			APEParser();
			virtual ~APEParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t);
			virtual IO::ParsedObject::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType);
		};
	};
};
#endif
