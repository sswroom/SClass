#ifndef _SM_PARSER_FILEPARSER_SZSPARSER
#define _SM_PARSER_FILEPARSER_SZSPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class SZSParser : public IO::FileParser
		{
		public:
			SZSParser();
			virtual ~SZSParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t);
			virtual IO::ParsedObject::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType);
		};
	};
};
#endif
