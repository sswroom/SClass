#ifndef _SM_PARSER_FILEPARSER_SFVPARSER
#define _SM_PARSER_FILEPARSER_SFVPARSER
#include "IO/FileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class SFVParser : public IO::FileParser
		{
		public:
			SFVParser();
			virtual ~SFVParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t);
			virtual IO::ParsedObject::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType);
		};
	};
};
#endif
