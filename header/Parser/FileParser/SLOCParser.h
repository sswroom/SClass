#ifndef _SM_PARSER_FILEPARSER_SLOCPARSER
#define _SM_PARSER_FILEPARSER_SLOCPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class SLOCParser : public IO::FileParser
		{
		public:
			SLOCParser();
			virtual ~SLOCParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t);
			virtual IO::ParsedObject::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType);
		};
	};
};
#endif
