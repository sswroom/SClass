#ifndef _SM_PARSER_FILEPARSER_DTSPARSER
#define _SM_PARSER_FILEPARSER_DTSPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class DTSParser : public IO::FileParser
		{
		public:
			DTSParser();
			virtual ~DTSParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t);
			virtual IO::ParsedObject::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType);
		};
	};
};
#endif
