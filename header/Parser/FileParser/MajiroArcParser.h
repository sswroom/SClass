#ifndef _SM_PARSER_FILEPARSER_MAJIROARCPARSER
#define _SM_PARSER_FILEPARSER_MAJIROARCPARSER
#include "IO/IFileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class MajiroArcParser : public IO::IFileParser
		{
		public:
			MajiroArcParser();
			virtual ~MajiroArcParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t);
			virtual IO::ParsedObject::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType);
		};
	};
};
#endif
