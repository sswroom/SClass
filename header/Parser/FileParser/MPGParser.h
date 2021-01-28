#ifndef _SM_PARSER_FILEPARSER_MPGPARSER
#define _SM_PARSER_FILEPARSER_MPGPARSER
#include "IO/IFileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class MPGParser : public IO::IFileParser
		{
		public:
			MPGParser();
			virtual ~MPGParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t);
			virtual IO::ParsedObject::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType);
		};
	};
};
#endif
