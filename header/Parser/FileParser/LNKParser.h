#ifndef _SM_PARSER_FILEPARSER_LNKPARSER
#define _SM_PARSER_FILEPARSER_LNKPARSER
#include "IO/IFileParser.h"

namespace Parser
{
	namespace FileParser
	{
		class LNKParser : public IO::IFileParser
		{
		public:
			LNKParser();
			virtual ~LNKParser();

			virtual Int32 GetName();
			virtual void PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t);
			virtual IO::ParsedObject::ParserType GetParserType();
			virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType);
		};
	};
};
#endif
