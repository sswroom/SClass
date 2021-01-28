#ifndef _SM_PARSER_FILEPARSER_ZWEIPARSER
#define _SM_PARSER_FILEPARSER_ZWEIPARSER
#include "IO/IFileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class ZWEIParser : public IO::IFileParser
		{
		public:
			ZWEIParser();
			virtual ~ZWEIParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t);
			virtual IO::ParsedObject::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType);
		};
	};
};
#endif
